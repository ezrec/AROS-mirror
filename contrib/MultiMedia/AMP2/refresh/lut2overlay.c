/*
 *
 * lut2overlay.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void lut2overlay(rect_type *rect, unsigned char *src[])
{
  unsigned long val, rgb, *dst, *pal;
  unsigned char *data;
  int i, j, add, row_add;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  pal = rect->palette;
  dst = rect->dst + rect->dst_offset;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      data = rect->osd[0] + add;
    } else {
      data = src[0] + add;
    }

    for (i=0; i<(rect->xloop >> 1); i++) {
      val = pal[(int)*data];
      data++;
      rgb = ((val & 0x00f80000) << 8) | ((val & 0x0000fc00) << 11) | ((val & 0x000000f8) << 13);
      val = pal[(int)*data];
      data++;
      rgb |= ((val & 0x00f80000) >> 8) | ((val & 0x0000fc00) >> 5) | ((val & 0x000000f8) >> 3);

      /* Stupid CGFX wants it in little endian mode, so flip/flop it */
      rgb = ((rgb & 0xff00ff00) >> 8) | ((rgb & 0x00ff00ff) << 8);
      *dst++ = rgb;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void lut2overlay_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long val, *dst;
  unsigned char *data, *gray;
  int i, j, add, row_add;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  gray = rect->gray;
  dst = rect->dst + rect->dst_offset;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      data = rect->osd[0] + add;
    } else {
      data = src[0] + add;
    }

    for (i=0; i<(rect->xloop >> 1); i++) {
      val = (gray[(int)*data] << 24);
      data++;
      val |= (gray[(int)*data] << 8);
      data++;
      *dst++ = val | 0x00800080;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}
