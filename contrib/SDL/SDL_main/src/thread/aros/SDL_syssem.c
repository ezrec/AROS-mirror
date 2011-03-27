/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002  Sam Lantinga
    Copyright (C) 2004  Harry Sintonen

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org

    Harry Sintonen
    sintonen@iki.fi

		20040824	Non-polling SDL_SemWaitTimeout
				Optimized

		20040822	Reimplementation. Improvements:
				- initial_value != 1 works properly
				- wait can be aborted via CTRL-C
				- doesn't nest per task

		20040513	SDL_SemTryWait() function return value fixed

		20040510	ObtainSemaphore inserted in SDL_SemWaitTimeout

		20040504	Debug lines added
				WarpOS code removed
				
	Yannick Erb
	yannick.erb@free.fr
	
		20110326	Adapted for AROS

*/

#include <stdlib.h> /* for malloc/free */

#include "SDL_error.h"
#include "SDL_thread.h"
#include "SDL_systhread_c.h"
#include "../../timer/SDL_systimer.h"

#include <exec/semaphores.h>
#include <devices/timer.h>
#include <dos/dos.h>

#include "../../main/aros/mydebug.h"

extern struct timerequest	*TimerIO;

struct waitnode
{
	struct MinNode node;
	struct Task   *task;
	ULONG          sigmask;
};

struct SDL_semaphore
{
	struct SignalSemaphore sem;
	struct MinList         waitlist;
	int                    sem_value;
};

struct mywaitdata
{
	struct MsgPort      port;
	struct timerequest  timereq;
	ULONG               extramask;
	BOOL                pending;
};

#define FALLBACKSIGNAL SIGBREAKB_CTRL_E

static
void mywaitdone(struct mywaitdata *data)
{
	if (data->pending)
	{
		data->pending = FALSE;
		AbortIO((struct IORequest *) &data->timereq);
		WaitIO((struct IORequest *) &data->timereq);
	}

	if ((BYTE) data->port.mp_SigBit != -1)
	{
		FreeSignal(data->port.mp_SigBit);
		data->port.mp_SigBit = (UBYTE) -1;
	}
}

static
int mywaitinit(struct mywaitdata *data, Uint32 timeout)
{
	data->extramask = 0;
	data->pending   = FALSE;

	if ((BYTE) (data->port.mp_SigBit = AllocSignal(-1)) != -1)
	{
		struct timerequest *req = TimerIO;

		data->port.mp_Node.ln_Type = NT_MSGPORT;
		data->port.mp_Flags        = PA_SIGNAL;
		data->port.mp_SigTask      = FindTask(NULL);
		NEWLIST(&data->port.mp_MsgList);

		data->timereq.tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
		data->timereq.tr_node.io_Message.mn_ReplyPort    = &data->port;
		data->timereq.tr_node.io_Message.mn_Length       = sizeof(data->timereq);
		data->timereq.tr_node.io_Device                  = req->tr_node.io_Device;
		data->timereq.tr_node.io_Unit                    = req->tr_node.io_Unit;

		return 0;
	}

	mywaitdone(data);

	return -1;
}

static
int mywait(struct mywaitdata *data, Uint32 timeout)
{
	ULONG wsig, sigs;

	wsig = 1 << data->timereq.tr_node.io_Message.mn_ReplyPort->mp_SigBit;

	if (!data->pending)
	{
		data->pending = TRUE;
		data->timereq.tr_node.io_Command = TR_ADDREQUEST;
		data->timereq.tr_time.tv_secs  = timeout / 1000;
		data->timereq.tr_time.tv_micro = (timeout % 1000) * 1000;
		SendIO((struct IORequest *) &data->timereq);
	}

	sigs = Wait(wsig | data->extramask | SIGBREAKF_CTRL_C);

	if (sigs & wsig)
	{
		data->pending = FALSE;
		WaitIO((struct IORequest *) &data->timereq);
	}
	else
	{
		if (data->pending && data->extramask == 0)
		{
			data->pending = FALSE;
			AbortIO((struct IORequest *) &data->timereq);
			WaitIO((struct IORequest *) &data->timereq);
		}
	}

	data->extramask &= sigs;

	if (sigs & SIGBREAKF_CTRL_C)
	{
		return -1;
	}

	return 0;
}

