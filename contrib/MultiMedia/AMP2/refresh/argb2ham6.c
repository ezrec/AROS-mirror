/*
 *
 * argb2ham6.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void argb2ham6_1(rect_type *rect, unsigned char *src[])
{
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[4], t[4], d[8];
  unsigned long *dst, val, *argb;
  unsigned long temp1, temp2;
  int i, j, add, row_add, xloop, yloop, skip, pixels;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  if (rect->aga_half_height) {
    row_add += rect->src_row_length << 2;
  }

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;

  skip = -1;
  j = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  argb = (unsigned long *)src[0];
  argb += rect->src_offset << 2;

  while (yloop--) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r;
      } else {
        pixels = 4;
      }

      dst = &d[0];
      for (i=0; i<pixels; i++) {
        /* ARGB -> BGRG */
        val = ((argb[0] & 0x000000ff) << 24) | ((argb[1] & 0x0000ff00) << 8) |
              ((argb[2] & 0x00ff0000) >>  8) | ((argb[4] & 0x0000ff00) >> 8);
        *dst++ = val;
        argb += 4;

        val = ((argb[0] & 0x000000ff) << 24) | ((argb[1] & 0x0000ff00) << 8) |
              ((argb[2] & 0x00ff0000) >>  8) | ((argb[4] & 0x0000ff00) >> 8);
        *dst++ = val;
        argb += 4;

        *p[i]++ = t[i];
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

        for (i=pixels; i<4; i++) {
          *p[i]++ = t[i];
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

        for (i=pixels; i<4; i++) {
          *p[i]++ = t[i];
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

      SPECIALMERGE(d0,d1);
      SPECIALMERGE(d2,d3);
      SPECIALMERGE(d4,d5);
      SPECIALMERGE(d6,d7);          

      TWOMERGE(d0,d4,d2,d6,16,0x0000ffff);
      TWOMERGE(d0,d4,d2,d6,2,0x33333333);
      TWOMERGE(d0,d2,d4,d6,8,0x00ff00ff);
      TWOMERGE(d0,d2,d4,d6,1,0x55555555);

      t[0] = d6;
      t[1] = d4;
      t[2] = d2;
      t[3] = d0;

      if (skip) {
        p[0] += skip;
        p[1] += skip;
        p[2] += skip;
        p[3] += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    add += row_add;
    if (rect->aga_half_height) {
      j += 2;
    } else {
      j++;
    }
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
}

void argb2ham6_2(rect_type *rect, unsigned char *src[])
{
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[4], t[4], d[8];
  unsigned long *dst, val, *argb;
  unsigned long temp1, temp2;
  int i, j, add, row_add, xloop, yloop, skip, pixels;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  if (rect->aga_half_height) {
    row_add += rect->src_row_length << 2;
  }

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;

  skip = -1;
  j = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  argb = (unsigned long *)src[0];
  argb += rect->src_offset << 2;

  while (yloop--) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r;
      } else {
        pixels = 4;
      }

      dst = &d[0];
      for (i=0; i<pixels; i++) {
        /* ARGB -> BGRG */
        val = ((argb[0] & 0x000000ff) << 24) | ((argb[0] & 0x0000ff00) << 8) |
              ((argb[1] & 0x00ff0000) >>  8) | ((argb[1] & 0x0000ff00) >> 8);
        *dst++ = val;
        argb += 2;

        val = ((argb[0] & 0x000000ff) << 24) | ((argb[0] & 0x0000ff00) << 8) |
              ((argb[1] & 0x00ff0000) >>  8) | ((argb[1] & 0x0000ff00) >> 8);
        *dst++ = val;
        argb += 2;

        *p[i]++ = t[i];
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

        for (i=pixels; i<4; i++) {
          *p[i]++ = t[i];
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

        for (i=pixels; i<4; i++) {
          *p[i]++ = t[i];
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

      SPECIALMERGE(d0,d1);
      SPECIALMERGE(d2,d3);
      SPECIALMERGE(d4,d5);
      SPECIALMERGE(d6,d7);          

      TWOMERGE(d0,d4,d2,d6,16,0x0000ffff);
      TWOMERGE(d0,d4,d2,d6,2,0x33333333);
      TWOMERGE(d0,d2,d4,d6,8,0x00ff00ff);
      TWOMERGE(d0,d2,d4,d6,1,0x55555555);

      t[0] = d6;
      t[1] = d4;
      t[2] = d2;
      t[3] = d0;

      if (skip) {
        p[0] += skip;
        p[1] += skip;
        p[2] += skip;
        p[3] += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    add += row_add;
    if (rect->aga_half_height) {
      j += 2;
    } else {
      j++;
    }
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
}

void argb2ham6_4(rect_type *rect, unsigned char *src[])
{
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[4], t[4], d[8];
  unsigned long *dst, val, *argb;
  unsigned long temp1, temp2;
  int i, j, add, row_add, xloop, yloop, skip, pixels;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  if (rect->aga_half_height) {
    row_add += rect->src_row_length << 2;
  }

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;

  skip = -1;
  j = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  argb = (unsigned long *)src[0];
  argb += rect->src_offset << 2;

  while (yloop--) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r;
      } else {
        pixels = 4;
      }

      dst = &d[0];
      for (i=0; i<pixels; i++) {
        /* ARGB -> RGBB */
        val = (argb[0] << 8) | (argb[0] & 0x000000ff);
        *dst++ = val;
        argb++;

        val = (argb[0] << 8) | (argb[0] & 0x000000ff);
        *dst++ = val;
        argb++;

        *p[i]++ = t[i];
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

        for (i=pixels; i<4; i++) {
          *p[i]++ = t[i];
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

        for (i=pixels; i<4; i++) {
          *p[i]++ = t[i];
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

      SPECIALMERGE(d0,d1);
      SPECIALMERGE(d2,d3);
      SPECIALMERGE(d4,d5);
      SPECIALMERGE(d6,d7);          

      TWOMERGE(d0,d4,d2,d6,16,0x0000ffff);
      TWOMERGE(d0,d4,d2,d6,2,0x33333333);
      TWOMERGE(d0,d2,d4,d6,8,0x00ff00ff);
      TWOMERGE(d0,d2,d4,d6,1,0x55555555);

      t[0] = d6;
      t[1] = d4;
      t[2] = d2;
      t[3] = d0;

      if (skip) {
        p[0] += skip;
        p[1] += skip;
        p[2] += skip;
        p[3] += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    add += row_add;
    if (rect->aga_half_height) {
      j += 2;
    } else {
      j++;
    }
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
}

void argb2ham6_1hq(rect_type *rect, unsigned char *src[])
{
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[6], t[6], d[8];
  unsigned long *dst, val, *argb;
  unsigned long temp1, temp2;
  int i, j, add, row_add, xloop, yloop, skip, pixels;
  unsigned char r, g, b, r_old, g_old, b_old, palcolor;
  unsigned long paldist, hamdist;
  int rdiff, gdiff, bdiff;

  row_add = rect->src_row_length << 2;
  add = rect->src_offset << 2;

  if (rect->aga_half_height) {
    row_add += rect->src_row_length << 2;
  }

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;
  p[4] = p[3] + rect->aga_rasize;
  p[5] = p[4] + rect->aga_rasize;

  skip = -1;
  j = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;
  t[4] = 0;
  t[5] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  argb = (unsigned long *)src[0];
  argb += rect->src_offset << 2;

  while (yloop--) {
    if (rect->use_osd[j]) {
      argb = ((unsigned long *)rect->osd[0]) + add;
    } else {
      argb = ((unsigned long *)src[0]) + add;
    }

    r_old = 0;
    g_old = 0;
    b_old = 0;
    while (xloop--) {
      if ((rect->aga_left_over_l) && (xloop == (rect->aga_xloop - 1))) {
        pixels = rect->aga_left_over_l << 1;
      } else if ((rect->aga_left_over_r) && (xloop == 0)) {
        pixels = rect->aga_left_over_r << 1;
      } else {
        pixels = 8;
      }

      dst = &d[0];
      for (i=0; i<pixels; i++) {
        r = (argb[0] >> 16) & 0xff;
        g = (argb[0] >> 8) & 0xff;
        b = (argb[0]) & 0xff;
        argb++;

        rdiff = r-r_old;
        gdiff = g-g_old;

        /* Calculate color distances */
        paldist = (r&0x7f)*(r&0x7f) + (g&0x3f)*(g&0x3f) + (b&0x7f)*(b&0x7f);

        /* HAM color distance, B */
        hamdist = (rdiff*rdiff) + (gdiff*gdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0x80)>>2) | ((g&0xc0)>>3) | ((b&0x80)>>5);

          val = (palcolor<<24);

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          val = (((b>>2) | 0x40) << 24);
          b_old = b;
        }

        r = (argb[0] >> 16) & 0xff;
        g = (argb[0] >> 8) & 0xff;
        b = (argb[0]) & 0xff;
        argb++;

        rdiff = r-r_old;
        bdiff = b-b_old;

        /* Calculate color distances */
        paldist = (r&0x7f)*(r&0x7f) + (g&0x3f)*(g&0x3f) + (b&0x7f)*(b&0x7f);

        /* HAM color distance, G */
        hamdist = (rdiff*rdiff) + (bdiff*bdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0x80)>>2) | ((g&0xc0)>>3) | ((b&0x80)>>5);

          val |= (palcolor<<16);

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          val |= (((g>>2) | 0xc0) << 16);
          g_old = g;
        }

        r = (argb[0] >> 16) & 0xff;
        g = (argb[0] >> 8) & 0xff;
        b = (argb[0]) & 0xff;
        argb++;

        gdiff = g-g_old;
        bdiff = b-b_old;

        /* Calculate color distances */
        paldist = (r&0x7f)*(r&0x7f) + (g&0x3f)*(g&0x3f) + (b&0x7f)*(b&0x7f);

        /* HAM color distance, R */
        hamdist = (gdiff*gdiff) + (bdiff*bdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0x80)>>2) | ((g&0xc0)>>3) | ((b&0x80)>>5);

          val |= (palcolor<<8);

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          val |= (((r>>2) | 0x80) << 8);
          r_old = r;
        }

        r = (argb[0] >> 16) & 0xff;
        g = (argb[0] >> 8) & 0xff;
        b = (argb[0]) & 0xff;
        argb++;

        rdiff = r-r_old;
        bdiff = b-b_old;

        /* Calculate color distances */
        paldist = (r&0x7f)*(r&0x7f) + (g&0x3f)*(g&0x3f) + (b&0x7f)*(b&0x7f);

        /* HAM color distance, G */
        hamdist = (rdiff*rdiff) + (bdiff*bdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0x80)>>2) | ((g&0xc0)>>3) | ((b&0x80)>>5);

          *dst++ = val | palcolor;

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          *dst++ = val | ((g>>2) | 0xc0);
          g_old = g;
        }

        if (i<6) {
          *p[i]++ = t[i];
        }
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

        for (i=pixels; i<6; i++) {
          *p[i]++ = t[i];
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

        for (i=pixels; i<6; i++) {
          *p[i]++ = t[i];
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

      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);

      t[0] = d3;
      t[1] = d1;
      t[2] = d6;
      t[3] = d4;
      t[4] = d2;
      t[5] = d0;

      if (skip) {
        p[0] += skip;
        p[1] += skip;
        p[2] += skip;
        p[3] += skip;
        p[4] += skip;
        p[5] += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    add += row_add;
    if (rect->aga_half_height) {
      j += 2;
    } else {
      j++;
    }
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
  *p[4]++ = t[4];
  *p[5]++ = t[5];
}
