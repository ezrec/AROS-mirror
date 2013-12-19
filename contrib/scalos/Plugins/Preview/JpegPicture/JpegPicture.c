// JpegPicture.c
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
#include <setjmp.h>
#include <stdio.h>

#include <clib/alib_protos.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/scalosgfx.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <defs.h>

//#define FILE	  void
#undef	GLOBAL

#include "jpeglib.h"
#include "jerror.h"

#include "JpegPicture.h"

//---------------------------------------------------------------------------------------

struct JpegErrorInfo
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmpbuf;
};

//---------------------------------------------------------------------------------------

#ifndef __AROS__
struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
#endif
struct ScalosGfxBase *ScalosGfxBase;
struct Library *CyberGfxBase;

#ifdef __amigaos4__
struct GraphicsIFace *IGraphics;
struct UtilityIFace *IUtility;
struct ExecIFace *IExec;
struct CyberGfxIFace *ICyberGfx;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
struct ScalosGfxIFace *IScalosGfx;
struct Library *NewlibBase;
struct Interface *INewlib;
#endif /* __amigaos4__ */

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

//---------------------------------------------------------------------------------------

static BOOL GenerateRemappedThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, struct jpeg_decompress_struct *cinfo,
	JSAMPARRAY buffer, ULONG quality, ULONG ReservedColors,
	ULONG ScaledWidth, ULONG ScaledHeight,
	struct Screen *WBScreen, struct ScalosPreviewResult *PVResult);
static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, struct jpeg_decompress_struct *cinfo,
	JSAMPARRAY buffer, ULONG quality,
        ULONG ScaledWidth, ULONG ScaledHeight,
	struct ScalosPreviewResult *PVResult);
static BOOL GenerateARGBFromJPG(struct ARGBHeader *argbSrc,
	struct jpeg_decompress_struct *cinfo, JSAMPARRAY buffer);
METHODDEF(void) JpegErrorExit(j_common_ptr cinfo);
METHODDEF(void) JpegOutputDummy(j_common_ptr cinfo);

