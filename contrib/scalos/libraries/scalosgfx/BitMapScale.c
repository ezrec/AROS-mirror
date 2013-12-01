// BitMapScale.c
// $Date$
// $Revision$

// Algorithms and code parts taken from here:
// http://www.compuphase.com/graphic/scale.htm

#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/scale.h>
#include <cybergraphx/cybergraphics.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalosgfx.h"

//-----------------------------------------------------------------------

#define MULT_SCALE	4096

#define PIXEL_NORTH 	(y ? - src->argb_Width : 0)
#define PIXEL_SOUTH 	((y <  src->argb_Height - 1) ? src->argb_Width : 0)
#define PIXEL_WEST 	(x ? -1 : 0)
#define PIXEL_EAST 	((x <  src->argb_Width - 1) ? 1 : 0)

struct ARGBLong
	{
	LONG Red;
	LONG Green;
	LONG Blue;
	LONG Alpha;
	};

#define min(a, b)	((a) < (b) ? (a) : (b))

//-----------------------------------------------------------------------

static struct ARGB ScaleARGBBicubic(const struct ARGB *src,
        ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY);
static struct ARGB ScaleNearestNeighbour(const struct ARGB *s,
        ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY);
static struct ARGB INLINE ScaleARGBBilinear(const struct ARGB *s,
        ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY);
static struct ARGB INLINE ScaleARGBAverage2x2(const struct ARGB *src,
	ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY);
static struct ARGB INLINE ScaleARGBAverage3x3(const struct ARGB *src,
	ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY);
static struct ARGB INLINE ScaleARGBAverage4x4(const struct ARGB *src,
	ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY);
#if 0
static void ScaleARGB2X(const struct ARGBHeader *src, struct ARGBHeader *Dest);
#endif

//-----------------------------------------------------------------------

static struct ARGB ScaleNearestNeighbour(const struct ARGB *src,
        ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY)
{
	(void) x;
	(void) y;
	(void) SourceWidth;
	(void) SourceHeight;
	(void) FractPartX;
	(void) FractPartY;

	return src[0];
}


