/*
 *
 * osd.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "osd.h"
#include "refresh.h"
#include "refresh_internal.h"
#include "subtitle.h"
#include "../parser/dvd/spu.h" /* DVD */

#include "../main/prefs.h"
#include "../amigaos/font.h"

static struct {
  unsigned long argb[256];
  unsigned long ayuv[256];
  unsigned char lut[256];
  unsigned short *rgb2lut;
} osd_palette;

void (*osd_refresh)(unsigned char *image[3], unsigned char *osd, int x, int y, int w, int h, int l_w);

#define CLIP \
  if ((x < 0) || (y < 0)) { \
    return; \
  } \
  if ((x + w) > refresh_rect.line_width) { \
    w = refresh_rect.line_width - x; \
  } \
  if ((y + h) > refresh_rect.height) { \
    h = refresh_rect.height - y; \
  } \
  if ((w <= 0) || (h <= 0)) { \
    return; \
  }

static void osd_refresh_dummy(unsigned char *image[3], unsigned char *src, int x, int y, int w, int h, int l_w)
{
}

static void osd_refresh_argb32(unsigned char *image[3], unsigned char *osd, int x, int y, int w, int h, int l_w)
{
  unsigned long *dst, val;
  unsigned char *src;
  int i, j;

  CLIP;

  for (i=0; i<h; i++) {
    src = &osd[(i * l_w)];
    dst = &((unsigned long *)refresh_rect.osd[0])[((i + y) * refresh_rect.line_width)];
    refresh_rect.buf_use_osd[i + y] = 1; /* Anything > 0 is ok */

    /* copy source to destination */
    memcpy(dst, &((unsigned long *)image[0])[((i + y) * refresh_rect.line_width)], refresh_rect.line_width * 4);
    dst += x;

    for (j=0; j<w; j++) {
      unsigned long argb;
      int alpha;

      argb  = osd_palette.argb[src[j]];
      alpha = (argb >> 24) & 0xff;

      if (alpha == 0xff) {
        *dst = argb;
      } else if (alpha > 0) {
        int sr, sg, sb, dr, dg, db, r, g, b;

        sr = ((argb >> 16) & 0xff);
        sg = ((argb >>  8) & 0xff);
        sb = ((argb >>  0) & 0xff);

        val = *dst;
        dr = ((val >> 16) & 0xff);
        dg = ((val >>  8) & 0xff);
        db = ((val >>  0) & 0xff);

        r = ((sr * alpha) + (dr * (0xff - alpha))) / 0xff;
        g = ((sg * alpha) + (dg * (0xff - alpha))) / 0xff;
        b = ((sb * alpha) + (db * (0xff - alpha))) / 0xff;

        *dst = (r << 16) | (g << 8) | b;
      }
      dst++;
    }
  }
}

