/*********************************************************************
----------------------------------------------------------------------

	MysticView
	definitions for timing functions
	
----------------------------------------------------------------------
*********************************************************************/


#ifndef MYSTIC_TIMING_H
#define	MYSTIC_TIMING_H 1

#include	<exec/semaphores.h>

struct trigger
{
	struct SignalSemaphore semaphore;
	APTR subtask;
	ULONG signalmask;
	BYTE triggersignal;
	struct Task *parenttask;
	LONG tickcount;
};

extern struct trigger *CreateTrigger(void);
extern void CloseTrigger(struct trigger *trigger);
extern ULONG SetTrigger(struct trigger *trigger, ULONG ticks);


#endif
