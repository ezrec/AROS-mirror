/*
 *
 * lut2aga.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void lut2aga(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *sptr, *dst;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3, t4, t5, t6, t7;
  unsigned long temp1, temp2;
  unsigned long row, add;
  int xloop, yloop, skip, y;

  row = rect->src_row_length;
  add = rect->aga_src_offset;

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
      sptr = ((unsigned long *)rect->osd[0]) + add;
    } else {
      sptr = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
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
      add += row;
      add += row;
      y += 2;
    } else {
      add += row;
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

void lut2aga_gray8(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *sptr, *dst;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3, t4, t5, t6, t7;
  unsigned long temp1, temp2;
  unsigned long row, add;
  int xloop, yloop, skip, y;

  row = rect->src_row_length;
  add = rect->aga_src_offset;

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
      sptr = ((unsigned long *)rect->osd[0]) + add;
    } else {
      sptr = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
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
      add += row;
      add += row;
      y += 2;
    } else {
      add += row;
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

void lut2aga_gray6(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3, *p4, *p5, *sptr, *dst, *lut;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3, t4, t5;
  unsigned long temp1, temp2;
  unsigned long row, add, col, d[8], val;
  unsigned char *gray;
  int i, pixels, xloop, yloop, skip, y;

  row = rect->src_row_length;
  add = rect->aga_src_offset;

  dst = rect->dst;
  dst += rect->aga_dst_offset;

  gray = rect->gray;

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
      lut = ((unsigned long *)rect->osd[0]) + add;
    } else {
      lut = ((unsigned long *)src[0]) + add;
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
        col = *lut++;
        val = (gray[(col >> 24) & 0xff] << 24);
        val |= (gray[(col >> 16) & 0xff] << 16);
        val |= (gray[(col >> 8) & 0xff] << 8);
        val |= gray[col & 0xff];
        *sptr++ = val;
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
      add += row;
      add += row;
      y += 2;
    } else {
      add += row;
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

void lut2aga_gray4(rect_type *rect, unsigned char *src[])
{
  unsigned long *p0, *p1, *p2, *p3, *sptr, *dst, *lut;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7;
  unsigned long t0, t1, t2, t3;
  unsigned long temp1, temp2;
  unsigned long row, add, col, d[8], val;
  unsigned char *gray;
  int i, pixels, xloop, yloop, skip, y;

  row = rect->src_row_length;
  add = rect->aga_src_offset;

  dst = rect->dst;
  dst += rect->aga_dst_offset;

  gray = rect->gray;

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
      lut = ((unsigned long *)rect->osd[0]) + add;
    } else {
      lut = ((unsigned long *)src[0]) + add;
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
        col = *lut++;
        val = (gray[(col >> 24) & 0xff] << 24);
        val |= (gray[(col >> 16) & 0xff] << 16);
        val |= (gray[(col >> 8) & 0xff] << 8);
        val |= gray[col & 0xff];
        *sptr++ = val;
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
      add += row;
      add += row;
      y += 2;
    } else {
      add += row;
      y++;
    }
  }

  *p0++ = t0;
  *p1++ = t1;
  *p2++ = t2;
  *p3++ = t3;
}
