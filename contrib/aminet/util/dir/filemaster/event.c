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

/*#define DEBUG 1*/

#include <dos/dos.h>
#include <graphics/gfx.h> 
#include <graphics/gfxmacros.h>
#include <utility/tagitem.h>   
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <stdio.h>  
#include <proto/dos.h>
#include <proto/exec.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/semaphores.h>
#include <exec/ports.h>
#include <intuition/intuition.h>
#include <intuition/sghooks.h>
#include <intuition/iobsolete.h>
#include <intuition/intuitionbase.h>   
#include <intuition/intuition.h>
#include <aros/debug.h>
#include <aros/bigendianio.h>
#include <graphics/gfxbase.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <devices/inputevent.h>
#include <aros/oldprograms.h> 
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <stdio.h> 
#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <intuition/gadgetclass.h>
#include <workbench/startup.h>
#include <strings.h>
#include <stdio.h>

#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"
#include "audio.h"
#include "commodity.h"
#include "child.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;

struct QuitConfig {
	unsigned areyousure:2;
};
struct SleepConfig {
	unsigned sleepwindow:2;
};

#define IDCMP IDCMP_MOUSEBUTTONS|IDCMP_GADGETDOWN|IDCMP_GADGETUP|IDCMP_RAWKEY|IDCMP_REFRESHWINDOW|IDCMP_NEWSIZE|IDCMP_CLOSEWINDOW|IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW

#ifndef AROS
extern void __asm RealTimeScroll(register __a0 struct ReqScrollStruct*);
#else /* AROS */
extern void RealTimeScroll(struct ReqScrollStruct*);
#endif /* AROS */

extern struct FMMain fmmain;
extern struct FMList fmlist[];
extern UBYTE workbench[];

void windownewsize(void);
void taskwindow(WORD);
void nextlist(struct ListInfo*,WORD);
void clickfile(void);
void parentpath(struct FMList*,BPTR);
WORD getparent(struct FMList*);
void setparent(struct FMList*);
void setcmenu(void);
void parse(void);
WORD suljenaytto(WORD);
WORD avaanaytto(WORD);
void freegadgets(struct Gadget*);
void askmodabort(void);
struct FMNode *iteratenode(struct FMNode**);
void gparent(void);
void aboutti(WORD);
void dirfileclick(struct FMList*,struct FMNode*,WORD);
void setnewconfig(void);
WORD config(void);

void operate(void);
//void __asm jump(register __a0 APTR,register __a1 APTR);
extern APTR launchtable[];
extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
extern struct FMList fmlist[];

void getdir(void);
void dirmeters(void);
void drives(struct FMList*);

static WORD sleep(WORD sleepv)
{
WORD ret=0;
if(!sleepv) if (!suljenaytto(3)) {
	return(0);
}
SetSignal(0,SIGBREAKF_CTRL_F);
fmmain.sleepon=fmmain.quitti;
Signal((struct Task*)fmmain.timeproc,SIGBREAKF_CTRL_F);
while(fmmain.sleepon==fmmain.quitti) waitsig(SIGBREAKF_CTRL_F);
if(fmmain.sleepon) {
	for(;;) {
		ret=avaanaytto(3);
		if(ret) break;
		DisplayBeep(0);
		Delay(50);
	}
}
fmmain.sleepon=0;
return(1);
}

void setsource(struct FMList *list1)
{
struct FMList *vlist;
if(list1==fmmain.sourcedir) return;
if(list1==fmmain.destdir) {
	vlist=fmmain.sourcedir;
	fmmain.sourcedir=list1;
	fmmain.destdir=vlist;
} else {
	fmmain.sourcedir->flags&=~(LDESTINATION|LSOURCE);
	fmmain.sourcedir=list1;
}
fmmain.sourcedir->flags&=~LDESTINATION;
fmmain.sourcedir->flags|=LSOURCE;
fmmain.destdir->flags&=~LSOURCE;
fmmain.destdir->flags|=LDESTINATION;
fmmessage(list1);
dirmeters();

}

static struct FMNode *mousenode(struct FMList *list,WORD xx,WORD yy)
{
struct ListInfo *li;
struct FMNode *node;
WORD cnt,apu1;

li=list->li;
if (xx>=li->dirx && xx<=li->edirx && yy>=li->diry && yy<li->ediry) {
	node=(struct FMNode*)list;
	iteratenode(&node);
	cnt=list->firstline;
	yy-=li->diry; if (yy<0) return(0);
	yy/=fmmain.listysize;
	apu1=cnt+yy;
	while(cnt--) iteratenode(&node);
	while(node && yy--) iteratenode(&node);
	if (!node||!node->succ) return(0);
	fmmain.sourcedir->keyline=apu1;
	return(node);
}
return(0);
}

