// wb39.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <exec/nodes.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <datatypes/iconobject.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>
#include <scalos/preferences.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/iffparse.h>

#include <clib/alib_protos.h>

#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/preferences.h>
#include <proto/wb.h>

#include <defs.h>
#ifdef __amigaos4__
#include <stdarg.h>
#endif

#include "wb39.h"
#include "wb39proto.h"
#include "volumegauge.h"

#ifdef __AROS__
#include "plugin.h"
#include "plugin-common.c"
#endif

// moved revision history to file "History"


/*

State of WB3.5/3.9 API emulation
================================

AddAppIconA				- partially supported :
	WBAPPICONA_SupportsOpen		- native support by Scalos ver >= 40
	WBAPPICONA_SupportsCopy		- native support by Scalos ver >= 40
	WBAPPICONA_SupportsRename	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsInformation	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsSnapshot	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsUnSnapshot	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsLeaveOut	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsPutAway	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsDelete	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsFormatDisk	- native support by Scalos ver >= 40
	WBAPPICONA_SupportsEmptyTrash	- native support by Scalos ver >= 40
	WBAPPICONA_PropagatePosition	- supported, only works with Scalos ver >= 40
	WBAPPICONA_RenderHook		- supported, only works with Scalos ver >= 40
	WBAPPICONA_NotifySelectState	- ./.
AddAppMenuItemA				- full native support by Scalos ver >= 40
AddAppWindowA				- supported
AddAppWindowDropZoneA			- supported
ChangeWorkbenchSelectionA		- supported
CloseWorkbenchObjectA			- supported
MakeWorkbenchObjectVisibleA		- supported
OpenWorkbenchObjectA			- supported
RemoveAppIcon				- partially supported
RemoveAppMenuItem			- ./. unchanged
RemoveAppWindow				- supported
RemoveAppWindowDropZone			- supported
WBInfo					- ./. unchanged
WorkbenchControlA			- fully supported :
	WBCTRLA_IsOpen	 		- supported
	WBCTRLA_DuplicateSearchPath 	- supported
	WBCTRLA_FreeSearchPath		- supported
	WBCTRLA_GetDefaultStackSize 	- supported with Scalos ver >= 40
	WBCTRLA_SetDefaultStackSize	- supported with Scalos ver >= 40
	WBCTRLA_RedrawAppIcon		- does Icon Redraw, only works with Scalos ver >= 40
	WBCTRLA_GetProgramList		- supported with Scalos ver >= 40
	WBCTRLA_FreeProgramList		- supported
	WBCTRLA_GetSelectedIconList	- supported
	WBCTRLA_FreeSelectedIconList	- supported
	WBCTRLA_GetOpenDrawerList	- supported
	WBCTRLA_FreeOpenDrawerList	- supported
	WBCTRLA_AddHiddenDeviceName	- supported
	WBCTRLA_RemoveHiddenDeviceName	- supported
	WBCTRLA_GetHiddenDeviceList	- supported
	WBCTRLA_FreeHiddenDeviceList	- supported
	WBCTRLA_GetTypeRestartTime	- supported, with Scalos ver >= 40
	WBCTRLA_SetTypeRestartTime	- supported, with Scalos ver >= 40
*/

// aus mempools.lib
#if !defined(__amigaos4__) && !defined(__AROS__)
extern int _STI_240_InitMemFunctions(void);
extern void _STD_240_TerminateMemFunctions(void);
#endif

static BOOL LateInit(void);
static BOOL OpenLibraries(void);
static void CloseLibraries(void);
static LIBFUNC_P3_PROTO(BOOL, myOpenWorkbenchObjectA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0);
static struct WBArg *BuildWBArg(struct TagItem *Tags, ULONG *ArgCount);
static void FreeWBArg(struct WBArg *ArgList, ULONG ArgCount, BOOL FreeLocks);
static LIBFUNC_P3_PROTO(BOOL, myCloseWorkbenchObjectA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0);
static LIBFUNC_P3_PROTO(ULONG, myWbprivate1,
	A0, STRPTR, Name,
	A1, BPTR, iLock,
	A6, struct Library *, WorkbenchBase, 0);
static LIBFUNC_P3_PROTO(BOOL, myMakeWorkbenchObjectVisibleA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0);
static void UpdateVolumeGauge(struct Gadget *gad, struct Window *win, BPTR iLock);
static LIBFUNC_P4_PROTO(BOOL, myChangeWorkbenchSelectionA,
	A0, STRPTR, name,
	A1, struct Hook *, hook,
	A2, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0);
static void FreeAllNodes(struct List *pl);      // +dm+ 20010518

#ifdef __amigaos4__
static LIBFUNC_P2VA_PROTO(BOOL, myOpenWorkbenchObject,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase);
static LIBFUNC_P2VA_PROTO(BOOL, myCloseWorkbenchObject,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase);
static LIBFUNC_P2VA_PROTO(BOOL, myMakeWorkbenchObjectVisible,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase);
static LIBFUNC_P3VA_PROTO(BOOL, myChangeWorkbenchSelection,
	A0, STRPTR, name,
	A1, struct Hook *, hook,
	A6, struct Library *, WorkbenchBase);
#endif

#ifndef __AROS__
struct DosLibrary *DOSBase = NULL;
struct Library *WorkbenchBase = NULL;
T_UTILITYBASE UtilityBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *LayersBase = NULL;
struct Library *IconBase = NULL;
struct Library *IFFParseBase = NULL;
#endif
struct Library *IconobjectBase = NULL;
struct ScalosBase *ScalosBase = NULL;
struct Library *GfxBase = NULL;

#ifdef __amigaos4__
struct Library *NewlibBase = NULL;

struct DOSIFace *IDOS = NULL;
struct IconobjectIFace *IIconobject = NULL;
struct WorkbenchIFace *IWorkbench = NULL;
struct UtilityIFace *IUtility = NULL;
struct ScalosIFace *IScalos = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct LayersIFace *ILayers = NULL;
struct GraphicsIFace *IGraphics = NULL;
struct IconIFace *IIcon = NULL;
struct IFFParseIFace *IIFFParse = NULL;
struct Interface *INewlib = NULL;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
extern T_UTILITYBASE __UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

APTR origOpenWorkbenchObjectA = NULL;
APTR origCloseWorkbenchObjectA = NULL;
APTR origAddAppWindowDropZoneA = NULL;
APTR origRemoveAppWindowDropZone = NULL;
APTR origWbprivate1 = NULL;
APTR origMakeWorkbenchObjectVisibleA = NULL;
APTR origChangeWorkbenchSelectionA = NULL;