static struct ARGB ScaleARGBBicubic(const struct ARGB *src,
        ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY)
{
	const LONG BcScale = MULT_SCALE >> 2;
	const LONG BcScale2 = BcScale * BcScale;
	const LONG BcScale4 = BcScale2 * 4;
	LONG BcFracX = FractPartX >> 2;
	LONG BcFracY = FractPartY >> 2;
	LONG BcFracX2 = BcFracX * BcFracX;
	LONG BcFracY2 = BcFracY * BcFracY;
	struct ARGBLong d[4];
	struct ARGBLong px;
	struct ARGB result;
	ULONG advx1, advy1, advx2, advy2;
	LONG s[4], t[4];

	// handle image borders
	advx1 = (x + 1 >= SourceWidth)  ? 0 : 1;
	advx2 = (x + 2 >= SourceWidth)  ? 0 : 2;
	advy1 = (y + 1 >= SourceHeight) ? 0 : 1 * SourceWidth;
	advy2 = (y + 2 >= SourceHeight) ? 0 : 2 * SourceWidth;

	d1(KPrintF("%s/%ld:  SourceWidth=%lu  SourceHeight=%lu\n", __FUNC__, __LINE__, SourceWidth, SourceHeight));
	d1(KPrintF("%s/%ld:  s=%08lx  advx1=%lu  advy1=%lu\n", __FUNC__, __LINE__, s, advx1, advy1));

	s[0] = (( 2 * BcScale - 1 * BcFracX)  * BcFracX  / BcScale  - 1 * BcScale) * BcFracX / BcScale;	//   - 1
	s[1] = ( -5 * BcScale + 3 * BcFracX ) * BcFracX2 / BcScale2 + 2 * BcScale;  			//   0
	s[2] = (( 4 * BcScale - 3 * BcFracX)  * BcFracX  / BcScale  + 1 * BcScale) * BcFracX / BcScale;	//  +1
	s[3] = ( -1 * BcScale + 1 * BcFracX)  * BcFracX2 / BcScale2;                 			//  +2

	t[0] = (( 2 * BcScale - 1 * BcFracY)  * BcFracY  / BcScale  - 1 * BcScale) * BcFracY / BcScale; //   - 1
	t[1] = ( -5 * BcScale + 3 * BcFracY)  * BcFracY2 / BcScale2 + 2 * BcScale;			//   0
	t[2] = (( 4 * BcScale - 3 * BcFracY)  * BcFracY  / BcScale  + 1 * BcScale) * BcFracY / BcScale; //  +1
	t[3] = ( -1 * BcScale + 1 * BcFracY)  * BcFracY2 / BcScale2;                 			//  +2

#define	GETPIXEL(x, y) \
	px.Red   = src[(x)+(y)].Red; \
	px.Green = src[(x)+(y)].Green; \
	px.Blue  = src[(x)+(y)].Blue; \
	px.Alpha = src[(x)+(y)].Alpha;

#define SCALEADDPIXEL(dst, src, scale ) \
	dst.Red   += scale * src.Red; \
	dst.Green += scale * src.Green; \
	dst.Blue  += scale * src.Blue; \
	dst.Alpha += scale * src.Alpha;

#define SCALEPIXEL(dst, src, scale ) \
	dst.Red   = scale * src.Red; \
	dst.Green = scale * src.Green; \
	dst.Blue  = scale * src.Blue; \
	dst.Alpha = scale * src.Alpha;

	GETPIXEL(advx1, advy1);         SCALEPIXEL(d[0], px, s[0]);
	GETPIXEL(0, advy1);             SCALEADDPIXEL(d[0], px, s[1]);
	GETPIXEL(advx1, advy1);         SCALEADDPIXEL(d[0], px, s[2]);
	GETPIXEL(advx2, advy1);         SCALEADDPIXEL(d[0], px, s[3]);

	GETPIXEL(advx1, 0);             SCALEPIXEL(d[1], px, s[0]);
	GETPIXEL(0, 0);                 SCALEADDPIXEL(d[1], px, s[1]);
	GETPIXEL(advx1, 0);             SCALEADDPIXEL(d[1], px, s[2]);
	GETPIXEL(advx2, 0);             SCALEADDPIXEL(d[1], px, s[3]);

	GETPIXEL(advx1, advy1);         SCALEPIXEL(d[2], px, s[0]);
	GETPIXEL(0, advy1);             SCALEADDPIXEL(d[2], px, s[1]);
	GETPIXEL(advx1, advy1);         SCALEADDPIXEL(d[2], px, s[2]);
	GETPIXEL(advx2, advy1);         SCALEADDPIXEL(d[2], px, s[3]);

	GETPIXEL(advx1, advy2);         SCALEPIXEL(d[3], px, s[0]);
	GETPIXEL(0, advy2);             SCALEADDPIXEL(d[3], px, s[1]);
	GETPIXEL(advx1, advy2);         SCALEADDPIXEL(d[3], px, s[2]);
	GETPIXEL(advx2, advy2);         SCALEADDPIXEL(d[3], px, s[3]);

	d[0].Red   = ( d[0].Red   * t[0] + d[1].Red   * t[1] + d[2].Red   * t[2] + d[3].Red   * t[3] ) / BcScale4;
	d[0].Green = ( d[0].Green * t[0] + d[1].Green * t[1] + d[2].Green * t[2] + d[3].Green * t[3] ) / BcScale4;
	d[0].Blue  = ( d[0].Blue  * t[0] + d[1].Blue  * t[1] + d[2].Blue  * t[2] + d[3].Blue  * t[3] ) / BcScale4;
	d[0].Alpha = ( d[0].Alpha * t[0] + d[1].Alpha * t[1] + d[2].Alpha * t[2] + d[3].Alpha * t[3] ) / BcScale4;

	if (d[0].Red < 0)
		result.Red = 0;
	else if (d[0].Red > 255 )
		result.Red = 255;
	else
		result.Red = d[0].Red;

	if (d[0].Green < 0)
		result.Green = 0;
	else if (d[0].Green > 255 )
		result.Green = 255;
	else
		result.Green = d[0].Green;

	if (d[0].Blue < 0)
		result.Blue = 0;
	else if (d[0].Blue > 255 )
		result.Blue = 255;
	else
		result.Blue = d[0].Blue;

	if (d[0].Alpha < 0)
		result.Alpha = 0;
	else if (d[0].Alpha > 255 )
		result.Alpha = 255;
	else
		result.Alpha = d[0].Alpha;

	return result;
}


