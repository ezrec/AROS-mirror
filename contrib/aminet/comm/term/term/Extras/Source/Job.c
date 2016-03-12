/*
**	Job.c
**
**	Primitive event job scheduler
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

/****************************************************************************/

	/* NullQueue(JobQueue *Queue):
	 *
	 *	Does nothing; called if waiting on a queue that doesn't have
	 *	any wait mask set. The SIGF_SINGLE bit is added in order
	 *	to trigger JOBTYPE_Once jobs.
	 */

STATIC ULONG
NullQueue(JobQueue *UnusedQueue)
{
	DB(kprintf("%s: %s\n",RexxPortName,__FUNC__));

	return(SIGF_SINGLE);
}

	/* WaitQueue(JobQueue *Queue):
	 *
	 *	Wait for any event to take place.
	 */

STATIC ULONG
WaitQueue(JobQueue *Queue)
{
	DB(kprintf("%s: %s %08lx\n",RexxPortName,__FUNC__,Queue->ReadyMask));

	return(Wait(Queue->ReadyMask));
}

	/* CheckQueue(JobQueue *Queue):
	 *
	 *	Don't wait for any event to take place, just check if any
	 *	signals are waiting. The SIGF_SINGLE bit is added in order
	 *	to trigger JOBTYPE_Once jobs.
	 */

STATIC ULONG
CheckQueue(JobQueue *Queue)
{
	DB(kprintf("%s: %s\n",RexxPortName,__FUNC__));

	return(SIGF_SINGLE | (SetSignal(0,Queue->ReadyMask) & Queue->ReadyMask));
}

/****************************************************************************/

	/* CallJobOnce(JobNode *Job):
	 *
	 *	Calls a job once, then removes it from the queue.
	 */

STATIC BOOL
CallJobOnce(JobNode *Job)
{
	DB(kprintf("%s: %s\n",RexxPortName,__FUNC__));

	(*Job->SubFunction)(Job);

	SuspendJob(Job->HomeQueue,Job);

	return(FALSE);
}

	/* DisposeJobOnce(JobNode *Job):
	 *
	 *	Calls a job once, then removes it from the queue and deletes it.
	 */

STATIC BOOL
DisposeJobOnce(JobNode *Job)
{
	DB(kprintf("%s: %s\n",RexxPortName,__FUNC__));

	(*Job->SubFunction)(Job);

	RemoveJob(Job->HomeQueue,Job);

	DeleteJob(Job);

	return(FALSE);
}

/****************************************************************************/

	/* RebuildReadyMask(JobQueue *Queue):
	 *
	 *	Rebuilds the wait mask for the queue and installs the
	 *	right routine to wait for something to happen.
	 */

STATIC VOID
RebuildReadyMask(JobQueue *Queue)
{
	JobNode *Node;
	ULONG Mask;

		/* Collect the mask bits. */

	for(Node = (JobNode *)Queue->ReadyList.mlh_Head, Mask = 0 ; Node->Node.ln_Succ ; Node = (JobNode *)Node->Node.ln_Succ)
	{
		if(Node->Type == JOBTYPE_Wait)
			Mask |= Node->Mask;
	}

	Queue->ReadyMask = Mask;

		/* If we have JOBTYPE_Once jobs, use the routine
		 * that just checks for the signals.
		 */

	if(Queue->OnceCounter > 0)
		Queue->QueueWait = CheckQueue;
	else
	{
			/* If we don't have any signals to wait for,
			 * use the dummy routine, otherwise wait
			 * for something to happen.
			 */

		if(Mask == 0)
			Queue->QueueWait = NullQueue;
		else
			Queue->QueueWait = WaitQueue;
	}
}

	/* GetNextJob(JobNode *Job):
	 *
	 *	Returns the next job to follow the given one.
	 */

STATIC JobNode *
GetNextJob(JobNode *Job)
{
	if(Job->Node.ln_Succ->ln_Succ)
		return((JobNode *)Job->Node.ln_Succ);
	else
		return(NULL);
}

	/* GetFirstJob(JobQueue *Queue):
	 *
	 *	Returns the first job of the active list.
	 */

STATIC JobNode *
GetFirstJob(JobQueue *Queue)
{
	if(IsListEmpty(&Queue->ReadyList))
		return(NULL);
	else
		return((JobNode *)Queue->ReadyList.mlh_Head);
}

