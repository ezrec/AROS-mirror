/*
 *
 * refresh_internal.h
 *
 */

#ifndef REFRESH_INTERNAL_H
#define REFRESH_INTERNAL_H


/* Internal refresh defines */

#define PIXEL_MODE_ARGB32  1
#define PIXEL_MODE_BGR24   2
#define PIXEL_MODE_RGB16   3
#define PIXEL_MODE_RGB15   4
#define PIXEL_MODE_RGB16PC 5
#define PIXEL_MODE_RGB15PC 6
#define PIXEL_MODE_LUT8    7
#define PIXEL_MODE_BGRA32  8

#define TYPE_CHUNKY  1
#define TYPE_PLANAR  2
#define TYPE_OVERLAY 3


/* Internal rect structure */

typedef struct {

  /* Destination */
  unsigned long *dst;

  /* Generic */
  int xloop;
  int yloop;
  int src_offset;
  int dst_offset;
  int src_row_length;
  int dst_row_length;
  int src_uv_offset;

  /* AGA */
  int aga_half_height;
  int aga_xloop;
  int aga_yloop;
  int aga_src_offset;
  int aga_dst_offset;
  int aga_left_over_l;
  int aga_left_over_r;
  int aga_dst_col_skip;
  int aga_rasize;

  /* YUV -> RGB */
  unsigned char *y_table;
  unsigned char *cb_table;
  unsigned char *cr_table;
  unsigned char *rgb_clip;
  int crv;
  int cgu;
  int cgv;
  int cbu;
  int cym;
  int cya;
  int cuv;

  /* OSD */
  unsigned char *osd[3];
  unsigned char *use_osd;

  /* ARGB32 palette */
  unsigned long *palette;

  /* GRAY lookup table */
  unsigned char *gray;

  /* CLIP table [-1024, 1024] -> [0, 255] */
  unsigned char *clip;

  /* Pixel mapping for 8bit window playback */
  unsigned char *pixel;


  /* The following is not used by the refresh functions */


  /* Pointers to start of buffers */
  unsigned char *buf_rgb_clip;
  unsigned char *buf_use_osd;

  /* Init and Exit functions */
  void (*init)(void);
  void (*exit)(void);

  /* Image size */
  int width;
  int height;
  int line_width;
  int mode;

  /* AmigaOS specific information */
  int screen_width;
  int screen_height;
  int bpr;
  int bpp;
  int screen_type;
  int pixel_mode;

} rect_type;


/* External refresh structure */

extern rect_type refresh_rect;


/* Colorspace conversion */

/* 10.22 fixed point math used by YUV2RGB conversion */
#define FIXBITS 22

/* Should be used to convert from RGB color to gray */
#define CR 5016388
#define CG 9848226
#define CB 1912603
#define FR 8388608
#define RGB2GRAY(r,g,b) (((((r)*cr)+((g)*cg)+((b)*cb)+fr)>>24)&0xff)


/* Dithering */

/* 4x4 ordered dither
 *
 * threshold pattern:
 *   0  8  2 10
 *  12  4 14  6
 *   3 11  1  9
 *  15  7 13  5
 */

#define DITHER_YUV_LINE(a,b,c,d,len) \
for (i=0; i<len; i+=8) { \
  y = *py++; \
  tu = *pu++ ^ cuv; \
  tv = *pv++ ^ cuv; \
  u = (tu>>25)&0x7f; \
  v = (tv>>25)&0x7f; \
  val = PIXEL(y_table[((y>>24)&0xff)+a]|cb_table[u+a]|cr_table[v+a]) << 24; \
  val |= PIXEL(y_table[((y>>16)&0xff)+b]|cb_table[u+b]|cr_table[v+b]) << 16; \
  u = (tu>>17)&0x7f; \
  v = (tv>>17)&0x7f; \
  val |= PIXEL(y_table[((y>>8)&0xff)+c]|cb_table[u+c]|cr_table[v+c]) << 8; \
  val |= PIXEL(y_table[(y&0xff)+d]|cb_table[u+d]|cr_table[v+d]); \
  *dst++ = val; \
  y = *py++; \
  u = (tu>>9)&0x7f; \
  v = (tv>>9)&0x7f; \
  val = PIXEL(y_table[((y>>24)&0xff)+a]|cb_table[u+a]|cr_table[v+a]) << 24; \
  val |= PIXEL(y_table[((y>>16)&0xff)+b]|cb_table[u+b]|cr_table[v+b]) << 16; \
  u = (tu>>1)&0x7f; \
  v = (tv>>1)&0x7f; \
  val |= PIXEL(y_table[((y>>8)&0xff)+c]|cb_table[u+c]|cr_table[v+c]) << 8; \
  val |= PIXEL(y_table[(y&0xff)+d]|cb_table[u+d]|cr_table[v+d]); \
  *dst++ = val; \
}