//-----------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	BOOL result = FALSE;

	d1(KPrintF("%s/%ld:   PluginBase=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		PluginBase, FindTask(NULL)->tc_Node.ln_Name));

	do	{
#if !defined(__amigaos4__) && !defined(__AROS__)
		if (_STI_240_InitMemFunctions())
			break;;
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
		d1(kprintf("%s/%ld: IntuitionBase=%08lx\n", __FUNC__, __LINE__, IntuitionBase));
		if (NULL == IntuitionBase)
			break;

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
		d1(kprintf("%s/%ld: GfxBase=%08lx\n", __FUNC__, __LINE__, GfxBase));
		if (NULL == GfxBase)
			break;

		ScalosGfxBase = (struct ScalosGfxBase *) OpenLibrary(SCALOSGFXNAME, 41);
		d1(KPrintF("%s/%ld: ScalosGfxBase=%08lx\n", __FUNC__, __LINE__, ScalosGfxBase));
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


#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
		__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */


		result = TRUE;
		d1(KPrintF("%s/%ld: Success\n", __FUNC__, __LINE__));
		} while (0);

	return result;
}


void closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

#if !defined(__amigaos4__) && !defined(__AROS__)
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
	BPTR fh = (BPTR)NULL;	      // filehandle for reading of image file
	LONG Success = FALSE;
	struct jpeg_decompress_struct cinfo;
	struct FileInfoBlock *fib = NULL;
	BOOL DecompressStarted = FALSE;
	APTR FileBuffer = NULL;

	(void) PluginBase;

	d1(KPrintF(__FILE__ "/%s/%ld:  START iconName=<%s>\n", __FUNC__, __LINE__, iconName));

	memset(&cinfo, 0, sizeof(cinfo));

	do	{
		struct JpegErrorInfo jerr;
		ULONG ThumbnailWidth;
		ULONG ThumbnailHeight;
		ULONG SupportedColors;
		ULONG ScreenDepth;
		struct Screen *WBScreen;
		struct BitMapHeader *bmhdExternal;
		BPTR oldDir;
		struct ScalosPreviewResult *PVResult;
		JSAMPARRAY buffer;	      /* Output row buffer */
		size_t BytesPerRow;
		BOOL DoRemap;
		ULONG quality;
		ULONG ReservedColors;
		size_t FileSize;

		memset(&jerr, 0, sizeof(jerr));

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
		fh = Open(iconName, MODE_OLDFILE);
		CurrentDir(oldDir);
		if ((BPTR)NULL == fh)
			break;

		fib = AllocDosObject(DOS_FIB, NULL);
		if (NULL == fib)
			break;

		if (!ExamineFH(fh, fib))
			break;

		FileSize = fib->fib_Size;

		FileBuffer = malloc(FileSize);
		if (NULL == FileBuffer)
			break;

		// Read entire file into FileBuffer
		if (FileSize != Read(fh, FileBuffer, FileSize))
			break;

		ScreenDepth = GetBitMapAttr(WBScreen->RastPort.BitMap, BMA_DEPTH);

		DoRemap = (ScreenDepth <= 8) || (NULL == CyberGfxBase) || SupportedColors <= 256;

		d1(KPrintF(__FILE__ "/%s/%ld:  DoRemap=%ld\n", __FUNC__, __LINE__, DoRemap));

		// Initialize the JPEG decompression object with default error handling.
		cinfo.err = jpeg_std_error(&jerr.pub);

		// set out own error handler
		jerr.pub.error_exit = JpegErrorExit;
		jerr.pub.output_message = JpegOutputDummy;

		if(!setjmp(jerr.setjmpbuf))
			{
			/* Here we are after setting the long jump */
			d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

			jpeg_create_decompress(&cinfo);

			// set memory as data source
			jpeg_mem_src(&cinfo, FileBuffer, FileSize);

			// obtain image info
			jpeg_read_header(&cinfo, TRUE);

//			cinfo.mem->max_memory_to_use = ???;

			d1(KPrintF(__FILE__ "/%s/%ld:  ImageWidth=%ld  ImageHeight=%ld  jpeg_color_space=%ld\n", \
				__FUNC__, __LINE__, cinfo.image_width, cinfo.image_height, cinfo.jpeg_color_space));

			if (bmhdExternal)
				{
				bmhdExternal->bmh_Width = cinfo.image_width;
				bmhdExternal->bmh_Height = cinfo.image_height;
				bmhdExternal->bmh_Depth = 24;
				}

			cinfo.dct_method = JDCT_IFAST;
			cinfo.do_fancy_upsampling = FALSE;
			cinfo.do_block_smoothing = FALSE;
			if (JCS_GRAYSCALE == cinfo.jpeg_color_space)
				cinfo.out_color_space = JCS_GRAYSCALE;
			else
				cinfo.out_color_space = JCS_RGB;

			d1(KPrintF(__FILE__ "/%s/%ld:  SupportedColors=%ld  ReservedColors=%ld\n", __FUNC__, __LINE__, SupportedColors, ReservedColors));

			if (DoRemap)
				{
				cinfo.desired_number_of_colors = SupportedColors;
				if (cinfo.desired_number_of_colors > 128)
					cinfo.desired_number_of_colors = 128;
				cinfo.desired_number_of_colors += ReservedColors;
				//cinfo.desired_number_of_colors = SupportedColors - ReservedColors;

				cinfo.quantize_colors = TRUE;
				cinfo.dither_mode = JDITHER_FS;
				cinfo.two_pass_quantize = (quality > 7) ? TRUE : FALSE;
				}

			ScalosGfxCalculateScaleAspect(cinfo.image_width, cinfo.image_height,
				&ThumbnailWidth, &ThumbnailHeight);

			// try to scale image as small as possible,
			// but not smaller than thumbnail
			if (cinfo.image_width >= 8 * ThumbnailWidth
				&& cinfo.image_height >= 8 * ThumbnailHeight)
				{
				cinfo.scale_num = 1;
				cinfo.scale_denom = 8;
				}
			else if (cinfo.image_width >= 4 * ThumbnailWidth
				&& cinfo.image_height >= 4 * ThumbnailHeight)
				{
				cinfo.scale_num = 1;
				cinfo.scale_denom = 4;
				}
			else if (cinfo.image_width >= 2 * ThumbnailWidth
				&& cinfo.image_height >= 2 * ThumbnailHeight)
				{
				cinfo.scale_num = 1;
				cinfo.scale_denom = 2;
				}

			d1(KPrintF(__FILE__ "/%s/%ld:  ImageWidth=%ld  ImageHeight=%ld\n", __FUNC__, __LINE__, cinfo.image_width, cinfo.image_height));
			d1(KPrintF(__FILE__ "/%s/%ld:  ThumbnailWidth=%ld  ThumbnailHeight=%ld\n", __FUNC__, __LINE__, ThumbnailWidth, ThumbnailHeight));

			jpeg_start_decompress(&cinfo);

			d1(KPrintF(__FILE__ "/%s/%ld:  after jpeg_start_decompress\n", __FUNC__, __LINE__));
			d1(KPrintF(__FILE__ "/%s/%ld:  output_width=%ld  output_height=%ld\n", __FUNC__, __LINE__, cinfo.output_width, cinfo.output_height));

			/* JSAMPLEs per row in output buffer */
			BytesPerRow = cinfo.output_width * cinfo.output_components;
			/* Make a one-row-high sample array that will go away when done with image */
			buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, BytesPerRow, 1);

			if (DoRemap)
				{
				d1(KPrintF(__FILE__ "/%s/%ld:  colormap=%08lx  actual_number_of_colors=%ld\n", \
					__FUNC__, __LINE__, cinfo.colormap, cinfo.actual_number_of_colors));

				Success = GenerateRemappedThumbnail(wt,
					iconName, &cinfo, buffer, quality, ReservedColors,
					ThumbnailWidth, ThumbnailHeight,
					WBScreen, PVResult);
				}
			else
				{
				Success = GenerateARGBThumbnail(wt,
					iconName, &cinfo, buffer, quality,
					ThumbnailWidth, ThumbnailHeight,
					PVResult);
				}
			}
		} while (0);

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	if (DecompressStarted)
		jpeg_finish_decompress(&cinfo);

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	jpeg_destroy_decompress(&cinfo);

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	if (fh)
		Close(fh);

	if (fib)
		FreeDosObject(DOS_FIB, fib);

	if (FileBuffer)
		free(FileBuffer);

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%ld\n", __FUNC__, __LINE__, Success));

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
	CONST_STRPTR iconName, struct jpeg_decompress_struct *cinfo,
	JSAMPARRAY buffer, ULONG quality, ULONG ReservedColors,
	ULONG ScaledWidth, ULONG ScaledHeight,
	struct Screen *WBScreen, struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
	struct ScalosBitMapAndColor *sacSrc = NULL;
	struct ScalosBitMapAndColor *sacDest = NULL;
	struct BitMap *TempBM = NULL;

	d1(KPrintF(__FILE__ "/%s/%ld:  START\n", __FUNC__, __LINE__));

	do	{
		ULONG n;
		struct ScaleBitMapArg sbma;
		struct RastPort rp;
		struct RastPort TempRp;
		ULONG y;

		if (cinfo->actual_number_of_colors > 256)
			break;
		if (NULL == cinfo->colormap)
			break;

		sacSrc = ScalosGfxCreateSAC(cinfo->output_width,
			cinfo->output_height,
			8,
			NULL, //WBScreen->RastPort.BitMap,
			NULL);
		if (NULL == sacSrc)
			break;

		// Fill sac->sac_ColorTable from cinfo->colormap
		switch (cinfo->out_color_components)
			{
		case 3:		// RGB
			for (n = 0; n < cinfo->actual_number_of_colors; n++)
				{
				sacSrc->sac_ColorTable[3 * n + 0] = RGB_8_TO_32(cinfo->colormap[0][n]);	// Red
				sacSrc->sac_ColorTable[3 * n + 1] = RGB_8_TO_32(cinfo->colormap[1][n]); // Green
				sacSrc->sac_ColorTable[3 * n + 2] = RGB_8_TO_32(cinfo->colormap[2][n]); // Blue

				d1(KPrintF(__FILE__ "/%s/%ld:  n=%ld  R=%08lx G=%08lx B=%08lx\n", \
					__FUNC__, __LINE__, n, sacSrc->sac_ColorTable[3 * n + 0], \
					sacSrc->sac_ColorTable[3 * n + 1], \
					sacSrc->sac_ColorTable[3 * n + 2]));
				}
			break;
		case 1:		// grayscale
			for (n = 0; n < cinfo->actual_number_of_colors; n++)
				{
				sacSrc->sac_ColorTable[3 * n + 0] =
				sacSrc->sac_ColorTable[3 * n + 1] =
				sacSrc->sac_ColorTable[3 * n + 2] = RGB_8_TO_32(cinfo->colormap[0][n]); // Gray R/G/B
				}
			break;
		default:
			break;
			}

		d1(KPrintF(__FILE__ "/%s/%ld:  ImageWidth=%ld  ImageHeight=%ld\n", __FUNC__, __LINE__, cinfo->output_width, cinfo->output_height));
		d1(KPrintF(__FILE__ "/%s/%ld:  actual_number_of_colors=%ld  sac_ColorTable=%08lx\n", __FUNC__, __LINE__, cinfo->actual_number_of_colors, sacSrc->sac_ColorTable));
		d1(KPrintF(__FILE__ "/%s/%ld:  out_color_components=%ld  output_components=%ld  colormap=%08lx\n", __FUNC__, __LINE__, cinfo->out_color_components, cinfo->output_components, cinfo->colormap));

		sacSrc->sac_TransparentColor = SAC_TRANSPARENT_NONE;

		InitRastPort(&rp);
		rp.BitMap = sacSrc->sac_BitMap;

		InitRastPort(&TempRp);
		TempRp = rp;
		TempRp.Layer = NULL;
		TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(cinfo->output_width), 1, 8, 0, NULL);
		if (NULL == TempBM)
			break;

		y = 0;

		d1(KPrintF(__FILE__ "/%s/%ld:  output_width=%ld  output_height=%ld\n", __FUNC__, __LINE__, cinfo->output_width, cinfo->output_height));

		/* Here we use the library's state variable cinfo.output_scanline as the
		 * loop counter, so that we don't have to keep track ourselves.
		 */
		while (cinfo->output_scanline < cinfo->output_height)
			{
			JDIMENSION num_scanlines;

			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			 * Here the array is only one element long, but you could ask for
			 * more than one scanline at a time if that's more convenient.
			 */
			num_scanlines = jpeg_read_scanlines(cinfo, buffer, 1);

			while (num_scanlines--)
				{
				UBYTE *srcPtr = buffer[0];

				WritePixelLine8(&rp,
					0, y,
					cinfo->output_width,
					srcPtr,
					&TempRp);
				y++;
				}
			}

		d1(KPrintF(__FILE__ "/%s/%ld:  actual_number_of_colors=%ld\n", __FUNC__, __LINE__, cinfo->actual_number_of_colors));

		sbma.sbma_SourceBM = sacSrc->sac_BitMap;
		sbma.sbma_SourceWidth = cinfo->output_width;
		sbma.sbma_SourceHeight = cinfo->output_height;
		sbma.sbma_DestWidth = &ScaledWidth;
		sbma.sbma_DestHeight = &ScaledHeight;
		sbma.sbma_NumberOfColors = cinfo->actual_number_of_colors + ReservedColors;
		sbma.sbma_ColorTable = sacSrc->sac_ColorTable;
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

		sacDest = ScalosGfxCreateEmptySAC();
		if (NULL == sacDest)
			break;

		sacDest->sac_BitMap = ScalosGfxScaleBitMap(&sbma, NULL);
		d1(KPrintF(__FILE__ "/%s/%ld:  sac_BitMap=%08lx\n", __FUNC__, __LINE__, sacDest->sac_BitMap));
		if (NULL == sacDest->sac_BitMap)
			break;

		sacDest->sac_NumColors = cinfo->actual_number_of_colors + ReservedColors;
		sacDest->sac_TransparentColor = sacSrc->sac_TransparentColor;
		sacDest->sac_Width = ScaledWidth;
		sacDest->sac_Height = ScaledHeight;
		sacDest->sac_Depth = GetBitMapAttr(sacDest->sac_BitMap, BMA_DEPTH);

		// move sac_ColorTable from sacSrc to sacDest
		// in order to avoid copying
		sacDest->sac_ColorTable = sacSrc->sac_ColorTable;
		sacSrc->sac_ColorTable = NULL;

		d1(KPrintF(__FILE__ "/%s/%ld:  sac_NumColors=%lu  sac_TransparentColor=%lu\n", \
			__FUNC__, __LINE__, sacDest->sac_NumColors, sacDest->sac_TransparentColor));

		PVResult->spvr_sac = sacDest;
		sacDest	= NULL;	    // do not free sac now

		Success = TRUE;
		} while (0);

	if (TempBM)
		FreeBitMap(TempBM);

	ScalosGfxFreeSAC(sacSrc);
	ScalosGfxFreeSAC(sacDest);

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%08lx\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	CONST_STRPTR iconName, struct jpeg_decompress_struct *cinfo,
	JSAMPARRAY buffer, ULONG quality,
        ULONG ScaledWidth, ULONG ScaledHeight,
	struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
	struct ARGBHeader argbSrc = { 0, 0, NULL };
	struct ARGBHeader argbDest = { 0, 0, NULL };

	d1(KPrintF(__FILE__ "/%s/%ld:  START\n", __FUNC__, __LINE__));

	do	{
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

		if (!GenerateARGBFromJPG(&argbSrc, cinfo, buffer))
			{
			d1(KPrintF(__FILE__ "/%s/%ld:  GenerateARGBFromJPG failed\n", __FUNC__, __LINE__));
			break;
			}

		argbDest.argb_Width = ScaledWidth;
		argbDest.argb_Height = ScaledHeight;

		argbDest.argb_ImageData = ScalosGfxScaleARGBArrayTags(&argbSrc,
			&argbDest.argb_Width,
			&argbDest.argb_Height,
			SCALOSGFX_ScaleARGBArrayFlags, ScaleFlags,
			TAG_END);
		d1(KPrintF(__FILE__ "/%s/%ld:  argbDest.argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbDest.argb_ImageData));
		if (NULL == argbDest.argb_ImageData)
			break;

		PVResult->spvr_ARGBheader = argbDest;
		memset(&argbDest, 0, sizeof(argbDest));

		Success = TRUE;
		} while (0);

	ScalosGfxFreeARGB(&argbDest.argb_ImageData);
	ScalosGfxFreeARGB(&argbSrc.argb_ImageData);

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static BOOL GenerateARGBFromJPG(struct ARGBHeader *argbSrc,
	struct jpeg_decompress_struct *cinfo, JSAMPARRAY buffer)
{
	BOOL Success = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld:  START\n", __FUNC__, __LINE__));

	do	{
		struct ARGB *destPtr;
		ULONG y;

		argbSrc->argb_Width = cinfo->output_width;
		argbSrc->argb_Height = cinfo->output_height;
		argbSrc->argb_ImageData = ScalosGfxCreateARGB(argbSrc->argb_Width, argbSrc->argb_Height, NULL);
		d1(KPrintF(__FILE__ "/%s/%ld:  argb_ImageData=%08lx\n", __FUNC__, __LINE__, argbSrc->argb_ImageData));
		if (NULL == argbSrc->argb_ImageData)
			break;

		destPtr = argbSrc->argb_ImageData;
		y = 0;

		d1(KPrintF(__FILE__ "/%s/%ld:  output_width=%ld  output_height=%ld\n", __FUNC__, __LINE__, cinfo->output_width, cinfo->output_height));

		/* Here we use the library's state variable cinfo.output_scanline as the
		 * loop counter, so that we don't have to keep track ourselves.
		 */
		while (cinfo->output_scanline < cinfo->output_height)
			{
			JDIMENSION num_scanlines;

			/* jpeg_read_scanlines expects an array of pointers to scanlines.
			 * Here the array is only one element long, but you could ask for
			 * more than one scanline at a time if that's more convenient.
			 */
			num_scanlines = jpeg_read_scanlines(cinfo, buffer, 1);

			while (num_scanlines--)
				{
				ULONG x;
				const UBYTE *srcPtr = buffer[0];

				switch (cinfo->out_color_space)
					{
				case JCS_RGB:
					for (x = 0; x < cinfo->output_width; x++)
						{
						destPtr->Alpha = 0xff;
						destPtr->Red   = *srcPtr++;
						destPtr->Green = *srcPtr++;
						destPtr->Blue  = *srcPtr++;

						destPtr++;
						}
					break;
				case JCS_GRAYSCALE:
					for (x = 0; x < cinfo->output_width; x++)
						{
						destPtr->Alpha = 0xff;
						destPtr->Red   = *srcPtr;
						destPtr->Green = *srcPtr;
						destPtr->Blue  = *srcPtr;

						srcPtr++;
						destPtr++;
						}
					break;
				default:
					break;
					}
				y++;
				}
			}

		Success = TRUE;
		} while (0);

	d1(KPrintF(__FILE__ "/%s/%ld:  Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------
#if 1
FILE *fopen(const char *name, const char *mode)
{
	LONG accessMode;
	FILE *fh;

	d1(kprintf("%s/%ld: name=<%s>  mode=<%s>\n", __FUNC__, __LINE__, name, mode));

	errno = 0;

	if ('r' == *mode)
		accessMode = MODE_OLDFILE;
	else
		accessMode = MODE_NEWFILE;

	fh = (FILE *) Open((STRPTR) name, accessMode);

	d1(kprintf("%s/%ld: name=<%s>  mode=<%s>  fh=%08lx\n", __FUNC__, __LINE__, name, mode, fh));
	if (NULL == fh)
		{
		LONG error = IoErr();

		d1(kprintf("%s/%ld: error=%ld\n", __FUNC__, __LINE__, error));

		if (ERROR_OBJECT_NOT_FOUND == error)
			errno = ENOENT;
		else
			errno = EIO;
		}

	return fh;
}
#endif
//-----------------------------------------------------------------------------
#if 1
int fclose(FILE *fh)
{
	d1(kprintf("%s/%ld: fh=%08lx\n", __FUNC__, __LINE__, fh));

	errno = 0;

	return Close((BPTR) fh) ? 0 : -1;
}
#endif
//-----------------------------------------------------------------------------
#if 1
size_t fread(void *buf, size_t length, size_t nobj, FILE *fh)
{
	d1(KPrintF(__FILE__ "/%s/%ld:  fh=%08lx  buf=%08lx  nobj=%lu  length=%lu\n", \
		__FUNC__, __LINE__, fh, buf, nobj, length));

	errno = 0;

	return (size_t) FRead((BPTR) fh, buf, length, nobj);
}
#endif
//-----------------------------------------------------------------------------
#if 1
size_t fwrite(const void *buf, size_t length, size_t nobj, FILE *fh)
{
	d1(KPrintF(__FILE__ "/%s/%ld:  fh=%08lx  buf=%08lx  nobj=%lu  length=%lu\n", \
		__FUNC__, __LINE__, fh, buf, nobj, length));

	return (size_t) FWrite((BPTR) fh, (STRPTR) buf, length, nobj);
}
#endif
//-----------------------------------------------------------------------------
#if 1
int fseek(FILE *fh, long offset, int origin)
{
	LONG OldPos;
	LONG mode;

	d1(KPrintF(__FILE__ "/%s/%ld:  fh=%08lx  offset=%ld  origin=%ld\n", \
		__FUNC__, __LINE__, fh, offset, origin));

	switch (origin)
		{
	case SEEK_SET:
		mode = OFFSET_BEGINNING;
		break;
	case SEEK_CUR:
		mode = OFFSET_CURRENT;
		break;
	case SEEK_END:
		mode = OFFSET_END;
		break;
	default:
		return -1;
		break;
		}

	errno = 0;

	OldPos = Seek((BPTR) fh, offset, mode);
	d1(kprintf("%s/%ld: OldPos=%ld\n", __FUNC__, __LINE__, OldPos));

	return OldPos >= 0 ? 0 : -1;
}
#endif
//-----------------------------------------------------------------------------

int unlink(const char *FileName)
{
	d1(kprintf("%s/%ld: FileName=<%s>\n", __FUNC__, __LINE__, FileName));

	errno = 0;

	return DeleteFile(FileName) ? 0 : -1;
}

//-----------------------------------------------------------------------------

int fprintf(FILE *fh, const char *fmt, ...)
{
	(void) fh;
	(void) fmt;

	return 0;
}

//-----------------------------------------------------------------------------

// libjpeg Error manager, long jumps back to main to do the necessary cleanup
METHODDEF(void) JpegErrorExit(j_common_ptr cinfo)
{
	struct JpegErrorInfo *jerr = (struct JpegErrorInfo *) cinfo->err;

	d1(kprintf("%s/%ld: msg_code=%08lx\n", __FUNC__, __LINE__, cinfo->err->msg_code));

	/* Always display the message */
	(*cinfo->err->output_message) (cinfo);

	/* Let the memory manager delete any temp files before we die */
	 jpeg_destroy(cinfo);

	longjmp(jerr->setjmpbuf, 1);
}

// libjpeg dummy error message output
METHODDEF(void) JpegOutputDummy(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message) (cinfo, buffer);

	d1(kprintf("%s\n", buffer));
}

//-----------------------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
int errno = 0;
#endif

#if 1
//int stderr = 0;
 FILE **__sF;
#endif

//-----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
