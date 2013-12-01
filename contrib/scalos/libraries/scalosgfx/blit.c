// blit.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/scale.h>
#ifdef __amigaos4__
#include <graphics/rpattr.h>
#include <graphics/GfxBase.h>
#include <graphics/composite.h>
#endif //__amigaos4__
#include <cybergraphx/cybergraphics.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
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

//-----------------------------------------------------------------------

#define	ALPHA_OPAQUE		255

struct ARGBMultData
	{
	struct ARGB amd_Numerator;
	struct ARGB amd_Denominator;
	};

struct BlitARGBAlphaData
	{
	ULONG baa_SrcLeft;
	ULONG baa_SrcTop;
	ULONG baa_DestLeft;
	ULONG baa_DestTop;
	ULONG baa_Transparency;
	const struct ARGB *baa_K;
	const struct ARGBHeader *baa_Src;
	};

struct BlitTransparentKData
	{
	const struct ARGB *btk_K;
	ULONG btk_Width;
	ULONG btk_Height;
	};

struct BlitTransparentData
	{
	struct ARGB *bt_BufferBg;
	ULONG bt_a;
	ULONG bt_mla;
	ULONG bt_Width;
	ULONG bt_Height;
	};

struct BlitTransparentAlphaData
	{
	const UBYTE *bta_Alpha;
	struct ARGB *bta_BufferBg;
	ULONG bta_Transparency;
	ULONG bta_Width;
	ULONG bta_Height;
	};

struct BlitTransparentAlphaKData
	{
	const UBYTE *btak_Alpha;
	struct ARGB *btak_BufferBg;
	const struct ARGB *btak_K;
	ULONG btak_Width;
	ULONG btak_Height;
	};

struct DoHookClipRectMsg
	{
	struct Layer *dhcr_Layer;
	struct Rectangle dhcr_Bounds;
	LONG dhcr_OffsetX;
	LONG dhcr_OffsetY;
	};

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT ARGBRectMultHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitARGBAlphaHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitARGBAlphaKTHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitARGBKTHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitTransparentKHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitTransparentHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitTransparentAlphaHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(ULONG) INTERRUPT BlitTransparentAlphaKHookFunc(struct Hook *hook, Object *o, Msg msg);

//-----------------------------------------------------------------------

void ARGBRectMult(struct RastPort *rp,
	struct ARGB Numerator, struct ARGB Denominator,
	WORD xMin, WORD yMin, WORD xMax, WORD yMax,
	struct ScalosGfxBase *ScalosGfxBase)
{
#ifdef __amigaos4__
	extern struct GfxBase *GfxBase;

