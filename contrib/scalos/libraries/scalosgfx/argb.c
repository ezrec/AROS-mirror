// argb.c
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

//-----------------------------------------------------------------------

static ULONG FindBestPen(const ULONG *ColorTable, ULONG NumColors, ULONG Red, ULONG Green, LONG Blue);
static long GetColorDistance(const ULONG *ColorTableEntry, ULONG Red, ULONG Green, ULONG Blue);

//-----------------------------------------------------------------------

void ARGBSetAlpha(struct ARGB *argb, ULONG Width, ULONG Height, UBYTE Alpha)
{
	ULONG y;

	d1(KPrintF("%s/%ld:  argb=%08lx  Width=%lu  Height=%lu  Alpha=%lu\n", \
		__FUNC__, __LINE__, argb, Width, Height, Alpha));

	for (y = 0; y < Height; y++)
		{
		ULONG x;

		for (x = 0; x < Width; x++)
			{
			argb->Alpha = Alpha;
			argb++;
			}
		}
}

//-----------------------------------------------------------------------

void ARGBSetAlphaFromMask(struct ARGBHeader *argbh, PLANEPTR MaskPlane)
{
	ULONG y;
	ULONG MaskBytesPerRow = ((argbh->argb_Width + 15) & ~0x0f) / 8;
	struct ARGB *pargb = argbh->argb_ImageData;

	for (y = 0; y < argbh->argb_Height; y++)
		{
		ULONG x;

		UWORD BitMask = 0x0080;
		const UBYTE *MaskPtr2 = MaskPlane;

		for (x = 0; x < argbh->argb_Width; x++, pargb++)
			{
			if (*MaskPtr2 & BitMask)
				pargb->Alpha = 0xff;
			else
				pargb->Alpha = 0;

			BitMask >>= 1;
			if (0 == BitMask)
				{
				BitMask = 0x0080;
				MaskPtr2++;
				}
			}

		MaskPlane += MaskBytesPerRow;
		}
}

//-----------------------------------------------------------------------

