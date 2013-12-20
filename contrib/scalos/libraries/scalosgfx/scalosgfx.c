// scalosgfx.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/resident.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <graphics/gfxbase.h>
#include <scalos/scalos.h>
#include <scalos/scalosgfx.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/scalosgfx.h>

#include <string.h>

#include "scalosgfx_base.h"
#include "scalosgfx.h"
#include <defs.h>
#include <Year.h>

//----------------------------------------------------------------------------

#define	IS_NOT_EVEN(len)		((len) & 1)
#define	EVEN(len)			(((len) + 1) & ~1)
#define	MAGIC_PREFS_LIST_ENTRY_LIST	((UWORD) -1)

#define	min(a,b)			((a) < (b) ? (a) : (b))

#ifdef __amigaos4__
#define ExtLib(base) ((struct ExtendedLibrary *)((ULONG)base + ((struct Library *)base)->lib_PosSize))
#define IScalosGfx ((struct ScaosGfxIFace *)ExtLib(ScalosGfxBase)->MainIFace)
#endif

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

// Standard library functions

//----------------------------------------------------------------------------

#ifndef __AROS__
struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase;
T_UTILITYBASE UtilityBase;
struct DosLibrary * DOSBase;
struct Library *CyberGfxBase;
#endif
struct GfxBase *GfxBase;
struct Library *LayersBase;
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct GraphicsIFace *IGraphics;
struct ExecIFace *IExec;
struct IntuitionIFace *IIntuition;
struct UtilityIFace *IUtility;
struct DOSIFace *IDOS;
struct CyberGfxIFace *ICyberGfx;
struct LayersIFace *ILayers;
#endif

//----------------------------------------------------------------------------

char ALIGNED libName[] = "scalosgfx.library";
char ALIGNED libIdString[] = "$VER: scalosgfx.library "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (29 Jul 2008 19:29:17) "
	COMPILER_STRING 
	" ©2006" CURRENTYEAR " The Scalos Team";

//----------------------------------------------------------------------------


