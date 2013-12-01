// Render.c
// $Date$
// $Revision$

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
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/scalos.h>
#include <proto/layers.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalosgfx.h"

#include <math.h>

//-----------------------------------------------------------------------

#define	abs(x)		((x) < 0 ? -(x) : x)

//-----------------------------------------------------------------------

static struct ARGB *CreateGradientVertical(LONG Width, LONG Height, struct ARGB Top,
	struct ARGB Bottom, struct ScalosGfxBase *ScalosGfxBase);
static struct ARGB *CreateGradientHorizontal(LONG Width, LONG Height, struct ARGB Left,
	struct ARGB Right, struct ScalosGfxBase *ScalosGfxBase);
#ifndef __amigaos4__
static BOOL DrawGradientVerticalRastPort(struct RastPort *rp, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Top, struct ARGB Bottom, struct ScalosGfxBase *ScalosGfxBase);
static BOOL DrawGradientHorizontalRastPort(struct RastPort *rp, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Left, struct ARGB Right, struct ScalosGfxBase *ScalosGfxBase);
#endif //__amigaos4__
static BOOL DrawGradientVertical(struct ARGBHeader *argbh, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Top, struct ARGB Bottom, struct ScalosGfxBase *ScalosGfxBase);
static BOOL DrawGradientHorizontal(struct ARGBHeader *argbh, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Left, struct ARGB Right, struct ScalosGfxBase *ScalosGfxBase);
static void DrawRGBPixelWeighted(struct ARGBHeader *argbh,
	WORD x, WORD y, const struct ARGB *color, UBYTE Weight);
static void DrawRGBPixelWeightedRastPort(struct RastPort *rp,
	WORD x, WORD y, const struct ARGB *color, UBYTE Weight);

//-----------------------------------------------------------------------

BOOL ScaDrawGradient(struct ARGBHeader *dest, LONG left, LONG top,
	LONG width, LONG height, struct gfxARGB *start, struct gfxARGB *stop,
	ULONG gradType, struct ScalosGfxBase *ScalosGfxBase)
{
	BOOL Success = FALSE;

	(void) ScalosGfxBase;

	if (SCALOS_GRADIENT_VERTICAL == gradType)
		{
		Success = DrawGradientVertical(dest, left, top, width, height,
			*start, *stop, ScalosGfxBase);
		}
	else if (SCALOS_GRADIENT_HORIZONTAL == gradType)
		{
		Success = DrawGradientHorizontal(dest, left, top, width, height,
			*start, *stop, ScalosGfxBase);
		}

	return Success;
}

//-----------------------------------------------------------------------

BOOL DrawGradientRastPort(struct RastPort *rp, LONG left, LONG top,
	LONG width, LONG height, struct gfxARGB *start, struct gfxARGB *stop,
	ULONG gradType, struct ScalosGfxBase *ScalosGfxBase)
{
	BOOL Success = FALSE;

	(void) ScalosGfxBase;

#ifdef __amigaos4__
	{
	struct Screen *pubScreen = NULL;
	struct DrawInfo *dri = NULL;

	do	{
		struct GradientSpec gradSpec;

		memset(&gradSpec, 0, sizeof(gradSpec));

		if (SCALOS_GRADIENT_VERTICAL == gradType)
			gradSpec.Direction = DirectionVector(0);
		else if (SCALOS_GRADIENT_HORIZONTAL == gradType)
			gradSpec.Direction = DirectionVector(90);
		else
			break;

		pubScreen = LockPubScreen(NULL);
		if (NULL == pubScreen)
			break;

		dri = GetScreenDrawInfo(pubScreen);
		if (NULL == dri)
			break;

		gradSpec.Mode = GRADMODE_COLOR;
		gradSpec.Specs.Abs.RGBStart[0] = start->Red;
		gradSpec.Specs.Abs.RGBStart[1] = start->Green;
		gradSpec.Specs.Abs.RGBStart[2] = start->Blue;
		gradSpec.Specs.Abs.RGBEnd[0] = stop->Red;
		gradSpec.Specs.Abs.RGBEnd[1] = stop->Green;
		gradSpec.Specs.Abs.RGBEnd[2] = stop->Blue;

		Success = DrawGradient(rp, left, top, width, height, NULL, 0, &gradSpec, dri);
		} while (0);

	if (dri)
		FreeScreenDrawInfo(pubScreen, dri);
	if (pubScreen)
		UnlockPubScreen(NULL, pubScreen);
	}
#else //__amigaos4__
	if (CyberGfxBase->lib_Version >= 50)
		{
		ULONG paGradType;

		if (SCALOS_GRADIENT_VERTICAL == gradType)
			paGradType = GRADTYPE_VERTICAL;
		else if (SCALOS_GRADIENT_HORIZONTAL == gradType)
			paGradType = GRADTYPE_HORIZONTAL;
		else
			return FALSE;

		d1(KPrintF("%s/%s/%ld: paGradType=%ld\n", __FILE__, __FUNC__, __LINE__, paGradType));

		ProcessPixelArrayTags(rp,
			left, top,
			width,
			height,
			POP_GRADIENT,
			0,
			PPAOPTAG_GRADIENTTYPE, paGradType,
			PPAOPTAG_GRADCOLOR1, *((ULONG *) start),
			PPAOPTAG_GRADCOLOR2, *((ULONG *) stop),
			TAG_END);

		Success = TRUE;
		}
	else
		{
		if (SCALOS_GRADIENT_VERTICAL == gradType)
			{
			Success = DrawGradientVerticalRastPort(rp, left, top,
				width, height, *start, *stop, ScalosGfxBase);
			}
		else if (SCALOS_GRADIENT_HORIZONTAL == gradType)
			{
			Success = DrawGradientHorizontalRastPort(rp, left, top,
				width, height, *start, *stop, ScalosGfxBase);
			}
		}
#endif //__amigaos4__

	return Success;
}

//-----------------------------------------------------------------------