struct ARGB *CreateARGBFromBitMap(struct BitMap *bm,
	ULONG Width, ULONG Height,
	ULONG NumberOfColors, const ULONG *ColorTable, PLANEPTR MaskPlane,
	struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *argb;
	UBYTE *LineArray = NULL;
	struct BitMap *TempBM = NULL;

	do	{
		struct RastPort rp;
		ULONG Depth;
		ULONG MaskBytesPerRow = ((Width + 15) & ~0x0f) / 8;

		d1(KPrintF("%s/%ld:  MaskPlane=%08lx  MaskBytesPerRow=%lu\n", __FUNC__, __LINE__, MaskPlane, MaskBytesPerRow));

		argb = AllocARGB(Width, Height, ScalosGfxBase);
		if (NULL == argb)
			break;

		Depth = GetBitMapAttr(bm, BMA_DEPTH);

		InitRastPort(&rp);
		rp.BitMap = bm;

		if (Depth <= 8 || (NULL == CyberGfxBase))
			{
			struct RastPort TempRp;
			ULONG y;
			struct ARGB *pargb = argb;
			size_t ArraySize = (((Width + 15) >> 4) << 4);

			LineArray = ScalosGfxAllocVecPooled(ScalosGfxBase, ArraySize);
			if (NULL == LineArray)
				break;

			InitRastPort(&TempRp);
			TempRp = rp;
			TempRp.Layer = NULL;
			TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(Width), 1, 8, 0, NULL);
			if (NULL == TempBM)
				break;

			for (y = 0; y < Height; y++)
				{
				ULONG x;
				UBYTE *pLine = LineArray;

				ReadPixelLine8(&rp,
					0, y,
					Width,
					LineArray,
					&TempRp);

				if (MaskPlane)
					{
					UWORD BitMask = 0x0080;
					const UBYTE *MaskPtr2 = MaskPlane;

					for (x = 0; x < Width; x++, pargb++)
						{
						UBYTE col = *pLine++;
						const ULONG *ColorReg = ColorTable + 3 * col;

						pargb->Red = ColorReg[0] >> 24;
						pargb->Green = ColorReg[1] >> 24;
						pargb->Blue = ColorReg[2] >> 24;

						if (*MaskPtr2 & BitMask)
							pargb->Alpha = 0xff;
						else
							pargb->Alpha = 0;

						BitMask >>= 1;
						if (0 == BitMask)
							{
							BitMask = 0x0080;
							MaskPtr2++;
							}
						}

					MaskPlane += MaskBytesPerRow;
					}
				else
					{
					for (x = 0; x < Width; x++, pargb++)
						{
						UBYTE col = *pLine++;
						const ULONG *ColorReg = ColorTable + 3 * col;

						pargb->Alpha = 0xff;
						pargb->Red = ColorReg[0] >> 24;
						pargb->Green = ColorReg[1] >> 24;
						pargb->Blue = ColorReg[2] >> 24;
						}
					}
				}
			}
		else
			{
			ReadPixelArray(argb, 0, 0,
				Width * sizeof(struct ARGB),
				&rp, 0, 0,
				Width, Height,
				RECTFMT_ARGB);
			if (MaskPlane)
				{
				struct ARGBHeader argbh;

				argbh.argb_Width = Width;
				argbh.argb_Height = Height;
				argbh.argb_ImageData = argb;

				d1(KPrintF("%s/%ld:  MaskPlane=%08lx\n", __FUNC__, __LINE__, MaskPlane));

				ARGBSetAlphaFromMask(&argbh, MaskPlane);
				}
			else
				{
				ARGBSetAlpha(argb, Width, Height, (UBYTE) ~0);
				}
			}
		} while (0);

	if (TempBM)
		FreeBitMap(TempBM);
	if (LineArray)
		ScalosGfxFreeVecPooled(ScalosGfxBase, LineArray);

	return argb;
}

//-----------------------------------------------------------------------

void WriteARGBToBitMap(struct ARGB *argb, struct BitMap *bm,
	ULONG Width, ULONG Height,
	ULONG NumberOfColors, const ULONG *ColorTable,
        struct ScalosGfxBase *ScalosGfxBase)
{
	ULONG Depth;
	struct RastPort rp;
	UBYTE *LineArray = NULL;
	struct BitMap *TempBM = NULL;

	do	{
		InitRastPort(&rp);
		rp.BitMap = bm;

		Depth = GetBitMapAttr(bm, BMA_DEPTH);

		d1(KPrintF("%s/%ld:  DestWidth=%lu  DestHeight=%ld\n", __FUNC__, __LINE__, Width, Height));

		if ((NULL == CyberGfxBase) || (Depth <= 8) || !GetCyberMapAttr(bm, CYBRMATTR_ISCYBERGFX))
			{
			struct RastPort TempRp;
			ULONG y;
			struct ARGB *pargb = argb;
			size_t ArraySize = (((Width + 15) >> 4) << 4);

			d1(KPrintF("%s/%ld:  DestWidth=%lu  DestHeight=%ld\n", __FUNC__, __LINE__, Width, Height));

			LineArray = ScalosGfxAllocVecPooled(ScalosGfxBase, ArraySize);
			if (NULL == LineArray)
				break;

			InitRastPort(&TempRp);
			TempRp = rp;
			TempRp.Layer = NULL;
			TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(Width), 1, 8, 0, NULL);
			if (NULL == TempBM)
				break;

			for (y = 0; y < Height; y++)
				{
				ULONG x;
				UBYTE *pLine = LineArray;

				for (x = 0; x < Width; x++)
					{
					*pLine++ = FindBestPen(ColorTable, NumberOfColors,
						pargb->Red << 24,
						pargb->Green << 24,
						pargb->Blue << 24);
					pargb++;
					}

				WritePixelLine8(&rp,
					0, y,
					Width,
					LineArray,
					&TempRp);
				}
			}
		else
			{
			WritePixelArray(argb, 0, 0,
				Width * sizeof(struct ARGB),
				&rp, 0, 0,
				Width, Height,
				RECTFMT_ARGB);
			}
		} while (0);

	if (TempBM)
		FreeBitMap(TempBM);
	if (LineArray)
		ScalosGfxFreeVecPooled(ScalosGfxBase, LineArray);
}

