// AmigaIconObject.c
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
#include <datatypes/iconobject.h>

#define	__USE_SYSBASE
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/exec.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/wb.h>

#include <string.h>
#include <ctype.h>

#include <defs.h>
#include <Year.h>

#include "AmigaIconObject.h"

//----------------------------------------------------------------------------

#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))

#define	NO_ICON_POSITION_WORD	((WORD) 0x8000)

//----------------------------------------------------------------------------

struct argb
	{
	UBYTE	a;	// alpha
	UBYTE	r;	// red
	UBYTE	g;	// green
	UBYTE	b;	// blue
	};

//----------------------------------------------------------------------------

#ifndef __AROS__
struct Library *WorkbenchBase;
struct GfxBase *GfxBase;
T_UTILITYBASE UtilityBase;
struct DosLibrary *DOSBase;
struct ExecBase *SysBase;
struct Library *IconBase;
struct IntuitionBase *IntuitionBase;
#endif
struct Library *IconObjectDTBase;
#ifdef __amigaos4__
struct Library *NewlibBase;
struct Interface *INewlib;
struct WorkbenchIFace *IWorkbench;
struct GraphicsIFace *IGraphics;
struct UtilityIFace *IUtility;
struct ExecIFace *IExec;
struct IconIFace *IIcon;
struct DOSIFace *IDOS;
struct IntuitionIFace *IIntuition;
#endif /* __amigaos4__ */

static void *MemPool;
static struct SignalSemaphore PubMemPoolSemaphore;

static Class *AmigaIconObjectClass;

//----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT AmigaIconObjectDispatcher(Class *cl, Object *o, Msg msg);

//-----------------------------------------------------------------------------

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops);
static ULONG DtDispose(Class *cl, Object *o, Msg msg);
static ULONG DtSet(Class *cl, Object *o, struct opSet *ops);
static ULONG DtGet(Class *cl, Object *o, struct opGet *opg);
static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl);
static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *iopw);
static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *iopw);
static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio);
static void DoUpdateWb(struct InstanceData *inst, CONST_STRPTR SavePath);

//----------------------------------------------------------------------------

static void ExchangeAttrs(struct DiskObject *DiskObj, struct WriteData *wd);

//----------------------------------------------------------------------------

static APTR MyAllocVecPooled(size_t Size);
static void MyFreeVecPooled(APTR mem);

//----------------------------------------------------------------------------

char ALIGNED libName[] = "amigaiconobject.datatype";
char ALIGNED libIdString[] = "$VER: amigaiconobject.datatype "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (22 Feb 2006 19:54:14) "
	COMPILER_STRING
	" ©1999" CURRENTYEAR " The Scalos Team";

//----------------------------------------------------------------------------

#ifdef __AROS__
AROS_LH0(ULONG, ObtainInfoEngine,
    struct Library *, libBase, 5, Amigaiconobject
)
{
	AROS_LIBFUNC_INIT
	return (ULONG) AmigaIconObjectClass;
	AROS_LIBFUNC_EXIT
}
#else
LIBFUNC(ObtainInfoEngine, libbase, ULONG)
{
	(void) libbase;

	return (ULONG) AmigaIconObjectClass;
}
LIBFUNC_END
#endif

//-----------------------------------------------------------------------------

// return 0 if error occurred
ULONG InitDatatype(struct AmigaIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%ld:\n", __FUNC__, __LINE__));

	dtLib->nib_Initialized = FALSE;

	return 1;
}