/*LIBFUNC_P6_DPROTO(struct ScaAppObjNode *, (*origAddAppWindowA),
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, struct Window *, window,
	A1, struct MsgPort *, msgport,
	A2, struct TagItem *, taglist,
	A6, struct Library *, WorkbenchBase);
LIBFUNC_P2_DPROTO(BOOL, (*origRemoveAppWindow),
	A0, struct ScaAppObjNode *, appWindow,
	A6, struct Library *, WorkbenchBase);
LIBFUNC_P5_DPROTO(void, (*origSCA_DrawDrag),
	A0, struct DragHandle *, draghandle,
	D0, LONG, X,
	D1, LONG, Y,
	D2, LONG, Flags,
	A6, struct Library *, ScalosBase);
static LIBFUNC_P3_DPROTO(BOOL, (*origWorkbenchControlA),
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase);
*/
APTR origAddAppWindowA = NULL;
APTR origRemoveAppWindow = NULL;
APTR origSCA_DrawDrag = NULL;
APTR origWorkbenchControlA = NULL;

#ifdef __amigaos4__
APTR origOpenWorkbenchObject = NULL;
APTR origCloseWorkbenchObject = NULL;
APTR origAddAppWindowDropZone = NULL;
APTR origMakeWorkbenchObjectVisible = NULL;
APTR origChangeWorkbenchSelection = NULL;
APTR origAddAppWindow = NULL;
APTR origWorkbenchControl = NULL;
#endif

static BOOL fInit = FALSE;

STRPTR DefIconsPath = "ENV:sys/def_";

struct List AppWindowList;		// list of all AppWindows
struct SignalSemaphore AppWindowSema;

ULONG DefaultStackSize = 4096;		// +dm+ 20010518 changed to 4096 (was 4095???)
ULONG fVolumeGauge = TRUE;
ULONG fTitleBar = TRUE;
ULONG TypeRestartTime = 3;		// Keyboard restart delay in s
BOOL NewIconsSupport = TRUE;
BOOL ColorIconsSupport = TRUE;


WORD ScalosPrefs_IconOffsets[4] = { 1, 1, 1, 1 };

struct ScaRootList *rList = NULL;

struct List HiddenDeviceList;   		// +dm+ 20010518 for storing names of hidden devices
struct SignalSemaphore HiddenDevListSema;	// +jl+ 20010523 Semaphore to protect HiddenDeviceList


M68KFUNC_P3(ULONG, myHookFunc,
	A0, Class *, cl,
	A2, Object *, obj,
	A1, Msg, msg)
{
	struct ScaRootList *rootList;
	struct ScaWindowStruct *sWin = NULL;
	ULONG Result;
	BOOL Scalos4014;

	rList = rootList = (struct ScaRootList *) obj;

	d1(kprintf("%s/%s/%ld: MethodID=%08lx\n"__FILE__, __FUNC__, __LINE__, msg->MethodID));

	if (rootList && rootList->rl_WindowTask)
		sWin = rootList->rl_WindowTask->mt_WindowStruct;

	switch (msg->MethodID)
		{
	case OM_NEW:	   /* First, pass up to superclass */
		LateInit();

		obj = (Object *) DoSuperMethodA(cl, obj, msg);
		if (obj)
			{
			struct WB39inst *inst = INST_DATA(cl, obj);

			inst->wb39i_VolumeGaugeObject = NULL;

			Result = (ULONG) obj;
			}
		else
			Result = 0;
		break;

	case SCCM_Window_Open:
		d1(kprintf("%s/%s/%ld: instOffset=%lu  instSize=%lu\n"__FILE__, __FUNC__, __LINE__, cl->cl_InstOffset, cl->cl_InstSize));

		Scalos4014 = ScalosBase->scb_LibNode.lib_Version >= 40 && ScalosBase->scb_LibNode.lib_Revision >= 14;

		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) 
			&& cl->cl_InstSize >= sizeof(struct WB39inst))
			{
			if (Scalos4014 && fVolumeGauge)
				{
				struct Gadget *gg;
				struct WB39inst *inst = INST_DATA(cl, obj);

				inst->wb39i_VolumeGaugeObject = NewObject(VOLUMEGAUGE_GetClass(), NULL, 
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

				gg = (struct Gadget *) inst->wb39i_VolumeGaugeObject;
				if (gg)
					{
					// insert volume gauge in front of other window gadgets
					DoMethod(sWin->ws_WindowTask->mt_MainObject, 
						SCCM_IconWin_AddGadget, gg);

					UpdateVolumeGauge(gg, NULL, sWin->ws_Lock);
					}
				}
			}

		Result = DoSuperMethodA(cl, obj, msg);

		d1(kprintf("%s/%s/%ld: RootList=%08lx  InternInfos=%08lx\n"__FILE__, __FUNC__, __LINE__, \
			rootList, rootList->rl_internInfos));

		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) 
			&& cl->cl_InstSize >= sizeof(struct WB39inst)
			&& !Scalos4014)
			{
			struct Window *win = rootList->rl_WindowTask->wt_Window;

			if (win && fVolumeGauge)
				{
				struct WB39inst *inst = INST_DATA(cl, obj);
				struct Gadget *gad;

				if (win->BorderLeft < 14)
					win->BorderLeft = 14;
				if (win->BorderLeft < win->BorderRight)
					win->BorderLeft = win->BorderRight;

				inst->wb39i_VolumeGaugeObject = NewObject(VOLUMEGAUGE_GetClass(), NULL, 
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

				gad = (struct Gadget *) inst->wb39i_VolumeGaugeObject;
				if (gad)
					{
					AddGadget(win, gad, ~0);
					RefreshGList(gad, win, NULL, 1);

					UpdateVolumeGauge(gad, win, sWin->ws_Lock);
					}
				RefreshWindowFrame(win);

				DoMethod(sWin->ws_WindowTask->mt_WindowObject, SCCM_Window_SetInnerSize);

				d1(kprintf("%s/%s/%ld: Lock=%08lx\n"__FILE__, __FUNC__, __LINE__, sWin->ws_Lock));
				}
			d1(KPrintF("%s/%s/%ld: Window=%08lx  bLeft=%ld  bRight=%ld  wb39i_VolumeGaugeObject=%08lx\n"__FILE__, __FUNC__, __LINE__, \
				win, win->BorderLeft, win->BorderRight, inst->wb39i_VolumeGaugeObject));
			}
		break;

	case SCCM_Window_Close:
		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) && cl->cl_InstSize >= sizeof(struct WB39inst))
			{
			struct WB39inst *inst = INST_DATA(cl, obj);
			struct Window *win = rootList->rl_WindowTask->wt_Window;
			struct Gadget *gad = (struct Gadget *) inst->wb39i_VolumeGaugeObject;

			d1(kprintf("%s/%s/%ld: Window=%08lx  gad=%08lx\n"__FILE__, __FUNC__, __LINE__, \
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
		if (sWin && (sWin->ws_Flags & WSV_FlagF_RootWindow) && cl->cl_InstSize >= sizeof(struct WB39inst))
			{
			struct WB39inst *inst = INST_DATA(cl, obj);
			struct Window *win = rootList->rl_WindowTask->wt_Window;
			struct Gadget *gad = (struct Gadget *) inst->wb39i_VolumeGaugeObject;

			d1(kprintf("%s/%s/%ld: Window=%08lx  gad=%08lx\n"__FILE__, __FUNC__, __LINE__, \
				win, gad));

			if (gad)
				UpdateVolumeGauge(gad, win, sWin->ws_Lock);
			}
		Result = DoSuperMethodA(cl, obj, msg);
		break;

	default:
		d1(kprintf("%s/%s/%ld: MethodID=%08lx\n"__FILE__, __FUNC__, __LINE__, msg->MethodID));
		Result = DoSuperMethodA(cl, obj, msg);
		break;
		}

	return Result;
}
M68KFUNC_END


static BOOL LateInit(void)
{
	static BOOL First = TRUE;

	d1(KPrintF("/%s/%ld: First=%ld\n", __FUNC__, __FILE__, __FUNC__, __LINE__, First));

	if (First)
		{
		ULONG EmulationMode = TRUE;

		rList = GetScalosRootList();
		d1(kprintf("%s/%s/%ld: ScaRootList=%08lx\n"__FILE__, __FUNC__, __LINE__, rList));

		{
		// Set Flag "Workbench open"
		// bset	#5,($2d,a6)
		UBYTE * wbb = (UBYTE *) WorkbenchBase;

		wbb[0x2d] |= 0x20;
		}

		if (ScalosBase->scb_LibNode.lib_Version >= 40)
			{
			SCA_ScalosControl(NULL,
				SCALOSCTRLA_GetEmulationMode, (ULONG) &EmulationMode,
				TAG_END);
			}

		NewList(&AppWindowList);
		NewList(&HiddenDeviceList);     // +dm+ 20010518

		InitSemaphore(&AppWindowSema);
		InitSemaphore(&HiddenDevListSema);	// +jl+ 20010523

		if (EmulationMode)
			{
#ifndef __amigaos4__
			STATIC_PATCHFUNC(myOpenWorkbenchObjectA)
			STATIC_PATCHFUNC(myCloseWorkbenchObjectA)
			STATIC_PATCHFUNC(myMakeWorkbenchObjectVisibleA)
			STATIC_PATCHFUNC(myChangeWorkbenchSelectionA)

			STATIC_PATCHFUNC(myAddAppWindowDropZoneA)
			STATIC_PATCHFUNC(myRemoveAppWindowDropZone)
			STATIC_PATCHFUNC(myAddAppWindowA)
			STATIC_PATCHFUNC(myRemoveAppWindow)

			STATIC_PATCHFUNC(myWorkbenchControlA)

			STATIC_PATCHFUNC(myWbprivate1)
			STATIC_PATCHFUNC(mySCA_DrawDrag)
#endif

			if (!AppIconsInit())
				return FALSE;

#if defined(__amigaos4__)
			origOpenWorkbenchObjectA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, OpenWorkbenchObjectA),
				(APTR)myOpenWorkbenchObjectA);
			origOpenWorkbenchObject = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, OpenWorkbenchObject),
				(APTR)myOpenWorkbenchObject);
			origCloseWorkbenchObjectA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, CloseWorkbenchObjectA),
				(APTR)myCloseWorkbenchObjectA);
			origCloseWorkbenchObject = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, CloseWorkbenchObject),
				(APTR)myCloseWorkbenchObject);
			origMakeWorkbenchObjectVisibleA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, MakeWorkbenchObjectVisibleA),
				(APTR)myMakeWorkbenchObjectVisibleA);
			origMakeWorkbenchObjectVisible = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, MakeWorkbenchObjectVisible),
				(APTR)myMakeWorkbenchObjectVisible);
			origChangeWorkbenchSelectionA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, ChangeWorkbenchSelectionA),
				(APTR)myChangeWorkbenchSelectionA);
			origChangeWorkbenchSelection = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, ChangeWorkbenchSelection),
				(APTR)myChangeWorkbenchSelection);

			origAddAppWindowDropZoneA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, AddAppWindowDropZoneA),
				(APTR)myAddAppWindowDropZoneA);
			origAddAppWindowDropZone = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, AddAppWindowDropZone),
				(APTR)myAddAppWindowDropZone);
			origRemoveAppWindowDropZone = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, RemoveAppWindowDropZone),
				(APTR)myRemoveAppWindowDropZone);
			origAddAppWindowA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, AddAppWindowA),
				(APTR)myAddAppWindowA);
			origAddAppWindow = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, AddAppWindow),
				(APTR)myAddAppWindow);
			origRemoveAppWindow = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, RemoveAppWindow),
				(APTR)myRemoveAppWindow);

			origWorkbenchControlA = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, WorkbenchControlA),
				(APTR)myWorkbenchControlA);
			origWorkbenchControl = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, WorkbenchControl),
				(APTR)myWorkbenchControl);

			origWbprivate1 = SetMethod((struct Interface *)IWorkbench,
				offsetof(struct WorkbenchIFace, Private1),
				(APTR)myWbprivate1);

			origSCA_DrawDrag = SetMethod((struct Interface *)IScalos,
				offsetof(struct ScalosIFace, SCA_DrawDrag),
				(APTR)mySCA_DrawDrag);
