// AmigaIconObj25.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/resident.h>
#include <graphics/gfxbase.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/iconobject.h>
#include <libraries/mcpgfx.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/utility.h>
#include <proto/exec.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/cybergraphics.h>

#include <string.h>
#include <ctype.h>

#include <defs.h>
#include <Year.h>

#include "AmigaIconObj35.h"

#ifndef ICONCTRLA_GetARGBImageData1
/* Define constants relative to ICONA_ErrorCode, which exists on all platforms */
#define ICONCTRLA_GetARGBImageData1 (ICONA_ErrorCode+300)
#define ICONCTRLA_GetARGBImageData2 (ICONA_ErrorCode+302)
#endif


//----------------------------------------------------------------------------

#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))


#define	NO_ICON_POSITION_WORD	((WORD) 0x8000)

//----------------------------------------------------------------------------

#ifndef __AROS__
struct GfxBase *GfxBase;
struct Library *LayersBase;
T_UTILITYBASE UtilityBase;
struct ExecBase *SysBase;
struct Library *IconBase;
struct Library *CyberGfxBase;
struct DosLibrary *DOSBase;
struct IntuitionBase *IntuitionBase;
#endif
struct Library *IconObjectDTBase;
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct GraphicsIFace *IGraphics;
struct LayersIFace *ILayers;
struct UtilityIFace *IUtility;
struct ExecIFace *IExec;
struct IconIFace *IIcon;
struct CyberGfxIFace *ICyberGfx;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
#endif

static void *MemPool;
static struct SignalSemaphore PubMemPoolSemaphore;

static Class *AmigaIconObj35Class;

//----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT AmigaIconObj35Dispatcher(Class *cl, Object *o, Msg msg);

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops);
static ULONG DtDispose(Class *cl, Object *o, Msg msg);
static ULONG DtSet(Class *cl, Object *o, struct opSet *ops);
static ULONG DtGet(Class *cl, Object *o, struct opGet *opg);
static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl);
static ULONG DtDraw(Class *cl, Object *o, struct iopDraw *iopd);
static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *iopw);
static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *iopw);
static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf);
static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio);

//----------------------------------------------------------------------------

static void FreePenList(struct Screen *screen, UBYTE **PenList, ULONG PaletteSize);
static void ExchangeAttrs(struct DiskObject *DiskObj, struct WriteData *wd);
static void DrawMappedImage(struct InstanceData *inst, 	struct RastPort *rp, 
	struct MappedImage *img, UWORD Left, UWORD Top);
static void MyDrawIconState(struct InstanceData *inst, struct RastPort *rp, 
	struct DrawInfo *drawInfo, ULONG State);
static UBYTE *GenerateMask(struct InstanceData *inst, struct MappedImage *img);
static void SetSuperImgBorders(Class *cl, Object *o, struct opSet *ops);
static void WriteARGBArray(const struct ARGB *SrcImgData,
	ULONG SrcX, ULONG SrcY, ULONG SrcBytesPerRow,
	struct RastPort *DestRp, ULONG DestX, ULONG DestY,
	ULONG SizeX, ULONG SizeY);
static BOOL MappedImageFromSAC(struct InstanceData *inst,
	struct MappedImage *mi, struct ScalosBitMapAndColor *sac);
static void FreeMappedImage(struct InstanceData *inst, struct MappedImage *mi);
static BOOL SelectedImageFromNormalImage(struct InstanceData *inst);
static void MySetFont(Object *o, struct RastPort *rp);

//----------------------------------------------------------------------------

static APTR MyAllocVecPooled(size_t Size);
static void MyFreeVecPooled(APTR mem);

//----------------------------------------------------------------------------

char ALIGNED libName[] = "amigaiconobj35.datatype";
char ALIGNED libIdString[] = "$VER: amigaiconobj35.datatype "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (23.05.2007) "
	COMPILER_STRING
	" ©1999" CURRENTYEAR " The Scalos Team";

//----------------------------------------------------------------------------

LIBFUNC(ObtainInfoEngine, libbase, ULONG)
{
	(void) libbase;
	return (ULONG) AmigaIconObj35Class;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

// return 0 if error occurred
ULONG InitDatatype(struct AmigaIconObj35DtLibBase *dtLib)
{
	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

	dtLib->nib_Initialized = FALSE;

	return 1;
}

// return 0 if error occurred
ULONG OpenDatatype(struct AmigaIconObj35DtLibBase *dtLib)
{
	d1(kprintf("%s/%ld:  OpenCnt=%ld\n", __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (!dtLib->nib_Initialized)
		{
		d1(kprintf("%s/%ld: \n", __FUNC__, __LINE__));

		dtLib->nib_Initialized = TRUE;

		InitSemaphore(&PubMemPoolSemaphore);

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
#ifdef __amigaos4__
		if (IntuitionBase != NULL)
			{
			IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
			if (IIntuition == NULL)
				{
				CloseLibrary((struct Library *)IntuitionBase);
				IntuitionBase = NULL;
				}
			}
#endif
		d1(kprintf("%s/%ld: IntuitionBase=%08lx\n", __FUNC__, __LINE__, IntuitionBase));
		if (NULL == IntuitionBase)
			return 0;

		DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 39);
#ifdef __amigaos4__
		if (DOSBase != NULL)
			{
			IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
			if (IDOS == NULL)
				{
				CloseLibrary((struct Library *)DOSBase);
				DOSBase = NULL;
				}
			}
#endif
		d1(kprintf("%s/%ld: DOSBase=%08lx\n", __FUNC__, __LINE__, DOSBase));
		if (NULL == DOSBase)
			return 0;

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
#ifdef __amigaos4__
		if (UtilityBase != NULL)
			{
			IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
			if (IUtility == NULL)
				{
				CloseLibrary((struct Library *)UtilityBase);
				UtilityBase = NULL;
				}
			}
#endif
		d1(kprintf("%s/%ld: UtilityBase=%08lx\n", __FUNC__, __LINE__, UtilityBase));
		if (NULL == UtilityBase)
			return 0;

		IconBase = OpenLibrary(ICONNAME, 44);
#ifdef __amigaos4__
		if (IconBase != NULL)
			{
			IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
			if (IIcon == NULL)
				{
				CloseLibrary((struct Library *)IconBase);
				IconBase = NULL;
				}
			}
#endif
		d1(kprintf("%s/%ld: IconBase=%08lx\n", __FUNC__, __LINE__, IconBase));
		if (NULL == IconBase)
			return 0;

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

		LayersBase = OpenLibrary("layers.library", 39);
#ifdef __amigaos4__
		if (LayersBase != NULL)
			{
			ILayers = (struct LayersIFace *)GetInterface(LayersBase, "main", 1, NULL);
			if (ILayers == NULL)
				{
				CloseLibrary(LayersBase);
				LayersBase = NULL;
				}
			}
#endif
		d1(kprintf("%s/%ld: LayersBase=%08lx\n", __FUNC__, __LINE__, LayersBase));
		if (NULL == LayersBase)
			return 0;

		IconObjectDTBase = OpenLibrary("datatypes/iconobject.datatype", 39);
		d1(kprintf("%s/%ld: IconObjectDTBase=%08lx\n", __FUNC__, __LINE__, IconObjectDTBase));
		if (NULL == IconObjectDTBase)
			return 0;

		MemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		d1(kprintf("%s/%ld: MemPool=%08lx\n", __FUNC__, __LINE__, MemPool));
		if (NULL == MemPool)
			return 0;

#ifndef __AROS__
		AmigaIconObj35Class = dtLib->nib_ClassLibrary.cl_Class = MakeClass(libName, 
			"iconobject.datatype", NULL, sizeof(struct InstanceData), 0);
		d1(kprintf("%s/%ld:  AmigaIconObj35Class=%08lx\n", __FUNC__, __LINE__, AmigaIconObj35Class));
		if (NULL == AmigaIconObj35Class)
			return 0;

		SETHOOKFUNC(AmigaIconObj35Class->cl_Dispatcher, AmigaIconObj35Dispatcher);
		AmigaIconObj35Class->cl_Dispatcher.h_Data = dtLib;

		// Make class available for the public
		AddClass(AmigaIconObj35Class);
#endif

		CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
#ifdef __amigaos4__
		if (CyberGfxBase != NULL)
			{
			ICyberGfx = (struct CyberGfxIFace *)GetInterface((struct Library *)CyberGfxBase, "main", 1, NULL);
			if (ICyberGfx == NULL)
				{
				CloseLibrary((struct Library *)CyberGfxBase);
				CyberGfxBase = NULL;
				}
			}
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			return 0;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			return 0;
#endif
		// CyberGfxBase may be NULL
		}		

	d1(kprintf("%s/%ld:  Open Success!\n", __FUNC__, __LINE__));

	return 1;
}

void CloseDatatype(struct AmigaIconObj35DtLibBase *dtLib)
{
	d1(kprintf("%s/%ld:  OpenCnt=%ld\n", __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt < 1)
		{
		if (AmigaIconObj35Class)
			{
#ifndef __AROS__
			RemoveClass(AmigaIconObj35Class);
			FreeClass(AmigaIconObj35Class);
			AmigaIconObj35Class = dtLib->nib_ClassLibrary.cl_Class = NULL;
#endif
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
		if (NULL != IconObjectDTBase)
			{
			CloseLibrary(IconObjectDTBase);
			IconObjectDTBase = NULL;
			}
		if (NULL != CyberGfxBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)ICyberGfx);
#endif
			CloseLibrary(CyberGfxBase);
			CyberGfxBase = NULL;
			}
		if (NULL != IconBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IIcon);
#endif
			CloseLibrary(IconBase);
			IconBase = NULL;
			}

		if (NULL != MemPool)
			{
			DeletePool(MemPool);
			MemPool = NULL;
			}

		if (NULL != LayersBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)ILayers);
#endif
			CloseLibrary(LayersBase);
			LayersBase = NULL;
			}
		if (NULL != GfxBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IGraphics);
#endif
			CloseLibrary((struct Library *) GfxBase);
			GfxBase = NULL;
			}
		if (NULL != IntuitionBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IIntuition);