#define DITHER_YUV_LINE0(len) DITHER_YUV_LINE( 0,  8,  2, 10, len)
#define DITHER_YUV_LINE1(len) DITHER_YUV_LINE(12,  4, 14,  6, len)
#define DITHER_YUV_LINE2(len) DITHER_YUV_LINE( 3, 11,  1,  9, len)
#define DITHER_YUV_LINE3(len) DITHER_YUV_LINE(15,  7, 13,  5, len)

#define DITHER_RGB_LINE(d1,d2,d3,d4,len) \
for (i=0; i<len; i+=4) { \
  val = *argb++; \
  r = clip[((val >> 16) & 0xff) + d1]; \
  g = clip[((val >>  8) & 0xff) + d1]; \
  b = clip[((val >>  0) & 0xff) + d1]; \
  lut = PIXEL((r & 0xe0) | ((g & 0xe0) >> 3) | ((b & 0xc0) >> 6)) << 24; \
  val = *argb++; \
  r = clip[((val >> 16) & 0xff) + d2]; \
  g = clip[((val >>  8) & 0xff) + d2]; \
  b = clip[((val >>  0) & 0xff) + d2]; \
  lut |= PIXEL((r & 0xe0) | ((g & 0xe0) >> 3) | ((b & 0xc0) >> 6)) << 16; \
  val = *argb++; \
  r = clip[((val >> 16) & 0xff) + d3]; \
  g = clip[((val >>  8) & 0xff) + d3]; \
  b = clip[((val >>  0) & 0xff) + d3]; \
  lut |= PIXEL((r & 0xe0) | ((g & 0xe0) >> 3) | ((b & 0xc0) >> 6)) << 8; \
  val = *argb++; \
  r = clip[((val >> 16) & 0xff) + d4]; \
  g = clip[((val >>  8) & 0xff) + d4]; \
  b = clip[((val >>  0) & 0xff) + d4]; \
  lut |= PIXEL((r & 0xe0) | ((g & 0xe0) >> 3) | ((b & 0xc0) >> 6)) << 0; \
  *dst++ = lut; \
}

#define DITHER_RGB_LINE0(len) DITHER_RGB_LINE( 0,  8,  2, 10, len)
#define DITHER_RGB_LINE1(len) DITHER_RGB_LINE(12,  4, 14,  6, len)
#define DITHER_RGB_LINE2(len) DITHER_RGB_LINE( 3, 11,  1,  9, len)
#define DITHER_RGB_LINE3(len) DITHER_RGB_LINE(15,  7, 13,  5, len)


/* C2P */

#define TWOMERGE(a, b, c, d, shift, mask) { \
  temp1 = b; \
  temp2 = d; \
  temp1 >>= shift; \
  temp2 >>= shift; \
  temp1 ^= a; \
  temp2 ^= c; \
  temp1 &= mask; \
  temp2 &= mask; \
  a ^= temp1; \
  c ^= temp2; \
  temp1 <<= shift; \
  temp2 <<= shift; \
  b ^= temp1; \
  d ^= temp2; \
}

#define SPECIALMERGE(reg0, reg1) \
  reg0 = ((reg0 & 0xf0f0f0f0) | ((reg1 >> 4) & 0x0f0f0f0f));


/* PPC */

