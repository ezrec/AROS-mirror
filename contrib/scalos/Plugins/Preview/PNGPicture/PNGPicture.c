// PNGPicture.c
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

#include "png.h"

#include "PNGPicture.h"

//---------------------------------------------------------------------------------------

struct ImageInfo
	{
	LONG image_width;
	LONG image_height;
	};

//---------------------------------------------------------------------------------------

#ifndef __AROS__
struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
#endif
struct Library *CyberGfxBase;
struct ScalosGfxBase *ScalosGfxBase;

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

static void *PngMemPool;
static struct SignalSemaphore PngMemPoolSemaphore;

//---------------------------------------------------------------------------------------

static BOOL GenerateThumbnailFromARGB(struct ScaWindowTask *wt,
	struct ARGBHeader *argbDest, ULONG quality,
	ULONG ScaledWidth, ULONG ScaledHeight,
	ULONG SupportedColors, ULONG ReservedColors,
	struct ScalosPreviewResult *PVResult);
static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	const struct ARGBHeader *argbh, ULONG quality,
        ULONG ScaledWidth, ULONG ScaledHeight,
	struct ScalosPreviewResult *PVResult);
static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size);
static void PngFreeMem(png_structp png_ptr, png_voidp ptr);
static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
static void PngError(png_structp png_ptr, png_const_charp error_message);
static void PngWarning(png_structp png_ptr, png_const_charp warning_message);
static APTR MyAllocVecPooled(size_t Size);
static void MyFreeVecPooled(APTR mem);

//-----------------------------------------------------------

BOOL initPlugin(struct PluginBase *PluginBase)
{
	BOOL result = FALSE;

	d1(KPrintF("%s/%ld:   PluginBase=%08lx  procName=<%s>\n", __FUNC__, __LINE__, \
		PluginBase, FindTask(NULL)->tc_Node.ln_Name));

	do	{
		InitSemaphore(&PngMemPoolSemaphore);

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

		PngMemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		if (NULL == PngMemPool)
			break;;

		result = TRUE;
		d1(KPrintF("%s/%ld: Success\n", __FUNC__, __LINE__));
		} while (0);

	return result;
}


