/*
**	MsgQueue.c
**
**	Special MsgPort like queue management
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* DestroyPooled(struct MsgItem *Item):
	 *
	 *	Default destructor for CreateMsgItem().
	 */

STATIC VOID
DestroyPooled(struct MsgItem *Item)
{
	FreeVecPooled(Item);
}

	/* GetMsgItem(struct MsgQueue *Queue):
	 *
	 *	Get the next message from the queue handle,
	 *	similar to GetMsg().
	 */

APTR
GetMsgItem(struct MsgQueue *Queue)
{
		/* Valid handle? */

	if(Queue)
	{
		APTR Item;

			/* Gain access */

		ObtainSemaphore(&Queue->Access);

			/* Any item available? */

		if(!IsListEmpty(&Queue->MsgList))
		{
				/* Remove the first item */

			Remove((struct Node *)(Item = Queue->MsgList.mlh_Head));

				/* Are there any tasks waiting for the queue to become smaller? */

			if(!IsListEmpty(&Queue->WaitList))
			{
				struct SemaphoreRequest *WaitRequest = (struct SemaphoreRequest *)Queue->WaitList.mlh_Head;

					/* Remove one waiting task from the list */

				Remove((struct Node *)WaitRequest);

					/* Wake the task up */

				Signal(WaitRequest->sr_Waiter,SIGF_SINGLE);
			}

				/* One message taken */

			if(Queue->QueueSize)
				Queue->QueueSize--;
		}
		else
			Item = NULL;

			/* Drop the semaphore */

		ReleaseSemaphore(&Queue->Access);

			/* Return the item read */

		return(Item);
	}
	else
		return(NULL);
}

	/* PutMsgItem(struct MsgQueue *Queue,struct MsgItem *Item):
	 *
	 *	Add a message item to a queue handle, similar to
	 *	PutMsg().
	 */

VOID
PutMsgItem(struct MsgQueue *Queue,struct MsgItem *Item)
{
		/* Valid data? */

	if(Queue && Item)
	{
			/* Gain access to the handle */

		ObtainSemaphore(&Queue->Access);

			/* Are we to discard this message? */

		if(Queue->Discard)
		{
			ReleaseSemaphore(&Queue->Access);

			DeleteMsgItem(Item);
		}
		else
		{
				/* Are we to watch for a maximum queue size? */

			if(Queue->MaxSize)
			{
					/* Maximum queue size already reached? */

				if(Queue->MaxSize == Queue->QueueSize)
				{
					struct SemaphoreRequest WaitRequest;

						/* That's me */

					WaitRequest.sr_Waiter = FindTask(NULL);

						/* Add this task to the waiting list */

					AddTail((struct List *)&Queue->WaitList,(struct Node *)&WaitRequest);

						/* Careful, please */

					Forbid();

						/* Drop the semaphore */

					ReleaseSemaphore(&Queue->Access);

						/* Clear the one-shot flag */

					ClrSignal(SIGF_SINGLE);

						/* Wait for the queue to become smaller */

					Wait(SIGF_SINGLE);

						/* Gain access to the handle */

					ObtainSemaphore(&Queue->Access);

						/* Reenable multitasking */

					Permit();
				}

					/* We are going to make the queue longer */

				Queue->QueueSize++;
			}

				/* Add the item to the list */

			AddTail((struct List *)&Queue->MsgList,(struct Node *)Item);

				/* Wake up the owner */

			Signal(Queue->SigTask,Queue->SigMask);

				/* Drop the semaphore */

			ReleaseSemaphore(&Queue->Access);
		}
	}
}

	/* DeleteMsgItem(struct MsgItem *Item):
	 *
	 *	Clean up after a message item.
	 */

VOID
DeleteMsgItem(struct MsgItem *Item)
{
		/* Valid data? */

	if(Item)
	{
			/* Do we have a destructor for this item? */

		if(Item->Destructor)
			(*Item->Destructor)(Item);
	}
}

	/* CreateMsgItem(LONG Size):
	 *
	 *	Allocate a new message item.
	 */