static WORD processclick(struct IntuiMessage *msg)
{
struct timeval tv1,tv2;
struct FMNode *node;
struct FMList *list1=0;
struct ListInfo *li;
WORD xx,yy,apu1,cnt1;
WORD dragtype=-1;

D(bug("event.c 198 processclick func \n"));  
//freeese
xx=msg->MouseX-fmmain.ikkuna->BorderLeft;
yy=msg->MouseY-fmmain.ikkuna->BorderTop;

D(bug("event.c 203 processclick func \n"));
if (msg->Code==MENUDOWN && xx>=fmmain.cmenux && xx<=fmmain.cmenux+fmmain.totalcmenuwidth && yy>=fmmain.cmenuy && yy<=fmmain.cmenuy+fmmain.totalcmenuheight) {
D(bug("event.c 205 processclick func \n"));
	fmmain.cmenu+=fmmain.cmenugadnumperline*fmconfig->cmenucolumns;
D(bug("event.c 207 processclick func \n"));
	apu1=0;
	for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
		D(bug("event.c 208 processclick func \n"));
		if(fmconfig->cmenuconfig[cnt1].position==1) 
		{
		D(bug("event.c 211 processclick func \n"));
		apu1++;
		}
	D(bug("event.c 214 processclick func \n"));
	}
D(bug("event.c 216 processclick func \n"));
	if(fmmain.cmenu>apu1) 
	{
D(bug("event.c 219 processclick func \n"));
	fmmain.cmenu=0;
}
D(bug("event.c 222 processclick func \n"));
	setcmenu();
D(bug("event.c 224 processclick func \n"));
}
D(bug("event.c 228 processclick func \n")); 

for(apu1=0;apu1<LISTS;apu1++) {
D(bug("event.c 231 processclick func \n")); 
	li=fmlist[apu1].li;
D(bug("event.c 233 processclick func \n")); 
	if(li&&xx>=li->x&&xx<=li->x+li->width&&yy>=li->y&&yy<=li->y+li->height) {
D(bug("event.c 235 processclick func \n"));  
		list1=&fmlist[apu1];
D(bug("event.c 237 processclick func \n")); 
		break;
D(bug("event.c 239 processclick func \n")); 
	}
D(bug("event.c 240 processclick func \n")); 
}
D(bug("event.c 243 processclick func \n")); 
if (yy>=fmmain.messageliney1 && msg->Code==MENUDOWN) askmodabort();
D(bug("event.c 245 processclick func \n")); 

if (!list1||!li) return(dragtype);
D(bug("event.c 246 processclick func \n")); 

if (msg->Code==MENUDOWN && fmmain.sourcedir->flags&LACTIVE) fmmain.sourcedir->flags|=LABORTREQ;
D(bug("event.c 249 processclick func \n")); 
setsource(list1);
D(bug("event.c 251 processclick func \n")); 
if(list1->flags&LALLOCATED) return(dragtype);
D(bug("event.c 253 processclick func \n"));
 
if(msg->Code==MENUDOWN) {
D(bug("event.c 258 processclick func \n")); 
	if(xx>=li->slider1.LeftEdge&&xx<=li->slider1.LeftEdge+li->slider1.Width&&yy>=li->slider1.TopEdge&&yy<=li->slider1.TopEdge+li->slider1.Height) {
		D(bug("event.c 260 processclick func \n")); 
		launch(launchtable[PARENTCONFIG],getconfignumber(PARENTCONFIG),0,0);
		D(bug("event.c 262 processclick func \n")); 
		/* gparent(); */
		return(dragtype);
	}
	if (yy>=li->topliney&&yy<=li->topliney+fmmain.txtysize+1) {
		if (list1->flags&LDIRLIST)
			launch((APTR)&getdir,0,0,MSUBPROC);
			else
			drives(list1);
	}
	if(fmconfig->rightmouse==1) {
		launch(launchtable[PARENTCONFIG],getconfignumber(PARENTCONFIG),0,0);
		/* gparent(); */
		return(dragtype);
	}
}
D(bug("event.c 278 processclick func \n")); 
node=mousenode(list1,xx,yy);
D(bug("event.c 280 processclick func \n")); 
if(!node) return(dragtype);

