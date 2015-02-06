// AppMenu.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/intuitionbase.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <libraries/pm.h>
#include <libraries/iffparse.h>
#include <prefs/font.h>
#include <prefs/prefhdr.h>
#include <prefs/workbench.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/datatypes.h>
#include <proto/gadtools.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local data items

SCALOSSEMAPHORE AppMenuListSemaphore;	// Semaphore to protect ToolsMenuItemList and AppMenuList
static struct List ToolsMenuItemList;	// list of AppMenuInfo's in the tools menu (only MN_ITEMs)

static struct List AppMenuList;		// list of AppMenuInfo's in new menus (only NM_TITLE)
SCALOSSEMAPHORE	MenuSemaphore;

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

// local functions

static struct NewMenu *CreateNewMenuArray(void);
static ULONG CountToolMenuItems(struct List *MenuInfoList);
static void InsertAppMenuItems(struct NewMenu *nmArray, ULONG *ndx, struct List *MenuInfoList);
static void ClearAllMenus(void);
static void SetAllMenus(void);
static ULONG CountListMembers(const struct List *theList);
static ULONG CountMenus(void);
static ULONG CountCurrentToolMenuItems(void);
static void SetMenuOnOffProc(struct ScaWindowTask *iwt, APTR args);

//----------------------------------------------------------------------------

void AppMenu_Init(void)
{
	NewList(&ToolsMenuItemList);
	NewList(&AppMenuList);
}


BOOL AppMenu_AddItem(struct AppMenuInfo *ami)
{
	BOOL Success = TRUE;

	if (NULL == ami)
		return FALSE;

	ScalosObtainSemaphore(&AppMenuListSemaphore);

	d1(kprintf("%s/%s/%ld: ami=%08lx  nm_Type=%ld\n", __FILE__, __FUNC__, __LINE__, ami, ami->ami_NewMenu.nm_Type));

	switch (ami->ami_NewMenu.nm_Type)
		{
	case NM_TITLE:
		// add new menu
		if (ami->ami_Parent)
			Success = FALSE;

		if (CountMenus() >= (NOMENU - 1))
			{
			// no more than NOMENU menus are allowed
			Success = FALSE;
			}

		if (Success)
			AddTail(&AppMenuList, &ami->ami_Node);
		break;

	case NM_ITEM:
		if (ami->ami_Parent)
			{
			// this is a child of a (non-tools) menu
			if (CountListMembers(&ami->ami_Parent->ami_ChildList) >= (NOITEM - 1))
				{
				// no more than NOITEM menu items are allowed
				Success = FALSE;
				}

			if (Success)
				AddTail(&ami->ami_Parent->ami_ChildList, &ami->ami_Node);
			}
		else
			{
			// add standard ToolMenu item
			// check here for no more than NOITEM entries in tools menu
			if (CountCurrentToolMenuItems() >= (NOITEM - 1))
				{
				Success = FALSE;
				}
			else
				AddTail(&ToolsMenuItemList, &ami->ami_Node);
			}
		break;

	case NM_SUB:
		if (CountListMembers(&ami->ami_Parent->ami_ChildList) >= (NOSUB - 1))
			{
			// no more than NOSUB subitems are allowed
			Success = FALSE;
			}

		if (Success)
			{
			// add new AppMenuInfo to parent's child list
			AddTail(&ami->ami_Parent->ami_ChildList, &ami->ami_Node);
			}
		break;
		}

	ScalosReleaseSemaphore(&AppMenuListSemaphore);

	return Success;
}


BOOL AppMenu_RemoveItem(struct AppMenuInfo *ami)
{
	BOOL Success = TRUE;

	if (NULL == ami)
		return FALSE;

	ScalosObtainSemaphore(&AppMenuListSemaphore);

	Remove(&ami->ami_Node);

	ScalosReleaseSemaphore(&AppMenuListSemaphore);

	return Success;
}


void AppMenu_DisposeAppMenuInfo(struct AppMenuInfo *ami)
{
	if (ami)
		{
		ami->ami_Magic = 0;

		if (ami->ami_NewMenu.nm_CommKey)
			FreeCopyString((STRPTR)ami->ami_NewMenu.nm_CommKey);
		if (ami->ami_NewMenu.nm_Label && NM_BARLABEL != ami->ami_NewMenu.nm_Label)
			FreeCopyString((STRPTR)ami->ami_NewMenu.nm_Label);

		ScalosFree(ami);
		}
}