struct MsgItem *
CreateMsgItem(LONG Size)
{
	struct MsgItem	*Item;

		/* Allocate space for the item and add the default destructor */

	if(Item = (struct MsgItem *)AllocVecPooled(Size,MEMF_ANY))
		Item->Destructor = DestroyPooled;

	return(Item);
}

	/* UnlockMsgQueue(struct MsgQueue *Queue):
	 *
	 *	Wake up all tasks waiting for the queue to become
	 *	smaller. Note: this will change the maximum queue
	 *	size.
	 */

VOID
UnlockMsgQueue(struct MsgQueue *Queue)
{
		/* Valid data? */

	if(Queue)
	{
		struct SemaphoreRequest *WaitRequest;

			/* Gain access to the handle */

		ObtainSemaphore(&Queue->Access);

			/* Notify all waiting tasks */

		for(WaitRequest = (struct SemaphoreRequest *)Queue->WaitList.mlh_Head ; WaitRequest->sr_Link.mln_Succ ; WaitRequest = (struct SemaphoreRequest *)WaitRequest->sr_Link.mln_Succ)
			Signal(WaitRequest->sr_Waiter,SIGF_SINGLE);

			/* Reset the maximum queue size */

		Queue->MaxSize = 0;

			/* Drop the semaphore */

		ReleaseSemaphore(&Queue->Access);
	}
}

	/* DeleteMsgQueue(struct MsgQueue *Queue):
	 *
	 *	Free a queue handle.
	 */

VOID
DeleteMsgQueue(struct MsgQueue *Queue)
{
		/* Valid data? */

	if(Queue)
	{
		struct MsgItem *Item,*Next;

			/* Make sure no tasks is waiting in the list */

		UnlockMsgQueue(Queue);

			/* Disable multitasking */

		Forbid();

			/* Clear the queue signal */

		ClrSignal(Queue->SigMask);

			/* Wait until all tasks are finished */

		while(Queue->QueueSize)
			Wait(Queue->SigMask);

			/* Reenable multitasking */

		Permit();

			/* Remove each item from the list */

		for(Item = (struct MsgItem *)Queue->MsgList.mlh_Head ; Next = (struct MsgItem *)Item->MinNode.mln_Succ ; Item = Next)
			DeleteMsgItem(Item);

			/* Free the signal bit if necessary */

		if(Queue->SigBit != -1)
			FreeSignal(Queue->SigBit);

			/* Free the handle */

		FreeVecPooled(Queue);
	}
}

	/* CreateMsgQueue(ULONG SigMask,LONG MaxSize):
	 *
	 *	Allocate a queue handle.
	 */

struct MsgQueue *
CreateMsgQueue(ULONG SigMask,LONG MaxSize)
{
	struct MsgQueue	*Queue;

		/* Allocate the queue handle */

	if(Queue = (struct MsgQueue *)AllocVecPooled(sizeof(struct MsgQueue),MEMF_ANY | MEMF_CLEAR))
	{
			/* Initialize the access semaphore */

		InitSemaphore(&Queue->Access);

			/* Initialize the message item list */

		NewList((struct List *)&Queue->MsgList);

			/* Reset the size data */

		Queue->QueueSize	= 0;
		Queue->MaxSize		= MaxSize;

			/* Initialize the queue wait list */

		NewList((struct List *)&Queue->WaitList);

			/* Store the owner address */

		Queue->SigTask = FindTask(NULL);

			/* Do we have a signal mask ready? */

		if(SigMask)
		{
				/* Use it */

			Queue->SigMask	= SigMask;
			Queue->SigBit	= -1;
		}
		else
		{
				/* Allocate a new signal */

			if((Queue->SigBit = AllocSignal(-1)) == -1)
			{
				FreeVecPooled(Queue);

				return(NULL);
			}
			else
				Queue->SigMask = (1L << Queue->SigBit);
		}
	}

	return(Queue);
}

	/* SetQueueDiscard(struct MsgQueue *Queue,BOOLEAN Mode):
	 *
	 *	Set whether new items should be added to the
	 *	queue or whether they should rather get
	 *	discarded instead.
	 */

VOID
SetQueueDiscard(struct MsgQueue *Queue,BOOL Mode)
{
	if(Queue)
	{
		ObtainSemaphore(&Queue->Access);

		Queue->Discard = Mode;

		ReleaseSemaphore(&Queue->Access);
	}
}
