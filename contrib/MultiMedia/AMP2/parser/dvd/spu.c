/*
 *
 * spu.c
 *
 */

#undef DEBUG
//#define DEBUG

#include <stdio.h>
#include <stdlib.h>

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "spu.h"
#include "../../refresh/osd.h"
#include "../../main/buffer.h"

static double show_time, hide_time, show_time_offset, hide_time_offset;

static unsigned char *testing = NULL;

static int x1, x2, y1, y2;
static unsigned char *store, *store_lores;
static int store_x, store_y;
static int subtitle_lores;

/* 53220 bytes maximum size according to DVD specs, using 65536 (full 16bits) just in case */
static unsigned char buffer[65536];

static int packet_size;
static int id; /* 0: up, 1: down */
static int aligned;
static int offset[2];

static unsigned long clut[16];

static unsigned char color[4];
static unsigned char trans[4];

unsigned char menu_color[4];
unsigned char menu_trans[4];

static int x;
static int y;
static int width = 0;
static int height = 0;

static unsigned int get_byte(void)
{
  unsigned char byte;

  byte = buffer[offset[id]];
  offset[id]++;

  return byte;
}

static int get_nibble (void)
{
  static int next;

  if (aligned) {
    next = get_byte();
    aligned = 0;
    return next >> 4;
  } else {
    aligned = 1;
    return next & 0xf;
  }
}

static void send_rle(length, colorid)
{
#if 0
  if (trans[colorid] > 0) {
    /* Get actual subtitle size */
    if (top_x == -1) {
      top_x = x;
    } else if (x < top_x) {
      top_x = x;
    }

    if (bottom_x == -1) {
      bottom_x = x;
    } else if ((x + length) > bottom_x) {
      bottom_x = x + length;
    }

    if (top_y == -1) {
      top_y = y;
    }

    if (bottom_y == -1) {
      bottom_y = y;
    } else if (y > bottom_y) {
      bottom_y = y;
    }
  }
#endif

  while (length--) {
    if (x++ < width) {
      *testing++ = colorid;
    }
  }
}

static void next_line(void)
{
  testing += store_x - width;

  if (y >= height) {
    return;
  }
  x = 0;
  y++;
  id = 1 - id;
}

static void terminate(void)
{
  while (y < height) {
    next_line();
  }
}

