#ifndef MYSTIC_SUBTASK_H
#define MYSTIC_SUBTASK_H

#include <exec/types.h>

#define	SUBTASK_STACKSIZE	10000

struct subtask
{
	struct Message msg;

	ULONG ID;					/* identifier with magic number */

	WORD prio;

	struct Task *parenttask;	/* Parent Task that launched subtask */
	struct Process *subproc;	/* the subtask */

	BYTE signal_abort;			/* signals subtask to abort */
	BYTE signal_go;				/* signals parent task when started */
	BYTE signal_ready;			/* signals parent task when ready */
	UBYTE pad;

	LONG (* function) (struct subtask *, BYTE);				/* sub task function */
	LONG result;								/* result from function */

	struct SignalSemaphore datasemaphore;		/* shared data protection */
	APTR data;									/* any kind of data */

	BOOL running;

	BOOL (* initfunction) (struct subtask *);				/* init function */

	BOOL selfdestruct;			/* subtask closes down automatically */

	char procname[100];
};

void AbortSubTask(APTR subtask);
LONG WaitSubTask(APTR subtask);
LONG CloseSubTask(APTR subtask);
APTR ObtainData(APTR subtask);
APTR ObtainDataShared(APTR subtask);
void ReleaseData(APTR subtask);
BYTE SubTaskReadySignal(APTR subtask);
APTR SubTask(LONG (* function)(struct subtask *, BYTE), APTR data, ULONG stacksize, WORD prio, char* name, BOOL (* initfunction)(struct subtask *), BOOL autodestruct);
BOOL SubTaskReady(APTR subtask);
void SignalSubTask(APTR subtask, ULONG signals);
void SignalParentTask(APTR subtask, ULONG signals);

#endif
