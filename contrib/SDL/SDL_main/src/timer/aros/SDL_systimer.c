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

#ifdef SDL_TIMER_AROS

#define __TIMER_NOLIBBASE__          

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <exec/types.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <devices/timer.h>
#include <proto/timer.h>
#include "../../main/aros/mydebug.h"

#include "SDL_thread.h"
#include "SDL_timer.h"
#include "../SDL_timer_c.h"

extern struct DosLibrary *DOSBase;
extern struct ExecBase *SysBase;

/* The first ticks value of the application */

struct timerequest	*TimerIO = NULL;
static clock_t start;
static struct Device 		*TimerBase = NULL;
static struct timeval 		basetime;
static struct MsgPort		*TimerMP = NULL;
static struct Task			*OwnerTask;

static int fallback = 0;

static void close_timer(void)
{
    D(bug("freeing timer resources..."));
    if(TimerIO) {
        CloseDevice((struct IORequest *)TimerIO);
        DeleteIORequest((struct IORequest *)TimerIO);
        TimerIO = NULL;
    }
    if(TimerMP) {
        DeleteMsgPort(TimerMP);
        TimerMP = NULL;
    }
    D(bug("OK\n"));
}

static BOOL open_timer(void)
{
  if (TimerBase || fallback) {
     return FALSE;
  }

  if (!(TimerMP = CreateMsgPort())) {
 	 start=clock();
     fallback = 1;
     return FALSE;
  }

  if (!(TimerIO = (struct timerequest *)
              CreateIORequest(TimerMP, sizeof(struct timerequest)))) {
     DeleteMsgPort(TimerMP);
	 start=clock();
     fallback = 1;
     return FALSE;
  }
  // Under AROS we use UNIT_MICROHZ to be more accurate for short delays
  // Delays under SDL are mainly used to synchronise frame updates
  // and therefore are usually less than 1/50 or 1/60 seconds
  if (OpenDevice("timer.device", UNIT_MICROHZ, &TimerIO->tr_node, 0)) {
	  DeleteMsgPort(TimerMP);
      DeleteIORequest((struct IORequest *)TimerIO);
 	  start=clock();
      fallback = 1;
      return FALSE;
  }
  
  OwnerTask = TimerMP->mp_SigTask;
  TimerBase = TimerIO->tr_node.io_Device;
  D(bug("Timer resource allocated.\n"));

  return TRUE;
}

void SDL_StartTicks(void)
{

  if (open_timer()) {
    GetSysTime(&basetime);
    D(bug("Basetime: %lusecs %lumicro\n", basetime.tv_secs, basetime.tv_micro));

#ifndef SHARED_LIB
    atexit(close_timer);
#endif
  }
}

Uint32 SDL_GetTicks (void)
{

    open_timer(); // TimerBase must be set for GetSysTime()
    if (fallback) {
        clock_t ticks;
        
        ticks=clock()-start;

#if CLOCKS_PER_SEC == 1000
        return(ticks);
#else
        return ticks*(1000/CLOCKS_PER_SEC);
#endif  
    }
    else {
        struct timeval tv;
        Uint32 tics;
        
        GetSysTime(&tv);

        if(basetime.tv_micro > tv.tv_micro) {           
           tv.tv_secs --;
          
           tv.tv_micro += 1000000;
        }
        tics = ((tv.tv_secs - basetime.tv_secs) * 1000) + 
            ((tv.tv_micro - basetime.tv_micro)/1000);

        return tics;
    }
}

void SDL_Delay (Uint32 ms)
{
/*
We should try to be more clever under AROS to avoid any busy looping
Busy looping causes the system to slow down, while an SDL application
is waiting to sync screen update, those delays are usually less than 
1/50 seconds, i.e. 20ms
*/
	if (OwnerTask == FindTask(NULL)) //SysBase->ThisTask)
	{
		TimerIO->tr_node.io_Command	= TR_ADDREQUEST;
		TimerIO->tr_time.tv_secs	= ms / 1000;
		TimerIO->tr_time.tv_micro	= (ms % 1000) * 1000;

		DoIO((struct IORequest *) TimerIO);
	}
	else
	{
		struct MsgPort port;

		if ((BYTE) (port.mp_SigBit = AllocSignal(-1)) != -1)
		{
			struct timerequest ioreq;

			port.mp_Node.ln_Type = NT_MSGPORT;
			port.mp_Flags        = PA_SIGNAL;
			port.mp_SigTask      = FindTask(NULL);
			NEWLIST(&port.mp_MsgList);

			ioreq.tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
			ioreq.tr_node.io_Message.mn_ReplyPort    = &port;
			ioreq.tr_node.io_Message.mn_Length       = sizeof(struct timerequest);
			ioreq.tr_node.io_Device                  = TimerIO->tr_node.io_Device;
			ioreq.tr_node.io_Unit                    = TimerIO->tr_node.io_Unit;
			ioreq.tr_node.io_Command                 = TR_ADDREQUEST;
			ioreq.tr_time.tv_secs                    = ms / 1000;
			ioreq.tr_time.tv_micro                   = (ms % 1000) * 1000;

			DoIO((struct IORequest *) &ioreq);

			FreeSignal(port.mp_SigBit);
		}
	}
}


/* Data to handle a single periodic alarm */
static int timer_alive = 0;
static SDL_Thread *timer_thread = NULL;

static int RunTimer(void *unused)
{
	D(bug("SYSTimer: Entering RunTimer loop..."));

	while ( timer_alive ) {
		if ( SDL_timer_running ) {
			SDL_ThreadedTimerCheck();
		}
		SDL_Delay(1); // Might use new SDL_Delay implementation but has to be thread safe before!
	}
	D(bug("SYSTimer: EXITING RunTimer loop..."));
	return(0);
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit(void)
{
	D(bug("Creating thread for the timer (NOITIMER)...\n"));

	timer_alive = 1;
	timer_thread = SDL_CreateThread(RunTimer, NULL);
	if ( timer_thread == NULL )
	{
		D(bug("Creazione del thread fallita...\n"));

		return(-1);
	}
	return(SDL_SetTimerThreaded(1));
}

void SDL_SYS_TimerQuit(void)
{
	timer_alive = 0;
	if ( timer_thread ) {
		SDL_WaitThread(timer_thread, NULL);
		timer_thread = NULL;
	}
}

int SDL_SYS_StartTimer(void)
{
	SDL_SetError("Internal logic error: AmigaOS uses threaded timer");
	return(-1);
}

void SDL_SYS_StopTimer(void)
{
	return;
}

#endif /* SDL_TIMER_AROS */