#endif
			CloseLibrary((struct Library *) IntuitionBase);
			IntuitionBase = NULL;
			}
		if (NULL != UtilityBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IUtility);
#endif
			CloseLibrary((struct Library *) UtilityBase);
			UtilityBase = NULL;
			}
		if (NULL != DOSBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IDOS);
#endif
			CloseLibrary((struct Library *)DOSBase);
			DOSBase = NULL;
			}
		}
	d1(kprintf("%s/%ld:  Done!\n", __FUNC__, __LINE__));
}

//-----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT AmigaIconObj35Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	switch (msg->MethodID)
		{
	case OM_NEW:
		o = (Object *) DoSuperMethodA(cl, o, msg);
		d1(kprintf("%s/%ld:  OM_NEW  o=%08lx\n", __FUNC__, __LINE__, o));
		if (o)
			{
			d1(kprintf("%s/%ld:  OM_NEW  o=%08lx\n", __FUNC__, __LINE__, o));
			if (!DtNew(cl, o, (struct opSet *) msg))
				{
				DoMethod(o, OM_DISPOSE);
				o = NULL;
				}
			}
		Result = (ULONG) o;
		break;

	case OM_DISPOSE:
		Result = DtDispose(cl, o, msg);
		break;

	case OM_SET:
		Result = DtSet(cl, o, (struct opSet *) msg);
		break;

	case OM_GET:
		Result = DtGet(cl, o, (struct opGet *) msg);
		break;

	case IDTM_Layout:
		Result = DtLayout(cl, o, (struct iopLayout *) msg);
		break;

	case IDTM_Draw:
		Result = DtDraw(cl, o, (struct iopDraw *) msg);
		break;

	case IDTM_FreeLayout:
		Result = DtFreeLayout(cl, o, (struct iopFreeLayout *) msg);
		break;

	case IDTM_Write:
		Result = DtWrite(cl, o, (struct iopWrite *) msg);
		break;

	case IDTM_NewImage:
		Result = DtNewImage(cl, o, (struct iopNewImage *) msg);
		break;

	case IDTM_CloneIconObject:
		Result = DtClone(cl, o, (struct iopCloneIconObject *) msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	BOOL Success = FALSE;

	do	{
		STRPTR IconName;
		ULONG SupportedIconTypes;
		ULONG DefIconType;
		struct Image *img;
		struct tmp TempData;

		memset(inst, 0, sizeof(struct InstanceData));

		SupportedIconTypes = GetTagData(IDTA_SupportedIconTypes, ~0, ops->ops_AttrList);
		if (!(SupportedIconTypes & IDTV_IconType_ColorIcon))
			break;

		inst->aio_ImageLeft = GetTagData(IDTA_InnerLeft, 0, ops->ops_AttrList);
		inst->aio_ImageTop  = GetTagData(IDTA_InnerTop,  0, ops->ops_AttrList);

		inst->aio_RenderHook = (struct Hook *) GetTagData(IDTA_RenderHook, (ULONG) NULL,  ops->ops_AttrList);

		if (FindTagItem(IDTA_CloneIconObject, ops->ops_AttrList))
			{
			}
		else
			{
			GetAttr(DTA_Name, o, (APTR) &IconName);
			d1(
				{
				char buf[1024];
				GetCurrentDirName(buf, 1024);
				kprintf("%s/%ld:  CurrentDir=<%s>\n", __FUNC__, __LINE__, buf);
				kprintf("%s/%ld:  o=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, o, IconName ? IconName : (STRPTR) "NULL");
				}
			);

			DefIconType = GetTagData(IDTA_DefType,  0,  ops->ops_AttrList);
			d1(kprintf("%s/%ld:  o=%08lx  DefIconType=%lu\n", __FUNC__, __LINE__, o, DefIconType));

			if (0 != DefIconType)
				{
				// Get default icon
				d1(kprintf("%s/%ld:  o=%08lx\n", __FUNC__, __LINE__, o));
				inst->aio_DiskObject = GetIconTags(IconName,
					ICONGETA_RemapIcon, FALSE,
					ICONGETA_GetDefaultType, DefIconType,
					ICONGETA_FailIfUnavailable, FALSE,
					TAG_END);
				d1(kprintf("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
				}
			else
				{
				// Get named icon
				if (NULL == IconName)
					break;

				inst->aio_DiskObject = (struct DiskObject *) GetTagData(AIDTA_Icon, (ULONG) NULL, ops->ops_AttrList);
				d1(kprintf("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
				if (inst->aio_DiskObject)
					{
					inst->aio_DoNotFreeDiskObject = TRUE;
					inst->aio_AlwaysUseDrawIconState = TRUE;
					}
				else
					{
					inst->aio_DiskObject = GetIconTags(IconName, 
						ICONGETA_RemapIcon, FALSE,
						ICONGETA_FailIfUnavailable, TRUE,
						TAG_END);
					}
				}

			d1(kprintf("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
			if (NULL == inst->aio_DiskObject)
				break;

			img = (struct Image *) inst->aio_DiskObject->do_Gadget.GadgetRender;
			if (img)
				{
				SetAttrs(o,
					GA_Width, img->Width,
					GA_Height, img->Height,
					IDTA_UnscaledWidth, img->Width,
					IDTA_UnscaledHeight, img->Height,
					TAG_END);

				d1(kprintf("%s/%ld:  o=%08lx  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, o, gg->Width, gg->Height));
				}

			// IconControlA()
			IconControl(inst->aio_DiskObject,
				ICONCTRLA_IsPaletteMapped, (ULONG) &TempData.tmp_ispalmapped,
				TAG_END);
			d1(kprintf("%s/%ld:  o=%08lx  ispalmapped=%ld\n", __FUNC__, __LINE__, o, TempData.tmp_ispalmapped));

			if (TempData.tmp_ispalmapped)
				{
				IconControl(inst->aio_DiskObject,
					ICONCTRLA_GetHeight, (ULONG) &TempData.tmp_height,
					ICONCTRLA_GetWidth, (ULONG) &TempData.tmp_width,

					ICONCTRLA_GetFrameless, (ULONG) &inst->aio_Borderless,

					ICONCTRLA_GetImageData1, (ULONG) &inst->aio_Image1.aiomi_ImageData,
					ICONCTRLA_GetPaletteSize1, (ULONG) &inst->aio_Image1.aiomi_PaletteSize,
					ICONCTRLA_GetPalette1, (ULONG) &inst->aio_Image1.aiomi_Palette,
					ICONCTRLA_GetTransparentColor1, (ULONG) &inst->aio_Image1.aiomi_TransparentColor,

					ICONCTRLA_GetImageData2, (ULONG) &inst->aio_Image2.aiomi_ImageData,
					ICONCTRLA_GetPaletteSize2, (ULONG) &inst->aio_Image2.aiomi_PaletteSize,
					ICONCTRLA_GetPalette2, (ULONG) &inst->aio_Image2.aiomi_Palette,
					ICONCTRLA_GetTransparentColor2, (ULONG) &inst->aio_Image2.aiomi_TransparentColor,
					TAG_END);

				inst->aio_fIsPaletteMapped = TRUE;

				inst->aio_MappedWidth = TempData.tmp_width;
				inst->aio_MappedHeight = TempData.tmp_height;

				SetAttrs(o,
					GA_Width, inst->aio_MappedWidth,
					GA_Height, inst->aio_MappedHeight,
					IDTA_UnscaledWidth, inst->aio_MappedWidth,
					IDTA_UnscaledHeight, inst->aio_MappedHeight,
					TAG_END);

				d1(kprintf("%s/%ld:  o=%08lx  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, o, gg->Width, gg->Height));

				GenerateMask(inst, &inst->aio_Image1);
				d1(KPrintF("%s/%ld:  o=%08lx  aio_ImageMask1=%08lx\n", __FUNC__, __LINE__, o, inst->aio_Image1.aiomi_Mask));

				GenerateMask(inst, &inst->aio_Image2);
				d1(KPrintF("%s/%ld:  o=%08lx  aio_ImageMask2=%08lx\n", __FUNC__, __LINE__, o, inst->aio_Image2.aiomi_Mask));
				}
			else
				{
				ULONG BytesPerRow, MaskSize;
				PLANEPTR Mask1 = NULL, Mask2 = NULL;
				ULONG *ARGBData1 = NULL, *ARGBData2 = NULL;

				IconControl(inst->aio_DiskObject,
					ICONCTRLA_GetHeight, (ULONG) &TempData.tmp_height,
					ICONCTRLA_GetWidth, (ULONG) &TempData.tmp_width,
					ICONCTRLA_GetImageMask1, (ULONG) &Mask1,
					ICONCTRLA_GetImageMask2, (ULONG) &Mask2,
					ICONCTRLA_GetARGBImageData1, (ULONG) &ARGBData1,
					ICONCTRLA_GetARGBImageData2, (ULONG) &ARGBData2,
					TAG_END);
				d1(KPrintF("%s/%ld:  o=%08lx  Mask1=%08lx  Mask2=%08lx\n",
					__FUNC__, __LINE__, o, Mask1, Mask2));
				d1(KPrintF("%s/%ld:  o=%08lx  ARGBData1=%08lx  ARGBData2=%08lx\n",
					__FUNC__, __LINE__, o, ARGBData1, ARGBData2));
				inst->aio_MappedWidth = TempData.tmp_width;
				inst->aio_MappedHeight = TempData.tmp_height;

				BytesPerRow = ((inst->aio_MappedWidth + 15) & 0xfff0) >> 3;
				MaskSize = inst->aio_MappedHeight * BytesPerRow;
				if (ARGBData1)
					{
					struct ARGBHeader argbh;

					argbh.argb_Width = TempData.tmp_width;
					argbh.argb_Height = TempData.tmp_height;
					argbh.argb_ImageData = (struct ARGB *)ARGBData1;

					SetAttrs(o,
						IDTA_CopyARGBImageData,	TRUE,
						IDTA_ARGBImageData, (ULONG) &argbh,
						TAG_END);
					}
				if (Mask1)
					{
					inst->aio_Image1.aiomi_Mask = MyAllocVecPooled(MaskSize);
					CopyMem(Mask1, inst->aio_Image1.aiomi_Mask, MaskSize);
					}
				else
					{
					inst->aio_Image1.aiomi_Mask = NULL;
					}

				if (ARGBData2)
					{
					struct ARGBHeader argbh;

					argbh.argb_Width = TempData.tmp_width;
					argbh.argb_Height = TempData.tmp_height;
					argbh.argb_ImageData = (struct ARGB *)ARGBData2;

					SetAttrs(o,
						IDTA_CopySelARGBImageData, TRUE,
						IDTA_SelARGBImageData, (ULONG) &argbh,
						TAG_END);
					}
				if (Mask2)
					{
					inst->aio_Image2.aiomi_Mask = MyAllocVecPooled(MaskSize);
					CopyMem(Mask2, inst->aio_Image2.aiomi_Mask, MaskSize);
					}
				else
					{
					inst->aio_Image2.aiomi_Mask = NULL;
					}
				}

			if (NO_ICON_POSITION == inst->aio_DiskObject->do_CurrentX)
				{
				gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_WORD;
				}
			else
				{
				gg->LeftEdge = inst->aio_DiskObject->do_CurrentX;
				gg->TopEdge = inst->aio_DiskObject->do_CurrentY;
				}

			d1(kprintf("%s/%ld:  o=%08lx  Left=%ld  Top=%ld\n", __FUNC__, __LINE__, o, gg->LeftEdge, gg->TopEdge));

			if (NULL == inst->aio_DiskObject->do_DrawerData)
				{
				inst->aio_myDrawerData = MyAllocVecPooled(sizeof(struct DrawerData));
				if (NULL == inst->aio_myDrawerData)
					break;

				memset(inst->aio_myDrawerData, 0, sizeof(struct DrawerData));

				switch (inst->aio_DiskObject->do_Type)
					{
				case WBTOOL:
				case WBPROJECT:
				case WBKICK:
					break;
				default:
					inst->aio_DiskObject->do_DrawerData = inst->aio_myDrawerData;
					break;
					}
				}

			if (inst->aio_DiskObject->do_DrawerData)
				{
				SetAttrs(o,
					IDTA_ViewModes, inst->aio_DiskObject->do_DrawerData->dd_ViewModes,
					IDTA_Flags, inst->aio_DiskObject->do_DrawerData->dd_Flags,
					IDTA_WinCurrentX, inst->aio_DiskObject->do_DrawerData->dd_CurrentX,
					IDTA_WinCurrentY, inst->aio_DiskObject->do_DrawerData->dd_CurrentY,
					IDTA_WindowRect, (ULONG) &inst->aio_DiskObject->do_DrawerData->dd_NewWindow,
					TAG_END);
				}

			SetAttrs(o,
				IDTA_Mask_Normal, (ULONG) inst->aio_Image1.aiomi_Mask,
				IDTA_Mask_Selected, (ULONG) inst->aio_Image2.aiomi_Mask,
				IDTA_Width_Mask_Normal, inst->aio_MappedWidth,
				IDTA_Height_Mask_Normal, inst->aio_MappedHeight,
				IDTA_Width_Mask_Selected, inst->aio_MappedWidth,
				IDTA_Height_Mask_Selected, inst->aio_MappedHeight,

				IDTA_ToolTypes, (ULONG) inst->aio_DiskObject->do_ToolTypes,
				IDTA_Stacksize, inst->aio_DiskObject->do_StackSize,
				IDTA_DefaultTool, (ULONG) inst->aio_DiskObject->do_DefaultTool,
				IDTA_Type, inst->aio_DiskObject->do_Type,
				IDTA_Borderless, inst->aio_Borderless,
				TAG_END);

			inst->aio_fLayoutOk = FALSE;

			SetSuperImgBorders(cl, o, ops);
			}

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%ld:  o=%08lx  Success=%ld\n", __FUNC__, __LINE__, o, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static ULONG DtDispose(Class *cl, Object *o, Msg msg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
//	struct ExtGadget *gg = (struct ExtGadget *) o;

	if (inst->aio_myDrawerData)
		{
		MyFreeVecPooled(inst->aio_myDrawerData);
		inst->aio_myDrawerData = NULL;
		}
	if (inst->aio_DiskObject && !inst->aio_DoNotFreeDiskObject)
		{
		FreeDiskObject(inst->aio_DiskObject);
		inst->aio_DiskObject = NULL;
		}
	FreeMappedImage(inst, &inst->aio_Image1);
	FreeMappedImage(inst, &inst->aio_Image2);

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------------------

static ULONG DtSet(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG Result;

	inst->aio_RenderHook = (struct Hook *) GetTagData(IDTA_RenderHook, (ULONG) inst->aio_RenderHook, ops->ops_AttrList);

	if (inst->aio_DoNotFreeDiskObject)
		{
		if (0 == GetTagData(IDTA_DoFreeDiskObject, 0, ops->ops_AttrList))
			inst->aio_DoNotFreeDiskObject = TRUE;
		}

	inst->aio_ImageLeft = GetTagData(IDTA_InnerLeft, inst->aio_ImageLeft, ops->ops_AttrList);
	inst->aio_ImageTop  = GetTagData(IDTA_InnerTop,  inst->aio_ImageTop,  ops->ops_AttrList);

	Result = DoSuperMethodA(cl, o, (Msg) ops);

	SetSuperImgBorders(cl, o, ops);

	return Result;
}

//----------------------------------------------------------------------------------------

static ULONG DtGet(Class *cl, Object *o, struct opGet *opg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG result = 1;

	switch (opg->opg_AttrID)
		{
	case IDTA_IconType:
		*opg->opg_Storage = ioICONTYPE_GlowIcon;
		break;

	case IDTA_NumberOfColorsSupported:
		*opg->opg_Storage = 256;
		break;

	case AIDTA_Icon:
		*opg->opg_Storage = (ULONG) inst->aio_DiskObject;
		break;

	case IDTA_Extention:
		*opg->opg_Storage = (ULONG) ".info";
		break;

	default:
		result = DoSuperMethodA(cl, o, (Msg) opg);
		break;
		}

	return result;
}

//-----------------------------------------------------------------------------

static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(kprintf("%s/%ld:  START o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	if (inst->aio_RenderHook)
		{
		SetAttrs(o, 
			IDTA_MultiLineText, FALSE,
			TAG_END);
		}

	DoSuperMethodA(cl, o, (Msg) iopl);

	d1(kprintf("%s/%ld:  o=%08lx  Screen=%08lx\n", __FUNC__, __LINE__, o, iopl->iopl_Screen));
	d1(kprintf("%s/%ld:  o=%08lx  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, o, gg->Width, gg->Height));
	d1(kprintf("%s/%ld:  o=%08lx  GadgetRender=%08lx  SelectRender=%08lx  Flags=%08lx\n", \
		__FUNC__, __LINE__, o, gg->GadgetRender, gg->SelectRender, iopl->iopl_Flags));

	inst->aio_Screen = iopl->iopl_Screen;

	if (gg->GadgetRender && (iopl->iopl_Flags & IOLAYOUTF_NormalImage))
		{
		if (inst->aio_AlwaysUseDrawIconState || !inst->aio_fIsPaletteMapped)
			{
			MyDrawIconState(inst, (struct RastPort *) gg->GadgetRender, 
				iopl->iopl_DrawInfo, IDS_NORMAL);
			}
		else
			{
			DrawMappedImage(inst, (struct RastPort *) gg->GadgetRender, &inst->aio_Image1, 
				inst->aio_ImageLeft, inst->aio_ImageTop);
			}
		}
	if (gg->SelectRender && (iopl->iopl_Flags & IOLAYOUTF_SelectedImage))
		{
		if (inst->aio_AlwaysUseDrawIconState || !inst->aio_fIsPaletteMapped)
			{
			MyDrawIconState(inst, (struct RastPort *) gg->SelectRender, 
				iopl->iopl_DrawInfo, IDS_SELECTED);
			}
		else
			{
			DrawMappedImage(inst, (struct RastPort *) gg->SelectRender, &inst->aio_Image2, 
				inst->aio_ImageLeft, inst->aio_ImageTop);
			}
		}

	d1(kprintf("%s/%ld:  END o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtDraw(Class *cl, Object *o, struct iopDraw *iopd)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(kprintf("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__,
		o, inst));

	// copy current position into DiskObject
	inst->aio_DiskObject->do_CurrentX = gg->LeftEdge;
	inst->aio_DiskObject->do_CurrentY = gg->TopEdge;

	// RenderHook NEEDS Layer !
	if (inst->aio_RenderHook && iopd->iopd_RastPort->Layer)
		{
		// Draw Icon via RenderHook
		d1(kprintf("%s/%ld: RenderHook\n", __FUNC__, __LINE__));

		if (!(iopd->iopd_DrawFlags & IODRAWF_NoImage))
			{
			static struct TagItem EmptyTagList[] = { { TAG_END } };
			LONG x, y;
			STRPTR IconText;
			ULONG FgPen = 1;
			struct AppIconRenderMsg arm;
			struct Region *IconRegion;
			struct Region *OldDamageList = NULL;

			d1(kprintf("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));
			d1(kprintf("%s/%s/%ld: BoundsWidth=%ld  BoundsHeight=%ld\n", __FILE__, __FUNC__, __LINE__, gg->BoundsWidth, gg->BoundsHeight));

			x = iopd->iopd_XOffset;
			y = iopd->iopd_YOffset;

			if (!(iopd->iopd_DrawFlags & IODRAWF_AbsolutePos))
				{
				x += gg->LeftEdge;
				y += gg->TopEdge;
				}

			GetAttr(IDTA_Text, o, (APTR) &IconText);
			GetAttr(IDTA_TextPen, o, &FgPen);

			SetABPenDrMd(iopd->iopd_RastPort, FgPen, 0, JAM1);
			MySetFont(o, iopd->iopd_RastPort);

			// Build AppIconRenderMsg
			arm.arm_RastPort = iopd->iopd_RastPort;
			arm.arm_Icon = inst->aio_DiskObject;
			arm.arm_Label = IconText;
			arm.arm_Tags = EmptyTagList;
			arm.arm_Left = x;
			arm.arm_Top = y;
			arm.arm_Width = gg->Width;
			arm.arm_Height = gg->Height;
			arm.arm_State = (gg->Flags & GFLG_SELECTED) ? IDS_SELECTED : IDS_NORMAL;

			LockLayerInfo(iopd->iopd_RastPort->Layer->LayerInfo);
			LockLayerRom(iopd->iopd_RastPort->Layer);

			d1(kprintf("%s/%ld:  DamageList=%08lx\n", __FUNC__, __LINE__, iopd->iopd_RastPort->Layer->DamageList));

			IconRegion = NewRegion();
			if (IconRegion)
				{
				struct Rectangle IconRect;

				d1(kprintf("%s/%ld:  IconRegion=%08lx\n", __FUNC__, __LINE__, IconRegion));

				IconRect.MinX = IconRect.MaxX = arm.arm_Left;
				IconRect.MinY = IconRect.MaxY = arm.arm_Top;
				IconRect.MaxX += arm.arm_Width - 1;
				IconRect.MaxY += arm.arm_Height - 1;

				OldDamageList = iopd->iopd_RastPort->Layer->DamageList;
				iopd->iopd_RastPort->Layer->DamageList = IconRegion;

				ClearRegion(iopd->iopd_RastPort->Layer->DamageList);
				OrRectRegion(iopd->iopd_RastPort->Layer->DamageList, &IconRect);

				if (gg->Width != gg->BoundsWidth || gg->Height != gg->BoundsHeight)
					{
					IconRect.MinX = IconRect.MaxX = arm.arm_Left - (gg->LeftEdge - gg->BoundsLeftEdge);
					IconRect.MinY = IconRect.MaxY = arm.arm_Top + gg->Height;
					IconRect.MaxX += gg->BoundsWidth - 1;
					IconRect.MaxY += gg->BoundsHeight - gg->Height;

					OrRectRegion(iopd->iopd_RastPort->Layer->DamageList, &IconRect);
					}
				}

			d1(kprintf("%s/%ld:  o=%08lx  AppIconRenderMsg=%08lx  rp=%08lx  Layer=%08lx x=%ld  y=%ld  w=%ld  h=%ld\n",\
				 __FUNC__, __LINE__, o, &arm, arm.arm_RastPort, arm.arm_RastPort->Layer, \
				arm.arm_Left, arm.arm_Top, arm.arm_Width, arm.arm_Height));

			BeginUpdate(iopd->iopd_RastPort->Layer);

			if (CallHookPkt(inst->aio_RenderHook, NULL, &arm))
				DoSuperMethodA(cl, o, (Msg) iopd);

			if (OldDamageList)
				{
				iopd->iopd_RastPort->Layer->DamageList = OldDamageList;
				EndUpdate(iopd->iopd_RastPort->Layer, FALSE);
				}
			else
				{
				iopd->iopd_RastPort->Layer->Flags &= ~0x0080;
				EndUpdate(iopd->iopd_RastPort->Layer, TRUE);
				}

			if (IconRegion)
				DisposeRegion(IconRegion);

			UnlockLayerRom(iopd->iopd_RastPort->Layer);
			UnlockLayerInfo(iopd->iopd_RastPort->Layer->LayerInfo);
			}
		}
	else
		{
		d1(kprintf("%s/%s/%ld: standard icon\n", __FILE__, __FUNC__, __LINE__));
		// standard icon: let SuperClass do the drawing 
		DoSuperMethodA(cl, o, (Msg) iopd);
		}

	d1(kprintf("%s/%ld:  END o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *iopw)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct WriteData wd;
	struct DrawerData *OrigDrawerData;
	LONG Result = RETURN_OK;
	ULONG NeedUpdateWB;

	d1(kprintf("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	NeedUpdateWB = GetTagData(ICONA_NotifyWorkbench, FALSE, iopw->iopw_Tags);

	memset(&wd, 0, sizeof(wd));

	GetAttr(IDTA_Type, 		o, &wd.aiowd_Type);
	GetAttr(IDTA_ViewModes, 	o, &wd.aiowd_ViewModes);
	GetAttr(IDTA_Flags, 		o, &wd.aiowd_Flags);
	GetAttr(IDTA_WinCurrentX, 	o, &wd.aiowd_CurrentX);
	GetAttr(IDTA_WinCurrentY, 	o, &wd.aiowd_CurrentY);
	GetAttr(IDTA_WindowRect, 	o, (ULONG *) &wd.aiowd_WindowRect);
	GetAttr(IDTA_Stacksize, 	o, &wd.aiowd_StackSize);
	GetAttr(IDTA_ToolTypes, 	o, (ULONG *) &wd.aiowd_ToolTypes);
	GetAttr(IDTA_DefaultTool, 	o, (ULONG *) &wd.aiowd_DefaultTool);

	ExchangeAttrs(inst->aio_DiskObject, &wd);

	OrigDrawerData = inst->aio_DiskObject->do_DrawerData;
	inst->aio_DiskObject->do_DrawerData = NULL;

	switch (inst->aio_DiskObject->do_Type)
		{
	case WBTOOL:
	case WBPROJECT:
	case WBKICK:
		break;
	default:
		if (OrigDrawerData)
			inst->aio_DiskObject->do_DrawerData = OrigDrawerData;
		else
			inst->aio_DiskObject->do_DrawerData = inst->aio_myDrawerData;

		inst->aio_DiskObject->do_DrawerData->dd_CurrentX = wd.aiowd_CurrentX;
		inst->aio_DiskObject->do_DrawerData->dd_CurrentY = wd.aiowd_CurrentY;

		if (wd.aiowd_WindowRect)
			{
			inst->aio_DiskObject->do_DrawerData->dd_NewWindow.LeftEdge = wd.aiowd_WindowRect->Left;
			inst->aio_DiskObject->do_DrawerData->dd_NewWindow.TopEdge = wd.aiowd_WindowRect->Top;
			inst->aio_DiskObject->do_DrawerData->dd_NewWindow.Width = wd.aiowd_WindowRect->Width;
			inst->aio_DiskObject->do_DrawerData->dd_NewWindow.Height = wd.aiowd_WindowRect->Height;
			inst->aio_DiskObject->do_DrawerData->dd_Flags = wd.aiowd_Flags;
			inst->aio_DiskObject->do_DrawerData->dd_ViewModes = wd.aiowd_ViewModes;
			}
		break;
		}

	if (GetTagData(ICONA_NoNewImage, 0, iopw->iopw_Tags))
		{
		}
	if (GetTagData(ICONA_NoPosition, 0, iopw->iopw_Tags))
		{
		inst->aio_DiskObject->do_CurrentX = inst->aio_DiskObject->do_CurrentY = NO_ICON_POSITION;
		}
	else
		{
		if (NO_ICON_POSITION_WORD == gg->LeftEdge)
			{
			inst->aio_DiskObject->do_CurrentX = inst->aio_DiskObject->do_CurrentY = NO_ICON_POSITION;
			}
		else
			{
			inst->aio_DiskObject->do_CurrentX = gg->LeftEdge;
			inst->aio_DiskObject->do_CurrentY = gg->TopEdge;
			}
		}

	d1(kprintf("%s/%ld:  GadgetRender=%08lx\n", __FUNC__, __LINE__, inst->aio_DiskObject->do_Gadget.GadgetRender));

	if (NULL == inst->aio_DiskObject->do_Gadget.GadgetRender)
		{
		// LayoutIconA()
		LayoutIcon(inst->aio_DiskObject, NULL, TAG_END);
		}

	d1(kprintf("%s/%ld:  Image2Data=%08lx\n", __FUNC__, __LINE__, inst->aio_Image2.aiomi_ImageData));

	if (inst->aio_Image2.aiomi_ImageData)
		{
		IconControl(inst->aio_DiskObject,
			ICONCTRLA_SetImageData2, (ULONG) inst->aio_Image2.aiomi_ImageData,
			ICONCTRLA_SetTransparentColor2, (ULONG) inst->aio_Image2.aiomi_TransparentColor,
			TAG_END);

		d1(kprintf("%s/%ld:  PaletteSize2=%lu\n", __FUNC__, __LINE__, inst->aio_Image2.aiomi_PaletteSize));

		if (inst->aio_Image2.aiomi_PaletteSize)
			{
			IconControl(inst->aio_DiskObject,
				ICONCTRLA_SetPaletteSize2, (ULONG) inst->aio_Image2.aiomi_PaletteSize,
				ICONCTRLA_SetPalette2, (ULONG) inst->aio_Image2.aiomi_Palette,
				TAG_END);
			}
		}

	d1(kprintf("%s/%ld:  PaletteSize1=%lu\n", __FUNC__, __LINE__, inst->aio_Image1.aiomi_PaletteSize));
	d1(kprintf("%s/%ld:  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, inst->aio_MappedWidth, inst->aio_MappedHeight));

	// IconControlA()
	IconControl(inst->aio_DiskObject,
		ICONCTRLA_SetImageData1, (ULONG) inst->aio_Image1.aiomi_ImageData,
		ICONCTRLA_SetPaletteSize1, (ULONG) inst->aio_Image1.aiomi_PaletteSize,
		ICONCTRLA_SetPalette1, (ULONG) inst->aio_Image1.aiomi_Palette,
		ICONCTRLA_SetTransparentColor1, (ULONG) inst->aio_Image1.aiomi_TransparentColor,
		ICONCTRLA_SetWidth, inst->aio_MappedWidth,
		ICONCTRLA_SetHeight, inst->aio_MappedHeight,
		TAG_END);

	// PutIconTagList()
	if (!PutIconTags(iopw->iopw_Path, inst->aio_DiskObject,
		ICONPUTA_NotifyWorkbench, NeedUpdateWB,
		TAG_END))
		{
		Result = IoErr();
		}

	// Restore diskobject
	inst->aio_DiskObject->do_DrawerData = OrigDrawerData;
	ExchangeAttrs(inst->aio_DiskObject, &wd);

	return (ULONG) Result;
}

//-----------------------------------------------------------------------------

static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *ioni)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%ld:  START o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	do	{
		if (NULL == ioni->ioni_NormalImage)
			break;

		if (!MappedImageFromSAC(inst, &inst->aio_Image1, ioni->ioni_NormalImage))
			break;

		if (ioni->ioni_SelectedImage)
			{
			if (!MappedImageFromSAC(inst, &inst->aio_Image2, ioni->ioni_SelectedImage))
				break;
			}
		else
			{
			if (!SelectedImageFromNormalImage(inst))
				break;
			}
		} while (0);

	d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

	SetAttrs(o,
		GA_Width, inst->aio_MappedWidth,
		GA_Height, inst->aio_MappedHeight,
		IDTA_UnscaledWidth, inst->aio_MappedWidth,
		IDTA_UnscaledHeight, inst->aio_MappedHeight,
		TAG_END);

	GenerateMask(inst, &inst->aio_Image1);
	d1(KPrintF("%s/%ld:  o=%08lx  aio_ImageMask1=%08lx\n", __FUNC__, __LINE__, o, inst->aio_Image1.aiomi_Mask));

	GenerateMask(inst, &inst->aio_Image2);
	d1(KPrintF("%s/%ld:  o=%08lx  aio_ImageMask2=%08lx\n", __FUNC__, __LINE__, o, inst->aio_Image1.aiomi_Mask));

	SetAttrs(o,
		IDTA_Mask_Normal, (ULONG) inst->aio_Image1.aiomi_Mask,
		IDTA_Mask_Selected, (ULONG) inst->aio_Image2.aiomi_Mask,
		IDTA_Width_Mask_Normal, inst->aio_MappedWidth,
		IDTA_Height_Mask_Normal, inst->aio_MappedHeight,
		IDTA_Width_Mask_Selected, inst->aio_MappedWidth,
		IDTA_Height_Mask_Selected, inst->aio_MappedHeight,
		TAG_END);

	d1(KPrintF("%s/%ld:  END o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf)
{
	struct InstanceData *inst = INST_DATA(cl, o);
//	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(kprintf("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	if (inst->aio_Screen && (opf->iopf_Flags & IOFREELAYOUTF_ScreenAvailable))
		{
		if (inst->aio_Image1.aiomi_PenList)
			{
			FreePenList(inst->aio_Screen, &inst->aio_Image1.aiomi_PenList, 
				inst->aio_Image1.aiomi_PaletteSize);
			}
		if (inst->aio_Image2.aiomi_PenList)
			{
			FreePenList(inst->aio_Screen, &inst->aio_Image2.aiomi_PenList, 
				inst->aio_Image2.aiomi_PaletteSize);
			}
		}
	else
		{
		if (inst->aio_Image1.aiomi_PenList)
			{
			MyFreeVecPooled(inst->aio_Image1.aiomi_PenList);
			inst->aio_Image1.aiomi_PenList = NULL;
			}
		if (inst->aio_Image2.aiomi_PenList)
			{
			MyFreeVecPooled(inst->aio_Image2.aiomi_PenList);
			inst->aio_Image2.aiomi_PenList = NULL;
			}
		}

	return DoSuperMethodA(cl, o, (Msg) opf);
}

//-----------------------------------------------------------------------------

static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio)
{
//	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%ld:  START o=%08lx\n", __FUNC__, __LINE__, o));

	d1(KPrintF("%s/%ld:  END o=%08lx\n", __FUNC__, __LINE__, o));

	return 0;
}

//----------------------------------------------------------------------------------------

static void FreePenList(struct Screen *screen, UBYTE **PenListPtr, ULONG PaletteSize)
{
	UBYTE *PenList = *PenListPtr;

	while (PaletteSize--)
		{
		ReleasePen(screen->ViewPort.ColorMap, *PenList++);
		}

	MyFreeVecPooled(*PenListPtr);
	*PenListPtr = NULL;
}

//----------------------------------------------------------------------------------------

static void ExchangeAttrs(struct DiskObject *DiskObj, struct WriteData *wd)
{
	UBYTE TempUB;
	ULONG TempUL;
	void *TempPtr;

	TempPtr = wd->aiowd_DefaultTool;
	wd->aiowd_DefaultTool = DiskObj->do_DefaultTool;
	DiskObj->do_DefaultTool = TempPtr;

	TempPtr = wd->aiowd_ToolTypes;
	wd->aiowd_ToolTypes = DiskObj->do_ToolTypes;
	DiskObj->do_ToolTypes = TempPtr;

	TempUL = wd->aiowd_StackSize;
	wd->aiowd_StackSize = DiskObj->do_StackSize;
	DiskObj->do_StackSize = TempUL;

	TempUB = (UBYTE) wd->aiowd_Type;
	wd->aiowd_Type = DiskObj->do_Type;
	DiskObj->do_Type = TempUB;
}

//----------------------------------------------------------------------------------------

static void DrawMappedImage(struct InstanceData *inst, 	struct RastPort *rp, 
	struct MappedImage *img, UWORD Left, UWORD Top)
{
	BOOL Success = FALSE;
	UBYTE *MappedImage = NULL;
	struct ARGB *ARGBArray = NULL;
	struct ARGB *ARGBImgArray = NULL;

	do	{
		ULONG BmWidth, BmHeight;
		ULONG Width, Height;

		BmWidth = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
		BmHeight = GetBitMapAttr(rp->BitMap, BMA_HEIGHT);

		Width = min(inst->aio_MappedWidth, BmWidth - Left);
		Height = min(inst->aio_MappedHeight, BmHeight - Top);

		if (CyberGfxBase && GetBitMapAttr(rp->BitMap, BMA_DEPTH) > 8)
			{
			// True color
			struct ARGB *ARGBPtr;
			struct ARGB *ARGBImgDestPtr;
			const struct ColorRegister *PalettePtr;
			UBYTE *ImgSrcPtr;
			ULONG n;
			ULONG ImgSize;

			ImgSize = inst->aio_MappedWidth * inst->aio_MappedHeight;
			ARGBImgArray = MyAllocVecPooled(ImgSize * sizeof(struct ARGB));
			d1(kprintf("%s/%ld:  ARGBImgArray=%08lx\n", __FUNC__, __LINE__, ARGBImgArray));
			if (NULL == ARGBImgArray)
				break;

			ARGBArray = MyAllocVecPooled(img->aiomi_PaletteSize * sizeof(struct ARGB));
			d1(kprintf("%s/%ld:  ARGBArray=%08lx\n", __FUNC__, __LINE__, ARGBArray));
			if (NULL == ARGBArray)
				break;

			PalettePtr = img->aiomi_Palette;
			ARGBPtr = ARGBArray;
			for (n = 0; n < img->aiomi_PaletteSize; n++, ARGBPtr++, PalettePtr++)
				{
				ARGBPtr->Alpha = 0xff;
				ARGBPtr->Red   = PalettePtr->red;
				ARGBPtr->Green = PalettePtr->green;
				ARGBPtr->Blue  = PalettePtr->blue;
				}

			ImgSrcPtr = img->aiomi_ImageData;
			ARGBImgDestPtr = ARGBImgArray;
			for (n = 0; n < ImgSize; n++)
				{
				*ARGBImgDestPtr++ = ARGBArray[*ImgSrcPtr++];
				}

			WriteARGBArray(ARGBImgArray, 0, 0,
				inst->aio_MappedWidth * sizeof(struct ARGB),
				rp, Left, Top, 
				Width, Height);
			}
		else
			{
			struct ColorRegister *PalettePtr;
			UBYTE *PenListPtr;
			UBYTE *ImgPtr;
			UBYTE *ImgDestPtr;
			ULONG Remainder;
			ULONG n;
			ULONG y;

			img->aiomi_PenList = MyAllocVecPooled(img->aiomi_PaletteSize);
			if (NULL == img->aiomi_PenList)
				break;

			MappedImage = MyAllocVecPooled(inst->aio_MappedHeight * ((inst->aio_MappedWidth + 15) & 0xfff0));
			if (NULL == MappedImage)
				break;

			PalettePtr = img->aiomi_Palette;
			PenListPtr = img->aiomi_PenList;
			for (n = 0; n < img->aiomi_PaletteSize; n++)
				{
				*PenListPtr++ = ObtainBestPenA(inst->aio_Screen->ViewPort.ColorMap,
					PalettePtr->red << 24,
					PalettePtr->green << 24,
					PalettePtr->blue << 24,
					NULL);
				PalettePtr++;
				}

			Remainder = ((inst->aio_MappedWidth + 15) & 0xfff0) - inst->aio_MappedWidth;
			ImgPtr = img->aiomi_ImageData;
			ImgDestPtr = MappedImage;
			for (y = 0; y < inst->aio_MappedHeight; y++)
				{
				ULONG x;

				for (x = 0; x < inst->aio_MappedWidth; x++)
					{
					*ImgDestPtr++ = img->aiomi_PenList[*ImgPtr++];
					}
				ImgDestPtr += Remainder;
				}

			if (CyberGfxBase && GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
				{
				WritePixelArray(MappedImage, 0, 0,
					(inst->aio_MappedWidth + 15) & 0xfff0,
					rp, Left, Top,
					Width, Height,
					RECTFMT_LUT8);
				}
			else
				{
				struct RastPort rpTemp;

				InitRastPort(&rpTemp);
				rpTemp.BitMap = AllocBitMap((inst->aio_MappedWidth + 15) & 0xfff0,
					inst->aio_MappedHeight, 8, 0, rp->BitMap);
				if (NULL == rpTemp.BitMap)	
					break;

				WritePixelArray8(rp, Left, Top,
					Left + inst->aio_MappedWidth - 1,
					Top + inst->aio_MappedHeight - 1,
					MappedImage, &rpTemp);

				FreeBitMap(rpTemp.BitMap);
				}
			}

		Success = TRUE;
		} while (0);

	if (ARGBImgArray)
		MyFreeVecPooled(ARGBImgArray);
	if (ARGBArray)
		MyFreeVecPooled(ARGBArray);
	if (MappedImage)
		MyFreeVecPooled(MappedImage);
	if (!Success && img->aiomi_PenList)
		{
		MyFreeVecPooled(img->aiomi_PenList);
		img->aiomi_PenList = NULL;
		}
}

//----------------------------------------------------------------------------------------

static void MyDrawIconState(struct InstanceData *inst, struct RastPort *rp, 
	struct DrawInfo *drawInfo, ULONG State)
{
	d1(kprintf("%s/%ld:  Screen=%08lx  aio_fLayoutOk=%ld\n", \
		__FUNC__, __LINE__, inst->aio_Screen, inst->aio_fLayoutOk));

	if (!inst->aio_fLayoutOk)
		{
		// LayoutIconA()
		LayoutIcon(inst->aio_DiskObject, inst->aio_Screen,
			TAG_END);

		inst->aio_fLayoutOk = TRUE;
		}

	// DrawIconStateA()
	DrawIconState(rp, inst->aio_DiskObject, NULL,
		inst->aio_ImageLeft, inst->aio_ImageTop, State,
		ICONDRAWA_DrawInfo, (ULONG) drawInfo,
		ICONDRAWA_Frameless, TRUE,
		ICONDRAWA_Borderless, TRUE,
		ICONDRAWA_EraseBackground, FALSE,
		TAG_END);
}

//----------------------------------------------------------------------------------------

static UBYTE *GenerateMask(struct InstanceData *inst, struct MappedImage *img)
{
	UBYTE *MaskPlane = NULL;

	do	{
		ULONG BytesPerRow;
		size_t MaskSize;
		ULONG y;
		UBYTE *MaskPtr;
		UBYTE *ImgPtr;

		if (~0 == img->aiomi_TransparentColor)
			break;
		if (NULL == img->aiomi_ImageData)
			break;

		BytesPerRow = ((inst->aio_MappedWidth + 15) & 0xfff0) >> 3;
		MaskSize = inst->aio_MappedHeight * BytesPerRow;

		MaskPlane = MyAllocVecPooled(MaskSize);
		if (NULL == MaskPlane)
			break;

		memset(MaskPlane, 0, MaskSize);

		ImgPtr = img->aiomi_ImageData;
		MaskPtr = MaskPlane;
		for (y = 0; y < inst->aio_MappedHeight; y++)
			{
			ULONG x;
			UBYTE *MaskPtr2 = MaskPtr;
			UBYTE BitMask = 0x80;

			for (x = 0; x < inst->aio_MappedWidth; x++)
				{
				if (*ImgPtr++ != img->aiomi_TransparentColor)
					{
					*MaskPtr2 |= BitMask;
					}
				BitMask >>= 1;
				if (0 == BitMask)
					{
					BitMask = 0x80;
					MaskPtr2++;
					}
				}

			MaskPtr += BytesPerRow;
			}

		img->aiomi_Mask = MaskPlane;
		} while (0);

	return MaskPlane;
}

//----------------------------------------------------------------------------------------

// set image borders of superclass to 0 
// for icons with Renderhook
static void SetSuperImgBorders(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	if (inst->aio_RenderHook)
		{
		static struct TagItem AttrList[] =
			{
			{ IDTA_InnerLeft, 0 },
			{ IDTA_InnerTop, 0 },
			{ IDTA_InnerRight, 0 },
			{ IDTA_InnerBottom, 0 },
			{ TAG_END, 0 },
			};

		DoSuperMethod(cl, o, OM_SET, AttrList, ops->ops_GInfo);
		}
}

//----------------------------------------------------------------------------------------

static APTR MyAllocVecPooled(size_t Size)
{
	APTR ptr;

	if (MemPool)
		{
		ObtainSemaphore(&PubMemPoolSemaphore);
		ptr = AllocPooled(MemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&PubMemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(kprintf("%/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FUNC__, __LINE__, MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%ld:  MemPool=%08lx  Size=%lu\n", __FUNC__, __LINE__, MemPool, Size));

	return NULL;
}


static void MyFreeVecPooled(APTR mem)
{
	d1(kprintf("%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FUNC__, __LINE__, MemPool, mem));
	if (MemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&PubMemPoolSemaphore);
		FreePooled(MemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&PubMemPoolSemaphore);
		}
}

//----------------------------------------------------------------------------------------

static void WriteARGBArray(const struct ARGB *SrcImgData,
	ULONG SrcX, ULONG SrcY, ULONG SrcBytesPerRow,
	struct RastPort *DestRp, ULONG DestX, ULONG DestY,
	ULONG SizeX, ULONG SizeY)
{
	struct BitMap *DestBM = DestRp->BitMap;
	ULONG DestWidth;
	ULONG DestHeight;

	DestWidth = GetCyberMapAttr(DestBM, CYBRMATTR_WIDTH);
	DestHeight = GetCyberMapAttr(DestBM, CYBRMATTR_HEIGHT);

	d1(KPrintF("%s/%ld:  DestX=%ld  DestY=%ld  SizeX=%ld  SizeY=%ld\n", __FUNC__, __LINE__, DestX, DestY, SizeX, SizeY));
	d1(KPrintF("%s/%ld:  SrcBytesPerRow=%ld\n", __FUNC__, __LINE__, SrcBytesPerRow));

	if (DestX + SizeX > DestWidth)
		SizeX = DestWidth - DestX;
	if (DestY + SizeY > DestHeight)
		SizeY = DestHeight - DestY;

	d1(KPrintF("%s/%ld:  DestX=%ld  DestY=%ld  SizeX=%ld  SizeY=%ld\n", __FUNC__, __LINE__, DestX, DestY, SizeX, SizeY));

	WritePixelArray((APTR) SrcImgData, SrcX, SrcY,
		SrcBytesPerRow,
		DestRp, DestX, DestY,
		SizeX, SizeY,
		RECTFMT_ARGB);
}

//----------------------------------------------------------------------------------------

static BOOL MappedImageFromSAC(struct InstanceData *inst,
	struct MappedImage *mi, struct ScalosBitMapAndColor *sac)
{
	struct MappedImage MiNew;
	struct BitMap *TempBM = NULL;
	BOOL Success = FALSE;

	do	{
		ULONG n;
		ULONG y;
		const ULONG *PaletteSrcPtr;
		struct ColorRegister *PaletteDestPtr;
		struct RastPort rp;
		struct RastPort TempRp;
		UBYTE *ImagePtr;
		ULONG NewWidth  = sac->sac_Width;
		ULONG NewHeight = sac->sac_Height;

		if (NULL == sac)
			break;

		InitRastPort(&rp);
		InitRastPort(&TempRp);

		rp.BitMap = sac->sac_BitMap;
		d1(KPrintF(__FILE__ "/%s/%ld: rp.BitMap=%08lx\n", __FUNC__, __LINE__, rp.BitMap));

		// setup temp. RastPort for use by WritePixelLine8()
		TempRp.Layer = NULL;
		TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(NewWidth), 1, 8, 0, NULL);

		if (NULL == TempBM)
			break;

		memset(&MiNew, 0, sizeof(MiNew));

		MiNew.aiomi_ImageData = MiNew.aiomi_AllocatedImageData = MyAllocVecPooled(PIXELARRAY8_BUFFERSIZE(NewWidth, NewHeight));
		if (NULL == MiNew.aiomi_ImageData)
			break;

		MiNew.aiomi_PaletteSize	= sac->sac_NumColors;
		MiNew.aiomi_Palette = MiNew.aiomi_AllocatedPalette = MyAllocVecPooled(sac->sac_NumColors * sizeof(struct ColorRegister));
		if (NULL == MiNew.aiomi_Palette)
			break;

		if (SAC_TRANSPARENT_NONE != sac->sac_TransparentColor)
			MiNew.aiomi_TransparentColor = sac->sac_TransparentColor;
		else
			MiNew.aiomi_TransparentColor = ~0;

		// nothing can go wrong from here on
		Success = TRUE;

		d1(KPrintF("%s/%ld: NumColors=%lu\n", __FUNC__, __LINE__, sac->sac_NumColors));

		// Fill nim_Palette fom sac_ColorTable
		for (n = 0, PaletteSrcPtr = sac->sac_ColorTable, PaletteDestPtr = MiNew.aiomi_Palette;
			n < sac->sac_NumColors; n++, PaletteDestPtr++)
			{
			PaletteDestPtr->red   = *PaletteSrcPtr++ >> 24;	// red
			PaletteDestPtr->green = *PaletteSrcPtr++ >> 24;	// green
			PaletteDestPtr->blue  = *PaletteSrcPtr++ >> 24;	// blue
			}

		d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

		// copy image data from sac_BitMap into aiomi_ImageData
		for (y = 0, ImagePtr = MiNew.aiomi_ImageData; y < NewHeight; y++)
			{
			d1(KPrintF("%s/%ld: y=%ld\n", __FUNC__, __LINE__, y));
			ReadPixelLine8(&rp, 0, y, NewWidth,
				ImagePtr, &TempRp);
			ImagePtr += NewWidth;
			}

		d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));

		inst->aio_MappedWidth = NewWidth;
		inst->aio_MappedHeight = NewHeight;
		} while (0);

	if (Success)
		{
		FreeMappedImage(inst, mi);
		*mi = MiNew;
		}
	else
		{
		FreeMappedImage(inst, &MiNew);
		}

	if (TempBM)
		FreeBitMap(TempBM);

	return Success;
}

//----------------------------------------------------------------------------------------

static void FreeMappedImage(struct InstanceData *inst, struct MappedImage *mi)
{
	if (mi)
		{
		if (mi->aiomi_PenList)
			{
			FreePenList(inst->aio_Screen, &mi->aiomi_PenList, mi->aiomi_PaletteSize);
			}
		if (mi->aiomi_AllocatedImageData)
			{
			MyFreeVecPooled(mi->aiomi_AllocatedImageData);
			mi->aiomi_AllocatedImageData = mi->aiomi_ImageData = NULL;
			}
		if (mi->aiomi_AllocatedPalette)
			{
			MyFreeVecPooled(mi->aiomi_AllocatedPalette);
			mi->aiomi_AllocatedPalette = mi->aiomi_Palette = NULL;
			mi->aiomi_PaletteSize = 0;
			}
		if (mi->aiomi_Mask)
			{
			MyFreeVecPooled(mi->aiomi_Mask);
			mi->aiomi_Mask = NULL;
			}
		}
}

//----------------------------------------------------------------------------------------

static BOOL SelectedImageFromNormalImage(struct InstanceData *inst)
{
	struct MappedImage MiNew;

	BOOL Success = FALSE;

	do	{
		ULONG n;
		const struct ColorRegister *PaletteSrcPtr;
		struct ColorRegister *PaletteDestPtr;


		memset(&MiNew, 0, sizeof(MiNew));

		// share aiomi_ImageData with normal image
		MiNew.aiomi_ImageData = inst->aio_Image1.aiomi_ImageData;

		MiNew.aiomi_PaletteSize	= inst->aio_Image1.aiomi_PaletteSize;
		MiNew.aiomi_Palette = MiNew.aiomi_AllocatedPalette = MyAllocVecPooled(MiNew.aiomi_PaletteSize * sizeof(struct ColorRegister));
		if (NULL == MiNew.aiomi_Palette)
			break;

		MiNew.aiomi_TransparentColor = inst->aio_Image1.aiomi_TransparentColor;

		// nothing can go wrong from here on
		Success = TRUE;

		d1(KPrintF("%s/%ld: NumColors=%lu\n", __FUNC__, __LINE__, MiNew.aiomi_PaletteSize));

		// copy palette from aio_Image1 and adjust colors
		for (n = 0, PaletteSrcPtr = inst->aio_Image1.aiomi_Palette, PaletteDestPtr = MiNew.aiomi_Palette;
			n < MiNew.aiomi_PaletteSize; n++)
			{
			*PaletteDestPtr = *PaletteSrcPtr++;
			ChangeToSelectedIconColor(PaletteDestPtr++);
			}

		d1(KPrintF("%s/%ld: \n", __FUNC__, __LINE__));
		} while (0);

	if (Success)
		{
		FreeMappedImage(inst, &inst->aio_Image2);
		inst->aio_Image2 = MiNew;
		}
	else
		{
		FreeMappedImage(inst, &MiNew);
		}

	return Success;
}

//----------------------------------------------------------------------------------------

static void MySetFont(Object *o, struct RastPort *rp)
{
	struct	Hook *FontHook = NULL;
	struct	TextFont *IconFont = NULL;

	GetAttr(IDTA_Font, o, (APTR) &IconFont);
	GetAttr(IDTA_FontHook, o, (APTR) &FontHook);

	if (FontHook)
		{
		APTR FontHandle = NULL;

		GetAttr(IDTA_Fonthandle, o, (APTR) &FontHandle);

		if (FontHandle || IconFont)
			{
			struct ioFontMsg_SetFont iofsf;

			iofsf.MsgID = iofMsg_SETFONT;
			iofsf.iofsf_RastPort = rp;
			iofsf.iofsf_FontHandle = FontHandle;
			iofsf.iofsf_TextFont = IconFont;

			CallHookPkt(FontHook, NULL, &iofsf);

			d1(KPrintF("%s/%ld: rp=%08lx  FontHook=%08lx  FontHandle=%08lx  IconFont=%08lx\n", \
				__FUNC__, __LINE__, rp, FontHook, FontHandle, IconFont));
			}
		}
	else
		{
		if (IconFont)
			SetFont(rp, IconFont);
		}
}

//----------------------------------------------------------------------------------------


#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2INITLIB(InitDatatype, 0);
ADD2EXPUNGELIB(CloseDatatype, 0);
ADD2OPENLIB(OpenDatatype, 0);

#endif
//-----------------------------------------------------------------------------
