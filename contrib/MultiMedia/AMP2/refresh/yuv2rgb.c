/*
 *
 * yuv2rgb.c
 *
 */

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "refresh_internal.h"

void yuv2argb32(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst, *py, *pu, *pv;
  int i, j, k, r, g, b;
  int y, u, v, ty, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst = rect->dst + rect->dst_offset;

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
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        ty = *py++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        y = cym * ((ty>>24)&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS] << 16;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 8;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        *dst++ = r | g | b;

        y = cym * ((ty>>16)&0xff) + cya;
        r = rgb_clip[(y + crv*v)>>FIXBITS] << 16;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 8;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        *dst++ = r | g | b;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        y = cym * ((ty>>8)&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS] << 16;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 8;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        *dst++ = r | g | b;

        y = cym * (ty&0xff) + cya;
        r = rgb_clip[(y + crv*v)>>FIXBITS] << 16;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 8;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        *dst++ = r | g | b;
      }
    }

    yadd += yrow;
    uvadd += (uvrow * (j & 1));
    dst += rect->dst_row_length - rect->xloop;
  }
}

void yuv2bgr24(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst, *py, *pu, *pv, val;
  int i, j, k, r, g, b;
  int y, u, v, ty, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst = rect->dst + rect->dst_offset;

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
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        ty = *py++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        y = cym * ((ty>>24)&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        val = r | g | b;

        y = cym * ((ty>>16)&0xff) + cya;
        r = rgb_clip[(y + crv*v)>>FIXBITS] << 16;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 24;
        b = rgb_clip[(y + cbu*u)>>FIXBITS];
        *dst++ = val | b;
        val = r | g;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        y = cym * ((ty>>8)&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS] << 24;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS];
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 8;
        *dst++ = val | g | b;
        val = r;

        y = cym * (ty&0xff) + cya;
        r = rgb_clip[(y + crv*v)>>FIXBITS];
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 8;
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 16;
        *dst++ = val | r | g | b;
      }
    }

    yadd += yrow;
    uvadd += (uvrow * (j & 1));
    dst += rect->dst_row_length - ((rect->xloop * 3) >> 2);
  }
}

#ifdef __PPC__

  /* This is what we have to do to get the bits in the right position.
   *
   *    16bit         15bit
   *
   * MB-ME -> SH : MB-ME -> SH
   * ----------- : -----------
   *  0- 4 -> 24 :  1- 5 -> 23
   *  5-10 -> 19 :  6-10 -> 18
   * 11-15 -> 13 : 11-15 -> 13
   * 16-20 ->  8 : 17-21 ->  7
   * 21-26 ->  3 : 22-26 ->  2
   * 27-31 -> 29 : 27-31 -> 29
   *
   */

  // 16
  #define YUV2RGB16_HI \
    r = rgb_clip[(y + cr)>>FIXBITS]; \
    g = rgb_clip[(y + cg)>>FIXBITS]; \
    b = rgb_clip[(y + cb)>>FIXBITS]; \
    RLWIMI(val, r, 24,  0,  4); \
    RLWIMI(val, g, 19,  5, 10); \
    RLWIMI(val, b, 13, 11, 15);

  #define YUV2RGB16_LO \
    r = rgb_clip[(y + cr)>>FIXBITS]; \
    g = rgb_clip[(y + cg)>>FIXBITS]; \
    b = rgb_clip[(y + cb)>>FIXBITS]; \
    RLWIMI(val, r,  8, 16, 20); \
    RLWIMI(val, g,  3, 21, 26); \
    RLWIMI(val, b, 29, 27, 31);

  // 15
  #define YUV2RGB15_HI \
    r = rgb_clip[(y + cr)>>FIXBITS]; \
    g = rgb_clip[(y + cg)>>FIXBITS]; \
    b = rgb_clip[(y + cb)>>FIXBITS]; \
    RLWIMI(val, r, 23,  1,  5); \
    RLWIMI(val, g, 18,  6, 10); \
    RLWIMI(val, b, 13, 11, 15);

  #define YUV2RGB15_LO \
    r = rgb_clip[(y + cr)>>FIXBITS]; \
    g = rgb_clip[(y + cg)>>FIXBITS]; \
    b = rgb_clip[(y + cb)>>FIXBITS]; \
    RLWIMI(val, r,  7, 17, 21); \
    RLWIMI(val, g,  2, 22, 26); \
    RLWIMI(val, b, 29, 27, 31);