#elif defined(__AROS__)
			origOpenWorkbenchObjectA = SetFunction(WorkbenchBase,
				-16 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myOpenWorkbenchObjectA, WorkbenchBase, 0));
			origCloseWorkbenchObjectA = SetFunction(WorkbenchBase,
				-17 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myCloseWorkbenchObjectA, WorkbenchBase, 0));
			origMakeWorkbenchObjectVisibleA = SetFunction(WorkbenchBase,
				-22 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myMakeWorkbenchObjectVisibleA, WorkbenchBase, 0));
			origChangeWorkbenchSelectionA = SetFunction(WorkbenchBase,
				-21 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myChangeWorkbenchSelectionA, WorkbenchBase, 0));

			origAddAppWindowDropZoneA = SetFunction(WorkbenchBase,
				-19 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myAddAppWindowDropZoneA, WorkbenchBase, 0));
			origRemoveAppWindowDropZone = SetFunction(WorkbenchBase,
				-20 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myRemoveAppWindowDropZone, WorkbenchBase, 0));
			origAddAppWindowA = SetFunction(WorkbenchBase,
				-8 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myAddAppWindowA, WorkbenchBase, 0));
			origRemoveAppWindow = SetFunction(WorkbenchBase,
				-9 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myRemoveAppWindow, WorkbenchBase, 0));

			origWorkbenchControlA = SetFunction(WorkbenchBase,
				-18 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myWorkbenchControlA, WorkbenchBase, 0));

			origWbprivate1 = SetFunction(WorkbenchBase,
				-5 * LIB_VECTSIZE, AROS_SLIB_ENTRY(myWbprivate1, WorkbenchBase, 0));

			origSCA_DrawDrag = SetFunction(&ScalosBase->scb_LibNode,
				-25 * LIB_VECTSIZE, AROS_SLIB_ENTRY(mySCA_DrawDrag, ScalosBase, 0));
