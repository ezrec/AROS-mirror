// GlowIconObject.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/resident.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/rpattr.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <libraries/iffparse.h>
#include <datatypes/iconobject.h>
#include <cybergraphx/cybergraphics.h>

#define	__USE_SYSBASE
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <proto/iffparse.h>
#include <proto/wb.h>
#include <proto/timer.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/icon.h>

#include <string.h>
#include <ctype.h>

#include <defs.h>
#include <Year.h>
#include <zlib.h>

#include "GlowIconObject.h"

//----------------------------------------------------------------------------

#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))


#define	NO_ICON_POSITION_WORD	((WORD) 0x8000)

//----------------------------------------------------------------------------

#ifndef __AROS__
struct ExecBase *SysBase;
struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct Library *WorkbenchBase;
struct Library *IconBase;
struct IntuitionBase *IntuitionBase;
struct Library *CyberGfxBase;
struct Library *IFFParseBase;
struct DosLibrary *DOSBase;
#endif
struct Library *IconObjectDTBase;
#ifdef TIMESTAMPS
T_TIMERBASE TimerBase;
#endif /* TIMESTAMPS */
#ifdef __amigaos4__
struct Library *NewlibBase;

struct Interface *INewlib;
struct ExecIFace *IExec;
struct GraphicsIFace *IGraphics;
struct UtilityIFace *IUtility;
struct WorkbenchIFace *IWorkbench;
struct IconIFace *IIcon;
struct IntuitionIFace *IIntuition;
struct CyberGfxIFace *ICyberGfx;
struct IFFParseIFace *IIFFParse;
struct DOSIFace *IDOS;
#endif /* __amigaos4__ */

static void *MemPool;
static struct SignalSemaphore PubMemPoolSemaphore;

static Class *GlowIconObjectClass;

//----------------------------------------------------------------------------
// Standard library functions

SAVEDS(ULONG) INTERRUPT GlowIconObjectDispatcher(Class *cl, Object *o, Msg msg);

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops);
static ULONG DtDispose(Class *cl, Object *o, Msg msg);
static ULONG DtSet(Class *cl, Object *o, struct opSet *ops);
static ULONG DtGet(Class *cl, Object *o, struct opGet *opg);
static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl);
static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf);
static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *iopw);
static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *iopw);
static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio);

//----------------------------------------------------------------------------

static void FreePenList(struct Screen *screen, struct NewImagePenList *PenList);
static BOOL DoClonePenList(struct Screen *scr, struct NewImagePenList *nipDest,
	const struct NewImagePenList *nipSrc);
static struct Image *DoCloneDoImage(struct Image *imgDest, const struct Image *imgSrc);
static void DoUpdateWb(CONST_STRPTR SavePath);
static void INLINE WriteARGBArray(const struct ARGB *SrcImgData,
	ULONG SrcX, ULONG SrcY, ULONG SrcBytesPerRow,
	struct RastPort *DestRp, ULONG DestX, ULONG DestY,
	ULONG SizeX, ULONG SizeY);
static BOOL INLINE DrawTrueColorImage(struct RastPort *rp, struct NewImage *img, 
	UWORD Left, UWORD Top, const struct ColorRegister *Palette, 
	BOOL DimmedImage);
static BOOL INLINE DrawRemappedImage(struct InstanceData *inst, struct RastPort *rp, 
	struct NewImagePenList *PenList, struct NewImage *img, 
	UWORD Left, UWORD Top, const struct ColorRegister *Palette, ULONG PaletteSize,
	BOOL DimmedImage);
static void DrawColorImage(struct InstanceData *inst, struct RastPort *rp, 
	struct NewImagePenList *PenList, struct NewImage *img, 
	UWORD Left, UWORD Top, const struct ColorRegister *Palette, ULONG PaletteSize,
	BOOL DimmedImage);
static void ReadIcon(struct InstanceData *inst, CONST_STRPTR Filename, BPTR IconFh);
static BOOL WriteIcon(Class *cl, Object *o,
	const struct DiskObject *WriteDiskObject, CONST_STRPTR Filename);
static BOOL ReadStandardIcon(struct InstanceData *inst, BPTR fd);
static BOOL WriteStandardIcon(struct InstanceData *inst,
	const struct DiskObject *WriteDiskObject, BPTR fd);
static struct NewImage *ReadNewImage(struct IFFHandle *iff, const struct FaceChunk *fc);
static struct NewImage *ReadARGBImage(struct IFFHandle *iff, const struct FaceChunk *fc);
static LONG WriteNewImage(struct IFFHandle *iff, const struct NewImage *ni);
static LONG WriteARGBImage(Class *cl, Object *o, struct IFFHandle *iff,
	const struct NewImage *ni, ULONG ARGBImageTag);
static void FreeNewImage(struct NewImage **ni);
static BOOL CloneNewImage(struct NewImage **niClone, const struct NewImage *niSrc);
static UBYTE *EncodeData(ULONG Depth, UWORD *DestLength,
	ULONG SrcLength, const UBYTE *SrcData);
static BOOL DecodeData(const UBYTE *src, UBYTE *dest, size_t SrcLength, 
	size_t DestLength, LONG BitsPerEntry);
static UBYTE INLINE GetNextDestByte(const UBYTE **src, size_t *SrcLength, short *srcBits, 
	UBYTE *srcByte, LONG BitsPerEntry);
static UBYTE *GenerateMask(struct NewImage *nim);
static struct NewImage *NewImageFromSAC(struct ScalosBitMapAndColor *sac);
static void GenerateNormalImageMask(Class *cl, Object *o);
static void GenerateSelectedImageMask(Class *cl, Object *o);
static struct NewImage *NewImageFromNormalImage(const struct InstanceData *inst, const struct NewImage *niNormal);
static SAVEDS(LONG) StreamHookDispatcher(struct Hook *hook, struct IFFHandle *iff, const struct IFFStreamCmd *cmd);
static void SetParentAttributes(Class *cl, Object *o);

#if defined(__AROS__)
static BOOL ReadConvertStandardIcon(BPTR fd, struct DiskObject *dobj);
static BOOL WriteConvertStandardIcon(BPTR fd, struct DiskObject *dobj);
static BOOL ReadConvertDrawerData(BPTR fd, struct DrawerData *drawer);
static BOOL WriteConvertDrawerData(BPTR fd, struct DrawerData *drawer);
static BOOL ReadConvertImage(BPTR fd, struct Image *img);
static BOOL WriteConvertImage(BPTR fd, struct Image *img);
#endif

//----------------------------------------------------------------------------

static APTR MyAllocVecPooled(size_t Size);
static void MyFreeVecPooled(APTR mem);
static void *ZLibAlloc(void *p, int items, int size);
static void ZLibFree(void *p, void *addr);

//----------------------------------------------------------------------------

char ALIGNED libName[] = "glowiconobject.datatype";
char ALIGNED libIdString[] = "$VER: glowiconobject.datatype "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (25 Jan 2008 22:31:59) "
	COMPILER_STRING
	" ©2004" CURRENTYEAR " The Scalos Team";

static struct Hook StreamHook = 
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(StreamHookDispatcher),	// h_Entry + h_SubEntry
	NULL,					// h_Data
	};

//----------------------------------------------------------------------------

	#ifdef __AROS__
AROS_LH0(ULONG, ObtainInfoEngine,
    struct Library *, libBase, 5, GlowIconobject
)
{
	AROS_LIBFUNC_INIT
	return (ULONG) GlowIconObjectClass;
	AROS_LIBFUNC_EXIT
}
#else
LIBFUNC(ObtainInfoEngine, libbase, ULONG)
{
	(void) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));
	
	return (ULONG) GlowIconObjectClass;
}
LIBFUNC_END
#endif

//-----------------------------------------------------------------------------

// return 0 if error occurred
ULONG InitDatatype(struct GlowIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	dtLib->nib_Initialized = FALSE;

	return 1;
}

