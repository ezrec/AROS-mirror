/*
 *
 * refresh.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else

#include "aros-inc.h"

#endif

#include "refresh.h"
#include "refresh_internal.h"
#include "osd.h"
#include "../main/main.h"
#include "../main/prefs.h"
#include "../amigaos/video.h"

typedef void (*refresh_t)(rect_type *rect, unsigned char *src[]);

rect_type refresh_rect;
static refresh_t refresh_func;

typedef struct {
  refresh_t refresh;
//  int mod_x;
//  int mod_y;
} refresh_struct;

/* GHHOWAxx xxxxxxxx xxxxxxxx ????????
 *
 * G = GRAY
 * H = HAM6
 * H = HAM8
 * O = OVER
 * W = WIN8
 * A = AGA8
 *
 *
 */
#define M_GRAY 0x80000000
#define M_HAM6 0x40000000
#define M_HAM8 0x20000000
#define M_OVER 0x10000000
#define M_WIN8 0x08000000
#define M_AGA8 0x04000000

typedef struct {
  unsigned long mode;
  refresh_struct yuv;
  refresh_struct argb;
  refresh_struct lut;
} refresh_array;

static refresh_array refreshers[] = {
/* HAM6 */
{0x40000001, {yuv2ham6_1}, {argb2ham6_1}, {NULL}},
{0x40000002, {yuv2ham6_2}, {argb2ham6_2}, {NULL}},
{0x40000003, {yuv2ham6_4}, {argb2ham6_4}, {NULL}},
{0x40000004, {yuv2ham6_1hq}, {argb2ham6_1hq}, {NULL}},

/* HAM8 */
{0x20000001, {yuv2ham8_1}, {argb2ham8_1}, {NULL}},
{0x20000002, {yuv2ham8_2}, {argb2ham8_2}, {NULL}},
{0x20000003, {yuv2ham8_4}, {argb2ham8_4}, {NULL}},
{0x20000004, {yuv2ham8_1hq}, {argb2ham8_1hq}, {NULL}},

/* AGA */
{0x04000000, {yuv2aga}, {argb2aga}, {lut2aga}},
{0x84000008, {yuv2aga_gray8}, {argb2aga_gray8}, {lut2aga_gray8}},
{0x84000006, {yuv2aga_gray6}, {argb2aga_gray6}, {lut2aga_gray6}},
{0x84000004, {yuv2aga_gray4}, {argb2aga_gray4}, {lut2aga_gray4}},

/* RGB */
{0x00000001, {yuv2argb32}, {argb2argb32}, {lut2argb32}},
{0x00000002, {yuv2bgr24}, {argb2bgr24}, {lut2bgr24}},
{0x00000003, {yuv2rgb16}, {argb2rgb16}, {lut2rgb16}},
{0x00000004, {yuv2rgb15}, {argb2rgb15}, {lut2rgb15}},
{0x00000005, {yuv2rgb16pc}, {argb2rgb16pc}, {lut2rgb16pc}},
{0x00000006, {yuv2rgb15pc}, {argb2rgb15pc}, {lut2rgb15pc}},
{0x00000008, {yuv2bgra32}, {argb2bgra32}, {lut2bgra32}},

/* GRAY */
{0x80000001, {yuv2argb32_gray}, {argb2argb32_gray}, {lut2argb32_gray}},
{0x80000002, {yuv2bgr24_gray}, {argb2bgr24_gray}, {lut2bgr24_gray}},
{0x80000003, {yuv2rgb16_gray}, {argb2rgb16_gray}, {lut2rgb16_gray}},
{0x80000004, {yuv2rgb15_gray}, {argb2rgb15_gray}, {lut2rgb15_gray}},
{0x80000005, {yuv2rgb16pc_gray}, {argb2rgb16pc_gray}, {lut2rgb16pc_gray}},
{0x80000006, {yuv2rgb15pc_gray}, {argb2rgb15pc_gray}, {lut2rgb15pc_gray}},
{0x80000008, {yuv2bgra32_gray}, {argb2bgra32_gray}, {lut2bgra32_gray}},

/* LUT */
{0x00000007, {yuv2lut}, {argb2lut}, {lut2lut}},
{0x80000007, {yuv2lut_gray}, {argb2lut_gray}, {lut2lut_gray}},

/* WINDOW */
{0x08000000, {yuv2win}, {argb2win}, {lut2win}},
{0x88000000, {yuv2win_gray}, {argb2win_gray}, {lut2win_gray}},

/* OVERLAY */
{0x10000000, {yuv2overlay}, {argb2overlay}, {lut2overlay}},
{0x90000000, {yuv2overlay_gray}, {argb2overlay_gray}, {lut2overlay_gray}},

/* END, DO NOT REMOVE */
{0x00000000, {NULL}, {NULL}, {NULL}}
};