#else

  // 16
  #define YUV2RGB16_HI \
    r = (rgb_clip[(y + cr)>>FIXBITS] & 0xf8) << 24; \
    g = (rgb_clip[(y + cg)>>FIXBITS] & 0xfc) << 19; \
    b = (rgb_clip[(y + cb)>>FIXBITS] & 0xf8) << 13; \
    val = r | g | b;

  #define YUV2RGB16_LO \
    r = (rgb_clip[(y + cr)>>FIXBITS] & 0xf8) << 8; \
    g = (rgb_clip[(y + cg)>>FIXBITS] & 0xfc) << 3; \
    b = (rgb_clip[(y + cb)>>FIXBITS] >> 3); \
    val |= r | g | b;

  // 15
  #define YUV2RGB15_HI \
    r = (rgb_clip[(y + cr)>>FIXBITS] & 0xf8) << 23; \
    g = (rgb_clip[(y + cg)>>FIXBITS] & 0xf8) << 18; \
    b = (rgb_clip[(y + cb)>>FIXBITS] & 0xf8) << 13; \
    val = r | g | b;

  #define YUV2RGB15_LO \
    r = (rgb_clip[(y + cr)>>FIXBITS] & 0xf8) << 7; \
    g = (rgb_clip[(y + cg)>>FIXBITS] & 0xf8) << 2; \
    b = (rgb_clip[(y + cb)>>FIXBITS] >> 3); \
    val |= r | g | b;

#endif

void yuv2rgb16(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst1, *dst2, *py1, *py2, *pu, *pv, val;
  int i, j, k, r, g, b, cr, cg, cb;
  int y, u, v, ty1, ty2, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst1 = rect->dst + rect->dst_offset;

  val = 0; /* To make GCC happy */

  for (j=0; j<rect->yloop; j+=2) {
    if (rect->use_osd[j]) {
      py1 = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py1 = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    py2 = py1 + yrow;
    dst2 = dst1 + rect->dst_row_length;

    for (i=0; i<(rect->xloop >> 3); i++) {
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        ty1 = *py1++;
        ty2 = *py2++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>24)&0xff) + cya;
        YUV2RGB16_HI;

        y = cym * ((ty1>>16)&0xff) + cya;
        YUV2RGB16_LO;
        *dst1++ = val;

        // 2
        y = cym * ((ty2>>24)&0xff) + cya;
        YUV2RGB16_HI;

        y = cym * ((ty2>>16)&0xff) + cya;
        YUV2RGB16_LO;
        *dst2++ = val;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>8)&0xff) + cya;
        YUV2RGB16_HI;

        y = cym * (ty1&0xff) + cya;
        YUV2RGB16_LO;
        *dst1++ = val;

        // 2
        y = cym * ((ty2>>8)&0xff) + cya;
        YUV2RGB16_HI;

        y = cym * (ty2&0xff) + cya;
        YUV2RGB16_LO;
        *dst2++ = val;
      }
    }

    yadd += yrow;
    yadd += yrow;
    uvadd += uvrow;

    dst1 += rect->dst_row_length - (rect->xloop >> 1);
    dst1 += rect->dst_row_length;
  }
}

