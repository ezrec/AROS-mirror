/*****************************************************************
------------------------------------------------------------------

	Subtask Administration

------------------------------------------------ tab-size = 4 ----
*****************************************************************/

#define MAGICNUMBER		0x0CBF0CBF

#include <assert.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <exec/tasks.h>
#include <exec/semaphores.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#ifdef __MORPHOS__
#include <public/proto/libamiga/amiga_protos.h>
#else
#include <clib/alib_protos.h>
#endif

#include "Mystic_Global.h"
#include "Mystic_Subtask.h"
#include "Mystic_Debug.h"



/*********************************************************************
----------------------------------------------------------------------

	SubTaskEntry

----------------------------------------------------------------------
*********************************************************************/

void SAVEDS SubTaskEntry(void)
{
	struct Task *mytask;
	struct subtask *subtask = NULL;
	char *myname;
	struct MsgPort *myport;
	BOOL success = TRUE;


	/*	find own name - it will be the same as for the message port */

	mytask = FindTask(NULL);
	myname = mytask->tc_Node.ln_Name;


	if (!(myport = CreatePort(myname, 1)))
	{
		DB(kprintf("*** Subtask %s Error: could not create message port.\n", myname));
		success = FALSE;
	}


	/*	wait for parent's message to arrive at the msgport */

	if (success)
	{
		subtask = (struct subtask *) WaitPort(myport);

		if (subtask->ID != MAGICNUMBER)
		{
			DB(kprintf("*** Subtask %s Error: handshaking error.\n", myname));
			success = FALSE;
		}
	}

	if ((subtask->signal_abort = AllocSignal(-1)) == -1)
	{
		DB(kprintf("*** Subtask %s Error: no signal.\n", myname));
		success = FALSE;
	}


	if (success)
	{
		if (subtask->initfunction)
		{
			success = subtask->initfunction((APTR) subtask->data);

			if (!success)
			{
				subtask->running = TRUE;
				Signal(subtask->parenttask, 1 << subtask->signal_go);
			}
		}

		if (success)
		{
			Forbid();

			/*	signal parent task that we're running */

			Signal(subtask->parenttask, 1 << subtask->signal_go);
			subtask->running = TRUE;

			/*	call function */

			if (!(SetSignal(0,0) & (1 << subtask->signal_abort)))
			{
				/*	now set our priority */

				SetTaskPri(mytask, subtask->prio);

				Permit();
				DB(kprintf("calling subtask function\n"));

				/*	call function */

				subtask->result = subtask->function((APTR)subtask, subtask->signal_abort);
			}
			else
			{
				Permit();
			}
		}
	}


	/*	close down in forbid status completely */

	Forbid();

	/*	signal parent task that we're ready */

	subtask->running = FALSE;

	if (subtask)
	{
		if (!subtask->selfdestruct)
		{
			Signal(subtask->parenttask, 1 << subtask->signal_ready);
		//	ReplyMsg((struct Message *) subtask);
			FreeSignal(subtask->signal_abort);
		}
		else
		{
			DB(kprintf("§§§ selfdestruct task closing down\n"));
			FreeSignal(subtask->signal_abort);
			free(subtask);
		}
	}

	if (myport)
	{
		DeletePort(myport);
	}

}

#ifdef __MORPHOS__
struct EmulLibEntry SubTaskEntry_GATE = { TRAP_LIBNR, 0, (void (*)(void))SubTaskEntry };
#endif

/*********************************************************************
----------------------------------------------------------------------

	subtask = SubTask(function, data, stacksize, prio, initfunc, selfdestruct)

----------------------------------------------------------------------
*********************************************************************/

APTR SubTask(LONG (* function)(struct subtask *, BYTE),
	APTR data, ULONG stacksize, WORD prio, char *name,
	BOOL (* initfunction)(struct subtask *),
	BOOL selfdestruct)
{
	struct subtask *st;
	BOOL success = FALSE;

	if ((st = malloc(sizeof(struct subtask))))
	{
		memset(st, 0, sizeof(struct subtask));

		st->parenttask = FindTask(NULL);
		st->signal_ready = -1;
		st->signal_go = -1;

		if (selfdestruct)
		{
			success = TRUE;
		}
		else
		{
			if ((st->signal_ready = AllocSignal(-1)) >= 0)
			{
				success = TRUE;
			}
		}

		if (success)
		{
			success = FALSE;

			if ((st->signal_go = AllocSignal(-1)) >= 0)
			{
				InitSemaphore(&st->datasemaphore);
				st->ID = MAGICNUMBER;
				st->function = function;
				st->initfunction = initfunction;
				st->data = data;
				st->prio = prio;
				st->running = TRUE;	//FALSE;
				st->msg.mn_Length = sizeof(struct subtask);
				st->selfdestruct = selfdestruct;

				sprintf(st->procname, name, GetUniqueID());

#ifdef __MORPHOS__
				if (st->subproc = CreateNewProcTags(NP_Entry, (ULONG) &SubTaskEntry_GATE,
					NP_StackSize, stacksize > 32768 ? stacksize: 32768,
					NP_Name, (ULONG) st->procname, TAG_DONE))
#else
				if ((st->subproc = CreateNewProcTags(NP_Entry, (ULONG) SubTaskEntry,
					NP_StackSize, stacksize > 4000 ? stacksize: 4000,
					NP_Name, (ULONG) st->procname, TAG_DONE)))
#endif
				{
					struct MsgPort *port = NULL;

					while (!port)
					{
						Forbid();
						port = FindPort(st->procname);
						Permit();
						if (!port)
						{
							Delay(1);
						}
					}

					PutMsg(port, (struct Message *) st);

					Wait(1 << st->signal_go);

					success = TRUE;

				}
				FreeSignal(st->signal_go);
			}
		}
	}


	if (!success)
	{
		if (st)
		{
			FreeSignal(st->signal_ready);
			free(st);
			st = NULL;
		}
	}

	return (APTR) st;
}


