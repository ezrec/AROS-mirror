/*********************************************************************
----------------------------------------------------------------------

	MysticView
	Timing

	submits regular trigger signals

----------------------------------------------------------------------
*********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "Mystic_Global.h"
#include "Mystic_Subtask.h"
#include "Mystic_Timing.h"
#include "Mystic_Debug.h"


/*********************************************************************
----------------------------------------------------------------------

	Refresh-Trigger Subtask

----------------------------------------------------------------------
*********************************************************************/

LONG SAVEDS triggerfunc(struct subtask *subtask, BYTE abortsignal)
{
	struct trigger *trigger;
	BOOL abort;

	trigger = ObtainData(subtask);

	do
	{
		ObtainSemaphore(&trigger->semaphore);
		if (trigger->tickcount > 0)
		{
			trigger->tickcount -= 3;
			if (trigger->tickcount <= 0)
			{
				Signal(trigger->parenttask, 1 << trigger->triggersignal);
			}
		}
		ReleaseSemaphore(&trigger->semaphore);

		Delay(3);

		abort = !!(SetSignal(0, 0) & (1 << abortsignal));

	} while (!abort);


	DB(kprintf("trigger task concluded.\n"));

	ReleaseData(subtask);

	return 0;
}


/*********************************************************************
----------------------------------------------------------------------

	DeleteTrigger(trigger)

	free a trigger

----------------------------------------------------------------------
*********************************************************************/

void DeleteTrigger(struct trigger *trigger)
{
	if (trigger)
	{
		FreeSignal(trigger->triggersignal);
		Free(trigger);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	finished = CloseTrigger(trigger)

	abort and free a trigger.

----------------------------------------------------------------------
*********************************************************************/

void CloseTrigger(struct trigger *trigger)
{
	if (trigger)
	{
		AbortSubTask(trigger->subtask);
		CloseSubTask(trigger->subtask);
		DeleteTrigger(trigger);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	trigger = CreateTrigger()

----------------------------------------------------------------------
*********************************************************************/

struct trigger *CreateTrigger(void)
{
	struct trigger *trigger;
	BOOL success = FALSE;

	if ((trigger = Malloc(sizeof(struct trigger))))
	{
		memset(trigger, 0, sizeof(struct trigger));
		trigger->tickcount = 0;
		trigger->parenttask = FindTask(NULL);
		InitSemaphore(&trigger->semaphore);
		if ((trigger->triggersignal = AllocSignal(-1)) >= 0)
		{
			if ((trigger->subtask = SubTask(triggerfunc, trigger, TRIGGERSTACK, MAXPRIORITY, "MysticView TriggerTask (%x)", NULL, FALSE)))
			{
				trigger->signalmask = 1 << SubTaskReadySignal(trigger->subtask);
				success = TRUE;
			}
		}
	}

	if (!success)
	{
		DeleteTrigger(trigger);
		trigger = NULL;
	}

	return trigger;
}


/*********************************************************************
----------------------------------------------------------------------

	triggersignal = SetTrigger(trigger, ticks)

	start trigger with given number of ticks

----------------------------------------------------------------------
*********************************************************************/

ULONG SetTrigger(struct trigger *trigger, ULONG ticks)
{
	ULONG r = 0;
	if (trigger /*&& ticks*/)
	{
		ObtainSemaphore(&trigger->semaphore);
		trigger->tickcount = ticks;
		r = (ULONG) (1 << (ULONG) trigger->triggersignal);
		ReleaseSemaphore(&trigger->semaphore);
	}
	return r;
}
