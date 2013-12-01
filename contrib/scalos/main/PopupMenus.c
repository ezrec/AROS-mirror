// PopupMenus.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/pm.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#ifdef __AROS__
#include <proto/popupmenu.h>
#else
#include <proto/pm.h>
#endif
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosmenuplugin.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/menu.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

struct SelectedIcon
{
	struct Node sli_Node;

	struct ScaIconNode *sli_Icon;
	struct internalScaWindowTask *sli_WindowTask;
};

struct LockedWindowEntry
{
	struct Node lwe_Node;

	struct internalScaWindowTask *lwe_WindowTask;
};

//----------------------------------------------------------------------------

// local functions

static void SetPopupMenuAfterIconSupportFlags(struct internalScaWindowTask *iwt, 
	struct PopupMenu *pm, struct ScaIconNode *in);
static void SetPopupMenuAfterWindowFlags(struct PopupMenu *pm, struct internalScaWindowTask *iwt);
static void SetPopupMenuCommandAttributes(struct internalScaWindowTask *iwt,
	const struct ScalosMenuTree *mTree, struct PopupMenu *pm,
	struct ScaIconNode *in, const struct IconMenuSupports *CmdTable, ULONG Flags);
static SAVEDS(CONST_STRPTR) PMGetStringFunc(struct Hook *theHook, ULONG dummy, ULONG *args);
static void ShowIconPopupMenu(struct internalScaWindowTask *iwt, struct msg_ShowPopupMenu *mpm);
static void ShowWindowPopupMenu(struct internalScaWindowTask *iwt, struct msg_ShowPopupMenu *mpm);
static ULONG CheckPopupMenu(struct internalScaWindowTask *iwt, struct ScaIconNode *in,
	const struct ScalosMenuTree *mTree, ULONG *Checkit, ULONG *Checked);
static void CreateSelectedIconList(struct List *SelectedIconList, 
	struct List *LockedWindowList, struct ScaIconNode *IconUnderPointer);
static void CleanupSelectedIconList(struct List *SelectedIconList, struct List *LockedWindowList);
static BOOL isPopupApplyToSelectedQualifier(ULONG Qualifier);

//----------------------------------------------------------------------------

// local data items

//----------------------------------------------------------------------------

// public data items

struct PopupMenu *PopupMenus[SCPOPUPMENU_Last];		// Array of Popup Menus

struct Hook PMGetStringHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(PMGetStringFunc),
	NULL
	};

//----------------------------------------------------------------------------

