/*
 *
 * argb2overlay.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void argb2overlay(rect_type *rect, unsigned char *src[])
{
  unsigned long val, rgb, *argb, *dst;
  int i, j, add, row_add;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst + rect->dst_offset;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    for (i=0; i<(rect->xloop >> 1); i++) {
      val = *argb++;
      rgb = ((val & 0x00f80000) << 8) | ((val & 0x0000fc00) << 11) | ((val & 0x000000f8) << 13);
      val = *argb++;
      rgb |= ((val & 0x00f80000) >> 8) | ((val & 0x0000fc00) >> 5) | ((val & 0x000000f8) >> 3);

      /* Stupid CGFX wants it in little endian mode, so flip/flop it */
      rgb = ((rgb & 0xff00ff00) >> 8) | ((rgb & 0x00ff00ff) << 8);
      *dst++ = rgb;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2overlay_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long val, *argb, *dst, gray;
  int i, j, add, row_add;
  int cr = CR, cg = CG, cb = CB, fr = FR;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst + rect->dst_offset;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    for (i=0; i<(rect->xloop >> 1); i++) {
      val = *argb++;
      gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 24;
      val = *argb++;
      gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 8;
      *dst++ = gray | 0x00800080;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}
