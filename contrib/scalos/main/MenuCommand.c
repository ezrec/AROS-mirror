// MenuCommand.c
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
#include <libraries/asl.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/pictureclass.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures


// extended SM_AsyncRoutine to run xxxProg() asynchronously
struct SM_AsyncProg
	{
	struct	SM_AsyncRoutine sma_Async;
	struct	MenuCmdArg sma_CmdArg;
	};

// xxxProg() Function called by ProgTask()
typedef void (*PROGFUNC)(struct internalScaWindowTask *, const struct MenuCmdArg *);

struct SM_StartProg
	{
	PROGFUNC spr_ProgFunc;

	struct internalScaWindowTask *spr_WindowTask;

	struct MenuCmdArg spr_Arg;
	};

struct CloneArg
	{
	struct WBArg *cla_wbArg;
	ULONG cla_NumArgs;	// number of entries in cla_wbArg
	};

//----------------------------------------------------------------------------

// local functions

static void SizeToFitProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void WindowPropertiesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void SelectContentsProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void NewDrawerProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void RenameProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuInfoProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void IconPropertiesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void BackDropProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void RedrawAllProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ViewByIconProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ViewByTextProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ViewByDateProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ViewBySizeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ViewByTypeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ViewByDefaultProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CloneProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ThumbnailCacheCleanupProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void UpdateProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CloseProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void FindProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void FormatDiskProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ShutdownProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ShowOnlyIconsProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ShowAllFilesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ShowDefaultProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CleanupProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CleanupByNameProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CleanupByDateProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CleanupBySizeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void CleanupByTypeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuOpenProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuOpenNewProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuOpenBrowserWindowProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void OpenParentProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ExecuteCmdProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void LastMessageProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void IconifyProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void RedrawProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void ResetScalosProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void AsyncResetScalosProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *dummy);
static void MenuSnapshotProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuUnsnapshotProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuUndoProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuRedoProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void SnapshotAllProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void SnapshotWindowProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void LeaveOutProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void PutAwayProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void EmptyTrashProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuCopyProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuCutProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuPasteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void MenuCreateThumbnailProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);

static void AsyncPasteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void SnapshotWindow(struct internalScaWindowTask *iwt);
static BOOL StartProgTask(struct internalScaWindowTask *iwt, 
	const struct MenuCmdArg *mcArg, PROGFUNC ProgProc, struct MsgPort *ReplyPort);
static SAVEDS(ULONG) ProgTask(struct SM_StartProg *sMsg, struct SM_RunProcess *msg);
static void AsyncNewDrawerProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void WBIconPropertiesStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static SAVEDS(ULONG) IconPropertiesStart(APTR aptr, struct SM_RunProcess *msg);
static void AsyncWindowPropertiesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
void AsyncRenameProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
void AsyncDeleteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static void LeaveOutIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static void PutAwayIconNode(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static void DoForAllWindowsShared(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep),
	void (*WindowProc)(struct internalScaWindowTask *iwt));
static void DoForAllIconsShared(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep),
	APTR undoStep);
static void DoForAllWindows(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep));
static void DoForAllIconsExclusive(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep),
	APTR undoStep);
static SAVEDS(ULONG) InfoStart(APTR aptr, struct SM_RunProcess *msg);
//static struct ScaIconNode *FindIconGlobal(BPTR dirLock, CONST_STRPTR Name);
static void ClearSelectionInWindow(struct internalScaWindowTask *iwt);
static void MakeCloneArgs(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount);
static void CloneInfoStart(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount);
static BOOL CloneWBArg(struct internalScaWindowTask *iwt, struct WBArg **wbArg, 
	struct ScaIconNode *in, ULONG *ArgCount);
static SAVEDS(ULONG) CloneFilesStart(APTR aptr, struct SM_RunProcess *msg);
static SAVEDS(LONG) IconCompareFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1);
static void SnapshotIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static void UnsnapshotIconAndSave(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static void UnsnapshotIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, BOOL SaveIcon, APTR undoStep);
static void AfterUnsnapshotIcon(struct internalScaWindowTask *iwt);
static void SetRect(struct ScaIconNode *in);
static void SetRect2(struct ScaIconNode *in, BPTR iconLock);
static void CloseWindowProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static ULONG FindIconType(struct WBArg *Arg);
static BOOL CallGlobalDeleteHook(struct WBArg *wbArg, ULONG nArgs, ULONG Action);
static void DeleteModuleKludge(struct WBArg *wbArg, ULONG nArgs);
static void RememberFilesToCopy(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static void RememberFilesToCut(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static ULONG CallTextInputHook(ULONG Action, STRPTR Prompt);
static STRPTR FindUniqueName(STRPTR Name, size_t MaxLength);
static BOOL CanBeSizedMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IsMainWindowMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL NotMainWindowMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL NotRootWindowMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL MustHaveLockMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IconsSelectedAndWriteableMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IconsSelectedMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IconsSelectedBrowserModeMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IconsSelectedNotBrowserModeMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL EmptyTrashMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL FormatDiskMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL LeaveOutMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL PutAwayMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL PasteMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL UndoMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL RedoMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL NotMainWindowAndNotReadingMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL SnapshotMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL NotReadingMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IsWriteableMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static BOOL IconsSelectedNoDiskMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in); // +jmc+ - Diable menu item if in->in_Device.
static SAVEDS(ULONG) AsyncStartModule(APTR aptr, struct SM_RunProcess *msg, CONST_STRPTR ModuleName);
static void CreateThumbnailStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep);
static void DoForAppIcons(ULONG AppMsgClass);

//--- +jmc+ --- CopyTo/MoveTo ------------------------------------------------
 
static void CopyToProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static SAVEDS(ULONG) CopyToStart(APTR aptr, struct SM_RunProcess *msg);

static void MoveToProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg);
static SAVEDS(ULONG) MoveToStart(APTR aptr, struct SM_RunProcess *msg);

static void CopyToMakeCloneArgs(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs);
static void CopyToCloneInfoStart(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs);
static BOOL CopyToCloneWBArg(struct internalScaWindowTask *iwt, struct WBArg **wbArg, 
	struct ScaIconNode *in, ULONG *ArgCount, ULONG MaxArgs);

static void MoveToMakeCloneArgs(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs);
static void MoveToCloneInfoStart(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs);

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------

// local Data items

//----------------------------------------------------------------------------

// public data items

struct ScalosMenuCommand MenuCommandTable[] =
	{
	{ "about",		SMCOMFLAGF_RunFromRootWindow,	MITYPE_NOTHING,		AboutProg,			NULL					},
	{ "backdrop",		SMCOMFLAGF_RunFromRootWindow,	MITYPEF_Backdrop,	BackDropProg,			IsMainWindowMenuEnable			},
	{ "cleanup",		0,				MITYPE_NOTHING,		CleanupProg,			NULL					},
	{ "cleanupbyname",	0,				MITYPE_NOTHING,		CleanupByNameProg,		NULL					},
	{ "cleanupbydate",	0,				MITYPE_NOTHING,		CleanupByDateProg,		NULL					},
	{ "cleanupbysize",	0,				MITYPE_NOTHING,		CleanupBySizeProg,		NULL					},
	{ "cleanupbytype",	0,				MITYPE_NOTHING,		CleanupByTypeProg,		NULL					},
	{ "clearselection",	0,				MITYPE_NOTHING,		ClearSelectionProg,		NULL					},
	{ "createthumbnail",	0,				MITYPE_NOTHING,		MenuCreateThumbnailProg,       	IconsSelectedAndWriteableMenuEnable	},
	{ "clone",		0,				MITYPE_NOTHING,		CloneProg,			IconsSelectedAndWriteableMenuEnable	},
	{ "close",		0,				MITYPE_NOTHING,		CloseProg,			NotMainWindowMenuEnable			},
	{ "copy",		0,				MITYPE_NOTHING,		MenuCopyProg,			IconsSelectedMenuEnable			},
	{ "copyto",		0,				MITYPE_NOTHING,		CopyToProg,			IconsSelectedMenuEnable			},
	{ "cut",		0,				MITYPE_NOTHING,		MenuCutProg,			IconsSelectedNoDiskMenuEnable		}, // IconsSelectedAndWriteableMenuEnable
	{ "delete",		0,				MITYPE_NOTHING,		DeleteProg,			IconsSelectedAndWriteableMenuEnable	},
	{ "emptytrashcan",	0,				MITYPE_NOTHING,		EmptyTrashProg,			EmptyTrashMenuEnable			},
	{ "executecommand",	0,				MITYPE_NOTHING,		ExecuteCmdProg,			NULL					},
	{ "find",		0,				MITYPE_NOTHING,		FindProg,			NULL					},
	{ "formatdisk",		0,				MITYPE_NOTHING,		FormatDiskProg,			FormatDiskMenuEnable			},
	{ "iconify",		0,				MITYPE_NOTHING,		IconifyProg,			CanBeSizedMenuEnable			},
	{ "iconinfo",		0,				MITYPE_NOTHING,		MenuInfoProg,			IconsSelectedMenuEnable			},
	{ "iconproperties",	0,				MITYPE_NOTHING,		IconPropertiesProg,		IconsSelectedMenuEnable			},
	{ "lastmsg",		0,				MITYPE_NOTHING,		LastMessageProg,		NULL					},
	{ "leaveout",		0,				MITYPE_NOTHING,		LeaveOutProg,			LeaveOutMenuEnable			},
	{ "makedir",		0,				MITYPE_NOTHING,		NewDrawerProg,			MustHaveLockMenuEnable			},
	{ "moveto",		0,				MITYPE_NOTHING,		MoveToProg,			IconsSelectedNoDiskMenuEnable		}, // IconsSelectedAndWriteableMenuEnable
	{ "open",		0,				MITYPE_NOTHING,		MenuOpenProg,			IconsSelectedMenuEnable			},
	{ "openinbrowserwindow",0,				MITYPE_NOTHING,		MenuOpenBrowserWindowProg,	IconsSelectedNotBrowserModeMenuEnable	},
	{ "openinnewwindow",	0,				MITYPE_NOTHING,		MenuOpenNewProg,		IconsSelectedBrowserModeMenuEnable	},
	{ "parent",		0,				MITYPE_NOTHING,		OpenParentProg,			NotRootWindowMenuEnable			},
	{ "paste",		0,				MITYPE_NOTHING,		MenuPasteProg,			PasteMenuEnable				},
	{ "putaway",		0,				MITYPE_NOTHING,		PutAwayProg,			PutAwayMenuEnable			},
	{ "quit",		SMCOMFLAGF_RunFromRootWindow,	MITYPE_NOTHING,		CloseWindowProg,		NULL					},
	{ "redo",		0,				MITYPE_NOTHING,		MenuRedoProg,			RedoMenuEnable				},
	{ "redraw",		0,				MITYPE_NOTHING,		RedrawProg,			NULL					},
	{ "redrawall",		SMCOMFLAGF_RunFromRootWindow,	MITYPE_NOTHING,		RedrawAllProg,			NULL					},
	{ "rename",		0,				MITYPE_NOTHING,		RenameProg,			IconsSelectedAndWriteableMenuEnable	},
	{ "reset",		SMCOMFLAGF_RunFromRootWindow,	MITYPE_NOTHING,		ResetScalosProg,		NULL					},
	{ "selectall",		0,				MITYPE_NOTHING,		SelectContentsProg,		NULL					},
	{ "showallfiles",	0,				MITYPEF_ShowAllFiles,	ShowAllFilesProg,		NotMainWindowAndNotReadingMenuEnable	},
	{ "showonlyicons",	0,				MITYPEF_ShowOnlyIcons,	ShowOnlyIconsProg,		NotMainWindowAndNotReadingMenuEnable	},
	{ "showdefault",	0,				MITYPEF_ShowDefault,	ShowDefaultProg,		NotMainWindowAndNotReadingMenuEnable	},
	{ "shutdown",		SMCOMFLAGF_RunFromRootWindow,	MITYPE_NOTHING,		ShutdownProg,			NULL					},
	{ "sizetofit",		0,				MITYPE_NOTHING,		SizeToFitProg,			CanBeSizedMenuEnable			},
	{ "snapshot",		0,				MITYPE_NOTHING,		MenuSnapshotProg,		SnapshotMenuEnable			},
	{ "snapshotall",	0,				MITYPE_NOTHING,		SnapshotAllProg,		IsWriteableMenuEnable			},
	{ "snapshotwindow",	0,				MITYPE_NOTHING,		SnapshotWindowProg,		IsWriteableMenuEnable			},
	{ "thumbnailcachecleanup",     0,			MITYPE_NOTHING,		ThumbnailCacheCleanupProg,	NULL					},
	{ "undo",		0,				MITYPE_NOTHING,		MenuUndoProg,			UndoMenuEnable				},
	{ "unsnapshot",		0,				MITYPE_NOTHING,		MenuUnsnapshotProg,		SnapshotMenuEnable			},
	{ "update",		0,				MITYPE_NOTHING,		UpdateProg,			NotReadingMenuEnable			},
	{ "updateall",		SMCOMFLAGF_RunFromRootWindow,	MITYPE_NOTHING,		UpdateAllProg,			NULL					},
	{ "viewbydate",		0,				MITYPEF_ViewByDate,	ViewByDateProg,			NotMainWindowAndNotReadingMenuEnable	},
	{ "viewbyicon",		0,				MITYPEF_ViewByIcon,	ViewByIconProg,			NotMainWindowAndNotReadingMenuEnable	},
	{ "viewbysize",		0,				MITYPEF_ViewBySize,	ViewBySizeProg,			NotMainWindowAndNotReadingMenuEnable	},
	{ "viewbytype",		0,				MITYPEF_ViewByType,	ViewByTypeProg,			NotMainWindowAndNotReadingMenuEnable	},
	{ "viewbytext",		0,				MITYPEF_ViewByText,	ViewByTextProg,			NotMainWindowAndNotReadingMenuEnable	},
	{ "viewbydefault",	0,				MITYPEF_ViewByDefault,	ViewByDefaultProg,		NotMainWindowAndNotReadingMenuEnable	},
	{ "windowproperties",	0,				MITYPE_NOTHING,		WindowPropertiesProg,		NotReadingMenuEnable			},
	{ NULL,			0,				0,			NULL,				NULL					},
	};

struct List globalCopyClipBoard;	// Clipboard for files/directories via copy/cut/paste
SCALOSSEMAPHORE ClipboardSemaphore;	// Semaphore to protect globalCopyClipBoard

//----------------------------------------------------------------------------

static void MenuInfoProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		WBInfoStart(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindowsShared(iwt, WBInfoStart, NULL);
		}
}

//----------------------------------------------------------------------------

static void IconPropertiesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		WBIconPropertiesStart(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindowsShared(iwt, WBIconPropertiesStart, NULL);
		}
}

//----------------------------------------------------------------------------

static void DoForAllWindowsShared(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep),
	void (*WindowProc)(struct internalScaWindowTask *iwt))
{
	struct ScaWindowStruct *ws;
	APTR undoStep;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	undoStep = UndoBeginStep();

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	DoForAllIconsShared(iwt, IconProc, undoStep);
	if (WindowProc)
		(*WindowProc)(iwt);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws != iwt->iwt_WindowTask.mt_WindowStruct)
			{
			DoForAllIconsShared((struct internalScaWindowTask *) ws->ws_WindowTask, IconProc, undoStep);
			if (WindowProc)
				(*WindowProc)((struct internalScaWindowTask *) ws->ws_WindowTask);
			}
		}

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	SCA_UnLockWindowList();

	UndoEndStep(iwt, undoStep);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
}


static void DoForAllIconsShared(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep),
	APTR undoStep)
{
	struct ScaIconNode *in;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			{
			d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
			(*IconProc)(iwt, in, undoStep);
			}
		}

	ScalosUnLockIconList(iwt);
}


static void DoForAllWindows(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep))
{
	struct ScaWindowStruct *ws;
	APTR undoStep;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	undoStep = UndoBeginStep();

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	DoForAllIconsExclusive(iwt, IconProc, undoStep);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws != iwt->iwt_WindowTask.mt_WindowStruct)
			DoForAllIconsExclusive((struct internalScaWindowTask *) ws->ws_WindowTask, IconProc, undoStep);
		}

	SCA_UnLockWindowList();

	UndoEndStep(iwt, undoStep);
}