static void osd_refresh_yuv420(unsigned char *image[3], unsigned char *osd, int x, int y, int w, int h, int l_w)
{
  unsigned char *src, *dsty, *dstu, *dstv;
  unsigned long ayuv, cuv;
  int i, j, lw;

  CLIP;

  lw = refresh_rect.line_width;

  /* IMPORTANT: Since we are dealing with YUV420 here, some renderers MAY only check the
   * use_osd array once for every EVEN row, thus we must start one row early otherwise we
   * will miss the first row of hour OSD image. It's similar for the bottom where we might
   * have to copy one additional row of original image data.
   */

  /* Y */
  if (y & 0x01) {
    refresh_rect.buf_use_osd[y - 1] = 1; /* Anything > 0 is ok */
    memcpy(&refresh_rect.osd[0][((y - 1) * lw)], &image[0][((y - 1) * lw)], lw);
  }

  for (i=0; i<h; i++) {
    src = &osd[(i * l_w)];
    dsty = &refresh_rect.osd[0][((i + y) * lw)];
    refresh_rect.buf_use_osd[i + y] = 1; /* Anything > 0 is ok */

    /* copy source to destination */
    memcpy(dsty, &image[0][((i + y) * lw)], lw);
    dsty += x;

    for (j=0; j<w; j++) {
      unsigned long y;
      int alpha;

      ayuv = osd_palette.ayuv[src[j]];
      alpha = (ayuv >> 24) & 0xff;
      y = (ayuv >> 16) & 0xff;

      if (alpha == 0xff) {
        *dsty = y;
      } else if (alpha > 0) {
        *dsty = ((y * alpha) + (*dsty * (0xff - alpha))) / 0xff;
      }
      dsty++;
    }
  }

  if ((h + y) & 0x01) {
    memcpy(&refresh_rect.osd[0][((h + y) * lw)], &image[0][((h + y) * lw)], lw);
  }

  if (prefs.colormode == PREFS_GRAY) {
    return;
  }

  /* UV */
  cuv = refresh_rect.cuv;
  lw /= 2;
  h = (h + 1) / 2;
  w = (w + 1) / 2;
  x /= 2;
  if (y & 0x01) {
    y /= 2;
    memcpy(&refresh_rect.osd[1][((h + y) * lw)], &image[1][((h + y) * lw)], lw);
    memcpy(&refresh_rect.osd[2][((h + y) * lw)], &image[2][((h + y) * lw)], lw);
  } else {
    y /= 2;
  }

  for (i=0; i<h; i++) {
    src = &osd[((i * 2) * l_w)];
    dstu = &refresh_rect.osd[1][((i + y) * lw)];
    dstv = &refresh_rect.osd[2][((i + y) * lw)];

    /* copy source to destination */
    memcpy(dstu, &image[1][((i + y) * lw)], lw);
    memcpy(dstv, &image[2][((i + y) * lw)], lw);
    dstu += x;
    dstv += x;

    for (j=0; j<w; j++) {
      int alpha;

      ayuv = osd_palette.ayuv[src[(j*2)]];
      alpha = (ayuv >> 24) & 0xff;

      if (alpha == 0xff) {
        *dstu = (ayuv >>  8) & 0xff;
        *dstv = (ayuv >>  0) & 0xff;
      } else if (alpha > 0) {
        int u, v, u1, v1, u2, v2;

        ayuv ^= cuv;

        u1 = (int)(signed char)((ayuv >>  8) & 0xff);
        v1 = (int)(signed char)((ayuv >>  0) & 0xff);

        u2 = (int)(signed char)((*dstu ^ cuv) & 0xff);
        v2 = (int)(signed char)((*dstv ^ cuv) & 0xff);

        u = ((u1 * alpha) + (u2 * (0xff - alpha))) / 0xff;
        v = ((v1 * alpha) + (v2 * (0xff - alpha))) / 0xff;

        *dstu = ((u ^ cuv) & 0xff);
        *dstv = ((v ^ cuv) & 0xff);
      }
      dstu++;
      dstv++;
    }
  }
}

static void osd_refresh_lut(unsigned char *image[3], unsigned char *osd, int x, int y, int w, int h, int l_w)
{
  unsigned char *dst;
  unsigned char *src;
  int i, j;

  CLIP;

  for (i=0; i<h; i++) {
    src = &osd[(i * l_w)];
    dst = &refresh_rect.osd[0][((i + y) * refresh_rect.line_width)];
    refresh_rect.buf_use_osd[i + y] = 1; /* Anything > 0 is ok */

    /* copy source to destination */
    memcpy(dst, &image[0][((i + y) * refresh_rect.line_width)], refresh_rect.line_width);
    dst += x;

    for (j=0; j<w; j++) {
      unsigned long argb;
      unsigned char c;
      int alpha;

      c = src[j];
      argb  = osd_palette.argb[c];
      alpha = (argb >> 24) & 0xff;

      if (alpha == 0xff) {
        *dst = osd_palette.lut[c];
      } else if (alpha > 0) {
        unsigned long rpal;
        int sr, sg, sb, dr, dg, db, r, g, b;

        sr = ((argb >> 16) & 0xff);
        sg = ((argb >>  8) & 0xff);
        sb = ((argb >>  0) & 0xff);

        rpal = refresh_rect.palette[*dst];

        dr = ((rpal >> 16) & 0xff);
        dg = ((rpal >>  8) & 0xff);
        db = ((rpal >>  0) & 0xff);

        r = ((sr * alpha) + (dr * (0xff - alpha))) / 0xff;
        g = ((sg * alpha) + (dg * (0xff - alpha))) / 0xff;
        b = ((sb * alpha) + (db * (0xff - alpha))) / 0xff;

        *dst = osd_palette.rgb2lut[(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3))];
      }
      dst++;
    }
  }
}

