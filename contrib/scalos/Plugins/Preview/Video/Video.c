// Video.c
// $Date$
// $Revision$

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <graphics/gfxbase.h>
#include <scalos/scalos.h>
#include <scalos/scalosgfx.h>
#include <datatypes/pictureclass.h>
#include <scalos/scalospreviewplugin.h>
#include <cybergraphx/cybergraphics.h>
#include <utility/hooks.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/scalosgfx.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <defs.h>

#undef	GLOBAL

#include <libavcodec/avcodec.h>
#include <libavcodec/imgconvert.h>
#include <avformat.h>
#include <swscale.h>

#include "Video.h"

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

#define	NO_STREAM	-1

struct AVInfo
	{
	int           	avinf_VideoStream;
	AVFormatContext	*avinf_pFormatContext;
	AVCodecContext	*avinf_pCodecContext;
	AVCodec		*avinf_pVideoCodec;
	AVStream	*avinf_pVideoStream;
	AVFrame		*avinf_pFrame;		// Video frame in native format
	AVFrame		*avinf_pRGBFrame;	// Converted Video frame in RGB format
	UBYTE		*avinf_Buffer;		// Buffer for RGB image data
	BOOL		avinf_CodecOpened;	// Flag: av_codec_open() succeeded
	};

struct VideoFrame
{
	int vfr_width;
	int vfr_height;
	int vfr_lineSize;
};

//---------------------------------------------------------------------------------------

#ifndef __AROS__
struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct Library *CyberGfxBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
#endif
struct Library *CyberGfxBase;
struct ScalosGfxBase *ScalosGfxBase;

#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct GraphicsIFace *IGraphics;
struct UtilityIFace *IUtility;
struct ExecIFace *IExec;
struct CyberGfxIFace *ICyberGfx;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct ScalosGfxIFace *IScalosGfx;
#endif /* __amigaos4__ */

/* won't compile for me when defined as constants - mazze */
#define FILMHOLE_WIDTH (12)
#define FILMHOLE_HEIGHT (10)

STRPTR VersTag = LIB_VERSTRING;

//---------------------------------------------------------------------------------------

static BOOL GenerateRemappedThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName,
	struct AVInfo *avInfo, struct VideoFrame *vFrame,
	ULONG quality, ULONG SupportedColors, ULONG ReservedColors,
	ULONG ScaledWidth, ULONG ScaledHeight,
	struct Screen *WBScreen, struct ScalosPreviewResult *PVResult);
static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName,
	struct AVInfo *avInfo, struct VideoFrame *vFrame,
	ULONG quality,
        ULONG ScaledWidth, ULONG ScaledHeight,
	struct ScalosPreviewResult *PVResult);
static void CleanupAVInfo(struct AVInfo *avInfo);
static BOOL InitializeVideo(struct AVInfo *avInfo);
static BOOL ReadVideoFrame(struct AVInfo *avInfo);
static BOOL GetScaledVideoFrame(struct AVInfo *avInfo, int scaledSize,
	ULONG quality, struct VideoFrame *videoFrame);
static BOOL ConvertAndScaleFrame(struct AVInfo *avInfo, int format, int scaledSize,
	int *scaledWidth, int *scaledHeight, ULONG quality);
static void CalculateDimensions(int srcWidth, int srcHeight, int squareSize, int *destWidth, int *destHeight);
static BOOL AllocFrameData(AVFrame **avFrame, int width, int height, int format);
static BOOL VideoSeekFrame(struct AVInfo *avInfo, float timeInSeconds);
static void OverlayFilmStrip(struct AVInfo *avInfo, struct VideoFrame *videoFrame);
static BOOL IsInterestingFrame(const struct AVInfo *avInfo);
static BOOL FindInterestingFrame(struct AVInfo *avInfo);
static void av_log_dummy_callback(void* ptr, int level, const char* fmt, va_list vl);

#if BYTEDUMP
static void ByteDump(const unsigned char *Data, size_t Length);
#endif /* BYTEDUMP */