static int depth;

/* Opens a screen and performs initial refresh setup */
static int init_refresh(int w, int h, int mode)
{
  int i, crv, cbu, cgu, cgv, cym, cya, R, G, B, col, y, gray;

  w &= ~15; /* mod 16 */
  h &= ~1; /* mod 2 */

  refresh_rect.width = w;
  refresh_rect.height = h;
  refresh_rect.mode = mode;

  /* Open screen */
  depth = open_screen(w, h, mode);

  if (depth < 0) {
    /* Could not open screen */
    return -1;
  }

  debug_printf("From AmigaOS : %d %d %d %d, %d %d (%d) : %d\n",
    refresh_rect.width,
    refresh_rect.height,
    refresh_rect.screen_width,
    refresh_rect.screen_height,
    refresh_rect.bpr,
    refresh_rect.bpp,
    depth,
    mode);

  crv = refresh_rect.crv;
  cgu = refresh_rect.cgu;
  cgv = refresh_rect.cgv;
  cbu = refresh_rect.cbu;
  cym = refresh_rect.cym;
  cya = refresh_rect.cya;

  /* GRAY lookup table */
  for (i=0; i<256; i++) {
    refresh_rect.gray[i] = refresh_rect.rgb_clip[(cym * i + cya) >> FIXBITS];
  }

  /* 8bit palettes */
  if ((depth == 8) && (mode == REFRESH_YUV420)) {
    if (prefs.colormode == PREFS_GRAY) {
      for (i=0; i<256; i++) {
        col = i; /* 8 bits */
        y = i;

        if (prefs.gray_depth == 6) {
          col = i >> 2; /* 6 bits */
          y = (col * 255) / 63;
        } else if (prefs.gray_depth == 4) {
          col = i >> 4; /* 4 bits */
          y = (col * 255) / 15;
        }

        gray = refresh_rect.rgb_clip[(cym * y + cya) >> FIXBITS];

        refresh_rect.palette[col] = (gray << 16) | (gray << 8) | gray;
      }

      if (prefs.gray_depth == 8) {
        set_palette(256);
      } else if (prefs.gray_depth == 6) {
        set_palette(64);
      } else {
        set_palette(16);
      }
    } else if (prefs.colormode == PREFS_COLOR) {
      int Y, Cb, Cr;

      /* 2000-10-21: Changed to use all 256 colors */

      /* color allocation:
       * i is the (internal) 8 bit color number, it consists of separate
       * bit fields for Y, U and V: i = (yyyyuuvv), we don't use yyyy=0000
       * and yyyy=1111, this leaves 32 colors for other applications
       *
       * the allocated colors correspond to the following Y, U and V values:
       * Y:   24, 40, 56, 72, 88, 104, 120, 136, 152, 168, 184, 200, 216, 232
       * U,V: -48, -16, 16, 48
       *
       * U and V values span only about half the color space; this gives
       * usually much better quality, although highly saturated colors can
       * not be displayed properly
       *
       * translation to R,G,B is implicitly done by the color look-up table
       */

      for (i=0; i<256; i++) {
        /* color space conversion */
        Y  = 16*((i>>4)&15) + 8;
        Cb = 32*((i>>2)&3)  - 48;
        Cr = 32*(i&3)       - 48;

        Y = cym * Y + cya;

        R = refresh_rect.rgb_clip[(Y + crv*Cr)>>FIXBITS];
        G = refresh_rect.rgb_clip[(Y + cgu*Cb + cgv*Cr)>>FIXBITS];
        B = refresh_rect.rgb_clip[(Y + cbu*Cb)>>FIXBITS];
        refresh_rect.palette[i] = (R << 16) | (G << 8) | B;
      }
      set_palette(256);
    }
  } else if ((depth == 8) && (mode == REFRESH_ARGB32)) {
    if (prefs.colormode == PREFS_GRAY) {
      for (i=0; i<256; i++) {
        col = i; /* 8 bits */
        y = i;

        if (prefs.gray_depth == 6) {
          col = i >> 2; /* 6 bits */
          y = (col * 255) / 63;
        } else if (prefs.gray_depth == 4) {
          col = i >> 4; /* 4 bits */
          y = (col * 255) / 15;
        }

        refresh_rect.palette[col] = (y << 16) | (y << 8) | y;
      }

      if (prefs.gray_depth == 8) {
        set_palette(256);
      } else if (prefs.gray_depth == 6) {
        set_palette(64);
      } else {
        set_palette(16);
      }
    } else {
      /* Set a 3:3:2 palette */
      for (i=0; i<256; i++) {
        R = (i & 0xe0);
        G = ((i & 0x1c) << 3);
        B = ((0xe0*((i & 0x03) << 6))/0xc0);
        refresh_rect.palette[i] = (R << 16) | (G << 8) | B;
      }
    }
    set_palette(256);
  }

  return 0;
}