static struct ARGB *CreateGradientVertical(LONG Width, LONG Height, struct ARGB Top,
	struct ARGB Bottom, struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *Source;
	LONG DeltaRed, DeltaGreen, DeltaBlue, DeltaAlpha;
	LONG StepRed, StepGreen, StepBlue, StepAlpha;
	LONG RemRed, RemGreen, RemBlue, RemAlpha;

	d1(KPrintF("%s/%s/%ld: Top   Red=%3ld Green=%2ld Blue=%3ld\n", __FILE__, __FUNC__, __LINE__, Top.Red, Top.Green, Top.Blue));
	d1(KPrintF("%s/%s/%ld: Bottom  Red=%3ld Green=%2ld Blue=%3ld\n", __FILE__, __FUNC__, __LINE__, Bottom.Red, Bottom.Green, Bottom.Blue));

	DeltaRed   = Bottom.Red - Top.Red;
	DeltaGreen = Bottom.Green - Top.Green;
	DeltaBlue  = Bottom.Blue - Top.Blue;
	DeltaAlpha = Bottom.Alpha - Top.Alpha;

	d1(KPrintF("%s/%s/%ld: DeltaRed=%ld  DeltaGreen=%ld  DeltaBlue=%ld\n", __FILE__, __FUNC__, __LINE__, DeltaRed, DeltaGreen, DeltaBlue));

	StepRed = DeltaRed / Height;
	RemRed  = DeltaRed % Height;

	d1(KPrintF("%s/%s/%ld: StepRed=%ld  RemRed=%ld\n", __FILE__, __FUNC__, __LINE__, StepRed, RemRed));

	StepGreen = DeltaGreen / Height;
	RemGreen  = DeltaGreen % Height;

	d1(KPrintF("%s/%s/%ld: StepGreen=%ld  RemGreen=%ld\n", __FILE__, __FUNC__, __LINE__, StepGreen, RemGreen));

	StepBlue = DeltaBlue / Height;
	RemBlue  = DeltaBlue % Height;

	d1(KPrintF("%s/%s/%ld: StepBlue=%ld  RemBlue=%ld\n", __FILE__, __FUNC__, __LINE__, StepBlue, RemBlue));

	StepAlpha = DeltaAlpha / Height;
	RemAlpha  = DeltaAlpha % Height;

	d1(KPrintF("%s/%s/%ld: StepAlpha=%ld  RemAlpha=%ld\n", __FILE__, __FUNC__, __LINE__, StepAlpha, RemAlpha));

	Source = AllocARGB(1, Height, ScalosGfxBase);
	if (Source)
		{
		LONG y;
		struct ARGB *pSrc = Source;
		LONG AccumulatedDiffRed = 0;
		LONG AccumulatedDiffGreen = 0;
		LONG AccumulatedDiffBlue = 0;
		LONG AccumulatedDiffAlpha = 0;

		// fill Source with RGB Top-to-Bottom gradient
		for (y = 0; y < Height; y++)
			{
			*pSrc = Top;

			Top.Red += StepRed;
			AccumulatedDiffRed += RemRed;
			if (AccumulatedDiffRed >= Height)
				{
				Top.Red++;
				AccumulatedDiffRed -= Height;
				}
			else if (AccumulatedDiffRed <= -Height)
				{
				Top.Red--;
				AccumulatedDiffRed += Height;
				}

			Top.Green += StepGreen;
			AccumulatedDiffGreen += RemGreen;
			if (AccumulatedDiffGreen >= Height)
				{
				Top.Green++;
				AccumulatedDiffGreen -= Height;
				}
			else if (AccumulatedDiffGreen <= -Height)
				{
				Top.Green--;
				AccumulatedDiffGreen += Height;
				}

			Top.Blue += StepBlue;
			AccumulatedDiffBlue += RemBlue;
			if (AccumulatedDiffBlue >= Height)
				{
				Top.Blue++;
				AccumulatedDiffBlue -= Height;
				}
			else if (AccumulatedDiffBlue <= -Height)
				{
				Top.Blue--;
				AccumulatedDiffBlue += Height;
				}

			Top.Alpha += StepAlpha;
			AccumulatedDiffAlpha += RemAlpha;
			if (AccumulatedDiffAlpha >= Height)
				{
				Top.Alpha++;
				AccumulatedDiffAlpha -= Height;
				}
			else if (AccumulatedDiffAlpha <= -Height)
				{
				Top.Alpha--;
				AccumulatedDiffAlpha += Height;
				}

			pSrc++;
			}
		d1(KPrintF("%s/%s/%ld: Top   Red=%3ld Green=%2ld Blue=%3ld\n", __FILE__, __FUNC__, __LINE__, Top.Red, Top.Green, Top.Blue));
		}

	return Source;
}

// ----------------------------------------------------------