void osd_init(int w, int h, int l_w, int mode)
{
  osd_refresh = osd_refresh_dummy;

  memset(&osd_palette, 0, sizeof(osd_palette));

  if (mode == REFRESH_ARGB32) {
    osd_refresh = osd_refresh_argb32;
    refresh_rect.osd[0] = malloc(l_w * h * 4);
    memset(refresh_rect.osd[0], 0, l_w * h * 4);
  } else if (mode == REFRESH_YUV420) {
    refresh_rect.osd[0] = malloc(l_w * h);
    refresh_rect.osd[1] = malloc(l_w * h / 4);
    refresh_rect.osd[2] = malloc(l_w * h / 4);
    memset(refresh_rect.osd[0], 0, l_w * h);
    memset(refresh_rect.osd[1], 0, l_w * h / 4);
    memset(refresh_rect.osd[2], 0, l_w * h / 4);
    osd_refresh = osd_refresh_yuv420;
  } else if (mode == REFRESH_LUT8) {
    refresh_rect.osd[0] = malloc(l_w * h);
    memset(refresh_rect.osd[0], 0, l_w * h);
    osd_palette.rgb2lut = malloc(65536 * 2);
    memset(osd_palette.rgb2lut, 0, 65536 * 2);
    osd_refresh = osd_refresh_lut;
  }

  /* DVD */
  spu_setup(w, h, (prefs.lores == PREFS_ON)); /* FIXME */
}

void osd_init_palette(unsigned long *argb, unsigned long *ayuv, int entries)
{
  int i, j;

  if (argb != NULL) {
    memcpy(osd_palette.argb, argb, 4*entries);
  }

  if (ayuv != NULL) {
    memcpy(osd_palette.ayuv, ayuv, 4*entries);
  }

  if (osd_refresh != osd_refresh_lut) {
    return;
  }

  /* OSD -> IMAGE palette converter */
  for (i=0; i<256; i++) {
    int or, og, ob, diff, col;
    or = (osd_palette.argb[i] >> 16) & 0xff;
    og = (osd_palette.argb[i] >>  8) & 0xff;
    ob = (osd_palette.argb[i] >>  0) & 0xff;

    diff = 256*256*3;
    col = 0;

    for (j=0; j<256; j++) {
      int r, g, b, dist;
      r = (refresh_rect.palette[j] >> 16) & 0xff;
      g = (refresh_rect.palette[j] >>  8) & 0xff;
      b = (refresh_rect.palette[j] >>  0) & 0xff;

      dist = (or - r) * (or - r) + (og - g) * (og - g) + (ob - b) * (ob - b);
      if (dist < diff) {
        diff = dist;
        col = j;
      }
    }

    osd_palette.lut[i] = col;
  }

  /* RGB16 -> IMAGE palette converter */
  for (i=0; i<65536; i++) {
    int or, og, ob, diff, col;
    or = (i & 0xf800) >> 8;
    og = (i & 0x07e0) >> 3;
    ob = (i & 0x001f) << 3;

    diff = 256*256*3;
    col = 0;

    for (j=0; j<256; j++) {
      int r, g, b, dist;
      r = (refresh_rect.palette[j] >> 16) & 0xff;
      g = (refresh_rect.palette[j] >>  8) & 0xff;
      b = (refresh_rect.palette[j] >>  0) & 0xff;

      dist = (or - r) * (or - r) + (og - g) * (og - g) + (ob - b) * (ob - b);
      if (dist < diff) {
        diff = dist;
        col = j;
      }
    }

    osd_palette.rgb2lut[i] = col;
  }
}

