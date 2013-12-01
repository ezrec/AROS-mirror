// Menu.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <libraries/pm.h>
#include <libraries/iffparse.h>
#include <libraries/ttengine.h>
#include <prefs/font.h>
#include <prefs/prefhdr.h>
#include <prefs/pointer.h>
#include <prefs/workbench.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#ifdef __AROS__
#include <proto/popupmenu.h>
#else
#include <proto/pm.h>
#endif
#include <proto/iffparse.h>
#include <proto/preferences.h>
#include <proto/gadtools.h>
#include <proto/diskfont.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/menu.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"
#include "DtImageClass.h"

//----------------------------------------------------------------------------

struct ScalosMenuTreeX
	{
	struct	ScalosMenuTree *mtrx_Next;
	APTR	mtrx_tree;
	UBYTE	mtrx_TreeType;
	UBYTE	mtrx_TreeFlags;
	UBYTE	mtrx_ComFlags;
	UBYTE	mtrx_ComType;
	ULONG	mtrx_stack;
	STRPTR	mtrx_name;
	};

enum ScalosMenuChunkId
	{ 
	SCMID_MainMenu = 0, 
	SCMID_Popup_Disk, 
	SCMID_Popup_Drawer, 
	SCMID_Popup_Tool, 
	SCMID_Popup_Trashcan, 
	SCMID_Popup_Window, 
	SCMID_Popup_AppIcon,
	SCMID_Popup_Desktop,

	MAX_PopupMenu	      // MUST be last entry
	}; 


struct ScalosMenuChunk
	{
	UWORD smch_MenuID;		// enum ScalosMenuChunkId
	UWORD smch_Entries;		// number of entries
	struct ScalosMenuTree smch_Menu[1];	// (variable) the menu entries
	};

struct ScalosMenuMutex
	{
	struct NewMenu *smmx_NewMenu;
	ULONG smmx_Mask;
	};

struct ScalosMenuImage
	{
	struct Node smi_Node;
	Object *smi_Image;
	};

//----------------------------------------------------------------------------

// public data items

struct IconMenuSupports IconSupportTable[] =
	{
	{ "open", 		INF_SupportsOpen	},
	{ "clone",		INF_SupportsCopy	},
	{ "rename",		INF_SupportsRename	},
	{ "iconinfo",		INF_SupportsInformation	},
	{ "snapshot",		INF_SupportsSnapshot	},
	{ "unsnapshot",		INF_SupportsUnSnapshot	},
	{ "leaveout",		INF_SupportsLeaveOut	},
	{ "putaway",		INF_SupportsPutAway	},
	{ "delete",		INF_SupportsDelete	},
	{ "formatdisk",		INF_SupportsFormatDisk	},
	{ "emptytrashcan",	INF_SupportsEmptyTrash	},
	{ NULL,			0			},
	};

// Window Menu Flags
#define	WMFLAG_ReadWrite	0x00000001	// invalid for read-only windows
#define	WMFLAG_NotRootWindow	0x00000002	// invalid for root window
#define	WMFLAG_NotBorderless	0x00000004	// invalid for borderless windows
#define	WMFLAG_RootWindow	0x00000008	// only allowed in root window
#define	WMFLAG_IsVirtualSize	0x00000010	// only allowed if window smaller than virtual size
#define	WMFLAG_HasParent	0x00000020	// only allowed if window has parent
#define	WMFLAG_NotTextWindow	0x00000040	// invalid for text windows
#define	WMFLAG_NotAllSelected	0x00000080	// invalid if all icons are selected
#define	WMFLAG_AnythingSelected	0x00000100	// invalid if no icon is selected

struct IconMenuSupports WindowMenuTable[] =
	{
	{ "snapshotall",	WMFLAG_ReadWrite				},
	{ "snapshotwindow",	WMFLAG_ReadWrite				},
	{ "makedir",		WMFLAG_ReadWrite | WMFLAG_NotRootWindow		},
	{ "iconify",		WMFLAG_NotRootWindow				},
	{ "parent",		WMFLAG_NotRootWindow | WMFLAG_HasParent		},
	{ "close",		WMFLAG_NotRootWindow				},
	{ "sizetofit",		WMFLAG_NotBorderless | WMFLAG_IsVirtualSize	},
	{ "backdrop",		WMFLAG_RootWindow				},
	{ "viewbyicon",		WMFLAG_NotRootWindow				},
	{ "viewbytext",		WMFLAG_NotRootWindow				},
	{ "viewbysize",		WMFLAG_NotRootWindow				},
	{ "viewbydate",		WMFLAG_NotRootWindow				},
	{ "viewbytype",		WMFLAG_NotRootWindow				},
	{ "viewbydefault",	WMFLAG_NotRootWindow				},
	{ "showonlyicons",	WMFLAG_NotRootWindow				},
	{ "showallfiles",	WMFLAG_NotRootWindow				},
	{ "showdefault",	WMFLAG_NotRootWindow				},
	{ "cleanup",		WMFLAG_NotTextWindow				},
	{ "cleanupbyname",	WMFLAG_NotTextWindow				},
	{ "cleanupbydate",	WMFLAG_NotTextWindow				},
	{ "cleanupbysize",	WMFLAG_NotTextWindow				},
	{ "cleanupbytype",	WMFLAG_NotTextWindow				},
	{ "selectall",		WMFLAG_NotAllSelected				},
	{ "clearselection",	WMFLAG_AnythingSelected				},
	{ NULL,			0						},
	};

struct Menu *MainMenu;			// +jl+ 20010710 window menu
struct NewMenu *MainNewMenu = NULL;	// NewMenu array for main menu

struct MenuInfo *ParseMenuList;		// List of MenuInfo's
SCALOSSEMAPHORE ParseMenuListSemaphore;	// Semaphore to protect ParseMenuList

//----------------------------------------------------------------------------

// local data items

static struct List MenuImageList;

static struct ScalosMenuChunk *MainMenuBuffer = NULL;
static struct ScalosMenuChunk *PopupMenuBuffer[MAX_PopupMenu];

static ULONG MenuPrefsCRC = 0;

// Remember MenuItems for mutual exclusion
static struct ScalosMenuMutex GenerateList_ViewByIcon;
static struct ScalosMenuMutex GenerateList_ViewByName;
static struct ScalosMenuMutex GenerateList_ViewByDate;
static struct ScalosMenuMutex GenerateList_ViewBySize;
static struct ScalosMenuMutex GenerateList_ViewByType;
static struct ScalosMenuMutex GenerateList_ViewByDefault;
static struct ScalosMenuMutex GenerateList_ShowOnlyIcons;
static struct ScalosMenuMutex GenerateList_ShowAllFiles;
static struct ScalosMenuMutex GenerateList_ShowDefault;

//----------------------------------------------------------------------------

// local functions

static ULONG GetSelectedIconsSupportFlags(void);
static BOOL CanBeSizedToFit(struct internalScaWindowTask *iwt);
static void SetIconMenuOnOff(struct internalScaWindowTask *iwt);
static void SetWindowMenuOnOff(struct internalScaWindowTask *iwt);
static void internalSetMenuOnOff(struct internalScaWindowTask *iwt,
	struct MenuInfo *mInfo, struct IconMenuSupports *MenuTable, ULONG MenuFlags);
