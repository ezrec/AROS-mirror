/*
 *
 * yuv2ham8.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void yuv2ham8_1(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[6], t[6], d[8];
  unsigned long *dst, *py, *pu, *pv, val;
  unsigned long temp1, temp2;
  int i, j, xloop, yloop, skip, line, pixels;
  int y, u, v, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;
  p[4] = p[3] + rect->aga_rasize;
  p[5] = p[4] + rect->aga_rasize;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  line = 0;
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

  while (yloop--) {
    if (rect->use_osd[j]) {
      py = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
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
        y = *py++;
        tu = *pu++ ^ cuv;
        tv = *pv++ ^ cuv;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        val = rgb_clip[((cym * ((y >> 24) & 0xff) + cya) + cbu*u)>>FIXBITS] << 24;
        val |= rgb_clip[((cym * (((y >> 16) & 0xff)) + cya) + cgu*u + cgv*v)>>FIXBITS] << 16;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        val |= rgb_clip[((cym * ((y >> 8) & 0xff) + cya) + crv*v)>>FIXBITS] << 8;
        val |= rgb_clip[((cym * (y & 0xff) + cya) + cgu*u + cgv*v)>>FIXBITS];
        *dst++ = val;
        y = *py++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        val = rgb_clip[((cym * ((y >> 24) & 0xff) + cya) + cbu*u)>>FIXBITS] << 24;
        val |= rgb_clip[((cym * ((y >> 16) & 0xff) + cya) + cgu*u + cgv*v)>>FIXBITS] << 16;
        u = tu >> 24;
        v = tv >> 24;
        val |= rgb_clip[((cym * ((y >> 8) & 0xff) + cya) + crv*v)>>FIXBITS] << 8;
        val |= rgb_clip[((cym * (y & 0xff) + cya) + cgu*u + cgv*v)>>FIXBITS];
        *dst++ = val;
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

      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      *p[4]++ = t[4];
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);
      *p[5]++ = t[5];

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

    if (rect->aga_half_height) {
      yadd += yrow;
      yadd += yrow;
      uvadd += uvrow;
      j += 2;
    } else {
      yadd += yrow;
      uvadd += (uvrow * (j & 1));
      j++;
    }

    line++;
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
  *p[4]++ = t[4];
  *p[5]++ = t[5];
}

void yuv2ham8_2(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[6], t[6], d[8];
  unsigned long *dst, *py, *pu, *pv, val;
  unsigned long temp1, temp2;
  int i, j, xloop, yloop, skip, line, pixels, xpos;
  int y, u, v, ty, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;
  p[4] = p[3] + rect->aga_rasize;
  p[5] = p[4] + rect->aga_rasize;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  line = 0;
  skip = -1;
  j = 0;

  tu = 0;
  tv = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;
  t[4] = 0;
  t[5] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[j]) {
      py = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    xpos = 1;
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
        ty = *py++;

        if (xpos & 1) {
          tu = *pu++ ^ cuv;
          tv = *pv++ ^ cuv;
        }

        y = cym * ((ty >> 24) & 0xff) + cya;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        val = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        val |= rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        y = cym * ((ty >> 16) & 0xff) + cya;
        val |= rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        val |= rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        *dst++ = val;

        y = cym * ((ty >> 8) & 0xff) + cya;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        val = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        val |= rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        y = cym * (ty & 0xff) + cya;
        val |= rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        val |= rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        *dst++ = val;

        xpos++;

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

      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      *p[4]++ = t[4];
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);
      *p[5]++ = t[5];

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

    if (rect->aga_half_height) {
      yadd += yrow;
      yadd += yrow;
      uvadd += uvrow;
      j += 2;
    } else {
      yadd += yrow;
      uvadd += (uvrow * (j & 1));
      j++;
    }

    line++;
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
  *p[4]++ = t[4];
  *p[5]++ = t[5];
}

void yuv2ham8_4(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[6], t[6], d[8];
  unsigned long *dst, *py, *pu, *pv, val;
  unsigned long temp1, temp2;
  int i, xloop, yloop, skip, line, pixels, xpos;
  int y, u, v, ty, tu, tv, crv, cbu, cgu, cgv, b, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;
  p[4] = p[3] + rect->aga_rasize;
  p[5] = p[4] + rect->aga_rasize;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  line = 0;
  skip = -1;

  ty = 0;
  tu = 0;
  tv = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;
  t[4] = 0;
  t[5] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[line]) {
      py = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    xpos = 0;
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
        if (xpos == 0) {
          ty = *py++;
          tu = *pu++ ^ cuv;
          tv = *pv++ ^ cuv;
        }

        if (xpos == 2) {
          ty = *py++;
        }

        y = cym * ((ty >> 24) & 0xff) + cya;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        val = rgb_clip[(y + crv*v)>>FIXBITS] << 24;
        val |= rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        val |= (b<<8) | b;
        *dst++ = val;
        y = cym * ((ty >> 16) & 0xff) + cya;
        val = rgb_clip[(y + crv*v)>>FIXBITS] << 24;
        val |= rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        val |= (b<<8) | b;
        *dst++ = val;

        ty <<= 16;

        xpos++;
        xpos &= 3;

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

      TWOMERGE(d0, d4, d1, d5, 16, 0x0000ffff);
      TWOMERGE(d2, d6, d3, d7, 16, 0x0000ffff);
      TWOMERGE(d0, d2, d1, d3, 8, 0x00ff00ff);
      TWOMERGE(d4, d6, d5, d7, 8, 0x00ff00ff);
      TWOMERGE(d0, d1, d2, d3, 4, 0x0f0f0f0f);
      *p[4]++ = t[4];
      TWOMERGE(d4, d5, d6, d7, 4, 0x0f0f0f0f);
      TWOMERGE(d0, d4, d1, d5, 2, 0x33333333);
      TWOMERGE(d2, d6, d3, d7, 2, 0x33333333);
      TWOMERGE(d0, d2, d1, d3, 1, 0x55555555);
      TWOMERGE(d4, d6, d5, d7, 1, 0x55555555);
      *p[5]++ = t[5];

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

    yadd += yrow;
    uvadd += (uvrow * (line & 1));

    line++;
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
  *p[4]++ = t[4];
  *p[5]++ = t[5];
}

void yuv2ham8_1hq(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long d0, d1, d2, d3, d4, d5, d6, d7, *p[8], t[8], d[8];
  unsigned long *dst, *py, *pu, *pv, val;
  unsigned long temp1, temp2;
  int i, j, xloop, yloop, skip, line, pixels, xpos;
  int y, u, v, ty, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;
  unsigned char r, g, b, r_old, g_old, b_old, palcolor;
  unsigned long paldist, hamdist;
  int rdiff, gdiff, bdiff;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  p[0] = rect->dst + rect->aga_dst_offset;
  p[1] = p[0] + rect->aga_rasize;
  p[2] = p[1] + rect->aga_rasize;
  p[3] = p[2] + rect->aga_rasize;
  p[4] = p[3] + rect->aga_rasize;
  p[5] = p[4] + rect->aga_rasize;
  p[6] = p[5] + rect->aga_rasize;
  p[7] = p[6] + rect->aga_rasize;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  line = 0;
  skip = -1;
  j = 0;

  ty = 0;
  tu = 0;
  tv = 0;

  t[0] = 0;
  t[1] = 0;
  t[2] = 0;
  t[3] = 0;
  t[4] = 0;
  t[5] = 0;
  t[6] = 0;
  t[7] = 0;

  xloop = rect->aga_xloop;
  yloop = rect->aga_yloop;

  while (yloop--) {
    if (rect->use_osd[j]) {
      py = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    xpos = 1;
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
        ty = *py++;
        if (xpos & 1) {
          tu = *pu++ ^ cuv;
          tv = *pv++ ^ cuv;
        }

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        y = cym * ((ty>>24) & 0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS];
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        b = rgb_clip[(y + cbu*u)>>FIXBITS];

        rdiff = r-r_old;
        gdiff = g-g_old;

        /* Calculate color distances */
        paldist = (r&0x3f)*(r&0x3f) + (g&0x3f)*(g&0x3f) + (b&0x3f)*(b&0x3f);

        /* HAM color distance, B */
        hamdist = (rdiff*rdiff) + (gdiff*gdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0xc0)>>2) | ((g&0xc0)>>4) | ((b&0xc0)>>6);

          val = (palcolor<<24);

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          val = (((b>>2) | 0x40) << 24);
          b_old = b;
        }

        y = cym * ((ty>>16) & 0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS];
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        b = rgb_clip[(y + cbu*u)>>FIXBITS];

        rdiff = r-r_old;
        bdiff = b-b_old;

        /* Calculate color distances */
        paldist = (r&0x3f)*(r&0x3f) + (g&0x3f)*(g&0x3f) + (b&0x3f)*(b&0x3f);

        /* HAM color distance, G */
        hamdist = (rdiff*rdiff) + (bdiff*bdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0xc0)>>2) | ((g&0xc0)>>4) | ((b&0xc0)>>6);

          val |= (palcolor<<16);

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          val |= (((g>>2) | 0xc0) << 16);
          g_old = g;
        }

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        y = cym * ((ty>>8) & 0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS];
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        b = rgb_clip[(y + cbu*u)>>FIXBITS];

        gdiff = g-g_old;
        bdiff = b-b_old;

        /* Calculate color distances */
        paldist = (r&0x3f)*(r&0x3f) + (g&0x3f)*(g&0x3f) + (b&0x3f)*(b&0x3f);

        /* HAM color distance, R */
        hamdist = (gdiff*gdiff) + (bdiff*bdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0xc0)>>2) | ((g&0xc0)>>4) | ((b&0xc0)>>6);

          val |= (palcolor<<8);

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          val |= (((r>>2) | 0x80) << 8);
          r_old = r;
        }

        y = cym * (ty&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS];
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        b = rgb_clip[(y + cbu*u)>>FIXBITS];

        rdiff = r-r_old;
        bdiff = b-b_old;

        /* Calculate color distances */
        paldist = (r&0x3f)*(r&0x3f) + (g&0x3f)*(g&0x3f) + (b&0x3f)*(b&0x3f);

        /* HAM color distance, G */
        hamdist = (rdiff*rdiff) + (bdiff*bdiff);

        if (paldist < hamdist) {
          /* Use color from the palette */
          palcolor = ((r&0xc0)>>2) | ((g&0xc0)>>4) | ((b&0xc0)>>6);

          *dst++ = val | palcolor;

          r_old = r&0xc0;
          g_old = g&0xc0;
          b_old = b&0xc0;
        } else {
          /* Use HAM finetuning */
          *dst++ = val | ((g>>2) | 0xc0);
          g_old = g;
        }

        xpos++;

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

        for (i=pixels; i<8; i++) {
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

        for (i=pixels; i<8; i++) {
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

      t[0] = d7;
      t[1] = d5;
      t[2] = d3;
      t[3] = d1;
      t[4] = d6;
      t[5] = d4;
      t[6] = d2;
      t[7] = d0;

      if (skip) {
        p[0] += skip;
        p[1] += skip;
        p[2] += skip;
        p[3] += skip;
        p[4] += skip;
        p[5] += skip;
        p[6] += skip;
        p[7] += skip;
        skip = 0;
      }
    }

    xloop = rect->aga_xloop;
    skip = rect->aga_dst_col_skip;

    if (rect->aga_half_height) {
      yadd += yrow;
      yadd += yrow;
      uvadd += uvrow;
      j += 2;
    } else {
      yadd += yrow;
      uvadd += (uvrow * (j & 1));
      j++;
    }

    line++;
  }

  *p[0]++ = t[0];
  *p[1]++ = t[1];
  *p[2]++ = t[2];
  *p[3]++ = t[3];
  *p[4]++ = t[4];
  *p[5]++ = t[5];
  *p[6]++ = t[6];
  *p[7]++ = t[7];
}
