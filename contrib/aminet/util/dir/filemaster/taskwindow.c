/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
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
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <exec/types.h>
#include <intuition/intuition.h>
#include <strings.h>
#include <stdio.h>

#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;


void setsource(struct FMList*);
void bdrives(struct FMList*);
void updatetasknumbers(struct ListInfo*);
WORD setlist(struct ListInfo*,struct FMList*);
void gadgettables(void);

extern struct FMMain fmmain;
extern struct FMList fmlist[];
extern struct FMConfig *fmconfig;

void taskwindow(WORD listnum)
{
struct GUIBase *gb;
UBYTE text[500],title[80];
WORD cnt;
UBYTE *txtptr;
UBYTE *ptr1;
struct IntuiMessage *imsg;

#ifdef V39
	SetWindowPointer(fmmain.ikkuna,WA_BusyPointer,TRUE,WA_PointerDelay,TRUE,TAG_DONE);
#endif
Forbid();
txtptr=text;
for (cnt=0;cnt<LISTS;cnt++) {
	ptr1=getstring(MSG_TASKWINDOW_EMPTY);
	if (fmlist[cnt].flags) {
		if (fmlist[cnt].flags&LDEVLIST) ptr1=getstring(MSG_TASKWINDOW_DEVLIST);
		if (fmlist[cnt].workname[0]) {
			ptr1=fmlist[cnt].workname;
		} else {
			if (fmlist[cnt].path[0]) ptr1=fmlist[cnt].path;
		}
	}
	sformat(txtptr,"(_%ld) %-39.39s",cnt,ptr1);
	txtptr+=45;
}
Permit();
txtptr=text;
sformat(title,getstring(MSG_TASKWINDOW_LISTS),(WORD)fmmain.li[listnum]->list->listnumber);
gb=getguibase(title);
for(cnt=0;cnt<LISTS;cnt++) {
	reqbutton(gb,txtptr,cnt+10,guiLEFT);
	txtptr+=45;
}
cnt=quickreq(gb);

if(cnt>=10&&cnt<=19) {
	cnt-=10;
	setlist(fmmain.li[listnum],&fmlist[cnt]);
}
#ifdef V39
	SetWindowPointer(fmmain.ikkuna,TAG_DONE);
#endif
while((imsg=(struct IntuiMessage*)GetMsg(fmmain.ikkuna->UserPort))) ReplyMsg((struct Message*)imsg);
}

void nextlist(struct ListInfo *li,WORD cnt)
{
WORD apu1;

apu1=li->list-&fmlist[0];
for(;;) {
	apu1+=cnt;
	if(apu1>=LISTS) apu1=0;
	if(apu1<0) apu1=LISTS-1;
	if(setlist(li,&fmlist[apu1])) break;
}
}

WORD setlist(struct ListInfo *li,struct FMList *list)
{
WORD apu1;

for(apu1=0;apu1<LISTS;apu1++) {
	if(fmmain.li[apu1]&&fmmain.li[apu1]->list==list&&fmmain.li[apu1]!=li) return(0);
}
if (li->list) li->list->li=0;
li->list=list;
list->li=li;
li->rscroll2.fmlist=li->rscroll1.fmlist=list;
updatetasknumbers(li);
if(list->path[0]==0||list->flags&LDEVLIST) bdrives(list);
if(list->flags&LALLOCATED)
	offgadget(&li->slider1,LISTGADGETS);
	else
	ongadget(&li->slider1,LISTGADGETS);
li->sinfo.Buffer=list->path;
csortlist(list);
parselist(list,0);
RefreshGList(&li->string,fmmain.ikkuna,0,1);
setsource(list);
outputlist(list);
updadirmeter(list);
return(1);
}

void updatetasknumbers(struct ListInfo *li)
{
WORD cnt;

if(!li) {
	for(cnt=0;cnt<WINDOWLISTS;cnt++) {
		if(fmmain.li[cnt]) {
			fmmain.li[cnt]->taskgadget.UserData=(APTR)(IPTR)(fmmain.li[cnt]->list->listnumber+'0');
			ongadget(&fmmain.li[cnt]->taskgadget,LISTGADGETS);
		}
	}	
} else {
	li->taskgadget.UserData=(APTR)(IPTR)(li->list->listnumber+'0');
	ongadget(&li->taskgadget,LISTGADGETS);
}
gadgettables();
}
