//
// pmdrawshadow.c - Renders shadows, uses CyberGfx if available.
//
// Copyright (C) 1996 - 2002 Henrik Isaksson
// All Rights Reserved.
//
// $Date$
// $Revision$
//

#include <graphics/view.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>

#include <cybergraphx/cybergraphics.h>
#include <proto/cybergraphics.h>
#include "pmpriv.h"



// PM_DrawShadow - This function draws a shadow in a PM_Window. The shadow
// will be a rectangle defined by x, y - xb, yb (left, top - right, bottom)

struct RGB
	{
	UBYTE Red;
	UBYTE Green;
	UBYTE Blue;
	};

// Precalculated alpha values for smooth dropshadows
static const UBYTE ShadowTable[] =
	{
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 254, 254, 254, 253, 253, 253,
	252, 252, 251, 250, 250, 249, 249, 248,
	247, 246, 246, 245, 244, 243, 242, 241,
	240, 240, 239, 238, 237, 235, 234, 233,
	232, 231, 230, 229, 227, 226, 225, 224,
	222, 221, 220, 218, 217, 216, 214, 213,
	211, 210, 208, 207, 206, 204, 202, 201,
	199, 198, 196, 195, 193, 191, 190, 188,
	187, 185, 183, 182, 180, 178, 177, 175,
	173, 172, 170, 168, 166, 165, 163, 161,
	160, 158, 156, 154, 153, 151, 149, 148,
	146, 144, 142, 141, 139, 137, 136, 134,
	132, 131, 129, 127, 126, 124, 122, 121,
	119, 117, 116, 114, 113, 111, 109, 108,
	106, 105, 103, 102, 100,  99,  97,  96,
	94,  93,  91,  90,  88,  87,  86,  84,
	83,  81,  80,  79,  77,  76,  75,  73,
	72,  71,  70,  68,  67,  66,  65,  64,
	62,  61,  60,  59,  58,  57,  56,  55,
	54,  53,  52,  51,  50,  49,  48,  47,
	46,  45,  44,  43,  42,  41,  40,  39,
	39,  38,  37,  36,  35,  35,  34,  33,
	32,  32,  31,  30,  30,  29,  28,  28,
	27,  26,  26,  25,  25,  24,  23,  23,
	22,  22,  21,  21,  20,  20,  19,  19,
	18,  18,  17,  17,  16,  16,  16,  15,
	15,  14,  14,  14,  13,  13,  13,  12,
	12,  12,  11,  11,  11,  10,  10,  10,
	10,   9,   9,   9,   9,   8,   8,   8,
	8,   7,   7,   7,   7,   7,   6,   6,
	6,   6,   6,   5,   5,   5,   5,   5,
	};

void PM_DrawShadow(struct PM_Window *w, int x, int y, int xb, int yb, UBYTE Type)
{
#ifndef __AROS__
//    	BOOL	realshadow = PM_Prefs->RealShadows;
	BOOL	realshadow = FALSE;

	// Shall we draw a real CGFX shadow?
	do	{
		ULONG depth, iscgfx;
		ULONG y0;
		LONG Width = xb - x + 1;
		LONG Height = yb - y + 1;
		size_t Size = Width * Height;
		struct RGB *SrcRGB;
		struct RGB *pRGB;
		const ULONG MaxAlpha = 150;

		if (!CyberGfx)
			break;
		
		iscgfx = GetCyberMapAttr(w->RPort->BitMap, CYBRMATTR_ISCYBERGFX);
		if (!iscgfx)
			break;

		depth = GetCyberMapAttr(w->RPort->BitMap, CYBRMATTR_DEPTH);
		if (depth <= 8)
			break;

		d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  xb=%ld  yb=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, xb, yb));
		d1(KPrintF("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, Width, Height));
		d1(KPrintF("%s/%s/%ld: Type=%ld\n", __FILE__, __FUNC__, __LINE__, Type));

		SrcRGB = PM_Mem_Alloc(Size * sizeof(struct RGB));

		if (NULL == SrcRGB)
			break;

		// read screen background into SrcRGB
		ReadPixelArray(SrcRGB,
			0, 0,
			Width * sizeof(struct RGB),
			w->RPort,
			x, y,
			Width,
			Height,
			RECTFMT_RGB);

		for (pRGB = SrcRGB, y0 = 0; y0 < Height; y0++)
			{
			ULONG x0;
			struct RGB *pLineRGB = pRGB;
			ULONG fy;
			ULONG  ky = (Height > 1) ? ((255 * y0) / (Height - 1)) : 255;

			if (Type & SHADOWFLAG_TOP)
				fy = ShadowTable[255 - ky];
			else if (Type & SHADOWFLAG_BOTTOM)
				fy = ShadowTable[ky];
			else
				fy = 255;

			for (x0 = 0; x0 < Width; x0++, pLineRGB++)
				{
				ULONG fx;
				ULONG f;
				ULONG kx = (Width > 1) ? ((255 * x0) / (Width - 1)) : 255;

				if (Type & SHADOWFLAG_LEFT)
					fx = ShadowTable[255 - kx];
				else if (Type & SHADOWFLAG_RIGHT)
					fx = ShadowTable[kx];
				else
					fx = 255;

				// calculate (1 - Alpha)
				f = 255 - (fx * fy * MaxAlpha) / (256 * 256);
				d1(KPrintF("%s/%s/%ld:  x=%ld  y=%ld  kx=%ld  ky=%ld  fx=%ld  fy=%ld  f=%ld  Type=%08lx\n", __FILE__, __FUNC__, __LINE__, x0, y0, kx, ky, fx, fy, f, Type));

				pLineRGB->Red   = (pLineRGB->Red   * f) / 256;
				pLineRGB->Green = (pLineRGB->Green * f) / 256;
				pLineRGB->Blue  = (pLineRGB->Blue  * f) / 256;
				}

			pRGB += Width;
			}

		WritePixelArray(SrcRGB,
			0, 0,
			Width * sizeof(struct RGB),
			w->RPort,
			x, y,
			Width,
			Height,
			RECTFMT_RGB);

		PM_Mem_Free(SrcRGB);

		realshadow = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	if (!realshadow)
#endif /* AROS */
		{
		// A "real" shadow could not be drawn, or it was not desired.
		static UWORD	    pat[] = { 0xaaaa, 0x5555 };

		SetAPen(w->RPort, 1); 	    // Should use DRIPen
		SetAfPt(w->RPort, pat, 1);
		PM_RectFill(w, x, y, xb, yb);
		SetAfPt(w->RPort, NULL, 0);
		}
}

