// DragDrop.c
// $Date$
// $Revision$
// $Id$


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
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#ifdef __AROS__
#include <proto/popupmenu.h>
#else
#include <proto/pm.h>
#endif
#include <proto/icon.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosmenuplugin.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/menu.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <DefIcons.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"
#include "DtImageClass.h"

//----------------------------------------------------------------------------

// local data definitions

#define DROPMENU

enum DropMenuImgIndex
	{
	DROPMENUIMAGE_Abort = 0,
	DROPMENUIMAGE_Copy,
	DROPMENUIMAGE_Move,
	DROPMENUIMAGE_CreateLink,

	DROPMENUIMAGE_MAX
	};

//----------------------------------------------------------------------------

// local functions
static SAVEDS(void) ASM INTERRUPT DropTask(void);
static struct DragNode *FindIconInDragList(struct internalScaWindowTask *iwt, const struct ScaIconNode *in);
static void DragDropFinish(struct internalScaWindowTask *iwt, struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest, LONG x, LONG y, struct DropOps *drops);
static void Icon2IconDrop(struct ScalosArg **ArgList,
	struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest,
	struct DropInfo *DrInfo);
static void Icon2DesktopDrop(struct ScalosArg **, struct ScaWindowStruct *Src,
	struct ScaWindowStruct *Dest,
	struct DropInfo *DrInfo);
static void Desktop2DesktopDrop(struct ScalosArg **, struct ScaWindowStruct *Src,
	struct ScaWindowStruct *Dest, struct DropInfo *DrInfo);
static void Desktop2IconDrop(struct ScalosArg **, struct ScaWindowStruct *Src,
	struct ScaWindowStruct *Dest, struct DropInfo *DrInfo);
static void DropStart(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest,
	struct ScaIconNode *in);
static ULONG Icon2IconDrop_Drawer(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep);
static ULONG Icon2IconDrop_File(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep);
static ULONG Icon2Icon_MoveDir(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep);
static ULONG Icon2Icon_CopyDir(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, CONST_STRPTR DestName, APTR undoStep);
static ULONG Icon2Icon_CreateLink(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep);
static ULONG DropAddIcon(struct MsgPort *ReplyPort, struct ScaWindowStruct *wsDest,
	BPTR DirLock, STRPTR IconName, WORD x, WORD y);
static void DropRemoveIcon(struct ScaWindowStruct *wsDest, BPTR DirLock, CONST_STRPTR IconName);
static void DronOnAppIcon(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtFound,
	struct ScaIconNode *in, LONG x, LONG y);
static void DronOnAppObject(struct internalScaWindowTask *iwt, struct AppObject *ao, LONG x, LONG y);
static void SameWindow(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtFound, LONG x, LONG y);
static void SameWindow2(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtFound, LONG x, LONG y);
static ULONG DragQuery_IconWin(struct ScaIconNode *, ULONG IconType);
static void MoveIconToNewPosition(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, WORD xNew, WORD yNew);
static BOOL DropPopupMenu(struct internalScaWindowTask *iwtSrc,
	struct DropInfo *DrInfo, const struct DropOps *AllowedDrops);

//----------------------------------------------------------------------------

// local data items

//----------------------------------------------------------------------------

void DragDrop(struct Window *win, LONG MouseX, LONG MouseY, ULONG Qualifier,
	struct internalScaWindowTask *iwt)
{
	struct internalScaWindowTask *iwtDest;
	struct ScaIconNode *inDest;
	enum ObjTypes ObjUnderMouse;
	LONG x = 0, y = 0;
	struct DropOps drops;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	drops.drop_Copy = drops.drop_Move = drops.drop_CreateLink = drops.drop_PutAway = drops.drop_LeaveOut = FALSE;
	if (isMakeLinkQualifier(Qualifier))
		{
		drops.drop_CreateLink = TRUE;
		}
	else if (isMoveQualifier(Qualifier))
		{
		drops.drop_Move = TRUE;
		}
	else if (isCopyQualifier(Qualifier))
		{
		drops.drop_Copy = TRUE;
		}

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	iwtDest = iwt->iwt_WinUnderPtr;
	inDest = iwt->iwt_IconUnderPtr;

	ObjUnderMouse = iwt->iwt_ObjectUnderMouse;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  MayDrop=%ld  iwtDest=<%s> inDest=%08lx  ObjType=%ld\n", __FILE__, __FUNC__, __LINE__, \
		iwt, iwt->iwt_DragMayDrop, iwt->iwt_WinTitle, inDest, ObjUnderMouse));
	d1(kprintf("%s/%s/%ld: WinUnderPtr=%08lx  IconUnderPtr=%08lx\n", __FILE__, __FUNC__, __LINE__, \
	 	iwt->iwt_WinUnderPtr, iwt->iwt_IconUnderPtr));

	IconWin_EndDrag(iwt);		// Signal Mouse leaving window/icon

	if (!iwt->iwt_DragMayDrop)
		{
		d1(KPrintF("%s/%s/%ld: No Drop allowed here\n", __FILE__, __FUNC__, __LINE__));

		SCA_UnLockWindowList();

		if (!CurrentPrefs.pref_EnableDropMenu)
			ClosePopupWindows(TRUE);

		if (iwt->iwt_DragIconList)
			{
			RestoreDragIcons(iwt);
			DisplayBeep(iwt->iwt_WinScreen);
			}
		return;
		}

	if (iwtDest)
		{
		x = MouseX + iwt->iwt_WindowTask.wt_Window->LeftEdge - iwtDest->iwt_WindowTask.wt_Window->LeftEdge
			- iwtDest->iwt_InnerLeft + iwtDest->iwt_WindowTask.wt_XOffset;
		y = MouseY + iwt->iwt_WindowTask.wt_Window->TopEdge - iwtDest->iwt_WindowTask.wt_Window->TopEdge
			- iwtDest->iwt_InnerTop + iwtDest->iwt_WindowTask.wt_YOffset;
		}