static struct ARGB *CreateGradientHorizontal(LONG Width, LONG Height, struct ARGB Left,
	struct ARGB Right, struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *Source;
	LONG DeltaRed, DeltaGreen, DeltaBlue, DeltaAlpha;
	LONG StepRed, StepGreen, StepBlue, StepAlpha;
	LONG RemRed, RemGreen, RemBlue, RemAlpha;

	d1(KPrintF("%s/%s/%ld: Left   Red=%3ld Green=%2ld Blue=%3ld\n", __FILE__, __FUNC__, __LINE__, Left.Red, Left.Green, Left.Blue));
	d1(KPrintF("%s/%s/%ld: Right  Red=%3ld Green=%2ld Blue=%3ld\n", __FILE__, __FUNC__, __LINE__, Right.Red, Right.Green, Right.Blue));

	DeltaRed   = Right.Red - Left.Red;
	DeltaGreen = Right.Green - Left.Green;
	DeltaBlue  = Right.Blue - Left.Blue;
	DeltaAlpha  = Right.Alpha - Left.Alpha;

	d1(KPrintF("%s/%s/%ld: DeltaRed=%ld  DeltaGreen=%ld  DeltaBlue=%ld\n", __FILE__, __FUNC__, __LINE__, DeltaRed, DeltaGreen, DeltaBlue));

	StepRed = DeltaRed / Width;
	RemRed  = DeltaRed % Width;

	d1(KPrintF("%s/%s/%ld: StepRed=%ld  RemRed=%ld\n", __FILE__, __FUNC__, __LINE__, StepRed, RemRed));

	StepGreen = DeltaGreen / Width;
	RemGreen  = DeltaGreen % Width;

	d1(KPrintF("%s/%s/%ld: StepGreen=%ld  RemGreen=%ld\n", __FILE__, __FUNC__, __LINE__, StepGreen, RemGreen));

	StepBlue = DeltaBlue / Width;
	RemBlue  = DeltaBlue % Width;

	d1(KPrintF("%s/%s/%ld: StepBlue=%ld  RemBlue=%ld\n", __FILE__, __FUNC__, __LINE__, StepBlue, RemBlue));

	StepAlpha = DeltaAlpha / Width;
	RemAlpha  = DeltaAlpha % Width;

	d1(KPrintF("%s/%s/%ld: StepAlpha=%ld  RemAlpha=%ld\n", __FILE__, __FUNC__, __LINE__, StepAlpha, RemAlpha));

	Source = AllocARGB(Width, 1, ScalosGfxBase);
	if (Source)
		{
		LONG x;
		struct ARGB *pSrc = Source;
		LONG AccumulatedDiffRed = 0;
		LONG AccumulatedDiffGreen = 0;
		LONG AccumulatedDiffBlue = 0;
		LONG AccumulatedDiffAlpha = 0;

		// fill Source with RGB left-to-right gradient
		for (x = 0; x < Width; x++)
			{
			*pSrc = Left;

			d1(KPrintF("%s/%s/%ld: x=%ld  RGB=%ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, x, Left.Red, Left.Green, Left.Blue));

			Left.Red += StepRed;
			AccumulatedDiffRed += RemRed;
			if (AccumulatedDiffRed >= Width)
				{
				Left.Red++;
				AccumulatedDiffRed -= Width;
				}
			else if (AccumulatedDiffRed <= -Width)
				{
				Left.Red--;
				AccumulatedDiffRed += Width;
				}

			Left.Green += StepGreen;
			AccumulatedDiffGreen += RemGreen;
			if (AccumulatedDiffGreen >= Width)
				{
				Left.Green++;
				AccumulatedDiffGreen -= Width;
				}
			else if (AccumulatedDiffGreen <= -Width)
				{
				Left.Green--;
				AccumulatedDiffGreen += Width;
				}

			Left.Blue += StepBlue;
			AccumulatedDiffBlue += RemBlue;
			if (AccumulatedDiffBlue >= Width)
				{
				Left.Blue++;
				AccumulatedDiffBlue -= Width;
				}
			else if (AccumulatedDiffBlue <= -Width)
				{
				Left.Blue--;
				AccumulatedDiffBlue += Width;
				}

			Left.Alpha += StepAlpha;
			AccumulatedDiffAlpha += RemAlpha;
			if (AccumulatedDiffAlpha >= Width)
				{
				Left.Alpha++;
				AccumulatedDiffAlpha -= Width;
				}
			else if (AccumulatedDiffAlpha <= -Width)
				{
				Left.Alpha--;
				AccumulatedDiffAlpha += Width;
				}

			pSrc++;
			}
		d1(KPrintF("%s/%s/%ld: Left   Red=%3ld Green=%2ld Blue=%3ld\n", __FILE__, __FUNC__, __LINE__, Left.Red, Left.Green, Left.Blue));
		}

	return Source;
}

//-----------------------------------------------------------------------

#ifndef __amigaos4__
static BOOL DrawGradientVerticalRastPort(struct RastPort *rp, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Top, struct ARGB Bottom, struct ScalosGfxBase *ScalosGfxBase)
{
	BOOL Success = FALSE;

	if (CyberGfxBase && GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
		{
		struct ARGB *Source;

		Source = CreateGradientVertical(Width, Height, Top, Bottom, ScalosGfxBase);
		if (Source)
			{
			// blit pre-generated gradient column by column into dest bitmap
			ULONG x;

			for (x = 0; x < Width; x++)
				{
				WritePixelArray(Source,
					0, 0,
					sizeof(struct ARGB) * 1,
					rp,
					x + xStart, yStart,
					1, Height,
					RECTFMT_ARGB);
				}

			FreeARGB(&Source, ScalosGfxBase);

			Success = TRUE;
			}
		}

	return Success;
}

// ----------------------------------------------------------

static BOOL DrawGradientHorizontalRastPort(struct RastPort *rp, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Left, struct ARGB Right, struct ScalosGfxBase *ScalosGfxBase)
{
	BOOL Success = FALSE;

	if (CyberGfxBase && GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
		{
		struct ARGB *Source;

		Source = CreateGradientHorizontal(Width, Height, Left, Right, ScalosGfxBase);
		if (Source)
			{
			ULONG y;

			// blit pre-generated gradient line by line into dest bitmap
			for (y = 0; y < Height; y++)
				{
				WritePixelArray(Source,
					0, 0,
					sizeof(struct ARGB) * Width,
					rp,
					xStart, y + yStart,
					Width, 1,
					RECTFMT_ARGB);
				}

			FreeARGB(&Source, ScalosGfxBase);
			Success = TRUE;
			}
		}

	return Success;
}
#endif //__amigaos4__

//-----------------------------------------------------------------------

static BOOL DrawGradientVertical(struct ARGBHeader *argbh, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Top, struct ARGB Bottom, struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *Source;
	BOOL Success = FALSE;

	Source = CreateGradientVertical(Width, Height, Top, Bottom, ScalosGfxBase);
	if (Source)
		{
		// blit pre-generated gradient column by column into dest
		ULONG x;
		struct ARGBHeader src;

		src.argb_Width = 1;
		src.argb_Height = Height;
		src.argb_ImageData = Source;

		for (x = 0; x < Width; x++)
			{
			BlitARGB(argbh, &src,
				x + xStart, yStart,
				0, 0,
				1, Height);
			}

		FreeARGB(&Source, ScalosGfxBase);

		Success = TRUE;
		}

	return Success;
}

// ----------------------------------------------------------

static BOOL DrawGradientHorizontal(struct ARGBHeader *argbh, LONG xStart, LONG yStart,
	LONG Width, LONG Height, struct ARGB Left, struct ARGB Right, struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *Source;
	BOOL Success = FALSE;

	Source = CreateGradientHorizontal(Width, Height, Left, Right, ScalosGfxBase);
	if (Source)
		{
		ULONG y;
		struct ARGBHeader src;

		src.argb_Width = Width;
		src.argb_Height = 1;
		src.argb_ImageData = Source;

		// blit pre-generated gradient line by line into dest
		for (y = 0; y < Height; y++)
			{
			BlitARGB(argbh, &src,
				xStart, y + yStart,
				0, 0,
				Width, 1);
			}

		FreeARGB(&Source, ScalosGfxBase);
		Success = TRUE;
		}

	return Success;
}

//-----------------------------------------------------------------------

static void DrawRGBPixelWeighted(struct ARGBHeader *argbh,
	WORD x, WORD y, const struct ARGB *color, UBYTE Weight)
{
	struct ARGB *argb = &argbh->argb_ImageData[y * argbh->argb_Width + x];
	struct ARGB b, r;
	double grayl, grayb;

	d1(KPrintF("%s/%s/%ld: Weight=%ld\n", __FILE__, __FUNC__, __LINE__, Weight));

	b = *argb;

	Weight = 255 - Weight;

	grayl = color->Red * 0.299 + color->Green * 0.587 + color->Blue * 0.114,
	grayb = b.Red * 0.299 + b.Green * 0.587 + b.Blue * 0.114;

	r.Red   = ( b.Red > color->Red ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( b.Red - color->Red ) + color->Red ) ) :
		( ( UBYTE )( ( ( double )( grayl<grayb?Weight:(Weight ^ 255)) )
		/ 255.0 * ( color->Red - b.Red ) + b.Red ) ) );
	r.Green = ( b.Green > color->Green ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( b.Green - color->Green ) + color->Green ) ) :
		( ( UBYTE )( ( ( double )( grayl < grayb ? Weight : (Weight ^ 255)) )
		/ 255.0 * ( color->Green - b.Green ) + b.Green ) ) );
	r.Blue  = ( b.Blue > color->Blue ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( b.Blue - color->Blue ) + color->Blue ) ) :
		( ( UBYTE )( ( ( double )( grayl <grayb ? Weight : (Weight ^ 255)) )
			/ 255.0 * ( color->Blue - b.Blue ) + b.Blue ) ) );
	r.Alpha  = ( b.Alpha > color->Alpha ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( b.Alpha - color->Alpha ) + color->Alpha ) ) :
		( ( UBYTE )( ( ( double )( grayl <grayb ? Weight : (Weight ^ 255)) )
			/ 255.0 * ( color->Alpha - b.Alpha ) + b.Alpha ) ) );
	*argb = r;
}