if(msg->Code==MENUDOWN&&fmconfig->rightmouse==0) dirfileclick(list1,node,xx);
D(bug("event.c 284 processclick func \n")); 
if (msg->Code==SELECTDOWN) {
	D(bug("event.c 285 processclick func \n")); 
	node->flags^=NSELECTED;
	if(node->flags&NSELECTED) dragtype=1; else dragtype=0;
	if (list1->flags&LDIRLIST&&node->flags&NSELECTED) endofdirtxt(list1,node);
	if (list1->flags&LDIRLIST&&!(node->flags&NSELECTED)) endofdirtxt(list1,0);
	outputlistline(list1,node);
	 D(bug("event.c 292 processclick func \n"));
	tv1.tv_secs=fmmain.secs;tv1.tv_micro=fmmain.mics;
	tv2.tv_secs=msg->Seconds;tv2.tv_micro=msg->Micros;
	SubTime(&tv2,&tv1);
	 D(bug("event.c 296 processclick func \n"));
	if (list1->lastclicked==node && !tv2.tv_secs && tv2.tv_micro/10000<=fmconfig->doubleclick) {
		dirfileclick(list1,node,xx);
		list1->lastclicked=0;
	} else {
		list1->lastclicked=node;
	}
	fmmain.secs=msg->Seconds; fmmain.mics=msg->Micros;
}
D(bug("event.c 303 processclick func \n")); 
return(dragtype);
}


static void processgadget(WORD id,ULONG class)
{
if (id>=301 && id<(301+TOTALCOMMANDS)) {
	if(id-301<TOTALCOMMANDS) runcommand(&fmmain.gadkeytab[id-301+LISTGADGETS*WINDOWLISTS]);
}
	/*  slider1s 101-> */
	/*  slider2s 111-> */
	/*  string gadgets 201-> */
	/*  taskgadget 401-> */
	/*  ltaskgadget 411-> */
	/*  rtaskgadget 421-> */

if (id>=401&&id<=410) {
	setsource(fmmain.li[id-401]->list);
	taskwindow(id-401);
	return;
}
if (id>=411&&id<=420) {
	setsource(fmmain.li[id-411]->list);
	nextlist(fmmain.sourcedir->li,-1);
	return;
}
if (id>=421&&id<=430) {
	setsource(fmmain.li[id-421]->list);
	nextlist(fmmain.sourcedir->li,1);
	return;
}
if (id>=201&&id<=210) {
	setsource(fmmain.li[id-201]->list);
	setparent(fmmain.sourcedir);
	fmmain.sourcedir->fmmessage1[0]=0;
	fmmain.sourcedir->fmmessage2[0]=0;
	fmmessage(fmmain.sourcedir);
	launch((APTR)&getdir,0,0,MSUBPROC);
	return;
}
if (id>=101&&id<=110) {
	setsource(fmmain.li[id-101]->list);
	RealTimeScroll(&fmmain.sourcedir->li->rscroll1);
	fmmain.sourcedir->firstline=fmmain.sourcedir->li->rscroll1.TopEntryNumber;
	return;
}
if (id>=111&&id<=120) {
	setsource(fmmain.li[id-111]->list);
	RealTimeScroll(&fmmain.sourcedir->li->rscroll2);
	fmmain.sourcedir->firstlinew=fmmain.sourcedir->li->rscroll2.TopEntryNumber;
	return;
}
}


void runhotkey(WORD hotkey)
{
struct GadKeyTab *gkt;

D(bug("event.c 325 \n")); 

gkt=&fmmain.gadkeytab[hotkey];

if((gkt->onelist&&gkt->listnum==fmmain.sourcedir->listnumber)||(!gkt->onelist)) {
D(bug("event.c 330 \n")); 
	if(gkt->cmc) {
	D(bug("event.c 331 \n")); 
		runcommand(gkt);
	} else {
		D(bug("event.c 334 \n")); 
		processgadget(gkt->g->GadgetID,0);
	}
	}
}

static void frameoutput(struct FMList *l)
{
fmmain.framenode=scrollifneeded(l,&l->keyline,&l->keylinew);
fmmain.framelist=l;
fmmain.framenode->flags|=NKEYBOARD;
outputlistline(fmmain.framelist,fmmain.framenode);
}
static WORD endframeoutput(void)
{
WORD was;

was=0;
if(fmmain.framenode) {
	fmmain.framenode->flags&=~NKEYBOARD;
	outputlistline(fmmain.framelist,fmmain.framenode);
	was=1;
}
fmmain.framenode=0;
fmmain.framelist=0;
return(was);
}

WORD smallevent(struct Window *win,ULONG *class,UWORD *code)
{
struct IntuiMessage *message;
ULONG sigs;

while(!(message=(struct IntuiMessage*)GetMsg(win->UserPort))) {
		sigs=Wait((1L<<win->UserPort->mp_SigBit)|SIGBREAKF_CTRL_C);
		if(sigs&SIGBREAKF_CTRL_C) {
			SetSignal(SIGBREAKF_CTRL_C,SIGBREAKF_CTRL_C);
			return(1);
		}
}
*class=message->Class;
*code=message->Code;
ReplyMsg((struct Message*)message);
return(0);
}

void getappdir(struct AppMessage *appmsg)
{
BPTR lock;

ActivateWindow(fmmain.ikkuna);
if(!(fmmain.sourcedir->flags&LALLOCATED)&&appmsg->am_NumArgs>=1) {
	lock=DupLock(appmsg->am_ArgList->wa_Lock);
	if(lock) {
		parentpath(fmmain.sourcedir,lock);
		launch((APTR)&getdir,0,0,MSUBPROC);
	}
}
}