void AppMenu_CreateMenu(void)
{
	struct MenuInfo *mInfo;
	struct NewMenu *nmArray;

	d1(kprintf("%s/%s/%ld: MainMenu=%08lx  MainNewMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu, MainNewMenu));

	if (NULL == MainNewMenu)
		return;

	ScalosObtainSemaphore(&ParseMenuListSemaphore);

	ClearMainMenu();

	SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &ParseMenuList);

	ScalosReleaseSemaphore(&ParseMenuListSemaphore);

	nmArray = CreateNewMenuArray();

	d1(kprintf("%s/%s/%ld: nmArray=%08lx\n", __FILE__, __FUNC__, __LINE__, nmArray));
	if (nmArray)
		{
		MainMenu = CreateMenusA(nmArray, NULL);
		ScalosFree(nmArray);
		}
	else
		{
		MainMenu = CreateMenusA(MainNewMenu, NULL);
		}

	d1(kprintf("%s/%s/%ld: MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu));

	ParseMenu(MainMenu);

	d1(kprintf("%s/%s/%ld: ParseMenuList=%08lx\n", __FILE__, __FUNC__, __LINE__, ParseMenuList));

	for (mInfo=ParseMenuList; mInfo; mInfo = (struct MenuInfo *) mInfo->minf_Node.mln_Succ)
		{
		if (MITYPEF_Backdrop & mInfo->minf_type)
			{
			if (CurrentPrefs.pref_WBBackdrop)
				mInfo->minf_item->Flags |= CHECKED;
			else
				mInfo->minf_item->Flags &= ~CHECKED;
			break;
			}
		}

	if (MainMenu)
		{
		LayoutMenus(MainMenu, iInfos.xii_iinfos.ii_visualinfo,
			GTMN_NewLookMenus, TRUE,
			TAG_END);
		}
}


void AppMenu_ResetMenu(void)
{
	struct ScaWindowStruct *wsAct;

	ClearAllMenus();

	ScalosObtainSemaphore(&MenuSemaphore);

	d1(kprintf("%s/%s/%ld: MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu));
	AppMenu_CreateMenu();

	SetAllMenus();

	ScalosReleaseSemaphore(&MenuSemaphore);

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	wsAct = FindActiveScalosWindow();
	if (wsAct)
		{
		struct internalScaWindowTask *iwt;

		iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;

		if (wsAct->ws_WindowTask == (struct ScaWindowTask *) iwt)
			SetMenuOnOff(iwt);
		else
			{
			struct SM_AsyncRoutine *smar;

			smar = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine, 0);
			if (smar)
				{
				smar->smar_EntryPoint = SetMenuOnOffProc;

				PutMsg(wsAct->ws_MessagePort, &smar->ScalosMessage.sm_Message);
				}
			}
		}

	SCA_UnLockWindowList();
}


static struct NewMenu *CreateNewMenuArray(void)
{
	struct NewMenu *nm, *nmArray;
	struct NewMenu *nmInsert = NULL;
	ULONG StdMenuItems = 0;
	ULONG ToolMenuItems;
	ULONG ndx;
	BOOL Checking;

	d1(kprintf("\n" "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	for (nm=MainNewMenu; NM_END != nm->nm_Type; nm++)
		StdMenuItems++;

	d1(kprintf("%s/%s/%ld: StdMenuItems=%ld\n", __FILE__, __FUNC__, __LINE__, StdMenuItems));

	// find insertion point for app menus - just after end of Tools menu
	for (nm=MainNewMenu, Checking=FALSE; NM_END != nm->nm_Type; nm++)
		{
		if (USERDATA_TOOLSMENU == nm->nm_UserData)
			{
			// Start of tools menu
			Checking = TRUE;
			}
		else
			{
			if (Checking)
				{
				nmInsert = nm;

				switch (nm->nm_Type)
					{
				case NM_TITLE:
				case NM_END:
					Checking = FALSE;
					break;
					}
				}
			}
		}
	if (NULL == nmInsert)
		nmInsert = nm;	// no entry after tools menu

	d1(kprintf("%s/%s/%ld: nmInsert=%08lx\n", __FILE__, __FUNC__, __LINE__, nmInsert));

	ScalosObtainSemaphoreShared(&AppMenuListSemaphore);

	ToolMenuItems = CountToolMenuItems(&ToolsMenuItemList) + CountToolMenuItems(&AppMenuList);

	d1(kprintf("%s/%s/%ld: ToolMenuItems=%lu\n", __FILE__, __FUNC__, __LINE__, ToolMenuItems));

	nmArray = ScalosAlloc((1 + StdMenuItems + ToolMenuItems) * sizeof(struct NewMenu));

	d1(kprintf("%s/%s/%ld: nmArray=%08lx\n", __FILE__, __FUNC__, __LINE__, nmArray));

	if (nmArray)
		{
		for (ndx=0, nm=MainNewMenu; NM_END != nm->nm_Type; nm++)
			{
			if (nmInsert == nm)
				{
				InsertAppMenuItems(nmArray, &ndx, &ToolsMenuItemList);
				InsertAppMenuItems(nmArray, &ndx, &AppMenuList);
				}

			nmArray[ndx++] = *nm;
			}

		nmArray[ndx].nm_Type = NM_END;

		d1(kprintf("%s/%s/%ld: ndx=%lu\n", __FILE__, __FUNC__, __LINE__, ndx));
		}

	ScalosReleaseSemaphore(&AppMenuListSemaphore);

	return nmArray;
}


static ULONG CountToolMenuItems(struct List *MenuInfoList)
{
	struct AppMenuInfo *ami;
	ULONG Count = 0;

	for (ami = (struct AppMenuInfo *) MenuInfoList->lh_Head;
		ami != (struct AppMenuInfo *) &MenuInfoList->lh_Tail;
		ami = (struct AppMenuInfo *) ami->ami_Node.ln_Succ)
		{
		Count++;
		Count += CountToolMenuItems(&ami->ami_ChildList);
		}

	return Count;
}


static void InsertAppMenuItems(struct NewMenu *nmArray, ULONG *ndx, struct List *MenuInfoList)
{
	struct AppMenuInfo *ami;

	for (ami = (struct AppMenuInfo *) MenuInfoList->lh_Head;
		ami != (struct AppMenuInfo *) &MenuInfoList->lh_Tail;
		ami = (struct AppMenuInfo *) ami->ami_Node.ln_Succ)
		{
		nmArray[*ndx] = ami->ami_NewMenu;
		(*ndx)++;

		InsertAppMenuItems(nmArray, ndx, &ami->ami_ChildList);
		}
}


static void ClearAllMenus(void)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		d1(kprintf("%s/%s/%ld: ws=%08lx <%s>  Window=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, ws, ws->ws_Name ? ws->ws_Name : (STRPTR) "", ws->ws_Window));
		if (ws->ws_Window)
			SafeClearMenuStrip(ws->ws_Window);
		}

	SCA_UnLockWindowList();
}


static void SetAllMenus(void)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws->ws_Window)
			SafeSetMenuStrip(ws->ws_Window);
		}

	SCA_UnLockWindowList();
}


static ULONG CountListMembers(const struct List *theList)
{
	struct Node *theNode;
	ULONG Count = 0;

	for (theNode = theList->lh_Head; 
		theNode != (struct Node *) &theList->lh_Tail;
		theNode = theNode->ln_Succ)
		{
		Count++;
		}

	return Count;
}


static ULONG CountMenus(void)
{
	struct Menu *theMenu;
	ULONG Count = 0;

	for (theMenu=MainMenu; theMenu; theMenu=theMenu->NextMenu)
		{
		Count++;
		}

	return Count;
}


static ULONG CountCurrentToolMenuItems(void)
{
	struct Menu *theMenu;
	ULONG Count = 0;

	for (theMenu=MainMenu; theMenu; theMenu=theMenu->NextMenu)
		{
		if (USERDATA_TOOLSMENU == GTMENU_USERDATA(theMenu))
			{
			struct MenuItem *theItem;

			for (theItem=theMenu->FirstItem; theItem; theItem=theItem->NextItem)
				Count++;

			break;
			}
		}

	return Count;
}

// this is only a wrapper with correct parameters to use with SM_AsyncRoutine
static void SetMenuOnOffProc(struct ScaWindowTask *iwt, APTR args)
{
	SetMenuOnOff((struct internalScaWindowTask *) iwt);
}


// Note: the windowlist should be locked
// Output: active ScaWindowStruct or NULL
struct ScaWindowStruct *FindActiveScalosWindow(void)
{
	struct ScaWindowStruct *ws;
	struct ScaWindowStruct *wsFound = NULL;
	ULONG iLock = ScaLockIBase(0);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws->ws_Window == IntuitionBase->ActiveWindow)
			{
			wsFound = ws;
			break;
			}
		}

	ScaUnlockIBase(iLock);

	return wsFound;
}