static void GenerateMainMenu(struct ScalosMenuTree *mTree, struct NewMenu **nm, LONG Level);
static struct PopupMenu *GeneratePopupMenu(struct ScalosMenuChunk *MenuChunk, LONG Level);
static struct PopupMenu *BuildPopupMenu(struct ScalosMenuTree *mTree, LONG Level);
static struct PopupMenu *MakePopupMenuItem(struct ScalosMenuTree *mTree);
static void AddAddresses(struct ScalosMenuTree *MenuTree, const UBYTE *BaseAddr);
static void ParseMenuItem(struct MenuItem *theItem, ULONG *MenuNumber, ULONG MenuNumIncrement);
static void MenuFunc_ViewByIcon(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ViewByText(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ViewByDate(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ViewBySize(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ViewByType(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ViewByDefault(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ShowOnlyIcons(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ShowAllFiles(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static void MenuFunc_ShowDefault(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo);
static ULONG MfOffMenu(struct Window *win, struct MenuInfo *mInfo);
static ULONG MfOnMenu(struct Window *win, struct MenuInfo *mInfo);
static void SubMenusOnOff(struct internalScaWindowTask *iwt, ULONG Changed, ULONG TextChanged);
static void AddMenuImage(Object *img);
static void CleanupMenuImages(void);

//----------------------------------------------------------------------------

static ULONG GetSelectedIconsSupportFlags(void)
{
	struct ScaWindowStruct *ws;

	ULONG SupportFlags = 0l;

	if (SCA_LockWindowList(SCA_LockWindowList_AttemptShared))
		{
		d1(kprintf("%s/%s/%ld: layer=%08lx\n", __FILE__, __FUNC__, __LINE__, layer));

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;

			d1(kprintf("%s/%s/%ld: ws=%08lx  ws_Window=%08lx  iwtx=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, ws, ws->ws_Window, iwtx));

			SupportFlags |= UpdateIconCount(iwtx);
			}

		SCA_UnLockWindowList();
		}

	return SupportFlags;
}


static BOOL CanBeSizedToFit(struct internalScaWindowTask *iwt)
{
	struct Rectangle VirtRect;
	WORD NewInnerWidth, NewInnerHeight;
	BOOL MayBeSized = FALSE;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: vWidth=%ld  vHeight=%ld  iWidth=%ld  iHeight=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_InvisibleWidth, iwt->iwt_InvisibleHeight, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));

	if (iwt->iwt_BackDrop)
		return FALSE;

	if (iwt->iwt_InvisibleWidth > 0 || iwt->iwt_InvisibleHeight > 0)
		return TRUE;

	if (ScalosAttemptSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore))
		{
		if (ScalosAttemptLockIconListShared(iwt))
			{
			GetIconsBoundingBox(iwt, &VirtRect);

			d1(KPrintF("%s/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
				VirtRect.MinX, VirtRect.MinY, VirtRect.MaxX, VirtRect.MaxY));

			if (NULL == iwt->iwt_WindowTask.wt_Window)
				{
				ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
				return FALSE;
				}

			NewInnerWidth = VirtRect.MaxX - VirtRect.MinX + 1;
			NewInnerHeight = VirtRect.MaxY - VirtRect.MinY + 1;

			if (iwt->iwt_InnerWidth != NewInnerWidth)
				{
				MayBeSized = TRUE;
				}
			if (iwt->iwt_InnerHeight != NewInnerHeight)
				{
				MayBeSized = TRUE;
				}
			ScalosUnLockIconList(iwt);
			}

		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return MayBeSized;
}


static void SetIconMenuOnOff(struct internalScaWindowTask *iwt)
{
	ULONG IconSupportsFlags = GetSelectedIconsSupportFlags();

	// 0xff, 7 : "only enable if icon selected"
	d1(KPrintF("%s/%s/%ld: IconSupportsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, IconSupportsFlags));

	if (ScalosAttemptSemaphoreShared(&ParseMenuListSemaphore))
		{
		internalSetMenuOnOff(iwt, ParseMenuList, IconSupportTable, IconSupportsFlags);

		ScalosReleaseSemaphore(&ParseMenuListSemaphore);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void SetWindowMenuOnOff(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));
	if (iwt->iwt_WindowTask.wt_Window)
		{
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		if (ScalosAttemptSemaphoreShared(&ParseMenuListSemaphore))
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			internalSetMenuOnOff(iwt, ParseMenuList, WindowMenuTable, GetWindowMenuFlags(iwt));
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			ScalosReleaseSemaphore(&ParseMenuListSemaphore);
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}
		}
	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


// sets window flag bits when the corresponding condition is TRUE
ULONG GetWindowMenuFlags(struct internalScaWindowTask *iwt)
{
	ULONG WindowFlags = 0;

	d1(KPrintF("%s/%s/%ld: START iwt <%s>\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WinTitle));

	if (IsIwtViewByIcon(iwt))
		WindowFlags |= WMFLAG_NotTextWindow;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	
	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		struct InfoData *infoData;

		WindowFlags |= WMFLAG_NotRootWindow | WMFLAG_HasParent;

		infoData = ScalosAllocInfoData();
		if (infoData)
			{
			Info(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, infoData);

			if (ClassCheckInfoData(infoData))
				WindowFlags |= WMFLAG_ReadWrite;

			ScalosFreeInfoData(&infoData);
			}
		}
	else
		{
		WindowFlags |= WMFLAG_RootWindow | WMFLAG_ReadWrite;
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// Count selected icons
	if (ScalosAttemptLockIconListShared(iwt))
		{
		struct ScaIconNode *in;
		ULONG TotalItems = 0, SelectedItems = 0;

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct Gadget *gg = (struct Gadget *) in->in_Icon;

			TotalItems++;

			d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", \
				__FILE__, __FUNC__, __LINE__, in, in->in_Name));

			if (gg->Flags & GFLG_SELECTED)
				SelectedItems++;
			}

		ScalosUnLockIconList(iwt);

		if (TotalItems != SelectedItems)
			WindowFlags |= WMFLAG_NotAllSelected;
		if (0 != SelectedItems)
			WindowFlags |= WMFLAG_AnythingSelected;
		}

	d1(KPrintF("%s/%s/%ld: BackDrop=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_BackDrop));

	if (!iwt->iwt_BackDrop)
		WindowFlags |= WMFLAG_NotBorderless;

	d1(KPrintF("%s/%s/%ld: VirtWidth=%ld  VirtHeight=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_InvisibleWidth, iwt->iwt_InvisibleHeight));

	if (CanBeSizedToFit(iwt))
		WindowFlags |= WMFLAG_IsVirtualSize;

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RootWindow)
		WindowFlags &= ~WMFLAG_HasParent;

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>  WindowFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		iwt, iwt->iwt_WinTitle, WindowFlags));

	return WindowFlags;
}


static void internalSetMenuOnOff(struct internalScaWindowTask *iwt,
	struct MenuInfo *mInfo, struct IconMenuSupports *MenuTable, ULONG MenuFlags)
{
	ULONG Changed = 0;
	ULONG TextChanged = 0;

	d1(KPrintF("\n" "%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	while (mInfo)
		{
		if (ScalosAttemptSemaphoreShared(&MenuSema))
			{
			struct ScalosMenuTree *mTree;

			d1(kprintf("%s/%s/%ld: mItem=%08lx  minf_type=%08lx\n", __FILE__, __FUNC__, __LINE__, mInfo->minf_item, mInfo->minf_type));

			mTree = (struct ScalosMenuTree *) GTMENUITEM_USERDATA(mInfo->minf_item);

			d1(kprintf("%s/%s/%ld: mTree=%08lx\n", __FILE__, __FUNC__, __LINE__, mTree));

			if (mTree && iwt->iwt_WindowTask.wt_Window && MainMenu == iwt->iwt_WindowTask.wt_Window->MenuStrip)
				{
				d1(kprintf("%s/%s/%ld: mTree=%08lx  Type=%ld\n", __FILE__, __FUNC__, __LINE__, mTree, mTree->mtre_type));

				if (SCAMENUTYPE_Command == mTree->mtre_type && mTree->MenuCombo.MenuCommand.mcom_name)
					{
					d1(kprintf("%s/%s/%ld: mTree=%08lx  name=<%s>  type=%ld\n", __FILE__, __FUNC__, __LINE__, \
						mTree, mTree->MenuCombo.MenuCommand.mcom_name, mTree->MenuCombo.MenuCommand.mcom_type));

					if (SCAMENUCMDTYPE_Command == mTree->MenuCombo.MenuCommand.mcom_type
						&& mTree->MenuCombo.MenuCommand.mcom_name)
						{
						ULONG n;

						if (0 == Stricmp("undo", mTree->MenuCombo.MenuCommand.mcom_name))
							{
							struct IntuiText *itx = mInfo->minf_item->ItemFill;
							d1(kprintf("%s/%s/%ld: mTree=%08lx  ItemFill=%08lx  IText=%08lx\n", __FILE__, __FUNC__, __LINE__, mTree, itx, itx->IText));
							itx->IText = (T_ITEXT) UndoGetDescription();
							TextChanged++;
							}
						if (0 == Stricmp("redo", mTree->MenuCombo.MenuCommand.mcom_name))
							{
							struct IntuiText *itx = mInfo->minf_item->ItemFill;
							d1(kprintf("%s/%s/%ld: mTree=%08lx  ItemFill=%08lx  IText=%08lx\n", __FILE__, __FUNC__, __LINE__, mTree, itx, itx->IText));
							itx->IText = (T_ITEXT) RedoGetDescription();
							TextChanged++;
							}

						for (n=0; MenuTable[n].ims_CommandName; n++)
							{
							d1(kprintf("%s/%s/%ld: ims_CommandName=<%s>\n", __FILE__, __FUNC__, __LINE__, MenuTable[n].ims_CommandName));

							if (0 == Stricmp((STRPTR) MenuTable[n].ims_CommandName, mTree->MenuCombo.MenuCommand.mcom_name))
								{
								d1(kprintf("%s/%s/%ld: name=<%s>  mInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, mTree->MenuCombo.MenuCommand.mcom_name, mInfo));

								d1(kprintf("%s/%s/%ld: MenuFlags=%08lx  FlagMask=%08lx\n", __FILE__, __FUNC__, __LINE__, \
									MenuFlags, MenuTable[n].ims_FlagMask));

								if (MenuTable[n].ims_FlagMask == (MenuFlags & MenuTable[n].ims_FlagMask))
									{
									d1(kprintf("%s/%s/%ld: ONMENU <%s> menunum=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
										mTree->MenuCombo.MenuCommand.mcom_name, mInfo->minf_number, mInfo->minf_item->Flags));

									Changed += MfOnMenu(iwt->iwt_WindowTask.wt_Window, mInfo);
									}
								else
									{
									d1(kprintf("%s/%s/%ld: OFFMENU <%s> menunum=%08lx  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
										mTree->MenuCombo.MenuCommand.mcom_name, mInfo->minf_number, mInfo->minf_item->Flags));

									Changed += MfOffMenu(iwt->iwt_WindowTask.wt_Window, mInfo);
									}
								}
							}
						}
					}
				}
			ScalosReleaseSemaphore(&MenuSema);
			}

		mInfo = (struct MenuInfo *) mInfo->minf_Node.mln_Succ;
		}

	d1(KPrintF("\n" "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SubMenusOnOff(iwt, Changed, TextChanged);

	d1(KPrintF("\n" "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


ULONG ReadMenuPrefs(void)
{
	static const LONG StopChunkList[] =
		{
		ID_PREF, MAKE_ID('M','E','N','U')
		};
	struct IFFHandle *iffHandle;
	ULONG Error;
	BOOL iffOpened = FALSE;
	struct ScalosMenuChunk *MenuChunk = NULL;

	d1(KPrintF("\n" "%s/%s/%ld: Start\n", __FILE__, __FUNC__, __LINE__));

	FreeMenuPrefs();

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	do	{
		iffHandle = AllocIFF();
		if (NULL == iffHandle)
			{
			Error = IoErr();
			break;
			}

		InitIFFasDOS(iffHandle);

		// first try to open 1.3 menu prefs
		iffHandle->iff_Stream = (IPTR) Open("ENV:Scalos/Menu13.prefs", MODE_OLDFILE);
		if (0 == iffHandle->iff_Stream)
			{
			// then try to open old menu prefs
			iffHandle->iff_Stream = (IPTR) Open("ENV:Scalos/Menu.prefs", MODE_OLDFILE);
			}
		if (0 == iffHandle->iff_Stream)
			{
			Error = IoErr();
			break;	// no menu prefs found
			}

		MenuPrefsCRC = GetPrefsCRCFromFH((BPTR) iffHandle->iff_Stream);

		Error = OpenIFF(iffHandle, IFFF_READ);
		if (RETURN_OK != Error)
			break;
		iffOpened = TRUE;

		Error = StopChunks(iffHandle, (LONG *) StopChunkList, sizeof(StopChunkList) / 2 * sizeof(ULONG));
		if (RETURN_OK != Error)
			break;

		while (RETURN_OK == ParseIFF(iffHandle, IFFPARSE_SCAN))
			{
			struct ContextNode *cn;
			struct NewMenu *nm;
			ULONG Entries;
			size_t length;

			cn = CurrentChunk(iffHandle);
			if (NULL == cn)
				break;

			if (MAKE_ID('M','E','N','U') != cn->cn_ID)
				continue;

			MenuChunk = ScalosAlloc(cn->cn_Size);
			if (NULL == MenuChunk)
				{
				Error = ERROR_NO_FREE_STORE;
				break;
				}

			if (ReadChunkBytes(iffHandle, MenuChunk, cn->cn_Size) < 0)
				{
				Error = IoErr();
				break;
				}

			MenuChunk->smch_MenuID = SCA_BE2WORD(MenuChunk->smch_MenuID);
			MenuChunk->smch_Entries = SCA_BE2WORD(MenuChunk->smch_Entries);

			AddAddresses(MenuChunk->smch_Menu, (UBYTE *) MenuChunk);

			switch (MenuChunk->smch_MenuID)
				{
			case SCMID_MainMenu:
				memset(&GenerateList_ViewByIcon, 0, sizeof(GenerateList_ViewByIcon));
				memset(&GenerateList_ViewByName, 0, sizeof(GenerateList_ViewByName));
				memset(&GenerateList_ViewBySize, 0, sizeof(GenerateList_ViewBySize));
				memset(&GenerateList_ViewByDate, 0, sizeof(GenerateList_ViewByDate));
				memset(&GenerateList_ViewByType, 0, sizeof(GenerateList_ViewByType));
				memset(&GenerateList_ViewByDefault, 0, sizeof(GenerateList_ViewByDefault));

				memset(&GenerateList_ShowOnlyIcons, 0, sizeof(GenerateList_ShowOnlyIcons));
				memset(&GenerateList_ShowAllFiles, 0, sizeof(GenerateList_ShowAllFiles));
				memset(&GenerateList_ShowDefault, 0, sizeof(GenerateList_ShowDefault));

				Entries = MenuChunk->smch_Entries;
				if (0 == Entries)
					Entries = 200;

				length = Entries * sizeof(struct NewMenu) + sizeof(ULONG);
				MainNewMenu = nm = ScalosAlloc(length);
				if (NULL == nm)
					{
					Error = ERROR_NO_FREE_STORE;
					break;
					}
				memset(MainNewMenu, 0, length);
				GenerateMainMenu(MenuChunk->smch_Menu, &nm, 1);

				if (GenerateList_ViewByIcon.smmx_NewMenu)
					{
					GenerateList_ViewByIcon.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ViewByName.smmx_Mask | 
						GenerateList_ViewByDate.smmx_Mask | 
						GenerateList_ViewBySize.smmx_Mask | 
						GenerateList_ViewByType.smmx_Mask |
						GenerateList_ViewByDefault.smmx_Mask;
					}
				if (GenerateList_ViewByName.smmx_NewMenu)
					{
					GenerateList_ViewByName.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ViewByIcon.smmx_Mask | 
						GenerateList_ViewByDate.smmx_Mask | 
						GenerateList_ViewBySize.smmx_Mask | 
						GenerateList_ViewByType.smmx_Mask |
						GenerateList_ViewByDefault.smmx_Mask;
					}
				if (GenerateList_ViewByDate.smmx_NewMenu)
					{
					GenerateList_ViewByDate.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ViewByIcon.smmx_Mask | 
						GenerateList_ViewByName.smmx_Mask | 
						GenerateList_ViewBySize.smmx_Mask | 
						GenerateList_ViewByType.smmx_Mask |
						GenerateList_ViewByDefault.smmx_Mask;
					}
				if (GenerateList_ViewBySize.smmx_NewMenu)
					{
					GenerateList_ViewBySize.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ViewByIcon.smmx_Mask | 
						GenerateList_ViewByDate.smmx_Mask | 
						GenerateList_ViewByName.smmx_Mask | 
						GenerateList_ViewByType.smmx_Mask |
						GenerateList_ViewByDefault.smmx_Mask;
					}
				if (GenerateList_ViewByType.smmx_NewMenu)
					{
					GenerateList_ViewByType.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ViewByIcon.smmx_Mask | 
						GenerateList_ViewByName.smmx_Mask | 
						GenerateList_ViewByDate.smmx_Mask | 
						GenerateList_ViewBySize.smmx_Mask |
						GenerateList_ViewByDefault.smmx_Mask;
					}
				if (GenerateList_ViewByDefault.smmx_NewMenu)
					{
					GenerateList_ViewByDefault.smmx_NewMenu->nm_MutualExclude =
						GenerateList_ViewByIcon.smmx_Mask |
						GenerateList_ViewByName.smmx_Mask |
						GenerateList_ViewByDate.smmx_Mask |
						GenerateList_ViewBySize.smmx_Mask |
						GenerateList_ViewByType.smmx_Mask;
					}

				if (GenerateList_ShowOnlyIcons.smmx_NewMenu)
					{
					GenerateList_ShowOnlyIcons.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ShowAllFiles.smmx_Mask |
						GenerateList_ShowDefault.smmx_Mask;
					}
				if (GenerateList_ShowAllFiles.smmx_NewMenu)
					{
					GenerateList_ShowAllFiles.smmx_NewMenu->nm_MutualExclude = 
						GenerateList_ShowOnlyIcons.smmx_Mask |
						GenerateList_ShowDefault.smmx_Mask;
					}
				if (GenerateList_ShowDefault.smmx_NewMenu)
					{
					GenerateList_ShowDefault.smmx_NewMenu->nm_MutualExclude =
						GenerateList_ShowOnlyIcons.smmx_Mask |
						GenerateList_ShowAllFiles.smmx_Mask;
					}

				MainMenuBuffer = MenuChunk;
				MenuChunk = NULL;
				break;

			case SCMID_Popup_Disk:
			case SCMID_Popup_Drawer:
			case SCMID_Popup_Tool:
			case SCMID_Popup_Trashcan:
			case SCMID_Popup_Window:
			case SCMID_Popup_AppIcon:
			case SCMID_Popup_Desktop:
				d1(KPrintF("%s/%s/%ld: smch_MenuID=%lu\n", __FILE__, __FUNC__, __LINE__, MenuChunk->smch_MenuID));
				PopupMenuBuffer[MenuChunk->smch_MenuID] = MenuChunk;
				PopupMenus[MenuChunk->smch_MenuID - 1] = GeneratePopupMenu(MenuChunk, MenuChunk->smch_MenuID - 1);
				MenuChunk = NULL;
				break;
				}

			if (RETURN_OK != Error)
				break;

			if (MenuChunk)
				{
				ScalosFree(MenuChunk);
				MenuChunk = NULL;
				}
			}
		
		} while (0);

	if (MenuChunk)
		ScalosFree(MenuChunk);
	if (iffHandle)
		{
		if (iffOpened)
			CloseIFF(iffHandle);
		if (iffHandle->iff_Stream)
			Close((BPTR) iffHandle->iff_Stream);
		FreeIFF(iffHandle);
		}

	d1(KPrintF("%s/%s/%ld: Error=%ld\n", __FILE__, __FUNC__, __LINE__, Error));

	return Error;
}


void FreeMenuPrefs(void)
{
	short n;

	d1(KPrintF("\n" "%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	PopupMenuFlag = FALSE;

	ClearMainMenu();
	CleanupMenuImages();

	if (MainNewMenu)
		{
		ScalosFree(MainNewMenu);
		MainNewMenu = NULL;
		}
	if (MainMenuBuffer)
		{
		ScalosFree(MainMenuBuffer);
		MainMenuBuffer = NULL;
		}
	for (n=0; n<MAX_PopupMenu; n++)
		{
		if (PopupMenuBuffer[n])
			{
			ScalosFree(PopupMenuBuffer[n]);
			PopupMenuBuffer[n] = NULL;
			}
		}
	for (n=0; n<SCPOPUPMENU_Last; n++)
		{
		if (PopupMenus[n])
			{
			PM_FreePopupMenu(PopupMenus[n]);
			PopupMenus[n] = NULL;
			}
		}
	d1(KPrintF("\n" "%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void GenerateMainMenu(struct ScalosMenuTree *mTree, struct NewMenu **nm, LONG Level)
{
	ULONG Mask = 1;

	d1(kprintf("%s/%s/%ld: mTree=%08lx  Type=%ld  Level=%ld\n", __FILE__, __FUNC__, __LINE__, \
		mTree, mTree->mtre_type, Level));

	while (mTree)
		{
		ULONG Flags;
		ULONG MutualExclude;
		struct ScalosMenuTree *mTreeChild;

		d1(kprintf("%s/%s/%ld: mTree=%08lx  Type=%ld\n", __FILE__, __FUNC__, __LINE__, mTree, mTree->mtre_type));

		switch (mTree->mtre_type)
			{
		case SCAMENUTYPE_Menu:
			d1(kprintf("%s/%s/%ld: Menu  Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
				mTree->mtre_name, mTree->mtre_name ? mTree->mtre_name : (STRPTR) ""));

			(*nm)->nm_UserData = NULL;
			if (Level < 2)
				(*nm)->nm_Type = NM_TITLE;
			else
				(*nm)->nm_Type = NM_ITEM;

			(*nm)->nm_Label = mTree->MenuCombo.MenuTree.mtre_name;

			(*nm)++;

			if (mTree->mtre_tree)
				GenerateMainMenu(mTree->mtre_tree, nm, Level + 1);
			break;

		case SCAMENUTYPE_MenuItem:
			d1(kprintf("%s/%s/%ld: MenuItem  Name=%08lx  <%s>  HotKey=<%s>\n", __FILE__, __FUNC__, __LINE__, \
				mTree->mtre_name, mTree->mtre_name ? mTree->mtre_name : (STRPTR) "",\
				mTree->MenuCombo.MenuTree.mtre_hotkey));

			if (Level < 3)
				(*nm)->nm_Type = NM_ITEM;
			else
				(*nm)->nm_Type = NM_SUB;

			if (mTree->MenuCombo.MenuTree.mtre_name)
				(*nm)->nm_Label = mTree->MenuCombo.MenuTree.mtre_name;
			else
				(*nm)->nm_Label = NM_BARLABEL;

			if (mTree->MenuCombo.MenuTree.mtre_hotkey[0])
				(*nm)->nm_CommKey = (STRPTR) mTree->MenuCombo.MenuTree.mtre_hotkey;
			else
				(*nm)->nm_CommKey = NULL;

			Flags = 0;
			MutualExclude = 0;

			mTreeChild = mTree->mtre_tree;

			if (mTreeChild && SCAMENUTYPE_Command == mTree->mtre_tree->mtre_type)
				{
				(*nm)->nm_UserData = mTreeChild;

				while (mTreeChild)
					{
					d1(kprintf("%s/%s/%ld: MenuCommand  Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
						mTreeChild->MenuCombo.MenuCommand.mcom_name, mTreeChild->MenuCombo.MenuCommand.mcom_name));

					if (SCAMENUCMDTYPE_Command == mTreeChild->MenuCombo.MenuCommand.mcom_type
						&& mTreeChild->MenuCombo.MenuCommand.mcom_name)
						{
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "undo"))
							{
							(*nm)->nm_Label = (STRPTR) UndoGetDescription();
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "redo"))
							{
							(*nm)->nm_Label = (STRPTR) RedoGetDescription();
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "backdrop"))
							{
							Flags = MENUTOGGLE | CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "viewbyicon"))
							{
							GenerateList_ViewByIcon.smmx_NewMenu = *nm;
							GenerateList_ViewByIcon.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "viewbytext"))
							{
							GenerateList_ViewByName.smmx_NewMenu = *nm;
							GenerateList_ViewByName.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "viewbysize"))
							{
							GenerateList_ViewBySize.smmx_NewMenu = *nm;
							GenerateList_ViewBySize.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "viewbydate"))
							{
							GenerateList_ViewByDate.smmx_NewMenu = *nm;
							GenerateList_ViewByDate.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "viewbytype"))
							{
							GenerateList_ViewByType.smmx_NewMenu = *nm;
							GenerateList_ViewByType.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "viewbydefault"))
							{
							GenerateList_ViewByDefault.smmx_NewMenu = *nm;
							GenerateList_ViewByDefault.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "showonlyicons"))
							{
							GenerateList_ShowOnlyIcons.smmx_NewMenu = *nm;
							GenerateList_ShowOnlyIcons.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "showallfiles"))
							{
							GenerateList_ShowAllFiles.smmx_NewMenu = *nm;
							GenerateList_ShowAllFiles.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						if (CompareCommand(mTreeChild->MenuCombo.MenuCommand.mcom_name, "showdefault"))
							{
							GenerateList_ShowDefault.smmx_NewMenu = *nm;
							GenerateList_ShowDefault.smmx_Mask = Mask;
							Flags = CHECKIT;
							}
						}

					mTreeChild = mTreeChild->mtre_Next;
					}
				}

			(*nm)->nm_Flags = Flags;
			(*nm)->nm_MutualExclude = MutualExclude;

			(*nm)++;
			break;

		case SCAMENUTYPE_Command:
			return;

		case SCAMENUTYPE_ToolsMenu:
			d1(kprintf("%s/%s/%ld: Menu  Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
				mTree->mtre_name, mTree->mtre_name ? mTree->mtre_name : (STRPTR) ""));

			(*nm)->nm_UserData = USERDATA_TOOLSMENU;
			if (Level < 2)
				(*nm)->nm_Type = NM_TITLE;
			else
				(*nm)->nm_Type = NM_ITEM;

			(*nm)->nm_Label = mTree->MenuCombo.MenuTree.mtre_name;

			(*nm)++;

			if (mTree->mtre_tree)
				GenerateMainMenu(mTree->mtre_tree, nm, Level + 1);
			break;
			}

		Mask <<= 1;

		mTree = mTree->mtre_Next;
		}
}


static struct PopupMenu *GeneratePopupMenu(struct ScalosMenuChunk *MenuChunk, LONG Level)
{
	d1(KPrintF("%s/%s/%ld: MenuChunk=%08lx  Level=%ld\n", __FILE__, __FUNC__, __LINE__, MenuChunk, Level));

	if (NULL == PopupMenuBase)
		return NULL;
	if (NULL == MenuChunk)
		return NULL;
	if (0 == MenuChunk->smch_Entries)
		return NULL;

	PopupMenuFlag = TRUE;

	return BuildPopupMenu(MenuChunk->smch_Menu, Level);
}


static struct PopupMenu *BuildPopupMenu(struct ScalosMenuTree *mTree, LONG Level)
{
	struct TagItem *TagBuffer, *tbAlloc;
	struct PopupMenu *pm = NULL;
	ULONG MaxTagItem = 205;

	d1(KPrintF("%s/%s/%ld: mTree=%08lx  type=%ld  Level=%ld\n", __FILE__, __FUNC__, __LINE__, mTree, mTree->mtre_type, Level));

	do	{
		TagBuffer = tbAlloc = ScalosAlloc(MaxTagItem * sizeof(struct TagItem));
		if (NULL == tbAlloc)
			break;

		if (Level >= 0)
			{
			d1(KPrintF("%s/%s/%ld: Level >= 0\n", __FILE__, __FUNC__, __LINE__));

			TagBuffer->ti_Tag = PM_Item;
			TagBuffer->ti_Data = (ULONG) PM_MakeItem(PM_ID, PM_TITLE_ID,
				PM_Title, (ULONG) GetLocString(MSGID_PMTITLE1NAME + Level),
				PM_NoSelect, TRUE,
				PM_ShinePen, TRUE,
				PM_Shadowed, FALSE,
				PM_Centre, TRUE,
				TAG_END);

			d1(KPrintF("%s/%s/%ld: Title created\n", __FILE__, __FUNC__, __LINE__));

			TagBuffer++;
			MaxTagItem--;

			TagBuffer->ti_Tag = PM_Item;
			TagBuffer->ti_Data = (ULONG) PM_MakeItem(PM_WideTitleBar, TRUE,
				TAG_END);

			TagBuffer++;
			MaxTagItem--;

			d1(KPrintF("%s/%s/%ld: TitleBar created\n", __FILE__, __FUNC__, __LINE__));
			}

		while (mTree && MaxTagItem > 1)
			{
			TagBuffer->ti_Tag = PM_Item;
			TagBuffer->ti_Data = (ULONG) MakePopupMenuItem(mTree);

			d1(KPrintF("%s/%s/%ld:  mTree=%08lx  Next=%08lx\n", __FILE__, __FUNC__, __LINE__, mTree, mTree->mtre_Next));

			TagBuffer++;
			MaxTagItem--;

			mTree = mTree->mtre_Next;
			}

		TagBuffer->ti_Tag = TAG_END;

		pm = PM_MakeMenuA(tbAlloc);
		} while (0);

	if (tbAlloc)
		ScalosFree(tbAlloc);

	return pm;
}


static struct PopupMenu *MakePopupMenuItem(struct ScalosMenuTree *mTree)
{
	struct PopupMenu *pm;

	d1(KPrintF("%s/%s/%ld: mTree=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, \
		mTree, mTree->MenuCombo.MenuTree.mtre_name, mTree->mtre_type));

	if (mTree->MenuCombo.MenuTree.mtre_name)
		{
		APTR UserData = NULL;
		CONST_STRPTR UnselectedIconName;
		CONST_STRPTR SelectedIconName;
		Object *SelectedImage = NULL;
		Object *UnselectedImage = NULL;

		d1(KPrintF("%s/%s/%ld: mtre_name=<%s>  mtre_flags=%02lx  mtre_iconnames=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, mTree->MenuCombo.MenuTree.mtre_name, \
			mTree->mtre_flags, mTree->MenuCombo.MenuTree.mtre_iconnames));

		UnselectedIconName = mTree->MenuCombo.MenuTree.mtre_iconnames;
		SelectedIconName = UnselectedIconName ? UnselectedIconName + 1 + strlen(UnselectedIconName) : NULL;
		if (UnselectedIconName && strlen(UnselectedIconName) >= 1)
			{
			UnselectedImage	= NewObject(DtImageClass, NULL,
				DTIMG_ImageName, (ULONG) UnselectedIconName,
				TAG_END);
			if (UnselectedImage)
				AddMenuImage(UnselectedImage);
			}
		if (SelectedIconName && strlen(SelectedIconName) >= 1)
			{
			SelectedImage = NewObject(DtImageClass, NULL,
				DTIMG_ImageName, (ULONG) SelectedIconName,
				TAG_END);
			if (SelectedImage)
				AddMenuImage(SelectedImage);
			}

		d1(KPrintF("%s/%s/%ld: UnselectedImage=%08lx SelectedImage=%08lx\n", __FILE__, __FUNC__, __LINE__, UnselectedImage, SelectedImage));

		if (mTree->mtre_tree && SCAMENUTYPE_Menu == mTree->mtre_type)
			{
			// Build Submenu
			pm = PM_MakeItem(PM_Sub, (ULONG) BuildPopupMenu(mTree->mtre_tree, -1),
				PM_UserData, NULL,
				PM_Title, (ULONG) mTree->MenuCombo.MenuTree.mtre_name,
				UnselectedImage ? PM_IconUnselected : TAG_IGNORE, (ULONG) UnselectedImage,
				SelectedImage   ? PM_IconSelected   : TAG_IGNORE, (ULONG) SelectedImage,
				TAG_END);
			}
		else
			{
			// Build standard menu item with commands as children
			BOOL isDefaultCommand = FALSE;

			if (mTree->mtre_tree && SCAMENUTYPE_Command == mTree->mtre_tree->mtre_type)
				{
				struct ScalosMenuTree *subTree;

				UserData = mTree->mtre_tree;
				d1(KPrintF("%s/%s/%ld: mtre_tree=%08lx  Next=%08lx\n", __FILE__, __FUNC__, __LINE__, mTree->mtre_tree, mTree->mtre_tree->mtre_Next));
				d1(KPrintF("%s/%s/%ld: UserData=%08lx\n", __FILE__, __FUNC__, __LINE__, UserData));

				for (subTree = mTree->mtre_tree; subTree; subTree = subTree->mtre_Next)
					{
					if (SCAMENUTYPE_Command == subTree->mtre_type
						&& SCAMENUCMDTYPE_Command == subTree->MenuCombo.MenuCommand.mcom_type
						&& subTree->MenuCombo.MenuCommand.mcom_name
						&& 0 == stricmp(subTree->MenuCombo.MenuCommand.mcom_name, "open"))
						{
						isDefaultCommand = TRUE;
						}
					}
				}

			d1(KPrintF("%s/%s/%ld: UserData=%08lx\n", __FILE__, __FUNC__, __LINE__, UserData));
			pm = PM_MakeItem(PM_UserData, (ULONG) UserData,
				PM_Title, (ULONG) mTree->MenuCombo.MenuTree.mtre_name,
				UnselectedImage ? PM_IconUnselected : TAG_IGNORE, (ULONG) UnselectedImage,
				SelectedImage   ? PM_IconSelected   : TAG_IGNORE, (ULONG) SelectedImage,
				PM_Bold, isDefaultCommand,
				TAG_END);
			}
		}
	else
		{
		pm = PM_MakeItem(PM_UserData, NULL,
			PM_TitleBar, TRUE,
			TAG_END);
		}

	d1(KPrintF("%s/%s/%ld: pm=%08lx\n", __FILE__, __FUNC__, __LINE__, pm));

	return pm;
}


static void AddAddresses(struct ScalosMenuTree *MenuTree, const UBYTE *BaseAddr)
{
	while (MenuTree)
		{
		if (SCAMENUTYPE_Command == MenuTree->mtre_type)
			{
			MenuTree->MenuCombo.MenuCommand.mcom_name = (APTR) SCA_BE2LONG(MenuTree->MenuCombo.MenuCommand.mcom_name);
			if (MenuTree->MenuCombo.MenuCommand.mcom_name)
				MenuTree->MenuCombo.MenuCommand.mcom_name += (ULONG) BaseAddr;
			MenuTree->MenuCombo.MenuCommand.mcom_stack = SCA_BE2LONG(MenuTree->MenuCombo.MenuCommand.mcom_stack);
			}
		else
			{
			MenuTree->MenuCombo.MenuTree.mtre_name = (APTR) SCA_BE2LONG(MenuTree->MenuCombo.MenuTree.mtre_name);
			if (MenuTree->MenuCombo.MenuTree.mtre_name)
				MenuTree->MenuCombo.MenuTree.mtre_name += (ULONG) BaseAddr;

			if (MenuTree->mtre_flags & MTREFLGF_IconNames)
				{
				MenuTree->MenuCombo.MenuTree.mtre_iconnames = (APTR) SCA_BE2LONG(MenuTree->MenuCombo.MenuTree.mtre_iconnames);
				if (MenuTree->MenuCombo.MenuTree.mtre_iconnames)
					{
					MenuTree->MenuCombo.MenuTree.mtre_iconnames += (ULONG) BaseAddr;
					}
					else
					{
					MenuTree->MenuCombo.MenuTree.mtre_iconnames = NULL;
					}
				}
			else
				{
				MenuTree->MenuCombo.MenuTree.mtre_iconnames = NULL;
				}
			}
		MenuTree->mtre_tree = (APTR) SCA_BE2LONG(MenuTree->mtre_tree);
		if (MenuTree->mtre_tree)
			{
			MenuTree->mtre_tree = (struct ScalosMenuTree *) (((UBYTE *) MenuTree->mtre_tree) + (ULONG) BaseAddr);
			AddAddresses(MenuTree->mtre_tree, BaseAddr);
			}
		MenuTree->mtre_Next = (APTR) SCA_BE2LONG(MenuTree->mtre_Next);
		if (MenuTree->mtre_Next)
			{
			MenuTree->mtre_Next = (struct ScalosMenuTree *) (((UBYTE *) MenuTree->mtre_Next) + (ULONG) BaseAddr);
			}
		MenuTree = MenuTree->mtre_Next;
		}
}


BOOL CompareCommand(CONST_STRPTR MenuComName, CONST_STRPTR CommandName)
{
	BOOL Match = FALSE;

	d1(kprintf("%s/%s/%ld: MenuCom=%08lx  Command=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuComName, CommandName));

	if (MenuComName && CommandName)
		{
		d1(kprintf("%s/%s/%ld: MenuCom=<%s>  Command=<%s>\n", __FILE__, __FUNC__, __LINE__, MenuComName, CommandName));

		while (*MenuComName && *CommandName && ' ' != *MenuComName)
			{
			UBYTE ch1, ch2;

			// For same reason, GCC 2.95.3 omits the comparison
			// without the auxiliary variables ch1,ch2
			ch1 = ToUpper(*MenuComName);
			ch2 = ToUpper(*CommandName);

			if (ch1 != ch2)
				break;

			MenuComName++;
			CommandName++;
			}

		d1(kprintf("%s/%s/%ld: CommandName=<%s>\n", __FILE__, __FUNC__, __LINE__, CommandName));

		if ('\0' == *CommandName)
			{
			switch (*MenuComName)
				{
			case ' ':
			case '\0':
				Match = TRUE;
				break;
				}
			}
		}

	d1(kprintf("%s/%s/%ld: Match=%ld\n", __FILE__, __FUNC__, __LINE__, Match));

	return Match;
}


void ParseMenu(struct Menu *theMenu)
{
	ULONG MenuNumber = 0;

	ScalosObtainSemaphore(&ParseMenuListSemaphore);

	while (theMenu)
		{
		d1(kprintf("%s/%s/%ld: theMenu=%08lx  UserData=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, theMenu, GTMENU_USERDATA(theMenu)));

		ParseMenuItem(theMenu->FirstItem, &MenuNumber, 0x20);

		MenuNumber = MENUNUM((MenuNumber + 1));

		theMenu = theMenu->NextMenu;
		}

	ScalosReleaseSemaphore(&ParseMenuListSemaphore);
}


static void ParseMenuItem(struct MenuItem *theItem, ULONG *MenuNumber, ULONG MenuNumIncrement)
{
	while (theItem)
		{
		struct ScalosMenuTree *mtre = GTMENUITEM_USERDATA(theItem);

		d1(kprintf("%s/%s/%ld: theItem=%08lx\n", __FILE__, __FUNC__, __LINE__, theItem));

		while (mtre)
			{
			d1(kprintf("%s/%s/%ld: mtre=%08lx  mtre_type=%ld\n", \
				__FILE__, __FUNC__, __LINE__, mtre, mtre->mtre_type));

			if (SCAMENUTYPE_Command == mtre->mtre_type
				&& SCAMENUCMDTYPE_Command == mtre->MenuCombo.MenuCommand.mcom_type
				&& mtre->MenuCombo.MenuCommand.mcom_name)
				{
				struct ScalosMenuCommand *mpi;
				BOOL Found = FALSE;

				for (mpi = &MenuCommandTable[0]; !Found && mpi->smcom_CommandName; mpi++)
					{
					if (CompareCommand(mtre->MenuCombo.MenuCommand.mcom_name, mpi->smcom_CommandName))
						{
						struct MenuInfo *minf;

						Found = TRUE;
						minf = (struct MenuInfo *) SCA_AllocStdNode(
							(struct ScalosNodeList *)(APTR) &ParseMenuList, NTYP_MenuInfo);
						if (minf)
							{
							minf->minf_MenuCmd = mpi;
							minf->minf_type = (UWORD) mpi->smcom_ParseFlags;
							minf->minf_item = theItem;
							minf->minf_number = *MenuNumber;
							}
						}
					}
				}

			mtre = mtre->mtre_Next;
			}

		if (theItem->SubItem)
			{
			ParseMenuItem(theItem->SubItem, MenuNumber, 0x800);
			*MenuNumber &= 0x7ff;
			}

		*MenuNumber += MenuNumIncrement;
		theItem = theItem->NextItem;
		}
}


void SetMenuOnOff(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (ScalosAttemptSemaphoreShared(&ParseMenuListSemaphore))
		{
		if (ScalosAttemptSemaphoreShared(&MenuSema))
			{
			struct MenuInfo *mInfo;

			for (mInfo=ParseMenuList; mInfo; mInfo = (struct MenuInfo *) mInfo->minf_Node.mln_Succ)
				{
				UWORD miType = mInfo->minf_type;
				BOOL MenuOn;

				d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

				
				if (miType & MITYPEF_ViewByIcon)
					MenuFunc_ViewByIcon(iwt, mInfo);
				if (miType & MITYPEF_ViewByText)
					MenuFunc_ViewByText(iwt, mInfo);
				if (miType & MITYPEF_ViewByDate)
					MenuFunc_ViewByDate(iwt, mInfo);
				if (miType & MITYPEF_ViewBySize)
					MenuFunc_ViewBySize(iwt, mInfo);
				if (miType & MITYPEF_ViewByType)
					MenuFunc_ViewByType(iwt, mInfo);
				if (miType & MITYPEF_ViewByDefault)
					MenuFunc_ViewByDefault(iwt, mInfo);
				if (miType & MITYPEF_ShowOnlyIcons)
					MenuFunc_ShowOnlyIcons(iwt, mInfo);
				if (miType & MITYPEF_ShowAllFiles)
					MenuFunc_ShowAllFiles(iwt, mInfo);
				if (miType & MITYPEF_ShowDefault)
					MenuFunc_ShowDefault(iwt, mInfo);

				if (mInfo->minf_MenuCmd->smcom_Enable)
					MenuOn = (*mInfo->minf_MenuCmd->smcom_Enable)(iwt, NULL);
				else
					MenuOn = TRUE;

				if (MenuOn)
					MfOnMenu(iwt->iwt_WindowTask.wt_Window, mInfo);
				else
					MfOffMenu(iwt->iwt_WindowTask.wt_Window, mInfo);
				}

			ScalosReleaseSemaphore(&MenuSema);
			}
		ScalosReleaseSemaphore(&ParseMenuListSemaphore);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SetIconMenuOnOff(iwt);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetWindowMenuOnOff(iwt);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetControlBarOnOff(iwt);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void MenuFunc_ViewByIcon(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (IDTV_ViewModes_Icon == iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ViewByText(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (IDTV_ViewModes_Name == iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ViewByDate(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (IDTV_ViewModes_Date == iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ViewBySize(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (IDTV_ViewModes_Size == iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ViewByType(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (IDTV_ViewModes_Type == iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ViewByDefault(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (IDTV_ViewModes_Default == iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ShowOnlyIcons(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (DDFLAGS_SHOWICONS == iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ShowAllFiles(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (DDFLAGS_SHOWALL == iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll)
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static void MenuFunc_ShowDefault(struct internalScaWindowTask *iwt, struct MenuInfo *mInfo)
{
	d1(kprintf("%s/%s/%ld: mInfo=%08lx Type=%04lx\n", __FILE__, __FUNC__, __LINE__, mInfo, mInfo->minf_type));

	if (DDFLAGS_SHOWDEFAULT == (DDFLAGS_SHOWMASK & iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll))
		{
		mInfo->minf_item->Flags |= CHECKED;
		}
	else
		{
		mInfo->minf_item->Flags &= ~CHECKED;
		}
}


static ULONG MfOffMenu(struct Window *win, struct MenuInfo *mInfo)
{
	ULONG Changed = 0;

	d1(kprintf("%s/%s/%ld: win=%08lx  mInfo=%08lx  MenuStrip=%08lx\n", __FILE__, __FUNC__, __LINE__, win, mInfo, win->MenuStrip));

	if (win && win->MenuStrip && (win->MenuStrip == MainMenu) &&
		(mInfo->minf_item->Flags & ITEMENABLED) )
		{
		d1(kprintf("%s/%s/%ld: win=%08lx  minf_number=%08lx  minf_item=%08lx\n", __FILE__, __FUNC__, __LINE__, win, mInfo->minf_number, mInfo->minf_item));
		mInfo->minf_item->Flags &= ~ITEMENABLED;
		Changed++;
		}
	return Changed;
}


static ULONG MfOnMenu(struct Window *win, struct MenuInfo *mInfo)
{
	ULONG Changed = 0;

	d1(kprintf("%s/%s/%ld: win=%08lx  mInfo=%08lx  MenuStrip=%08lx\n", __FILE__, __FUNC__, __LINE__, win, mInfo, win->MenuStrip));

	if (win && win->MenuStrip && (win->MenuStrip == MainMenu) &&
		!(mInfo->minf_item->Flags & ITEMENABLED) )
		{
		d1(kprintf("%s/%s/%ld: win=%08lx  minf_number=%08lx  minf_item=%08lx\n", __FILE__, __FUNC__, __LINE__, win, mInfo->minf_number, mInfo->minf_item));
		mInfo->minf_item->Flags |= ITEMENABLED;
		Changed++;
		}
	return Changed;
}



// Disable MenuItem if it has SubItems and every SubItem is disabled.
static void SubMenusOnOff(struct internalScaWindowTask *iwt, ULONG Changed, ULONG TextChanged)
{
	struct Menu *menu = MainMenu;

	while (menu)
		{
		struct MenuItem *mItem;

		for (mItem=menu->FirstItem; mItem; mItem=mItem->NextItem)
			{
			BOOL ItemOn = FALSE;

			if (mItem->SubItem)
				{
				struct MenuItem *mSub;

				for (mSub=mItem->SubItem; !ItemOn && mSub; mSub=mSub->NextItem)
					{
					if (mSub->Flags & ITEMENABLED)
						ItemOn = TRUE;
					}

				if (ItemOn)
					{
					if (!(mItem->Flags & ITEMENABLED))
						Changed++;

					mItem->Flags |= ITEMENABLED;
					}
				else
					{
					if (mItem->Flags & ITEMENABLED)
						Changed++;

					mItem->Flags &= ~ITEMENABLED;
					}
				}
			}

		menu = menu->NextMenu;
		}

	d1(KPrintF("%s/%s/%ld: Changed=%lu\n", __FILE__, __FUNC__, __LINE__, Changed));

	if (Changed && iwt->iwt_WindowTask.wt_Window && (iwt->iwt_WindowTask.wt_Window->MenuStrip == MainMenu))
		{
		d1(KPrintF("%s/%s/%ld: calling ResetMenuStrip\n", __FILE__, __FUNC__, __LINE__));
		if (TextChanged)
			{
			ClearMenuStrip(iwt->iwt_WindowTask.wt_Window);
			LayoutMenus(MainMenu, iInfos.xii_iinfos.ii_visualinfo,
				GTMN_NewLookMenus, TRUE,
				TAG_END);
			SetMenuStrip(iwt->iwt_WindowTask.wt_Window, MainMenu);
			}
		else
			{
			ResetMenuStrip(iwt->iwt_WindowTask.wt_Window, MainMenu);
			}
		}
}


void NewMenuPrefs(struct internalScaWindowTask *iwt, struct NotifyMessage *msg)
{
	struct MainTask *mt = MainWindowTask;

	d1(KPrintF("%s/%s/%ld: smnp_PrefsFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, SMNPFLAGF_MENUPREFS));

	if (mt->mt_ChangedMenuPrefsName)
		FreeCopyString(mt->mt_ChangedMenuPrefsName);
	mt->mt_ChangedMenuPrefsName = AllocCopyString(msg->nm_NReq->nr_FullName);

	if (mt->mt_ChangedMenuPrefsName)
		{
		struct SM_NewPreferences *smnp;

		smnp = (struct SM_NewPreferences *) SCA_AllocMessage(MTYP_NewPreferences, 0);
		if (smnp)
			{
			smnp->smnp_PrefsFlags = SMNPFLAGF_MENUPREFS;
			PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smnp->ScalosMessage.sm_Message);
			}
		}
}


BOOL ChangedMenuPrefs(struct MainTask *mt)
{
	ULONG NewMenuPrefsCRC;
	BOOL Changed = FALSE;

	NewMenuPrefsCRC = GetPrefsCRCFromName(mt->mt_ChangedMenuPrefsName);
	d1(KPrintF("%s/%s/%ld: NewMenuPrefsCRC=%08lx  MenuPrefsCRC=%08lx\n",
		__FILE__, __FUNC__, __LINE__, NewMenuPrefsCRC, MenuPrefsCRC));

	if (NewMenuPrefsCRC != MenuPrefsCRC)
		{
		d1(kprintf("\n" "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		Changed = TRUE;

		ClearMainMenu();

		ScalosObtainSemaphore(&ParseMenuListSemaphore);

		FreeMenuPrefs();

		SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &ParseMenuList);

		ScalosReleaseSemaphore(&ParseMenuListSemaphore);

		ReadMenuPrefs();	// updates MenuPrefsCRC

		d1(kprintf("%s/%s/%ld: MainNewMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainNewMenu));

		AppMenu_ResetMenu();
		}

	return Changed;
}


void CreateScalosMenu(void)
{
	d1(kprintf("%s/%s/%ld: MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu));
	AppMenu_CreateMenu();
}


ULONG UpdateIconCount(struct internalScaWindowTask *iwt)
{
	ULONG SupportFlags = 0L;

	if (ScalosAttemptLockIconListShared(iwt))
		{
		struct ScaIconNode *in;
		ULONG TotalItems = 0, SelectedItems = 0;

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct Gadget *gg = (struct Gadget *) in->in_Icon;

			TotalItems++;

			d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", \
				__FILE__, __FUNC__, __LINE__, in, in->in_Name));

			if (gg->Flags & GFLG_SELECTED)
				{
				SupportFlags |= in->in_SupportFlags;
				SelectedItems++;
				}
			}

		ScalosUnLockIconList(iwt);

		d1(kprintf("%s/%s/%ld: iwt=%08lx  StatusBar=%08lx  StatusTextImage=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_StatusBar, iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText]));

		if (iwt->iwt_StatusBar && iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText] && !iwt->iwt_Reading)
			{
			char StatusBarText[256];
			CONST_STRPTR oldText;

			ScaFormatStringMaxLength(StatusBarText, sizeof(StatusBarText),
				GetLocString(MSGID_STATUSBARTEXT), TotalItems, SelectedItems);

			d1(kprintf("%s/%s/%ld: StatusBarText=<%s>\n", __FILE__, __FUNC__, __LINE__, StatusBarText));

			GetAttr(GBTDTA_Text, (Object *) iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText], (APTR) &oldText);

			if (NULL == oldText || 0 != strcmp(oldText, StatusBarText))
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_UpdateStatusBar,
					iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText],
					GBTDTA_Text, StatusBarText,
					TAG_END);
				}
			}

		SetControlBarOnOff(iwt);
		}

	return SupportFlags;
}


void ClearMainMenu(void)
{
	d1(kprintf("%s/%s/%ld: MainMenu=%08lx\n", __FILE__, __FUNC__, __LINE__, MainMenu));

	if (MainMenu)
		{
		struct ScaWindowStruct *ws;

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (ws->ws_Window)
				SafeClearMenuStrip(ws->ws_Window);
			}

		FreeMenus(MainMenu);
		MainMenu = NULL;

		SCA_UnLockWindowList();
		}
}


// add record about allocated menu image to global list,
// for easy freeing of menu images when menu prefs are (re)loaded
static void AddMenuImage(Object *img)
{
	struct ScalosMenuImage *smi;

	// make sure MenuImageList is initialized
	if (NULL == MenuImageList.lh_Head)
		NewList(&MenuImageList);

	smi = ScalosAlloc(sizeof(struct ScalosMenuImage));
	if (smi)
		{
		smi->smi_Image = img;
		AddTail(&MenuImageList, &smi->smi_Node);
		}
}

static void CleanupMenuImages(void)
{
	struct ScalosMenuImage *smi;

	// make sure MenuImageList is initialized
	if (NULL == MenuImageList.lh_Head)
		NewList(&MenuImageList);


	while ((smi = (struct ScalosMenuImage *) RemHead(&MenuImageList)))
		{
		if (smi->smi_Image)
			{
			DisposeObject(smi->smi_Image);
			smi->smi_Image = NULL;
			}
		ScalosFree(smi);
		}
}

