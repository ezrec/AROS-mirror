/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002  Sam Lantinga

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
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <exec/types.h>
#ifdef __SASC
#include <proto/dos.h>
#include <clib/graphics_protos.h>
#include <pragmas/graphics.h>
#include <clib/exec_protos.h>
#include <pragmas/exec.h>
#elif defined(WARPOS) || defined(AROS)
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#else
#include <inline/dos.h>
#include <inline/exec.h>
#include <inline/graphics.h>
#endif
#include "mydebug.h"

extern struct DosLibrary *DOSBase;
extern struct ExecBase *SysBase;
static struct GfxBase *GfxBase;

#include "SDL_error.h"
#include "SDL_timer.h"
#include "SDL_timer_c.h"

#if defined(DISABLE_THREADS) || defined(FORK_HACK)
#define USE_ITIMER
#endif

/* The first ticks value of the application */

#if !defined(__PPC__) || defined(MORPHOS)
static clock_t start;

#define USE_SYSTIME

#ifdef USE_SYSTIME
#include <devices/timer.h>
#include <proto/timer.h>

static struct timerequest *TimerIO = NULL;
static struct Device *TimerBase = NULL;
static struct timeval basetime;
static struct MsgPort     *TimerMP = NULL;

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
#endif

void SDL_StartTicks(void)
{
#ifndef USE_SYSTIME
	/* Set first ticks value */
	start=clock();
#else
  struct MsgPort     *TimerMP;

  if (TimerBase || fallback) return;
  
  if (!(TimerMP = CreateMsgPort())) {
 	 start=clock();
     fallback = 1;
     return;
  }

  if (!(TimerIO = (struct timerequest *)
              CreateIORequest(TimerMP, sizeof(struct timerequest)))) {
     DeleteMsgPort(TimerMP);
	 start=clock();
     fallback = 1;
     return;
  }

  if (OpenDevice("timer.device", UNIT_VBLANK, &TimerIO->tr_node, 0)) {
      DeleteMsgPort(TimerMP);
      DeleteIORequest((struct IORequest *)TimerIO);
 	  start=clock();
      fallback = 1;
      return;
  }

  TimerBase = TimerIO->tr_node.io_Device;
  D(bug("Timer resource allocated.\n"));

  GetSysTime(&basetime);
  D(bug("Basetime: %lusecs %lumicro\n", basetime.tv_secs, basetime.tv_micro));

#ifndef SHARED_LIB
  atexit(close_timer);
#endif
  
#endif
}

