/*
 *
 * lut2gray.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void lut2argb32_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *gray;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      val = gray[(col >> 24) & 0xff];
      *ptr++ = (val << 16) | (val << 8) | val;
      val = gray[(col >> 16) & 0xff];
      *ptr++ = (val << 16) | (val << 8) | val;
      val = gray[(col >> 8) & 0xff];
      *ptr++ = (val << 16) | (val << 8) | val;
      val = gray[col & 0xff];
      *ptr++ = (val << 16) | (val << 8) | val;
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

void lut2bgr24_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, col;
  unsigned char *gray, g1, g2;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      g1 = gray[col >> 24];
      g2 = gray[(col >> 16) & 0xff];
      *ptr++ = (g1 << 24) | (g1 << 16) | (g1 << 8) | g2;
      g1 = gray[(col >> 8) & 0xff];
      *ptr++ = (g2 << 24) | (g2 << 16) | (g1 << 8) | g1;
      g2 = gray[col & 0xff];
      *ptr++ = (g1 << 24) | (g2 << 16) | (g2 << 8) | g2;
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

void lut2rgb16_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *gray;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      val = ((gray[(col >> 24)] & 0xf8) << 13) | ((gray[(col >> 16) & 0xff] & 0xf8) >> 3);
      *ptr++ = val | (val << 6) | (val << 11);
      val = ((gray[(col >> 8) & 0xff] & 0xf8) << 13) | ((gray[col & 0xff] & 0xf8) >> 3);
      *ptr++ = val | (val << 6) | (val << 11);
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

void lut2rgb15_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *gray;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      val = ((gray[(col >> 24)] & 0xf8) << 13) | ((gray[(col >> 16) & 0xff] & 0xf8) >> 3);
      *ptr++ = val | (val << 5) | (val << 10);
      val = ((gray[(col >> 8) & 0xff] & 0xf8) << 13) | ((gray[col & 0xff] & 0xf8) >> 3);
      *ptr++ = val | (val << 5) | (val << 10);
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

/* First change ABCD to CDAB then write byteswapped to get BADC */

void lut2rgb16pc_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *gray;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      val = ((gray[(col >> 24)] & 0xf8) >> 3) | ((gray[(col >> 16) & 0xff] & 0xf8) << 13);
      val = val | (val << 6) | (val << 11);
      SWAP32_W(ptr, val);
      ptr++;
      val = ((gray[(col >> 8) & 0xff] & 0xf8) >> 3) | ((gray[col & 0xff] & 0xf8) << 13);
      val = val | (val << 6) | (val << 11);
      SWAP32_W(ptr, val);
      ptr++;
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

void lut2rgb15pc_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *gray;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      val = ((gray[(col >> 24)] & 0xf8) >> 3) | ((gray[(col >> 16) & 0xff] & 0xf8) << 13);
      val = val | (val << 5) | (val << 10);
      SWAP32_W(ptr, val);
      ptr++;
      val = ((gray[(col >> 8) & 0xff] & 0xf8) >> 3) | ((gray[col & 0xff] & 0xf8) << 13);
      val = val | (val << 5) | (val << 10);
      SWAP32_W(ptr, val);
      ptr++;
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}

void lut2bgra32_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, yadd;
  unsigned long *sptr, *dptr, *ptr, val, col;
  unsigned char *gray;
  int x, y, xloop;

  gray = rect->gray;

  yrow = rect->src_row_length;
  yadd = rect->src_offset;

  dptr = rect->dst;
  dptr += rect->dst_offset;

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
      val = gray[(col >> 24) & 0xff];
      *ptr++ = (val << 24) | (val << 16) | (val << 8);
      val = gray[(col >> 16) & 0xff];
      *ptr++ = (val << 24) | (val << 16) | (val << 8);
      val = gray[(col >> 8) & 0xff];
      *ptr++ = (val << 24) | (val << 16) | (val << 8);
      val = gray[col & 0xff];
      *ptr++ = (val << 24) | (val << 16) | (val << 8);
    }
    dptr += rect->dst_row_length;

    yadd += yrow;
  }
}