//-----------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	BOOL result = FALSE;

	d1(KPrintF("%s/%s/%ld:   PluginBase=%08lx  procName=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		PluginBase, FindTask(NULL)->tc_Node.ln_Name));

	do	{
#ifndef __amigaos4__
		if (_STI_240_InitMemFunctions())
			break;
#endif /* __amigaos4__ */
		DOSBase  = (APTR) OpenLibrary( "dos.library", 39 );
		if (NULL == DOSBase)
			break;

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
		if (NULL == UtilityBase)
			break;

		CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
		// CyberGfxBase may be NULL

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		d1(kprintf("%s/%s/%ld: IntuitionBase=%08lx\n", __FILE__, __FUNC__, __LINE__, IntuitionBase));
		if (NULL == IntuitionBase)
			break;

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
		d1(kprintf("%s/%s/%ld: GfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, GfxBase));
		if (NULL == GfxBase)
			break;

		ScalosGfxBase = (struct ScalosGfxBase *) OpenLibrary(SCALOSGFXNAME, 41);
		d1(KPrintF("%s/%s/%ld: ScalosGfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosGfxBase));
		if (NULL == ScalosGfxBase)
			break;

#ifdef __amigaos4__
		IDOS = (struct DOSIFace *) GetInterface((struct Library *)DOSBase, "main", 1, NULL);
		if (IDOS == NULL)
			break;
		if (CyberGfxBase != NULL)       // CyberGfxBase may be NULL
			{
			ICyberGfx = (struct CyberGfxIFace *) GetInterface((struct Library *) CyberGfxBase, "main", 1, NULL);
			if (ICyberGfx == NULL)
				break;
			}
		IUtility = (struct UtilityIFace *) GetInterface((struct Library *) UtilityBase, "main", 1, NULL);
		if (NULL == IUtility)
			break;
		IIntuition = (struct IntuitionIFace *) GetInterface((struct Library *) IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			break;
		IGraphics = (struct GraphicsIFace *) GetInterface((struct Library *) GfxBase, "main", 1, NULL);
		if (NULL == IGraphics)
			break;
		IScalosGfx = (struct ScalosGfxIFace *) GetInterface((struct Library *) ScalosGfxBase, "main", 1, NULL);
		if (NULL == IScalosGfx)
			break;
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;
#endif /* __amigaos4__ */

		if (!InitExtra())
			break;

		av_register_all();

		result = TRUE;
		d1(KPrintF("%s/%s/%ld: Success\n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	return result;
}


void closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	ExitExtra();

#ifndef __amigaos4__
	_STD_240_TerminateMemFunctions();
#endif

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
}

//--------------------------------------------------------------------------

LIBFUNC_P5(LONG, LIBSCAPreviewGenerate,
	A0, struct ScaWindowTask *, wt,
	A1, BPTR, dirLock,
	A2, CONST_STRPTR, iconName,
	A3, struct TagItem *, tagList,
	A6, struct PluginBase *, PluginBase, 5)
{
	LONG Success = FALSE;

	(void) PluginBase;

	if (InitInstance())
		{
		BPTR oldDir = (BPTR)NULL;
		struct AVInfo avInfo;
		struct VideoFrame vFrame;

		memset(&vFrame, 0, sizeof(struct VideoFrame));

		d1(KPrintF("%s/%s/%ld:  START iconName=<%s>\n", __FILE__, __FUNC__, __LINE__, iconName));

		do	{
			ULONG ThumbnailWidth;
			ULONG ThumbnailHeight;
			ULONG SupportedColors;
			ULONG ScreenDepth;
			struct Screen *WBScreen;
			struct BitMapHeader *bmhdExternal;
			struct ScalosPreviewResult *PVResult;
			BOOL DoRemap;
			ULONG quality;
			ULONG ReservedColors;
			int rc;

			memset(&avInfo, 0, sizeof(struct AVInfo));

			av_log_set_callback(av_log_dummy_callback);

			d1(KPrintF("%s/%s/%ld:  after av_log_set_callback\n", __FILE__, __FUNC__, __LINE__));

			PVResult = (struct ScalosPreviewResult *) GetTagData(SCALOSPREVIEW_ResultStruct, (ULONG)NULL, tagList);
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

			oldDir = CurrentDir(dirLock);

			d1(KPrintF("%s/%s/%ld:  before av_open_input_file\n", __FILE__, __FUNC__, __LINE__));

			// Open video file
			rc = av_open_input_file(&avInfo.avinf_pFormatContext, iconName, NULL, 0, NULL);
			d1(KPrintF("%s/%s/%ld:  rc=%d\n", __FILE__, __FUNC__, __LINE__, rc));
			if (0 != rc)
				{
				d1(KPrintF("%s/%s/%ld:  av_open_input_file failed\n", __FILE__, __FUNC__, __LINE__));
				break;
				}

			d1(KPrintF("%s/%s/%ld:  before InitializeVideo\n", __FILE__, __FUNC__, __LINE__));

			if (!InitializeVideo(&avInfo))
				break;

			d1(KPrintF("%s/%s/%ld:  after InitializeVideo\n", __FILE__, __FUNC__, __LINE__));

			if (!ReadVideoFrame(&avInfo)) 	      //before seeking, a frame has to be decoded
				break;

			d1(KPrintF(__FILE__ "%s/%s/%ld:  ReadVideoFrame success\n", __FILE__, __FUNC__, __LINE__));

			if (bmhdExternal)
				{
#if 1
				bmhdExternal->bmh_Width  = avInfo.avinf_pCodecContext->width;
				bmhdExternal->bmh_Height = avInfo.avinf_pCodecContext->height;
#else
				bmhdExternal->bmh_Width  = 2;
				bmhdExternal->bmh_Height = 2;
#endif
				bmhdExternal->bmh_Depth  = 24;
				}

			if (!FindInterestingFrame(&avInfo))
				break;

			if (!GetScaledVideoFrame(&avInfo, ThumbnailWidth, quality, &vFrame))
				break;
			
			d1(KPrintF("%s/%s/%ld:  vfr_width=%ld  vfr_height=%ld  vfr_lineSize=%ld\n", __FILE__, __FUNC__, __LINE__, vFrame.vfr_width, vFrame.vfr_height, vFrame.vfr_lineSize));

			if (vFrame.vfr_width > FILMHOLE_WIDTH * 2)
				{
				OverlayFilmStrip(&avInfo, &vFrame);
				}

			ScreenDepth = GetBitMapAttr(WBScreen->RastPort.BitMap, BMA_DEPTH);

			DoRemap = (ScreenDepth <= 8) || (NULL == CyberGfxBase) || SupportedColors <= 256;

			d1(KPrintF("%s/%s/%ld:  DoRemap=%ld\n", __FILE__, __FUNC__, __LINE__, DoRemap));

			if (DoRemap)
				{
				d1(KPrintF("%s/%s/%ld:  SupportedColors=%lu  ReservedColors=%ld\n", \
					__FILE__, __FUNC__, __LINE__, SupportedColors, ReservedColors));

				Success = GenerateRemappedThumbnail(wt,
					iconName,
					&avInfo, &vFrame,
					quality, SupportedColors, ReservedColors,
					ThumbnailWidth, ThumbnailHeight,
					WBScreen, PVResult);
				}
			else
				{
				Success = GenerateARGBThumbnail(wt,
					iconName,
					&avInfo, &vFrame,
	                                quality,
					ThumbnailWidth, ThumbnailHeight,
					PVResult);
				}
			} while (0);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		if (oldDir)
			CurrentDir(oldDir);

		CleanupAVInfo(&avInfo);

		ExitInstance();
		}

	d1(KPrintF("%s/%s/%ld:  END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

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
	CONST_STRPTR iconName,
	struct AVInfo *avInfo, struct VideoFrame *vFrame,
	ULONG quality, ULONG SupportedColors, ULONG ReservedColors,
	ULONG ScaledWidth, ULONG ScaledHeight,
	struct Screen *WBScreen, struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
	struct ARGBHeader argbTemp = { 0, 0, NULL };
	struct ScalosBitMapAndColor *sacSrc = NULL;
	struct ScalosBitMapAndColor *sacDest = NULL;
	struct BitMap *TempBM = NULL;

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct ARGB *pLine;
		const UBYTE *src;
		ULONG Depth;
		ULONG y;

		argbTemp.argb_Width = vFrame->vfr_width;
		argbTemp.argb_Height = vFrame->vfr_height;
		argbTemp.argb_ImageData = ScalosGfxCreateARGB(argbTemp.argb_Width, argbTemp.argb_Height, NULL);
		d1(KPrintF("%s/%s/%ld:  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbTemp.argb_ImageData));
		if (NULL == argbTemp.argb_ImageData)
			break;

		// Fill argbSrc with image from vFrame
		pLine = argbTemp.argb_ImageData;
		src = avInfo->avinf_pFrame->data[0];
		for (y = 0; y < vFrame->vfr_height; y++)
			{
			ULONG x;

			for (x = 0; x < vFrame->vfr_width; x++)
				{
				pLine->Red = *src++;
				pLine->Green = *src++;
				pLine->Blue = *src++;
				pLine->Alpha = (UBYTE) ~0;

				pLine++;
				}
			}

		// Limit color depth to the number of colors supported by the icon
		for (Depth = 0; Depth < 7 && (1 << Depth) < SupportedColors; Depth++)
			;

		d1(KPrintF(__FILE__ "/%s/%ld:  argb_ImageData: a=%ld  r=%ld  g=%ld  b=%ld\n", \
			__FUNC__, __LINE__, argbTemp.argb_ImageData->Alpha, \
			argbTemp.argb_ImageData->Red, argbTemp.argb_ImageData->Green, \
			argbTemp.argb_ImageData->Blue));

		d1(KPrintF(__FILE__ "/%s/%ld:  SupportedColors=%lu  Depth=%lu  ReservedColors=%lu\n", \
			__FUNC__, __LINE__, SupportedColors, Depth, ReservedColors));

		PVResult->spvr_sac = ScalosGfxMedianCutTags(&argbTemp, Depth,
			SCALOSGFX_MedianCutFlags, SCALOSGFXFLAGF_MedianCut_FloydSteinberg,
			SCALOSGFX_MedianCutFriendBitMap, NULL,
			SCALOSGFX_MedianCutReservedColors, ReservedColors,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  sac=%08lx\n", __FUNC__, __LINE__, PVResult->spvr_sac));

		d1(KPrintF(__FILE__ "/%s/%ld:  sac_BitMap=%08lx\n", __FUNC__, __LINE__, PVResult->spvr_sac->sac_BitMap));
		d1(ByteDump((unsigned char *) PVResult->spvr_sac->sac_BitMap, sizeof(struct BitMap)));

		Success = TRUE;
		} while (0);

	if (TempBM)
		FreeBitMap(TempBM);

	ScalosGfxFreeARGB(&argbTemp.argb_ImageData);
	ScalosGfxFreeSAC(sacSrc);
	ScalosGfxFreeSAC(sacDest);

	d1(KPrintF("%s/%s/%ld:  END Success=%08lx\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName,
	struct AVInfo *avInfo, struct VideoFrame *vFrame,
	ULONG quality,
        ULONG ScaledWidth, ULONG ScaledHeight,
	struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
	struct ARGBHeader argbSrc = { 0, 0, NULL };

	d1(KPrintF("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct ARGB *pLine;
		const UBYTE *src;
		ULONG y;

		argbSrc.argb_Width = vFrame->vfr_width;
		argbSrc.argb_Height = vFrame->vfr_height;
		argbSrc.argb_ImageData = ScalosGfxCreateARGB(argbSrc.argb_Width, argbSrc.argb_Height, NULL);
		d1(KPrintF("%s/%s/%ld:  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbSrc.argb_ImageData));
		if (NULL == argbSrc.argb_ImageData)
			break;

		// Fill argbSrc with image from vFrame
		pLine = argbSrc.argb_ImageData;
		src = avInfo->avinf_pFrame->data[0];
		for (y = 0; y < vFrame->vfr_height; y++)
			{
			ULONG x;

			for (x = 0; x < vFrame->vfr_width; x++)
				{
				pLine->Red = *src++;
				pLine->Green = *src++;
				pLine->Blue = *src++;
				pLine->Alpha = (UBYTE) ~0;

				pLine++;
				}
			}

		PVResult->spvr_ARGBheader = argbSrc;
		memset(&argbSrc, 0, sizeof(argbSrc));

		Success = TRUE;
		} while (0);

	if (!Success)
		ScalosGfxFreeARGB(&argbSrc.argb_ImageData);

	d1(KPrintF("%s/%s/%ld:  END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static void CleanupAVInfo(struct AVInfo *avInfo)
{
	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));

	d1(KPrintF("%s/%s/%ld:  avinf_pCodecContext=%08lx  avinf_CodecOpened=%ld\n", __FILE__, __FUNC__, __LINE__, \
		avInfo->avinf_pCodecContext, avInfo->avinf_CodecOpened));
	if (avInfo->avinf_CodecOpened)
		{
		avcodec_close(avInfo->avinf_pCodecContext);
		avInfo->avinf_pCodecContext = NULL;
		avInfo->avinf_CodecOpened = FALSE;
		}
	d1(KPrintF("%s/%s/%ld:  avinf_pFormatContext=%08lx\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_pFormatContext));
	if (avInfo->avinf_pFormatContext)
		{
		av_close_input_file(avInfo->avinf_pFormatContext);
		avInfo->avinf_pFormatContext = NULL;
		}
	d1(KPrintF("%s/%s/%ld:  avinf_pFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_pFrame));
	if (avInfo->avinf_pFrame)
		{
		av_free(avInfo->avinf_pFrame);
		avInfo->avinf_pFrame = NULL;
		}
	d1(KPrintF("%s/%s/%ld:  avinf_pRGBFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_pRGBFrame));
	if (avInfo->avinf_pRGBFrame)
		{
		av_free(avInfo->avinf_pRGBFrame);
		avInfo->avinf_pRGBFrame = NULL;
		}
	d1(KPrintF("%s/%s/%ld:  avinf_Buffer=%08lx\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_Buffer));
	if (avInfo->avinf_Buffer)
		{
		free(avInfo->avinf_Buffer);
		avInfo->avinf_Buffer = NULL;
		}
	d1(KPrintF("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------------

static BOOL InitializeVideo(struct AVInfo *avInfo)
{
	BOOL Success = FALSE;

	do	{
		d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

		// Retrieve stream information
		if (av_find_stream_info(avInfo->avinf_pFormatContext) < 0)
			{
			d1(KPrintF("%s/%s/%ld:  av_find_stream_info failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld: nb_streams=%ld\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_pFormatContext->nb_streams));
		if (avInfo->avinf_pFormatContext->nb_streams < 1)
			{
			d1(KPrintF("%s/%s/%ld:  no streams found\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		// Find the first video stream
		avInfo->avinf_VideoStream = av_find_default_stream_index(avInfo->avinf_pFormatContext);

		d1(KPrintF("%s/%s/%ld: avinf_VideoStream=%ld\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_VideoStream));
		if (NO_STREAM == avInfo->avinf_VideoStream)
			{
			d1(KPrintF("%s/%s/%ld: Could not find video stream\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		avInfo->avinf_pVideoStream = avInfo->avinf_pFormatContext->streams[avInfo->avinf_VideoStream];

		// Remember pointer to codec context for the video stream
		avInfo->avinf_pCodecContext = avInfo->avinf_pVideoStream->codec;

		// Find decoder for the video stream
                avInfo->avinf_pVideoCodec = avcodec_find_decoder(avInfo->avinf_pCodecContext->codec_id);

		d1(KPrintF("%s/%s/%ld: avinf_pCodecContext=%08lx  avinf_pVideoCodec=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, avInfo->avinf_pCodecContext, avInfo->avinf_pVideoCodec));
		if (NULL == avInfo->avinf_pVideoCodec)
			{
			d1(KPrintF("%s/%s/%ld: Video Codec not found\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld: Codec name=<%s>\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_pVideoCodec->name));

		avInfo->avinf_pCodecContext->workaround_bugs = 1;
		avInfo->avinf_pFormatContext->flags |= AVFMT_FLAG_GENPTS;
		if(avInfo->avinf_pVideoCodec->capabilities & CODEC_CAP_TRUNCATED)
			avInfo->avinf_pCodecContext->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

#if 1
		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  pix_fmt=%ld\n", __FILE__, __FUNC__, __LINE__, \
			avInfo->avinf_pCodecContext->width, avInfo->avinf_pCodecContext->height, \
                        avInfo->avinf_pCodecContext->pix_fmt));

		if (0 == avInfo->avinf_pCodecContext->width)
			{
			break;
			}
#endif
		// Open codec
		if (avcodec_open(avInfo->avinf_pCodecContext, avInfo->avinf_pVideoCodec) < 0)
			{
			d1(KPrintF("%s/%s/%ld: avcodec_open failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}
		avInfo->avinf_CodecOpened = TRUE;

		// Allocate video frame
		avInfo->avinf_pFrame = avcodec_alloc_frame();
		d1(KPrintF("%s/%s/%ld:  avinf_pFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, avInfo->avinf_pFrame));
		if (NULL == avInfo->avinf_pFrame)
			break;

		if (!AllocFrameData(&avInfo->avinf_pRGBFrame,
			avInfo->avinf_pCodecContext->width,
			avInfo->avinf_pCodecContext->height,
			PIX_FMT_RGB24))
			{
			break;
			}

                Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: END  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL ReadVideoFrame(struct AVInfo *avInfo)
{
	AVPacket    packet;
	int frameFinished = FALSE;
	ULONG Tries = 0;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	av_init_packet(&packet);

	while (av_read_frame(avInfo->avinf_pFormatContext, &packet) >= 0)
		{
		// Is this a packet from the video stream?
		if (packet.stream_index == avInfo->avinf_VideoStream)
			{
			int size = packet.size;
			UBYTE *buffer = packet.data;

			while ((size > 0) && !frameFinished)
				{
				int bytesDecoded;

				bytesDecoded = avcodec_decode_video(avInfo->avinf_pCodecContext,
					avInfo->avinf_pFrame,
			                &frameFinished,
					buffer,
					size);

				d1(KPrintF("%s/%s/%ld: packet.size=%d  frameFinished=%ld  bytesDecoded=%ld\n", \
					__FILE__, __FUNC__, __LINE__, packet.size, frameFinished, bytesDecoded));
				d1(KPrintF("%s/%s/%ld: pix_fmt=%d  width=%ld  height=%ld\n", \
					__FILE__, __FUNC__, __LINE__, avInfo->avinf_pCodecContext->pix_fmt, \
					avInfo->avinf_pCodecContext->width, avInfo->avinf_pCodecContext->height));

				if (bytesDecoded < 0)
					break;	// failure

				if (frameFinished && (bytesDecoded >= 0))
					{
					struct SwsContext *ImgConvertContext;
					int rc;

					d1(	{ \
						char buffer[80]; \
						sprintf(buffer, "pts=%llu  dts=%llu", packet.pts, packet.dts);
						KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buffer); \
						} );

					// Convert frame image from native format to RGB
					ImgConvertContext = sws_getContext(avInfo->avinf_pCodecContext->width,
						avInfo->avinf_pCodecContext->height,
						avInfo->avinf_pCodecContext->pix_fmt,
						avInfo->avinf_pCodecContext->width,
						avInfo->avinf_pCodecContext->height,
						PIX_FMT_RGB24,
						SWS_FAST_BILINEAR,
						NULL,
						NULL,
						NULL);

					if (NULL == ImgConvertContext)
						break;

					rc = sws_scale(ImgConvertContext,
						avInfo->avinf_pFrame->data,
						avInfo->avinf_pFrame->linesize,
						0,
						avInfo->avinf_pCodecContext->height,
						avInfo->avinf_pRGBFrame->data,
						avInfo->avinf_pRGBFrame->linesize);

					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					sws_freeContext(ImgConvertContext);

					if (-1 == rc)
						break;	// failure
					break;
					}

				buffer += bytesDecoded;
				size -= bytesDecoded;

				if (Tries++ > 1000)
					break;
				}
			}

		av_free_packet(&packet);

		if (Tries++ > 1000)
			break;
		}

	d1(KPrintF("%s/%s/%ld: frameFinished=%ld\n", __FILE__, __FUNC__, __LINE__, frameFinished));

	av_free_packet(&packet);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return (BOOL) frameFinished;
}

//-----------------------------------------------------------------------------

static BOOL GetScaledVideoFrame(struct AVInfo *avInfo, int scaledSize,
	ULONG quality, struct VideoFrame *videoFrame)
{
	BOOL Success = FALSE;

	do	{
		int scaledWidth, scaledHeight;

		if (!ConvertAndScaleFrame(avInfo, PIX_FMT_RGB24, scaledSize,
			&scaledWidth, &scaledHeight, quality))
			break;

		d1(KPrintF("%s/%s/%ld: ConvertAndScaleFrame success\n", __FILE__, __FUNC__, __LINE__));

		videoFrame->vfr_width = scaledWidth;
		videoFrame->vfr_height = scaledHeight;
		videoFrame->vfr_lineSize = avInfo->avinf_pFrame->linesize[0];

		d1(KPrintF("%s/%s/%ld: vfr_width=%ld  vfr_height=%ld  vfr_lineSize=%ld\n", __FILE__, __FUNC__, __LINE__, \
			videoFrame->vfr_width, videoFrame->vfr_height, videoFrame->vfr_lineSize));

		Success = TRUE;
		} while (0);

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL ConvertAndScaleFrame(struct AVInfo *avInfo, int pixFormat,
	int scaledSize, int *scaledWidth, int *scaledHeight, ULONG quality)
{
	AVFrame *convertedFrame = NULL;
	struct SwsContext *scaleContext;
	BOOL Success = FALSE;

	do	{
		int ScaleFlags;

		if (quality > 10)
			ScaleFlags = SWS_BICUBIC;
		else if (quality > 5)
			ScaleFlags = SWS_BILINEAR;
		else
			ScaleFlags = SWS_FAST_BILINEAR;

		CalculateDimensions(avInfo->avinf_pCodecContext->width,
			avInfo->avinf_pCodecContext->height,
			scaledSize,
			scaledWidth,
	                scaledHeight);
		d1(KPrintF("%s/%s/%ld: width=%ld  height=%ld\n", __FILE__, __FUNC__, __LINE__,
			avInfo->avinf_pCodecContext->width, avInfo->avinf_pCodecContext->height));
		d1(KPrintF("%s/%s/%ld: scaledWidth=%ld  scaledHeight=%ld\n", \
			__FILE__, __FUNC__, __LINE__, *scaledWidth, *scaledHeight));

		d1({								\
		ULONG n;							\
		const UBYTE *p = avInfo->avinf_pFrame->data[0];			\
										\
		for (n = 0; n < avInfo->avinf_pCodecContext->height; n++)       \
			{                                                       \
			ByteDump(p, avInfo->avinf_pFrame->linesize[0]);         \
			p += avInfo->avinf_pFrame->linesize[0];                 \
			}                                                       \
		});

		scaleContext = sws_getContext(avInfo->avinf_pCodecContext->width,
			avInfo->avinf_pCodecContext->height,
			avInfo->avinf_pCodecContext->pix_fmt,
			*scaledWidth,
			*scaledHeight,
			pixFormat,
			ScaleFlags,
			NULL,
			NULL,
	                NULL);

		d1(KPrintF("%s/%s/%ld: scaleContext=%08lx\n", __FILE__, __FUNC__, __LINE__, scaleContext));
		if (NULL == scaleContext)
			break;

		if (!AllocFrameData(&convertedFrame, *scaledWidth, *scaledHeight, pixFormat))
			break;

		d1(KPrintF("%s/%s/%ld: convertedFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, convertedFrame));
		if (NULL == convertedFrame)
			break;

		sws_scale(scaleContext,
			avInfo->avinf_pFrame->data,
			avInfo->avinf_pFrame->linesize,
			0,
			avInfo->avinf_pCodecContext->height,
			convertedFrame->data,
			convertedFrame->linesize);

		Success = TRUE;
		} while (0);

	if (scaleContext)
		sws_freeContext(scaleContext);

	if (Success)
		{
		av_free(avInfo->avinf_pFrame);
		avInfo->avinf_pFrame = convertedFrame;
		}

	return Success;
}

//-----------------------------------------------------------------------------

static void CalculateDimensions(int srcWidth, int srcHeight, int squareSize, int *destWidth, int *destHeight)
{
	if (srcWidth > srcHeight)
		{
		*destWidth  = squareSize;
		*destHeight = (int) ((float)(squareSize) / srcWidth * srcHeight);
		}
	else
		{
		*destWidth  = (int)((float)(squareSize) / srcHeight * srcWidth);
		*destHeight = squareSize;
		}
}

//-----------------------------------------------------------------------------

static BOOL AllocFrameData(AVFrame **avFrame, int width, int height, int pixFormat)
{
	int numBytes;

	*avFrame = avcodec_alloc_frame();
	d1(KPrintF("%s/%s/%ld: *avFrame=%08lx\n", __FILE__, __FUNC__, __LINE__, *avFrame));
	if (NULL == *avFrame)
		return FALSE;

	numBytes = avpicture_get_size(pixFormat, width, height);
	d1(KPrintF("%s/%s/%ld: numBytes=%ld\n", __FILE__, __FUNC__, __LINE__, numBytes));

	if (0 != avpicture_alloc((AVPicture *) *avFrame, pixFormat, width, height))
		{
		d1(KPrintF("%s/%s/%ld: avpicture_alloc failed\n", __FILE__, __FUNC__, __LINE__));
		return FALSE;
		}

	return TRUE;
}

//-----------------------------------------------------------------------------

static BOOL VideoSeekFrame(struct AVInfo *avInfo, float timeInSeconds)
{
	int rc;
//	  int64_t timestamp = (int64_t) (10 * AV_TIME_BASE * timeInSeconds);
        int64_t timestamp = (int64_t) (AV_TIME_BASE * timeInSeconds);

	do	{
		int count = 0;
		BOOL foundFrame;

		d1(	{ \
			char buffer[40]; \
			sprintf(buffer, "timeInSeconds=%f  timestamp=%llu", timeInSeconds, timestamp); \
			KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buffer); \
			} );

		rc = av_seek_frame(avInfo->avinf_pFormatContext, -1, timestamp, 0);
		d1(KPrintF("%s/%s/%ld: rc=%ld\n", __FILE__, __FUNC__, __LINE__, rc));
		if (rc < 0)
			break;

		do      {
			foundFrame = ReadVideoFrame(avInfo);

			d1(KPrintF("%s/%s/%ld: foundFrame=%ld  count=%ld\n", __FILE__, __FUNC__, __LINE__, foundFrame, ++count));
			} while (!foundFrame /* && !avInfo->avinf_pFrame->key_frame */ && ++count < 20);

		d1(	{ \
			char buffer[80]; \

			sprintf(buffer, "pts=%f  coded_picture_number=%d  display_picture_number=%d", \
				(float) (avInfo->avinf_pFrame->pts * avInfo->avinf_pCodecContext->time_base.num) \
				/ (float) avInfo->avinf_pCodecContext->time_base.den, \
				avInfo->avinf_pFrame->coded_picture_number, \
                                avInfo->avinf_pFrame->display_picture_number); \
			KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buffer); \
			} );
		} while (0);

	return TRUE;
}

//-----------------------------------------------------------------------------

static void OverlayFilmStrip(struct AVInfo *avInfo, struct VideoFrame *videoFrame)
{
	static const UBYTE filmHole[FILMHOLE_WIDTH * FILMHOLE_HEIGHT * 3] =
	{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x23, 0x23, 0x7a, 0x7a, 0x7a, 0x83, 0x83, 0x83, 0x8c, 0x8c, 0x8c, 0x90, 0x90, 0x90, 0x8e, 0x8e, 0x8e, 0x52, 0x52, 0x52, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x6e, 0x6e, 0x83, 0x83, 0x83, 0x93, 0x93, 0x93, 0xa3, 0xa3, 0xa3, 0xab, 0xab, 0xab, 0xa8, 0xa8, 0xa8, 0x9b, 0x9b, 0x9b, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x72, 0x72, 0x8e, 0x8e, 0x8e, 0xa4, 0xa4, 0xa4, 0xbb, 0xbb, 0xbb, 0xc4, 0xc4, 0xc4, 0xc1, 0xc1, 0xc1, 0xaf, 0xaf, 0xaf, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3e, 0x3e, 0x90, 0x90, 0x90, 0xa6, 0xa6, 0xa6, 0xbe, 0xbe, 0xbe, 0xc8, 0xc8, 0xc8, 0xc4, 0xc4, 0xc4, 0x8e, 0x8e, 0x8e, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x09, 0x09, 0x09, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	int frameIndex = 0;
	int filmHoleIndex = 0;
	int offset = (videoFrame->vfr_width * 3) - 3;
	UBYTE *pRGB = avInfo->avinf_pFrame->data[0];
	ULONG i;

	for (i = 0; i < videoFrame->vfr_height; ++i)
		{
		ULONG j;

		for (j = 0; j < FILMHOLE_WIDTH * 3; j+=3)
			{
			int currentFilmHoleIndex = filmHoleIndex + j;

			pRGB[frameIndex + j]     = filmHole[currentFilmHoleIndex];
			pRGB[frameIndex + j + 1] = filmHole[currentFilmHoleIndex + 1];
			pRGB[frameIndex + j + 2] = filmHole[currentFilmHoleIndex + 2];

			pRGB[frameIndex + offset - j]     = filmHole[currentFilmHoleIndex];
			pRGB[frameIndex + offset - j + 1] = filmHole[currentFilmHoleIndex + 1];
			pRGB[frameIndex + offset - j + 2] = filmHole[currentFilmHoleIndex + 2];
			}
		frameIndex += videoFrame->vfr_lineSize;
		filmHoleIndex = (i % FILMHOLE_HEIGHT) * FILMHOLE_WIDTH * 3;
		}
}

//-----------------------------------------------------------------------------

#define BORING_IMAGE_VARIANCE 512.0		/* Tweak this if necessary */

/* This function attempts to detect images that are mostly solid images
 * It does this by calculating the statistical variance of the
 * black-and-white image */
static BOOL IsInterestingFrame(const struct AVInfo *avInfo)
{
	/* We're gonna assume 8-bit samples. If anyone uses anything different,
	 * it doesn't really matter cause it's gonna be ugly anyways */
	float variance = 0.0f;
	float average = 0.0f;
	ULONG NumberOfSamples;
	ULONG y;

	/* FIXME: If this proves to be a performance issue, this function
	 * can be modified to perhaps only check 3 rows. I doubt this'll
	 * be a problem though. */

	NumberOfSamples = avInfo->avinf_pCodecContext->width * avInfo->avinf_pCodecContext->height;
	NumberOfSamples /= 2;

	// Iterate through the RGB image to calculate average
	for (y = 0; y < avInfo->avinf_pCodecContext->height; y += 2)
		{
		const UBYTE *pBuffer = avInfo->avinf_pRGBFrame->data[0] + y * avInfo->avinf_pRGBFrame->linesize[0];
		ULONG x;

		for (x = 0; x < avInfo->avinf_pCodecContext->width; x++)
			{
			average += (float) (pBuffer[0] + pBuffer[1] + pBuffer[2]);
			pBuffer	+= 3;
			}
		}
	average /= ((float) (3 * NumberOfSamples));

	/* Calculate the variance */
	for (y = 0; y < avInfo->avinf_pCodecContext->height; y++)
		{
		const UBYTE *pBuffer = avInfo->avinf_pRGBFrame->data[0] + y * avInfo->avinf_pRGBFrame->linesize[0];
		ULONG x;

		for (x = 0; x < avInfo->avinf_pCodecContext->width; x++)
			{
			float tmp = (float) ((pBuffer[0] + pBuffer[1] + pBuffer[2]) / 3) - average;

			variance += tmp * tmp;
			pBuffer	+= 3;
			}
		}
	variance /= ((float) (NumberOfSamples - 1));

	d1(	{ \
		char buffer[80]; \
		sprintf(buffer, "average=%f  variance=%f", average, variance); \
		KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buffer); \
		} );

	d1(KPrintF("%s/%s/%ld: IsInterestingFrame=%ld\n", __FILE__, __FUNC__, __LINE__, (variance > BORING_IMAGE_VARIANCE)));

	return (variance > BORING_IMAGE_VARIANCE);
}

//-----------------------------------------------------------------------------

static BOOL FindInterestingFrame(struct AVInfo *avInfo)
{
	ULONG n = 0;
	float framePosition = 1.2;
	float FramePositionStep = 0.25;

	for (n = 0; !IsInterestingFrame(avInfo) && n < 10; n++)
		{
		d1(KPrintF("%s/%s/%ld: n=%ld\n", __FILE__, __FUNC__, __LINE__, n));
		if (!VideoSeekFrame(avInfo, framePosition))
			{
			d1(KPrintF("%s/%s/%ld: VideoSeekFrame failed n=%ld\n", __FILE__, __FUNC__, __LINE__, n));
			return FALSE;
			}
		framePosition += FramePositionStep;
		FramePositionStep += FramePositionStep;	// double step size on each loop
		}

	d1(KPrintF("%s/%s/%ld: Success n=%ld\n", __FILE__, __FUNC__, __LINE__, n));

	return TRUE;
}

//-----------------------------------------------------------------------------

static void av_log_dummy_callback(void* ptr, int level, const char* fmt, va_list vl)
{
#if 0
	char buf[256];

	d1(KPrintF("%s/%s/%ld: fmt=<%s>  level=%ld\n", __FILE__, __FUNC__, __LINE__, fmt, level));

	(void) ptr;
	(void) fmt;
	(void) vl;

	vsnprintf(buf, sizeof(buf), fmt, vl);
	d1(KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, buf));
#endif
}

//-----------------------------------------------------------------------------

void abort(void)
{
	while (1)
		;
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