// return 0 if error occurred
ULONG OpenDatatype(struct AmigaIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%ld:  OpenCnt=%ld\n", __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (!dtLib->nib_Initialized)
		{
		d1(kprintf("%s/%ld: \n", __FUNC__, __LINE__));

		dtLib->nib_Initialized = TRUE;

		InitSemaphore(&PubMemPoolSemaphore);

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
#endif /* __amigaos4__ */
		d1(KPrintF("%s/%ld: DOSBase=%08lx\n", __FUNC__, __LINE__, IntuitionBase));
		if (NULL == DOSBase)
			return 0;

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
#endif /* __amigaos4__ */
		d1(kprintf("%s/%ld: IntuitionBase=%08lx\n", __FUNC__, __LINE__, IntuitionBase));
		if (NULL == IntuitionBase)
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
#endif /* __amigaos4__ */
		d1(kprintf("%s/%ld: UtilityBase=%08lx\n", __FUNC__, __LINE__, UtilityBase));
		if (NULL == UtilityBase)
			return 0;

		IconBase = OpenLibrary(ICONNAME, 39);
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
#endif /* __amigaos4__ */
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
#endif /* __amigaos4__ */
		d1(kprintf("%s/%ld: GfxBase=%08lx\n", __FUNC__, __LINE__, GfxBase));
		if (NULL == GfxBase)
			return 0;

		WorkbenchBase = OpenLibrary("workbench.library", 39);
		d1(KPrintF("%s/%s/%ld: WorkbenchBase=%08lx\n", __FILE__, __FUNC__, __LINE__, WorkbenchBase));
		if (NULL == WorkbenchBase)
			return 0;
#ifdef __amigaos4__
		IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
		if (NULL == IWorkbench)
			return 0;
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			return 0;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			return 0;
#endif /* __amigaos4__ */

		IconObjectDTBase = OpenLibrary("datatypes/iconobject.datatype", 39);
		if (NULL == IconObjectDTBase)
			return 0;

		MemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		d1(kprintf("%s/%ld: MemPool=%08lx\n", __FUNC__, __LINE__, MemPool));
		if (NULL == MemPool)
			return 0;

		AmigaIconObjectClass = dtLib->nib_ClassLibrary.cl_Class = MakeClass(libName, 
			"iconobject.datatype", NULL, sizeof(struct InstanceData), 0);
		d1(kprintf("%s/%ld:  AmigaIconObjectClass=%08lx\n", __FUNC__, __LINE__, AmigaIconObjectClass));
		if (NULL == AmigaIconObjectClass)
			return 0;

		SETHOOKFUNC(AmigaIconObjectClass->cl_Dispatcher, AmigaIconObjectDispatcher);
		AmigaIconObjectClass->cl_Dispatcher.h_Data = dtLib;

		// Make class available for the public
		AddClass(AmigaIconObjectClass);
		}		

	d1(kprintf("%s/%ld:  Open Success!\n", __FUNC__, __LINE__));

	return 1;
}

ULONG CloseDatatype(struct AmigaIconObjectDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt < 1)
		{
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
		if (AmigaIconObjectClass)
			{
			RemoveClass(AmigaIconObjectClass);
			FreeClass(AmigaIconObjectClass);
			AmigaIconObjectClass = dtLib->nib_ClassLibrary.cl_Class = NULL;
			}

		if (NULL != IconObjectDTBase)
			{
			CloseLibrary(IconObjectDTBase);
			IconObjectDTBase = NULL;
			}
		if (NULL != IconBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IIcon);
#endif /* __amigaos4__ */
			CloseLibrary(IconBase);
			IconBase = NULL;
			}

		if (NULL != MemPool)
			{
			DeletePool(MemPool);
			MemPool = NULL;
			}

		if (NULL != GfxBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IGraphics);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) GfxBase);
			GfxBase = NULL;
			}
		if (NULL != DOSBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IDOS);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) DOSBase);
			DOSBase = NULL;
			}
#ifdef __amigaos4__
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
		if (NULL != IntuitionBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IIntuition);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) IntuitionBase);
			IntuitionBase = NULL;
			}
		if (NULL != UtilityBase)
			{
#ifdef __amigaos4__
			DropInterface((struct Interface *)IUtility);
#endif /* __amigaos4__ */
			CloseLibrary((struct Library *) UtilityBase);
			UtilityBase = NULL;
			}
		}
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));
	return 1;
}

