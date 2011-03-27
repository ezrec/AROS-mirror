/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* BeOS thread management routines for SDL */

#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "../SDL_thread_c.h"
#include "../SDL_systhread.h"
#include "../../main/aros/mydebug.h"

typedef struct {
	int (*func)(void *);
	void *data;
	SDL_Thread *info;
	struct Task *wait;
} thread_args;

Uint32 RunThread(char *args)
{
	thread_args *data=(thread_args *)atol(args);

	struct Task *Father;

	D(bug("Received data: %lx\n",data));
	Father=data->wait;

	SDL_RunThread(data);

	Signal(Father,SIGBREAKF_CTRL_F);
	D(bug("Thread with data %lx ended\n",data));
	return(0);
}


int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	/* Create the thread and go! */
	char buffer[20];

	D(bug("Sending %lx to the new thread...\n",args));

	if(args)
		SDL_snprintf(buffer, SDL_arraysize(buffer),"%ld",args);

	thread->handle=(struct Task *)CreateNewProcTags(NP_Output,Output(),
					NP_Name,(ULONG)"SDL subtask",
					NP_CloseOutput, FALSE,
					NP_StackSize, 98304,
					NP_Entry,(ULONG)RunThread,
					args ? NP_Arguments : TAG_IGNORE,(ULONG)buffer,
					TAG_DONE);

	if(!thread->handle)
	{
		SDL_SetError("Not enough resources to create thread");
		return(-1);
	}

	return(0);
}

void SDL_SYS_SetupThread(void)
{
}

Uint32 SDL_ThreadID(void)
{
	return((Uint32)FindTask(NULL));
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	SetSignal(0L,SIGBREAKF_CTRL_F|SIGBREAKF_CTRL_C);
	Wait(SIGBREAKF_CTRL_F|SIGBREAKF_CTRL_C);
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{
	Signal((struct Task *)thread->handle,SIGBREAKF_CTRL_C);
}