	d1(kprintf("%s/%s/%ld: MouseX=%ld  MouseY=%ld  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, MouseX, MouseY, x, y));

	switch (ObjUnderMouse)
		{
	case OUM_ScalosWindow:
		if (iwtDest)
			{
			d1(kprintf("%s/%s/%ld: Drop on Scalos Window \n", __FILE__, __FUNC__, __LINE__));
			if (iwt == iwtDest && !drops.drop_Copy)
				SameWindow(iwt, iwtDest, x, y);
			else
				DragDropFinish(iwt, iwt->iwt_WindowTask.mt_WindowStruct, iwtDest->iwt_WindowTask.mt_WindowStruct, x, y, &drops);

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
#if 0
			// if iwtDest was a Popup window, close it now!
			if (iwtDest->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_DdPopupWindow)
				{
				struct SM_CloseWindow *msg;

				msg = (struct SM_CloseWindow *) SCA_AllocMessage(MTYP_CloseWindow, 0);
				if (msg)
					{
					msg->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
					PutMsg(iwtDest->iwt_WindowTask.mt_WindowStruct->ws_MessagePort, &msg->ScalosMessage.sm_Message);
					}
				}
#endif
			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}
		break;

	case OUM_ForeignWindow:
		d1(kprintf("%s/%s/%ld: Drop on foreign Window\n", __FILE__, __FUNC__, __LINE__));
		RestoreDragIcons(iwt);
		DisplayBeep(iwt->iwt_WinScreen);
		break;

	case OUM_AppWindow:
		d1(kprintf("%s/%s/%ld: Drop on AppWindow\n", __FILE__, __FUNC__, __LINE__));
		if (iInfos.xii_iinfos.ii_AppWindowStruct)
			{
			struct internalScaWindowTask *iwtx = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
			struct AppObject *ao;

			ScalosObtainSemaphoreShared(iwtx->iwt_AppListSemaphore);

			for (ao = iwtx->iwt_AppList; ao; ao = (struct AppObject *) ao->appo_Node.mln_Succ)
				{
				if (APPTYPE_AppWindow == ao->appo_type &&
					ao->appo_object.appoo_Window == win)
					{
					// AppWindow found
					ScalosReleaseSemaphore(iwtx->iwt_AppListSemaphore);

					DronOnAppObject(iwt, ao, MouseX, MouseY);

					SCA_UnLockWindowList();
					RestoreDragIcons(iwt);
					return;
					}
				}

			// non-AppWindow
			ScalosReleaseSemaphore(iwtx->iwt_AppListSemaphore);

			d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			DisplayBeep(iwt->iwt_WinScreen);
			}
		break;

	case OUM_Icon:
		d1(KPrintF("%s/%s/%ld: Drop on Scalos Icon\n", __FILE__, __FUNC__, __LINE__));

		if (NULL == FindIconInDragList(iwt, inDest))
			{
			ULONG IconType;

			GetAttr(IDTA_Type, inDest->in_Icon, &IconType);

			d1(kprintf("%s/%s/%ld: Icon=<%s> Type=%ld\n", __FILE__, __FUNC__, __LINE__, GetIconName(inDest), IconType));

			switch (IconType)
				{
			case WBAPPICON:
				DronOnAppIcon(iwt, iwtDest, inDest, x, y);
				break;
			case WBTOOL:
			case WB_TEXTICON_TOOL:
				if (CurrentPrefs.pref_DragStartFlag)
					DropStart(iwt, iwtDest, inDest);
				break;
			default:
				if (iwt == iwtDest)
					{
					if (DragQuery_IconWin(inDest, IconType))
						{
						RestoreDragIcons(iwt);
						DragDropFinish(iwt, iwt->iwt_WindowTask.mt_WindowStruct, iwtDest->iwt_WindowTask.mt_WindowStruct, x, y, &drops);
						}
					else
						{
						SameWindow(iwt, iwtDest, x, y);
						}
					}
				else
					{
					DragDropFinish(iwt, iwt->iwt_WindowTask.mt_WindowStruct, iwtDest->iwt_WindowTask.mt_WindowStruct, x, y, &drops);
					}
				break;
				}
			}
		else
			{
			if (iwt == iwtDest)
				SameWindow(iwt, iwtDest, x, y);
			}
		break;
	default:
		d1(KPrintF("%s/%s/%ld: Just moved around icon\n", __FILE__, __FUNC__, __LINE__));
		break;
		}

	RestoreDragIcons(iwt);
	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static SAVEDS(void) ASM INTERRUPT DropTask(void)
{
	struct Process *myProc = (struct Process *) FindTask(NULL);
	struct SM_DropProc *sMsg;

	do	{
		sMsg = (struct SM_DropProc *) GetMsg(&myProc->pr_MsgPort);

		if (NULL == sMsg)
			WaitPort(&myProc->pr_MsgPort);
		} while (NULL == sMsg);

	d1(kprintf("%s/%s/%ld: sMsg=%08lx  Type=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg, sMsg->sdpm_ScalosMessage.sm_MessageType));

	if (ID_IMSG == sMsg->sdpm_ScalosMessage.sm_Signature &&
		MTYP_DropProc == sMsg->sdpm_ScalosMessage.sm_MessageType)
		{
		struct ScalosArg *sarg;
		ULONG ArgCount;
		struct ScaWindowStruct *wsSrc, *wsDest;
		struct ScalosArg *ArgList;
		DROPFUNC DropFunc;
		struct DropInfo DrInfo;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ScalosObtainSemaphoreShared(sMsg->sdpm_SourceWindowStruct->ws_WindowTask->wt_WindowSemaphore);
		ScalosObtainSemaphoreShared(sMsg->sdpm_DestWindowStruct->ws_WindowTask->wt_WindowSemaphore);

		DrInfo.drin_LastDropMenuResult = DROPMENURESULT_Unknown;

		DrInfo.drin_x = sMsg->sdpm_MouseX;
		DrInfo.drin_y = sMsg->sdpm_MouseY;
		DrInfo.drin_Arg = NULL;
		DrInfo.drin_DestLock = (BPTR)NULL;
		wsSrc = sMsg->sdpm_SourceWindowStruct;
		wsDest = sMsg->sdpm_DestWindowStruct;
		DrInfo.drin_Drops = sMsg->sdpm_Drops;
		DropFunc = sMsg->sdpm_DropFunc;
		ArgList = sMsg->sdpm_ArgList;

		ReplyMsg((struct Message *) sMsg);

		for (sarg=ArgList, ArgCount=0; sarg; sarg = (struct ScalosArg *) sarg->scarg_Node.mln_Succ)
			ArgCount++;

		d1(kprintf("%s/%s/%ld: ArgList=%08lx  ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgList, ArgCount));

		DrInfo.drin_FileTransObj = SCA_NewScalosObjectTags((STRPTR) "FileTransfer.sca",
			SCCA_FileTrans_Screen, (ULONG) iInfos.xii_iinfos.ii_Screen,
			SCCA_FileTrans_Number, ArgCount,
			SCCA_FileTrans_ReplaceMode, SCCV_ReplaceMode_Ask,
			TAG_END);

		d1(KPrintF("%s/%s/%ld: FileTransObj=%08lx  wsSrc=%08lx  wsDest=%08lx\n", __FILE__, __FUNC__, __LINE__, \
			DrInfo.drin_FileTransObj, wsSrc, wsDest));

		if (DrInfo.drin_FileTransObj)
			{
			(*DropFunc)(&ArgList, wsSrc, wsDest, &DrInfo);

			SCA_DisposeScalosObject(DrInfo.drin_FileTransObj);
			}

		d1(kprintf("%s/%s/%ld: ClassDragFinish_IconWin(ArgList=%08lx)\n", __FILE__, __FUNC__, __LINE__, ArgList));
		ClassDragFinish_IconWin(&ArgList);
		}
	else
		ReplyMsg((struct Message *) sMsg);
}


static struct DragNode *FindIconInDragList(struct internalScaWindowTask *iwt, const struct ScaIconNode *in)
{
	struct DragNode *dn;

	for (dn = iwt->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		if (dn->drgn_iconnode == in)
			{
			return dn;
			}
		}

	return NULL;
}


static void DragDropFinish(struct internalScaWindowTask *iwt, struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest, LONG x, LONG y, struct DropOps *drops)
{
	struct ScalosArg *argList;

	d1(KPrintF("%s/%s/%ld: Started: - iwt=%08lx  wsSrc=<%s>  wsDest=<%s>  x=%ld  y=%ld  Copy=%ld  Move=%ld  CreateLink=%ld\n", \
		__FILE__, __FUNC__, __LINE__, iwt, wsSrc->ws_Name, wsDest->ws_Name, x, y, drops->drop_Copy, drops->drop_Move, drops->drop_CreateLink));

	RestoreDragIcons(iwt);

	if (NULL == wsSrc || NULL == wsDest)
		return;

	argList = NULL;

	switch (wsSrc->ws_WindowType)
		{
	case WSV_Type_DeviceWindow:
		d1(kprintf("%s/%s/%ld: DeviceWindow\n", __FILE__, __FUNC__, __LINE__));
		ClassDragBegin_DeviceWin(&argList, &iwt->iwt_DragNodeList);
		break;
	case WSV_Type_IconWindow:
		d1(kprintf("%s/%s/%ld: IconWindow CALL ClassDragBegin_IconWin(&argList, &iwt->iwt_DragNodeList)\n", __FILE__, __FUNC__, __LINE__));
		ClassDragBegin_IconWin(&argList, &iwt->iwt_DragNodeList);
		break;
		}

	d1(kprintf("%s/%s/%ld: Src WindowType=%ld  argList=%08lx\n", __FILE__, __FUNC__, __LINE__, wsSrc->ws_WindowType, argList));

	if (argList)
		{
		DROPFUNC DropProc = NULL;

		if (WSV_Type_IconWindow == wsSrc->ws_WindowType && WSV_Type_IconWindow == wsDest->ws_WindowType)
			{
			d1(kprintf("%s/%s/%ld: Icon 2 Icon\n",__FILE__, __FUNC__, __LINE__));
			DropProc = Icon2IconDrop;
			}
		else if (WSV_Type_IconWindow == wsSrc->ws_WindowType && WSV_Type_DeviceWindow == wsDest->ws_WindowType)
			{
			d1(kprintf("%s/%s/%ld: Icon 2 Device\n", __FILE__, __FUNC__, __LINE__));
			DropProc = Icon2DesktopDrop;
			}
		else if (WSV_Type_DeviceWindow == wsSrc->ws_WindowType && WSV_Type_IconWindow == wsDest->ws_WindowType)
			{
			d1(kprintf("%s/%s/%ld: Device 2 Icon\n", __FILE__, __FUNC__, __LINE__));
			DropProc = Desktop2IconDrop;
			}
		else if (WSV_Type_DeviceWindow == wsSrc->ws_WindowType && WSV_Type_DeviceWindow == wsDest->ws_WindowType)
			{
			d1(kprintf("%s/%s/%ld: Device 2 Device\n", __FILE__, __FUNC__, __LINE__));
			DropProc = Desktop2DesktopDrop;
			}

		if (DropProc)
			{
			struct SM_DropProc *sMsg;

			sMsg = (struct SM_DropProc *) SCA_AllocMessage(MTYP_DropProc, 0);

			if (sMsg)
				{
				STATIC_PATCHFUNC(DropTask)
				struct Process *proc;

				sMsg->sdpm_ScalosMessage.sm_Message.mn_ReplyPort = iwt->iwt_WindowTask.wt_IconPort;
				sMsg->sdpm_DropFunc = DropProc;
				sMsg->sdpm_SourceWindowStruct = wsSrc;
				sMsg->sdpm_DestWindowStruct =  wsDest;
				sMsg->sdpm_ArgList = argList;
				sMsg->sdpm_MouseX = x;
				sMsg->sdpm_MouseY = y;
				sMsg->sdpm_Drops = *drops;

				// CreateNewProc()
				proc = CreateNewProcTags(NP_WindowPtr, NULL,
					NP_StackSize, 8192,
					NP_Cli, TRUE,
					NP_CommandName, (ULONG) "Scalos_Drag&Drop",
					NP_Name, (ULONG) "Scalos_Drag&Drop",
					NP_Entry, (ULONG) PATCH_NEWFUNC(DropTask),
					NP_Path, DupWBPathList(),
					TAG_END);

				d1(kprintf("%s/%s/%ld: proc=%08lx\n", __FILE__, __FUNC__, __LINE__, proc));

				if (proc)
					{
					d1(kprintf("%s/%s/%ld: proc=%08lx\n", __FILE__, __FUNC__, __LINE__, proc));

					PutMsg(&proc->pr_MsgPort, &sMsg->sdpm_ScalosMessage.sm_Message);

					d1(kprintf("%s/%s/%ld: CALL WaitReply for: (%s)\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WinTitle));

					WaitReply(iwt->iwt_WindowTask.wt_IconPort, iwt, MTYP_DropProc);

					}
				else
					{
					d1(kprintf("%s/%s/%ld: CALL ClassDragFinish_IconWin(argList)\n", __FILE__, __FUNC__, __LINE__));
					ClassDragFinish_IconWin(&argList);
					}
				}
			}
		else
			{
			d1(kprintf("%s/%s/%ld: DropProc = NULL - CALL ClassDragFinish_IconWin(argList)\n", __FILE__, __FUNC__, __LINE__));
			ClassDragFinish_IconWin(&argList);

			}

		}

// .drag_error:

	// d1(kprintf("%s/%s/%ld: CALL IconWin_EndDrag(iwt) \n", __FILE__, __FUNC__, __LINE__));

	IconWin_EndDrag(iwt); // Signal Mouse leaving window/icon.

	// d1(kprintf("%s/%s/%ld: Finished. \n", __FILE__, __FUNC__, __LINE__));
}


static void DropStart(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest,
	struct ScaIconNode *inDest)
{
	struct WBArg *Buffer;
	ULONG ArgCount = 0;

	d1(KPrintF("%s/%s/%ld: Started: iwt=%08lx  icon=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, inDest, GetIconName(inDest)));
	d1(KPrintF("%s/%s/%ld: iwt_DragIconList=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragIconList));

	do	{
		struct ScaIconNode *in;
		ULONG NumberOfWbArgs;
		struct WBArg *wbArg;

		for (NumberOfWbArgs = 1, in = iwt->iwt_DragIconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
			{
			NumberOfWbArgs++;
			}

		d1(KPrintF("%s/%s/%ld: NumberOfWbArgs=%lu\n", __FILE__, __FUNC__, __LINE__, NumberOfWbArgs));

		Buffer = ScalosAlloc(NumberOfWbArgs * sizeof(struct WBArg));
		d1(KPrintF("%s/%s/%ld: Buffer=%08lx\n", __FILE__, __FUNC__, __LINE__, Buffer));
		if (NULL == Buffer)
			break;

		memset(Buffer, 0, NumberOfWbArgs * sizeof(struct WBArg));

		// Add destination as first argument
		if (!DoMethod(iwtDest->iwt_WindowTask.mt_MainObject, SCCM_IconWin_MakeWBArg, inDest, Buffer))
			break;

		// Now add all icons from iwt_DragIconList
		for (ArgCount = 1, in = iwt->iwt_DragIconList, wbArg = Buffer + 1;
				in; in = (struct ScaIconNode *) in->in_Node.mln_Succ, wbArg++)
			{
			d1(KPrintF("%s/%s/%ld: add icon=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, in, GetIconName(in)));
			ArgCount += DoMethod(iwt->iwt_WindowTask.mt_MainObject,
				SCCM_IconWin_MakeWBArg, in, wbArg);
			}

		d1(KPrintF("%s/%s/%ld: ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));

		RestoreDragIcons(iwt);

		SCA_WBStart(Buffer, NULL, ArgCount);
		} while (0);

	if (Buffer)
		{
		SCA_FreeWBArgs(Buffer, ArgCount, SCAF_FreeNames);
		ScalosFree(Buffer);
		}
	if (iwt->iwt_DragIconList)
		RestoreDragIcons(iwt);

	d1(KPrintF("%s/%s/%ld: Finished: iwt=%08lx  icon=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, inDest, GetIconName(inDest)));
}


static void Icon2IconDrop(struct ScalosArg **ArgList,
	struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest,
	struct DropInfo *DrInfo)
{
	struct internalScaWindowTask *iwtSrc, *iwtDest;
	BPTR SrcLock = (BPTR)NULL;
	BPTR oldDir = NOT_A_LOCK;
	BOOL WindowsLocked = TRUE;

	if (NULL == wsSrc || NULL == wsDest)
		return;

	iwtSrc = (struct internalScaWindowTask *) wsSrc->ws_WindowTask;
	iwtDest = (struct internalScaWindowTask *) wsDest->ws_WindowTask;

	if (NULL == iwtSrc || NULL == iwtDest)
		return;

	d1(kprintf("%s/%s/%ld: ArgList=%08lx\n", __FILE__, __FUNC__, __LINE__, ArgList));
	d1(kprintf("%s/%s/%ld: Src=%08lx <%s>  Dest=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		wsSrc, wsSrc->ws_Name, wsDest, wsDest->ws_Name));

	do	{
		APTR undoStep;
		ULONG Result = RETURN_OK;
		struct ScaIconNode *in;

		if (wsSrc->ws_Lock)
			{
			SrcLock = DupLock(wsSrc->ws_Lock);
			if ((BPTR)NULL == SrcLock)
				break;
			}
		if (wsDest->ws_Lock)
			{
			DrInfo->drin_DestLock = DupLock(wsDest->ws_Lock);
			if ((BPTR)NULL == DrInfo->drin_DestLock)
				break;
			}

		oldDir = CurrentDir(DrInfo->drin_DestLock);

		ScalosLockIconListShared(iwtDest);

		in = CheckMouseIcon(&iwtDest->iwt_WindowTask.wt_IconList,
			iwtDest, DrInfo->drin_x, DrInfo->drin_y);
		if (in)
			{
			BPTR IconLock = (BPTR)NULL;

			if (in->in_Lock)
				{
				BPTR oldDir2 = CurrentDir(in->in_Lock);

				IconLock = Lock((STRPTR) GetIconName(in), ACCESS_READ);
				CurrentDir(oldDir2);
				}
			else
				{
				ULONG IconType;

				GetAttr(IDTA_Type, in->in_Icon, &IconType);
				d1(kprintf("%s/%s/%ld: in=%08lx <%s>  IconType=%ld\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in), IconType));

				switch (IconType)
					{
				case WBDISK:
					IconLock = DiskInfoLock(in);
					break;

				case WBDRAWER:
				case WBGARBAGE:
				case WB_TEXTICON_DRAWER:
					IconLock = Lock((STRPTR) GetIconName(in), ACCESS_READ);
					break;
					}
				}

			if (IconLock)
				{
				UnLock(DrInfo->drin_DestLock);
				DrInfo->drin_DestLock = IconLock;
				DrInfo->drin_x = NO_ICON_POSITION_SHORT;
				DrInfo->drin_y = NO_ICON_POSITION_SHORT;
				}
			}

		ScalosUnLockIconList(iwtDest);

		CurrentDir(oldDir);
		oldDir = NOT_A_LOCK;

		ScalosReleaseSemaphore(iwtSrc->iwt_WindowTask.wt_WindowSemaphore);
		ScalosReleaseSemaphore(iwtDest->iwt_WindowTask.wt_WindowSemaphore);
		WindowsLocked = FALSE;

		undoStep = UndoBeginStep();

		for (DrInfo->drin_Arg = *ArgList;
				RESULT_UserAborted != Result && DrInfo->drin_Arg;
				DrInfo->drin_Arg = (struct ScalosArg *) DrInfo->drin_Arg->scarg_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: DrInfo->drin_Arg=%08lx <%s>  IconType=%ld\n", \
				__FILE__, __FUNC__, __LINE__, \
				DrInfo->drin_Arg, DrInfo->drin_Arg->scarg_name, DrInfo->drin_Arg->scarg_icontype));

			switch (DrInfo->drin_Arg->scarg_icontype)
				{
			case WBDRAWER:
			case WB_TEXTICON_DRAWER:
			case WBGARBAGE:
				Result = Icon2IconDrop_Drawer(iwtSrc, DrInfo, undoStep);
				break;
			case WBTOOL:
			case WB_TEXTICON_TOOL:
				Result = Icon2IconDrop_File(iwtSrc, DrInfo, undoStep);
				break;
				}
			}

		UndoEndStep(iwtSrc, undoStep);

		d1(kprintf("%s/%s/%ld: ClassDragFinish_IconWin(ArgList=%08lx)\n", __FILE__, __FUNC__, __LINE__, ArgList));
		ClassDragFinish_IconWin(ArgList);
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (SrcLock)
		UnLock(SrcLock);
	if (DrInfo->drin_DestLock)
		{
		UnLock(DrInfo->drin_DestLock);
		DrInfo->drin_DestLock = (BPTR)NULL;
		}

	if (WindowsLocked)
		{
		ScalosReleaseSemaphore(iwtSrc->iwt_WindowTask.wt_WindowSemaphore);
		ScalosReleaseSemaphore(iwtDest->iwt_WindowTask.wt_WindowSemaphore);
		}
}


static ULONG Icon2IconDrop_Drawer(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep)
{
	LONG sameLock = ScaSameLock(DrInfo->drin_Arg->scarg_lock, DrInfo->drin_DestLock);
	STRPTR DestName = NULL;
	ULONG Result = RETURN_OK;
	struct DropOps AllowedDrops;

	debugLock_d1(DrInfo->drin_Arg->scarg_lock);
	debugLock_d1(DrInfo->drin_DestLock);

	d1(KPrintF("%s/%s/%ld: sameLock=%ld\n", __FILE__, __FUNC__, __LINE__, sameLock));

	AllowedDrops.drop_CreateLink = TRUE;
	AllowedDrops.drop_Copy = TRUE;
	AllowedDrops.drop_Move = LOCK_SAME != sameLock;

	if (!DropPopupMenu(iwtSrc, DrInfo, &AllowedDrops))
		return RETURN_OK;

	if (DrInfo->drin_Drops.drop_CreateLink)
		{
		return Icon2Icon_CreateLink(iwtSrc, DrInfo, undoStep);
		}
	else if (DrInfo->drin_Drops.drop_Move)
		{
		// force move - do nothing if same directory
		if (LOCK_SAME != sameLock)
			sameLock = LOCK_SAME_VOLUME;
		}
	else if (DrInfo->drin_Drops.drop_Copy)
		{
		// source and dest are in same directory

		if (LOCK_SAME == sameLock)
			{
			DestName = AllocPathBuffer();

			BumpRevision(DestName, DrInfo->drin_Arg->scarg_name);
			}

		// force copy
		sameLock = LOCK_DIFFERENT;
		}

	d1(kprintf("%s/%s/%ld: sameLock=%ld\n", __FILE__, __FUNC__, __LINE__, sameLock));

	switch (sameLock)
		{
	case LOCK_SAME:
		return RETURN_OK;

	case LOCK_DIFFERENT:
		Result = Icon2Icon_CopyDir(iwtSrc, DrInfo, DestName, undoStep);
		break;

	case LOCK_SAME_VOLUME:
		Result = Icon2Icon_MoveDir(iwtSrc, DrInfo, undoStep);
		break;
		}

	if (DestName)
		FreePathBuffer(DestName);

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static ULONG Icon2IconDrop_File(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep)
{
	LONG sameLock = ScaSameLock(DrInfo->drin_Arg->scarg_lock, DrInfo->drin_DestLock);
	STRPTR DestName = NULL;
	ULONG Result = RETURN_OK;
	LONG xi = DrInfo->drin_x, yi = DrInfo->drin_y;
	struct DropOps AllowedDrops;

	debugLock_d1(DrInfo->drin_Arg->scarg_lock);
	debugLock_d1(DrInfo->drin_DestLock);

	d1(KPrintF("%s/%s/%ld: sameLock=%ld\n", __FILE__, __FUNC__, __LINE__, sameLock));

	if (NO_ICON_POSITION_SHORT != xi)
		{
		xi += DrInfo->drin_Arg->scarg_xpos;
		yi += DrInfo->drin_Arg->scarg_ypos;
		}

	AllowedDrops.drop_CreateLink = TRUE;
	AllowedDrops.drop_Copy = TRUE;
	AllowedDrops.drop_Move = LOCK_SAME != sameLock;

	if (!DropPopupMenu(iwtSrc, DrInfo, &AllowedDrops))
		return RETURN_OK;

	if (DrInfo->drin_Drops.drop_CreateLink)
		{
		return Icon2Icon_CreateLink(iwtSrc, DrInfo, undoStep);
		}
	else if (DrInfo->drin_Drops.drop_Move)
		{
		// force move - do nothing if same directory
		if (LOCK_SAME != sameLock)
			sameLock = LOCK_SAME_VOLUME;
		}
	else if (DrInfo->drin_Drops.drop_Copy)
		{
		if (LOCK_SAME == sameLock)
			{
			// source and dest are in same directory
			DestName = AllocPathBuffer();

			BumpRevision(DestName, DrInfo->drin_Arg->scarg_name);
			}

		// force copy
		sameLock = LOCK_DIFFERENT;
		}

	d1(kprintf("%s/%s/%ld: sameLock=%ld\n", __FILE__, __FUNC__, __LINE__, sameLock));

	switch (sameLock)
		{
	case LOCK_SAME:
		return RETURN_OK;

	case LOCK_DIFFERENT:
		UndoAddEvent(iwtSrc, UNDO_Copy,
			UNDOTAG_UndoMultiStep, undoStep,
			UNDOTAG_CopySrcDirLock, DrInfo->drin_Arg->scarg_lock,
			UNDOTAG_CopyDestDirLock, DrInfo->drin_DestLock,
			UNDOTAG_CopySrcName, DrInfo->drin_Arg->scarg_name,
			UNDOTAG_CopyDestName, DestName,
			UNDOTAG_IconPosX, xi,
			UNDOTAG_IconPosY, yi,
			TAG_END);
		Result = DoMethod(DrInfo->drin_FileTransObj, SCCM_FileTrans_Copy,
			DrInfo->drin_Arg->scarg_lock, DrInfo->drin_DestLock,
                        DrInfo->drin_Arg->scarg_name,
			DestName, xi, yi);
		break;

	case LOCK_SAME_VOLUME:
		Result = Icon2Icon_MoveDir(iwtSrc, DrInfo, undoStep);
		break;
		}

	if (DestName)
		FreePathBuffer(DestName);

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static ULONG Icon2Icon_MoveDir(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep)
{
	ULONG Result;
	LONG xi = DrInfo->drin_x, yi = DrInfo->drin_y;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	if (NO_ICON_POSITION_SHORT != xi)
		{
		xi += DrInfo->drin_Arg->scarg_xpos;
		yi += DrInfo->drin_Arg->scarg_ypos;
		}

	UndoAddEvent(iwtSrc, UNDO_Move,
		UNDOTAG_UndoMultiStep, undoStep,
		UNDOTAG_CopySrcDirLock, DrInfo->drin_Arg->scarg_lock,
		UNDOTAG_CopyDestDirLock, DrInfo->drin_DestLock,
		UNDOTAG_CopySrcName, DrInfo->drin_Arg->scarg_name,
		UNDOTAG_IconPosX, xi,
		UNDOTAG_IconPosY, yi,
		TAG_END);

	Result = DoMethod(DrInfo->drin_FileTransObj, SCCM_FileTrans_Move,
		DrInfo->drin_Arg->scarg_lock, DrInfo->drin_DestLock,
                DrInfo->drin_Arg->scarg_name, xi, yi);

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static ULONG Icon2Icon_CopyDir(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, CONST_STRPTR DestName, APTR undoStep)
{
	ULONG Result;
	LONG xi = DrInfo->drin_x, yi = DrInfo->drin_y;

	if (NO_ICON_POSITION_SHORT != xi)
		{
		xi += DrInfo->drin_Arg->scarg_xpos;
		yi += DrInfo->drin_Arg->scarg_ypos;
		}

	UndoAddEvent(iwtSrc, UNDO_Copy,
		UNDOTAG_UndoMultiStep, undoStep,
		UNDOTAG_CopySrcDirLock, DrInfo->drin_Arg->scarg_lock,
		UNDOTAG_CopyDestDirLock, DrInfo->drin_DestLock,
		UNDOTAG_CopySrcName, DrInfo->drin_Arg->scarg_name,
		UNDOTAG_CopyDestName, DestName,
		UNDOTAG_IconPosX, xi,
		UNDOTAG_IconPosY, yi,
		TAG_END);

	Result = DoMethod(DrInfo->drin_FileTransObj, SCCM_FileTrans_Copy,
		DrInfo->drin_Arg->scarg_lock, DrInfo->drin_DestLock,
		DrInfo->drin_Arg->scarg_name, DestName, xi, yi);

	return Result;
}


static ULONG Icon2Icon_CreateLink(struct internalScaWindowTask *iwtSrc, struct DropInfo *DrInfo, APTR undoStep)
{
	ULONG Result;
	LONG xi = DrInfo->drin_x, yi = DrInfo->drin_y;

	if (NO_ICON_POSITION_SHORT != xi)
		{
		xi += DrInfo->drin_Arg->scarg_xpos;
		yi += DrInfo->drin_Arg->scarg_ypos;
		}

	UndoAddEvent(iwtSrc, UNDO_CreateLink,
		UNDOTAG_UndoMultiStep, undoStep,
		UNDOTAG_CopySrcDirLock, DrInfo->drin_Arg->scarg_lock,
		UNDOTAG_CopyDestDirLock, DrInfo->drin_DestLock,
		UNDOTAG_CopySrcName, DrInfo->drin_Arg->scarg_name,
		UNDOTAG_IconPosX, xi,
		UNDOTAG_IconPosY, yi,
		TAG_END);

	Result = DoMethod(DrInfo->drin_FileTransObj, SCCM_FileTrans_CreateLink,
		DrInfo->drin_Arg->scarg_lock, DrInfo->drin_DestLock,
		DrInfo->drin_Arg->scarg_name,
		DrInfo->drin_Arg->scarg_name,
		xi, yi);

	d1(kprintf("%s/%s/%ld: Result=%08lx\n", __FILE__, __FUNC__, __LINE__, Result));

	return Result;
}


static void Icon2DesktopDrop(struct ScalosArg **ArgList,
	struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest,
	struct DropInfo *DrInfo)
{
	struct internalScaWindowTask *iwtSrc, *iwtDest;
	struct MsgPort *replyPort;
	struct BackDropList bdl;	// +jmc+

	d1(KPrintF("%s/%s/%ld: Src=%08lx <%s>  Dest=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		wsSrc, wsSrc->ws_Name, wsDest, wsDest->ws_Name));

	if (NULL == wsSrc || NULL == wsDest)
		return;

	iwtSrc = (struct internalScaWindowTask *) wsSrc->ws_WindowTask;
	iwtDest = (struct internalScaWindowTask *) wsDest->ws_WindowTask;

	if (NULL == iwtSrc || NULL == iwtDest)
		return;

	BackDropInitList(&bdl);	// +jmc+

	do	{
		APTR undoStep;
		struct ScaIconNode *in;
		struct ScalosArg *arg;

		ScalosLockIconListShared(iwtDest);

		in = CheckMouseIcon(&iwtDest->iwt_WindowTask.wt_IconList, iwtDest,
			DrInfo->drin_x, DrInfo->drin_y);
		d1(kprintf("%s/%s/%ld: in=%08lx  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, in, DrInfo->drin_x, DrInfo->drin_y));
		if (in)
			{
			// Drop on some icon
			ULONG IconType;

			GetAttr(IDTA_Type, in->in_Icon, &IconType);

			d1(kprintf("%s/%s/%ld: in=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, in, \
				GetIconName(in), IconType));
			switch (IconType)
				{
			case WBDISK:
			case WBDRAWER:
			case WB_TEXTICON_DRAWER:
			case WBGARBAGE:
				ScalosUnLockIconList(iwtDest);
				Icon2IconDrop(ArgList, wsSrc, wsDest, DrInfo);
				return;
				break;
				}
			}

		ScalosUnLockIconList(iwtDest);

		// Drop onto desktop
		replyPort = CreateMsgPort();
		if (NULL == replyPort)
			break;

		undoStep = UndoBeginStep();

		for (arg = *ArgList; arg; arg = (struct ScalosArg *) arg->scarg_Node.mln_Succ)
			{
			ULONG Result;

			d1(KPrintF("%s/%s/%ld: DrInfo->drin_Arg=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, \
				arg, arg->scarg_name, arg->scarg_icontype));

			if (WBDISK == arg->scarg_icontype)
				continue;

			DrInfo->drin_Drops.drop_LeaveOut = TRUE;
			if (CurrentPrefs.pref_AutoLeaveOut)
				{
				d1(KPrintF("%s/%s/%ld: drin_x=%ld  drin_y=%ld\n", __FILE__, __FUNC__, __LINE__, DrInfo->drin_x, DrInfo->drin_y));
				d1(KPrintF("%s/%s/%ld: scarg_xpos=%ld  scarg_ypos=%ld\n", __FILE__, __FUNC__, __LINE__, arg->scarg_xpos, arg->scarg_ypos));

				UndoAddEvent(iwtSrc, UNDO_Leaveout,
					UNDOTAG_UndoMultiStep, undoStep,
					UNDOTAG_IconDirLock, arg->scarg_lock,
					UNDOTAG_IconName, arg->scarg_name,
					UNDOTAG_IconPosX, DrInfo->drin_x + arg->scarg_xpos,
					UNDOTAG_IconPosY, DrInfo->drin_y + arg->scarg_ypos,
					TAG_END);

				DoLeaveOutIcon(iwtSrc,
					arg->scarg_lock,
					arg->scarg_name,
					DrInfo->drin_x + arg->scarg_xpos,
					DrInfo->drin_y + arg->scarg_ypos);

				d1(KPrintF("%s/%s/%ld: DoLeaveout(name=<%s>)  Type=%ld\n", __FILE__, __FUNC__, __LINE__, \
					arg->scarg_name, arg->scarg_icontype));
				}
			else
				{
				// Add icon to destination window
				Result = DropAddIcon(replyPort, wsDest,
					arg->scarg_lock,
					arg->scarg_name,
					DrInfo->drin_x + arg->scarg_xpos,
					DrInfo->drin_y + arg->scarg_ypos);

				d1(KPrintF("%s/%s/%ld: DropAddIcon(name=<%s>)  Type=%ld\n", __FILE__, __FUNC__, __LINE__, \
					arg->scarg_name, arg->scarg_icontype));
				// +jmc+
				if (IsPermanentBackDropIcon(iwtDest, &bdl, arg->scarg_lock, arg->scarg_name))
					{
					ScalosLockIconListExclusive(iwtDest);

					for (in = iwtDest->iwt_WindowTask.wt_IconList; in; in=(struct ScaIconNode *) in->in_Node.mln_Succ)
						{
						d1(KPrintF("%s/%s/%ld: in_Name=%08lx\n", __FILE__, __FUNC__, __LINE__, in->in_Name));

						if (in->in_Lock && (LOCK_SAME == SameLock(in->in_Lock, arg->scarg_lock))
							&& in->in_Name && 0 == Stricmp(arg->scarg_name, in->in_Name))
							{
							struct SM_RedrawIconObj *sMsg;

							SetAttrs(in->in_Icon,
								IDTA_OverlayType, ICONOVERLAYF_LeftOut,
								TAG_END);

							d1(KPrintF("%s/%s/%ld: scarg_name=<%s> in->in_Name=<%s> ICONOVERLAYF_LeftOut is SET\n", __FILE__, __FUNC__, __LINE__, \
								arg->scarg_name, in->in_Name));

							sMsg = (struct SM_RedrawIconObj *) SCA_AllocMessage(MTYP_RedrawIconObj, 0);

							d1(KPrintF("%s/%s/%ld: sMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, sMsg));

							if (sMsg)
								{
								sMsg->smrio_IconObject = in->in_Icon;
								sMsg->smrio_Flags = SMRIOFLAGF_IconListLocked;
								PutMsg(iwtDest->iwt_WindowTask.wt_IconPort,
									&sMsg->ScalosMessage.sm_Message);
								}
							}
						}

					ScalosUnLockIconList(iwtDest);
					}

				if (0 == Result)
					continue;

				// Remove icon from source window
				DropRemoveIcon(wsSrc, arg->scarg_lock, arg->scarg_name);

				// Special handling for "view all" textwindows.
				// here both object and icon are separate entries, which must be removed both!
				if (!IsIwtViewByIcon(iwtSrc) && IsShowAll(wsSrc))
					{
					STRPTR IconInfoName = ScalosAlloc(1 + strlen(arg->scarg_name) + strlen(".info"));

					if (IconInfoName)
						{
						strcpy(IconInfoName, arg->scarg_name);
						strcat(IconInfoName, ".info");

						// remove associated ".info" icon from source window
						DropRemoveIcon(wsSrc, arg->scarg_lock, IconInfoName);

						ScalosFree(IconInfoName);
						}
					}
				}
			}

		UndoEndStep(iwtSrc, undoStep);
		} while (0);

	if (replyPort)
		DeleteMsgPort(replyPort);

	BackdropFreeList(&bdl);

	ScalosReleaseSemaphore(iwtSrc->iwt_WindowTask.wt_WindowSemaphore);
	ScalosReleaseSemaphore(iwtDest->iwt_WindowTask.wt_WindowSemaphore);

	ClassDragFinish_IconWin(ArgList);
}


static void Desktop2DesktopDrop(struct ScalosArg **ArgList, struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest, struct DropInfo *DrInfo)
{
	struct internalScaWindowTask *iwtDest;
	struct ScaIconNode *in;

	d1(KPrintF("%s/%s/%ld: Src=%08lx <%s>  Dest=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		wsSrc, wsSrc->ws_Name, wsDest, wsDest->ws_Name));

	if (NULL == wsSrc || NULL == wsDest)
		return;

	iwtDest = (struct internalScaWindowTask *) wsDest->ws_WindowTask;

	if (NULL == iwtDest)
		return;

	ScalosLockIconListShared(iwtDest);

	in = CheckMouseIcon(&iwtDest->iwt_WindowTask.wt_IconList, iwtDest, DrInfo->drin_x, DrInfo->drin_y);
	if (in)
		{
		ULONG IconType;

		GetAttr(IDTA_Type, in->in_Icon, &IconType);
		switch (IconType)
			{
		case WBDISK:
			for (DrInfo->drin_Arg = *ArgList; DrInfo->drin_Arg; DrInfo->drin_Arg = (struct ScalosArg *) DrInfo->drin_Arg->scarg_Node.mln_Succ)
				{
				STRPTR p;
				BPTR diskcopyLock;
				char ch;

				d1(kprintf("%s/%s/%ld: DrInfo->drin_Arg=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, DrInfo->drin_Arg, DrInfo->drin_Arg->scarg_name));

				if (WBDISK != DrInfo->drin_Arg->scarg_icontype)
					continue;

				p = PathPart((STRPTR) CurrentPrefs.pref_DefDiskCopy);
				ch = *p;
				*p = '\0';

				diskcopyLock = Lock((STRPTR) CurrentPrefs.pref_DefDiskCopy, ACCESS_READ);

				*p = ch;

				if (diskcopyLock)
					{
					struct WBArg ArgArray[3];

					ArgArray[0].wa_Lock = diskcopyLock;
					ArgArray[0].wa_Name = FilePart((STRPTR) CurrentPrefs.pref_DefDiskCopy);

					ArgArray[1].wa_Lock = (BPTR)NULL;
					ArgArray[1].wa_Name = NULL;
					if (DrInfo->drin_Arg->scarg_lock)
						ArgArray[1].wa_Lock = DupLock(DrInfo->drin_Arg->scarg_lock);
					else
						ArgArray[1].wa_Name = DrInfo->drin_Arg->scarg_name;

					ArgArray[2].wa_Lock = DiskInfoLock(in);
					ArgArray[2].wa_Name = NULL;

					if ((BPTR)NULL == ArgArray[2].wa_Lock)
						ArgArray[2].wa_Name = in->in_DeviceIcon->di_Device;

					SCA_WBStart(ArgArray, NULL, 3);
					}

				DrInfo->drin_Arg->scarg_icontype = 0;
				}
			break;
		default:
			ScalosUnLockIconList(iwtDest);
			Desktop2IconDrop(ArgList, wsSrc, wsDest, DrInfo);
			return;
			break;
			}
		}

	ScalosUnLockIconList(iwtDest);

	Desktop2IconDrop(ArgList, wsSrc, wsDest, DrInfo);
}


// drop icon(s) from desktop (=device window) to some icon window
static void Desktop2IconDrop(struct ScalosArg **ArgList,
	struct ScaWindowStruct *wsSrc,
	struct ScaWindowStruct *wsDest,
	struct DropInfo *DrInfo)
{
	struct internalScaWindowTask *iwtSrc, *iwtDest;
	BPTR SrcLock = (BPTR)NULL;
	BPTR oldDir = NOT_A_LOCK;
	struct MsgPort *replyPort = NULL;
	BOOL WindowsLocked = TRUE;
	APTR undoStep = NULL;

	d1(KPrintF("%s/%s/%ld: START Src=%08lx <%s>  Dest=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, \
		wsSrc, wsSrc->ws_Name, wsDest, wsDest->ws_Name));

	if (NULL == wsSrc || NULL == wsDest)
		return;

	iwtSrc = (struct internalScaWindowTask *) wsSrc->ws_WindowTask;
	iwtDest = (struct internalScaWindowTask *) wsDest->ws_WindowTask;

	if (NULL == iwtSrc || NULL == iwtDest)
		return;

	do	{
		ULONG Result = RETURN_OK;
		struct ScaIconNode *in;

		if (wsSrc->ws_Lock)
			{
			SrcLock = DupLock(wsSrc->ws_Lock);
			if ((BPTR)NULL == SrcLock)
				break;
			}
		if (wsDest->ws_Lock)
			{
			DrInfo->drin_DestLock = DupLock(wsDest->ws_Lock);
			if ((BPTR)NULL == DrInfo->drin_DestLock)
				break;
			}

		oldDir = CurrentDir(DrInfo->drin_DestLock);

		ScalosLockIconListShared(iwtDest);

		in = CheckMouseIcon(&iwtDest->iwt_WindowTask.wt_IconList,
			iwtDest, DrInfo->drin_x, DrInfo->drin_y);
		d1(kprintf("%s/%s/%ld: in=%08lx  iwtDest=%08lx\n", __FILE__, __FUNC__, __LINE__, in, iwtDest));
		if (in)
			{
			ULONG IconType;
			BPTR IconLock = (BPTR)NULL;

			GetAttr(IDTA_Type, in->in_Icon, &IconType);
			switch (IconType)
				{
			case WBDISK:
				IconLock = DiskInfoLock(in);
				break;
			case WBDRAWER:
			case WBGARBAGE:
			case WB_TEXTICON_DRAWER:
				if (in->in_Lock)
					{
					BPTR oldDir2 = CurrentDir(in->in_Lock);

					IconLock = Lock((STRPTR) GetIconName(in), ACCESS_READ);

					CurrentDir(oldDir2);
					}
				else
					IconLock = Lock((STRPTR) GetIconName(in), ACCESS_READ);
				break;
				}

			if (IconLock)
				{
				UnLock(DrInfo->drin_DestLock);
				DrInfo->drin_DestLock = IconLock;
				DrInfo->drin_x = NO_ICON_POSITION_SHORT;
				DrInfo->drin_y = NO_ICON_POSITION_SHORT;
				}
			}

		ScalosUnLockIconList(iwtDest);

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		CurrentDir(oldDir);
		oldDir = NOT_A_LOCK;

		replyPort = CreateMsgPort();
		if (NULL == replyPort)
			break;

		undoStep = UndoBeginStep();

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		for (DrInfo->drin_Arg = *ArgList; DrInfo->drin_Arg; DrInfo->drin_Arg = (struct ScalosArg *) DrInfo->drin_Arg->scarg_Node.mln_Succ)
			{
			d1(kprintf("%s/%s/%ld: DrInfo->drin_Arg=%08lx <%s>  IconType=%ld\n", \
				__FILE__, __FUNC__, __LINE__, \
				DrInfo->drin_Arg, DrInfo->drin_Arg->scarg_name, DrInfo->drin_Arg->scarg_icontype));

			if (WBDISK == DrInfo->drin_Arg->scarg_icontype)
				continue;

			debugLock_d1(DrInfo->drin_DestLock);
			debugLock_d1(DrInfo->drin_Arg->scarg_lock);

			if (LOCK_SAME == ScaSameLock(DrInfo->drin_DestLock, DrInfo->drin_Arg->scarg_lock))
				{
				// drop left-out icon back into its native window
				d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

				DrInfo->drin_Drops.drop_PutAway = TRUE;

				UndoAddEvent(iwtSrc, UNDO_PutAway,
					UNDOTAG_UndoMultiStep, undoStep,
					UNDOTAG_IconDirLock, DrInfo->drin_Arg->scarg_lock,
					UNDOTAG_IconName, DrInfo->drin_Arg->scarg_name,
					TAG_END);
				
				PutAwayIcon(iwtSrc,
					DrInfo->drin_Arg->scarg_lock,
					DrInfo->drin_Arg->scarg_name,
                                        CurrentPrefs.pref_AutoLeaveOut);

				d1(kprintf("%s/%s/%ld: PutAwayIcon(name=<%s>)  IconType=%ld\n", \
					__FILE__, __FUNC__, __LINE__, \
					DrInfo->drin_Arg->scarg_name, DrInfo->drin_Arg->scarg_icontype));

				if (NO_ICON_POSITION_SHORT == DrInfo->drin_x)
					{
					struct ScaUpdateIcon_IW upd;

					upd.ui_iw_Lock = DrInfo->drin_Arg->scarg_lock;
					upd.ui_iw_Name = DrInfo->drin_Arg->scarg_name;
					upd.ui_IconType = ICONTYPE_NONE;

					SCA_UpdateIcon(SIV_IconWin, &upd, sizeof(upd));
					}
				else
					{
					ULONG Result;

					Result = DropAddIcon(replyPort, wsDest,
						DrInfo->drin_Arg->scarg_lock,
						DrInfo->drin_Arg->scarg_name,
						DrInfo->drin_x + DrInfo->drin_Arg->scarg_xpos,
						DrInfo->drin_y + DrInfo->drin_Arg->scarg_ypos);

					if (0 == Result)
						{
						DropAddIcon(replyPort, wsSrc,
							DrInfo->drin_Arg->scarg_lock,
							DrInfo->drin_Arg->scarg_name,
							DrInfo->drin_x + DrInfo->drin_Arg->scarg_xpos,
							DrInfo->drin_y + DrInfo->drin_Arg->scarg_ypos);
						}

					// Special handling for "view all" textwindows.
					// here both object and icon are separate entries, which must be added both!
					if (!IsIwtViewByIcon(iwtDest) && IsShowAll(wsDest))
						{
						STRPTR IconInfoName = ScalosAlloc(1 + strlen(DrInfo->drin_Arg->scarg_name) + strlen(".info"));

						if (IconInfoName)
							{
							strcpy(IconInfoName, DrInfo->drin_Arg->scarg_name);
							strcat(IconInfoName, ".info");

							// add associated ".info" icon to destination window
							DropAddIcon(replyPort, wsDest,
								DrInfo->drin_Arg->scarg_lock,
								IconInfoName,
								DrInfo->drin_x + DrInfo->drin_Arg->scarg_xpos,
								DrInfo->drin_y + DrInfo->drin_Arg->scarg_ypos);

							ScalosFree(IconInfoName);
							}
						}
					}
				}
			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
			}

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		DeleteMsgPort(replyPort);
		replyPort = NULL;

		ScalosReleaseSemaphore(iwtSrc->iwt_WindowTask.wt_WindowSemaphore);
		ScalosReleaseSemaphore(iwtDest->iwt_WindowTask.wt_WindowSemaphore);
		WindowsLocked = FALSE;

		for (DrInfo->drin_Arg = *ArgList;
				RESULT_UserAborted != Result && DrInfo->drin_Arg;
				DrInfo->drin_Arg = (struct ScalosArg *) DrInfo->drin_Arg->scarg_Node.mln_Succ)
			{
			LONG xi = DrInfo->drin_x, yi = DrInfo->drin_y;

			d1(kprintf("%s/%s/%ld: DrInfo->drin_Arg=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, DrInfo->drin_Arg, DrInfo->drin_Arg->scarg_name));

			if (NO_ICON_POSITION_SHORT != xi)
				{
				xi += DrInfo->drin_Arg->scarg_xpos;
				yi += DrInfo->drin_Arg->scarg_ypos;
				}

			switch (DrInfo->drin_Arg->scarg_icontype)
				{
			case WBDRAWER:
			case WB_TEXTICON_DRAWER:
			case WBGARBAGE:
				Result = Icon2IconDrop_Drawer(iwtSrc, DrInfo, undoStep);
				break;
			case WBTOOL:
			case WB_TEXTICON_TOOL:
				Result = Icon2IconDrop_File(iwtSrc, DrInfo, undoStep);
				break;

			case WBDISK:
				if (LOCK_DIFFERENT == ScaSameLock(DrInfo->drin_DestLock, DrInfo->drin_Arg->scarg_lock) ||
						DrInfo->drin_Drops.drop_Copy || DrInfo->drin_Drops.drop_CreateLink)
					{
					STRPTR Name = NULL;
					BPTR oldDir2 = BNULL;

					debugLock_d1(DrInfo->drin_DestLock);
					debugLock_d1(DrInfo->drin_Arg->scarg_lock);

					do	{
						struct DropOps AllowedDrops;

						AllowedDrops.drop_CreateLink = TRUE;
						AllowedDrops.drop_Copy = TRUE;
						AllowedDrops.drop_Move = FALSE;

						if (!DropPopupMenu(iwtSrc, DrInfo, &AllowedDrops))
							break;

						oldDir2 = CurrentDir(DrInfo->drin_DestLock);

						Name = AllocPathBuffer();
						if (NULL == Name)
							break;

						if (!NameFromLock(DrInfo->drin_Arg->scarg_lock, Name, Max_PathLen-1))
							break;

						d1(kprintf("%s/%s/%ld: Name=<%s>\n", __FILE__, __FUNC__, __LINE__, Name));

						StripTrailingColon(Name);

						d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

						if (DrInfo->drin_Drops.drop_CreateLink)
							{
							d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
							UndoAddEvent(iwtSrc, UNDO_CreateLink,
								UNDOTAG_UndoMultiStep, undoStep,
								UNDOTAG_CopySrcDirLock, DrInfo->drin_Arg->scarg_lock,
								UNDOTAG_CopyDestDirLock, DrInfo->drin_DestLock,
								UNDOTAG_CopySrcName, "",
								UNDOTAG_CopyDestName, Name,
								UNDOTAG_IconPosX, xi,
								UNDOTAG_IconPosY, yi,
								TAG_END);
							Result = DoMethod(DrInfo->drin_FileTransObj,
								SCCM_FileTrans_CreateLink,
								DrInfo->drin_Arg->scarg_lock,
                                                                DrInfo->drin_DestLock,
								"", Name,
								xi, yi);
							}
						else
							{
							d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
							UndoAddEvent(iwtSrc, UNDO_Copy,
								UNDOTAG_UndoMultiStep, undoStep,
								UNDOTAG_CopySrcDirLock, DrInfo->drin_Arg->scarg_lock,
								UNDOTAG_CopyDestDirLock, DrInfo->drin_DestLock,
								UNDOTAG_CopySrcName, "",
								UNDOTAG_CopyDestName, Name,
								UNDOTAG_IconPosX, xi,
								UNDOTAG_IconPosY, yi,
								TAG_END);
							d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
							Result = DoMethod(DrInfo->drin_FileTransObj,
								SCCM_FileTrans_Copy,
								DrInfo->drin_Arg->scarg_lock,
                                                                DrInfo->drin_DestLock,
								"", Name,
								xi, yi);
							d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
							}

						d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
						} while (0);

					if (Name)
						FreePathBuffer(Name);

					CurrentDir(oldDir2);
					}
				break;
				}
			d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
			}

		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

		ClassDragFinish_IconWin(ArgList);
		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
		} while (0);

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	if (undoStep)
		UndoEndStep(iwtSrc, undoStep);
	if (replyPort)
		DeleteMsgPort(replyPort);
	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (SrcLock)
		UnLock(SrcLock);
	if (DrInfo->drin_DestLock)
		{
		UnLock(DrInfo->drin_DestLock);
		DrInfo->drin_DestLock = (BPTR)NULL;
		}

	if (WindowsLocked)
		{
		ScalosReleaseSemaphore(iwtSrc->iwt_WindowTask.wt_WindowSemaphore);
		ScalosReleaseSemaphore(iwtDest->iwt_WindowTask.wt_WindowSemaphore);
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}


static ULONG DropAddIcon(struct MsgPort *ReplyPort, struct ScaWindowStruct *wsDest,
	BPTR DirLock, STRPTR IconName, WORD x, WORD y)
{
	struct SM_AddIcon *aMsg;
	ULONG Result;

	aMsg = (struct SM_AddIcon *) SCA_AllocMessage(MTYP_AddIcon, 0);
	d1(kprintf("%s/%s/%ld:  aMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, aMsg));
	if (NULL == aMsg)
		return 0;

	aMsg->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort;
	aMsg->smai_x = x;
	aMsg->smai_y = y;
	aMsg->smai_DirLock = DirLock;
	aMsg->smai_IconName = IconName;

	PutMsg(wsDest->ws_MessagePort, &aMsg->ScalosMessage.sm_Message);

	d1(kprintf("%s/%s/%ld: before WaitPort\n", __FILE__, __FUNC__, __LINE__));

	WaitPort(ReplyPort);

	d1(kprintf("%s/%s/%ld:  after WaitPort\n", __FILE__, __FUNC__, __LINE__));

	aMsg = (struct SM_AddIcon *) GetMsg(ReplyPort);
	Result = *((ULONG *) &(aMsg->smai_x));

	SCA_FreeMessage(&aMsg->ScalosMessage);

	return Result;
}


static void DropRemoveIcon(struct ScaWindowStruct *wsDest, BPTR DirLock, CONST_STRPTR IconName)
{
	struct SM_RemIcon *rMsg;

	// allocate space for the icon name at the end of the SM_RemIcon structure
	rMsg = (struct SM_RemIcon *) SCA_AllocMessage(MTYP_RemIcon, 1 + strlen(IconName));
	d1(KPrintF("%s/%s/%ld:  IconName=<%s> rMsg=%08lx\n", __FILE__, __FUNC__, __LINE__, IconName, rMsg));
	if (rMsg)
		{
		STRPTR ClonedIconName;

		ClonedIconName = (STRPTR) (rMsg + 1);
		strcpy(ClonedIconName, IconName);

		rMsg->ScalosMessage.sm_Message.mn_ReplyPort = NULL;
		rMsg->smri_DirLock = DirLock;
		rMsg->smri_IconName = ClonedIconName;

		PutMsg(wsDest->ws_MessagePort, &rMsg->ScalosMessage.sm_Message);
		}
}


ULONG ScalosDropAddIcon(BPTR DirLock, CONST_STRPTR IconName, WORD x, WORD y)
{
	struct MsgPort *ReplyPort;
	ULONG Result = 0;
	struct ScaWindowStruct *ws;

	ReplyPort = CreateMsgPort();
	d1(KPrintF("%s/%s/%ld: ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, ReplyPort));
	if (NULL == ReplyPort)
		return (ULONG) IoErr();

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws->ws_Lock && LOCK_SAME == SameLock(DirLock, ws->ws_Lock))
			{
			d1(kprintf("%s/%s/%ld: found Window\n", __FILE__, __FUNC__, __LINE__));

			if (&ws->ws_Task->pr_Task == FindTask(NULL))
				{
				d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
				Result = DoMethod(ws->ws_WindowTask->mt_MainObject,
					SCCM_IconWin_AddIcon,
					SCCM_ADDICON_MAKEXY(x, y),
					DirLock,
					IconName);
				}
			else
				{
				d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
				Result = DropAddIcon(ReplyPort,
					ws,
					DirLock,
					(STRPTR) IconName,
					x, y);
				}
			}
		}

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	SCA_UnLockWindowList();

	d1(KPrintF("%s/%s/%ld: Result=%ld\n", __FILE__, __FUNC__, __LINE__, Result));

	DeleteMsgPort(ReplyPort);

	return Result;
}


void ScalosDropRemoveIcon(BPTR DirLock, CONST_STRPTR IconName)
{
	struct ScaWindowStruct *ws;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		if (ws->ws_Lock && LOCK_SAME == SameLock(DirLock, ws->ws_Lock))
			{
			DropRemoveIcon(ws, DirLock, (STRPTR) IconName);
			}
		}

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	SCA_UnLockWindowList();
}


static void DronOnAppIcon(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest,
	struct ScaIconNode *in, LONG x, LONG y)
{
	d1(kprintf("%s/%s/%ld: iwt=%08lx  iwtFound=%08lx  Icon=%08lx <%s>  x=%ld  y=%ld\n", \
		__LINE__, iwt, iwtDest, in, GetIconName(in), x, y));

	RestoreDragIcons(iwt);

	ScalosLockIconListShared(iwtDest);

	if (iwtDest->iwt_AppListSemaphore)
		{
		struct AppObject *ao;

		ScalosObtainSemaphoreShared(iwtDest->iwt_AppListSemaphore);

		for (ao=iwtDest->iwt_AppList; ao; ao = (struct AppObject *) ao->appo_Node.mln_Succ)
			{
			if (APPTYPE_AppIcon == ao->appo_type &&
				in->in_Icon == ao->appo_object.appoo_IconObject)
				{
				ScalosUnLockIconList(iwtDest);

				if (ScalosAttemptSemaphoreShared(iwtDest->iwt_WindowTask.wt_WindowSemaphore))
					{
					DronOnAppObject(iwt, ao, x, y);

					ScalosReleaseSemaphore(iwtDest->iwt_WindowTask.wt_WindowSemaphore);

					ScalosLockIconListShared(iwtDest);
					break;
					}
				}
			}

		ScalosReleaseSemaphore(iwtDest->iwt_AppListSemaphore);
		}

	ScalosUnLockIconList(iwtDest);
}


static void DronOnAppObject(struct internalScaWindowTask *iwt, struct AppObject *ao, LONG x, LONG y)
{
	struct DragNode *dn;
	struct AppMessage *am;
	struct WBArg *argList;
	size_t Len;
	ULONG ArgCount;
	ULONG NumberOfDroppedIcons;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  AppObj=%08lx  x=%ld  y=%ld\n", \
		__LINE__, iwt, ao, x, y));

	for (dn = iwt->iwt_DragNodeList, NumberOfDroppedIcons = 0;
			dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		 NumberOfDroppedIcons++;

	Len = sizeof(struct AppMessage) + sizeof(struct WBArg) * NumberOfDroppedIcons;

	am = AllocVec(Len, MEMF_PUBLIC | MEMF_CLEAR);
	if (NULL == am)
		return;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	argList = (struct WBArg *) &am[1];

	am->am_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
	am->am_Message.mn_Length = Len;

	switch (ao->appo_type)
		{
	case APPTYPE_AppIcon:
		am->am_Type = AMTYPE_APPICON;
		break;
	case APPTYPE_AppWindow:
		am->am_Type = AMTYPE_APPWINDOW;
		break;
	case APPTYPE_AppMenuItem:
		am->am_Type = AMTYPE_APPMENUITEM;
		break;
		}

	am->am_UserData = ao->appo_userdata;
	am->am_ID = ao->appo_id;
	am->am_Version = AM_VERSION;

	x += iwt->iwt_WindowTask.wt_Window->LeftEdge;
	y += iwt->iwt_WindowTask.wt_Window->TopEdge;

	if (APPTYPE_AppWindow == ao->appo_type)
		{
		x -= ao->appo_object.appoo_Window->LeftEdge;
		y -= ao->appo_object.appoo_Window->TopEdge;
		}

	am->am_MouseX = x;
	am->am_MouseY = y;

	CurrentTime(&am->am_Seconds, &am->am_Micros);

	am->am_Reserved[0] = ID_APPM;
	am->am_ArgList = argList;

	d1(kprintf("%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	ArgCount = 0;

	ScalosLockIconListShared(iwt);

	for (dn=iwt->iwt_DragNodeList; dn && ArgCount < NumberOfDroppedIcons; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		struct ScaIconNode *in = dn->drgn_iconnode;
		ULONG IconType;
		BPTR oldDir;
		BPTR fLock;

		GetAttr(IDTA_Type, dn->drgn_icon, &IconType);

		d1(kprintf("%s/%s/%ld: dn=%08lx <%s>  Type=%ld\n", __FILE__, __FUNC__, __LINE__, dn, GetIconName(in), IconType));

		switch (IconType)
			{
		case WBTOOL:
		case WBPROJECT:
		case WB_TEXTICON_TOOL:		// Text Tool
//.tool:
			if (in->in_Name)
				{
				if (in->in_Lock)
					fLock = DupLock(in->in_Lock);
				else
					{
					if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock)
						fLock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
					else
						fLock = (BPTR)NULL;
					}

				argList->wa_Lock = fLock;
				argList->wa_Name = AllocCopyString(in->in_Name);

				argList++;
				ArgCount++;
				}
			break;

		case WBAPPICON:
			break;

		default:
			oldDir = CurrentDir(in->in_Lock ? in->in_Lock : iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

			if (in->in_DeviceIcon)
				fLock = DiskInfoLock(in);
			else
				fLock = Lock((STRPTR) GetIconName(in), ACCESS_READ);

			CurrentDir(oldDir);

			if (fLock)
				{
				argList->wa_Lock = fLock;
				argList->wa_Name = AllocCopyString(NULL);

				argList++;
				ArgCount++;
				}
			break;
			}
		}

	ScalosUnLockIconList(iwt);

	d1(kprintf("%s/%s/%ld: ArgCount=%ld\n", __FILE__, __FUNC__, __LINE__, ArgCount));

	am->am_NumArgs = ArgCount;
	if (0 == am->am_NumArgs)
		am->am_ArgList = NULL;

	PutMsg(ao->appo_msgport, &am->am_Message);
}


static void SameWindow(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest, LONG x, LONG y)
{
	struct DragNode *dn;
	struct Region *ClipRegion;
	BOOL fIconsMoved = FALSE;
	APTR undoStep;
	LONG x1, y1;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  iwtFound=%08lx  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, iwtDest, x, y));

	if (iwt->iwt_RemIconsFlag && IsIwtViewByIcon(iwt) && CurrentPrefs.pref_AutoRemoveFlag)
		{
		SameWindow2(iwt, iwtDest, x, y);
		return;
		}

	ScalosLockIconListExclusive(iwt);

	x1 = iwt->iwt_InnerLeft - iwt->iwt_WindowTask.wt_XOffset;
	y1 = iwt->iwt_InnerTop - iwt->iwt_WindowTask.wt_YOffset;

	ClipRegion = NewRegion();

	undoStep = UndoBeginStep();

	for (dn=iwt->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		struct ScaIconNode *in = dn->drgn_iconnode;
		const struct ExtGadget *gg = (const struct ExtGadget *) in->in_Icon;

		d1(kprintf("%s/%s/%ld: in=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, in, GetIconName(in)));

		if (!(in->in_Flags & INF_Sticky))
			{
			struct Rectangle IconRect;

			IconRect.MinX = x1 + gg->BoundsLeftEdge;
			IconRect.MaxX = IconRect.MinX + gg->BoundsWidth - 1;
			IconRect.MinY = y1 + gg->BoundsTopEdge;
			IconRect.MaxY = IconRect.MinY + gg->BoundsHeight - 1;

			UndoAddEvent(iwt, UNDO_ChangeIconPos,
				UNDOTAG_UndoMultiStep, undoStep,
				UNDOTAG_IconDirLock, in->in_Lock ?
					in->in_Lock : iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				UNDOTAG_IconNode, in,
				UNDOTAG_IconPosX, dn->drgn_x + x,
				UNDOTAG_IconPosY, y + dn->drgn_y,
				TAG_END);

			d1(kprintf("%s/%s/%ld: IconRect x=%ld  %ld  y=%ld  %ld\n", __FILE__, __FUNC__, __LINE__, \
				IconRect.MinX, IconRect.MaxX, IconRect.MinY, IconRect.MaxY));

			OrRectRegion(ClipRegion, &IconRect);

			fIconsMoved = TRUE;

			MoveIconToNewPosition(iwt, in, dn->drgn_x + x, y + dn->drgn_y);
			}
		}

	d1(kprintf("%s/%s/%ld: IconsMoved=%ld\n", __FILE__, __FUNC__, __LINE__, fIconsMoved));

	UndoEndStep(iwt, undoStep);

	if (fIconsMoved)
		{
		struct Rectangle WindowRect;

		WindowRect.MinX = WindowRect.MaxX = iwt->iwt_InnerLeft;
		WindowRect.MinY = WindowRect.MaxY = iwt->iwt_InnerTop;
		WindowRect.MaxX += iwt->iwt_InnerWidth - 1;
		WindowRect.MaxY += iwt->iwt_InnerHeight - 1;

		AndRectRegion(ClipRegion, &WindowRect);

		if (iwt->iwt_DragFlags & DRAGFLAGF_WindowLocked)
			{
			iwt->iwt_DragFlags |= DRAGFLAGF_UpdatePending;
			}
		else
			{
			struct Region *oldClipRegion;

			d1(kprintf("%s/%s/%ld: DragFlags=%08lx  ClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_DragFlags, ClipRegion));

			oldClipRegion = InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, ClipRegion);

			EraseRect(iwt->iwt_WindowTask.wt_Window->RPort,
				0, 0,
				iwt->iwt_WindowTask.wt_Window->Width - 1,
				iwt->iwt_WindowTask.wt_Window->Height - 1);

			RefreshIcons(iwt, NULL);	// refresh icons NOT in iwt_DragNodeList

			InstallClipRegion(iwt->iwt_WindowTask.wt_Window->WLayer, oldClipRegion);
			d1(kprintf("%s/%s/%ld: oldClipRegion=%08lx\n", __FILE__, __FUNC__, __LINE__, oldClipRegion));
			}

		DisposeRegion(ClipRegion);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

		DragRefreshIcons(iwt);	// refresh icons from iwt_DragNodeList

		ScalosUnLockIconList(iwt);

		DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
			SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}
	else
		{
		ScalosUnLockIconList(iwt);
		DisposeRegion(ClipRegion);
		}
}


static void SameWindow2(struct internalScaWindowTask *iwt, struct internalScaWindowTask *iwtDest, LONG x, LONG y)
{
	APTR undoStep;
	struct DragNode *dn;
	struct ScaIconNode *initialDragIconList;

	d1(KPrintF("%s/%s/%ld: iwt=%08lx  iwtFound=%08lx  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, iwt, iwtDest, x, y));

	ScalosLockIconListExclusive(iwt);

	initialDragIconList = iwt->iwt_DragIconList;

	undoStep = UndoBeginStep();

	for (dn=iwt->iwt_DragNodeList; dn; dn = (struct DragNode *) dn->drgn_Node.mln_Succ)
		{
		SCA_MoveIconNode(&iwt->iwt_DragIconList, &iwt->iwt_WindowTask.wt_IconList, dn->drgn_iconnode);

		if (!(dn->drgn_iconnode->in_Flags & INF_Sticky))
			{
			UndoAddEvent(iwt, UNDO_ChangeIconPos,
				UNDOTAG_UndoMultiStep, undoStep,
				UNDOTAG_IconDirLock, dn->drgn_iconnode->in_Lock ?
					dn->drgn_iconnode->in_Lock : iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock,
				UNDOTAG_IconNode, dn->drgn_iconnode,
				UNDOTAG_IconPosX, dn->drgn_x + x,
				UNDOTAG_IconPosY, y + dn->drgn_y,
				TAG_END);

			MoveIconToNewPosition(iwt, dn->drgn_iconnode, dn->drgn_x + x, y + dn->drgn_y);
			}
		}

	UndoEndStep(iwt, undoStep);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);

	RefreshIconList(iwt, initialDragIconList, NULL);

	ScalosUnLockIconList(iwt);

	DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_IconWin_SetVirtSize,
		SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
}


static ULONG DragQuery_IconWin(struct ScaIconNode *in, ULONG IconType)
{
	switch (IconType)
		{
	case WBDISK:
	case WBDRAWER:
	case WBGARBAGE:
	case WB_TEXTICON_DRAWER:
		return TRUE;
		break;
		}

	return FALSE;
}


static void MoveIconToNewPosition(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, WORD xNew, WORD yNew)
{
	struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;
	WORD x0, y0;
	BOOL NewPosNoGood = FALSE;
	WORD origLeft, origTop, origBoundsLeft, origBoundsTop;

	origLeft = gg->LeftEdge;
	origBoundsLeft = gg->BoundsLeftEdge;
	origTop = gg->TopEdge;
	origBoundsTop = gg->BoundsTopEdge;

	x0 = gg->LeftEdge - gg->BoundsLeftEdge;

	gg->LeftEdge = xNew;
	gg->BoundsLeftEdge = gg->LeftEdge - x0;

	y0 = gg->TopEdge - gg->BoundsTopEdge;

	gg->TopEdge = yNew;
	gg->BoundsTopEdge = gg->TopEdge - y0;

	if (iwt->iwt_BackDrop)
		NewPosNoGood = CheckPosition(iwt, in);

	if (!CurrentPrefs.pref_DDIconsMayOverlap)
		NewPosNoGood = NewPosNoGood || CheckOverlap(iwt, in);

	if (NewPosNoGood)
		{
		// Put icon back to original position
		// and move it back to wt_IconList
		ScalosLockIconListExclusive(iwt);

		gg->LeftEdge = origLeft;
		gg->BoundsLeftEdge = origBoundsLeft;
		gg->TopEdge = origTop;
		gg->BoundsTopEdge = origBoundsTop;

		SCA_MoveIconNode(&iwt->iwt_WindowTask.wt_LateIconList, &iwt->iwt_WindowTask.wt_IconList, in);

		ScalosUnLockIconList(iwt);
		}
}



static BOOL DropPopupMenu(struct internalScaWindowTask *iwtSrc,
	struct DropInfo *DrInfo, const struct DropOps *AllowedDrops)
{
	BOOL DoDrop = TRUE;

	if (CurrentPrefs.pref_EnableDropMenu && !DrInfo->drin_Drops.drop_PutAway && !DrInfo->drin_Drops.drop_LeaveOut)
		{
		if (DrInfo->drin_LastAllowedDrops.drop_Copy              != AllowedDrops->drop_Copy
			|| DrInfo->drin_LastAllowedDrops.drop_Move       != AllowedDrops->drop_Move
			|| DrInfo->drin_LastAllowedDrops.drop_CreateLink != AllowedDrops->drop_CreateLink
			|| DrInfo->drin_LastAllowedDrops.drop_PutAway    != AllowedDrops->drop_PutAway
			|| DrInfo->drin_LastAllowedDrops.drop_LeaveOut   != AllowedDrops->drop_LeaveOut)
			DrInfo->drin_LastDropMenuResult = DROPMENURESULT_Unknown;

		d1(KPrintF("%s/%s/%ld: drin_LastDropMenuResult=%ld\n", __FILE__, __FUNC__, __LINE__, DrInfo->drin_LastDropMenuResult));

		if (DROPMENURESULT_Unknown != DrInfo->drin_LastDropMenuResult)
			{
			switch (DrInfo->drin_LastDropMenuResult)
				{
			case DROPMENURESULT_Copy:
				DrInfo->drin_Drops.drop_Copy = TRUE;
				DrInfo->drin_Drops.drop_Move = FALSE;
				DrInfo->drin_Drops.drop_CreateLink = FALSE;
				break;
			case DROPMENURESULT_Move:
				DrInfo->drin_Drops.drop_Copy = FALSE;
				DrInfo->drin_Drops.drop_Move = TRUE;
				DrInfo->drin_Drops.drop_CreateLink = FALSE;
				break;
			case DROPMENURESULT_CreateLink:
				DrInfo->drin_Drops.drop_Copy = FALSE;
				DrInfo->drin_Drops.drop_Move = FALSE;
				DrInfo->drin_Drops.drop_CreateLink = TRUE;
				break;
			default:
				DoDrop = FALSE;
				break;
				}
			}
		else
			{
			struct PopupMenu *pm;
			struct PopupMenu *pmLast;
			Object *MenuImages[DROPMENUIMAGE_MAX];
			ULONG n;

			MenuImages[DROPMENUIMAGE_Abort]	= NewObject(DtImageClass, NULL,
				DTIMG_ImageName, (ULONG) "THEME:Menu/DropMenu/Abort",
				TAG_END);
			d1(KPrintF("%s/%s/%ld: DROPMENUIMAGE_Abort=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuImages[DROPMENUIMAGE_Abort]));
			MenuImages[DROPMENUIMAGE_Copy] = NewObject(DtImageClass, NULL,
				DTIMG_ImageName, (ULONG) "THEME:Menu/DropMenu/Copy",
				TAG_END);
			d1(KPrintF("%s/%s/%ld: DROPMENUIMAGE_Copy=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuImages[DROPMENUIMAGE_Copy]));
			MenuImages[DROPMENUIMAGE_Move] = NewObject(DtImageClass, NULL,
				DTIMG_ImageName, (ULONG) "THEME:Menu/DropMenu/Move",
				TAG_END);
			d1(KPrintF("%s/%s/%ld: DROPMENUIMAGE_Move=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuImages[DROPMENUIMAGE_Move]));
			MenuImages[DROPMENUIMAGE_CreateLink] = NewObject(DtImageClass, NULL,
				DTIMG_ImageName, (ULONG) "THEME:Menu/DropMenu/CreateLink",
				TAG_END);
			d1(KPrintF("%s/%s/%ld: DROPMENUIMAGE_CreateLink=%08lx\n", __FILE__, __FUNC__, __LINE__, MenuImages[DROPMENUIMAGE_CreateLink]));

			pm = PM_MakeMenu(PM_Item, (ULONG) PM_MakeItem(PM_TitleID, MSGID_DROPMENU_DROPMENUTITLE,
							PM_NoSelect, TRUE,
							PM_ShinePen, TRUE,
							PM_Shadowed, TRUE,
							PM_Center, TRUE,
							TAG_END),
					PM_Item, (ULONG) (pmLast = PM_MakeItem(PM_WideTitleBar, TRUE,
							TAG_END)),
					PM_Item, (ULONG) PM_MakeItem(PM_TitleBar, TRUE,
							TAG_END),
					PM_Item, (ULONG) PM_MakeItem(PM_TitleID, MSGID_DROPMENU_CANCEL,
							PM_UserData, DROPMENURESULT_Cancel,
							MenuImages[DROPMENUIMAGE_Abort]	? PM_IconUnselected : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_Abort],
							MenuImages[DROPMENUIMAGE_Abort]	? PM_IconSelected   : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_Abort],
							TAG_END),
					TAG_END);

			d1(KPrintF("%s/%s/%ld: pm=%08lx  pmLast=%08lx\n", __FILE__, __FUNC__, __LINE__, pm, pmLast));
			if (pm)
				{
				ULONG UserData;
				struct PopupMenu *pmLastNext;

				d1(KPrintF("%s/%s/%ld: drop_Copy=%ld\n", __FILE__, __FUNC__, __LINE__, AllowedDrops->drop_Copy));
				d1(KPrintF("%s/%s/%ld: drop_Move=%ld\n", __FILE__, __FUNC__, __LINE__, AllowedDrops->drop_Move));
				d1(KPrintF("%s/%s/%ld: drop_CreateLink=%ld\n", __FILE__, __FUNC__, __LINE__, AllowedDrops->drop_CreateLink));

				if (AllowedDrops->drop_Copy)
					{
					d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
					PM_InsertMenuItem(pm,
						PM_Insert_After, (ULONG) pmLast,
						PM_Insert_Item, (ULONG) (pmLastNext = PM_MakeItem(PM_TitleID, MSGID_DROPMENU_COPY,
							PM_UserData, DROPMENURESULT_Copy,
							MenuImages[DROPMENUIMAGE_Copy] ? PM_IconUnselected : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_Copy],
							MenuImages[DROPMENUIMAGE_Copy] ? PM_IconSelected   : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_Copy],
							TAG_END)),
						TAG_END);
					pmLast = pmLastNext;
					d1(KPrintF("%s/%s/%ld: pmLast=%08lx\n", __FILE__, __FUNC__, __LINE__, pmLast));
					}
				if (AllowedDrops->drop_Move)
					{
					d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
					PM_InsertMenuItem(pm,
						PM_Insert_After, (ULONG) pmLast,
						PM_Insert_Item, (ULONG) (pmLastNext = PM_MakeItem(PM_TitleID, MSGID_DROPMENU_MOVE,
							PM_UserData, DROPMENURESULT_Move,
							MenuImages[DROPMENUIMAGE_Move] ? PM_IconUnselected : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_Move],
							MenuImages[DROPMENUIMAGE_Move] ? PM_IconSelected   : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_Move],
							TAG_END)),
						TAG_END);
					pmLast = pmLastNext;
					d1(KPrintF("%s/%s/%ld: pmLast=%08lx\n", __FILE__, __FUNC__, __LINE__, pmLast));
					}
				if (AllowedDrops->drop_CreateLink)
					{
					d1(KPrintF("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
					PM_InsertMenuItem(pm,
						PM_Insert_After, (ULONG) pmLast,
						PM_Insert_Item, (ULONG) (pmLastNext = PM_MakeItem(PM_TitleID, MSGID_DROPMENU_CREATE_LINK,
							PM_UserData, DROPMENURESULT_CreateLink,
							MenuImages[DROPMENUIMAGE_CreateLink] ? PM_IconUnselected : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_CreateLink],
							MenuImages[DROPMENUIMAGE_CreateLink] ? PM_IconSelected   : TAG_IGNORE, (ULONG) MenuImages[DROPMENUIMAGE_CreateLink],
							TAG_END)),
						TAG_END);
					//pmLast = pmLastNext;
					d1(KPrintF("%s/%s/%ld: pmLast=%08lx\n", __FILE__, __FUNC__, __LINE__, pmLast));
					}

				// PM_OpenPopupMenuA()
				UserData = PM_OpenPopupMenu(iwtSrc->iwt_WindowTask.wt_Window,
					PM_Menu, (ULONG) pm,
					PM_LocaleHook, (ULONG) &PMGetStringHook,
					TAG_END);
				d1(KPrintF("%s/%s/%ld: UserData=%ld\n", __FILE__, __FUNC__, __LINE__, UserData));

				DrInfo->drin_LastAllowedDrops = *AllowedDrops;
				DrInfo->drin_LastDropMenuResult = UserData;

				switch (UserData)
					{
				case DROPMENURESULT_Copy:
					DrInfo->drin_Drops.drop_Copy = TRUE;
					DrInfo->drin_Drops.drop_Move = FALSE;
					DrInfo->drin_Drops.drop_CreateLink = FALSE;
					break;
				case DROPMENURESULT_Move:
					DrInfo->drin_Drops.drop_Copy = FALSE;
					DrInfo->drin_Drops.drop_Move = TRUE;
					DrInfo->drin_Drops.drop_CreateLink = FALSE;
					break;
				case DROPMENURESULT_CreateLink:
					DrInfo->drin_Drops.drop_Copy = FALSE;
					DrInfo->drin_Drops.drop_Move = FALSE;
					DrInfo->drin_Drops.drop_CreateLink = TRUE;
					break;
				default:
					DoDrop = FALSE;
					break;
					}
				}

			for (n = 0; n < Sizeof(MenuImages); n++)
				{
				if (MenuImages[n])
					DisposeObject(MenuImages[n]);
				}
			}
		}

	ClosePopupWindows(TRUE);

	return DoDrop;
}

