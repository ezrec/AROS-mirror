// Patches-classic.c
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

// local data structures

#if defined(__MORPHOS__)
	#define PPATCHFUNC(x)		 { TRAP_LIB, 0, (APTR) x }
	#define	PPATCH_NEWFUNC(ple)	 (APTR) &ple->pe_EmulEntry
#else
	#define	PPATCHFUNC(x)		 (APTR) x
	#define	PPATCH_NEWFUNC(ple)	 ple->pe_NewFunction
#endif

struct PatchEntry
	{
	struct Library **pe_Library;
	ULONG pe_MinVersion;
	LONG pe_LibOffset;
#ifdef __MORPHOS__
	struct EmulLibEntry pe_EmulEntry;
#else
	APTR pe_NewFunction;
#endif
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

//----------------------------------------------------------------------------

#define LVOCloseWorkBench 	-13  * LIB_VECTSIZE
#define LVOOpenWorkBench 	-35  * LIB_VECTSIZE
#define LVOOpenScreenTagList 	-102 * LIB_VECTSIZE
#define LVOOpenWindowTagList	-101 * LIB_VECTSIZE

#define LVOUpdateWorkbench	-5   * LIB_VECTSIZE	/* private1 */
#define LVOwbPrivate2 	        -6   * LIB_VECTSIZE
#define LVOAddAppWindowA 	-8   * LIB_VECTSIZE
#define LVORemoveAppWindow 	-9   * LIB_VECTSIZE
#define LVOAddAppIconA 		-10  * LIB_VECTSIZE
#define LVORemoveAppIcon 	-11  * LIB_VECTSIZE
#define LVOAddAppMenuItemA 	-12  * LIB_VECTSIZE
#define LVORemoveAppMenuItem 	-13  * LIB_VECTSIZE
#define LVOSetBackFill		-14  * LIB_VECTSIZE
#define LVOWBInfo		-15  * LIB_VECTSIZE

#define LVOPutDiskObject 	-14  * LIB_VECTSIZE
#define LVODeleteDiskObject 	-23  * LIB_VECTSIZE
#define LVOPutIconTagList	-31  * LIB_VECTSIZE

#define LVODeleteFile		-12  * LIB_VECTSIZE
#define LVORename 		-13  * LIB_VECTSIZE
#define LVOOpen			-5   * LIB_VECTSIZE
#define LVOClose		-6   * LIB_VECTSIZE
#define LVOCreateDir		-20  * LIB_VECTSIZE

//----------------------------------------------------------------------------

static const struct PatchEntry EmulationPatches[] =
	{
#ifdef TEST_OPENWINDOWTAGLIST
	{ (struct Library **) &IntuitionBase, 39,	LVOOpenWindowTagList,	PPATCHFUNC(sca_OpenWindowTagList),	(APTR) &OldOpenWindowTagList },
#endif /* TEST_OPENWINDOWTAGLIST */
	{ (struct Library **) &IntuitionBase, 39,	LVOCloseWorkBench,	PPATCHFUNC(sca_CloseWorkBench),		(APTR) &OldCloseWB },
	{ (struct Library **) &IntuitionBase, 39,	LVOOpenWorkBench,	PPATCHFUNC(sca_OpenWorkBench),		(APTR) &OldOpenWB },
	{ (struct Library **) &IntuitionBase, 39,	LVOOpenScreenTagList,	PPATCHFUNC(sca_OpenScreenTagList),	(APTR) &OldOpenScreenTagList },
	{ (struct Library **) &WorkbenchBase, 39,	LVOAddAppIconA,		PPATCHFUNC(sca_AddAppIconA),	     	(APTR) &OldAddAppIconA },
	{ (struct Library **) &WorkbenchBase, 39,	LVORemoveAppIcon,	PPATCHFUNC(sca_RemoveAppIcon),	     	(APTR) &OldRemoveAppIcon },
	{ (struct Library **) &WorkbenchBase, 39,	LVOAddAppWindowA,	PPATCHFUNC(sca_AddAppWindowA),	     	(APTR) &OldAddAppWindowA },
	{ (struct Library **) &WorkbenchBase, 39,	LVORemoveAppWindow,	PPATCHFUNC(sca_RemoveAppWindow),     	(APTR) &OldRemoveAppWindow },
	{ (struct Library **) &WorkbenchBase, 39,	LVOAddAppMenuItemA,	PPATCHFUNC(sca_AddAppMenuItemA),     	(APTR) &OldAppAppMenuItemA },
	{ (struct Library **) &WorkbenchBase, 39,	LVORemoveAppMenuItem,	PPATCHFUNC(sca_RemoveAppMenuItem),   	(APTR) &OldRemoveAppMenuItem },
//	{ (struct Library **) &WorkbenchBase, 39,	LVOwbPrivate2,		PPATCHFUNC(sca_GetLocString),	     	(APTR) &OldwbPrivate2 },
	{ NULL, 				0, 0, 			PPATCHFUNC(NULL), 		     	NULL },
	};
static const struct PatchEntry StandardPatches[] =
	{
	{ (struct Library **) &IconBase, 39, 	LVOPutDiskObject,	PPATCHFUNC(sca_PutDiskObject),	     	(APTR) &OldPutDiskObject },
	{ (struct Library **) &IconBase, 39, 	LVODeleteDiskObject,	PPATCHFUNC(sca_DeleteDiskObject),    	(APTR) &OldDeleteDiskObject },
	{ (struct Library **) &IconBase, 44, 	LVOPutIconTagList,	PPATCHFUNC(sca_PutIconTagList),    	(APTR) &OldPutIconTagList},

	{ (struct Library **) &DOSBase,	 39,	LVODeleteFile,		PPATCHFUNC(sca_DeleteFile),	     	(APTR) &OldDeleteFile },
	{ (struct Library **) &DOSBase,	 39,	LVORename,		PPATCHFUNC(sca_Rename),		     	(APTR) &OldRename },
	{ (struct Library **) &DOSBase,	 39,	LVOCreateDir,		PPATCHFUNC(sca_CreateDir),	        (APTR) &OldCreateDir },
	{ (struct Library **) &DOSBase,	 39,	LVOOpen,		PPATCHFUNC(sca_Open),	            	(APTR) &OldOpen },
	{ (struct Library **) &DOSBase,	 39,	LVOClose,		PPATCHFUNC(sca_Close),		     	(APTR) &OldClose },
	{ NULL, 				0, 0, 			PPATCHFUNC(NULL),			NULL },
	};
static const struct PatchEntry HardEmulationPatches[] =
	{
	{ (struct Library **) &WorkbenchBase, 39, 	LVOUpdateWorkbench,	PPATCHFUNC(sca_UpdateWorkbench),     	(APTR) &OldUpdateWorkbench },
	{ (struct Library **) &WorkbenchBase, 39, 	LVOSetBackFill,		PPATCHFUNC(sca_SetBackFill),	     	(APTR) &OldSetBackFill },
	{ (struct Library **) &WorkbenchBase, 39, 	LVOWBInfo,		PPATCHFUNC(sca_WBInfo),	        	(APTR) &OldWBInfo },
	{ NULL, 				0, 0, 			PPATCHFUNC(NULL), 			NULL },
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

	d1(KPrintF("%s/%s/%ld: PatchList=%08lx\n", __FILE__, __FUNC__, __LINE__, PatchList));

	while (PatchList->pe_Library)
		{
		if (NULL == *PatchList->pe_OldFunction 
			&& (*PatchList->pe_Library)->lib_Version >= PatchList->pe_MinVersion)
			{
			*PatchList->pe_OldFunction = SetFunction(*PatchList->pe_Library, 
				PatchList->pe_LibOffset, PPATCH_NEWFUNC(PatchList));

			d1(KPrintF("%s/%s/%ld: Library=%08lx  LVO=%ld  NewFunc=%08lx  OldFunc=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, *PatchList->pe_Library, \
				PatchList->pe_LibOffset, PPATCH_NEWFUNC(PatchList), \
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

	while (pl->pe_Library)
		{
		if (pl->pe_OldFunction)
			{
			APTR patchedFunction;

			patchedFunction = SetFunction(*pl->pe_Library,
				pl->pe_LibOffset, *pl->pe_OldFunction);
			*pl->pe_OldFunction = NULL;

			d1(KPrintF("%s/%s/%ld: Library=%08lx  LVO=%ld  NewFunc=%08lx  patchedFunc=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, *PatchList->pe_Library, \
				PatchList->pe_LibOffset, PPATCH_NEWFUNC(pl), patchedFunction));
#if !defined(__MORPHOS__)
			if (patchedFunction != PPATCH_NEWFUNC(pl))
				{
				*pl->pe_OldFunction = SetFunction(*pl->pe_Library,
					pl->pe_LibOffset, patchedFunction);

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