//-----------------------------------------------------------------------

// Draw an antialiased ellipse
void DrawARGBEllipse(struct ARGBHeader *argbh,
	LONG xc, LONG yc, LONG rx, LONG ry,
	WORD Segment, struct ARGB rgbColor1, struct ARGB rgbColor2)
{
        int i;
        int a2, b2, ds, dt, dxt, t, s, d;
        WORD x, y, xs, ys, dyt, xx, yy, xc2, yc2;
	double cp;
        UBYTE weight, iweight;

	// Sanity check for 0 radii
	if ((rx < 0) || (ry < 0))
		return;

	// Special case for rx=0 - draw a vertical line
	if (rx == 0)
		{
		DrawLine(argbh, xc, yc - ry, xc, yc + ry, rgbColor1);
		return;
		}
	// Special case for ry=0 - draw a horizontal line
	if (ry == 0)
		{
		DrawLine(argbh, xc - rx, yc, xc + rx, yc, rgbColor1);
		return;
		}

        /* Variable setup */
        a2 = rx * rx;
        b2 = ry * ry;

        ds = 2 * a2;
        dt = 2 * b2;

        xc2 = 2 * xc;
        yc2 = 2 * yc;

	dxt = (int) (a2 / sqrt((double) (a2 + b2)));

        t = 0;
        s = -2 * a2 * ry;
        d = 0;

        x = xc;
        y = yc - ry;

	// counting of ellipse octants:
	//    8  1
	//  7      2
	//  6      3
	//    5  4

        /* Draw */

	for (i = 1; i <= dxt; i++)
		{
		x--;
		d += t - b2;

		if (d >= 0)
			ys = y - 1;
		else if ((d - s - a2) > 0)
			{
		        if ((2 * d - s - a2) >= 0)
				ys = y + 1;
			else
				{
				ys = y;
				y++;
				d -= s + a2;
				s += ds;
				}
			}
		else
			{
		        y++;
		        ys = y + 1;
		        d -= s + a2;
		        s += ds;
			}

		t -= dt;

		/* Calculate alpha */
		if (s != 0.0)
			{
			cp = (double) abs(d) / (double) abs(s);
			if (cp > 1.0)
				{
				cp = 1.0;
				}
			}
		else
			{
			cp = 1.0;
			}

		/* Calculate weights */
		weight = (UBYTE) (cp * 255);
		iweight = 255 - weight;

		/* Upper half */
		xx = xc2 - x;
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeighted(argbh, x, y, &rgbColor2, iweight);	   // octand 8
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeighted(argbh, xx, y, &rgbColor1, iweight);   // octand 1

		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeighted(argbh, x, ys, &rgbColor2, weight);	   // octand 8
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeighted(argbh, xx, ys, &rgbColor1, weight);   // octand 1

		/* Lower half */
		yy = yc2 - y;
		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeighted(argbh, x, yy, &rgbColor1, iweight);   // octand 5
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeighted(argbh, xx, yy, &rgbColor2, iweight);  // octand 4

		yy = yc2 - ys;
		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeighted(argbh, x, yy, &rgbColor1, weight);	   // octand 5
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeighted(argbh, xx, yy, &rgbColor2, weight);   // octand 4
		}

	dyt = abs(y - yc);

	for (i = 1; i <= dyt; i++)
		{
		y++;
		d -= s + a2;

		if (d <= 0)
		    xs = x + 1;
		else if ((d + t - b2) < 0)
			{
			if ((2 * d + t - b2) <= 0)
				xs = x - 1;
			else
				{
				xs = x;
				x--;
				d += t - b2;
				t -= dt;
				}
			}
		else
			{
		        x--;
		        xs = x - 1;
		        d += t - b2;
		        t -= dt;
			}

		s += ds;

		/* Calculate alpha */
		if (t != 0.0)
			{
			cp = (double) abs(d) / (double) abs(t);
			if (cp > 1.0)
				{
				cp = 1.0;
				}
			}
		else
			{
			cp = 1.0;
			}

		/* Calculate weight */
		weight = (UBYTE) (cp * 255);
		iweight = 255 - weight;

		/* Left half */
		yy = yc2 - y;
		xx = xc2 - x;
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeighted(argbh, x, y, &rgbColor1, iweight);	   // octand 7
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeighted(argbh, xx, y, &rgbColor2, iweight);   // octand 2

		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeighted(argbh, x, yy, &rgbColor2, iweight);   // octand 6
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeighted(argbh, xx, yy, &rgbColor1, iweight);  // octand 3

		/* Right half */
		xx = 2 * xc - xs;
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeighted(argbh, xs, y, &rgbColor1, weight);    // octand 7
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeighted(argbh, xx, y, &rgbColor2, weight);    // octand 2?

		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeighted(argbh, xs, yy, &rgbColor2, weight);   // octand 6?
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeighted(argbh, xx, yy, &rgbColor1, weight);   // octand 3?
		}
}

