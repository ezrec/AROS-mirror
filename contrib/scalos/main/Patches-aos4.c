// Patches-aos4.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <datatypes/pictureclass.h>
#include <dos/dostags.h>
#include <guigfx/guigfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/guigfx.h>
#include <proto/datatypes.h>
#include <proto/gadtools.h>
#include <proto/wb.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/palette.h>
#include <scalos/pattern.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"
#include "Patches.h"

//----------------------------------------------------------------------------
// Revision history :
//
// 20011203	jl	initial history
//----------------------------------------------------------------------------

// local data structures

struct PatchEntry
	{
	struct Interface **pe_Interface;
	LONG pe_IFaceOffset;
	APTR pe_NewFunction;
	APTR *pe_OldFunction;
	};

//----------------------------------------------------------------------------

// local functions

static void SetPatches(const struct PatchEntry *PatchList);
static BOOL RemovePatches(const struct PatchEntry *PatchList);

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

// local data items
static LIBFUNC_P7VA_PROTO(struct AppIcon *, sca_AddAppIcon,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, IconText,
	A1, struct MsgPort *, iconPort,
	A2, BPTR, lock,
	A3, struct DiskObject *, DiskObj,
	A6, struct Library *, wbBase);
static LIBFUNC_P5VA_PROTO(struct AppWindow *, sca_AddAppWindow,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, struct Window *, window,
	A1, struct MsgPort *, iconPort,
	A6, struct Library *, wbBase);
static LIBFUNC_P5VA_PROTO(struct AppMenuItem *, sca_AddAppMenuItem,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, text,
	A1, struct MsgPort *, iconPort,
	A6, struct Library *, wbBase);
static LIBFUNC_P2VA_PROTO(struct Screen *, sca_OpenScreen,
	A0, struct NewScreen *, newScr,
	A6, struct IntuitionBase *, iBase);

static APTR OldAddAppIcon;
static APTR OldAddAppWindow;
static APTR OldAppAppMenuItem;
static APTR OldOpenScreen;

//----------------------------------------------------------------------------

#define OffsetCloseWorkBench 		offsetof(struct IntuitionIFace, CloseWorkBench)
#define OffsetOpenWorkBench 	   	offsetof(struct IntuitionIFace, OpenWorkBench)
#define OffsetOpenScreenTagList    	offsetof(struct IntuitionIFace, OpenScreenTagList)
#define OffsetOpenScreen           	offsetof(struct IntuitionIFace, OpenScreen)

#define OffsetUpdateWorkbench	   	offsetof(struct WorkbenchIFace, UpdateWorkbench)
//#define OffsetwbPrivate2 		offsetof(struct WorkbenchIFace, Private1)
#define OffsetAddAppWindowA 	   	offsetof(struct WorkbenchIFace, AddAppWindowA)
#define OffsetAddAppWindow	   	offsetof(struct WorkbenchIFace, AddAppWindow)
#define OffsetRemoveAppWindow      	offsetof(struct WorkbenchIFace, RemoveAppWindow)
#define OffsetAddAppIconA          	offsetof(struct WorkbenchIFace, AddAppIconA)
#define OffsetAddAppIcon           	offsetof(struct WorkbenchIFace, AddAppIcon)
#define OffsetRemoveAppIcon        	offsetof(struct WorkbenchIFace, RemoveAppIcon)
#define OffsetAddAppMenuItemA      	offsetof(struct WorkbenchIFace, AddAppMenuItemA)
#define OffsetAddAppMenuItem       	offsetof(struct WorkbenchIFace, AddAppMenuItem)
#define OffsetRemoveAppMenuItem 	offsetof(struct WorkbenchIFace, RemoveAppMenuItem)
#define OffsetWBInfo			offsetof(struct WorkbenchIFace, WBInfo)
//#define OffsetSetBackFill          	offsetof(struct WorkbenchIFace, ...)

#define OffsetPutDiskObject        	offsetof(struct IconIFace, PutDiskObject)
#define OffsetDeleteDiskObject     	offsetof(struct IconIFace, DeleteDiskObject)
#define OffsetPutIconTagList		offsetof(struct IconIFace, PutIconTagList)

#define OffsetRename 			offsetof(struct DOSIFace, Rename)
#define OffsetDeleteFile		offsetof(struct DOSIFace, DeleteFile)
#define OffsetOpen 			offsetof(struct DOSIFace, Open)
#define OffsetClose 			offsetof(struct DOSIFace, Close)
#define OffsetCreateDir 		offsetof(struct DOSIFace, CreateDir)