BOOL ScalosGfxInit(struct ScalosGfxBase *ScalosGfxBase)
{
	d1(kprintf("%s/%ld:   START ScalosGfxBase=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		ScalosGfxBase, FindTask(NULL)->tc_Node.ln_Name));

	NewList(&ScalosGfxBase->sgb_PrefsList);

	InitSemaphore(&ScalosGfxBase->sgb_MemPoolSemaphore);
	InitSemaphore(&ScalosGfxBase->sgb_PrefsListSem);

	d1(kprintf("%s/%ld: END Success\n", __FUNC__, __LINE__));

	return TRUE;	// Success
}

//-----------------------------------------------------------------------------

BOOL ScalosGfxOpen(struct ScalosGfxBase *ScalosGfxBase)
{
	BOOL Success = FALSE;

	d1(kprintf("%s/%ld:   START ScalosGfxBase=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		ScalosGfxBase, FindTask(NULL)->tc_Node.ln_Name));

	do	{
		IntuitionBase = (APTR) OpenLibrary( "intuition.library", 39 );
#ifdef __amigaos4__
		if (IntuitionBase)
			{
			IIntuition = (APTR) GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
			if (!IIntuition)
				{
				CloseLibrary((struct Library *)IntuitionBase);
				IntuitionBase = NULL;
				}
			}
#endif
		if (NULL == IntuitionBase)
			break;

		UtilityBase = (APTR) OpenLibrary( "utility.library", 39 );
#ifdef __amigaos4__
		if (UtilityBase)
			{
			IUtility = (APTR) GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
			if (!IUtility)
				{
				CloseLibrary((struct Library *)UtilityBase);
				UtilityBase = NULL;
				}
			}
#endif
		if (NULL == UtilityBase)
			break;

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
#ifdef __amigaos4__
		if (GfxBase != NULL)
			{
			IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
			if (IGraphics == NULL)
				{
				CloseLibrary((struct Library *)GfxBase);
				GfxBase = NULL;
				}
			}
#endif
		d1(kprintf("%s/%ld: GfxBase=%08lx\n", __FUNC__, __LINE__, GfxBase));
		if (NULL == GfxBase)
			return 0;

		CyberGfxBase = (APTR) OpenLibrary( "cybergraphics.library", 40);
#ifdef __amigaos4__
		if (CyberGfxBase)
			{
			ICyberGfx = (APTR) GetInterface((struct Library *)CyberGfxBase, "main", 1, NULL);
			if (!ICyberGfx)
				{
				CloseLibrary(CyberGfxBase);
				CyberGfxBase = NULL;
				}
			}
#endif
		// CyberGfxBase may be NULL

		DOSBase = (APTR) OpenLibrary( "dos.library", 39 );
#ifdef __amigaos4__
		if (DOSBase)
			{
			IDOS = (APTR) GetInterface((struct Library *)DOSBase, "main", 1, NULL);
			if (!IDOS)
				{
				CloseLibrary((struct Library *)DOSBase);
				DOSBase = NULL;
				}
			}
#endif
		if (NULL == DOSBase)
			break;

		LayersBase = OpenLibrary( "layers.library", 39 );
#ifdef __amigaos4__
		if (LayersBase)
			{
			ILayers = (APTR) GetInterface(LayersBase, "main", 1, NULL);
			if (!ILayers)
				{
				CloseLibrary(LayersBase);
				LayersBase = NULL;
				}
			}
#endif
		if (NULL == LayersBase)
			break;

#ifdef __amigaos4__
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;
#endif

		ScalosGfxBase->sgb_MemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR, 8192, 256);
		if (NULL == ScalosGfxBase->sgb_MemPool)
			break;

		Success = TRUE;
		} while (0);
	
	d1(kprintf("%s/%ld: END Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

BOOL ScalosGfxCleanup(struct ScalosGfxBase *ScalosGfxBase)
{
	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

	if (CyberGfxBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ICyberGfx);
#endif
		CloseLibrary(CyberGfxBase);
		CyberGfxBase = NULL;
		}
	if (NULL != GfxBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IGraphics);
#endif
		CloseLibrary((struct Library *) GfxBase);
		GfxBase = NULL;
		}
	if (UtilityBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IUtility);
#endif
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
	if (IntuitionBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IIntuition);
#endif
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
	if (DOSBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IDOS);
#endif
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
	if (LayersBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ILayers);
#endif
		CloseLibrary(LayersBase);
		LayersBase = NULL;
		}
#ifdef __amigaos4__
	if (INewlib)
		{
		DropInterface(INewlib);
		INewlib = NULL;
		}
	if (NewlibBase)
		{
		CloseLibrary(NewlibBase);
		NewlibBase = NULL;
		}
#endif
	if (ScalosGfxBase->sgb_MemPool)
		{
		DeletePool(ScalosGfxBase->sgb_MemPool);
		ScalosGfxBase->sgb_MemPool = NULL;
		}
	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));
	return TRUE;
}

//-----------------------------------------------------------------------------

LIBFUNC_P1(struct ScalosBitMapAndColor *, LIBScalosGfxCreateEmptySAC,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 5)
{
	return AllocEmptySAC(ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P6(struct ScalosBitMapAndColor *, LIBScalosGfxCreateSAC,
	D0, ULONG, width,
	D1, ULONG, height,
	D2, ULONG, depth,
	A0, struct BitMap *, friendBM,
	A1, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 6)
{
	(void) tagList;

	return AllocSAC(width, height, depth, friendBM, tagList, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBScalosGfxFreeSAC,
	A0, struct ScalosBitMapAndColor *, sac,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 7)
{
	FreeSAC(sac, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(struct gfxARGB *, LIBScalosGfxCreateARGB,
	D0, ULONG, width,
	D1, ULONG, height,
	A0, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 8)
{
	(void) tagList;

	return AllocARGB(width, height, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P2(VOID, LIBScalosGfxFreeARGB,
	A0, struct gfxARGB **, argb,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 9)
{
	FreeARGB(argb, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(VOID, LIBScalosGfxARGBSetAlpha,
	A0, struct ARGBHeader *, argbh,
	D0, UBYTE, alpha,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 10)
{
	(void) ScalosGfxBase;

	ARGBSetAlpha(argbh->argb_ImageData,
		argbh->argb_Width, argbh->argb_Height, alpha);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(VOID, LIBScalosGfxARGBSetAlphaMask,
	A0, struct ARGBHeader *, argbh,
	A1, PLANEPTR, maskPlane,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 11)
{
	(void) ScalosGfxBase;

	ARGBSetAlphaFromMask(argbh, maskPlane);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P7(struct gfxARGB *, LIBScalosGfxCreateARGBFromBitMap,
	A0, struct BitMap *,bm,
	D0, ULONG, width,
	D1, ULONG, height,
	D2, ULONG, numberOfColors,
	A1, const ULONG *, colorTable,
	A2, PLANEPTR, maskPlane,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 12)
{
	return CreateARGBFromBitMap(bm, width, height,
		numberOfColors, colorTable, maskPlane, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(VOID, LIBScalosGfxFillARGBFromBitMap,
	A0, struct ARGBHeader *, argbh,
	A1, struct BitMap *, srcBM,
	A2, PLANEPTR, maskPlane,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 13)
{
	(void) ScalosGfxBase;

	FillARGBFromBitMap(argbh, srcBM, maskPlane);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(VOID, LIBScalosGfxWriteARGBToBitMap,
	A0, struct ARGBHeader *, argbh,
	A1, struct BitMap *, bm,
	D0, ULONG, numberOfColors,
	A2, const ULONG *, colorTable,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 14)
{
	(void) ScalosGfxBase;

	WriteARGBToBitMap(argbh->argb_ImageData, bm,
		argbh->argb_Width, argbh->argb_Height,
		numberOfColors, colorTable, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P4(struct ScalosBitMapAndColor *, LIBScalosGfxMedianCut,
	A0, struct ARGBHeader *, argbh,
	D0, ULONG, depth,
	A1, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 15)
{
	ULONG flags;
	BOOL floyd;
	struct BitMap *friendBM;
	ULONG ReservedColors;
	ULONG NewColors;
	struct ScalosBitMapAndColor *sac = NULL;

	d1(KPrintF(__FILE__ "/%s/%ld:  START  tagList=%08lx\n", __FUNC__, __LINE__, tagList));

	flags = GetTagData(SCALOSGFX_MedianCutFlags, 0, tagList);
	floyd = flags & SCALOSGFXFLAGF_MedianCut_FloydSteinberg;
	ReservedColors = GetTagData(SCALOSGFX_MedianCutReservedColors, 0, tagList);
	friendBM = (struct BitMap *) GetTagData(SCALOSGFX_MedianCutFriendBitMap, (ULONG)NULL, tagList);

	d1(KPrintF(__FILE__ "/%s/%ld:  flags=%08lx  ReservedColors=%lu\n", __FUNC__, __LINE__, flags, ReservedColors));

	NewColors = 1 << depth;

	do	{
		NewColors += ReservedColors;
		if (NewColors > 256)
			break;

		// adjst depth for reserved colors
		while (NewColors > (1 << depth))
			depth++;

		d1(KPrintF("%s/%ld:  NewColors=%lu  depth=%lu  ReservedColors=%lu\n", \
			__FUNC__, __LINE__, NewColors, depth, ReservedColors));

		sac = MedianCut(argbh, depth, NewColors, floyd, friendBM, ScalosGfxBase);
		} while (0);

	return sac;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(struct gfxARGB *, LIBScalosGfxScaleARGBArray,
	A0, const struct ARGBHeader *, src,
	A1, ULONG *, destWidth,
	A2, ULONG *, destHeight,
	A3, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 16)
{
	ULONG flags;

	flags = GetTagData(SCALOSGFX_ScaleARGBArrayFlags, 0, tagList);

	return ScaleARGBArray(src, destWidth, destHeight, flags, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P3(struct BitMap *, LIBScalosGfxScaleBitMap,
	A0, struct ScaleBitMapArg *, sbma,
	A1, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 17)
{
	(void) tagList;

	return ScaleBitMap(sbma->sbma_SourceBM,
		sbma->sbma_SourceWidth,
		sbma->sbma_SourceHeight,
		sbma->sbma_DestWidth,
		sbma->sbma_DestHeight,
		sbma->sbma_NumberOfColors,
		sbma->sbma_ColorTable,
		sbma->sbma_Flags,
		sbma->sbma_ScreenBM,
                ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P5(VOID, LIBScalosGfxCalculateScaleAspect,
	D0, ULONG, sourceWidth,
	D1, ULONG, sourceHeight,
	A0, ULONG *, destWidth,
	A1, ULONG *, destHeight,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 18)
{
	(void) ScalosGfxBase;

	CalculateScaleAspect(sourceWidth, sourceHeight, destWidth, destHeight);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(VOID, LIBScalosGfxBlitARGB,
	A0, struct ARGBHeader *, DestARGB,
	A1, const struct ARGBHeader *, SrcARGB,
	D0, LONG, DestLeft,
	D1, LONG, DestTop,
	D2, LONG, SrcLeft,
	D3, LONG, SrcTop,
	D4, LONG, Width,
	D5, LONG, Height,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 19)
{
	(void) ScalosGfxBase;

	BlitARGB(DestARGB, SrcARGB, DestLeft, DestTop,
		SrcLeft, SrcTop, Width, Height);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

// Fill a rectangle with a given ARGB color.

LIBFUNC_P7(VOID, LIBScalosGfxFillRectARGB,
	A0, struct ARGBHeader *, DestARGB,
	A1, const struct gfxARGB *, fillARGB,
	D0, LONG, left,
	D1, LONG, top,
	D2, LONG, width,
	D3, LONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 20)
{
	(void) ScalosGfxBase;

	FillARGB(DestARGB, fillARGB, left, top, width, height);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

// Fill entire ARGB with given ARGB color

LIBFUNC_P3(VOID, LIBScalosGfxSetARGB,
	A0, struct ARGBHeader *, DestARGB,
	A1, const struct gfxARGB *, fillARGB,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 21)
{
	(void) ScalosGfxBase;

	SetARGB(DestARGB, fillARGB);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

// replace ColorMap in sac by given new one

LIBFUNC_P4(BOOL, LIBScalosGfxNewColorMap,
	A0, struct ScalosBitMapAndColor *, sac,
	A1, const ULONG *, colorMap,
	D0, ULONG, colorEntries,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 22)
{
	(void) ScalosGfxBase;

	return SetNewSacColorMap(sac, colorMap, colorEntries, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P8(VOID, LIBScalosGfxARGBRectMult,
	A0, struct RastPort *, rp,
	A1, const struct ARGB *, numerator,
	A2, const struct ARGB *, denominator,
	D0, WORD, xMin,
	D1, WORD, yMin,
	D2, WORD, xMax,
	D3, WORD, yMax,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 23)
{
	ARGBRectMult(rp, *numerator, *denominator,
		xMin, yMin,
                xMax, yMax,
		ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(VOID, LIBScalosGfxBlitARGBAlpha,
	A0, struct RastPort *, rp,
	A1, const struct ARGBHeader *, srcH,
	D0, ULONG, destLeft,
	D1, ULONG, destTop,
	D2, ULONG, srcLeft,
	D3, ULONG, srcTop,
	D4, ULONG, width,
	D5, ULONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 24)
{
	BlitARGBAlpha(rp, srcH,
		destLeft, destTop,
		srcLeft, srcTop,
		width, height,
		ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P7(VOID, LIBScalosGfxBlitARGBAlphaTagList,
	A0, struct RastPort *, rp,
	A1, const struct ARGBHeader *, srcH,
	D0, ULONG, destLeft,
	D1, ULONG, destTop,
	A3, const struct IBox *, srcSize,
	A2, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 25)
{
	const struct ARGB KDefault = { 255, 0, 0, 0 };
	const struct ARGB *K;
	ULONG Trans;
	ULONG NoAlpha;

	K = (const struct ARGB *) GetTagData(SCALOSGFX_BlitARGBHilight, (ULONG) NULL, tagList);
	Trans = GetTagData(SCALOSGFX_BlitARGBTransparency, 255, tagList);
	NoAlpha	= GetTagData(SCALOSGFX_BlitARGBNoAlpha, FALSE, tagList);

	if (NoAlpha)
		{
		if (NULL == K)
			K = &KDefault;

		d1(KPrintF("%s/%ld:  Red=%ld  Green=%ld  Blue=%ld  Trans=%lu\n", __FUNC__, __LINE__, K->Red, K->Green, K->Blue, Trans));

		BlitARGBKT(rp, srcH,
			destLeft, destTop,
			srcSize->Left, srcSize->Top,
			srcSize->Width, srcSize->Height,
			K, Trans,
			ScalosGfxBase);
		}
	else if (K || 255 != Trans)
		{
		d1(KPrintF("%s/%ld:  Red=%ld  Green=%ld  Blue=%ld  Trans=%lu\n", __FUNC__, __LINE__, K->Red, K->Green, K->Blue, Trans));
		BlitARGBAlphaKT(rp, srcH,
			destLeft, destTop,
			srcSize->Left, srcSize->Top,
			srcSize->Width, srcSize->Height,
			K, Trans,
			ScalosGfxBase);
		}
	else
		{
		BlitARGBAlpha(rp, srcH,
			destLeft, destTop,
			srcSize->Left, srcSize->Top,
			srcSize->Width, srcSize->Height,
			ScalosGfxBase);
		}
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P8(VOID, LIBScalosGfxBlitIcon,
	A0, struct RastPort *, rpBackground,
	A1, struct RastPort *, rpIcon,
	D0, ULONG, left,
	D1, ULONG, top,
	D2, ULONG, width,
	D3, ULONG, height,
	A2, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 26)
{
	const struct ARGB *K;
	const UBYTE *Alpha;
	ULONG Trans;

	K = (const struct ARGB *) GetTagData(SCALOSGFX_BlitIconHilight, (ULONG) NULL, tagList);
	Alpha = (const UBYTE *) GetTagData(SCALOSGFX_BlitIconAlpha, (ULONG) NULL, tagList);
	Trans = GetTagData(SCALOSGFX_BlitIconTransparency, 255, tagList);

	if (Alpha)
		{
		if (K)
			{
			BlitTransparentAlphaK(rpBackground, rpIcon,
				left, top, width, height,
				K, Alpha, ScalosGfxBase);
			}
		else
			{
			BlitTransparentAlpha(rpBackground, rpIcon,
				left, top, width, height,
				Trans, Alpha, ScalosGfxBase);
			}
		}
	else
		{
		if (K)
			{
			BlitTransparentK(rpBackground, rpIcon,
				left, top, width, height,
				K, ScalosGfxBase);
			}
		else
			{
			BlitTransparent(rpBackground, rpIcon,
				left, top, width, height,
				Trans, ScalosGfxBase);
			}
		}
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(BOOL, LIBScalosGfxDrawGradient,
	A0, struct ARGBHeader *, dest,
	D0, LONG, left,
	D1, LONG, top,
	D2, LONG, width,
	D3, LONG, height,
	A1, struct gfxARGB *, start,
	A2, struct gfxARGB *, stop,
	D4, ULONG, gradType,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 27)
{
	return ScaDrawGradient(dest, left, top, width, height, start, stop,  gradType, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(BOOL, LIBScalosGfxDrawGradientRastPort,
	A0, struct RastPort *, rp,
	D0, LONG, left,
	D1, LONG, top,
	D2, LONG, width,
	D3, LONG, height,
	A1, struct gfxARGB *, start,
	A2, struct gfxARGB *, stop,
	D4, ULONG, gradType,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 28)
{
	return DrawGradientRastPort(rp, left, top, width, height, start, stop,  gradType, ScalosGfxBase);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P7(VOID, LIBScalosGfxDrawLine,
	A0, struct ARGBHeader *, dest,
	D0, LONG, fromX,
	D1, LONG, fromY,
	D2, LONG, toX,
	D3, LONG, toY,
	A1, const struct gfxARGB *, lineColor,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 29)
{
	(void) ScalosGfxBase;

	DrawLine(dest, fromX, fromY, toX, toY, *lineColor);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P7(VOID, LIBScalosGfxDrawLineRastPort,
	A0, struct RastPort *, rp,
	D0, LONG, fromX,
	D1, LONG, fromY,
	D2, LONG, toX,
	D3, LONG, toY,
	A1, const struct gfxARGB *, lineColor,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 30)
{
	(void) ScalosGfxBase;

	DrawLineRastPort(rp, fromX, fromY, toX, toY, *lineColor);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(VOID, LIBScalosGfxDrawEllipse,
	A0, struct ARGBHeader *, dest,
	D0, LONG, xCenter,
	D1, LONG, yCenter,
	D2, LONG, radiusX,
	D3, LONG, radiusY,
	D4, WORD, segment,
	A1, const struct gfxARGB *, color1,
	A2, const struct gfxARGB *, color2,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 31)
{
	(void) ScalosGfxBase;

	DrawARGBEllipse(dest, xCenter, yCenter, radiusX, radiusY,
		segment, *color1, *color2);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

LIBFUNC_P9(VOID, LIBScalosGfxDrawEllipseRastPort,
	A0, struct RastPort *, rp,
	D0, LONG, xCenter,
	D1, LONG, yCenter,
	D2, LONG, radiusX,
	D3, LONG, radiusY,
	D4, WORD, segment,
	A1, const struct gfxARGB *, color1,
	A2, const struct gfxARGB *, color2,
	A6, struct ScalosGfxBase *, ScalosGfxBase, 32)
{
	(void) ScalosGfxBase;

	DrawARGBEllipseRastPort(rp, xCenter, yCenter, radiusX, radiusY,
		segment, *color1, *color2);
}
LIBFUNC_END

//-----------------------------------------------------------------------------

APTR ScalosGfxAllocVecPooled(struct ScalosGfxBase *ScalosGfxBase, ULONG Size)
{
	APTR ptr;

	if (ScalosGfxBase->sgb_MemPool)
		{
		ObtainSemaphore(&ScalosGfxBase->sgb_MemPoolSemaphore);
		ptr = AllocPooled(ScalosGfxBase->sgb_MemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&ScalosGfxBase->sgb_MemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FUNC__, __LINE__, ScalosGfxBase->sgb_MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%ld:  MemPool=%08lx  Size=%lu\n", __FUNC__, __LINE__, ScalosGfxBase->sgb_MemPool, Size));

	return NULL;
}

//-----------------------------------------------------------------------------

void ScalosGfxFreeVecPooled(struct ScalosGfxBase *ScalosGfxBase, APTR mem)
{
	d1(kprintf("%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FUNC__, __LINE__, ScalosGfxBase->sgb_MemPool, mem));
	if (ScalosGfxBase->sgb_MemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&ScalosGfxBase->sgb_MemPoolSemaphore);
		FreePooled(ScalosGfxBase->sgb_MemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&ScalosGfxBase->sgb_MemPoolSemaphore);
		}
}

//-----------------------------------------------------------------------------

#if !defined(__SASC) && !defined(__amigaos4__)
void exit(int x)
{
   (void) x;
   while (1)
	  ;
}

#endif /* !defined(__SASC) */

//-----------------------------------------------------------------------------

#if defined(__SASC)
void _XCEXIT(long x)
{
}
#endif /* defined(__SASC) */

//-----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2INITLIB(ScalosGfxInit, 0);
ADD2EXPUNGELIB(ScalosGfxCleanup, 0);
ADD2OPENLIB(ScalosGfxOpen, 0);

#endif

//-----------------------------------------------------------------------------
