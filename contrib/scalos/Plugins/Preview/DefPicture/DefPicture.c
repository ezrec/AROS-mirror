// DefPicture.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <datatypes/pictureclass.h>
#include <graphics/gfxbase.h>
#include <scalos/scalos.h>
#include <scalos/scalosgfx.h>
#include <scalos/scalospreviewplugin.h>
#include <cybergraphx/cybergraphics.h>
#include <utility/hooks.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/datatypes.h>
#include <proto/utility.h>
#include <proto/scalosgfx.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include <defs.h>

#include "DefPicture.h"

//---------------------------------------------------------------------------------------

// set BYTEDUMP to 1 if you need to use ByteDump()
#define BYTEDUMP	0

#if 1
#define DUMP_ARGB_LINE(argbh, component, yy)
#else
#define DUMP_ARGB_LINE(argbh, component, yy)	  \
	{ \
	int n; \
	const struct ARGB  *argb = &(argbh)->argb_ImageData[(yy) * (argbh)->argb_Width]; \
	KPrintF(__FILE__ "/%s/%ld: w=%ld  h=%ld  yy=%ld  #component:\n", __FUNC__, __LINE__, \
		(argbh)->argb_Width, (argbh)->argb_Height, yy); \
	KPrintF(__FILE__ "/%s/%ld: ", __FUNC__, __LINE__); \
	for (n = 0; n < (argbh)->argb_Width; n++, argb++) \
		{ \
		if (n > 0 && 0 == n % 16) \
			KPrintF("\n" __FILE__ "/%s/%ld: ", __FUNC__, __LINE__); \
		KPrintF("%02lx ", argb->component); \
		} \
	KPrintF("\n"); \
	}
#endif

//---------------------------------------------------------------------------------------

#ifndef __AROS__
struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct Library *CyberGfxBase;
struct Library *DataTypesBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
#endif
struct ScalosGfxBase *ScalosGfxBase;

#ifdef __amigaos4__
struct GraphicsIFace *IGraphics;
struct UtilityIFace *IUtility;
struct CyberGfxIFace *ICyberGfx;
struct DataTypesIFace *IDataTypes;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct ScalosGfxIFace *IScalosGfx;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

//---------------------------------------------------------------------------------------

static BOOL GenerateRemappedThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, Object *ImageObj,
	ULONG quality, ULONG ReservedColors,
	ULONG ScaledWidth, ULONG ScaledHeight,
	ULONG SupportedColors, struct Screen *WBScreen,
        struct ScalosPreviewResult *PVResult);
static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, Object *ImageObj,
	ULONG quality, ULONG ReservedColors,
        ULONG ScaledWidth, ULONG ScaledHeight,
	ULONG SupportedColors, struct ScalosPreviewResult *PVResult);
static BOOL GenerateThumbnailFromARGB(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, struct ARGBHeader *argbDest,
	ULONG SupportedColors, ULONG ReservedColors,
	struct ScalosPreviewResult *PVResult);
static BOOL IsPictureDT43(Object *o);
#if BYTEDUMP
static void ByteDump(const unsigned char *Data, size_t Length);
#endif /* BYTEDUMP */

// #define KPrintF DebugPrintF