// IconProc might modify wt_IconList !!
static void DoForAllIconsExclusive(struct internalScaWindowTask *iwt,
	void (*IconProc)(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep),
	APTR undoStep)
{
	ULONG IconCount;
	struct ScaIconNode *in;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosLockIconListExclusive(iwt);

	// Count icons in window
	for (IconCount = 0, in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		IconCount++;

	if (IconCount)
		{
		struct ScaIconNode **IconArray;

		IconArray = ScalosAlloc(IconCount * sizeof(struct ScaIconNode *));
		if (IconArray)
			{
			ULONG n;

			// Collect addresses of all icons in IconArray
			for (n = 0, in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				IconArray[n++] = in;

			// Now iterate through the collected icons
			for (n = 0; n < IconCount; n++)
				{
				// verify that IconArray[n] is still present
				for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					if (in == IconArray[n])
						{
						struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

						if (gg->Flags & GFLG_SELECTED)
							(*IconProc)(iwt, in, undoStep);
						break;
						}
					}
				}

			ScalosFree(IconArray);
			}
		}

	ScalosUnLockIconList(iwt);
}

//----------------------------------------------------------------------------

void WBInfoStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	ULONG IconType;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	(void) undoStep;

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_Information, in);
		}
	else
		{
		struct WBArg InfoArgs;

		if (in->in_DeviceIcon)
			{
			InfoArgs.wa_Lock = DiskInfoLock(in);
			InfoArgs.wa_Name = AllocCopyString("Disk");	// look for "Disk.info"
			}
		else
			{
			InfoArgs.wa_Name = AllocCopyString(GetIconName(in));

			if (in->in_Lock)
				InfoArgs.wa_Lock = DupLock(in->in_Lock);
			else
				InfoArgs.wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
			}

		d1(KPrintF("%s/%s/%ld: InfoArgs.wa_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
			InfoArgs.wa_Name, InfoArgs.wa_Name ? InfoArgs.wa_Name : (BYTE *) ""));
		debugLock_d1(InfoArgs.wa_Lock);

		RunProcess(&iwt->iwt_WindowTask, InfoStart, sizeof(InfoArgs)/sizeof(ULONG), &InfoArgs, iInfos.xii_iinfos.ii_MainMsgPort);
		}
}


static SAVEDS(ULONG) InfoStart(APTR aptr, struct SM_RunProcess *msg)
{
	return AsyncStartModule(aptr, msg, "Information.module");
}

//----------------------------------------------------------------------------

static void SizeToFitProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct Rectangle VirtRect;
	WORD NewInnerWidth, NewInnerHeight;
	WORD NewLeft, NewTop, NewWidth, NewHeight;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: vWidth=%ld  vHeight=%ld  iWidth=%ld  iHeight=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_InvisibleWidth, iwt->iwt_InvisibleHeight, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));

	if (iwt->iwt_BackDrop)
		return;

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);

	GetIconsBoundingBox(iwt, &VirtRect);

	d1(kprintf("%s/%s/%ld: MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		VirtRect.MinX, VirtRect.MinY, VirtRect.MaxX, VirtRect.MaxY));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		{
		ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
		return;
		}

	NewInnerWidth = VirtRect.MaxX - VirtRect.MinX + 1;
	NewInnerHeight = VirtRect.MaxY - VirtRect.MinY + 1;

	NewLeft = iwt->iwt_WindowTask.wt_Window->LeftEdge;
	NewTop = iwt->iwt_WindowTask.wt_Window->TopEdge;
	NewWidth = iwt->iwt_WindowTask.wt_Window->Width;
	NewHeight = iwt->iwt_WindowTask.wt_Window->Height;

	if (iwt->iwt_InnerWidth != NewInnerWidth)
		{
		NewWidth = NewInnerWidth + iwt->iwt_InnerLeft + iwt->iwt_InnerRight;

		if (NewWidth > iwt->iwt_WinScreen->Width)
			NewWidth = iwt->iwt_WinScreen->Width;
		if (NewLeft + NewWidth > iwt->iwt_WinScreen->Width)
			NewLeft = iwt->iwt_WinScreen->Width - NewWidth;
		}
	if (iwt->iwt_InnerHeight != NewInnerHeight)
		{
		NewHeight = NewInnerHeight + iwt->iwt_InnerTop + iwt->iwt_InnerBottom;

		if (NewHeight > iwt->iwt_WinScreen->Height)
			NewHeight = iwt->iwt_WinScreen->Height;
		if (NewLeft + NewHeight > iwt->iwt_WinScreen->Height)
			NewLeft = iwt->iwt_WinScreen->Height - NewHeight;
		}

	UndoAddEvent(iwt, UNDO_SizeWindow,
		UNDOTAG_WindowTask, iwt,
		UNDOTAG_OldWindowLeft, iwt->iwt_WindowTask.wt_Window->LeftEdge,
		UNDOTAG_OldWindowTop, iwt->iwt_WindowTask.wt_Window->TopEdge,
		UNDOTAG_OldWindowWidth, iwt->iwt_WindowTask.wt_Window->Width,
		UNDOTAG_OldWindowHeight, iwt->iwt_WindowTask.wt_Window->Height,
		UNDOTAG_NewWindowLeft, NewLeft,
		UNDOTAG_NewWindowTop, NewTop,
		UNDOTAG_NewWindowWidth, NewWidth,
		UNDOTAG_NewWindowHeight, NewHeight,
		UNDOTAG_OldWindowVirtX, iwt->iwt_WindowTask.wt_XOffset,
		UNDOTAG_OldWindowVirtY, iwt->iwt_WindowTask.wt_YOffset,
		UNDOTAG_NewWindowVirtX, VirtRect.MinX,
		UNDOTAG_NewWindowVirtY, VirtRect.MinY,
		TAG_END);

	d1(kprintf("%s/%s/%ld: XOffset=%ld  YOffset=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_WindowTask.wt_XOffset, iwt->iwt_WindowTask.wt_YOffset));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, 
		VirtRect.MinX - iwt->iwt_WindowTask.wt_XOffset,
		VirtRect.MinY - iwt->iwt_WindowTask.wt_YOffset);

	d1(KPrintF("%s/%s/%ld: Left=%ld  Top=%ld  Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
		NewLeft, NewTop, NewWidth, NewHeight));

	ChangeWindowBox(iwt->iwt_WindowTask.wt_Window, NewLeft, NewTop, NewWidth, NewHeight);

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
}

//----------------------------------------------------------------------------

static void WindowPropertiesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);	// prevent window from closing

	StartProgTask(iwt, mcArg, AsyncWindowPropertiesProg, (struct MsgPort *) SCCV_RunProcess_WaitReply);

	d1(kprintf("%s/%s/%ld: Release WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
}

//----------------------------------------------------------------------------

void ClearSelectionProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	// Deselect all icons in all windows
	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		ClearSelectionInWindow((struct internalScaWindowTask *) ws->ws_WindowTask);
		}

	SCA_UnLockWindowList();

	AdjustIconActive(iwt);

	SetMenuOnOff(iwt);
}


static void SelectContentsProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct ScaIconNode *in;
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	// Deselect all icons in other windows
	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws != iwt->iwt_WindowTask.mt_WindowStruct)
			ClearSelectionInWindow((struct internalScaWindowTask *) ws->ws_WindowTask);
		}

	ScalosLockIconListExclusive(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, TRUE);
		}

	ScalosUnLockIconList(iwt);

	SCA_UnLockWindowList();

	AdjustIconActive(iwt);

	SetMenuOnOff(iwt);
}


static void NewDrawerProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	LONG Result;
	char NewDrawerName[108];

	d1(kprintf("%s/%s/%ld: Obtain WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);	// prevent window from closing

	stccpy(NewDrawerName, GetLocString(MSGID_NEW_DRAWER_NAME), sizeof(NewDrawerName));

	Result = CallTextInputHook(TIACTION_NewDrawer, FindUniqueName(NewDrawerName, sizeof(NewDrawerName)));

	if (Result)
		StartProgTask(iwt, mcArg, AsyncNewDrawerProg, (struct MsgPort *) SCCV_RunProcess_WaitReply);

	d1(kprintf("%s/%s/%ld: Release WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
}


static void RenameProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: Obtain WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);	// prevent window from closing

	StartProgTask(iwt, mcArg, AsyncRenameProg, (struct MsgPort *) SCCV_RunProcess_WaitReply);

	d1(kprintf("%s/%s/%ld: Release WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
}


void DeleteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: Obtain WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);	// prevent window from closing

	StartProgTask(iwt, mcArg, AsyncDeleteProg, (struct MsgPort *) SCCV_RunProcess_WaitReply);

	d1(kprintf("%s/%s/%ld: Release WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
}


static BOOL StartProgTask(struct internalScaWindowTask *iwt, 
	const struct MenuCmdArg *mcArg, PROGFUNC ProgProc, struct MsgPort *ReplyPort)
{
	struct SM_StartProg sMsg;
	BOOL Success;

	sMsg.spr_ProgFunc = ProgProc;
	sMsg.spr_WindowTask = iwt;
	sMsg.spr_Arg = *mcArg;

	Success = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_RunProcess, ProgTask,
		&sMsg, sizeof(sMsg), ReplyPort);

	return Success;
}


static SAVEDS(ULONG) ProgTask(struct SM_StartProg *sMsg, struct SM_RunProcess *msg)
{
	static CONST_STRPTR ProgTaskName = "Scalos_RunProg";
	struct internalScaWindowTask *iwt;
	PROGFUNC ProgFunc;
	APTR prWindowPtr;
	struct MenuCmdArg Arg;
	struct Process *myProc = (struct Process *) FindTask(NULL);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	prWindowPtr = myProc->pr_WindowPtr;
	myProc->pr_WindowPtr = (APTR) ~0;    // suppress error requesters

	myProc->pr_Task.tc_Node.ln_Name = (STRPTR) ProgTaskName;
	SetProgramName(ProgTaskName);

	iwt = sMsg->spr_WindowTask;
	ProgFunc = sMsg->spr_ProgFunc;
	Arg = sMsg->spr_Arg;

	d1(kprintf("%s/%s/%ld: Obtain WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);

	(*ProgFunc)(iwt, &Arg);

	d1(kprintf("%s/%s/%ld: Release WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);

	// restore pr_WindowPtr
	myProc->pr_WindowPtr = prWindowPtr;

	return 0;
}

//----------------------------------------------------------------------------

static void AsyncNewDrawerProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	BOOL Success;
	struct WBArg wbArg[2];

	wbArg[1].wa_Name = NULL;
	wbArg[1].wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
	if ((BPTR)NULL == wbArg[1].wa_Lock)
		return;

	wbArg[0].wa_Name = (STRPTR) "newdrawer.module";
	wbArg[0].wa_Lock = LockScaModsDir();
	if ((BPTR)NULL == wbArg[0].wa_Lock)
		{
		UnLock(wbArg[1].wa_Lock);
		return;
		}

	// SCA_WBStart()
	Success = SCA_WBStartTags(wbArg, 2,
			SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
			SCA_Flags, SCAF_WBStart_Wait,
			SCA_WaitTimeout, 0,
			TAG_END);

	if (!Success)
		{
		UnLock(wbArg[0].wa_Lock);
		UnLock(wbArg[1].wa_Lock);
		}

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
}

//----------------------------------------------------------------------------

static void WBIconPropertiesStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	struct WBArg InfoArgs;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	if (in->in_DeviceIcon)
		{
		InfoArgs.wa_Lock = DiskInfoLock(in);
		InfoArgs.wa_Name = AllocCopyString("Disk");	// look for "Disk.info"
		}
	else
		{
		InfoArgs.wa_Name = AllocCopyString(GetIconName(in));

		if (in->in_Lock)
			InfoArgs.wa_Lock = DupLock(in->in_Lock);
		else
			InfoArgs.wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		}

	d1(kprintf("%s/%s/%ld: InfoArgs.wa_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, \
		InfoArgs.wa_Name, InfoArgs.wa_Name ? InfoArgs.wa_Name : ""));
	debugLock_d1(InfoArgs.wa_Lock);

	RunProcess(&iwt->iwt_WindowTask, IconPropertiesStart, sizeof(InfoArgs)/sizeof(ULONG), &InfoArgs, iInfos.xii_iinfos.ii_MainMsgPort);
}


static SAVEDS(ULONG) IconPropertiesStart(APTR aptr, struct SM_RunProcess *msg)
{
	ULONG Result;

	d1(KPrintF("%s/%s/%ld: START aptr=%08lx  msg=%08lx\n", __FILE__, __FUNC__, __LINE__, aptr, msg));
	Result = AsyncStartModule(aptr, msg, "iconproperties.module");
	d1(KPrintF("%s/%s/%ld: END  Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}

//----------------------------------------------------------------------------

static void AsyncWindowPropertiesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	BOOL Success;
	struct WBArg wbArg[2];

	wbArg[1].wa_Name = NULL;
	wbArg[1].wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
	if ((BPTR)NULL == wbArg[1].wa_Lock)
		return;

	wbArg[0].wa_Name = (STRPTR) "windowproperties.module";
	wbArg[0].wa_Lock = LockScaModsDir();
	if ((BPTR)NULL == wbArg[0].wa_Lock)
		{
		UnLock(wbArg[1].wa_Lock);
		return;
		}

	// SCA_WBStart()
	Success = SCA_WBStartTags(wbArg, 2,
			SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
			SCA_Flags, SCAF_WBStart_Wait,
			SCA_WaitTimeout, 0,
			TAG_END);

	if (!Success)
		{
		UnLock(wbArg[0].wa_Lock);
		UnLock(wbArg[1].wa_Lock);
		}

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
}

//----------------------------------------------------------------------------

void AsyncRenameProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	BOOL Success;
	struct WBArg *wbArg;
	ULONG nArgs;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		ULONG tiResult;
		ULONG n;
		ULONG NumberOfWbArgs = 1;

		d1(KPrintF("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		d1(KPrintF("%s/%s/%ld: NumberOfWbArgs=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfWbArgs));

		wbArg = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		d1(KPrintF("%s/%s/%ld: wbArg=%08lx\n", __FILE__, __FUNC__, __LINE__, wbArg));
		if (NULL == wbArg)
			return;

		wbArg[0].wa_Name = (STRPTR) "rename.module";
		wbArg[0].wa_Lock = LockScaModsDir();
		if ((BPTR)NULL == wbArg[0].wa_Lock)
			break;

		nArgs = 1;	// first argument is program itself

		if (NumberOfWbArgs > 1)
			{
			if (mcArg->mca_IconNode)
				{
				nArgs += DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg,
					mcArg->mca_IconNode, &wbArg[1]);
				}
			else
				{
				nArgs += SCA_MakeWBArgs(&wbArg[1], NULL, NumberOfWbArgs - 1);
				}
			}

		d1(KPrintF("%s/%s/%ld: nArgs=%lu\n", __FILE__, __FUNC__, __LINE__, nArgs));

		DoForAppIcons(AMCLASSICON_Rename);

		if (nArgs < 2)
			break;

		for (n=1, tiResult=0; !tiResult && n<nArgs; n++)
			{
			BPTR oldDir = CurrentDir(wbArg[n].wa_Lock);

			if (strlen(wbArg[n].wa_Name) > 0)
				{
				tiResult = CallTextInputHook(TIACTION_Rename, wbArg[n].wa_Name);
				}
			else
				{
				STRPTR VolumeName = AllocPathBuffer();

				if (VolumeName)
					{
					NameFromLock(wbArg[n].wa_Lock, VolumeName, Max_PathLen);
					StripTrailingColon(VolumeName);

					tiResult = CallTextInputHook(TIACTION_RelabelVolume, VolumeName);
					FreePathBuffer(VolumeName);
					}
				else
					tiResult = 1;
				}

			CurrentDir(oldDir);
			}

		d1(KPrintF("%s/%s/%ld: tiResult=%lu\n", __FILE__, __FUNC__, __LINE__, tiResult));

		if (tiResult)
			{
			// SCA_WBStart()
			Success = SCA_WBStartTags(wbArg, nArgs,
					SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
					SCA_Flags, SCAF_WBStart_Wait,
					SCA_WaitTimeout, 0,
					TAG_END);

			d1(KPrintF("%s/%s/%ld: Success=%lu\n", __FILE__, __FUNC__, __LINE__, Success));

			if (NumberOfWbArgs > 1)
				{
				SCA_FreeWBArgs(&wbArg[1], nArgs - 1,
					Success ? SCAF_FreeNames : SCAF_FreeNames | SCAF_FreeLocks);
				}
			if (Success)
				wbArg[0].wa_Lock = (BPTR)NULL;

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
			}
		else
			{
			// Rename done by TextInputHook
			if (NumberOfWbArgs > 1)
				{
				SCA_FreeWBArgs(&wbArg[1], nArgs - 1, SCAF_FreeNames | SCAF_FreeLocks);
				}
			}

		} while (0);

	d1(KPrintF("%s/%s/%ld: wbArg=%08lx\n", __FILE__, __FUNC__, __LINE__, wbArg));

	if (wbArg)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);

		ScalosFree(wbArg);
		}

	d1(KPrintF("%s/%s/%ld: END", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

void AsyncDeleteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	BOOL Success = FALSE;
	struct WBArg *wbArg;

	do	{
		ULONG nArgs = 1;
		ULONG NumberOfWbArgs = 1;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArg)
			return;

		if (NumberOfWbArgs > 1)
			{
			if (mcArg->mca_IconNode)
				{
				nArgs += DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg,
					mcArg->mca_IconNode, &wbArg[1]
					);
				}
			else
				{
				nArgs += SCA_MakeWBArgs(&wbArg[1], NULL, NumberOfWbArgs - 1);
				}
			}

		DoForAppIcons(AMCLASSICON_Delete);

		if (nArgs < 2)
			break;

		ScalosObtainSemaphoreShared(&DeleteHookSemaphore);

		if (globalDeleteHook)
			{
			Success = CallGlobalDeleteHook(&wbArg[1], nArgs - 1, DLACTION_BeginDiscard);
			}
		else
			{
			wbArg[0].wa_Name = (STRPTR) "delete.module";
			wbArg[0].wa_Lock = LockScaModsDir();
			if ((BPTR)NULL == wbArg[0].wa_Lock)
				break;

			DeleteModuleKludge(wbArg, nArgs);
			}

		ScalosReleaseSemaphore(&DeleteHookSemaphore);

		if (!Success)
			{
			// SCA_WBStart()
			Success = SCA_WBStartTags(wbArg, nArgs,
					SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
					SCA_Flags, SCAF_WBStart_Wait,
					SCA_WaitTimeout, 0,
					TAG_END);

			if (NumberOfWbArgs > 1)
				{
				SCA_FreeWBArgs(&wbArg[1], nArgs - 1,
					Success ? SCAF_FreeNames : SCAF_FreeNames | SCAF_FreeLocks);
				}
			if (Success)
				wbArg[0].wa_Lock = (BPTR)NULL;
			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
		} while (0);

	if (wbArg)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);

		ScalosFree(wbArg);
		}
}

//----------------------------------------------------------------------------

static void LeaveOutIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	ULONG IconType = 0;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: Icon=<%s>  Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, IconName, in->in_Lock));

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (0 == IconType)
		return;

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_LeaveOut, in);
		return;
		}
	else
		{
		BPTR DirLock;

		if (in->in_Lock)
			DirLock = in->in_Lock;
		else
			DirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

		UndoAddEvent(iwt, UNDO_Leaveout,
			UNDOTAG_UndoMultiStep, undoStep,
			UNDOTAG_IconName, GetIconName(in),
			UNDOTAG_IconDirLock, DirLock,
			UNDOTAG_IconPosX, NO_ICON_POSITION_SHORT,
			UNDOTAG_IconPosY, NO_ICON_POSITION_SHORT,
			TAG_END);

		DoLeaveOutIcon(iwt, DirLock, GetIconName(in),
			NO_ICON_POSITION_SHORT, NO_ICON_POSITION_SHORT);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
}