//----------------------------------------------------------------------------
static const struct PatchEntry EmulationPatches[] =
	{
	{ (struct Interface **)(APTR) &IIntuition,	OffsetCloseWorkBench,		(APTR)sca_CloseWorkBench,         (APTR) &OldCloseWB },
	{ (struct Interface **)(APTR) &IIntuition,	OffsetOpenWorkBench,		(APTR)sca_OpenWorkBench,          (APTR) &OldOpenWB },
	{ (struct Interface **)(APTR) &IIntuition,	OffsetOpenScreenTagList,	(APTR)sca_OpenScreenTagList,      (APTR) &OldOpenScreenTagList },
	{ (struct Interface **)(APTR) &IIntuition,	OffsetOpenScreen,		(APTR)sca_OpenScreen,             (APTR) &OldOpenScreen },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetAddAppIconA,		(APTR)sca_AddAppIconA,            (APTR) &OldAddAppIconA },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetAddAppIcon,		(APTR)sca_AddAppIcon,             (APTR) &OldAddAppIcon },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetRemoveAppIcon,		(APTR)sca_RemoveAppIcon,          (APTR) &OldRemoveAppIcon },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetAddAppWindowA,		(APTR)sca_AddAppWindowA,          (APTR) &OldAddAppWindowA },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetAddAppWindow,		(APTR)sca_AddAppWindow,           (APTR) &OldAddAppWindow },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetRemoveAppWindow,		(APTR)sca_RemoveAppWindow,        (APTR) &OldRemoveAppWindow },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetAddAppMenuItemA,		(APTR)sca_AddAppMenuItemA,        (APTR) &OldAppAppMenuItemA },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetAddAppMenuItem,		(APTR)sca_AddAppMenuItem,         (APTR) &OldAppAppMenuItem },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetRemoveAppMenuItem,	(APTR)sca_RemoveAppMenuItem,      (APTR) &OldRemoveAppMenuItem },
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetWBInfo,			(APTR)sca_WBInfo,      		  (APTR) &OldWBInfo },

//	{ (struct Interface **)(APTR) &IWorkbench,	OffsetwbPrivate2,		(APTR)sca_GetLocString,           (APTR) &OldwbPrivate2 },
	{ NULL, 					0, 				(APTR)NULL, 		       	  NULL },
	};
static const struct PatchEntry StandardPatches[] =
	{
	{ (struct Interface **)(APTR) &IIcon,		OffsetPutDiskObject,		(APTR)sca_PutDiskObject,	(APTR) &OldPutDiskObject },
	{ (struct Interface **)(APTR) &IIcon,		OffsetDeleteDiskObject,		(APTR)sca_DeleteDiskObject,	(APTR) &OldDeleteDiskObject },
	{ (struct Interface **)(APTR) &IIcon,		OffsetPutIconTagList,		(APTR)sca_PutIconTagList,	(APTR) &OldPutIconTagList},

	{ (struct Interface **)(APTR) &IDOS,		OffsetDeleteFile,		(APTR)sca_DeleteFile,		(APTR) &OldDeleteFile },
	{ (struct Interface **)(APTR) &IDOS,		OffsetRename,			(APTR)sca_Rename,		(APTR) &OldRename },
	{ (struct Interface **)(APTR) &IDOS,		OffsetOpen,			(APTR)sca_Open, 		(APTR) &OldOpen },
	{ (struct Interface **)(APTR) &IDOS,		OffsetClose,			(APTR)sca_Close,        	(APTR) &OldClose },
	{ (struct Interface **)(APTR) &IDOS,		OffsetCreateDir,		(APTR)sca_CreateDir,		(APTR) &OldCreateDir },
	{ NULL, 					0,				(APTR)NULL,		          NULL },
	};
static const struct PatchEntry HardEmulationPatches[] =
	{
	{ (struct Interface **)(APTR) &IWorkbench,	OffsetUpdateWorkbench,		(APTR)sca_UpdateWorkbench,        (APTR) &OldUpdateWorkbench },
//	{ (struct Interface **)(APTR) &IWorkbench,	OffsetSetBackFill,		(APTR)sca_SetBackFill,            (APTR) &OldSetBackFill },
	{ NULL, 					0,				(APTR)NULL, 			  NULL },
	};

//----------------------------------------------------------------------------

void SetAllPatches(struct MainTask *mt)
{
	PatchInit();

	SetPatches(StandardPatches);

	if (mt->emulation)
		{
		if (CurrentPrefs.pref_HardEmulation)
			SetPatches(HardEmulationPatches);

		SetPatches(EmulationPatches);
		}
}


