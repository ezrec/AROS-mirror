/*
 *
 * argb2lut.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void argb2lut(rect_type *rect, unsigned char *src[])
{
#define PIXEL(x) (x)
  unsigned long val, lut, *argb, *dst, r, g, b;
  int i, j, add, row_add;
  unsigned char *clip;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst + rect->dst_offset;
  clip = rect->clip - 8;

  for (j=0; j<rect->yloop; j++) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    switch (j & 0x03) {
      case 0:
        DITHER_RGB_LINE0(rect->xloop);
        break;

      case 1:
        DITHER_RGB_LINE1(rect->xloop);
        break;

      case 2:
        DITHER_RGB_LINE2(rect->xloop);
        break;

      case 3:
        DITHER_RGB_LINE3(rect->xloop);
        break;
    }

    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 2);
  }
#undef PIXEL
}

void argb2lut_gray(rect_type *rect, unsigned char *src[])
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

    for (i=0; i<(rect->xloop >> 2); i++) {
      val = *argb++;
      gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 24;
      val = *argb++;
      gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 16;
      val = *argb++;
      gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 8;
      val = *argb++;
      gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
      *dst++ = gray;
    }
    add += row_add;
    dst += rect->dst_row_length - (rect->xloop >> 2);
  }
}