// return 0 if error occurred
ULONG OpenDatatype(struct GlowIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (!dtLib->nib_Initialized)
		{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		dtLib->nib_Initialized = TRUE;

		InitSemaphore(&PubMemPoolSemaphore);

		DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 39);
		d1(kprintf("%s/%s/%ld: DOSBase=%08lx\n", __FILE__, __FUNC__, __LINE__, DOSBase));
		if (NULL == DOSBase)
			return 0;
#ifdef __amigaos4__
		IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
		if (NULL == IDOS)
			return 0;
#endif /* __amigaos4__ */

		IFFParseBase = OpenLibrary("iffparse.library", 39);
		d1(kprintf("%s/%s/%ld: IFFParseBase=%08lx\n", __FILE__, __FUNC__, __LINE__, IFFParseBase));
		if (NULL == IFFParseBase)
			return 0;
#ifdef __amigaos4__
		IIFFParse = (struct IFFParseIFace *)GetInterface((struct Library *)IFFParseBase, "main", 1, NULL);
		if (NULL == IIFFParse)
			return 0;
#endif /* __amigaos4__ */

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		d1(kprintf("%s/%s/%ld: IntuitionBase=%08lx\n", __FILE__, __FUNC__, __LINE__, IntuitionBase));
		if (NULL == IntuitionBase)
			return 0;
#ifdef __amigaos4__
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			return 0;
#endif /* __amigaos4__ */

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
		d1(kprintf("%s/%s/%ld: UtilityBase=%08lx\n", __FILE__, __FUNC__, __LINE__, UtilityBase));
		if (NULL == UtilityBase)
			return 0;
#ifdef __amigaos4__
		IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
		if (NULL == IUtility)
			return 0;
#endif /* __amigaos4__ */

		IconBase = OpenLibrary(ICONNAME, 39);
		d1(kprintf("%s/%s/%ld: IconBase=%08lx\n", __FILE__, __FUNC__, __LINE__, IconBase));
		if (NULL == IconBase)
			return 0;
#ifdef __amigaos4__
		IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
		if (NULL == IIcon)
			return 0;
#endif /* __amigaos4__ */

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
		d1(kprintf("%s/%s/%ld: GfxBase=%08lx\n", __FILE__, __FUNC__, __LINE__, GfxBase));
		if (NULL == GfxBase)
			return 0;
#ifdef __amigaos4__
		IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
		if (NULL == IGraphics)
			return 0;
#endif /* __amigaos4__ */

		WorkbenchBase = OpenLibrary("workbench.library", 39);
		d1(KPrintF("%s/%s/%ld: WorkbenchBase=%08lx\n", __FILE__, __FUNC__, __LINE__, WorkbenchBase));
		if (NULL == WorkbenchBase)
			return 0;
#ifdef __amigaos4__
		IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
		if (NULL == IWorkbench)
			return 0;
#endif /* __amigaos4__ */

		IconObjectDTBase = OpenLibrary("datatypes/iconobject.datatype", 39);
		if (NULL == IconObjectDTBase)
			return 0;

#ifdef TIMESTAMPS
		{
		struct timerequest *iorequest;

		iorequest = (struct timerequest *) CreateIORequest(CreateMsgPort(), sizeof(struct timerequest));
		OpenDevice("timer.device", UNIT_VBLANK, &iorequest->tr_node, 0);
		TimerBase = (T_TIMERBASE) iorequest->tr_node.io_Device;
		}
#endif /* TIMESTAMPS */

#if !defined(__amigaos4__) && !defined(__AROS__)
		if (_STI_240_InitMemFunctions())
			return 0;
#endif /* __amigaos4__ */

		MemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		d1(kprintf("%s/%s/%ld: MemPool=%08lx\n", __FILE__, __FUNC__, __LINE__, MemPool));
		if (NULL == MemPool)
			return 0;

		GlowIconObjectClass = dtLib->nib_ClassLibrary.cl_Class = MakeClass(libName, 
			"iconobject.datatype", NULL, sizeof(struct InstanceData), 0);
		d1(kprintf("%s/%s/%ld:  GlowIconObjectClass=%08lx\n", __FILE__, __FUNC__, __LINE__, GlowIconObjectClass));
		if (NULL == GlowIconObjectClass)
			return 0;

		SETHOOKFUNC(GlowIconObjectClass->cl_Dispatcher, GlowIconObjectDispatcher);
		GlowIconObjectClass->cl_Dispatcher.h_Data = dtLib;

		// Make class available for the public
		AddClass(GlowIconObjectClass);
#ifdef __AROS__
		dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 10;
#endif

		CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
		// CyberGfxBase may be NULL
#ifdef __amigaos4__
		if (NULL != CyberGfxBase)
			{
			ICyberGfx = (struct CyberGfxIFace *)GetInterface((struct Library *)CyberGfxBase, "main", 1, NULL);
			if (NULL == ICyberGfx)
				{
				CloseLibrary(CyberGfxBase);
				CyberGfxBase = NULL;
				}
			}
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			return 0;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			return 0;
#endif /* __amigaos4__ */
		}

	d1(kprintf("%s/%s/%ld:  Open Success!\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

ULONG CloseDatatype(struct GlowIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt < 1)
		{
		if (GlowIconObjectClass)
			{
			RemoveClass(GlowIconObjectClass);
			FreeClass(GlowIconObjectClass);
			GlowIconObjectClass = dtLib->nib_ClassLibrary.cl_Class = NULL;
			}

		if (NULL != IconObjectDTBase)
			{
			CloseLibrary(IconObjectDTBase);
			IconObjectDTBase = NULL;
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
		if (NULL != IWorkbench)
			{
			DropInterface((struct Interface *)IWorkbench);
			IWorkbench = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != WorkbenchBase)
			{
			CloseLibrary(WorkbenchBase);
			WorkbenchBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != IIcon)
			{
			DropInterface((struct Interface *)IIcon);
			IIcon = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != IconBase)
			{
			CloseLibrary(IconBase);
			IconBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != ICyberGfx)
			{
			DropInterface((struct Interface *)ICyberGfx);
			ICyberGfx = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != CyberGfxBase)
			{
			CloseLibrary(CyberGfxBase);
			CyberGfxBase = NULL;
			}
#if !defined(__amigaos4__) && !defined(__AROS__)
		_STD_240_TerminateMemFunctions();
#endif /* __amigaos4__ */
		if (NULL != MemPool)
			{
			DeletePool(MemPool);
			MemPool = NULL;
			}

#ifdef __amigaos4__
		if (NULL != IGraphics)
			{
			DropInterface((struct Interface *)IGraphics);
			IGraphics = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != GfxBase)
			{
			CloseLibrary((struct Library *) GfxBase);
			GfxBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != IIntuition)
			{
			DropInterface((struct Interface *)IIntuition);
			IIntuition = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != IntuitionBase)
			{
			CloseLibrary((struct Library *) IntuitionBase);
			IntuitionBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != IIFFParse)
			{
			DropInterface((struct Interface *)IIFFParse);
			IIFFParse = NULL;
			}
#endif /* __amigaos4__ */
		if (IFFParseBase)
			{
			CloseLibrary(IFFParseBase);
			IFFParseBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != IUtility)
			{
			DropInterface((struct Interface *)IUtility);
			IUtility = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != UtilityBase)
			{
			CloseLibrary((struct Library *) UtilityBase);
			UtilityBase = NULL;
			}
#ifdef __amigaos4__
		if (NULL != IDOS)
			{
			DropInterface((struct Interface *)IDOS);
			IDOS = NULL;
			}
#endif /* __amigaos4__ */
		if (NULL != DOSBase)
			{
			CloseLibrary((struct Library *)DOSBase);
			DOSBase = NULL;
			}
		}
	return 1;
}

//-----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT GlowIconObjectDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(kprintf("%s/%s/%ld:  MethodID=%08lx o=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID, o));

	switch (msg->MethodID)
		{
	case OM_NEW:
		o = (Object *) DoSuperMethodA(cl, o, msg);
		d1(kprintf("%s/%s/%ld:  OM_NEW  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		if (o)
			{
			d1(kprintf("%s/%s/%ld:  OM_NEW  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
			if (!DtNew(cl, o, (struct opSet *) msg))
				{
				DoMethod(o, OM_DISPOSE);
				o = NULL;
				}
			}
		Result = (ULONG) o;
		break;

	case OM_DISPOSE:
		d1(kprintf("%s/%s/%ld:  OM_DISPOSE  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
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

	case IDTM_FreeLayout:
		d1(kprintf("%s/%s/%ld:  IDTM_FreeLayout  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
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
		d1(kprintf("%s/%s/%ld:  default  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	d1(kprintf("%s/%s/%ld:  MethodID=%08lx o=%08lx Result=%ld\n", __FILE__, __FUNC__, __LINE__,
		msg->MethodID, o, Result));
	
	return Result;
}

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	BOOL Success = FALSE;

	do	{
		ULONG DefIconType;
		STRPTR IconName;
		ULONG SupportedIconTypes;
		BPTR IconFh;

		d1(kprintf("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
		TIMESTAMP_d1();

		memset(inst, 0, sizeof(struct InstanceData));

		inst->aio_BackfillPenNorm = inst->aio_BackfillPenSel = IDTA_BACKFILL_NONE;

		SupportedIconTypes = GetTagData(IDTA_SupportedIconTypes, ~0, ops->ops_AttrList);
		if (!(SupportedIconTypes & IDTV_IconType_ColorIcon))
			break;

		inst->aio_ImageLeft = GetTagData(IDTA_InnerLeft, 0, ops->ops_AttrList);
		inst->aio_ImageTop  = GetTagData(IDTA_InnerTop,  0, ops->ops_AttrList);

		DefIconType = GetTagData(IDTA_DefType,  0,  ops->ops_AttrList);
		d1(kprintf("%s/%s/%ld:  o=%08lx  DefIconType=%lu\n", __FILE__, __FUNC__, __LINE__, o, DefIconType));

		if (0 != DefIconType)
			break;

		if (NULL != (struct DiskObject *) GetTagData(AIDTA_Icon, (ULONG) NULL, ops->ops_AttrList))
			break;

		if (FindTagItem(IDTA_CloneIconObject, ops->ops_AttrList))
			{
			d1(KPrintF("%s/%s/%ld: IDTA_CloneIconObject\n", __FILE__, __FUNC__, __LINE__));
			}
		else
			{
			GetAttr(DTA_Name, o, (APTR) &IconName);
			d1(KPrintF("%s/%s/%ld:  o=%08lx  IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, o, IconName ? IconName : (STRPTR) ""));

			// Get named icon
			if (NULL == IconName)
				break;

			IconFh = (BPTR)GetTagData(DTA_Handle, 0, ops->ops_AttrList);
			d1(KPrintF("%s/%s/%ld: IconFh=%08lx\n", __FILE__, __FUNC__, __LINE__, IconFh));

			TIMESTAMP_d1();
			ReadIcon(inst, IconName, IconFh);
			TIMESTAMP_d1();
			if (NULL == inst->aio_Image1)
				break;		// fall back to standard icon if no NewImage

			if (NULL == inst->aio_Image2)
				{
				// No selected image available.
				// Generate darkened selected image from normal image
				struct NewImage *ni;

				TIMESTAMP_d1();
				ni = NewImageFromNormalImage(inst, inst->aio_Image1);
				d1(KPrintF("%s/%s/%ld: ni=%08lx\n", __FILE__, __FUNC__, __LINE__, ni));
				if (NULL == ni)
					break;

				FreeNewImage(&inst->aio_Image2);
				inst->aio_Image2 = ni;
				}

			d1(kprintf("%s/%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_DiskObject));
			if (NULL == inst->aio_DiskObject)
				break;

			if (NO_ICON_POSITION == inst->aio_DiskObject->do_CurrentX)
				{
				gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_WORD;
				}
			else
				{
				gg->LeftEdge = inst->aio_DiskObject->do_CurrentX;
				gg->TopEdge = inst->aio_DiskObject->do_CurrentY;
				}

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

			SetParentAttributes(cl, o);
			TIMESTAMP_d1();
			}

		d1(kprintf("%s/%s/%ld:  o=%08lx  GadgetRender=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->GadgetRender));
		TIMESTAMP_d1();

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld:  END o=%08lx  Success=%ld\n", __FILE__, __FUNC__, __LINE__, o, Success));
	TIMESTAMP_d1();

	return Success;
}

//-----------------------------------------------------------------------------

static ULONG DtDispose(Class *cl, Object *o, Msg msg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
//	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	d1(kprintf("%s/%s/%ld:  o=%08lx  aio_myDrawerData=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_myDrawerData));
	if (inst->aio_myDrawerData)
		{
		MyFreeVecPooled(inst->aio_myDrawerData);
		inst->aio_myDrawerData = NULL;
		if (inst->aio_DiskObject)
			inst->aio_DiskObject->do_DrawerData = NULL;
		}

	if (inst->aio_DefaultTool)
		{
		MyFreeVecPooled(inst->aio_DefaultTool);
		inst->aio_DefaultTool = NULL;
		}
	if (inst->aio_ToolTypes)
		{
		STRPTR *ToolTypePtr = inst->aio_ToolTypes;

		d1(KPrintF("%s/%s/%ld:  do_ToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->aio_DiskObject->do_ToolTypes));

		while (*ToolTypePtr)
			{
			d1(KPrintF("%s/%s/%ld:  *ToolTypePtr=<%s>\n", __FILE__, __FUNC__, __LINE__, *ToolTypePtr));
			MyFreeVecPooled(*ToolTypePtr);
			*ToolTypePtr = NULL;
			ToolTypePtr++;
			}

		MyFreeVecPooled(inst->aio_ToolTypes);
		inst->aio_ToolTypes = NULL;
		}
	if (inst->aio_ToolWindow)
		{
		MyFreeVecPooled(inst->aio_ToolWindow);
		inst->aio_ToolWindow = NULL;
		}

	d1(kprintf("%s/%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_DiskObject));

	if (inst->aio_DiskObject)
		{
		MyFreeVecPooled(inst->aio_DiskObject);
		inst->aio_DiskObject = NULL;
		}
	if (inst->aio_DoImage1.ImageData)
		{
		MyFreeVecPooled(inst->aio_DoImage1.ImageData);
		inst->aio_DoImage1.ImageData = NULL;
		}
	if (inst->aio_DoImage2.ImageData)
		{
		MyFreeVecPooled(inst->aio_DoImage2.ImageData);
		inst->aio_DoImage2.ImageData = NULL;
		}
	if (inst->aio_PenList1.nip_PenArray)
		{
		FreePenList(inst->aio_Screen, &inst->aio_PenList1); 
		}
	if (inst->aio_PenList2.nip_PenArray)
		{
		FreePenList(inst->aio_Screen, &inst->aio_PenList2);
		}

	FreeNewImage(&inst->aio_Image1);
	FreeNewImage(&inst->aio_Image2);

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------------------

static ULONG DtSet(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	inst->aio_ImageLeft = GetTagData(IDTA_InnerLeft, inst->aio_ImageLeft, ops->ops_AttrList);
	inst->aio_ImageTop  = GetTagData(IDTA_InnerTop,  inst->aio_ImageTop,  ops->ops_AttrList);

	inst->aio_BackfillPenNorm = GetTagData(IDTA_Backfill, inst->aio_BackfillPenNorm, ops->ops_AttrList);
	inst->aio_BackfillPenSel = GetTagData(IDTA_BackfillSel, inst->aio_BackfillPenSel, ops->ops_AttrList);

	return DoSuperMethodA(cl, o, (Msg) ops);
}

//----------------------------------------------------------------------------------------

static ULONG DtGet(Class *cl, Object *o, struct opGet *opg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG result = 1;

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	switch (opg->opg_AttrID)
		{
	case IDTA_IconType:
		*opg->opg_Storage = ioICONTYPE_GlowIcon;
		break;

	case IDTA_NumberOfColorsSupported:
		*opg->opg_Storage = inst->aio_Image1->nim_Palette ? 256 : (1 + 0x00ffffff);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  IDTA_NumberOfColorsSupported=%lu\n", __FILE__, __FUNC__, __LINE__, o, *opg->opg_Storage));
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

	d1(kprintf("%s/%s/%ld:  o=%08lx  GadgetRender=%08lx\n", __FILE__, __FUNC__, __LINE__, o, ((struct ExtGadget *) o)->GadgetRender));

	return result;
}

//-----------------------------------------------------------------------------

static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	TIMESTAMP_d1();
	DoSuperMethodA(cl, o, (Msg) iopl);
	TIMESTAMP_d1();

	d1(kprintf("%s/%s/%ld:  o=%08lx  Screen=%08lx\n", __FILE__, __FUNC__, __LINE__, o, iopl->iopl_Screen));
	d1(KPrintF("%s/%s/%ld:  o=%08lx  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, o, gg->Width, gg->Height));
	d1(kprintf("%s/%s/%ld:  o=%08lx  GadgetRender=%08lx  SelectRender=%08lx  Flags=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, o, gg->GadgetRender, gg->SelectRender, iopl->iopl_Flags));

	// skip layout if ARGB image (no nim_Palette)
	if (inst->aio_Image1->nim_Palette)
		{
		inst->aio_Screen = iopl->iopl_Screen;

		if (gg->GadgetRender && (iopl->iopl_Flags & IOLAYOUTF_NormalImage))
			{
			d1(KPrintF("%s/%s/%ld:  o=%08lx  Image1 nim_Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_Image1->nim_Palette));
			if (inst->aio_Image1->nim_Palette)
				{
				TIMESTAMP_d1();
				DrawColorImage(inst, (struct RastPort *) gg->GadgetRender,
					&inst->aio_PenList1, inst->aio_Image1,
					inst->aio_ImageLeft, inst->aio_ImageTop,
					inst->aio_Image1->nim_Palette, inst->aio_Image1->nim_PaletteSize,
					FALSE);
				TIMESTAMP_d1();
				}
			}
		if (gg->SelectRender && (iopl->iopl_Flags & IOLAYOUTF_SelectedImage))
			{
			BOOL DimImage;
			const struct ColorRegister *Palette;
			ULONG PaletteSize;
			struct NewImage *nim;

			d1(KPrintF("%s/%s/%ld:  o=%08lx  Image2=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_Image2));
			TIMESTAMP_d1();

			if (inst->aio_Image2)
				{
				nim = inst->aio_Image2;

				if (inst->aio_Image2->nim_ImageChunk.ic_Flags & ICF_HasPalette)
					{
					Palette = inst->aio_Image2->nim_Palette;
					PaletteSize = inst->aio_Image2->nim_PaletteSize;
					}
				else
					{
					Palette = inst->aio_Image1->nim_Palette;
					PaletteSize = inst->aio_Image1->nim_PaletteSize;
					}
				DimImage = FALSE;
				}
			else
				{
				nim = inst->aio_Image1;
				Palette = inst->aio_Image1->nim_Palette;
				PaletteSize = inst->aio_Image1->nim_PaletteSize;
				DimImage = TRUE;
				}

			d1(KPrintF("%s/%s/%ld:  o=%08lx  Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_Image1->nim_Palette));
			DrawColorImage(inst, (struct RastPort *) gg->SelectRender,
				&inst->aio_PenList2, nim,
				inst->aio_ImageLeft, inst->aio_ImageTop,
				Palette, PaletteSize, DimImage);
			}
		}

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
	TIMESTAMP_d1();

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf)
{
	struct InstanceData *inst = INST_DATA(cl, o);
//	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	if (inst->aio_Screen && (opf->iopf_Flags & IOFREELAYOUTF_ScreenAvailable))
		{
		if (inst->aio_PenList1.nip_PenArray)
			{
			FreePenList(inst->aio_Screen, &inst->aio_PenList1); 
			}
		if (inst->aio_PenList2.nip_PenArray)
			{
			FreePenList(inst->aio_Screen, &inst->aio_PenList2);
			}
		}
	else
		{
		if (inst->aio_PenList1.nip_PenArray)
			{
			MyFreeVecPooled(inst->aio_PenList1.nip_PenArray);
			inst->aio_PenList1.nip_PenArray = NULL;
			}
		if (inst->aio_PenList2.nip_PenArray)
			{
			MyFreeVecPooled(inst->aio_PenList2.nip_PenArray);
			inst->aio_PenList2.nip_PenArray = NULL;
			}
		}

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return DoSuperMethodA(cl, o, (Msg) opf);
}

//-----------------------------------------------------------------------------

static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *iopw)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct WriteData wd;
	struct DiskObject *WriteDiskObject;
	struct DiskObject DiskObjectCopy;
	ULONG Result = RETURN_OK;

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	do	{
		ULONG NeedUpdateWB;
		IPTR storage;

		memset(&DiskObjectCopy, 0, sizeof(DiskObjectCopy));

		NeedUpdateWB = GetTagData(ICONA_NotifyWorkbench, FALSE, iopw->iopw_Tags);

		// Allocate empty DiskObject
		WriteDiskObject = GetDiskObjectNew(NULL);
		d1(kprintf("%s/%s/%ld:  o=%08lx  WriteDiskObject=%08lx\n", __FILE__, __FUNC__, __LINE__, o, WriteDiskObject));
		if (NULL == WriteDiskObject)
			break;

		// Remember original contents of WriteDiskObject
		DiskObjectCopy = *WriteDiskObject;

		memset(&wd, 0, sizeof(wd));

		GetAttr(IDTA_Type, 		o, &storage);
		wd.aiowd_Type = storage;
		GetAttr(IDTA_ViewModes, 	o, &storage);
		wd.aiowd_ViewModes = storage;
		GetAttr(IDTA_Flags, 		o, &storage);
		wd.aiowd_Flags = storage;
		GetAttr(IDTA_WinCurrentX, 	o, &storage);
		wd.aiowd_CurrentX = storage;
		GetAttr(IDTA_WinCurrentY, 	o, &storage);
		wd.aiowd_CurrentY = storage;
		GetAttr(IDTA_WindowRect, 	o, &storage);
		wd.aiowd_WindowRect = (struct IBox *)storage;
		GetAttr(IDTA_Stacksize, 	o, &storage);
		wd.aiowd_StackSize = storage;
		GetAttr(IDTA_ToolTypes, 	o, &storage);
		wd.aiowd_ToolTypes = (STRPTR *)storage;
		GetAttr(IDTA_DefaultTool, 	o, &storage);
		wd.aiowd_DefaultTool = (STRPTR)storage;

		*WriteDiskObject = *inst->aio_DiskObject;

		WriteDiskObject->do_DefaultTool = wd.aiowd_DefaultTool;
		WriteDiskObject->do_ToolTypes = wd.aiowd_ToolTypes;
		WriteDiskObject->do_StackSize = wd.aiowd_StackSize;
		WriteDiskObject->do_Type = wd.aiowd_Type;
		WriteDiskObject->do_DrawerData = NULL;
		WriteDiskObject->do_Gadget = inst->aio_DiskObject->do_Gadget;
		WriteDiskObject->do_ToolWindow = inst->aio_DiskObject->do_ToolWindow;

		switch (WriteDiskObject->do_Type)
			{
		case WBTOOL:
		case WBPROJECT:
		case WBKICK:
			break;
		default:
			if (inst->aio_DiskObject->do_DrawerData)
				WriteDiskObject->do_DrawerData = inst->aio_DiskObject->do_DrawerData;
			else
				WriteDiskObject->do_DrawerData = inst->aio_myDrawerData;

			WriteDiskObject->do_DrawerData->dd_CurrentX = wd.aiowd_CurrentX;
			WriteDiskObject->do_DrawerData->dd_CurrentY = wd.aiowd_CurrentY;

			if (wd.aiowd_WindowRect)
				{
				WriteDiskObject->do_DrawerData->dd_NewWindow.LeftEdge = wd.aiowd_WindowRect->Left;
				WriteDiskObject->do_DrawerData->dd_NewWindow.TopEdge = wd.aiowd_WindowRect->Top;
				WriteDiskObject->do_DrawerData->dd_NewWindow.Width = wd.aiowd_WindowRect->Width;
				WriteDiskObject->do_DrawerData->dd_NewWindow.Height = wd.aiowd_WindowRect->Height;
				WriteDiskObject->do_DrawerData->dd_Flags = wd.aiowd_Flags;
				WriteDiskObject->do_DrawerData->dd_ViewModes = wd.aiowd_ViewModes;
				}
			break;
			}

		if (GetTagData(ICONA_NoPosition, 0, iopw->iopw_Tags))
			{
			WriteDiskObject->do_CurrentX = WriteDiskObject->do_CurrentY = NO_ICON_POSITION;
			}
		else
			{
			if (NO_ICON_POSITION_WORD == gg->LeftEdge)
				{
				WriteDiskObject->do_CurrentX = WriteDiskObject->do_CurrentY = NO_ICON_POSITION;
				}
			else
				{
				WriteDiskObject->do_CurrentX = gg->LeftEdge;
				WriteDiskObject->do_CurrentY = gg->TopEdge;
				}
			}

		d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, o, inst, iopw->iopw_Path));

		if (GetTagData(ICONA_NoNewImage, 0, iopw->iopw_Tags))
			{
			}

		if (NULL == inst->aio_Image1->nim_Palette)
			{
			// Special check for ARGB icons:
			// if no IDTA_SelARGBImageData is present (e.g. for created Thumbnails),
			// then make sure inst->aio_Image2 is cleared.
			struct ARGBHeader *argbh = NULL;

			GetAttr(IDTA_SelARGBImageData, o, (APTR) &argbh);
			if (NULL == argbh || NULL == argbh->argb_ImageData)
				FreeNewImage(&inst->aio_Image2);
			}

		if (!WriteIcon(cl, o, WriteDiskObject, iopw->iopw_Path))
			{
			Result = IoErr();
			break;
			}

		if (NeedUpdateWB)
			DoUpdateWb(iopw->iopw_Path);
		} while (0);

	if (WriteDiskObject)
		{
		// Restore original contents of WriteDiskObject
		*WriteDiskObject = DiskObjectCopy;
		FreeDiskObject(WriteDiskObject);
		}

	return Result;
}

//-----------------------------------------------------------------------------

static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *ioni)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	do	{
		struct NewImage *ni;

		if (NULL == ioni->ioni_NormalImage)
			break;

		ni = NewImageFromSAC(ioni->ioni_NormalImage);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__, ni));
		if (NULL == ni)
			break;

		if (inst->aio_PenList1.nip_PenArray)
			{
			FreePenList(inst->aio_Screen, &inst->aio_PenList1);
			}

		FreeNewImage(&inst->aio_Image1);
		inst->aio_Image1 = ni;

		if (ioni->ioni_SelectedImage)
			{
			ni = NewImageFromSAC(ioni->ioni_SelectedImage);
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__, ni));
			if (NULL == ni)
				break;
			}
		else
			{
			ni = NewImageFromNormalImage(inst, inst->aio_Image1);
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__, ni));
			if (NULL == ni)
				break;
			}

		if (inst->aio_PenList2.nip_PenArray)
			{
			FreePenList(inst->aio_Screen, &inst->aio_PenList2);
			}

		FreeNewImage(&inst->aio_Image2);
		inst->aio_Image2 = ni;
		} while (0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// update aio_FaceChunk
	inst->aio_FaceChunk.fc_Width  = inst->aio_Image1->nim_Width;
	inst->aio_FaceChunk.fc_Height = inst->aio_Image1->nim_Height;
	if (inst->aio_Image2)
		{
		if (inst->aio_Image2->nim_Width > inst->aio_Image1->nim_Width)
			inst->aio_FaceChunk.fc_Width = inst->aio_Image2->nim_Width;
		if (inst->aio_Image2->nim_Height > inst->aio_Image1->nim_Height)
			inst->aio_FaceChunk.fc_Height = inst->aio_Image2->nim_Height;
		}
	inst->aio_FaceChunk.fc_Width--;
	inst->aio_FaceChunk.fc_Height--;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SetAttrs(o,
		GA_Width, inst->aio_Image1->nim_Width,
		GA_Height, inst->aio_Image1->nim_Height,
		IDTA_UnscaledWidth, inst->aio_Image1->nim_Width,
		IDTA_UnscaledHeight, inst->aio_Image1->nim_Height,
		TAG_END);

	GenerateNormalImageMask(cl, o);
	GenerateSelectedImageMask(cl, o);

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	Object *oClone;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	do	{
		struct ExtGadget *gg = (struct ExtGadget *) o;
		struct ARGBHeader *NrmARGBImage = &inst->aio_Image1->nim_ARGBheader;
		struct ARGBHeader *SelARGBImage = &inst->aio_Image2->nim_ARGBheader;
		struct InstanceData *instClone;

		oClone = (Object *) NewObject(cl, NULL,
			DTA_Name, "cloned",	// REQUIRED, otherwise NewObject() will fail
			IDTA_CloneIconObject, o,
			TAG_MORE, iocio->iocio_TagList,
			TAG_END);
		d1(KPrintF("%s/%ld:  o=%08lx  inst=%08lx  oClone=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, oClone));
		if (NULL == oClone)
			break;

		instClone = INST_DATA(cl, oClone);

		*instClone = *inst;

		if (inst->aio_myDrawerData)
			{
			instClone->aio_myDrawerData = MyAllocVecPooled(sizeof(struct DrawerData));
			if (NULL == instClone->aio_myDrawerData)
				break;

			*(instClone->aio_myDrawerData) = *(inst->aio_myDrawerData);
			}

		if (inst->aio_DefaultTool)
			{
			// Clone Default Tool
			instClone->aio_DefaultTool = MyAllocVecPooled(1 + strlen(inst->aio_DefaultTool));
			d1(KPrintF("%s/%s/%ld:  aio_DefaultTool=%08lx\n", __FILE__, __FUNC__, __LINE__, instClone->aio_DefaultTool));
			if (NULL == instClone->aio_DefaultTool)
				break;

			strcpy(instClone->aio_DefaultTool, inst->aio_DefaultTool);
			}
		if (inst->aio_ToolWindow)
			{
			// Clone Tool Window
			instClone->aio_ToolWindow = MyAllocVecPooled(1 + strlen(inst->aio_ToolWindow));
			d1(KPrintF("%s/%s/%ld:  aio_ToolWindow=%08lx\n", __FILE__, __FUNC__, __LINE__, instClone->aio_ToolWindow));
			if (NULL == instClone->aio_ToolWindow)
				break;

			strcpy(instClone->aio_ToolWindow, inst->aio_ToolWindow);
			}

		if (inst->aio_ToolTypes)
			{
			// Clone ToolTypes array
			STRPTR *ToolTypeSrc = inst->aio_ToolTypes;
			STRPTR *ToolTypeDest;

			instClone->aio_ToolTypes = instClone->aio_DiskObject->do_ToolTypes = (STRPTR *) MyAllocVecPooled(inst->aio_ToolTypesLength);
			if (NULL == instClone->aio_DiskObject->do_ToolTypes)
				break;

			d1(KPrintF("%s/%s/%ld:  do_ToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, instClone->aio_DiskObject->do_ToolTypes));

			ToolTypeDest = instClone->aio_ToolTypes;

			while (*ToolTypeSrc)
				{
				d1(KPrintF("%s/%s/%ld:  *ToolTypePtr=<%s>\n", __FILE__, __FUNC__, __LINE__, *ToolTypeSrc));
				*ToolTypeDest = MyAllocVecPooled(1 + strlen(*ToolTypeSrc));
				if (NULL == *ToolTypeDest)
					break;

				strcpy(*ToolTypeDest, *ToolTypeSrc);

				ToolTypeSrc++;
				ToolTypeDest++;
				}
			*ToolTypeDest = NULL;
			}
		if (inst->aio_DiskObject)
			{
			// Clone DiskObject
			instClone->aio_DiskObject = MyAllocVecPooled(sizeof(struct DiskObject));
			d1(KPrintF("%s/%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FILE__, __FUNC__, __LINE__, o, instClone->aio_DiskObject));
			if (NULL == instClone->aio_DiskObject)
				break;

			*(instClone->aio_DiskObject) = *(inst->aio_DiskObject);
			}

		// Clone aio_DoImage1
		instClone->aio_DiskObject->do_Gadget.GadgetRender = DoCloneDoImage(&instClone->aio_DoImage1, &inst->aio_DoImage1);

		// Clone aio_DoImage2
		instClone->aio_DiskObject->do_Gadget.SelectRender = DoCloneDoImage(&instClone->aio_DoImage2, &inst->aio_DoImage2);

		// Clone Normal Image Pen list
		if (!DoClonePenList(inst->aio_Screen, &instClone->aio_PenList1, &inst->aio_PenList1))
			break;
		// Clone Selected Image Pen list
		if (!DoClonePenList(inst->aio_Screen, &instClone->aio_PenList2, &inst->aio_PenList2))
			break;
 
	       if (!CloneNewImage(&instClone->aio_Image1, inst->aio_Image1))
			break;
		if (!CloneNewImage(&instClone->aio_Image2, inst->aio_Image2))
			break;

		SetAttrsA(oClone, iocio->iocio_TagList);

		SetParentAttributes(cl, oClone);

		d1(KPrintF("%s/%s/%ld:  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, gg->Width, gg->Height));

		d1(KPrintF("%s/%s/%ld:  NrmARGBImage=%08lx  argb_ImageData=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
			NrmARGBImage, NrmARGBImage->argb_ImageData, NrmARGBImage->argb_Width, NrmARGBImage->argb_Height));
		d1(KPrintF("%s/%s/%ld:  SelARGBImage=%08lx  argb_ImageData=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
			SelARGBImage, SelARGBImage->argb_ImageData, SelARGBImage->argb_Width, SelARGBImage->argb_Height));

		GetAttr(IDTA_ARGBImageData,     o, (APTR) &NrmARGBImage);
		GetAttr(IDTA_SelARGBImageData,  o, (APTR) &SelARGBImage);

		d1(KPrintF("%s/%s/%ld:  NrmARGBImage=%08lx  argb_ImageData=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
			NrmARGBImage, NrmARGBImage->argb_ImageData, NrmARGBImage->argb_Width, NrmARGBImage->argb_Height));
		d1(KPrintF("%s/%s/%ld:  SelARGBImage=%08lx  argb_ImageData=%08lx  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
			SelARGBImage, SelARGBImage->argb_ImageData, SelARGBImage->argb_Width, SelARGBImage->argb_Height));

		// clone size of original icon
		SetAttrs(oClone,
			GA_Width, gg->Width,
			GA_Height, gg->Height,
			TAG_END);

		if (NrmARGBImage != &inst->aio_Image1->nim_ARGBheader
			&& NrmARGBImage->argb_ImageData
			&& NrmARGBImage->argb_Width > 0
			&& NrmARGBImage->argb_Height )
			{
			if ((NrmARGBImage->argb_Width > gg->Width) || (NrmARGBImage->argb_Height > gg->Height))
				{
				SetAttrs(oClone,
					GA_Width, NrmARGBImage->argb_Width,
					GA_Height, NrmARGBImage->argb_Height,
					TAG_END);
				}
			// IDTA_ARGBImageData has been changed, clone it now!
			SetAttrs(oClone,
				IDTA_CopyARGBImageData, TRUE,
				IDTA_ARGBImageData, (APTR) NrmARGBImage,
				TAG_END);
			}

		d1(KPrintF("%s/%s/%ld:  NrmARGBImage=%08lx  nim_ARGBheader=%08lx\n", __FILE__, __FUNC__, __LINE__, &inst->aio_Image1->nim_ARGBheader));
		if (SelARGBImage != &inst->aio_Image2->nim_ARGBheader
			&& SelARGBImage->argb_ImageData
			&& SelARGBImage->argb_Width > 0
			&& SelARGBImage->argb_Height )
			{
			if ((SelARGBImage->argb_Width > gg->Width) || (SelARGBImage->argb_Height > gg->Height))
				{
				SetAttrs(oClone,
					GA_Width, SelARGBImage->argb_Width,
					GA_Height, SelARGBImage->argb_Height,
					TAG_END);
				}
			// IDTA_SelARGBImageData has been changed, clone it now!
			SetAttrs(oClone,
				IDTA_CopySelARGBImageData, TRUE,
				IDTA_SelARGBImageData, (APTR) SelARGBImage,
				TAG_END);
			}

		Success = TRUE;
		} while (0);

	if (oClone && !Success)
		{
		DoMethod(oClone, OM_DISPOSE);
		oClone = NULL;
		}

	d1(KPrintF("%s/%ld:  END o=%08lx  inst=%08lx  oClone=%08lx\n", __FUNC__, __LINE__, o, inst, oClone));

	return (ULONG) oClone;
}

//-----------------------------------------------------------------------------

static BOOL DoClonePenList(struct Screen *scr, struct NewImagePenList *nipDest,
	const struct NewImagePenList *nipSrc)
{
	BOOL Success = FALSE;

	if (nipSrc->nip_PenArray)
		{
		do	{
			const UBYTE *PenArraySrc = nipSrc->nip_PenArray;
			UBYTE *PenArrayDest;
			ULONG n;

			nipDest->nip_PenArray = MyAllocVecPooled(nipDest->nip_PaletteSize);
			if (NULL == nipDest->nip_PenArray)
				break;

			memcpy(nipDest->nip_PenArray, nipSrc->nip_PenArray, nipDest->nip_PaletteSize);

			PenArrayDest = nipDest->nip_PenArray;
			for (n = 0; n < nipDest->nip_PaletteSize; n++)
				{
				*PenArrayDest++ = ObtainPen(scr->ViewPort.ColorMap,
					*PenArraySrc++,
					0, 0, 0,
					PEN_NO_SETCOLOR);
				}

			Success = TRUE;
		} while (0);
		}
	else
		{
		Success = TRUE;
		}

	return Success;
}

//-----------------------------------------------------------------------------

static struct Image *DoCloneDoImage(struct Image *imgDest, const struct Image *imgSrc)
{
	if (imgSrc->ImageData)
		{
		do	{
			size_t len = RASSIZE(imgDest->Width, imgDest->Height) * imgDest->Depth;

			imgDest->ImageData = MyAllocVecPooled(len);
			if (NULL == imgDest->ImageData)
				break;

			memcpy(imgDest->ImageData, imgSrc->ImageData, len);

			return imgDest;
			} while (0);
		}

	return NULL;
}

//-----------------------------------------------------------------------------

static void DoUpdateWb(CONST_STRPTR SavePath)
{
	BPTR fLock;
	BPTR pLock = (BPTR)NULL;

	do	{
		CONST_STRPTR fName;

		fLock = Lock(SavePath, ACCESS_READ);
		if ((BPTR)NULL == fLock)
			break;

		pLock = ParentDir(fLock);
		if ((BPTR)NULL == pLock)
			break;

		fName = FilePart(SavePath);

		UpdateWorkbench(fName, pLock, UPDATEWB_ObjectAdded);
		} while (0);

	if (pLock)
		UnLock(pLock);
	if (fLock)
		UnLock(fLock);
}

//----------------------------------------------------------------------------------------

static void FreePenList(struct Screen *screen, struct NewImagePenList *PenList)
{
	UBYTE *PenArray = PenList->nip_PenArray;

	while (PenList->nip_PaletteSize--)
		{
		ReleasePen(screen->ViewPort.ColorMap, *PenArray++);
		}

	MyFreeVecPooled(PenList->nip_PenArray);
	PenList->nip_PenArray = NULL;
}

//----------------------------------------------------------------------------------------

static void INLINE WriteARGBArray(const struct ARGB *SrcImgData,
	ULONG SrcX, ULONG SrcY, ULONG SrcBytesPerRow,
	struct RastPort *DestRp, ULONG DestX, ULONG DestY,
	ULONG SizeX, ULONG SizeY)
{
	struct BitMap *DestBM = DestRp->BitMap;
	ULONG DestWidth;
	ULONG DestHeight;

	DestWidth = GetCyberMapAttr(DestBM, CYBRMATTR_WIDTH);
	DestHeight = GetCyberMapAttr(DestBM, CYBRMATTR_HEIGHT);

	d1(KPrintF("%s/%s/%ld:  DestX=%ld  DestY=%ld  SizeX=%ld  SizeY=%ld\n", __FILE__, __FUNC__, __LINE__, DestX, DestY, SizeX, SizeY));

	if (DestX + SizeX > DestWidth)
		SizeX = DestWidth - DestX;
	if (DestY + SizeY > DestHeight)
		SizeY = DestHeight - DestY;

	d1(KPrintF("%s/%s/%ld:  DestX=%ld  DestY=%ld  SizeX=%ld  SizeY=%ld\n", __FILE__, __FUNC__, __LINE__, DestX, DestY, SizeX, SizeY));

	WritePixelArray((APTR) SrcImgData, SrcX, SrcY,
		SrcBytesPerRow,
		DestRp, DestX, DestY,
		SizeX, SizeY,
                RECTFMT_ARGB);
}

//----------------------------------------------------------------------------------------

// True color
static BOOL INLINE DrawTrueColorImage(struct RastPort *rp, struct NewImage *img, 
	UWORD Left, UWORD Top, const struct ColorRegister *Palette, 
	BOOL DimmedImage)
{
	struct ARGB *ARGBImgArray;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		ULONG BmWidth, BmHeight;
		ULONG Width, Height;
		const struct ColorRegister *PalettePtr;
		size_t ARGBImgSize;
		struct ARGB *ARGBImgDestPtr;
		UBYTE *ImgSrcPtr;
		ULONG n;
		ULONG ImgSize;

		BmWidth = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
		BmHeight = GetBitMapAttr(rp->BitMap, BMA_HEIGHT);

		Width = min(img->nim_Width, BmWidth - Left);
		Height = min(img->nim_Height, BmHeight - Top);

		ImgSize = img->nim_Width * img->nim_Height;
		ARGBImgSize = ImgSize * sizeof(struct ARGB);

		ARGBImgArray = MyAllocVecPooled(ARGBImgSize);
		d1(KPrintF("%s/%s/%ld:  ARGBImgArray=%08lx\n", __FILE__, __FUNC__, __LINE__, ARGBImgArray));
		if (NULL == ARGBImgArray)
			break;

		ImgSrcPtr = img->nim_ImageData;
		ARGBImgDestPtr = ARGBImgArray;
		PalettePtr = Palette;

		if (DimmedImage)
			{
			for (n = 0; n < ImgSize; n++, ARGBImgDestPtr++)
				{
				const struct ColorRegister *PaletteEntry;

				PaletteEntry = &PalettePtr[*ImgSrcPtr++];
				ARGBImgDestPtr->Alpha = 0xff;
				ARGBImgDestPtr->Red   = PaletteEntry->red   / 2;
				ARGBImgDestPtr->Green = PaletteEntry->green / 2;
				ARGBImgDestPtr->Blue  = PaletteEntry->blue  / 2;
				}
			}
		else
			{
			for (n = 0; n < ImgSize; n++, ARGBImgDestPtr++)
				{
				const struct ColorRegister *PaletteEntry;

				PaletteEntry = &PalettePtr[*ImgSrcPtr++];
				ARGBImgDestPtr->Alpha = 0xff;
				ARGBImgDestPtr->Red   = PaletteEntry->red;
				ARGBImgDestPtr->Green = PaletteEntry->green;
				ARGBImgDestPtr->Blue  = PaletteEntry->blue;
				}
			}

		WriteARGBArray(ARGBImgArray, 0, 0,
			img->nim_Width * sizeof(struct ARGB),
			rp, Left, Top,
			Width, Height);

		Success = TRUE;
		} while (0);

	if (ARGBImgArray)
		MyFreeVecPooled(ARGBImgArray);

	return Success;
}

//----------------------------------------------------------------------------------------

static BOOL INLINE DrawRemappedImage(struct InstanceData *inst, struct RastPort *rp, 
	struct NewImagePenList *PenList, struct NewImage *img, 
	UWORD Left, UWORD Top, const struct ColorRegister *Palette, ULONG PaletteSize,
	BOOL DimmedImage)
{
	UBYTE *MappedImage = NULL;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld:  START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		const struct ColorRegister *PalettePtr;
		UBYTE *PenListPtr;
		UBYTE *ImgPtr;
		UBYTE *ImgDestPtr;
		ULONG Remainder;
		ULONG n;
		ULONG y;

		PenList->nip_PenArray = MyAllocVecPooled(PaletteSize);
		if (NULL == PenList->nip_PenArray)
			break;

		PalettePtr = Palette;
		PenListPtr = PenList->nip_PenArray;
		for (n = 0; n < PaletteSize; n++)
			{
			if (DimmedImage)
				{
				*PenListPtr++ = ObtainBestPenA(inst->aio_Screen->ViewPort.ColorMap,
					PalettePtr->red << 23,
					PalettePtr->green << 23,
					PalettePtr->blue << 23,
					NULL);
				}
			else
				{
				*PenListPtr++ = ObtainBestPenA(inst->aio_Screen->ViewPort.ColorMap,
					PalettePtr->red << 24,
					PalettePtr->green << 24,
					PalettePtr->blue << 24,
					NULL);
				}
			PalettePtr++;
			}

		MappedImage = MyAllocVecPooled(img->nim_Height * ((img->nim_Width + 15) & 0xfff0));
		if (NULL == MappedImage)
			break;

		Remainder = ((img->nim_Width + 15) & 0xfff0) - img->nim_Width;
		ImgPtr = img->nim_ImageData;
		ImgDestPtr = MappedImage;
		for (y = 0; y < img->nim_Height; y++)
			{
			ULONG x;

			for (x = 0; x < img->nim_Width; x++)
				{
				*ImgDestPtr++ = PenList->nip_PenArray[*ImgPtr++];
				}
			ImgDestPtr += Remainder;
			}

		if (CyberGfxBase && GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
			{
			WritePixelArray(MappedImage, 0, 0,
				(img->nim_Width + 15) & 0xfff0,
				rp, Left, Top,
				img->nim_Width, img->nim_Height,
				RECTFMT_LUT8);
			}
		else
			{
			struct RastPort rpTemp;

			InitRastPort(&rpTemp);
			rpTemp.BitMap = AllocBitMap((img->nim_Width + 15) & 0xfff0,
				img->nim_Height, 8, 0, rp->BitMap);
			if (NULL == rpTemp.BitMap)	
				break;

			WritePixelArray8(rp, Left, Top,
				Left + img->nim_Width - 1,
				Top + img->nim_Height - 1,
				MappedImage, &rpTemp);

			FreeBitMap(rpTemp.BitMap);
			}

		Success = TRUE;
		} while (0);

	if (MappedImage)
		MyFreeVecPooled(MappedImage);

	return Success;
}

//-----------------------------------------------------------------------------

static void DrawColorImage(struct InstanceData *inst, struct RastPort *rp, 
	struct NewImagePenList *PenList, struct NewImage *img, 
	UWORD Left, UWORD Top, const struct ColorRegister *Palette, ULONG PaletteSize,
	BOOL DimmedImage)
{
	BOOL Success;

	d1(kprintf("%s/%s/%ld:  PaletteSize=%lu  Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, img->nim_PaletteSize, Palette));

	if (PenList->nip_PenArray)
		{
		MyFreeVecPooled(PenList->nip_PenArray);
		PenList->nip_PenArray = NULL;
		}

	PenList->nip_PaletteSize = PaletteSize;

	if (CyberGfxBase && (GetBitMapAttr(rp->BitMap, BMA_DEPTH) > 8))
		{
		Success = DrawTrueColorImage(rp, img, Left, Top, Palette, DimmedImage);
		}
	else
		{
		d1(KPrintF("%s/%s/%ld:  CyberGfxBase=%08lx  BMA_DEPTH=%ld\n", __FILE__, __FUNC__, __LINE__, CyberGfxBase, GetBitMapAttr(rp->BitMap, BMA_DEPTH)));
		Success = DrawRemappedImage(inst, rp, PenList, img, 
			Left, Top, Palette, PaletteSize, DimmedImage);
		}

	if (!Success && PenList->nip_PenArray)
		{
		MyFreeVecPooled(PenList->nip_PenArray);
		PenList->nip_PenArray = NULL;
		}
}

//----------------------------------------------------------------------------------------

static void ReadIcon(struct InstanceData *inst, CONST_STRPTR Filename, BPTR IconFh)
{
	static const LONG StopChunkList[] =
		{
		ID_ICON, ID_FACE,
		ID_ICON, ID_IMAG,
		ID_ICON, ID_ARGB,
		};
	STRPTR IconInfoName;		// full filename of icon (including ".info")
	BPTR fd = BNULL;
	struct IFFHandle *iff = NULL;
	BOOL iffOpened = FALSE;

	do	{
		BOOL FaceChunkOk = FALSE;
		BOOL Image1Ok = FALSE;
		LONG Result;

		IconInfoName = MyAllocVecPooled(1 + strlen(Filename) + strlen(".info"));
		if (NULL == IconInfoName)
			break;

		strcpy(IconInfoName, Filename);
		strcat(IconInfoName, ".info");

		if (IconFh)
			fd = IconFh;
		else
			fd = Open(IconInfoName, MODE_OLDFILE);
		d1(kprintf("%s/%s/%ld: fd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, fd, IconInfoName));
		if (BNULL == fd)
			break;

		if (!ReadStandardIcon(inst, fd))
			break;

		// ---- End of standard icon data ------------------

		iff = AllocIFF();
		d1(kprintf(__FILE__ "/" "%s/%s/%ld: AlloCIFF Result=%ld\n", __FILE__, __FUNC__, __LINE__, IoErr()));
		if (NULL == iff)
			break;

		InitIFF(iff, IFFF_RSEEK | IFFF_READ, &StreamHook);
		iff->iff_Stream = (IPTR)fd;

		Result = OpenIFF(iff, IFFF_READ);
		d1(kprintf(__FILE__ "/" "%s/%s/%ld: OpenIFF Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		Result = StopChunks(iff, StopChunkList, Sizeof(StopChunkList) / 2);
		d1(kprintf(__FILE__ "/" "%s/%s/%ld: StopChunks Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		while (1)
			{
			struct ContextNode *cn;

			Result = ParseIFF(iff, IFFPARSE_SCAN);
			if (RETURN_OK != Result)
				{
				d1(kprintf(__FILE__ "/" "%s/%s/%ld: ParseIFF Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
				break;
				}

			cn = CurrentChunk(iff);

			if (ID_FACE == cn->cn_ID)
				{
				LONG Actual;

				d1(kprintf("%s/%s/%ld: FACE Chunk len=%lu\n", __FILE__, __FUNC__, __LINE__, cn->cn_Size));
				if (cn->cn_Size != sizeof(struct FaceChunk))
					break;

				Actual = ReadChunkBytes(iff, &inst->aio_FaceChunk, sizeof(inst->aio_FaceChunk));
				if (Actual != sizeof(inst->aio_FaceChunk))
					break;

				inst->aio_FaceChunk.fc_MaxPaletteBytes = SCA_BE2WORD(inst->aio_FaceChunk.fc_MaxPaletteBytes);

				d1(kprintf("%s/%s/%ld: FACE id read OK.\n", __FILE__, __FUNC__, __LINE__));

				d1(kprintf("%s/%s/%ld: FACE Width=%ld  Height=%ld   Aspect=%02lx  Flags=%02lx  MaxPaletteBytes=%lu\n", \
					__FILE__, __FUNC__, __LINE__, \
					inst->aio_FaceChunk.fc_Width+1, inst->aio_FaceChunk.fc_Height+1,\
					inst->aio_FaceChunk.fc_AspectRatio,\
					inst->aio_FaceChunk.fc_Flags,\
					inst->aio_FaceChunk.fc_MaxPaletteBytes));

				inst->aio_Borderless = inst->aio_FaceChunk.fc_Flags & FCF_Borderless;

				FaceChunkOk = TRUE;
				}
			else if (ID_IMAG == cn->cn_ID && cn->cn_Size >= sizeof(struct ImageChunk) && FaceChunkOk)
				{
				d1(KPrintF("%s/%s/%ld: IMAG Chunk len=%lu\n", __FILE__, __FUNC__, __LINE__, cn->cn_Size));

				if (Image1Ok)
					{
					inst->aio_Image2 = ReadNewImage(iff, &inst->aio_FaceChunk);
					if (NULL == inst->aio_Image2)
						break;

					d1(kprintf("%s/%s/%ld: Image 2 read Ok.\n", __FILE__, __FUNC__, __LINE__));
					}
				else
					{
					inst->aio_Image1 = ReadNewImage(iff, &inst->aio_FaceChunk);
					if (NULL == inst->aio_Image1)
						break;

					d1(kprintf("%s/%s/%ld: Image 1 read Ok.\n", __FILE__, __FUNC__, __LINE__));
					Image1Ok = TRUE;
					}
				}
			else if (ID_ARGB == cn->cn_ID && cn->cn_Size >= sizeof(struct ImageChunk) && FaceChunkOk)
				{
				d1(KPrintF("%s/%s/%ld: ARGB Chunk len=%lu\n", __FILE__, __FUNC__, __LINE__, cn->cn_Size));

				if (Image1Ok)
					{
					inst->aio_Image2 = ReadARGBImage(iff, &inst->aio_FaceChunk);
					if (NULL == inst->aio_Image2)
						break;

					d1(KPrintF("%s/%s/%ld: Image 2 read Ok.\n", __FILE__, __FUNC__, __LINE__));
					}
				else
					{
					inst->aio_Image1 = ReadARGBImage(iff, &inst->aio_FaceChunk);
					if (NULL == inst->aio_Image1)
						break;

					d1(KPrintF("%s/%s/%ld: Image 1 read Ok.\n", __FILE__, __FUNC__, __LINE__));
					Image1Ok = TRUE;
					}
				}
			}
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		FreeIFF(iff);
		}
	if (fd && (BNULL == IconFh))
		Close(fd);

	if (IconInfoName)
		MyFreeVecPooled(IconInfoName);
}

//----------------------------------------------------------------------------------------

static BOOL WriteIcon(Class *cl, Object *o,
	const struct DiskObject *WriteDiskObject, CONST_STRPTR Filename)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	STRPTR IconInfoName;	       // full filename of icon (including ".info")
	BPTR fd = (BPTR)NULL;
	struct IFFHandle *iff = NULL;
	BOOL iffOpened = FALSE;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: START <%s>\n", __FILE__, __FUNC__, __LINE__, Filename));

	do	{
		UWORD PaletteSize;
		struct FaceChunk fc;
		LONG Result;

		IconInfoName = MyAllocVecPooled(1 + strlen(Filename) + strlen(".info"));
		if (NULL == IconInfoName)
			{
			SetIoErr(ERROR_NO_FREE_STORE);
			break;
			}

		strcpy(IconInfoName, Filename);
		strcat(IconInfoName, ".info");

		fd = Open(IconInfoName, MODE_NEWFILE);
		d1(kprintf("%s/%s/%ld: fd=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, fd, IconInfoName));
		if ((BPTR)NULL == fd)
			break;

		if (!WriteStandardIcon(inst, WriteDiskObject, fd))
			break;

		// ---- End of standard icon data ------------------

		iff = AllocIFF();
		d1(kprintf(__FILE__ "/" "%s/%s/%ld: AlloCIFF Result=%ld\n", __FILE__, __FUNC__, __LINE__, IoErr()));
		if (NULL == iff)
			break;

		InitIFF(iff, IFFF_RSEEK | IFFF_WRITE, &StreamHook);
		iff->iff_Stream = (IPTR)fd;

		Result = OpenIFF(iff, IFFF_WRITE);
		d1(kprintf(__FILE__ "/" "%s/%s/%ld: OpenIFF Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		iffOpened = TRUE;

		Result = PushChunk(iff, ID_ICON, ID_FORM, IFFSIZE_UNKNOWN);
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: PushChunk(ID_FORM) Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		Result = PushChunk(iff, ID_ICON, ID_FACE, sizeof(struct FaceChunk));
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: PushChunk(ID_FACE) Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		fc = inst->aio_FaceChunk;

		if (NULL == inst->aio_Image1->nim_Palette)
			{
			// make sure FaceChunk has correct size for ARGB image
			struct ARGBHeader *argbh = NULL;

			GetAttr(IDTA_ARGBImageData, o, (APTR) &argbh);
			if (argbh && argbh->argb_ImageData)
				{
				fc.fc_Width  = argbh->argb_Width - 1;
				fc.fc_Height = argbh->argb_Height - 1;
				}
			}

		PaletteSize = inst->aio_Image1->nim_PaletteSize;

		if (inst->aio_Image2 && inst->aio_Image2->nim_PaletteSize)
			{
			if (inst->aio_Image2->nim_PaletteSize > PaletteSize)
				{
				PaletteSize = inst->aio_Image2->nim_PaletteSize;
				}
			}

		PaletteSize = PaletteSize * 3 - 1;
		fc.fc_MaxPaletteBytes = PaletteSize * sizeof(struct ColorRegister) - 1;

		fc.fc_MaxPaletteBytes = SCA_WORD2BE(fc.fc_MaxPaletteBytes);

		if (sizeof(struct FaceChunk) != WriteChunkBytes(iff,
			&fc, sizeof(struct FaceChunk)))
			break;

		Result = PopChunk(iff);	// ID_FACE
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: PopChunk Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: nim_Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->aio_Image1->nim_Palette));
		if (inst->aio_Image1->nim_Palette)
			Result = WriteNewImage(iff, inst->aio_Image1);
		else
			Result = WriteARGBImage(cl, o, iff, inst->aio_Image1, IDTA_ARGBImageData);
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: WriteNewImage(1) Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		if (inst->aio_Image2)
			{
			if (inst->aio_Image1->nim_Palette)
				Result = WriteNewImage(iff, inst->aio_Image2);
			else
				Result = WriteARGBImage(cl, o, iff, inst->aio_Image2, IDTA_SelARGBImageData);
			d1(KPrintF(__FILE__ "/" "%s/%s/%ld: WriteNewImage(2)Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			if (RETURN_OK != Result)
				break;
			}

		Result = PopChunk(iff);	// ID_ICON
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: PopChunk Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
		if (RETURN_OK != Result)
			break;

		Success = TRUE;
		} while (0);

	if (iff)
		{
		if (iffOpened)
			CloseIFF(iff);

		FreeIFF(iff);
		}
	if (fd)
		Close(fd);
	if (IconInfoName)
		MyFreeVecPooled(IconInfoName);

	d1(KPrintF(__FILE__ "/" "%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------------------

static BOOL ReadStandardIcon(struct InstanceData *inst, BPTR fd)
{
	BOOL Success = FALSE;

	do	{
		UWORD FileFormatRevision;
		ULONG len;

		inst->aio_DiskObject = MyAllocVecPooled(sizeof(struct DiskObject));
		if (NULL == inst->aio_DiskObject)
			break;

#if defined(__AROS__)
		if (!ReadConvertStandardIcon(fd, inst->aio_DiskObject))
			break;
#else
		if (sizeof(struct DiskObject) != FRead(fd, inst->aio_DiskObject, 1, sizeof(struct DiskObject)))
			break;
#endif

		d1(kprintf("%s/%s/%ld: DiskObject read Ok, size=%ld.\n", __FILE__, __FUNC__, __LINE__, sizeof(struct DiskObject)));

		FileFormatRevision = ((ULONG) inst->aio_DiskObject->do_Gadget.UserData) & WB_DISKREVISIONMASK;

		d1(kprintf("%s/%s/%ld: do_Magic=%04lx  Version=%04lx\n", __FILE__, __FUNC__, __LINE__, inst->aio_DiskObject->do_Magic, inst->aio_DiskObject->do_Version));

		if (inst->aio_DiskObject->do_Magic == WB_DISKMAGIC)
			{
			d1(kprintf("%s/%s/%ld: do_Magic = %04lx  Version=%ld  Revision=%ld\n", __FILE__, __FUNC__, __LINE__,\
				inst->aio_DiskObject->do_Magic, inst->aio_DiskObject->do_Version, FileFormatRevision));
			}
		else
			break;

		d1(kprintf("%s/%s/%ld: File Position = %ld\n", __FILE__, __FUNC__, __LINE__, Seek(fd, 0, OFFSET_CURRENT)));
		d1(kprintf("%s/%s/%ld: DrawerData = %08lx\n", __FILE__, __FUNC__, __LINE__, inst->aio_DiskObject->do_DrawerData));

		if (inst->aio_DiskObject->do_DrawerData)
			{
			inst->aio_DiskObject->do_DrawerData = NULL;

			d1(kprintf("%s/%s/%ld: old-drawerdata expected, size=%ld.\n", __FILE__, __FUNC__, __LINE__, OLDDRAWERDATAFILESIZE));

			inst->aio_DiskObject->do_DrawerData = inst->aio_myDrawerData = MyAllocVecPooled(sizeof(struct DrawerData));
			if (NULL == inst->aio_myDrawerData)
				break;

#if defined(__AROS__)
			if (!ReadConvertDrawerData(fd, inst->aio_myDrawerData))
				break;
#else
			if (OLDDRAWERDATAFILESIZE != FRead(fd, inst->aio_myDrawerData, 1, OLDDRAWERDATAFILESIZE))
				break;
#endif

			d1(kprintf("%s/%s/%ld: OldDrawerData read Ok.\n", __FILE__, __FUNC__, __LINE__));
			}

		d1(kprintf("%s/%s/%ld: Image 1 expected, size=%ld\n", __FILE__, __FUNC__, __LINE__, sizeof(inst->aio_DoImage1)));

#if defined(__AROS__)
		if (!ReadConvertImage(fd, &inst->aio_DoImage1))
			break;
#else
		if (sizeof(inst->aio_DoImage1) != FRead(fd, &inst->aio_DoImage1, 1, sizeof(inst->aio_DoImage1)))
			break;
#endif

		d1(kprintf("%s/%s/%ld: Image 1 read Ok.\n", __FILE__, __FUNC__, __LINE__));

		d1(kprintf("%s/%s/%ld: Image 1: Left=%d  Top=%d  Width=%d  Height=%d  Depth=%d\n", \
			__FILE__, __FUNC__, __LINE__, \
			inst->aio_DoImage1.LeftEdge, inst->aio_DoImage1.TopEdge, \
			inst->aio_DoImage1.Width, inst->aio_DoImage1.Height, inst->aio_DoImage1.Depth));

		len = RASSIZE(inst->aio_DoImage1.Width, inst->aio_DoImage1.Height) * inst->aio_DoImage1.Depth;

		d1(kprintf("%s/%s/%ld: ImageData 1 expected, size=%ld\n", __FILE__, __FUNC__, __LINE__, len));

		inst->aio_DiskObject->do_Gadget.GadgetRender = &inst->aio_DoImage1;

		inst->aio_DoImage1.ImageData = MyAllocVecPooled(len);
		if (NULL == inst->aio_DoImage1.ImageData)
			break;

		if (len != FRead(fd, inst->aio_DoImage1.ImageData, 1, len))
			break;

		d1(kprintf("%s/%s/%ld: ImageData 1 skipped Ok.\n", __FILE__, __FUNC__, __LINE__));

		inst->aio_DiskObject->do_Gadget.GadgetRender = &inst->aio_DoImage1;

		d1(kprintf("%s/%s/%ld: Gadget Flags = %08lx\n", __FILE__, __FUNC__, __LINE__, inst->aio_DiskObject->do_Gadget.Flags));

		if (GFLG_GADGHIMAGE & inst->aio_DiskObject->do_Gadget.Flags)
			{
			d1(kprintf("%s/%s/%ld: Image 2 expected, size=%ld\n", __FILE__, __FUNC__, __LINE__, sizeof(inst->aio_DoImage2)));


#if defined(__AROS__)
			if (!ReadConvertImage(fd, &inst->aio_DoImage2))
				break;
#else
			if (sizeof(inst->aio_DoImage2) != FRead(fd, &inst->aio_DoImage2, 1, sizeof(inst->aio_DoImage2)))
				break;
#endif

			d1(kprintf("%s/%s/%ld: Image 2 read Ok.\n", __FILE__, __FUNC__, __LINE__));

			inst->aio_DiskObject->do_Gadget.SelectRender = &inst->aio_DoImage2;

			d1(kprintf("%s/%s/%ld: Image 2: Left=%d  Top=%d  Width=%d  Height=%d  Depth=%d\n", \
				__FILE__, __FUNC__, __LINE__, \
				inst->aio_DoImage2.LeftEdge, inst->aio_DoImage2.TopEdge, \
				inst->aio_DoImage2.Width, inst->aio_DoImage2.Height, inst->aio_DoImage2.Depth));

			len = RASSIZE(inst->aio_DoImage2.Width, inst->aio_DoImage2.Height) * inst->aio_DoImage2.Depth;

			d1(kprintf("%s/%s/%ld: ImageData 2 expected, size=%ld\n", __FILE__, __FUNC__, __LINE__, len));

			inst->aio_DoImage2.ImageData = MyAllocVecPooled(len);
			if (NULL == inst->aio_DoImage2.ImageData)
				break;

			if (len != FRead(fd, inst->aio_DoImage2.ImageData, 1, len))
				break;

			d1(kprintf("%s/%s/%ld: ImageData 2 read Ok.\n", __FILE__, __FUNC__, __LINE__));
			}
		else
			{
			inst->aio_DiskObject->do_Gadget.SelectRender = NULL;
			d1(kprintf("%s/%s/%ld: No alternate image.\n", __FILE__, __FUNC__, __LINE__));
			}

		if (inst->aio_DiskObject->do_DefaultTool)
			{
			inst->aio_DiskObject->do_DefaultTool = NULL;

			d1(kprintf("%s/%s/%ld: DefaultTool Length expected.\n", __FILE__, __FUNC__, __LINE__));
			if (sizeof(len) != FRead(fd, &len, 1, sizeof(len)))
				break;

			len = SCA_BE2LONG(len);

			d1(kprintf("%s/%s/%ld: DefaultTool length read Ok, length=%lu.\n", __FILE__, __FUNC__, __LINE__, len));

			if (len)
				{
				inst->aio_DefaultTool = inst->aio_DiskObject->do_DefaultTool = MyAllocVecPooled(len);
				if (NULL == inst->aio_DiskObject->do_DefaultTool)
					break;

				if (len != FRead(fd, inst->aio_DiskObject->do_DefaultTool, 1, len))
					break;

				d1(kprintf("%s/%s/%ld: DefaultTool read Ok, <%s>.\n", \
					__FILE__, __FUNC__, __LINE__, inst->aio_DiskObject->do_DefaultTool));
				}
			}
		else
			{
			d1(kprintf("%s/%s/%ld: No Default Tool.\n", __FILE__, __FUNC__, __LINE__));
			}

		if (inst->aio_DiskObject->do_ToolTypes)
			{
			inst->aio_DiskObject->do_ToolTypes = NULL;

			d1(kprintf("%s/%s/%ld: Expecting size of ToolTypes Array.\n", __FILE__, __FUNC__, __LINE__));
			if (sizeof(len) != FRead(fd, &len, 1, sizeof(len)))
				break;

			len = SCA_BE2LONG(len);

			d1(KPrintF("%s/%s/%ld: Size of ToolTypes Array read Ok, %lu.\n", __FILE__, __FUNC__, __LINE__, len));
			inst->aio_ToolTypesLength = len;
			if (len)
				{
				short n;

				inst->aio_ToolTypes = inst->aio_DiskObject->do_ToolTypes = (STRPTR *) MyAllocVecPooled(len);
				if (NULL == inst->aio_DiskObject->do_ToolTypes)
					break;

				memset(inst->aio_DiskObject->do_ToolTypes, 0, len);

				for (n=0; n<(len-1)/sizeof(STRPTR); n++)
					{
					ULONG sLen;

					if (sizeof(sLen) != FRead(fd, &sLen, 1, sizeof(sLen)))
						break;

					sLen = SCA_BE2LONG(sLen);

					inst->aio_DiskObject->do_ToolTypes[n] = MyAllocVecPooled(sLen);
					if (inst->aio_DiskObject->do_ToolTypes[n])
						FRead(fd, inst->aio_DiskObject->do_ToolTypes[n], 1, sLen);
					else
						Seek(fd, sLen, OFFSET_CURRENT);

					d1(kprintf("%s/%s/%ld: Read ToolType #%ld, len=%ld val=<%s>\n", \
						__FILE__, __FUNC__, __LINE__, n, sLen, inst->aio_DiskObject->do_ToolTypes[n]));
					}
				}
			}
		else
			d1(kprintf("%s/%s/%ld: No ToolTypes.\n", __FILE__, __FUNC__, __LINE__));

		if (inst->aio_DiskObject->do_ToolWindow)
			{
			inst->aio_DiskObject->do_ToolWindow = NULL;

			d1(kprintf("%s/%s/%ld: ToolWindow expected.\n", __FILE__, __FUNC__, __LINE__));

			if (sizeof(len) != FRead(fd, &len, 1, sizeof(len)))
				break;

			len = SCA_BE2LONG(len);

			inst->aio_ToolWindow = inst->aio_DiskObject->do_ToolWindow = MyAllocVecPooled(len);
			if (NULL == inst->aio_DiskObject->do_ToolWindow)
				break;

			if (len != FRead(fd, inst->aio_DiskObject->do_ToolWindow, 1, len))
				break;

			d1(kprintf("%s/%s/%ld: ToolWindow read ok, len=%ld\n", __FILE__, __FUNC__, __LINE__, len));
			}
		else
			d1(kprintf("%s/%s/%ld: No ToolWindow.\n", __FILE__, __FUNC__, __LINE__));

		if (inst->aio_DiskObject->do_DrawerData && FileFormatRevision > 0 && FileFormatRevision <= WB_DISKREVISION)
			{
			UBYTE *RemainingDrawerData = ((UBYTE *) inst->aio_myDrawerData) + OLDDRAWERDATAFILESIZE;

			ULONG ddSize = 6; // size of structs can be different on AROS
			// sizeof(struct DrawerData) - OLDDRAWERDATAFILESIZE

			d1(kprintf("%s/%s/%ld: remaining-drawerdata expected, size=%ld.\n", \
				__FILE__, __FUNC__, __LINE__, 6 /* sizeof(struct DrawerData) - OLDDRAWERDATAFILESIZE */));

			if (ddSize != FRead(fd, RemainingDrawerData, 1, ddSize))
				break;

			d1(kprintf("%s/%s/%ld: remaining-DrawerData read Ok.\n", __FILE__, __FUNC__, __LINE__));
			}

		Success = TRUE;
		} while (0);

	return Success;
}

//----------------------------------------------------------------------------------------

static BOOL WriteStandardIcon(struct InstanceData *inst,
	const struct DiskObject *WriteDiskObject, BPTR fd)
{
	BOOL Success = FALSE;

	do	{
		struct DiskObject wdo;
		ULONG len;
		ULONG val_be; // for storing values in BE order

		wdo = *WriteDiskObject;

		wdo.do_Gadget.UserData = (APTR) WB_DISKREVISION;

#if defined(__AROS__)
		if (!WriteConvertStandardIcon(fd, &wdo))
			break;
#else
		if (sizeof(struct DiskObject) != FWrite(fd, &wdo, 1, sizeof(struct DiskObject)))
			break;
#endif

		d1(KPrintF("%s/%s/%ld: DiskObject written Ok, size=%ld.\n", __FILE__, __FUNC__, __LINE__, sizeof(struct DiskObject)));

		if (wdo.do_DrawerData)
			{
			d1(KPrintF("%s/%s/%ld: write old-drawerdata, size=%ld.\n", __FILE__, __FUNC__, __LINE__, OLDDRAWERDATAFILESIZE));

#if defined(__AROS__)
			if (!WriteConvertDrawerData(fd, inst->aio_myDrawerData))
				break;
#else
			if (OLDDRAWERDATAFILESIZE != FWrite(fd, inst->aio_myDrawerData, 1, OLDDRAWERDATAFILESIZE))
				break;
#endif

			d1(KPrintF("%s/%s/%ld: OldDrawerData written Ok.\n", __FILE__, __FUNC__, __LINE__));
			}

		d1(KPrintF("%s/%s/%ld: write Image 1, size=%ld\n", __FILE__, __FUNC__, __LINE__, sizeof(inst->aio_DoImage1)));

#if defined(__AROS__)
		if (!WriteConvertImage(fd, &inst->aio_DoImage1))
			break;
#else
		if (sizeof(inst->aio_DoImage1) != FWrite(fd, &inst->aio_DoImage1, 1, sizeof(inst->aio_DoImage1)))
			break;
#endif

		d1(KPrintF("%s/%s/%ld: Image 1 written Ok.\n", __FILE__, __FUNC__, __LINE__));

		len = RASSIZE(inst->aio_DoImage1.Width, inst->aio_DoImage1.Height) * inst->aio_DoImage1.Depth;

		d1(KPrintF("%s/%s/%ld: write ImageData 1, size=%ld\n", __FILE__, __FUNC__, __LINE__, len));

		if (len != FWrite(fd, inst->aio_DoImage1.ImageData, 1, len))
			break;

		d1(KPrintF("%s/%s/%ld: ImageData 1 written Ok.\n", __FILE__, __FUNC__, __LINE__));

		if (GFLG_GADGHIMAGE & wdo.do_Gadget.Flags)
			{
			d1(KPrintF("%s/%s/%ld: write Image 2, size=%ld\n", __FILE__, __FUNC__, __LINE__, sizeof(inst->aio_DoImage2)));

#if defined(__AROS__)
			if (!WriteConvertImage(fd, &inst->aio_DoImage2))
				break;
#else
			if (sizeof(inst->aio_DoImage2) != FWrite(fd, &inst->aio_DoImage2, 1, sizeof(inst->aio_DoImage2)))
				break;
#endif

			d1(KPrintF("%s/%s/%ld: Image 2 written Ok.\n", __FILE__, __FUNC__, __LINE__));

			len = RASSIZE(inst->aio_DoImage2.Width, inst->aio_DoImage2.Height) * inst->aio_DoImage2.Depth;

			d1(KPrintF("%s/%s/%ld: write ImageData 2, size=%ld\n", __FILE__, __FUNC__, __LINE__, len));

			if (len != FWrite(fd, inst->aio_DoImage2.ImageData, 1, len))
				break;

			d1(KPrintF("%s/%s/%ld: ImageData 2 written Ok.\n", __FILE__, __FUNC__, __LINE__));
			}

		if (wdo.do_DefaultTool)
			{
			len = 1 + strlen(wdo.do_DefaultTool);

			d1(KPrintF("%s/%s/%ld: write DefaultTool Length.\n", __FILE__, __FUNC__, __LINE__));
			val_be = SCA_LONG2BE(len);
			if (sizeof(val_be) != FWrite(fd, &val_be, 1, sizeof(val_be)))
				break;

			d1(KPrintF("%s/%s/%ld: DefaultTool length written Ok, length=%lu.\n", __FILE__, __FUNC__, __LINE__, val_be));

			if (len)
				{
				if (len != FWrite(fd, wdo.do_DefaultTool, 1, len))
					break;

				d1(KPrintF("%s/%s/%ld: DefaultTool written Ok, <%s>.\n", \
					__FILE__, __FUNC__, __LINE__, wdo.do_DefaultTool));
				}
			}

		if (wdo.do_ToolTypes)
			{
			short n;

			for (n = 0; wdo.do_ToolTypes[n]; n++)
				;

			len = (1 + n) * sizeof(STRPTR);

			d1(KPrintF("%s/%s/%ld: write size of ToolTypes Array = %ld.\n", __FILE__, __FUNC__, __LINE__, len));
			val_be = SCA_LONG2BE(len);
			if (sizeof(val_be) != FWrite(fd, &val_be, 1, sizeof(val_be)))
				break;

			d1(KPrintF("%s/%s/%ld: Size of ToolTypes Array written Ok, %lu.\n", __FILE__, __FUNC__, __LINE__, len));


			for (n=0; n<(len-1) / sizeof(STRPTR); n++)
				{
				ULONG sLen;

				sLen = 1 + strlen(wdo.do_ToolTypes[n]);

				val_be = SCA_LONG2BE(sLen);
				if (sizeof(val_be) != FWrite(fd, &val_be, 1, sizeof(val_be)))
					break;

				if (sLen != FWrite(fd, wdo.do_ToolTypes[n], 1, sLen))
					break;

				d1(KPrintF("%s/%s/%ld: ToolType #%ld  written, len=%ld val=<%s>\n", \
					__FILE__, __FUNC__, __LINE__, n, sLen, wdo.do_ToolTypes[n]));
				}
			}

		if (wdo.do_ToolWindow)
			{
			len = 1 + strlen(wdo.do_ToolWindow);

			d1(KPrintF("%s/%s/%ld: write ToolWindow.\n", __FILE__, __FUNC__, __LINE__));

			val_be = SCA_LONG2BE(len);
			if (sizeof(val_be) != FWrite(fd, &val_be, 1, sizeof(val_be)))
				break;

			if (len != FWrite(fd, wdo.do_ToolWindow, 1, len))
				break;

			d1(KPrintF("%s/%s/%ld: ToolWindow written ok, len=%ld\n", __FILE__, __FUNC__, __LINE__, len));
			}

		if (wdo.do_DrawerData)
			{
			UBYTE *RemainingDrawerData = ((UBYTE *) wdo.do_DrawerData) + OLDDRAWERDATAFILESIZE;
			ULONG ddSize = 6; // size of structs can be different on AROS
			// sizeof(struct DrawerData) - OLDDRAWERDATAFILESIZE;

			d1(KPrintF("%s/%s/%ld: write remaining-drawerdata, size=%ld.\n", \
				__FILE__, __FUNC__, __LINE__, 6 /* sizeof(struct DrawerData) - OLDDRAWERDATAFILESIZE */));

			if (ddSize != FWrite(fd, RemainingDrawerData, 1, ddSize))
				break;

			d1(KPrintF("%s/%s/%ld: remaining-DrawerData written Ok.\n", __FILE__, __FUNC__, __LINE__));
			}

		Success = TRUE;
		} while (0);

	return Success;
}

//----------------------------------------------------------------------------------------

static struct NewImage *ReadNewImage(struct IFFHandle *iff, const struct FaceChunk *fc)
{
	struct NewImage *ni = NULL;
	BOOL Success = FALSE;
	UBYTE *UnCompressBuffer = NULL;

	do	{
		ULONG PaletteSize;

		ni = MyAllocVecPooled(sizeof(struct NewImage));
		if (NULL == ni)
			break;

		memset(ni, 0, sizeof(struct NewImage));

		if (sizeof(ni->nim_ImageChunk) != ReadChunkBytes(iff, &ni->nim_ImageChunk, sizeof(ni->nim_ImageChunk)))
			break;

		ni->nim_ImageChunk.ic_NumImageBytes = SCA_BE2WORD(ni->nim_ImageChunk.ic_NumImageBytes);
		ni->nim_ImageChunk.ic_NumPaletteBytes = SCA_BE2WORD(ni->nim_ImageChunk.ic_NumPaletteBytes);

		d1(kprintf("%s/%s/%ld: IMAG Chunk Read Ok.\n", __FILE__, __FUNC__, __LINE__));

		d1(kprintf("%s/%s/%ld: Transparent=%ld  ic_PaletteSize=%ld  Flags=%02lx  ImgComp=%02lx  PalComp=%02lx  BitsPerPixel=%ld  NumImageBytes=%ld  NumPaletteBytes=%ld\n",\
			__FILE__, __FUNC__, __LINE__, \
			(ULONG) ni->nim_ImageChunk.ic_TransparentColor,\
			(ULONG) ni->nim_ImageChunk.ic_PaletteSize, \
			(ULONG) ni->nim_ImageChunk.ic_Flags, \
			(ULONG) ni->nim_ImageChunk.ic_ImageCompressionType,\
			(ULONG) ni->nim_ImageChunk.ic_PaletteCompressionType, \
			(ULONG) ni->nim_ImageChunk.ic_BitsPerPixel,\
			(ULONG) ni->nim_ImageChunk.ic_NumImageBytes, 
			(ULONG) ni->nim_ImageChunk.ic_NumPaletteBytes));

		ni->nim_Width = 1 + fc->fc_Width;
		ni->nim_Height = 1 + fc->fc_Height;
		ni->nim_PaletteSize = ni->nim_ImageChunk.ic_PaletteSize ? (1 + ni->nim_ImageChunk.ic_PaletteSize) : 0;

		PaletteSize = ni->nim_PaletteSize * sizeof(struct ColorRegister);

		ni->nim_Palette = MyAllocVecPooled(PaletteSize);
		d1(kprintf("%s/%s/%ld: nim_Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, ni->nim_Palette));
		if (NULL == ni->nim_Palette)
			break;

		UnCompressBuffer = MyAllocVecPooled(1 + max(ni->nim_ImageChunk.ic_NumImageBytes, ni->nim_ImageChunk.ic_NumPaletteBytes));
		d1(kprintf("%s/%s/%ld: UnCompressBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, UnCompressBuffer));
		if (NULL == UnCompressBuffer)
			break;

		ni->nim_ImageData = MyAllocVecPooled(ni->nim_Width * ni->nim_Height);
		d1(kprintf("%s/%s/%ld: nim_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, ni->nim_ImageData));
		if (NULL == ni->nim_ImageData)
			break;

		if (1 + ni->nim_ImageChunk.ic_NumImageBytes != ReadChunkBytes(iff, UnCompressBuffer, 1 + ni->nim_ImageChunk.ic_NumImageBytes))
			break;

		d1(kprintf("%s/%s/%ld: Image Data Read Ok.\n", __FILE__, __FUNC__, __LINE__));

		d1(kprintf("%s/%s/%ld: BufferData = %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n",\
			__FILE__, __FUNC__, __LINE__, \
			UnCompressBuffer[0], UnCompressBuffer[1], UnCompressBuffer[2], UnCompressBuffer[3], \
			UnCompressBuffer[4], UnCompressBuffer[5], UnCompressBuffer[6], UnCompressBuffer[7]));

		if (cmpNone == ni->nim_ImageChunk.ic_ImageCompressionType)
			{
			d1(KPrintF("%s/%s/%ld: ic_NumImageBytes=%lu  w*x=%lu\n", \
				__FILE__, __FUNC__, __LINE__, ni->nim_ImageChunk.ic_NumImageBytes, ni->nim_Width * ni->nim_Height));

			CopyMem(UnCompressBuffer, ni->nim_ImageData,
				min(ni->nim_ImageChunk.ic_NumImageBytes, ni->nim_Width * ni->nim_Height));
			}
		else if (cmpByteRun1 == ni->nim_ImageChunk.ic_ImageCompressionType)
			{
			d1(KPrintF("%s/%s/%ld: decompressing Image Data.\n", __FILE__, __FUNC__, __LINE__));

			if (!DecodeData(UnCompressBuffer, ni->nim_ImageData,
				ni->nim_ImageChunk.ic_NumImageBytes, ni->nim_Width * ni->nim_Height, 
				ni->nim_ImageChunk.ic_BitsPerPixel))
				{
				d1(KPrintF("%s/%s/%ld: Image Data decompression failed.\n", __FILE__, __FUNC__, __LINE__));
				break;
				}
			}
		else
			{
			// unknown image compression type
			d1(KPrintF("%s/%s/%ld: unknown Image Data decompression type.\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld: Image Data Decoded Ok.\n", __FILE__, __FUNC__, __LINE__));

#if 0
		// Dump ImageData
		if (ni->nim_ImageData)
			{
			short n;

			kprintf("-------ImageData1----------\n"));
			for (n=0; n<ni->nim_Width * ni->nim_Height; n++)
				{
				if (0 == n % 16)
					kprintf("\n%04lx: ", n);
				printf("%02lx ", ni->nim_ImageData[n]);
				}
			kprintf("\n");
			}
#endif

		if (ni->nim_ImageChunk.ic_Flags & ICF_HasPalette)
			{
			if (1 + ni->nim_ImageChunk.ic_NumPaletteBytes != ReadChunkBytes(iff, UnCompressBuffer, 1 + ni->nim_ImageChunk.ic_NumPaletteBytes))
				break;
			d1(kprintf("%s/%s/%ld: Palette Data Read Ok.\n", __FILE__, __FUNC__, __LINE__));

			if (cmpNone == ni->nim_ImageChunk.ic_PaletteCompressionType)
				{
				if (1 + ni->nim_ImageChunk.ic_NumPaletteBytes != PaletteSize)
					break;

				CopyMem(UnCompressBuffer, ni->nim_Palette, PaletteSize);
				}
			else if (cmpByteRun1 == ni->nim_ImageChunk.ic_PaletteCompressionType)
				{
				d1(KPrintF("%s/%s/%ld: decompressing Palette Data.\n", __FILE__, __FUNC__, __LINE__));

				if (!DecodeData(UnCompressBuffer, (UBYTE *) ni->nim_Palette, 
					1 + ni->nim_ImageChunk.ic_NumPaletteBytes, PaletteSize,
					8))
					{
					d1(KPrintF("%s/%s/%ld: Palette Data decompression failed.\n", __FILE__, __FUNC__, __LINE__));
					break;
					}
				}
			else
				{
				// unknown palette compression type
				d1(KPrintF("%s/%s/%ld: unknown Palette Data decompression type.\n", __FILE__, __FUNC__, __LINE__));
				break;
				}

			d1(kprintf("%s/%s/%ld: Palette Data Decoded Ok.\n", __FILE__, __FUNC__, __LINE__));
#if 0
			kprintf("-------Palette----------\n");
			for (n=0; n<ni->nim_PaletteSize; n++)
				{
				kprintf("Palette :  r=%02lx  g=%02lx  b=%02lx\n", 
					ni->nim_Palette[n].red, ni->nim_Palette[n].green, ni->nim_Palette[n].blue);
				}
#endif
			}

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (UnCompressBuffer)
		MyFreeVecPooled(UnCompressBuffer);

	if (!Success)
		FreeNewImage(&ni);

	return ni;
}

//----------------------------------------------------------------------------------------

static struct NewImage *ReadARGBImage(struct IFFHandle *iff, const struct FaceChunk *fc)
{
	struct NewImage *ni = NULL;
	BOOL Success = FALSE;
	UBYTE *UnCompressBuffer = NULL;

	do	{
		ULONG ImageSize;
		ULONG ReadLength;
		ULONG BytesRead;
		z_stream stream;
		int zError;

		ni = MyAllocVecPooled(sizeof(struct NewImage));
		if (NULL == ni)
			break;

		memset(ni, 0, sizeof(struct NewImage));

		if (sizeof(ni->nim_ImageChunk) != ReadChunkBytes(iff, &ni->nim_ImageChunk, sizeof(ni->nim_ImageChunk)))
			break;

		ni->nim_ImageChunk.ic_NumImageBytes = SCA_BE2WORD(ni->nim_ImageChunk.ic_NumImageBytes);
		ni->nim_ImageChunk.ic_NumPaletteBytes = SCA_BE2WORD(ni->nim_ImageChunk.ic_NumPaletteBytes);

		d1(KPrintF("%s/%s/%ld: ARGB Chunk Read Ok.\n", __FILE__, __FUNC__, __LINE__));

		d1(KPrintF("%s/%s/%ld: Transparent=%ld  ic_PaletteSize=%ld  Flags=%02lx  ImgComp=%02lx  PalComp=%02lx  BitsPerPixel=%ld  NumImageBytes=%ld  NumPaletteBytes=%ld\n",\
			__FILE__, __FUNC__, __LINE__, \
			(ULONG) ni->nim_ImageChunk.ic_TransparentColor,\
			(ULONG) ni->nim_ImageChunk.ic_PaletteSize, \
			(ULONG) ni->nim_ImageChunk.ic_Flags, \
			(ULONG) ni->nim_ImageChunk.ic_ImageCompressionType,\
			(ULONG) ni->nim_ImageChunk.ic_PaletteCompressionType, \
			(ULONG) ni->nim_ImageChunk.ic_BitsPerPixel,\
			(ULONG) ni->nim_ImageChunk.ic_NumImageBytes,
			(ULONG) ni->nim_ImageChunk.ic_NumPaletteBytes));

		ni->nim_ARGBheader.argb_Width = ni->nim_Width = 1 + fc->fc_Width;
		ni->nim_ARGBheader.argb_Height = ni->nim_Height = 1 + fc->fc_Height;
		ni->nim_PaletteSize = 1 + ni->nim_ImageChunk.ic_PaletteSize;

		ImageSize = ni->nim_ImageChunk.ic_NumImageBytes + 1;
		ReadLength = ((ImageSize + 1) >> 1) << 1;	// round to next even number

		UnCompressBuffer = MyAllocVecPooled(ReadLength);
		d1(KPrintF("%s/%s/%ld: UnCompressBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, UnCompressBuffer));
		if (NULL == UnCompressBuffer)
			break;

		BytesRead = ReadChunkBytes(iff, UnCompressBuffer, ImageSize);
		d1(KPrintF("%s/%s/%ld: ReadLength=%lu  BytesRead=%lu\n", __FILE__, __FUNC__, __LINE__, ImageSize, BytesRead));
		if (BytesRead != ImageSize)
			break;

		stream.avail_out = ni->nim_Width * ni->nim_Height * sizeof(struct ARGB);

		ni->nim_ImageData = MyAllocVecPooled(stream.avail_out);
		d1(KPrintF("%s/%s/%ld: nim_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, ni->nim_ImageData));
		if (NULL == ni->nim_ImageData)
			break;

		ni->nim_ARGBheader.argb_ImageData = (struct ARGB *) ni->nim_ImageData;

		stream.next_in = UnCompressBuffer;
		stream.avail_in = ImageSize;
		stream.next_out = ni->nim_ImageData;
		stream.zalloc = (alloc_func) ZLibAlloc;
		stream.zfree = (free_func) ZLibFree;

		d1(KPrintF("%s/%s/%ld: avail_out=%ld\n", __FILE__, __FUNC__, __LINE__, stream.avail_out));

		zError = inflateInit(&stream);
		d1(KPrintF("%s/%s/%ld: zError=%ld\n", __FILE__, __FUNC__, __LINE__, zError));
		if (Z_OK != zError)
			break;

		zError = inflate(&stream, Z_FINISH);
		d1(KPrintF("%s/%s/%ld: zError=%ld\n", __FILE__, __FUNC__, __LINE__, zError));
		if (Z_STREAM_END != zError)
			{
			inflateEnd(&stream);
			break;
			}

		zError = inflateEnd(&stream);
		d1(KPrintF("%s/%s/%ld: zError=%ld\n", __FILE__, __FUNC__, __LINE__, zError));
		if (Z_OK != zError)
			break;

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (UnCompressBuffer)
		MyFreeVecPooled(UnCompressBuffer);

	if (!Success)
		FreeNewImage(&ni);

	return ni;
}

//----------------------------------------------------------------------------------------

static LONG WriteNewImage(struct IFFHandle *iff, const struct NewImage *ni)
{
	UBYTE *PaletteCompressed = NULL;
	UBYTE *ImageDataCompressed = NULL;
	LONG ErrorCode;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	do	{
		struct ImageChunk SaveImageChunk;
		ULONG Size;
		UWORD NumImageBytesOld;
		UWORD NumPaletteBytesOld;

		SaveImageChunk = ni->nim_ImageChunk;

		if (ni->nim_PaletteSize && ni->nim_Palette
				&& (ni->nim_ImageChunk.ic_Flags & ICF_HasPalette))
			{
			SaveImageChunk.ic_Flags	|= ICF_HasPalette;

			Size = ni->nim_PaletteSize * sizeof(struct ColorRegister);
			PaletteCompressed = EncodeData(8,
				&SaveImageChunk.ic_NumPaletteBytes,
				Size,
				(UBYTE *) ni->nim_Palette);
			d1(KPrintF("%s/%s/%ld: PaletteCompressed=%08lx\n", __FILE__, __FUNC__, __LINE__, PaletteCompressed));
			if (NULL == PaletteCompressed)
				{
				SetIoErr(ErrorCode = ERROR_NO_FREE_STORE);
				break;
				}

			d1(KPrintF("%s/%s/%ld: Size=%lu  ic_NumPaletteBytes=%lu\n", __FILE__, __FUNC__, __LINE__, Size, SaveImageChunk.ic_NumPaletteBytes));

			if (Size == SaveImageChunk.ic_NumPaletteBytes)
				SaveImageChunk.ic_PaletteCompressionType = cmpNone;
			else
				SaveImageChunk.ic_PaletteCompressionType = cmpByteRun1;
			}
		else
			{
			SaveImageChunk.ic_Flags	&= ~ICF_HasPalette;
			SaveImageChunk.ic_NumPaletteBytes = 0;
			}

		Size = ni->nim_Width * ni->nim_Height;
		ImageDataCompressed = EncodeData(SaveImageChunk.ic_BitsPerPixel,
			&SaveImageChunk.ic_NumImageBytes,
			Size,
                        ni->nim_ImageData);
			d1(KPrintF("%s/%s/%ld: ImageDataCompressed=%08lx\n", __FILE__, __FUNC__, __LINE__, ImageDataCompressed));
		if (NULL == ImageDataCompressed)
			{
			SetIoErr(ErrorCode = ERROR_NO_FREE_STORE);
			break;
			}

		d1(KPrintF("%s/%s/%ld: Depth=%ld  Size=%lu  ic_NumImageBytes=%lu\n", __FILE__, __FUNC__, __LINE__, SaveImageChunk.ic_BitsPerPixel, Size, SaveImageChunk.ic_NumImageBytes));

		d1(KPrintF("%s/%s/%ld: ImageData = %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n",\
			__FILE__, __FUNC__, __LINE__, \
			ni->nim_ImageData[0], ni->nim_ImageData[1], ni->nim_ImageData[2], ni->nim_ImageData[3], \
			ni->nim_ImageData[4], ni->nim_ImageData[5], ni->nim_ImageData[6], ni->nim_ImageData[7]));
		d1(KPrintF("%s/%s/%ld: ImageDataCompressed = %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx \n",\
			__FILE__, __FUNC__, __LINE__, \
			ImageDataCompressed[0], ImageDataCompressed[1], ImageDataCompressed[2], ImageDataCompressed[3], \
			ImageDataCompressed[4], ImageDataCompressed[5], ImageDataCompressed[6], ImageDataCompressed[7]));

		if (Size == SaveImageChunk.ic_NumImageBytes)
			SaveImageChunk.ic_ImageCompressionType = cmpNone;
		else
			SaveImageChunk.ic_ImageCompressionType = cmpByteRun1;

		SaveImageChunk.ic_NumImageBytes--;
		if (SaveImageChunk.ic_NumPaletteBytes)
			SaveImageChunk.ic_NumPaletteBytes--;

		ErrorCode = PushChunk(iff, ID_ICON, ID_IMAG, IFFSIZE_UNKNOWN);
		d1(KPrintF("%s/%s/%ld: PushChunk(ID_IMAG) returned %ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));
		if (RETURN_OK != ErrorCode)
			break;

		NumImageBytesOld = SaveImageChunk.ic_NumImageBytes;
		NumPaletteBytesOld = SaveImageChunk.ic_NumPaletteBytes;
		SaveImageChunk.ic_NumImageBytes = SCA_WORD2BE(SaveImageChunk.ic_NumImageBytes);
		SaveImageChunk.ic_NumPaletteBytes = SCA_WORD2BE(SaveImageChunk.ic_NumPaletteBytes);

		if (sizeof(SaveImageChunk) != WriteChunkBytes(iff,
			&SaveImageChunk, sizeof(SaveImageChunk)))
			{
			d1(KPrintF("%s/%s/%ld: WriteChunk(imagehunk) failed\n", __FILE__, __FUNC__, __LINE__));
			ErrorCode = IoErr();
			break;
			}

		SaveImageChunk.ic_NumImageBytes = NumImageBytesOld;
		SaveImageChunk.ic_NumPaletteBytes = NumPaletteBytesOld;

		Size = SaveImageChunk.ic_NumImageBytes + 1;

		if (Size !=  WriteChunkBytes(iff, ImageDataCompressed, Size))
			{
			d1(KPrintF("%s/%s/%ld: WriteChunk(image) failed\n", __FILE__, __FUNC__, __LINE__));
			ErrorCode = IoErr();
			break;
			}

		if (SaveImageChunk.ic_NumPaletteBytes)
			{
			Size = SaveImageChunk.ic_NumPaletteBytes + 1;

			if (Size != WriteChunkBytes(iff, PaletteCompressed, Size))
				{
				d1(KPrintF("%s/%s/%ld: WriteChunk(palette) failed\n", __FILE__, __FUNC__, __LINE__));
				ErrorCode = IoErr();
				break;
				}
			}

		ErrorCode = PopChunk(iff);
		d1(KPrintF("%s/%s/%ld: PopChunk returned %ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));
		if (RETURN_OK != ErrorCode)
			break;
	} while (0);

	if (ImageDataCompressed)
		MyFreeVecPooled(ImageDataCompressed);

	if (PaletteCompressed)
		MyFreeVecPooled(PaletteCompressed);

	d1(KPrintF("%s/%s/%ld: END ErrorCode=%ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));

	return ErrorCode;
}

//----------------------------------------------------------------------------------------

static LONG WriteARGBImage(Class *cl, Object *o, struct IFFHandle *iff,
	const struct NewImage *ni, ULONG ARGBImageTag)
{
	UBYTE *ImageDataCompressed = NULL;
	LONG ErrorCode;
	z_stream stream;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	memset(&stream, 0, sizeof(stream));

	do	{
		struct ImageChunk SaveImageChunk;
		struct ARGBHeader *argbh = NULL;
		ULONG Size;
		UWORD NumImageBytesOld; // remember size before endian conversion
		int zError;

		memset(&argbh, 0, sizeof(argbh));

		SaveImageChunk = ni->nim_ImageChunk;

		SaveImageChunk.ic_Flags	&= ~ICF_HasPalette;
		SaveImageChunk.ic_NumPaletteBytes = 0;
		SaveImageChunk.ic_BitsPerPixel = 0;	// MUST be 0 for OS4 to recognize icon!

		GetAttr(ARGBImageTag, o, (APTR) &argbh);
		d1(KPrintF("%s/%s/%ld: argbh=%08lx  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, argbh, argbh->argb_ImageData));
		if (NULL == argbh->argb_ImageData)
			{
			SetIoErr(ErrorCode = ERROR_OBJECT_WRONG_TYPE);
			break;
			}

		stream.avail_in = Size = argbh->argb_Width * argbh->argb_Height * sizeof(struct ARGB);
		stream.next_in = (Bytef *) argbh->argb_ImageData;

		stream.avail_out = Size;
		stream.next_out = ImageDataCompressed = MyAllocVecPooled(stream.avail_out);
		d1(KPrintF("%s/%s/%ld: ImageDataCompressed=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, ImageDataCompressed, Size));
		if (NULL == ImageDataCompressed)
			{
			SetIoErr(ErrorCode = ERROR_NO_FREE_STORE);
			break;
			}

		stream.zalloc = (alloc_func) ZLibAlloc;
		stream.zfree = (free_func) ZLibFree;
		stream.opaque = Z_NULL;

		zError = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
		d1(KPrintF("%s/%s/%ld: zError=%ld\n", __FILE__, __FUNC__, __LINE__, zError));
		if (Z_OK != zError)
			{
			SetIoErr(ErrorCode = ERROR_NO_FREE_STORE);
			break;
			}

		zError = deflate(&stream, Z_FINISH);
		d1(KPrintF("%s/%s/%ld: zError=%ld\n", __FILE__, __FUNC__, __LINE__, zError));
		if (Z_STREAM_END != zError)
			{
			SetIoErr(ErrorCode = ERROR_NO_FREE_STORE);
			break;
			}

		SaveImageChunk.ic_NumImageBytes = Size - stream.avail_out;

		d1(KPrintF("%s/%s/%ld: Depth=%ld  Size=%lu  ic_NumImageBytes=%lu\n", __FILE__, __FUNC__, __LINE__, SaveImageChunk.ic_BitsPerPixel, Size, SaveImageChunk.ic_NumImageBytes));

		if (Size == SaveImageChunk.ic_NumImageBytes)
			SaveImageChunk.ic_ImageCompressionType = cmpNone;
		else
			SaveImageChunk.ic_ImageCompressionType = cmpByteRun1;

		SaveImageChunk.ic_NumImageBytes--;

		ErrorCode = PushChunk(iff, ID_ICON, ID_ARGB, IFFSIZE_UNKNOWN);
		d1(KPrintF("%s/%s/%ld: PushChunk(ID_ARGB) returned %ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));
		if (RETURN_OK != ErrorCode)
			break;

		NumImageBytesOld = SaveImageChunk.ic_NumImageBytes;
		SaveImageChunk.ic_NumImageBytes = SCA_WORD2BE(SaveImageChunk.ic_NumImageBytes);

		if (sizeof(SaveImageChunk) != WriteChunkBytes(iff,
			&SaveImageChunk, sizeof(SaveImageChunk)))
			{
			d1(KPrintF("%s/%s/%ld: WriteChunk(imagehunk) failed\n", __FILE__, __FUNC__, __LINE__));
			ErrorCode = IoErr();
			break;
			}

		SaveImageChunk.ic_NumImageBytes = NumImageBytesOld;
		Size = SaveImageChunk.ic_NumImageBytes + 1;

		if (Size !=  WriteChunkBytes(iff, ImageDataCompressed, Size))
			{
			d1(KPrintF("%s/%s/%ld: WriteChunk(image) failed\n", __FILE__, __FUNC__, __LINE__));
			ErrorCode = IoErr();
			break;
			}

		ErrorCode = PopChunk(iff);
		d1(KPrintF("%s/%s/%ld: PopChunk returned %ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));
		if (RETURN_OK != ErrorCode)
			break;
	} while (0);

	if (ImageDataCompressed)
		MyFreeVecPooled(ImageDataCompressed);

	(void) deflateEnd(&stream);

	d1(KPrintF("%s/%s/%ld: END ErrorCode=%ld\n", __FILE__, __FUNC__, __LINE__, ErrorCode));

	return ErrorCode;
}

//----------------------------------------------------------------------------------------

static void FreeNewImage(struct NewImage **ni)
{
	if (*ni)
		{
		if ((*ni)->nim_ImageData)
			{
			MyFreeVecPooled((*ni)->nim_ImageData);
			(*ni)->nim_ImageData = NULL;
			}
		if ((*ni)->nim_Palette)
			{
			MyFreeVecPooled((*ni)->nim_Palette);
			(*ni)->nim_Palette = NULL;
			}
		MyFreeVecPooled(*ni);
		*ni = NULL;
		}
}

//----------------------------------------------------------------------------------------

static BOOL CloneNewImage(struct NewImage **niClone, const struct NewImage *niSrc)
{
	BOOL Success = FALSE;

	do	{
		if (niSrc)
			{
			size_t len;

			*niClone = MyAllocVecPooled(sizeof(struct NewImage));
			if (NULL == *niClone)
				break;

			if (niSrc->nim_ImageData)
				{
				// Clone nim_ImageData
				**niClone = *niSrc;
				len = (*niClone)->nim_Width * (*niClone)->nim_Height;
				(*niClone)->nim_Palette = NULL;

				(*niClone)->nim_ImageData = MyAllocVecPooled(len);
				d1(KPrintF("%s/%s/%ld: nim_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, (*niClone)->nim_ImageData));
				if (NULL == (*niClone)->nim_ImageData)
					break;

				memcpy((*niClone)->nim_ImageData, niSrc->nim_ImageData, len);
				}

			if (niSrc->nim_Palette)
				{
				// Clone nim_Palette
				len = niSrc->nim_PaletteSize * sizeof(struct ColorRegister);

				(*niClone)->nim_Palette = MyAllocVecPooled(len);
				d1(KPrintF("%s/%s/%ld: nim_Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, (*niClone)->nim_Palette));

				if (NULL == (*niClone)->nim_Palette)
					break;

				memcpy((*niClone)->nim_Palette, niSrc->nim_Palette, len);
				}

			Success = TRUE;
			}
		else
			{
			*niClone = NULL;
			Success = TRUE;
			}
		} while (0);

	if (*niClone && !Success)
		{
		// Free partially allocated clone
		FreeNewImage(niClone);
		}

	return Success;
}

//----------------------------------------------------------------------------------------
// EncodeData() based on ModifyIcon source by Dirk Stöcker
//----------------------------------------------------------------------------------------

static UBYTE *EncodeData(ULONG Depth, UWORD *DestLength,
	ULONG SrcLength, const UBYTE *SrcData)
{
	int   i, j, k;
	ULONG bitbuf, numbits;
	UBYTE *buf;
	LONG  ressize, numcopy, numequal;

	buf = MyAllocVecPooled(SrcLength * 2);
	if (NULL == buf)
		return NULL;

	numcopy = 0;
	numequal = 1;
	bitbuf = 0;
	numbits = 0;
	ressize = 0;
	k = 0; /* the real output pointer */

	for (i = 1; (numequal || numcopy) && (ressize < SrcLength);)
		{
		if (i < SrcLength && numequal && (SrcData[i-1] == SrcData[i]))
			{
			++numequal;
                        ++i;
			}
		else if (i < SrcLength && numequal * Depth <= 16)
			{
			numcopy += numequal;
			numequal = 1;
                        ++i;
			}
		else
			{
			/* care for end case, where it maybe better to join the two */
			if (i == SrcLength && numcopy + numequal <= 128 && (numequal-1) * Depth <= 8)
				{
				numcopy += numequal;
                                numequal = 0;
				}

			if (numcopy)
				{
				if ((j = numcopy) > 128)
					j = 128;

				bitbuf = (bitbuf<<8) | (j-1);
				numcopy -= j;
				}
			else
				{
				if ((j = numequal) > 128)
					j = 128;

				bitbuf = (bitbuf<<8) | (256-(j-1));
				numequal -= j;
				k += j-1;
				j = 1;
				}

			buf[ressize++] = (bitbuf >> numbits);

			while (j--)
				{
				numbits += Depth;
				bitbuf = (bitbuf << Depth) | SrcData[k++];

				if (numbits >= 8)
					{
					numbits -= 8;
					buf[ressize++] = (bitbuf >> numbits);
					}
				}

			if (i < SrcLength && !numcopy && !numequal)
				{
				numequal = 1;
                                ++i;
				}
			}
		}

	if (numbits)
		buf[ressize++] = bitbuf << (8-numbits);

	if (ressize >= SrcLength)
		{
		// no RLE
		ressize = SrcLength;
		CopyMem((APTR) SrcData, buf, SrcLength);
		}

	*DestLength = ressize;

	return buf;
}

//----------------------------------------------------------------------------------------

static UBYTE INLINE GetNextDestByte(const UBYTE **src, size_t *SrcLength, short *srcBits, 
	UBYTE *srcByte, LONG BitsPerEntry)
{
	UBYTE Mask;
	short destBits;
	short Shift;
	UBYTE Result = 0;

	destBits = 0;
	Mask = (1 << BitsPerEntry) - 1;
	Shift = 8 - BitsPerEntry - *srcBits;

	d1(kprintf("%s/%s/%ld: Shift=%ld  BitsPerEntry=%ld  Mask=%02lx\n", \
		__FILE__, __FUNC__, __LINE__, Shift, BitsPerEntry, Mask));

	while (destBits < BitsPerEntry)
		{
		if (Shift < 0)
			{
			d1(kprintf("%s/%s/%ld: (1) srcByte=%02lx  sMask=%02lx  Shift=%ld\n", \
				__FILE__, __FUNC__, __LINE__, *srcByte, (Mask >> -Shift), Shift));

			Result  |= (*srcByte & (Mask >> -Shift)) << -Shift;
			destBits += BitsPerEntry + Shift;
			*srcBits += BitsPerEntry + Shift;
			Shift += 8;
			}
		else
			{
			d1(kprintf("%s/%s/%ld: (2) srcByte=%ld  sMask=%02lx  Shift=%ld\n", \
				__FILE__, __FUNC__, __LINE__, *srcByte, (Mask << Shift), Shift));

			Result |= (*srcByte & (Mask << Shift)) >> Shift;

			if (Shift + BitsPerEntry > 8)
				{
				destBits += 8 - Shift;
				*srcBits += 8 - Shift;
				}
			else
				{
				destBits += BitsPerEntry;
				*srcBits += BitsPerEntry;
				}

			Shift -= BitsPerEntry;
			if (Shift <= -BitsPerEntry)
				Shift += 8;
			}

		d1(kprintf("%s/%s/%ld: Result = %02lx\n", __FILE__, __FUNC__, __LINE__, Result));

		if (*SrcLength && *srcBits >= 8)
			{
			*srcBits %= 8;
			(*src)++;
			(*SrcLength)--;
			*srcByte = **src;
			}
		}


	d1(kprintf("%s/%s/%ld: srcBits=%ld  destBits=%ld  SrcByte=%02lx  Result=%02lx\n", \
		__FILE__, __FUNC__, __LINE__, *srcBits, destBits, *srcByte, Result));

	return Result;
}

//----------------------------------------------------------------------------------------

static BOOL DecodeData(const UBYTE *src, UBYTE *dest, size_t SrcLength, size_t DestLength, LONG BitsPerEntry)
{
	short srcBits;
	signed char Rep = 0;
	short RepeatCount = 0;
	UBYTE RepeatByte = 0;
	UBYTE srcByte;

	srcByte = *src;
	srcBits = 0;

	while (DestLength)
		{
		d1(KPrintF("%s/%s/%ld: DestLength=%ld  in=%02lx  out=%02lx\n",\
			__FILE__, __FUNC__, __LINE__, DestLength, srcByte, *dest));

		if (0 == RepeatCount)
			{
			Rep = GetNextDestByte(&src, &SrcLength, &srcBits, &srcByte, 8);

			if (Rep < 0)
				{
				RepeatCount = 1 - Rep;
				RepeatByte = GetNextDestByte(&src, &SrcLength, &srcBits, &srcByte, BitsPerEntry);
				}
			else
				RepeatCount = 1 + Rep;

			d1(kprintf("%s/%s/%ld:   Rep=%02lx  RepeatCount=%ld  RepeatByte=%02lx\n", \
				__FILE__, __FUNC__, __LINE__, Rep & 0xff, RepeatCount, RepeatByte));
			}

		if (Rep >= 0)
			*dest++ = GetNextDestByte(&src, &SrcLength, &srcBits, &srcByte, BitsPerEntry);
		else
			*dest++ = RepeatByte;

		RepeatCount--;
		DestLength--;
		}

	d1(KPrintF("%s/%s/%ld: END SrcLength=%lu DestLength=%lu\n", __FILE__, __FUNC__, __LINE__, SrcLength, DestLength));

	return (BOOL) (0 == DestLength && 0 == SrcLength);
}

//----------------------------------------------------------------------------------------

static UBYTE *GenerateMask(struct NewImage *nim)
{
	ULONG Size;
	UBYTE *MaskArray;
	ULONG BytesPerRow;

	d1(KPrintF("%s/%s/%ld: ic_Flags=%02lx  ic_TransparentColor=%ld\n", \
		__FILE__, __FUNC__, __LINE__, nim->nim_ImageChunk.ic_Flags, nim->nim_ImageChunk.ic_TransparentColor));

	BytesPerRow = BYTESPERROW(nim->nim_Width);
	Size = nim->nim_Height * BytesPerRow;
	MaskArray = MyAllocVecPooled(Size);
	if (MaskArray)
		{
		if (nim->nim_ImageChunk.ic_Flags & ICF_IsTransparent)
			{
			const UBYTE *ChunkyData = nim->nim_ImageData;
			UBYTE *MaskPtr = MaskArray;
			ULONG y;

			memset(MaskArray, 0, Size);

			for (y = 0; y < nim->nim_Height; y++)
				{
				ULONG x;
				UBYTE BitMask = 0x80;
				UBYTE *MaskPtr2 = MaskPtr;

				for (x = 0; x < nim->nim_Width; x++)
					{
					if (nim->nim_ImageChunk.ic_TransparentColor != *ChunkyData++)
						*MaskPtr2 |= BitMask;
					BitMask >>= 1;
					if (0 == BitMask)
						{
						BitMask = 0x80;
						MaskPtr2++;
						}
					}

				MaskPtr += BytesPerRow;
				}
			}
		else
			{
			memset(MaskArray, ~0, Size);
			}
		}

	return MaskArray;
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

			d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, MemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, MemPool, Size));

	return NULL;
}


static void MyFreeVecPooled(APTR mem)
{
	d1(kprintf("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, MemPool, mem));
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

static struct NewImage *NewImageFromSAC(struct ScalosBitMapAndColor *sac)
{
	struct NewImage *ni = NULL;
	struct BitMap *TempBM;
	BOOL Success = FALSE;

	do	{
		ULONG n;
		ULONG y;
		const ULONG *PalettePtr;
		struct RastPort rp;
		struct RastPort TempRp;
		UBYTE *ImagePtr;
		ULONG NewWidth  = sac->sac_Width;
		ULONG NewHeight = sac->sac_Height;

		InitRastPort(&rp);
		InitRastPort(&TempRp);

		rp.BitMap = sac->sac_BitMap;
		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: rp.BitMap=%08lx\n", __FILE__, __FUNC__, __LINE__, rp.BitMap));

		// setup temp. RastPort for use by WritePixelLine8()
		TempRp.Layer = NULL;
		TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(NewWidth), 1, 8, 0, NULL);

		if (NULL == TempBM)
			break;
		ni = MyAllocVecPooled(sizeof(struct NewImage));
		if (NULL == ni)
			break;

		memset(ni, 0, sizeof(struct NewImage));

		ni->nim_Width = NewWidth;
		ni->nim_Height = NewHeight;

		ni->nim_ImageChunk.ic_TransparentColor = sac->sac_TransparentColor;
		ni->nim_ImageChunk.ic_Flags |= ICF_HasPalette;
		if (SAC_TRANSPARENT_NONE != sac->sac_TransparentColor)
			ni->nim_ImageChunk.ic_Flags |= ICF_IsTransparent;

		d1(KPrintF(__FILE__ "/" "%s/%s/%ld: sac_TransparentColor=%ld\n", __FILE__, __FUNC__, __LINE__, sac->sac_TransparentColor));

		ni->nim_ImageChunk.ic_PaletteSize = sac->sac_NumColors - 1;
		ni->nim_ImageChunk.ic_BitsPerPixel = sac->sac_Depth;

		ni->nim_ImageData = MyAllocVecPooled(PIXELARRAY8_BUFFERSIZE(ni->nim_Width, ni->nim_Height));
		if (NULL == ni->nim_ImageData)
			break;

		ni->nim_PaletteSize = sac->sac_NumColors;
		ni->nim_Palette = MyAllocVecPooled(sac->sac_NumColors * sizeof(struct ColorRegister));
		if (NULL == ni->nim_Palette)
			break;

		// nothing can go wrong from here on
		Success = TRUE;

		d1(KPrintF("%s/%s/%ld: NumColors=%lu\n", __FILE__, __FUNC__, __LINE__, sac->sac_NumColors));

		// Fill nim_Palette fom sac_ColorTable
		for (n = 0, PalettePtr = sac->sac_ColorTable; n < sac->sac_NumColors; n++)
			{
			ni->nim_Palette[n].red   = *PalettePtr++ >> 24;
			ni->nim_Palette[n].green = *PalettePtr++ >> 24;
			ni->nim_Palette[n].blue  = *PalettePtr++ >> 24;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// copy image data from sac_BitMap into nim_ImageData
		for (y = 0, ImagePtr = ni->nim_ImageData; y < NewHeight; y++)
			{
			d1(KPrintF("%s/%s/%ld: y=%ld\n", __FILE__, __FUNC__, __LINE__, y));
			ReadPixelLine8(&rp, 0, y, NewWidth,
				ImagePtr, &TempRp);
			ImagePtr += NewWidth;
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	if (!Success)
		FreeNewImage(&ni);

	if (TempBM)
		FreeBitMap(TempBM);

	return ni;
}

//----------------------------------------------------------------------------------------

static void GenerateNormalImageMask(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	UBYTE *MaskNormal = NULL;

	if (inst->aio_Image1->nim_ImageChunk.ic_Flags & ICF_IsTransparent)
		{
		MaskNormal = GenerateMask(inst->aio_Image1);
		}

	d1(KPrintF("%s/%s/%ld:  MaskNormal=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskNormal));

	SetAttrs(o, IDTA_Mask_Normal, (ULONG) MaskNormal,
		IDTA_Width_Mask_Normal, inst->aio_Image1->nim_Width,
		IDTA_Height_Mask_Normal, inst->aio_Image1->nim_Height,
		TAG_END);

	if (MaskNormal)
		MyFreeVecPooled(MaskNormal);
}

//----------------------------------------------------------------------------------------
		
static void GenerateSelectedImageMask(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	UBYTE *MaskSelected = NULL;

        if (inst->aio_Image2 && (inst->aio_Image2->nim_ImageChunk.ic_Flags & ICF_IsTransparent))
		{
		MaskSelected = GenerateMask(inst->aio_Image2);
		}

	d1(KPrintF("%s/%s/%ld:  MaskSelected=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskSelected));

	SetAttrs(o, IDTA_Mask_Selected, (ULONG) MaskSelected,
		IDTA_Width_Mask_Selected, inst->aio_Image2->nim_Width,
		IDTA_Height_Mask_Selected, inst->aio_Image2->nim_Height,
		TAG_END);

	if (MaskSelected)
		MyFreeVecPooled(MaskSelected);
}

//----------------------------------------------------------------------------------------

static struct NewImage *NewImageFromNormalImage(const struct InstanceData *inst, const struct NewImage *niNormal)
{
	struct NewImage *ni = NULL;
	BOOL Success = FALSE;

	d1(KPrintF("%s/%s/%ld: inst=%08lx, niNormal=%08lx\n", __FILE__, __FUNC__, __LINE__, inst, niNormal));

	do	{
		ULONG n;
		const struct ColorRegister *PaletteSrcPtr;
		struct ColorRegister *PaletteDestPtr;
		size_t ImgSize;
		ULONG Backfill[2] = { IDTA_BACKFILL_NONE, IDTA_BACKFILL_NONE };
		ULONG BackfillCount = 0;

		ni = MyAllocVecPooled(sizeof(struct NewImage));
		if (NULL == ni)
			break;

		memset(ni, 0, sizeof(struct NewImage));

		ni->nim_Width = niNormal->nim_Width;
		ni->nim_Height = niNormal->nim_Height;

		ImgSize = ni->nim_Width * ni->nim_Height;

		ni->nim_ImageChunk.ic_TransparentColor = niNormal->nim_ImageChunk.ic_TransparentColor;
		ni->nim_ImageChunk.ic_Flags = niNormal->nim_ImageChunk.ic_Flags;

		d1(KPrintF("%s/%s/%ld: ic_TransparentColor=%ld\n", __FILE__, __FUNC__, __LINE__, ni->nim_ImageChunk.ic_TransparentColor));
		d1(KPrintF("%s/%s/%ld: ic_PaletteSize=%lu\n", __FILE__, __FUNC__, __LINE__, niNormal->nim_ImageChunk.ic_PaletteSize));

		ni->nim_ImageChunk.ic_PaletteSize = niNormal->nim_ImageChunk.ic_PaletteSize;
		ni->nim_ImageChunk.ic_BitsPerPixel = niNormal->nim_ImageChunk.ic_BitsPerPixel;

		ni->nim_ImageData = MyAllocVecPooled(ImgSize);
		if (NULL == ni->nim_ImageData)
			break;

		ni->nim_PaletteSize = niNormal->nim_PaletteSize;

		if (niNormal->nim_Palette)
			{
			ni->nim_Palette = MyAllocVecPooled(ni->nim_PaletteSize * sizeof(struct ColorRegister));
			if (NULL == ni->nim_Palette)
				break;

			// nothing can go wrong from here on
			Success = TRUE;

			// remember color table indices for backfill colors
			if (IDTA_BACKFILL_NONE != inst->aio_BackfillPenSel)
				Backfill[BackfillCount++] = ni->nim_PaletteSize - 1;
			if (IDTA_BACKFILL_NONE != inst->aio_BackfillPenNorm)
				Backfill[BackfillCount]	= ni->nim_PaletteSize - 2;

			d1(KPrintF("%s/%s/%ld: nim_PaletteSize=%lu  aio_BackfillPenNorm=%ld  aio_BackfillPenSel=%ld\n", \
				__FILE__, __FUNC__, __LINE__, ni->nim_PaletteSize, inst->aio_BackfillPenNorm, inst->aio_BackfillPenSel));

			// copy palette from niNormal and adjust colors
			for (n = 0, PaletteSrcPtr = niNormal->nim_Palette, PaletteDestPtr = ni->nim_Palette;
				n < ni->nim_PaletteSize; n++)
				{
				if (n == Backfill[0] || n == Backfill[1])
					{
					// do not modify backfill colors
					*PaletteDestPtr = *PaletteSrcPtr;
					}
				else
					{
					PaletteDestPtr->red   = (2 * PaletteSrcPtr->red)   / 3;
					PaletteDestPtr->green = (2 * PaletteSrcPtr->green) / 3;
					PaletteDestPtr->blue  = (2 * PaletteSrcPtr->blue)  / 3;
					}
				PaletteDestPtr++;
				PaletteSrcPtr++;
				}

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// copy image data from niNormal into nim_ImageData
			memcpy(ni->nim_ImageData, niNormal->nim_ImageData, ImgSize);
			}
		else
			{
			// we have no palette
			// 32bit GlowIcon
			size_t ISize = niNormal->nim_ARGBheader.argb_Width * niNormal->nim_ARGBheader.argb_Height;
			const struct ARGB *Src;
			struct ARGB *Dest;

			ni->nim_ARGBheader.argb_Width = niNormal->nim_ARGBheader.argb_Width;
			ni->nim_ARGBheader.argb_Height = niNormal->nim_ARGBheader.argb_Height;

			ni->nim_ARGBheader.argb_ImageData = MyAllocVecPooled(ISize * sizeof(struct ARGB));

			d1(KPrintF("%s/%s/%ld: ISize=%lu  argb_ImageData=%08lx\n", __FILE__, __FUNC__, __LINE__, ISize, ni->nim_ARGBheader.argb_ImageData));
			if (NULL == ni->nim_ARGBheader.argb_ImageData)
				break;
			
			// nothing can go wrong from here on
			Success = TRUE;

			// Copy image data and adjust RGB values
			Src = niNormal->nim_ARGBheader.argb_ImageData;
			Dest = ni->nim_ARGBheader.argb_ImageData;
			for (n = 0; n < ISize; n++, Src++, Dest++)
				{
				Dest->Red   = (2 * Src->Red  ) / 3;
				Dest->Green = (2 * Src->Green) / 3;
				Dest->Blue  = (2 * Src->Blue ) / 3;
				Dest->Alpha = Src->Alpha;
				}
                        }

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	if (!Success)
		FreeNewImage(&ni);

	d1(KPrintF("%s/%s/%ld: ni=%08lx\n", __FILE__, __FUNC__, __LINE__, ni));

	return ni;
}

//----------------------------------------------------------------------------------------

static void *ZLibAlloc(void *p, int items, int size)
{
	return MyAllocVecPooled(items * size);
}


static void ZLibFree(void *p, void *addr)
{
	MyFreeVecPooled(addr);
}

//----------------------------------------------------------------------------------------

static SAVEDS(LONG) StreamHookDispatcher(struct Hook *hook, struct IFFHandle *iff, const struct IFFStreamCmd *cmd)
{
	LONG Result = 0;

	switch (cmd->sc_Command)
		{
	case IFFCMD_INIT:
		/* Prepare your stream for reading. This is used for certain
		streams that can't be read immediately upon opening, and need
		further preparation. (The clipboard.device is an example of
		such a stream.)  This operation is allowed to fail;  any
		error code will be returned directly to the client. sc_Buf
		and sc_NBytes have no meaning here. */
		break;
	case IFFCMD_CLEANUP:
		/* Terminate the transaction with the associated stream. This
		is used with streams that can't simply be closed. (Again,
		the clipboard is an example of such a stream.)  This
		operation is not permitted to fail;  any error returned will
		be ignored (best to return 0, though). sc_Buf and sc_NBytes
		have no meaning here. */
		break;
	case IFFCMD_READ:
		/* Read from the stream. You are to read sc_NBytes from the
		stream and place them in the buffer pointed to by sc_Buf.
		Any (non-zero) error returned will be remapped by the parser
		into IFFERR_READ. */
		if (1 != FRead((BPTR)iff->iff_Stream, cmd->sc_Buf, cmd->sc_NBytes, 1))
			Result = IFFERR_READ;
		break;
	case IFFCMD_WRITE:
		/* Write to the stream. You are to write sc_NBytes to the
		stream from the buffer pointed to by sc_Buf. Any (non-zero)
		error returned will be remapped by the parser into
		IFFERR_WRITE. */
		if (1 != FWrite((BPTR)iff->iff_Stream, cmd->sc_Buf, cmd->sc_NBytes, 1))
			Result = IFFERR_WRITE;
		d1(KPrintF("%s/%s/%ld: IFFCMD_WRITE Result=%ld  sc_NBytes=%ld\n", __FILE__, __FUNC__, __LINE__, Result, cmd->sc_NBytes));
		break;
	case IFFCMD_SEEK:
		/* Seek on the stream. You are to perform a seek on the stream
		relative to the current position. sc_NBytes is signed;
		negative values mean seek backward, positive values mean seek
		forward. sc_Buf has no meaning here. Any (non-zero) error
		returned will be remapped by the parser into IFFERR_SEEK. */
		if (Seek((BPTR)iff->iff_Stream, cmd->sc_NBytes, OFFSET_CURRENT) < 0)
			Result = IFFERR_SEEK;
		d1(KPrintF("%s/%s/%ld: IFFCMD_SEEK Result=%ld  sc_NBytes=%ld\n", __FILE__, __FUNC__, __LINE__, Result, cmd->sc_NBytes));
		break;
	default:
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------------------

static void SetParentAttributes(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	TIMESTAMP_d1();
	SetAttrs(o,
		GA_Width, inst->aio_Image1->nim_Width,
		GA_Height, inst->aio_Image1->nim_Height,
		TAG_END);
	d1(KPrintF("%s/%s/%ld:  o=%08lx  Image1 nim_Palette=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst->aio_Image1->nim_Palette));

	if (NULL == inst->aio_Image1->nim_Palette)
		{
		// 32bit GlowIcon
		SetAttrs(o,
			IDTA_CopyARGBImageData,	FALSE,
			IDTA_ARGBImageData, (ULONG) &inst->aio_Image1->nim_ARGBheader,
			TAG_END);
		}
	TIMESTAMP_d1();
	if (inst->aio_Image2 && NULL == inst->aio_Image2->nim_Palette)
		{
		// 32bit GlowIcon
		SetAttrs(o,
			IDTA_CopySelARGBImageData, FALSE,
			IDTA_SelARGBImageData, (ULONG) &inst->aio_Image2->nim_ARGBheader,
			TAG_END);
		}

	if (inst->aio_Image1->nim_Palette)
		{
		TIMESTAMP_d1();
		GenerateNormalImageMask(cl, o);
		TIMESTAMP_d1();
		GenerateSelectedImageMask(cl, o);
		TIMESTAMP_d1();
		}

	TIMESTAMP_d1();

	if (inst->aio_DiskObject->do_DrawerData)
		{
		TIMESTAMP_d1();
		SetAttrs(o,
			IDTA_ViewModes, inst->aio_DiskObject->do_DrawerData->dd_ViewModes,
			IDTA_Flags, inst->aio_DiskObject->do_DrawerData->dd_Flags,
			IDTA_WinCurrentX, inst->aio_DiskObject->do_DrawerData->dd_CurrentX,
			IDTA_WinCurrentY, inst->aio_DiskObject->do_DrawerData->dd_CurrentY,
			IDTA_WindowRect, (ULONG) &inst->aio_DiskObject->do_DrawerData->dd_NewWindow,
			TAG_END);
		}
	TIMESTAMP_d1();
	SetAttrs(o,
		IDTA_ToolTypes, (ULONG) inst->aio_DiskObject->do_ToolTypes,
		IDTA_Stacksize, inst->aio_DiskObject->do_StackSize,
		IDTA_DefaultTool, (ULONG) inst->aio_DiskObject->do_DefaultTool,
		IDTA_Type, inst->aio_DiskObject->do_Type,
		IDTA_Borderless, inst->aio_Borderless,
		TAG_END);
}

//----------------------------------------------------------------------------------------

#if defined(__AROS__)
static BOOL ReadConvertStandardIcon(BPTR fd, struct DiskObject *dobj)
{
	BOOL success = FALSE;

	APTR block = MyAllocVecPooled(78); // sizeof struct DiskObject on 68k
	if (NULL != block)
		{
		if (78 == FRead(fd, block, 1, 78))
			{
			dobj->do_Magic = SCA_BE2WORD(*(WORD *)block);
			dobj->do_Version = SCA_BE2WORD(*(WORD *)(block + 2));
			// Ignore 4
			dobj->do_Gadget.LeftEdge = SCA_BE2WORD(*(WORD *)(block + 8));
			dobj->do_Gadget.TopEdge = SCA_BE2WORD(*(WORD *)(block + 10));
			dobj->do_Gadget.Width = SCA_BE2WORD(*(WORD *)(block + 12));
			dobj->do_Gadget.Height = SCA_BE2WORD(*(WORD *)(block + 14));
			dobj->do_Gadget.Flags = SCA_BE2WORD(*(WORD *)(block + 16));
			dobj->do_Gadget.Activation = SCA_BE2WORD(*(WORD *)(block + 18));
			dobj->do_Gadget.GadgetType = SCA_BE2WORD(*(WORD *)(block + 20));
			dobj->do_Gadget.GadgetRender = (APTR)SCA_BE2LONG(*(LONG *)(block + 22));
			dobj->do_Gadget.SelectRender = (APTR)SCA_BE2LONG(*(LONG *)(block + 26));
			dobj->do_Gadget.GadgetText = (APTR)SCA_BE2LONG(*(LONG *)(block + 30));
			dobj->do_Gadget.MutualExclude = SCA_BE2LONG(*(LONG *)(block + 34));
			dobj->do_Gadget.SpecialInfo = (APTR)SCA_BE2LONG(*(LONG *)(block + 38));
			dobj->do_Gadget.GadgetID = SCA_BE2WORD(*(WORD *)(block + 42));
			dobj->do_Gadget.UserData = (APTR)SCA_BE2LONG(*(LONG *)(block + 44));
			dobj->do_Type = *(BYTE *)(block + 48);
			// Ignore 1
			dobj->do_DefaultTool = (APTR)SCA_BE2LONG(*(LONG *)(block + 50));
			dobj->do_ToolTypes = (APTR)SCA_BE2LONG(*(LONG *)(block + 54));
			dobj->do_CurrentX = SCA_BE2LONG(*(LONG *)(block + 58));
			dobj->do_CurrentY = SCA_BE2LONG(*(LONG *)(block + 62));
			dobj->do_DrawerData = (APTR)SCA_BE2LONG(*(LONG *)(block + 66));
			dobj->do_ToolWindow = (APTR)SCA_BE2LONG(*(LONG *)(block + 70));
			dobj->do_StackSize = SCA_BE2LONG(*(LONG *)(block + 74));

			success = TRUE;
			}
		MyFreeVecPooled(block);
		}
	return success;
}

//----------------------------------------------------------------------------------------

static BOOL WriteConvertStandardIcon(BPTR fd, struct DiskObject *dobj)
{
	BOOL success = FALSE;

	APTR block = MyAllocVecPooled(78); // sizeof struct DiskObject on 68k
	if (NULL != block)
		{
		memset(block, 0, 78);
		*(WORD *)block = SCA_WORD2BE(dobj->do_Magic);
		*(WORD *)(block + 2) = SCA_WORD2BE(dobj->do_Version);
		// Ignore 4
		*(WORD *)(block + 8) = SCA_WORD2BE(dobj->do_Gadget.LeftEdge);
		*(WORD *)(block + 10) = SCA_WORD2BE(dobj->do_Gadget.TopEdge);
		*(WORD *)(block + 12) = SCA_WORD2BE(dobj->do_Gadget.Width);
		*(WORD *)(block + 14) = SCA_WORD2BE(dobj->do_Gadget.Height);
		*(WORD *)(block + 16) = SCA_WORD2BE(dobj->do_Gadget.Flags);
		*(WORD *)(block + 18) = SCA_WORD2BE(dobj->do_Gadget.Activation);
		*(WORD *)(block + 20) = SCA_WORD2BE(dobj->do_Gadget.GadgetType);
		*(LONG *)(block + 22) = SCA_LONG2BE((LONG)dobj->do_Gadget.GadgetRender);
		*(LONG *)(block + 26) = SCA_LONG2BE((LONG)dobj->do_Gadget.SelectRender);
		*(LONG *)(block + 30) = SCA_LONG2BE((LONG)dobj->do_Gadget.GadgetText);
		*(LONG *)(block + 34) = SCA_LONG2BE(dobj->do_Gadget.MutualExclude);
		*(LONG *)(block + 38) = SCA_LONG2BE((LONG)dobj->do_Gadget.SpecialInfo);
		*(WORD *)(block + 42) = SCA_WORD2BE(dobj->do_Gadget.GadgetID);
		*(LONG *)(block + 44) = SCA_LONG2BE((LONG)dobj->do_Gadget.UserData);
		*(BYTE *)(block + 48) = dobj->do_Type;
		// Ignore 1
		*(LONG *)(block + 50) = SCA_LONG2BE((LONG)dobj->do_DefaultTool);
		*(LONG *)(block + 54) = SCA_LONG2BE((LONG)dobj->do_ToolTypes);
		*(LONG *)(block + 58) = SCA_LONG2BE(dobj->do_CurrentX);
		*(LONG *)(block + 62) = SCA_LONG2BE(dobj->do_CurrentY);
		*(LONG *)(block + 66) = SCA_LONG2BE((LONG)dobj->do_DrawerData);
		*(LONG *)(block + 70) = SCA_LONG2BE((LONG)dobj->do_ToolWindow);
		*(LONG *)(block + 74) = SCA_LONG2BE(dobj->do_StackSize);

		if (78 == FWrite(fd, block, 1, 78))
			success = TRUE;

		MyFreeVecPooled(block);
		}
	return success;
}

//----------------------------------------------------------------------------------------

static BOOL ReadConvertDrawerData(BPTR fd, struct DrawerData *drawer)
{
	BOOL success = FALSE;

	APTR block = MyAllocVecPooled(56); // sizeof struct OldDrawerData on 68k
	if (NULL != block)
		{
		if (56 == FRead(fd, block, 1, 56))
			{
			drawer->dd_NewWindow.LeftEdge = SCA_BE2WORD(*(WORD *)block);
			drawer->dd_NewWindow.TopEdge = SCA_BE2WORD(*(WORD *)(block + 2));
			drawer->dd_NewWindow.Width = SCA_BE2WORD(*(WORD *)(block + 4));
			drawer->dd_NewWindow.Height = SCA_BE2WORD(*(WORD *)(block + 6));
			drawer->dd_NewWindow.DetailPen = *(BYTE *)(block + 8);
			drawer->dd_NewWindow.BlockPen = *(BYTE *)(block + 9);
			drawer->dd_NewWindow.IDCMPFlags = SCA_BE2LONG(*(LONG *)(block + 10));
			drawer->dd_NewWindow.Flags = SCA_BE2LONG(*(LONG *)(block + 14));
			// Ignore 20
			drawer->dd_NewWindow.MinWidth = SCA_BE2WORD(*(WORD *)(block + 38));
			drawer->dd_NewWindow.MinHeight = SCA_BE2WORD(*(WORD *)(block + 40));
			drawer->dd_NewWindow.MaxWidth = SCA_BE2WORD(*(WORD *)(block + 42));
			drawer->dd_NewWindow.MaxHeight = SCA_BE2WORD(*(WORD *)(block + 44));
			drawer->dd_NewWindow.Type = SCA_BE2WORD(*(WORD *)(block + 46));
			drawer->dd_CurrentX = SCA_BE2LONG(*(LONG *)(block + 48));
			drawer->dd_CurrentY = SCA_BE2LONG(*(LONG *)(block + 52));

			success = TRUE;
			}
		MyFreeVecPooled(block);
		}
	return success;
}

//----------------------------------------------------------------------------------------

static BOOL WriteConvertDrawerData(BPTR fd, struct DrawerData *drawer)
{
	BOOL success = FALSE;

	APTR block = MyAllocVecPooled(56); // sizeof struct OldDrawerData on 68k
	if (NULL != block)
		{
		memset(block, 0, 56);
		*(WORD *)block = SCA_WORD2BE(drawer->dd_NewWindow.LeftEdge);
		*(WORD *)(block + 2) = SCA_WORD2BE(drawer->dd_NewWindow.TopEdge);
		*(WORD *)(block + 4) = SCA_WORD2BE(drawer->dd_NewWindow.Width);
		*(WORD *)(block + 6) = SCA_WORD2BE(drawer->dd_NewWindow.Height);
		*(BYTE *)(block + 8) = drawer->dd_NewWindow.DetailPen;
		*(BYTE *)(block + 9) = drawer->dd_NewWindow.BlockPen;
		*(LONG *)(block + 10) = SCA_LONG2BE(drawer->dd_NewWindow.IDCMPFlags);
		*(LONG *)(block + 14) = SCA_LONG2BE(drawer->dd_NewWindow.Flags);
		// Ignore 20
		*(WORD *)(block + 38) = SCA_WORD2BE(drawer->dd_NewWindow.MinWidth);
		*(WORD *)(block + 40) = SCA_WORD2BE(drawer->dd_NewWindow.MinHeight);
		*(WORD *)(block + 42) = SCA_WORD2BE(drawer->dd_NewWindow.MaxWidth);
		*(WORD *)(block + 44) = SCA_WORD2BE(drawer->dd_NewWindow.MaxHeight);
		*(WORD *)(block + 46) = SCA_WORD2BE(drawer->dd_NewWindow.Type);
		*(LONG *)(block + 48) = SCA_LONG2BE(drawer->dd_CurrentX);
		*(LONG *)(block + 52) = SCA_LONG2BE(drawer->dd_CurrentY);

		if (56 == FWrite(fd, block, 1, 56))
			success = TRUE;

		MyFreeVecPooled(block);
		}
	return success;
}

//----------------------------------------------------------------------------------------

static BOOL ReadConvertImage(BPTR fd, struct Image *img)
{
	BOOL success = FALSE;

	APTR block = MyAllocVecPooled(20); // sizeof struct Image on 68k
	if (NULL != block)
		{
		if (20 == FRead(fd, block, 1, 20))
			{
			img->LeftEdge = SCA_BE2WORD(*(WORD *)block);
			img->TopEdge = SCA_BE2WORD(*(WORD *)(block + 2));
			img->Width = SCA_BE2WORD(*(WORD *)(block + 4));
			img->Height = SCA_BE2WORD(*(WORD *)(block + 6));
			img->Depth = SCA_BE2WORD(*(WORD *)(block + 8));
			img->ImageData = (APTR)SCA_BE2LONG(*(LONG *)(block + 10));
			img->PlanePick = *(BYTE *)(block + 14);
			img->PlaneOnOff = *(BYTE *)(block + 15);
			img->NextImage = (APTR)SCA_BE2LONG(*(LONG *)(block + 16));

			success = TRUE;
			}

		MyFreeVecPooled(block);
		}

	return success;
}

//----------------------------------------------------------------------------------------

static BOOL WriteConvertImage(BPTR fd, struct Image *img)
{
	BOOL success = FALSE;

	APTR block = MyAllocVecPooled(20); // sizeof struct Image on 68k
	if (NULL != block)
		{
		memset(block, 0, 20);
		*(WORD *)block = SCA_WORD2BE(img->LeftEdge);
		*(WORD *)(block + 2) = SCA_WORD2BE(img->TopEdge) ;
		*(WORD *)(block + 4) = SCA_WORD2BE(img->Width);
		*(WORD *)(block + 6) = SCA_WORD2BE(img->Height);
		*(WORD *)(block + 8) = SCA_WORD2BE(img->Depth);
		*(LONG *)(block + 10) = SCA_LONG2BE((LONG)img->ImageData);
		*(BYTE *)(block + 14) = img->PlanePick;
		*(BYTE *)(block + 15) = img->PlaneOnOff;
		*(LONG *)(block + 16) = SCA_LONG2BE((LONG)img->NextImage);

		if (20 == FWrite(fd, block, 1, 20))
			success = TRUE;

		MyFreeVecPooled(block);
		}
	return success;
}
#endif

//----------------------------------------------------------------------------------------

#ifndef __amigaos4__
void exit(int x)
{
	(void) x;
	while (1)
		;
}
#endif /* __amigaos4__ */

//----------------------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2INITLIB(InitDatatype, 0);
ADD2EXPUNGELIB(CloseDatatype, 0);
ADD2OPENLIB(OpenDatatype, 0);

#endif

//----------------------------------------------------------------------------------------