/* Create a semaphore, initialized with value */
SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{
	SDL_sem *sem;

	D(bug("[SDL] SDL_CreateSemaphore(%ld)\n", initial_value));

	sem = malloc(sizeof(*sem));
	if (sem)
	{
		memset(&sem->sem, 0, sizeof(sem->sem));
		InitSemaphore(&sem->sem);

		NEWLIST(&sem->waitlist);

		sem->sem_value = initial_value;
	}
	else
	{
		SDL_OutOfMemory();
	}

	return sem;
}

void SDL_DestroySemaphore(SDL_sem *sem)
{
	D(bug("[SDL] SDL_DestroySemaphore()\n"));
	D(bug("Destroying semaphore %lx...\n",sem));

	if (sem)
	{
		#if 1

		struct mywaitdata data;

		if (mywaitinit(&data, 10) == 0)
		{
			ObtainSemaphore(&sem->sem);

			sem->sem_value = -1;

			while (!IsListEmpty((struct List *) &sem->waitlist))
			{
				struct waitnode *wn;
				int res;

				D(bug("[SDL] bug, semaphore %lx busy!\n", sem));

				for (wn = (struct waitnode *) sem->waitlist.mlh_Head;
				     wn->node.mln_Succ;
				     wn = (struct waitnode *) wn->node.mln_Succ)
				{
					Signal(wn->task, wn->sigmask);
				}

				ReleaseSemaphore(&sem->sem);

				res = mywait(&data, 10);

				ObtainSemaphore(&sem->sem);

				if (res < 0)
				{
					break;
				}
			}

			ReleaseSemaphore(&sem->sem);
		}

		mywaitdone(&data);

		#endif

		free(sem);
	}
}