void refresh_dummy(rect_type *rect, unsigned char *src[])
{
}

/* Returns a pointer to a suitable refresh routine */
int refresh_open(int w, int h, int l_w, unsigned long mode, double framerate)
{
  unsigned long search = 0x00000000;
  refresh_t temp = NULL;
  int i, ystart = 0, aga_width;

  /* Init */
  refresh_func = refresh_dummy;
  if (init_refresh(w, h, mode) < 0) {
    return -1;
  }

  refresh_rect.line_width = l_w; /* FIXME: Clean up this "rect" mess */

  refresh_rect.src_row_length = l_w / 4; /* LONG */
  refresh_rect.dst_row_length = refresh_rect.bpr / 4; /* LONG */

  if (prefs.colormode == PREFS_HAM) {
    refresh_rect.screen_width /= prefs.ham_width;
  }

  if (refresh_rect.screen_width < refresh_rect.width) {
    refresh_rect.src_offset = (refresh_rect.width - refresh_rect.screen_width) / 8; /* LONG */
    refresh_rect.dst_offset = 0;
    refresh_rect.xloop = refresh_rect.screen_width;
  } else {
    refresh_rect.src_offset = 0;
    refresh_rect.dst_offset = (refresh_rect.screen_width - refresh_rect.width) / 8; /* LONG */
    refresh_rect.xloop = refresh_rect.width;
  }

  if (prefs.colormode == PREFS_HAM) {
    refresh_rect.screen_width *= prefs.ham_width;
    refresh_rect.xloop *= prefs.ham_width;
    refresh_rect.dst_offset *= prefs.ham_width;
  }

  refresh_rect.src_uv_offset = refresh_rect.src_offset; /* HACK */
  refresh_rect.dst_offset *= refresh_rect.bpp;

  if (prefs.half == PREFS_ON) {
    if ((refresh_rect.screen_height * 2) < refresh_rect.height) {
      refresh_rect.src_offset += ((refresh_rect.height - (refresh_rect.screen_height * 2)) / 2) * (refresh_rect.src_row_length);
      refresh_rect.dst_offset += 0;
      refresh_rect.yloop = refresh_rect.screen_height;
      ystart = (refresh_rect.height - (refresh_rect.screen_height * 2)) / 2;
    } else {
      refresh_rect.src_offset += 0;
      refresh_rect.dst_offset += (((refresh_rect.screen_height * 2) - refresh_rect.height) / 4) * (refresh_rect.dst_row_length);
      refresh_rect.yloop = refresh_rect.height / 2;
      ystart = 0;
    }
  } else {
    if (refresh_rect.screen_height < refresh_rect.height) {
      refresh_rect.src_offset += ((refresh_rect.height - refresh_rect.screen_height) / 2) * (refresh_rect.src_row_length);
      refresh_rect.dst_offset += 0;
      refresh_rect.yloop = refresh_rect.screen_height;
      ystart = (refresh_rect.height - refresh_rect.screen_height) / 2;
    } else {
      refresh_rect.src_offset += 0;
      refresh_rect.dst_offset += ((refresh_rect.screen_height - refresh_rect.height) / 2) * (refresh_rect.dst_row_length);
      refresh_rect.yloop = refresh_rect.height;
      ystart = 0;
    }
  }

  refresh_rect.src_uv_offset = ((refresh_rect.src_offset - refresh_rect.src_uv_offset) / 4) + refresh_rect.src_uv_offset / 2;

  /* AGA */
  aga_width = (refresh_rect.xloop + 63) & ~63;

  refresh_rect.aga_xloop = aga_width / 32;
  refresh_rect.aga_yloop = refresh_rect.yloop;

  refresh_rect.aga_src_offset = refresh_rect.src_offset;
  refresh_rect.aga_dst_offset = refresh_rect.dst_offset / 8;

  refresh_rect.aga_dst_col_skip = (refresh_rect.screen_width - aga_width) / 32;

  refresh_rect.aga_left_over_l = (refresh_rect.xloop & 63) / 16;
  refresh_rect.aga_left_over_r = refresh_rect.aga_left_over_l;
  refresh_rect.aga_rasize = (refresh_rect.screen_width * refresh_rect.screen_height) / 32;

  /* Function */
  if (prefs.colormode == PREFS_HAM) {
    if (prefs.ham_depth == 8) {
      search |= M_HAM8;
    } else {
      search |= M_HAM6;
    }
    if (prefs.ham_width == 1) {
      if (prefs.ham_quality == PREFS_NORMAL) {
        search |= 1;
      } else {
        search |= 4;
      }
    } else if (prefs.ham_width == 2) {
      search |= 2;
    } else if (prefs.ham_width == 4) {
      search |= 3;
    }
  } else {
    if (prefs.colormode == PREFS_GRAY) {
      search |= M_GRAY;
    }
    if (refresh_rect.screen_type == TYPE_OVERLAY) {
      search |= M_OVER;
    } else if ((prefs.window == PREFS_ON) && (depth == 8)) {
      search |= M_WIN8;
    } else if (refresh_rect.screen_type == TYPE_PLANAR) {
      search |= M_AGA8;
      if (prefs.colormode == PREFS_GRAY) {
        search |= prefs.gray_depth;
      }
    } else {
      search |= refresh_rect.pixel_mode;
    }
  }

  i = 0;
  while (refreshers[i].mode != 0x00000000) {
    if (refreshers[i].mode == search) {
      if (refresh_rect.mode == REFRESH_YUV420) {
        temp = refreshers[i].yuv.refresh;
      } else if (refresh_rect.mode == REFRESH_ARGB32) {
        temp = refreshers[i].argb.refresh;
      } else if (refresh_rect.mode == REFRESH_LUT8) {
        temp = refreshers[i].lut.refresh;
      }
      printf("REFRESH FOUND: %08lx\n", search);
      break;
    }
    i++;
  }

  if (prefs.half == PREFS_ON) {
    refresh_rect.aga_half_height = 1;
  }

  debug_printf("OSD & SUBTITLE init\n");

  /* Init OSD stuff */
  if (prefs.osd == PREFS_ON) {
    osd_init(w, h, l_w, refresh_rect.mode);
    subtitle_init(framerate);
  }

  debug_printf("%d %d %d %d %d %d %d : %d : %d %d %d %d %d %d %d %d : %d\n",
    refresh_rect.xloop,
    refresh_rect.yloop,
    refresh_rect.src_offset,
    refresh_rect.dst_offset,
    refresh_rect.src_row_length,
    refresh_rect.dst_row_length,
    refresh_rect.src_uv_offset,
    refresh_rect.aga_half_height,
    refresh_rect.aga_xloop,
    refresh_rect.aga_yloop,
    refresh_rect.aga_src_offset,
    refresh_rect.aga_dst_offset,
    refresh_rect.aga_dst_col_skip,
    refresh_rect.aga_left_over_l,
    refresh_rect.aga_left_over_r,
    refresh_rect.aga_rasize,
    ystart);

  refresh_func = temp;

  refresh_rect.use_osd = &refresh_rect.buf_use_osd[ystart];

  if (refresh_func == NULL) {
    refresh_func = refresh_dummy;
    return -1;
  }

  return 0;
}