//-----------------------------------------------------------------------

void DrawLine(struct ARGBHeader *argbh, LONG X0, LONG Y0, LONG X1, LONG Y1, struct ARGB Color32)
{
	int XDir, DeltaX, DeltaY;
	unsigned short ErrorAdj;
	unsigned short ErrorAccTemp, Weighting;
	unsigned short ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
	struct ARGB l;
	double grayl;

	l = Color32;
	grayl = Color32.Red * 0.299 + Color32.Green * 0.587 + Color32.Blue * 0.114;

	d1(KPrintF("%s/%s/%ld: r=%ld g=%ld b=%ld  Color32=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, l.Blue, l.Green, l.Blue, Color32));

	/* Make sure the line runs top to bottom */
	if (Y0 > Y1)
		{
		int Temp = Y0;

		Y0 = Y1;
                Y1 = Temp;
		Temp = X0;
		X0 = X1;
                X1 = Temp;
		}

	DeltaX = X1 - X0;
	DeltaY = Y1 - Y0;

	if( DeltaX >= 0 )
		{
		XDir = 1;
		}
	else
		{
		XDir   = -1;
		DeltaX = - DeltaX; /* make DeltaX positive */
		}

	/* Special-case horizontal, vertical, and diagonal lines, which
	require no weighting because they go right through the center of
	every pixel */
	if (0 == DeltaX)
		{
		do      {
			Y0++;
			argbh->argb_ImageData[Y0 * argbh->argb_Width + X0] = Color32;
			} while (--DeltaY != 0);
		return;
		}
	if (0 == DeltaY)
		{
		do      {
			X0 += XDir;
			argbh->argb_ImageData[Y0 * argbh->argb_Width + X0] = Color32;
			} while (--DeltaX != 0);
		return;
		}

	/* Draw the initial pixel, which is always exactly intersected by
	  the line and so needs no weighting */
	argbh->argb_ImageData[Y0 * argbh->argb_Width + X0] = Color32;
#if 0
	if (DeltaX == DeltaY)
		{
		/* Diagonal line */
		do      {
			X0 += XDir;
			Y0++;
			argbh->argb_ImageData[Y0 * argbh->argb_Width + X0] = Color32;
			} while (--DeltaY != 0);
		return;
		}
#endif

	/* Line is not horizontal, diagonal, or vertical */

	d1(KPrintF("%s/%s/%ld: DeltaX=%ld  DeltaY=%ld\n", __FILE__, __FUNC__, __LINE__, DeltaX, DeltaY));

	/* Is this an X-major or Y-major line? */
	if (DeltaY > DeltaX)
		{
		/* Y-major line; calculate 16-bit fixed-point fractional part of a
		pixel that X advances each time Y advances 1 pixel, truncating the
		result so that we won't overrun the endpoint along the X axis */

		ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;

		/* Draw all pixels other than the first and last */
		while (--DeltaY)
			{
			struct ARGB b, r;
			double grayb;

	                ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
	                ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
			if (ErrorAcc <= ErrorAccTemp)
				{
				/* The error accumulator turned over, so advance the X coord */
				X0 += XDir;
				}
			Y0++; 	/* Y-major, so always advance Y */
				/* The IntensityBits most significant bits of ErrorAcc give us the
				intensity weighting for this pixel, and the complement of the
				weighting for the paired pixel */
	                Weighting = ErrorAcc >> 8;

//			ASSERT( Weighting < 256 );
//			ASSERT( ( Weighting ^ 255 ) < 256 );

			b = argbh->argb_ImageData[Y0 * argbh->argb_Width + X0];
			grayb = b.Red * 0.299 + b.Green * 0.587 + b.Blue * 0.114;

			r.Red   = ( b.Red > l.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				  (Weighting ^ 255)) ) / 255.0 * ( b.Red - l.Red ) + l.Red ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( b.Blue - b.Red ) + b.Red ) ) );
			r.Green = ( b.Green > l.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
			          (Weighting ^ 255)) ) / 255.0 * ( b.Green - l.Green ) + l.Green ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( l.Green - b.Green ) + b.Green ) ) );
			r.Blue  = ( b.Blue > l.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
			          (Weighting ^ 255)) ) / 255.0 * ( b.Blue - l.Blue ) + l.Blue ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( l.Blue - b.Blue ) + b.Blue ) ) );
			r.Alpha  = ( b.Alpha > l.Alpha ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
			          (Weighting ^ 255)) ) / 255.0 * ( b.Alpha - l.Alpha ) + l.Alpha ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( l.Alpha - b.Alpha ) + b.Alpha ) ) );

			argbh->argb_ImageData[Y0 * argbh->argb_Width + X0] = r;
			d1(KPrintF("%s/%s/%ld: (x=%ld,Y=%ld)  r=%ld g=%ld b=%ld a=%ld\n", __FILE__, __FUNC__, __LINE__, X0, Y0, r.Red, r.Green, r.Blue, r.Alpha));

			b = argbh->argb_ImageData[Y0 * argbh->argb_Width + X0 + XDir];
	                grayb = b.Red * 0.299 + b.Green * 0.587 + b.Blue * 0.114;

			r.Red   = ( b.Red > l.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( b.Red - l.Red ) + l.Red ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( l.Red - b.Red ) + b.Red ) ) );
			r.Green = ( b.Green > l.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( b.Green - l.Green ) + l.Green ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( l.Green - b.Green ) + b.Green ) ) );
			r.Blue  = ( b.Blue > l.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( b.Blue - l.Blue ) + l.Blue ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( l.Blue - b.Blue ) + b.Blue ) ) );
			r.Alpha  = ( b.Alpha > l.Alpha ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( b.Alpha - l.Alpha ) + l.Alpha ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( l.Alpha - b.Alpha ) + b.Alpha ) ) );

			argbh->argb_ImageData[Y0 * argbh->argb_Width + X0 + XDir] = r;
			d1(KPrintF("%s/%s/%ld: (x=%ld,Y=%ld)  r=%ld g=%ld b=%ld a=%ld\n", __FILE__, __FUNC__, __LINE__, X0+XDir, Y0, r.Red, r.Green, r.Blue, r.Alpha));
			}
		}
	else
		{
		/* It's an X-major line; calculate 16-bit fixed-point fractional part of a
		pixel that Y advances each time X advances 1 pixel, truncating the
		result to avoid overrunning the endpoint along the X axis */

		ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;

		/* Draw all pixels other than the first and last */

		while (--DeltaX)
			{
			struct ARGB b, r;
			double grayb;

		        ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
		        ErrorAcc += ErrorAdj;      /* calculate error for next pixel */

			if (ErrorAcc <= ErrorAccTemp)
				{
				/* The error accumulator turned over, so advance the Y coord */
				Y0++;
				}

			X0 += XDir; 	/* X-major, so always advance X */
					/* The IntensityBits most significant bits of ErrorAcc give us the
					intensity weighting for this pixel, and the complement of the
					weighting for the paired pixel */

		        Weighting = ErrorAcc >> 8;

//		  	ASSERT( Weighting < 256 );
//		  	ASSERT( ( Weighting ^ 255 ) < 256 );

			b = argbh->argb_ImageData[Y0 * argbh->argb_Width + X0];
			grayb = b.Red * 0.299 + b.Green * 0.587 + b.Blue * 0.114;

			r.Red   = ( b.Red > l.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( b.Red - l.Red ) + l.Red ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( l.Red - b.Red ) + b.Red ) ) );
			r.Green = ( b.Green > l.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( b.Green - l.Green ) + l.Green ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( l.Green - b.Green ) + b.Green ) ) );
			r.Blue  = ( b.Blue > l.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( b.Blue - l.Blue ) + l.Blue ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( l.Blue - b.Blue ) + b.Blue ) ) );
			r.Alpha  = ( b.Alpha > l.Alpha ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( b.Alpha - l.Alpha ) + l.Alpha ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( l.Alpha - b.Alpha ) + b.Alpha ) ) );

			argbh->argb_ImageData[Y0 * argbh->argb_Width + X0] = r;
			d1(KPrintF("%s/%s/%ld: (x=%ld,Y=%ld)  r=%ld g=%ld b=%ld a=%ld\n", __FILE__, __FUNC__, __LINE__, X0, Y0, r.Red, r.Green, r.Blue, r.Alpha));

			b = argbh->argb_ImageData[(Y0 + 1) * argbh->argb_Width + X0];
		        grayb = b.Red * 0.299 + b.Green * 0.587 + b.Blue * 0.114;

			r.Red   = ( b.Red > l.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( b.Red - l.Red ) + l.Red ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( l.Red - b.Red ) + b.Red ) ) );
			r.Green = ( b.Green > l.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( b.Green - l.Green ) + l.Green ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( l.Green - b.Green ) + b.Green ) ) );
			r.Blue  = ( b.Blue > l.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( b.Blue - l.Blue ) + l.Blue ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( l.Blue - b.Blue ) + b.Blue ) ) );
			r.Alpha  = ( b.Alpha > l.Alpha ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( b.Alpha - l.Alpha ) + l.Alpha ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( l.Alpha - b.Alpha ) + b.Alpha ) ) );

			argbh->argb_ImageData[(Y0 + 1) * argbh->argb_Width + X0] = r;
			d1(KPrintF("%s/%s/%ld: (x=%ld,Y=%ld)  r=%ld g=%ld b=%ld a=%ld\n", __FILE__, __FUNC__, __LINE__, X0, Y0+1, r.Red, r.Green, r.Blue, r.Alpha));
			}
		}

	/* Draw the final pixel, which is always exactly intersected by the line
	and so needs no weighting */
	argbh->argb_ImageData[Y1 * argbh->argb_Width + X1] = Color32;
}