int SDL_SemTryWait(SDL_sem *sem)
{
	int retval;

	D(bug("[SDL] SDL_SemTryWait()\n"));

	if (!sem)
	{
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	ObtainSemaphore(&sem->sem);

	if (sem->sem_value > 0)
	{
		--sem->sem_value;
		retval = 0;
	}
	else
	{
		retval = SDL_MUTEX_TIMEDOUT;
	}

	ReleaseSemaphore(&sem->sem);

	return retval;
}

int SDL_SemWait(SDL_sem *sem)
{
	int retval;
	struct waitnode wn;
	LONG signal;

	D(bug("[SDL] SDL_SemWait(0x%08.8lx) from thread 0x%08.8lx\n", (IPTR)sem, (IPTR)FindTask(NULL)));

	if (!sem)
	{
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	retval = 0;
	signal = -1;

	ObtainSemaphore(&sem->sem);

	D(bug("[SDL] SDL_SemWait(): from thread 0x%08.8lx initial sem_value: %ld\n", (IPTR)FindTask(NULL), sem->sem_value));

	while (sem->sem_value <= 0)
	{
		ULONG sigmask;

		if (signal == -1)
		{
			wn.task = FindTask(NULL);
			signal = AllocSignal(-1);
			if (signal == -1)
			{
				signal = FALLBACKSIGNAL;
				SetSignal(1 << FALLBACKSIGNAL, 0);
			}
			wn.sigmask = 1 << signal;

			ADDTAIL(&sem->waitlist, &wn);
		}

		D(bug("[SDL] SDL_SemWait(): waiting for semaphore... \n"));

		ReleaseSemaphore(&sem->sem);

		sigmask = Wait(wn.sigmask | SIGBREAKF_CTRL_C);

		ObtainSemaphore(&sem->sem);

		if (sigmask & SIGBREAKF_CTRL_C)
		{
			D(bug("[SDL] SDL_SemWait(): was aborted...\n"));
			retval = -1;
			break;
		}
	}

	if (signal != -1)
	{
		REMOVE(&wn);

		if (signal != FALLBACKSIGNAL)
		{
			FreeSignal(signal);
		}
	}

	if (retval == 0)
	{
		--sem->sem_value;

		D(bug("[SDL] SDL_SemWait(): final sem_value: %ld\n", sem->sem_value));
	}

	ReleaseSemaphore(&sem->sem);
	D(bug("done...\n"));

	return retval;
}

int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout)
{
	struct mywaitdata data;
	int retval;

	D(bug("[SDL] SDL_SemWaitTimeout()\n"));

	if (!sem)
	{
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	/* Try the easy cases first */
	if (timeout == 0)
	{
		return SDL_SemTryWait(sem);
	}
	if (timeout == SDL_MUTEX_MAXWAIT)
	{
		return SDL_SemWait(sem);
	}

	#if 1

	if (mywaitinit(&data, timeout) == 0)
	{
		struct waitnode wn;
		LONG signal;

		retval = 0;
		signal = -1;

		ObtainSemaphore(&sem->sem);

		while (sem->sem_value <= 0)
		{
			if (signal == -1)
			{
				wn.task = FindTask(NULL);
				signal = AllocSignal(-1);
				if (signal == -1)
				{
					signal = FALLBACKSIGNAL;
					SetSignal(1 << FALLBACKSIGNAL, 0);
				}
				wn.sigmask = 1 << signal;

				ADDTAIL(&sem->waitlist, &wn);
			}

			ReleaseSemaphore(&sem->sem);

			data.extramask = wn.sigmask;
			retval = mywait(&data, timeout);

			ObtainSemaphore(&sem->sem);

			/* CTRL-C? */
			if (retval < 0)
			{
				break;
			}

			/* Timed out? (== no 'semaphore released'-signal) */
			if (data.extramask == 0)
			{
				retval = SDL_MUTEX_TIMEDOUT;

				break;
			}

			/*
			 * Not timed out, someone released the semaphore.
			 * See if we can get it. If we can't, continue waiting.
			 */
		}

		if (signal != -1)
		{
			REMOVE(&wn);

			if (signal != FALLBACKSIGNAL)
			{
				FreeSignal(signal);
			}
		}

		/* If can obtain (no timeout, no break), obtain it */
		if (retval == 0)
		{
			--sem->sem_value;

			D(bug("[SDL] SDL_SemWaitTimeout(): final sem_value: %ld\n", sem->sem_value));
		}

		ReleaseSemaphore(&sem->sem);
	}
	else retval = -1;

	mywaitdone(&data);

	#else

	/* Ack!  We have to busy wait... */
	timeout += SDL_GetTicks();

	if (mywaitinit(&data, 1) == 0)
	{
		do
		{
			retval = SDL_SemTryWait(sem);
			if (retval == 0)
			{
				break;
			}

			if (mywait(&data, 1) < 0)
			{
				retval = -1;
				break;
			}

		} while (SDL_GetTicks() < timeout);
	}
	else retval = -1;

	mywaitdone(&data);

	#endif

	return retval;
}

Uint32 SDL_SemValue(SDL_sem *sem)
{
	int retval;

	D(bug("[SDL] SDL_SemValue()\n"));

	if (!sem)
	{
		SDL_SetError("Passed a NULL semaphore");
		return 0;
	}

	ObtainSemaphoreShared(&sem->sem);

	retval = sem->sem_value;

	ReleaseSemaphore(&sem->sem);

	if (retval < 0)
	{
		retval = 0;
	}

	return (Uint32) retval;
}

int SDL_SemPost(SDL_sem *sem)
{
	struct waitnode *wn;

	if (!sem)
	{
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	ObtainSemaphore(&sem->sem);

	D(bug("[SDL] SDL_SemPost(0x%08.8lx): initial sem_value: %ld\n", (IPTR)sem, sem->sem_value));

	#if 1

	/* Wake whatever task happens to be first in the waitlist */
	wn = (struct waitnode *) sem->waitlist.mlh_Head;
	if (wn->node.mln_Succ)
	{
		Signal(wn->task, wn->sigmask);
	}

	#else

	for (wn = (struct waitnode *) sem->waitlist.mlh_Head;
	     wn->node.mln_Succ;
	     wn = (struct waitnode *) wn->node.mln_Succ)
	{
		Signal(wn->task, wn->sigmask);
	}

	#endif

	++sem->sem_value;

	D(bug("[SDL] SDL_SemPost(): final sem_value: %ld\n", sem->sem_value));

	ReleaseSemaphore(&sem->sem);

	return 0;
}