//-----------------------------------------------------------------------

void FreeARGB(struct ARGB **argb, struct ScalosGfxBase *ScalosGfxBase)
{
	if (*argb)
		{
		WaitBlit();
		d1(KPrintF("%s/%ld:  argb=%08lx\n", __FUNC__, __LINE__, *argb));
		ScalosGfxFreeVecPooled(ScalosGfxBase, *argb);
		*argb = NULL;
		}
}

//-----------------------------------------------------------------------

struct ARGB *AllocARGB(ULONG Width, ULONG Height, struct ScalosGfxBase *ScalosGfxBase)
{
	struct ARGB *argb;

	if (0 == Width || 0 == Height)
		return NULL;

	argb = ScalosGfxAllocVecPooled(ScalosGfxBase, Width * Height * sizeof(struct ARGB));
	d1(KPrintF("%s/%ld:  ARGB=%08lx  Width=%lu  Height=%ld\n", __FUNC__, __LINE__, argb, Width, Height));

	return argb;
}

//-----------------------------------------------------------------------

void FillARGBFromBitMap(struct ARGBHeader *argbh, struct BitMap *srcBM,
	PLANEPTR MaskPlane)
{
	ULONG PixFmt;
	ULONG BytesPerRow;
	ULONG BytesPerPixel;
	const UBYTE *src;
	struct ARGB *dest;
	APTR handle;

	if ((NULL == CyberGfxBase) || !GetCyberMapAttr(srcBM, CYBRMATTR_ISCYBERGFX))
		return;

	d1(KPrintF("%s/%ld:  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, argbh->argb_Width, argbh->argb_Height));

	handle = LockBitMapTags(srcBM,
		LBMI_PIXFMT, (ULONG) &PixFmt,
		LBMI_BASEADDRESS, (ULONG) &src,
		LBMI_BYTESPERROW, (ULONG) &BytesPerRow,
		LBMI_BYTESPERPIX, (ULONG) &BytesPerPixel,
		TAG_END);

	d1(KPrintF(__FILE__ "/%s/%ld: handle=%08lx\n", __FUNC__, __LINE__, handle));
	if (handle)
		{
		ULONG y;

		d1(KPrintF("%s/%ld:  src=%08lx\n", __FUNC__, __LINE__, src));
		d1(KPrintF("%s/%ld:  PixFmt=%ld  BytesPerRow=%ld\n", __FUNC__, __LINE__, PixFmt, BytesPerRow));

		dest = argbh->argb_ImageData;

		switch (PixFmt)
			{
		case PIXFMT_BGR24:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UBYTE *srcPtr = src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Blue  = srcPtr[0];
					dest->Green = srcPtr[1];
					dest->Red   = srcPtr[2];
					dest->Alpha = (UBYTE) ~0;

					dest++;
					srcPtr += BytesPerPixel;
					}

				src += BytesPerRow;
				}
			break;
		case PIXFMT_BGRA32:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UBYTE *srcPtr = src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Blue  = srcPtr[0];
					dest->Green = srcPtr[1];
					dest->Red   = srcPtr[2];
					dest->Alpha = srcPtr[3];

					dest++;
					srcPtr += BytesPerPixel;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_RGB24:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UBYTE *srcPtr = src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = srcPtr[0];
					dest->Green = srcPtr[1];
					dest->Blue  = srcPtr[2];
					dest->Alpha = (UBYTE) ~0;

					dest++;
					srcPtr += BytesPerPixel;
					}

				src += BytesPerRow;
				}
			break;
		case PIXFMT_RGBA32:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UBYTE *srcPtr = src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = srcPtr[0];
					dest->Green = srcPtr[1];
					dest->Blue  = srcPtr[2];
					dest->Alpha = srcPtr[3];

					dest++;
					srcPtr += BytesPerPixel;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_ARGB32:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UBYTE *srcPtr = src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = srcPtr[1];
					dest->Green = srcPtr[2];
					dest->Blue  = srcPtr[3];
					dest->Alpha = srcPtr[0];

					dest++;
					srcPtr += BytesPerPixel;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_RGB16:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_RGB16(srcPtr);
					dest->Green = GET_GREEN_RGB16(srcPtr);
					dest->Blue  = GET_BLUE_RGB16(srcPtr);
					dest->Alpha = (UBYTE) ~0;

					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_BGR16:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_BGR16(srcPtr);
					dest->Green = GET_GREEN_BGR16(srcPtr);
					dest->Blue  = GET_BLUE_BGR16(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_RGB15:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_RGB15(srcPtr);
					dest->Green = GET_GREEN_RGB15(srcPtr);
					dest->Blue  = GET_BLUE_RGB15(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_BGR15:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_BGR15(srcPtr);
					dest->Green = GET_GREEN_BGR15(srcPtr);
					dest->Blue  = GET_BLUE_BGR15(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_RGB16PC:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_RGB16PC(srcPtr);
					dest->Green = GET_GREEN_RGB16PC(srcPtr);
					dest->Blue  = GET_BLUE_RGB16PC(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_BGR16PC:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_BGR16PC(srcPtr);
					dest->Green = GET_GREEN_BGR16PC(srcPtr);
					dest->Blue  = GET_BLUE_BGR16PC(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_RGB15PC:
			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_RGB15PC(srcPtr);
					dest->Green = GET_GREEN_RGB15PC(srcPtr);
					dest->Blue  = GET_BLUE_RGB15PC(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		case PIXFMT_BGR15PC:
			d1(KPrintF("%s/%ld:  src=%08lx\n", __FUNC__, __LINE__, src));

			dest = argbh->argb_ImageData;

			for (y = 0; y < argbh->argb_Height; y++)
				{
				ULONG x;
				const UWORD *srcPtr = (UWORD *) src;

				for (x = 0; x < argbh->argb_Width; x++)
					{
					dest->Red   = GET_RED_BGR15PC(srcPtr);
					dest->Green = GET_GREEN_BGR15PC(srcPtr);
					dest->Blue  = GET_BLUE_BGR15PC(srcPtr);
					dest->Alpha = (UBYTE) ~0;
					srcPtr++;
					dest++;
					}

				src += BytesPerRow;
				}
			break;

		default:
			break;
			}

		UnLockBitMap(handle);
		if (MaskPlane)
			ARGBSetAlphaFromMask(argbh, MaskPlane);
		}
	else
		{
		kprintf(__FILE__ "/%s/%ld: Can't lock bitmap\n", __FUNC__, __LINE__);
		}
}

//-----------------------------------------------------------------------

static ULONG FindBestPen(const ULONG *ColorTable, ULONG NumColors, ULONG Red, ULONG Green, LONG Blue)
{
	ULONG BestPen = 0;
	long BestDistance = LONG_MAX;
	ULONG n;

	for (n = 0; n < NumColors; n++)
		{
		long ColorDistance = GetColorDistance(ColorTable, Red, Green, Blue);

		d1(KPrintF("%s/%ld:  R=%08lx G=%08lx b=%08lx Best=%ld  Dist=%ld\n", \
			__LINE__, ColorTable[0], ColorTable[1], ColorTable[2], BestDistance, ColorDistance));

		if (ColorDistance < BestDistance)
			{
			BestDistance = ColorDistance;
			BestPen = n;
	    		}

	    	ColorTable += 3;
		}

	d1(KPrintF("%s/%ld:  R=%08lx G=%08lx B=%08lx  NumColors=%lu  BestPen=%lu\n", \
		__LINE__, Red, Green, Blue, NumColors, BestPen));

	return BestPen;
}

//-----------------------------------------------------------------------

static long GetColorDistance(const ULONG *ColorTableEntry, ULONG Red, ULONG Green, ULONG Blue)
{
	LONG dRed,dGreen,dBlue;

	dRed   = (LONG) (Red   >> 24) - (LONG) (ColorTableEntry[0] >> 24);
	dGreen = (LONG) (Green >> 24) - (LONG) (ColorTableEntry[1] >> 24);
	dBlue  = (LONG) (Blue  >> 24) - (LONG) (ColorTableEntry[2] >> 24);

	return dRed * dRed + dGreen * dGreen +dBlue * dBlue;
}

//-----------------------------------------------------------------------

struct ScalosBitMapAndColor *AllocEmptySAC(struct ScalosGfxBase *ScalosGfxBase)
{
	struct ScalosBitMapAndColor *sac;

	sac = ScalosGfxAllocVecPooled(ScalosGfxBase, sizeof(struct ScalosBitMapAndColor));
	if (sac)
		memset(sac, 0, sizeof(struct ScalosBitMapAndColor));

	d1(KPrintF(__FILE__ "/%s/%ld:  END  sac=%08lx\n", __FUNC__, __LINE__, sac));

	return sac;
}

//-----------------------------------------------------------------------

struct ScalosBitMapAndColor *AllocSAC(ULONG Width, ULONG Height, ULONG Depth,
	struct BitMap *FriendBM, struct TagItem *TagList,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct ScalosBitMapAndColor *sac = NULL;
	BOOL Success = FALSE;

	(void) TagList;

	d1(KPrintF(__FILE__ "/%s/%ld:  START  Width=%lu  Heioght=%lu  Depth=%lu  FriendBM=%08lx\n", \
		__FUNC__, __LINE__, Width, Height, Depth, FriendBM));

	do	{
		if (Depth < 1 || Depth > 8)
			break;
		if (Width < 1 || Height < 1)
			break;

		sac = AllocEmptySAC(ScalosGfxBase);
		if (NULL == sac)
			break;

		sac->sac_Width = Width;
		sac->sac_Height = Height;
		sac->sac_Depth = Depth;

		sac->sac_BitMap = AllocBitMap(Width, Height, 8, BMF_CLEAR, FriendBM);
		d1(KPrintF(__FILE__ "/%s/%ld:  sac_BitMap=%08lx\n", __FUNC__, __LINE__, sac->sac_BitMap));
		if (NULL == sac->sac_BitMap)
			break;

		sac->sac_NumColors = 1 << Depth;

		sac->sac_ColorTable = ScalosGfxAllocVecPooled(ScalosGfxBase, SAC_COLORTABLESIZE(sac));
		d1(KPrintF(__FILE__ "/%s/%ld:  sac_ColorTable=%08lx\n", __FUNC__, __LINE__, sac->sac_ColorTable));
		if (NULL == sac->sac_ColorTable)
			break;

		Success = TRUE;
		} while (0);

	if (!Success)
		{
		FreeSAC(sac, ScalosGfxBase);
		sac = NULL;
		}

	d1(KPrintF(__FILE__ "/%s/%ld:  END  sac=%08lx\n", __FUNC__, __LINE__, sac));
        return sac;
}

//-----------------------------------------------------------------------

void FreeSAC(struct ScalosBitMapAndColor *sac, struct ScalosGfxBase *ScalosGfxBase)
{
	d1(KPrintF(__FILE__ "/%s/%ld:  START  sac=%08lx\n", __FUNC__, __LINE__, sac));
	if (sac)
		{
		d1(KPrintF(__FILE__ "/%s/%ld:  sac_ColorTable=%08lx\n", __FUNC__, __LINE__, sac->sac_ColorTable));
		if (sac->sac_ColorTable)
			{
			if (!(sac->sac_Flags & SACFLAGF_NO_FREE_COLORTABLE))
				ScalosGfxFreeVecPooled(ScalosGfxBase, sac->sac_ColorTable);
			sac->sac_ColorTable = NULL;
			}
		d1(KPrintF(__FILE__ "/%s/%ld:  sac_Flags=%08lx  sac_BitMap=%08lx\n", __FUNC__, __LINE__, sac->sac_Flags, sac->sac_BitMap));
		if (sac->sac_BitMap)
			{
			if (!(sac->sac_Flags & SACFLAGF_NO_FREE_BITMAP))
				FreeBitMap(sac->sac_BitMap);
			sac->sac_BitMap = NULL;
			}
		ScalosGfxFreeVecPooled(ScalosGfxBase, sac);
		}
	d1(KPrintF(__FILE__ "/%s/%ld:  END\n", __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------

void BlitARGB(struct ARGBHeader *DestARGB, const struct ARGBHeader *SrcARGB,
	LONG DestLeft, LONG DestTop, LONG SrcLeft, LONG SrcTop,
	LONG Width, LONG Height)
{
	struct ARGB *dest = DestARGB->argb_ImageData + DestTop * DestARGB->argb_Width;
	const struct ARGB *src = SrcARGB->argb_ImageData + SrcTop * SrcARGB->argb_Width;
	LONG y;

	for (y = 0; y < Height; y++)
		{
		memcpy(dest + DestLeft, src + SrcLeft, Width * sizeof(struct ARGB));
		dest += DestARGB->argb_Width;
		src  += SrcARGB->argb_Width;
		}
}

//-----------------------------------------------------------------------

void FillARGB(struct ARGBHeader *DestARGB, const struct ARGB *fillARGB,
	LONG left, LONG top, LONG width, LONG height)
{
	struct ARGB *dest = DestARGB->argb_ImageData + top * DestARGB->argb_Width;
	LONG y;

	for (y = 0; y < height; y++)
		{
		LONG x;
		struct ARGB *destRow = dest + left;

		for (x = 0; x < width; x++)
			*destRow++ = *fillARGB;

		dest += DestARGB->argb_Width;
		}
}

//-----------------------------------------------------------------------

void SetARGB(struct ARGBHeader *DestARGB, const struct ARGB *fillARGB)
{
	struct ARGB *dest = DestARGB->argb_ImageData;
	size_t Length = DestARGB->argb_Width * DestARGB->argb_Height;

	while (Length--)
		*dest++ = *fillARGB;
}

//-----------------------------------------------------------------------

BOOL SetNewSacColorMap(struct ScalosBitMapAndColor *sac, const ULONG *colorMap,
	ULONG colorEntries, struct ScalosGfxBase *ScalosGfxBase)
{
	BOOL Success = FALSE;

	do	{
		if (NULL == sac)
			break;

		if (sac->sac_ColorTable)
			{
			if (!(sac->sac_Flags & SACFLAGF_NO_FREE_COLORTABLE))
				ScalosGfxFreeVecPooled(ScalosGfxBase, sac->sac_ColorTable);
			sac->sac_ColorTable = NULL;
			}

		sac->sac_NumColors = colorEntries;
		sac->sac_Flags &= ~SACFLAGF_NO_FREE_COLORTABLE;

		sac->sac_ColorTable = ScalosGfxAllocVecPooled(ScalosGfxBase, SAC_COLORTABLESIZE(sac));
		if (NULL == sac->sac_ColorTable)
			break;

		memcpy(sac->sac_ColorTable, colorMap, SAC_COLORTABLESIZE(sac));

		Success = TRUE;
		} while (0);

	return Success;
}

//-----------------------------------------------------------------------