static struct ARGB ScaleARGBBilinear(const struct ARGB *src,
        ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY)
{
	ULONG v[2][2];
	ULONG vm[2][2];
	struct ARGB temp[2][2];
	struct ARGB d;
	ULONG advx, advy;

	// handle image borders
	advx = (x + 1 >= SourceWidth) ? 0 : 1;
	advy = (y + 1 >= SourceHeight) ? 0 : SourceWidth;

	d1(KPrintF("%s/%ld:  SourceWidth=%lu  SourceHeight=%lu\n", __FUNC__, __LINE__, SourceWidth, SourceHeight));
	d1(KPrintF("%s/%ld:  advx=%lu  advy=%lu\n", __FUNC__, __LINE__, advx, advy));

	// copy pixel data to temp
	temp[0][0] = src[0];
	temp[0][1] = src[advx];
	temp[1][0] = src[advy];
	temp[1][1] = src[advy + advx];

	vm[0][0] = (MULT_SCALE - FractPartX) * (MULT_SCALE - FractPartY);
	vm[0][1] = FractPartX * (MULT_SCALE - FractPartY);
	vm[1][0] = (MULT_SCALE - FractPartX) * FractPartY;
	vm[1][1] = FractPartX * FractPartY;

	// Alpha
	v[0][0] = vm[0][0] * temp[0][0].Alpha;
	v[0][1] = vm[0][1] * temp[0][1].Alpha;
	v[1][0] = vm[1][0] * temp[1][0].Alpha;
	v[1][1] = vm[1][1] * temp[1][1].Alpha;
	d.Alpha = (v[0][0] + v[1][0]
		 + v[0][1] + v[1][1] + MULT_SCALE * MULT_SCALE / 2) / (MULT_SCALE * MULT_SCALE);

	// Red
	v[0][0] = vm[0][0] * temp[0][0].Red;
	v[0][1] = vm[0][1] * temp[0][1].Red;
	v[1][0] = vm[1][0] * temp[1][0].Red;
	v[1][1] = vm[1][1] * temp[1][1].Red;
	d.Red =  (v[0][0] + v[1][0]
		+ v[0][1] + v[1][1] + MULT_SCALE * MULT_SCALE / 2) / (MULT_SCALE * MULT_SCALE);

	// Green
	v[0][0] = vm[0][0] * temp[0][0].Green;
	v[0][1] = vm[0][1] * temp[0][1].Green;
	v[1][0] = vm[1][0] * temp[1][0].Green;
	v[1][1] = vm[1][1] * temp[1][1].Green;
	d.Green = (v[0][0] + v[1][0]
		 + v[0][1] + v[1][1] + MULT_SCALE * MULT_SCALE / 2) / (MULT_SCALE * MULT_SCALE);

	// Blue
	v[0][0] = vm[0][0] * temp[0][0].Blue;
	v[0][1] = vm[0][1] * temp[0][1].Blue;
	v[1][0] = vm[1][0] * temp[1][0].Blue;
	v[1][1] = vm[1][1] * temp[1][1].Blue;
	d.Blue = (v[0][0] + v[1][0]
		+ v[0][1] + v[1][1] + MULT_SCALE * MULT_SCALE / 2) / (MULT_SCALE * MULT_SCALE);

	return d;
}


static struct ARGB INLINE ScaleARGBAverage2x2(const struct ARGB *src,
	ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY)
{
	ULONG v[2][2];
	struct ARGB temp[2][2];
	struct ARGB d;
	LONG BackX;
	LONG BackY;

	// handle image borders
	BackX = (x < 1) ? 0 : -1;
	BackY = (y < 1) ? 0 : -SourceWidth;

	// copy pixel data to temp
	temp[0][0] = src[BackY + BackX];
	temp[0][1] = src[BackY];
	temp[1][0] = src[BackX];
	temp[1][1] = src[0];

	// Alpha
	v[0][0] = temp[0][0].Alpha;
	v[0][1] = temp[0][1].Alpha;
	v[1][0] = temp[1][0].Alpha;
	v[1][1] = temp[1][1].Alpha;
	d.Alpha = (v[0][0] + v[0][1] + v[1][0] + v[1][1]) / 4;

	// Red
	v[0][0] = temp[0][0].Red;
	v[0][1] = temp[0][1].Red;
	v[1][0] = temp[1][0].Red;
	v[1][1] = temp[1][1].Red;
	d.Red = (v[0][0] + v[0][1] + v[1][0] + v[1][1]) / 4;

	// Green
	v[0][0] = temp[0][0].Green;
	v[0][1] = temp[0][1].Green;
	v[1][0] = temp[1][0].Green;
	v[1][1] = temp[1][1].Green;
	d.Green = (v[0][0] + v[0][1] + v[1][0] + v[1][1]) / 4;

	// Blue
	v[0][0] = temp[0][0].Blue;
	v[0][1] = temp[0][1].Blue;
	v[1][0] = temp[1][0].Blue;
	v[1][1] = temp[1][1].Blue;
	d.Blue = (v[0][0] + v[0][1] + v[1][0] + v[1][1]) / 4;

	return d;
}