#else
			origOpenWorkbenchObjectA = SetFunction(WorkbenchBase,
				-96, PATCH_NEWFUNC(myOpenWorkbenchObjectA));
			origCloseWorkbenchObjectA = SetFunction(WorkbenchBase,
				-102, PATCH_NEWFUNC(myCloseWorkbenchObjectA));
			origMakeWorkbenchObjectVisibleA = SetFunction(WorkbenchBase,
				 -132, PATCH_NEWFUNC(myMakeWorkbenchObjectVisibleA));
			origChangeWorkbenchSelectionA = SetFunction(WorkbenchBase,
				-126, PATCH_NEWFUNC(myChangeWorkbenchSelectionA));

			origAddAppWindowDropZoneA = SetFunction(WorkbenchBase,
				-114, PATCH_NEWFUNC(myAddAppWindowDropZoneA));
			origRemoveAppWindowDropZone = SetFunction(WorkbenchBase,
				-120, PATCH_NEWFUNC(myRemoveAppWindowDropZone));
			origAddAppWindowA = SetFunction(WorkbenchBase,
				-48, PATCH_NEWFUNC(myAddAppWindowA));
			origRemoveAppWindow = SetFunction(WorkbenchBase,
				-54, PATCH_NEWFUNC(myRemoveAppWindow));

			origWorkbenchControlA = SetFunction(WorkbenchBase,
				-108, PATCH_NEWFUNC(myWorkbenchControlA));

			origWbprivate1 = SetFunction(WorkbenchBase,
				-30, PATCH_NEWFUNC(myWbprivate1));

			origSCA_DrawDrag = SetFunction(&ScalosBase->scb_LibNode,
				-150, PATCH_NEWFUNC(mySCA_DrawDrag));
#endif
			DetachMyProcess();
			}

		// try to use current WB settings
		ParseWBPrefs("ENV:sys/Workbench.prefs");

		// Read some Scalos preferences settings
		ReadScalosPrefs();

		First = FALSE;
		}

	return TRUE;
}


BOOL initPlugin(struct PluginBase *libbase)
{
	d1(KPrintF("%s/%s/%ld: Start.\n"__FILE__, __FUNC__, __LINE__));

	(void)libbase;

#ifdef __AROS__
	libbase->pl_PlugID = MAKE_ID('P','L','U','G');
#endif

	if (!fInit)
		{
		if (!OpenLibraries())
			return FALSE;

		d1(KPrintF("%s/%s/%ld: IconobjectBase=%08lx WorkbenchBase=%08lx UtilityBase=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, \
                        IconobjectBase, WorkbenchBase, UtilityBase));

#if !defined(__amigaos4__) && !defined(__AROS__)
		_STI_240_InitMemFunctions();
#endif

		fInit = TRUE;       // +dm+ 20010520 Added
		}

	d1(KPrintF("%s/%s/%ld: End.\n"__FILE__, __FUNC__, __LINE__));

	return TRUE;
}


BOOL closePlugin(struct PluginBase *libbase)
{
	fInit = FALSE;

	d1(kprintf("%s/%s/%ld: \n"__FILE__, __FUNC__, __LINE__));

	ShutdownMyProcess();

#if defined(__amigaos4__)
	if (origWbprivate1)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, Private1),
			(APTR) origWbprivate1);
		origWbprivate1 = NULL;
		}
	if (origWorkbenchControlA)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, WorkbenchControlA),
			(APTR) origWorkbenchControlA);
		origWorkbenchControlA = NULL;
		}
	if (origRemoveAppWindow)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, RemoveAppWindow),
			(APTR) origRemoveAppWindow);
		origRemoveAppWindow = NULL;
		}
	if (origAddAppWindowA)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, AddAppWindowA),
			(APTR) origAddAppWindowA);
		origAddAppWindowA = NULL;
		}
	if (origRemoveAppWindowDropZone)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, RemoveAppWindowDropZone),
			(APTR) origRemoveAppWindowDropZone);
		origRemoveAppWindowDropZone = NULL;
		}
	if (origAddAppWindowDropZoneA)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, AddAppWindowDropZoneA),
			(APTR) origAddAppWindowDropZoneA);
		origAddAppWindowDropZoneA = NULL;
		}
	if (origMakeWorkbenchObjectVisibleA)
		{
		SetMethod((struct Interface *)IWorkbench,  offsetof(struct WorkbenchIFace, MakeWorkbenchObjectVisibleA),
			(APTR) origMakeWorkbenchObjectVisibleA);
		origMakeWorkbenchObjectVisibleA = NULL;
		}
	if (origOpenWorkbenchObjectA)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, OpenWorkbenchObjectA),
			(APTR) origOpenWorkbenchObjectA);
		origOpenWorkbenchObjectA = NULL;
		}
	if (origOpenWorkbenchObject)
		{
		SetMethod((struct Interface *)IWorkbench, offsetof(struct WorkbenchIFace, OpenWorkbenchObject),
			(APTR) origOpenWorkbenchObject);
		origOpenWorkbenchObject = NULL;
		}
	if (origSCA_DrawDrag)
		{
		SetMethod((struct Interface *)IScalos, offsetof(struct ScalosIFace, SCA_DrawDrag),
			(APTR) origSCA_DrawDrag);
		origSCA_DrawDrag = NULL;
		}
#elif defined(__AROS__)
	if (origWbprivate1)
		{
		SetFunction(WorkbenchBase, -5 * LIB_VECTSIZE, (ULONG (* const )()) origWbprivate1);
		origWbprivate1 = NULL;
		}
	if (origWorkbenchControlA)
		{
		SetFunction(WorkbenchBase, -18 * LIB_VECTSIZE, (ULONG (* const )()) origWorkbenchControlA);
		origWorkbenchControlA = NULL;
		}
	if (origRemoveAppWindow)
		{
		SetFunction(WorkbenchBase, -9 * LIB_VECTSIZE, (ULONG (* const )()) origRemoveAppWindow);
		origRemoveAppWindow = NULL;
		}
	if (origAddAppWindowA)
		{
		SetFunction(WorkbenchBase, -8 * LIB_VECTSIZE, (ULONG (* const )()) origAddAppWindowA);
		origAddAppWindowA = NULL;
		}
	if (origRemoveAppWindowDropZone)
		{
		SetFunction(WorkbenchBase, -20 * LIB_VECTSIZE, (ULONG (* const )()) origRemoveAppWindowDropZone);
		origRemoveAppWindowDropZone = NULL;
		}
	if (origAddAppWindowDropZoneA)
		{
		SetFunction(WorkbenchBase, -19 * LIB_VECTSIZE, (ULONG (* const )()) origAddAppWindowDropZoneA);
		origAddAppWindowDropZoneA = NULL;
		}
	if (origMakeWorkbenchObjectVisibleA)
		{
		SetFunction(WorkbenchBase,  -22 * LIB_VECTSIZE, (ULONG (* const )()) origMakeWorkbenchObjectVisibleA);
		origMakeWorkbenchObjectVisibleA = NULL;
		}
	if (origOpenWorkbenchObjectA)
		{
		SetFunction(WorkbenchBase, -16 * LIB_VECTSIZE, (ULONG (* const )()) origOpenWorkbenchObjectA);
		origOpenWorkbenchObjectA = NULL;
		}
	if (origSCA_DrawDrag)
		{
		SetFunction(&ScalosBase->scb_LibNode, -25 * LIB_VECTSIZE, (ULONG (* const )()) origSCA_DrawDrag);
		origSCA_DrawDrag = NULL;
		}