void osd_exit(void)
{
  int i;

  for (i=0; i<3; i++) {
    if (refresh_rect.osd[i] != NULL) {
      free(refresh_rect.osd[i]);
      refresh_rect.osd[i] = NULL;
    }
  }

  if (osd_palette.rgb2lut != NULL) {
    free(osd_palette.rgb2lut);
    osd_palette.rgb2lut = NULL;
  }

  /* DVD */
  spu_exit();
}

/* SUBTITLE HANDLING */

int dvd_hack = 0; /* FIXME: Remove */

osd_char osd_chars[256];

static struct {
  int x, y, w, h;
  int in_use, bpr;
  unsigned char *image;
  int line_h;
} osd_subtitle;

static char real_text[256];

static void subtitle_print(int line, unsigned char *text)
{
  unsigned char *data, *image;
  int c, i, j, k, l, x, w, h, len;

  if ((line < 1) || (line > 3)) {
    return;
  }

  /* only show required number of lines */
  osd_subtitle.h = (line * osd_subtitle.line_h);
  line--;

  /* add some space before and after the text */
  strcpy(real_text, "  ");
  strcat(real_text, text);
  strcat(real_text, "  ");

  len = strlen(real_text);

  x = 0;
  for (i=0; i<len; i++) {
    c = real_text[i];
    x += osd_chars[c].space;

    if (x > osd_subtitle.w) {
      x = osd_subtitle.w;
      len = i;
      break;
    }
  }

  x = (osd_subtitle.w - x) / 2; /* Center subtitle */

  image = osd_subtitle.image + (line * osd_subtitle.line_h * osd_subtitle.bpr);

  /* clear subtitle */
  memset(image, 255, osd_subtitle.bpr * osd_subtitle.line_h);

  for (i=0; i<len; i++) {
    c = real_text[i];
    w = osd_chars[c].width;
    h = osd_chars[c].height;
    data = osd_chars[c].data;

    if (data != NULL) {
      for (j=0; j<h; j++) {
        for (k=0; k<w; k++) {
          image[(j * osd_subtitle.bpr) + x + k + (osd_subtitle.bpr * 3)] = *data++;
        }
      }

      /* any space to add ? */
      l = osd_chars[c].space - w;
      if (l > 0) {
        for (j=0; j<h; j++) {
          for (k=0; k<l; k++) {
            image[(j * osd_subtitle.bpr) + x + k + w + (osd_subtitle.bpr * 3)] = 0;
          }
        }
      }
    } else {
      for (j=0; j<h; j++) {
        for (k=0; k<osd_chars[c].space; k++) {
          image[(j * osd_subtitle.bpr) + x + k + (osd_subtitle.bpr * 3)] = 0;
        }
      }
    }

    x += osd_chars[c].space;
  }
}

#undef DEBUG

void subtitle_refresh(unsigned char *src[], double image_time)
{
  static double show = 0.0, hide = 0.0;
  int i, j;

  memset(refresh_rect.buf_use_osd, 0, 1024);

  if (dvd_hack) {
    spu_render(src, image_time);
    return;
  }

#ifdef DEBUG
  osd_refresh(src, osd_subtitle.image, osd_subtitle.x, osd_subtitle.y, osd_subtitle.w, osd_subtitle.h, osd_subtitle.bpr);
  return;
#endif

  if ((image_time < show) || (image_time > hide)) {
    osd_subtitle.in_use = 0;
  }

  if (osd_subtitle.in_use) {
    osd_refresh(src, osd_subtitle.image, osd_subtitle.x, osd_subtitle.y, osd_subtitle.w, osd_subtitle.h, osd_subtitle.bpr);
    return; /* we are done */
  }

  i=0;
  while (subtitle[i].line[0] != NULL) {
    if ((subtitle[i].show <= image_time) && (subtitle[i].hide >= image_time)) {
#ifdef DEBUG
      if (subtitle[i].line[1] != NULL) {
        printf("%.3f -> %.3f : '%s' | '%s'\n", subtitle[i].show, subtitle[i].hide, subtitle[i].line[0], subtitle[i].line[1]);
      } else {
        printf("%.3f -> %.3f : '%s' | 'N/A'\n", subtitle[i].show, subtitle[i].hide, subtitle[i].line[0]);
      }
#endif
      show = subtitle[i].show;
      hide = subtitle[i].hide;

      osd_subtitle.in_use = 1;

      for (j=0; j<2; j++) {
        if (subtitle[i].line[j] == NULL) {
          break;
        }

        /* render subtitle */
        subtitle_print(j + 1, subtitle[i].line[j]);
      }
    }
    i++;
  }
}

