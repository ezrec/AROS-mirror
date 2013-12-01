// RootClass.c
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
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/iconobject.h>
#include <proto/rexxsyslib.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data definitions

struct RootClassInstance
	{
	struct ScaRootList rci_RootList;
	};

struct EventListener
	{
	struct Node el_Node;
	ULONG el_Method;
	struct MsgPort *el_Port;
	ULONG el_Count;
	};

//----------------------------------------------------------------------------

// local functions

static ULONG RootClass_Dispatcher(Class *cl, Object *o, Msg msg);
static ULONG RootClass_New(Class *cl, Object *o, Msg msg);
static ULONG Root_HandleMessages(Class *cl, Object *o, Msg msg);
static ULONG Root_CheckForMessages(Class *cl, Object *o, Msg msg);
static ULONG RootClass_Message(Class *cl, Object *o, Msg msg);
static int IDCMPFuncTableComp(const void *keyval, const void *datum);
static ULONG RootClass_RunProcess(Class *cl, Object *o, Msg msg);
static ULONG RootClass_AddToClipboard(Class *cl, Object *o, Msg msg);
static ULONG RootClass_ClearClipboard(Class *cl, Object *o, Msg msg);
static ULONG RootClass_GetLocalizedString(Class *cl, Object *o, Msg msg);
static struct EventListener *RootClass_AddListener(Class *cl, Object *o, Msg msg);
static ULONG RootClass_RemListener(Class *cl, Object *o, Msg msg);
static ULONG RootMessageUnIconify(struct internalScaWindowTask *iwt, struct AppMessage *appMsg);
static void SnapshotIconifiedIcon(struct internalScaWindowTask *iwt);
static void UnSnapshotIconifiedIcon(struct internalScaWindowTask *iwt);
static void UnShadowIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static void GenerateEvents(Class *cl, Object *o, Msg msg);

//----------------------------------------------------------------------------

// public data items :
SCALOSSEMAPHORE ListenerSema;

//----------------------------------------------------------------------------

// private data items :

static struct List ListenerList;

//----------------------------------------------------------------------------



Class * initRootClass(void)
{
	Class *RootClass = NULL;
	struct ScalosClass *classNode;

	d1(kprintf("%s/%s/%d: Enter initRootClass\n", __FILE__, __FUNC__, __LINE__));

	ScalosObtainSemaphore(&ClassListSemaphore);

	d1(kprintf("%s/%s/%d: Got Semaphore\n", __FILE__, __FUNC__, __LINE__));

	NewList(&ListenerList);
	ScalosInitSemaphore(&ListenerSema);

	do	{
		classNode = (struct ScalosClass *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &ScalosClassList, NTYP_PluginClass);
		if (NULL == classNode)
			break;

		classNode->sccl_name = (STRPTR) "Root.sca";

		RootClass = classNode->sccl_class = MakeClass( NULL,
				(STRPTR) "rootclass",
				NULL,
				sizeof(struct RootClassInstance),
				0 );

		if (RootClass)
			{
			// initialize the cl_Dispatcher Hook
			SETHOOKFUNC(RootClass->cl_Dispatcher, RootClass_Dispatcher);
			}
		} while (0);

	d1(kprintf("%s/%s/%d: Created class\n", __FILE__, __FUNC__, __LINE__));

	ScalosReleaseSemaphore(&ClassListSemaphore);

	d1(kprintf("%s/%s/%d: Released semaphore\n", __FILE__, __FUNC__, __LINE__));

	return RootClass;
}