#else
	if (origWbprivate1)
		{
		SetFunction(WorkbenchBase, -30, (ULONG (* const )()) origWbprivate1);
		origWbprivate1 = NULL;
		}
	if (origWorkbenchControlA)
		{
		SetFunction(WorkbenchBase, -108, (ULONG (* const )()) origWorkbenchControlA);
		origWorkbenchControlA = NULL;
		}
	if (origRemoveAppWindow)
		{
		SetFunction(WorkbenchBase, -54, (ULONG (* const )()) origRemoveAppWindow);
		origRemoveAppWindow = NULL;
		}
	if (origAddAppWindowA)
		{
		SetFunction(WorkbenchBase, -48, (ULONG (* const )()) origAddAppWindowA);
		origAddAppWindowA = NULL;
		}
	if (origRemoveAppWindowDropZone)
		{
		SetFunction(WorkbenchBase, -120, (ULONG (* const )()) origRemoveAppWindowDropZone);
		origRemoveAppWindowDropZone = NULL;
		}
	if (origAddAppWindowDropZoneA)
		{
		SetFunction(WorkbenchBase, -114, (ULONG (* const )()) origAddAppWindowDropZoneA);
		origAddAppWindowDropZoneA = NULL;
		}
	if (origMakeWorkbenchObjectVisibleA)
		{
		SetFunction(WorkbenchBase,  -132, (ULONG (* const )()) origMakeWorkbenchObjectVisibleA);
		origMakeWorkbenchObjectVisibleA = NULL;
		}
	if (origOpenWorkbenchObjectA)
		{
		SetFunction(WorkbenchBase, -96, (ULONG (* const )()) origOpenWorkbenchObjectA);
		origOpenWorkbenchObjectA = NULL;
		}
	if (origSCA_DrawDrag)
		{
		SetFunction(&ScalosBase->scb_LibNode, -150, (ULONG (* const )()) origSCA_DrawDrag);
		origSCA_DrawDrag = NULL;
		}
#endif

	FreeAllNodes(&HiddenDeviceList);    // +dm+ 20010518
	CloseLibraries();

#if !defined(__amigaos4__) && !defined(__AROS__)
	_STD_240_TerminateMemFunctions();
#endif

	d1(kprintf("%s/%s/%ld: \n"__FILE__, __FUNC__, __LINE__));

	return TRUE;
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

	IconobjectBase = OpenLibrary("iconobject.library", 39);
	if (NULL == IconobjectBase)
		return FALSE;
#ifdef __amigaos4__
	IIconobject = (struct IconobjectIFace *)GetInterface((struct Library *)IconobjectBase, "main", 1, NULL);
	if (NULL == IIconobject)
		return FALSE;
#endif

	WorkbenchBase = OpenLibrary(WORKBENCH_NAME, 44);
	if (NULL == WorkbenchBase)
		return FALSE;
#ifdef __amigaos4__
	IWorkbench = (struct WorkbenchIFace *)GetInterface((struct Library *)WorkbenchBase, "main", 1, NULL);
	if (NULL == IWorkbench)
		return FALSE;
#endif

	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 40);
	if (NULL == UtilityBase)
		return FALSE;
#ifdef __amigaos4__
	IUtility = (struct UtilityIFace *)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
	if (NULL == IUtility)
		return FALSE;
#endif

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__UtilityBase = UtilityBase;
#endif /* defined(__GNUC__) && !defined(__MORPHOS__) */

	ScalosBase = (struct ScalosBase *) OpenLibrary(SCALOSNAME, 41);
	if (NULL == ScalosBase)
		return FALSE;
#ifdef __amigaos4__
	IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
	if (NULL == IScalos)
		return FALSE;
#endif

	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 40);
	if (NULL == IntuitionBase)
		return FALSE;
#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	if (NULL == IIntuition)
		return FALSE;
#endif

	LayersBase = OpenLibrary("layers.library", 40);
	if (NULL == LayersBase)
		return FALSE;
#ifdef __amigaos4__
	ILayers = (struct LayersIFace *)GetInterface((struct Library *)LayersBase, "main", 1, NULL);
	if (NULL == ILayers)
		return FALSE;
#endif

	GfxBase = OpenLibrary("graphics.library", 40);
	if (NULL == GfxBase)
		return FALSE;
#ifdef __amigaos4__
	IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	if (NULL == IGraphics)
		return FALSE;
#endif

	IconBase = OpenLibrary("icon.library", 41);
	if (NULL == IconBase)
		return FALSE;
#ifdef __amigaos4__
	IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
	if (NULL == IIcon)
		return FALSE;
#endif

	IFFParseBase = OpenLibrary("iffparse.library", 39);
	if (NULL == IFFParseBase)
		return FALSE;
#ifdef __amigaos4__
	IIFFParse = (struct IFFParseIFace *)GetInterface((struct Library *)IFFParseBase, "main", 1, NULL);
	if (NULL == IIFFParse)
		return FALSE;

	NewlibBase = OpenLibrary("newlib.library", 0);
	if (NULL == NewlibBase)
		return FALSE;
	INewlib = GetInterface(NewlibBase, "main", 1, NULL);
	if (NULL == INewlib)
		return FALSE;
#endif

	return TRUE;
}


