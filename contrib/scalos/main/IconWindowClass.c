// IconWindowClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <graphics/gfxmacros.h>
#include <graphics/rpattr.h>
#include <libraries/asl.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/console.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

#define	CHECKUPDATE_TIMEOUT	2		// delay for IconWinCheckUpdate() in s

typedef LONG ASM (*CLEANUPBYSORTFUNC)(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1);


// IconWindow class instance data +jl+ 20010906
struct IconWindowClassInstance
	{
	ULONG ici_Dunno;
	struct NotifyRequest *ici_NotifyReq;
	STRPTR ici_DrawerName;
	};

struct IconListNode
	{
	struct MinNode iln_Node;
	struct ScaIconNode *iln_IconNode;
	};

//----------------------------------------------------------------------------

// local functions

static SAVEDS(ULONG) IconWindowClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ReadIconList(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ReadIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Message(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ScheduleUpdate(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ShowIconToolTip(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DragBegin(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DragQuery(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DragFinish(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DragEnter(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DragLeave(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DragDrop(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Set(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Get(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_New(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Dispose(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Open(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_UpdateIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_UpdateIconTags(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_AddIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_RemIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_MakeWbArg(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_CountWbArg(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_SetVirtSize(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Redraw(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Sleep(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Wakeup(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DrawIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_LayoutIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Ping(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_RawKey(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_GetDefIcon(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_MenuCommand(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_NewViewMode(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_DeltaMove(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_UnCleanup(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_UnCleanupRegion(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Cleanup(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Update(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_AddToStatusBar(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_RemFromStatusBar(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_UpdateStatusBar(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_AddToControlBar(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_RemFromControlBar(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_UpdateControlBar(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ShowPopupMenu(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ShowGadgetToolTip(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ActivateIconLeft(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ActivateIconRight(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ActivateIconUp(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ActivateIconDown(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ActivateIconNext(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ActivateIconPrevious(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_CleanupByName(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_CleanupByDate(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_CleanupBySize(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_CleanupByType(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_AddGadget(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ImmediateCheckUpdate(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_GetIconFileType(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_ClearIconFileTypes(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_NewPatternNumber(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_RandomizePatternNumber(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_NewPath(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_HistoryBack(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_HistoryForward(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_StartNotify(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_Browse(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_StartPopOpenTimer(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_StopPopOpenTimer(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_AddUndoEvent(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_BeginUndoStep(Class *cl, Object *o, Msg msg);
static ULONG IconWindowClass_EndUndoStep(Class *cl, Object *o, Msg msg);

static SAVEDS(ULONG) IconWinBrowseProc(APTR aptr, struct SM_RunProcess *msg);
static ULONG IconWinNotify(struct internalScaWindowTask *iwt, struct IntuiMessage *im);
static void ScrollWindowXY(struct internalScaWindowTask *iwt, LONG dx, LONG dy);
static void IconWin_RemIcon(struct internalScaWindowTask *iwt, struct WBArg *IconArg);
static void SetVirtSize(struct internalScaWindowTask *iwt, ULONG Flags);
static void RedrawIconWindow(struct internalScaWindowTask *iwt, ULONG Flags);
static void ReLayoutIcons(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static void ChangeSelectedIconUpDown(struct internalScaWindowTask *iwt, UWORD Code);
static void ChangeSelectedIconLeftRight(struct internalScaWindowTask *iwt, UWORD Code);
static void ChangeSelectedIconNext(struct internalScaWindowTask *iwt);
static void ChangeSelectedIconPrev(struct internalScaWindowTask *iwt);
static void MakeIconVisible(struct internalScaWindowTask *iwt, struct ScaIconNode *sIcon);
static void BuildIconColumn(struct IconListNode **DestIconList, struct ScaIconNode *SrcIconList, struct ScaIconNode *inCol);
static void BuildIconRow(struct IconListNode **DestIconList, struct ScaIconNode *SrcIconList, struct ScaIconNode *inCol);
static void BuildIconListSorted(struct IconListNode **DestIconList, struct ScaIconNode *SrcIconList);
static SAVEDS(LONG) IconCompareXFunc(struct Hook *hook, struct IconListNode *iln2, struct IconListNode *iln1);
static SAVEDS(LONG) IconCompareYFunc(struct Hook *hook, struct IconListNode *iln2, struct IconListNode *iln1);
static SAVEDS(LONG) IconCompareNameFunc(struct Hook *hook, struct IconListNode *iln2, struct IconListNode *iln1);
static struct ScaIconNode *FindFirstSelectedIcon(struct internalScaWindowTask *iwt);
static void SelectIconByName(struct internalScaWindowTask *iwt, CONST_STRPTR Name);
static void SelectNewIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *NewSelected);
static void SetShowType(struct internalScaWindowTask *iwt, ULONG newShowType);
static ULONG IconWindowCleanupBy(struct internalScaWindowTask *, CLEANUPBYSORTFUNC sortFunc);
static SAVEDS(LONG) IconSortByNameFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1);
static SAVEDS(LONG) IconSortByDateFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1);
static SAVEDS(LONG) IconSortBySizeFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1);
static SAVEDS(LONG) IconSortByTypeFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1);
static void SetThumbnailView(struct internalScaWindowTask *iwt, ULONG NewThumbnailMode);
static void SetThumbnailsGenerating(struct internalScaWindowTask *iwt, BOOL ThumbnailsGenerating);
static void GetIconsTotalBoundingBox(struct internalScaWindowTask *iwt);
static void IconWindowUpdateTransparency(struct internalScaWindowTask *iwt);
static void SendNewIconPathMsg(struct internalScaWindowTask *iwt, CONST_STRPTR Path, struct TagItem *TagList);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------


struct ScalosClass *initIconWindowClass(const struct PluginClass *plug)
{
	struct ScalosClass *IconWindowClass;

	IconWindowClass = SCA_MakeScalosClass(plug->plug_classname, 
			plug->plug_superclassname,
			sizeof(struct IconWindowClassInstance),
			NULL);

	if (IconWindowClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(IconWindowClass->sccl_class->cl_Dispatcher, IconWindowClass_Dispatcher);
		}

	return IconWindowClass;
}


static SAVEDS(ULONG) IconWindowClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(KPrintF("%s/%s/%ld: MethodID=%08lx\n", __FILE__, __FUNC__, __LINE__, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = IconWindowClass_New(cl, o, msg);
		break;

	case OM_DISPOSE:
		Result = IconWindowClass_Dispose(cl, o, msg);
		break;

	case OM_SET:
		Result = IconWindowClass_Set(cl, o, msg);
		break;

	case OM_GET:
		Result = IconWindowClass_Get(cl, o, msg);
		break;

	case SCCM_IconWin_ReadIconList:
		Result = IconWindowClass_ReadIconList(cl, o, msg);
		break;

	case SCCM_IconWin_ReadIcon:
		Result = IconWindowClass_ReadIcon(cl, o, msg);
		break;

	case SCCM_IconWin_DragBegin:
		Result = IconWindowClass_DragBegin(cl, o, msg);
		break;

	case SCCM_IconWin_DragQuery:
		Result = IconWindowClass_DragQuery(cl, o, msg);
		break;

	case SCCM_IconWin_DragFinish:
		Result = IconWindowClass_DragFinish(cl, o, msg);
		break;

	case SCCM_IconWin_DragEnter:
		Result = IconWindowClass_DragEnter(cl, o, msg);
		break;

	case SCCM_IconWin_DragLeave:
		Result = IconWindowClass_DragLeave(cl, o, msg);
		break;

	case SCCM_IconWin_DragDrop:
		Result = IconWindowClass_DragDrop(cl, o, msg);
		break;

	case SCCM_IconWin_Open:
		Result = IconWindowClass_Open(cl, o, msg);
		break;

	case SCCM_IconWin_UpdateIcon:
		Result = IconWindowClass_UpdateIcon(cl, o, msg);
		break;

	case SCCM_IconWin_UpdateIconTags:
		Result = IconWindowClass_UpdateIconTags(cl, o, msg);
		break;

	case SCCM_IconWin_AddIcon:
		Result = IconWindowClass_AddIcon(cl, o, msg);
		break;

	case SCCM_IconWin_RemIcon:
		Result = IconWindowClass_RemIcon(cl, o, msg);
		break;

	case SCCM_IconWin_MakeWBArg:
		Result = IconWindowClass_MakeWbArg(cl, o, msg);
		break;

	case SCCM_IconWin_CountWBArg:
		Result = IconWindowClass_CountWbArg(cl, o, msg);
		break;

	case SCCM_IconWin_CleanUp:
		Result = IconWindowClass_Cleanup(cl, o, msg);
		break;

	case SCCM_IconWin_UnCleanUp:
		Result = IconWindowClass_UnCleanup(cl, o, msg);
		break;

	case SCCM_IconWin_UnCleanUpRegion:
		Result = IconWindowClass_UnCleanupRegion(cl, o, msg);
		break;

	case SCCM_IconWin_SetVirtSize:
		Result = IconWindowClass_SetVirtSize(cl, o, msg);
		break;

	case SCCM_IconWin_Redraw:
		Result = IconWindowClass_Redraw(cl, o, msg);
		break;

	case SCCM_IconWin_Update:
		Result = IconWindowClass_Update(cl, o, msg);
		break;

	case SCCM_IconWin_ScheduleUpdate:
		Result = IconWindowClass_ScheduleUpdate(cl, o, msg);
		break;

	case SCCM_IconWin_ShowIconToolTip:
		Result = IconWindowClass_ShowIconToolTip(cl, o, msg);
		break;

	case SCCM_IconWin_RawKey:
		Result = IconWindowClass_RawKey(cl, o, msg);
		break;

	case SCCM_IconWin_Sleep:
		Result = IconWindowClass_Sleep(cl, o, msg);
		break;

	case SCCM_IconWin_WakeUp:
		Result = IconWindowClass_Wakeup(cl, o, msg);
		break;

	case SCCM_IconWin_MenuCommand:
		Result = IconWindowClass_MenuCommand(cl, o, msg);
		break;

	case SCCM_IconWin_NewViewMode:
		Result = IconWindowClass_NewViewMode(cl, o, msg);
		break;

	case SCCM_Message:
		Result = IconWindowClass_Message(cl, o, msg);
		break;

	case SCCM_Ping:
		Result = IconWindowClass_Ping(cl, o, msg);
		break;

	case SCCM_IconWin_DrawIcon:
		Result = IconWindowClass_DrawIcon(cl, o, msg);
		break;

	case SCCM_IconWin_LayoutIcon:
		Result = IconWindowClass_LayoutIcon(cl, o, msg);
		break;

	case SCCM_IconWin_DeltaMove:
		Result = IconWindowClass_DeltaMove(cl, o, msg);
		break;

	case SCCM_IconWin_GetDefIcon:
		Result = IconWindowClass_GetDefIcon(cl, o, msg);
		break;

	case SCCM_IconWin_AddToStatusBar:
		Result = IconWindowClass_AddToStatusBar(cl, o, msg);
		break;

	case SCCM_IconWin_RemFromStatusBar:
		Result = IconWindowClass_RemFromStatusBar(cl, o, msg);
		break;

	case SCCM_IconWin_UpdateStatusBar:
		Result = IconWindowClass_UpdateStatusBar(cl, o, msg);
		break;

	case SCCM_IconWin_AddToControlBar:
		Result = IconWindowClass_AddToControlBar(cl, o, msg);
		break;

	case SCCM_IconWin_RemFromControlBar:
		Result = IconWindowClass_RemFromControlBar(cl, o, msg);
		break;

	case SCCM_IconWin_UpdateControlBar:
		Result = IconWindowClass_UpdateControlBar(cl, o, msg);
		break;

	case SCCM_IconWin_ShowPopupMenu:
		Result = IconWindowClass_ShowPopupMenu(cl, o, msg);
		break;

	case SCCM_IconWin_ShowGadgetToolTip:
		Result = IconWindowClass_ShowGadgetToolTip(cl, o, msg);
		break;

	case SCCM_IconWin_ActivateIconLeft:
		Result = IconWindowClass_ActivateIconLeft(cl, o, msg);
		break;

	case SCCM_IconWin_ActivateIconRight:
		Result = IconWindowClass_ActivateIconRight(cl, o, msg);
		break;

	case SCCM_IconWin_ActivateIconUp:
		Result = IconWindowClass_ActivateIconUp(cl, o, msg);
		break;

	case SCCM_IconWin_ActivateIconDown:
		Result = IconWindowClass_ActivateIconDown(cl, o, msg);
		break;

	case SCCM_IconWin_ActivateIconNext:
		Result = IconWindowClass_ActivateIconNext(cl, o, msg);
		break;

	case SCCM_IconWin_ActivateIconPrevious:
		Result = IconWindowClass_ActivateIconPrevious(cl, o, msg);
		break;

	case SCCM_IconWin_CleanUpByName:
		Result = IconWindowClass_CleanupByName(cl, o, msg);
		break;

	case SCCM_IconWin_CleanUpByDate:
		Result = IconWindowClass_CleanupByDate(cl, o, msg);
		break;

	case SCCM_IconWin_CleanUpBySize:
		Result = IconWindowClass_CleanupBySize(cl, o, msg);
		break;

	case SCCM_IconWin_CleanUpByType:
		Result = IconWindowClass_CleanupByType(cl, o, msg);
		break;

	case SCCM_IconWin_AddGadget:
		Result = IconWindowClass_AddGadget(cl, o, msg);
		break;

	case SCCM_IconWin_ImmediateCheckUpdate:
		Result = IconWindowClass_ImmediateCheckUpdate(cl, o, msg);
		break;

	case SCCM_IconWin_GetIconFileType:
		Result = IconWindowClass_GetIconFileType(cl, o, msg);
		break;

	case SCCM_IconWin_ClearIconFileTypes:
		Result = IconWindowClass_ClearIconFileTypes(cl, o, msg);
		break;

	case SCCM_IconWin_NewPatternNumber:
		Result = IconWindowClass_NewPatternNumber(cl, o, msg);
		break;

	case SCCM_IconWin_RandomizePatternNumber:
		Result = IconWindowClass_RandomizePatternNumber(cl, o, msg);
		break;

	case SCCM_IconWin_NewPath:
		Result = IconWindowClass_NewPath(cl, o, msg);
		break;

	case SCCM_IconWin_History_Back:
		Result = IconWindowClass_HistoryBack(cl, o, msg);
		break;

	case SCCM_IconWin_History_Forward:
		Result = IconWindowClass_HistoryForward(cl, o, msg);
		break;

	case SCCM_IconWin_StartNotify:
		Result = IconWindowClass_StartNotify(cl, o, msg);
		break;

	case SCCM_IconWin_Browse:
		Result = IconWindowClass_Browse(cl, o, msg);
		break;

	case SCCM_IconWin_WBStartupFinished:
		// currently No-Op
		Result = 0;
		break;

	case SCCM_IconWin_StartPopOpenTimer:
		Result = IconWindowClass_StartPopOpenTimer(cl, o, msg);
		break;

	case SCCM_IconWin_StopPopOpenTimer:
		Result = IconWindowClass_StopPopOpenTimer(cl, o, msg);
		break;

	case SCCM_IconWin_AddUndoEvent:
		Result = IconWindowClass_AddUndoEvent(cl, o, msg);
		break;

	case SCCM_IconWin_BeginUndoStep:
		Result = IconWindowClass_BeginUndoStep(cl, o, msg);
		break;

	case SCCM_IconWin_EndUndoStep:
		Result = IconWindowClass_EndUndoStep(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG IconWindowClass_ReadIconList(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_ReadIconList *mrl = (struct msg_ReadIconList *) msg;
	enum ScanDirResult Result;
	UWORD PreviousViewAll;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  ws=%08lx  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.mt_WindowStruct, iwt->iwt_WindowTask.wt_Window));

	if (mrl->mrl_Flags & SCCV_IconWin_ReadIconList_ShowAll)
		iwt->iwt_OldShowType = DDFLAGS_SHOWALL;
	else
		iwt->iwt_OldShowType = iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll;

	d1(KPrintF("%s/%s/%ld: ws_ViewAll=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll));

	PreviousViewAll = iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll;

	Result = ReadIconList(iwt);

	d1(KPrintF("%s/%s/%ld: ws_ViewAll=%ld  Result=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll, Result));

	if (RETURN_OK == Result && PreviousViewAll != iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll)
		Result = ReadIconList(iwt);

	return (ULONG) Result;
}


static ULONG IconWindowClass_ReadIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_ReadIcon *mri = (struct msg_ReadIcon *) msg;

	d1(kprintf("%s/%s/%ld: wt_Window=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window));

	return (ULONG) IconWindowReadIcon(iwt, mri->mri_Name, mri->mri_ria);
}


static ULONG IconWindowClass_Message(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Message *msm = (struct msg_Message *) msg;

	if (NOTIFY_CLASS == msm->msm_iMsg->Class && NOTIFY_CODE == msm->msm_iMsg->Code)
		return IconWinNotify(iwt, msm->msm_iMsg);

	return DoSuperMethodA(cl, o, msg);
}


// scan icon window directory for changes and add/remove/change icons
// Result: Success, FALSE indicates failure

static ULONG IconWindowClass_ScheduleUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_CheckUpdatePending;

	iwt->iwt_NotifyCounter = CHECKUPDATE_TIMEOUT;

	return 0;
}


static ULONG IconWindowClass_ShowIconToolTip(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_ShowIconToolTip *mtt = (struct msg_ShowIconToolTip *) msg;

	if (CurrentPrefs.pref_iCandy >= 1)
		{
		DoMethod(o, SCCM_IconWin_GetIconFileType, mtt->mtt_IconNode);

		return IconWinShowIconToolTip(iwt, mtt->mtt_IconNode);
		}
	else
		return 0;
}


static ULONG IconWindowClass_ShowGadgetToolTip(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_ShowGadgetToolTip *sgtt = (struct msg_ShowGadgetToolTip *) msg;

	if (CurrentPrefs.pref_iCandy >= 1)
		return IconWinShowGadgetToolTip(iwt, sgtt->sgtt_GadgetID, NULL);
	else
		return 0;
}


static ULONG IconWindowClass_RawKey(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RawKey *mrk = (struct msg_RawKey *) msg;
	LONG    hs, vs;		// +dm+ 20011106 Window scroll distances

	d1(kprintf("%s/%s/%ld: Code=%04lx  Qualifier=%04lx\n", __FILE__, __FUNC__, __LINE__, mrk->mrk_iMsg->Code, mrk->mrk_iMsg->Qualifier));

	ResetToolTips(iwt);

	switch (mrk->mrk_iMsg->Code)
		{
	case CURSORUP:				// up
	case CURSORDOWN:			// down
	case CURSORRIGHT:			// right
	case CURSORLEFT:			// left
		if (0 == ClassCountSelectedIcons(iwt))
			{
			// Scroll window with arrow keys
			if (!iwt->iwt_BackDrop)
				{
				// +dm+ 20011106
				hs = CurrentPrefs.pref_WindowHScroll;
				vs = CurrentPrefs.pref_WindowVScroll;

				if(mrk->mrk_iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
					{
					hs = iwt->iwt_InnerWidth;
					vs = iwt->iwt_InnerHeight;
					}
				// +dm+ 20011106

				switch (mrk->mrk_iMsg->Code)
					{			
				case CURSORUP:				// up
					ScrollWindowXY(iwt, 0, -vs);
					break;
				case CURSORDOWN:			// down
					ScrollWindowXY(iwt, 0, vs);
					break;
				case CURSORLEFT:			// left
					ScrollWindowXY(iwt, -hs, 0);
					break;
				case CURSORRIGHT:			// right
					ScrollWindowXY(iwt, hs, 0);
					break;
					}
				}
			}
		else
			{
			switch (mrk->mrk_iMsg->Code)
				{			
			case CURSORUP:
				DoMethod(o, SCCM_IconWin_ActivateIconUp);
				break;
			case CURSORDOWN:
				DoMethod(o, SCCM_IconWin_ActivateIconDown);
				break;
			case CURSORLEFT:
				DoMethod(o, SCCM_IconWin_ActivateIconLeft);
				break;
			case CURSORRIGHT:
				DoMethod(o, SCCM_IconWin_ActivateIconRight);
				break;
				}
			}
		break;

	case 0x63:				// Control down
	case 0x63 | IECODE_UP_PREFIX:		// Control up
		if (VGADGETID_DRAGMOUSEMOVE == iwt->iwt_MoveGadId)
			IDCMPDragMouseMove(iwt, mrk->mrk_iMsg);
		break;

	case 0x46:				// Del
		d1(KPrintF("%s/%s/%ld: <Del>  IconActive=%ld\n", __FILE__, __FUNC__, __LINE__, IconActive));
		if (IconActive)
			{
			struct MenuCmdArg mca = { NULL, NULL, 0 };

			DeleteProg(iwt, &mca);
			}
		break;

	case 0x44:				// Return
		if (IconActive && !(mrk->mrk_iMsg->Qualifier & 
			(IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT | IEQUALIFIER_CONTROL | 
			IEQUALIFIER_LALT | IEQUALIFIER_RALT | IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND)))
			{
			//struct msg_Open *mop = (struct msg_Open *) msg;
			d1(kprintf("%s/%s/%ld: <Return>\n", __FILE__, __FUNC__, __LINE__));
			IconDoubleClick(iwt, FindFirstSelectedIcon(iwt), 0L);
			}
		break;

	case 0x42:				// Tab
		if (mrk->mrk_iMsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
			{
			DoMethod(o, SCCM_IconWin_ActivateIconPrevious);
			}
		else
			{
			DoMethod(o, SCCM_IconWin_ActivateIconNext);
			}
		break;

	case NM_WHEEL_UP:
	case NM_WHEEL_DOWN:
	case NM_WHEEL_LEFT:
	case NM_WHEEL_RIGHT:
		if (!iwt->iwt_BackDrop)
			{
			d1(KPrintF("%s/%s/%ld: Code=%ld  MouseX=%ld  MouseY=%ld\n", __FILE__, __FUNC__, __LINE__, mrk->mrk_iMsg->Code, mrk->mrk_iMsg->MouseX, mrk->mrk_iMsg->MouseY));

			if (mrk->mrk_iMsg->MouseX >= iwt->iwt_InnerLeft
				&& mrk->mrk_iMsg->MouseX < (iwt->iwt_InnerLeft + iwt->iwt_InnerWidth)
				&& mrk->mrk_iMsg->MouseY >= iwt->iwt_InnerTop
				&& mrk->mrk_iMsg->MouseY < (iwt->iwt_InnerTop + iwt->iwt_InnerHeight))
				{
				// If mouse is inside window scrolling area, scroll window
				d1(KPrintF("%s/%s/%ld: Code=%ld  MouseX=%ld  MouseY=%ld\n", __FILE__, __FUNC__, __LINE__, mrk->mrk_iMsg->Code, mrk->mrk_iMsg->MouseX, mrk->mrk_iMsg->MouseY));

				switch (mrk->mrk_iMsg->Code)
					{			
				case NM_WHEEL_UP:
					ScrollWindowXY(iwt, 0, -20);
					break;
				case NM_WHEEL_DOWN:
					ScrollWindowXY(iwt, 0, 20);
					break;
				case NM_WHEEL_LEFT:
					ScrollWindowXY(iwt, -20, 0);
					break;
				case NM_WHEEL_RIGHT:
					ScrollWindowXY(iwt, 20, 0);
					break;
					}
				}
			else if (PointInGadget(mrk->mrk_iMsg->MouseX, mrk->mrk_iMsg->MouseY,
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Window, iwt->iwt_ControlBar))
				{
				// mouse is over control bar
				// let controlbar check if any embedded gadget wants to react to our mouse wheel event
				ULONG Dummy;
				struct InputEvent ie;

				d1(KPrintF("%s/%s/%ld: Code=%ld  MouseX=%ld  MouseY=%ld\n", __FILE__, __FUNC__, __LINE__, mrk->mrk_iMsg->Code, mrk->mrk_iMsg->MouseX, mrk->mrk_iMsg->MouseY));

				ie.ie_NextEvent = NULL;
				ie.ie_Class = mrk->mrk_iMsg->Class;
				ie.ie_SubClass = 0;
				ie.ie_Code = mrk->mrk_iMsg->Code;
				ie.ie_Qualifier = mrk->mrk_iMsg->Qualifier;
				ie.ie_position.ie_xy.ie_x = mrk->mrk_iMsg->MouseX;
				ie.ie_position.ie_xy.ie_y = mrk->mrk_iMsg->MouseY;
				ie.ie_TimeStamp.tv_secs = mrk->mrk_iMsg->Seconds;
				ie.ie_TimeStamp.tv_micro = mrk->mrk_iMsg->Micros;

				DoGadgetMethod(iwt->iwt_ControlBar,
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Window,
					NULL,
					GBCL_HANDLEMOUSEWHEEL,
					NULL,		// GadgetInfo is provided by DoGadgetMethod
					&ie,
					&Dummy,
					SCCM_ADDICON_MAKEXY(mrk->mrk_iMsg->MouseX - iwt->iwt_ControlBar->LeftEdge,
						mrk->mrk_iMsg->MouseY - iwt->iwt_ControlBar->TopEdge),
					NULL);
				}
			else if (PointInGadget(mrk->mrk_iMsg->MouseX, mrk->mrk_iMsg->MouseY,
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Window, iwt->iwt_PropBottom))
				{
				// if mouse is over horizontal scroller, scroll window left/right
				switch (mrk->mrk_iMsg->Code)
					{
				case NM_WHEEL_UP:
				case NM_WHEEL_LEFT:
					ScrollWindowXY(iwt, -20, 0);
					break;
				case NM_WHEEL_DOWN:
				case NM_WHEEL_RIGHT:
					ScrollWindowXY(iwt, 20, 0);
					break;
					}
				}
			else if (PointInGadget(mrk->mrk_iMsg->MouseX, mrk->mrk_iMsg->MouseY,
					iwt->iwt_WindowTask.mt_WindowStruct->ws_Window, iwt->iwt_PropSide))
				{
				// if mouse is over vertical scroller, scroll window up/down
				switch (mrk->mrk_iMsg->Code)
					{
				case NM_WHEEL_UP:
				case NM_WHEEL_LEFT:
					ScrollWindowXY(iwt, 0, -20);
					break;
				case NM_WHEEL_DOWN:
				case NM_WHEEL_RIGHT:
					ScrollWindowXY(iwt, 0, 20);
					break;
					}
				}
			}
		break;

	default:
		{
		char KeyBuffer[20];
		struct InputEvent ie;
		LONG Actual;

		d1(kprintf("%s/%s/%ld: Code=%04lx  Qualifier=%04lx\n",
			__FILE__, __FUNC__, __LINE__, mrk->mrk_iMsg->Code, mrk->mrk_iMsg->Qualifier));

		ie.ie_NextEvent = NULL;
		ie.ie_Class = IECLASS_RAWKEY;
		ie.ie_SubClass = 0;
		ie.ie_Code = mrk->mrk_iMsg->Code;
		ie.ie_Qualifier = mrk->mrk_iMsg->Qualifier;
		memset(&ie.ie_position.ie_dead, 0, sizeof(ie.ie_position.ie_dead));

		ie.ie_TimeStamp.tv_secs = mrk->mrk_iMsg->Seconds;
		ie.ie_TimeStamp.tv_micro = mrk->mrk_iMsg->Micros;

		Actual = RawKeyConvert(&ie, KeyBuffer, sizeof(KeyBuffer) - 1, NULL);

		if (Actual > 0)
			{
			ULONG fTyping = FALSE;

			KeyBuffer[Actual] = '\0';

			d1(kprintf("%s/%s/%ld: Actual=%ld  KeyBuffer=<%s>\n", __FILE__, __FUNC__, __LINE__, Actual, KeyBuffer));

			GetAttr(SCCA_IconWin_Typing, o, &fTyping);
			if (fTyping)
				{
				SafeStrCat(iwt->iwt_TypingBuffer, KeyBuffer, sizeof(iwt->iwt_TypingBuffer));
				}
			else
				{
				SetAttrs(o, SCCA_IconWin_Typing, TRUE, TAG_END);
				iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags |= WSV_FlagF_Typing;

				stccpy(iwt->iwt_TypingBuffer, KeyBuffer, sizeof(iwt->iwt_TypingBuffer));
				}

			iwt->iwt_TypeRestartTimer = CurrentPrefs.pref_TypeRestartTime;

			d1(kprintf("%s/%s/%ld: iwt_TypingBuffer=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_TypingBuffer));

			SelectIconByName(iwt, iwt->iwt_TypingBuffer);
			}

		ie.ie_position.ie_dead.ie_prev2DownQual = ie.ie_position.ie_dead.ie_prev1DownQual;
		ie.ie_position.ie_dead.ie_prev2DownCode = ie.ie_position.ie_dead.ie_prev1DownCode;
		ie.ie_position.ie_dead.ie_prev1DownQual = ie.ie_Qualifier;
		ie.ie_position.ie_dead.ie_prev1DownCode = ie.ie_Code;

		iwt->iwt_LastInputEvent = ie;
		}
		break;
		}

	SetMenuOnOff(iwt);

	return 0;
}


static ULONG IconWindowClass_DragBegin(Class *cl, Object *o, Msg msg)
{
//	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
//	struct msg_DragBegin *mdb = (struct msg_DragBegin *) msg;

	return 0;
}


static ULONG IconWindowClass_DragQuery(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DragBegin *mdb = (struct msg_DragBegin *) msg;

	return ClassDragQuery(&mdb->mdb_DragEnter, iwt);
}


static ULONG IconWindowClass_DragFinish(Class *cl, Object *o, Msg msg)
{
//	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
//	struct msg_DragBegin *mdb = (struct msg_DragBegin *) msg;

	return 0;
}


static ULONG IconWindowClass_DragEnter(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DragBegin *mdb = (struct msg_DragBegin *) msg;

	return ClassDragEnter(&mdb->mdb_DragEnter, iwt);
}


static ULONG IconWindowClass_DragLeave(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DragBegin *mdb = (struct msg_DragBegin *) msg;

	return ClassDragLeave(&mdb->mdb_DragEnter, iwt);
}


static ULONG IconWindowClass_DragDrop(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DragDrop *mdd = (struct msg_DragDrop *) msg;

	DragDrop(mdd->mdd_DropWindow, 
		mdd->mdd_MouseX, mdd->mdd_MouseY, 
		mdd->mdd_Qualifier,
		iwt);

	return 0;
}


static ULONG IconWindowClass_Set(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct opSet *ops = (struct opSet *) msg;
	struct TagItem *TagList = ops->ops_AttrList;
	struct TagItem *ti;
	BOOL RequiresUpdate = FALSE;

	while ((ti = NextTagItem(&TagList)))
		{
		d1(kprintf("%s/%s/%ld: ti=%08lx  Tag=%08lx  Data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti, ti->ti_Tag, ti->ti_Data));

		switch (ti->ti_Tag)
			{
		case SCCA_IconWin_IconScaleFactor:
			if (iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor != ti->ti_Data)
				{
				iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor = ti->ti_Data;
				RequiresUpdate = TRUE;
				}
			break;

		case SCCA_IconWin_IconSizeConstraints:
			if (0 != memcmp(&iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
				(struct Rectangle *) ti->ti_Data,
				sizeof(iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints)))
				{
				iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints = *((struct Rectangle *) ti->ti_Data);
				RequiresUpdate = TRUE;
				}
			break;

		case SCCA_IconWin_ActiveTransparency:
			if (iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityActive != ti->ti_Data)
				{
				iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityActive = ti->ti_Data;
				IconWindowUpdateTransparency(iwt);
				}
			break;

		case SCCA_IconWin_InactiveTransparency:
			if (iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityInactive != ti->ti_Data)
				{
				iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityInactive = ti->ti_Data;
				IconWindowUpdateTransparency(iwt);
				}
			break;

		case SCCA_IconWin_ShowType:
			SetShowType(iwt, ti->ti_Data);
			break;

		case SCCA_IconWin_Reading:
			if (iwt->iwt_Reading != (UBYTE) (ti->ti_Data != 0))
				{
				iwt->iwt_Reading = (UBYTE) (ti->ti_Data != 0);
				if (iwt->iwt_StatusBar)
					{
					if (iwt->iwt_Reading)
						{
						if (NULL == iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading])
							{
							iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarImage.sca",
								DTA_Name, (ULONG) "THEME:Window/StatusBar/Reading",
								GBIDTA_WindowTask, (ULONG) iwt,
								GA_ID, SBAR_GadgetID_Reading,
								TAG_END);

							if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading])
								{
								DoMethod(o,
									SCCM_IconWin_AddToStatusBar,
									iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading], TAG_END);
								}
							}
						}
					else
						{
						if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading])
							{
							DoMethod(o,
								SCCM_IconWin_RemFromStatusBar,
								iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading]);
							iwt->iwt_StatusBarMembers[STATUSBARGADGET_Reading] = NULL;
							}
						}
					}
				}
			break;

		case SCCA_IconWin_Typing:
			if (iwt->iwt_Typing != (UBYTE) (ti->ti_Data != 0))
				{
				iwt->iwt_Typing = (UBYTE) (ti->ti_Data != 0);
				if (iwt->iwt_StatusBar)
					{
					if (iwt->iwt_Typing)
						{
						if (NULL == iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing])
							{
							iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarImage.sca",
								DTA_Name, (ULONG) "THEME:Window/StatusBar/Typing",
								GBIDTA_WindowTask, (ULONG) iwt,
								GA_ID, SBAR_GadgetID_Typing,
								TAG_END);

							if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing])
								{
								DoMethod(o,
									SCCM_IconWin_AddToStatusBar,
									iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing], TAG_END);
								}
							}
						}
					else
						{
						if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing])
							{
							DoMethod(o,
								SCCM_IconWin_RemFromStatusBar,
								iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing]);
							iwt->iwt_StatusBarMembers[STATUSBARGADGET_Typing] = NULL;
							}
						}
					}
				}
			break;

		case SCCA_IconWin_StatusBar:
			{
			struct SM_ShowStatusBar *smsb;

			d1(kprintf("%s/%s/%ld: SCCA_IconWin_StatusBar  Data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			smsb = (struct SM_ShowStatusBar *) SCA_AllocMessage(MTYP_ShowStatusBar, 0);
			d1(kprintf("%s/%s/%ld: smsb=%08lx\n", __FILE__, __FUNC__, __LINE__, smsb));
			if (smsb)
				{
				smsb->smsb_Visible = ti->ti_Data;
				PutMsg(iwt->iwt_WindowTask.wt_IconPort, &smsb->ScalosMessage.sm_Message);
				}
			}
			break;

		case SCCA_IconWin_ControlBar:
			{
			struct SM_ShowControlBar *smcb;

			d1(kprintf("%s/%s/%ld: SCCA_IconWin_ControlBar  Data=%08lx\n", __FILE__, __FUNC__, __LINE__, ti->ti_Data));
			d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

			smcb = (struct SM_ShowControlBar *) SCA_AllocMessage(MTYP_ShowControlBar, 0);
			d1(kprintf("%s/%s/%ld: smsb=%08lx\n", __FILE__, __FUNC__, __LINE__, smcb));
			if (smcb)
				{
				smcb->smcb_Visible = ti->ti_Data;
				PutMsg(iwt->iwt_WindowTask.wt_IconPort, &smcb->ScalosMessage.sm_Message);
				}
			}
			break;

		case SCCA_IconWin_ThumbnailsGenerating:
			SetThumbnailsGenerating(iwt, ti->ti_Data);
			break;

		case SCCA_IconWin_ThumbnailView:
			SetThumbnailView(iwt, ti->ti_Data);
			break;
			}
		}

	if (RequiresUpdate)
		{
		struct SM_Update *smu;

		smu = (struct SM_Update *) SCA_AllocMessage(MTYP_Update, 0);
		d1(kprintf("%s/%s/%ld: smu=%08lx\n", __FILE__, __FUNC__, __LINE__, smu));
		if (smu)
			{
			PutMsg(iwt->iwt_WindowTask.wt_IconPort, &smu->ScalosMessage.sm_Message);
			}
		}

	return 0;
}


static ULONG IconWindowClass_Get(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct opGet *opg = (struct opGet *) msg;

	switch (opg->opg_AttrID)
		{
	case SCCA_IconWin_IconSizeConstraints:
		*opg->opg_Storage = (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints;
		break;

	case SCCA_IconWin_IconScaleFactor:
		*opg->opg_Storage = iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor;
		break;

	case SCCA_IconWin_ActiveTransparency:
		*opg->opg_Storage = iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityActive;
		break;

	case SCCA_IconWin_InactiveTransparency:
		*opg->opg_Storage = iwt->iwt_WindowTask.mt_WindowStruct->ws_WindowOpacityInactive;
		break;

	case SCCA_IconWin_ShowType:
		*opg->opg_Storage = iwt->iwt_WindowTask.mt_WindowStruct->ws_ViewAll;
		break;

	case SCCA_IconWin_Reading:
		*opg->opg_Storage = iwt->iwt_Reading;
		break;

	case SCCA_IconWin_Typing:
		*opg->opg_Storage = iwt->iwt_Typing;
		break;

	case SCCA_IconWin_InnerWidth:
		*opg->opg_Storage = iwt->iwt_InnerWidth;
		break;

	case SCCA_IconWin_InnerHeight:
		*opg->opg_Storage = iwt->iwt_InnerHeight;
		break;

	case SCCA_IconWin_InnerLeft:
		*opg->opg_Storage = iwt->iwt_InnerLeft;
		break;

	case SCCA_IconWin_InnerTop:
		*opg->opg_Storage = iwt->iwt_InnerTop;
		break;

	case SCCA_IconWin_InnerRight:
		*opg->opg_Storage = iwt->iwt_InnerRight;
		break;

	case SCCA_IconWin_InnerBottom:
		*opg->opg_Storage = iwt->iwt_InnerBottom;
		break;

	case SCCA_IconWin_IconFont:
		*opg->opg_Storage = (ULONG) iwt->iwt_IconFont;
		break;
	
	case SCCA_IconWin_LayersLocked:
		*opg->opg_Storage = (ULONG) iwt->iwt_LockFlag;
		break;

	case SCCA_IconWin_ThumbnailView:
		*opg->opg_Storage = (ULONG) iwt->iwt_ThumbnailMode;
		break;

	case SCCA_IconWin_ThumbnailsGenerating:
		*opg->opg_Storage = (ULONG) iwt->iwt_ThumbnailGenerationPending;
		break;

	case SCCA_IconWin_ControlBar:
		*opg->opg_Storage = (ULONG) !(iwt->iwt_WindowTask.mt_WindowStruct->ws_MoreFlags & WSV_MoreFlagF_NoControlBar);
		break;

	default:
		DoSuperMethodA(cl, o, msg);
		break;
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_New(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt;

	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (o)	
		{
		ULONG n;
		//struct IconWindowClassInstance *inst = INST_DATA(cl, o);

		iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

		iwt->iwt_IconFont = IconWindowFont;
		iwt->iwt_IconTTFont = IconWindowTTFont;

		iwt->iwt_TextGadFlag = FALSE;
		iwt->iwt_TextWindowGadgetHeight = 0;

		iwt->iwt_IconUnderPtr = NULL;
		iwt->iwt_WinUnderPtr = NULL;

		// initialize iwt_IconLayoutInfo
		memset(&iwt->iwt_IconLayoutInfo.ili_LayoutArea, 0, sizeof(iwt->iwt_IconLayoutInfo.ili_LayoutArea));
		memset(&iwt->iwt_IconLayoutInfo.ili_Restricted, 0, sizeof(iwt->iwt_IconLayoutInfo.ili_Restricted));

		for (n = 0; n < Sizeof(iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes); n++)
			{
			if (n < Sizeof(CurrentPrefs.pref_IconWindowLayoutModes))
				iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes[n] = CurrentPrefs.pref_IconWindowLayoutModes[n];
			else
				iwt->iwt_IconLayoutInfo.ili_IconTypeLayoutModes[n] = ICONLAYOUT_Columns; //default
			}

		DoMethod(o, SCCM_IconWin_StartNotify);
		}

	return (ULONG) o;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Dispose(Class *cl, Object *o, Msg msg)
{
	struct IconWindowClassInstance *inst = INST_DATA(cl, o);

	if (inst->ici_NotifyReq)
		{
		ScalosEndNotify(inst->ici_NotifyReq);

		ScalosFree(inst->ici_NotifyReq);
		inst->ici_NotifyReq = NULL;
		}
	if (inst->ici_DrawerName)
		{
		FreePathBuffer(inst->ici_DrawerName);
		inst->ici_DrawerName = NULL;
		}

	return DoSuperMethodA(cl, o, msg);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Open(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Open *mop = (struct msg_Open *) msg;

	d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>  mop_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		mop->mop_IconNode, GetIconName(mop->mop_IconNode), mop->mop_Flags));

	return (ULONG) IconDoubleClick(iwt, mop->mop_IconNode, mop->mop_Flags);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_UpdateIcon(Class *cl, Object *o, Msg msg)
{
	struct msg_RemIcon *mri = (struct msg_RemIcon *) msg;

	return DoMethod(o, SCCM_IconWin_UpdateIconTags, mri->mri_Lock, mri->mri_Name, TAG_END);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_UpdateIconTags(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_UpdateIconTags *muit = (struct msg_UpdateIconTags *) msg;
	struct DateStamp Now;
	BOOL UpdateNow = TRUE;


	d1(KPrintF("%s/%s/%ld: START Name=<%s>\n", __FILE__, __FUNC__, __LINE__, muit->muit_Name));
	debugLock_d1(muit->muit_Lock);

	DateStamp(&Now);
	SubtractDateStamp(&Now, &iwt->iwt_LastIconUpdateTime);

	d1(KPrintF("%s/%s/%ld: Now: Days=%ld  Minute=%ld  Tick=%ld\n", __FILE__, __FUNC__, __LINE__, \
		Now.ds_Days, Now.ds_Minute, Now.ds_Tick));

	if (0 == Now.ds_Days && 0 == Now.ds_Minute && Now.ds_Tick < TICKS_PER_SECOND / 2)
		{
		if (++iwt->iwt_IconUpdateCount > MAX_ICON_UPDATE_COUNT)
			{
			d1(KPrintF("%s/%s/%ld: schedule delayed update\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ScheduleUpdate);
			DateStamp(&iwt->iwt_LastIconUpdateTime);

			UpdateNow = FALSE;
			}
		}
	
	if (UpdateNow)
		{
		if (BNULL == iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock ||
			LOCK_SAME == ScaSameLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, muit->muit_Lock))
			{
			struct SM_RealUpdateIcon *rui = (struct SM_RealUpdateIcon *) 
				SCA_AllocMessage(MTYP_AsyncRoutine, 
					sizeof(struct SM_RealUpdateIcon) - sizeof(struct ScalosMessage));

			if (rui)
				{
				rui->rui_Args.uid_IconType = GetTagData(UPDATEICON_IconType, ICONTYPE_NONE, (struct TagItem *) &muit->muit_TagList);
				rui->rui_AsyncRoutine.smar_EntryPoint = (ASYNCROUTINEFUNC) RealUpdateIcon;
				if (muit->muit_Lock)
					rui->rui_Args.uid_WBArg.wa_Lock = DupLock(muit->muit_Lock);
				if (muit->muit_Name)
					rui->rui_Args.uid_WBArg.wa_Name = AllocCopyString(muit->muit_Name);

				d1(kprintf("%s/%s/%ld: Args=%08lx\n", __FILE__, __FUNC__, __LINE__, &rui->rui_Args));
				d1(kprintf("%s/%s/%ld: uid_IconType=%lu\n", __FILE__, __FUNC__, __LINE__, rui->rui_Args.uid_IconType));
				d1(kprintf("%s/%s/%ld: PutMsg iwt=%08lx  Msg=%08lx \n", __FILE__, __FUNC__, __LINE__, iwt, &rui->rui_AsyncRoutine.ScalosMessage.sm_Message));

				PutMsg(iwt->iwt_WindowTask.wt_IconPort, &rui->rui_AsyncRoutine.ScalosMessage.sm_Message);
				}
			}
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_AddIcon(Class *cl, Object *o, Msg msg)
{
	//struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_AddIcon *mai = (struct msg_AddIcon *) msg;
	BPTR oldDir;
	struct ScaReadIconArg ria;
	struct ScaIconNode *in;

	if (NULL == mai->mai_Name || strlen(mai->mai_Name) < 1)
		return 0;

	oldDir = CurrentDir(mai->mai_Lock);

	ria.ria_x = mai->mai_x;
	ria.ria_y = mai->mai_y;
	ria.ria_Lock = mai->mai_Lock;
	ria.ria_IconType = ICONTYPE_NONE;

	d1(KPrintF("%s/%s/%ld: Name=<%s>  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, mai->mai_Name, mai->mai_x, mai->mai_y));
	debugLock_d1(ria.ria_Lock);

	in = (struct ScaIconNode *) DoMethod(o,
		SCCM_IconWin_ReadIcon,
		mai->mai_Name, &ria);

	d1(kprintf("%s/%s/%ld: in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

	CurrentDir(oldDir);

	return (ULONG) in;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_RemIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RemIcon *mri = (struct msg_RemIcon *) msg;
	struct WBArg remArg;

	d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, mri->mri_Name));
	debugLock_d1(mri->mri_Lock);

	remArg.wa_Lock = mri->mri_Lock;
	remArg.wa_Name = (STRPTR) mri->mri_Name;

	IconWin_RemIcon(iwt, &remArg);

	return 1;
}

//----------------------------------------------------------------------------

static void IconWin_RemIcon(struct internalScaWindowTask *iwt, struct WBArg *IconArg)
{
	struct ScaIconNode *in, *inNext;
	BOOL Found = FALSE;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  lock=%08lx  IconName=<%s>\n", \
		__FILE__, __FUNC__, __LINE__, iwt, IconArg->wa_Lock, IconArg->wa_Name));

	if (!IsViewByIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes))
		return;

	ScalosLockIconListExclusive(iwt);

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in=inNext)
		{
		d1(KPrintF("%s/%s/%ld: in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

		if (in->in_Name && 0 == Stricmp(IconArg->wa_Name, in->in_Name))
			{
			struct ScaIconNode *dummyIconList = NULL;

			d1(KPrintF("%s/%s/%ld: FOUND in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

			Found = TRUE;

			SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &dummyIconList, in);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			RemoveIcons(iwt, &dummyIconList);	// visually erase icons

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			debugLock_d1(in->in_Lock);
			if (in->in_Lock)
				RewriteBackdrop(in);

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			FreeIconNode(iwt, &dummyIconList, in);
			break;
			}
		}

	ScalosUnLockIconList(iwt);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (Found)
		{
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_MakeWbArg(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_MakeWbArg *mwa = (struct msg_MakeWbArg *) msg;
	ULONG Count = 0;

	mwa->mwa_Buffer->wa_Name = NULL;
	mwa->mwa_Buffer->wa_Lock = BNULL;

	d1(kprintf("%s/%s/%ld: Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, mwa->mwa_Icon));

	if (mwa->mwa_Icon)
		{
		ULONG IconType;

		GetAttr(IDTA_Type, mwa->mwa_Icon->in_Icon, &IconType);

		d1(kprintf("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));

		switch (IconType)
			{
		case WBAPPICON:
			return Count;
			break;

		case WBTOOL:
		case WBPROJECT:
		case WB_TEXTICON_TOOL:
			if (mwa->mwa_Icon->in_Lock)
				mwa->mwa_Buffer->wa_Lock = DupLock(mwa->mwa_Icon->in_Lock);
			else
				{
				if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
					mwa->mwa_Buffer->wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
				}

			mwa->mwa_Buffer->wa_Name = (STRPTR) GetIconName(mwa->mwa_Icon);

			Count = 1;
			break;

		default:
			if (mwa->mwa_Icon->in_DeviceIcon)
				{
				// Device/Volume
				mwa->mwa_Buffer->wa_Lock = DiskInfoLock(mwa->mwa_Icon);

				if (mwa->mwa_Buffer->wa_Lock)
					{
					Count = 1;
					}
				else
					{
					mwa->mwa_Buffer->wa_Name = mwa->mwa_Icon->in_DeviceIcon->di_Device;

					Count = 1;
					}
				}
			else
				{
				// Drawer
				if (mwa->mwa_Icon->in_Lock)
					mwa->mwa_Buffer->wa_Lock = DupLock(mwa->mwa_Icon->in_Lock);
				else
					mwa->mwa_Buffer->wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

				mwa->mwa_Buffer->wa_Name = (STRPTR) GetIconName(mwa->mwa_Icon);
//				mwa->mwa_Buffer->wa_Lock = Lock(mwa->mwa_Icon->in_Name, ACCESS_READ);

				if (mwa->mwa_Buffer->wa_Lock)
					{
					Count = 1;
					}
				else
					{
					Count = 0;
					}
				}
			break;
			}
		}
	else
		{
		// if no icon, return duplicate of window lock and name = NULL
		mwa->mwa_Buffer->wa_Lock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

		Count = 1;
		}

	d1(kprintf("%s/%s/%ld: Count=%ld\n", __FILE__, __FUNC__, __LINE__, Count));

	if (Count)
		{
		mwa->mwa_Buffer->wa_Name = AllocCopyString(mwa->mwa_Buffer->wa_Name);	// replaces NULL by ""

		if (NULL == mwa->mwa_Buffer->wa_Name)
			{
			if (mwa->mwa_Buffer->wa_Lock)
				{
				UnLock(mwa->mwa_Buffer->wa_Lock);
				mwa->mwa_Buffer->wa_Lock = BNULL;
				}
			Count = 0;
			}

		if (Count)
			{
			d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, mwa->mwa_Buffer->wa_Name ? mwa->mwa_Buffer->wa_Name : ""));
			debugLock_d1(mwa->mwa_Buffer->wa_Lock);
			mwa->mwa_Buffer++;
			}
		}

	d1(kprintf("%s/%s/%ld: Count=%ld\n", __FILE__, __FUNC__, __LINE__, Count));

	return Count;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_CountWbArg(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_CountWbArg *mca = (struct msg_CountWbArg *) msg;
	ULONG Count = 0;

	d1(KPrintF("%s/%s/%ld: Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, mca->mca_Icon));

	if (mca->mca_Icon)
		{
		ULONG IconType;

		GetAttr(IDTA_Type, mca->mca_Icon->in_Icon, &IconType);

		d1(kprintf("%s/%s/%ld: IconType=%ld\n", __FILE__, __FUNC__, __LINE__, IconType));

		switch (IconType)
			{
		case WBAPPICON:
			return Count;
			break;

		case WBTOOL:
		case WBPROJECT:
		case WB_TEXTICON_TOOL:
			Count = 1;
			break;

		default:
			if (mca->mca_Icon->in_DeviceIcon)
				{
				// Device/Volume
				Count = 1;
				}
			else
				{
				// Drawer
				BPTR DirLock;

				if (mca->mca_Icon->in_Lock)
					DirLock = mca->mca_Icon->in_Lock;
				else
					DirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

				if (DirLock)
					Count = 1;
				}
			break;
			}
		}
	else
		{
		Count = 1;
		}

	d1(kprintf("%s/%s/%ld: Count=%ld\n", __FILE__, __FUNC__, __LINE__, Count));

	return Count;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_SetVirtSize(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_SetVirtSize *msv = (struct msg_SetVirtSize *) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SetVirtSize(iwt, msv->msv_Flags);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	SetMenuOnOff(iwt);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Redraw(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Redraw *mrd = (struct msg_Redraw *) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	RedrawIconWindow(iwt, mrd->mrd_Flags);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Sleep(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaWindowStruct *ws;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 1;

	ws = iwt->iwt_WindowTask.mt_WindowStruct;

	if (ws == iInfos.xii_iinfos.ii_MainWindowStruct && !iwt->iwt_BackDrop)
		{
		CurrentPrefs.pref_WBWindowBox.Left = iwt->iwt_WindowTask.wt_Window->LeftEdge;
		CurrentPrefs.pref_WBWindowBox.Top = iwt->iwt_WindowTask.wt_Window->TopEdge;
		CurrentPrefs.pref_WBWindowBox.Width = iwt->iwt_WindowTask.wt_Window->Width;
		CurrentPrefs.pref_WBWindowBox.Height = iwt->iwt_WindowTask.wt_Window->Height;

		CurrentPrefs.pref_WBWindowXOffset = iwt->iwt_WindowTask.wt_XOffset;
		CurrentPrefs.pref_WBWindowYOffset = iwt->iwt_WindowTask.wt_YOffset;
		}

	if (iwt->iwt_Reading)
		{
		struct Region *oldClipRegion = iwt->iwt_RemRegion;

		iwt->iwt_RemRegion = NULL;
		d1(kprintf("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));

		DisposeRegion(InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, oldClipRegion));
		d1(kprintf("%s/%s/%ld: oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, oldClipRegion));
		}

	ws->ws_Left = iwt->iwt_WindowTask.wt_Window->LeftEdge;
	ws->ws_Top = iwt->iwt_WindowTask.wt_Window->TopEdge;
	ws->ws_Width = iwt->iwt_WindowTask.wt_Window->Width - iwt->iwt_ExtraWidth;
	ws->ws_Height = iwt->iwt_WindowTask.wt_Window->Height - iwt->iwt_ExtraHeight;

	ws->ws_Window = NULL;
	ws->ws_Flags |= WSV_FlagF_TaskSleeps;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Close);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Wakeup(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_Wakeup *mwu = (struct msg_Wakeup *) msg;
	struct ScaWindowStruct *ws;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	iwt->iwt_WinScreen = iInfos.xii_iinfos.ii_Screen;
	iwt->iwt_WinDrawInfo = iInfos.xii_iinfos.ii_DrawInfo;

	if (iwt->iwt_IconifyFlag)
		return 0;

	if (iwt->iwt_StatusBar)
		{
		SetAttrs(iwt->iwt_StatusBar,
			GBDTA_BGPen, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_BG],
			TAG_END);
		}
	if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText])
		{
		SetAttrs(iwt->iwt_StatusBarMembers[STATUSBARGADGET_StatusText],
			// GBTDTA_TextFont, (ULONG) iwt->iwt_IconFont,
			GBTDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_STATUSBAR_TEXT],
			TAG_END);
		}

	if (NULL == (struct Window *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_Open))
		return 0;

	if (NULL == iwt->iwt_WindowTask.wt_IconList &&
		NULL == iwt->iwt_WindowTask.wt_LateIconList)
		{
		if (0 != DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update))
			return 0;
		}

	ws = iwt->iwt_WindowTask.mt_WindowStruct;

	d1(kprintf("%s/%s/%ld: wt_Window=%08lx  Relayout=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_Window, mwu->mwu_ReLayout));

	SafeSetMenuStrip(iwt->iwt_WindowTask.wt_Window);

	ws->ws_Window = iwt->iwt_WindowTask.wt_Window;
	ws->ws_Flags &= ~WSV_FlagF_TaskSleeps;

	Scalos_DoneRastPort(iwt->iwt_WindowTask.wt_Window->RPort);
	Scalos_SetFont(iwt->iwt_WindowTask.wt_Window->RPort, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);
	SetABPenDrMd(iwt->iwt_WindowTask.wt_Window->RPort, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawMode);

	ScalosLockIconListExclusive(iwt);

	d1(kprintf("%s/%s/%ld: wt_Window=%08lx  iwt_WinScreen=%08lx  iwt_WinDrawInfo=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_WindowTask.wt_Window, iwt->iwt_WinScreen, iwt->iwt_WinDrawInfo));

	if (mwu->mwu_ReLayout)
		{
		ReLayoutIcons(iwt, iwt->iwt_WindowTask.wt_IconList);
		ReLayoutIcons(iwt, iwt->iwt_WindowTask.wt_LateIconList);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	RefreshIcons(iwt, NULL);

	if (iwt->iwt_WindowTask.wt_LateIconList)
		{
		struct ScaIconNode *in;

		for (in=iwt->iwt_WindowTask.wt_LateIconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

			d1(kprintf("%s/%s/%ld: in=%08lx <%s>  in_Icon=%08lx  GadgetRender=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, in, GetIconName(in), in->in_Icon, gg->GadgetRender));

			if (NULL == gg->GadgetRender)
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_LayoutIcon,
					in->in_Icon,
					IOLAYOUTF_NormalImage);
				}
			}

		if (!iwt->iwt_Reading)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
			}
		}

	ScalosUnLockIconList(iwt);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
		SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

	SetMenuOnOff(iwt);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_DrawIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DrawIcon *mdi = (struct msg_DrawIcon *) msg;


	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	DrawIconObject(iwt, mdi->mdi_IconObject, 0L);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_LayoutIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_LayoutIcon *mli = (struct msg_LayoutIcon *) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  wt_Window=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, iwt->iwt_WindowTask.wt_Window));
	d1(KPrintF("%s/%s/%ld: mli_IconObject=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, mli->mli_IconObject));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	return DoMethod(mli->mli_IconObject,
		IDTM_Layout,
		iwt->iwt_WinScreen,
		iwt->iwt_WindowTask.wt_Window,
		iwt->iwt_WindowTask.wt_Window->RPort,
		iwt->iwt_WinDrawInfo,
		mli->mli_LayoutFlags);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Ping(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (!(iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked))
		{
		d1(kprintf("%s/%s/%ld: iwt=%08lx  TypeRestartTimer=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_TypeRestartTimer));

		// Try to complete postponed window refresh
		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RefreshPending)
			{
			d1(KPrintF("\n" "%s/%s/%ld: try to complete postponed refresh  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
			if (ScalosAttemptLockIconListShared(iwt))
				{
				d1(KPrintF("\n" "%s/%s/%ld: finish postponed refresh  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

				iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_RefreshPending;

				RefreshIcons(iwt, iwt->iwt_DamagedRegion);

				DisposeRegion(iwt->iwt_DamagedRegion);
				iwt->iwt_DamagedRegion = NULL;

				ScalosUnLockIconList(iwt);
				}
			}

		if (iwt->iwt_TypeRestartTimer && 0 == --iwt->iwt_TypeRestartTimer)
			{
			SetAttrs(o, SCCA_IconWin_Typing, FALSE, TAG_END);
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_Typing;
			}

		if (iwt->iwt_NotifyCounter && 0 == --iwt->iwt_NotifyCounter)
			{
			iwt->iwt_NotifyCounter = 0;

			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_CheckUpdatePending;

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
			}
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_GetDefIcon(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_GetDefIcon *mgd = (struct msg_GetDefIcon *) msg;
	Object *IconObj;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, mgd->mgd_Name));
	d1(KPrintF("%s/%s/%ld: mgd_IconType=%lu\n", __FILE__, __FUNC__, __LINE__, mgd->mgd_IconType));

	debugLock_d1(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

	IconObj = SCA_GetDefIconObjectTags(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
			mgd->mgd_Name,
			IDTA_Text, (ULONG) mgd->mgd_Name,
//			  DTA_Name, (ULONG) mgd->mgd_Name,
			IDTA_Type, mgd->mgd_IconType,
			IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
			IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
			IDTA_FrameType, CurrentPrefs.pref_FrameType,
			IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
			IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
			IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
			IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
			IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
			IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
			IDTA_TextMode, CurrentPrefs.pref_TextMode,
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
			IDTA_TextStyle, FS_NORMAL,
			IDTA_Font, (ULONG) iwt->iwt_IconFont,
			IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
			IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
			IDTA_SizeConstraints, (ULONG) &iwt->iwt_WindowTask.mt_WindowStruct->ws_IconSizeConstraints,
			IDTA_ScalePercentage, iwt->iwt_WindowTask.mt_WindowStruct->ws_IconScaleFactor,
			TAG_END);

	d1(KPrintF("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, mgd->mgd_Name));
	d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));

	if (IconObj)
		{
		struct ExtGadget *gg = (struct ExtGadget *) IconObj;

		// set DTA_Name separately after creation of IconObj
		// otherwise the taglist contains DTA_Name twice, first with deficon name,
		// and second with mgd->mgd_Name !
		SetAttrs(IconObj,
			DTA_Name, (ULONG) mgd->mgd_Name,
			TAG_END);

		gg->LeftEdge = gg->TopEdge = NO_ICON_POSITION_SHORT;
		}

	return (ULONG) IconObj;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_MenuCommand(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_MenuCommand *mcm = (struct msg_MenuCommand *) msg;
	struct ScalosMenuCommand *cmd;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	for (cmd = &MenuCommandTable[0]; cmd->smcom_CommandName; cmd++)
		{
		if (CompareCommand(cmd->smcom_CommandName, mcm->mcm_CmdName))
			{
			if (iInfos.xii_iinfos.ii_MainWindowStruct == iwt->iwt_WindowTask.mt_WindowStruct || !(cmd->smcom_Flags & SMCOMFLAGF_RunFromRootWindow))
				{
				struct MenuCmdArg mcArg;

				d1(kprintf("%s/%s/%ld: CmdName=<%s>  IconNode=%08lx  State=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, mcm->mcm_CmdName, mcm->mcm_IconNode, mcm->mcm_State));

				mcArg.mca_CommandName = mcm->mcm_CmdName;
				mcArg.mca_IconNode = mcm->mcm_IconNode;
				mcArg.mca_State = mcm->mcm_State;

				d1(KPrintF("%s/%s/%ld: cmd=%08lx  CommandName=<%s>  Routine=%08lx\n", \
					__FILE__, __FUNC__, __LINE__, cmd, cmd->smcom_CommandName, cmd->smcom_Routine));

				(*cmd->smcom_Routine)(iwt, &mcArg);

				if (iwt->iwt_CloseWindow)
					{
					struct SM_AsyncRoutine *msg = (struct SM_AsyncRoutine *) SCA_AllocMessage(MTYP_AsyncRoutine, 0);

					if (msg)
						{
						msg->smar_EntryPoint = (ASYNCROUTINEFUNC) CloseWindowMsg;

						d1(kprintf("%s/%s/%ld: PutMsg iwt=%08lx  Msg=%08lx \n", __FILE__, __FUNC__, __LINE__, iwt, &msg->ScalosMessage.sm_Message));
						PutMsg(iwt->iwt_WindowTask.wt_IconPort, &msg->ScalosMessage.sm_Message);
						}
					}
				}
			else
				{
				struct SM_MenuCmd *msg = (struct SM_MenuCmd *) SCA_AllocMessage(MTYP_AsyncRoutine,
					sizeof(struct SM_MenuCmd) - sizeof(struct ScalosMessage));

				if (msg)
					{
					msg->smc_AsyncRoutine.smar_EntryPoint = (ASYNCROUTINEFUNC) cmd->smcom_Routine;
					msg->smc_Flags = mcm->mcm_State;
					msg->smc_IconNode = mcm->mcm_IconNode;

					d1(kprintf("%s/%s/%ld: CmdName=<%s>  IconNode=%08lx  State=%08lx\n", \
						__FILE__, __FUNC__, __LINE__, mcm->mcm_CmdName, mcm->mcm_IconNode, mcm->mcm_State));

					d1(kprintf("%s/%s/%ld: PutMsg Msg=%08lx \n", __FILE__, __FUNC__, __LINE__, &msg->smc_AsyncRoutine.ScalosMessage.sm_Message));
					PutMsg(iInfos.xii_iinfos.ii_MainWindowStruct->ws_MessagePort,
						&msg->smc_AsyncRoutine.ScalosMessage.sm_Message);
					}
				}
			break;
			}
		}

	d1(KPrintF("%s/%s/%ld: finished.\n", __FILE__, __FUNC__, __LINE__));

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_NewViewMode(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_NewViewMode *mnv = (struct msg_NewViewMode *) msg;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	ULONG Result = 0;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (mnv->mnv_NewMode != ws->ws_Viewmodes)
		{
		if (IsViewByIcon(mnv->mnv_NewMode) != IsViewByIcon(ws->ws_Viewmodes))
			{
			// switching from text view to icon view or vice versa
			FlushThumbnailEntries(iwt);

			ScalosObtainSemaphore(&iwt->iwt_UpdateSemaphore);

			if (IsViewByIcon(mnv->mnv_NewMode))
				ws->ms_ClassName = (STRPTR) "IconWindow.sca";
			else
				ws->ms_ClassName = (STRPTR) "TextWindow.sca";

			Result = (ULONG) SCA_NewScalosObjectTags(ws->ms_ClassName,
					SCCA_WindowTask, (ULONG) iwt,
					TAG_END);

			ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
			}

		ws->ws_Viewmodes = mnv->mnv_NewMode;

		ControlBarUpdateViewMode(iwt, ws->ws_Viewmodes);
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_DeltaMove(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_DeltaMove *mdm = (struct msg_DeltaMove *) msg;
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ws = iwt->iwt_WindowTask.mt_WindowStruct;

	iwt->iwt_WindowTask.wt_XOffset += mdm->mdm_DeltaX;
	iwt->iwt_WindowTask.wt_YOffset += mdm->mdm_DeltaY;

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return 0;

	d1(KPrintF("%s/%s/%ld: DeltaX=%ld  DeltaY=%ld\n", __FILE__, __FUNC__, __LINE__, mdm->mdm_DeltaX, mdm->mdm_DeltaY));

	if (0 != mdm->mdm_DeltaX || 0 != mdm->mdm_DeltaY)
		{
		WORD MinX, MaxX, MinY, MaxY;

		ws->ws_xoffset = iwt->iwt_WindowTask.wt_XOffset;
		ws->ws_yoffset = iwt->iwt_WindowTask.wt_YOffset;

		MinX = MaxX = iwt->iwt_InnerLeft;
		MinY = MaxY = iwt->iwt_InnerTop;
		MaxX += iwt->iwt_InnerWidth - 1;
		MaxY += iwt->iwt_InnerHeight - 1;

		if (abs(mdm->mdm_DeltaX) >= iwt->iwt_InnerWidth / 2 ||
			abs(mdm->mdm_DeltaY) >= iwt->iwt_InnerHeight / 2)
			{
			// .dontmovesoft

			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort, MinX, MinY, MaxX, MaxY);

			ScalosLockIconListShared(iwt);

			RefreshIcons(iwt, NULL);

			ScalosUnLockIconList(iwt);
			}
		else
			{
			// .movesoft
			struct Region *clipRegion;

			ScrollWindowRaster(iwt->iwt_WindowTask.wt_Window,
				mdm->mdm_DeltaX, mdm->mdm_DeltaY,
				MinX, MinY, MaxX, MaxY);

			clipRegion = NewRegion();
			if (clipRegion)
				{
				struct Rectangle clipRect;

				d1(kprintf("%s/%s/%ld: wLeft=%ld  wTop=%ld  wWidth=%ld  wHeight=%ld\n", __FILE__, __FUNC__, __LINE__, \
					iwt->iwt_InnerLeft, iwt->iwt_InnerTop, iwt->iwt_InnerWidth, iwt->iwt_InnerHeight));

				d1(kprintf("%s/%s/%ld: DeltaX=%ld  DeltaY=%ld\n", __FILE__, __FUNC__, __LINE__, \
					mdm->mdm_DeltaX, mdm->mdm_DeltaY));

				if (mdm->mdm_DeltaX)
					{
					clipRect.MinX = MinX;
					clipRect.MaxX = MaxX;
					clipRect.MinY = MinY;
					clipRect.MaxY = MaxY;

					if (mdm->mdm_DeltaX >= 0)
						{
						// shift left
						clipRect.MinX = MaxX - mdm->mdm_DeltaX;
						}
					else
						{
						// shift right
						clipRect.MaxX = MinX - mdm->mdm_DeltaX;
						}

					d1(kprintf("%s/%s/%ld: clipRect MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
						clipRect.MinX, clipRect.MinY, clipRect.MaxX, clipRect.MaxY));

					OrRectRegion(clipRegion, &clipRect);
					}

				if (mdm->mdm_DeltaY)
					{
					clipRect.MinX = MinX;
					clipRect.MaxX = MaxX;
					clipRect.MinY = MinY;
					clipRect.MaxY = MaxY;

					if (mdm->mdm_DeltaY >= 0)
						{
						// shift Up
						clipRect.MinY = MaxY - mdm->mdm_DeltaY;
						}
					else
						{
						// shift down
						clipRect.MaxY = MinY - mdm->mdm_DeltaY;
						}

					d1(kprintf("%s/%s/%ld: clipRect MinX=%ld  MinY=%ld  MaxX=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
						clipRect.MinX, clipRect.MinY, clipRect.MaxX, clipRect.MaxY));

					OrRectRegion(clipRegion, &clipRect);
					}
				}

			if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
				{
				iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
				}
			else
				{
				struct Region *oldClipRegion = NULL;

				ScalosLockIconListShared(iwt);

				if (clipRegion)
					{
					oldClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, clipRegion);
					d1(kprintf("%s/%s/%ld: oldClipRegion=%08lx, clipRegion\n", __FILE__, __FUNC__, __LINE__, oldClipRegion, clipRegion));
					}

				RefreshIcons(iwt, clipRegion);

				if (clipRegion)
					{
					InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, oldClipRegion);
					d1(kprintf("%s/%s/%ld: oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, oldClipRegion));
					DisposeRegion(clipRegion);
					}

				ScalosUnLockIconList(iwt);
				}
			}
		}

	// the icon under the mouse pointer probably has changed by deltamove
	if (CurrentPrefs.pref_MarkIconUnderMouse)
		{
		d1(KPrintF("%s/%s/%ld: pref_MarkIconUnderMouse - CALL HighlightIconUnderMouse() function!\n", __FILE__, __FUNC__, __LINE__));
		HighlightIconUnderMouse();
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_UnCleanup(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (IsIwtViewByIcon(iwt) && iwt->iwt_WindowTask.wt_Window)
		{
		IconWindow_UnCleanup(iwt, NULL);
		}

	d1(KPrintF("%s/%s/%ld: Finished iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_UnCleanupRegion(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_UnCleanUpRegion *ucr = (struct msg_UnCleanUpRegion *) msg;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (IsIwtViewByIcon(iwt) && iwt->iwt_WindowTask.wt_Window)
		{
		IconWindow_UnCleanup(iwt, ucr->ucr_UnCleanUpRegion);
		}

	d1(KPrintF("%s/%s/%ld: Finished iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Cleanup(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (IsIwtViewByIcon(iwt))
		{
		if (iwt->iwt_WindowTask.wt_Window)
			{
			IconWindow_Cleanup(iwt);
			}

		// rearrange thumbnails so visible icons are thumbnailed first
		RearrangeThumbnailList(iwt);
		}

	d1(KPrintF("%s/%s/%ld: Finished iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Update(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaWindowStruct *ws;
	enum ScanDirResult Result, ReadResult;
	LONG oldPatternNr, newPatternNr;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	FlushThumbnailEntries(iwt);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	UpdateIconOverlays(iwt);

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return SCANDIR_OK;

	ws = iwt->iwt_WindowTask.mt_WindowStruct;

	ScalosLockIconListExclusive(iwt);

	FreeIconList(iwt, &iwt->iwt_WindowTask.wt_IconList);
	FreeIconList(iwt, &iwt->iwt_WindowTask.wt_LateIconList);

	ScalosUnLockIconList(iwt);

	if (IsIwtViewByIcon(iwt))
		{
		newPatternNr = PATTERNNR_IconWindowDefault;
		oldPatternNr = PATTERNNR_TextWindowDefault;
		}
	else
		{
		newPatternNr = PATTERNNR_TextWindowDefault;
		oldPatternNr = PATTERNNR_IconWindowDefault;
		}

	d1(KPrintF("%s/%s/%ld: oldPatternNr=%ld  newPatternNr=%ld  ws_PatternNumber=%ld\n", \
		__FILE__, __FUNC__, __LINE__, oldPatternNr, newPatternNr, ws->ws_PatternNumber));

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (oldPatternNr == ws->ws_PatternNumber)
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_NewPatternNumber, newPatternNr);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (IsIwtViewByIcon(iwt))
		{
		iwt->iwt_IconFont = IconWindowFont;
		iwt->iwt_IconTTFont = IconWindowTTFont;
		}
	else
		{
		iwt->iwt_IconFont = TextWindowFont;
		iwt->iwt_IconTTFont = TextWindowTTFont;
		}

	if (iwt->iwt_WindowTask.wt_Window)
		{
		Scalos_DoneRastPort(iwt->iwt_WindowTask.wt_Window->RPort);
		Scalos_SetFont(iwt->iwt_WindowTask.wt_Window->RPort, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);
		}

	IconActive = FALSE;

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
		SCCA_IconWin_Reading, TRUE,
		TAG_END);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	SetMenuOnOff(iwt);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (iwt->iwt_WindowTask.wt_Window)
		{
		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
			0, 0,
			iwt->iwt_WindowTask.wt_Window->Width - 1,
			iwt->iwt_WindowTask.wt_Window->Height - 1);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		RefreshIcons(iwt, NULL);
		}

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	Result = ReadResult = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ReadIconList, 0L);

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
		SCCA_IconWin_Reading, FALSE,
		TAG_END);

	SetMenuOnOff(iwt);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (ScanDirIsError(ReadResult))
		{
		iwt->iwt_CloseWindow = TRUE;
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_SetVirtSize,
                        SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return (ULONG) Result;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_AddToStatusBar(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_AddToStatusBar *mab = (struct msg_AddToStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_StatusBar)
		{
		DoGadgetMethod(iwt->iwt_StatusBar, iwt->iwt_WindowTask.wt_Window, NULL,
			OM_ADDMEMBER,
			NULL,
			(ULONG) mab->mab_NewMember,
			(ULONG) mab->mab_TagList);

		if (iwt->iwt_WindowTask.wt_Window)
			RefreshGList(iwt->iwt_StatusBar, iwt->iwt_WindowTask.wt_Window, NULL, 1);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_RemFromStatusBar(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RemFromStatusBar *mrb = (struct msg_RemFromStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_StatusBar)
		{
		DoGadgetMethod(iwt->iwt_StatusBar, iwt->iwt_WindowTask.wt_Window, NULL,
			OM_REMMEMBER,
			NULL,
			(ULONG) mrb->mrb_OldMember);

		if (iwt->iwt_WindowTask.wt_Window)
			RefreshGList(iwt->iwt_StatusBar, iwt->iwt_WindowTask.wt_Window, NULL, 1);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_UpdateStatusBar(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_UpdateStatusBar *mub = (struct msg_UpdateStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  Member=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, mub->mub_Member));

	if (iwt->iwt_StatusBar)
		{
		SetAttrsA((Object *) mub->mub_Member, (struct TagItem *) mub->mub_TagList);

		DoGadgetMethod(iwt->iwt_StatusBar,
			iwt->iwt_WindowTask.wt_Window, NULL,
			GBCL_UPDATEMEMBER,
			NULL,			// GadgetInfo gets filled in here by DoGadgetMethod()
			(ULONG) mub->mub_Member,
			NULL);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_AddToControlBar(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_AddToStatusBar *mab = (struct msg_AddToStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_ControlBar)
		{
		DoGadgetMethod(iwt->iwt_ControlBar, iwt->iwt_WindowTask.wt_Window, NULL,
			OM_ADDMEMBER,
			NULL,
			(ULONG) mab->mab_NewMember,
			(ULONG) mab->mab_TagList);

		if (iwt->iwt_WindowTask.wt_Window)
			RefreshGList(iwt->iwt_ControlBar, iwt->iwt_WindowTask.wt_Window, NULL, 1);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_RemFromControlBar(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_RemFromStatusBar *mrb = (struct msg_RemFromStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_ControlBar)
		{
		DoGadgetMethod(iwt->iwt_ControlBar, iwt->iwt_WindowTask.wt_Window, NULL,
			OM_REMMEMBER,
			NULL,
			(ULONG) mrb->mrb_OldMember);

		if (iwt->iwt_WindowTask.wt_Window)
			RefreshGList(iwt->iwt_ControlBar, iwt->iwt_WindowTask.wt_Window, NULL, 1);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_UpdateControlBar(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_UpdateStatusBar *mub = (struct msg_UpdateStatusBar *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  Member=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, mub->mub_Member));

	if (iwt->iwt_ControlBar)
		{
		SetAttrsA((Object *) mub->mub_Member, (struct TagItem *) mub->mub_TagList);

		DoGadgetMethod(iwt->iwt_ControlBar,
			iwt->iwt_WindowTask.wt_Window, NULL,
			GBCL_UPDATEMEMBER,
			NULL,			// GadgetInfo gets filled in here by DoGadgetMethod()
			(ULONG) mub->mub_Member,
			NULL);
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ShowPopupMenu(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_ShowPopupMenu *mpm = (struct msg_ShowPopupMenu *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_ActivatePending)
		{
		if (iwt->iwt_WindowTask.wt_Window->Flags & WFLG_WINDOWACTIVE)
			{
			iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags &= ~WSV_FlagF_ActivatePending;
			}
		else
			{
			// Window activation pending, delay popup menu until window has been made active
			iwt->iwt_PopupMenuPending = ScalosAlloc(sizeof(struct msg_ShowPopupMenu));

			if (iwt->iwt_PopupMenuPending)
				*iwt->iwt_PopupMenuPending = *mpm;

			d1(kprintf("\n" "%s/%s/%ld: iwt_PopupMenuPending=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_PopupMenuPending));

			return 0;
			}
		}

	d1(kprintf("\n" "%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ShowPopupMenu(iwt, mpm);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ActivateIconLeft(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	ChangeSelectedIconLeftRight(iwt, CURSORLEFT);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ActivateIconRight(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	ChangeSelectedIconLeftRight(iwt, CURSORRIGHT);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ActivateIconUp(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	ChangeSelectedIconUpDown(iwt, CURSORUP);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ActivateIconDown(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	ChangeSelectedIconUpDown(iwt, CURSORDOWN);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ActivateIconNext(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	ChangeSelectedIconNext(iwt);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ActivateIconPrevious(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	ChangeSelectedIconPrev(iwt);

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_CleanupByName(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	return IconWindowCleanupBy(iwt, IconSortByNameFunc);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_CleanupByDate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	return IconWindowCleanupBy(iwt, IconSortByDateFunc);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_CleanupBySize(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	return IconWindowCleanupBy(iwt, IconSortBySizeFunc);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_CleanupByType(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	return IconWindowCleanupBy(iwt, IconSortByTypeFunc);
}

//----------------------------------------------------------------------------

// insert user-supplied gadget in front of other window gadgets
static ULONG IconWindowClass_AddGadget(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_AddGadget *mag = (struct msg_AddGadget *) msg;

	if (mag->mag_NewGadget)
		{
		mag->mag_NewGadget->NextGadget = iwt->iwt_Gadgets;
		iwt->iwt_Gadgets = mag->mag_NewGadget;
		}

	return 1;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_ImmediateCheckUpdate(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	return IconWindowCheckUpdate(iwt);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_GetIconFileType(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_GetIconFileType *mft = (struct msg_GetIconFileType *) msg;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  in=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, GetIconName(mft->mft_IconNode)));

	if (!(mft->mft_IconNode->in_Flags & INF_Identified))
		{
		d1(KPrintF("%s/%s/%ld: in_FileType=%08lx\n", __FILE__, __FUNC__, __LINE__, mft->mft_IconNode->in_FileType));

		if (!IS_TYPENODE(mft->mft_IconNode->in_FileType))
			{
			if (mft->mft_IconNode->in_Flags & INF_TextIcon)
				GetAttr(TIDTA_TypeNode, mft->mft_IconNode->in_Icon, (ULONG *) &mft->mft_IconNode->in_FileType);
			else
				GetAttr(IDTA_Type, mft->mft_IconNode->in_Icon, (ULONG *) &mft->mft_IconNode->in_FileType);
			}

		d1(KPrintF("%s/%s/%ld: in_FileType=%08lx  in_Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, mft->mft_IconNode->in_FileType, mft->mft_IconNode->in_Lock));

		if (!IS_TYPENODE(mft->mft_IconNode->in_FileType)
			&& ((struct TypeNode *) WBAPPICON) != mft->mft_IconNode->in_FileType)
			{
			BPTR dLock;

			if (mft->mft_IconNode->in_Lock)
				dLock = mft->mft_IconNode->in_Lock;
			else
				dLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			debugLock_d1(dLock);

			mft->mft_IconNode->in_FileType = DefIconsIdentify(dLock, GetIconName(mft->mft_IconNode), ICONTYPE_NONE);
			}

		if (NULL != mft->mft_IconNode->in_FileType)
			mft->mft_IconNode->in_Flags |= INF_Identified;
		}

	if (mft->mft_IconNode->in_Name)
		{
		BPTR dirLock;
		BPTR newLock = (BPTR)NULL;

		d1(KPrintF("%s/%s/%ld: in_FileType=%08lx\n", __FILE__, __FUNC__, __LINE__, mft->mft_IconNode->in_FileType));

		switch ((ULONG) mft->mft_IconNode->in_FileType)
			{
		case WBDRAWER:
		case WB_TEXTICON_DRAWER:
			mft->mft_IconNode->in_FileType = (struct TypeNode *) WBDRAWER;
			break;

		case WBGARBAGE:
			mft->mft_IconNode->in_FileType = (struct TypeNode *) WBGARBAGE;
			SetAttrs(mft->mft_IconNode->in_Icon,
				IDTA_Type, WBGARBAGE,
				TAG_END);
			break;

		case WBDISK:
			mft->mft_IconNode->in_FileType = (struct TypeNode *) WBDISK;
			break;

		case WBTOOL:
		case WBPROJECT:
		case WB_TEXTICON_TOOL:		// Text Tool
			if (mft->mft_IconNode->in_DeviceIcon)
				dirLock = newLock = DiskInfoLock(mft->mft_IconNode);
			else if (mft->mft_IconNode->in_Lock)
				dirLock = mft->mft_IconNode->in_Lock;
			else
				dirLock = iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;

			d1(kprintf("%s/%s/%ld: dirLock=%08lx\n", __FILE__, __FUNC__, __LINE__, dirLock));

			if (dirLock)
				mft->mft_IconNode->in_FileType = DefIconsIdentify(dirLock, mft->mft_IconNode->in_Name, ICONTYPE_NONE);

			d1(kprintf("%s/%s/%ld: FileType=%08lx\n", __FILE__, __FUNC__, __LINE__, mft->mft_IconNode->in_FileType));
			break;

		default:
			break;
			}

		if (newLock)
			UnLock(newLock);
		}

	d1(kprintf("%s/%s/%ld: mft->mft_IconNode=<%s>  IconType=%ld  iwt_ReadOnly=%ld\n", __FILE__, __FUNC__, __LINE__, GetIconName(mft->mft_IconNode), (ULONG) mft->mft_IconNode->in_FileType, iwt->iwt_ReadOnly));

	SetIconSupportsFlags(mft->mft_IconNode, !iwt->iwt_ReadOnly);

	return 0;
}

//----------------------------------------------------------------------------

// Remove all references to DefIcons structure members
static ULONG IconWindowClass_ClearIconFileTypes(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaIconNode *in;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));

	ScalosLockIconListShared(iwt);

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		ULONG IconType;

		switch ((ULONG) in->in_FileType)
			{
		case WBTOOL:
		case WBPROJECT:
		case WBDISK:
		case WBDRAWER:
		case WBGARBAGE:
			break;
		default:
			GetAttr(IDTA_Type, in->in_Icon, &IconType);
			in->in_FileType = (struct TypeNode *) IconType;
			break;
			}
		}
	for (in = iwt->iwt_WindowTask.wt_LateIconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		ULONG IconType;

		switch ((ULONG) in->in_FileType)
			{
		case WBTOOL:
		case WBPROJECT:
		case WBDISK:
		case WBDRAWER:
		case WBGARBAGE:
			break;
		default:
			GetAttr(IDTA_Type, in->in_Icon, &IconType);
			in->in_FileType = (struct TypeNode *) IconType;
			break;
			}
		}

	ScalosUnLockIconList(iwt);

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_NewPatternNumber(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_NewPatternNumber *npn = (struct msg_NewPatternNumber *) msg;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  NewPatternNr=%lu\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, npn->npn_PatternNumber));

	if (npn->npn_PatternNumber != ws->ws_PatternNumber)
		{
		ws->ws_PatternNumber = npn->npn_PatternNumber;

		ws->ws_PatternNode = GetPatternNode(ws->ws_PatternNumber, NULL);

		NewWindowPatternMsg(iwt, NULL, (struct PatternNode **) &ws->ws_PatternNode);
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_RandomizePatternNumber(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	struct PatternNode *NewPatternNode;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  NewPatternNr=%lu\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, npn->npn_PatternNumber));

	NewPatternNode = GetPatternNode(ws->ws_PatternNumber, (struct PatternNode *) ws->ws_PatternNode);
	if (NewPatternNode != (struct PatternNode *) ws->ws_PatternNode)
		{
		ws->ws_PatternNode = NewPatternNode;
		NewWindowPatternMsg(iwt, NULL, (struct PatternNode **) &ws->ws_PatternNode);
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_NewPath(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_NewPath *npa = (struct msg_NewPath *) msg;
	BPTR NewPathLock = (BPTR)NULL;
	BOOL UpdateSemaphoreLocked = FALSE;
	ULONG Success = FALSE;
	struct ScaIconNode *OldIconList = NULL;
	STRPTR NewWindowTitle = NULL;
	STRPTR NewWsName = NULL;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  <%s>  Path=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, npa->npa_Path));
	d1(kprintf("%s/%s/%ld: Task=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL), FindTask(NULL)->tc_Node.ln_Name));

	do	{
		struct WindowHistoryEntry *whe;
		struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
		struct FileLock *fLock;
		CONST_STRPTR clp;
		STRPTR lp;
		BPTR parentLock;
		BPTR oldDir;
		BPTR oldWsLock;
		BOOL isRootWindow;
		struct IconWindowProperties iwpOld;
		struct IconWindowProperties iwpNew;
		BOOL WindowListLocked = FALSE;
		struct ScaWindowStruct *wsIconListLocked = NULL;
		Object *IconObj;
		Object *allocIconObj = NULL;
		struct ScaIconNode *newIconList = NULL;
		enum ScanDirResult scanDirResult = SCANDIR_OK;

		iwpNew.iwp_ThumbnailView = CurrentPrefs.pref_ShowThumbnails;
		iwpNew.iwp_ThumbnailsLifetimeDays = CurrentPrefs.pref_ThumbnailMaxAge;
		iwpNew.iwp_PatternNumber = IsViewByIcon(ws->ws_Viewmodes)
			? PATTERNNR_IconWindowDefault : PATTERNNR_TextWindowDefault;

		iwpOld.iwp_PatternNumber = ws->ws_PatternNumber;
		iwpOld.iwp_ThumbnailView = ws->ws_ThumbnailView;
		iwpOld.iwp_ThumbnailsLifetimeDays = ws->ws_ThumbnailsLifetimeDays;
		iwpOld.iwp_Viewmodes = iwpNew.iwp_Viewmodes = ws->ws_Viewmodes;
		iwpOld.iwp_ShowAll = iwpNew.iwp_ShowAll = ws->ws_ViewAll;
		iwpOld.iwp_XOffset = iwpNew.iwp_XOffset = iwt->iwt_WindowTask.wt_XOffset;
		iwpOld.iwp_YOffset = iwpNew.iwp_YOffset = iwt->iwt_WindowTask.wt_YOffset;
		iwpOld.iwp_OpacityActive = iwpNew.iwp_OpacityActive = ws->ws_WindowOpacityActive;
		iwpOld.iwp_OpacityInactive = iwpNew.iwp_OpacityInactive = ws->ws_WindowOpacityInactive;
		OldIconList = iwt->iwt_WindowTask.wt_IconList;
		memcpy(iwpOld.iwp_WidthArray, iwt->iwt_WidthArray, sizeof(iwpOld.iwp_WidthArray));

		d1(KPrintF("%s/%s/%ld: &wt_IconList=%08lx  wt_IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			&iwt->iwt_WindowTask.wt_IconList, iwt->iwt_WindowTask.wt_IconList));

		if (WSV_Type_DeviceWindow == ws->ws_WindowType)
			break;	// not supported for desktop window

		if (NULL == npa->npa_Path || strlen(npa->npa_Path) < 1)
			break;	// new path invalid

		iwt->iwt_AbortScanDir = TRUE;		// signal pending ScanDir to abort ASAP
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		if (!AttemptSemaphoreNoNest(&iwt->iwt_UpdateSemaphore))
			{
			d1(KPrintF("%s/%s/%ld: AttemptSemaphoreNoNest(&iwt->iwt_UpdateSemaphore)\n", __FILE__, __FUNC__, __LINE__));
			SendNewIconPathMsg(iwt, npa->npa_Path, (struct TagItem *) npa->npa_TagList);
			OldIconList = NULL;	// do not free IconList
			break;
			}
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		UpdateSemaphoreLocked = TRUE;

		FlushThumbnailEntries(iwt);

		oldDir = CurrentDir(ws->ws_Lock);
		NewPathLock = Lock(npa->npa_Path, ACCESS_READ);

		CurrentDir(oldDir);
		debugLock_d1(NewPathLock);
		if ((BPTR)NULL == NewPathLock)
			break;

		if (LOCK_SAME == SameLock(NewPathLock, ws->ws_Lock))
			break;		// stop here if path hasn't changed

		parentLock = ParentDir(NewPathLock);
		debugLock_d1(parentLock);
		if (parentLock)
			{
			// this is not a root window
			isRootWindow = FALSE;
			clp = CurrentPrefs.pref_StandardWindowTitle;
			UnLock(parentLock);
			}
		else
			{
			// this is a root window
			isRootWindow = TRUE;
			clp = CurrentPrefs.pref_MainWindowTitle;
			}

		d1(KPrintF("%s/%s/%ld: NewPathLock=%08lx\n", __FILE__, __FUNC__, __LINE__, NewPathLock));

		if (NewPathLock)
			{
			whe = WindowFindHistoryEntry(iwt, NewPathLock);

			d1(KPrintF("%s/%s/%ld: whe=%08lx\n", __FILE__, __FUNC__, __LINE__, whe));
			if (whe)
				{
				iwpNew.iwp_Viewmodes = whe->whe_Viewmodes;
				iwpNew.iwp_XOffset = whe->whe_XOffset;
				iwpNew.iwp_YOffset = whe->whe_YOffset;
				iwpNew.iwp_ShowAll = whe->whe_ViewAll;

				d1(KPrintF("%s/%s/%ld: whe_ScanDirResult=%ld\n", __FILE__, __FUNC__, __LINE__, whe->whe_ScanDirResult));

				if (SCANDIR_OK == whe->whe_ScanDirResult)
					newIconList = whe->whe_IconList;
				else
					FreeIconList(iwt, &whe->whe_IconList);

				memcpy(iwpNew.iwp_WidthArray, whe->whe_WidthArray, sizeof(iwpNew.iwp_WidthArray));

				ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
				}
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		NewWindowTitle = AllocCopyString(clp);
		if (NULL == NewWindowTitle)
			break;

		NewWsName = GetWsNameFromLock(NewPathLock);
		if (NULL == NewWsName)
			break;

		d1(KPrintF("%s/%s/%ld: NewWsName=<%s>\n", __FILE__, __FUNC__, __LINE__, NewWsName));

		IconObj = FunctionsFindIconObjectForPath(npa->npa_Path, &WindowListLocked,
			&wsIconListLocked, &allocIconObj);
		d1(KPrintF("%s/%s/%ld: IconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, IconObj));
		if (IconObj)
			FunctionsGetSettingsFromIconObject(&iwpNew, IconObj);
		if (allocIconObj)
			DisposeIconObject(allocIconObj);
		if (wsIconListLocked)
			{
			d1(KPrintF("\n" "%s/%s/%ld:  wsLocked=%08lx\n", __FILE__, __FUNC__, __LINE__, wsIconListLocked));
			ScalosReleaseSemaphore(wsIconListLocked->ws_WindowTask->wt_IconSemaphore);
			}
		if (WindowListLocked)
			{
			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			SCA_UnLockWindowList();
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (!ScalosAttemptLockIconListExclusive(iwt))
			{
			d1(KPrintF("%s/%s/%ld: ScalosAttemptLockIconListExclusive failed\n", __FILE__, __FUNC__, __LINE__));
			SendNewIconPathMsg(iwt, npa->npa_Path, (struct TagItem *) npa->npa_TagList);
			OldIconList = NULL;	// do not free IconList
			break;
			}

		d1(KPrintF("%s/%s/%ld: ws_Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_Lock));
		d1(KPrintF("%s/%s/%ld: &wt_IconList=%08lx  wt_IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			&iwt->iwt_WindowTask.wt_IconList, iwt->iwt_WindowTask.wt_IconList));

		// visually erase all icons
		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
			0, 0,
			iwt->iwt_WindowTask.wt_Window->Width - 1,
			iwt->iwt_WindowTask.wt_Window->Height - 1);

		d1(KPrintF("%s/%s/%ld: wt_IconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconList));

		iwt->iwt_WindowTask.wt_IconList = NULL;

		// free all icons
		FreeIconList(iwt, &iwt->iwt_WindowTask.wt_LateIconList);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ScalosUnLockIconList(iwt);

		d1(KPrintF("%s/%s/%ld: UnLocking old ws_Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_Lock));

		CurrentDir(NewPathLock);

		d1(KPrintF("%s/%s/%ld: mt_WindowObject=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.mt_WindowObject));
		d1(KPrintF("%s/%s/%ld: npa_Path=%08lx\n", __FILE__, __FUNC__, __LINE__, npa->npa_Path));

		if (NULL == iwt->iwt_WindowTask.mt_WindowObject)
			{
			d1(KPrintF("%s/%s/%ld: mt_WindowObject==NULL\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject,
			SCCM_Window_NewPath,
			npa->npa_Path,
			TAG_END);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		// make sure ws_Lock is always valid!
		oldWsLock = ws->ws_Lock;
		ws->ws_Lock = NewPathLock;
		UnLock(oldWsLock);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		fLock = BADDR(ws->ws_Lock);
		iwt->iwt_OldFileSys = SetFileSysTask(fLock->fl_Task);

		// cancel notification for old path
		// and establish notification for new path
		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_StartNotify);

		iwt->iwt_ReadOnly = !ClassIsDiskWritable(ws->ws_Lock);

		SetIconWindowReadOnly(iwt, iwt->iwt_ReadOnly);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (ws->ws_Name)
			FreeCopyString(ws->ws_Name);
		ws->ws_Name = NewWsName;
		d1(KPrintF("%s/%s/%ld: ws_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, ws->ws_Name));

		lp = ws->ws_Title;
		ws->ws_Title = NewWindowTitle;
		FreeCopyString(lp);
		d1(KPrintF("%s/%s/%ld: ws_Title=%08lx\n", __FILE__, __FUNC__, __LINE__, ws->ws_Title));

		if (isRootWindow)
			ws->ws_Flags |= WSV_FlagF_RootWindow;
		else
			ws->ws_Flags &= ~WSV_FlagF_RootWindow;

		d1(KPrintF("%s/%s/%ld: isRootWindow=%ld  ws_Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, isRootWindow, ws->ws_Flags));

		SetAttrs(iwt->iwt_TitleObject,
				SCCA_Title_Format, (ULONG) (ws->ws_Title ? ws->ws_Title : (STRPTR) "Scalos"),
				TAG_END);
		iwt->iwt_WinTitle = (STRPTR) DoMethod(iwt->iwt_TitleObject, SCCM_Title_Generate);
		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_SetTitle, iwt->iwt_WinTitle);

		iwpNew.iwp_Viewmodes = GetTagData(SCA_ViewModes, iwpNew.iwp_Viewmodes, (struct TagItem *) npa->npa_TagList);
		iwpNew.iwp_XOffset = GetTagData(SCA_XOffset, iwpNew.iwp_XOffset, (struct TagItem *) npa->npa_TagList);
		iwpNew.iwp_YOffset = GetTagData(SCA_YOffset, iwpNew.iwp_YOffset, (struct TagItem *) npa->npa_TagList);
		iwpNew.iwp_ShowAll = GetTagData(SCA_ShowAllMode, iwpNew.iwp_ShowAll, (struct TagItem *) npa->npa_TagList);
		iwpNew.iwp_OpacityActive = GetTagData(SCA_TransparencyActive, iwpNew.iwp_OpacityActive, (struct TagItem *) npa->npa_TagList);
		iwpNew.iwp_OpacityInactive = GetTagData(SCA_TransparencyInactive, iwpNew.iwp_OpacityInactive, (struct TagItem *) npa->npa_TagList);
		newIconList = (struct ScaIconNode *) GetTagData(SCA_IconList, (ULONG) newIconList, (struct TagItem *) npa->npa_TagList);

		d1(KPrintF("%s/%s/%ld: newIconList=%08lx\n", __FILE__, __FUNC__, __LINE__, newIconList));

		ws->ws_ThumbnailsLifetimeDays = iwpNew.iwp_ThumbnailsLifetimeDays;

		d1(KPrintF("%s/%s/%ld: iwpOld.iwp_ShowAll=%ld  iwpOld.iwp_Viewmodes=%ld\n", __FILE__, __FUNC__, __LINE__, iwpOld.iwp_ShowAll, iwpOld.iwp_Viewmodes));
		d1(KPrintF("%s/%s/%ld: iwpNew.iwp_ShowAll=%ld  iwpNew.iwp_Viewmodes=%ld\n", __FILE__, __FUNC__, __LINE__, iwpNew.iwp_ShowAll, iwpNew.iwp_Viewmodes));

		if (iwpOld.iwp_ThumbnailView != iwpNew.iwp_ThumbnailView)
			{
			d1(KPrintF("%s/%s/%ld: iwpNew.iwp_ThumbnailView=%lu\n", __FILE__, __FUNC__, __LINE__, iwpNew.iwp_ThumbnailView));
			SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
				SCCA_IconWin_ThumbnailView, iwpNew.iwp_ThumbnailView,
				TAG_END);
			}
		if (iwpOld.iwp_PatternNumber != iwpNew.iwp_PatternNumber)
			{
			d1(KPrintF("%s/%s/%ld: iwpNew.iwp_PatternNumber=%lu\n", __FILE__, __FUNC__, __LINE__, iwpNew.iwp_PatternNumber));
			DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_NewPatternNumber, iwpNew.iwp_PatternNumber);
			}
		if (iwpNew.iwp_ShowAll != iwpOld.iwp_ShowAll)
			{
			// Keeping UpdateSemaphore locked here prevents any premature updates
			SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
				SCCA_IconWin_ShowType, iwpNew.iwp_ShowAll,
				TAG_END);
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ViewWindowBy(iwt, iwpNew.iwp_Viewmodes);

		// Release UpdateSemaphore now, otherwise SCCM_IconWin_Update won't work!
		ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);
		UpdateSemaphoreLocked = FALSE;

		iwt->iwt_WindowTask.wt_XOffset = iwpNew.iwp_XOffset;
		iwt->iwt_WindowTask.wt_YOffset = iwpNew.iwp_YOffset;

		d1(KPrintF("%s/%s/%ld: newIconList=%08lx\n", __FILE__, __FUNC__, __LINE__, newIconList));
		if (newIconList)
			{
			iwt->iwt_WindowTask.wt_IconList = newIconList;
			memcpy(iwt->iwt_WidthArray, iwpNew.iwp_WidthArray, sizeof(iwt->iwt_WidthArray));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
				SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
			RefreshIcons(iwt, NULL);

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ImmediateCheckUpdate);
			}
		else
			{
			scanDirResult = DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

			d1(KPrintF("%s/%s/%ld: SCCM_IconWin_Update Result = %08lx\n", __FILE__, __FUNC__, __LINE__, scanDirResult));
			if (ScanDirIsError(scanDirResult))
				break;	// error reading icons
			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		UpdateIconCount(iwt);

		if (iwt->iwt_CurrentHistoryEntry)
			{
			iwt->iwt_CurrentHistoryEntry->whe_Viewmodes = iwpOld.iwp_Viewmodes;
			iwt->iwt_CurrentHistoryEntry->whe_ViewAll = iwpOld.iwp_ShowAll;
			iwt->iwt_CurrentHistoryEntry->whe_XOffset = iwpOld.iwp_XOffset;
			iwt->iwt_CurrentHistoryEntry->whe_YOffset = iwpOld.iwp_YOffset;
			iwt->iwt_CurrentHistoryEntry->whe_IconList = OldIconList;
			memcpy(iwt->iwt_CurrentHistoryEntry->whe_WidthArray, iwpOld.iwp_WidthArray, sizeof(iwt->iwt_CurrentHistoryEntry->whe_WidthArray));
			OldIconList = NULL;
			}
		else
			{
			FreeIconList(iwt, &OldIconList);
			OldIconList = NULL;
			}

		// Update window history
		whe = WindowFindHistoryEntry(iwt, ws->ws_Lock);
		d1(KPrintF("%s/%s/%ld: whe=%08lx\n", __FILE__, __FUNC__, __LINE__, whe));
		if (NULL == whe)
			{
			ScalosObtainSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
			whe = WindowAddHistoryEntry(iwt, ws->ws_Lock);
			if (NULL == whe)
				ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
			else
				{
				whe->whe_ScanDirResult = scanDirResult;
				}
			}

		d1(KPrintF("%s/%s/%ld: whe=%08lx\n", __FILE__, __FUNC__, __LINE__, whe));

		if (whe)
			{
			d1(KPrintF("%s/%s/%ld: iwpOld.iwp_ShowAll=%ld  iwpOld.iwp_Viewmodes=%ld\n", __FILE__, __FUNC__, __LINE__, iwpOld.iwp_ShowAll, iwpOld.iwp_Viewmodes));

			iwt->iwt_CurrentHistoryEntry = whe;
			iwt->iwt_CurrentHistoryEntry->whe_IconList = NULL;
			ControlBarUpdateHistory(iwt);
			ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
			}

		SetMenuOnOff(iwt);	// must be called AFTER updating iwt_CurrentHistoryEntry !

		Success = TRUE;
                } while (0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (Success)
		{
		FreeIconList(iwt, &OldIconList);
		}
	else
		{
		// if changing path didn't succeed,
		// free everything we allocated until now
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (NewWindowTitle)
			FreeCopyString(NewWindowTitle);
		if (NewWsName)
			FreeCopyString(NewWsName);
		if (NewPathLock)
			UnLock(NewPathLock);
		}

	if (UpdateSemaphoreLocked)
		ScalosReleaseSemaphore(&iwt->iwt_UpdateSemaphore);

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	return Success;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_HistoryBack(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		ScalosObtainSemaphoreShared(&iwt->iwt_WindowHistoryListSemaphore);

		if (!IsListEmpty(&iwt->iwt_HistoryList)	      // should not happen
			&& NULL != iwt->iwt_CurrentHistoryEntry		// should not happen
			&&  iwt->iwt_CurrentHistoryEntry != (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_Head)
			{
			struct WindowHistoryEntry *whe = (struct WindowHistoryEntry *) iwt->iwt_CurrentHistoryEntry->whe_Node.ln_Pred;

			d1(KPrintF("%s/%s/%ld: iwt_CurrentHistoryEntry=%08lx  ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_CurrentHistoryEntry, iwt->iwt_CurrentHistoryEntry->whe_Node.ln_Name));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_NewPath,
				whe->whe_Node.ln_Name,
				SCA_ShowAllMode, whe->whe_ViewAll,
				SCA_ViewModes, whe->whe_Viewmodes,
				SCA_XOffset, whe->whe_XOffset,
				SCA_YOffset, whe->whe_YOffset,
				TAG_END);

			d1(KPrintF("%s/%s/%ld: SCA_ShowAllMode=%ld  SCA_ViewModes=%ld\n", __FILE__, __FUNC__, __LINE__, whe->whe_ViewAll, whe->whe_Viewmodes));
			d1(KPrintF("%s/%s/%ld: whe=%08lx  ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, whe, whe->whe_Node.ln_Name));
			}

		ScalosReleaseSemaphore(&iwt->iwt_WindowHistoryListSemaphore);
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_HistoryForward(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		if (!IsListEmpty(&iwt->iwt_HistoryList)	      // should not happen
			&& NULL != iwt->iwt_CurrentHistoryEntry		// should not happen
			&&  iwt->iwt_CurrentHistoryEntry != (struct WindowHistoryEntry *) iwt->iwt_HistoryList.lh_TailPred)
			{
			struct WindowHistoryEntry *whe = (struct WindowHistoryEntry *) iwt->iwt_CurrentHistoryEntry->whe_Node.ln_Succ;

			d1(KPrintF("%s/%s/%ld: iwt_CurrentHistoryEntry=%08lx  ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_CurrentHistoryEntry, iwt->iwt_CurrentHistoryEntry->whe_Node.ln_Name));

			DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_NewPath,
				whe->whe_Node.ln_Name,
				SCA_ShowAllMode, whe->whe_ViewAll,
				SCA_ViewModes, whe->whe_Viewmodes,
				SCA_XOffset, whe->whe_XOffset,
				SCA_YOffset, whe->whe_YOffset,
				TAG_END);

			d1(KPrintF("%s/%s/%ld: SCA_ShowAllMode=%ld  SCA_ViewModes=%ld\n", __FILE__, __FUNC__, __LINE__, whe->whe_ViewAll, whe->whe_Viewmodes));
			d1(KPrintF("%s/%s/%ld: whe=%08lx  ln_Name=<%s>\n", __FILE__, __FUNC__, __LINE__, whe, whe->whe_Node.ln_Name));
			}

		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_StartNotify(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	BOOL Success = FALSE;

	if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
		{
		struct IconWindowClassInstance *inst = INST_DATA(cl, o);

		do	{
			if (inst->ici_NotifyReq)
				ScalosEndNotify(inst->ici_NotifyReq);	// cancel old notification
			else
				{
				inst->ici_NotifyReq = ScalosAlloc(sizeof(struct NotifyRequest));
				if (inst->ici_NotifyReq)
					memset(inst->ici_NotifyReq, 0, sizeof(struct NotifyRequest));
				}
			if (NULL == inst->ici_NotifyReq)
				break;

			inst->ici_DrawerName = AllocPathBuffer();
			if (NULL == inst->ici_DrawerName)
				break;

			if (!NameFromLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, inst->ici_DrawerName, MAX_FileName-1))
				break;

			inst->ici_NotifyReq->nr_Name = strchr(inst->ici_DrawerName, ':');
			if (NULL == inst->ici_NotifyReq->nr_Name)
				inst->ici_NotifyReq->nr_Name = inst->ici_DrawerName;

			inst->ici_NotifyReq->nr_stuff.nr_Msg.nr_Port = iwt->iwt_WindowTask.wt_IconPort;
			inst->ici_NotifyReq->nr_Flags = NRF_SEND_MESSAGE;
			inst->ici_NotifyReq->nr_UserData = (ULONG)NULL;

			if (!StartNotify(inst->ici_NotifyReq))
				break;

			Success = TRUE;
			} while (0);

		if (!Success)
			{
			if (inst->ici_NotifyReq)
				{
				if (inst->ici_DrawerName)
					{
					FreePathBuffer(inst->ici_DrawerName);
					inst->ici_DrawerName = NULL;
					}
				ScalosFree(inst->ici_NotifyReq);
				inst->ici_NotifyReq = NULL;
				}
			}
		}

	return (ULONG) Success;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_Browse(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;

	if (iwt->iwt_ControlBar && iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock && iwt->iwt_WindowTask.wt_Window)
		{
		RunProcess(&iwt->iwt_WindowTask, IconWinBrowseProc,
			0, NULL, iInfos.xii_iinfos.ii_MainMsgPort);
                }

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_StartPopOpenTimer(Class *cl, Object *o, Msg msg)
{
	//struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_StartPopOpenTimer *spot = (struct msg_StartPopOpenTimer *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  dh=%08lx  spot_IconNode=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		iwt, iwt->iwt_WinTitle, spot->spot_DragHandle, \
		spot->spot_IconNode, GetIconName(spot->spot_IconNode)));

	if (spot->spot_DragHandle && CurrentPrefs.pref_EnablePopupWindows)
		{
		spot->spot_DragHandle->drgh_PopOpenDestWindow = (struct internalScaWindowTask *) spot->spot_DestWindow;
		spot->spot_DragHandle->drgh_PopOpenIcon = spot->spot_IconNode;
		spot->spot_DragHandle->drgh_PopOpenTickCount = CurrentPrefs.pref_PopupWindowDelaySeconds * 10;	   // ~10 ticks/s
		}

	return 0;
}
//----------------------------------------------------------------------------

static ULONG IconWindowClass_StopPopOpenTimer(Class *cl, Object *o, Msg msg)
{
	//struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_StopPopOpenTimer *stop = (struct msg_StopPopOpenTimer *) msg;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>  dh=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt, \
		iwt->iwt_WinTitle, stop->stop_DragHandle));

	if (stop->stop_DragHandle)
		{
		stop->stop_DragHandle->drgh_PopOpenDestWindow = NULL;
		stop->stop_DragHandle->drgh_PopOpenIcon = NULL;
		stop->stop_DragHandle->drgh_PopOpenTickCount = 0;
		}

	return 0;
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_AddUndoEvent(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_AddUndoEvent *aue = (struct msg_AddUndoEvent *) msg;

	return UndoAddEventTagList(iwt, aue->aue_Type, (struct TagItem *) aue->aue_TagList);
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_BeginUndoStep(Class *cl, Object *o, Msg msg)
{
	return (ULONG) UndoBeginStep();
}

//----------------------------------------------------------------------------

static ULONG IconWindowClass_EndUndoStep(Class *cl, Object *o, Msg msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) ((struct ScaRootList *) o)->rl_WindowTask;
	struct msg_EndUndoStep *eus = (struct msg_EndUndoStep *) msg;

	UndoEndStep(iwt, eus->eus_UndoStep);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(ULONG) IconWinBrowseProc(APTR aptr, struct SM_RunProcess *msg)
{
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) msg->WindowTask;
	STRPTR DrawerName = NULL;

	do	{
		struct SM_NewWindowPath *smnwp;

		if (NULL == iwt->iwt_AslFileRequest)
			iwt->iwt_AslFileRequest = AllocAslRequest(ASL_FileRequest, NULL);
		if (NULL == iwt->iwt_AslFileRequest)
			break;

		DrawerName = AllocPathBuffer();
		if (NULL == DrawerName)
			break;

		if (!NameFromLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock, DrawerName, MAX_FileName-1))
			break;

		if (!AslRequestTags(iwt->iwt_AslFileRequest,
			ASLFR_Window, iwt->iwt_WindowTask.wt_Window,
			ASLFR_PrivateIDCMP, TRUE,
			ASLFR_DrawersOnly, TRUE,
			ASLFR_RejectIcons, TRUE,
			ASLFR_TitleText, GetLocString(MSGID_BROWSE_ASLTITLE),
			ASLFR_InitialDrawer, DrawerName,
			TAG_END))
			break;

		smnwp = (struct SM_NewWindowPath *) SCA_AllocMessage(MTYP_NewWindowPath, 0);

		if (smnwp)
			{
			smnwp->smnwp_Path = AllocCopyString(iwt->iwt_AslFileRequest->fr_Drawer);
			PutMsg(iwt->iwt_WindowTask.mt_WindowStruct->ws_MessagePort, &smnwp->ScalosMessage.sm_Message);
			}
		} while (0);

	if (DrawerName)
		FreePathBuffer(DrawerName);

	return 0;
}


static ULONG IconWinNotify(struct internalScaWindowTask *iwt, struct IntuiMessage *im)
{
	d1(kprintf("%s/%s/%ld: NOTIFY iwt=%08lx  <%s>  Seconds=%ld  Micros=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, im->Seconds, im->Micros));

	return DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ScheduleUpdate);
}


static void ScrollWindowXY(struct internalScaWindowTask *iwt, LONG dx, LONG dy)
{
	ULONG svsFlags = 0;

	if (dx != 0)
		svsFlags |= SETVIRTF_AdjustBottomSlider;
	if (dy != 0)
		svsFlags |= SETVIRTF_AdjustRightSlider;

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, dx, dy);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, svsFlags);
}


static void SetVirtSize(struct internalScaWindowTask *iwt, ULONG Flags)
{
	d1(KPrintF("%s/%s/%ld: Flags=%04lx\n", __FILE__, __FUNC__, __LINE__, Flags));

	if (NULL == iwt->iwt_Gadgets)
		return;
	if (iwt->iwt_BackDrop)
		return;

	GetIconsTotalBoundingBox(iwt);

	d1(KPrintF("%s/%s/%ld: MinX=%ld  MaxX=%ld  MinY=%ld  MaxY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt->iwt_IconBBox.MinX, iwt->iwt_IconBBox.MaxX, iwt->iwt_IconBBox.MinY, iwt->iwt_IconBBox.MaxY));

	if (Flags & SETVIRTF_AdjustBottomSlider)
		{
		LONG Total, Visible, Top;

		d1(kprintf("%s/%s/%ld: InnerWidth=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerWidth));

		Visible = iwt->iwt_InnerWidth;
		Top = 0;
		Total = 0;

		if (LONG_MAX != iwt->iwt_IconBBox.MinX)
			{
			LONG Right = iwt->iwt_InnerWidth + iwt->iwt_WindowTask.wt_XOffset - 1;

			iwt->iwt_IconBBox.MinX--;
			iwt->iwt_IconBBox.MaxX++;

			Top = iwt->iwt_IconBBox.MinX;
			if (iwt->iwt_WindowTask.wt_XOffset < Top)
				Top = iwt->iwt_WindowTask.wt_XOffset;

			Total = iwt->iwt_IconBBox.MaxX;
			if (Right > Total)
				Total = Right;

			Total -= Top;

			iwt->iwt_InvisibleWidth = Total - Visible;
			iwt->iwt_MinX = Top;

			Top = iwt->iwt_WindowTask.wt_XOffset - Top;
			}

		d1(kprintf("%s/%s/%ld: Horizontal  Top=%ld  Total=%ld  Visible=%ld\n", __FILE__, __FUNC__, __LINE__, Top, Total, Visible));

		iwt->iwt_BottomScrollerScale = 0;

		while (Total > USHRT_MAX)
			{
			iwt->iwt_BottomScrollerScale++;
			Total >>= 1;
			}

		Visible >>= iwt->iwt_BottomScrollerScale;
		Top >>= iwt->iwt_BottomScrollerScale;

		d1(kprintf("%s/%s/%ld: Horizontal  Top=%ld  Total=%ld  Visible=%ld  Scale=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Top, Total, Visible, iwt->iwt_BottomScrollerScale));

		SetGadgetAttrs(iwt->iwt_PropBottom, iwt->iwt_WindowTask.wt_Window, NULL,
			PGA_Total, Total,
			PGA_Visible, Visible,
			PGA_Top, Top,
			TAG_END);
		}

	if (Flags & SETVIRTF_AdjustRightSlider)
		{
		LONG Total, Visible, Top;

		d1(kprintf("%s/%s/%ld: InnerHeight=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_InnerHeight));

		Visible = iwt->iwt_InnerHeight;
		Top = 0;
		Total = 0;

		if (LONG_MAX != iwt->iwt_IconBBox.MinY)
			{
			LONG Bottom = iwt->iwt_InnerHeight + iwt->iwt_WindowTask.wt_YOffset - 1;

			iwt->iwt_IconBBox.MinY--;
			iwt->iwt_IconBBox.MaxY++;

			Top = iwt->iwt_IconBBox.MinY;
			if (iwt->iwt_WindowTask.wt_YOffset < Top)
				Top = iwt->iwt_WindowTask.wt_YOffset;

			Total = iwt->iwt_IconBBox.MaxY;
			if (Bottom > Total)
				Total = Bottom;

			Total -= Top;

			iwt->iwt_InvisibleHeight = Total - Visible;
			iwt->iwt_MinY = Top;

			Top = iwt->iwt_WindowTask.wt_YOffset - Top;
			}

		d1(kprintf("%s/%s/%ld: Vertical  Top=%ld  Total=%ld  Visible=%ld\n", __FILE__, __FUNC__, __LINE__, Top, Total, Visible));

		iwt->iwt_SideScrollerScale = 0;

		while (Total > USHRT_MAX)
			{
			iwt->iwt_SideScrollerScale++;
			Total >>= 1;
			}

		Visible >>= iwt->iwt_SideScrollerScale;
		Top >>= iwt->iwt_SideScrollerScale;

		d1(kprintf("%s/%s/%ld: Horizontal  Top=%ld  Total=%ld  Visible=%ld  Scale=%ld\n", \
			__FILE__, __FUNC__, __LINE__, Top, Total, Visible, iwt->iwt_SideScrollerScale));

		SetGadgetAttrs(iwt->iwt_PropSide, iwt->iwt_WindowTask.wt_Window, NULL,
			PGA_Total, Total,
			PGA_Visible, Visible,
			PGA_Top, Top,
			TAG_END);
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	// rearrange thumbnails so visible icons are thumbnailed first
	RearrangeThumbnailList(iwt);

	if (iwt->iwt_WindowTask.wt_Window)
		{
		WindowLimits(iwt->iwt_WindowTask.wt_Window,
			iwt->iwt_InnerLeft + iwt->iwt_InnerRight + 100,
			iwt->iwt_InnerTop + iwt->iwt_InnerBottom + 25,
			~0, ~0);
		}
}


static void RedrawIconWindow(struct internalScaWindowTask *iwt, ULONG Flags)
{
	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: iwt_IconTTFont=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_IconTTFont));

	Scalos_DoneRastPort(iwt->iwt_WindowTask.wt_Window->RPort);
	Scalos_SetFont(iwt->iwt_WindowTask.wt_Window->RPort, iwt->iwt_IconFont, &iwt->iwt_IconTTFont);
	SetABPenDrMd(iwt->iwt_WindowTask.wt_Window->RPort, FontPrefs.fprf_FontFrontPen, FontPrefs.fprf_FontBackPen, FontPrefs.fprf_TextDrawMode);

	ScalosLockIconListShared(iwt);

	if (Flags & REDRAWF_ReLayoutIcons)
		{
		ReLayoutIcons(iwt, iwt->iwt_WindowTask.wt_IconList);
		ReLayoutIcons(iwt, iwt->iwt_WindowTask.wt_LateIconList);
		}

	if (!(Flags & REDRAWF_DontEraseWindow))
		{
		EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
			0, 0, 
			iwt->iwt_WindowTask.wt_Window->Width - 1,
			iwt->iwt_WindowTask.wt_Window->Height - 1);
		}

	RefreshIcons(iwt, NULL);

	ScalosUnLockIconList(iwt);

//	if (iwt->iwt_StatusBar)
//		RefreshGList(iwt->iwt_StatusBar, iwt->iwt_WindowTask.wt_Window, NULL, 1);

	if (!(Flags & REDRAWF_DontRefreshWindowFrame))
		RefreshWindowFrame(iwt->iwt_WindowTask.wt_Window);
}


static void ReLayoutIcons(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	ULONG Flags = 0L;

	if (!IsViewByIcon(iwt->iwt_WindowTask.mt_WindowStruct->ws_Viewmodes))
		return;

	d1(kprintf("%s/%s/%ld: in=%08lx\n", __FILE__, __FUNC__, __LINE__, in));

	iwt->iwt_IconTTFont = IconWindowTTFont;
	iwt->iwt_IconFont = IconWindowFont;

	if (iwt->iwt_WinScreen == iwt->iwt_OrigWinScreen)
		Flags |= IOFREELAYOUTF_ScreenAvailable;

	d1(kprintf("%s/%s/%ld: iwt_IconTTFont=%08lx  ttff_Normal=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, iwt->iwt_IconTTFont, iwt->iwt_IconTTFont.ttff_Normal));

	while (in)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		d1(kprintf("%s/%s/%ld: in=%08lx  in_Icon=%08lx\n", __FILE__, __FUNC__, __LINE__, in, in->in_Icon));

		DoMethod(in->in_Icon, IDTM_FreeLayout, Flags);

		SetAttrs(in->in_Icon,
			IDTA_InnerBottom, CurrentPrefs.pref_ImageBorders.Bottom,
			IDTA_InnerRight, CurrentPrefs.pref_ImageBorders.Right,
			IDTA_InnerTop, CurrentPrefs.pref_ImageBorders.Top,
			IDTA_InnerLeft, CurrentPrefs.pref_ImageBorders.Left,
			IDTA_FrameTypeSel, CurrentPrefs.pref_FrameTypeSel,
			IDTA_FrameType, CurrentPrefs.pref_FrameType,
			IDTA_TextSkip, CurrentPrefs.pref_TextSkip,
			IDTA_MultiLineText, (ULONG) CurrentPrefs.pref_IconTextMuliLine,
			IDTA_HalfShinePen, PalettePrefs.pal_PensList[PENIDX_HSHINEPEN],
			IDTA_HalfShadowPen, PalettePrefs.pal_PensList[PENIDX_HSHADOWPEN],
			IDTA_TextPen, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPEN],
			IDTA_TextPenSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENSEL],
			IDTA_TextPenShadow, PalettePrefs.pal_PensList[PENIDX_ICONTEXTSHADOWPEN],
			IDTA_TextPenOutline, PalettePrefs.pal_PensList[PENIDX_ICONTEXTOUTLINEPEN],
			IDTA_TextPenBgSel, PalettePrefs.pal_PensList[PENIDX_ICONTEXTPENBGSEL],
			IDTA_SelectedTextRectangle, CurrentPrefs.pref_SelectedTextRectangle,
			IDTA_SelTextRectBorderX, CurrentPrefs.pref_SelTextRectBorderX,
			IDTA_SelTextRectBorderY, CurrentPrefs.pref_SelTextRectBorderY,
			IDTA_SelTextRectRadius, CurrentPrefs.pref_SelTextRectRadius,
			TIDTA_TextPenDrawerNormal, PalettePrefs.pal_PensList[PENIDX_DRAWERTEXT],
			TIDTA_TextPenDrawerSelected, PalettePrefs.pal_PensList[PENIDX_DRAWERTEXTSEL],
			IDTA_TextMode, CurrentPrefs.pref_TextMode,
			IDTA_TextDrawMode, FontPrefs.fprf_TextDrawMode,
			IDTA_TextBackPen, FontPrefs.fprf_FontBackPen,
			IDTA_Font, (ULONG) iwt->iwt_IconFont,
			IDTA_Fonthandle, (ULONG) &iwt->iwt_IconTTFont,
			IDTA_FontHook, (ULONG) (TTEngineBase ? &ScalosFontHook : NULL),
			TIDTA_Font, (ULONG) iwt->iwt_IconFont,
			TIDTA_TTFont, (ULONG) &iwt->iwt_IconTTFont,
			TAG_END);

		DoMethod(in->in_Icon, IDTM_Layout,
			iwt->iwt_WinScreen, 
			iwt->iwt_WindowTask.wt_Window,
			iwt->iwt_WindowTask.wt_Window->RPort,
			iwt->iwt_WinDrawInfo,
			(gg->Flags & GFLG_SELECTED) ? 
				IOLAYOUTF_NormalImage | IOLAYOUTF_SelectedImage :
				IOLAYOUTF_NormalImage);

		in = (struct ScaIconNode *) in->in_Node.mln_Succ;
		}

	iwt->iwt_OrigWinScreen = iwt->iwt_WinScreen;
}


static void ChangeSelectedIconUpDown(struct internalScaWindowTask *iwt, UWORD Code)
{
	struct ScaIconNode *inSelected;

	ScalosLockIconListShared(iwt);

	// find (first) selected icon
	inSelected = FindFirstSelectedIcon(iwt);

	if (inSelected)
		{
		struct IconListNode *IconList = NULL;
		struct IconListNode *iln;
		struct ScaIconNode *NewSelected = inSelected;

		// build list of all icons in same column as the previously selected one
		BuildIconColumn(&IconList, iwt->iwt_WindowTask.wt_IconList, inSelected);

		for (iln=IconList; iln; iln = (struct IconListNode *) iln->iln_Node.mln_Succ)
			{
			if (iln->iln_IconNode == inSelected)
				{
				switch (Code)
					{
				case CURSORUP:
					if (iln->iln_Node.mln_Pred)
						NewSelected = ((struct IconListNode *) iln->iln_Node.mln_Pred)->iln_IconNode;
					break;
				case CURSORDOWN:
					if (iln->iln_Node.mln_Succ)
						NewSelected = ((struct IconListNode *) iln->iln_Node.mln_Succ)->iln_IconNode;
					break;
					}
				break;
				}
			}

		SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &IconList);

		if (inSelected != NewSelected)
			SelectNewIcon(iwt, NewSelected);
		}

	ScalosUnLockIconList(iwt);
}


static void ChangeSelectedIconLeftRight(struct internalScaWindowTask *iwt, UWORD Code)
{
	struct ScaIconNode *inSelected;

	ScalosLockIconListShared(iwt);

	// find (first) selected icon
	inSelected = FindFirstSelectedIcon(iwt);

	if (inSelected)
		{
		struct IconListNode *IconList = NULL;
		struct IconListNode *iln;
		struct ScaIconNode *NewSelected = inSelected;

		// build list of all icons in same row as the previously selected one
		BuildIconRow(&IconList, iwt->iwt_WindowTask.wt_IconList, inSelected);

		for (iln=IconList; iln; iln = (struct IconListNode *) iln->iln_Node.mln_Succ)
			{
			if (iln->iln_IconNode == inSelected)
				{
				switch (Code)
					{
				case CURSORLEFT:
					if (iln->iln_Node.mln_Pred)
						NewSelected = ((struct IconListNode *) iln->iln_Node.mln_Pred)->iln_IconNode;
					break;
				case CURSORRIGHT:
					if (iln->iln_Node.mln_Succ)
						NewSelected = ((struct IconListNode *) iln->iln_Node.mln_Succ)->iln_IconNode;
					break;
					}
				break;
				}
			}

		SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &IconList);

		if (inSelected != NewSelected)
			SelectNewIcon(iwt, NewSelected);
		}

	ScalosUnLockIconList(iwt);
}


static void ChangeSelectedIconNext(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *inSelected;
	struct IconListNode *IconList = NULL;
	struct ScaIconNode *NewSelected;

	ScalosLockIconListShared(iwt);

	// find (first) selected icon
	inSelected = NewSelected = FindFirstSelectedIcon(iwt);

	// build sorted list of all icons
	BuildIconListSorted(&IconList, iwt->iwt_WindowTask.wt_IconList);

	if (inSelected)
		{
		struct IconListNode *iln;

		for (iln=IconList; iln; iln = (struct IconListNode *) iln->iln_Node.mln_Succ)
			{
			if (iln->iln_IconNode == inSelected)
				{
				if (iln->iln_Node.mln_Succ)
					NewSelected = ((struct IconListNode *) iln->iln_Node.mln_Succ)->iln_IconNode;
				else
					NewSelected = IconList->iln_IconNode;	// Select first node
				break;
				}
			}
		}
	else
		{
		NewSelected = IconList->iln_IconNode;	// Select first node
		}

	SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &IconList);

	if (inSelected != NewSelected)
		SelectNewIcon(iwt, NewSelected);

	ScalosUnLockIconList(iwt);
}


static void ChangeSelectedIconPrev(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *inSelected;
	struct IconListNode *IconList = NULL;
	struct IconListNode *ilnLast;
	struct ScaIconNode *NewSelected;

	ScalosLockIconListShared(iwt);

	// find (first) selected icon
	inSelected = NewSelected = FindFirstSelectedIcon(iwt);

	// build sorted list of all icons
	BuildIconListSorted(&IconList, iwt->iwt_WindowTask.wt_IconList);

	// find last node in list
	for (ilnLast=IconList; ilnLast && ilnLast->iln_Node.mln_Succ; ilnLast = (struct IconListNode *) ilnLast->iln_Node.mln_Succ)
		;

	if (inSelected)
		{
		struct IconListNode *iln;

		for (iln=IconList; iln; iln = (struct IconListNode *) iln->iln_Node.mln_Succ)
			{
			if (iln->iln_IconNode == inSelected)
				{
				if (iln->iln_Node.mln_Pred)
					NewSelected = ((struct IconListNode *) iln->iln_Node.mln_Pred)->iln_IconNode;
				else
					NewSelected = ilnLast->iln_IconNode;	// select last node
				break;
				}
			}
		}
	else
		{
		NewSelected = ilnLast->iln_IconNode;	// select last node
		}

	SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &IconList);

	if (inSelected != NewSelected)
		SelectNewIcon(iwt, NewSelected);

	ScalosUnLockIconList(iwt);
}


static void MakeIconVisible(struct internalScaWindowTask *iwt, struct ScaIconNode *sIcon)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;
	struct ExtGadget *gg = (struct ExtGadget *) sIcon->in_Icon;
	LONG deltaX = 0, deltaY = 0;
	WORD iconLeft, iconTop, iconRight, iconBottom;
	WORD winLeft, winTop, winRight, winBottom;

	iconLeft = gg->BoundsLeftEdge - ws->ws_xoffset;
	iconRight = iconLeft + gg->BoundsWidth - 1;
	iconTop = gg->BoundsTopEdge - ws->ws_yoffset;
	iconBottom = iconTop + gg->BoundsHeight - 1;

	winLeft = 0;
	winTop = 0;
	winRight = iwt->iwt_InnerWidth;
	winBottom = iwt->iwt_InnerHeight;

	d1(kprintf("%s/%s/%ld: iconLeft=%ld  iconRight=%ld  iconTop=%ld  iconBottom=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iconLeft, iconRight, iconTop, iconBottom);)
	d1(kprintf("%s/%s/%ld: winLeft=%ld  winRight=%ld  winTop=%ld  winBottom=%ld\n", \
		__FILE__, __FUNC__, __LINE__, winLeft, winRight, winTop, winBottom);)

	// no horizontal movement for text windows!
	if (IsIwtViewByIcon(iwt))
		{
		if (iconLeft < winLeft)
			{
			// must move Right
			deltaX = iconLeft - winLeft;
			}
		else if (iconRight > winRight)
			{
			// must move iconLeft
			deltaX = iconRight - winRight;
			}
		}

	if (iconTop < winTop)
		{
		// must move DOWN
		deltaY = iconTop - winTop;
		}
	else if (iconBottom > winBottom)
		{
		// must move UP
		deltaY = iconBottom - winBottom;
		}

	d1(kprintf("%s/%s/%ld: deltaX=%ld  deltaY=%ld\n", __FILE__, __FUNC__, __LINE__, deltaX, deltaY);)

	if (deltaX != 0 || deltaY != 0)
		{
		ScalosUnLockIconList(iwt);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_DeltaMove, deltaX, deltaY);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject,
			SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);

		ScalosLockIconListShared(iwt);
		}
}


static void BuildIconColumn(struct IconListNode **DestIconList, struct ScaIconNode *SrcIconList, struct ScaIconNode *inCol)
{
	struct ExtGadget *ggCol = (struct ExtGadget *) inCol->in_Icon;
	struct Hook SortHook;

	while (SrcIconList)
		{
		struct ExtGadget *gg = (struct ExtGadget *) SrcIconList->in_Icon;
		WORD Center = gg->BoundsLeftEdge + gg->BoundsWidth / 2;

		if (Center >= ggCol->BoundsLeftEdge && 
			Center < ggCol->BoundsLeftEdge + ggCol->BoundsWidth)
			{
			struct IconListNode *iln;

			iln = (struct IconListNode *) SCA_AllocNode((struct ScalosNodeList *) DestIconList, 
				sizeof(struct IconListNode) - sizeof(struct MinNode));

			if (iln)
				iln->iln_IconNode = SrcIconList;
			}

		SrcIconList = (struct ScaIconNode *) SrcIconList->in_Node.mln_Succ;
		}

	SETHOOKFUNC(SortHook, IconCompareYFunc);
	SortHook.h_Data = NULL;

	SCA_SortNodes((struct ScalosNodeList *) DestIconList, &SortHook, SCA_SortType_Best);
}


static void BuildIconRow(struct IconListNode **DestIconList, struct ScaIconNode *SrcIconList, struct ScaIconNode *inCol)
{
	struct ExtGadget *ggCol = (struct ExtGadget *) inCol->in_Icon;
	struct Hook SortHook;

	while (SrcIconList)
		{
		struct ExtGadget *gg = (struct ExtGadget *) SrcIconList->in_Icon;
		WORD Center = gg->BoundsTopEdge + gg->BoundsHeight / 2;

		if (Center >= ggCol->BoundsTopEdge && 
			Center < ggCol->BoundsTopEdge + ggCol->BoundsHeight)
			{
			struct IconListNode *iln;

			iln = (struct IconListNode *) SCA_AllocNode((struct ScalosNodeList *) DestIconList, 
				sizeof(struct IconListNode) - sizeof(struct MinNode));

			if (iln)
				iln->iln_IconNode = SrcIconList;
			}

		SrcIconList = (struct ScaIconNode *) SrcIconList->in_Node.mln_Succ;
		}

	SETHOOKFUNC(SortHook, IconCompareXFunc);
	SortHook.h_Data = NULL;

	SCA_SortNodes((struct ScalosNodeList *) DestIconList, &SortHook, SCA_SortType_Best);
}


static void BuildIconListSorted(struct IconListNode **DestIconList, struct ScaIconNode *SrcIconList)
{
	struct Hook SortHook;

	while (SrcIconList)
		{
		struct IconListNode *iln;

		iln = (struct IconListNode *) SCA_AllocNode((struct ScalosNodeList *) DestIconList, 
			sizeof(struct IconListNode) - sizeof(struct MinNode));

		if (iln)
			iln->iln_IconNode = SrcIconList;

		SrcIconList = (struct ScaIconNode *) SrcIconList->in_Node.mln_Succ;
		}

	SETHOOKFUNC(SortHook, IconCompareNameFunc);
	SortHook.h_Data = NULL;

	SCA_SortNodes((struct ScalosNodeList *) DestIconList, &SortHook, SCA_SortType_Best);
}


static SAVEDS(LONG) IconCompareXFunc(struct Hook *hook, struct IconListNode *iln2, struct IconListNode *iln1)
{
	struct ExtGadget *gg1 = (struct ExtGadget *) iln1->iln_IconNode->in_Icon;
	struct ExtGadget *gg2 = (struct ExtGadget *) iln2->iln_IconNode->in_Icon;

	d1(kprintf("%s/%s/%ld: gg1=%08lx  gg2=%08lx  Left1=%ld  Left2=%ld\n", __FILE__, __FUNC__, __LINE__, \
		gg1, gg2, gg1->BoundsLeftEdge, gg2->BoundsLeftEdge));

	return gg2->BoundsLeftEdge - gg1->BoundsLeftEdge;
}


static SAVEDS(LONG) IconCompareYFunc(struct Hook *hook, struct IconListNode *iln2, struct IconListNode *iln1)
{
	struct ExtGadget *gg1 = (struct ExtGadget *) iln1->iln_IconNode->in_Icon;
	struct ExtGadget *gg2 = (struct ExtGadget *) iln2->iln_IconNode->in_Icon;

	d1(kprintf("%s/%s/%ld: gg1=%08lx  gg2=%08lx  Top1=%ld  Top2=%ld\n", \
		__FILE__, __FUNC__, __LINE__, gg1, gg2, gg1->BoundsTopEdge, gg2->BoundsTopEdge));

	return gg2->BoundsTopEdge - gg1->BoundsTopEdge;
}


static SAVEDS(LONG) IconCompareNameFunc(struct Hook *hook, struct IconListNode *iln2, struct IconListNode *iln1)
{
	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, gg1, gg2));

	return Stricmp((STRPTR) GetIconName(iln2->iln_IconNode), (STRPTR) GetIconName(iln1->iln_IconNode));
}


// find first selected icon
static struct ScaIconNode *FindFirstSelectedIcon(struct internalScaWindowTask *iwt)
{
	struct ScaIconNode *in;

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		if (gg->Flags & GFLG_SELECTED)
			{
			return in;
			}
		}

	return NULL;
}


static void SelectIconByName(struct internalScaWindowTask *iwt, CONST_STRPTR Name)
{
	struct ScaIconNode *in;
	size_t Length = strlen(Name);

	if (Length > 0)
		{
		BOOL First = TRUE;

		ScalosLockIconListShared(iwt);

		for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			CONST_STRPTR IconName = GetIconName(in);

			if (IconName && strlen(IconName) >= Length &&
				(0 == Strnicmp((STRPTR) Name, (STRPTR) IconName, Length)))
				{
				// Select all matching icon(s)
				ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, TRUE);

				if (First)
					{
					// Make first matching icon visible
					MakeIconVisible(iwt, in);
					First = FALSE;
					}
				}
			else
				{
				// Deselect all non-matching icon(s)
				ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, FALSE);
				}
			}

		AdjustIconActive(iwt);

		ScalosUnLockIconList(iwt);
		}
}


static void SelectNewIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *NewSelected)
{
	struct ScaIconNode *in;

	for (in=iwt->iwt_WindowTask.wt_IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		ClassSelectIcon(iwt->iwt_WindowTask.mt_WindowStruct, in, in == NewSelected);
		}

	// ..and make sure it is visible
	MakeIconVisible(iwt, NewSelected);

	AdjustIconActive(iwt);
}


static void SetShowType(struct internalScaWindowTask *iwt, ULONG newShowType)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  newShowType=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, newShowType));

	if (IsShowAll(ws) == IsShowAllType(newShowType))
		return;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (IsShowAll(ws))
		{
		ws->ws_Flags &= ~WSV_FlagF_ShowAllFiles;
		ws->ws_ViewAll = iwt->iwt_OldShowType = DDFLAGS_SHOWICONS;
		}
	else
		{
		ws->ws_Flags |= WSV_FlagF_ShowAllFiles;
		ws->ws_ViewAll = iwt->iwt_OldShowType = DDFLAGS_SHOWALL;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (NULL == iwt->iwt_WindowTask.wt_Window)
		return;

	if (iwt->iwt_Reading)
		return;

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	ControlBarUpdateShowMode(iwt);

	// Update status bar display
	if (iwt->iwt_StatusBar)
		{
		if (IsShowAllType(newShowType))
			{
			if (NULL == iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll])
				{
				iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarImage.sca",
					DTA_Name, (ULONG) "THEME:Window/StatusBar/ShowAll",
					GBIDTA_WindowTask, (ULONG) iwt,
					GA_ID, SBAR_GadgetID_ShowAll,
					TAG_END);

				if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll])
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_AddToStatusBar,
						iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll],
                                                TAG_END);
					}
				}
			}
		else
			{
			if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll])
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_RemFromStatusBar,
					iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll]);
				iwt->iwt_StatusBarMembers[STATUSBARGADGET_ShowAll] = NULL;
				}
			}
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (!IsIwtViewByIcon(iwt) || IsShowAllType(newShowType))
		{
		// Switch from "View only icons" to "View all"
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_Reading, TRUE,
			TAG_END);

		SetMenuOnOff(iwt);

		iwt->iwt_RemRegion = (struct Region *) DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_InitClipRegion);
		d1(KPrintF("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));

		if (ScanDirIsError(DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_ReadIconList, SCCV_IconWin_ReadIconList_ShowAll)))
			iwt->iwt_CloseWindow = TRUE;

		DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_RemClipRegion, iwt->iwt_RemRegion);
		iwt->iwt_RemRegion = NULL;
		d1(KPrintF("%s/%s/%ld: iwt_RemRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_RemRegion));

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_Reading, FALSE,
			TAG_END);

		SetMenuOnOff(iwt);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, 
			SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
		}
	else
		{
		// Switch from "View all" to "View only icons"
		struct ScaIconNode *IconList = NULL;
		struct ScaIconNode *in, *inNext;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_Reading, TRUE,
			TAG_END);

		SetMenuOnOff(iwt);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ScalosLockIconListExclusive(iwt);

		// Remove default icons from window
		// but keep all thumbnail icons
		for (in=iwt->iwt_WindowTask.wt_IconList; in; in=inNext)
			{
			ULONG IconUserFlags;

			inNext = (struct ScaIconNode *) in->in_Node.mln_Succ;

			GetAttr(IDTA_UserFlags, in->in_Icon, &IconUserFlags);

			if ((in->in_Flags & INF_DefaultIcon)
				&& !(IconUserFlags & ICONOBJ_USERFLAGF_Thumbnail))
				{
				SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList, &IconList, in);
				}
			}

		ScalosUnLockIconList(iwt);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (IconList)
			{
			RemoveIcons(iwt, &IconList);
			FreeIconList(iwt, &IconList);

			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
				SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);
			}

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		SetAttrs(iwt->iwt_WindowTask.mt_MainObject,
			SCCA_IconWin_Reading, FALSE,
			TAG_END);

		SetMenuOnOff(iwt);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static ULONG IconWindowCleanupBy(struct internalScaWindowTask *iwt, CLEANUPBYSORTFUNC sortFunc)
{
	struct Hook SortHook;
	struct ScaIconNode *in;
	WORD MaxWidth = 0;
	WORD MaxHeight = 0;
	LONG x, y;

	if (!IsIwtViewByIcon(iwt))
		return 0;

	if (!ScalosAttemptLockIconListExclusive(iwt))
		return 0;

	// move all icons from wt_IconList to wt_LateIconList
	while (iwt->iwt_WindowTask.wt_IconList)
		{
		SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_IconList,
			&iwt->iwt_WindowTask.wt_LateIconList,
			iwt->iwt_WindowTask.wt_IconList);
		}

	SETHOOKFUNC(SortHook, sortFunc);
	SortHook.h_Data = NULL;
	SCA_SortNodes((struct ScalosNodeList *) &iwt->iwt_WindowTask.wt_LateIconList, &SortHook, SCA_SortType_Best);

	// calculate maximum icon width/height dimensions
	for (in = iwt->iwt_WindowTask.wt_LateIconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		MaxWidth = max(MaxWidth, gg->BoundsWidth);
		MaxHeight = max(MaxHeight, gg->BoundsHeight);
		}

	d1(kprintf("%s/%s/%ld: MaxWidth=%ld  MaxHeight=%ld\n", __FILE__, __FUNC__, __LINE__, MaxWidth, MaxHeight));

	// now move all icons to new position
	x = CurrentPrefs.pref_CleanupSpace.Left;
	y = CurrentPrefs.pref_CleanupSpace.Top;
	for (in = iwt->iwt_WindowTask.wt_LateIconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		WORD nextX = x + CurrentPrefs.pref_CleanupSpace.XSkip + MaxWidth;
		WORD xOffset;
		WORD yOffset;
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		// center icon inside the (MaxWidth/MaxHeight) icon cell
		xOffset = (MaxWidth - gg->BoundsWidth) / 2;
		yOffset = (MaxHeight - gg->BoundsHeight) / 2;

		if (nextX > (iwt->iwt_InnerWidth - CurrentPrefs.pref_CleanupSpace.Left))
			{
			// icon doesn't fit in window width, start a new row
			x = CurrentPrefs.pref_CleanupSpace.Left;
			y += CurrentPrefs.pref_CleanupSpace.YSkip + MaxHeight;
			}

		gg->LeftEdge = (gg->LeftEdge - gg->BoundsLeftEdge) + x + xOffset;
		gg->BoundsLeftEdge = x + xOffset;

		gg->TopEdge = (gg->TopEdge - gg->BoundsTopEdge) + y + yOffset;
		gg->BoundsTopEdge = y + yOffset;

		x += CurrentPrefs.pref_CleanupSpace.XSkip + MaxWidth;
		}

	// move all icons back to wt_IconList
	while (iwt->iwt_WindowTask.wt_LateIconList)
		{
		SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_LateIconList,
			&iwt->iwt_WindowTask.wt_IconList,
			iwt->iwt_WindowTask.wt_LateIconList);
		}

	ScalosUnLockIconList(iwt);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Redraw, REDRAWF_DontRefreshWindowFrame);
	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustBottomSlider | SETVIRTF_AdjustRightSlider);

	return 1;
}


static SAVEDS(LONG) IconSortByNameFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1)
{
	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	return Stricmp((STRPTR) GetIconName(in2), (STRPTR) GetIconName(in1));
}


static SAVEDS(LONG) IconSortByDateFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1)
{
	LONG Result;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));
/*
	if (!(in2->in_Flags & INF_File))
		{
		if (in1->in_Flags & INF_File)
			return -1;
		}
	else
		{
		if (!(in1->in_Flags & INF_File))
			return 1;
		}
*/
	Result = CompareDates(&in2->in_FileDateStamp, &in1->in_FileDateStamp);

	if (0 == Result)
		Result = Stricmp((STRPTR) GetIconName(in2), (STRPTR) GetIconName(in1));

	return Result;
}


static SAVEDS(LONG) IconSortBySizeFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1)
{
	LONG Result;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));
/*
	if (!(in2->in_Flags & INF_File))
		{
		if (in1->in_Flags & INF_File)
			return -1;
		}
	else
		{
		if (!(in1->in_Flags & INF_File))
			return 1;
		}
*/
	Result = in2->in_FileSize - in1->in_FileSize;

	if (0 == Result)
		Result = Stricmp((STRPTR) GetIconName(in2), (STRPTR) GetIconName(in1));

	return Result;
}


static SAVEDS(LONG) IconSortByTypeFunc(struct Hook *hook, struct ScaIconNode *in2, struct ScaIconNode *in1)
{
	ULONG Type1, Type2;

	d1(kprintf("%s/%s/%ld: in1=%08lx  in2=%08lx\n", __FILE__, __FUNC__, __LINE__, in1, in2));

	if (!(in2->in_Flags & INF_File))
		{
		if (in1->in_Flags & INF_File)
			return -1;
		}
	else
		{
		if (!(in1->in_Flags & INF_File))
			return 1;
		}

	GetAttr(IDTA_Type, in1->in_Icon, &Type1);
	GetAttr(IDTA_Type, in2->in_Icon, &Type2);

	d1(kprintf("%s/%s/%ld: Type1=%ld  Type2=%ld\n", __FILE__, __FUNC__, __LINE__, Type1, Type2));

	if (Type1 != Type2)
		return ((LONG) Type2 - (LONG) Type1);

	return Stricmp((STRPTR) GetIconName(in2), (STRPTR) GetIconName(in1));
}


static void SetThumbnailView(struct internalScaWindowTask *iwt, ULONG NewThumbnailMode)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

	d1(kprintf("%s/%s/%ld: NewThumbnailMode=%ld  iwt_ThumbnailMode=%ld\n", \
		__FILE__, __FUNC__, __LINE__, NewThumbnailMode, iwt->iwt_ThumbnailMode));

	if (iwt->iwt_ThumbnailMode != NewThumbnailMode)
		{
		switch (NewThumbnailMode)
			{
		case THUMBNAILS_Never:
		case THUMBNAILS_Always:
		case THUMBNAILS_AsDefault:
			iwt->iwt_ThumbnailMode = ws->ws_ThumbnailView = NewThumbnailMode;
			break;
		default:
			iwt->iwt_ThumbnailMode = ws->ws_ThumbnailView = CurrentPrefs.pref_ShowThumbnails;
			break;
			}

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_Update);

		if (iwt->iwt_StatusBar)
			{
			CONST_STRPTR GadgetName;
			ULONG GadgetID = 0;

			if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_Thumbnails])
				{
				DoMethod(iwt->iwt_WindowTask.mt_MainObject,
					SCCM_IconWin_RemFromStatusBar,
					iwt->iwt_StatusBarMembers[STATUSBARGADGET_Thumbnails]);
				iwt->iwt_StatusBarMembers[STATUSBARGADGET_Thumbnails] = NULL;
				}

			switch (iwt->iwt_ThumbnailMode)
				{
			case THUMBNAILS_Always:
				GadgetName = "THEME:Window/StatusBar/ThumbnailsAlways";
				GadgetID = SBAR_GadgetID_ThumbnailsAlways;
				break;
			case THUMBNAILS_AsDefault:
				GadgetName = "THEME:Window/StatusBar/ThumbnailsAsDefault";
				GadgetID = SBAR_GadgetID_ThumbnailsAsDefault;
				break;
			case THUMBNAILS_Never:
			default:
				GadgetName = NULL;
				break;
				}
			if (GadgetName)
				{
				iwt->iwt_StatusBarMembers[STATUSBARGADGET_Thumbnails] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarImage.sca",
					DTA_Name, (ULONG) GadgetName,
					GBIDTA_WindowTask, (ULONG) iwt,
					GA_ID, GadgetID,
					TAG_END);

				if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_Thumbnails])
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_AddToStatusBar,
						iwt->iwt_StatusBarMembers[STATUSBARGADGET_Thumbnails],
                                                TAG_END);
					}
				}
			}
		}
}


static void SetThumbnailsGenerating(struct internalScaWindowTask *iwt, BOOL ThumbnailsGenerating)
{
	if (ThumbnailsGenerating != iwt->iwt_ThumbnailGenerationPending)
		{
		if (iwt->iwt_StatusBar)
			{
			if (ThumbnailsGenerating)
				{
				if (NULL == iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate])
					{
					iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate] = (struct Gadget *) SCA_NewScalosObjectTags("GadgetBarImage.sca",
						DTA_Name, (ULONG) "THEME:Window/StatusBar/ThumbnailsGenerate",
						GBIDTA_WindowTask, (ULONG) iwt,
						GA_ID, SBAR_GadgetID_ThumbnailsGenerate,
						TAG_END);

					if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate])
						{
						DoMethod(iwt->iwt_WindowTask.mt_MainObject,
							SCCM_IconWin_AddToStatusBar,
							iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate],
                                                        TAG_END);
						}
					}
				}
			else
				{
				if (iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate])
					{
					DoMethod(iwt->iwt_WindowTask.mt_MainObject,
						SCCM_IconWin_RemFromStatusBar,
						iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate]);
					iwt->iwt_StatusBarMembers[STATUSBARGADGET_ThumbnailsGenerate] = NULL;
					}
				}
			}

		iwt->iwt_ThumbnailGenerationPending = ThumbnailsGenerating;
		}
}


static void GetIconsTotalBoundingBox(struct internalScaWindowTask *iwt)
{
	const struct ScaIconNode *in;

	ScalosLockIconListShared(iwt);

	iwt->iwt_IconBBox.MinX = iwt->iwt_IconBBox.MinY = LONG_MAX;
	iwt->iwt_IconBBox.MaxX = iwt->iwt_IconBBox.MaxY = 0;

	for (in = iwt->iwt_WindowTask.wt_IconList; in; in = (const struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		const struct ExtGadget *gg = (const struct ExtGadget *) in->in_Icon;

		d1(kprintf("%s/%s/%ld: <%s> Left=%ld Top=%ld Width=%ld Height=%ld\n", \
			__FILE__, __FUNC__, __LINE__, GetIconName(in), gg->BoundsLeftEdge, gg->BoundsTopEdge, gg->BoundsWidth, gg->BoundsHeight));

		if (gg->BoundsLeftEdge < iwt->iwt_IconBBox.MinX)
			iwt->iwt_IconBBox.MinX = gg->BoundsLeftEdge;
		if (gg->BoundsLeftEdge + gg->BoundsWidth >= iwt->iwt_IconBBox.MaxX)
			iwt->iwt_IconBBox.MaxX = gg->BoundsLeftEdge + gg->BoundsWidth;
		if (gg->BoundsTopEdge < iwt->iwt_IconBBox.MinY)
			iwt->iwt_IconBBox.MinY = gg->BoundsTopEdge;
		if (gg->BoundsTopEdge + gg->BoundsHeight >= iwt->iwt_IconBBox.MaxY)
			iwt->iwt_IconBBox.MaxY = gg->BoundsTopEdge + gg->BoundsHeight;
		}

	ScalosUnLockIconList(iwt);
}


static void IconWindowUpdateTransparency(struct internalScaWindowTask *iwt)
{
	struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

	if (ws->ws_Window && !iwt->iwt_BackDrop)
		{
		// update window transparency
		SetAttrs(iwt->iwt_WindowTask.mt_WindowObject,
			SCCA_Window_Transparency,
				(ws->ws_Window->Flags & WFLG_WINDOWACTIVE)
					? ws->ws_WindowOpacityActive
					: ws->ws_WindowOpacityInactive,
			TAG_END);
		}
}


static void SendNewIconPathMsg(struct internalScaWindowTask *iwt, CONST_STRPTR Path, struct TagItem *TagList)
{
	struct SM_NewWindowPath *smnwp;

	smnwp = (struct SM_NewWindowPath *) SCA_AllocMessage(MTYP_NewWindowPath, 0);

	if (smnwp)
		{
		struct ScaWindowStruct *ws = iwt->iwt_WindowTask.mt_WindowStruct;

		smnwp->smnwp_Path = AllocCopyString(Path);
		smnwp->smnwp_TagList = CloneTagItems((struct TagItem *) TagList);
		PutMsg(ws->ws_MessagePort, &smnwp->ScalosMessage.sm_Message);
		}
}