void refresh_image(unsigned char *src[3], double image_time)
{
  if (refresh_rect.init != NULL) {
    refresh_rect.init();
  }

  if (prefs.osd == PREFS_ON) {
    subtitle_refresh(src, image_time);
  }

  refresh_func(&refresh_rect, src);

  if (refresh_rect.exit != NULL) {
    refresh_rect.exit();
  }
}

void refresh_palette(unsigned long *palette, int entries)
{
  int i, y, col, cr = CR, cg = CG, cb = CB, fr = FR;
  unsigned char gray;
  unsigned long val;

  memset(refresh_rect.palette, 0, 256*4);
  memcpy(refresh_rect.palette, palette, entries*4);

  /* GRAY lookup table */
  for (i=0; i<256; i++) {
    val = refresh_rect.palette[i];
    gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
    refresh_rect.gray[i] = gray;

    if (prefs.colormode == PREFS_GRAY) {
      refresh_rect.palette[i] = (gray << 16) | (gray << 8) | gray;
    }
  }

  if ((prefs.colormode == PREFS_GRAY) && (prefs.gray_depth != 8)) {
    for (i=0; i<256; i++) {
      if (prefs.gray_depth == 6) {
        col = i >> 2; /* 6 bits */
        y = (col * 255) / 63;
      } else {
        col = i >> 4; /* 4 bits */
        y = (col * 255) / 15;
      }
      refresh_rect.palette[col] = (y << 16) | (y << 8) | y;
    }

    if (prefs.gray_depth == 6) {
      set_palette(64);
    } else {
      set_palette(16);
    }
  } else {
    set_palette(entries);
  }

  if (prefs.osd == PREFS_ON) {
    osd_init_palette(NULL, NULL, 0);
  }
}

