/*
 *
 * yuv2win.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void yuv2win(rect_type *rect, unsigned char *src[])
{
#define PIXEL(x) (pixel[x])
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst, *py, *pu, *pv, y, u, v, val, tu, tv;
  int i, j, src_add, dst_add, cuv;
  unsigned char *y_table, *cb_table, *cr_table;
  unsigned char *pixel;

  cuv = rect->cuv ^ 0x80808080;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  y_table = rect->y_table;
  cb_table = rect->cb_table;
  cr_table = rect->cr_table;

  src_add = rect->src_row_length - (rect->xloop >> 2);
  dst_add = rect->dst_row_length - (rect->xloop >> 2);

  dst = rect->dst;
  dst += rect->dst_offset;

  pixel = rect->pixel;

  for (j=0; j<rect->yloop; j+=2) {
    if (rect->use_osd[j]) {
      py = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    if ((j & 2) == 0) {
      DITHER_YUV_LINE0(rect->xloop)
      pu -= rect->xloop >> 3;
      pv -= rect->xloop >> 3;
      py += src_add;
      dst += dst_add;

      DITHER_YUV_LINE1(rect->xloop)
      dst += dst_add;
    } else {
      DITHER_YUV_LINE2(rect->xloop)
      pu -= rect->xloop >> 3;
      pv -= rect->xloop >> 3;
      py += src_add;
      dst += dst_add;

      DITHER_YUV_LINE3(rect->xloop)
      dst += dst_add;
    }

    yadd += yrow;
    yadd += yrow;
    uvadd += uvrow;
  }
#undef PIXEL
}

void yuv2win_gray(rect_type *rect, unsigned char *src[])
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