static void PutAwayIconNode(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	ULONG IconType = 0;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: Icon=<%s>  Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), in->in_Lock));

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (0 == IconType)
		return;

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_PutAway, in);
		return;
		}
	else
		{
		STRPTR Path = NULL;
		STRPTR IconName;
		BPTR iconDirLockClone = (BPTR)NULL;

		do	{
			BPTR iconDirLock;
			struct ScaWindowStruct *wsDest;

			if (in->in_Lock)
				iconDirLock = in->in_Lock;
			else
				iconDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			IconName = AllocCopyString(GetIconName(in));
			if (NULL == IconName)
				break;

			debugLock_d1(iconDirLock);

			iconDirLockClone = DupLock(iconDirLock);
			if ((BPTR)NULL == iconDirLockClone)
				break;

			UndoAddEvent(iwt, UNDO_PutAway,
				UNDOTAG_UndoMultiStep, undoStep,
				UNDOTAG_IconName, IconName,
				UNDOTAG_IconDirLock, iconDirLock,
				TAG_END);

			// !!! side effects: UnLocks(iconDirLock) and frees <in> !!!
			PutAwayIcon(iwt, iconDirLock, IconName, TRUE);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

			// add icon to owning drawer window
			SCA_LockWindowList(SCA_LockWindowList_Shared);

			for (wsDest = winlist.wl_WindowStruct; wsDest; wsDest = (struct ScaWindowStruct *) wsDest->ws_Node.mln_Succ)
				{
				debugLock_d1(iconDirLockClone);
				d1(if (wsDest->ws_Lock) debugLock_d1(wsDest->ws_Lock));

				if (wsDest->ws_Lock && LOCK_SAME == ScaSameLock(iconDirLockClone, wsDest->ws_Lock))
					{
					struct internalScaWindowTask *iwtDest = (struct internalScaWindowTask *) wsDest->ws_WindowTask;

					d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

					DoMethod(iwtDest->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_ReadIcon,
						IconName,
						NULL);

					// Special handling for "view all" textwindows.
					// here both object and icon are separate entries, which must be removed both!
					if (!IsIwtViewByIcon(iwtDest) &&
						IsShowAll(iwtDest->iwt_WindowTask.mt_WindowStruct))
						{
						Path = AllocPathBuffer();
						if (NULL == Path)
							break;

						stccpy(Path, IconName, Max_PathLen);
						SafeStrCat(Path, ".info", Max_PathLen);

						DoMethod(iwtDest->iwt_WindowTask.mt_MainObject,
							SCCM_IconWin_ReadIcon,
							Path,
							NULL);
						}
					}
				}

			SCA_UnLockWindowList();

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			} while (0);

		if (IconName)
			FreeCopyString(IconName);
		if (iconDirLockClone)
			UnLock(iconDirLockClone);
		if (Path)
			FreePathBuffer(Path);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static void SnapshotWindow(struct internalScaWindowTask *iwt)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	CONST_STRPTR iconName;
	Object *IconObj, *allocIconObj = NULL;
	BPTR parentLock = (BPTR)NULL;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		struct IBox WinRect;
		ULONG IconViewMode;
		ULONG ddFlags;

		WinRect.Left = ws->ws_Left;
		WinRect.Top = ws->ws_Top;
		WinRect.Width = ws->ws_Width;
		WinRect.Height = ws->ws_Height;

		IconObj = ClassGetWindowIconObject(iwt, &allocIconObj);

		d1(KPrintF("%s/%s/%ld: iconName=<%s>  IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconName, IconObj));
		if (NULL == IconObj)
			break;

		IconViewMode = TranslateViewModesToIcon(ws->ws_Viewmodes);
		ddFlags = ws->ws_ViewAll;
		if (SortOrder_Ascending == ws->ws_SortOrder)
			ddFlags |= DDFLAGS_SORTASC;
		else if (SortOrder_Descending == ws->ws_SortOrder)
			ddFlags |= DDFLAGS_SORTDESC;

		d1(KPrintF("%s/%s/%ld: ws_ViewAll=%04lx  ddFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_ViewAll, ddFlags));

		SetAttrs(IconObj,
			IDTA_ViewModes, IconViewMode,
			IDTA_Flags, ddFlags,
			IDTA_WindowRect, (ULONG) &WinRect,
			IDTA_WinCurrentY, ws->ws_yoffset,
			IDTA_WinCurrentX, ws->ws_xoffset,
			TAG_END);

		GetAttr(DTA_Name, IconObj, (APTR) &iconName);

		d1(kprintf("%s/%s/%ld: iconName=<%s>  IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconName, IconObj));

		if (ws->ws_Flags & WSV_FlagF_RootWindow)
			parentLock = DupLock(ws->ws_Lock);
		else
			parentLock = ParentDir(ws->ws_Lock);
		if ((BPTR)NULL == parentLock)
			break;

		SaveIconObject(IconObj, parentLock, iconName, FALSE, NULL);
		} while (0);

	if (allocIconObj)
		DisposeIconObject(allocIconObj);
	if (parentLock)
		UnLock(parentLock);
}

#if 0
static struct ScaIconNode *FindIconGlobal(BPTR dirLock, CONST_STRPTR Name)
{
	struct ScaWindowStruct *ws;
	struct ScaIconNode *in = NULL;
	STRPTR allocName = NULL;
	BPTR parentLock = NULL;

	d1(kprintf("%s/%s/%ld: dirLock=%08lx  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, dirLock, Name));
	debugLock_d1(dirLock);

	if (strlen(Name) < 1)
		{
		// dirLock is really ObjectLock
		parentLock = ParentDir(dirLock);
		if (NULL == parentLock)
			return NULL;

		allocName = AllocPathBuffer();
		if (NULL == allocName)
			{
			UnLock(parentLock);
			return NULL;
			}

		NameFromLock(dirLock, allocName, Max_PathLen - 1);
		Name = FilePart(allocName);
		dirLock = parentLock;

		d1(kprintf("%s/%s/%ld: allocName=<%s>\n", __FILE__, __FUNC__, __LINE__, allocName));

		d1(kprintf("%s/%s/%ld: dirLock=%08lx  Name=<%s>\n", __FILE__, __FUNC__, __LINE__, dirLock, Name));
		debugLock_d1(dirLock);
		}

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; NULL == in && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (LOCK_SAME == ScaSameLock(ws->ws_Lock, dirLock))
			{
			struct ScaIconNode *inx;
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

			d1(kprintf("%s/%s/%ld: Window FOUND iwt=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			ScalosLockIconListShared(iwt);

			for (inx=iwt->iwt_WindowTask.wt_IconList; NULL == in && inx; inx = (struct ScaIconNode *) inx->in_Node.mln_Succ)
				{
				if (0 == Stricmp(inx->in_Name, Name))
					in = inx;
				}

			ScalosUnLockIconList(iwt);
			}
		}

	if (NULL == in)
		{
		// no icon found.
		// now look in main window for left-out (=backdrop) icons.

		struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_MainWindowStruct->ws_WindowTask;
		struct ScaIconNode *inx;

		ScalosLockIconListShared(iwt);

		for (inx=iwt->iwt_WindowTask.wt_IconList; NULL == in && inx; inx = (struct ScaIconNode *) inx->in_Node.mln_Succ)
			{
			if (inx->in_Lock && 
				LOCK_SAME == ScaSameLock(inx->in_Lock, dirLock) &&
				0 == Stricmp(inx->in_Name, Name))
				{
				in = inx;
				}
			}

		ScalosUnLockIconList(iwt);
		}

	SCA_UnLockWindowList();

	d1(kprintf("%s/%s/%ld: in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

	if (allocName)
		FreePathBuffer(allocName);
	if (parentLock)
		UnLock(parentLock);

	return in;
}
#endif

static void BackDropProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	UBYTE newState;

	newState = mcArg->mca_State != 0;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: State=%ld\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_State));
	d1(kprintf("%s/%s/%ld: CommandName=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_CommandName));

	d1(kprintf("%s/%s/%ld: iwt_BackDrop=%ld  newState=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_BackDrop, newState));

	if (newState != iwt->iwt_BackDrop)
		{
		struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
		struct PatternNode *ptNode;

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Sleep);

		iwt->iwt_BackDrop = newState;

		if (iwt->iwt_BackDrop)
			{
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_Backdrop;
			iwt->iwt_WindowTask.wt_XOffset = iwt->iwt_WindowTask.mt_WindowStruct->ws_xoffset = 0;
			iwt->iwt_WindowTask.wt_YOffset = iwt->iwt_WindowTask.mt_WindowStruct->ws_yoffset = 0;
			}
		else
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_Backdrop;

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_WakeUp, 1);

		ptNode = ws->ws_PatternNode;

		d1(kprintf("%s/%s/%ld: type=%ld\n", __FILE__, __FUNC__, __LINE__, ptNode->ptn_type));

		if (PatternPrefs.patt_NewRenderFlag && ptNode &&
			((SCP_RenderType_FitSize == ptNode->ptn_type)
			|| (SCP_RenderType_ScaledMin == ptNode->ptn_type)
                        || (SCP_RenderType_Centered == ptNode->ptn_type))
			&& GuiGFXBase && iwt->iwt_WindowTask.wt_PatternInfo.ptinf_bitmap)
			{
			d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			NewWindowPatternMsg(iwt, NULL, &ptNode);
			}

		SCA_UnLockWindowList();
		}
}


void UpdateAllProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	(void) mcArg;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		d1(KPrintF("%s/%s/%ld:  Name=<%s>  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_Name, ws->ws_Flags));

		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			struct ScalosMessage *msg;

			msg = SCA_AllocMessage(MTYP_Update, 0);

			if (msg)
				PutMsg(ws->ws_MessagePort, &msg->sm_Message);
			}
		}

	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

}


static void RedrawAllProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps))
			{
			struct ScalosMessage *msg;

			msg = SCA_AllocMessage(MTYP_Redraw, 0);

			if (msg)
				PutMsg(ws->ws_MessagePort, &msg->sm_Message);
			}
		}

	SCA_UnLockWindowList();
}


void GetIconsBoundingBox(struct internalScaWindowTask *iwt, struct Rectangle *BBox)
{
	struct ScaIconNode *in;

	BBox->MinX = BBox->MinY = SHRT_MAX;
	BBox->MaxX = BBox->MaxY = SHRT_MIN;

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (BBox->MinX > gg->BoundsLeftEdge)
			BBox->MinX = gg->BoundsLeftEdge;
		if (BBox->MinY > gg->BoundsTopEdge)
			BBox->MinY = gg->BoundsTopEdge;

		if (BBox->MaxX < gg->BoundsLeftEdge + gg->BoundsWidth - 1)
			BBox->MaxX = gg->BoundsLeftEdge + gg->BoundsWidth - 1;
		if (BBox->MaxY < gg->BoundsTopEdge + gg->BoundsHeight - 1)
			BBox->MaxY = gg->BoundsTopEdge + gg->BoundsHeight - 1;
		}

	BBox->MinX -= CurrentPrefs.pref_CleanupSpace.Left;
	BBox->MaxX += CurrentPrefs.pref_CleanupSpace.Left;
	BBox->MinY -= CurrentPrefs.pref_CleanupSpace.Top;
	BBox->MaxY += CurrentPrefs.pref_CleanupSpace.Top;

	ScalosUnLockIconList(iwt);
}


static void ClearSelectionInWindow(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *in;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, FALSE);
		}

	ScalosUnLockIconList(iwt);
}


static void ViewByIconProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ViewWindowBy(iwt, IDTV_ViewModes_Icon);
}


static void ViewByTextProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ViewWindowBy(iwt, IDTV_ViewModes_Name);
}


static void ViewByDateProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ViewWindowBy(iwt, IDTV_ViewModes_Date);
}


static void ViewBySizeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ViewWindowBy(iwt, IDTV_ViewModes_Size);
}


static void ViewByTypeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ViewWindowBy(iwt, IDTV_ViewModes_Type);
}

static void ViewByDefaultProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ViewWindowBy(iwt, IDTV_ViewModes_Default);
}

BOOL ViewWindowBy(struct internalScaWindowTask *iwt, UBYTE NewViewByType)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	Object *newObject;
	UBYTE OldViewByType;
	BOOL Updated = FALSE;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: ws_Viewmodes=%ld  NewViewByType=%ld\n", \
		__FILE__, __FUNC__, __LINE__, ws->ws_Viewmodes, NewViewByType));

	if (ws->ws_Viewmodes == NewViewByType)
		return Updated;

	FlushThumbnailEntries(iwt);

	OldViewByType = ws->ws_Viewmodes;

	newObject = (Object *) DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_NewViewMode, NewViewByType);
	d1(KPrintF("%s/%s/%ld: newObject=%08lx\n", __FILE__, __FUNC__, __LINE__, newObject));
	if (newObject)
		{
		Object *oldObject;
		ws->ws_Viewmodes = OldViewByType;

		if (iwt->iwt_WindowTask.wt_Window)
			{
			struct Region *origClipRegion = iwt->iwt_RemRegion;

			d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));
			iwt->iwt_RemRegion = NULL;

			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, origClipRegion);
			}

		oldObject = iwt->iwt_WindowTask.mt_MainObject;
		iwt->iwt_WindowTask.mt_MainObject = newObject;

		ws->ws_Viewmodes = NewViewByType;

		DisposeObject(oldObject);
		}

	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetInnerSize);

	iwt->iwt_WindowTask.wt_XOffset = 0;
	iwt->iwt_WindowTask.wt_YOffset = 0;

	ws->ws_xoffset = iwt->iwt_WindowTask.wt_XOffset;
	ws->ws_yoffset = iwt->iwt_WindowTask.wt_YOffset;

	d1(KPrintF("%s/%s/%ld: newObject=%08lx\n", __FILE__, __FUNC__, __LINE__, newObject));
	if (newObject)
		{
		// switch between icon/text window
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);
		Updated = TRUE;
		}
	else
		{
		// all text window, different sorting
		if (!IsIwtViewByIcon(iwt))
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_TextWin_DrawColumnHeaders);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
		}
	d1(KPrintF("%s/%s/%ld: END  Updated=%ld\n", __FILE__, __FUNC__, __LINE__, Updated));

	return Updated;
}


