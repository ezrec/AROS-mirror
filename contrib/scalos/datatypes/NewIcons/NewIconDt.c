// NewIconDt.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/resident.h>
#include <intuition/classes.h>
#include <intuition/gadgetclass.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <utility/tagitem.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/newicon.h>
#include <scalos/preferences.h>

#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/preferences.h>
#include <proto/newicon.h>
#include <proto/cybergraphics.h>

#include <clib/alib_protos.h>

#include <datatypes/iconobject.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <defs.h>
#include <Year.h>

#include "NewIconDt.h"

//-----------------------------------------------------------------------------

#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))

//-----------------------------------------------------------------------------

void *NewMemPool;

Class *NewIconClass;

const ULONG InstanceSize = sizeof(struct InstanceData);

static struct SignalSemaphore NewMemPoolSemaphore;

struct ExecBase *SysBase;
struct NewIconBase *NewIconBase;
static struct Library *IconObjectDTBase;
T_UTILITYBASE UtilityBase;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library *PreferencesBase;
struct Library *CyberGfxBase;

//----------------------------------------------------------------------------
// Standard library functions

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Expungelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

static LIBFUNC_PROTO(ObtainInfoEngine, libbase, ULONG);

//----------------------------------------------------------------------------

static ULONG InitDatatype(struct NewIconDtLibBase *dtLib);
static ULONG OpenDatatype(struct NewIconDtLibBase *dtLib);
static void CloseDatatype(struct NewIconDtLibBase *dtLib);

static SAVEDS(ULONG) INTERRUPT NewIconDispatcher(Class *cl, Object *o, Msg msg);

static BOOL DtNew(Class *cl, Object *o, struct opSet *ops);
static ULONG DtDispose(Class *cl, Object *o, Msg msg);
static ULONG DtSet(Class *cl, Object *o, struct opSet *ops);
static ULONG DtGet(Class *cl, Object *o, struct opGet *opg);
static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *opl);
static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf);
static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *opw);
static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *iopw);
static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio);

static void DoFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf);
static void ReadPrefs(struct NewIconDtLibBase *dtLib);
static UBYTE **CloneToolTypes(const UBYTE **OrigToolTypes);
static UBYTE *GenerateMask(struct ExtChunkyImage *ChImg);
static void DtDrawImage(LONG LeftEdge, LONG TopEdge, struct RastPort *rp, struct Image *img);
static UBYTE *DrawCyberImage(struct InstanceData *inst, LONG LeftEdge, LONG TopEdge, 
	struct RastPort *rp, struct ChunkyImage *cki);
static void WriteARGBArray(const struct ARGB *SrcImgData,
	ULONG SrcX, ULONG SrcY, ULONG SrcBytesPerRow,
	struct RastPort *DestRp, ULONG DestX, ULONG DestY,
	ULONG SizeX, ULONG SizeY);
static struct ExtChunkyImage *ChunkyImageFromSAC(struct ScalosBitMapAndColor *sac);
static void FreeChunkyImage(struct ExtChunkyImage **ci);
static void GenerateMasks(Class *cl, Object *o);

static APTR MyAllocVecPooled(size_t Size);
static void MyFreeVecPooled(APTR mem);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

#define	LIB_VERSION	40
#define	LIB_REVISION	11

static char ALIGNED libName[] = "newiconobject.datatype";
static char ALIGNED libIdString[] = "$VER: newiconobject.datatype "
        STR(LIB_VERSION) "." STR(LIB_REVISION)
	" (21 Jan 2007 20:35:12) "
	COMPILER_STRING
	" ©1999" CURRENTYEAR " The Scalos Team";

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
#ifdef __MORPHOS__
	(APTR) FUNCARRAY_32BIT_NATIVE,
#endif
	Openlib,
	Closelib,
	Expungelib,
	Extfunclib,
	ObtainInfoEngine,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct NewIconDtLibBase),
	(ULONG) functable,
	0,
	(ULONG) Initlib
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
#ifdef __MORPHOS__
	RTF_PPC | RTF_AUTOINIT,
#else
	RTF_AUTOINIT,
#endif
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};