/* Here are macros using inline PPC asm to speed up some operations. Whenever possible, there
 * will also be a portable macro that works on all big endian systems, in case I decide to
 * compile a 68k version or similar one day.
 */

#ifdef __PPC__

  #define SWAP32_W(address, val) \
    ({ unsigned long rS = (val), rB = (unsigned long)(address); asm("stwbrx %0,0,%1" : : "r" (rS), "r" (rB)); })

  #define RLWIMI(dst, src, SH, MB, ME) \
    ({ asm("rlwimi %0,%1,%2,%3,%4" : : "r" (dst), "r" (src), "g" (SH), "g" (MB), "g" (ME)); })

#else

  #define SWAP32_W(address, val) \
    ({ unsigned long x = (val); x = (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24); *(unsigned long *)address = x; })

#endif


/* YUV (YCrCb) 420 */

/* yuv2ham6.c */
void yuv2ham6_1(rect_type *rect, unsigned char *src[]);
void yuv2ham6_2(rect_type *rect, unsigned char *src[]);
void yuv2ham6_4(rect_type *rect, unsigned char *src[]);
void yuv2ham6_1hq(rect_type *rect, unsigned char *src[]);

/* yuv2ham8.c */
void yuv2ham8_1(rect_type *rect, unsigned char *src[]);
void yuv2ham8_2(rect_type *rect, unsigned char *src[]);
void yuv2ham8_4(rect_type *rect, unsigned char *src[]);
void yuv2ham8_1hq(rect_type *rect, unsigned char *src[]);

/* yuv2aga.c */
void yuv2aga(rect_type *rect, unsigned char *src[]);
void yuv2aga_gray8(rect_type *rect, unsigned char *src[]);
void yuv2aga_gray6(rect_type *rect, unsigned char *src[]);
void yuv2aga_gray4(rect_type *rect, unsigned char *src[]);

/* yuv2rgb.c */
void yuv2argb32(rect_type *rect, unsigned char *src[]);
void yuv2bgr24(rect_type *rect, unsigned char *src[]);
void yuv2rgb16(rect_type *rect, unsigned char *src[]);
void yuv2rgb15(rect_type *rect, unsigned char *src[]);
void yuv2rgb16pc(rect_type *rect, unsigned char *src[]);
void yuv2rgb15pc(rect_type *rect, unsigned char *src[]);
void yuv2bgra32(rect_type *rect, unsigned char *src[]);

/* yuv2gray.c */
void yuv2argb32_gray(rect_type *rect, unsigned char *src[]);
void yuv2bgr24_gray(rect_type *rect, unsigned char *src[]);
void yuv2rgb16_gray(rect_type *rect, unsigned char *src[]);
void yuv2rgb15_gray(rect_type *rect, unsigned char *src[]);
void yuv2rgb16pc_gray(rect_type *rect, unsigned char *src[]);
void yuv2rgb15pc_gray(rect_type *rect, unsigned char *src[]);
void yuv2bgra32_gray(rect_type *rect, unsigned char *src[]);

/* yuv2lut.c */
void yuv2lut(rect_type *rect, unsigned char *src[]);
void yuv2lut_gray(rect_type *rect, unsigned char *src[]);

/* yuv2win.c */
void yuv2win(rect_type *rect, unsigned char *src[]);
void yuv2win_gray(rect_type *rect, unsigned char *src[]);

/* yuv2overlay.c */
void yuv2overlay(rect_type *rect, unsigned char *src[]);
void yuv2overlay_gray(rect_type *rect, unsigned char *src[]);


/* ARGB32 */

/* argb2ham6.c */
void argb2ham6_1(rect_type *rect, unsigned char *src[]);
void argb2ham6_2(rect_type *rect, unsigned char *src[]);
void argb2ham6_4(rect_type *rect, unsigned char *src[]);
void argb2ham6_1hq(rect_type *rect, unsigned char *src[]);

/* argb2ham8.c */
void argb2ham8_1(rect_type *rect, unsigned char *src[]);
void argb2ham8_2(rect_type *rect, unsigned char *src[]);
void argb2ham8_4(rect_type *rect, unsigned char *src[]);
void argb2ham8_1hq(rect_type *rect, unsigned char *src[]);