//-----------------------------------------------------------------------

// Draw an antialiased ellipse into a RastPort
void DrawARGBEllipseRastPort(struct RastPort *rp,
	LONG xc, LONG yc, LONG rx, LONG ry,
	WORD Segment, struct ARGB rgbColor1, struct ARGB rgbColor2)
{
        int i;
        int a2, b2, ds, dt, dxt, t, s, d;
        WORD x, y, xs, ys, dyt, xx, yy, xc2, yc2;
	double cp;
        UBYTE weight, iweight;

	if ((NULL == CyberGfxBase) || !GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
		return;

	// Sanity check for 0 radii
	if ((rx < 0) || (ry < 0))
		return;

	// Special case for rx=0 - draw a vertical line
	if (rx == 0)
		{
		//SetAPen(rp, color1);
		Move(rp, xc, yc - ry);
		Draw(rp, xc, yc + ry);
		return;
		}
	// Special case for ry=0 - draw a horizontal line
	if (ry == 0)
		{
		//SetAPen(rp, color1);
		Move(rp, xc - rx, yc);
		Draw(rp, xc + rx, yc);
		return;
		}

        /* Variable setup */
        a2 = rx * rx;
        b2 = ry * ry;

        ds = 2 * a2;
        dt = 2 * b2;

        xc2 = 2 * xc;
        yc2 = 2 * yc;

	dxt = (int) (a2 / sqrt((double) (a2 + b2)));

        t = 0;
        s = -2 * a2 * ry;
        d = 0;

        x = xc;
        y = yc - ry;

	// counting of ellipse octants:
	//    8  1
	//  7      2
	//  6      3
	//    5  4

        /* Draw */

	for (i = 1; i <= dxt; i++)
		{
		x--;
		d += t - b2;

		if (d >= 0)
			ys = y - 1;
		else if ((d - s - a2) > 0)
			{
		        if ((2 * d - s - a2) >= 0)
				ys = y + 1;
			else
				{
				ys = y;
				y++;
				d -= s + a2;
				s += ds;
				}
			}
		else
			{
		        y++;
		        ys = y + 1;
		        d -= s + a2;
		        s += ds;
			}

		t -= dt;

		/* Calculate alpha */
		if (s != 0.0)
			{
			cp = (double) abs(d) / (double) abs(s);
			if (cp > 1.0)
				{
				cp = 1.0;
				}
			}
		else
			{
			cp = 1.0;
			}

		/* Calculate weights */
		weight = (UBYTE) (cp * 255);
		iweight = 255 - weight;

		/* Upper half */
		xx = xc2 - x;
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeightedRastPort(rp, x, y, &rgbColor2, iweight);	// octand 8
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, y, &rgbColor1, iweight);	// octand 1

		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeightedRastPort(rp, x, ys, &rgbColor2, weight);	// octand 8
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, ys, &rgbColor1, weight);	// octand 1

		/* Lower half */
		yy = yc2 - y;
		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeightedRastPort(rp, x, yy, &rgbColor1, iweight);	// octand 5
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, yy, &rgbColor2, iweight);	// octand 4

		yy = yc2 - ys;
		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeightedRastPort(rp, x, yy, &rgbColor1, weight);	// octand 5
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, yy, &rgbColor2, weight);	// octand 4
		}

	dyt = abs(y - yc);

	for (i = 1; i <= dyt; i++)
		{
		y++;
		d -= s + a2;

		if (d <= 0)
		    xs = x + 1;
		else if ((d + t - b2) < 0)
			{
			if ((2 * d + t - b2) <= 0)
				xs = x - 1;
			else
				{
				xs = x;
				x--;
				d += t - b2;
				t -= dt;
				}
			}
		else
			{
		        x--;
		        xs = x - 1;
		        d += t - b2;
		        t -= dt;
			}

		s += ds;

		/* Calculate alpha */
		if (t != 0.0)
			{
			cp = (double) abs(d) / (double) abs(t);
			if (cp > 1.0)
				{
				cp = 1.0;
				}
			}
		else
			{
			cp = 1.0;
			}

		/* Calculate weight */
		weight = (UBYTE) (cp * 255);
		iweight = 255 - weight;

		/* Left half */
		yy = yc2 - y;
		xx = xc2 - x;
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeightedRastPort(rp, x, y, &rgbColor1, iweight);	// octand 7
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, y, &rgbColor2, iweight);	// octand 2

		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeightedRastPort(rp, x, yy, &rgbColor2, iweight);	// octand 6
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, yy, &rgbColor1, iweight); 	// octand 3

		/* Right half */
		xx = 2 * xc - xs;
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
			DrawRGBPixelWeightedRastPort(rp, xs, y, &rgbColor1, weight); 	// octand 7
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, y, &rgbColor2, weight);   	// octand 2?

		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
			DrawRGBPixelWeightedRastPort(rp, xs, yy, &rgbColor2, weight);	// octand 6?
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
			DrawRGBPixelWeightedRastPort(rp, xx, yy, &rgbColor1, weight);	// octand 3?
		}
}

