/*
 *
 * argb2aga.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void argb2aga(rect_type *rect, unsigned char *src[])
{
#define PIXEL(x) (x)
  unsigned long *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, d[8];
  unsigned long *dst, val, *argb, lut, r, g, b;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3, t4, t5, t6, t7;
  unsigned long temp1, temp2;
  int i, j, xloop, yloop, skip, line, pixels, add, row_add;
  unsigned char *clip;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  clip = rect->clip - 8;

  p0 = rect->dst + rect->aga_dst_offset;
  p1 = p0 + rect->aga_rasize;
  p2 = p1 + rect->aga_rasize;
  p3 = p2 + rect->aga_rasize;
  p4 = p3 + rect->aga_rasize;
  p5 = p4 + rect->aga_rasize;
  p6 = p5 + rect->aga_rasize;
  p7 = p6 + rect->aga_rasize;

  line = 0;
  skip = -1;
  j = 0;

  t0 = 0;
  t1 = 0;
  t2 = 0;
  t3 = 0;
  t4 = 0;
  t5 = 0;
  t6 = 0;
  t7 = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l << 3;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r << 3;
      } else {
        pixels = 32;
      }

      dst = &d[0];
      switch (line & 3) {
        case 0:
          DITHER_RGB_LINE0(pixels)
          break;
        case 1:
          DITHER_RGB_LINE1(pixels)
          break;
        case 2:
          DITHER_RGB_LINE2(pixels)
          break;
        case 3:
          DITHER_RGB_LINE3(pixels)
          break;
      }
      dst = &d[0];

      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        d0 = 0;
        d1 = 0;
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;

        switch(rect->aga_left_over_l) {
          case 3:
            d2 = *dst++;
            d3 = *dst++;
          case 2:
            d4 = *dst++;
            d5 = *dst++;
        }

        d6 = *dst++;
        d7 = *dst++;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;
        d6 = 0;
        d7 = 0;

        switch(rect->aga_left_over_r) {
          case 3:
            d5 = dst[5];
            d4 = dst[4];
          case 2:
            d3 = dst[3];
            d2 = dst[2];
        }

        d1 = dst[1];
        d0 = dst[0];
      } else {
        d0 = *dst++;
        d1 = *dst++;
        d2 = *dst++;
        d3 = *dst++;
        d4 = *dst++;
        d5 = *dst++;
        d6 = *dst++;
        d7 = *dst++;
      }

      *p0++ = t0;
      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      *p1++ = t1;
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      *p2++ = t2;
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      *p3++ = t3;
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      *p4++ = t4;
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      *p5++ = t5;
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      *p6++ = t6;
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);
      *p7++ = t7;

      t0 = d7;
      t1 = d5;
      t2 = d3;
      t3 = d1;
      t4 = d6;
      t5 = d4;
      t6 = d2;
      t7 = d0;

      if (skip) {
        p0 += skip;
        p1 += skip;
        p2 += skip;
        p3 += skip;
        p4 += skip;
        p5 += skip;
        p6 += skip;
        p7 += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    if (rect->aga_half_height) {
      add += row_add;
      add += row_add;
      j += 2;
    } else {
      add += row_add;
      j++;
    }

    line++;
  }

  *p0++ = t0;
  *p1++ = t1;
  *p2++ = t2;
  *p3++ = t3;
  *p4++ = t4;
  *p5++ = t5;
  *p6++ = t6;
  *p7++ = t7;
#undef PIXEL
}

void argb2aga_gray8(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7;
  unsigned long *sptr, *dst, d[8], *argb, val, gray;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3, t4, t5, t6, t7;
  unsigned long temp1, temp2;
  unsigned long row_add, add;
  int i, pixels, xloop, yloop, skip, y;
  int cr = CR, cg = CG, cb = CB, fr = FR;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst;
  dst += rect->aga_dst_offset;

  p0 = dst;
  p1 = p0 + rect->aga_rasize;
  p2 = p1 + rect->aga_rasize;
  p3 = p2 + rect->aga_rasize;
  p4 = p3 + rect->aga_rasize;
  p5 = p4 + rect->aga_rasize;
  p6 = p5 + rect->aga_rasize;
  p7 = p6 + rect->aga_rasize;

  skip = -1;
  y = 0;

  t0 = 0;
  t1 = 0;
  t2 = 0;
  t3 = 0;
  t4 = 0;
  t5 = 0;
  t6 = 0;
  t7 = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[y]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l << 3;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r << 3;
      } else {
        pixels = 32;
      }

      sptr = &d[0];
      for (i=0; i<pixels; i+=4) {
        val = *argb++;
        gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 24;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 16;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 8;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
        *sptr++ = gray;
      }
      sptr = &d[0];

      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        d0 = 0;
        d1 = 0;
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;

        switch(rect->aga_left_over_l) {
          case 3:
            d2 = *sptr++;
            d3 = *sptr++;
          case 2:
            d4 = *sptr++;
            d5 = *sptr++;
        }

        d6 = *sptr++;
        d7 = *sptr++;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;
        d6 = 0;
        d7 = 0;

        switch(rect->aga_left_over_r) {
          case 3:
            d5 = sptr[5];
            d4 = sptr[4];
          case 2:
            d3 = sptr[3];
            d2 = sptr[2];
        }

        d1 = sptr[1];
        d0 = sptr[0];
        src += rect->aga_left_over_r << 1;
      } else {
        d0 = *sptr++;
        d1 = *sptr++;
        d2 = *sptr++;
        d3 = *sptr++;
        d4 = *sptr++;
        d5 = *sptr++;
        d6 = *sptr++;
        d7 = *sptr++;
      }

      *p0++ = t0;
      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      *p1++ = t1;
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      *p2++ = t2;
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      *p3++ = t3;
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      *p4++ = t4;
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      *p5++ = t5;
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      *p6++ = t6;
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);
      *p7++ = t7;

      t0 = d7;
      t1 = d5;
      t2 = d3;
      t3 = d1;
      t4 = d6;
      t5 = d4;
      t6 = d2;
      t7 = d0;

      if (skip) {
        p0 += skip;
        p1 += skip;
        p2 += skip;
        p3 += skip;
        p4 += skip;
        p5 += skip;
        p6 += skip;
        p7 += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    if (rect->aga_half_height) {
      add += row_add;
      add += row_add;
      y += 2;
    } else {
      add += row_add;
      y++;
    }
  }

  *p0++ = t0;
  *p1++ = t1;
  *p2++ = t2;
  *p3++ = t3;
  *p4++ = t4;
  *p5++ = t5;
  *p6++ = t6;
  *p7++ = t7;
}

void argb2aga_gray6(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3, *p4, *p5;
  unsigned long *sptr, *dst, d[8], *argb, val, gray;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3, t4, t5;
  unsigned long temp1, temp2;
  unsigned long row_add, add;
  int i, pixels, xloop, yloop, skip, y;
  int cr = CR, cg = CG, cb = CB, fr = FR;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst;
  dst += rect->aga_dst_offset;

  p0 = dst;
  p1 = p0 + rect->aga_rasize;
  p2 = p1 + rect->aga_rasize;
  p3 = p2 + rect->aga_rasize;
  p4 = p3 + rect->aga_rasize;
  p5 = p4 + rect->aga_rasize;

  skip = -1;
  y = 0;

  t0 = 0;
  t1 = 0;
  t2 = 0;
  t3 = 0;
  t4 = 0;
  t5 = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[y]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l << 3;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r << 3;
      } else {
        pixels = 32;
      }

      sptr = &d[0];
      for (i=0; i<pixels; i+=4) {
        val = *argb++;
        gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 24;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 16;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 8;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
        *sptr++ = gray;
      }
      sptr = &d[0];

      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        d0 = 0;
        d1 = 0;
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;

        switch(rect->aga_left_over_l) {
          case 3:
            d2 = *sptr++;
            d3 = *sptr++;
          case 2:
            d4 = *sptr++;
            d5 = *sptr++;
        }

        d6 = *sptr++;
        d7 = *sptr++;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;
        d6 = 0;
        d7 = 0;

        switch(rect->aga_left_over_r) {
          case 3:
            d5 = sptr[5];
            d4 = sptr[4];
          case 2:
            d3 = sptr[3];
            d2 = sptr[2];
        }

        d1 = sptr[1];
        d0 = sptr[0];
        src += rect->aga_left_over_r << 1;
      } else {
        d0 = *sptr++;
        d1 = *sptr++;
        d2 = *sptr++;
        d3 = *sptr++;
        d4 = *sptr++;
        d5 = *sptr++;
        d6 = *sptr++;
        d7 = *sptr++;
      }

      *p0++ = t0;
      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      *p1++ = t1;
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      *p2++ = t2;
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      *p3++ = t3;
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      *p4++ = t4;
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);
      *p5++ = t5;

      t0 = d3;
      t1 = d1;
      t2 = d6;
      t3 = d4;
      t4 = d2;
      t5 = d0;

      if (skip) {
        p0 += skip;
        p1 += skip;
        p2 += skip;
        p3 += skip;
        p4 += skip;
        p5 += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    if (rect->aga_half_height) {
      add += row_add;
      add += row_add;
      y += 2;
    } else {
      add += row_add;
      y++;
    }
  }

  *p0++ = t0;
  *p1++ = t1;
  *p2++ = t2;
  *p3++ = t3;
  *p4++ = t4;
  *p5++ = t5;
}

void argb2aga_gray4(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3;
  unsigned long *sptr, *dst, d[8], *argb, val, gray;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3;
  unsigned long temp1, temp2;
  unsigned long row_add, add;
  int i, pixels, xloop, yloop, skip, y;
  int cr = CR, cg = CG, cb = CB, fr = FR;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  dst = rect->dst;
  dst += rect->aga_dst_offset;

  p0 = dst;
  p1 = p0 + rect->aga_rasize;
  p2 = p1 + rect->aga_rasize;
  p3 = p2 + rect->aga_rasize;

  skip = -1;
  y = 0;

  t0 = 0;
  t1 = 0;
  t2 = 0;
  t3 = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[y]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l << 3;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r << 3;
      } else {
        pixels = 32;
      }

      sptr = &d[0];
      for (i=0; i<pixels; i+=4) {
        val = *argb++;
        gray = RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 24;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 16;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff)) << 8;
        val = *argb++;
        gray |= RGB2GRAY(((val>>16)&0xff), ((val>>8)&0xff), (val&0xff));
        *sptr++ = gray;
      }
      sptr = &d[0];

      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        d0 = 0;
        d1 = 0;
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;

        switch(rect->aga_left_over_l) {
          case 3:
            d2 = *sptr++;
            d3 = *sptr++;
          case 2:
            d4 = *sptr++;
            d5 = *sptr++;
        }

        d6 = *sptr++;
        d7 = *sptr++;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        d2 = 0;
        d3 = 0;
        d4 = 0;
        d5 = 0;
        d6 = 0;
        d7 = 0;

        switch(rect->aga_left_over_r) {
          case 3:
            d5 = sptr[5];
            d4 = sptr[4];
          case 2:
            d3 = sptr[3];
            d2 = sptr[2];
        }

        d1 = sptr[1];
        d0 = sptr[0];
        src += rect->aga_left_over_r << 1;
      } else {
        d0 = *sptr++;
        d1 = *sptr++;
        d2 = *sptr++;
        d3 = *sptr++;
        d4 = *sptr++;
        d5 = *sptr++;
        d6 = *sptr++;
        d7 = *sptr++;
      }

      SPECIALMERGE(d0,d1);
      SPECIALMERGE(d2,d3);
      SPECIALMERGE(d4,d5);
      SPECIALMERGE(d6,d7);          

      TWOMERGE(d0,d4,d2,d6,16,0x0000ffff);
      *p0++ = t0;
      TWOMERGE(d0,d4,d2,d6,2,0x33333333);
      *p1++ = t1;
      TWOMERGE(d0,d2,d4,d6,8,0x00ff00ff);
      *p2++ = t2;
      TWOMERGE(d0,d2,d4,d6,1,0x55555555);
      *p3++ = t3;

      t0 = d6;
      t1 = d4;
      t2 = d2;
      t3 = d0;

      if (skip) {
        p0 += skip;
        p1 += skip;
        p2 += skip;
        p3 += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    if (rect->aga_half_height) {
      add += row_add;
      add += row_add;
      y += 2;
    } else {
      add += row_add;
      y++;
    }
  }

  *p0++ = t0;
  *p1++ = t1;
  *p2++ = t2;
  *p3++ = t3;
}