static void CloneProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct WBArg *wbArg, *wbArgAlloc;
	ULONG ArgCount = 0;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		struct CloneArg processArgs;
		ULONG NumberOfWbArgs = 1;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = wbArgAlloc = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArgAlloc)
			return;

		if (mcArg->mca_IconNode)
			{
			ULONG IconType;

			GetAttr(IDTA_Type, mcArg->mca_IconNode->in_Icon, &IconType);

			if (WB_TEXTICON_TOOL != IconType &&
				WB_TEXTICON_DRAWER != IconType &&
				WBDRAWER != IconType &&
				WBTOOL != IconType &&
				WBPROJECT != IconType)
				{
				break;
				}

			if (!CloneWBArg(iwt, &wbArg, mcArg->mca_IconNode, &ArgCount))
				break;

			ArgCount++;

			d1(kprintf("%s/%s/%ld: NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));
			}
		else
			{
			MakeCloneArgs(iwt, &wbArg, &ArgCount);
			}

		d1(KPrintF("%s/%s/%ld: NumArgs=%ld  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, ArgCount, iwt));

		DoForAppIcons(AMCLASSICON_Copy);

		if (ArgCount < 1)
			break;

		processArgs.cla_wbArg = wbArgAlloc;
		processArgs.cla_NumArgs = ArgCount;

		RunProcess(&iwt->iwt_WindowTask, CloneFilesStart, 
			sizeof(processArgs)/sizeof(ULONG), (struct WBArg *)(APTR) &processArgs, NULL);

		// wbArgAlloc will be freed inside <CloneFilesStart>
		wbArgAlloc = NULL;
		ArgCount = 0;
		} while (0);

	if (wbArgAlloc)
		{
		if (ArgCount > 0)
			SCA_FreeWBArgs(wbArgAlloc, ArgCount, SCAF_FreeNames | SCAF_FreeLocks);

		ScalosFree(wbArgAlloc);
		}
}


static void MakeCloneArgs(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	CloneInfoStart(iwt, wbArg, ArgCount);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws != iwt->iwt_WindowTask.mt_WindowStruct)
			CloneInfoStart((struct internalScaWindowTask *) ws->ws_WindowTask, wbArg, ArgCount);
		}

	SCA_UnLockWindowList();
}


static void CloneInfoStart(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount)
{
	struct ScaIconNode *in;

	ScalosLockIconListShared(iwt);

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			{
			ULONG IconType;

			GetAttr(IDTA_Type, in->in_Icon, &IconType);

			switch (IconType)
				{
			case WB_TEXTICON_TOOL:
			case WB_TEXTICON_DRAWER:
			case WBDRAWER:
			case WBTOOL:
			case WBPROJECT:
				d1(kprintf("%s/%s/%ld: icon=<%s> NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), *ArgCount));
				CloneWBArg(iwt, wbArg, in, ArgCount);
				break;
			default:
				break;
				}
			}
		}

	ScalosUnLockIconList(iwt);
}


static BOOL CloneWBArg(struct internalScaWindowTask *iwt, struct WBArg **wbArg, 
	struct ScaIconNode *in, ULONG *ArgCount)
{
	if (NULL == in->in_Name)
		return FALSE;

	if (in->in_Lock)
		{
		(*wbArg)->wa_Lock = DupLock(in->in_Lock);
		}
	else
		{
		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
			(*wbArg)->wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		else
			(*wbArg)->wa_Lock = (BPTR)NULL;

		}

	(*wbArg)->wa_Name = AllocCopyString(GetIconName(in));

	if (NULL == (*wbArg)->wa_Name)
		{
		if ((*wbArg)->wa_Lock)
			UnLock((*wbArg)->wa_Lock);

		return FALSE;
		}

	(*wbArg)++;
	(*ArgCount)++;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static SAVEDS(ULONG) CloneFilesStart(APTR aptr, struct SM_RunProcess *msg)
{
	struct CloneArg *arg = aptr;
	STRPTR NewName;
	STRPTR OldIconName = NULL;
	APTR undoStep;
	Object *fileTransObj = NULL;
	ULONG n;

	do	{
		undoStep = UndoBeginStep();

		NewName = AllocPathBuffer();
		if (NULL == NewName)
			break;

		OldIconName = AllocPathBuffer();
		if (NULL == OldIconName)
			break;

		fileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca", 
			SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
			SCCA_FileTrans_Number, arg->cla_NumArgs,
			SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
			TAG_END);
		if (NULL == fileTransObj)
			break;

		d1(kprintf("%s/%s/%ld: NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, arg->cla_NumArgs));

		for (n=0; n < arg->cla_NumArgs; n++)
			{
			BumpRevision(NewName, arg->cla_wbArg[n].wa_Name);

			debugLock_d1(arg->cla_wbArg[n].wa_Lock);
			d1(kprintf("%s/%s/%ld: wa_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, arg->cla_wbArg[n].wa_Name));

			d1(kprintf("%s/%s/%ld: OldName=<%s>  NewName=<%s>\n", __FILE__, __FUNC__, __LINE__, arg->cla_wbArg[n].wa_Name, NewName));

			UndoAddEvent((struct internalScaWindowTask *) msg->WindowTask, UNDO_Copy,
				UNDOTAG_UndoMultiStep, undoStep,
				UNDOTAG_CopySrcDirLock, arg->cla_wbArg[n].wa_Lock,
				UNDOTAG_CopyDestDirLock, arg->cla_wbArg[n].wa_Lock,
				UNDOTAG_CopySrcName, arg->cla_wbArg[n].wa_Name,
				UNDOTAG_CopyDestName, NewName,
				TAG_END);

			// clone file/drawer
			DoMethod(fileTransObj, SCCM_FileTrans_Copy,
				arg->cla_wbArg[n].wa_Lock, arg->cla_wbArg[n].wa_Lock, 
				arg->cla_wbArg[n].wa_Name, NewName);
			}
		} while (0);

	SCA_FreeWBArgs(arg->cla_wbArg, arg->cla_NumArgs, SCAF_FreeNames | SCAF_FreeLocks);
	ScalosFree(arg->cla_wbArg);

	if (undoStep)
		UndoEndStep((struct internalScaWindowTask *) msg->WindowTask, undoStep);
	if (fileTransObj)
		SCA_DisposeScalosObject(fileTransObj);
	if (OldIconName)
		FreePathBuffer(OldIconName);
	if (NewName)
		FreePathBuffer(NewName);

	return 0;
}

//---------------------------------------------------------------------------------------

static void ThumbnailCacheCleanupProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ThumbnailsStartCleanup(TRUE);
}

//---------------------------------------------------------------------------------------

static void UpdateProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);
}

//---------------------------------------------------------------------------------------

static void CloseProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	iwt->iwt_CloseWindow = TRUE;
}


//---------------------------------------------------------------------------------------

static void FindProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct WBArg *wbArg;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		ULONG NumberOfWbArgs = 1;
		ULONG nArgs = 0;
		BOOL Success;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArg)
			return;

		wbArg[0].wa_Name = (STRPTR) "Find.module";
		wbArg[0].wa_Lock = LockScaModsDir();
		if ((BPTR)NULL == wbArg[0].wa_Lock)
			break;

		nArgs++;
		d1(KPrintF("%s/%s/%ld: nArgs=%ld\n", __FILE__, __FUNC__, __LINE__, nArgs));

		if (NumberOfWbArgs > 1)
			{
			if (mcArg->mca_IconNode)
				{
				d1(KPrintF("%s/%s/%ld: NumberOfWbArgs=%ld\n", __FILE__, __FUNC__, __LINE__, NumberOfWbArgs));
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg,
					mcArg->mca_IconNode, &wbArg[1]);
				nArgs++;
				}
			else
				{
				d1(KPrintF("%s/%s/%ld: NumberOfWbArgs=%ld\n", __FILE__, __FUNC__, __LINE__, NumberOfWbArgs));
				nArgs += SCA_MakeWBArgs(&wbArg[1], NULL, NumberOfWbArgs - 1);
				}
			}

		d1(KPrintF("%s/%s/%ld: nArgs=%ld\n", __FILE__, __FUNC__, __LINE__, nArgs));

		// SCA_WBStart()
		Success = SCA_WBStartTags(wbArg, nArgs,
				SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
//				  SCA_Flags, SCAF_WBStart_Wait,
//				  SCA_WaitTimeout, 0,
				TAG_END);

		d1(KPrintF("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

		if (NumberOfWbArgs > 1)
			{
			SCA_FreeWBArgs(&wbArg[1], nArgs - 1,
				Success ? SCAF_FreeNames : SCAF_FreeNames | SCAF_FreeLocks);
			}
		if (Success)
			wbArg[0].wa_Lock = (BPTR)NULL;
		} while (0);

	d1(KPrintF("%s/%s/%ld: wbArg=%08lx\n", __FILE__, __FUNC__, __LINE__, wbArg));

	if (wbArg)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);

		ScalosFree(wbArg);
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  iwt_WinTitle=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}

//---------------------------------------------------------------------------------------

static void FormatDiskProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct WBArg *wbArg;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		ULONG NumberOfWbArgs = 1;
		ULONG nArgs = 0;
		BOOL Success;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArg)
			return;

		wbArg[0].wa_Name = (STRPTR) "Format_Disk.module";
		wbArg[0].wa_Lock = LockScaModsDir();
		if ((BPTR)NULL == wbArg[0].wa_Lock)
			break;

		nArgs++;

		if (NumberOfWbArgs > 1)
			{
			if (mcArg->mca_IconNode)
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg,
					mcArg->mca_IconNode, &wbArg[1]
					);
				nArgs++;
				}
			else
				{
				nArgs += SCA_MakeWBArgs(&wbArg[1], NULL, NumberOfWbArgs - 1);
				}
			}

		DoForAppIcons(AMCLASSICON_FormatDisk);

		if (nArgs < 2)
			break;

		// SCA_WBStart()
		Success = SCA_WBStartTags(wbArg, nArgs,
				SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
				SCA_Flags, SCAF_WBStart_Wait,
				SCA_WaitTimeout, 0,
				TAG_END);

		if (NumberOfWbArgs > 1)
			{
			SCA_FreeWBArgs(&wbArg[1], nArgs - 1,
				Success ? SCAF_FreeNames : SCAF_FreeNames | SCAF_FreeLocks);
			}
		if (Success)
			wbArg[0].wa_Lock = (BPTR)NULL;
		} while (0);

	d1(KPrintF("%s/%s/%ld: START wbArg=%08lx\n", __FILE__, __FUNC__, __LINE__, wbArg));

	if (wbArg)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);

		ScalosFree(wbArg);
		}

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx  iwt_WinTitle=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
}


static void ShutdownProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	BOOL Success;
	struct WBArg wbArg[2];

	do	{
		wbArg[0].wa_Name = (STRPTR) "Shutdown.module";
		wbArg[0].wa_Lock = LockScaModsDir();
		if ((BPTR)NULL == wbArg[0].wa_Lock)
			break;

		// SCA_WBStart()
		Success = SCA_WBStartTags(wbArg, 1,
				SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
				SCA_Flags, SCAF_WBStart_Wait,
				SCA_WaitTimeout, 0,
				TAG_END);

		if (!Success)
			UnLock(wbArg[0].wa_Lock);
		} while (0);
}

//---------------------------------------------------------------------------------------

static void ShowOnlyIconsProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
		SCCA_IconWin_ShowType, DDFLAGS_SHOWICONS,
		TAG_END);
}

//---------------------------------------------------------------------------------------

static void ShowAllFilesProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
		SCCA_IconWin_ShowType, DDFLAGS_SHOWALL,
		TAG_END);
}

//---------------------------------------------------------------------------------------

static void ShowDefaultProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
		SCCA_IconWin_ShowType, DDFLAGS_SHOWDEFAULT,
		TAG_END);
}

//---------------------------------------------------------------------------------------

static void CleanupProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct Hook SortHook;
	struct ScaIconNode *in, *inNext;

	if (!IsIwtViewByIcon(iwt))
		return;

	ScalosLockIconListExclusive(iwt);

	UndoAddEvent(iwt, UNDO_Cleanup,
		UNDOTAG_IconList, iwt->iwt_WindowTask.wt_IconList,
		UNDOTAG_WindowTask, iwt,
		UNDOTAG_CleanupMode, CLEANUP_Default,
		TAG_END);

	for (in=iwt->iwt_WindowTask.wt_LateIconList; in; in=inNext)
		{
		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_LateIconList, &iwt->iwt_WindowTask.wt_IconList, in);
		}

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		in->in_Flags &= ~INF_IconVisible;

		in->in_OldLeftEdge = NO_ICON_POSITION_SHORT;
		in->in_OldTopEdge = NO_ICON_POSITION_SHORT;
		}

	iwt->iwt_WindowTask.wt_LateIconList = iwt->iwt_WindowTask.wt_IconList;
	iwt->iwt_WindowTask.wt_IconList = NULL;

	SETHOOKFUNC(SortHook, IconCompareFunc);
	SortHook.h_Data = NULL;

	SCA_SortNodes((struct ScalosNodeList *) &iwt->iwt_WindowTask.wt_LateIconList, &SortHook, SCA_SortType_Best);

	ScalosUnLockIconList(iwt);

	iwt->iwt_WindowTask.wt_XOffset = iwt->iwt_WindowTask.wt_YOffset = 0;
	iwt->iwt_WindowTask.mt_WindowStruct->ws_xoffset = iwt->iwt_WindowTask.mt_WindowStruct->ws_yoffset = 0;

	if (iwt->iwt_WindowTask.wt_Window)
		{
		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort, 0, 0,
			iwt->iwt_WindowTask.wt_Window->Width - 1,
			iwt->iwt_WindowTask.wt_Window->Height -1);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
		}
	else
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
		}
}

//---------------------------------------------------------------------------------------

static void CleanupByNameProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	UndoAddEvent(iwt, UNDO_Cleanup,
		UNDOTAG_IconList, iwt->iwt_WindowTask.wt_IconList,
		UNDOTAG_WindowTask, iwt,
		UNDOTAG_CleanupMode, CLEANUP_ByName,
		TAG_END);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpByName);
}

//---------------------------------------------------------------------------------------

static void CleanupByDateProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	UndoAddEvent(iwt, UNDO_Cleanup,
		UNDOTAG_IconList, iwt->iwt_WindowTask.wt_IconList,
		UNDOTAG_WindowTask, iwt,
		UNDOTAG_CleanupMode, CLEANUP_ByDate,
		TAG_END);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpByDate);
}

//---------------------------------------------------------------------------------------

static void CleanupBySizeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	UndoAddEvent(iwt, UNDO_Cleanup,
		UNDOTAG_IconList, iwt->iwt_WindowTask.wt_IconList,
		UNDOTAG_WindowTask, iwt,
		UNDOTAG_CleanupMode, CLEANUP_BySize,
		TAG_END);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpBySize);
}

//---------------------------------------------------------------------------------------

static void CleanupByTypeProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	UndoAddEvent(iwt, UNDO_Cleanup,
		UNDOTAG_IconList, iwt->iwt_WindowTask.wt_IconList,
		UNDOTAG_WindowTask, iwt,
		UNDOTAG_CleanupMode, CLEANUP_ByType,
		TAG_END);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUpByType);
}

//---------------------------------------------------------------------------------------

static SAVEDS(LONG) IconCompareFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1)
{
	struct ExtGadget *gg1 = (struct ExtGadget *) in1->in_Icon;
	struct ExtGadget *gg2 = (struct ExtGadget *) in2->in_Icon;
	WORD xDiff, yDiff;

	(void) hook;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	xDiff = gg1->LeftEdge - gg2->LeftEdge;

	if (xDiff > 16)
		return -1;
	else if (xDiff < -16)
		return 1;

	yDiff = gg1->TopEdge - gg2->TopEdge;
	if (yDiff < 0)
		return 1;
	else if (yDiff > 0)
		return -1;

	return 0;
}

//---------------------------------------------------------------------------------------

static void MenuOpenProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	ULONG IconType = 0;

	if (mcArg->mca_IconNode)
		GetAttr(IDTA_Type, mcArg->mca_IconNode->in_Icon, &IconType);

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_LeaveOut, mcArg->mca_IconNode);
		return;
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_Open,
			mcArg->mca_IconNode,
			ICONWINOPENF_IgnoreFileTypes);
		}
}

//---------------------------------------------------------------------------------------

static void MenuOpenNewProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_Open,
		mcArg->mca_IconNode,
		ICONWINOPENF_IgnoreFileTypes | ICONWINOPENF_NewWindow);
}