#ifdef __MORPHOS__
ULONG __abox__=1;
#endif

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase))
{
	struct NewIconDtLibBase *dtLib = (struct NewIconDtLibBase *) libbase;

	SysBase = sysbase;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 5;
	dtLib->nib_SegList = seglist;

	if (!InitDatatype(dtLib))
		{
		CALLLIBFUNC(Expungelib, &dtLib->nib_ClassLibrary.cl_Lib);
		dtLib = NULL;
		}

	return dtLib ? &dtLib->nib_ClassLibrary.cl_Lib : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct NewIconDtLibBase *dtLib = (struct NewIconDtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(dtLib))
		{
		CALLLIBFUNC(Closelib, &dtLib->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &dtLib->nib_ClassLibrary.cl_Lib;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct NewIconDtLibBase *dtLib = (struct NewIconDtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (dtLib->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &dtLib->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct NewIconDtLibBase *dtLib = (struct NewIconDtLibBase *) libbase;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		ULONG size = dtLib->nib_ClassLibrary.cl_Lib.lib_NegSize + dtLib->nib_ClassLibrary.cl_Lib.lib_PosSize;
		UBYTE *ptr = (UBYTE *) dtLib - dtLib->nib_ClassLibrary.cl_Lib.lib_NegSize;
		struct SegList *libseglist = dtLib->nib_SegList;

		Remove((struct Node *) dtLib);
		CloseDatatype(dtLib);
		FreeMem(ptr,size);

		return libseglist;
		}

	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;
	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));
	return 0;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

static LIBFUNC(ObtainInfoEngine, libbase, ULONG)
{
	(void) libbase;

	return (ULONG) NewIconClass;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

// return 0 if error occurred
static ULONG InitDatatype(struct NewIconDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	dtLib->nib_Initialized = FALSE;

	return 1;
}

// return 0 if error occurred
static ULONG OpenDatatype(struct NewIconDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (!dtLib->nib_Initialized)
		{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		dtLib->nib_Initialized = TRUE;

		InitSemaphore(&NewMemPoolSemaphore);

		IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
		if (NULL == IntuitionBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
		if (NULL == UtilityBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		GfxBase = (struct GfxBase *) OpenLibrary(GRAPHICSNAME, 39);
		if (NULL == GfxBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		NewIconBase = (struct NewIconBase *) OpenLibrary("newicon.library", 37);
		d1(kprintf("%s/%s/%ld: NewIconBase=%08lx\n", __FILE__, __FUNC__, __LINE__, NewIconBase));
		if (NULL == NewIconBase)
			return 0;

		CyberGfxBase = OpenLibrary(CYBERGFXNAME, 0);
		// CyberGfxBase may be NULL

		// we explicitely need to open our parent datatype - otherwise MakeClass() will fail!
		IconObjectDTBase = OpenLibrary("datatypes/amigaiconobject.datatype", 39);
		if (NULL == IconObjectDTBase)
			return 0;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		NewMemPool = CreatePool(MEMPOOL_MEMFLAGS, MEMPOOL_PUDDLESIZE, MEMPOOL_THRESHSIZE);
		if (NULL == NewMemPool)
			return 0;
		
		NewIconClass = dtLib->nib_ClassLibrary.cl_Class = MakeClass(libName, 
			"amigaiconobject.datatype", NULL, sizeof(struct InstanceData), 0);
		d1(kprintf("%s/%s/%ld:  NewIconClass=%08lx\n", __FILE__, __FUNC__, __LINE__, NewIconClass));
		if (NULL == NewIconClass)
			return 0;

		NewIconClass->cl_Dispatcher.h_Entry = (HOOKFUNC) HookEntry;
		NewIconClass->cl_Dispatcher.h_SubEntry = (HOOKFUNC) NewIconDispatcher;
		NewIconClass->cl_Dispatcher.h_Data = dtLib;

		// Make class available for the public
		AddClass(NewIconClass);

		ReadPrefs(dtLib);
		}		

	d1(kprintf("%s/%s/%ld:  Open Success!\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}


static void CloseDatatype(struct NewIconDtLibBase *dtLib)
{
	d1(kprintf("%s/%s/%ld:  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt));

	if (dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt < 1)
		{
		if (NewIconClass)
			{
			RemoveClass(NewIconClass);
			FreeClass(NewIconClass);
			NewIconClass = dtLib->nib_ClassLibrary.cl_Class = NULL;
			}

		if (NULL != NewMemPool)
			{
			DeletePool(NewMemPool);
			NewMemPool = NULL;
			}

		if (NULL != PreferencesBase)
			{
			CloseLibrary(PreferencesBase);
			PreferencesBase = NULL;
			}
		if (NULL != GfxBase)
			{
			CloseLibrary((struct Library *) GfxBase);
			GfxBase = NULL;
			}
		if (NULL != IconObjectDTBase)
			{
			CloseLibrary(IconObjectDTBase);
			IconObjectDTBase = NULL;
			}
		if (NULL != CyberGfxBase)
			{
			CloseLibrary(CyberGfxBase);
			CyberGfxBase = NULL;
			}
		if (NULL != NewIconBase)
			{
			CloseLibrary(&NewIconBase->nib_Lib);
			NewIconBase = NULL;
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
		}
}

//-----------------------------------------------------------------------------

void _XCEXIT(long x)
{
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) INTERRUPT NewIconDispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

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
		Result = DtDispose(cl, o, msg);
		break;

	case IDTM_Layout:
		Result = DtLayout(cl, o, (struct iopLayout *) msg);
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

	case OM_SET:
		Result = DtSet(cl, o, (struct opSet *) msg);
		break;

	case OM_GET:
		Result = DtGet(cl, o, (struct opGet *) msg);
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
//	struct ExtGadget *gg = (struct ExtGadget *) o;
	BOOL Success = FALSE;

	do	{
		ULONG SupportedIconTypes;
		ULONG DefIconType;

		memset(inst, 0, sizeof(struct InstanceData));

		SupportedIconTypes = GetTagData(IDTA_SupportedIconTypes, ~0, ops->ops_AttrList);
		if (!(SupportedIconTypes & IDTV_IconType_NewIcon))
			break;

		if (FindTagItem(IDTA_CloneIconObject, ops->ops_AttrList))
			{
			}
		else
			{
			DefIconType = GetTagData(IDTA_DefType, 0, ops->ops_AttrList);
			d1(KPrintF("%s/%s/%ld:  DefIconType=%ld\n", __FILE__, __FUNC__, __LINE__, DefIconType));
			if (DefIconType)
				{
				if (NewIconBase->nib_Lib.lib_Version < 40)
					break;

				inst->nio_newicon = GetDefNewDiskObject(DefIconType);
				if (NULL == inst->nio_newicon)
					break;
				}
			else
				{
				CONST_STRPTR IconName;

				IconName = (CONST_STRPTR) GetTagData(DTA_Name, NULL, ops->ops_AttrList);
				d1(KPrintF("%s/%s/%ld:  IconName=<%s>\n", __FILE__, __FUNC__, __LINE__, IconName ? IconName : (STRPTR) ""));
				if (NULL == IconName)
					break;

				inst->nio_newicon = GetNewDiskObject((STRPTR) IconName);
				if (NULL == inst->nio_newicon)
					break;
				}

			inst->nio_icon = inst->nio_newicon->ndo_StdObject;
			inst->nio_normchunky.eci_Chunky = inst->nio_newicon->ndo_NormalImage;
			inst->nio_selchunky.eci_Chunky = inst->nio_newicon->ndo_SelectedImage;
			inst->nio_normchunky.eci_TransparentColor = inst->nio_selchunky.eci_TransparentColor = 0;

			d1(KPrintF("%s/%s/%ld:  icon=%08lx  normchunky=%08lx  selchunky=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, inst->nio_icon, inst->nio_normchunky.eci_Chunky, inst->nio_selchunky.eci_Chunky));

			if (inst->nio_normchunky.eci_Chunky)
				{
				SetAttrs(o,
					GA_Width, inst->nio_normchunky.eci_Chunky->Width,
					GA_Height, inst->nio_normchunky.eci_Chunky->Height,
					IDTA_UnscaledWidth, inst->nio_normchunky.eci_Chunky->Width,
					IDTA_UnscaledHeight, inst->nio_normchunky.eci_Chunky->Height,
					TAG_END);

				d1(kprintf("%s/%s/%ld:  nio_normchunky.eci_Chunky->Flags=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, inst->nio_normchunky.eci_Chunky->Flags));

				GenerateMasks(cl, o);
				}
			}

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld:  o=%08lx  Success=%ld\n", __FILE__, __FUNC__, __LINE__, o, Success));

	return Success;
}

//-----------------------------------------------------------------------------

static ULONG DtDispose(Class *cl, Object *o, Msg msg)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	struct iopFreeLayout opf;

	d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
	d1(KPrintF("%s/%s/%ld:  o=%08lx  nio_DoNotFreeDiskObject=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->nio_DoNotFreeDiskObject));

	opf.MethodID = IDTM_FreeLayout;
	opf.iopf_Flags = IOFREELAYOUTF_ScreenAvailable;

	DoFreeLayout(cl, o, &opf);

	FreeChunkyImage(&inst->nio_AllocNormChunky);
	FreeChunkyImage(&inst->nio_AllocSelChunky);

	if (inst->nio_newicon)
		{
		FreeNewDiskObject(inst->nio_newicon);
		inst->nio_newicon = NULL;
		}
//	else
//		{
//		if (!inst->nio_DoNotFreeDiskObject)
//			newiconPrivate3(inst->nio_icon);
//		}

	return DoSuperMethodA(cl, o, msg);
}

//-----------------------------------------------------------------------------

static ULONG DtSet(Class *cl, Object *o, struct opSet *ops)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	inst->nio_DoNotFreeDiskObject = !GetTagData(IDTA_DoFreeDiskObject, 
		!inst->nio_DoNotFreeDiskObject, ops->ops_AttrList);

	d1(kprintf("%s/%s/%ld:  o=%08lx  nio_DoNotFreeDiskObject=%ld\n", \
		__FILE__, __FUNC__, __LINE__, o, inst->nio_DoNotFreeDiskObject));

	return DoSuperMethodA(cl, o, (Msg) ops);
}

//-----------------------------------------------------------------------------

static ULONG DtGet(Class *cl, Object *o, struct opGet *opg)
{
	ULONG result = 1;

	switch (opg->opg_AttrID)
		{
	case IDTA_IconType:
		*opg->opg_Storage = ioICONTYPE_NewIcon;
		break;

	case IDTA_NumberOfColorsSupported:
		*opg->opg_Storage = 256;
		break;

	default:
		result = DoSuperMethodA(cl, o, (Msg) opg);
		break;
		}

	return result;
}

//-----------------------------------------------------------------------------

static ULONG DtLayout(Class *cl, Object *o, struct iopLayout *opl)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG Result;

	d1(KPrintF("%s/%s/%ld:  START o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
	d1({ ULONG xxx; DoSuperMethod(cl, o, IDTA_Type, &xxx); });

	// DoSuperSuperMethodA()
	Result = DoSuperMethodA(cl->cl_Super, o, (Msg) opl);

	if (inst->nio_normchunky.eci_Chunky)
		{
		struct ExtGadget *gg = (struct ExtGadget *) o;
		LONG LeftEdge;
		LONG TopEdge;

		GetAttr(IDTA_InnerLeft, o, (ULONG *) &LeftEdge);
		GetAttr(IDTA_InnerTop, o, (ULONG *) &TopEdge);

		d1(KPrintF("%s/%s/%ld:  Left=%ld  Top=%ld  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, LeftEdge, TopEdge, opl->iopl_Flags));

		if (gg->GadgetRender && (opl->iopl_Flags & IOLAYOUTF_NormalImage))
			{
			struct RastPort *rp = (struct RastPort *) gg->GadgetRender;

			if (inst->nio_normimage)
				{
				DtDrawImage(LeftEdge, TopEdge, rp, inst->nio_normimage);
				}
			else
				{
				if (CyberGfxBase && 0 != GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
					{
					inst->nio_imgscreen = opl->iopl_Screen;
					inst->nio_normpens = DrawCyberImage(inst, LeftEdge, TopEdge, 
						rp, inst->nio_normchunky.eci_Chunky);
					}
				else
					{
					inst->nio_normimage = RemapChunkyImage(inst->nio_normchunky.eci_Chunky, opl->iopl_Screen);
					DtDrawImage(LeftEdge, TopEdge, rp, inst->nio_normimage);
					}
				}
			}
		if (gg->SelectRender && (IOLAYOUTF_SelectedImage & opl->iopl_Flags))
			{
			struct RastPort *rp = (struct RastPort *) gg->SelectRender;

			d1(KPrintF("%s/%s/%ld:  nio_selimage=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->nio_selimage));

			if (inst->nio_selimage)
				{
				DtDrawImage(LeftEdge, TopEdge, rp, inst->nio_selimage);
				}
			else
				{
				if (CyberGfxBase && 0 != GetCyberMapAttr(rp->BitMap, CYBRMATTR_ISCYBERGFX))
					{
					d1(KPrintF("%s/%s/%ld:  nio_selchunky.eci_Chunky=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->nio_selchunky.eci_Chunky));
					inst->nio_imgscreen = opl->iopl_Screen;
					inst->nio_selpens = DrawCyberImage(inst, LeftEdge, TopEdge, rp, 
						inst->nio_selchunky.eci_Chunky ? inst->nio_selchunky.eci_Chunky : inst->nio_normchunky.eci_Chunky);
					}
				else
					{
					inst->nio_selimage = RemapChunkyImage(inst->nio_selchunky.eci_Chunky ? inst->nio_selchunky.eci_Chunky : inst->nio_normchunky.eci_Chunky,
						 opl->iopl_Screen);
					DtDrawImage(LeftEdge, TopEdge, rp, inst->nio_selimage);
					}
				}
			}
		}
	else
		{
		Result = DoSuperMethodA(cl, o, (Msg) opl);
		}

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
	d1({ ULONG xxx; DoSuperMethod(cl, o, IDTA_Type, &xxx); });

	return Result;
}

//-----------------------------------------------------------------------------

static ULONG DtFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf)
{
	d1(KPrintF("%s/%s/%ld:  START o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
	d1({ ULONG xxx; DoSuperMethod(cl, o, IDTA_Type, &xxx); });

	DoFreeLayout(cl, o, opf);

	d1(KPrintF("%s/%s/%ld:  o=%08lx \n", __FILE__, __FUNC__, __LINE__, o));
	d1({ ULONG xxx; DoSuperMethod(cl, o, IDTA_Type, &xxx); });

	return DoSuperMethodA(cl, o, (Msg) opf);
}

//-----------------------------------------------------------------------------

static ULONG DtWrite(Class *cl, Object *o, struct iopWrite *opw)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	ULONG Result;

	d1(kprintf("%s/%s/%ld:  o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	if (inst->nio_normchunky.eci_Chunky)
		{
		struct NewDiskObject SaveNewDiskObj;
		UBYTE **ToolTypesClone;
		UBYTE **OrigToolTypes;
		const UBYTE **ToolTypes = NULL;

		SaveNewDiskObj.ndo_StdObject = inst->nio_newicon->ndo_StdObject;
		SaveNewDiskObj.ndo_NormalImage = inst->nio_normchunky.eci_Chunky;
		SaveNewDiskObj.ndo_SelectedImage = inst->nio_selchunky.eci_Chunky;

		GetAttr(IDTA_ToolTypes, o, (ULONG *) &ToolTypes);
		ToolTypesClone = CloneToolTypes(ToolTypes);

		OrigToolTypes = inst->nio_icon->do_ToolTypes;
		inst->nio_icon->do_ToolTypes = ToolTypesClone;

		SetAttrs(o, IDTA_ToolTypes, (ULONG) newiconPrivate2(&SaveNewDiskObj), TAG_END);

		Result = DoSuperMethodA(cl, o, (Msg) opw);

		// restore original ToolTypes
		inst->nio_icon->do_ToolTypes = OrigToolTypes;
		SetAttrs(o, IDTA_ToolTypes, (ULONG) ToolTypesClone, TAG_END);

		// free ToolType array clone
		MyFreeVecPooled(ToolTypesClone);
		}
	else
		{
		Result = DoSuperMethodA(cl, o, (Msg) opw);
		}

	return Result;
}

//-----------------------------------------------------------------------------

static ULONG DtNewImage(Class *cl, Object *o, struct iopNewImage *ioni)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	do	{
		struct ExtChunkyImage *ci;

		if (NULL == ioni->ioni_NormalImage)
			break;

		ci = ChunkyImageFromSAC(ioni->ioni_NormalImage);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__, ci));
		if (NULL == ci)
			break;

		if (inst->nio_normimage)
			{
			FreeRemappedImage(inst->nio_normimage, inst->nio_imgscreen);
			inst->nio_normimage = NULL;
			}
		if (inst->nio_normpens)
			{
			ULONG n;

			for (n = 0; n < inst->nio_normchunky.eci_Chunky->NumColors; n++)
				{
				ReleasePen(inst->nio_imgscreen->ViewPort.ColorMap, inst->nio_normpens[n]);
				}

			MyFreeVecPooled(inst->nio_normpens);
			inst->nio_normpens = NULL;
			}

		inst->nio_normchunky = *ci;
		FreeChunkyImage(&inst->nio_AllocNormChunky);
		inst->nio_AllocNormChunky = ci;

		if (ioni->ioni_SelectedImage)
			{
			ci = ChunkyImageFromSAC(ioni->ioni_SelectedImage);
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__, ci));
			if (NULL == ci)
				break;
			}
		else
			{
			ci = NULL;
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}

		if (inst->nio_selpens)
			{
			ULONG NumColors;
			ULONG n;

			if (inst->nio_selchunky.eci_Chunky)
				NumColors = inst->nio_selchunky.eci_Chunky->NumColors;
			else
				NumColors = inst->nio_normchunky.eci_Chunky->NumColors;

			for (n = 0; n < NumColors; n++)
				{
				ReleasePen(inst->nio_imgscreen->ViewPort.ColorMap, inst->nio_selpens[n]);
				}

			MyFreeVecPooled(inst->nio_selpens);
			inst->nio_selpens = NULL;
			}
		if (inst->nio_selimage)
			{
			FreeRemappedImage(inst->nio_selimage, inst->nio_imgscreen);
			inst->nio_selimage = NULL;
			}

		d1(KPrintF("%s/%s/%ld: ci=%08lx\n", __FILE__, __FUNC__, __LINE__, ci));
		if (ci)
			inst->nio_selchunky = *ci;
		else
			memset(&inst->nio_selchunky, 0, sizeof(inst->nio_selchunky));

		FreeChunkyImage(&inst->nio_AllocSelChunky);
		inst->nio_AllocSelChunky = ci;
		} while (0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SetAttrs(o,
		GA_Width, inst->nio_normchunky.eci_Chunky->Width,
		GA_Height, inst->nio_normchunky.eci_Chunky->Height,
		TAG_END);

	GenerateMasks(cl, o);

	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static void DoFreeLayout(Class *cl, Object *o, struct iopFreeLayout *opf)
{
	struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%s/%ld:  o=%08lx  inst=%08lx  nio_normchunky.eci_Chunky=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst, inst->nio_normchunky.eci_Chunky));

	if (inst->nio_normchunky.eci_Chunky)
		{
		struct Screen *RemapScreen;

		if (inst->nio_imgscreen && (opf->iopf_Flags & IOFREELAYOUTF_ScreenAvailable))
			RemapScreen = inst->nio_imgscreen;
		else
			RemapScreen = NULL;

		d1(kprintf("%s/%s/%ld:  normpens=%08lx\n", __FILE__, __FUNC__, __LINE__, inst->nio_normpens));

		if (inst->nio_normpens)
			{
			if (RemapScreen)
				{
				ULONG n;

				d1(kprintf("%s/%s/%ld:  normchunky.eci_Chunky=%08lx  NumColors=%ld\n", \
					__FILE__, __FUNC__, __LINE__, inst->nio_normchunky.eci_Chunky, inst->nio_normchunky.eci_Chunky->NumColors));

				for (n = 0; n < inst->nio_normchunky.eci_Chunky->NumColors; n++)
					{
					ReleasePen(inst->nio_imgscreen->ViewPort.ColorMap, inst->nio_normpens[n]);
					}
				}

			MyFreeVecPooled(inst->nio_normpens);
			inst->nio_normpens = NULL;
			}
		if (inst->nio_selpens)
			{
			if (RemapScreen)
				{
				ULONG NumColors;
				ULONG n;

				if (inst->nio_selchunky.eci_Chunky)
					NumColors = inst->nio_selchunky.eci_Chunky->NumColors;
				else
					NumColors = inst->nio_normchunky.eci_Chunky->NumColors;

				for (n = 0; n < NumColors; n++)
					{
					ReleasePen(inst->nio_imgscreen->ViewPort.ColorMap, inst->nio_selpens[n]);
					}
				}

			MyFreeVecPooled(inst->nio_selpens);
			inst->nio_selpens = NULL;
			}
		if (inst->nio_normimage)
			{
			FreeRemappedImage(inst->nio_normimage, RemapScreen);
			inst->nio_normimage = NULL;
			}
		if (inst->nio_selimage)
			{
			FreeRemappedImage(inst->nio_selimage, RemapScreen);
			inst->nio_selimage = NULL;
			}
		}
	d1(KPrintF("%s/%s/%ld:  END o=%08lx  inst=%08lx\n", __FILE__, __FUNC__, __LINE__, o, inst));
}

//-----------------------------------------------------------------------------

static ULONG DtClone(Class *cl, Object *o, struct iopCloneIconObject *iocio)
{
	//struct InstanceData *inst = INST_DATA(cl, o);

	d1(KPrintF("%s/%ld:  START o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	d1(KPrintF("%s/%ld:  END o=%08lx  inst=%08lx\n", __FUNC__, __LINE__, o, inst));

	return 0;
}

//-----------------------------------------------------------------------------

static void ReadPrefs(struct NewIconDtLibBase *dtLib)
{
	APTR PrefsHandle = NULL;

	// set default values for preferences settings
	dtLib->nib_NewIconsTransparent = TRUE;
	dtLib->nib_NewIconsPrecision = 4;

	do	{
		PreferencesBase = OpenLibrary("preferences.library", 39);
		if (NULL == PreferencesBase)
			break;

		PrefsHandle = AllocPrefsHandle("Scalos");
		if (NULL == PrefsHandle)
			break;
		ReadPrefsHandle(PrefsHandle, "ENV:Scalos/Scalos.prefs");

		GetPreferences(PrefsHandle, MAKE_ID('M','A','I','N'), SCP_NewIconsTransparent, 
			&dtLib->nib_NewIconsTransparent, sizeof(dtLib->nib_NewIconsTransparent));
		GetPreferences(PrefsHandle, MAKE_ID('M','A','I','N'), SCP_NewIconsPrecision, 
			&dtLib->nib_NewIconsPrecision, sizeof(dtLib->nib_NewIconsPrecision));

		d1(kprintf("%s/%s/%ld:  Transparent=%ld  Precision=%ld\n", \
			__FILE__, __FUNC__, __LINE__, dtLib->nib_NewIconsTransparent, dtLib->nib_NewIconsPrecision));
		} while (0);

	if (PrefsHandle)
		FreePrefsHandle(PrefsHandle);
	if (PreferencesBase)
		{
		CloseLibrary(PreferencesBase);
		PreferencesBase = NULL;
		}
}

//-----------------------------------------------------------------------------

static UBYTE **CloneToolTypes(const UBYTE **OrigToolTypes)
{
	const UBYTE **TTPtr;
	UBYTE **TTClone;
	size_t AllocLen = sizeof(UBYTE *);
	size_t StringCount = 1;

	d1(kprintf("%s/%s/%ld:  OrigToolTypes=%08lx\n", __FILE__, __FUNC__, __LINE__, OrigToolTypes));

	for (TTPtr = OrigToolTypes; *TTPtr; TTPtr++)
		{
		d1(kprintf("%s/%s/%ld:  *TTPtr=<%s>\n", __FILE__, __FUNC__, __LINE__, *TTPtr));
		StringCount++;
		AllocLen += sizeof(UBYTE *) + 1 + strlen(*TTPtr);
		}

	d1(kprintf("%s/%s/%ld:  StringCount=%lu  AllocLen=%lu\n", __FILE__, __FUNC__, __LINE__, StringCount, AllocLen));

	TTClone = MyAllocVecPooled(AllocLen);
	if (TTClone)
		{
		UBYTE **TTDestPtr;
		UBYTE *StringSpace;

		StringSpace = ((UBYTE *) TTClone) + StringCount * sizeof(UBYTE *);
		TTPtr = OrigToolTypes;
		TTDestPtr = TTClone;

		while (*TTPtr)
			{
			*TTDestPtr = StringSpace;
			strcpy(*TTDestPtr, *TTPtr);

			StringSpace += 1 + strlen(*TTPtr);
			TTDestPtr++;
			TTPtr++;
			}
		*TTDestPtr = NULL;
		}

	d1(kprintf("%s/%s/%ld:  TTClone=%08lx\n", __FILE__, __FUNC__, __LINE__, TTClone));

	return TTClone;
}

//-----------------------------------------------------------------------------

static UBYTE *GenerateMask(struct ExtChunkyImage *eci)
{
	ULONG Size;
	UBYTE *MaskArray;
	ULONG BytesPerRow;

	d1(KPrintF("%s/%s/%ld:  START  eci=%08lx\n", __FILE__, __FUNC__, __LINE__, eci));
	d1(KPrintF("%s/%s/%ld:  eci_Chunky=%08lx\n", __FILE__, __FUNC__, __LINE__, eci->eci_Chunky));

	BytesPerRow = ((eci->eci_Chunky->Width + 15) & 0xfff0) >> 3;
	Size = eci->eci_Chunky->Height * BytesPerRow;
	MaskArray = MyAllocVecPooled(Size);
	d1(KPrintF("%s/%s/%ld:  MaskArray=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskArray));
	if (MaskArray)
		{
		const UBYTE *ChunkyData = eci->eci_Chunky->ChunkyData;
		UBYTE *MaskPtr = MaskArray;
		ULONG y;

		d1(KPrintF("%s/%s/%ld:  ChunkyDat=%08lx\n", __FILE__, __FUNC__, __LINE__, eci->eci_Chunky->ChunkyData));
		memset(MaskArray, 0, Size);

		for (y = 0; y < eci->eci_Chunky->Height; y++)
			{
			ULONG x;
			UBYTE BitMask = 0x80;
			UBYTE *MaskPtr2 = MaskPtr;

			for (x = 0; x < eci->eci_Chunky->Width; x++)
				{
				if (*ChunkyData++ != eci->eci_TransparentColor)
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

	d1(KPrintF("%s/%s/%ld:  END  eci=%08lx  MaskArray=%08lx\n", __FILE__, __FUNC__, __LINE__, eci, MaskArray));

	return MaskArray;
}

//-----------------------------------------------------------------------------

static void DtDrawImage(LONG LeftEdge, LONG TopEdge, struct RastPort *rp, struct Image *img)
{
	DrawImage(rp, img, LeftEdge, TopEdge);
}

//-----------------------------------------------------------------------------

static UBYTE *DrawCyberImage(struct InstanceData *inst, LONG LeftEdge, LONG TopEdge, 
	struct RastPort *rp, struct ChunkyImage *cki)
{
	UBYTE *PenArray = NULL;
	ULONG BmDepth;
	ULONG ImgSize;
	ULONG n;
	UBYTE *PalettePtr;
	const UBYTE *ImgSrcPtr;
	ULONG BmWidth, BmHeight;
	ULONG Width, Height;

	BmWidth = GetBitMapAttr(rp->BitMap, BMA_WIDTH);
	BmHeight = GetBitMapAttr(rp->BitMap, BMA_HEIGHT);

	Width = min(cki->Width, BmWidth - LeftEdge);
	Height = min(cki->Height, BmHeight - TopEdge);

	ImgSize = cki->Width * cki->Height;

	BmDepth = GetBitMapAttr(rp->BitMap, BMA_DEPTH);

	d1(KPrintF("%s/%s/%ld:  inst=%08lx  Left=%ld  Top=%ld  Size=%lu  Depth=%ld\n", \
		__FILE__, __FUNC__, __LINE__, inst, LeftEdge, TopEdge, ImgSize, BmDepth));
	d1(kprintf("%s/%s/%ld:  Width=%lu  Height=%lu\n", __FILE__, __FUNC__, __LINE__, cki->Width, cki->Height));

	if (BmDepth > 8)
		{
		// True color
		struct ARGB *ARGBArray = NULL;
		struct ARGB *ARGBImgArray;
		struct ARGB *ARGBPtr;
		struct ARGB *ARGBImgDestPtr;

		do	{
			ARGBImgArray = MyAllocVecPooled(ImgSize * sizeof(struct ARGB));
			d1(kprintf("%s/%s/%ld:  ARGBImgArray=%08lx\n", __FILE__, __FUNC__, __LINE__, ARGBImgArray));
			if (NULL == ARGBImgArray)
				break;

			ARGBArray = MyAllocVecPooled(cki->NumColors * sizeof(struct ARGB));
			d1(kprintf("%s/%s/%ld:  ARGBArray=%08lx\n", __FILE__, __FUNC__, __LINE__, ARGBArray));
			if (NULL == ARGBArray)
				break;

			PalettePtr = cki->Palette;
			ARGBPtr = ARGBArray;
			for (n = 0; n < cki->NumColors; n++, ARGBPtr++)
				{
				ARGBPtr->Alpha = 0;
				ARGBPtr->Red   = *PalettePtr++;
				ARGBPtr->Green = *PalettePtr++;
				ARGBPtr->Blue  = *PalettePtr++;
				}

			ImgSrcPtr = cki->ChunkyData;
			ARGBImgDestPtr = ARGBImgArray;
			for (n = 0; n < ImgSize; n++)
				{
				*ARGBImgDestPtr++ = ARGBArray[*ImgSrcPtr++];
				}

			d1(kprintf("%s/%s/%ld:  ARGBImgArray=%08lx\n", __FILE__, __FUNC__, __LINE__, ARGBImgArray));

			WriteARGBArray(ARGBImgArray, 0, 0,
				cki->Width * sizeof(struct ARGB),
				rp, LeftEdge, TopEdge, 
				Width, Height);
			} while (0);

		if (ARGBArray)
			MyFreeVecPooled(ARGBArray);
		if (ARGBImgArray)
			MyFreeVecPooled(ARGBImgArray);
		}
	else
		{
		UBYTE *ImgArray;
		UBYTE *PenArrayPtr;
		UBYTE *ImgDestPtr;

		do	{
			ImgArray = MyAllocVecPooled(ImgSize);
			d1(kprintf("%s/%s/%ld:  ImgArray=%08lx\n", __FILE__, __FUNC__, __LINE__, ImgArray));
			if (NULL == ImgArray)
				break;

			PenArray = MyAllocVecPooled(cki->NumColors);
			if (NULL == PenArray)
				break;

			PalettePtr = cki->Palette;
			PenArrayPtr = PenArray;
			for (n = 0; n < cki->NumColors; n++, PalettePtr += 3)
				{
				*PenArrayPtr++ = ObtainBestPen(inst->nio_imgscreen->ViewPort.ColorMap,
					PalettePtr[0] << 24, 
					PalettePtr[1] << 24, 
					PalettePtr[2] << 24,
					OBP_Precision, NewIconBase->nib_Precision,
					TAG_END);
				}

			ImgSrcPtr = cki->ChunkyData;
			ImgDestPtr = ImgArray;
			for (n = 0; n < ImgSize; n++)
				{
				*ImgDestPtr++ = PenArray[*ImgSrcPtr++];
				}

			WritePixelArray(ImgArray, 0, 0, cki->Width,
				rp, LeftEdge, TopEdge, 
				Width, Height,
				RECTFMT_LUT8);
			} while (0);

		if (ImgArray)
			MyFreeVecPooled(ImgArray);
		}


	return PenArray;
}

//-----------------------------------------------------------------------------

static APTR MyAllocVecPooled(size_t Size)
{
	APTR ptr;

	if (NewMemPool)
		{
		ObtainSemaphore(&NewMemPoolSemaphore);
		ptr = AllocPooled(NewMemPool, Size + sizeof(size_t));
		ReleaseSemaphore(&NewMemPoolSemaphore);
		if (ptr)
			{
			size_t *sptr = (size_t *) ptr;

			sptr[0] = Size;

			d1(KPrintF("%s/%s/%ld:  MemPool=%08lx  Size=%lu  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, NewMemPool, Size, &sptr[1]));
			return (APTR)(&sptr[1]);
			}
		}

	d1(KPrintF("%s/%s/%ld:  MemPool=%08lx  Size=%lu\n", __FILE__, __FUNC__, __LINE__, NewMemPool, Size));

	return NULL;
}


static void MyFreeVecPooled(APTR mem)
{
	d1(KPrintF("%s/%s/%ld:  MemPool=%08lx  mem=%08lx\n", __FILE__, __FUNC__, __LINE__, NewMemPool, mem));
	if (NewMemPool && mem)
		{
		size_t size;
		size_t *sptr = (size_t *) mem;

		mem = &sptr[-1];
		size = sptr[-1];

		ObtainSemaphore(&NewMemPoolSemaphore);
		FreePooled(NewMemPool, mem, size + sizeof(size_t));
		ReleaseSemaphore(&NewMemPoolSemaphore);
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

	d1(KPrintF("%s/%s/%ld:  DestX=%ld  DestY=%ld  SizeX=%ld  SizeY=%ld\n", __FILE__, __FUNC__, __LINE__, DestX, DestY, SizeX, SizeY));
	d1(KPrintF("%s/%s/%ld:  SrcBytesPerRow=%ld\n", __FILE__, __FUNC__, __LINE__, SrcBytesPerRow, DestPixelFormat));

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

static struct ExtChunkyImage *ChunkyImageFromSAC(struct ScalosBitMapAndColor *sac)
{
	struct ExtChunkyImage *ci = NULL;
	struct BitMap *TempBM;
	BOOL Success = FALSE;

	do	{
		ULONG n;
		ULONG y;
		const ULONG *PaletteSrcPtr;
		UBYTE *PaletteDestPtr;
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
		ci = MyAllocVecPooled(sizeof(struct ExtChunkyImage) + sizeof(struct ChunkyImage));
		if (NULL == ci)
			break;

		memset(ci, 0, sizeof(struct ExtChunkyImage));
		ci->eci_Chunky = (struct ChunkyImage *) (ci + 1);
		memset(ci->eci_Chunky, 0, sizeof(struct ChunkyImage));

		if (SAC_TRANSPARENT_NONE != sac->sac_TransparentColor)
			ci->eci_Chunky->Flags |= CIF_COLOR_0_TRANSP;

		ci->eci_Chunky->Width = NewWidth;
		ci->eci_Chunky->Height = NewHeight;

		ci->eci_Chunky->ChunkyData = MyAllocVecPooled(PIXELARRAY8_BUFFERSIZE(ci->eci_Chunky->Width, ci->eci_Chunky->Height));
		if (NULL == ci->eci_Chunky->ChunkyData)
			break;

		ci->eci_Chunky->NumColors = sac->sac_NumColors;
		ci->eci_Chunky->Palette = MyAllocVecPooled(sac->sac_NumColors * 3);
		if (NULL == ci->eci_Chunky->Palette)
			break;

		ci->eci_TransparentColor = sac->sac_TransparentColor;

		// nothing can go wrong from here on
		Success = TRUE;

		d1(KPrintF("%s/%s/%ld: NumColors=%lu\n", __FILE__, __FUNC__, __LINE__, sac->sac_NumColors));

		// Fill nim_Palette fom sac_ColorTable
		for (n = 0, PaletteSrcPtr = sac->sac_ColorTable, PaletteDestPtr = ci->eci_Chunky->Palette;
			 n < sac->sac_NumColors; n++)
			{
			*PaletteDestPtr++ = *PaletteSrcPtr++ >> 24;	   // red
			*PaletteDestPtr++ = *PaletteSrcPtr++ >> 24;        // green
			*PaletteDestPtr++ = *PaletteSrcPtr++ >> 24;	   // blue
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// copy image data from sac_BitMap into nim_ImageData
		for (y = 0, ImagePtr = ci->eci_Chunky->ChunkyData; y < NewHeight; y++)
			{
			d1(KPrintF("%s/%s/%ld: y=%ld\n", __FILE__, __FUNC__, __LINE__, y));
			ReadPixelLine8(&rp, 0, y, NewWidth,
				ImagePtr, &TempRp);
			ImagePtr += NewWidth;
			}

		d1(KPrintF("%s/%s/%ld: ci=%08lx  eci_Chunky=%08lx\n", __FILE__, __FUNC__, __LINE__, ci, ci->eci_Chunky));
		} while (0);

	if (!Success)
		FreeChunkyImage(&ci);

	if (TempBM)
		FreeBitMap(TempBM);

	return ci;
}

//----------------------------------------------------------------------------------------

static void FreeChunkyImage(struct ExtChunkyImage **ci)
{
	if (*ci && (*ci)->eci_Chunky)
		{
		if ((*ci)->eci_Chunky->Palette)
			{
			MyFreeVecPooled((*ci)->eci_Chunky->Palette);
			(*ci)->eci_Chunky->Palette = NULL;
			}
		if ((*ci)->eci_Chunky->ChunkyData)
			{
			MyFreeVecPooled((*ci)->eci_Chunky->ChunkyData);
			(*ci)->eci_Chunky->ChunkyData = NULL;
			}
		MyFreeVecPooled(*ci);
		*ci = NULL;
		}
}

//----------------------------------------------------------------------------------------

static void GenerateMasks(Class *cl, Object *o)
{
	struct InstanceData *inst = INST_DATA(cl, o);
	UBYTE *MaskNormal = NULL;
	UBYTE *MaskSelected = NULL;
	UBYTE *AllocMaskNormal = NULL;
	UBYTE *AllocMaskSelected = NULL;

	d1(KPrintF("%s/%s/%ld: START  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));

	if (inst->nio_normchunky.eci_Chunky->Flags & CIF_COLOR_0_TRANSP)
		{
		d1(KPrintF("%s/%s/%ld: Generate Normal Mask\n", __FILE__, __FUNC__, __LINE__));
		MaskNormal = MaskSelected = AllocMaskNormal= GenerateMask(&inst->nio_normchunky);
		if (inst->nio_selchunky.eci_Chunky)
			{
			d1(KPrintF("%s/%s/%ld: Generate Selected Mask\n", __FILE__, __FUNC__, __LINE__));
			MaskSelected = AllocMaskSelected = GenerateMask(&inst->nio_selchunky);
			}
		}

	d1(KPrintF("%s/%s/%ld:  MaskNormal=%08lx  MaskSelected=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, MaskNormal, MaskSelected));

	SetAttrs(o, IDTA_Mask_Normal, (ULONG) MaskNormal,
		IDTA_Mask_Selected, (ULONG) MaskSelected,
		IDTA_Width_Mask_Normal, inst->nio_normchunky.eci_Chunky->Width,
		IDTA_Height_Mask_Normal, inst->nio_normchunky.eci_Chunky->Height,
		IDTA_Width_Mask_Selected, inst->nio_selchunky.eci_Chunky
			? inst->nio_selchunky.eci_Chunky->Width : inst->nio_normchunky.eci_Chunky->Width,
		IDTA_Height_Mask_Selected, inst->nio_selchunky.eci_Chunky
			? inst->nio_selchunky.eci_Chunky->Height : inst->nio_normchunky.eci_Chunky->Height,
		TAG_END);

	d1(KPrintF("%s/%s/%ld: MaskNormel=%08lx  MaskSelected=%08lx\n", __FILE__, __FUNC__, __LINE__, MaskNormal, MaskSelected));

	if (AllocMaskNormal)
		MyFreeVecPooled(AllocMaskNormal);
	if (AllocMaskSelected)
		MyFreeVecPooled(AllocMaskSelected);

	d1(KPrintF("%s/%s/%ld: END  o=%08lx\n", __FILE__, __FUNC__, __LINE__, o));
}

//----------------------------------------------------------------------------------------