/* argb2aga.c */
void argb2aga(rect_type *rect, unsigned char *src[]);
void argb2aga_gray8(rect_type *rect, unsigned char *src[]);
void argb2aga_gray6(rect_type *rect, unsigned char *src[]);
void argb2aga_gray4(rect_type *rect, unsigned char *src[]);

/* argb2rgb.c */
void argb2argb32(rect_type *rect, unsigned char *src[]);
void argb2bgr24(rect_type *rect, unsigned char *src[]);
void argb2rgb16(rect_type *rect, unsigned char *src[]);
void argb2rgb15(rect_type *rect, unsigned char *src[]);
void argb2rgb16pc(rect_type *rect, unsigned char *src[]);
void argb2rgb15pc(rect_type *rect, unsigned char *src[]);
void argb2bgra32(rect_type *rect, unsigned char *src[]);

/* argb2gray.c */
void argb2argb32_gray(rect_type *rect, unsigned char *src[]);
void argb2bgr24_gray(rect_type *rect, unsigned char *src[]);
void argb2rgb16_gray(rect_type *rect, unsigned char *src[]);
void argb2rgb15_gray(rect_type *rect, unsigned char *src[]);
void argb2rgb16pc_gray(rect_type *rect, unsigned char *src[]);
void argb2rgb15pc_gray(rect_type *rect, unsigned char *src[]);
void argb2bgra32_gray(rect_type *rect, unsigned char *src[]);

/* argb2lut.c */
void argb2lut(rect_type *rect, unsigned char *src[]);
void argb2lut_gray(rect_type *rect, unsigned char *src[]);

/* argb2win.c */
void argb2win(rect_type *rect, unsigned char *src[]);
void argb2win_gray(rect_type *rect, unsigned char *src[]);

/* argb2overlay.c */
void argb2overlay(rect_type *rect, unsigned char *src[]);
void argb2overlay_gray(rect_type *rect, unsigned char *src[]);


/* LUT8 */

/* lut2aga.c */
void lut2aga(rect_type *rect, unsigned char *src[]);
void lut2aga_gray8(rect_type *rect, unsigned char *src[]);
void lut2aga_gray6(rect_type *rect, unsigned char *src[]);
void lut2aga_gray4(rect_type *rect, unsigned char *src[]);

/* lut2rgb.c */
void lut2argb32(rect_type *rect, unsigned char *src[]);
void lut2bgr24(rect_type *rect, unsigned char *src[]);
void lut2rgb16(rect_type *rect, unsigned char *src[]);
void lut2rgb15(rect_type *rect, unsigned char *src[]);
void lut2rgb16pc(rect_type *rect, unsigned char *src[]);
void lut2rgb15pc(rect_type *rect, unsigned char *src[]);
void lut2bgra32(rect_type *rect, unsigned char *src[]);

/* lut2gray.c */
void lut2argb32_gray(rect_type *rect, unsigned char *src[]);
void lut2bgr24_gray(rect_type *rect, unsigned char *src[]);
void lut2rgb16_gray(rect_type *rect, unsigned char *src[]);
void lut2rgb15_gray(rect_type *rect, unsigned char *src[]);
void lut2rgb16pc_gray(rect_type *rect, unsigned char *src[]);
void lut2rgb15pc_gray(rect_type *rect, unsigned char *src[]);
void lut2bgra32_gray(rect_type *rect, unsigned char *src[]);

/* lut2lut.c */
void lut2lut(rect_type *rect, unsigned char *src[]);
void lut2lut_gray(rect_type *rect, unsigned char *src[]);

/* lut2win.c */
void lut2win(rect_type *rect, unsigned char *src[]);
void lut2win_gray(rect_type *rect, unsigned char *src[]);

/* lut2overlay.c */
void lut2overlay(rect_type *rect, unsigned char *src[]);
void lut2overlay_gray(rect_type *rect, unsigned char *src[]);

#endif
