/*
 *
 * argb2gray.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void argb2argb32_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long *argb, *dst, *dptr, gray, val;
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

    dptr = dst;
    for (i=0; i<rect->xloop; i++) {
      val = *argb++;
      gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      *dptr++ = (gray << 16) | (gray << 8) | gray;
    }
    add += row_add;
    dst += rect->dst_row_length;
  }
}

void argb2bgr24_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long val, *argb, *dst, gray1, gray2;
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

    for (i=0; i<(rect->xloop >> 2); i++) {
      val = *argb++;
      gray1 = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      val = *argb++;
      gray2 = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      *dst++ = (gray1 << 24) | (gray1 << 16) | (gray1 << 8) | gray2;

      val = *argb++;
      gray1 = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      *dst++ = (gray2 << 24) | (gray2 << 16) | (gray1 << 8) | gray1;

      val = *argb++;
      gray2 = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      *dst++ = (gray1 << 24) | (gray2 << 16) | (gray2 << 8) | gray2;
    }
    add += row_add;
    dst += rect->dst_row_length - ((rect->xloop * 3) >> 2);
  }
}

void argb2rgb16_gray(rect_type *rect, unsigned char *src[])
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
      gray = (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) << 13;
      val = *argb++;
      gray |= (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) >> 3;
      *dst++ = gray | (gray << 6) | (gray << 11);
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2rgb15_gray(rect_type *rect, unsigned char *src[])
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
      gray = (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) << 13;
      val = *argb++;
      gray |= (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) >> 3;
      *dst++ = gray | (gray << 5) | (gray << 10);
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

/* First change ABCD to CDAB then write byteswapped to get BADC */

void argb2rgb16pc_gray(rect_type *rect, unsigned char *src[])
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
      gray = (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) >> 3;
      val = *argb++;
      gray |= (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) << 13;
      gray = gray | (gray << 6) | (gray << 11);
      SWAP32_W(dst, gray);
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2rgb15pc_gray(rect_type *rect, unsigned char *src[])
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
      gray = (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) >> 3;
      val = *argb++;
      gray |= (RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) & 0xf8) << 13;
      gray = gray | (gray << 5) | (gray << 10);
      SWAP32_W(dst, gray);
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 1);
  }
}

void argb2bgra32_gray(rect_type *rect, unsigned char *src[])
{
  unsigned long *argb, *dst, *dptr, gray, val;
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

    dptr = dst;
    for (i=0; i<rect->xloop; i++) {
      val = *argb++;
      gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      *dptr++ = (gray << 24) | (gray << 16) | (gray << 8);
    }
    add += row_add;
    dst += rect->dst_row_length;
  }
}