/*********************************************************************
----------------------------------------------------------------------

	UBYTE SubTaskReadySignal(subtask)

----------------------------------------------------------------------
*********************************************************************/

BYTE SubTaskReadySignal(APTR subtask)
{
	return ((struct subtask *) subtask)->signal_ready;
}


/*********************************************************************
----------------------------------------------------------------------

	AbortSubTask(subtask)

	when this function returns, it must not be assumed
	that the subtask actually aborted. you must still
	WaitSubTaks() or CloseSubTask() thereafter.

----------------------------------------------------------------------
*********************************************************************/

void AbortSubTask(APTR subtask)
{
	if (subtask)
	{
		struct subtask *st = (struct subtask *) subtask;

		DB(kprintf("§ aborting subtask %s:%lx\n", st->procname, subtask));

		Forbid();
		if (st->running)
		{
			Signal(&st->subproc->pr_Task, 1 << st->signal_abort);
		}
		else
		{
//			DB(kprintf("*** subtask was not running!!!\n"));
		}
		Permit();
	}
}



/*********************************************************************
----------------------------------------------------------------------

	ready = SubTaskReady(subtask)

----------------------------------------------------------------------
*********************************************************************/

BOOL SubTaskReady(APTR subtask)
{
	BOOL result = TRUE;

	if (subtask)
	{
		struct subtask *st = (struct subtask *) subtask;
		ULONG mask = 1 << st->signal_ready;
		result = !!(SetSignal(0, 0) & mask);
	}

	return result;
}


/*********************************************************************
----------------------------------------------------------------------

	result = WaitSubTask(subtask)

----------------------------------------------------------------------
*********************************************************************/

LONG WaitSubTask(APTR subtask)
{
	if (subtask)
	{
		struct subtask *st = (struct subtask *) subtask;

		DB(kprintf("§ waiting for subtask %s:%lx\n", st->procname, subtask));

		if (st->running)
		{
			Wait(1 << st->signal_ready);
		}
		else
		{
			DB(kprintf("*** subtask was not running!!!\n"));
		}

		return st->result;
	}

	return NULL;
}


/*********************************************************************
----------------------------------------------------------------------

	CloseSubTask(subtask)

----------------------------------------------------------------------
*********************************************************************/

LONG CloseSubTask(APTR subtask)
{
	if (subtask)
	{
		LONG result;
		struct subtask *st = (struct subtask *) subtask;

		DB(kprintf("§ waiting for subtask %s:%lx to close down\n", st->procname, subtask));

		result = WaitSubTask(subtask);

		SetSignal(0, 1 << st->signal_ready);

		FreeSignal(st->signal_ready);		//!
		free(subtask);

		return result;
	}

	return NULL;
}



/*********************************************************************
----------------------------------------------------------------------

	ObtainData(subtask)

----------------------------------------------------------------------
*********************************************************************/

APTR ObtainData(APTR subtask)
{
	struct subtask *st = (struct subtask *) subtask;

	ObtainSemaphore(&st->datasemaphore);

	return st->data;
}


/*********************************************************************
----------------------------------------------------------------------

	ObtainDataShared(subtask)

----------------------------------------------------------------------
*********************************************************************/

APTR ObtainDataShared(APTR subtask)
{
	struct subtask *st = (struct subtask *) subtask;

	ObtainSemaphoreShared(&st->datasemaphore);

	return st->data;
}


/*********************************************************************
----------------------------------------------------------------------

	ReleaseData(subtask)

----------------------------------------------------------------------
*********************************************************************/

void ReleaseData(APTR subtask)
{
	struct subtask *st = (struct subtask *) subtask;

	ReleaseSemaphore(&st->datasemaphore);
}



/*********************************************************************
----------------------------------------------------------------------

	SignalSubTask(subtask, signals)

----------------------------------------------------------------------
*********************************************************************/

//!!!!!!!!!!

void SignalSubTask(APTR subtask, ULONG signals)
{
	if (subtask)
	{
		struct subtask *st = (struct subtask *) subtask;

		DB(kprintf("§ signalling subtask %s:%lx\n", st->procname, subtask));

		Signal(&st->subproc->pr_Task, signals);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	SignalParentTask(subtask, signals)

----------------------------------------------------------------------
*********************************************************************/

void SignalParentTask(APTR subtask, ULONG signals)
{
	if (subtask)
	{
		struct subtask *st = (struct subtask *) subtask;
		Signal(st->parenttask, signals);
	}
}
