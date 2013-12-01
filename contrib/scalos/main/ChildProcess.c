// ChildProcess.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <utility/hooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local data items

//----------------------------------------------------------------------------

// local functions

static SAVEDS(void) INTERRUPT ProcRunnerTask(void);

//----------------------------------------------------------------------------

// NumLongs : number of longwords in ArgArray (!!not number of entries!!)
BOOL RunProcess(struct ScaWindowTask *wt, RUNPROCFUNC Routine, ULONG NumLongs,
	struct WBArg *ArgArray, struct MsgPort *ReplyPort)
{
	BOOL Success = FALSE;
	struct SM_RunProcess *msg;
	ULONG NumBytes;

	d1(kprintf("%s/%s/%ld: START  wt=%08lx  Routine=%08lx  NumLongs=%lu  ArgArray=%08lx  ReplyPort=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, wt, Routine, NumLongs, ArgArray, ReplyPort));

	if (NumLongs > 77)
		return FALSE;

	NumBytes = NumLongs * sizeof(ULONG);

	msg = (struct SM_RunProcess *) SCA_AllocMessage(MTYP_RunProcess, NumBytes);
	if (msg)
		{
		msg->ScalosMessage.sm_Message.mn_ReplyPort = ReplyPort;
		msg->WindowTask = wt;
		msg->EntryPoint = Routine;

		if (NumBytes > 0)
			CopyMem(ArgArray, &msg->Flags, NumBytes);

		d1(KPrintF("%s/%s/%ld: msg=%08lx  iwt=%08lx  Routine=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, iwt, Routine));

		Success = ChildProcessRun((struct internalScaWindowTask *) wt,
			&msg->ScalosMessage,
			NP_Priority, 0,
			NP_Name, (ULONG) "Scalos_SubProcess_Runner",
			TAG_END);

		d1(kprintf("%s/%s/%ld: newProc=%08lx\n", __FILE__, __FUNC__, __LINE__, newProc));
		}

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

BOOL ChildProcessRun(struct internalScaWindowTask *iwt, struct ScalosMessage *msg, ULONG FirstTag, ...)
{
	va_list args;
	struct TagItem *TagList = NULL;
	BOOL Success = FALSE;
	struct SM_StartChildProcess *mStart = NULL;
	struct MsgPort *StartReplyPort;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	va_start(args, FirstTag);

	do	{
		STATIC_PATCHFUNC(ProcRunnerTask)
		struct Process *newProc;
		BPTR WBPath;

		StartReplyPort = CreateMsgPort();
		if (NULL == StartReplyPort)
			break;

		mStart = (struct SM_StartChildProcess *) SCA_AllocMessage(MTYP_StartChildProcess, 0);
		if (NULL == mStart)
			break;

		mStart->ScalosMessage.sm_Message.mn_ReplyPort = StartReplyPort;
		mStart->smscp_WindowTask = &iwt->iwt_WindowTask;

		TagList = ScalosVTagList(FirstTag, args);
		if (NULL == TagList)
			break;

		ScalosObtainSemaphoreShared(&iwt->iwt_ChildProcessSemaphore);

		WBPath = DupWBPathList();
		d1(kprintf("%s/%s/%ld: WBPath=%08lx\n", __FILE__, __FUNC__, __LINE__, WBPath));

		newProc = CreateNewProcTags(NP_WindowPtr, NULL,
			NP_StackSize, CurrentPrefs.pref_DefaultStackSize,
			NP_Cli, TRUE,
			NP_Entry, (ULONG) PATCH_NEWFUNC(ProcRunnerTask),
			WBPath ? NP_Path : TAG_IGNORE, WBPath,
			TAG_MORE, TagList,
			TAG_END);

		d1(kprintf("%s/%s/%ld: newProc=%08lx\n", __FILE__, __FUNC__, __LINE__, newProc));

		if (NULL == newProc)
			{
			ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
			break;
			}

		d1(kprintf("%s/%s/%ld: sending SM_StartChildProcess\n", __FILE__, __FUNC__, __LINE__));

		PutMsg(&newProc->pr_MsgPort, &mStart->ScalosMessage.sm_Message);

		d1(kprintf("%s/%s/%ld: wait for SM_StartChildProcess reply\n", __FILE__, __FUNC__, __LINE__));

		// wait for message reply fro child process
		WaitPort(StartReplyPort);
		mStart = (struct SM_StartChildProcess *) GetMsg(StartReplyPort);

		d1(kprintf("%s/%s/%ld: SM_StartChildProcess reply received - mStart=%08lx\n", __FILE__, __FUNC__, __LINE__, mStart));

		if (NULL == mStart)
			{
			ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
			break;
			}

		// keep semaphore locked until we have received the SM_StartChildProcess
		// and know that the child process has locked the semaphore
		ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);

		d1(kprintf("%s/%s/%ld: send SM_RunProcess msg=%08lx\n", __FILE__, __FUNC__, __LINE__, msg));

		PutMsg(&newProc->pr_MsgPort, &msg->sm_Message);
		msg = NULL;
	
		Success = TRUE;
		d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));
		} while (0);

	if (StartReplyPort)
		DeleteMsgPort(StartReplyPort);
	if (mStart)
		SCA_FreeMessage(&mStart->ScalosMessage);
	if (TagList)
		FreeTagItems(TagList);
	if (msg)
		SCA_FreeMessage(msg);

	va_end(args);

	d1(kprintf("%s/%s/%ld: END Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
}

//----------------------------------------------------------------------------

// SubProcess Runner Task
static SAVEDS(void) INTERRUPT ProcRunnerTask(void)
{
	struct Process *myProc = (struct Process *) FindTask(NULL);
	struct internalScaWindowTask *iwt = NULL;
	struct SM_StartChildProcess *mStart;
	struct SM_RunProcess *msg;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	// Scalos MUST not quit while subprocess is running
	ScalosObtainSemaphoreShared(&QuitSemaphore);

	d1(kprintf("%s/%s/%ld: wait for MTYP_StartChildProcess\n", __FILE__, __FUNC__, __LINE__));

	// First, wait for MTYP_StartChildProcess message and reply it
	do	{
		WaitPort(&myProc->pr_MsgPort);
		mStart = (struct SM_StartChildProcess *) GetMsg(&myProc->pr_MsgPort);

		d1(kprintf("%s/%s/%ld: mStart=%08lx\n", __FILE__, __FUNC__, __LINE__, mStart));
		if (mStart)
			{
			iwt = (struct internalScaWindowTask *) mStart->smscp_WindowTask;

			d1(kprintf("%s/%s/%ld: MTYP_StartChildProcess received - iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

			if (iwt)
				{
				ScalosObtainSemaphoreShared(&iwt->iwt_ChildProcessSemaphore);
				}

			ReplyMsg(&mStart->ScalosMessage.sm_Message);
			}
		} while (NULL == mStart);

	d1(kprintf("%s/%s/%ld: wait for SM_RunProcess - iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	do	{
		WaitPort(&myProc->pr_MsgPort);
		msg = (struct SM_RunProcess *) GetMsg(&myProc->pr_MsgPort);
		} while (NULL == msg);

	d1(KPrintF("%s/%s/%ld: SM_RunProcess received - msg=%08lx  ReplyPort=%08lx  Len=%lu\n", \
		__FILE__, __FUNC__, __LINE__, msg, msg->ScalosMessage.sm_Message.mn_ReplyPort, \
                msg->ScalosMessage.sm_Message.mn_Length));

	if (ID_IMSG == msg->ScalosMessage.sm_Signature)
		{
		struct SM_AsyncBackFill *smab;

		switch (msg->ScalosMessage.sm_MessageType)
			{
		case MTYP_RunProcess:
			d1(kprintf("%s/%s/%ld: MTYP_RunProcess msg=%08lx  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, iwt));

			(msg->EntryPoint)(&msg->Flags, msg);
			break;

		case MTYP_AsyncBackFill:
			smab = (struct SM_AsyncBackFill *) msg;

			d1(KPrintF("%s/%s/%ld: MTYP_AsyncBackFill msg=%08lx  ReplyPort=%08lx  iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, smab, msg->ScalosMessage.sm_Message.mn_ReplyPort, iwt));
			d1(KPrintF("%s/%s/%ld: smab_BackfillFunc=%08lx\n", __FILE__, __FUNC__, __LINE__, smab->smab_BackfillFunc));

			(smab->smab_BackfillFunc)(NULL, msg);
			break;

		default:
			d1(kprintf("%s/%s/%ld: unkown sm_MessageType=%ld  msg=%08lx  iwt=%08lx\n", \
				__FILE__, __FUNC__, __LINE__, msg->ScalosMessage.sm_MessageType, msg, iwt));
			break;
			}

		d1(KPrintF("%s/%s/%ld: msg=%08lx  ReplyPort=%08lx  MsgType=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, msg, msg->ScalosMessage.sm_Message.mn_ReplyPort, msg->ScalosMessage.sm_MessageType));
		}

	d1(KPrintF("%s/%s/%ld: msg=%08lx  ReplyPort=%08lx\n", __FILE__, __FUNC__, __LINE__, msg, msg->ScalosMessage.sm_Message.mn_ReplyPort));

	if (msg->ScalosMessage.sm_Message.mn_ReplyPort)
		{
		d1(Forbid(); KPrintF("%s/%s/%ld: MsgList Head=%08lx  TailPred=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, msg->ScalosMessage.sm_Message.mn_ReplyPort->mp_MsgList.lh_Head, \
			msg->ScalosMessage.sm_Message.mn_ReplyPort->mp_MsgList.lh_TailPred); Permit());

		ReplyMsg(&msg->ScalosMessage.sm_Message);

		d1(Forbid(); KPrintF("%s/%s/%ld: MsgList Head=%08lx  Succ=%08lx  TailPred=%08lx  Pred=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, \
			msg->ScalosMessage.sm_Message.mn_ReplyPort->mp_MsgList.lh_Head, \
			msg->ScalosMessage.sm_Message.mn_ReplyPort->mp_MsgList.lh_Head->ln_Succ, \
			msg->ScalosMessage.sm_Message.mn_ReplyPort->mp_MsgList.lh_TailPred, \
			msg->ScalosMessage.sm_Message.mn_ReplyPort->mp_MsgList.lh_TailPred->ln_Pred); Permit());
		}
	else
		SCA_FreeMessage(&msg->ScalosMessage);

	if (iwt)
		{
		d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));
		ScalosReleaseSemaphore(&iwt->iwt_ChildProcessSemaphore);
		}

	d1(kprintf("%s/%s/%ld: iwt=%08lx\n", __FILE__, __FUNC__, __LINE__, iwt));

	ScalosReleaseSemaphore(&QuitSemaphore);

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------