void subtitle_init(double framerate)
{
  int i, mh;

  /* FIXME */
  osd_palette.argb[0] = 0x7f000000;
  osd_palette.argb[1] = 0xffffffff;
  osd_palette.argb[2] = 0x3fffffff;
  osd_palette.argb[255] = 0x00000000; /* transparent */

  /* FIXME */
  osd_palette.ayuv[0] = 0x7f000000 | (refresh_rect.cuv & 0x0000ffff);
  osd_palette.ayuv[1] = 0xffff0000 | (refresh_rect.cuv & 0x0000ffff);
  osd_palette.ayuv[2] = 0x3fff0000 | (refresh_rect.cuv & 0x0000ffff);
  osd_palette.ayuv[255] = 0x00000000; /* transparent */

  if (dvd_hack) {
    return;
  }

  if (framerate == 0.0) {
    prefs.osd = PREFS_OFF;
    return;
  }

  if (subtitle_parse_init()) {
    prefs.osd = PREFS_OFF;
    return;
  }

  if (subtitle_parse(prefs.subtitle, framerate)) {
    prefs.osd = PREFS_OFF;
    return;
  }

  if (osd_font_init()) {
    subtitle_parse_exit();
    prefs.osd = PREFS_OFF;
    return;
  }

  mh = 0;
  for (i=0; i<256; i++) {
    if (osd_chars[i].height > mh) {
      mh = osd_chars[i].height;
    }
  }

#ifdef DEBUG
  printf("max char height: %d\n", mh);
#endif

  mh += 3 * 2; /* 3 lines above and below */

  osd_subtitle.h = mh * 3;
  osd_subtitle.w = refresh_rect.width;
  osd_subtitle.bpr = refresh_rect.width;
  osd_subtitle.in_use = 0;
  osd_subtitle.line_h = mh;

  osd_subtitle.image = malloc(osd_subtitle.bpr * osd_subtitle.h);
  memset(osd_subtitle.image, 255, (osd_subtitle.bpr * osd_subtitle.h));

#ifdef DEBUG
{
  unsigned char *image;
  image = osd_subtitle.image;
  memset(image, 0, (osd_subtitle.bpr * osd_subtitle.line_h));
  image += (osd_subtitle.bpr * osd_subtitle.line_h);
  memset(image, 1, (osd_subtitle.bpr * osd_subtitle.line_h));
  image += (osd_subtitle.bpr * osd_subtitle.line_h);
  memset(image, 2, (osd_subtitle.bpr * osd_subtitle.line_h));
}
#endif

  osd_subtitle.x = 0;
  osd_subtitle.y = (refresh_rect.height - osd_subtitle.h);
#ifdef DEBUG
  osd_subtitle.y = (refresh_rect.height - osd_subtitle.h) / 2;
#endif
  if (osd_subtitle.y < 0) {
    osd_subtitle.y = 0;
  }

#ifdef DEBUG
  subtitle_print(1, "AMP Rulez!");
#endif
}

void subtitle_exit(void)
{
  subtitle_parse_exit();
  osd_font_exit();
}