void yuv2rgb15(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst1, *dst2, *py1, *py2, *pu, *pv, val;
  int i, j, k, r, g, b, cr, cg, cb;
  int y, u, v, ty1, ty2, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst1 = rect->dst + rect->dst_offset;

  val = 0; /* To make GCC happy */

  for (j=0; j<rect->yloop; j+=2) {
    if (rect->use_osd[j]) {
      py1 = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py1 = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    py2 = py1 + yrow;
    dst2 = dst1 + rect->dst_row_length;

    for (i=0; i<(rect->xloop >> 3); i++) {
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        ty1 = *py1++;
        ty2 = *py2++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>24)&0xff) + cya;
        YUV2RGB15_HI;

        y = cym * ((ty1>>16)&0xff) + cya;
        YUV2RGB15_LO;
        *dst1++ = val;

        // 2
        y = cym * ((ty2>>24)&0xff) + cya;
        YUV2RGB15_HI;

        y = cym * ((ty2>>16)&0xff) + cya;
        YUV2RGB15_LO;
        *dst2++ = val;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>8)&0xff) + cya;
        YUV2RGB15_HI;

        y = cym * (ty1&0xff) + cya;
        YUV2RGB15_LO;
        *dst1++ = val;

        // 2
        y = cym * ((ty2>>8)&0xff) + cya;
        YUV2RGB15_HI;

        y = cym * (ty2&0xff) + cya;
        YUV2RGB15_LO;
        *dst2++ = val;
      }
    }

    yadd += yrow;
    yadd += yrow;
    uvadd += uvrow;

    dst1 += rect->dst_row_length - (rect->xloop >> 1);
    dst1 += rect->dst_row_length;
  }
}

/* First change ABCD to CDAB then write byteswapped to get BADC */

void yuv2rgb16pc(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst1, *dst2, *py1, *py2, *pu, *pv, val;
  int i, j, k, r, g, b, cr, cg, cb;
  int y, u, v, ty1, ty2, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst1 = rect->dst + rect->dst_offset;

  val = 0; /* To make GCC happy */

  for (j=0; j<rect->yloop; j+=2) {
    if (rect->use_osd[j]) {
      py1 = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py1 = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    py2 = py1 + yrow;
    dst2 = dst1 + rect->dst_row_length;

    for (i=0; i<(rect->xloop >> 3); i++) {
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        val = 0; /* GCC suxx */
        ty1 = *py1++;
        ty2 = *py2++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>24)&0xff) + cya;
        YUV2RGB16_LO;

        y = cym * ((ty1>>16)&0xff) + cya;
        YUV2RGB16_HI;
        SWAP32_W(dst1, val);
        dst1++;

        // 2
        y = cym * ((ty2>>24)&0xff) + cya;
        YUV2RGB16_LO;

        y = cym * ((ty2>>16)&0xff) + cya;
        YUV2RGB16_HI;
        SWAP32_W(dst2, val);
        dst2++;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>8)&0xff) + cya;
        YUV2RGB16_LO;

        y = cym * (ty1&0xff) + cya;
        YUV2RGB16_HI;
        SWAP32_W(dst1, val);
        dst1++;

        // 2
        y = cym * ((ty2>>8)&0xff) + cya;
        YUV2RGB16_LO;

        y = cym * (ty2&0xff) + cya;
        YUV2RGB16_HI;
        SWAP32_W(dst2, val);
        dst2++;
      }
    }

    yadd += yrow;
    yadd += yrow;
    uvadd += uvrow;

    dst1 += rect->dst_row_length - (rect->xloop >> 1);
    dst1 += rect->dst_row_length;
  }
}