//---------------------------------------------------------------------------------------

static void MenuOpenBrowserWindowProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject,
		SCCM_IconWin_Open,
		mcArg->mca_IconNode,
		ICONWINOPENF_IgnoreFileTypes | ICONWINOPENF_BrowserWindow);
}

//---------------------------------------------------------------------------------------

static void OpenParentProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	BPTR parentLock = (BPTR)NULL;
	STRPTR ParentName = NULL;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	do	{
		if ((BPTR)NULL == ws->ws_Lock)
			break;

		parentLock = ParentDir(ws->ws_Lock);
		if ((BPTR)NULL == parentLock)
			break;

		ParentName = AllocPathBuffer();
		if (NULL == ParentName)
			break;

		if (!NameFromLock(parentLock, ParentName, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (ws->ws_Flags & WSV_FlagF_BrowserMode)
			{
			WindowNewPath(iwt, ParentName);
			}
		else
			{
			SCA_OpenIconWindowTags(SCA_Flags, SCAF_OpenWindow_ScalosPort,
				SCA_Path, (ULONG) ParentName,
				SCA_CheckOverlappingIcons, CurrentPrefs.pref_CheckOverlappingIcons,
				TAG_END);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	if (ParentName)
		FreePathBuffer(ParentName);
	if (parentLock)
		UnLock(parentLock);
}

//---------------------------------------------------------------------------------------

static void ExecuteCmdProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	CONST_STRPTR ProgramName;
	ULONG Result;

	if (mcArg->mca_IconNode)
		ProgramName = GetIconName(mcArg->mca_IconNode);
	else
		ProgramName = "";

	Result = CallTextInputHook(TIACTION_Execute, (STRPTR) ProgramName);

	if (Result)
		{
		struct WBArg Args[2];

		Args[0].wa_Lock = LockScaModsDir();

		if (Args[0].wa_Lock)
			{
			Args[0].wa_Name = (STRPTR) "Execute_Command.module";
			Args[1].wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
			Args[1].wa_Name = (STRPTR) ProgramName;

			// SCA_WBStart()
			SCA_WBStartTags(Args, 2,
				SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
				TAG_END);
			}
		}
}

//---------------------------------------------------------------------------------------

static void LastMessageProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	SCA_ScreenTitleMsg((CONST_STRPTR) ~0, NULL);
}

//---------------------------------------------------------------------------------------

static void IconifyProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Iconify);
}

//---------------------------------------------------------------------------------------

static void RedrawProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, REDRAWF_DontRefreshWindowFrame);
}

//---------------------------------------------------------------------------------------

static void ResetScalosProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct MenuCmdArg DummyArg;

	StartProgTask(iwt, &DummyArg, AsyncResetScalosProg, (struct MsgPort *) SCCV_RunProcess_NoReply);
}

//---------------------------------------------------------------------------------------

static void AsyncResetScalosProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *dummy)
{
	struct ScaWindowStruct *WinList = NULL;
	struct ScaWindowStruct *ws, *wsNext;
	struct MsgPort *ReplyPort;
	ULONG iLock;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	(void) dummy;

	ReplyPort = CreateMsgPort();

	d1(kprintf("%s/%s/%ld: ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, ReplyPort));
	if (ReplyPort)
		{
		struct SM_AppSleep *smas = (struct SM_AppSleep *) SCA_AllocMessage(MTYP_AppSleep, 0);
		if (smas)
			{
			smas->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort;

			d1(kprintf("%s/%s/%ld: Before PutMsg\n", __FILE__, __FUNC__, __LINE__));
			PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smas->ScalosMessage.sm_Message);
			d1(kprintf("%s/%s/%ld: After PutMsg\n", __FILE__, __FUNC__, __LINE__));

			WaitReply(ReplyPort, iwt, MTYP_AppSleep);
			d1(kprintf("%s/%s/%ld: After WaitReply\n", __FILE__, __FUNC__, __LINE__));
			}
		}

	SCA_LockWindowList(SCA_LockWindowList_Exclusiv);
	d1(kprintf("%s/%s/%ld: Window List Loecked\n", __FILE__, __FUNC__, __LINE__));

	iLock = ScaLockIBase(0);
	d1(kprintf("%s/%s/%ld: iLock=%08lx\n", __FILE__, __FUNC__, __LINE__, iLock));

	for (ws=winlist.wl_WindowStruct; ws; ws=wsNext)
		{
		wsNext = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ;

		if (ws->ws_Window)
			{
			BOOL Found = FALSE;
			struct Window *win;

			for (win=iwt->iwt_WinScreen->FirstWindow; win; win=win->NextWindow)
				{
				if (win == ws->ws_Window)
					{
					Found = TRUE;
					break;
					}
				}

			if (!Found)
				{
				SCA_MoveWSNode(&winlist.wl_WindowStruct, &WinList, ws);
				}
			}
		}

	ScaUnlockIBase(iLock);
	d1(kprintf("%s/%s/%ld: After UnlockIBase\n", __FILE__, __FUNC__, __LINE__));

	for (ws=WinList; ws; ws=wsNext)
		{
		wsNext = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ;

		d1(kprintf("%s/%s/%ld: ws=%08lx\n", __FILE__, __FUNC__, __LINE__, ws));

		FreeBackFill(&((struct internalScaWindowTask *) ws->ws_WindowTask)->iwt_WindowTask.wt_PatternInfo);
		ScalosFree(ws->ws_WindowTask);

		SCA_FreeNode((struct ScalosNodeList *)(APTR) &WinList, &ws->ws_Node);
		}

	SCA_UnLockWindowList();
	d1(kprintf("%s/%s/%ld: Window List Unlocked\n", __FILE__, __FUNC__, __LINE__));

	if (ReplyPort)
		{
		struct SM_AppWakeup *smaw = (struct SM_AppWakeup *) SCA_AllocMessage(MTYP_AppWakeup, 0);
		if (smaw)
			{
			smaw->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort;
			smaw->smaw_ReLayout = FALSE;

			d1(kprintf("%s/%s/%ld: Before PutMsg\n", __FILE__, __FUNC__, __LINE__));
			PutMsg(iInfos.xii_iinfos.ii_MainMsgPort, &smaw->ScalosMessage.sm_Message);
			d1(kprintf("%s/%s/%ld: After PutMsg\n", __FILE__, __FUNC__, __LINE__));

			WaitReply(ReplyPort, iwt, MTYP_AppWakeup);
			d1(kprintf("%s/%s/%ld: After WaitReply\n", __FILE__, __FUNC__, __LINE__));
			}

		DeleteMsgPort(ReplyPort);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//---------------------------------------------------------------------------------------

static void MenuSnapshotProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		UndoAddEvent(iwt, UNDO_Snapshot,
			UNDOTAG_IconNode, mcArg->mca_IconNode,
			UNDOTAG_IconDirLock, mcArg->mca_IconNode->in_Lock ?
				mcArg->mca_IconNode->in_Lock :
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
			UNDOTAG_SaveIcon, TRUE,
			TAG_END);

		SCA_LockWindowList(SCA_LockWindowList_Shared);
		SnapshotIcon(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindowsShared(iwt, SnapshotIcon, NULL);
		}
}

//---------------------------------------------------------------------------------------

static void MenuUnsnapshotProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		UndoAddEvent(iwt, UNDO_Unsnapshot,
			UNDOTAG_IconNode, mcArg->mca_IconNode,
			UNDOTAG_IconDirLock, mcArg->mca_IconNode->in_Lock ?
				mcArg->mca_IconNode->in_Lock :
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
			UNDOTAG_SaveIcon, TRUE,
			TAG_END);

		SCA_LockWindowList(SCA_LockWindowList_Shared);
		UnsnapshotIcon(iwt, mcArg->mca_IconNode, TRUE, NULL);
		AfterUnsnapshotIcon(iwt);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindowsShared(iwt, UnsnapshotIconAndSave, AfterUnsnapshotIcon);
		}
}

//---------------------------------------------------------------------------------------

static void MenuUndoProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	Undo(iwt);
}

//---------------------------------------------------------------------------------------

static void MenuRedoProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	Redo(iwt);
}

//---------------------------------------------------------------------------------------

static void SnapshotIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	ULONG IconType = 0;

	d1(KPrintF("%s/%s/%ld:  START iwt=%08lx  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (0 == IconType)
		return;

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_Snapshot, in);
		return;
		}
	else
		{
		BPTR destDirLock;

		SetIconWindowRect(in);

		if (in->in_DeviceIcon)
			{
			destDirLock = DiskInfoLock(in);

			if (destDirLock)
				{
				UndoAddEvent(iwt, UNDO_Snapshot,
					UNDOTAG_UndoMultiStep, undoStep,
					UNDOTAG_IconNode, in,
					UNDOTAG_IconDirLock, destDirLock,
					UNDOTAG_SaveIcon, TRUE,
					TAG_END);

				ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

				UnLock(destDirLock);
				}
			}
		else
			{
			if (in->in_Lock)
				destDirLock = in->in_Lock;
			else
				destDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			UndoAddEvent(iwt, UNDO_Snapshot,
				UNDOTAG_UndoMultiStep, undoStep,
				UNDOTAG_IconNode, in,
				UNDOTAG_IconDirLock, destDirLock,
				UNDOTAG_SaveIcon, TRUE,
				TAG_END);

			ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);
			}

		in->in_Flags &= ~INF_FreeIconPosition;
		in->in_SupportFlags |= INF_SupportsUnSnapshot;
		}

	d1(KPrintF("%s/%s/%ld: END in=%08lx  <%s>  inFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_Flags));
}

//---------------------------------------------------------------------------------------

static void UnsnapshotIconAndSave(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	UnsnapshotIcon(iwt, in, TRUE, undoStep);
}

//---------------------------------------------------------------------------------------

static void UnsnapshotIcon(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, BOOL SaveIcon, APTR undoStep)
{
	ULONG IconType = 0;

	d1(KPrintF("%s/%s/%ld:  iwt=%08lx  in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	GetAttr(IDTA_Type, in->in_Icon, &IconType);

	if (0 == IconType)
		return;

	if (WBAPPICON == IconType)
		{
		WindowSendAppIconMsg(iwt, AMCLASSICON_UnSnapshot, in);
		return;
		}
	else
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
		BPTR destDirLock;
		WORD LeftEdge, TopEdge;

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		SetIconWindowRect(in);

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		LeftEdge = gg->LeftEdge;
		TopEdge = gg->TopEdge;

		if (in->in_DeviceIcon)
			{
			destDirLock = DiskInfoLock(in);

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			if (destDirLock)
				{
				UndoAddEvent(iwt, UNDO_Unsnapshot,
					UNDOTAG_UndoMultiStep, undoStep,
					UNDOTAG_IconNode, in,
					UNDOTAG_IconDirLock, destDirLock,
					UNDOTAG_SaveIcon, SaveIcon,
					TAG_END);

				gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_SHORT;

				if (SaveIcon)
					ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

				UnLock(destDirLock);
				}
			}
		else
			{
			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			if (in->in_Lock)
				destDirLock = in->in_Lock;
			else
				destDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

			UndoAddEvent(iwt, UNDO_Unsnapshot,
				UNDOTAG_UndoMultiStep, undoStep,
				UNDOTAG_IconNode, in,
				UNDOTAG_IconDirLock, destDirLock,
				UNDOTAG_SaveIcon, SaveIcon,
				TAG_END);

			gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_SHORT;

			if (SaveIcon)
				ScalosPutIcon(in, destDirLock, in->in_Flags & INF_DefaultIcon);

			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
			}

		gg->LeftEdge = LeftEdge;
		gg->TopEdge = TopEdge;

		in->in_Flags |= INF_FreeIconPosition;
		in->in_SupportFlags |= INF_SupportsSnapshot;
		in->in_SupportFlags &= ~INF_SupportsUnSnapshot;
		}

	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>  inFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_Flags));
}

//---------------------------------------------------------------------------------------

static void AfterUnsnapshotIcon(struct internalScaWindowTask *iwt)
{
	d1(KPrintF("%s/%s/%ld:START\n", __FILE__, __FUNC__, __LINE__));

	if (IsIwtViewByIcon(iwt))
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_UnCleanUp);
		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
		d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
		}

	d1(KPrintF("%s/%s/%ld:END\n", __FILE__, __FUNC__, __LINE__));
}

//---------------------------------------------------------------------------------------

void SetIconWindowRect(struct ScaIconNode *in)
{
	if (in->in_Lock)
		{
		BPTR oldDir = CurrentDir(in->in_Lock);

		SetRect(in);

		CurrentDir(oldDir);
		}
	else
		{
		ULONG IconType = 0;

		GetAttr(IDTA_Type, in->in_Icon, &IconType);

		if (WBDISK == IconType)
			{
			SetRect2(in, DiskInfoLock(in));
			}
		else
			SetRect(in);
		}
}

//---------------------------------------------------------------------------------------

static void SetRect(struct ScaIconNode *in)
{
	if (in->in_Name)
		{
		BPTR iconLock = Lock((STRPTR) GetIconName(in), ACCESS_READ);

		if (iconLock)
			{
			SetRect2(in, iconLock);
			UnLock(iconLock);
			}
		}
}

//---------------------------------------------------------------------------------------

static void SetRect2(struct ScaIconNode *in, BPTR iconLock)
{
	if (iconLock)
		{
		struct ScaWindowStruct *ws;

		SCA_LockWindowList(SCA_LockWindowList_Shared);

		for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if (LOCK_SAME == ScaSameLock(iconLock, ws->ws_Lock))
				{
				ULONG ddFlags;
				struct IBox WinRect;
				ULONG IconViewMode = TranslateViewModesToIcon(ws->ws_Viewmodes);

				WinRect.Left = ws->ws_Left;
				WinRect.Top = ws->ws_Top;
				WinRect.Width = ws->ws_Width;
				WinRect.Height = ws->ws_Height;

				ddFlags = ws->ws_ViewAll;
				if (SortOrder_Ascending == ws->ws_SortOrder)
					ddFlags |= DDFLAGS_SORTASC;
				else if (SortOrder_Descending == ws->ws_SortOrder)
					ddFlags |= DDFLAGS_SORTDESC;

				d1(KPrintF("%s/%s/%ld: ws_ViewAll=%04lx  ddFlags=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_ViewAll, ddFlags));

				SetAttrs(in->in_Icon,
					IDTA_ViewModes, IconViewMode,
					IDTA_Flags, ddFlags,
					IDTA_WindowRect, (ULONG) &WinRect,
					IDTA_WinCurrentY, ws->ws_yoffset,
					IDTA_WinCurrentX, ws->ws_xoffset,
					TAG_END);
				}
			}

		SCA_UnLockWindowList();
		}
}


static void SnapshotAllProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct ScaIconNode *in;

	ScalosLockIconListShared(iwt);

	if (IsIwtViewByIcon(iwt))
		{
		APTR undoStep = UndoBeginStep();

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			SnapshotIcon(iwt, in, undoStep);
			}
		UndoEndStep(iwt, undoStep);
		}

	ScalosUnLockIconList(iwt);

	SnapshotWindowProg(iwt, mcArg);
}

//---------------------------------------------------------------------------------------

static void SnapshotWindowProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	if (iInfos.xii_iinfos.ii_MainWindowStruct == iwt->iwt_WindowTask.mt_WindowStruct)
		WriteWBConfig();
	else
		SnapshotWindow(iwt);
}

//---------------------------------------------------------------------------------------

static void LeaveOutProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		LeaveOutIcon(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindows(iwt, LeaveOutIcon);
		}
}

//---------------------------------------------------------------------------------------

static void PutAwayProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		PutAwayIconNode(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindows(iwt, PutAwayIconNode);
		}
}

//---------------------------------------------------------------------------------------

