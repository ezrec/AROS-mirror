/* Copyright 2000 Kjetil S. Matheussen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. */




/**************** OVERVIEW ***************************************

   Handles timing from the AmigaOS. Uses realtime.library for this
   purpose, which seems to work very fine. Unfortunately, the
   documentation for realtime.library is not the best, so it might
   be better ways to do things. But I guess its good enough.
   And yes, realtime.library uses CIA for timing (I think..).

*****************************************************************/


// #include <dos.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <clib/alib_protos.h>
#include <proto/realtime.h>
#include <libraries/realtime.h>
#include <proto/dos.h>

#include "nsmtracker.h"
#  include <libcore/compiler.h>

#include "Amiga_player_proc.h"

#include "../common/player_proc.h"

STime playertime;

struct Player *player=NULL;
struct Conductor *conductor=NULL;

struct Task *amigatimertask=NULL;
extern struct Task *mytask;

ULONG waitforamigatimersig= -1;
ULONG startamigatimersig=-1;
ULONG pausesig=-1;
ULONG pausedplayingsig=-1;

extern LONG ptask2mtasksig;
extern void (*Ptask2MtaskCallBack)(void);

bool waitforamigatimersigokey=true;

extern struct Library *RealTimeBase;

extern char *screenname;
char *playertaskname;

struct myHook{
	struct Hook hook;
	struct Task *task;
	ULONG signal;
};

struct myHook *myhook;	/* Has to be global because of the GB. */


STime GetCurrentSTime(void){
	return (STime)(conductor->cdt_ClockTime*(PFREQ/1200));		// 1200 is a constant from realtime.libary
}

STime lasttime=0;

ULONG nulloutsig=-1;

void SAVEDS Amiga_timertask(void){
	STime newtime;

	ULONG mysignal;
	ULONG tempsigbit;
	ULONG temp2sigbit;

	startamigatimersig=AllocSignal(-1);
	tempsigbit=waitforamigatimersig=AllocSignal(-1);
	temp2sigbit=pausesig=AllocSignal(-1);
	nulloutsig=AllocSignal(-1);

	if(-1==waitforamigatimersig || -1==startamigatimersig || -1==pausesig || -1==nulloutsig){
		waitforamigatimersigokey=false;
		waitforamigatimersig=0;
		return;
	}


	waitforamigatimersig=1L<<waitforamigatimersig;
	pausesig=1L<<pausesig;

	Wait(1L<<startamigatimersig);
	FreeSignal(waitforamigatimersig);


	for(;;){
		mysignal=Wait(waitforamigatimersig | SIGBREAKF_CTRL_C | pausesig | 1L<<nulloutsig);

		if(mysignal & 1L<<nulloutsig){
		  lasttime=0;
		  SetConductorState(player,CONDSTATE_RUNNING,0);
		}

		if(mysignal & pausesig){
			Pdebug("paused player\n");
				Signal(mytask,1L<<pausedplayingsig);
				Wait(pausesig);
			Pdebug("stopped paused player\n");
			continue;
		}

		if(mysignal & waitforamigatimersig){
		  newtime=conductor->cdt_ClockTime*(PFREQ/1200);		// 1200 is a constant from realtime.libary
		  PlayerTask(newtime-lasttime);
		  lasttime=newtime;
		}

		if(mysignal & SIGBREAKF_CTRL_C) break;

	}

	FreeSignal(nulloutsig);
	FreeSignal(tempsigbit);
	FreeSignal(temp2sigbit);
}


ULONG ASM SAVEDS timercallback(
	REG(a0) struct myHook *myhook
){
	Signal(myhook->task,myhook->signal);
	return 0;
}

void Amiga_endtimertask(void){
	struct Task *task;

	if(amigatimertask!=NULL){
		Signal(amigatimertask,SIGBREAKF_CTRL_C);
	}

	Delay(10);

	for(;;){
		Forbid();
			task=FindTask(playertaskname);
		Permit();
		if(task==NULL) break;

		Delay(50);
	}

	if(pausedplayingsig!=-1) FreeSignal(pausedplayingsig);
}