void event(WORD sleepv)
{
ULONG class,signalmask,sigs;
struct IntuiMessage *message=0;
struct AppMessage *appwinmsg=0;
struct IntuiMessage fakeintu;
struct InputEvent ie;
struct FMNode *apunode1,*apunode2;
struct FMList *apulist;
UWORD code;
UBYTE asciicode;
WORD draggi=-1,dragged=0;
WORD apu1,apu2,apu3,framewas;
LONG apu4;
UBYTE chartab[2];
struct ReqScrollStruct *rss;
WORD waiti=0,superhotkey;
D(bug("event.c 410 \n")); 
signalmask=1L<<fmmain.ikkuna->UserPort->mp_SigBit;
D(bug("event.c 412 \n")); 

if(fmmain.appwinport) signalmask|=(1L<<fmmain.appwinport->mp_SigBit);
signalmask|=SIGBREAKF_CTRL_F;

ie.ie_Class=IECLASS_RAWKEY;
ie.ie_SubClass=0;
if(sleepv) {
	   D(bug("event.c 420 \n")); 
	launch(launchtable[SLEEPCONFIG],getconfignumber(SLEEPCONFIG),0,0);
	sleep(1);
	if(fmmain.kill) return;
}
D(bug("event.c 424 \n")); 
setsource(&fmlist[1]);
setsource(&fmlist[0]);

#ifndef AROS
Signal((struct Task*)fmmain.timeproc,SIGBREAKF_CTRL_D);
#endif

D(bug("event.c 428 \n"));
 
fmmain.fmactive=1|2;

eventti:
   D(bug("event.c 434 \n")); 
framewas=0; class=0; code=0; fmmain.quitti=0;
superhotkey=-1;
while(!fmmain.quitti) {
	if(waiti) {
		 D(bug("event.c 467 \n"));  
		if(framewas) frameoutput(fmmain.sourcedir);
		D(bug("event.c 469 \n"));  
		sigs=waitsig(signalmask);
		D(bug("event.c 471 \n"));  
		waiti=0;
		if(sigs&SIGBREAKF_CTRL_F) {
			if(fmmain.passhotkey) {
				 D(bug("event.c 445 \n")); 
				runhotkey(fmmain.passhotkey-1);
				fmmain.passhotkey=0;
			} else {
				D(bug("event.c 448 \n"));      
				launch(launchtable[SLEEPCONFIG],getconfignumber(SLEEPCONFIG),0,0);
			}
		}
	}
	if(fmmain.appwinport) {
		appwinmsg=(struct AppMessage*)GetMsg(fmmain.appwinport);
		if(appwinmsg) {
			   D(bug("event.c 457 \n")); 
			getappdir(appwinmsg);
			ReplyMsg((struct Message*)appwinmsg);
			appwinmsg=0;
			continue;
		}
	}
	if(superhotkey>=0) runhotkey(superhotkey);
	superhotkey=-1;
	message=(struct IntuiMessage*)GetMsg(fmmain.ikkuna->UserPort);
            D(bug("event.c 467 \n")); 

	if(!message) {
		waiti=1;
		continue;
	}

	  D(bug("event.c superhotkey 473 \n"));         
	 //superhotkey=checkhotkeys(message);
		  D(bug("event.c 475 \n"));         
	framewas=endframeoutput();

	class=message->Class;
	code=message->Code;
	fakeintu.MouseX=message->MouseX;
	fakeintu.MouseY=message->MouseY;
	fakeintu.Seconds=message->Seconds;
	fakeintu.Micros=message->Micros;
	asciicode=0;
   D(bug("event.c 492 \n"));  
	if (class==IDCMP_RAWKEY&&!(code&0x80)&&!(fmmain.sourcedir->flags&LALLOCATED)) {
		 D(bug("event.c 494 \n"));
		switch(code)
		{
		case 0x4c:	/* cursor up */
		 D(bug("event.c 491 case cursor up \n"));
		if(message->Qualifier&0x0003) fmmain.sourcedir->keyline-=fmmain.sourcedir->li->visiblelines-2;
		if(message->Qualifier&0x0008) fmmain.sourcedir->keyline=1;
		fmmain.sourcedir->keyline--;
		frameoutput(fmmain.sourcedir);
		break;
		case 0x4d:	/* cursor down */
		   D(bug("event.c 498 case cursor down \n")); 
		if(message->Qualifier&0x0003) fmmain.sourcedir->keyline+=fmmain.sourcedir->li->visiblelines-2;
		if(message->Qualifier&0x0008) fmmain.sourcedir->keyline=fmmain.sourcedir->totlines-2;
		fmmain.sourcedir->keyline++;
		frameoutput(fmmain.sourcedir);
		break;
		case 0x4e:	/* cursor left */
		   D(bug("event.c 505 case cursor left \n")); 
		if(message->Qualifier&0x0008)
			fmmain.sourcedir->keylinew=fmmain.sourcedir->totlinesw-1;
			else
			fmmain.sourcedir->keylinew+=fmmain.sourcedir->li->visiblelinesw;
		frameoutput(fmmain.sourcedir);
		break;
		case 0x4f:	/* cursor right */
		   D(bug("event.c 513 case cursor right \n")); 
		if(message->Qualifier&0x0008)
			fmmain.sourcedir->keylinew=0;
			else
			fmmain.sourcedir->keylinew-=fmmain.sourcedir->li->visiblelinesw;
		frameoutput(fmmain.sourcedir);
		break;
		}
	}
	if(class==IDCMP_RAWKEY) {
		   D(bug("event.c 523 if IDCMP_RAWKEY \n")); 
		apu1=0;
		switch(code)
		{
		case 0x00:	/* ~pushed */
		 D(bug("event.c 528 if IDCMP_RAWKEY \n"));  
		if(fmmain.modflag==MODPLAY) *((UBYTE*)(fmmain.modptr))=1;
		apu1=1;
		break;
		case 0x80:	/* ~released */
		 D(bug("event.c 533 if IDCMP_RAWKEY \n"));  
		if(fmmain.modflag==MODPLAY) *((UBYTE*)(fmmain.modptr))=0;
		 D(bug("event.c 535 if IDCMP_RAWKEY \n"));  
		apu1=1;
		break;
		}
		if(!apu1) {
			 D(bug("event.c 540 if IDCMP_RAWKEY \n"));  
			if (!(message->Code&0x80)) {
				 D(bug("event.c 542 if IDCMP_RAWKEY \n"));  
				ie.ie_Code=message->Code;
				D(bug("event.c 544 if IDCMP_RAWKEY \n")); 
				ie.ie_Qualifier=message->Qualifier;
				D(bug("event.c 546 if IDCMP_RAWKEY \n")); 
			#ifndef AROS
				ie.ie_EventAddress=(APTR*)*((ULONG*)message->IAddress);  
			#else 
				ie.ie_EventAddress=message->IAddress;
			#endif
				D(bug("event.c 548 if IDCMP_RAWKEY \n")); 
				if (MapRawKey(&ie,chartab,2,0)==1) asciicode=ToUpper(chartab[0]);
				 D(bug("event.c 550 if IDCMP_RAWKEY \n"));  
			}
			 D(bug("event.c 563 if IDCMP_RAWKEY \n")); 
		}
	 D(bug("event.c 565 if IDCMP_RAWKEY \n")); 
	}
	 D(bug("event.c 567 if IDCMP_RAWKEY \n")); 
	if(asciicode) {
	 D(bug("event.c 569 if IDCMP_RAWKEY \n")); 
		framewas=0;
		if(asciicode==9) {
		 D(bug("event.c 572 if IDCMP_RAWKEY \n")); 
			apu1=fmmain.sourcedir->listnumber;
			for(;;) {
				if (message->Qualifier&0x03) apu1--; else apu1++;
				if (apu1==LISTS) apu1=0;
				if (apu1<0) apu1=LISTS-1;
				if(fmlist[apu1].li) {
					setsource(&fmlist[apu1]);
					break;
				}
			}
			if (!(fmmain.sourcedir->flags&LALLOCATED)) {
				frameoutput(fmmain.sourcedir);
			}
		} else if (!(fmmain.sourcedir->flags&LALLOCATED)&&asciicode) {
			 D(bug("event.c 587 if IDCMP_RAWKEY \n")); 
			switch(asciicode)
			{
			/*break;*/
			case 13:	/* return */
			 D(bug("event.c 592 case 13 \n")); 
			fakeintu.MouseX=fmmain.sourcedir->li->dirx+fmmain.ikkuna->BorderLeft;
			fakeintu.MouseY=(fmmain.sourcedir->keyline-fmmain.sourcedir->firstline)*fmmain.listysize+fmmain.sourcedir->li->diry+fmmain.ikkuna->BorderTop;
			if(message->Qualifier&0x03) fakeintu.Code=MENUDOWN; else fakeintu.Code=SELECTDOWN;
			fakeintu.Class=IDCMP_MOUSEBUTTONS;
			 D(bug("event.c 597 processclick \n")); 
			processclick(&fakeintu);
			 D(bug("event.c 599 back from processclick \n"));  
			if(!(fmmain.sourcedir->flags&LALLOCATED)) {
				frameoutput(fmmain.sourcedir);
			}
			break;
			}
		}
	}
D(bug("event.c 572 before switch(class)\n"));      

	switch(class)
	{
	case IDCMP_ACTIVEWINDOW:
	  D(bug("case IDCMP_ACTIVEWINDOW\n"));
	fmmain.fmactive|=2;
	framewas=0;
	break;
      case IDCMP_INACTIVEWINDOW:
  D(bug("case IDCMP_INACTIVEWINDOW\n"));
        fmmain.fmactive&=~2;
        framewas=0;
        break;
    case IDCMP_RAWKEY:
  D(bug("case IDCMP_RAWKEY\n"));
        if(code&0x80&&fmmain.framenode&&!(fmmain.sourcedir->flags&LALLOCATED)) {
                frameoutput(fmmain.sourcedir);
        }
        break;
   case IDCMP_CLOSEWINDOW:
  D(bug("case  IDCMP_CLOSEWINDOW\n"));
        framewas=0;
        launch(launchtable[QUITCONFIG],getconfignumber(QUITCONFIG),0,0);
        break;
   case IDCMP_REFRESHWINDOW:
  D(bug("case IDCMP_REFRESHWINDOW\n"));
        BeginRefresh(fmmain.ikkuna);
        EndRefresh(fmmain.ikkuna,1);
        break;
    case IDCMP_NEWSIZE:
  D(bug("case IDCMP_NEWSIZE\n"));
        framewas=0;
        windownewsize();
        break;
     case IDCMP_DISKINSERTED: 
 D(bug("case IDCMP_DISKINSERTED\n"));  
     case IDCMP_DISKREMOVED:
  D(bug("case IDCMP_DISKREMOVED\n"));
        framewas=0;
        for(code=0;code<WINDOWLISTS;code++) {
                if (fmmain.li[code]&&!(fmmain.li[code]->list->flags&LALLOCATED)&&fmmain.li[code]->
                        list->flags&LDEVLIST) drives(fmmain.li[code]->list);
        }
        break;     

	case IDCMP_MOUSEMOVE:
  D(bug("case IDCMP_MOUSEMOVE\n")); 
	framewas=0;
	apu1=message->MouseY-fmmain.ikkuna->BorderTop;
	apulist=fmmain.sourcedir;
	if(draggi==-1||!(message->Qualifier&IEQUALIFIER_LEFTBUTTON)) {
		ReportMouse(0,fmmain.ikkuna);
		draggi=-1;dragged=0;
		break;
	}
	apunode1=mousenode(apulist,apulist->li->dirx,apu1);
	do {
		apu4=-1;
		if(!apunode1&&apu1>=0) {
			rss=&apulist->li->rscroll1;
			if(apu1<apulist->li->diry&&apulist->firstline) {
					apulist->firstline--;
					apu4=apulist->firstline;
			} else if (apu1>apulist->li->ediry&&apulist->firstline<apulist->totlines+apulist->li->visiblelines) {
					apulist->firstline++;
					apu4=apulist->firstline+apulist->li->visiblelines-1;
			}
			if(apu4>=0) {
				apunode2=(struct FMNode*)apulist;
				iteratenode(&apunode2);
				while(apu4--) iteratenode(&apunode2);
				if(apunode2&&apunode2->succ) {
					if(draggi)
						apunode2->flags|=NSELECTED;
						else
						apunode2->flags&=~NSELECTED;
				}
				apu3=rss->LineSpacing;
				rss->LineSpacing=1;
				apu2=rss->TopEntryNumber;
				updatelist(apulist);
				rss->TopEntryNumber=apu2;
				RealTimeScroll(rss);
				apulist->firstline=rss->TopEntryNumber;
				rss->LineSpacing=apu3;
				dragged=1;
			}
		}
	} while(apu1&&!fmmain.ikkuna->UserPort->mp_MsgList.lh_Head->ln_Succ);
	apu1=0;
	apunode2=(struct FMNode*)apulist;
	if(!(!apunode1||apunode1==apulist->lastclicked||!apulist->lastclicked||apunode1==apunode2||draggi<0)) {
		while(iteratenode(&apunode2)) {
			apu3=-1;
			if(apunode2==apunode1||apunode2==apulist->lastclicked) apu3=apu1^1;
			if(apu3==1) apu1=apu3;
			apu2=apunode2->flags&NSELECTED;
			if(apu1&&((apu2&&!draggi)|(!apu2&&draggi))) {
				apunode2->flags^=NSELECTED;
				outputlistline(apulist,apunode2);
				dragged=1;
			}
			if(!apu3) apu1=apu3;
		}
	}
	break;

	case IDCMP_GADGETUP:
        D(bug("case IDCMP_GADGETUP\n")); 
  
	case IDCMP_GADGETDOWN:
         D(bug("case IDCMP_GADGETDOWN\n"));   
	framewas=0;
	processgadget(((struct Gadget*)message->IAddress)->GadgetID,class);
	break;


	case IDCMP_MOUSEBUTTONS:
  D(bug("case IDCMP_MOUSEBUTTONS\n")); 
	framewas=0;
	if (code==SELECTUP||code==SELECTDOWN||code==MENUDOWN||code==MENUUP) {
		D(bug("event.c 694"));
		if(code==SELECTUP&&dragged) {
			  D(bug("event.c 696"));  
			endofdirtxt(fmmain.sourcedir,0);
			  D(bug("event.c 698"));  
			ReportMouse(0,fmmain.ikkuna);
			  D(bug("event.c 700"));  
			draggi=-1;
			  D(bug("event.c 702"));  
			dragged=0;
			  D(bug("event.c 704"));  
		} else {	
			  D(bug("event.c 706"));

			draggi=processclick(message);

			  D(bug("event.c 708"));  
			if(code==SELECTDOWN&&draggi>=0) ReportMouse(1,fmmain.ikkuna);
			 D(bug("event.c 710")); 
		}
	}
	break;
	}


	ReplyMsg((struct Message*)message);
	message=0;

	fmmain.fmactive&=~1;
	if(fmmain.newconfig&0x0f) {
		if((fmmain.newconfig&0x0f)==3) {
			while(!suljenaytto(3));
			fmconfig->screentype=fmmain.newconfig>>8;
			if (!avaanaytto(3)) fmmain.quitti=1;
			for(apu1=0;apu1<LISTS;apu1++) if(fmlist[apu1].flags==0) drives(&fmlist[apu1]);
		} else if ((fmmain.newconfig&0x0f)==2) {
			while(!suljenaytto(2));
			fmconfig->screentype=fmmain.newconfig>>8;
			if (!avaanaytto(2)) fmmain.quitti=1;
		} else {
			windownewsize();
		}
		framewas=0;
	}
	fmmain.newconfig=0;
	fmmain.fmactive|=1;

}
if(fmmain.quitti>4) {
	if(fmmain.kill) return;
	sleep(0);
	if(fmmain.kill) return;
	goto eventti;
}
if(fmmain.ikkuna) {
	if(tstsubproc(1)) goto eventti;
} else {
	while(tstsubproc(0)) Delay(10);
}
if (fmmain.ikkuna&&fmmain.quitti==2&&!requestmsg(getstring(MSG_MAIN_QUIT),MSG_YES,MSG_NO,MSG_MAIN_AREYOUSURE)) goto eventti;
aboutti(1);
while(tstsubproc(0)) Delay(10);
endframeoutput();
}