static void EmptyTrashProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	BOOL Success = FALSE;
	struct WBArg *wbArg;
	ULONG nArgs = 0;

	do	{
		ULONG NumberOfWbArgs = 1;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArg)
			return;

		wbArg[0].wa_Name = (STRPTR) "Empty_Trashcan.module";
		wbArg[0].wa_Lock = LockScaModsDir();
		if ((BPTR)NULL == wbArg[0].wa_Lock)
			break;

		nArgs++;

		if (NumberOfWbArgs > 1)
			{
			if (mcArg->mca_IconNode)
				{
				ULONG IconType;

				GetAttr(IDTA_Type, mcArg->mca_IconNode->in_Icon, &IconType);

				if (WBGARBAGE != IconType)
					break;

				DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg,
					mcArg->mca_IconNode, &wbArg[1]
					);
				nArgs++;
				}
			else
				{
				struct ScaWindowStruct *ws;
				struct WBArg *WBArgPtr = &wbArg[1];

				SCA_LockWindowList(SCA_LockWindowList_Shared);

				for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
					{
					struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) ws->ws_WindowTask;
					struct ScaIconNode *in;

					ScalosLockIconListShared(iwtx);

					for (in=iwtx->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
						{
						struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

						if (gg->Flags & GFLG_SELECTED)
							{
							ULONG IconType;

							GetAttr(IDTA_Type, in->in_Icon, &IconType);

							if (WBGARBAGE == IconType)
								{
								if (DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg,
									in, WBArgPtr))
									{
									nArgs++;
									WBArgPtr++;
									}
								}
							}
						}

					ScalosUnLockIconList(iwtx);
					}

				SCA_UnLockWindowList();
				}
			}

		DoForAppIcons(AMCLASSICON_EmptyTrash);

		if (nArgs < 2)
			break;

		ScalosObtainSemaphoreShared(&DeleteHookSemaphore);

		if (globalDeleteHook)
			{
			Success = CallGlobalDeleteHook(&wbArg[1], nArgs - 1, DLACTION_BeginEmptyTrash);
			}

		ScalosReleaseSemaphore(&DeleteHookSemaphore);

		if (!Success)
			{
			// SCA_WBStart()
			Success = SCA_WBStartTags(wbArg, nArgs,
					SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
					SCA_Flags, SCAF_WBStart_Wait,
					SCA_WaitTimeout, 0,
					TAG_END);

			if (NumberOfWbArgs > 1)
				{
				SCA_FreeWBArgs(&wbArg[1], nArgs - 1,
					Success ? SCAF_FreeNames : SCAF_FreeNames | SCAF_FreeLocks);
				}
			if (Success)
				wbArg[0].wa_Lock = (BPTR)NULL;

			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
		} while (0);

	if (wbArg)
		{
		if (wbArg[0].wa_Lock)
			UnLock(wbArg[0].wa_Lock);

		ScalosFree(wbArg);
		}
}

//---------------------------------------------------------------------------------------

static void CloseWindowProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	iwt->iwt_CloseWindow = TRUE;
}

//---------------------------------------------------------------------------------------

static ULONG FindIconType(struct WBArg *Arg)
{
	struct ScaWindowStruct *ws;
	ULONG IconType = 0;
	BOOL Found = FALSE;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; !Found && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;

		if (ws->ws_Lock)
			{
			if (LOCK_SAME == ScaSameLock(Arg->wa_Lock, ws->ws_Lock))
				{
				struct ScaIconNode *in;

				ScalosLockIconListShared(iwt);

				for (in=iwt->iwt_WindowTask.wt_IconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
					{
					if (0 == strcmp(Arg->wa_Name, GetIconName(in)))
						{
						GetAttr(IDTA_Type, in->in_Icon, &IconType);
						Found = TRUE;
						}
					}

				ScalosUnLockIconList(iwt);
				}
			}
		else
			{
			struct ScaIconNode *in;

			ScalosLockIconListShared(iwt);

			for (in=iwt->iwt_WindowTask.wt_IconList; !Found && in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				if (in->in_DeviceIcon)
					{
					if ((BPTR)NULL == Arg->wa_Lock
						&& (0 == strcmp(Arg->wa_Name, GetIconName(in))))
						{
						GetAttr(IDTA_Type, in->in_Icon, &IconType);
						Found = TRUE;
						}
					}
				if (in->in_Lock)
					{
					if (LOCK_SAME == ScaSameLock(in->in_Lock, Arg->wa_Lock)
						&& (0 == strcmp(Arg->wa_Name, GetIconName(in))))
						{
						GetAttr(IDTA_Type, in->in_Icon, &IconType);
						Found = TRUE;
						}
					}

				}

			ScalosUnLockIconList(iwt);
			}
		}

	SCA_UnLockWindowList();

	return IconType;
}

//---------------------------------------------------------------------------------------

static BOOL CallGlobalDeleteHook(struct WBArg *wbArg, ULONG nArgs, ULONG Action)
{
	BOOL Success = FALSE;
	ULONG Result;

	do	{
		ULONG n;
		struct DeleteBeginMsg dbm;
		struct DeleteEndMsg dem;

		dbm.dbm_Length = sizeof(dbm);
		dbm.dbm_Action = Action;

		Result = CallHookPkt(globalDeleteHook, NULL, &dbm);
		if (RETURN_OK != Result)
			break;

		for (n=0; n<nArgs; n++)
			{
			struct DeleteDataMsg ddm;
			BOOL isDrawer;
			BOOL isTrashcan = FALSE;

			switch (FindIconType(&wbArg[n]))
				{
			case WBGARBAGE:
				isTrashcan = TRUE;
				isDrawer = TRUE;
				break;
			case WBDRAWER:
			case WBDEVICE:
			case WBKICK:
			case WB_TEXTICON_DRAWER:
				isDrawer = TRUE;
				break;
			default:
				isDrawer = FALSE;
				break;
				}

			ddm.ddm_Length = sizeof(ddm);
			ddm.ddm_Action = isDrawer ? DLACTION_DeleteContents : DLACTION_DeleteObject;
			ddm.ddm_Lock = wbArg[n].wa_Lock;
			ddm.ddm_Name = wbArg[n].wa_Name;

			d1(kprintf("%s/%s/%ld: ddm.ddm_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, ddm.ddm_Name));
			debugLock_d1(ddm.ddm_Lock);

			Result = CallHookPkt(globalDeleteHook, NULL, &ddm);
			if (RETURN_OK != Result)
				break;

			if (isDrawer && !isTrashcan)
				{
				ddm.ddm_Length = sizeof(ddm);
				ddm.ddm_Action = DLACTION_DeleteObject;
				ddm.ddm_Lock = wbArg[n].wa_Lock;
				ddm.ddm_Name = wbArg[n].wa_Name;

				d1(kprintf("%s/%s/%ld: ddm.ddm_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, ddm.ddm_Name));
				debugLock_d1(ddm.ddm_Lock);

				Result = CallHookPkt(globalDeleteHook, NULL, &ddm);
				}
			}

		if (RETURN_OK != Result)
			break;

		dem.dem_Length = sizeof(dem);
		dem.dem_Action = DLACTION_End;

		Result = CallHookPkt(globalDeleteHook, NULL, &dem);	

		if (RETURN_OK != Result)
			break;

		Success = TRUE;
		SCA_FreeWBArgs(wbArg, nArgs, SCAF_FreeNames | SCAF_FreeLocks);
		} while (0);

	return Success;
}

//---------------------------------------------------------------------------------------

// Workaround for broken "delete.module" implementations.
// every directory WBArg gets changed
// from	<Lock to Parent-Dir> <Directory-Name>
// to	<Lock to Directory> <empty-Name>
static void DeleteModuleKludge(struct WBArg *wbArg, ULONG nArgs)
{
	T_ExamineData *fib;

	d1(kprintf("%s/%s/%ld: START fib=%08lx\n", __FILE__, __FUNC__, __LINE__, fib));

	if (ScalosExamineBegin(&fib))
		{
		ULONG n;

		// do not change wbArg[0] here!
		for (n=1; n<nArgs; n++)
			{
			BPTR oldDir;
			BPTR oldLock = (BPTR)NULL;
			BPTR dirLock;

			oldDir = CurrentDir(wbArg[n].wa_Lock);

			dirLock = Lock(wbArg[n].wa_Name, ACCESS_READ);
			if (dirLock)
				{
				if (ScalosExamineLock(dirLock, &fib) && ScalosExamineIsDrawer(fib))
					{
					FreeCopyString(wbArg[n].wa_Name);
					oldLock = wbArg[n].wa_Lock;

					wbArg[n].wa_Lock = DupLock(dirLock);
					wbArg[n].wa_Name = AllocCopyString("");
					}

				UnLock(dirLock);
				}

			CurrentDir(oldDir);

			if (oldLock)
				UnLock(oldLock);

			d1(kprintf("%s/%s/%ld: wa_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, wbArg[n].wa_Name));
			debugLock_d1(wbArg[n].wa_Lock);
			}

		ScalosExamineEnd(&fib);
		}
}

//---------------------------------------------------------------------------------------

static void MenuCopyProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_ClearClipboard);

	if (mcArg->mca_IconNode)
		{
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		RememberFilesToCopy(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindowsShared(iwt, RememberFilesToCopy, NULL);
		}
}

//---------------------------------------------------------------------------------------

static void MenuCutProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_ClearClipboard);

	if (mcArg->mca_IconNode)
		{
		SCA_LockWindowList(SCA_LockWindowList_Shared);
		RememberFilesToCut(iwt, mcArg->mca_IconNode, NULL);
		SCA_UnLockWindowList();
		}
	else
		{
		DoForAllWindowsShared(iwt, RememberFilesToCut, NULL);
		}
}

//---------------------------------------------------------------------------------------

static void MenuPasteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(kprintf("%s/%s/%ld: Obtain WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosObtainSemaphoreShared(iwt->iwt_WindowTask.wt_WindowSemaphore);	// prevent window from closing

	StartProgTask(iwt, mcArg, AsyncPasteProg, SCCV_RunProcess_NoReply);

	d1(kprintf("%s/%s/%ld: Release WindowSemaphore iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosReleaseSemaphore(iwt->iwt_WindowTask.wt_WindowSemaphore);
}

//---------------------------------------------------------------------------------------

static void AsyncPasteProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	Object *fileTransObj;
	BPTR DestLock = (BPTR)NULL;
	APTR undoStep = NULL;

	d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		BPTR parentDirLock;
		BPTR newLock = (BPTR)NULL;
		ULONG IconType = 0;

		d1(kprintf("%s/%s/%ld: mca_IconNode=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode, GetIconName(mcArg->mca_IconNode)));

		GetAttr(IDTA_Type, mcArg->mca_IconNode->in_Icon, &IconType);

		switch (IconType)
			{
		case WBDISK:
		case WBDRAWER:
		case WBDEVICE:
		case WBGARBAGE: 		// +++ JMC 26.11.2004
		case WB_TEXTICON_DRAWER:
			if (mcArg->mca_IconNode->in_DeviceIcon)
				{
				parentDirLock = newLock = DiskInfoLock(mcArg->mca_IconNode);
				}
			else
				{
				if (mcArg->mca_IconNode->in_Lock)
					parentDirLock = mcArg->mca_IconNode->in_Lock;
				else
					parentDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
				}

			debugLock_d1(parentDirLock);

			if (parentDirLock)
				{
				BPTR oldDir= CurrentDir(parentDirLock);

				DestLock = Lock((STRPTR) GetIconName(mcArg->mca_IconNode), ACCESS_READ);
				debugLock_d1(DestLock);

				CurrentDir(oldDir);
				}
			break;
			}

		if (newLock)
			UnLock(newLock);
		}

	if ((BPTR)NULL == DestLock)
		DestLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

	debugLock_d1(DestLock);

	fileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca", 
		SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
//		SCCA_FileTrans_Number, ArgCount,
		SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
		TAG_END);

	if (fileTransObj)
		{
		struct FileTransArg *fta;
		ULONG Result = RETURN_OK;

		undoStep = UndoBeginStep();

		while (RETURN_OK == Result)
			{
			CONST_STRPTR destName;
			STRPTR destNameCopy = NULL;
			size_t len;

			ScalosObtainSemaphore(&ClipboardSemaphore);
			fta = (struct FileTransArg *) RemHead(&globalCopyClipBoard);
			ScalosReleaseSemaphore(&ClipboardSemaphore);

			if (NULL == fta)
				break;

			debugLock_d1(fta->ftarg_Arg.wa_Lock);
			d1(kprintf("%s/%s/%ld: ftarg_Arg.wa_Name=<%s>  OpCode=%ld\n", \
				__FILE__, __FUNC__, __LINE__, fta->ftarg_Arg.wa_Name, fta->ftarg_Opcode));

			switch (fta->ftarg_Opcode)
				{
			case FTOPCODE_Copy:
				len = strlen(fta->ftarg_Arg.wa_Name);
				destName = fta->ftarg_Arg.wa_Name;
				if (':' == destName[len - 1])
					{
					// Replace trailing ":" by "/" for copying volumes/disks to directories

					destNameCopy = AllocCopyString(destName);
					if (destNameCopy)
						{
						fta->ftarg_Arg.wa_Name[0] = '\0';

						destName = destNameCopy;
						StripTrailingColon(destNameCopy);
						}
					}

				UndoAddEvent(iwt, UNDO_Copy,
					UNDOTAG_UndoMultiStep, undoStep,
					UNDOTAG_CopySrcDirLock, fta->ftarg_Arg.wa_Lock,
					UNDOTAG_CopyDestDirLock, DestLock,
					UNDOTAG_CopySrcName, fta->ftarg_Arg.wa_Name,
					UNDOTAG_CopyDestName, destName,
					TAG_END);

				Result = DoMethod(fileTransObj, SCCM_FileTrans_Copy, 
					fta->ftarg_Arg.wa_Lock, DestLock, 
					fta->ftarg_Arg.wa_Name,		// srcName
					destName,			// destName
					NO_ICON_POSITION_SHORT, NO_ICON_POSITION_SHORT);
				break;

			case FTOPCODE_Move:
				UndoAddEvent(iwt, UNDO_Move,
					UNDOTAG_UndoMultiStep, undoStep,
					UNDOTAG_CopySrcDirLock, fta->ftarg_Arg.wa_Lock,
					UNDOTAG_CopyDestDirLock, DestLock,
					UNDOTAG_CopySrcName, fta->ftarg_Arg.wa_Name,
					TAG_END);

				Result = DoMethod(fileTransObj, SCCM_FileTrans_Move,
					fta->ftarg_Arg.wa_Lock, DestLock, 
					fta->ftarg_Arg.wa_Name,
					NO_ICON_POSITION_SHORT, NO_ICON_POSITION_SHORT);
				break;
				}

			if (destNameCopy)
				FreeCopyString(destNameCopy);
			if (fta->ftarg_Arg.wa_Name)
				{
				FreeCopyString(fta->ftarg_Arg.wa_Name);
				fta->ftarg_Arg.wa_Name = NULL;
				}
			if (fta->ftarg_Arg.wa_Lock)
				{
				UnLock(fta->ftarg_Arg.wa_Lock);
				fta->ftarg_Arg.wa_Lock = (BPTR)NULL;
				}

			ScalosFree(fta);

			d1(kprintf("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));
			}

		SCA_DisposeScalosObject(fileTransObj);
		}

	UndoEndStep(iwt, undoStep);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_ClearClipboard);
}

//---------------------------------------------------------------------------------------

static void MenuCreateThumbnailProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	d1(KPrintF("%s/%s/%ld: START mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));

	if (mcArg->mca_IconNode)
		{
		CreateThumbnailStart(iwt, mcArg->mca_IconNode, NULL);
		}
	else
		{
		DoForAllWindows(iwt, CreateThumbnailStart);
		}
	d1(KPrintF("%s/%s/%ld: END mca_IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, mcArg->mca_IconNode));
}

//---------------------------------------------------------------------------------------

static void RememberFilesToCopy(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	(void) undoStep;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_AddToClipboard, iwt, in, FTOPCODE_Copy);
}

//---------------------------------------------------------------------------------------

static void RememberFilesToCut(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	struct ScaIconNode inCopy;
	struct ScaIconNode *IconList = &inCopy;
	ULONG UserFlags = 0L;

	(void) undoStep;

	GetAttr(IDTA_UserFlags, in->in_Icon, &UserFlags);
	UserFlags |= ICONOBJ_USERFLAGF_DrawShadowed;
	SetAttrs(in->in_Icon, IDTA_UserFlags, UserFlags, TAG_END);

	inCopy = *in;
	inCopy.in_Node.mln_Succ = NULL;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_AddToClipboard, iwt, in, FTOPCODE_Move);

	RemoveIcons(iwt, &IconList);
	RefreshIconList(iwt, IconList, NULL);
}

//---------------------------------------------------------------------------------------

static ULONG CallTextInputHook(ULONG Action, STRPTR Prompt)
{
	struct TextInputMsg tim;

	if (NULL == globalTextInputHook)
		return 1;

	tim.tim_Length = sizeof(tim);
	tim.tim_Action = Action;
	tim.tim_Prompt = Prompt;

	return CallHookPkt(globalTextInputHook, NULL, &tim);
}

//---------------------------------------------------------------------------------------

static STRPTR FindUniqueName(STRPTR Name, size_t MaxLength)
{
	STRPTR lp = Name + strlen(Name);
	ULONG Nr = 0;
	BPTR fLock;

	do	{
		*lp = '\0';

		if (Nr)
			{
			char NrString[20];

			ScaFormatStringMaxLength(NrString, sizeof(NrString), "_%lu", Nr);
			SafeStrCat(Name, NrString, MaxLength);
			}

		fLock = Lock(Name, ACCESS_READ);
		if (fLock)
			{
			Nr++;
			UnLock(fLock);
			}
		} while (fLock);

	return Name;
}

//---------------------------------------------------------------------------------------

static BOOL CanBeSizedMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	(void) in;

	if (iwt->iwt_BackDrop)
		return FALSE;
	else
		return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL IsMainWindowMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	(void) in;

	return (BOOL) (WSV_Type_DeviceWindow == iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType);
}

//---------------------------------------------------------------------------------------

static BOOL NotMainWindowMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	(void) in;

	return (BOOL) !(WSV_Type_DeviceWindow == iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType);
}

//---------------------------------------------------------------------------------------

static BOOL NotRootWindowMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	(void) in;

	return (BOOL) !(iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RootWindow);
}

//---------------------------------------------------------------------------------------

static BOOL MustHaveLockMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		return TRUE;

	if (in && in->in_Lock)
		return TRUE;

	return FALSE;
}

//---------------------------------------------------------------------------------------

static BOOL IconsSelectedAndWriteableMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	d1(KPrintF("%s/%s/%ld: in=%08lx  IconActive=%ld\n", __FILE__, __FUNC__, __LINE__, in, IconActive));

	if (NULL == in && 0 == IconActive)
		return FALSE;
	if (iwt->iwt_ReadOnly)
		return FALSE;

	if (in && (in->in_Flags & INF_VolumeWriteProtected))
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL IconsSelectedNoDiskMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (NULL == in && 0 == IconActive)
		return FALSE;

	if (in && in->in_DeviceIcon)
		return FALSE;

	if (iwt->iwt_ReadOnly)
		return FALSE;

	if (in && (in->in_Flags & INF_VolumeWriteProtected))
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL IconsSelectedMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (NULL == in && 0 == IconActive)
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL IconsSelectedBrowserModeMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (!(iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_BrowserMode))
		return FALSE;		// only enabled if browser mode

	return IconsSelectedMenuEnable(iwt, in);
}

//---------------------------------------------------------------------------------------

static BOOL IconsSelectedNotBrowserModeMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_BrowserMode)
		return FALSE;		// only enabled if not browser mode

	return IconsSelectedMenuEnable(iwt, in);
}