	if (GfxBase->LibNode.lib_Version >= 51)
		{
		struct BitMap *SrcBM = NULL;
		struct BitMap *AllocDestBM = NULL;

		d1(KPrintF(__FILE__ "/%s/%ld: xMin=%ld  xMax=%ld  yMin=%ld  yMax=%ld\n", \
			__FUNC__, __LINE__, xMin, xMax, yMin, yMax));
		d1(KPrintF(__FILE__ "/%s/%ld: Layer=%08lx\n", __FUNC__, __LINE__, rp->Layer));

		do	{
			enum enCompositeError rc;
			LONG Width = 1 + xMax - xMin;
			LONG Height = 1 + yMax - yMin;
			float alpha;
			struct RastPort srcRp;
			enum enPDOperator op;

			InitRastPort(&srcRp);
			SrcBM = AllocBitMap(Width, Height,
				32,
				BMF_CLEAR | BMF_SPECIALFMT | SHIFT_PIXFMT(PIXFMT_ARGB32),
				rp->BitMap);
			d1(KPrintF(__FILE__ "/%s/%ld: SrcBM=%08lx\n", __FUNC__, __LINE__, SrcBM));
			if (NULL == SrcBM)
				break;

			srcRp.BitMap = SrcBM;

			if (Numerator.Red > Denominator.Red)
				{
				// brighten
				d1(KPrintF(__FILE__ "/%s/%ld: brighten\n", __FUNC__, __LINE__));

				SetRPAttrs(&srcRp,
					RPTAG_APenColor, 0xffffffff,
					TAG_END);
				RectFill(&srcRp, 0, 0, Width - 1, Height - 1);

				alpha = (float) Numerator.Red / (float) Denominator.Red;

				op = COMPOSITE_Plus;
				}
			else
				{
				// darken
				float f;

				d1(KPrintF(__FILE__ "/%s/%ld: darken\n", __FUNC__, __LINE__));

				if (Denominator.Red)
					f = (float) Numerator.Red / (float) Denominator.Red;
				else
					f = 0.0;

				SetRPAttrs(&srcRp,
					RPTAG_APenColor, 0xff000000,
					TAG_END);
				RectFill(&srcRp, 0, 0, Width - 1, Height - 1);

				if (f)
					alpha = (1.0 - f) / f;
				else
					alpha = 0.0;

				op = COMPOSITE_Src_Over_Dest;
				}

			if (rp->Layer)
				{
				struct RastPort destRp;
				struct BitMap *destBM = AllocDestBM = AllocBitMap(Width, Height,
					32,
					BMF_SPECIALFMT | SHIFT_PIXFMT(PIXFMT_ARGB32),
					rp->BitMap);
				d1(KPrintF(__FILE__ "/%s/%ld: SrcBM=%08lx\n", __FUNC__, __LINE__, SrcBM));
				if (NULL == AllocDestBM)
					break;

				InitRastPort(&destRp);
				destRp.BitMap = destBM;

				ClipBlit(rp,
					xMin, yMin,
					&destRp,
					0, 0,
					Width, Height,
					0xc0);

				// CompositeTagList()
				rc = CompositeTags(op,
					SrcBM,
					destBM,
					COMPTAG_SrcAlpha, COMP_FLOAT_TO_FIX(alpha),
					COMPTAG_SrcX, 0,
					COMPTAG_SrcY, 0,
					COMPTAG_SrcWidth, Width,
					COMPTAG_SrcHeight, Height,
					COMPTAG_Flags, COMPFLAG_SrcAlphaOverride,
					TAG_END);

				ClipBlit(&destRp,
					0, 0,
					rp,
					xMin, yMin,
					Width, Height,
					0xc0);
				}
			else
				{
				// CompositeTagList()
				rc = CompositeTags(op,
					SrcBM,
					rp->BitMap,
					COMPTAG_SrcAlpha, COMP_FLOAT_TO_FIX(alpha),
					COMPTAG_SrcX, 0,
					COMPTAG_SrcY, 0,
					COMPTAG_SrcWidth, Width,
					COMPTAG_SrcHeight, Height,
					COMPTAG_OffsetX, xMin,
					COMPTAG_OffsetY, yMin,
					COMPTAG_Flags, COMPFLAG_SrcAlphaOverride,
					TAG_END);
				}

			d1(KPrintF(__FILE__ "/%s/%ld: rc=%ld\n", __FUNC__, __LINE__, rc));
			} while (0);

		if (SrcBM)
			FreeBitMap(SrcBM);
		if (AllocDestBM)
			FreeBitMap(AllocDestBM);
		}
	else
#endif //__amigaos4__
	if (CyberGfxBase->lib_Version >= 51)
		{
		if (Numerator.Red > Denominator.Red)
			{
			ProcessPixelArray(rp,
				xMin,
				yMin,
				1 + xMax - xMin,
				1 + yMax - yMin,
				POP_BRIGHTEN,
				(256 * Numerator.Red) / Denominator.Red,
				NULL);
			}
		else
			{
			ProcessPixelArray(rp,
				xMin,
				yMin,
				1 + xMax - xMin,
				1 + yMax - yMin,
				POP_DARKEN,
				(256 * Denominator.Red) / Numerator.Red,
				NULL);
			}
		}
	else
		{
		ULONG Width = xMax - xMin + 1;
		struct ARGBMultData amd;
		struct Rectangle ARGBRectMultRect;
		struct Hook ARGBRectMultHook;

		amd.amd_Numerator = Numerator;
		amd.amd_Denominator = Denominator;

		ARGBRectMultRect.MinX = xMin;
		ARGBRectMultRect.MinY = yMin;
		ARGBRectMultRect.MaxX = Width - 1;
		ARGBRectMultRect.MaxY = yMax;

		SETHOOKFUNC(ARGBRectMultHook, ARGBRectMultHookFunc);
		ARGBRectMultHook.h_Data = &amd;

		DoHookClipRects(&ARGBRectMultHook, rp, &ARGBRectMultRect);
		}
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT ARGBRectMultHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct ARGBMultData *amd = (struct ARGBMultData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG y;
		LONG xMin = dhcr->dhcr_Bounds.MinX;
		LONG yMin = dhcr->dhcr_Bounds.MinY;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * yMin;

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[0];
					Green = pLine[1];
					Blue  = pLine[2];
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					pLine[0] = Red;
					pLine[1] = Green;
					pLine[2] = Blue;

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[2];
					Green = pLine[1];
					Blue  = pLine[0];
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					pLine[2] = Red;
					pLine[1] = Green;
					pLine[0] = Blue;

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[1];
					Green = pLine[2];
					Blue  = pLine[3];
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					pLine[1] = Red;
					pLine[2] = Green;
					pLine[3] = Blue;

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB16(pLine16);
					Green = GET_GREEN_RGB16(pLine16);
					Blue  = GET_BLUE_RGB16(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_RGB16(Blue)
						+  SET_GREEN_RGB16(Green)
						+  SET_RED_RGB16(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR16(pLine16);
					Green = GET_GREEN_BGR16(pLine16);
					Blue  = GET_BLUE_BGR16(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_BGR16(Blue)
						+  SET_GREEN_BGR16(Green)
						+  SET_RED_BGR16(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB15(pLine16);
					Green = GET_GREEN_RGB15(pLine16);
					Blue  = GET_BLUE_RGB15(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_RGB15(Blue)
						+  SET_GREEN_RGB15(Green)
						+  SET_RED_RGB15(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR15(pLine16);
					Green = GET_GREEN_BGR15(pLine16);
					Blue  = GET_BLUE_BGR15(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_BGR15(Blue)
						+  SET_GREEN_BGR15(Green)
						+  SET_RED_BGR15(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB15PC(pLine16);
					Green = GET_GREEN_RGB15PC(pLine16);
					Blue  = GET_BLUE_RGB15PC(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_RGB15PC(Blue)
						+  SET_GREEN_RGB15PC(Green)
						+  SET_RED_RGB15PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR15PC(pLine16);
					Green = GET_GREEN_BGR15PC(pLine16);
					Blue  = GET_BLUE_BGR15PC(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_BGR15PC(Blue)
						+  SET_GREEN_BGR15PC(Green)
						+  SET_RED_BGR15PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB16PC(pLine16);
					Green = GET_GREEN_RGB16PC(pLine16);
					Blue  = GET_BLUE_RGB16PC(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_RGB16PC(Blue)
						+  SET_GREEN_RGB16PC(Green)
						+  SET_RED_RGB16PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + xMin * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR16PC(pLine16);
					Green = GET_GREEN_BGR16PC(pLine16);
					Blue  = GET_BLUE_BGR16PC(pLine16);
					Red   = amd->amd_Numerator.Red   * Red   / amd->amd_Denominator.Red;
					Green = amd->amd_Numerator.Green * Green / amd->amd_Denominator.Green;
					Blue  = amd->amd_Numerator.Blue  * Blue  / amd->amd_Denominator.Blue;
					Red = min(255, Red);
					Green = min(255, Green);
					Blue = min(255, Blue);
					*pLine16 = SET_BLUE_BGR16PC(Blue)
						+  SET_GREEN_BGR16PC(Green)
						+  SET_RED_BGR16PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		default:
			break;
			}

		UnLockBitMap(handle);
		}
	else
		{
		kprintf(__FILE__ "/%s/%ld: Can't lock bitmap\n", __FUNC__, __LINE__);
		}

	return 0;
}

//-----------------------------------------------------------------------

// Blit with Alpha from Src into RastPort rp
void BlitARGBAlpha(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct BlitARGBAlphaData baa;
	struct Rectangle BlitARGBAlphaRect;
	struct Hook BlitARGBAlphaHook;

	baa.baa_SrcLeft = SrcLeft;
	baa.baa_SrcTop  = SrcTop;
	baa.baa_DestLeft = DestLeft;
	baa.baa_DestTop  = DestTop;
	baa.baa_Src = SrcH;

	BlitARGBAlphaRect.MinX = BlitARGBAlphaRect.MaxX = DestLeft;
	BlitARGBAlphaRect.MinY = BlitARGBAlphaRect.MaxY = DestTop;
	BlitARGBAlphaRect.MaxX += Width - 1;
	BlitARGBAlphaRect.MaxY += Height - 1;

	SETHOOKFUNC(BlitARGBAlphaHook, BlitARGBAlphaHookFunc);
	BlitARGBAlphaHook.h_Data = &baa;

	DoHookClipRects(&BlitARGBAlphaHook, rp, &BlitARGBAlphaRect);
}

//-----------------------------------------------------------------------

// Blit with Alpha from Src into RastPort rp
// with additional Transparency and Tint "K"

void BlitARGBAlphaKT(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
	const struct ARGB *K, ULONG Transparency,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct BlitARGBAlphaData baa;
	struct Rectangle BlitARGBAlphaKTRect;
	struct Hook BlitARGBAlphaKTHook;

	baa.baa_SrcLeft = SrcLeft;
	baa.baa_SrcTop  = SrcTop;
	baa.baa_DestLeft = DestLeft;
	baa.baa_DestTop  = DestTop;
	baa.baa_Src = SrcH;
	baa.baa_K = K;
	baa.baa_Transparency = Transparency;

	BlitARGBAlphaKTRect.MinX = BlitARGBAlphaKTRect.MaxX = DestLeft;
	BlitARGBAlphaKTRect.MinY = BlitARGBAlphaKTRect.MaxY = DestTop;
	BlitARGBAlphaKTRect.MaxX += Width - 1;
	BlitARGBAlphaKTRect.MaxY += Height - 1;

	SETHOOKFUNC(BlitARGBAlphaKTHook, BlitARGBAlphaKTHookFunc);
	BlitARGBAlphaKTHook.h_Data = &baa;

	DoHookClipRects(&BlitARGBAlphaKTHook, rp, &BlitARGBAlphaKTRect);
}

//-----------------------------------------------------------------------

// Blit without Alpha from Src into RastPort rp
// with additional Transparency and Tint "K"

void BlitARGBKT(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
	const struct ARGB *K, ULONG Transparency,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct BlitARGBAlphaData baa;
	struct Rectangle BlitARGBKTRect;
	struct Hook BlitARGBKTHook;

	baa.baa_SrcLeft = SrcLeft;
	baa.baa_SrcTop  = SrcTop;
	baa.baa_DestLeft = DestLeft;
	baa.baa_DestTop  = DestTop;
	baa.baa_Src = SrcH;
	baa.baa_K = K;
	baa.baa_Transparency = Transparency;

	BlitARGBKTRect.MinX = BlitARGBKTRect.MaxX = DestLeft;
	BlitARGBKTRect.MinY = BlitARGBKTRect.MaxY = DestTop;
	BlitARGBKTRect.MaxX += Width - 1;
	BlitARGBKTRect.MaxY += Height - 1;

	SETHOOKFUNC(BlitARGBKTHook, BlitARGBKTHookFunc);
	BlitARGBKTHook.h_Data = &baa;

	DoHookClipRects(&BlitARGBKTHook, rp, &BlitARGBKTRect);
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitARGBAlphaHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitARGBAlphaData *baa = (struct BlitARGBAlphaData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG y;
		ULONG SrcLeft = baa->baa_SrcLeft + (dhcr->dhcr_OffsetX - baa->baa_DestLeft);
		ULONG SrcTop  = baa->baa_SrcTop  + (dhcr->dhcr_OffsetY - baa->baa_DestTop);
		const struct ARGB *Src = baa->baa_Src->argb_ImageData + SrcTop * baa->baa_Src->argb_Width;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;

		d1(KPrintF(__FILE__ "/%s/%ld: baa_SrcLeft=%ld  baa_SrcTop=%ld\n", __FUNC__, __LINE__, baa->baa_SrcLeft, baa->baa_SrcTop));
		d1(KPrintF(__FILE__ "/%s/%ld: baa_DestLeft=%ld  baa_DestTop=%ld\n", __FUNC__, __LINE__, baa->baa_DestLeft, baa->baa_DestTop));
		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: SrcLeft=%ld  SrcTop=%ld\n", __FUNC__, __LINE__, SrcLeft, SrcTop));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[0] = SrcPtr->Red;
						pLine[1] = SrcPtr->Green;
						pLine[2] = SrcPtr->Blue;
						break;
					default:
						mla = 257 - a;

						pLine[0] = (a * SrcPtr->Red   + mla * pLine[0])   >> 8;
						pLine[1] = (a * SrcPtr->Green + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcPtr->Blue  + mla * pLine[2])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[0] = SrcPtr->Blue;
						pLine[1] = SrcPtr->Green;
						pLine[2] = SrcPtr->Red;
						break;
					default:
						mla = 257 - a;

						pLine[0] = (a * SrcPtr->Blue  + mla * pLine[0])   >> 8;
						pLine[1] = (a * SrcPtr->Green + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcPtr->Red   + mla * pLine[2])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[1] = SrcPtr->Red;
						pLine[2] = SrcPtr->Green;
						pLine[3] = SrcPtr->Blue;
						break;
					default:
						mla = 257 - a;

						pLine[1] = (a * SrcPtr->Red   + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcPtr->Green + mla * pLine[2])   >> 8;
						pLine[3] = (a * SrcPtr->Blue  + mla * pLine[3])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB16(SrcPtr->Blue)
							+  SET_GREEN_RGB16(SrcPtr->Green)
							+  SET_RED_RGB16(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB16(pLine16);
						Green = GET_GREEN_RGB16(pLine16);
						Blue  = GET_BLUE_RGB16(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB16(Blue)
							+  SET_GREEN_RGB16(Green)
							+  SET_RED_RGB16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR16(SrcPtr->Blue)
							+  SET_GREEN_BGR16(SrcPtr->Green)
							+  SET_RED_BGR16(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;
		
						Blue   = GET_BLUE_BGR16(pLine16);
						Green  = GET_GREEN_BGR16(pLine16);
						Red    = GET_RED_BGR16(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR16(Blue)
							+  SET_GREEN_BGR16(Green)
							+  SET_RED_BGR16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB15(SrcPtr->Blue)
							+  SET_GREEN_RGB15(SrcPtr->Green)
							+  SET_RED_RGB15(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;
		
						Red   = GET_RED_RGB15(pLine16);
						Green = GET_GREEN_RGB15(pLine16);
						Blue  = GET_BLUE_RGB15(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB15(Blue)
							+  SET_GREEN_RGB15(Green)
							+  SET_RED_RGB15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR15(SrcPtr->Blue)
							+  SET_GREEN_BGR15(SrcPtr->Green)
							+  SET_RED_BGR15(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR15(pLine16);
						Green = GET_GREEN_BGR15(pLine16);
						Blue  = GET_BLUE_BGR15(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR15(Blue)
							+  SET_GREEN_BGR15(Green)
							+  SET_RED_BGR15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB15PC(SrcPtr->Blue)
							+  SET_GREEN_RGB15PC(SrcPtr->Green)
							+  SET_RED_RGB15PC(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB15PC(pLine16);
						Green = GET_GREEN_RGB15PC(pLine16);
						Blue  = GET_BLUE_RGB15PC(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB15PC(Blue)
							+  SET_GREEN_RGB15PC(Green)
							+  SET_RED_RGB15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR15PC(SrcPtr->Blue)
							+  SET_GREEN_BGR15PC(SrcPtr->Green)
							+  SET_RED_BGR15PC(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;
		
						Red   = GET_RED_BGR15PC(pLine16);
						Green = GET_GREEN_BGR15PC(pLine16);
						Blue  = GET_BLUE_BGR15PC(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR15PC(Blue)
							+  SET_GREEN_BGR15PC(Green)
							+  SET_RED_BGR15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB16PC(SrcPtr->Blue)
							+  SET_GREEN_RGB16PC(SrcPtr->Green)
							+  SET_RED_RGB16PC(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;
		
						Red   = GET_RED_RGB16PC(pLine16);
						Green = GET_GREEN_RGB16PC(pLine16);
						Blue  = GET_BLUE_RGB16PC(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB16PC(Blue)
							+  SET_GREEN_RGB16PC(Green)
							+  SET_RED_RGB16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = (ULONG) SrcPtr->Alpha;
					ULONG mla;

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR16PC(SrcPtr->Blue)
							+  SET_GREEN_BGR16PC(SrcPtr->Green)
							+  SET_RED_BGR16PC(SrcPtr->Red);
						break;
					default:
						mla = 257 - a;
		
						Blue   = GET_BLUE_BGR16PC(pLine16);
						Green  = GET_GREEN_BGR16PC(pLine16);
						Red    = GET_RED_BGR16PC(pLine16);
						Red   = (a * SrcPtr->Red   + mla * Red)   >> 8;
						Green = (a * SrcPtr->Green + mla * Green) >> 8;
						Blue  = (a * SrcPtr->Blue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR16PC(Blue)
							+  SET_GREEN_BGR16PC(Green)
							+  SET_RED_BGR16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}

				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		LONG width = dhcr->dhcr_Bounds.MaxX - dhcr->dhcr_Bounds.MinX + 1;
		LONG height = dhcr->dhcr_Bounds.MaxY - dhcr->dhcr_Bounds.MinY + 1;
		if (width > 0 && height > 0)
			{
			BytesPerPixel = 3;
			BytesPerRow = BytesPerPixel * width;
			Addr = AllocVec(BytesPerRow * height, MEMF_ANY);
			if (Addr)
				{
				ULONG y;
				ULONG SrcLeft = baa->baa_SrcLeft + (dhcr->dhcr_OffsetX - baa->baa_DestLeft);
				ULONG SrcTop  = baa->baa_SrcTop  + (dhcr->dhcr_OffsetY - baa->baa_DestTop);
				const struct ARGB *Src = baa->baa_Src->argb_ImageData + SrcTop * baa->baa_Src->argb_Width;
				UBYTE *pPixel = ((UBYTE *) Addr); // + BytesPerRow * dhcr->dhcr_Bounds.MinY;

				ReadPixelArray(Addr, 0, 0, BytesPerRow, rp,
					baa->baa_DestLeft, baa->baa_DestTop,
					width, height,
					RECTFMT_RGB);

				//PIXFMT_RGB24
				for (y = 0; y < height; y++)
					{
					ULONG x;
					UBYTE *pLine = pPixel; // + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
					const struct ARGB *SrcPtr = Src + SrcLeft;

					for (x = 0; x < width; x++)
						{
						ULONG a = (ULONG) SrcPtr->Alpha;
						ULONG mla;

						switch (a)
							{
						case 0:
						case 1:
							break;
						case ALPHA_OPAQUE:
							pLine[0] = SrcPtr->Red;
							pLine[1] = SrcPtr->Green;
							pLine[2] = SrcPtr->Blue;
							break;
						default:
							mla = 257 - a;

							pLine[0] = (a * SrcPtr->Red   + mla * pLine[0])   >> 8;
							pLine[1] = (a * SrcPtr->Green + mla * pLine[1])   >> 8;
							pLine[2] = (a * SrcPtr->Blue  + mla * pLine[2])   >> 8;
							break;
							}

						pLine += BytesPerPixel;
						SrcPtr++;
						}

					pPixel += BytesPerRow;
					Src += baa->baa_Src->argb_Width;
					}
				WritePixelArray(Addr, 0, 0, BytesPerRow, rp, 
					baa->baa_DestLeft, baa->baa_DestTop,
					width, height,
					RECTFMT_RGB);
				FreeVec(Addr);
				}
			}
		}

	return 0;
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitARGBAlphaKTHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitARGBAlphaData *baa = (struct BlitARGBAlphaData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG y;
		ULONG SrcLeft = baa->baa_SrcLeft + (dhcr->dhcr_OffsetX - baa->baa_DestLeft);
		ULONG SrcTop  = baa->baa_SrcTop  + (dhcr->dhcr_OffsetY - baa->baa_DestTop);
		const struct ARGB *Src = baa->baa_Src->argb_ImageData + SrcTop * baa->baa_Src->argb_Width;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;
		ULONG T = baa->baa_Transparency;

		d1(KPrintF(__FILE__ "/%s/%ld: baa_SrcLeft=%ld  baa_SrcTop=%ld\n", __FUNC__, __LINE__, baa->baa_SrcLeft, baa->baa_SrcTop));
		d1(KPrintF(__FILE__ "/%s/%ld: baa_DestLeft=%ld  baa_DestTop=%ld\n", __FUNC__, __LINE__, baa->baa_DestLeft, baa->baa_DestTop));
		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: SrcLeft=%ld  SrcTop=%ld\n", __FUNC__, __LINE__, SrcLeft, SrcTop));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[0] = SrcRed;
						pLine[1] = SrcGreen;
						pLine[2] = SrcBlue;
						break;
					default:
						mla = 257 - a;

						pLine[0] = (a * SrcRed   + mla * pLine[0])   >> 8;
						pLine[1] = (a * SrcGreen + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcBlue  + mla * pLine[2])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[0] = SrcBlue;
						pLine[1] = SrcGreen;
						pLine[2] = SrcRed;
						break;
					default:
						mla = 257 - a;

						pLine[0] = (a * SrcBlue  + mla * pLine[0])   >> 8;
						pLine[1] = (a * SrcGreen + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcRed   + mla * pLine[2])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[1] = SrcRed;
						pLine[2] = SrcGreen;
						pLine[3] = SrcBlue;
						break;
					default:
						mla = 257 - a;

						pLine[1] = (a * SrcRed   + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcGreen + mla * pLine[2])   >> 8;
						pLine[3] = (a * SrcBlue  + mla * pLine[3])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB16(SrcBlue)
							+  SET_GREEN_RGB16(SrcGreen)
							+  SET_RED_RGB16(SrcRed);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB16(pLine16);
						Green = GET_GREEN_RGB16(pLine16);
						Blue  = GET_BLUE_RGB16(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB16(Blue)
							+  SET_GREEN_RGB16(Green)
							+  SET_RED_RGB16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR16(SrcBlue)
							+  SET_GREEN_BGR16(SrcGreen)
							+  SET_RED_BGR16(SrcRed);
						break;
					default:
						mla = 257 - a;

						Blue   = GET_BLUE_BGR16(pLine16);
						Green  = GET_GREEN_BGR16(pLine16);
						Red    = GET_RED_BGR16(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR16(Blue)
							+  SET_GREEN_BGR16(Green)
							+  SET_RED_BGR16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB15(SrcBlue)
							+  SET_GREEN_RGB15(SrcGreen)
							+  SET_RED_RGB15(SrcRed);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB15(pLine16);
						Green = GET_GREEN_RGB15(pLine16);
						Blue  = GET_BLUE_RGB15(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB15(Blue)
							+  SET_GREEN_RGB15(Green)
							+  SET_RED_RGB15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR15(SrcBlue)
							+  SET_GREEN_BGR15(SrcGreen)
							+  SET_RED_BGR15(SrcRed);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR15(pLine16);
						Green = GET_GREEN_BGR15(pLine16);
						Blue  = GET_BLUE_BGR15(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR15(Blue)
							+  SET_GREEN_BGR15(Green)
							+  SET_RED_BGR15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB15PC(SrcBlue)
							+  SET_GREEN_RGB15PC(SrcGreen)
							+  SET_RED_RGB15PC(SrcRed);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB15PC(pLine16);
						Green = GET_GREEN_RGB15PC(pLine16);
						Blue  = GET_BLUE_RGB15PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB15PC(Blue)
							+  SET_GREEN_RGB15PC(Green)
							+  SET_RED_RGB15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR15PC(SrcBlue)
							+  SET_GREEN_BGR15PC(SrcGreen)
							+  SET_RED_BGR15PC(SrcRed);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR15PC(pLine16);
						Green = GET_GREEN_BGR15PC(pLine16);
						Blue  = GET_BLUE_BGR15PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR15PC(Blue)
							+  SET_GREEN_BGR15PC(Green)
							+  SET_RED_BGR15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB16PC(SrcBlue)
							+  SET_GREEN_RGB16PC(SrcGreen)
							+  SET_RED_RGB16PC(SrcRed);
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB16PC(pLine16);
						Green = GET_GREEN_RGB16PC(pLine16);
						Blue  = GET_BLUE_RGB16PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB16PC(Blue)
							+  SET_GREEN_RGB16PC(Green)
							+  SET_RED_RGB16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;
					ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
					ULONG mla;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR16PC(SrcBlue)
							+  SET_GREEN_BGR16PC(SrcGreen)
							+  SET_RED_BGR16PC(SrcRed);
						break;
					default:
						mla = 257 - a;

						Blue   = GET_BLUE_BGR16PC(pLine16);
						Green  = GET_GREEN_BGR16PC(pLine16);
						Red    = GET_RED_BGR16PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR16PC(Blue)
							+  SET_GREEN_BGR16PC(Green)
							+  SET_RED_BGR16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		LONG width = dhcr->dhcr_Bounds.MaxX - dhcr->dhcr_Bounds.MinX + 1;
		LONG height = dhcr->dhcr_Bounds.MaxY - dhcr->dhcr_Bounds.MinY + 1;
		if (width > 0 && height > 0)
			{
			ULONG T = baa->baa_Transparency;
			BytesPerPixel = 3;
			BytesPerRow = BytesPerPixel * width;
			Addr = AllocVec(BytesPerRow * height, MEMF_ANY);

			if (Addr && width > 0 && height > 0)
				{
				ULONG y;
				ULONG SrcLeft = baa->baa_SrcLeft + (dhcr->dhcr_OffsetX - baa->baa_DestLeft);
				ULONG SrcTop  = baa->baa_SrcTop  + (dhcr->dhcr_OffsetY - baa->baa_DestTop);
				const struct ARGB *Src = baa->baa_Src->argb_ImageData + SrcTop * baa->baa_Src->argb_Width;
				UBYTE *pPixel = ((UBYTE *) Addr); // + BytesPerRow * dhcr->dhcr_Bounds.MinY;

				ReadPixelArray(Addr, 0, 0, BytesPerRow, rp,
					baa->baa_DestLeft, baa->baa_DestTop,
					width, height,
					RECTFMT_RGB);

				for (y = 0; y < height; y++)
					{
					ULONG x;
					UBYTE *pLine = pPixel; // + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
					const struct ARGB *SrcPtr = Src + SrcLeft;
					UWORD SrcRed, SrcGreen, SrcBlue;

					for (x = 0; x < width; x++)
						{
						ULONG a = ((T * SrcPtr->Alpha) >> 8) + 1;
						ULONG mla;

						SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
						SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
						SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

						switch (a)
							{
						case 0:
						case 1:
							break;
						case ALPHA_OPAQUE:
							pLine[0] = SrcRed;
							pLine[1] = SrcGreen;
							pLine[2] = SrcBlue;
							break;
						default:
							mla = 257 - a;

							pLine[0] = (a * SrcRed   + mla * pLine[0])   >> 8;
							pLine[1] = (a * SrcGreen + mla * pLine[1])   >> 8;
							pLine[2] = (a * SrcBlue  + mla * pLine[2])   >> 8;
							break;
							}

						pLine += BytesPerPixel;
						SrcPtr++;
						}
					pPixel += BytesPerRow;
					Src += baa->baa_Src->argb_Width;
					}
				WritePixelArray(Addr, 0, 0, BytesPerRow, rp, 
					baa->baa_DestLeft, baa->baa_DestTop,
					width, height,
					RECTFMT_RGB);
				FreeVec(Addr);
				}
			}
		}

	return 0;
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitARGBKTHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitARGBAlphaData *baa = (struct BlitARGBAlphaData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG y;
		ULONG SrcLeft = baa->baa_SrcLeft + (dhcr->dhcr_OffsetX - baa->baa_DestLeft);
		ULONG SrcTop  = baa->baa_SrcTop  + (dhcr->dhcr_OffsetY - baa->baa_DestTop);
		const struct ARGB *Src = baa->baa_Src->argb_ImageData + SrcTop * baa->baa_Src->argb_Width;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;
		ULONG T = baa->baa_Transparency;
		ULONG a = T + 1;
		ULONG mla = 257 - a;

		d1(KPrintF(__FILE__ "/%s/%ld: baa_SrcLeft=%ld  baa_SrcTop=%ld\n", __FUNC__, __LINE__, baa->baa_SrcLeft, baa->baa_SrcTop));
		d1(KPrintF(__FILE__ "/%s/%ld: baa_DestLeft=%ld  baa_DestTop=%ld\n", __FUNC__, __LINE__, baa->baa_DestLeft, baa->baa_DestTop));
		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: SrcLeft=%ld  SrcTop=%ld\n", __FUNC__, __LINE__, SrcLeft, SrcTop));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[0] = SrcRed;
						pLine[1] = SrcGreen;
						pLine[2] = SrcBlue;
						break;
					default:
						pLine[0] = (a * SrcRed   + mla * pLine[0])   >> 8;
						pLine[1] = (a * SrcGreen + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcBlue  + mla * pLine[2])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[0] = SrcBlue;
						pLine[1] = SrcGreen;
						pLine[2] = SrcRed;
						break;
					default:
						pLine[0] = (a * SrcBlue  + mla * pLine[0])   >> 8;
						pLine[1] = (a * SrcGreen + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcRed   + mla * pLine[2])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						pLine[1] = SrcRed;
						pLine[2] = SrcGreen;
						pLine[3] = SrcBlue;
						break;
					default:
						pLine[1] = (a * SrcRed   + mla * pLine[1])   >> 8;
						pLine[2] = (a * SrcGreen + mla * pLine[2])   >> 8;
						pLine[3] = (a * SrcBlue  + mla * pLine[3])   >> 8;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB16(SrcBlue)
							+  SET_GREEN_RGB16(SrcGreen)
							+  SET_RED_RGB16(SrcRed);
						break;
					default:
						Red   = GET_RED_RGB16(pLine16);
						Green = GET_GREEN_RGB16(pLine16);
						Blue  = GET_BLUE_RGB16(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB16(Blue)
							+  SET_GREEN_RGB16(Green)
							+  SET_RED_RGB16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR16(SrcBlue)
							+  SET_GREEN_BGR16(SrcGreen)
							+  SET_RED_BGR16(SrcRed);
						break;
					default:
						Blue   = GET_BLUE_BGR16(pLine16);
						Green  = GET_GREEN_BGR16(pLine16);
						Red    = GET_RED_BGR16(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR16(Blue)
							+  SET_GREEN_BGR16(Green)
							+  SET_RED_BGR16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB15(SrcBlue)
							+  SET_GREEN_RGB15(SrcGreen)
							+  SET_RED_RGB15(SrcRed);
						break;
					default:
						Red   = GET_RED_RGB15(pLine16);
						Green = GET_GREEN_RGB15(pLine16);
						Blue  = GET_BLUE_RGB15(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB15(Blue)
							+  SET_GREEN_RGB15(Green)
							+  SET_RED_RGB15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR15(SrcBlue)
							+  SET_GREEN_BGR15(SrcGreen)
							+  SET_RED_BGR15(SrcRed);
						break;
					default:
						Red   = GET_RED_BGR15(pLine16);
						Green = GET_GREEN_BGR15(pLine16);
						Blue  = GET_BLUE_BGR15(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR15(Blue)
							+  SET_GREEN_BGR15(Green)
							+  SET_RED_BGR15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB15PC(SrcBlue)
							+  SET_GREEN_RGB15PC(SrcGreen)
							+  SET_RED_RGB15PC(SrcRed);
						break;
					default:
						Red   = GET_RED_RGB15PC(pLine16);
						Green = GET_GREEN_RGB15PC(pLine16);
						Blue  = GET_BLUE_RGB15PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB15PC(Blue)
							+  SET_GREEN_RGB15PC(Green)
							+  SET_RED_RGB15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR15PC(SrcBlue)
							+  SET_GREEN_BGR15PC(SrcGreen)
							+  SET_RED_BGR15PC(SrcRed);
						break;
					default:
						Red   = GET_RED_BGR15PC(pLine16);
						Green = GET_GREEN_BGR15PC(pLine16);
						Blue  = GET_BLUE_BGR15PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR15PC(Blue)
							+  SET_GREEN_BGR15PC(Green)
							+  SET_RED_BGR15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_RGB16PC(SrcBlue)
							+  SET_GREEN_RGB16PC(SrcGreen)
							+  SET_RED_RGB16PC(SrcRed);
						break;
					default:
						Red   = GET_RED_RGB16PC(pLine16);
						Green = GET_GREEN_RGB16PC(pLine16);
						Blue  = GET_BLUE_RGB16PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_RGB16PC(Blue)
							+  SET_GREEN_RGB16PC(Green)
							+  SET_RED_RGB16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + SrcLeft;
				UWORD Red, Green, Blue;
				UWORD SrcRed, SrcGreen, SrcBlue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					SrcRed   = min(255, SrcPtr->Red   + baa->baa_K->Red);
					SrcGreen = min(255, SrcPtr->Green + baa->baa_K->Green);
					SrcBlue  = min(255, SrcPtr->Blue  + baa->baa_K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						break;
					case ALPHA_OPAQUE:
						*pLine16 = SET_BLUE_BGR16PC(SrcBlue)
							+  SET_GREEN_BGR16PC(SrcGreen)
							+  SET_RED_BGR16PC(SrcRed);
						break;
					default:
						Blue   = GET_BLUE_BGR16PC(pLine16);
						Green  = GET_GREEN_BGR16PC(pLine16);
						Red    = GET_RED_BGR16PC(pLine16);
						Red   = (a * SrcRed   + mla * Red)   >> 8;
						Green = (a * SrcGreen + mla * Green) >> 8;
						Blue  = (a * SrcBlue  + mla * Blue)  >> 8;
						*pLine16 = SET_BLUE_BGR16PC(Blue)
							+  SET_GREEN_BGR16PC(Green)
							+  SET_RED_BGR16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += baa->baa_Src->argb_Width;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		kprintf(__FILE__ "/%s/%ld: Can't lock bitmap\n", __FUNC__, __LINE__);
		}

	return 0;
}

//-----------------------------------------------------------------------

void BlitTransparentK(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	const struct ARGB *K,
	struct ScalosGfxBase *ScalosGfxBase)
{
		struct BlitTransparentKData btk;
		struct Rectangle BlitTransparentKRect;
		struct Hook BlitTransparentKHook;

		d1(KPrintF(__FILE__ "/%s/%ld: R=%ld  G=%ld  B=%ld\n", __FUNC__, __LINE__, K->Red, K->Green, K->Blue));

		btk.btk_K = K;
		btk.btk_Width = Width;
		btk.btk_Height = Height;

		BlitTransparentKRect.MinX = 0;
		BlitTransparentKRect.MinY = 0;
		BlitTransparentKRect.MaxX = Width - 1;
		BlitTransparentKRect.MaxY = Height - 1;

		SETHOOKFUNC(BlitTransparentKHook, BlitTransparentKHookFunc);
		BlitTransparentKHook.h_Data = &btk;

		DoHookClipRects(&BlitTransparentKHook, rpIcon, &BlitTransparentKRect);
}

//-----------------------------------------------------------------------

void BlitTransparent(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	ULONG Trans,
	struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *BufferBg;

	BufferBg = AllocARGB(Width, Height, ScalosGfxBase);

	if (BufferBg)
		{
		struct BlitTransparentData bt;
		struct Rectangle BlitTransparentRect;
		struct Hook BlitTransparentHook;

		d1(KPrintF(__FILE__ "/%s/%ld: Trans=%ld\n", __FUNC__, __LINE__, Trans));

		// Read background
		ReadPixelArray(BufferBg,
			0, 0,
			Width * sizeof(struct ARGB),
			rpBackground,
			Left, Top,
			Width, Height,
                        RECTFMT_ARGB);

		bt.bt_a	= Trans + 1;
		bt.bt_mla = 257 - bt.bt_a;

		d1(KPrintF(__FILE__ "/%s/%ld: a=%lu  mla=%lu\n", __FUNC__, __LINE__, bt.bt_a, bt.bt_mla));

		if (ALPHA_OPAQUE != bt.bt_a)
			{
			bt.bt_BufferBg = BufferBg;
			bt.bt_Width = Width;
			bt.bt_Height = Height;

			BlitTransparentRect.MinX = 0;
			BlitTransparentRect.MinY = 0;
			BlitTransparentRect.MaxX = Width - 1;
			BlitTransparentRect.MaxY = Height - 1;

			SETHOOKFUNC(BlitTransparentHook, BlitTransparentHookFunc);
			BlitTransparentHook.h_Data = &bt;

			DoHookClipRects(&BlitTransparentHook, rpIcon, &BlitTransparentRect);
			}

		FreeARGB(&BufferBg, ScalosGfxBase);
		}
}

//-----------------------------------------------------------------------

void BlitTransparentAlpha(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	ULONG Trans, const UBYTE *Alpha,
	struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *BufferBg;

	BufferBg = AllocARGB(Width, Height, ScalosGfxBase);

	if (BufferBg)
		{
		struct BlitTransparentAlphaData bta;
		struct Rectangle BlitTransparentAlphaRect;
		struct Hook BlitTransparentAlphaHook;

		d1(KPrintF(__FILE__ "/%s/%ld: Trans=%ld\n", __FUNC__, __LINE__, Trans));

		// Read background
		ReadPixelArray(BufferBg,
			0, 0,
			Width * sizeof(struct ARGB),
			rpBackground,
			Left, Top,
			Width, Height,
                        RECTFMT_ARGB);

		bta.bta_BufferBg = BufferBg;
		bta.bta_Alpha = Alpha;
		bta.bta_Transparency = Trans;
		bta.bta_Width = Width;
		bta.bta_Height = Height;

		BlitTransparentAlphaRect.MinX = 0;
		BlitTransparentAlphaRect.MinY = 0;
		BlitTransparentAlphaRect.MaxX = Width - 1;
		BlitTransparentAlphaRect.MaxY = Height - 1;

		SETHOOKFUNC(BlitTransparentAlphaHook, BlitTransparentAlphaHookFunc);
		BlitTransparentAlphaHook.h_Data = &bta;

		DoHookClipRects(&BlitTransparentAlphaHook, rpIcon, &BlitTransparentAlphaRect);
 
		FreeARGB(&BufferBg, ScalosGfxBase);
		}
}

//-----------------------------------------------------------------------

void BlitTransparentAlphaK(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	const struct ARGB *K, const UBYTE *Alpha,
	struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *BufferBg;

	BufferBg = AllocARGB(Width, Height, ScalosGfxBase);

	if (BufferBg)
		{
		struct BlitTransparentAlphaKData btak;
		struct Rectangle BlitTransparentAlphaKRect;
		struct Hook BlitTransparentAlphaKHook;

		d1(KPrintF(__FILE__ "/%s/%ld: R=%ld  G=%ld  B=%ld\n", __FUNC__, __LINE__, K->Red, K->Green, K->Blue));

		// Read background
		ReadPixelArray(BufferBg,
			0, 0,
			Width * sizeof(struct ARGB),
			rpBackground,
			Left, Top,
			Width, Height,
                        RECTFMT_ARGB);

		btak.btak_BufferBg = BufferBg;
		btak.btak_Alpha = Alpha;
		btak.btak_K = K;
		btak.btak_Width = Width;
		btak.btak_Height = Height;

		BlitTransparentAlphaKRect.MinX = 0;
		BlitTransparentAlphaKRect.MinY = 0;
		BlitTransparentAlphaKRect.MaxX = Width - 1;
		BlitTransparentAlphaKRect.MaxY = Height - 1;

		SETHOOKFUNC(BlitTransparentAlphaKHook, BlitTransparentAlphaKHookFunc);
		BlitTransparentAlphaKHook.h_Data = &btak;

		DoHookClipRects(&BlitTransparentAlphaKHook, rpIcon, &BlitTransparentAlphaKRect);

		FreeARGB(&BufferBg, ScalosGfxBase);
		}
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitTransparentKHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitTransparentKData *btk = (struct BlitTransparentKData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG y;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;
		const struct ARGB *K = btk->btk_K;

		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[0];
					Green = pLine[1];
					Blue  = pLine[2];
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					pLine[0] = Red;
					pLine[1] = Green;
					pLine[2] = Blue;

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[2];
					Green = pLine[1];
					Blue  = pLine[0];
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					pLine[2] = Red;
					pLine[1] = Green;
					pLine[0] = Blue;

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[1];
					Green = pLine[2];
					Blue  = pLine[3];
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					pLine[1] = Red;
					pLine[2] = Green;
					pLine[3] = Blue;

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB16(pLine16);
					Green = GET_GREEN_RGB16(pLine16);
					Blue  = GET_BLUE_RGB16(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_RGB16(Blue)
						+  SET_GREEN_RGB16(Green)
						+  SET_RED_RGB16(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR16(pLine16);
					Green = GET_GREEN_BGR16(pLine16);
					Blue  = GET_BLUE_BGR16(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_BGR16(Blue)
						+  SET_GREEN_BGR16(Green)
						+  SET_RED_BGR16(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB15(pLine16);
					Green = GET_GREEN_RGB15(pLine16);
					Blue  = GET_BLUE_RGB15(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_RGB15(Blue)
						+  SET_GREEN_RGB15(Green)
						+  SET_RED_RGB15(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR15(pLine16);
					Green = GET_GREEN_BGR15(pLine16);
					Blue  = GET_BLUE_BGR15(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_BGR15(Blue)
						+  SET_GREEN_BGR15(Green)
						+  SET_RED_BGR15(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB15PC(pLine16);
					Green = GET_GREEN_RGB15PC(pLine16);
					Blue  = GET_BLUE_RGB15PC(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_RGB15PC(Blue)
						+  SET_GREEN_RGB15PC(Green)
						+  SET_RED_RGB15PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR15PC(pLine16);
					Green = GET_GREEN_BGR15PC(pLine16);
					Blue  = GET_BLUE_BGR15PC(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_BGR15PC(Blue)
						+  SET_GREEN_BGR15PC(Green)
						+  SET_RED_BGR15PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB16PC(pLine16);
					Green = GET_GREEN_RGB16PC(pLine16);
					Blue  = GET_BLUE_RGB16PC(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_RGB16PC(Blue)
						+  SET_GREEN_RGB16PC(Green)
						+  SET_RED_RGB16PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR16PC(pLine16);
					Green = GET_GREEN_BGR16PC(pLine16);
					Blue  = GET_BLUE_BGR16PC(pLine16);
					Red = min(255, Red + K->Red);
					Green = min(255, Green + K->Green);
					Blue = min(255, Blue + K->Blue);
					*pLine16 = SET_BLUE_BGR16PC(Blue)
						+  SET_GREEN_BGR16PC(Green)
						+  SET_RED_BGR16PC(Red);

					pLine += BytesPerPixel;
					}
				pPixel += BytesPerRow;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		kprintf(__FILE__ "/%s/%ld: Can't lock bitmap\n", __FUNC__, __LINE__);
		}

	return 0;
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitTransparentHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitTransparentData *bt = (struct BlitTransparentData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG a = bt->bt_a;
		ULONG mla = bt->bt_mla;
		ULONG y;
		const struct ARGB *Src = bt->bt_BufferBg + dhcr->dhcr_OffsetY * bt->bt_Width;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;

		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[0];
					Green = pLine[1];
					Blue  = pLine[2];
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					pLine[0] = Red;
					pLine[1] = Green;
					pLine[2] = Blue;

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[2];
					Green = pLine[1];
					Blue  = pLine[0];
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					pLine[2] = Red;
					pLine[1] = Green;
					pLine[0] = Blue;

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					Red   = pLine[1];
					Green = pLine[2];
					Blue  = pLine[3];
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					pLine[1] = Red;
					pLine[2] = Green;
					pLine[3] = Blue;

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB16(pLine16);
					Green = GET_GREEN_RGB16(pLine16);
					Blue  = GET_BLUE_RGB16(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_RGB16(Blue)
						+  SET_GREEN_RGB16(Green)
						+  SET_RED_RGB16(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR16(pLine16);
					Green = GET_GREEN_BGR16(pLine16);
					Blue  = GET_BLUE_BGR16(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_BGR16(Blue)
						+  SET_GREEN_BGR16(Green)
						+  SET_RED_BGR16(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB15(pLine16);
					Green = GET_GREEN_RGB15(pLine16);
					Blue  = GET_BLUE_RGB15(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_RGB15(Blue)
						+  SET_GREEN_RGB15(Green)
						+  SET_RED_RGB15(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR15(pLine16);
					Green = GET_GREEN_BGR15(pLine16);
					Blue  = GET_BLUE_BGR15(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_BGR15(Blue)
						+  SET_GREEN_BGR15(Green)
						+  SET_RED_BGR15(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB15PC(pLine16);
					Green = GET_GREEN_RGB15PC(pLine16);
					Blue  = GET_BLUE_RGB15PC(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_RGB15PC(Blue)
						+  SET_GREEN_RGB15PC(Green)
						+  SET_RED_RGB15PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR15PC(pLine16);
					Green = GET_GREEN_BGR15PC(pLine16);
					Blue  = GET_BLUE_BGR15PC(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_BGR15PC(Blue)
						+  SET_GREEN_BGR15PC(Green)
						+  SET_RED_BGR15PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_RGB16PC(pLine16);
					Green = GET_GREEN_RGB16PC(pLine16);
					Blue  = GET_BLUE_RGB16PC(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_RGB16PC(Blue)
						+  SET_GREEN_RGB16PC(Green)
						+  SET_RED_RGB16PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					Red   = GET_RED_BGR16PC(pLine16);
					Green = GET_GREEN_BGR16PC(pLine16);
					Blue  = GET_BLUE_BGR16PC(pLine16);
					Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
					Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
					Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
					*pLine16 = SET_BLUE_BGR16PC(Blue)
						+  SET_GREEN_BGR16PC(Green)
						+  SET_RED_BGR16PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				Src += bt->bt_Width;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		LONG width;
		LONG height;

		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: bt_BufferBg=%p\n", __FUNC__, __LINE__, bt->bt_BufferBg));
		d1(KPrintF(__FILE__ "/%s/%ld: bt_a=%ld      bt_mla=%ld\n", __FUNC__, __LINE__, bt->bt_a, bt->bt_mla));
		d1(KPrintF(__FILE__ "/%s/%ld: bt_Width=%ld  bt_Height=%ld\n", __FUNC__, __LINE__, bt->bt_Width, bt->bt_Height));

		width = dhcr->dhcr_Bounds.MaxX - dhcr->dhcr_Bounds.MinX + 1;
		height = dhcr->dhcr_Bounds.MaxY - dhcr->dhcr_Bounds.MinY + 1;
		if (width > 0 && height > 0)
			{
			BytesPerPixel = 3;
			BytesPerRow = BytesPerPixel * width;
			Addr = AllocVec(BytesPerRow * height, MEMF_ANY);
			if (Addr && width > 0 && height > 0)
				{
				ULONG a = bt->bt_a;
				ULONG mla = bt->bt_mla;
				ULONG y;
				const struct ARGB *Src = bt->bt_BufferBg + dhcr->dhcr_OffsetY * bt->bt_Width;
				UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;

				ReadPixelArray(Addr, 0, 0, BytesPerRow, rp,
					0, 0,
					width, height,
					RECTFMT_RGB);

				for (y = 0; y < height; y++)
					{
					ULONG x;
					UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
					const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
					UWORD Red, Green, Blue;

					for (x = 0; x < width; x++)
						{
						Red   = pLine[0];
						Green = pLine[1];
						Blue  = pLine[2];
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						pLine[0] = Red;
						pLine[1] = Green;
						pLine[2] = Blue;

						pLine += BytesPerPixel;
						SrcPtr++;
						}
					pPixel += BytesPerRow;
					Src += bt->bt_Width;
					}
				WritePixelArray(Addr, 0, 0, BytesPerRow, rp, 
					0, 0,
					width, height,
					RECTFMT_RGB);
				FreeVec(Addr);
				}
			}
		}

	return 0;
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitTransparentAlphaHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitTransparentAlphaData *bta = (struct BlitTransparentAlphaData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG a;
		ULONG mla;
		ULONG y;
		const struct ARGB *Src = bta->bta_BufferBg + dhcr->dhcr_OffsetY * bta->bta_Width;
		const UBYTE *pAlpha = bta->bta_Alpha + dhcr->dhcr_OffsetY * bta->bta_Width;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;
		ULONG Trans = bta->bta_Transparency;

		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						pLine[0] = SrcPtr->Red;
						pLine[1] = SrcPtr->Green;
						pLine[2] = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = pLine[0];
						Green = pLine[1];
						Blue  = pLine[2];

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						pLine[0] = Red;
						pLine[1] = Green;
						pLine[2] = Blue;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						pLine[2] = SrcPtr->Red;
						pLine[1] = SrcPtr->Green;
						pLine[0] = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = pLine[2];
						Green = pLine[1];
						Blue  = pLine[0];

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						pLine[2] = Red;
						pLine[1] = Green;
						pLine[0] = Blue;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						pLine[1] = SrcPtr->Red;
						pLine[2] = SrcPtr->Green;
						pLine[3] = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = pLine[1];
						Green = pLine[2];
						Blue  = pLine[3];

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						pLine[1] = Red;
						pLine[2] = Green;
						pLine[3] = Blue;
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_RGB16(SrcPtr->Blue)
							+  SET_GREEN_RGB16(SrcPtr->Green)
							+  SET_RED_RGB16(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB16(pLine16);
						Green = GET_GREEN_RGB16(pLine16);
						Blue  = GET_BLUE_RGB16(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_RGB16(Blue)
							+  SET_GREEN_RGB16(Green)
							+  SET_RED_RGB16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_BGR16(SrcPtr->Blue)
							+  SET_GREEN_BGR16(SrcPtr->Green)
							+  SET_RED_BGR16(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR16(pLine16);
						Green = GET_GREEN_BGR16(pLine16);
						Blue  = GET_BLUE_BGR16(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_BGR16(Blue)
							+  SET_GREEN_BGR16(Green)
							+  SET_RED_BGR16(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_RGB15(SrcPtr->Blue)
							+  SET_GREEN_RGB15(SrcPtr->Green)
							+  SET_RED_RGB15(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB15(pLine16);
						Green = GET_GREEN_RGB15(pLine16);
						Blue  = GET_BLUE_RGB15(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_RGB15(Blue)
							+  SET_GREEN_RGB15(Green)
							+  SET_RED_RGB15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_BGR15(SrcPtr->Blue)
							+  SET_GREEN_BGR15(SrcPtr->Green)
							+  SET_RED_BGR15(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR15(pLine16);
						Green = GET_GREEN_BGR15(pLine16);
						Blue  = GET_BLUE_BGR15(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_BGR15(Blue)
							+  SET_GREEN_BGR15(Green)
							+  SET_RED_BGR15(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_RGB15PC(SrcPtr->Blue)
							+  SET_GREEN_RGB15PC(SrcPtr->Green)
							+  SET_RED_RGB15PC(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB15PC(pLine16);
						Green = GET_GREEN_RGB15PC(pLine16);
						Blue  = GET_BLUE_RGB15PC(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_RGB15PC(Blue)
							+  SET_GREEN_RGB15PC(Green)
							+  SET_RED_RGB15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_BGR15PC(SrcPtr->Blue)
							+  SET_GREEN_BGR15PC(SrcPtr->Green)
							+  SET_RED_BGR15PC(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR15PC(pLine16);
						Green = GET_GREEN_BGR15PC(pLine16);
						Blue  = GET_BLUE_BGR15PC(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_BGR15PC(Blue)
							+  SET_GREEN_BGR15PC(Green)
							+  SET_RED_BGR15PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_RGB16PC(SrcPtr->Blue)
							+  SET_GREEN_RGB16PC(SrcPtr->Green)
							+  SET_RED_RGB16PC(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_RGB16PC(pLine16);
						Green = GET_GREEN_RGB16PC(pLine16);
						Blue  = GET_BLUE_RGB16PC(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_RGB16PC(Blue)
							+  SET_GREEN_RGB16PC(Green)
							+  SET_RED_RGB16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					a = ((Trans * a) >> 8) + 1;

					switch (a)
						{
					case 0:
					case 1:
						*pLine16 = SET_BLUE_BGR16PC(SrcPtr->Blue)
							+  SET_GREEN_BGR16PC(SrcPtr->Green)
							+  SET_RED_BGR16PC(SrcPtr->Red);
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;

						Red   = GET_RED_BGR16PC(pLine16);
						Green = GET_GREEN_BGR16PC(pLine16);
						Blue  = GET_BLUE_BGR16PC(pLine16);

						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;

						*pLine16 = SET_BLUE_BGR16PC(Blue)
							+  SET_GREEN_BGR16PC(Green)
							+  SET_RED_BGR16PC(Red);
						break;
						}

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += bta->bta_Width;
				Src += bta->bta_Width;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		kprintf(__FILE__ "/%s/%ld: Can't lock bitmap\n", __FUNC__, __LINE__);
		}

	return 0;
}

//-----------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT BlitTransparentAlphaKHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct RastPort *rp = (struct RastPort *) o;
	struct DoHookClipRectMsg *dhcr = (struct DoHookClipRectMsg *) msg;
	struct BlitTransparentAlphaKData *btak = (struct BlitTransparentAlphaKData *) hook->h_Data;
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	APTR Addr;
	APTR handle;

	handle = LockBitMapTags(rp->BitMap,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		LBMI_BASEADDRESS, (ULONG) &Addr,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG a;
		ULONG mla;
		ULONG y;
		const struct ARGB *Src = btak->btak_BufferBg + dhcr->dhcr_OffsetY * btak->btak_Width;
		const UBYTE *pAlpha = btak->btak_Alpha + dhcr->dhcr_OffsetY * btak->btak_Width;
		UBYTE *pPixel = ((UBYTE *) Addr) + BytesPerRow * dhcr->dhcr_Bounds.MinY;
		const struct ARGB *K = btak->btak_K;

		d1(KPrintF(__FILE__ "/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_Bounds.MinX, dhcr->dhcr_Bounds.MinY, dhcr->dhcr_Bounds.MaxX, dhcr->dhcr_Bounds.MaxY));
		d1(KPrintF(__FILE__ "/%s/%ld: dhcr_OffsetX=%ld  dhcr_OffsetY=%ld\n", __FUNC__, __LINE__, dhcr->dhcr_OffsetX, dhcr->dhcr_OffsetY));
		d1(KPrintF(__FILE__ "/%s/%ld: BytesPerPixel=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, BytesPerPixel, BytesPerRow));

		d1(KPrintF(__FILE__ "/%s/%ld: PixFmt=%lu\n", __FUNC__, __LINE__, PixFmt));

		switch (PixFmt)
			{
		case PIXFMT_RGB24:
		case PIXFMT_RGBA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					a = (ULONG) *pLineAlpha++;

					Red   = pLine[0];
					Green = pLine[1];
					Blue  = pLine[2];

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					pLine[1] = Red;
					pLine[1] = Green;
					pLine[2] = Blue;

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_BGR24:
		case PIXFMT_BGRA32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					a = (ULONG) *pLineAlpha++;

					Red   = pLine[2];
					Green = pLine[1];
					Blue  = pLine[0];

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					pLine[2] = Red;
					pLine[1] = Green;
					pLine[0] = Blue;

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_ARGB32:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					a = (ULONG) *pLineAlpha++;

					Red   = pLine[1];
					Green = pLine[2];
					Blue  = pLine[3];

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					pLine[1] = Red;
					pLine[2] = Green;
					pLine[3] = Blue;

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_RGB16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_RGB16(pLine16);
					Green = GET_GREEN_RGB16(pLine16);
					Blue  = GET_BLUE_RGB16(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_RGB16(Blue)
						+  SET_GREEN_RGB16(Green)
						+  SET_RED_RGB16(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_BGR16:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_BGR16(pLine16);
					Green = GET_GREEN_BGR16(pLine16);
					Blue  = GET_BLUE_BGR16(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_BGR16(Blue)
						+  SET_GREEN_BGR16(Green)
						+  SET_RED_BGR16(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_RGB15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_RGB15(pLine16);
					Green = GET_GREEN_RGB15(pLine16);
					Blue  = GET_BLUE_RGB15(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_RGB15(Blue)
						+  SET_GREEN_RGB15(Green)
						+  SET_RED_RGB15(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_BGR15:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;
					
					Red   = GET_RED_BGR15(pLine16);
					Green = GET_GREEN_BGR15(pLine16);
					Blue  = GET_BLUE_BGR15(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_BGR15(Blue)
						+  SET_GREEN_BGR15(Green)
						+  SET_RED_BGR15(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_RGB15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_RGB15PC(pLine16);
					Green = GET_GREEN_RGB15PC(pLine16);
					Blue  = GET_BLUE_RGB15PC(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_RGB15PC(Blue)
						+  SET_GREEN_RGB15PC(Green)
						+  SET_RED_RGB15PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_BGR15PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_BGR15PC(pLine16);
					Green = GET_GREEN_BGR15PC(pLine16);
					Blue  = GET_BLUE_BGR15PC(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_BGR15PC(Blue)
						+  SET_GREEN_BGR15PC(Green)
						+  SET_RED_BGR15PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_RGB16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_RGB16PC(pLine16);
					Green = GET_GREEN_RGB16PC(pLine16);
					Blue  = GET_BLUE_RGB16PC(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_RGB16PC(Blue)
						+  SET_GREEN_RGB16PC(Green)
						+  SET_RED_RGB16PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		case PIXFMT_BGR16PC:
			for (y = dhcr->dhcr_Bounds.MinY; y <= dhcr->dhcr_Bounds.MaxY; y++)
				{
				ULONG x;
				UBYTE *pLine = pPixel + dhcr->dhcr_Bounds.MinX * BytesPerPixel;
				const struct ARGB *SrcPtr = Src + dhcr->dhcr_OffsetX;
				const UBYTE *pLineAlpha = pAlpha + dhcr->dhcr_OffsetX;
				UWORD Red, Green, Blue;

				for (x = dhcr->dhcr_Bounds.MinX; x <= dhcr->dhcr_Bounds.MaxX; x++)
					{
					UWORD *pLine16 = (UWORD *) pLine;

					a = (ULONG) *pLineAlpha++;

					Red   = GET_RED_BGR16PC(pLine16);
					Green = GET_GREEN_BGR16PC(pLine16);
					Blue  = GET_BLUE_BGR16PC(pLine16);

					Red     = min(0xff, Red + K->Red);
					Green   = min(0xff, Green + K->Green);
					Blue    = min(0xff, Blue + K->Blue);

					switch (a)
						{
					case 0:
					case 1:
						Red   = SrcPtr->Red;
						Green = SrcPtr->Green;
						Blue  = SrcPtr->Blue;
						break;
					case ALPHA_OPAQUE:
						break;
					default:
						mla = 257 - a;
						Red   = (SrcPtr->Red   * mla + Red   * a) >> 8;
						Green = (SrcPtr->Green * mla + Green * a) >> 8 ;
						Blue  = (SrcPtr->Blue  * mla + Blue  * a) >> 8;
						break;
						}

					*pLine16 = SET_BLUE_BGR16PC(Blue)
						+  SET_GREEN_BGR16PC(Green)
						+  SET_RED_BGR16PC(Red);

					pLine += BytesPerPixel;
					SrcPtr++;
					}
				pPixel += BytesPerRow;
				pAlpha += btak->btak_Width;
				Src += btak->btak_Width;
				}
			break;
		default:
			break;
			}
		UnLockBitMap(handle);
		}
	else
		{
		kprintf(__FILE__ "/%s/%ld: Can't lock bitmap\n", __FUNC__, __LINE__);
		}

	return 0;
}

//-----------------------------------------------------------------------