void dirfileclick(struct FMList *list1,struct FMNode *node,WORD xx)
{
setparent(list1);
if (node->flags & (NDEVICE|NASSIGN|NDIRECTORY)) {
	if (list1->flags & LDIRLIST) {
		AddPart(list1->path,NDFILE(node),PATH);
	} else {

		if (xx>=list1->li->dirx+8*fmmain.listxsize&&xx<list1->li->edirx&&NDCOMM(node)[0])
			strcpy(list1->path,NDCOMM(node));
			else
			strcpy(list1->path,NDFILE(node));
		if (node->flags&NASSIGN) parentpath(list1,0);

	}
	launch((APTR)&getdir,0,0,MSUBPROC);
} else {
	list1->fmmessage1[0]=0;
	list1->fmmessage2[0]=0;
	fmmessage(list1);
	list1->lastclicked=node;
	launch((APTR)&clickfile,0,0,MSUBPROC);

}
}

void gquit(void)
{
struct ProcMsg *pm;
struct QuitConfig *config;

pm=sinitproc();
config=getconfig(pm->cmc);
if(config->areyousure)
	fmmain.quitti=2;
	else
	fmmain.quitti=1;
deinitproc(pm);
}
WORD getappicon(void)
{
struct SleepConfig *sc;

sc=getconfignumber(SLEEPCONFIG)->moreconfig;
return((WORD)(sc->sleepwindow));
}
void gsleep(void)
{
struct ProcMsg *pm;
struct SleepConfig *config;

pm=sinitproc();
config=getconfig(pm->cmc);
if(config->sleepwindow)
	fmmain.quitti=6;
	else
	fmmain.quitti=5;
deinitproc(pm);
}
void gstod(void)
{
struct ProcMsg *pm;

pm=sinitproc();

if(((fmmain.sourcedir->flags|fmmain.destdir->flags)&LALLOCATED)==0) {
	clearlist(fmmain.destdir);
	duplist(fmmain.sourcedir,fmmain.destdir);
	fmmain.destdir->firstline=fmmain.sourcedir->firstline;
	fmmain.destdir->firstlinew=fmmain.sourcedir->firstlinew;
	strcpy(fmmain.destdir->path,fmmain.sourcedir->path);
	fmmain.destdir->flags&=~(LDIRLIST|LDEVLIST);
	fmmain.destdir->flags|=fmmain.sourcedir->flags&(LDIRLIST|LDEVLIST);
	if (fmmain.destdir->li) RefreshGList(&fmmain.destdir->li->string,fmmain.ikkuna,0,1);
	csortlist(fmmain.destdir);
	parselist(fmmain.destdir,0);
	updadirmeter(fmmain.destdir);
	outputlist(fmmain.destdir);
}

deinitproc(pm);
}