static void CloseLibraries(void)
{
#ifdef __amigaos4__
	if (IIFFParse)
		{
		DropInterface((struct Interface *)IIFFParse);
		IIFFParse = NULL;
		}
#endif
	if (IFFParseBase)
		{
		CloseLibrary(IFFParseBase);
		IFFParseBase = NULL;
		}
#ifdef __amigaos4__
	if (IIcon)
		{
		DropInterface((struct Interface *)IIcon);
		IIcon = NULL;
		}
#endif
	if (IconBase)
		{
		CloseLibrary(IconBase);
		IconBase = NULL;
		}
#ifdef __amigaos4__
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
	if (IIconobject)
		{
		DropInterface((struct Interface *)IIconobject);
		IIconobject = NULL;
		}
#endif
	if (IconobjectBase)
		{
		CloseLibrary(IconobjectBase);
		IconobjectBase = NULL;
		}
#ifdef __amigaos4__
	if (IWorkbench)
		{
		DropInterface((struct Interface *)IWorkbench);
		IWorkbench = NULL;
		}
#endif
	if (WorkbenchBase)
		{
		CloseLibrary(WorkbenchBase);
		WorkbenchBase = NULL;
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
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif
	if (ScalosBase)
		{
		CloseLibrary((struct Library *) ScalosBase);
		ScalosBase = NULL;
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


// OpenWorkbenchObjectA emulation
static LIBFUNC_P3(BOOL, myOpenWorkbenchObjectA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct WBArg *ArgList;
	struct Process *myProc;
	Object *IconObj = NULL;
	struct DiskObject *diskObj = NULL;
	struct TagItem TagList[5];
	ULONG ArgCount = 1;
	BPTR fLock, dLock = (BPTR)NULL;
	BOOL Result = FALSE;

	/* Suppress warning */
	(void)WorkbenchBase;

	d1(KPrintF("%s/%ld\n", __FUNC__, __FILE__, __FUNC__, __LINE__));

	myProc = (struct Process *) FindTask(NULL);
	if (NT_PROCESS == myProc->pr_Task.tc_Node.ln_Type)
		{
		dLock = myProc->pr_CurrentDir;
		}

	fLock = Lock(name, ACCESS_READ);
	if (fLock)
		{
		char Buffer[256];
		struct FileInfoBlock *fib;

		NameFromLock(fLock, Buffer, sizeof(Buffer));

		fib = AllocDosObject(DOS_FIB, TAG_END);
		if (fib)
			{
			LONG DirEntryType;
			BOOL ShowAllFiles = FALSE;
			ULONG ViewByMode = SCAV_ViewModes_Icon;
			struct TagItem *tiShowAll, *tiViewBy;

			Examine(fLock, fib);
			UnLock(fLock);

			d1(KPrintF("%s/%s/%ld: name=<%s> \n"__FILE__, __FUNC__, __LINE__, fib->fib_FileName));

			DirEntryType = fib->fib_DirEntryType;

			FreeDosObject(DOS_FIB, fib);

			d1(KPrintF(__FUNC__ "/%ld\n"__FILE__, __FUNC__, __LINE__));

			tiShowAll = FindTagItem(WBOPENA_Show, tags);
			if (tiShowAll)
				{
				switch (tiShowAll->ti_Data)
					{
				case DDFLAGS_SHOWICONS:
					ShowAllFiles = FALSE;
					break;
				case DDFLAGS_SHOWALL:
					ShowAllFiles = TRUE;
					break;
				default:
					tiShowAll = NULL;
					break;
					}
				}

			tiViewBy = FindTagItem(WBOPENA_ViewBy, tags);
			if (tiViewBy)
				{
				switch (tiViewBy->ti_Data)
					{
				case DDVM_BYICON:
					ViewByMode = SCAV_ViewModes_Icon;
					break;
				case DDVM_BYNAME:
					ViewByMode = SCAV_ViewModes_Name;
					break;
				case DDVM_BYDATE:
					ViewByMode = SCAV_ViewModes_Date;
					break;
				case DDVM_BYSIZE:
					ViewByMode = SCAV_ViewModes_Size;
					break;
				case DDVM_BYTYPE:
				default:
					tiViewBy = NULL;
					break;
					}
				}

			d1(KPrintF(__FUNC__ "/%ld  DirEntryType=%ld\n"__FILE__, __FUNC__, __LINE__, DirEntryType));

			switch (DirEntryType)
				{
			case ST_ROOT:
			case ST_USERDIR:
			case ST_LINKDIR:
				// Volume or Directory : open Icon Window
				d1(KPrintF(__FUNC__ "/%ld Buffer=<%s>\n"__FILE__, __FUNC__, __LINE__, Buffer));
				Result = NULL != SCA_OpenDrawerByNameTags(Buffer,
					tiShowAll ? SCA_ShowAllFiles : TAG_IGNORE, ShowAllFiles,
					tiViewBy ? SCA_ViewModes : TAG_IGNORE, ViewByMode,
					TAG_END);

				d1(KPrintF(__FUNC__ "/%ld  Result=%08lx\n"__FILE__, __FUNC__, __LINE__, Result));

				return Result;
				break;

			case ST_FILE:
			case ST_LINKFILE:
				diskObj = GetDiskObjectNew(Buffer);
				if (diskObj)
					{
					IconObj = Convert2IconObjectA(diskObj, NULL);
					}

				d1(kprintf("%s/%s/%ld: FILE diskobj=%08lx  iconobj=%08lx\n", \
					__LINE__, diskObj, IconObj));
				break;
				}

			d1(kprintf(__FUNC__ "/%ld\n"__FILE__, __FUNC__, __LINE__));
			}
		else
			UnLock(fLock);
		}

	d1(kprintf(__FUNC__ "/%ld\n"__FILE__, __FUNC__, __LINE__));

	ArgList = BuildWBArg(tags, &ArgCount);
	if (NULL == ArgList)
		{
		if (IconObj)
			DisposeIconObject(IconObj);
		if (diskObj)
			FreeDiskObject(diskObj);
		return FALSE;
		}

	d1(kprintf("%s/%s/%ld: diskobj=%08lx  iconobj=%08lx\n", \
		__LINE__, diskObj, IconObj));

	if (NULL == IconObj)
		{
		IconObj = NewIconObjectTags(name, TAG_END);
		d1(kprintf("%s/%s/%ld: diskobj=%08lx  iconobj=%08lx\n", \
			__LINE__, diskObj, IconObj));
		}
	if (IconObj)
		{
		ArgList[0].wa_Lock = DupLock(dLock);
		ArgList[0].wa_Name = name;

		TagList[0].ti_Tag = SCA_IconObject;
		TagList[0].ti_Data = (ULONG) IconObj;
		TagList[1].ti_Tag = SCA_Stacksize;
		TagList[1].ti_Data = DefaultStackSize;
		TagList[2].ti_Tag = SCA_Flags;
		TagList[2].ti_Data = SCAV_WBStart_PathSearch;
		TagList[3].ti_Tag = TAG_END;
		TagList[3].ti_Data = 0;

		Result = SCA_WBStart(ArgList, TagList, ArgCount);

		FreeWBArg(ArgList, ArgCount, FALSE);
		}
	else
		FreeWBArg(ArgList, ArgCount, TRUE);

	d1(kprintf(__FUNC__ "/%ld\n"__FILE__, __FUNC__, __LINE__));
	if (IconObj)
		DisposeIconObject(IconObj);
	d1(kprintf(__FUNC__ "/%ld\n"__FILE__, __FUNC__, __LINE__));
	if (diskObj)
		FreeDiskObject(diskObj);

	d1(kprintf("%s/%s/%ld: Result=%ld\n"__FILE__, __FUNC__, __LINE__, Result));

	return Result;
}
LIBFUNC_END


static struct WBArg *BuildWBArg(struct TagItem *tags, ULONG *ArgCount)
{
	struct WBArg *ArgList;
	struct TagItem *tp;
	struct TagItem *tag;
	short n;

	*ArgCount = 1;

	// Count arguments (program itself is argument #0)
	tp = tags;
	d1(kprintf("%s/%s/%ld: tags=%08lx  tp=%08lx\n", \
		__LINE__, tags, tp));

	while (tag = NextTagItem(&tp))
		{
		if (WBOPENA_ArgName == tag->ti_Tag)
			{
			(*ArgCount)++;
			d1(kprintf("%s/%s/%ld: ArgCount=%ld\n", \
				__LINE__, *ArgCount));
			}
		}

	d1(kprintf("%s/%s/%ld: tags=%08lx  ArgCount=%ld\n", \
		__LINE__, tags, *ArgCount));

	// build Program's ArgList
	ArgList = calloc(sizeof(struct WBArg), *ArgCount);
	if (NULL == ArgList)
		return NULL;

	tp = tags;
	n = 1;
	while (tag = NextTagItem(&tp))
		{
		switch (tag->ti_Tag)
			{
		case WBOPENA_ArgLock:
			if (ArgList[n].wa_Lock)
				n++;
			ArgList[n].wa_Lock = DupLock((BPTR)tag->ti_Data);
#if 0
			{
			char Path[256];

			NameFromLock(tag->ti_Data, Path, sizeof(Path));
			kprintf("%s/%s/%ld: Arg[%ld] ArgLock=<%s>\n", \
			__LINE__, n, Path);
			}
#endif
			break;
		case WBOPENA_ArgName:
			if (ArgList[n].wa_Name)
				n++;
			ArgList[n].wa_Name = (STRPTR) tag->ti_Data;
			d1(kprintf("%s/%s/%ld: Arg[%ld] ArgName=<%s>\n", \
				__LINE__, n, tag->ti_Data));
			break;

		default:
			d1(kprintf("%s/%s/%ld: tag=%08lx  data=%08lx\n", \
				__LINE__, tag->ti_Tag, tag->ti_Data));
			break;
			}
		}

	return ArgList;
}


static void FreeWBArg(struct WBArg *ArgList, ULONG ArgCount, BOOL FreeLocks)
{
	short n;

	if (NULL == ArgList)
		return;

	if (FreeLocks)
		{
		for (n=0; n<ArgCount; n++)
			{
			if (ArgList[n].wa_Lock)
				UnLock(ArgList[n].wa_Lock);
			}
		}

	d1(KPrintF("%s/%s/%ld: diskobj=%08lx  iconobj=%08lx\n", \
			__LINE__, diskObj, IconObj));

	free(ArgList);
}


// CloseWorkbenchObjectA emulation
static LIBFUNC_P3(BOOL, myCloseWorkbenchObjectA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0)
{
	BOOL Found = FALSE;
	struct ScaWindowList *swList = SCA_LockWindowList(SCA_LockWindowList_Shared);
	BPTR nLock;

	/* Suppress warning */
	(void)WorkbenchBase;
	(void) tags;

	if (NULL == name)
		return Found;

	nLock = Lock(name, ACCESS_READ);
	if ((BPTR)NULL == nLock)
		return Found;

	if (swList)
		{
		struct ScaWindowStruct *swi;

		for (swi=swList->wl_WindowStruct; !Found && swi; swi = (struct ScaWindowStruct *) swi->ws_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: swi=%08lx Lock=%08lx nLock=%08lx\n"__FILE__, __FUNC__, __LINE__, \
				swi, swi->ws_Lock, nLock));

			if (swi->ws_Lock && LOCK_SAME == SameLock(swi->ws_Lock, nLock))
				{
				CloseScalosWindow(swi);
				// swi is now invalid
				swi = NULL;
				Found = TRUE;
				break;
				}
			}

		if (swi)
			SCA_UnLockWindowList();
		}

	UnLock(nLock);

	d1(kprintf("%s/%s/%ld: Found=%ld\n"__FILE__, __FUNC__, __LINE__, Found));

	return Found;
}
LIBFUNC_END


// "updateicon"
static LIBFUNC_P3(ULONG, myWbprivate1,
	A0, STRPTR, Name,
	A1, BPTR, iLock,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct Task *myTask = FindTask(NULL);

	/* Suppress warning */
	(void)WorkbenchBase;

	d1(kprintf("%s/%s/%ld:  Lock=%08lx  Name=<%s>\n"__FILE__, __FUNC__, __LINE__, iLock, Name));

	if (myTask->tc_Node.ln_Name)
		{
		d1(kprintf("%s/%s/%ld: Task=<%s>\n"__FILE__, __FUNC__, __LINE__, myTask->tc_Node.ln_Name));

		if (0 != Stricmp(myTask->tc_Node.ln_Name, "Scalos_Window_Task") &&
			0 != Stricmp(myTask->tc_Node.ln_Name, "Scalos_Drag&Drop"))
			{
			struct ScaUpdateIcon_IW Upd;

			Upd.ui_iw_Lock = iLock;
			Upd.ui_iw_Name = Name;

			d1(kprintf("%s/%s/%ld:  Lock=%08lx  Name=<%s>\n"__FILE__, __FUNC__, __LINE__, iLock, Name));

			SCA_UpdateIcon(WSV_Type_IconWindow, &Upd, sizeof(Upd));
			}
		}

	return 0;
}
LIBFUNC_END

/* ====================================================== */

static void UpdateVolumeGauge(struct Gadget *gad, struct Window *win, BPTR iLock)
{
	struct InfoData *id = AllocVec(sizeof(struct InfoData), MEMF_PUBLIC);

	if (id)
		{
		Info(iLock, id);

		d1(kprintf("%s/%s/%ld: NumBlocks=%lu  NumBlocksUsed=%lu\n"__FILE__, __FUNC__, __LINE__, \
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


static LIBFUNC_P3(BOOL, myMakeWorkbenchObjectVisibleA,
	A0, STRPTR, name,
	A1, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct Process *myProc;
	BPTR fLock;
	// dLock = (BPTR)NULL; // unused
	BOOL Result = FALSE;

	/* Suppress warning */
	(void)WorkbenchBase;
	(void) tags;

	myProc = (struct Process *) FindTask(NULL);
	if (NT_PROCESS == myProc->pr_Task.tc_Node.ln_Type)
		{
		// dLock = myProc->pr_CurrentDir;
		}

	fLock = Lock(name, ACCESS_READ);
	if (fLock)
		{
		struct ScaWindowStruct *swi;
		BPTR parentLock = ParentDir(fLock);

		swi = FindWindowByLock(parentLock);
		if (swi)
			{
			struct ScaIconNode *sIcon = GetIconByName(swi, FilePart(name));

			if (sIcon)
				{
				Result = TRUE;

				MakeIconVisible(swi, sIcon);
				ReleaseSemaphore(swi->ws_WindowTask->wt_IconSemaphore);
				}

			SCA_UnLockWindowList();
			}

		UnLock(parentLock);
		UnLock(fLock);
		}

	return Result;
}
LIBFUNC_END


// ChangeWorkbenchSelectionA()
static LIBFUNC_P4(BOOL, myChangeWorkbenchSelectionA,
	A0, STRPTR, name,
	A1, struct Hook *, hook,
	A2, struct TagItem *, tags,
	A6, struct Library *, WorkbenchBase, 0)
{
	struct ScaWindowStruct *swi = NULL;
	BOOL Result = FALSE;

	/* Suppress warning */
	(void)WorkbenchBase;

	d1(kprintf("%s/%s/%ld: name=%08lx  hook=%08lx  tags=%08lx\n"__FILE__, __FUNC__, __LINE__, name, hook, tags));
	d1(kprintf("%s/%s/%ld: name=<%s>\n"__FILE__, __FUNC__, __LINE__, name ? name : (STRPTR) ""));

	if (name)
		{
		BPTR fLock = Lock(name, ACCESS_READ);

		d1(kprintf("%s/%s/%ld: fLock=%08lx\n"__FILE__, __FUNC__, __LINE__, fLock));
		if (fLock)
			{
			swi = FindWindowByLock(fLock);

			d1(kprintf("%s/%s/%ld: swi=%08lx\n"__FILE__, __FUNC__, __LINE__, swi));
			UnLock(fLock);

			if (swi)
				SCA_UnLockWindowList();
			}
		}
	else
		{
		// get Workbench Backdrop window
		swi = FindWindowByName("root");
		}

	d1(kprintf("%s/%s/%ld: swi=%08lx\n"__FILE__, __FUNC__, __LINE__, swi));

	if (swi)
		{
		struct ScaWindowTask *wt = swi->ws_WindowTask;
		struct ScaIconNode *icon;
		BOOL GoOn = TRUE;

		Result = TRUE;

		ObtainSemaphore(wt->wt_IconSemaphore);

		for (icon=wt->wt_IconList; GoOn && icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) icon->in_Icon;
			struct IconSelectMsg ism;
			ULONG IconType;
			WORD iconLeft, iconTop;
			struct ScaWindowStruct *swiDrawer = GetIconNodeOpenWindow(swi->ws_Lock, icon);

			ism.ism_Name = NULL;

			GetAttr(IDTA_Text, icon->in_Icon, (ULONG *) &ism.ism_Name);
			GetAttr(IDTA_Type, icon->in_Icon, (ULONG *) &IconType);

			d1(kprintf("%s/%s/%ld: icon=%08lx <%s>  IconType=%ld  swiDrawer=%08lx\n", \
				__LINE__, icon, ism.ism_Name, IconType, swiDrawer));

			// Translate Scalos-specific icon types
			switch (IconType)
				{
			case WB_TEXTICON_TOOL:
				IconType = WBTOOL;
				break;
			case WB_TEXTICON_DRAWER:
				IconType = WBDRAWER;
				break;
			default:
				break;
				}

			iconLeft = gg->BoundsLeftEdge + swi->ws_Window->BorderLeft - swi->ws_xoffset;
			iconTop = gg->BoundsTopEdge + swi->ws_Window->BorderTop - swi->ws_yoffset;

			ism.ism_Length = sizeof(ism);
			ism.ism_Drawer = swi->ws_Lock;
			ism.ism_Type = IconType;
			ism.ism_Selected = (gg->Flags & GFLG_SELECTED) ? TRUE : FALSE;
			ism.ism_Tags = tags;
			ism.ism_DrawerWindow = swiDrawer ? swiDrawer->ws_Window : NULL;
			ism.ism_ParentWindow = swi->ws_Window;
			ism.ism_Left = iconLeft;
			ism.ism_Top = iconTop;
			ism.ism_Width = gg->BoundsWidth;
			ism.ism_Height = gg->BoundsHeight;

			switch (CallHookPkt(hook, NULL, &ism))
				{
			case ISMACTION_Unselect:
				d1(kprintf("%s/%s/%ld: ISMACTION_Unselect\n"__FILE__, __FUNC__, __LINE__));
				SelectIcon(swi, icon, FALSE);
				break;
			case ISMACTION_Select:
				d1(kprintf("%s/%s/%ld: ISMACTION_Select\n"__FILE__, __FUNC__, __LINE__));
				SelectIcon(swi, icon, TRUE);
				break;
			case ISMACTION_Ignore:
				d1(kprintf("%s/%s/%ld: ISMACTION_Ignore\n"__FILE__, __FUNC__, __LINE__));
				break;
			case ISMACTION_Stop:
				d1(kprintf("%s/%s/%ld: ISMACTION_Stop\n"__FILE__, __FUNC__, __LINE__));
				GoOn = FALSE;
				break;
				}

			if (swiDrawer)
				SCA_UnLockWindowList();

			if (ism.ism_Left != iconLeft || ism.ism_Top != iconTop)
				{
				d1(kprintf("%s/%s/%ld: oldLeft=%ld  Left=%ld\n"__FILE__, __FUNC__, __LINE__, ism.ism_Left, iconLeft));
				d1(kprintf("%s/%s/%ld: oldTop=%ld  Top=%ld\n"__FILE__, __FUNC__, __LINE__, ism.ism_Top, iconTop));

				MoveIcon(swi, icon, 
					ism.ism_Left - swi->ws_Window->BorderLeft + swi->ws_xoffset,
					ism.ism_Top - swi->ws_Window->BorderTop + swi->ws_yoffset);
				}
			}

		ReleaseSemaphore(wt->wt_IconSemaphore);
		}

	d1(kprintf("%s/%s/%ld: Result=%ld\n"__FILE__, __FUNC__, __LINE__, Result));    // +dm+ 20010518 Added another %ld

	return Result;
}
LIBFUNC_END


// +dm+ 20010518 start
// Needed for not having to free the list structure also
static void FreeAllNodes(struct List *pl)
{
	struct Node *xNode;

	d1(kprintf("%s/%s/%ld: List=%08lx\n"__FILE__, __FUNC__, __LINE__, pl));
	if(pl)
	{
		while(xNode = RemHead(pl))
		{
			d1(kprintf("%s/%s/%ld: Node=%08lx  Name=%08lx  Succ=%08lx\n"__FILE__, __FUNC__, __LINE__, xNode, xNode->ln_Name, xNode->ln_Succ));
			if(xNode->ln_Name) free(xNode->ln_Name);
			free(xNode);
		}
	}
}
// +dm+ 20010518 end

#ifdef __amigaos4__
static LIBFUNC_P2VA(BOOL, myOpenWorkbenchObject,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase)
{
	BOOL ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, name);

	(void)WorkbenchBase;

	ret = OpenWorkbenchObjectA(name, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P2VA(BOOL, myCloseWorkbenchObject,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase)
{
	BOOL ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, name);

	(void)WorkbenchBase;

	ret = CloseWorkbenchObjectA(name, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P2VA(BOOL, myMakeWorkbenchObjectVisible,
	A0, STRPTR, name,
	A6, struct Library *, WorkbenchBase)
{
	BOOL ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, name);

	(void)WorkbenchBase;

	ret = MakeWorkbenchObjectVisibleA(name, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P3VA(BOOL, myChangeWorkbenchSelection,
	A0, STRPTR, name,
	A1, struct Hook *, hook,
	A6, struct Library *, WorkbenchBase)
{
	BOOL ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, hook);

	(void)WorkbenchBase;

	ret = ChangeWorkbenchSelectionA(name, hook, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

#endif

#ifdef __SASC
#elif !defined(__amigaos4__) /* __SASC */

void exit(int x)
{
	(void) x;
	while (1)
		;
}

APTR _WBenchMsg;

#endif /* __SASC */

//----------------------------------------------------------------------------

#if defined(__AROS__)

#include "aros/symbolsets.h"

ADD2EXPUNGELIB(closePlugin, 0);
ADD2OPENLIB(initPlugin, 0);

#endif

//----------------------------------------------------------------------------
