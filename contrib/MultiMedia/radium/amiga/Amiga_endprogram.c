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





#include <proto/intuition.h>
#include <proto/diskfont.h>
#include <proto/graphics.h>
#include <proto/input.h>
#include <proto/reqtools.h>
#include <proto/alib.h>


#include "nsmtracker.h"
#include "Amiga_player_proc.h"
#include "Amiga_bs.h"
#include "Amiga_bs_edit_proc.h"
#include "instrprop/Amiga_instrprop.h"

#include <proto/exec.h>

#include "../common/OS_endprogram_proc.h"

extern LONG Bptask2mtasksig;
extern LONG Bdebugsig;
extern LONG inputsig;


void OS_EndProgram(void){

	printf("end1\n");
	Amiga_endplayer();
	printf("end2\n");

	CloseBlockSelectWindow();
	printf("end3\n");

	if(HelpWnd!=NULL) CloseHelpWindow();
	printf("end4\n");
	if(CPPWindowWnd!=NULL) CloseCPPWindowWindow();
	printf("end5\n");

	if(Bptask2mtasksig!=-1) FreeSignal(Bptask2mtasksig);
	printf("end6\n");
	if(Bdebugsig!=-1) FreeSignal(Bdebugsig);
	printf("end7\n");

}

extern struct TextFont *sysfont;
extern bool useworkbench;
extern struct IOStdReq *InputIO;
extern struct MsgPort *keyPort;

void OS_EndProgram2(void){
//	uinit_memory();
	printf("bend1\n");

	if(Scr!=NULL){
		if(useworkbench){
			UnlockPubScreen( NULL, Scr );
		}else{
			CloseScreen( Scr );
		}
	}

	if(sysfont!=NULL) CloseFont(sysfont);
	Scr = NULL;

	CloseDevice((struct IORequest *)InputIO);
	DeleteExtIO((struct IORequest *)InputIO);
	DeletePort(keyPort);

	CloseLibrary((struct Library *)ReqToolsBase);
}