void gconfig(void)
{
struct ProcMsg *pm;
pm=sinitproc();
if(!tstsubproc(1)) {
	fmmain.newconfig=config();
	Signal((struct Task*)fmmain.timeproc,SIGBREAKF_CTRL_D);
}
deinitproc(pm);
}
void gdrives(void)
{
struct ProcMsg *pm;
pm=sinitproc();
if (!(fmmain.sourcedir->flags&LALLOCATED)) drives(fmmain.sourcedir);
deinitproc(pm);
}
void gparent(void)
{
struct ProcMsg *pm;
UBYTE *apup1;

pm=sinitproc();
if (!(fmmain.sourcedir->flags&LALLOCATED)) {
	if(*(fmmain.sourcedir->path+strlen(fmmain.sourcedir->path)-1)==':') {
		drives(fmmain.sourcedir);
	} else {
		apup1=PathPart(fmmain.sourcedir->path);*apup1=0; 
		if (apup1==&fmmain.sourcedir->path[0]) {
			drives(fmmain.sourcedir);
		} else {
			if (!getparent(fmmain.sourcedir)) launch((APTR)&getdir,0,0,MSUBPROC);
		}
	}
}
deinitproc(pm);
}
void ginvert(void)
{
struct ProcMsg *pm;
struct FMNode *node;

pm=sinitproc();
if (!(fmmain.sourcedir->flags&LALLOCATED)) {
	for(node=fmmain.sourcedir->head;node->succ;node=node->succ) {
		if(node->flags&NMATCHED) node->flags^=NSELECTED;
	}
	outputlist(fmmain.sourcedir);
	endofdirtxt(fmmain.sourcedir,0);
}
deinitproc(pm);
}
void gclear(void)
{
struct ProcMsg *pm;
struct FMNode *node;

pm=sinitproc();
if (!(fmmain.sourcedir->flags&LALLOCATED)) {
	for(node=fmmain.sourcedir->head;node->succ;node=node->succ) {
		node->flags&=~NSELECTED;
	}
	outputlist(fmmain.sourcedir);
	endofdirtxt(fmmain.sourcedir,0);
}
deinitproc(pm);
}