void yuv2rgb15pc(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst1, *dst2, *py1, *py2, *pu, *pv, val;
  int i, j, k, r, g, b, cr, cg, cb;
  int y, u, v, ty1, ty2, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst1 = rect->dst + rect->dst_offset;

  val = 0; /* To make GCC happy */

  for (j=0; j<rect->yloop; j+=2) {
    if (rect->use_osd[j]) {
      py1 = ((unsigned long *)rect->osd[0]) + yadd;
      pu = ((unsigned long *)rect->osd[1]) + uvadd;
      pv = ((unsigned long *)rect->osd[2]) + uvadd;
    } else {
      py1 = ((unsigned long *)src[0]) + yadd;
      pu = ((unsigned long *)src[1]) + uvadd;
      pv = ((unsigned long *)src[2]) + uvadd;
    }

    py2 = py1 + yrow;
    dst2 = dst1 + rect->dst_row_length;

    for (i=0; i<(rect->xloop >> 3); i++) {
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        val = 0; /* GCC suxx */
        ty1 = *py1++;
        ty2 = *py2++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>24)&0xff) + cya;
        YUV2RGB15_LO;

        y = cym * ((ty1>>16)&0xff) + cya;
        YUV2RGB15_HI;
        SWAP32_W(dst1, val);
        dst1++;

        // 2
        y = cym * ((ty2>>24)&0xff) + cya;
        YUV2RGB15_LO;

        y = cym * ((ty2>>16)&0xff) + cya;
        YUV2RGB15_HI;
        SWAP32_W(dst2, val);
        dst2++;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;

        cr = crv*v;
        cg = cgu*u + cgv*v;
        cb = cbu*u;

        // 1
        y = cym * ((ty1>>8)&0xff) + cya;
        YUV2RGB15_LO;

        y = cym * (ty1&0xff) + cya;
        YUV2RGB15_HI;
        SWAP32_W(dst1, val);
        dst1++;

        // 2
        y = cym * ((ty2>>8)&0xff) + cya;
        YUV2RGB15_LO;

        y = cym * (ty2&0xff) + cya;
        YUV2RGB15_HI;
        SWAP32_W(dst2, val);
        dst2++;
      }
    }

    yadd += yrow;
    yadd += yrow;
    uvadd += uvrow;

    dst1 += rect->dst_row_length - (rect->xloop >> 1);
    dst1 += rect->dst_row_length;
  }
}

void yuv2bgra32(rect_type *rect, unsigned char *src[])
{
  unsigned long yrow, uvrow, yadd, uvadd;
  unsigned long *dst, *py, *pu, *pv;
  int i, j, k, r, g, b;
  int y, u, v, ty, tu, tv, crv, cbu, cgu, cgv, cya, cym, cuv;
  unsigned char *rgb_clip;

  yrow = rect->src_row_length;
  uvrow = rect->src_row_length >> 1;
  yadd = rect->src_offset;
  uvadd = rect->src_uv_offset;

  crv = rect->crv;
  cbu = rect->cbu;
  cgu = rect->cgu;
  cgv = rect->cgv;
  cym = rect->cym;
  cya = rect->cya;
  cuv = rect->cuv;

  rgb_clip = rect->rgb_clip;

  dst = rect->dst + rect->dst_offset;

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
      tu = *pu++ ^ cuv;
      tv = *pv++ ^ cuv;
      for (k=0; k<2; k++) {
        ty = *py++;
        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        y = cym * ((ty>>24)&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        *dst++ = r | g | b;

        y = cym * ((ty>>16)&0xff) + cya;
        r = rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        *dst++ = r | g | b;

        u = tu >> 24;
        v = tv >> 24;
        tu <<= 8;
        tv <<= 8;
        y = cym * ((ty>>8)&0xff) + cya;

        r = rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        *dst++ = r | g | b;

        y = cym * (ty&0xff) + cya;
        r = rgb_clip[(y + crv*v)>>FIXBITS] << 8;
        g = rgb_clip[(y + cgu*u + cgv*v)>>FIXBITS] << 16;
        b = rgb_clip[(y + cbu*u)>>FIXBITS] << 24;
        *dst++ = r | g | b;
      }
    }

    yadd += yrow;
    uvadd += (uvrow * (j & 1));
    dst += rect->dst_row_length - rect->xloop;
  }
}