/****************************************************************************/

	/* DeleteJobQueue(JobQueue *Queue):
	 *
	 *	Deletes a job queue, releases the allocated memory.
	 */

VOID
DeleteJobQueue(JobQueue *Queue)
{
	FreeVecPooled(Queue);
}

	/* CreateJobQueue():
	 *
	 *	Creates a job queue and initializes its defaults.
	 */

JobQueue *
CreateJobQueue()
{
	JobQueue *Queue;

	if(Queue = (JobQueue *)AllocVecPooled(sizeof(JobQueue),MEMF_ANY|MEMF_CLEAR))
	{
		NewList((struct List *)&Queue->ReadyList);

		InitSemaphore(&Queue->AccessLock);

		RebuildReadyMask(Queue);
	}

	return(Queue);
}

	/* DeleteJob(JobNode *Node):
	 *
	 *	Deletes a job node, releases the allocated memory.
	 */

VOID
DeleteJob(JobNode *Node)
{
	FreeVecPooled(Node);
}

	/* CreateJob(STRPTR Name,WORD Type,JOBFUNCTION Function,ULONG Mask):
	 *
	 *	Creates a new job node with name, type, invocation function
	 *	and signal mask.
	 */

JobNode *
CreateJob(STRPTR Name,WORD Type,JOBFUNCTION Function,ULONG Mask)
{
	JobNode *Job;
	LONG Extra;

		/* Add extra space if name is given. */

	if(Name)
		Extra = strlen(Name) + 1;
	else
		Extra = 0;

	if(Job = (JobNode *)AllocVecPooled(sizeof(JobNode) + Extra,MEMF_ANY|MEMF_CLEAR))
	{
			/* Fill in the name. */

		if(Name)
			strcpy(Job->Node.ln_Name = (char *)(Job + 1),Name);
		else
			Job->Node.ln_Name = "Untitled";

			/* Remember the type. */

		Job->Type = Type;

			/* JOBTYPE_Once jobs are special. They will invoke the
			 * routine to call and immediately remove the job from
			 * the active list.
			 */

		if(Job->Type != JOBTYPE_Wait)
		{
			Job->SubFunction	= Function;
			Job->Mask			= (ULONG)~0;

			switch(Job->Type)
			{
				case JOBTYPE_Once:

					Job->Function = CallJobOnce;
					break;

				case JOBTYPE_Always:

					Job->Function = Function;
					break;

				case JOBTYPE_Disposable:

					Job->SubFunction = DisposeJobOnce;
					break;
			}
		}
		else
		{
			Job->Function	= Function;
			Job->Mask		= Mask;
		}
	}

	return(Job);
}

	/* RemoveJob(JobQueue *Queue,JobNode *Job):
	 *
	 *	Remove a job from the active list.
	 */

VOID
RemoveJob(JobQueue *Queue,JobNode *Job)
{
	DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

	if(Job->Active)
	{
		ObtainSemaphore(&Queue->AccessLock);

		Remove((struct Node *)Job);

		if(Job->Type != JOBTYPE_Wait)
			Queue->OnceCounter--;

		RebuildReadyMask(Queue);

		if(Job == Queue->NextJob)
			Queue->NextJob = GetNextJob(Job);

		Job->Active = FALSE;

		ReleaseSemaphore(&Queue->AccessLock);
	}
}

	/* AddJob(JobQueue *Queue,JobNode *Job):
	 *
	 *	Add a list to the active job list.
	 */

VOID
AddJob(JobQueue *Queue,JobNode *Job)
{
	ObtainSemaphore(&Queue->AccessLock);

	if(Queue->NextJob == NULL)
		Queue->NextJob = Job;

	AddTail((struct List *)&Queue->ReadyList,(struct Node *)Job);

	Job->Active = TRUE;
	Job->HomeQueue = Queue;

	if(Job->Type != JOBTYPE_Wait)
		Queue->OnceCounter++;

	RebuildReadyMask(Queue);

	ReleaseSemaphore(&Queue->AccessLock);
}

	/* SuspendJob(JobQueue *Queue,JobNode *Job):
	 *
	 *	Suspend an active job.
	 */

VOID
SuspendJob(JobQueue *Queue,JobNode *Job)
{
	DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

	ObtainSemaphore(&Queue->AccessLock);

	if(Job->Active)
		RemoveJob(Queue,Job);

	ReleaseSemaphore(&Queue->AccessLock);
}

	/* ActivateJob(JobQueue *Queue,JobNode *Job):
	 *
	 *	Reactivate a suspended job.
	 */