ULONG waitsig(ULONG sigs)
{
ULONG rsigs;

for(;;) {
	rsigs=Wait(sigs|SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D);
	if(rsigs&SIGBREAKF_CTRL_C) {
		Signal((struct Task*)fmmain.timeproc,SIGBREAKF_CTRL_C);
	}
	if(rsigs&SIGBREAKF_CTRL_D) {
		Signal((struct Task*)fmmain.timeproc,SIGBREAKF_CTRL_D);
	}
	rsigs&=~((LONG)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D));
	if(rsigs) break;
}
return(rsigs);
}


void *sleepconfigdefault(struct CMenuConfig *cmc)
{
struct SleepConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct SleepConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct SleepConfig*)cmc->moreconfig;
config->sleepwindow|=2;
return(cmc->moreconfig);
}

WORD sleepconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
struct SleepConfig *sc;
WORD sleepwindow,appwindow;

sc=getconfig(cmc);
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_SLEEP_SLEEPWINDOW,200,guiUC|guiLEFT);
reqinfomsg(gb,MSG_SLEEP_APPICON,201,guiUC|guiLEFT);
setguigroup(gb,2,0);
sleepwindow=sc->sleepwindow&1;
appwindow=(sc->sleepwindow&2)?1:0;
reqcycle2msg(gb,200,&sleepwindow);
reqcycle2msg(gb,201,&appwindow);
commandanswer(gb);
c=quickreq(gb);
sc->sleepwindow=sleepwindow;
sc->sleepwindow|=appwindow<<1;
if(!appwindow) sc->sleepwindow|=1;
return(c);
}

void *quitconfigdefault(struct CMenuConfig *cmc)
{
struct QuitConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct QuitConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct QuitConfig*)cmc->moreconfig;
config->areyousure=1;
return(cmc->moreconfig);
}

WORD quitconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
struct QuitConfig *qc;
WORD ays;

qc=getconfig(cmc);
ays=qc->areyousure;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_MAIN_AREYOUSURE,200,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqcycle2msg(gb,200,&ays);
commandanswer(gb);
c=quickreq(gb);
qc->areyousure=ays;
return(c);
}