ULONG RemoveAllPatches(struct MainTask *mt)
{
	if (!RemovePatches(StandardPatches))
		return FALSE;

	if (mt->emulation)
		{
		if (!RemovePatches(EmulationPatches))
			{
			SetPatches(StandardPatches);
			return FALSE;
			}

		if (CurrentPrefs.pref_HardEmulation)
			{
			if (!RemovePatches(HardEmulationPatches))
				{
				SetPatches(EmulationPatches);
				SetPatches(StandardPatches);
				}
			}

		}

	PatchCleanup();

	return TRUE;
}

static void SetPatches(const struct PatchEntry *PatchList)
{
	Forbid();

	d1(kprintf("%s/%s/%ld: PatchList=%08lx\n", __FILE__, __FUNC__, __LINE__, PatchList));

	while (PatchList->pe_Interface)
		{
		if (NULL == *PatchList->pe_OldFunction)
			{
			*PatchList->pe_OldFunction = SetMethod(*PatchList->pe_Interface,
				PatchList->pe_IFaceOffset, PatchList->pe_NewFunction);

			d1(KPrintF("%s/%s/%ld: Library=%08lx  Offset=%ld  NewFunc=%08lx  OldFunc=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, *PatchList->pe_Interface, \
				PatchList->pe_IFaceOffset, PatchList->pe_NewFunction, \
				*PatchList->pe_OldFunction));
			}

		PatchList++;
		}

//	CacheClearU();
	Permit();
}


static BOOL RemovePatches(const struct PatchEntry *PatchList)
{
	const struct PatchEntry *pl = PatchList;
	BOOL Success = TRUE;

	d1(KPrintF("%s/%s/%ld: PatchList=%08lx\n", __FILE__, __FUNC__, __LINE__, PatchList));

	Forbid();

	while (pl->pe_Interface)
		{
		if (pl->pe_OldFunction)
			{
			APTR patchedFunction;

			patchedFunction = SetMethod(*pl->pe_Interface,
				pl->pe_IFaceOffset, *pl->pe_OldFunction);
			*pl->pe_OldFunction = NULL;

			d1(KPrintF("%s/%s/%ld: Library=%08lx  Offset=%ld  NewFunc=%08lx  patchedFunc=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, *pl->pe_Interface, \
				pl->pe_IFaceOffset, pl->pe_NewFunction, patchedFunction));
#if !defined(__MORPHOS__)
			if (patchedFunction != pl->pe_NewFunction)
				{
				*pl->pe_OldFunction = SetMethod(*pl->pe_Interface,
					pl->pe_IFaceOffset, patchedFunction);

				Success = FALSE;
				}
#endif /* __MORPHOS__ */
			}

		pl++;
		}

//	CacheClearU();
	Permit();

	d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (!Success)
		SetPatches(PatchList);

	return Success;
}

static LIBFUNC_P7VA(struct AppIcon *, sca_AddAppIcon,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, IconText,
	A1, struct MsgPort *, iconPort,
	A2, BPTR, lock,
	A3, struct DiskObject *, DiskObj,
	A6, struct Library *, wbBase)
{
	struct AppIcon *ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, DiskObj);

	(void)wbBase;

	ret = AddAppIconA(Id, UserData, IconText, iconPort, lock, DiskObj,
		 va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P5VA(struct AppWindow *, sca_AddAppWindow,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, struct Window *, window,
	A1, struct MsgPort *, iconPort,
	A6, struct Library *, wbBase)
{
	struct AppWindow *ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, iconPort);

	(void)wbBase;

	ret = AddAppWindowA(Id, UserData, window, iconPort, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P5VA(struct AppMenuItem *, sca_AddAppMenuItem,
	D0, ULONG, Id,
	D1, ULONG, UserData,
	A0, CONST_STRPTR, text,
	A1, struct MsgPort *, iconPort,
	A6, struct Library *, wbBase)
{
	struct AppMenuItem *ret;
	struct WorkbenchIFace *IWorkbench = (struct WorkbenchIFace *)self;
  	va_list args;
	va_startlinear(args, iconPort);

	(void)wbBase;

	ret = AddAppMenuItemA(Id, UserData, text, iconPort, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P2VA(struct Screen *, sca_OpenScreen,
	A0, struct NewScreen *, newScr,
	A6, struct IntuitionBase *, iBase)
{
	struct Screen *ret;
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)self;
  	va_list args;
	va_startlinear(args, newScr);

	(void)iBase;

	ret = OpenScreenTagList(newScr, va_getlinearva(args, const struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