//-----------------------------------------------------------------------

#define	COMBINE_RGB(r,b,g)      (0xff000000 | \
	(((r) & 0xff) << 16) | \
	(((g) & 0xff) << 8) | \
        ((b) & 0xff))
#define	GET_R_BYTE(color)	((UBYTE) ((color) >> 16))
#define	GET_G_BYTE(color)	((UBYTE) ((color) >> 8))
#define	GET_B_BYTE(color)	((UBYTE) (color))

// Draw an antialiased line into a RastPort
void DrawLineRastPort(struct RastPort *rp, LONG X0, LONG Y0, LONG X1, LONG Y1, struct ARGB FgColor)
{
	int XDir, DeltaX, DeltaY;
	unsigned short ErrorAdj;
	unsigned short ErrorAccTemp, Weighting;
	unsigned short ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
	double grayl;
	ULONG Color32;

	if ((NULL == CyberGfxBase) || !GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
		return;

	grayl = FgColor.Red * 0.299 + FgColor.Green * 0.587 + FgColor.Blue * 0.114;
	Color32 = COMBINE_RGB(FgColor.Red, FgColor.Green, FgColor.Blue);

	d1(KPrintF("%s/%s/%ld:  r=%ld g=%ld b=%ld  Color32=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, FgColor.Red, FgColor.Green, FgColor.Blue, Color32));

	/* Make sure the line runs top to bottom */
	if (Y0 > Y1)
		{
		int Temp = Y0;

		Y0 = Y1;
                Y1 = Temp;
		Temp = X0;
		X0 = X1;
                X1 = Temp;
		}

	DeltaX = X1 - X0;
	DeltaY = Y1 - Y0;

	if( DeltaX >= 0 )
		{
		XDir = 1;
		}
	else
		{
		XDir   = -1;
		DeltaX = - DeltaX; /* make DeltaX positive */
		}

	/* Special-case horizontal, vertical, and diagonal lines, which
	require no weighting because they go right through the center of
	every pixel */
	if (DeltaY == 0 || DeltaX == 0)
		{
		Move(rp, X0, Y0);
		Draw(rp, X1, Y1);
		return;
		}

	/* Draw the initial pixel, which is always exactly intersected by
	  the line and so needs no weighting */
	WriteRGBPixel(rp, X0, Y0, Color32);
#if 0
	if (DeltaX == DeltaY)
		{
		/* Diagonal line */
		do      {
			X0 += XDir;
			Y0++;
			WriteRGBPixel(rp, X0, Y0, Color32);
			} while (--DeltaY != 0);
		return;
		}
#endif

	/* Line is not horizontal, diagonal, or vertical */


	/* Is this an X-major or Y-major line? */
	if (DeltaY > DeltaX)
		{
		/* Y-major line; calculate 16-bit fixed-point fractional part of a
		pixel that X advances each time Y advances 1 pixel, truncating the
		result so that we won't overrun the endpoint along the X axis */

		ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;

		/* Draw all pixels other than the first and last */
		while (--DeltaY)
			{
			ULONG clrBackGround;
			UBYTE rb, gb, bb;
			UBYTE rr, gr, br;
			double grayb;

	                ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
	                ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
			if (ErrorAcc <= ErrorAccTemp)
				{
				/* The error accumulator turned over, so advance the X coord */
				X0 += XDir;
				}
			Y0++; 	/* Y-major, so always advance Y */
				/* The IntensityBits most significant bits of ErrorAcc give us the
				intensity weighting for this pixel, and the complement of the
				weighting for the paired pixel */
	                Weighting = ErrorAcc >> 8;

//			  ASSERT( Weighting < 256 );
//			  ASSERT( ( Weighting ^ 255 ) < 256 );

			clrBackGround = ReadRGBPixel(rp, X0, Y0);
			rb = GET_R_BYTE(clrBackGround);
			gb = GET_G_BYTE(clrBackGround);
			bb = GET_B_BYTE(clrBackGround);
			grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

			rr = ( rb > FgColor.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
			          (Weighting ^ 255)) ) / 255.0 * ( rb - FgColor.Red ) + FgColor.Red ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( FgColor.Red - rb ) + rb ) ) );
			gr = ( gb > FgColor.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
			          (Weighting ^ 255)) ) / 255.0 * ( gb - FgColor.Green ) + FgColor.Green ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( FgColor.Green - gb ) + gb ) ) );
			br = ( bb > FgColor.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
			          (Weighting ^ 255)) ) / 255.0 * ( bb - FgColor.Blue ) + FgColor.Blue ) ) :
				  ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
			          / 255.0 * ( FgColor.Blue - bb ) + bb ) ) );
			WriteRGBPixel(rp, X0, Y0, COMBINE_RGB(rr, gr, br));

			clrBackGround = ReadRGBPixel(rp, X0 + XDir, Y0);
			rb = GET_R_BYTE( clrBackGround );
			gb = GET_G_BYTE( clrBackGround );
			bb = GET_B_BYTE( clrBackGround );
	                grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

			rr = ( rb > FgColor.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( rb - FgColor.Red ) + FgColor.Red ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( FgColor.Red - rb ) + rb ) ) );
			gr = ( gb > FgColor.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( gb - FgColor.Green ) + FgColor.Green ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( FgColor.Green - gb ) + gb ) ) );
			br = ( bb > FgColor.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
			    Weighting) ) / 255.0 * ( bb - FgColor.Blue ) + FgColor.Blue ) ) :
			    ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
			    / 255.0 * ( FgColor.Blue - bb ) + bb ) ) );

			WriteRGBPixel(rp, X0 + XDir, Y0, COMBINE_RGB(rr, gr, br));
			}
		}
	else
		{
		/* It's an X-major line; calculate 16-bit fixed-point fractional part of a
		pixel that Y advances each time X advances 1 pixel, truncating the
		result to avoid overrunning the endpoint along the X axis */

		ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;

		/* Draw all pixels other than the first and last */

		while (--DeltaX)
			{
			ULONG clrBackGround;
			UBYTE rb, gb, bb;
			UBYTE rr, gr, br;
			double grayb;

		        ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
		        ErrorAcc += ErrorAdj;      /* calculate error for next pixel */

			if (ErrorAcc <= ErrorAccTemp)
				{
				/* The error accumulator turned over, so advance the Y coord */
				Y0++;
				}

			X0 += XDir; 	/* X-major, so always advance X */
					/* The IntensityBits most significant bits of ErrorAcc give us the
					intensity weighting for this pixel, and the complement of the
					weighting for the paired pixel */

		        Weighting = ErrorAcc >> 8;

	//		  ASSERT( Weighting < 256 );
	//		  ASSERT( ( Weighting ^ 255 ) < 256 );

			clrBackGround = ReadRGBPixel(rp, X0, Y0);
			rb = GET_R_BYTE(clrBackGround);
			gb = GET_G_BYTE(clrBackGround);
			bb = GET_B_BYTE(clrBackGround);
			grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

			rr = ( rb > FgColor.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( rb - FgColor.Red ) + FgColor.Red ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( FgColor.Red - rb ) + rb ) ) );
			gr = ( gb > FgColor.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( gb - FgColor.Green ) + FgColor.Green ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( FgColor.Green - gb ) + gb ) ) );
			br = ( bb > FgColor.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:
				(Weighting ^ 255)) ) / 255.0 * ( bb - FgColor.Blue ) + FgColor.Blue ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) )
				/ 255.0 * ( FgColor.Blue - bb ) + bb ) ) );

			WriteRGBPixel(rp, X0, Y0, COMBINE_RGB(rr, gr, br));

			clrBackGround = ReadRGBPixel(rp, X0, Y0 + 1);
			rb = GET_R_BYTE(clrBackGround);
			gb = GET_G_BYTE(clrBackGround);
			bb = GET_B_BYTE(clrBackGround);
		        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

			rr = ( rb > FgColor.Red ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( rb - FgColor.Red ) + FgColor.Red ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( FgColor.Red - rb ) + rb ) ) );
			gr = ( gb > FgColor.Green ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( gb - FgColor.Green ) + FgColor.Green ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( FgColor.Green - gb ) + gb ) ) );
			br = ( bb > FgColor.Blue ? ( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):
				Weighting) ) / 255.0 * ( bb - FgColor.Blue ) + FgColor.Blue ) ) :
				( ( UBYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) )
				/ 255.0 * ( FgColor.Blue - bb ) + bb ) ) );

			WriteRGBPixel(rp, X0, Y0 + 1, COMBINE_RGB(rr, gr, br));
			}
		}

	/* Draw the final pixel, which is always exactly intersected by the line
	and so needs no weighting */
	WriteRGBPixel(rp, X1, Y1, Color32);
}