//---------------------------------------------------------------------------------------

static BOOL EmptyTrashMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (NULL == in && 0 == IconActive)
		return FALSE;

//	???

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL FormatDiskMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (NULL == in && 0 == IconActive)
		return FALSE;

//	???

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL LeaveOutMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (NULL == in && 0 == IconActive)
		return FALSE;
	if (iwt->iwt_ReadOnly)
		return FALSE;
	if (WSV_Type_DeviceWindow == iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType)
		return FALSE;	// "leave out" not allowed from desktop window

//	???

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL PutAwayMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (NULL == in && 0 == IconActive)
		return FALSE;
	if (iwt->iwt_ReadOnly)
		return FALSE;
	if (WSV_Type_DeviceWindow != iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType)
		return FALSE;	// "put away" only allowed from desktop window

//	???

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL PasteMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL ClipboardEmpty;

	d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	if (iwt->iwt_ReadOnly)
		return FALSE;

	d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	ScalosObtainSemaphoreShared(&ClipboardSemaphore);
	ClipboardEmpty = IsListEmpty(&globalCopyClipBoard);
	ScalosReleaseSemaphore(&ClipboardSemaphore);
	if (ClipboardEmpty)
		return FALSE;

	d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		return TRUE;

	d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	// Desktop window, left-out icon
	if (in && in->in_Lock)
		return TRUE;

	// Desktop window, device icon
	if (in && in->in_DeviceIcon)
		return TRUE;

	d1(KPrintF("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

	return FALSE;
}

//---------------------------------------------------------------------------------------

static BOOL UndoMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL UndoListEmpty;

	ScalosObtainSemaphoreShared(&UndoListListSemaphore);
	UndoListEmpty = IsListEmpty(&globalUndoList);
	ScalosReleaseSemaphore(&UndoListListSemaphore);
	if (UndoListEmpty)
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL RedoMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	BOOL RedoListEmpty;

	ScalosObtainSemaphoreShared(&RedoListListSemaphore);
	RedoListEmpty = IsListEmpty(&globalRedoList);
	ScalosReleaseSemaphore(&RedoListListSemaphore);
	if (RedoListEmpty)
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL NotMainWindowAndNotReadingMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (WSV_Type_DeviceWindow == iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowType)
		return FALSE;
	if (iwt->iwt_Reading)
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL SnapshotMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (!IsIwtViewByIcon(iwt))
		return FALSE;
	if (NULL == in && 0 == IconActive)
		return FALSE;
	if (iwt->iwt_ReadOnly)
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL NotReadingMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	(void) in;

	if (iwt->iwt_Reading)
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------

static BOOL IsWriteableMenuEnable(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (iwt->iwt_ReadOnly)
		return FALSE;
	if (in && (in->in_Flags & INF_VolumeWriteProtected))
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) AsyncStartModule(APTR aptr, struct SM_RunProcess *msg, CONST_STRPTR ModuleName)
{
	struct WBArg *arg = aptr;
	BPTR ModsDirLock;
	BPTR oldDir = NOT_A_LOCK;
	BPTR infoModLock = (BPTR)NULL;
	BOOL Ok = FALSE;

	debugLock_d1(arg->wa_Lock);
	d1(kprintf("%s/%s/%ld: START arg->wa_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, arg->wa_Name, arg->wa_Name ? arg->wa_Name : ""));

	do	{
		ModsDirLock = LockScaModsDir();

		d1(kprintf("%s/%s/%ld: arg->wa_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, arg->wa_Name, arg->wa_Name ? arg->wa_Name : ""));
		debugLock_d1(ModsDirLock);
		if ((BPTR)NULL == ModsDirLock)
			break;

		oldDir = CurrentDir(ModsDirLock);
		infoModLock = Lock((STRPTR) ModuleName, ACCESS_READ);

		d1(kprintf("%s/%s/%ld: arg->wa_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, arg->wa_Name, arg->wa_Name ? arg->wa_Name : ""));
		debugLock_d1(infoModLock);
		if (infoModLock)
			{
			struct WBArg argArray[2];

			argArray[0].wa_Name = (STRPTR) ModuleName;
			argArray[0].wa_Lock = ModsDirLock;
			argArray[1] = *arg;

			if (SCA_WBStartTags(argArray, 2,
				SCA_Stacksize, CurrentPrefs.pref_DefaultStackSize,
				TAG_END))
				{
				d1(kprintf("%s/%s/%ld: SCA_WBStartTags Ok\n", __FILE__, __FUNC__, __LINE__));

				arg->wa_Lock = (BPTR)NULL;
				ModsDirLock = (BPTR)NULL;     // will be UnLock()ed

				Ok = TRUE;
				}
			}
		} while (0);

	d1(kprintf("%s/%s/%ld: arg->wa_Name=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, arg->wa_Name, arg->wa_Name ? arg->wa_Name : ""));

	if (!Ok)
		WBInfo(arg->wa_Lock, arg->wa_Name, iInfos.xii_iinfos.ii_Screen);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (ModsDirLock)
		UnLock(ModsDirLock);
	if (infoModLock)
		UnLock(infoModLock);
	if (arg->wa_Lock)
		UnLock(arg->wa_Lock);
	if (arg->wa_Name)
		FreeCopyString(arg->wa_Name);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//---------------------------------------------------------------------------------------
//------- CopyTo: MakeCloneArgs/CopyToCloneInfoStart/CopyToCloneWBArg -------------------

static void CopyToMakeCloneArgs(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	CopyToCloneInfoStart(iwt, wbArg, ArgCount, MaxArgs);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws != iwt->iwt_WindowTask.mt_WindowStruct)
			CopyToCloneInfoStart((struct internalScaWindowTask *) ws->ws_WindowTask, wbArg, ArgCount, MaxArgs);
		}

	SCA_UnLockWindowList();
}

//---------------------------------------------------------------------------------------

static void CopyToCloneInfoStart(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs)
{
	struct ScaIconNode *in;

	if ((*ArgCount) < MaxArgs)
		{
		ScalosLockIconListShared(iwt);

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			if (gg->Flags & GFLG_SELECTED)
				{
				ULONG IconType;

				GetAttr(IDTA_Type, in->in_Icon, &IconType);

				switch (IconType)
					{
				case WB_TEXTICON_TOOL:
				case WB_TEXTICON_DRAWER:
				case WBDRAWER:
				case WBGARBAGE:
				case WBTOOL:
				case WBPROJECT:
				case WBDISK:
					d1(kprintf("%s/%s/%ld: icon=<%s> NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), *ArgCount));
					CopyToCloneWBArg(iwt, wbArg, in, ArgCount, MaxArgs);
					break;
				default:
					break;
					}
				}
			}

		ScalosUnLockIconList(iwt);
		}
}

//---------------------------------------------------------------------------------------

static BOOL CopyToCloneWBArg(struct internalScaWindowTask *iwt, struct WBArg **wbArg, 
	struct ScaIconNode *in, ULONG *ArgCount, ULONG MaxArgs)
{
	CONST_STRPTR newName;
	size_t len;
	BPTR NLock= (BPTR)NULL;

	if ((*ArgCount) < MaxArgs)
		{
		if (NULL == in->in_Name)
			return FALSE;

		(*wbArg)->wa_Name = AllocCopyString(GetIconName(in));
		len = strlen((*wbArg)->wa_Name);
		newName = (*wbArg)->wa_Name;

		if (':' == newName[len - 1])
			{
			NLock = Lock((*wbArg)->wa_Name, SHARED_LOCK);
			(*wbArg)->wa_Lock = DupLock(NLock);
			}
		else
			{
			if (in->in_Lock)
				{
				(*wbArg)->wa_Lock = DupLock(in->in_Lock);
				d1(kprintf("%s/%s/%ld: (*wbArg)->wa_Lock = <%s>\n", __FILE__, __FUNC__, __LINE__, TestLock));
				}
			else
				{
				if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
					(*wbArg)->wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
				else
					(*wbArg)->wa_Lock = (BPTR)NULL;
				}
			}

		// (*wbArg)->wa_Name = AllocCopyString(GetIconName(in));

		d1(kprintf("%s/%s/%ld: (*wbArg)->wa_Name = <%s>\n", __FILE__, __FUNC__, __LINE__, (*wbArg)->wa_Name));

		if (NULL == (*wbArg)->wa_Name)
			{
			if ((*wbArg)->wa_Lock)
				UnLock((*wbArg)->wa_Lock);

			return FALSE;
			}

		if (NLock)
			UnLock(NLock);

		(*wbArg)++;
		(*ArgCount)++;

		return TRUE;
		}

	return FALSE;
}

//---------------------------------------------------------------------------------------

static void CopyToProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct WBArg *wbArg, *wbArgAlloc;
	ULONG ArgCount = 0;

	do	{
		struct CloneArg processArgs;
		ULONG NumberOfWbArgs = 1;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = wbArgAlloc = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArgAlloc)
			return;

		if (mcArg->mca_IconNode)
			{
			ULONG IconType;

			GetAttr(IDTA_Type, mcArg->mca_IconNode->in_Icon, &IconType);

			d1(kprintf("%s/%s/%ld: IconType = [%08lx] Name=<%s>\n", __FILE__, __FUNC__, __LINE__, IconType, mcArg->mca_IconNode->in_Name));

			if (WBKICK == IconType)
				{
				d1(kprintf("%s/%s/%ld: IconType WBKICK = %08lx\n", __FILE__, __FUNC__, __LINE__, IconType));
				break;
				}

			if (!CopyToCloneWBArg(iwt, &wbArg, mcArg->mca_IconNode, &ArgCount, NumberOfWbArgs))
				break;

			// ArgCount++;

			d1(kprintf("%s/%s/%ld: CopyToCloneWBArg() ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));
			}
		else
			{
			CopyToMakeCloneArgs(iwt, &wbArg, &ArgCount, NumberOfWbArgs);

			d1(kprintf("%s/%s/%ld: CopyToMakeCloneArgs() iwt=[%08lx] &ArgCount=[%ld])\n", __FILE__, __FUNC__, __LINE__, iwt, ArgCount));
			}

		processArgs.cla_wbArg = wbArgAlloc;
		processArgs.cla_NumArgs = ArgCount;

		RunProcess(&iwt->iwt_WindowTask, CopyToStart,
			sizeof(processArgs)/sizeof(ULONG), (struct WBArg *)(APTR) &processArgs, NULL);

		// wbArgAlloc will be freed inside <CopyToStart>
		wbArgAlloc = NULL;
		ArgCount = 0;
		} while (0);

	if (wbArgAlloc)
		{
		if (ArgCount > 0)
			SCA_FreeWBArgs(wbArgAlloc, ArgCount, SCAF_FreeNames | SCAF_FreeLocks);

		ScalosFree(wbArgAlloc);
		}
}

//---------------------------------------------------------------------------------------

static SAVEDS(ULONG) CopyToStart(APTR aptr, struct SM_RunProcess *msg)
{
	struct CloneArg *arg = aptr;
	Object *fileTransObj = NULL;
	ULONG n;
	BPTR dirLock = (BPTR)NULL;
	APTR undoStep = NULL;
	struct FileRequester *dirReq;

	do	{
		BOOL Success;
		char Path[512];


		NameFromLock(arg->cla_wbArg[0].wa_Lock, Path, sizeof(Path));

		d1(kprintf("%s/%s/%ld: arg->cla_wbArg[0].wa_Lock=<%s>\n", __FILE__, __FUNC__, __LINE__, Path));

		// AllocAslRequest()
		dirReq = AllocAslRequestTags(ASL_FileRequest,
				ASLFR_PrivateIDCMP, TRUE,
				ASLFR_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				ASLFR_TitleText, (ULONG) GetLocString(MSGID_COPYTO_ASLTITLE),
				ASLFR_DoSaveMode, TRUE,
				ASLFR_DrawersOnly, TRUE,
				TAG_END);

		if (NULL == dirReq)
			break;

		// AslRequest()
		Success = AslRequestTags(dirReq,
				ASLFR_InitialDrawer, (ULONG) Path,
				TAG_END);

		if (Success)
			{
			CONST_STRPTR destName;
			STRPTR destNameCopy = NULL;
			size_t len;
			char NLockName[512];
			char LockdirName[256];
			char TestRootName[256];
			BOOL ErrorFound;

			undoStep = UndoBeginStep();

			dirLock = Lock(dirReq->fr_Drawer, ACCESS_READ);
			if ((BPTR)NULL == dirLock)
				break;

			stccpy(LockdirName, dirReq->fr_Drawer, sizeof(LockdirName));

			fileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca", 
				SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				SCCA_FileTrans_Number, arg->cla_NumArgs,
				SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
				TAG_END);

			if (NULL == fileTransObj)
				break;

			d1(kprintf("%s/%s/%ld: NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, arg->cla_NumArgs));

			for (n=0; n < arg->cla_NumArgs; n++)
				{
				ErrorFound = FALSE;

				debugLock_d1(arg->cla_wbArg[n].wa_Lock);

				NameFromLock(arg->cla_wbArg[n].wa_Lock, NLockName, sizeof(NLockName));

				if (arg->cla_wbArg[n].wa_Name && strlen(arg->cla_wbArg[n].wa_Name) > 0)
					AddPart(NLockName, arg->cla_wbArg[n].wa_Name, sizeof(NLockName));

				if (LOCK_SAME == ScaSameLock(dirLock, arg->cla_wbArg[n].wa_Lock)) 	// Can't copy a object to itself!
					{
					DisplayBeep(NULL);
					ErrorFound = TRUE;
					d1(kprintf("%s/%s/%ld: ERROR SAME LOCK!: ErrorFound=[%ld] Source Lock=<%s> Destination Lock=<%s>\n", \
						 __FILE__, __FUNC__, __LINE__, ErrorFound, NLockName, dirReq->fr_Drawer));
					}

				if (!ErrorFound)
					{
					stccpy(TestRootName, LockdirName, strlen(NLockName) + 1); // Check if destination with a max string lenght
					if (0 == Stricmp(TestRootName, NLockName))		  // from source's string lenght = source.
						{
						DisplayBeep(NULL);
						ErrorFound = TRUE;
						d1(kprintf("%s/%s/%ld: SAME! : ErrorFound=[%ld] NLockName=<%s> TestRootname=<%s>\n", \
							 __FILE__, __FUNC__, __LINE__, ErrorFound, NLockName, TestRootName));
						}
					}

				if (!ErrorFound)
					{
					len = strlen(arg->cla_wbArg[n].wa_Name);
					destName = arg->cla_wbArg[n].wa_Name;


					d1(kprintf("%s/%s/%ld: Copy NLockName=<%s> to Destination=<%s>\n", __FILE__, __FUNC__, __LINE__, NLockName, LockdirName));

					if (':' == destName[len - 1])
						{
						// Replace trailing ":" by "/" for copying volumes/disks to directories
						destNameCopy = AllocCopyString(destName);
						if (destNameCopy)
							{
							arg->cla_wbArg[n].wa_Name[0] = '\0';
							destName = destNameCopy;
							StripTrailingColon(destNameCopy);
							d1(kprintf("%s/%s/%ld: destName=<%s> destNameCopy=<%s>\n", __FILE__, __FUNC__, __LINE__, destName, destNameCopy));
							}
						}

					UndoAddEvent((struct internalScaWindowTask *) msg->WindowTask, UNDO_Copy,
						UNDOTAG_UndoMultiStep, undoStep,
						UNDOTAG_CopySrcDirLock, arg->cla_wbArg[n].wa_Lock,
						UNDOTAG_CopyDestDirLock, dirLock,
						UNDOTAG_CopySrcName, arg->cla_wbArg[n].wa_Name,
						UNDOTAG_CopyDestName, destName,
						TAG_END);

					DoMethod(fileTransObj, SCCM_FileTrans_Copy,
						arg->cla_wbArg[n].wa_Lock, dirLock,
						arg->cla_wbArg[n].wa_Name,
						destName,
						NO_ICON_POSITION_SHORT, NO_ICON_POSITION_SHORT);
					}
				}

			if (destNameCopy)
					FreeCopyString(destNameCopy);
			}
		} while (0);

	UndoEndStep((struct internalScaWindowTask *) msg->WindowTask, undoStep);

	SCA_FreeWBArgs(arg->cla_wbArg, arg->cla_NumArgs, SCAF_FreeNames | SCAF_FreeLocks);
	ScalosFree(arg->cla_wbArg);

	if (fileTransObj)
		SCA_DisposeScalosObject(fileTransObj);

	if (dirLock)
		UnLock(dirLock);

	if (dirReq)
		FreeAslRequest(dirReq);

	return 0;
}

//---------------------------------------------------------------------------------------
//------- MoveTo: MoveToMakeCloneArgs/MoveToCloneInfoStart ------------------------------
//---------------------------------------------------------------------------------------

static void MoveToMakeCloneArgs(struct internalScaWindowTask *iwt, 
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	MoveToCloneInfoStart(iwt, wbArg, ArgCount, MaxArgs);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws != iwt->iwt_WindowTask.mt_WindowStruct)
			MoveToCloneInfoStart((struct internalScaWindowTask *) ws->ws_WindowTask, wbArg, ArgCount, MaxArgs);
		}
	SCA_UnLockWindowList();
}

//---------------------------------------------------------------------------------------

static void MoveToCloneInfoStart(struct internalScaWindowTask *iwt,
	struct WBArg **wbArg, ULONG *ArgCount, ULONG MaxArgs)
{
	struct ScaIconNode *in;

	if ((*ArgCount) <= MaxArgs)
		{
		ScalosLockIconListShared(iwt);

		d1(KPrintF("%s/%s/%ld: ScalosLockIconListShared(iwt)\n", __FILE__, __FUNC__, __LINE__));

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			if (gg->Flags & GFLG_SELECTED)
				{
				ULONG IconType;

				GetAttr(IDTA_Type, in->in_Icon, &IconType);

				switch (IconType)
					{
				case WBDISK:
				case WBDEVICE:
				case WBKICK:
					d1(kprintf("%s/%s/%ld: icon=<%s> NumArgs=[%ld] IconType=%08lx\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), *ArgCount, IconType));

					DisplayBeep(NULL);

					UseRequestArgs(NULL, MSGID_MOVETO_DEVICE, MSGID_OKNAME, 1, GetIconName(in));

					ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, FALSE);     // UnSelect drive icon.

					CopyToCloneWBArg(iwt, wbArg, in, ArgCount, MaxArgs);
					//CloneWBArg(iwt, wbArg, in, ArgCount);
					break;
				case WB_TEXTICON_TOOL:
				case WB_TEXTICON_DRAWER:
				case WBDRAWER:
				case WBGARBAGE:
				case WBTOOL:
				case WBPROJECT:
					d1(kprintf("%s/%s/%ld: icon=<%s> NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, GetIconName(in), *ArgCount));
					//CloneWBArg(iwt, wbArg, in, ArgCount);
					CopyToCloneWBArg(iwt, wbArg, in, ArgCount, MaxArgs);
					break;

				default:
					break;
					}
				}
			}

		ScalosUnLockIconList(iwt);
		}
}

//---------------------------------------------------------------------------------------

static void MoveToProg(struct internalScaWindowTask *iwt, const struct MenuCmdArg *mcArg)
{
	struct WBArg *wbArg, *wbArgAlloc;
	ULONG ArgCount = 0;

	do	{
		struct CloneArg processArgs;
		ULONG NumberOfWbArgs = 1;

		if (mcArg->mca_IconNode)
			NumberOfWbArgs += 1;
		else
			NumberOfWbArgs += SCA_CountWBArgs(mcArg->mca_IconNode);

		wbArg = wbArgAlloc = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		if (NULL == wbArgAlloc)
			return;

		if (mcArg->mca_IconNode)
			{
			ULONG IconType;

			GetAttr(IDTA_Type, mcArg->mca_IconNode->in_Icon, &IconType);

			if (WBDISK == IconType ||
				WBDEVICE == IconType ||
				WBKICK == IconType)
				{
				DisplayBeep(NULL);
				break;
				}

			if (!CopyToCloneWBArg(iwt, &wbArg, mcArg->mca_IconNode, &ArgCount, NumberOfWbArgs))
				break;

			// ArgCount++;

			d1(kprintf("%s/%s/%ld: [a] ArgCount=[%ld]\n", __FILE__, __FUNC__, __LINE__, ArgCount));
			}
		else
			{
			MoveToMakeCloneArgs(iwt, &wbArg, &ArgCount, NumberOfWbArgs);
			}

		d1(KPrintF("%s/%s/%ld: [b] ArgCount=[%ld] iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, ArgCount, iwt));

		processArgs.cla_wbArg = wbArgAlloc;
		processArgs.cla_NumArgs = ArgCount;

		RunProcess(&iwt->iwt_WindowTask, MoveToStart,
			sizeof(processArgs)/sizeof(ULONG), (struct WBArg *)(APTR) &processArgs, NULL);

		// wbArgAlloc will be freed inside <MoveToStart>
		wbArgAlloc = NULL;
		ArgCount = 0;
		} while (0);

	if (wbArgAlloc)
		{
		if (ArgCount > 0)
			SCA_FreeWBArgs(wbArgAlloc, ArgCount, SCAF_FreeNames | SCAF_FreeLocks);

		ScalosFree(wbArgAlloc);
		}
}

//---------------------------------------------------------------------------------------

static SAVEDS(ULONG) MoveToStart(APTR aptr, struct SM_RunProcess *msg)
{
	struct CloneArg *arg = aptr;
	Object *fileTransObj = NULL;
	ULONG n;
	BPTR dirLock = (BPTR)NULL;
	APTR undoStep = NULL;
	struct FileRequester *dirReq;


	d1(kprintf("%s/%s/%ld: START NumArgs=[%ld]\n", __FILE__, __FUNC__, __LINE__, arg->cla_NumArgs));

	do	{
		BOOL Success;
		ULONG np = 0;
		char Path[512];

		// AllocAslRequest()
		dirReq = AllocAslRequestTags(ASL_FileRequest,
				ASLFR_PrivateIDCMP, TRUE,
				ASLFR_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				ASLFR_TitleText, (ULONG) GetLocString(MSGID_MOVETO_ASLTITLE),
				ASLFR_DoSaveMode, TRUE,
				ASLFR_DrawersOnly, TRUE,
				TAG_END);

		if (NULL == dirReq)
			break;

		for (n=0; n < arg->cla_NumArgs; n++)
			{
			np++;
			if (':' == arg->cla_wbArg[n].wa_Name[strlen(arg->cla_wbArg[n].wa_Name) - 1])	// Check number of drives if selected from workbench
				np--;									// Decrease "np" variable if a drive is found.
			else
				{
				if ((BPTR)NULL != arg->cla_wbArg[n].wa_Lock)				      // If object isn't a drive, select last valid path found
					NameFromLock(arg->cla_wbArg[n].wa_Lock, Path, sizeof(Path));	// for "ASLFR_InitialDrawer" field.
				}

			d1(kprintf("%s/%s/%ld: CHECKING ARGS: n=[%ld] np=[%ld] NumArgs=[%ld] Path=<%s> arg->cla_wbArg[%ld]=<%s>\n", \
				 __FILE__, __FUNC__, __LINE__, n, np, arg->cla_NumArgs, Path, n, arg->cla_wbArg[n].wa_Name));
			}

		d1(kprintf("%s/%s/%ld: END np=[%ld] NumArgs=[%ld]\n", __FILE__, __FUNC__, __LINE__, np, arg->cla_NumArgs));

		if (0 == np)										// if "np= 0" all objects found are drives,
			{										// so abort MoveTo function.
			d1(kprintf("%s/%s/%ld: np=[%ld] NumArgs=[%ld]\n", \
				__FILE__, __FUNC__, __LINE__, np, arg->cla_NumArgs));
			break;
			}

		// AslRequest()
		Success = AslRequestTags(dirReq,
				ASLFR_InitialDrawer, (ULONG) Path,
				TAG_END);

		if (Success)
			{
			char NLockName[512];
			char ProgramIcon[512];
			char LockdirName[256];
			char TestRootName[256];
			CONST_STRPTR SrcName;
			size_t len;
			BOOL ErrorFound;

			undoStep = UndoBeginStep();

			dirLock = Lock(dirReq->fr_Drawer, ACCESS_READ);
			if ((BPTR)NULL == dirLock)
				break;

			stccpy(LockdirName, dirReq->fr_Drawer, sizeof(LockdirName));

			fileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca", 
				SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
				SCCA_FileTrans_Number, arg->cla_NumArgs,
				SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
				TAG_END);

			if (NULL == fileTransObj)
				break;

			d1(kprintf("%s/%s/%ld: NumArgs=%ld\n", __FILE__, __FUNC__, __LINE__, arg->cla_NumArgs));

			for (n=0; n <arg->cla_NumArgs; n++)
				{
				ErrorFound = FALSE;

				debugLock_d1(arg->cla_wbArg[n].wa_Lock);

				len = strlen(arg->cla_wbArg[n].wa_Name);
				SrcName = arg->cla_wbArg[n].wa_Name;

				NameFromLock(arg->cla_wbArg[n].wa_Lock, NLockName, sizeof(NLockName));

				if (SrcName && len > 0)
					AddPart(NLockName, SrcName, sizeof(NLockName));

				if (LOCK_SAME == ScaSameLock(dirLock, arg->cla_wbArg[n].wa_Lock)) 	// Can't copy a object to itself!
					{
					DisplayBeep(NULL);
					ErrorFound = TRUE;
					d1(kprintf("%s/%s/%ld: ERROR SAME LOCK!: ErrorFound=[%ld] Source Lock=<%s> Destination Lock=<%s>\n", \
						 __FILE__, __FUNC__, __LINE__, ErrorFound, NLockName, dirReq->fr_Drawer));
					}

				if (!ErrorFound)
					{
					stccpy(TestRootName, LockdirName, strlen(NLockName) + 1); // Check if destination with a max string lenght
					if (0 == Stricmp(TestRootName, NLockName))		  // from source's string lenght = source.
						{
						DisplayBeep(NULL);
						ErrorFound = TRUE;
						d1(kprintf("%s/%s/%ld: SAME! : ErrorFound=[%ld] NLockName=<%s> TestRootname=<%s>\n", \
							 __FILE__, __FUNC__, __LINE__, ErrorFound, NLockName, TestRootName));
						}
					}

				if (!ErrorFound)
					{
					if (':' == SrcName[len - 1])	// Skip all drives found, from wb arg.
						ErrorFound = TRUE;
					}

				if (!ErrorFound)
					{
					BPTR IconLock;

					d1(kprintf("%s/%s/%ld: NUMBER=[%ld] ErrorFound=[%ld] [Move <%s> - Full path = <%s>] To =<%s>\n", \
						 __FILE__, __FUNC__, __LINE__, n, ErrorFound, SrcName, NLockName, LockdirName));

					UndoAddEvent((struct internalScaWindowTask *) msg->WindowTask, UNDO_Move,
						UNDOTAG_UndoMultiStep, undoStep,
						UNDOTAG_CopySrcDirLock, arg->cla_wbArg[n].wa_Lock,
						UNDOTAG_CopyDestDirLock, dirLock,
						UNDOTAG_CopySrcName, SrcName,
						TAG_END);

					DoMethod(fileTransObj, SCCM_FileTrans_Move,
						arg->cla_wbArg[n].wa_Lock, dirLock,
						SrcName,
						NO_ICON_POSITION_SHORT, NO_ICON_POSITION_SHORT);

					snprintf(ProgramIcon, sizeof(ProgramIcon), "%s.info", SrcName);

					if (IconLock = Lock(ProgramIcon, SHARED_LOCK))
						{
						UnLock(IconLock);

						d1(kprintf("%s/%s/%ld: FOUND ICON HERE!!!: [Move Icon <%s>] To =<%s>\n", \
							 __FILE__, __FUNC__, __LINE__, ProgramIcon, LockdirName));

						UndoAddEvent((struct internalScaWindowTask *) msg->WindowTask, UNDO_Move,
							UNDOTAG_UndoMultiStep, undoStep,
							UNDOTAG_CopySrcDirLock, arg->cla_wbArg[n].wa_Lock,
							UNDOTAG_CopyDestDirLock, dirLock,
							UNDOTAG_CopySrcName, ProgramIcon,
							TAG_END);

						DoMethod(fileTransObj, SCCM_FileTrans_Move,
							arg->cla_wbArg[n].wa_Lock, dirLock,
							ProgramIcon,
							NO_ICON_POSITION_SHORT, NO_ICON_POSITION_SHORT);

						}
					}
				}
			}
		} while (0);

	UndoEndStep((struct internalScaWindowTask *) msg->WindowTask, undoStep);

	SCA_FreeWBArgs(arg->cla_wbArg, arg->cla_NumArgs, SCAF_FreeNames | SCAF_FreeLocks);
	ScalosFree(arg->cla_wbArg);

	if (fileTransObj)
		SCA_DisposeScalosObject(fileTransObj);

	if (dirLock)
		UnLock(dirLock);

	if (dirReq)
		FreeAslRequest(dirReq);

	return 0;
}

//----------------------------------------------------------------------------

static void CreateThumbnailStart(struct internalScaWindowTask *iwt, struct ScaIconNode *in, APTR undoStep)
{
	BPTR IconDirLock;
	Object *OrigIconObj;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));

	if (in->in_Lock)
		IconDirLock = in->in_Lock;
	else
		IconDirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

	OrigIconObj = CloneIconObject(in->in_Icon);

	if (AddThumbnailIcon(iwt, in->in_Icon,
		IconDirLock,
		GetIconName(in),
		THUMBNAILICONF_SAVEICON | THUMBNAILICONF_NOICONPOSITION,
		undoStep))
		{
		d1(KPrintF("%s/%s/%ld: AddThumbnailIcon succeeded\n", __FILE__, __FUNC__, __LINE__));

		UnsnapshotIcon(iwt, in, FALSE, undoStep);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DoMethod(in->in_Icon, IDTM_FreeLayout, IOFREELAYOUTF_ScreenAvailable);
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_LayoutIcon,
			in->in_Icon, IOLAYOUTF_NormalImage);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		AfterUnsnapshotIcon(iwt);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// Start thumbnail generation process
		GenerateThumbnails(iwt);
		}

	if (OrigIconObj)
		DisposeIconObject(OrigIconObj);

	d1(KPrintF("%s/%s/%ld: END iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
}

//---------------------------------------------------------------------------------------

static void DoForAppIcons(ULONG AppMsgClass)
{
	struct ScaWindowStruct *ws;

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (!(ws->ws_Flags & WSV_FlagF_TaskSleeps)
			&& ws->ws_Window
			&& (0 == ws->ws_Lock) )
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ws->ws_WindowTask;
			struct ScaIconNode *in;

			d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			ScalosLockIconListShared(iwt);

			for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
				{
				struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

				if (gg->Flags & GFLG_SELECTED)
					{
					ULONG IconType;

					GetAttr(IDTA_Type, in->in_Icon, &IconType);

					if (WBAPPICON == IconType)
						{
						d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));
						WindowSendAppIconMsg(iwt, AppMsgClass, in);
						}
					}
				}

			ScalosUnLockIconList(iwt);

			break;
			}
		}

	SCA_UnLockWindowList();
}

//---------------------------------------------------------------------------------------