void refresh_yuv2rgb(int crv, int cgu, int cgv, int cbu, int cym, int cya, int cuv)
{
  refresh_rect.crv = crv;
  refresh_rect.cgu = cgu;
  refresh_rect.cgv = cgv;
  refresh_rect.cbu = cbu;
  refresh_rect.cym = cym;
  refresh_rect.cya = cya;
  refresh_rect.cuv = cuv;
}

/* Performs a speedtest */
static void refresh_speedtest(void)
{
  #define FRAMES 500
  struct timeval st = { 0, 0 }, dt = { 0, 0 };
  unsigned char *buf[3];
  double time;
  int i;

#if 0
  if (refresh_open(352, 288, 352, REFRESH_ARGB32, 25.0) < 0) {
    return;
  }
#else
  if (refresh_open(352, 288, 352, REFRESH_YUV420, 25.0) < 0) {
    return;
  }
#endif

  buf[0] = malloc(352*288*4);
  buf[1] = malloc(352*288/4);
  buf[2] = malloc(352*288/4);
  memset(buf[0], 0, 352*288*4);
  memset(buf[1], 128, 352*288/4);
  memset(buf[2], 128, 352*288/4);

  GetSysTimePPC(&st);
  for (i=0; i<FRAMES; i++) {
    refresh_func(&refresh_rect, buf);
  }
  GetSysTimePPC(&dt);
  SubTimePPC(&dt, &st);
  time = (double)dt.tv_sec + ((double)dt.tv_usec / 1000000.0);
  printf("Time: %.3f, Frames: %d, FPS: %.3f\n", time, FRAMES, ((double)FRAMES / time));

  refresh_close();
}

/* Performs a test */
void refresh_test(char *filename)
{
  unsigned char *buf[3];
  int refresh = 0;
  FILE *fp = NULL;

  if (strstr(filename, ".yuv") != NULL) {
    refresh = REFRESH_YUV420;
    refresh_rect.crv = 6694222;
    refresh_rect.cgu = -1643170;
    refresh_rect.cgv = -3409833;
    refresh_rect.cbu = 8460885;
    refresh_rect.cym = 4883779;
    refresh_rect.cya = -617108528;
    refresh_rect.cuv = 0x80808080;
  } else if (strstr(filename, ".cvid") != NULL) {
    refresh = REFRESH_YUV420;
    refresh_rect.crv = 8388608;
    refresh_rect.cgu = -2097152;
    refresh_rect.cgv = -4194304;
    refresh_rect.cbu = 8388608;
    refresh_rect.cym = 4194304;
    refresh_rect.cya = -606076928;
    refresh_rect.cuv = 0x00000000;
  } else if (strstr(filename, ".argb") != NULL) {
    refresh = REFRESH_ARGB32;
  } else if (strstr(filename, ".lut") != NULL) {
    refresh = REFRESH_LUT8;
  }

  if (refresh == 0) {
    refresh_speedtest();
    return;
  }

  buf[0] = malloc(352*288*4);
  memset(buf[0], 0, 352*288*4);
  buf[1] = buf[0] + (352*288);
  buf[2] = buf[1] + (352*288/4);

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("file %s not found\n", filename);
    return;
  }
  fread(buf[0], 1, 352*288*4, fp);

  if (refresh_open(352, 288, 352, refresh, 25.0) < 0) {
    return;
  }

  if (refresh == REFRESH_LUT8) {
    refresh_palette((unsigned long *)(buf[0] + 352*288), 256);
  }

  refresh_image(buf, 0.0);

  /* Stupid hack for stupid cgxvideo */
  if (refresh_rect.screen_type == TYPE_OVERLAY) {
    if (refresh_rect.init != NULL) {
      refresh_rect.init();
    }
    if (refresh_rect.exit != NULL) {
      refresh_rect.exit();
    }
  }

  for (;;) {
    volatile unsigned char *lmb = (unsigned char *)0xbfe001;
    if ((*lmb & 0x40) == 0) { /* LMB */
      break;
    }
    WaitTime(0, 100000);
  }

  refresh_close();
}