static int render_subtitle(void)
{
  int i, data_size;

#ifdef DEBUG
  int j;
#endif

  data_size = (buffer[2] << 8) + buffer[3];

#ifdef DEBUG
printf("data_size: %d\n", data_size);
#endif

show_time_offset = (double)((buffer[data_size] << 8) + buffer[data_size+1]) / 100.0;
#ifdef DEBUG
printf("Show: %.2f\n", show_time_offset);
#endif

  /* parse the header */
  i = data_size + 4;
//  while ((i < packet_size) && (buffer[i] != 0xff)) {
  while (i < packet_size) {
    switch (buffer[i]) {
      case 0x00: /* FORCE DISPLAY */
        i++;
        break;

      case 0x01: /* SHOW */
        i++;
        break;

      case 0x02: /* HIDE */
//        i++; // = packet_size;
        i = packet_size;
        break;

      case 0x03: /* palette */
        color[3] = buffer[i+1] >> 4;
        color[2] = buffer[i+1] & 0xf;
        color[1] = buffer[i+2] >> 4;
        color[0] = buffer[i+2] & 0xf;
        i += 3;
#ifdef DEBUG
for (j=0; j<4; j++) { printf("color %d -> %d\n", j, color[j]); }
#endif
        break;

      case 0x04: /* transparency palette */
        trans[3] = buffer[i+1] >> 4;
        trans[2] = buffer[i+1] & 0xf;
        trans[1] = buffer[i+2] >> 4;
        trans[0] = buffer[i+2] & 0xf;
        i += 3;
#ifdef DEBUG
for (j=0; j<4; j++) { printf("trans %d : %d\n", j, trans[j]); }
#endif
        break;

      case 0x05: /* image coordinates */
        x2 = ((buffer[i+2] & 0x0f) << 8) + buffer[i+3];
        x1 = (buffer[i+1] << 4) + (buffer[i+2] >> 4);
        y2 = ((buffer[i+5] & 0x0f) << 8) + buffer[i+6];
        y1 = (buffer[i+4] << 4) + (buffer[i+5] >> 4);
        i += 7;
        width = x2 - x1 + 1;
        height = y2 - y1 + 1;
        testing += (y1 * store_x) + x1;
#ifdef DEBUG
printf("coord: %dx%d -> %dx%d : %dx%d\n", x1,y1,x2,y2,width,height);
#endif
        break;

      case 0x06: /* image 1 / image 2 offsets */
        offset[0] = (buffer[i+1] << 8) + buffer[i+2];
        offset[1] = (buffer[i+3] << 8) + buffer[i+4];
        i += 5;
#ifdef DEBUG
printf("Offset: %d, %d\n", offset[0], offset[1]);
#endif
        break;

      case 0x07: /* WIPE */
        break;

      case 0xff: /* "ff xx yy zz uu" with 'zz uu' == start of control packet */
hide_time_offset = (double)((buffer[i+1] << 8) + buffer[i+2]) / 100.0;
#ifdef DEBUG
printf("Hide: %.2f\n", hide_time_offset);
#endif
        i += 5; /* Don't bother to parse this one */
        break;

      default:
        printf("invalid sequence in control header (%.2x) @ %d !\n", buffer[i], i);
        i++; /* Skip and hope for the best */
    }
  }

  id = 0;
  aligned = 1;

  x = 0;
  y = 0;

#if 1
  while ((offset[1] < data_size + 2) && (y < height)) {
    int code;

    code = get_nibble();
    if (code >= 0x4) { /* 4 .. f */
      found_code:
      send_rle (code >> 2, code & 3);
      if (x >= width) {
        if (!aligned) {
          get_nibble();
        }
        next_line();
      }
      continue;
    }

    code = (code << 4) + get_nibble();
    if (code >= 0x10) { /* 1x .. 3x */
      goto found_code;
    }
     
    code = (code << 4) + get_nibble();
    if (code >= 0x40) { /* 04x .. 0fx */
      goto found_code;
    }

    code = (code << 4) + get_nibble();
    if (code >= 0x100) { /* 01xx .. 03xx */
      goto found_code;
    }

    /* 00xx - should only happen for 00 00 */
    send_rle(width, code & 3);
    if (!aligned) {
      code = (code << 4) + get_nibble(); /* 0 0x xx */
    }

/*
    if (code) {
      printf("got unknown code 00%x (offset %x side %x, x=%d, y=%d)\n", code, offset[id], id, x, y);
      next_line ();
      continue;
    }
*/

    /* aligned 00 00 */
    next_line();
  }
#else
  while ((offset[1] < data_size + 2) && (y < height)) {
    int code;

    code = get_nibble();
    if (code < 0x0004) {
      code = (code << 4) | get_nibble();
      if (code < 0x0010) {
        code = (code << 4) | get_nibble();
        if (code < 0x0040) {
          code = (code << 4) | get_nibble();
        }
      }
    }

    if ((code & 0xff00) == 0) {
      send_rle(width, code & 3);
      if (!aligned) {
        get_nibble();
      }
      next_line();
    } else {
      send_rle(code >> 2, code & 3);
      if (x >= width) {
        if (!aligned) {
          get_nibble();
        }
        next_line();
      }
    }
  }
#endif

  terminate();

//  top_x += x1;
//  top_y += y1;
//  bottom_x += x1;
//  bottom_y += y1;

//#ifdef DEBUG
//printf("%d %d %d %d\n", top_x, top_y, bottom_x, bottom_y);
//#endif

  return 0;
}

int spu_setup(int width, int height, int lores)
{
  printf("SUBTITLE: %d x %d\n", width, height);

#if 0
  if (lores) {
    /* FIXME: REMOVE SOMEHOW! */
    width *= 2;
    height *= 2;
#ifdef DEBUG
    printf("-> %d x %d\n", width, height);
#endif
  }
#endif

  show_time = 0.0;
  hide_time = 0.0;
  store_x = width;
  store_y = height;

  store = malloc(width * height);
  memset(store, 0, width * height);

  store_lores = malloc(width * height);
  memset(store_lores, 0, width * height);

  subtitle_lores = lores;
  return 1;
}

void spu_exit(void)
{
}

void spu_palette(unsigned char *palette)
{
  /* Palette stored as ?YUV?YUV?YUV... */
  memcpy(clut, palette, 16*4);
}

static int do_show_sub = 0, state = 0, bytes_left = 0, buffer_pos = 0;

//static int lazy_state = 0; /* = 1 -> Wait for PTS/DTS */