static struct ARGB INLINE ScaleARGBAverage3x3(const struct ARGB *src,
	ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY)
{
	ULONG v[3][3];
	struct ARGB temp[3][3];
	struct ARGB d;
	LONG BackX;
	LONG BackY;

	// handle image borders
	BackX = (x < 1) ? 0 : -1;
	BackY = (y < 1) ? 0 : -SourceWidth;

	// copy pixel data to temp
	temp[0][0] = src[BackY + BackX];
	temp[0][1] = src[BackY];
	temp[0][2] = src[BackY + 1];
	temp[1][0] = src[BackX];
	temp[1][1] = src[0];
	temp[1][2] = src[1];
	temp[2][0] = src[SourceWidth + BackX];
	temp[2][1] = src[SourceWidth];
	temp[2][2] = src[SourceWidth + 1];

	// Alpha
	v[0][0] = temp[0][0].Alpha; v[0][1] = temp[0][1].Alpha; v[0][2] = temp[0][2].Alpha;
	v[1][0] = temp[1][0].Alpha; v[1][1] = temp[1][1].Alpha; v[1][2] = temp[1][2].Alpha;
	v[2][0] = temp[2][0].Alpha; v[2][1] = temp[2][1].Alpha; v[2][2] = temp[2][2].Alpha;
	d.Alpha = (v[0][0] + v[0][1] + v[0][2]
		 + v[1][0] + v[1][1] + v[1][2]
		 + v[2][0] + v[2][1] + v[2][2]) / 9;

	// Red
	v[0][0] = temp[0][0].Red; v[0][1] = temp[0][1].Red; v[0][2] = temp[0][2].Red;
	v[1][0] = temp[1][0].Red; v[1][1] = temp[1][1].Red; v[1][2] = temp[1][2].Red;
	v[2][0] = temp[2][0].Red; v[2][1] = temp[2][1].Red; v[2][2] = temp[2][2].Red;
	d.Red = (v[0][0] + v[0][1] + v[0][2]
	       + v[1][0] + v[1][1] + v[1][2]
	       + v[2][0] + v[2][1] + v[2][2]) / 9;

	// Green
	v[0][0] = temp[0][0].Green; v[0][1] = temp[0][1].Green; v[0][2] = temp[0][2].Green;
	v[1][0] = temp[1][0].Green; v[1][1] = temp[1][1].Green; v[1][2] = temp[1][2].Green;
	v[2][0] = temp[2][0].Green; v[2][1] = temp[2][1].Green; v[2][2] = temp[2][2].Green;
	d.Green = (v[0][0] + v[0][1] + v[0][2]
		 + v[1][0] + v[1][1] + v[1][2]
		 + v[2][0] + v[2][1] + v[2][2]) / 9;

	// Blue
	v[0][0] = temp[0][0].Blue; v[0][1] = temp[0][1].Blue; v[0][2] = temp[0][2].Blue;
	v[1][0] = temp[1][0].Blue; v[1][1] = temp[1][1].Blue; v[1][2] = temp[1][2].Blue;
	v[2][0] = temp[2][0].Blue; v[2][1] = temp[2][1].Blue; v[2][2] = temp[2][2].Blue;
	d.Blue = (v[0][0] + v[0][1] + v[0][2]
		+ v[1][0] + v[1][1] + v[1][2]
		+ v[2][0] + v[2][1] + v[2][2]) / 9;

	return d;
}


