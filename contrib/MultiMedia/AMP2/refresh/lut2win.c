/*
 *
 * lut2win.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void lut2win(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *pixel;
  int x, y, xloop;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

  pixel = rect->pixel;

  xloop = rect->xloop >> 2;
  for (y = 0; y < rect->yloop; y++) {
    if (rect->use_osd[y]) {
      sptr = ((unsigned long *)rect->osd[0]) + yadd;
    } else {
      sptr = ((unsigned long *)src[0]) + yadd;
    }

    ptr = dptr;
    for (x = 0; x < xloop; x++) {
      col = *sptr++;
      val = (pixel[(col >> 24) & 0xff] << 24);
      val |= (pixel[(col >> 16) & 0xff] << 16);
      val |= (pixel[(col >> 8) & 0xff] << 8);
      val |= pixel[col & 0xff];
      *ptr++ = val;
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

void lut2win_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *pixel;
  int x, y, xloop;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

  pixel = rect->pixel;

  xloop = rect->xloop >> 2;
  for (y = 0; y < rect->yloop; y++) {
    if (rect->use_osd[y]) {
      sptr = ((unsigned long *)rect->osd[0]) + yadd;
    } else {
      sptr = ((unsigned long *)src[0]) + yadd;
    }

    ptr = dptr;
    for (x = 0; x < xloop; x++) {
      col = *sptr++;
      val = (pixel[(col >> 24) & 0xff] << 24);
      val |= (pixel[(col >> 16) & 0xff] << 16);
      val |= (pixel[(col >> 8) & 0xff] << 8);
      val |= pixel[col & 0xff];
      *ptr++ = val;
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}