BOOL closePlugin(struct PluginBase *PluginBase)
{
	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif

	if (NULL != PngMemPool)
		{
		DeletePool(PngMemPool);
		PngMemPool = NULL;
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
	BPTR fh = (BPTR)NULL;	      // filehandle for reading of image file
	struct ARGBHeader argbh;
	LONG Success = FALSE;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	struct ImageInfo cinfo;

	(void) PluginBase;

	d1(KPrintF(__FILE__ "/%s/%ld:  START iconName=<%s>\n", __FUNC__, __LINE__, iconName));

	memset(&cinfo, 0, sizeof(cinfo));
	memset(&argbh, 0, sizeof(argbh));

	do	{
		UBYTE id[8];
		struct ARGB *argb;
		ULONG ThumbnailWidth;
		ULONG ThumbnailHeight;
		ULONG SupportedColors;
		ULONG ScreenDepth;
		struct Screen *WBScreen;
		struct BitMapHeader *bmhdExternal;
		BPTR oldDir;
		struct ScalosPreviewResult *PVResult;
		size_t BytesPerRow;
		BOOL DoRemap;
		ULONG quality;
		ULONG ReservedColors;
		LONG Length;
		png_bytepp row_pointers;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		ULONG y;

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

		ScreenDepth = GetBitMapAttr(WBScreen->RastPort.BitMap, BMA_DEPTH);

		DoRemap = (ScreenDepth <= 8) || (NULL == CyberGfxBase) || SupportedColors <= 256;

		d1(KPrintF(__FILE__ "/%s/%ld:  DoRemap=%ld\n", __FUNC__, __LINE__, DoRemap));

		// Immediately Check if it's a PNG

		Length = FRead(fh, (APTR) id, 1, sizeof(id));
		d1(kprintf("%s/%s/%ld:  Length=%ld\n", __FILE__, __FUNC__, __LINE__, Length));
		if (sizeof(id) != Length)
			break;

		d1(kprintf("%s/%s/%ld:  id=%02lx %02lx %02lx %02lx\n", __FILE__, __FUNC__, __LINE__, id[0], id[1], id[2], id[3]));

		/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
		      break if they match */
		if (0 != png_sig_cmp((png_bytep) id, (png_size_t) 0, sizeof(id)))
			{
			d1(kprintf("%s/%s/%ld:  png_sig_cmp failed\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
		      (png_voidp) NULL, PngError, PngWarning, NULL, PngAllocMem, PngFreeMem);

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));
		if (NULL == png_ptr)
			break;

		/* Allocate/initialize the memory for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		d1(KPrintF("%s/%s/%ld:  info_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, info_ptr));
		if (NULL == info_ptr)
			break;

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));
		d1(KPrintF("%s/%s/%ld:  sizeof(jmp_buf)=%lu  png_jmpbuf=%08lx\n", __FILE__, __FUNC__, __LINE__, sizeof(jmp_buf), png_jmpbuf(png_ptr)));
		// safety check against jmp_buf size mismatch between libpng and Scalos
		if (NULL == png_jmpbuf(png_ptr))
			break;

		if (setjmp(png_jmpbuf(png_ptr)))
			{
			d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));
			Success	= FALSE;
			break;
			}

		d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx\n", __FILE__, __FUNC__, __LINE__, png_ptr));

		png_set_read_fn(png_ptr, (void *) fh, user_read_data);

		png_set_sig_bytes(png_ptr, sizeof(id));

		png_set_keep_unknown_chunks(png_ptr,
			PNG_HANDLE_CHUNK_IF_SAFE, NULL, 0);

		png_set_palette_to_rgb(png_ptr);
		png_set_expand_gray_1_2_4_to_8(png_ptr);
		png_set_tRNS_to_alpha(png_ptr);
		png_set_gray_to_rgb(png_ptr);
		png_set_add_alpha(png_ptr, ~0, PNG_FILLER_BEFORE);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		png_read_png(png_ptr, info_ptr,
			PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING
                        | PNG_TRANSFORM_SWAP_ALPHA | PNG_TRANSFORM_EXPAND, NULL);

		d1(KPrintF("%s/%s/%ld:  \n", __FILE__, __FUNC__, __LINE__));

		png_get_IHDR(png_ptr, info_ptr,
			&width, &height, &bit_depth, &color_type,
			&interlace_type, NULL, NULL);

		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  depth=%ld color_type=%ld\n", __FILE__, __FUNC__, __LINE__, width, height, bit_depth, color_type));

		row_pointers = png_get_rows(png_ptr, info_ptr);
		d1(KPrintF("%s/%s/%ld:  row_pointers=%08lx\n", __FILE__, __FUNC__, __LINE__, row_pointers));
		if (NULL == row_pointers)
			break;

		if (bmhdExternal)
			{
			bmhdExternal->bmh_Width = width;
			bmhdExternal->bmh_Height = height;
			bmhdExternal->bmh_Depth = 24;
			}

		ScalosGfxCalculateScaleAspect(width, height,
			&ThumbnailWidth, &ThumbnailHeight);

		d1(KPrintF(__FILE__ "/%s/%ld:  ImageWidth=%ld  ImageHeight=%ld\n", __FUNC__, __LINE__, cinfo.image_width, cinfo.image_height));
		d1(KPrintF(__FILE__ "/%s/%ld:  ThumbnailWidth=%ld  ThumbnailHeight=%ld\n", __FUNC__, __LINE__, ThumbnailWidth, ThumbnailHeight));

		argbh.argb_Width = width;
		argbh.argb_Height = height;
		argbh.argb_ImageData = ScalosGfxCreateARGBTags(argbh.argb_Width, argbh.argb_Height, TAG_END);

		d1(KPrintF("%s/%s/%ld:  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbh.argb_ImageData));
		if (NULL == argbh.argb_ImageData)
			break;

		d1(KPrintF("%s/%s/%ld:  width=%ld  height=%ld  depth=%ld color_type=%ld\n", __FILE__, __FUNC__, __LINE__, width, height, bit_depth, color_type));

		BytesPerRow = png_get_rowbytes(png_ptr, info_ptr);

		d1(KPrintF("%s/%s/%ld:  BytesPerRow=%lu\n", __FILE__, __FUNC__, __LINE__, BytesPerRow));

		argb = argbh.argb_ImageData;
		for (y = 0; y < height; y++)
			{
			memcpy(argb, row_pointers[y], BytesPerRow);
			argb += width;
			}

		if (DoRemap)
			{
			if ((SupportedColors + ReservedColors) >= 256)
				SupportedColors = 256 - ReservedColors;

			Success = GenerateThumbnailFromARGB(wt,
				&argbh,
				quality,
				ThumbnailWidth,
				ThumbnailHeight,
				SupportedColors,
				ReservedColors,
				PVResult);
			}
		else
			{
			Success = GenerateARGBThumbnail(wt,
				&argbh, quality,
				ThumbnailWidth, ThumbnailHeight,
				PVResult);
			}
		} while (0);

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	if (fh)
		Close(fh);

	if (!Success)
		ScalosGfxFreeARGB(&argbh.argb_ImageData);

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

static BOOL GenerateThumbnailFromARGB(struct ScaWindowTask *wt,
	struct ARGBHeader *argbSrc, ULONG quality,
	ULONG ScaledWidth, ULONG ScaledHeight,
	ULONG SupportedColors, ULONG ReservedColors,
        struct ScalosPreviewResult *PVResult)
{
	struct ARGBHeader argbDest = { 0, 0, NULL };
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

	d1(KPrintF(__FILE__ "/%s/%ld:  argb_Width=%ld  argb_Height=%ld  ImageData=%08lx\n", \
		__FUNC__, __LINE__, argbSrc->argb_Width, argbSrc->argb_Height,\
		argbSrc->argb_ImageData));
	d1(KPrintF(__FILE__ "/%s/%ld:  SupportedColors=%lu\n", __FUNC__, __LINE__, SupportedColors));

	argbDest.argb_Width = ScaledWidth;
	argbDest.argb_Height = ScaledHeight;
	argbDest.argb_ImageData = ScalosGfxScaleARGBArrayTags(argbSrc,
		&argbDest.argb_Width,
		&argbDest.argb_Height,
		SCALOSGFX_ScaleARGBArrayFlags, ScaleFlags,
		TAG_END);

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

		PVResult->spvr_sac = ScalosGfxMedianCutTags(&argbDest, Depth,
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
		PVResult->spvr_ARGBheader = argbDest;
		memset(&argbDest, 0, sizeof(argbDest));
		}

	d1(KPrintF(__FILE__ "/%s/%ld:  \n", __FUNC__, __LINE__));

	ScalosGfxFreeARGB(&argbDest.argb_ImageData);

	d1(KPrintF(__FILE__ "/%s/%ld:  END\n", __FUNC__, __LINE__));

	return TRUE;
}

//-----------------------------------------------------------------------------

static BOOL GenerateARGBThumbnail(struct ScaWindowTask *wt,
	const struct ARGBHeader *argbh, ULONG quality,
        ULONG ScaledWidth, ULONG ScaledHeight,
	struct ScalosPreviewResult *PVResult)
{
	BOOL Success = FALSE;
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

		argbDest.argb_Width = ScaledWidth;
		argbDest.argb_Height = ScaledHeight;

		argbDest.argb_ImageData = ScalosGfxScaleARGBArrayTags(argbh,
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

	d1(KPrintF(__FILE__ "/%s/%ld:  END Success=%ld\n", __FUNC__, __LINE__, Success));

	return Success;
}

//-----------------------------------------------------------------------------

void abort(void)
{
	while (1)
		;
}

//-----------------------------------------------------------------------------

static png_voidp PngAllocMem(png_structp png_ptr, png_size_t size)
{
	(void) png_ptr;

	return MyAllocVecPooled(size);
}

//-----------------------------------------------------------------------------

static void PngFreeMem(png_structp png_ptr, png_voidp ptr)
{
	(void) png_ptr;
	MyFreeVecPooled(ptr);
}

//-----------------------------------------------------------------------------

static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	BPTR fh = (BPTR) png_get_io_ptr(png_ptr);

	if (1 != FRead(fh, data, 1, length))
		{
//		png_error();
		}
}

//-----------------------------------------------------------------------------

static void PngError(png_structp png_ptr, png_const_charp error_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, error_message));

	longjmp(png_jmpbuf(png_ptr), 1);
}

//-----------------------------------------------------------------------------

static void PngWarning(png_structp png_ptr, png_const_charp warning_message)
{
	d1(KPrintF("%s/%s/%ld:  png_ptr=%08lx  message=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, png_ptr, warning_message));
//	png_default_warning(png_ptr, warning_message);
}

//-----------------------------------------------------------------------------

static APTR MyAllocVecPooled(size_t Size)
{
	APTR ptr;

	if (PngMemPool)
		{
		ObtainSemaphore(&PngMemPoolSemaphore);
		ptr = AllocPooled(PngMemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&PngMemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, PngMemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, PngMemPool, Size));

	return NULL;
}


static void MyFreeVecPooled(APTR mem)
{
	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, PngMemPool, mem));
	if (PngMemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&PngMemPoolSemaphore);
		FreePooled(PngMemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&PngMemPoolSemaphore);
		}
}

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