static struct ARGB INLINE ScaleARGBAverage4x4(const struct ARGB *src,
	ULONG x, ULONG y,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG FractPartX, ULONG FractPartY)
{
	ULONG v[4][4];
	struct ARGB temp[4][4];
	struct ARGB d;
	LONG BackX;
	LONG BackY;

	// handle image borders
	BackX = (x < 1) ? 0 : -1;
	BackY = (y < 1) ? 0 : -SourceWidth;

	// copy pixel data to temp
	temp[0][0] = src[BackY + BackX];
	temp[0][1] = src[BackY];
	temp[0][2] = src[BackY + 1];
	temp[0][3] = src[BackY + 2];
	temp[1][0] = src[BackX];
	temp[1][1] = src[0];
	temp[1][2] = src[1];
	temp[1][3] = src[2];
	temp[2][0] = src[SourceWidth + BackX];
	temp[2][1] = src[SourceWidth];
	temp[2][2] = src[SourceWidth + 1];
	temp[2][3] = src[SourceWidth + 2];
	temp[3][0] = src[SourceWidth * 2 + BackX];
	temp[3][1] = src[SourceWidth * 2];
	temp[3][2] = src[SourceWidth * 2 + 1];
	temp[3][3] = src[SourceWidth * 2 + 2];

	// Alpha
	v[0][0] = temp[0][0].Alpha; v[0][1] = temp[0][1].Alpha; v[0][2] = temp[0][2].Alpha; v[0][3] = temp[0][3].Alpha;
	v[1][0] = temp[1][0].Alpha; v[1][1] = temp[1][1].Alpha; v[1][2] = temp[1][2].Alpha; v[1][3] = temp[1][3].Alpha;
	v[2][0] = temp[2][0].Alpha; v[2][1] = temp[2][1].Alpha; v[2][2] = temp[2][2].Alpha; v[2][3] = temp[2][3].Alpha;
	v[3][0] = temp[3][0].Alpha; v[3][1] = temp[3][1].Alpha; v[3][2] = temp[3][2].Alpha; v[3][3] = temp[3][3].Alpha;
	d.Alpha = (v[0][0] + v[0][1] + v[0][2] + v[0][3]
		 + v[1][0] + v[1][1] + v[1][2] + v[1][3]
		 + v[2][0] + v[2][1] + v[2][2] + v[2][3]
                 + v[3][0] + v[3][1] + v[3][2] + v[3][3]) / 16;

	// Red
	v[0][0] = temp[0][0].Red; v[0][1] = temp[0][1].Red; v[0][2] = temp[0][2].Red; v[0][3] = temp[0][3].Red;
	v[1][0] = temp[1][0].Red; v[1][1] = temp[1][1].Red; v[1][2] = temp[1][2].Red; v[1][3] = temp[1][3].Red;
	v[2][0] = temp[2][0].Red; v[2][1] = temp[2][1].Red; v[2][2] = temp[2][2].Red; v[2][3] = temp[2][3].Red;
	v[3][0] = temp[3][0].Red; v[3][1] = temp[3][1].Red; v[3][2] = temp[3][2].Red; v[3][3] = temp[3][3].Red;
	d.Red = (v[0][0] + v[0][1] + v[0][2] + v[0][3]
	       + v[1][0] + v[1][1] + v[1][2] + v[1][3]
	       + v[2][0] + v[2][1] + v[2][2] + v[2][3]
               + v[3][0] + v[3][1] + v[3][2] + v[3][3]) / 16;

	// Green
	v[0][0] = temp[0][0].Green; v[0][1] = temp[0][1].Green; v[0][2] = temp[0][2].Green; v[0][3] = temp[0][3].Green;
	v[1][0] = temp[1][0].Green; v[1][1] = temp[1][1].Green; v[1][2] = temp[1][2].Green; v[1][3] = temp[1][3].Green;
	v[2][0] = temp[2][0].Green; v[2][1] = temp[2][1].Green; v[2][2] = temp[2][2].Green; v[2][3] = temp[2][3].Green;
	v[3][0] = temp[3][0].Green; v[3][1] = temp[3][1].Green; v[3][2] = temp[3][2].Green; v[3][3] = temp[3][3].Green;
	d.Green = (v[0][0] + v[0][1] + v[0][2] + v[0][3]
		 + v[1][0] + v[1][1] + v[1][2] + v[1][3]
		 + v[2][0] + v[2][1] + v[2][2] + v[2][3]
                 + v[3][0] + v[3][1] + v[3][2] + v[3][3]) / 16;

	// Blue
	v[0][0] = temp[0][0].Blue; v[0][1] = temp[0][1].Blue; v[0][2] = temp[0][2].Blue; v[0][3] = temp[0][3].Blue;
	v[1][0] = temp[1][0].Blue; v[1][1] = temp[1][1].Blue; v[1][2] = temp[1][2].Blue; v[1][3] = temp[1][3].Blue;
	v[2][0] = temp[2][0].Blue; v[2][1] = temp[2][1].Blue; v[2][2] = temp[2][2].Blue; v[2][3] = temp[2][3].Blue;
	v[3][0] = temp[3][0].Blue; v[3][1] = temp[3][1].Blue; v[3][2] = temp[3][2].Blue; v[3][3] = temp[3][3].Blue;
	d.Blue = (v[0][0] + v[0][1] + v[0][2] + v[0][3]
		+ v[1][0] + v[1][1] + v[1][2] + v[1][3]
		+ v[2][0] + v[2][1] + v[2][2] + v[2][3]
		+ v[3][0] + v[3][1] + v[3][2] + v[3][3]) / 16;

	return d;
}


