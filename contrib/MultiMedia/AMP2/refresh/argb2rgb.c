/*
 *
 * argb2rgb.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void argb2argb32(rect_type *rect, unsigned char *src[])
{
  unsigned long *argb, *dst;
  int j, add, row_add;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst + rect->dst_offset;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    memcpy(dst, argb, (rect->xloop << 2));
    add += row_add;
    dst += rect->dst_row_length;
  }
}

void argb2bgr24(rect_type *rect, unsigned char *src[])
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

    for (i=0; i<(rect->xloop >> 2); i++) {
      val = *argb++;
      rgb = ((val & 0x000000ff) << 24) | ((val & 0x0000ff00) << 8) | ((val & 0x00ff0000) >> 8);
      val = *argb++;

      *dst++ = rgb | (val & 0x000000ff);
      rgb = ((val & 0x0000ff00) << 16) | (val & 0x00ff0000);
      val = *argb++;

      *dst++ = rgb | ((val & 0x000000ff) << 8) | ((val & 0x0000ff00) >> 8);
      rgb = ((val & 0x00ff0000) << 8);
      val = *argb++;

      *dst++ = rgb | ((val & 0x000000ff) << 16) | (val & 0x0000ff00) | ((val & 0x00ff0000) >> 16);
    }
    add += row_add;
    dst += rect->dst_row_length - ((rect->xloop * 3) >> 2);
  }
}

void argb2rgb16(rect_type *rect, unsigned char *src[])
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
      *dst++ = rgb | ((val & 0x00f80000) >> 8) | ((val & 0x0000fc00) >> 5) | ((val & 0x000000f8) >> 3);
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2rgb15(rect_type *rect, unsigned char *src[])
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
      rgb = ((val & 0x00f80000) << 7) | ((val & 0x0000f800) << 10) | ((val & 0x000000f8) << 13);
      val = *argb++;
      *dst++ = rgb | ((val & 0x00f80000) >> 9) | ((val & 0x0000f800) >> 6) | ((val & 0x000000f8) >> 3);
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

/* First change ABCD to CDAB then write byteswapped to get BADC */

void argb2rgb16pc(rect_type *rect, unsigned char *src[])
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
      rgb = ((val & 0x00f80000) >> 8) | ((val & 0x0000fc00) >> 5) | ((val & 0x000000f8) >> 3);
      val = *argb++;
      rgb |= ((val & 0x00f80000) << 8) | ((val & 0x0000fc00) << 11) | ((val & 0x000000f8) << 13);
      SWAP32_W(dst, rgb);
      dst++;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2rgb15pc(rect_type *rect, unsigned char *src[])
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
      rgb = ((val & 0x00f80000) >> 9) | ((val & 0x0000f800) >> 6) | ((val & 0x000000f8) >> 3);
      val = *argb++;
      rgb |= ((val & 0x00f80000) << 7) | ((val & 0x0000f800) << 10) | ((val & 0x000000f8) << 13);
      SWAP32_W(dst, rgb);
      dst++;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2bgra32(rect_type *rect, unsigned char *src[])
{
  unsigned long *argb, *dst, *s, *d;
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

    s = argb;
    d = dst;
    for (i=0; i<rect->xloop; i++) {
      SWAP32_W(d, *s);
      d++;
      s++;
    }

    add += row_add;
    dst += rect->dst_row_length;
  }
}
