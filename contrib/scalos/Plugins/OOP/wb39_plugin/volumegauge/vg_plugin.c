// vg_plugin.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/layers.h>

#include <defs.h>

#include "volumegauge.h"


// Instance structure for overridden window.sca class
struct VGinst
	{
	Object *vgi_VolumeGaugeObject;
	};


// aus debug.lib
extern int kprintf(const char *fmt, ...);


static BOOL Init(void);
VOID closePlugin(void);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static void UpdateVolumeGauge(struct Gadget *gad, struct Window *win, BPTR iLock);


// Instance Size
const ULONG VGinstSize = sizeof(struct VGinst);


struct DosLibrary *DOSBase;
T_UTILITYBASE UtilityBase;
struct IntuitionBase *IntuitionBase;
struct Library *LayersBase;
struct Library *GfxBase;
struct Library *ScalosBase;
#ifdef __amigaos4__
struct Library *NewlibBase;

struct DOSIFace *IDOS;
struct UtilityIFace *IUtility;
struct IntuitionIFace *IIntuition;
struct LayersIFace *ILayers;
struct GraphicsIFace *IGraphics;
struct ScalosIFace *IScalos;
struct Interface *INewlib;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

static BOOL fInit;


M68KFUNC_P3(ULONG, myHookFunc,
	A0, Class *, cl,
	A2, Object *, obj,
	A1, Msg, msg)
{
	struct ScaRootList *rootList = (struct ScaRootList *) obj;
	struct ScaWindowStruct *sWin = NULL;
	ULONG Result;
	BOOL Scalos4014;

	d(kprintf(__FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));

	if (rootList && rootList->rl_WindowTask)
		sWin = rootList->rl_WindowTask->mt_WindowStruct;

	switch (msg->MethodID)
		{
	case OM_NEW:	   /* First, pass up to superclass */
		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		if (obj)
			{
			struct VGinst *inst = INST_DATA(cl, obj);

			inst->vgi_VolumeGaugeObject = NULL;

			Result = (ULONG) obj;
			}
		else
			Result = 0;
		break;

	case SCCM_Window_Open:
		Scalos4014 = ScalosBase->lib_Version >= 40 && ScalosBase->lib_Revision >= 14;

		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) 
			&& cl->cl_InstSize >= sizeof(struct VGinst)
			&& Scalos4014)
			{
			struct Gadget *gg;
			struct VGinst *inst = INST_DATA(cl, obj);

			inst->vgi_VolumeGaugeObject = NewObject(VOLUMEGAUGE_GetClass(), NULL, 
				GA_DrawInfo, (ULONG) rootList->rl_internInfos->ii_DrawInfo,
				GA_LeftBorder, TRUE,
				GA_RelSpecial, TRUE,
				VOLUMEGAUGE_Ticks, 5,
				VOLUMEGAUGE_ShortTicks, TRUE,
				VOLUMEGAUGE_TickSize, 3,
				VOLUMEGAUGE_Min, 0,
				VOLUMEGAUGE_Max, 1000,
				VOLUMEGAUGE_Level, 500,
				TAG_END);

			gg = (struct Gadget *) inst->vgi_VolumeGaugeObject;
			if (gg)
				{
				// insert volume gauge in front of other window gadgets
				DoMethod(sWin->ws_WindowTask->mt_MainObject, 
					SCCM_IconWin_AddGadget, gg);

				UpdateVolumeGauge(gg, NULL, sWin->ws_Lock);
				}
			}

		Result = DoSuperMethodA(cl, obj, msg);

		d(kprintf(__FUNC__ "/%ld: RootList=%08lx  InternInfos=%08lx\n", __LINE__, \
			rootList, rootList->rl_internInfos));

		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) 
			&& cl->cl_InstSize >= sizeof(struct VGinst)
			&& !Scalos4014)
			{
			struct Window *win = rootList->rl_WindowTask->wt_Window;

			if (win)
				{
				struct VGinst *inst = INST_DATA(cl, obj);
				struct Gadget *gad;

				if (win->BorderLeft < 14)
					win->BorderLeft = 14;
				if (win->BorderLeft < win->BorderRight)
					win->BorderLeft = win->BorderRight;

				inst->vgi_VolumeGaugeObject = NewObject(VOLUMEGAUGE_GetClass(), NULL, 
					GA_Top, win->BorderTop + 2,
					GA_Left, 4,
					GA_Width, win->BorderLeft - 4 - 4,
					GA_RelHeight, - (win->BorderTop + 2) - win->BorderBottom,
					GA_DrawInfo, (ULONG) rootList->rl_internInfos->ii_DrawInfo,
					GA_LeftBorder, TRUE,
					GA_RelSpecial, TRUE,
					VOLUMEGAUGE_Ticks, 5,
					VOLUMEGAUGE_ShortTicks, TRUE,
					VOLUMEGAUGE_TickSize, 3,
					VOLUMEGAUGE_Min, 0,
					VOLUMEGAUGE_Max, 1000,
					VOLUMEGAUGE_Level, 500,
					TAG_END);

				gad = (struct Gadget *) inst->vgi_VolumeGaugeObject;
				if (gad)
					{
					AddGadget(win, gad, ~0);
					RefreshGList(gad, win, NULL, 1);

					UpdateVolumeGauge(gad, win, sWin->ws_Lock);
					}
				RefreshWindowFrame(win);

				DoMethod(sWin->ws_WindowTask->mt_WindowObject, SCCM_Window_SetInnerSize);

				d1(kprintf(__FUNC__ "/%ld: Lock=%08lx\n", __LINE__, sWin->ws_Lock));
				}
			d1(kprintf(__FUNC__ "/%ld: Window=%08lx  bLeft=%ld  bRight=%ld  vgi_VolumeGaugeObject=%08lx\n", __LINE__, \
				win, win->BorderLeft, win->BorderRight, inst->vgi_VolumeGaugeObject));
			}
		break;

	case SCCM_Window_Close:
		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) && cl->cl_InstSize >= sizeof(struct VGinst))
			{
			struct VGinst *inst = INST_DATA(cl, obj);
			struct Window *win = rootList->rl_WindowTask->wt_Window;
			struct Gadget *gad = (struct Gadget *) inst->vgi_VolumeGaugeObject;

			d(kprintf(__FUNC__ "/%ld: Window=%08lx  gad=%08lx\n", __LINE__, \
				win, gad));

			if (gad)
				{
				RemoveGadget(win, gad);
				DisposeObject((Object *) gad);
				}
			}
		Result = DoSuperMethodA(cl, obj, msg);
		break;

	case SCCM_Window_SetTitle:
		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) && cl->cl_InstSize >= sizeof(struct VGinst))
			{
			struct VGinst *inst = INST_DATA(cl, obj);
			struct Window *win = rootList->rl_WindowTask->wt_Window;
			struct Gadget *gad = (struct Gadget *) inst->vgi_VolumeGaugeObject;

			d(kprintf(__FUNC__ "/%ld: Window=%08lx  gad=%08lx\n", __LINE__, \
				win, gad));

			if (gad)
				UpdateVolumeGauge(gad, win, sWin->ws_Lock);
			}
		Result = DoSuperMethodA(cl, obj, msg);
		break;

	default:
		d(kprintf(__FUNC__ "/%ld: MethodID=%08lx\n", __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
M68KFUNC_END


BOOL initPlugin(void)
{
	d(kprintf(__FUNC__ "/%ld: \n", __LINE__));

	if (!fInit)
		{
		BOOL Success = Init();

		if (!Success)
			closePlugin();

		return Success;
		}

	return TRUE;
}


static BOOL Init(void)
{
	if (!OpenLibraries())
		return FALSE;

	d(kprintf(__FUNC__ "/%ld: UtilityBase=%08lx\n", \
		__LINE__, UtilityBase));

	fInit = TRUE;

	return TRUE;
}


VOID closePlugin(void)
{
	CloseLibraries();

	fInit = FALSE;
}


static BOOL OpenLibraries(void)
{
	DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 39);
	if (NULL == DOSBase)
		return FALSE;
#ifdef __amigaos4__
	IDOS = (struct DOSIFace *)GetInterface((struct Library *)DOSBase, "main", 1, NULL);
	if (NULL == IDOS)
		return FALSE;
#endif

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
	if (NULL == UtilityBase)
		return FALSE;
#ifdef __amigaos4__
	IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) */

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	LayersBase = OpenLibrary("layers.library", 39);
	if (NULL == LayersBase)
		return FALSE;
#ifdef __amigaos4__
	ILayers = (struct LayersIFace *)GetInterface((struct Library *)LayersBase, "main", 1, NULL);
	if (NULL == ILayers)
		return FALSE;
#endif

	GfxBase = OpenLibrary("graphics.library", 39);
	if (NULL == GfxBase)
		return FALSE;
#ifdef __amigaos4__
	IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	if (NULL == IGraphics)
		return FALSE;

	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	ScalosBase = OpenLibrary(SCALOSNAME, 41);
	if (NULL == ScalosBase)
		return FALSE;
#ifdef __amigaos4__
	IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
	if (NULL == IScalos)
		return FALSE;
#endif

	return TRUE;
}