// Scale 2x with directional interpolation
#if 0
static void ScaleARGB2X(const struct ARGBHeader *src, struct ARGBHeader *Dest)
{
	ULONG y;
	const struct ARGB *SourcePtr = src->argb_ImageData;
	ULONG DestWidth = 2 * src->argb_Width;

	for (y = 0; y < src->argb_Height; y++)
	{
		ULONG x;
		struct ARGB *TargetLineStart;

		TargetLineStart = Dest->argb_ImageData + 2 * y * Dest->argb_Width;

		for (x = 0; x <  src->argb_Width; x++)
		{
			struct ARGB v[3][3];
			struct ARGB *Target;

			Target = TargetLineStart + 2 * x;

			v[0][2] = SourcePtr[PIXEL_NORTH];
			v[1][0] = SourcePtr[PIXEL_WEST];
			v[1][1] = SourcePtr[0];

			v[1][2] = SourcePtr[PIXEL_EAST];
			v[2][1] = SourcePtr[PIXEL_SOUTH];

			Target[0]              = (ARGB_EQ(v[1][0], v[0][2]) && !ARGB_EQ(v[0][2], v[1][2]) && !ARGB_EQ(v[1][0], v[2][1])) ? v[1][0] : v[1][1];
			Target[1]              = (ARGB_EQ(v[0][2], v[1][2]) && !ARGB_EQ(v[0][2], v[1][0]) && !ARGB_EQ(v[1][2], v[2][1])) ? v[1][2] : v[1][1];
			Target[DestWidth]      = (ARGB_EQ(v[1][0], v[2][1]) && !ARGB_EQ(v[1][0], v[0][2]) && !ARGB_EQ(v[2][1], v[1][2])) ? v[1][0] : v[1][1];
			Target[DestWidth + 1]  = (ARGB_EQ(v[2][1], v[1][2]) && !ARGB_EQ(v[1][0], v[2][1]) && !ARGB_EQ(v[0][2], v[1][2])) ? v[1][2] : v[1][1];

			SourcePtr++;
		}
	}
}
#endif