//-----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT AmigaIconObjectDispatcher(Class *cl, Object *o, Msg msg)
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
		memset(inst, 0, sizeof(struct InstanceData));

		inst->aio_ImageLeft = GetTagData(IDTA_InnerLeft, 0, ops->ops_AttrList);
		inst->aio_ImageTop  = GetTagData(IDTA_InnerTop,  0, ops->ops_AttrList);

		if (FindTagItem(IDTA_CloneIconObject, ops->ops_AttrList))
			{
			}
		else
			{
			ULONG DefIconType;
			struct Image *img;

			DefIconType = GetTagData(IDTA_DefType,  0,  ops->ops_AttrList);
			d1(kprintf("%s/%ld:  o=%08lx  DefIconType=%lu\n", __FUNC__, __LINE__, o, DefIconType));

			if (0 != DefIconType)
				{
				// Get default icon
				inst->aio_DiskObject = GetDefDiskObject(DefIconType);
				d1(kprintf("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
				}
			else
				{
				STRPTR IconName;

				inst->aio_DiskObject = (struct DiskObject *) GetTagData(AIDTA_Icon, (ULONG) NULL, ops->ops_AttrList);
				d1(kprintf("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
				if (inst->aio_DiskObject)
					{
					inst->aio_DoNotFreeDiskObject = TRUE;
					}
				else
					{
					GetAttr(DTA_Name, o, (APTR) &IconName);
					d1(kprintf("%s/%ld:  o=%08lx  IconName=<%s>\n", __FUNC__, __LINE__, o, IconName ? IconName : (STRPTR) ""));

					// Get named icon
					if (NULL == IconName)
						break;

					inst->aio_DiskObject = GetDiskObject(IconName);
					d1(kprintf("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
					}
				}

			d1(KPrintF("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
			if (NULL == inst->aio_DiskObject)
				break;

			img = (struct Image *) inst->aio_DiskObject->do_Gadget.GadgetRender;

			if (NULL == img)
				break;

			SetAttrs(o,
				GA_Width, img->Width,
				GA_Height, img->Height,
				TAG_END);
			d1(kprintf("%s/%ld:  o=%08lx  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, o, gg->Width, gg->Height));

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

			if (WBDISK == inst->aio_DiskObject->do_Type
				|| WBDRAWER == inst->aio_DiskObject->do_Type)
				{
				if (NULL == inst->aio_DiskObject->do_DrawerData)
					{
					inst->aio_myDrawerData = MyAllocVecPooled(sizeof(struct DrawerData));
					if (NULL == inst->aio_myDrawerData)
						break;

					memset(inst->aio_myDrawerData, 0, sizeof(struct DrawerData));

					inst->aio_DiskObject->do_DrawerData = inst->aio_myDrawerData;
					}

				SetAttrs(o,
					IDTA_ViewModes, inst->aio_DiskObject->do_DrawerData->dd_ViewModes,
					IDTA_Flags, inst->aio_DiskObject->do_DrawerData->dd_Flags,
					IDTA_WinCurrentX, inst->aio_DiskObject->do_DrawerData->dd_CurrentX,
					IDTA_WinCurrentY, inst->aio_DiskObject->do_DrawerData->dd_CurrentY,
					IDTA_WindowRect, (ULONG) &inst->aio_DiskObject->do_DrawerData->dd_NewWindow,
					TAG_END);
				}

			SetAttrs(o,
				IDTA_ToolTypes, (ULONG) inst->aio_DiskObject->do_ToolTypes,
				IDTA_Stacksize, inst->aio_DiskObject->do_StackSize,
				IDTA_DefaultTool, (ULONG) inst->aio_DiskObject->do_DefaultTool,
				IDTA_Type, inst->aio_DiskObject->do_Type,
				TAG_END);
			}

		d1(kprintf("%s/%ld:  o=%08lx  GadgetRender=%08lx\n", __FUNC__, __LINE__, o, gg->GadgetRender));

		Success = TRUE;
		} while (0);

	d1(KPrintF("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
	d1(KPrintF("%s/%ld:  o=%08lx  Success=%ld\n", __FUNC__, __LINE__, o, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static ULONG DtDispose(Class *cl, Object *o, Msg msg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
//	struct ExtGadget *gg = (struct ExtGadget *) o;

	d1(KPrintF("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));
	d1(KPrintF("%s/%ld:  o=%08lx  aio_myDrawerData=%08lx\n", __FUNC__, __LINE__, o, inst->aio_myDrawerData));

	if (inst->aio_myDrawerData)
		{
		MyFreeVecPooled(inst->aio_myDrawerData);
		inst->aio_myDrawerData = NULL;
		if (inst->aio_DiskObject)
			inst->aio_DiskObject->do_DrawerData = NULL;
		}

	d1(KPrintF("%s/%ld:  o=%08lx  aio_DiskObject=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject));
	if (inst->aio_DiskObject)
		{
		if (!inst->aio_DoNotFreeDiskObject)
			FreeDiskObject(inst->aio_DiskObject);
		inst->aio_DiskObject = NULL;
		}

	d1(KPrintF("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------------------

static ULONG DtSet(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(kprintf("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	if (inst->aio_DoNotFreeDiskObject)
		{
		if (0 == GetTagData(IDTA_DoFreeDiskObject, 0, ops->ops_AttrList))
			inst->aio_DoNotFreeDiskObject = TRUE;
		}

	inst->aio_ImageLeft = GetTagData(IDTA_InnerLeft, inst->aio_ImageLeft, ops->ops_AttrList);
	inst->aio_ImageTop  = GetTagData(IDTA_InnerTop,  inst->aio_ImageTop,  ops->ops_AttrList);

	return DoSuperMethodA(cl, o, (Msg) ops);
}

//----------------------------------------------------------------------------------------

static ULONG DtGet(Class *cl, Object *o, struct opGet *opg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG result = 1;

	d1(KPrintF("%s/%ld:  o=%08lx  inst=%08lx  AttrID=%08lx\n", __FUNC__, __LINE__, o, inst, opg->opg_AttrID));

	switch (opg->opg_AttrID)
		{
	case IDTA_IconType:
		*opg->opg_Storage = ioICONTYPE_Standard;
		break;

	case IDTA_NumberOfColorsSupported:
		*opg->opg_Storage = 32;
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

	d1(kprintf("%s/%ld:  o=%08lx  GadgetRender=%08lx\n", __FUNC__, __LINE__, o, ((struct ExtGadget *) o)->GadgetRender));

	return result;
}

//--------------------------------------------- --------------------------------

static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *iopl)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct Image *NormImg = (struct Image *) inst->aio_DiskObject->do_Gadget.GadgetRender;
	struct Image *SelImg = (struct Image *) inst->aio_DiskObject->do_Gadget.SelectRender;

	d1(kprintf("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	DoSuperMethodA(cl, o, (Msg) iopl);

	d1(kprintf("%s/%ld:  o=%08lx  Screen=%08lx\n", __FUNC__, __LINE__, o, iopl->iopl_Screen));
	d1(kprintf("%s/%ld:  o=%08lx  Width=%ld  Height=%ld\n", __FUNC__, __LINE__, o, gg->Width, gg->Height));
	d1(kprintf("%s/%ld:  o=%08lx  GadgetRender=%08lx  SelectRender=%08lx  Flags=%08lx\n", \
		__FUNC__, __LINE__, o, gg->GadgetRender, gg->SelectRender, iopl->iopl_Flags));

	inst->aio_Screen = iopl->iopl_Screen;

	if (gg->GadgetRender && (iopl->iopl_Flags & IOLAYOUTF_NormalImage))
		{
		struct RastPort *rp = (struct RastPort *) gg->GadgetRender;

		DrawImage(rp, NormImg,  inst->aio_ImageLeft, inst->aio_ImageTop);
		}
	if (gg->SelectRender && (iopl->iopl_Flags & IOLAYOUTF_SelectedImage))
		{
		struct RastPort *rp = (struct RastPort *) gg->SelectRender;
		UWORD HighLight = inst->aio_DiskObject->do_Gadget.Flags & GFLG_GADGHIGHBITS;

		d1(kprintf("%s/%ld:  o=%08lx  Flags=%08lx\n", __FUNC__, __LINE__, o, inst->aio_DiskObject->do_Gadget.Flags));

		if (GFLG_GADGHIMAGE == HighLight)
			{
			d1(kprintf("%s/%ld:  o=%08lx  GFLG_GADGHIMAGE\n", __FUNC__, __LINE__, o));
			DrawImage(rp, SelImg, inst->aio_ImageLeft, inst->aio_ImageTop);
			}
		else if (GFLG_GADGHNONE == HighLight)
			{
			d1(kprintf("%s/%ld:  o=%08lx  GFLG_GADGHNONE\n", __FUNC__, __LINE__, o));
			DrawImage(rp, NormImg, inst->aio_ImageLeft, inst->aio_ImageTop);
			}
		else if (GFLG_GADGHCOMP == HighLight)
			{
			d1(kprintf("%s/%ld:  o=%08lx  ?\n", __FUNC__, __LINE__, o));

			DrawImage(rp, NormImg, inst->aio_ImageLeft, inst->aio_ImageTop);

			SetDrMd(rp, JAM1 | COMPLEMENT);

			BltPattern(rp, NULL, 
				inst->aio_ImageLeft, inst->aio_ImageTop,
				inst->aio_ImageLeft + gg->Width - 1,
				inst->aio_ImageTop + gg->Height - 1,
				0);
			WaitBlit();
			}
		else	// GFLG_GADGBACKFILL
			{
			//    GADGBACKFILL is similar to GADGHCOMP, but ensures that there is no
			//    "ring" around the selected image.  It does this by first
			//    complementing the image, and then flooding all color 3 pixels that
			//    are on the border of the image to color 0.  All other flag bits
			ULONG OldWriteMask;
			PLANEPTR BlitMask;

			DrawImage(rp, NormImg, inst->aio_ImageLeft, inst->aio_ImageTop);

			SetDrMd(rp, JAM1 | COMPLEMENT);
			GetRPAttrs(rp, 
				RPTAG_WriteMask, (ULONG) &OldWriteMask,
				TAG_END);
			if (SelImg)
				SetWriteMask(rp, SelImg->PlanePick);

			d1(kprintf("%s/%ld:  o=%08lx  GFLG_GADGHBOX\n", __FUNC__, __LINE__, o));

			do	{
				struct RastPort TempRp;
				struct TmpRas TempRas;
				UWORD *MaskPtr;
				UWORD *ImgSrcPtr;
				WORD ByteCount;
				ULONG ImgSize;
				ULONG d, y;

				ByteCount = (((NormImg->Width + 15) & 0xfff0) >> 3) + 4;
				ImgSize = (2 + NormImg->Height) * ByteCount * sizeof(UWORD);

				BlitMask = MyAllocVecPooled(ImgSize);
				d1(kprintf("%s/%ld:  BlitMask=%08lx\n", __FUNC__, __LINE__, BlitMask));
				if (NULL == BlitMask)
					break;

				InitRastPort(&TempRp);
				TempRp.TmpRas = &TempRas;

				InitTmpRas(&TempRas, BlitMask, ImgSize);

				TempRp.BitMap = AllocBitMap((NormImg->Width + 0x2f) & 0xfff0,
					NormImg->Height + 2, 2,
					BMF_STANDARD | BMF_CLEAR, NULL);
				d1(kprintf("%s/%ld:  TempRp.BitMap=%08lx\n", __FUNC__, __LINE__, TempRp.BitMap));
				if (NULL == TempRp.BitMap)
					break;

				ImgSrcPtr = NormImg->ImageData;
				for (d = 0; d < NormImg->Depth; d++)
					{
					if (NormImg->PlanePick & (1 << d))
						{
						MaskPtr = (UWORD *) (TempRp.BitMap->Planes[0] + (2 + ByteCount));

						for (y = 0; y < NormImg->Height; y++)
							{
							ULONG x;

							for (x = 0; x < (ByteCount >> 1) - 2; x++)
								{
								*MaskPtr++ |= *ImgSrcPtr++;
								}

							MaskPtr += 2;
							}
						}
					}

				SetAPen(&TempRp, 2);
				Flood(&TempRp, 1, 0, 0);
				WaitBlit();

				ImgSrcPtr = (UWORD *) (TempRp.BitMap->Planes[1] + (2 + ByteCount));
				MaskPtr = (UWORD *) BlitMask;
				for (y = 0; y < NormImg->Height; y++)
					{
					ULONG x;

					for (x = 0; x < (ByteCount >> 1) - 2; x++)
						{
						*MaskPtr++ = ~(*ImgSrcPtr++);
						}

					ImgSrcPtr += 2;
					}

				FreeBitMap(TempRp.BitMap);

				BltPattern(rp, BlitMask, 
					inst->aio_ImageLeft, inst->aio_ImageTop,
					inst->aio_ImageLeft + NormImg->Width - 1,
					inst->aio_ImageTop + NormImg->Height - 1,
					ByteCount - 4);
				WaitBlit();
				} while (0);

			if (BlitMask)
				MyFreeVecPooled(BlitMask);

			// Restore original WriteMask
			SetWriteMask(rp, OldWriteMask);
			}
		}

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *iopw)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct ExtGadget *gg = (struct ExtGadget *) o;
	struct WriteData wd;
	struct DrawerData *OrigDrawerData;
	ULONG NeedUpdateWB;
	LONG Result = RETURN_OK;

	d1(kprintf("%s/%ld:  o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	memset(&wd, 0, sizeof(wd));

	NeedUpdateWB = GetTagData(ICONA_NotifyWorkbench, FALSE, iopw->iopw_Tags);

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

	if (!PutDiskObject(iopw->iopw_Path, inst->aio_DiskObject))
		Result = IoErr();

	// Restore diskobject
	inst->aio_DiskObject->do_DrawerData = OrigDrawerData;
	ExchangeAttrs(inst->aio_DiskObject, &wd);

	if ((RETURN_OK == Result) && NeedUpdateWB)
		DoUpdateWb(inst, iopw->iopw_Path);

	return (ULONG) Result;
}

//-----------------------------------------------------------------------------

static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *ioni)
{
//	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%ld:  START o=%08lx\n", __FUNC__, __LINE__, o));

	d1(KPrintF("%s/%ld:  END o=%08lxlx\n", __FUNC__, __LINE__, o));

	return 0;
}

//-----------------------------------------------------------------------------

static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio)
{
	d1(struct InstanceData *inst = INST_DATA(cl, o);)

	d1(KPrintF("%s/%ld:  START o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	d1(KPrintF("%s/%ld:  END o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

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

			d1(kprintf("%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FUNC__, __LINE__, MemPool, Size, &sptr[1]));
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

static void DoUpdateWb(struct InstanceData *inst, CONST_STRPTR SavePath)
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

//-----------------------------------------------------------------------------
#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2INITLIB(InitDatatype, 0);
ADD2EXPUNGELIB(CloseDatatype, 0);
ADD2OPENLIB(OpenDatatype, 0);

#endif

//-----------------------------------------------------------------------------