static int lazy_fread(unsigned char *buffer, int length)
{
  static unsigned char *lazy_buffer = NULL;
  static int buffer_len = 0;
  static int buffer_pos = 0;
  double dummy = -1.0;
  int i;

  for (i=0; i<length; i++) {
    if (buffer_pos >= buffer_len) {
      buffer_len = subtitle_fread(&lazy_buffer, &dummy);
      if (buffer_len <= 0) {
        return i;
      }

/*
      if (dummy > 0.0) {
        show_time = dummy;
        lazy_state = 0;
      } else if (lazy_state == 1) {
        return 0;
      }
*/
      /* FIXME: Is this true anymore ??? */
/*
      if ((buffer_len == 0) && (dummy == -2.0)) {
        printf("SUBTITLE SEEKING\n");
        show_time = 0.0;
        state = 0;
        lazy_state = 1;
        return 0;
      }
*/
      buffer_pos = 0;
    }
    buffer[i] = lazy_buffer[buffer_pos++];
  }

  return length;
}

int dvd_menu = 0, dvd_x, dvd_y, dvd_w, dvd_h; /* FIXME, must be protected by semaphore !!! */

void spu_render(unsigned char *src[], double video_time)
{
  int bytes_read, i;

show_sub:
  if (do_show_sub) {
    /* we need a palette */
    unsigned long clip_color[4];
    unsigned char *a, *c;

    if (dvd_menu) {
      a = menu_trans;
      c = menu_color;
    } else {
      a = trans;
      c = color;
    }

    for (i=0; i<4; i++) {
      clip_color[i] = clut[c[i]] & 0x00ffffff;
      clip_color[i] |= (((a[i] * 0xff) / 0x0f) << 24);
    }

    osd_init_palette(NULL, clip_color, 4);

#if 0
    osd_refresh(src, store, 0, 0, 720, 576, 720);
#else
    if (subtitle_lores) {
#if 0
    osd_refresh(src, store_lores, 0, 0, 720 / 2, 576 / 2, 720 / 2);
#else
      unsigned char *osd = store_lores;
      int x, y, w, h;

      x = dvd_x / 2;
      y = dvd_y / 2;
      w = dvd_w / 2;
      h = dvd_h / 2;

      osd += (360 * y) + x;
      osd_refresh(src, osd, x, y, w, h, 360);
#endif
    } else {
      unsigned char *osd = store;

      osd += (720 * dvd_y) + dvd_x;
      osd_refresh(src, osd, dvd_x, dvd_y, dvd_w, dvd_h, 720);
    }
#endif
  }

  if (show_time == 0.0) {
    if (state == 0) {
      if (lazy_fread(&buffer[0], 2) == 0) {
        return;
      }
      packet_size = (buffer[0] << 8) + buffer[1];

#ifdef DEBUG
printf("packet_size: %d\n", packet_size);
#endif

      state = 1;
      bytes_left = packet_size - 2;
      buffer_pos = 2;
    }

    if (state == 1) {
      bytes_read = lazy_fread(&buffer[buffer_pos], bytes_left);
      if (bytes_read > 0) {
        buffer_pos += bytes_read;
        bytes_left -= bytes_read;
      }
      if (bytes_left > 0) {
        return;
      }
      state = 0;
    }

    testing = store;

#ifdef DEBUG
printf("render subtitle\n");
#endif

    if (render_subtitle()) {
      return;
    }
    if (subtitle_lores) {
      int x, y, w, h;

      w = store_x / 2;
      h = store_y / 2;

      for (y=0; y<h; y++) {
        for (x=0; x<w; x++) {
          store_lores[(y*w)+x] = store[((y*2)*store_x) + (x*2)];
        }
      }
    }
    show_time += show_time_offset;
    hide_time = show_time + hide_time_offset;
#ifdef DEBUG
    printf("show_time: %.2f, hide_time: %.2f (current: %.2f)\n", show_time, hide_time, video_time);
#endif

do_show_sub = 1;
goto show_sub;

  }

#if 0
  if (video_time >= show_time) {
    do_show_sub = 1;
  }

  if (video_time >= hide_time) {
    show_time = 0.0;
    do_show_sub = 0;
  }

  /* If the user seeks backwards the current video time will/may get lower than show time, if this
     happends we will not get any subtitles until we have reached the previous place in the movie,
     this is not acceptable, so make sure this never happends */
  if ((do_show_sub == 1) && (video_time < show_time)) {
    show_time = 0.0;
    do_show_sub = 0;
  }
#endif
}
