/*
 *
 * yuv2overlay.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void yuv2overlay(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst, *py, *pu, *pv, y, u, v, cuv;
  int i, j;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  cuv = rect->cuv ^ 0x80808080; /* Overlay wants it the other way around */

  dst = rect->dst;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      py = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    for (i=0; i<(rect->xloop >> 3); i++) {
      y = *py++;
      u = *pu++ ^ cuv;
      v = *pv++ ^ cuv;
      *dst++ = (y&0xff000000) | ((u>>8)&0x00ff0000) | ((y>>8)&0x0000ff00) | (v>>24);
      *dst++ = ((y<<16)&0xff000000) | (u&0x00ff0000) | ((y<<8)&0x0000ff00) | ((v>>16)&0x000000ff);
      y = *py++;
      *dst++ = (y&0xff000000) | ((u<<8)&0x00ff0000) | ((y>>8)&0x0000ff00) | ((v>>8)&0x000000ff);
      *dst++ = ((y<<16)&0xff000000) | ((u<<16)&0x00ff0000) | ((y<<8)&0x0000ff00) | (v&0x000000ff);
    }

    yadd += yrow;
    uvadd += (uvrow * (j & 1));
  }
}

void yuv2overlay_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, col;
  int x, y, xloop;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;

  xloop = rect->xloop >> 2;
  for (y = 0; y < rect->yloop; y++) {
    if (rect->use_osd[y]) {
      sptr = ((unsigned long *)rect->osd[0]) + yadd;
    } else {
      sptr = ((unsigned long *)src[0]) + yadd;
    }

    for (x = 0; x < xloop; x++) {
      col = *sptr++;
      *dptr++ = (col & 0xff000000) | ((col & 0x00ff0000) >> 8) | 0x00800080;
      *dptr++ = ((col & 0x0000ff00) << 16) | ((col & 0x000000ff) << 8) | 0x00800080;
    }

    yadd += yrow;
  }
}