VOID
ActivateJob(JobQueue *Queue,JobNode *Job)
{
	DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

	ObtainSemaphore(&Queue->AccessLock);

	if(!Job->Active)
		AddJob(Queue,Job);

	ReleaseSemaphore(&Queue->AccessLock);
}

	/* UpdateJobMask(JobQueue *Queue,JobNode *Job,ULONG Mask):
	 *
	 *	Update a job wait mask.
	 */

VOID
UpdateJobMask(JobQueue *Queue,JobNode *Job,ULONG Mask)
{
	DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

	if(Job->Type == JOBTYPE_Wait)
	{
		ObtainSemaphore(&Queue->AccessLock);

		Job->Mask = Mask;

		RebuildReadyMask(Queue);

		ReleaseSemaphore(&Queue->AccessLock);
	}
}

	/* UpdateJob(JobQueue *Queue,JobNode *Job,ULONG Mask):
	 *
	 *	Activate or susped a job based upon the signal mask.
	 *	If the mask is not NULL, the job mask will be updated,
	 *	otherwise it will be suspended.
	 */

VOID
UpdateJob(JobQueue *Queue,JobNode *Job,ULONG Mask)
{
	if(Queue && Job)
	{
		DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

		if(Job->Type == JOBTYPE_Wait)
		{
			if(Mask == 0)
				SuspendJob(Queue,Job);
			else
			{
				ObtainSemaphore(&Queue->AccessLock);

				ActivateJob(Queue,Job);

				UpdateJobMask(Queue,Job,Mask);

				ReleaseSemaphore(&Queue->AccessLock);
			}
		}
	}
}

	/* GetJobMask(JobNode *Job):
	 *
	 *	Get the job wait signal mask, if any.
	 */

ULONG
GetJobMask(JobNode *Job)
{
	ULONG Mask;

	DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

	Mask = 0;

	if(Job)
	{
		if(Job->Active && Job->Type == JOBTYPE_Wait)
			Mask = Job->Mask;
	}

	return(Mask);
}

	/* RunJob(JobNode *Job):
	 *
	 *	Run a single job unless it has been suspended.
	 */

BOOL
RunJob(JobNode *Job)
{
	DB(kprintf("%s: %s %s\n",RexxPortName,__FUNC__,Job->Node.ln_Name));

		/* Check if the job is an active one. */

	if(Job->Active)
		return((*Job->Function)(Job));
	else
		return(FALSE);
}

	/* ProcessJobQueue(JobQueue *Queue):
	 *
	 *	This is where the work takes place.
	 */

VOID
ProcessJobQueue(JobQueue *Queue)
{
	ULONG Signals;
	JobNode *Job;
	BOOL Running;

	DB(kprintf("%s: waiting (mask %08lx)...\n",RexxPortName,Queue->ReadyMask));

		/* Wait for something to happen. */

	Signals = (*Queue->QueueWait)(Queue);

	DB(kprintf("%s: got %08lx\n",RexxPortName,Signals));

		/* Prepare to walk down the list. */

	ObtainSemaphore(&Queue->AccessLock);

	while(TRUE)
	{
			/* Nothing has happened, yet. */

		Running = FALSE;

			/* Walk down the list... */

		while(Job = Queue->NextJob)
		{
			Queue->NextJob = GetNextJob(Job);

			DB(kprintf("%s: checking |%s| mask %08lx & job = %08lx\n",RexxPortName,Job->Node.ln_Name,Job->Mask,Signals & Job->Mask));

				/* Work to be done by this job? */

			if(Signals & Job->Mask)
			{
					/* Drop the access lock. */

				ReleaseSemaphore(&Queue->AccessLock);

					/* Do the job. */

				DB(kprintf("%s: do job |%s|\n",RexxPortName,Job->Node.ln_Name));
				Running |= (*Job->Function)(Job);

					/* And pick up the lock again. */

				ObtainSemaphore(&Queue->AccessLock);
			}
		}

			/* Stop if the list is empty now, otherwise
			 * restart at the top of the list.
			 */

		if(!(Queue->NextJob = GetFirstJob(Queue)))
			break;

			/* If there isn't more work to be done, leave the loop. */

		if(!Running)
			break;
	}

		/* Drop the access lock. */

	ReleaseSemaphore(&Queue->AccessLock);
}