void Amiga_endplayer(void){
	if(player!=NULL){
		SetConductorState(player,CONDSTATE_STOPPED,0);
		DeletePlayer(player);
	}
	Amiga_endtimertask();
}


void Amiga_initplayer2(void){
	debug("init1\n");
	Signal(amigatimertask,1L<<startamigatimersig);
	debug("init2, SetConductorState: %x, player:%x\n",RealTimeBase,player);
	SetConductorState(player,CONDSTATE_RUNNING,0);
	debug("init3\n");
}

extern char *screenname;

#ifdef __AROS__
APTR CreatePlayer(Tag tag, ...){
	return CreatePlayerA((struct TagItem *)&tag );
}
#endif

bool SAVEDS Amiga_initplayer1(void){
	IPTR playererrorcode = 0;
	APTR lockhandle;

	playertaskname=malloc(500);
	sprintf(playertaskname,"Player task for %s\n",screenname);

	pausedplayingsig=AllocSignal(-1);
	if(pausedplayingsig==-1){
		fprintf(stderr,"Could not create pausedplayingsig\n");
		return false;
	}

	myhook=talloc(sizeof(struct myHook));

	amigatimertask=CreateTask(playertaskname,PLAYERPRI,Amiga_timertask,20000L);

	if(amigatimertask==NULL){
		fprintf(stderr,"Could not create timer-task\n");
		return false;
	}


	myhook->hook.h_Entry=(HOOKFUNC)&timercallback;
	myhook->task=amigatimertask;

	while(waitforamigatimersig==-1) Delay(20);

	if( ! waitforamigatimersigokey){
		fprintf(stderr,"Could not allocate signal(s) in the timer-task.\n");
		return false;
	}

	myhook->signal=waitforamigatimersig;

	lockhandle=LockRealTime(RT_CONDUCTORS);
		player=CreatePlayer(
			PLAYER_Name,screenname,
			PLAYER_Conductor, screenname,
			PLAYER_Hook,&myhook->hook,
			PLAYER_Priority, PLAYERPRI*2,
			PLAYER_ErrorCode, &playererrorcode,
			TAG_DONE
		);
	if(lockhandle!=NULL) UnlockRealTime(lockhandle);

	if(player==NULL){
		fprintf(stderr,"Cant create Player from realtime.library:\n");
		switch(playererrorcode){
			case RTE_NOMEMORY:
				fprintf(stderr,"/* memory allocation failed      */\n");
				break;
			case RTE_NOCONDUCTOR:
				fprintf(stderr,"/* player needs a conductor      */\n");
				break;
			case RTE_NOTIMER:
				fprintf(stderr,"/* timer (CIA) allocation failed */\n");
				break;
			case RTE_PLAYING:
				fprintf(stderr,"/* can't shuttle while playing   */\n");
				break;
		}
		Amiga_endtimertask();
		return false;
	}

	conductor=player->pl_Source;

	return true;
}

void StartPlayer(void){
  if(nulloutsig!=-1) Signal(amigatimertask,1L<<nulloutsig);
  /*
  Forbid();
  lasttime=0;
  SetConductorState(player,CONDSTATE_RUNNING,0);
  Permit();
  */
}

void PausePlayer(void){
//	printf("starting to pause player\n");
	ULONG signal;

	Signal(amigatimertask,pausesig);

	for(;;){
		signal=Wait(1L<<pausedplayingsig | ptask2mtasksig); 
		if(signal & 1L<<pausedplayingsig) break;
		if(signal & ptask2mtasksig){
			(*Ptask2MtaskCallBack)();
		}
	}


//	printf("finished to pause player\n");
}

void StopPausePlayer(void){
	Signal(amigatimertask,pausesig);
}





