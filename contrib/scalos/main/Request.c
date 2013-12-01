// Request.c
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
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>
#include <scalos/GadgetBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "functions.h"
#include "locale.h"
#include "Variables.h"

//----------------------------------------------------------------------------
// Revision history :
//
// 20011117	jl	initial history
//----------------------------------------------------------------------------

// local data definitions

//----------------------------------------------------------------------------

// local functions

static SAVEDS(void) INTERRUPT RequestTask(void);

//----------------------------------------------------------------------------

// public data items :

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT RequestTask(void)
{
	struct Process *myProc = (struct Process *) FindTask(NULL);
	struct SM_Requester *msg;

	do	{
		WaitPort(&myProc->pr_MsgPort);
		msg = (struct SM_Requester *) GetMsg(&myProc->pr_MsgPort);
		} while (NULL == msg);

	msg->smrq_MultiPurpose.smrq_ReqResult = EasyRequestArgs(msg->smrq_MultiPurpose.smrq_ParentWindow,
		&msg->smrq_ez, NULL,
		msg->smrq_ArgList);

	ReplyMsg(&msg->ScalosMessage.sm_Message);
}


void UseAsyncRequestArgs(struct internalScaWindowTask *iwt, 
	LONG BodyTextNum, LONG GadgetTextNum, ULONG NumArgs, ...)
{
	STATIC_PATCHFUNC(RequestTask)
	struct SM_Requester *msg;
	struct Process *RequestProc;
	va_list args;

	va_start(args, NumArgs);

	msg = (struct SM_Requester *) SCA_AllocMessage(MTYP_Requester, sizeof(ULONG) * NumArgs);
	if (msg)
		{
		ULONG *pArg = (ULONG *) msg->smrq_ArgListBuffer;

		msg->smrq_ArgList = msg->smrq_ArgListBuffer;
		while (NumArgs--)
			*pArg++ = va_arg(args, ULONG);

		msg->smrq_MultiPurpose.smrq_ParentWindow = iwt->iwt_WindowTask.wt_Window;
		msg->ScalosMessage.sm_Message.mn_ReplyPort = iwt->iwt_WindowTask.wt_IconPort;

		msg->smrq_ez.es_StructSize = sizeof(msg->smrq_ez);
		msg->smrq_ez.es_Flags = 0;
		msg->smrq_ez.es_Title = (STRPTR) GetLocString(MSGID_REQTITLE);
		msg->smrq_ez.es_TextFormat = (STRPTR) GetLocString(BodyTextNum);
		msg->smrq_ez.es_GadgetFormat = (STRPTR) GetLocString(GadgetTextNum);

		RequestProc = CreateNewProcTags(NP_WindowPtr, NULL,
			NP_Name, (ULONG) "Scalos_Request",
			NP_Entry, (ULONG) PATCH_NEWFUNC(RequestTask),
			TAG_END);

		if (RequestProc)
			{
			PutMsg(&RequestProc->pr_MsgPort, &msg->ScalosMessage.sm_Message);
			}
		}

	va_end(args);
}


LONG UseRequestArgs(struct Window *parentWin, 
	ULONG BodyTextNum, ULONG GadgetsTextNum, ULONG NumArgs, ...)
{
	LONG Result = 0;
	va_list args;
	ULONG *ArgList;

	va_start(args, NumArgs);

	ArgList = ScalosAlloc(1 + NumArgs * sizeof(ULONG));
	if (ArgList)
		{
		ULONG *pArg = ArgList;

		while (NumArgs--)
			*pArg++ = va_arg(args, ULONG);

		Result = UseRequest(parentWin, BodyTextNum, GadgetsTextNum, ArgList);
		ScalosFree(ArgList);
		}

	va_end(args);

	return Result;
}


LONG UseRequest(struct Window *parentWin, ULONG BodyTextNum, 
	ULONG GadgetsTextNum, APTR ArgList)
{
	struct EasyStruct ez;

	ez.es_StructSize = sizeof(ez);
	ez.es_Flags = 0;
	ez.es_Title = (STRPTR) GetLocString(MSGID_REQTITLE);
	ez.es_TextFormat = (STRPTR) GetLocString(BodyTextNum);
	ez.es_GadgetFormat = (STRPTR) GetLocString(GadgetsTextNum);

	return EasyRequestArgs(parentWin, &ez, NULL, ArgList);
}