static void CloseLibraries(void)
{
#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif
	if (ScalosBase)
		{
		CloseLibrary(ScalosBase);
		ScalosBase = NULL;
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
	if (IGraphics)
		{
		DropInterface((struct Interface *)IGraphics);
		IGraphics = NULL;
		}
#endif
	if (GfxBase)
		{
		CloseLibrary(GfxBase);
		GfxBase = NULL;
		}
#ifdef __amigaos4__
	if (IDOS)
		{
		DropInterface((struct Interface *)IDOS);
		IDOS = NULL;
		}
#endif
	if (DOSBase)
		{
		CloseLibrary((struct Library *) DOSBase);
		DOSBase = NULL;
		}
#ifdef __amigaos4__
	if (IUtility)
		{
		DropInterface((struct Interface *)IUtility);
		IUtility = NULL;
		}
#endif
	if (UtilityBase)
		{
		CloseLibrary((struct Library *) UtilityBase);
		UtilityBase = NULL;
		}
#ifdef __amigaos4__
	if (IIntuition)
		{
		DropInterface((struct Interface *)IIntuition);
		IIntuition = NULL;
		}
#endif
	if (IntuitionBase)
		{
		CloseLibrary((struct Library *) IntuitionBase);
		IntuitionBase = NULL;
		}
#ifdef __amigaos4__
	if (ILayers)
		{
		DropInterface((struct Interface *)ILayers);
		ILayers = NULL;
		}
#endif
	if (LayersBase)
		{
		CloseLibrary(LayersBase);
		LayersBase = NULL;
		}
}


/* ====================================================== */

static void UpdateVolumeGauge(struct Gadget *gad, struct Window *win, BPTR iLock)
{
	struct InfoData *id = AllocVec(sizeof(struct InfoData), MEMF_PUBLIC);

	if (id)
		{
		Info(iLock, id);

		d(kprintf(__FUNC__ "/%ld: NumBlocks=%lu  NumBlocksUsed=%lu\n", __LINE__, \
			id->id_NumBlocks, id->id_NumBlocksUsed));

		SetGadgetAttrs(gad, win, NULL,
			VOLUMEGAUGE_Level, id->id_NumBlocksUsed,
			VOLUMEGAUGE_Max, id->id_NumBlocks,
			TAG_END);

		FreeVec(id);
		}
}


void _XCEXIT(long x)
{
}