// Result :
//  0 - No popup menu
//  1 - Popup menu ok
// -1 - something is wrong
LONG TestPopup(struct internalScaWindowTask *iwt, WORD MouseX, WORD MouseY, UWORD Qualifier)
{
	struct internalScaWindowTask *iwtUnderPointer = NULL;
	struct ScaIconNode *inUnderPointer = NULL;
	struct Window *win;
	LONG Result = 0;

	do	{
		struct PopupMenu *pm = NULL;
		struct SM_ShowPopupMenu *smpm;

		d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

		ResetToolTips(iwt);

		if (NULL == PopupMenuBase || !PopupMenuFlag)
			{
			d1(KPrintF("%s/%s/%ld: PopupMenuBase=%08lx  PopupMenuFlag=%ld\n", __FILE__, __FUNC__, __LINE__, PopupMenuBase, PopupMenuFlag));
			Result = -1;
			break;
			}

		QueryObjectUnderPointer(&iwtUnderPointer, &inUnderPointer, NULL, &win);

		d1(KPrintF("%s/%s/%ld: iwt=%08lx  inUnderPointer=%08lx  win=%08lx\n", __FILE__, __FUNC__, __LINE__, iwtUnderPointer, inUnderPointer, win));

		if (NULL == iwtUnderPointer)
			break;

		if (inUnderPointer)
			{
			// Pointer is over icon
			struct FileTypeDef *ftd;

			Result = 1;

			ftd = FindFileType(iwtUnderPointer, inUnderPointer);
			d1(KPrintF("%s/%s/%ld: ftd=%08lx  ftd_PopupMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd, ftd ? ftd->ftd_PopupMenu : NULL));
			if (ftd && ftd->ftd_PopupMenu)
				{
				pm = ftd->ftd_PopupMenu;

				d1(kprintf("%s/%s/%ld: pm=%08lx\n", __FILE__, __FUNC__, __LINE__, pm));
				PM_LayoutMenuA(iwt->iwt_WindowTask.wt_Window, pm, NULL);
				}
			else
				{
				ULONG IconType;

				GetAttr(IDTA_Type, inUnderPointer->in_Icon, &IconType);

				d1(KPrintF("%s/%s/%ld: inUnderPointer=%08lx  <%s>   Type=%ld\n", __FILE__, __FUNC__, __LINE__, \
					inUnderPointer, inUnderPointer->in_Name ? inUnderPointer->in_Name : (STRPTR) "", IconType));

				switch (IconType)
					{
				case WBDISK:
				case WBKICK:
					pm = PopupMenus[SCPOPUPMENU_Disk];
					break;

				case WBDRAWER:
				case WBDEVICE:
				case WB_TEXTICON_DRAWER:
					pm = PopupMenus[SCPOPUPMENU_Drawer];
					break;

				case WBTOOL:
				case WBPROJECT:
				case WB_TEXTICON_TOOL:
					pm = PopupMenus[SCPOPUPMENU_Tool];
					break;

				case WBAPPICON:
					if (inUnderPointer->in_SupportFlags)
						pm = PopupMenus[SCPOPUPMENU_AppIcon];
					break;

				case WBGARBAGE:
					pm = PopupMenus[SCPOPUPMENU_Trashcan];
					break;

				default:
					break;
					}
				}

			d1(KPrintF("%s/%s/%ld: pm=%08lx\n", __FILE__, __FUNC__, __LINE__, pm));

			if (pm)
				{
				if (iwtUnderPointer->iwt_WindowTask.wt_Window
						&& !(iwtUnderPointer->iwt_WindowTask.wt_Window->Flags & WFLG_WINDOWACTIVE))
					{
					iwtUnderPointer->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_ActivatePending;

					ActivateWindow(iwtUnderPointer->iwt_WindowTask.wt_Window);
					}

				smpm = (struct SM_ShowPopupMenu *) SCA_AllocMessage(MTYP_ShowPopupMenu, 0);

				if (smpm)
					{
					smpm->ScalosMessage.sm_Message.mn_ReplyPort = iwt->iwt_WindowTask.wt_IconPort;
					smpm->smpm_PopupMenu = pm;
					smpm->smpm_IconNode = inUnderPointer;
					smpm->smpm_FileType = ftd;
					smpm->smpm_Flags = SHOWPOPUPFLGF_IconSemaLocked | SHOWPOPUPFLGF_WinListLocked;
					smpm->smpm_Qualifier = Qualifier;

					ftd = NULL;	// don't release it here !

					d1(KPrintF("%s/%s/%ld: inUnderPointer=%08lx  iwtUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, inUnderPointer, iwtUnderPointer));

					PutMsg(iwtUnderPointer->iwt_WindowTask.wt_IconPort, &smpm->ScalosMessage.sm_Message);

					// WinList and IconSemaphore will be unlocked by SCCM_IconWin_ShowPopupMenu
					iwtUnderPointer	= NULL;
					inUnderPointer = NULL;
					}
				}

			d1(KPrintF("%s/%s/%ld: inUnderPointer=%08lx  iwtUnderPointer=%08lx\n", __FILE__, __FUNC__, __LINE__, inUnderPointer, iwtUnderPointer));

			ReleaseFileType(ftd);
			}
		else
			{
			// Pointer over Scalos window
			struct internalScaWindowTask *iwtMain = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
			WORD x, y;

			x = MouseX + iwt->iwt_WindowTask.wt_Window->LeftEdge - iwtUnderPointer->iwt_WindowTask.wt_Window->LeftEdge - iwtUnderPointer->iwt_InnerLeft;
			y = MouseY + iwt->iwt_WindowTask.wt_Window->TopEdge - iwtUnderPointer->iwt_WindowTask.wt_Window->TopEdge - iwtUnderPointer->iwt_InnerTop;

			d1(KPrintF("%s/%s/%ld: x=%ld  y=%ld  pref_FullPopupFlag=%ld\n", __FILE__, __FUNC__, __LINE__, x, y, CurrentPrefs.pref_FullPopupFlag));

			pm = PopupMenus[(iwtUnderPointer == iwtMain) ? SCPOPUPMENU_Desktop : SCPOPUPMENU_Window];

			if ((y < 0 || !CurrentPrefs.pref_FullPopupFlag || (iwtUnderPointer == iwtMain)) && x >= 0
				&& x < iwtUnderPointer->iwt_WindowTask.wt_Window->Width
				&& y < iwtUnderPointer->iwt_WindowTask.wt_Window->Height
				&& pm)
				{
				Result = 1;

				if (iwtUnderPointer->iwt_WindowTask.wt_Window
						&& !(iwtUnderPointer->iwt_WindowTask.wt_Window->Flags & WFLG_WINDOWACTIVE))
					{
					iwtUnderPointer->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_ActivatePending;

					ActivateWindow(iwtUnderPointer->iwt_WindowTask.wt_Window);
					}

				smpm = (struct SM_ShowPopupMenu *) SCA_AllocMessage(MTYP_ShowPopupMenu, 0);

				if (smpm)
					{
					smpm->ScalosMessage.sm_Message.mn_ReplyPort = iwt->iwt_WindowTask.wt_IconPort;

					smpm->smpm_PopupMenu = pm;
					smpm->smpm_IconNode = NULL;
					smpm->smpm_FileType = NULL;
					smpm->smpm_Flags = SHOWPOPUPFLGF_WinListLocked;
					smpm->smpm_Qualifier = Qualifier;

					PutMsg(iwtUnderPointer->iwt_WindowTask.wt_IconPort, &smpm->ScalosMessage.sm_Message);
					}

				// WinList and IconSemaphore will be unlocked by SCCM_IconWin_ShowPopupMenu
				iwtUnderPointer = NULL;
				}
			}
		} while (0);

	if (iwtUnderPointer)
		{
		if (inUnderPointer)
			ScalosUnLockIconList(iwtUnderPointer);
		SCA_UnLockWindowList();
		}

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static void SetPopupMenuAfterIconSupportFlags(struct internalScaWindowTask *iwt, 
	struct PopupMenu *pm, struct ScaIconNode *in)
{
	ULONG IconType;

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	while (pm)
		{
		struct PopupMenu *pmSub = NULL;
		struct ScalosMenuTree *mTree = NULL;

		PM_GetItemAttrs(pm,
			PM_UserData, (ULONG) &mTree,
			TAG_END);

		if (mTree)
			{
			d1(kprintf("%s/%s/%ld: mTree=%08lx  Type=%ld\n", __FILE__, __FUNC__, __LINE__, mTree, mTree->mtre_type));

			if (SCAMENUTYPE_Command == mTree->mtre_type)
				SetPopupMenuCommandAttributes(iwt, mTree, pm, in, IconSupportTable, in->in_SupportFlags);
			}

		PM_GetItemAttrs(pm,
			PM_Sub, (ULONG) &pmSub,
			TAG_END);
		if (pmSub)
			SetPopupMenuAfterIconSupportFlags(iwt, pmSub, in);

		pm = pm->Next;
		}
}


static void SetPopupMenuAfterWindowFlags(struct PopupMenu *pm, struct internalScaWindowTask *iwt)
{
	ULONG WindowFlags = GetWindowMenuFlags(iwt);

	while (pm)
		{
		struct PopupMenu *pmSub = NULL;
		struct ScalosMenuTree *mTree = NULL;

		PM_GetItemAttrs(pm,
			PM_UserData, (ULONG) &mTree,
			TAG_END);

		if (mTree)
			{
			d1(kprintf("%s/%s/%ld: mTree=%08lx  Type=%ld\n", __FILE__, __FUNC__, __LINE__, mTree, mTree->mtre_type));

			if (SCAMENUTYPE_Command == mTree->mtre_type)
				SetPopupMenuCommandAttributes(iwt, mTree, pm, NULL, WindowMenuTable, WindowFlags);
			}

		PM_GetItemAttrs(pm,
			PM_Sub, (ULONG) &pmSub,
			TAG_END);
		if (pmSub)
			SetPopupMenuAfterWindowFlags(pmSub, iwt);

		pm = pm->Next;
		}
}


static void SetPopupMenuCommandAttributes(struct internalScaWindowTask *iwt,
	const struct ScalosMenuTree *mTree, struct PopupMenu *pm, struct ScaIconNode *in,
	const struct IconMenuSupports *CmdTable, ULONG Flags)
{
	if (SCAMENUTYPE_Command == mTree->mtre_type)
		{
		ULONG Checkit = FALSE, Checked = FALSE;

		d1(kprintf("%s/%s/%ld: mTree=%08lx  name=<%s>  type=%ld\n", __FILE__, __FUNC__, __LINE__, \
			mTree, mTree->MenuCombo.MenuCommand.mcom_name, mTree->MenuCombo.MenuCommand.mcom_type));

		if (SCAMENUCMDTYPE_Command == mTree->MenuCombo.MenuCommand.mcom_type)
			{
			ULONG Disabled = CheckPopupMenu(iwt, in, mTree, &Checkit, &Checked);
			ULONG n;

			if (0 == Stricmp("undo", mTree->MenuCombo.MenuCommand.mcom_name))
				{
				PM_SetItemAttrs(pm,
					PM_Title, (ULONG) UndoGetDescription(),
					TAG_END);
				}
			else if (0 == Stricmp("redo", mTree->MenuCombo.MenuCommand.mcom_name))
				{
				PM_SetItemAttrs(pm,
					PM_Title, (ULONG) RedoGetDescription(),
					TAG_END);
				}

			for (n=0; CmdTable[n].ims_CommandName; n++)
				{
				if (0 == Stricmp((STRPTR) CmdTable[n].ims_CommandName, mTree->MenuCombo.MenuCommand.mcom_name))
					{
					d1(kprintf("%s/%s/%ld: name=<%s>\n", __FILE__, __FUNC__, __LINE__, mTree->MenuCombo.MenuCommand.mcom_name));

					d1(kprintf("%s/%s/%ld: Flags=%08lx  FlagMask=%08lx\n", __FILE__, __FUNC__, __LINE__, \
						Flags, CmdTable[n].ims_FlagMask));

					Disabled = Disabled || (CmdTable[n].ims_FlagMask != (Flags & CmdTable[n].ims_FlagMask));
					break;
					}
				}

			// PM_SetItemAttrsA(
			PM_SetItemAttrs(pm,
				PM_Disabled, Disabled,
				PM_Checkit, Checkit,
				PM_Checked, Checked,
				TAG_END);
			}
		}
}


static SAVEDS(CONST_STRPTR) PMGetStringFunc(struct Hook *theHook, ULONG dummy, ULONG *args)
{
	return GetLocString(*args);
}


void ShowPopupMenu(struct internalScaWindowTask *iwt, struct msg_ShowPopupMenu *mpm)
{
	d1(kprintf("%s/%s/%ld: mpm_IconNode=%08lx  mpm_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, mpm->mpm_IconNode, mpm->mpm_Flags));

	if (mpm->mpm_IconNode)
		{
		// Display icon popup menu
		ShowIconPopupMenu(iwt, mpm);
		}
	else
		{
		// Display Window popup menu
		ShowWindowPopupMenu(iwt, mpm);
		}

	if (mpm->mpm_Flags & SHOWPOPUPFLGF_FreePopupMenu)
		PM_FreePopupMenu(mpm->mpm_PopupMenu);

	ReleaseFileType(mpm->mpm_FileType);
}


static void ShowIconPopupMenu(struct internalScaWindowTask *iwt, struct msg_ShowPopupMenu *mpm)
{
	struct List SelectedIconList;
	struct List LockedWindowList;
	struct PopupMenu *pmTitle2 = NULL;
	struct ScaIconNode *inUnderMouse = mpm->mpm_IconNode;
	Object *IconObj = mpm->mpm_IconNode->in_Icon;
	struct ExtGadget *gg = (struct ExtGadget *) mpm->mpm_IconNode->in_Icon;
	struct PopupMenu *pmTitle;
	struct ScalosMenuTree *mtr;
	struct SelectedIcon *sli;
	ULONG wasSelected;
	ULONG SelectedIconCount = 0;
	BOOL ApplyToAllSelectedIcons;

	NewList(&SelectedIconList);
	NewList(&LockedWindowList);

	ApplyToAllSelectedIcons = isPopupApplyToSelectedQualifier(mpm->mpm_Qualifier);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_GetIconFileType, mpm->mpm_IconNode);

	wasSelected = gg->Flags & GFLG_SELECTED;

	d1(KPrintF("%s/%s/%ld: pm=%08lx  mpm_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, mpm->mpm_PopupMenu, mpm->mpm_Flags));

	if (mpm->mpm_IconNode)
		SelectedIconCount++;

	CreateSelectedIconList(&SelectedIconList, &LockedWindowList, mpm->mpm_IconNode);

	for (sli = (struct SelectedIcon *) SelectedIconList.lh_Head;
		sli != (struct SelectedIcon *) &SelectedIconList.lh_Tail;
		sli = (struct SelectedIcon *) sli->sli_Node.ln_Succ)
		{
		if (ApplyToAllSelectedIcons)
			{
			DisplayIconDropMark(sli->sli_WindowTask, sli->sli_Icon);	// Show Drop Mark
			}
		else
			{
			// Temporarily deselect all icons except the one under the mouse pointer
			ClassSelectIcon(sli->sli_WindowTask->iwt_WindowTask.mt_WindowStruct, sli->sli_Icon, FALSE);
			}
		SelectedIconCount++;
		}

	// Show Drop Mark on icon under mouse pointer
	DisplayIconDropMark(iwt, mpm->mpm_IconNode);

	// Make sure icon under mouse pointer is selected
	if (!wasSelected)
		ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, mpm->mpm_IconNode, TRUE);

	pmTitle = PM_FindItem(mpm->mpm_PopupMenu, PM_TITLE_ID);
	if (pmTitle)
		{
		CONST_STRPTR MenuTitle = GetIconName(mpm->mpm_IconNode);

		if (strlen(MenuTitle) > 0)
			{
			d1(kprintf("%s/%s/%ld: in_Name=<%s> %08lx\n", \
				__FILE__, __FUNC__, __LINE__, mpm->mpm_IconNode->in_Name, mpm->mpm_IconNode->in_Name));

			pmTitle2 = PM_MakeItem(PM_Title, (ULONG) MenuTitle,
				PM_NoSelect, TRUE,
				PM_ShinePen, TRUE,
				PM_Shadowed, TRUE,
				PM_Center, TRUE,
				TAG_END);

			if (pmTitle2)
				{
				PM_InsertMenuItem(mpm->mpm_PopupMenu,
					PM_Insert_After, (ULONG) pmTitle,
					PM_Insert_Item, (ULONG) pmTitle2,
					TAG_END);
				}
			}
		}

	SetPopupMenuAfterIconSupportFlags(iwt, mpm->mpm_PopupMenu, mpm->mpm_IconNode);

	ScalosObtainSemaphore(&LayersSema);

	// PM_OpenPopupMenuA()
	mtr = (struct ScalosMenuTree *) PM_OpenPopupMenu(iwt->iwt_WindowTask.wt_Window,
		PM_Menu, (ULONG) mpm->mpm_PopupMenu,
		PM_LocaleHook, (ULONG) &PMGetStringHook,
		TAG_END);

	d1(KPrintF("%s/%s/%ld: mtr=%08lx\n", __FILE__, __FUNC__, __LINE__, mtr));

	ScalosReleaseSemaphore(&LayersSema);

	// Erase Drop Mark on icon under mouse pointer
	EraseIconDropMark(iwt, mpm->mpm_IconNode);

	// Deselect icon under mouse pointer if is wasn't selected initially
	if (!ApplyToAllSelectedIcons && !wasSelected)
		ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, mpm->mpm_IconNode, FALSE);

	for (sli = (struct SelectedIcon *) SelectedIconList.lh_Head;
		sli != (struct SelectedIcon *) &SelectedIconList.lh_Tail;
		sli = (struct SelectedIcon *) sli->sli_Node.ln_Succ)
		{
		if (ApplyToAllSelectedIcons)
			{
			// Erase Drop Mark
			EraseIconDropMark(sli->sli_WindowTask, sli->sli_Icon);
			}
		else
			{
			// Reselect all previously selected icons
			ClassSelectIcon(sli->sli_WindowTask->iwt_WindowTask.mt_WindowStruct, sli->sli_Icon, TRUE);
			}
		}

	CleanupSelectedIconList(&SelectedIconList, &LockedWindowList);

	if (pmTitle2)
		{
		PM_RemoveMenuItem(mpm->mpm_PopupMenu, pmTitle2);
		PM_FreePopupMenu(pmTitle2);
		}

	d1(KPrintF("%s/%s/%ld: mtr=%08lx  mpm_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, mtr, mpm->mpm_Flags));

	if (mpm->mpm_Flags & SHOWPOPUPFLGF_WinListLocked)
		{
		d1(kprintf("%s/%s/%ld: SCA_UnLockWindowList \n", __FILE__, __FUNC__, __LINE__));
		SCA_UnLockWindowList();
		}
	if (mpm->mpm_Flags & SHOWPOPUPFLGF_IconSemaLocked)
		{
		d1(kprintf("%s/%s/%ld: Release iconsemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
		ScalosUnLockIconList(iwt);
		}

	if (mtr)
		{
		d1(kprintf("%s/%s/%ld: ApplyToAllSelectedIcons=%ld  SelectedIconCount=%ld\n", __FILE__, __FUNC__, __LINE__, ApplyToAllSelectedIcons, SelectedIconCount));

		if (ApplyToAllSelectedIcons && (SelectedIconCount > 1))
			RunMenuCommandExt(iwt, iwt, mtr, NULL, 0);
		else
			RunMenuCommandExt(iwt, iwt, mtr, mpm->mpm_IconNode, 0);
		}

	// Deselect icon under mouse pointer if is wasn't selected initially
	// if ApplyToAllSelectedIcons, the icon needs to stay selected until the
	// menu command is applied
	if (ApplyToAllSelectedIcons && !wasSelected)
		{
		// Unfortunately, mpm->mpm_IconNode might have got invalid
		// during execution of the menu command.
		// So we need to check if itäs still valid:
		struct ScaIconNode *in;
		BOOL Found = FALSE;

		ScalosLockIconListShared(iwt);

		for (in = iwt->iwt_WindowTask.wt_IconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_Icon == IconObj)
				{
				Found = TRUE;
				}
			}
		for (in = iwt->iwt_WindowTask.wt_LateIconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			if (in->in_Icon == IconObj)
				{
				Found =	TRUE;
				}
			}

		if (Found)
			ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, inUnderMouse, FALSE);

		ScalosUnLockIconList(iwt);
		}
}


static void ShowWindowPopupMenu(struct internalScaWindowTask *iwt, struct msg_ShowPopupMenu *mpm)
{
	struct PopupMenu *pmTitle2 = NULL;
	struct ScalosMenuTree *mtr;
	struct PopupMenu *pmTitle;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	pmTitle = PM_FindItem(mpm->mpm_PopupMenu, PM_TITLE_ID);
	if (pmTitle && iwt->iwt_WindowTask.mt_WindowStruct->ws_Name)
		{
		pmTitle2 = PM_MakeItem(PM_Title, (ULONG) iwt->iwt_WindowTask.mt_WindowStruct->ws_Name,
			PM_NoSelect, TRUE,
			PM_ShinePen, TRUE,
			PM_Shadowed, TRUE,
			PM_Center, TRUE,
			TAG_END);

		if (pmTitle2)
			{
			PM_InsertMenuItem(mpm->mpm_PopupMenu,
				PM_Insert_After, (ULONG) pmTitle,
				PM_Insert_Item, (ULONG) pmTitle2,
				TAG_END);
			}
		}

	SetPopupMenuAfterWindowFlags(mpm->mpm_PopupMenu, iwt);

	ScalosObtainSemaphore(&LayersSema);

	// PM_OpenPopupMenuA()
	mtr = (struct ScalosMenuTree *) PM_OpenPopupMenu(iwt->iwt_WindowTask.wt_Window,
		PM_Menu, (ULONG) mpm->mpm_PopupMenu,
		PM_LocaleHook, (ULONG) &PMGetStringHook,
		TAG_END);

	ScalosReleaseSemaphore(&LayersSema);

	if (pmTitle2)
		{
		PM_RemoveMenuItem(mpm->mpm_PopupMenu, pmTitle2);
		PM_FreePopupMenu(pmTitle2);
		}

	d1(kprintf("%s/%s/%ld: before RunMenuCommand, mtr=%08lx  mpm_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, mtr, mpm->mpm_Flags));

	if (mpm->mpm_Flags & SHOWPOPUPFLGF_WinListLocked)
		SCA_UnLockWindowList();
	if (mpm->mpm_Flags & SHOWPOPUPFLGF_IconSemaLocked)
		ScalosUnLockIconList(iwt);

	if (mtr)
		RunMenuCommandExt(iwt, iwt, mtr, NULL, 0);

	d1(KPrintF("%s/%s/%ld: after RunMenuCommand\n", __FILE__, __FUNC__, __LINE__));
}


static ULONG CheckPopupMenu(struct internalScaWindowTask *iwt, struct ScaIconNode *in,
	 const struct ScalosMenuTree *mTree, ULONG *Checkit, ULONG *Checked)
{
	ULONG Disabled = FALSE;
	struct ScalosMenuCommand *mpi;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

	*Checkit = FALSE;
	*Checked = FALSE;

	for (mpi = &MenuCommandTable[0]; mpi->smcom_CommandName; mpi++)
		{
		if (CompareCommand(mTree->MenuCombo.MenuCommand.mcom_name, mpi->smcom_CommandName))
			{
			if (mpi->smcom_ParseFlags & MITYPEF_ViewByIcon)
				{
				*Checkit = TRUE;
				*Checked = IDTV_ViewModes_Icon == ws->ws_Viewmodes;
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ViewByText)
				{
				*Checkit = TRUE;
				*Checked = IDTV_ViewModes_Name == ws->ws_Viewmodes;
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ViewByDefault)
				{
				*Checkit = TRUE;
				*Checked = IDTV_ViewModes_Default == ws->ws_Viewmodes;
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ViewByDate)
				{
				*Checkit = TRUE;
				*Checked = IDTV_ViewModes_Date == ws->ws_Viewmodes;
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ViewBySize)
				{
				*Checkit = TRUE;
				*Checked = IDTV_ViewModes_Size == ws->ws_Viewmodes;
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ViewByType)
				{
				*Checkit = TRUE;
				*Checked = IDTV_ViewModes_Type == ws->ws_Viewmodes;
				}

			if (mpi->smcom_ParseFlags & MITYPEF_ShowDefault)
				{
				*Checkit = TRUE;
				*Checked = DDFLAGS_SHOWDEFAULT == (DDFLAGS_SHOWMASK & ws->ws_ViewAll);
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ShowOnlyIcons)
				{
				*Checkit = TRUE;
				*Checked = DDFLAGS_SHOWICONS == ws->ws_ViewAll;
				}
			if (mpi->smcom_ParseFlags & MITYPEF_ShowAllFiles)
				{
				*Checkit = TRUE;
				*Checked = DDFLAGS_SHOWALL == ws->ws_ViewAll;
				}

			if (mpi->smcom_Enable)
				Disabled = !(*mpi->smcom_Enable)(iwt, in);

			d1(KPrintF("%s/%s/%ld: mpi->smcom_CommandName=<%s>  Disabled=%ld\n", __FILE__, __FUNC__, __LINE__, mpi->smcom_CommandName, Disabled));
			break;
			}
		}

	return Disabled;
}


static void CreateSelectedIconList(struct List *SelectedIconList, 
	struct List *LockedWindowList, struct ScaIconNode *IconUnderPointer)
{
	struct ScaWindowStruct *ws;
	ULONG TotalIconCount = 0;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

		d1(kprintf("%s/%s/%ld: ws=%08lx  ws_Window=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, ws, ws->ws_Window));

		if (ScalosAttemptLockIconListShared(iwtx))
			{
			struct ScaIconNode *in;

			ULONG IconCount = 0;

			for (in=iwtx->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				struct Gadget *gg = (struct Gadget *) in->in_Icon;

				d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", \
					__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

				if (in != IconUnderPointer && (gg->Flags & GFLG_SELECTED))
					{
					struct SelectedIcon *sli;

					d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", \
						__FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

					IconCount++;
					TotalIconCount++;

					sli = ScalosAlloc(sizeof(struct SelectedIcon));

					if (sli)
						{
						sli->sli_Icon = in;
						sli->sli_WindowTask = iwtx;

						AddTail(SelectedIconList, &sli->sli_Node);
						}
					}
				}

			if (IconCount)
				{
				struct LockedWindowEntry *lwe;

				lwe = ScalosAlloc(sizeof(struct LockedWindowEntry));
				if (lwe)
					{
					lwe->lwe_WindowTask = iwtx;
					AddTail(LockedWindowList, &lwe->lwe_Node);
					}
				else
					{
					ScalosUnLockIconList(iwtx);
					}
				}
			else
				{
				ScalosUnLockIconList(iwtx);
				}
			}
		}

	if (0 == TotalIconCount)
		SCA_UnLockWindowList();
}


static void CleanupSelectedIconList(struct List *SelectedIconList, struct List *LockedWindowList)
{
	struct LockedWindowEntry *lwe;
	struct SelectedIcon *sli;
	BOOL WindowListLocked;

	WindowListLocked = !IsListEmpty(SelectedIconList);

	while ((sli = (struct SelectedIcon *) RemTail(SelectedIconList)))
		{
		ScalosFree(sli);
		}
	while ((lwe = (struct LockedWindowEntry *) RemTail(LockedWindowList)))
		{
		ScalosUnLockIconList(lwe->lwe_WindowTask);
		ScalosFree(lwe);
		}

	if (WindowListLocked)
		SCA_UnLockWindowList();
}


static BOOL isPopupApplyToSelectedQualifier(ULONG Qualifier)
{
	return (BOOL) (CurrentPrefs.pref_PopupApplySelectedAlways ||
		(Qualifier & CurrentPrefs.pref_PopupApplySelectedQualifier) == CurrentPrefs.pref_PopupApplySelectedQualifier);
}