static ULONG RootClass_Dispatcher(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	GenerateEvents(cl, o, msg);

	switch (msg->MethodID)
		{
	case OM_NEW:
		Result = (ULONG) RootClass_New(cl, o, msg);
		break;

	case SCCM_HandleMessages:
		Result = (ULONG) Root_HandleMessages(cl, o, msg);
		break;

	case SCCM_CheckForMessages:
		Result = (ULONG) Root_CheckForMessages(cl, o, msg);
		break;

	case SCCM_Message:
		Result = (ULONG) RootClass_Message(cl, o, msg);
		break;

	case SCCM_RunProcess:
		Result = (ULONG) RootClass_RunProcess(cl, o, msg);
		break;

	case SCCM_AddToClipboard:
		Result = (ULONG) RootClass_AddToClipboard(cl, o, msg);
		break;

	case SCCM_ClearClipboard:
		Result = (ULONG) RootClass_ClearClipboard(cl, o, msg);
		break;

	case SCCM_GetLocalizedString:
		Result = (ULONG) RootClass_GetLocalizedString(cl, o, msg);
		break;

	case SCCM_WindowStartComplete:
		d1(kprintf("%s/%s/%d: SCCM_WindowStartComplete\n", __FILE__, __FUNC__, __LINE__));
		Result = 0;
		break;

	case SCCM_AddListener:
		Result = (ULONG) RootClass_AddListener(cl, o, msg);
		break;

	case SCCM_RemListener:
		Result = RootClass_RemListener(cl, o, msg);
		break;

	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}


static ULONG RootClass_New(Class *cl, Object *o, Msg msg)
{
	o = (Object *) DoSuperMethodA(cl, o, msg);
	if (o)	
		{
		struct RootClassInstance *inst = INST_DATA(cl, o);
		struct opSet *ops = (struct opSet *) msg;

		inst->rci_RootList.rl_WindowTask = (struct ScaWindowTask *) GetTagData(SCCA_WindowTask, (ULONG)NULL, ops->ops_AttrList);
		inst->rci_RootList.rl_internInfos = &iInfos.xii_iinfos;
		}

	return (ULONG) o;
}


static ULONG Root_HandleMessages(Class *cl, Object *o, Msg msg)
{
	struct RootClassInstance *inst = INST_DATA(cl, o);
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) inst->rci_RootList.rl_WindowTask;
	struct Message *NewMsg;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>  iwt_CloseWindow=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
		iwt, iwt->iwt_WinTitle ? iwt->iwt_WinTitle : (STRPTR) "", iwt->iwt_CloseWindow));

	while (iwt->iwt_WindowTask.wt_IconPort)
		{
		d1(kprintf("%s/%s/%ld: iwt=%08lx  IconPort=%08lx \n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort));

		d1(Forbid(); kprintf("%s/%s/%ld: iwt=%08lx  MsgList Head=%08lx  TailPred=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_Head, \
			iwt->iwt_WindowTask.wt_IconPort->mp_MsgList.lh_TailPred); Permit());

		NewMsg = GetMsg(iwt->iwt_WindowTask.wt_IconPort);

		d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  IconPort=%08lx  NewMsg=%08lx \n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort, NewMsg); Permit());
		if (NewMsg)
			{
			ULONG rcvdMsgType;

			if (IDCMP_CLOSEWINDOW == HandleWindowTaskIconPortMessages(iwt, NewMsg, &rcvdMsgType))
				break;
			}
		else
			{
			d1(KPrintF("%s/%s/%ld: iwt_MoveGadId=%ld\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_MoveGadId));
			if (VGADGETID_IDLE == iwt->iwt_MoveGadId)
				break;

			if (!iwt->iwt_CloseWindow)
				{
				// do not WaitPort if window is about to close
				WaitPort(iwt->iwt_WindowTask.wt_IconPort);
				}

			d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
			}
		}

	d1(KPrintF("%s/%s/%ld: END Return 0\n", __FILE__, __FUNC__, __LINE__));

	return (iwt->iwt_CloseWindow ? IDCMP_CLOSEWINDOW : 0UL);
}


static ULONG Root_CheckForMessages(Class *cl, Object *o, Msg msg)
{
	struct RootClassInstance *inst = INST_DATA(cl, o);
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) inst->rci_RootList.rl_WindowTask;
	struct Message *NewMsg;
	struct List MsgList;

	d1(KPrintF("%s/%s/%ld: START iwt=%08lx  <%s>  iwt_CloseWindow=%ld\n", \
		__FILE__, __FUNC__, __LINE__, \
		iwt, iwt->iwt_WinTitle ? iwt->iwt_WinTitle : (STRPTR) "", iwt->iwt_CloseWindow));

	NewList(&MsgList);

	// First move all currently present messages to MsgList
	while (iwt->iwt_WindowTask.wt_IconPort
		&& (NewMsg = GetMsg(iwt->iwt_WindowTask.wt_IconPort)))
		{
		AddTail(&MsgList, &NewMsg->mn_Node);
		}

	// Now handle all stored messages from MsgList
	// and ignore any new messages at wt_IconPort
	// (which might be added during HandleWindowTaskIconPortMessages)
	while ((NewMsg = (struct Message *) RemHead(&MsgList)))
		{
		ULONG rcvdMsgType;

		d1(Forbid(); KPrintF("%s/%s/%ld: iwt=%08lx  IconPort=%08lx  NewMsg=%08lx \n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WindowTask.wt_IconPort, NewMsg); Permit());

		if (IDCMP_CLOSEWINDOW == HandleWindowTaskIconPortMessages(iwt, NewMsg, &rcvdMsgType))
			break;
		}

	d1(KPrintF("%s/%s/%ld: END Return 0\n", __FILE__, __FUNC__, __LINE__));

	return (iwt->iwt_CloseWindow ? IDCMP_CLOSEWINDOW : 0UL);
}


static ULONG RootClass_Message(Class *cl, Object *o, Msg msg)
{
	struct RootClassInstance *inst = INST_DATA(cl, o);
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) inst->rci_RootList.rl_WindowTask;
	struct msg_Message *msm = (struct msg_Message *) msg;
	struct AppMessage *aMsg = (struct AppMessage *) msm->msm_iMsg;
	ULONG Result;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle));
	d1(kprintf("%s/%s/%ld: am_ID=%08lx\n", __FILE__, __FUNC__, __LINE__, aMsg->am_ID));

	if (MAKE_ID('I','C','F','Y') == aMsg->am_ID)
		{
		Result = RootMessageUnIconify(iwt, aMsg);
		}
	else
		{
		const struct IDCMPTableEntry *ite;
		BOOL DoReply = TRUE;

		d1(kprintf("%s/%s/%ld: Class=%08lx  Code=%04lx\n", __FILE__, __FUNC__, __LINE__, msm->msm_iMsg->Class, msm->msm_iMsg->Code));

		Result = (IDCMP_CLOSEWINDOW == msm->msm_iMsg->Class);

		ite = (struct IDCMPTableEntry *) bsearch(&msm->msm_iMsg->Class,
			IDCMPFuncTable,
			IDCMPFuncTableSize,
                        sizeof(struct IDCMPTableEntry),
			IDCMPFuncTableComp);

		if (ite)
			{
			d1(kprintf("%s/%s/%ld: Class=%08lx  ite_IDCMP=%08lx\n", __FILE__, __FUNC__, __LINE__, msm->msm_iMsg->Class, ite->ite_IDCMP));

			if ((*ite->ite_MsgFunc)(iwt, msm->msm_iMsg))
				{
				d1(KPrintF("%s/%s/%ld: Class=%08lx  NOREPLY\n", __FILE__, __FUNC__, __LINE__, msm->msm_iMsg->Class));
				DoReply = FALSE;
				}
			}

		if (DoReply)
			ReplyMsg(&msm->msm_iMsg->ExecMessage);
		}

	return Result;
}


static int IDCMPFuncTableComp(const void *keyval, const void *datum)
{
	ULONG IDCMPClass = *((ULONG *) keyval);
	const struct IDCMPTableEntry *ite = (struct IDCMPTableEntry *) datum;

	if (IDCMPClass < ite->ite_IDCMP)
		return -1;
	if (IDCMPClass > ite->ite_IDCMP)
		return 1;

	return 0;
}

static ULONG RootClass_RunProcess(Class *cl, Object *o, Msg msg)
{
	struct RootClassInstance *inst = INST_DATA(cl, o);
	struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) inst->rci_RootList.rl_WindowTask;
	struct msg_RunProcess *mrp = (struct msg_RunProcess *) msg;
	ULONG Result = FALSE;

	d1(kprintf("%s/%s/%ld: iwt=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, iwt, iwt->iwt_WinTitle, mrp->mrp_ReplyPort));

	if (SCCV_RunProcess_WaitReply == (ULONG) mrp->mrp_ReplyPort)
		{
		if (ScalosAttemptSemaphoreShared(&iwt->iwt_ChildProcessSemaphore))
			{
			iwt->iwt_IconPortOutstanding++;

			d1(kprintf("%s/%s/%ld: wt_IconPort=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt->iwt_WindowTask.wt_IconPort));

			Result = RunProcess(&iwt->iwt_WindowTask, mrp->mrp_EntryPoint,
				(mrp->mrp_ArgSize + 3) / sizeof(LONG),
				(struct WBArg *) mrp->mrp_Args, iwt->iwt_WindowTask.wt_IconPort);

			if (Result)
				Result = WaitReply(iwt->iwt_WindowTask.wt_IconPort, iwt, MTYP_RunProcess);

			if (iwt->iwt_IconPortOutstanding > 0)
				iwt->iwt_IconPortOutstanding--;

			ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
			}
		}
	else
		{
		Result = RunProcess(&iwt->iwt_WindowTask, mrp->mrp_EntryPoint, 
			(mrp->mrp_ArgSize + 3) / sizeof(LONG), 
			(struct WBArg *) mrp->mrp_Args, mrp->mrp_ReplyPort);
		}

	return Result;
}


static ULONG RootClass_AddToClipboard(Class *cl, Object *o, Msg msg)
{
	struct msg_AddToClipboard *acp = (struct msg_AddToClipboard *) msg;
	BPTR destDirLock;
	BPTR newLock = (BPTR)NULL;
	ULONG IconType = 0;
	struct FileTransArg *fta;

	d1(kprintf("%s/%s/%ld: in=%08lx  <%s>  Opcode=%ld  Clipboard=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, acp->acb_in, GetIconName(acp->acb_in), OpCode, &globalCopyClipBoard));

	GetAttr(IDTA_Type, acp->acb_in->in_Icon, &IconType);

	if (0 == IconType || WBAPPICON == IconType)
		return 0;

	if (acp->acb_in->in_DeviceIcon)
		{
		destDirLock = newLock = DiskInfoLock(acp->acb_in);
		}
	else
		{
		if (acp->acb_in->in_Lock)
			destDirLock = acp->acb_in->in_Lock;
		else
			destDirLock = acp->acb_iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock;
		}

	fta = ScalosAlloc(sizeof(struct FileTransArg));
	if (fta)
		{
		fta->ftarg_iwt = acp->acb_iwt;
		fta->ftarg_in = acp->acb_in;

		fta->ftarg_Opcode = acp->acb_Opcode;

		fta->ftarg_Arg.wa_Name = AllocCopyString(GetIconName(acp->acb_in));
		fta->ftarg_Arg.wa_Lock = DupLock(destDirLock);

		d1(kprintf("%s/%s/%ld:  wa_Name=<%s>  wa_Lock=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, fta->ftarg_Arg.wa_Name, fta->ftarg_Arg.wa_Lock));

		if (fta->ftarg_Arg.wa_Name && fta->ftarg_Arg.wa_Lock)
			{
			ScalosObtainSemaphore(&ClipboardSemaphore);
			AddTail(&globalCopyClipBoard, &fta->ftarg_Node);
			ScalosReleaseSemaphore(&ClipboardSemaphore);
			}
		else
			ScalosFree(fta);
		}

	if (newLock)
		UnLock(newLock);

	return 0;
}


static ULONG RootClass_ClearClipboard(Class *cl, Object *o, Msg msg)
{
	struct FileTransArg *fta;

	d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));

	while (1)
		{
		ScalosObtainSemaphore(&ClipboardSemaphore);
		fta = (struct FileTransArg *) RemHead(&globalCopyClipBoard);
		ScalosReleaseSemaphore(&ClipboardSemaphore);

		if (NULL == fta)
			break;

		UnShadowIcon(fta->ftarg_iwt, fta->ftarg_in);

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
		}

	return 0;
}


static ULONG RootClass_GetLocalizedString(Class *cl, Object *o, Msg msg)
{
	struct msg_GetLocString *mgl = (struct msg_GetLocString *) msg;

	return (ULONG) GetLocString(mgl->mgl_StringID);
}


static struct EventListener *RootClass_AddListener(Class *cl, Object *o, Msg msg)
{
	const struct msg_AddListener *mal = (const struct msg_AddListener *) msg;
	struct EventListener *el;

	el = ScalosAlloc(sizeof(struct EventListener));
	if (el)
		{
		el->el_Method = mal->mal_Method;
		el->el_Port = mal->mal_Port;
		el->el_Count = mal->mal_Count;

		ScalosObtainSemaphore(&ListenerSema);
		AddTail(&ListenerList, &el->el_Node);
		ScalosReleaseSemaphore(&ListenerSema);
		};

	return el;
}


static ULONG RootClass_RemListener(Class *cl, Object *o, Msg msg)
{
	const struct msg_RemoveListener *mrl = (const struct msg_RemoveListener *) msg;
	struct EventListener *el;

	ScalosObtainSemaphore(&ListenerSema);

	for (el = (struct EventListener *) ListenerList.lh_Head;
		el != (struct EventListener *) &ListenerList.lh_Tail;
		el = (struct EventListener *) el->el_Node.ln_Succ)
		{
		if ((APTR) el == mrl->mrl_EventHandle)
			{
			Remove(&el->el_Node);
			ScalosFree(el);
			break;
			}
		}

	ScalosReleaseSemaphore(&ListenerSema);

	return 0;
}


// process message vom iconified window's AppIcon
static ULONG RootMessageUnIconify(struct internalScaWindowTask *iwt, struct AppMessage *appMsg)
{
	struct ScaWindowStruct *ws = (struct ScaWindowStruct *) appMsg->am_UserData;
	ULONG Result = 0;

	d1(kprintf("%s/%s/%ld: Class=%ld  NumARgs=%ld\n", __FILE__, __FUNC__, __LINE__, appMsg->am_Class, appMsg->am_NumArgs));

	switch (appMsg->am_Class)
		{
	case AMCLASSICON_Open:
		if (0 == appMsg->am_NumArgs)
			{
			DoMethod(iwt->iwt_WindowTask.mt_WindowObject, SCCM_Window_UnIconify);
			}
		break;

	case AMCLASSICON_Snapshot:
		d1(kprintf("%s/%s/%ld: AMCLASSICON_Snapshot\n", __FILE__, __FUNC__, __LINE__));
		SnapshotIconifiedIcon((struct internalScaWindowTask *) ws->ws_WindowTask);
		break;

	case AMCLASSICON_UnSnapshot:
		d1(kprintf("%s/%s/%ld: AMCLASSICON_UnSnapshot\n", __FILE__, __FUNC__, __LINE__));
		UnSnapshotIconifiedIcon((struct internalScaWindowTask *) ws->ws_WindowTask);
		break;
		}

	return Result;
}


static void SnapshotIconifiedIcon(struct internalScaWindowTask *iwt)
{
	BPTR oldDir = NOT_A_LOCK;
	BPTR parentLock = (BPTR)NULL;
	Object *allocIconObj = NULL;
	Object *iconObj = ClassGetWindowIconObject(iwt, &allocIconObj);

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	do	{
		struct ExtGadget *gg = (struct ExtGadget *) iwt->iwt_myAppIcon;
		char Buffer[20];

		if (NULL == iconObj)
			break;
		if (NULL == gg)
			break;

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RootWindow)
			parentLock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		else
			parentLock = ParentDir(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

		if ((BPTR)NULL == parentLock)
			break;

		oldDir = CurrentDir(parentLock);

		ScaFormatStringMaxLength(Buffer, sizeof(Buffer), "%ld", (LONG) gg->LeftEdge);
		SetToolType(iconObj, "SCALOS_ICONIFIED_XPOS", Buffer, FALSE);

		ScaFormatStringMaxLength(Buffer, sizeof(Buffer), "%ld", (LONG) gg->TopEdge);
		SetToolType(iconObj, "SCALOS_ICONIFIED_YPOS", Buffer, TRUE);
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (parentLock)
		UnLock(parentLock);

	if (allocIconObj)
		DisposeIconObject(allocIconObj);
}


static void UnSnapshotIconifiedIcon(struct internalScaWindowTask *iwt)
{
	BPTR oldDir = NOT_A_LOCK;
	BPTR parentLock = (BPTR)NULL;
	Object *allocIconObj = NULL;
	Object *iconObj = ClassGetWindowIconObject(iwt, &allocIconObj);

	d1(kprintf("%s/%s/%ld: iconObj=%08lx\n", __FILE__, __FUNC__, __LINE__, iconObj));

	do	{
		if (NULL == iconObj)
			break;

		if (iwt->iwt_WindowTask.mt_WindowStruct->ws_Flags & WSV_FlagF_RootWindow)
			parentLock = DupLock(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);
		else
			parentLock = ParentDir(iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

		if ((BPTR)NULL == parentLock)
			break;;

		oldDir = CurrentDir(parentLock);

		SetToolType(iconObj, "SCALOS_ICONIFIED_XPOS", "-1", FALSE);
		SetToolType(iconObj, "SCALOS_ICONIFIED_YPOS", "-1", TRUE);
		} while (0);

	if (IS_VALID_LOCK(oldDir))
		CurrentDir(oldDir);
	if (parentLock)
		UnLock(parentLock);

	if (allocIconObj)
		DisposeIconObject(allocIconObj);
}


static void UnShadowIcon(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ScaWindowStruct *ws;

	// check if both iwt and in are still valid

	SCA_LockWindowList(SCA_LockWindowList_Shared);

	for (ws=winlist.wl_WindowStruct; ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
		{
		struct internalScaWindowTask *iwt2 = (struct internalScaWindowTask *) ws->ws_WindowTask;

		if (iwt2 == iwt)
			{
			// we found the window
			struct ScaIconNode *in2;

			ScalosLockIconListShared(iwt2);

			for (in2=iwt2->iwt_WindowTask.wt_IconList; in2; in2 = (struct ScaIconNode *) in2->in_Node.mln_Succ)
				{
				if (in2 == in)
					{
					// we found the icon
					struct ScaIconNode inCopy;
					struct ScaIconNode *IconList = &inCopy;
					ULONG UserFlags = 0L;

					GetAttr(IDTA_UserFlags, in2->in_Icon, &UserFlags);
					UserFlags &= ~ICONOBJ_USERFLAGF_DrawShadowed;
					SetAttrs(in2->in_Icon, IDTA_UserFlags, UserFlags, TAG_END);

					inCopy = *in2;

					inCopy.in_Node.mln_Succ = NULL;

					RemoveIcons(iwt2, &IconList);
					RefreshIconList(iwt2, IconList, NULL);

					break;
					}
				}

			ScalosUnLockIconList(iwt2);
			break;
			}
		}

	SCA_UnLockWindowList();
}


static void GenerateEvents(Class *cl, Object *o, Msg msg)
{
	struct EventListener *el;

	ScalosObtainSemaphore(&ListenerSema);

	for (el = (struct EventListener *) ListenerList.lh_Head;
		el != (struct EventListener *) &ListenerList.lh_Tail; )
		{
		struct EventListener *elNext = (struct EventListener *) el->el_Node.ln_Succ;

		if (msg->MethodID == el->el_Method)
			{
			struct SM_RootEvent *smre = (struct SM_RootEvent *) SCA_AllocMessage(MTYP_RootEvent, 0);

			d1(KPrintF("%s/%s/%ld:  smre=%08lx\n", __FILE__, __FUNC__, __LINE__, smre));

			if (smre)
				{
				smre->smre_MethodID = msg->MethodID;
				smre->smre_EventHandle = el;
				smre->smre_Class = cl;
				smre->smre_Object = o;
				smre->smre_Message = msg;
				smre->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;

				PutMsg(el->el_Port, &smre->ScalosMessage.sm_Message);
				}

			if (1 == el->el_Count--)
				{
				Remove(&el->el_Node);
				ScalosFree(el);
				}
			break;
			}

		el = elNext;
		}

	ScalosReleaseSemaphore(&ListenerSema);
}