//-----------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	d1(KPrintF("%s/%ld:   PluginBase=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		PluginBase, FindTask(NULL)->tc_Node.ln_Name));

	DOSBase  = (APTR) OpenLibrary( "dos.library", 39 );
	if (NULL == DOSBase)
		return FALSE;

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;

	DataTypesBase = OpenLibrary("datatypes.library", 39);
	if (NULL == DataTypesBase)
		return FALSE;

	CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
	// CyberGfxBase may be NULL

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	d1(kprintf("%s/%s/%ld: IntuitionBase=%08lx\n", __FILE__, __FUNC__, __LINE__, IntuitionBase));
	if (NULL == IntuitionBase)
		return FALSE;

	GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
	d1(kprintf("%s/%s/%ld: GfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, GfxBase));
	if (NULL == GfxBase)
		return FALSE;

	ScalosGfxBase = (struct ScalosGfxBase *) OpenLibrary(SCALOSGFXNAME, 41);
	d1(KPrintF("%s/%s/%ld: ScalosGfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosGfxBase));
	if (NULL == ScalosGfxBase)
		return FALSE;

#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (IDOS == NULL)
		return FALSE;
	IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
	IDataTypes = (struct DataTypesIFace *)GetInterface((struct Library *)DataTypesBase, "main", 1, NULL);
	if (IDataTypes == NULL)
		return FALSE;
	if (CyberGfxBase != NULL)       // CyberGfxBase may be NULL
		{
		ICyberGfx = (struct CyberGfxIFace *)GetInterface(CyberGfxBase, "main", 1, NULL);
		if (ICyberGfx == NULL)
			return FALSE;
		}
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
	IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	if (NULL == IGraphics)
		return FALSE;
	IScalosGfx = (struct ScalosGfxIFace *)GetInterface((struct Library *)ScalosGfxBase, "main", 1, NULL);
	if (NULL == IScalosGfx)
		return FALSE;
	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif /* __amigaos4__ */


#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

	return TRUE;       // Succes
}


BOOL closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

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
	if (ICyberGfx)
		{
		DropInterface((struct Interface *)ICyberGfx);
		ICyberGfx = NULL;
		}
	if (IScalosGfx)
		{
		DropInterface((struct Interface *)IScalosGfx);
		IScalosGfx = NULL;
		}
	if (IGraphics)
		{
		DropInterface((struct Interface *)IGraphics);
		IGraphics = NULL;
		}
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
	if (IDataTypes)
		{
		DropInterface((struct Interface *)IDataTypes);
		IDataTypes = NULL;
		}
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif /* __amigaos4__ */

	if (CyberGfxBase)
		{
		CloseLibrary(CyberGfxBase);
		CyberGfxBase = NULL;
		}
	if (NULL != ScalosGfxBase)
		{
		CloseLibrary((struct Library *) ScalosGfxBase);
		ScalosGfxBase = NULL;
		}
	if (NULL != GfxBase)
		{
		CloseLibrary((struct Library *) GfxBase);
		GfxBase = NULL;
		}
	if (NULL != IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
	if (NULL != DataTypesBase)
		{
		CloseLibrary(DataTypesBase);
		DataTypesBase = NULL;
		}

	if (NULL != UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
	if (NULL != DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
	return TRUE;
}

//--------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSCAPreviewGenerate,
	A0, struct ScaWindowTask *, wt,
	A1, BPTR, dirLock,
	A2, CONST_STRPTR, iconName,
	A3, struct TagItem *, tagList,
	A6, struct PluginBase *, PluginBase, 5)
{
	Object *ImageObj  = NULL;
	LONG Success = FALSE;
	struct ScalosPreviewResult *PVResult = NULL;
#if defined(__MORPHOS__)
	struct BitMap BitMapCopy;
#endif /* __MORPHOS__ */

	(void) dirLock;
	(void) PluginBase;

	d1(KPrintF(__FILE__ "/%s/%ld: tagList=%lx\n", __FUNC__, __LINE__, tagList));

	do	{
		ULONG ThumbnailWidth;
		ULONG ThumbnailHeight;
		ULONG SupportedColors;
		ULONG ScreenDepth;
		struct Screen *WBScreen;
		struct BitMapHeader *bmhdExternal;
		struct BitMapHeader *bmhd = NULL;
		ULONG quality;
		ULONG ReservedColors;

#if defined(__MORPHOS__)
		memset(&BitMapCopy, 0, sizeof(BitMapCopy));
#endif /* __MORPHOS__ */

		PVResult = (struct ScalosPreviewResult *) GetTagData(SCALOSPREVIEW_ResultStruct, (ULONG)NULL, tagList);
		d1(KPrintF(__FILE__ "/%s/%ld: PVResult=%08lx\n", __FUNC__, __LINE__, PVResult));
		if (NULL == PVResult)
			break;

		// clear result
		memset(PVResult, 0, sizeof(*PVResult));

		ThumbnailWidth = GetTagData(SCALOSPREVIEW_ThumbnailWidth, 0, tagList);
		if (0 ==  ThumbnailWidth)
			break;
		ThumbnailHeight = GetTagData(SCALOSPREVIEW_ThumbnailHeight, 0, tagList);
		if (0 == ThumbnailHeight)
			break;

		SupportedColors = GetTagData(SCALOSPREVIEW_SupportedColors, 0, tagList);
		if (SupportedColors < 16)
			break;

		ReservedColors = GetTagData(SCALOSPREVIEW_ReservedColors, 0, tagList);
		if (SupportedColors < ReservedColors + 16)
			break;

		bmhdExternal = (struct BitMapHeader *) GetTagData(SCALOSPREVIEW_ImgBitMapHeader, (ULONG)NULL, tagList);
		quality = GetTagData(SCALOSPREVIEW_Quality, SCALOSPREVIEWA_Quality_Max, tagList);

		WBScreen = (struct Screen *) GetTagData(SCALOSPREVIEW_Screen, (ULONG)NULL, tagList);
		if (NULL == WBScreen)
			break;

		ScreenDepth = GetBitMapAttr(WBScreen->RastPort.BitMap, BMA_DEPTH);

		ImageObj = NewDTObject((STRPTR) iconName,
			DTA_SourceType, DTST_FILE,
			DTA_GroupID, GID_PICTURE,
			PDTA_DestMode, PMODE_V43,
			PDTA_Remap, FALSE,
			OBP_Precision, PRECISION_EXACT,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  ImageObj=%08lx\n", __FUNC__, __LINE__, ImageObj));
		if (NULL == ImageObj)
			break;

		if (IsPictureDT43(ImageObj))
			{
			d1(KPrintF(__FILE__ "/%s/%ld:  IsPictureDT43\n", __FUNC__, __LINE__));
			SetDTAttrs(ImageObj, NULL, NULL,
				PDTA_Screen, (ULONG) WBScreen,
				PDTA_UseFriendBitMap, ScreenDepth <= 8,	       /* +dm+ Do not use a friend bitmap for 8bit or less screens */
				TAG_END);
			}
		else
			{
			SetDTAttrs(ImageObj, NULL, NULL,
				PDTA_UseFriendBitMap, FALSE,
				TAG_END);
			}

		if (GetDTAttrs(ImageObj,
			PDTA_BitMapHeader, (ULONG) &bmhd,
			TAG_END) < 1)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld:  bmhd=%08lx\n", __FUNC__, __LINE__, bmhd));

		if (NULL == bmhd)
			break;

		if (bmhdExternal)
			*bmhdExternal = *bmhd;

		ScalosGfxCalculateScaleAspect(bmhd->bmh_Width, bmhd->bmh_Height,
			&ThumbnailWidth, &ThumbnailHeight);

		d1(KPrintF(__FILE__ "/%s/%ld:  ImageWidth=%ld  ImageHeight=%ld  ImageDepth=%ld\n", __FUNC__, __LINE__, bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth));
		d1(KPrintF(__FILE__ "/%s/%ld:  ThumbnailWidth=%ld  ThumbnailHeight=%ld\n", __FUNC__, __LINE__, ThumbnailWidth, ThumbnailHeight));

		if ((ScreenDepth <= 8) || (NULL == CyberGfxBase))
			{
			Success = GenerateRemappedThumbnail(wt,
				iconName, ImageObj,
				quality, ReservedColors,
                                ThumbnailWidth, ThumbnailHeight,
				SupportedColors, WBScreen, PVResult);
			}
		else
			{
			Success = GenerateARGBThumbnail(wt,
				iconName, ImageObj,
				quality, ReservedColors,
				ThumbnailWidth, ThumbnailHeight,
				SupportedColors, PVResult);
			}

#if defined(__MORPHOS__)
		if (PVResult && PVResult->spvr_sac && PVResult->spvr_sac->sac_BitMap)
			{
			BitMapCopy = *PVResult->spvr_sac->sac_BitMap;
			}
#endif /* __MORPHOS__ */
		} while (0);

	if (ImageObj)
		DisposeDTObject(ImageObj);

#if defined(__MORPHOS__)
	if (PVResult && PVResult->spvr_sac && PVResult->spvr_sac->sac_BitMap)
		{
		// *** work-around ***
		// ATM (MorphOS 1.4.5), some datatypes (e.g. GIF and BMP) have a serous bug
		// that causes the last byte of sac_BitMap to be decremented by 1 when the
		// datatypes object is disposed.

		d1(KPrintF(__FILE__ "/%s/%ld:  sac_BitMap=%08lx\n", __FUNC__, __LINE__, PVResult->spvr_sac->sac_BitMap));
		d1(ByteDump((unsigned char *) PVResult->spvr_sac->sac_BitMap, sizeof(struct BitMap)));

		if (0 != memcmp(&BitMapCopy, PVResult->spvr_sac->sac_BitMap, sizeof(struct BitMap)))
			*PVResult->spvr_sac->sac_BitMap = BitMapCopy;
		}
#endif /* __MORPHOS__ */

	return Success;
}
LIBFUNC_END

//----------------------------------------------------------------------------

#ifndef __amigaos4__

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

#ifndef __SASC
// Replacement for SAS/C library functions

void exit(int x)
{
   (void) x;
   while (1)
      ;
}

APTR _WBenchMsg;

#endif /* __SASC */

#endif /* __amigaos4__ */

//-----------------------------------------------------------------------------

static BOOL GenerateRemappedThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, Object *ImageObj,
	ULONG quality, ULONG ReservedColors,
	ULONG ScaledWidth, ULONG ScaledHeight,
	ULONG SupportedColors, struct Screen *WBScreen,
        struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
	struct ScalosBitMapAndColor *sac;

	d1(KPrintF(__FILE__ "/%s/%ld:  START\n", __FUNC__, __LINE__));

	do	{
		struct BitMapHeader *bmhd = NULL;
		struct BitMap *bm = NULL;
		ULONG Result;
		struct ScaleBitMapArg sbma;
		ULONG *ColorTable;

		sac = ScalosGfxCreateEmptySAC();
		if (NULL == sac)
			break;

		// SetDTAttrsA()
		SetDTAttrs(ImageObj,
			wt->wt_Window, NULL,
			PDTA_Remap, TRUE,
			TAG_END);

		if (GetDTAttrs(ImageObj,
			PDTA_DestBitMap, (ULONG) &bm,
			PDTA_BitMapHeader, (ULONG) &bmhd,
			PDTA_NumColors, (ULONG) &sac->sac_NumColors,
			PDTA_CRegs, (ULONG) &ColorTable,
			TAG_END) < 4)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld:  bm=%08lx  bmhd=%08lx\n", __FUNC__, __LINE__, bm, bmhd));
		if (NULL == bm)
			break;
		if (NULL == bmhd)
			break;
		if (sac->sac_NumColors > 256)
			break;
		if (NULL == sac->sac_ColorTable)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld:  ImageWidth=%ld  ImageHeight=%ld  ImageDepth=%ld\n", __FUNC__, __LINE__, bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth));
		d1(KPrintF(__FILE__ "/%s/%ld:  sac_NumColors=%ld  sac_ColorTable=%08lx\n", __FUNC__, __LINE__, sac->sac_NumColors, sac->sac_ColorTable));

		if (!ScalosGfxNewColorMap(sac, ColorTable, sac->sac_NumColors + ReservedColors))
			break;

		// DoDTMethodA()
		Result = DoDTMethod(ImageObj,
			NULL,
			NULL,
			DTM_PROCLAYOUT,
			NULL,
			TRUE);
		d1(KPrintF(__FILE__ "/%s/%ld:  Result=%08lx\n", __FUNC__, __LINE__, Result));
		if (!Result)
			break;;

		if (mskHasTransparentColor == bmhd->bmh_Masking)
			sac->sac_TransparentColor = bmhd->bmh_Transparent;
		else
			sac->sac_TransparentColor = SAC_TRANSPARENT_NONE;

		d1(KPrintF(__FILE__ "/%s/%ld:  bmh_Masking=%ld  sac_TransparentColor=%ld  bmh_Transparent=%ld\n", \
			__FUNC__, __LINE__,  bmhd->bmh_Masking, sac->sac_TransparentColor, bmhd->bmh_Transparent));

		sbma.sbma_SourceBM = bm;
		sbma.sbma_SourceWidth = bmhd->bmh_Width;
		sbma.sbma_SourceHeight = bmhd->bmh_Height;
		sbma.sbma_DestWidth = &ScaledWidth;
		sbma.sbma_DestHeight = &ScaledHeight;
		sbma.sbma_NumberOfColors = sac->sac_NumColors;
		sbma.sbma_ColorTable = sac->sac_ColorTable;
		sbma.sbma_ScreenBM = WBScreen->RastPort.BitMap;
		sbma.sbma_Flags = SCALEFLAGF_DOUBLESIZE | SCALEFLAGF_AVERAGE;

		if (quality >= (3 * SCALOSPREVIEWA_Quality_Max) / 4)
			{
			sbma.sbma_Flags |= SCALEFLAGF_BICUBIC;
			}
		else if (quality >= SCALOSPREVIEWA_Quality_Max / 2)
			{
			sbma.sbma_Flags |= SCALEFLAGF_BILINEAR;
			}

		sac->sac_BitMap = ScalosGfxScaleBitMap(&sbma, NULL);

		d1(KPrintF(__FILE__ "/%s/%ld:  sac_BitMap=%08lx\n", __FUNC__, __LINE__, sac->sac_BitMap));
		if (NULL == sac->sac_BitMap)
			break;

		sac->sac_Width = ScaledWidth;
		sac->sac_Height = ScaledHeight;
		sac->sac_Depth = GetBitMapAttr(sac->sac_BitMap, BMA_DEPTH);

		PVResult->spvr_sac = sac;
		sac = NULL;	// do not free sac now

		Success = TRUE;
		} while (0);

	ScalosGfxFreeSAC(sac);

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%08lx\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, Object *ImageObj,
	ULONG quality, ULONG ReservedColors,
        ULONG ScaledWidth, ULONG ScaledHeight,
	ULONG SupportedColors, struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
	struct ARGBHeader argbSrc = { 0, 0, NULL };
	struct ARGBHeader argbDest = { 0, 0, NULL };

	d1(KPrintF(__FILE__ "/%s/%ld:  START  iconName=<%s>\n", __FUNC__, __LINE__, iconName));

	do	{
		struct BitMapHeader *bmhd = NULL;
		struct BitMap *bm = NULL;
		PLANEPTR MaskPlane = NULL;
		const ULONG *CRegs = NULL;
		ULONG NumberOfColors;
		ULONG Result;
		ULONG ScaleFlags = SCALEFLAGF_DOUBLESIZE;

		if (quality >= (3 * SCALOSPREVIEWA_Quality_Max) / 4)
			{
			ScaleFlags |= SCALEFLAGF_BICUBIC;
			}
		else if (quality >= SCALOSPREVIEWA_Quality_Max / 2)
			{
			ScaleFlags |= SCALEFLAGF_BILINEAR;
			}
		else
			{
			ScaleFlags |= SCALEFLAGF_AVERAGE;
			}

		// DoDTMethodA()
		Result = DoDTMethod(ImageObj,
			NULL,
			NULL,
			DTM_PROCLAYOUT,
			NULL,
			TRUE);
		if (!Result)
			break;;

		if (GetDTAttrs(ImageObj,
			PDTA_DestBitMap, (ULONG) &bm,
			PDTA_BitMapHeader, (ULONG) &bmhd,
			TAG_END) < 2)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld:  bm=%08lx  bmhd=%08lx  bmh_Depth=%ld\n", __FUNC__, __LINE__, bm, bmhd, bmhd->bmh_Depth));
		if (NULL == bm)
			break;
		if (NULL == bmhd)
			break;

		argbSrc.argb_Width = bmhd->bmh_Width;
		argbSrc.argb_Height = bmhd->bmh_Height;

		(void) GetDTAttrs(ImageObj,
			PDTA_MaskPlane, (ULONG) &MaskPlane,
			TAG_END);

		d1(KPrintF(__FILE__ "/%s/%ld:  MaskPlane=%08lx\n", __FUNC__, __LINE__, MaskPlane));

		if (GetDTAttrs(ImageObj,
			PDTA_NumColors, (ULONG) &NumberOfColors,
			PDTA_CRegs, (ULONG) &CRegs,
			TAG_END) < 2)
			break;

		d1(KPrintF(__FILE__ "/%s/%ld:  NumberOfColors=%ld  CRegs=%08lx\n", __FUNC__, __LINE__, NumberOfColors, CRegs));
		d1(ByteDump((unsigned char *) CRegs, 128));

		if (bmhd->bmh_Depth <= 8)
			{
			argbSrc.argb_ImageData = ScalosGfxCreateARGBFromBitMap(bm,
				bmhd->bmh_Width, bmhd->bmh_Height,
				NumberOfColors, CRegs, MaskPlane);

			d1(KPrintF(__FILE__ "/%s/%ld:  argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbSrc.argb_ImageData));
			if (NULL == argbSrc.argb_ImageData)
				break;
			}
		else
			{
			BOOL setAlphaOpaque = FALSE;

			argbSrc.argb_ImageData = ScalosGfxCreateARGB(bmhd->bmh_Width, bmhd->bmh_Height, NULL);
			d1(KPrintF(__FILE__ "/%s/%ld:  argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbSrc.argb_ImageData));
			if (NULL == argbSrc.argb_ImageData)
				break;

			d1(KPrintF(__FILE__ "/%s/%ld:  bm=%08lx  Width=%ld  Height=%ld  Depth=%ld\n", \
				__FUNC__, __LINE__, bm, GetBitMapAttr(bm, BMA_WIDTH), \
				GetBitMapAttr(bm, BMA_HEIGHT), GetBitMapAttr(bm, BMA_DEPTH)));

			if (CyberGfxBase
				&& GetBitMapAttr(bm, BMA_DEPTH) > 24
                                && GetCyberMapAttr(bm, CYBRMATTR_ISCYBERGFX))
				{
				d1(KPrintF(__FILE__ "/%s/%ld:  MaskPlane=%08lx  bmh_Depth=%ld\n", __FUNC__, __LINE__, MaskPlane, bmhd->bmh_Depth));
                                
				if (bmhd->bmh_Depth > 24)
					{
					// for ARGB formats, use Alpha from BitMap instead of MaskPlane
					MaskPlane = NULL;
					}
				else
					{
					if (NULL == MaskPlane)
						setAlphaOpaque = TRUE;
					}
				}

		if (mskHasAlpha == bmhd->bmh_Masking)
			{
			DoMethod(ImageObj,
				PDTM_READPIXELARRAY,
				argbSrc.argb_ImageData,
				PBPAFMT_ARGB,
				bmhd->bmh_Width * sizeof(struct ARGB),
				0,
				0,
				bmhd->bmh_Width,
				bmhd->bmh_Height
				);
			}
		else
			{
			ScalosGfxFillARGBFromBitMap(&argbSrc, bm, MaskPlane);
			}

			if (setAlphaOpaque)
				ScalosGfxARGBSetAlpha(&argbSrc, ~0);
			}

		d1(ByteDump((unsigned char *) argbSrc.argb_ImageData, 128));
		d1(KPrintF(__FILE__ "/%s/%ld:  Result=%08lx\n", __FUNC__, __LINE__, Result));

		argbDest.argb_Width = ScaledWidth;
		argbDest.argb_Height = ScaledHeight;

		DUMP_ARGB_LINE(&argbSrc, Alpha, argbSrc.argb_Height / 2);

		argbDest.argb_ImageData = ScalosGfxScaleARGBArrayTags(&argbSrc,
			&argbDest.argb_Width,
			&argbDest.argb_Height,
			SCALOSGFX_ScaleARGBArrayFlags, ScaleFlags,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  argbDest.argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbDest.argb_ImageData));
		if (NULL == argbDest.argb_ImageData)
			break;

		if (!GenerateThumbnailFromARGB(wt,
			iconName, &argbDest,
			SupportedColors, ReservedColors,
                        PVResult))
			{
			break;
			}

		Success = TRUE;
		} while (0);

	ScalosGfxFreeARGB(&argbDest.argb_ImageData);
	ScalosGfxFreeARGB(&argbSrc.argb_ImageData);

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL GenerateThumbnailFromARGB(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, struct ARGBHeader *argbDest,
	ULONG SupportedColors, ULONG ReservedColors,
        struct ScalosPreviewResult *PVResult)
{
	d1(KPrintF(__FILE__ "/%s/%ld:  START  iconName=<%s>\n", __FUNC__, __LINE__, iconName));
	d1(KPrintF(__FILE__ "/%s/%ld:  argb_Width=%ld  argb_Height=%ld  ImageData=%08lx\n", \
		__FUNC__, __LINE__, argbDest->argb_Width, argbDest->argb_Height,\
		argbDest->argb_ImageData));
	d1(KPrintF(__FILE__ "/%s/%ld:  SupportedColors=%lu\n", __FUNC__, __LINE__, SupportedColors));

	if (SupportedColors <= 256)
		{
		ULONG Depth;

		// Limit color depth to the number of colors supported by the icon
		for (Depth = 0; Depth < 7 && (1 << Depth) < SupportedColors; Depth++)
			;

		d1(KPrintF(__FILE__ "/%s/%ld:  argb_ImageData: a=%ld  r=%ld  g=%ld  b=%ld\n", \
			__FUNC__, __LINE__, argbDest->argb_ImageData->Alpha, \
			argbDest->argb_ImageData->Red, argbDest->argb_ImageData->Green, \
			argbDest->argb_ImageData->Blue));

		d1(KPrintF(__FILE__ "/%s/%ld:  SupportedColors=%lu  Depth=%lu  ReservedColors=%lu\n", \
			__FUNC__, __LINE__, SupportedColors, Depth, ReservedColors));

		PVResult->spvr_sac = ScalosGfxMedianCutTags(argbDest, Depth,
			SCALOSGFX_MedianCutFlags, SCALOSGFXFLAGF_MedianCut_FloydSteinberg,
			SCALOSGFX_MedianCutFriendBitMap, NULL,
			SCALOSGFX_MedianCutReservedColors, ReservedColors,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  sac=%08lx\n", __FUNC__, __LINE__, PVResult->spvr_sac));

		d1(KPrintF(__FILE__ "/%s/%ld:  sac_BitMap=%08lx\n", __FUNC__, __LINE__, PVResult->spvr_sac->sac_BitMap));
		d1(ByteDump((unsigned char *) PVResult->spvr_sac->sac_BitMap, sizeof(struct BitMap)));
                }
	else
		{
		DUMP_ARGB_LINE(argbDest, Alpha, argbDest->argb_Height / 2)
		PVResult->spvr_ARGBheader = *argbDest;
		memset(argbDest, 0, sizeof(*argbDest));
		}

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	if (argbDest)
		{
		ScalosGfxFreeARGB(&argbDest->argb_ImageData);
		}

	d1(KPrintF(__FILE__ "/%s/%ld:  END\n", __FUNC__, __LINE__));

	return TRUE;
}

//-----------------------------------------------------------------------------

// Work-around for OS3.0/3.1 and Picasso96 picture.datatype V43.41
// This datatype seems to understand PMODE_V43, but in combination with
// PDTA_UseFriendBitMap and PDTA_Screen, returns a very weird BitMap
// that I was unable to deal with: Depth=16, but no CyberGfx BitMap
// Looks like an extended standard BitMap with 16 plane pointers.

static BOOL IsPictureDT43(Object *o)
{
	const ULONG *MethodArray = NULL;
	BOOL IsV43 = FALSE;

	GetDTAttrs(o,
		DTA_Methods, (ULONG) &MethodArray,
		TAG_END);

	while (MethodArray && ~0 != *MethodArray)
		{
		if (PDTM_SCALE == *MethodArray)
			{
			IsV43 = TRUE;
			break;
			}
		MethodArray++;
		}

	return IsV43;
}

//-----------------------------------------------------------------------------

#if BYTEDUMP
static void ByteDump(const unsigned char *Data, size_t Length)
{
	unsigned long count;
	unsigned char Line[80], *lp;

	lp = Line;
	for (count=0; count<Length; count++)
		{
		*lp++ = isprint(*Data) ? *Data : '.';
		KPrintF("%02lx ", (ULONG) *Data++);
		if ((count+1) % 16 == 0)
			{
			*lp = '\0';
			lp = Line;
			KPrintF("\t%s\n", Line);
			}
		}

	*lp = '\0';
	while (count % 16 != 0)
		{
		KPrintF("   ");
		count++;
		}
	KPrintF("\t%s\n", Line);
}
#endif /* BYTEDUMP */

//-----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
