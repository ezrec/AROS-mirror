#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/*
    functions.h
*/

#include "chunky.h"
#include <string.h>
#include <stdlib.h>

#define	random(x) (rand() % (x))
#define	frandom(x) ((float)rand() * (float)(x) / (float)(RAND_MAX))

#ifdef __GNUC__

	static __inline double mestinvsqrt(double x)
	{
		asm ("frsqrte %0,%1" : "=f" (x) : "0" (x));
		return x;
	}

	static	__inline double mestinv(double x)
	{
		asm ("fres %0,%1" : "=f" (x) : "0" (x));
		return x;
	}

	#define	estinvsqrt(x) mestinvsqrt(x)
	#define	estinv(x) mestinv(x)

#else

double estinv(double);
double estinvsqrt(double);

#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) <= (b) ? (a) : (b))
#endif

float frand(float max);
double fsqrt(double x);
char *strdup(const char*);
char *strcat(char *, const char *);
char *strncpy(char *, const char *, size_t);
int	stricmp(const char *, const char *);
char *strstr(const char *, const char *);

#define GETR5(c)	(((c)>>11)&0x1f)
#define GETG6(c)	(((c)>>5)&0x3f)
#define GETB5(c)	((c) & 0x1f)

#define MASKR5(c)	 ((c) & 0xf800)
#define MASKG6(c)	 ((c) & 0x7e0)
#define MASKB5(c)	 ((c) & 0x1f)

#define	ColorF2I(r,g,b)	 ( ( (int)( (r)*255.0f ) << 16 ) | ( (int)( (g)*255.0f ) << 8 ) | ( (int)( (b)*255.0f ) ) )
#define	ColorI2I(r,g,b) (ULONG)( (r)<<16 | (g)<<8 | (b) )

#define	ftoi(x)	((int)(x))

static inline int align(int a, int b)
{
	return (a + b - 1) & (~(b - 1));
}

void InitFSQRT();

void chk_PutImageBrightnessPPC(struct chkimage *image,int x0,int y0,unsigned char *dest,int destwidth,int destheight,int bri);
void chk_AddImagePPC(struct chkimage *image,int x0,int y0,unsigned char *dest,int destwidth,int destheight);

/* string functions */

char *strduplicate(char *str);
#define	strDuplicate(str) strduplicate(str)

void strFixPath(char *str);
char *strReplaceExt(char *oldstring, char *oldext, char *newext);
char *strSetExtension(char *str, char *ext);
char *strPathOnly(char *str);
int strIsDir(char *path);
int	strMatch(char *name, char *pattern);
char *strReplace(char *oldname, char *newname);

/* params functions */

void ParamsInit(int argc, char *argv[]);
int	FindParam(char *param);

/* memory allocation functions */

#define	MALLOC_STRUCT(s)	malloc( sizeof(s) )
#define	CALLOC_STRUCT(s)	calloc(1,sizeof(s))

/* conversion functions */

void convertARGB1555_to_ARGB8888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB1555_to_RGB888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB1555_to_RGB565( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB4444_to_ARGB8888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB4444_to_RGB888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB4444_to_RGB565( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB565_to_ARGB8888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB565_to_RGB888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB565_to_RGB565( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_RGB565( 	  unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_RGB888( 	  unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB8888_to_ARGB8888( 	  unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertL8_to_L8( 	  unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_ARGB8888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_L8( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_A8( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_RGBA4444( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_ARGB4444( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB8888_to_ARGB4444( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_RGB888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB8888_to_RGB888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_RGB565(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_ARGB8888( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB0888_to_ARGB8888( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_A8(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGBA8888_to_L8(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertARGB8888_to_L8(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertA8_to_RGBA8888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertA8_to_RGB888( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertL8_to_RGB888( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertL8_to_ARGB8888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertA8_to_ARGB8888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertA8_to_ARGB4444(      unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertL8_to_RGB565( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_ARGB4444(      unsigned char *src, unsigned char *dst, int xsize, int ysize );
void convertARGB8888_to_RGBA8888( 	   unsigned char *src, unsigned char *dst,int xsize, int ysize );
void convertRGB888_to_RGBA8888(      unsigned char *src, unsigned char *dst,int xsize, int ysize );

void expandRGB888_to_RGBA8888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void expandRGB888_to_ARGB8888( unsigned char *src, unsigned char *dst,int xsize, int ysize );
void expandL8_to_ARGB8888( unsigned char *src, unsigned char *dst,int xsize, int ysize );

void convertRGB888_to_YUV422(unsigned char *src, unsigned char *dst,int xsize, int ysize);
void convertRGB888_to_YUV422_custom(unsigned char *src, unsigned char *dst,int xsize, int ysize);
void convertYUV422_to_RGB888(unsigned char *src, unsigned char *dst,int xsize, int ysize);
void convertYUV422_to_RGB888_custom(unsigned char *src, unsigned char *dst,int xsize, int ysize);
void convertYUV422_to_ARGB8888_custom(unsigned char *src, unsigned char *dst, int xsize, int ysize);

void ConversionInit(void);

float fClamp(float a);
#define clamp(_a, _amin, _amax)       \
({                                     \
	typeof(_a) a = _a;                \
	typeof(_a) amin = _amin;          \
	typeof(_a) amax = _amax;          \
	a < amin ? amin : a > amax ? amax : a;  \
})

void scaleCoordsWithAspect(int width, int height, int destWidth, int destHeight, int *newWidth, int *newHeight);


#endif