//-----------------------------------------------------------------------

static void DrawRGBPixelWeightedRastPort(struct RastPort *rp,
	WORD x, WORD y, const struct ARGB *color, UBYTE Weight)
{
	ULONG clrBackGround;
	UBYTE rb, gb, bb;
	UBYTE rr, gr, br;
	double grayl, grayb;

	d1(KPrintF("%s/%s/%ld: Weight=%ld\n", __FILE__, __FUNC__, __LINE__, Weight));

	clrBackGround = ReadRGBPixel(rp, x, y);
	rb = GET_R_BYTE(clrBackGround);
	gb = GET_G_BYTE(clrBackGround);
	bb = GET_B_BYTE(clrBackGround);

	Weight = 255 - Weight;

	grayl = color->Red * 0.299 + color->Green * 0.587 + color->Blue * 0.114,
	grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;

	rr = ( rb > color->Red ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( rb - color->Red ) + color->Red ) ) :
		( ( UBYTE )( ( ( double )( grayl<grayb?Weight:(Weight ^ 255)) )
		/ 255.0 * ( color->Red - rb ) + rb ) ) );
	gr = ( gb > color->Green ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( gb - color->Green ) + color->Green ) ) :
		( ( UBYTE )( ( ( double )( grayl < grayb ? Weight : (Weight ^ 255)) )
		/ 255.0 * ( color->Green - gb ) + gb ) ) );
	br = ( bb > color->Blue ? ( ( UBYTE )( ( ( double )( grayl < grayb ? Weight:
		(Weight ^ 255)) ) / 255.0 * ( bb - color->Blue ) + color->Blue ) ) :
		( ( UBYTE )( ( ( double )( grayl <grayb ? Weight : (Weight ^ 255)) )
			/ 255.0 * ( color->Blue - bb ) + bb ) ) );

	WriteRGBPixel(rp, x, y, COMBINE_RGB(rr, gr, br));
}

//-----------------------------------------------------------------------