Uint32 SDL_GetTicks (void)
{
#ifdef USE_SYSTIME
    if (fallback) {
#endif
        clock_t ticks;
        
        ticks=clock()-start;

#if CLOCKS_PER_SEC == 1000
        return(ticks);
#else
        return ticks*(1000/CLOCKS_PER_SEC);
#endif  

#ifdef USE_SYSTIME
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
#endif
}

void SDL_Delay (Uint32 ms)
{
// Do a busy wait if time is less than 50ms

	if(ms<50)
	{
#ifndef USE_SYSTIME
		clock_t to_wait=clock();
// most archs have clocks per sec == 1000
#if CLOCKS_PER_SEC != 1000
		ms*=(CLOCKS_PER_SEC/1000);
#endif
		to_wait+=ms;

		while(clock()<to_wait);
#else
        Uint32 to_wait = SDL_GetTicks() + ms;

        while(SDL_GetTicks() < to_wait);
#endif
	}
	else
	{
		Delay(ms/20);
	}
}

#elif defined(WARPOS)
/* Use the powerpc.library function GetSysTimePPC to get the time */
static ULONG start;

void SDL_StartTicks(void)
{
    /* Set first ticks value */
    struct timeval tval;
    GetSysTimePPC(&tval);
    start = (tval.tv_secs*1000 + tval.tv_micro/1000);
}

Uint32 SDL_GetTicks (void)
{
    ULONG ticks;
    struct timeval tval;
    GetSysTimePPC(&tval);

    ticks = (tval.tv_secs*1000 + tval.tv_micro/1000);

    return (ticks-start); /* return time in ms */
}

void SDL_Delay (Uint32 ms)
{
// Do a busy wait if time is less than 50ms

    if(ms<50)
    {
        ULONG to_wait;
        ULONG t;
        struct timeval tval;
        GetSysTimePPC(&tval);
        t = (tval.tv_secs*1000000 + tval.tv_micro);
        to_wait = t + (ms*1000);

        do {
            GetSysTimePPC(&tval);
            t = (tval.tv_secs*1000000 + tval.tv_micro);
        }
        while(t < to_wait);
    }
    else
    {
        Delay(ms/20);
    }
}


#else

ULONG MY_CLOCKS_PER_SEC;

void PPC_TimerInit(void);
APTR MyTimer;

ULONG start[2];

void SDL_StartTicks(void)
{
	/* Set first ticks value */
	if(!MyTimer)
		PPC_TimerInit();

	PPCGetTimerObject(MyTimer,PPCTIMERTAG_CURRENTTICKS,start);
	start[1]>>=10;
	start[1]|=((result[0]&0x3ff)<<22);
	start[0]>>=10;
}

Uint32 SDL_GetTicks (void)
{
	ULONG result[2];
	PPCGetTimerObject(MyTimer,PPCTIMERTAG_CURRENTTICKS,result);

//	PPCAsr64p(result,10);
// Non va, la emulo:

	result[1]>>=10;
	result[1]|=((result[0]&0x3ff)<<22);

// Non mi interessa piu' result[0]

	return result[1]*1000/MY_CLOCKS_PER_SEC;
}

void SDL_Delay (Uint32 ms)
{
// Do a busy wait if time is less than 50ms

	if(ms<50)
	{
		ULONG to_wait[2],actual[2];
		PPCGetTimerObject(MyTimer,PPCTIMERTAG_CURRENTTICKS,result);
		actual[1]=0;
		to_wait[1]+=ms*1000/MY_CLOCKS_PER_SEC;

		while(actual[1]<to_wait[1])
		{
			PPCGetTimerObject(MyTimer,PPCTIMERTAG_CURRENTTICKS,actual);
		}
	}
	else
	{
		Delay(ms/50);
	}
}

void PPC_TimerInit(void)
{
	struct TagItem tags[]=
		{
			PPCTIMERTAG_CPU,TRUE,
			TAG_DONE,0
		};


	if(MyTimer=PPCCreateTimerObject(tags))
	{
		ULONG result[2];

		PPCGetTimerObject(MyTimer,PPCTIMERTAG_TICKSPERSEC,result);
		D(bug("Timer inizializzato, TPS: %lu - %lu\n",result[0],result[1]));
//		PPCAsr64p(result,10);
		result[1]>>=10;
		result[1]|=((result[0]&0x3ff)<<22);
		result[0]>>=10;

		D(bug("Shiftato TPS: %lu - %lu\n",result[0],result[1]));
		MY_CLOCKS_PER_SEC=result[1];

		PPCGetTimerObject(MyTimer,PPCTIMERTAG_CURRENTTICKS,result);

		D(bug("Current ticks: %lu - %lu\n",result[0],result[1]));
		result[1]>>=10;
		result[1]|=((result[0]&0x3ff)<<22);
		result[0]>>=10;
//		PPCAsr64p(result,10);
		D(bug("Shiftato: %lu - %lu\n",result[0],result[1]));
	}
	else
	{
		D(bug("Errore nell'inizializzazione del timer!\n"));
	}
}

#endif

#include "SDL_thread.h"

/* Data to handle a single periodic alarm */
static int timer_alive = 0;
static SDL_Thread *timer_thread = NULL;

static int RunTimer(void *unused)
{
	D(bug("SYSTimer: Entering RunTimer loop..."));

	if(GfxBase==NULL)
		GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37);

	while ( timer_alive ) {
		if ( SDL_timer_running ) {
			SDL_ThreadedTimerCheck();
		}
		if(GfxBase)
			WaitTOF();  // Check the timer every fifth of seconds. Was SDL_Delay(1)->BusyWait!
		else
			Delay(1);
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
