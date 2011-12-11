/*
**	EmulationProcess.c
**
**	Terminal emulation process
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC struct Process *EmulationProcess;

STATIC VOID SAVE_DS
EmulationProcessEntry(VOID)
{
	if(TerminalQueue = CreateMsgQueue(0,40))
	{
		struct MsgQueue	*Queue;
		struct DataMsg *Msg;
		ULONG Mask;
		BOOL Done;

		EmulationProcess = (struct Process *)FindTask(NULL);

		Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

		Queue = TerminalQueue;

		Mask = SIG_KILL | TerminalQueue->SigMask;

		Done = FALSE;

		do
		{
			if(Wait(Mask) & SIG_KILL)
			{
				Forbid();

				TerminalQueue = NULL;

				Permit();

				break;
			}

			ObtainSemaphore(&TerminalSemaphore);

			ClearCursor();

			while(Msg = GetMsgItem(TerminalQueue))
			{
					/* Remember the data. */

				if(RememberOutput)
					RememberOutputText(Msg->Data,Msg->Size);

				(*ConProcessData)(Msg->Data,Msg->Size);

				DeleteMsgItem((struct MsgItem *)Msg);

				if(SetSignal(0,0) & SIG_KILL)
				{
					Forbid();

					TerminalQueue = NULL;

					Permit();

					Done = TRUE;

					break;
				}
			}

			DrawCursor();

			ReleaseSemaphore(&TerminalSemaphore);
		}
		while(!Done);

		while(Msg = GetMsgItem(Queue))
			DeleteMsgItem((struct MsgItem *)Msg);

		DeleteMsgQueue(Queue);
	}

	Forbid();

	EmulationProcess = NULL;

	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
}

VOID
DeleteEmulationProcess()
{
	ShakeHands((struct Task *)EmulationProcess,SIG_KILL);
}

BOOL
CreateEmulationProcess()
{
	if(!EmulationProcess)
	{
		StartProcessWaitForHandshake("term Emulation Process",(TASKENTRY)EmulationProcessEntry,
			NP_StackSize,	8000,
			NP_WindowPtr,	Window,
		TAG_DONE);
	}

	if(EmulationProcess)
		return(TRUE);
	else
		return(FALSE);
}