/* Close the screen/window */
void refresh_close(void)
{
  close_screen();
  if (prefs.osd == PREFS_ON) {
    subtitle_exit();
    osd_exit();
  }
}

/* Allocate resources */
int refresh_init()
{
  int i, v;

  /* Reset refresh structure */
  memset(&refresh_rect, 0, sizeof(rect_type));

  /* Allocate line flags */
  refresh_rect.buf_use_osd = malloc(1024);
  memset(refresh_rect.buf_use_osd, 0, 1024);

  refresh_rect.use_osd = refresh_rect.buf_use_osd;

  /* Allocate palette */
  refresh_rect.palette = malloc(256 * 4);
  memset(refresh_rect.palette, 0, 256 * 4);

  /* GRAY lookup table */
  refresh_rect.gray = malloc(256);
  memset(refresh_rect.gray, 0, 256);

  /* Clip table */
  refresh_rect.buf_rgb_clip = (unsigned char *)malloc(2048);
  memset(refresh_rect.buf_rgb_clip, 0, 2048);

  refresh_rect.rgb_clip = refresh_rect.buf_rgb_clip;
  refresh_rect.rgb_clip += 1024;
  refresh_rect.clip = refresh_rect.rgb_clip;

  for (i=-1024; i<1024; i++) {
    refresh_rect.rgb_clip[i] = (i<0) ? 0 : ((i>255) ? 255 : i);
  }

  /* [-277, 534] => (534 - 277) / 2 = 129
   *
   * [-277, 534] - [129, 129] = [-406, 405]
   *
   * This change will give us one bit more to play with :)
   */
  refresh_rect.rgb_clip -= (-129);

  /* ITU-R Rec. 624-4 System B, G */
  /* SMPTE 170M */

  /*
   * 6694222 -1643170 -3409833 8460885 4883779 -617108528 0x80808080
   * 10.22 : max:  405, min: -406, errors:      169, mse:      169
   */

  refresh_rect.crv = 6694222;
  refresh_rect.cgu = -1643170;
  refresh_rect.cgv = -3409833;
  refresh_rect.cbu = 8460885;
  refresh_rect.cym = 4883779;
  refresh_rect.cya = -617108528;
  refresh_rect.cuv = 0x80808080;

  /* CVID, Radius Cinepak */

  /*
   * 8388608 -2097152 -4194304 8388608 4194304 -606076928 0x00000000
   * 10.22 : max:  364, min: -401, errors:        0, mse:        0
   */

  /* 8bit YUV to RGB dither */

  refresh_rect.y_table = malloc(256+16);
  refresh_rect.cb_table = malloc(128+16);
  refresh_rect.cr_table = malloc(128+16);

  for (i=-8; i<256+8; i++) {
    v = i>>4;
    if (v<0) {
      v = 0;
    } else if (v>15) {
      v = 15;
    }
    refresh_rect.y_table[i+8] = v<<4;
  }

  for (i=0; i<128+16; i++) {
    v = (i-40)>>4;
    if (v<0) {
      v = 0;
    } else if (v>3) {
      v = 3;
    }
    refresh_rect.cb_table[i] = v<<2;
    refresh_rect.cr_table[i] = v;
  }

  return 0;
}

/* Free any allocated resources */
void refresh_exit(void)
{
  /* FIXME: Memory leak */
}