struct ARGB *ScaleARGBArray(const struct ARGBHeader *src,
	ULONG *DestWidth, ULONG *DestHeight, ULONG Flags,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *DestArray = NULL;
	struct ARGBHeader TempARGB;

	memset(&TempARGB, 0, sizeof(TempARGB));

	if (Flags & SCALEFLAGF_CORRECTASPECT)
	{
		CalculateScaleAspect(src->argb_Width, src->argb_Height, DestWidth, DestHeight);
	}

	do	{
		struct ARGB (*ScalingFunction)(const struct ARGB *, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);
		ULONG y;
		struct ARGB *DestPtr;
		ULONG IntPartX, IntPartY;

		d1(KPrintF("%s/%ld:  DestWidth=%lu  DestHeight=%lu\n", __FUNC__, __LINE__, *DestWidth, *DestHeight));

		if (!(Flags & (SCALEFLAGF_BILINEAR | SCALEFLAGF_AVERAGE | SCALEFLAGF_DOUBLESIZE)))
			break;

		DestArray = AllocARGB(*DestWidth, *DestHeight, ScalosGfxBase);
		if (NULL == DestArray)
			break;

		if (src->argb_Width == *DestWidth && src->argb_Height == *DestHeight)
			{
			memcpy(DestArray, src->argb_ImageData,
				src->argb_Width * src->argb_Height * sizeof(struct ARGB));
			break;
			}

#if 0
		while ((Flags & SCALEFLAGF_DOUBLESIZE) &&
			((*DestWidth > src->argb_Width) || (*DestHeight > src->argb_Height)))
			{
			struct ARGBHeader Temp2ARGB;

			memset(&Temp2ARGB, 0, sizeof(Temp2ARGB));

			if (TempARGB.argb_ImageData)
				{
				Temp2ARGB = TempARGB;
				src = &Temp2ARGB;
				}

			TempARGB.argb_Width = 2 * src->argb_Width;
			TempARGB.argb_Height = 2 * src->argb_Height;
			TempARGB.argb_ImageData = AllocARGB(TempARGB.argb_Width, TempARGB.argb_Height,
				ScalosGfxBase);
			if (TempARGB.argb_ImageData)
				{
				ScaleARGB2X(src, &TempARGB);

				src = &TempARGB;
				}

			if (Temp2ARGB.argb_ImageData)
				FreeARGB(&Temp2ARGB.argb_ImageData, ScalosGfxBase);
			d1(KPrintF("%s/%ld:  ScaleARGB2X  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, src->argb_Width, src->argb_Height));
			}
#endif

		IntPartX = (src->argb_Width * MULT_SCALE) / *DestWidth;
		IntPartY = (src->argb_Height * MULT_SCALE) / *DestHeight;

#if 0
		ScalingFunction = ScaleARGBAverage2x2;
#else
		if ((Flags & SCALEFLAGF_AVERAGE)
			&& ((*DestWidth <= src->argb_Width / 4) || (*DestHeight <= src->argb_Height / 4)))
			{
			d1(KPrintF("%s/%ld:  ScaleARGBAverage4x4  Flags=%08lx\n", __FUNC__, __LINE__, Flags));
			ScalingFunction = ScaleARGBAverage4x4;
			}
		else if ((Flags & SCALEFLAGF_AVERAGE)
			&& (((2 * *DestWidth) < src->argb_Width / 5) || ((2 * *DestHeight) < src->argb_Height / 5)))
			{
			d1(KPrintF("%s/%ld:  ScaleARGBAverage3x3  Flags=%08lx\n", __FUNC__, __LINE__, Flags));
			ScalingFunction = ScaleARGBAverage3x3;
			}
		else if ((Flags & SCALEFLAGF_AVERAGE)
			&& (((5 * *DestWidth) < src->argb_Width / 3) || ((5 * *DestHeight) < src->argb_Height / 3)))
			{
			d1(KPrintF("%s/%ld:  ScaleARGBAverage2x2  Flags=%08lx\n", __FUNC__, __LINE__, Flags));
			ScalingFunction = ScaleARGBAverage2x2;
			}
		else if (Flags & SCALEFLAGF_BICUBIC)
			{
			d1(KPrintF("%s/%ld:  ScaleARGBBicubic  Flags=%08lx\n", __FUNC__, __LINE__, Flags));
			ScalingFunction	= ScaleARGBBicubic;
			}
		else if (Flags & SCALEFLAGF_BILINEAR)
			{
			d1(KPrintF("%s/%ld:  ScaleARGBBilinear  Flags=%08lx\n", __FUNC__, __LINE__, Flags));
			ScalingFunction	= ScaleARGBBilinear;
			}
		else
			{
			d1(KPrintF("%s/%ld:  ScaleNearestNeighbour  Flags=%08lx\n", __FUNC__, __LINE__, Flags));
			ScalingFunction = ScaleNearestNeighbour;
			}
#endif

		d1(KPrintF("%s/%ld:  IntPartX=%lu  IntPartY=%lu\n", __FUNC__, __LINE__, IntPartX, IntPartY));

		DestPtr = DestArray;
		for (y = 0; y < *DestHeight; y++)
			{
			ULONG x;
			ULONG TargetY;
			LONG FractPartY;
			const struct ARGB *SrcLineStart;

			TargetY = (IntPartY * y);
			FractPartY = TargetY & (MULT_SCALE - 1);
			TargetY /= MULT_SCALE;

			d1(KPrintF("%s/%ld:  DestPtr=%08lx  TargetY=%lu  FractPartY=%lu\n", __FUNC__, __LINE__, DestPtr, TargetY, FractPartY));

			SrcLineStart = src->argb_ImageData + TargetY * src->argb_Width;

			for (x = 0; x < *DestWidth; x++)
				{
				LONG FractPartX;
				ULONG TargetX;
				const struct ARGB *SourcePtr;

				TargetX = (IntPartX * x);
				FractPartX = TargetX & (MULT_SCALE - 1);
				TargetX	/= MULT_SCALE;

				SourcePtr = SrcLineStart + TargetX;

				*DestPtr = ScalingFunction(SourcePtr,
					TargetX, TargetY,
					src->argb_Width, src->argb_Height,
					FractPartX, FractPartY);

				DestPtr++;
				}
			}
		} while (0);

	if (TempARGB.argb_ImageData)
		FreeARGB(&TempARGB.argb_ImageData, ScalosGfxBase);

	return DestArray;
}


struct BitMap *ScaleBitMap(struct BitMap *SourceBM,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG *DestWidth, ULONG *DestHeight,
	ULONG NumberOfColors, const ULONG *ColorTable,
	ULONG Flags, struct BitMap *ScreenBM,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGBHeader Source;
	struct ARGB *DestArray = NULL;
	ULONG Depth;
	struct BitMap *DestBM = NULL;

	d1(KPrintF("%s/%ld:  START\n", __FUNC__, __LINE__));

	memset(&Source, 0, sizeof(Source));

	Depth = GetBitMapAttr(SourceBM, BMA_DEPTH);

	if (Flags & SCALEFLAGF_CORRECTASPECT)
		{
		CalculateScaleAspect(SourceWidth, SourceHeight,
			 DestWidth, DestHeight);
		}

	d1(KPrintF("%s/%ld:  CyberGfxBase=%08lx  ScreenBM=%08lx\n", __FUNC__, __LINE__, CyberGfxBase, ScreenBM));
	d1(KPrintF("%s/%ld:  SourceWidth=%lu  SourceHeight=%ld\n", __FUNC__, __LINE__, SourceWidth, SourceHeight));
	d1(KPrintF("%s/%ld:  DestWidth=%lu  DestHeight=%ld\n", __FUNC__, __LINE__, *DestWidth, *DestHeight));
	d1(KPrintF("%s/%ld:  NumberOfColors=%ld Depth=%lu\n", __FUNC__, __LINE__, NumberOfColors, Depth));

	do	{
		if (Source.argb_Width == *DestWidth && Source.argb_Height == *DestHeight)
			break;

		Source.argb_Width = SourceWidth;
		Source.argb_Height = SourceHeight;

		DestBM = AllocBitMap(*DestWidth, *DestHeight,
			Depth, 0, SourceBM);
		if (NULL == DestBM)
			break;

		d1(KPrintF("%s/%ld:  DestBM: Width=%lu  Height=%ld  Depth=%ld\n", \
			__FUNC__, __LINE__, \
			GetBitMapAttr(DestBM, BMA_WIDTH), \
			GetBitMapAttr(DestBM, BMA_HEIGHT), \
			GetBitMapAttr(DestBM, BMA_DEPTH)));

		if (CyberGfxBase)
			{
			d1(KPrintF("%s/%ld:  ARGB Scaling\n", __FUNC__, __LINE__));

			Source.argb_ImageData = CreateARGBFromBitMap(SourceBM,
				Source.argb_Width, Source.argb_Height,
				NumberOfColors, ColorTable, NULL, ScalosGfxBase);
			d1(KPrintF("%s/%ld:  argb_ImageData=%08lx\n", __FUNC__, __LINE__, Source.argb_ImageData));
			if (NULL == Source.argb_ImageData)
				break;

			DestArray = ScaleARGBArray(&Source,
				DestWidth, DestHeight,
				Flags, ScalosGfxBase);
			d1(KPrintF("%s/%ld:  DestArray=%08lx\n", __FUNC__, __LINE__, DestArray));
			if (NULL == DestArray)
				break;

			WriteARGBToBitMap(DestArray,
				DestBM, *DestWidth, *DestHeight,
				NumberOfColors, ColorTable, ScalosGfxBase);
			}
		else
			{
			struct BitScaleArgs bitScaleArgs;

			d1(KPrintF("%s/%ld:  BitMapScale Scaling\n", __FUNC__, __LINE__));

			bitScaleArgs.bsa_SrcBitMap = SourceBM;
			bitScaleArgs.bsa_DestBitMap = DestBM;

			bitScaleArgs.bsa_SrcX = 0;
			bitScaleArgs.bsa_SrcY = 0;

			bitScaleArgs.bsa_SrcWidth = SourceWidth;
			bitScaleArgs.bsa_SrcHeight = SourceHeight;

			bitScaleArgs.bsa_DestX = 0;
			bitScaleArgs.bsa_DestY = 0;

			bitScaleArgs.bsa_Flags = 0;

			bitScaleArgs.bsa_XSrcFactor = SourceWidth;
			bitScaleArgs.bsa_YSrcFactor = SourceHeight;
			bitScaleArgs.bsa_XDestFactor = *DestWidth;
			bitScaleArgs.bsa_YDestFactor = *DestHeight;

			BitMapScale(&bitScaleArgs);
			}
		} while (0);

	d1(KPrintF("%s/%ld:  \n", __FUNC__, __LINE__));

	FreeARGB(&Source.argb_ImageData, ScalosGfxBase);
	FreeARGB(&DestArray, ScalosGfxBase);

	d1(KPrintF("%s/%ld:  END\n", __FUNC__, __LINE__));

	return DestBM;
}


void CalculateScaleAspect(ULONG SourceWidth, ULONG SourceHeight,
	ULONG *DestWidth, ULONG *DestHeight)
{
	d1(KPrintF("%s/%ld:  SourceWidth=%lu  SourceHeight=%lu\n", __FUNC__, __LINE__, SourceWidth, SourceHeight));

	if (SourceWidth > SourceHeight)
		{
		// image is wider than tall
		*DestHeight = (SourceHeight *
			((*DestWidth << 16) / SourceWidth)) >> 16;
		if (*DestHeight < 1)
			*DestHeight = 1;
		}
	else
		{
		// image is taller than wide
		*DestWidth = (SourceWidth *
			((*DestHeight << 16) / SourceHeight)) >> 16;
		if (*DestWidth < 1)
			*DestWidth = 1;
		}

	d1(KPrintF("%s/%ld:  DestWidth=%lu  DestHeight=%lu\n", __FUNC__, __LINE__, *DestWidth, *DestHeight));
}

