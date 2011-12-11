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
#include <stdio.h>
 
#include <aros/bigendianio.h>
#include <graphics/gfxbase.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <devices/inputevent.h>
 
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <proto/commodities.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <devices/timer.h>
#include <utility/hooks.h>
#include <strings.h>
#include <stdio.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "commodity.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

#define STARTSLEEP 1
#define SLEEPING 2
#define QUITSLEEP 3
#define QUITSLEEPING 4
#define KILLSLEEP 7

void getappdir(struct AppMessage*);
void draw3dbox(struct RastPort*,WORD,WORD,WORD,WORD,WORD);
void textextent(struct RastPort*,UBYTE*,WORD*,WORD*);
WORD getappicon(void);

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
extern struct FMList fmlist[];
extern struct Locale *locale;
extern UBYTE nformatstring2[];

static WORD nostring(void)
{
WORD cnt1;

for(cnt1=0;cnt1<LISTS;cnt1++) {
	if(fmlist[cnt1].li&&fmlist[cnt1].li->string.Flags&GACT_ACTIVEGADGET) return(0);
}
return(1);
}

void remappicon(void)
{
struct AppMessage *appmsg;

if(fmmain.appicon) {
	RemoveAppIcon(fmmain.appicon);
	fmmain.appicon=0;
}
if(fmmain.appiconport) {
	while((appmsg=(struct AppMessage *)GetMsg(fmmain.appiconport))) ReplyMsg((struct Message*)appmsg);
	DeleteMsgPort(fmmain.appiconport);
	fmmain.appiconport=0;
}
}
struct AppIcon *addappicon(void)
{
if(fmmain.appicon&&!(getappicon()&2)) {
	remappicon();
	return(0);
}
if(fmmain.appicon||!(getappicon()&2)) return(fmmain.appicon);
if(!fmmain.dobj) return(0);
remappicon();
fmmain.appiconport=CreateMsgPort();
if(fmmain.appiconport) {
	fmmain.dobj->do_Type=0;
	fmmain.dobj->do_CurrentX=-1;
	fmmain.dobj->do_CurrentY=-1;
	fmmain.appicon=AddAppIconA(0,0,"FM",fmmain.appiconport,0,fmmain.dobj,0);
	if(fmmain.appicon) return(fmmain.appicon);
}
remappicon();
return(0);
}

void clockformat(UBYTE *src)
{
LONG afast,achip;
struct DateStamp stamp;
UBYTE sfast[60],schip[60],stotal[60],sdate[60];

afast=AvailMem(MEMF_FAST); achip=AvailMem(MEMF_CHIP);
sformat(sfast,nformatstring2,afast);
sformat(schip,nformatstring2,achip);
sformat(stotal,nformatstring2,afast+achip);
DateStamp(&stamp);
longtodatestring(sdate,dstolong(&stamp));
sformatmsg(src,MSG_CLOCK_FORMATSTRING,sfast,schip,stotal,sdate);
}

void remsleep(struct Window *swin)
{
if(swin) {
	fmconfig->sleepwindowtop=swin->LeftEdge;
	fmconfig->sleepwindowleft=swin->TopEdge;
	CloseWindow(swin);
}
}

WORD setsleep(struct Window **pswin)
{
UBYTE clockstr[400];
WORD apu1,apu2;
struct Screen *wbscr=0;
struct Window *swin=0;

if(!(wbscr=LockPubScreen(0))) return(0);
clockformat(clockstr);
textextent(&wbscr->RastPort,clockstr,&apu1,&apu1);
addappicon();
if((getappicon()&1)||!fmmain.appiconport) {
	apu2=wbscr->Font->ta_YSize+wbscr->WBorTop+1;
	swin=OpenWindowTags(0,
			WA_Left,fmconfig->sleepwindowtop,
			WA_Top,fmconfig->sleepwindowleft,
			WA_MinHeight,apu2,WA_AutoAdjust,TRUE,
			WA_Width,apu1+8*8,WA_Height,apu2,
			WA_IDCMP,IDCMP_MOUSEBUTTONS|IDCMP_CLOSEWINDOW,
			WA_Flags,WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_RMBTRAP|WFLG_NOCAREREFRESH,
			TAG_DONE
	);
}
*pswin=swin;
UnlockPubScreen(0,wbscr);
if(!fmmain.appicon&&!swin) return(0);
return(1);
}

struct timerequest *gettimerequest(struct MsgPort **amp)
{
struct timerequest *tr;
struct MsgPort *mp;

if ((mp=CreateMsgPort())) {
	if ((tr=(struct timerequest*)CreateIORequest(mp,sizeof(struct timerequest)))) {
		*amp=mp;
		if (!OpenDevice("timer.device",UNIT_VBLANK,(struct IORequest*)tr,0)) return(tr);
		DeleteIORequest(&tr->tr_node);
	}
	DeleteMsgPort(mp);
}
return(0);
}

void __saveds fmclock(void)
{
struct ProcMsg *pm;
CxObj *cxmsg;
ULONG msgtype,msgid;
struct Task *task;
struct RastPort rp;
UBYTE clockstr[400];
WORD xx,len;
struct TextExtent txtext;
struct timerequest *tr;
struct MsgPort *tp;
ULONG sigmask,sleepsigmask,sigs,tpsig;
ULONG swinmask=0,saimask=0;
WORD timeri;
struct Window *swin=0;
struct IntuiMessage *imsg;
struct AppMessage *appiconmsg=0;
WORD sleepmode;
pm=sinitproc();
task=FindTask(0);
SetTaskPri(task,fmconfig->subpriority);
createbrokerall();
if(!(tr=gettimerequest(&tp))) goto error;
tpsig=1L<<tp->mp_SigBit;
sigmask=SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|tpsig;
if(fmmain.cxport) sigmask|=(1L<<fmmain.cxport->mp_SigBit);
fmmain.timeproc=(struct Process*)task;
addappicon();

memseti(clockstr,32,399);
InitRastPort(&rp);
SetFont(&rp,fmmain.txtfont);

xx=fmconfig->spaceh+fmmain.txtfont->tf_XSize;
timeri=0;sleepmode=0;sleepsigmask=0;
while(fmmain.clock) {

	addappicon();
	if(!timeri) {
		tr->tr_node.io_Command=TR_ADDREQUEST;
		tr->tr_time.tv_secs=0;
		tr->tr_time.tv_micro=500000;
		SendIO((struct IORequest*)tr);
		timeri=1;
	}

	sigs=Wait(sigmask|sleepsigmask|SIGBREAKF_CTRL_F);

	switch(sleepmode)
	{
	case 0:
	if(fmmain.appiconport) {
		appiconmsg=(struct AppMessage*)GetMsg(fmmain.appiconport);
		if(appiconmsg) {
			ScreenToFront(fmmain.naytto);
			if(fmmain.ikkuna) WindowToFront(fmmain.ikkuna);
			getappdir(appiconmsg);
			ReplyMsg((struct Message*)appiconmsg);
			appiconmsg=0;
		}
	}
	if(fmmain.sleepon==5||fmmain.sleepon==6) {
		sleepmode=STARTSLEEP;
		Signal((struct Task*)fmmain.timeproc,SIGBREAKF_CTRL_F);
	}
	break;
	case STARTSLEEP:
	if(!setsleep(&swin)) {
		sleepmode=QUITSLEEP;
	} else {
		sleepmode=SLEEPING;
		swinmask=0;saimask=0;
		if(swin) swinmask=1L<<swin->UserPort->mp_SigBit;
		if(fmmain.appiconport) saimask=1L<<fmmain.appiconport->mp_SigBit;
		sleepsigmask=swinmask|saimask;
	}
	break;
	case SLEEPING:
	if(saimask&&(sigs&saimask)) sleepmode=QUITSLEEP;
	if(swinmask&&(sigs&swinmask)) {
		while((imsg=(struct IntuiMessage*)GetMsg(swin->UserPort))) {
			if(imsg->Class==IDCMP_CLOSEWINDOW) {
				sleepmode=KILLSLEEP;
			}
			if(imsg->Class==IDCMP_MOUSEBUTTONS&&imsg->Code==MENUDOWN) sleepmode=QUITSLEEP;
			ReplyMsg((struct Message*)imsg);
		}
	}
	break;
	case QUITSLEEP:
	case KILLSLEEP:
	remsleep(swin);
	swin=0;
	if(sleepmode==KILLSLEEP) {
		fmmain.kill=1;
		fmmain.sleepon=0;
	} else {
		fmmain.sleepon=7;
	}
	sleepmode=0;
	Signal((struct Task*)fmmain.myproc,SIGBREAKF_CTRL_F);
	break;
	}

	if(sigs&SIGBREAKF_CTRL_C) {
		fmmain.kill=1;
		if(!sleepmode) Signal((struct Task*)fmmain.myproc,SIGBREAKF_CTRL_F);
	}

	if(sigs&SIGBREAKF_CTRL_D&&fmmain.broker) {
		DeleteCxObjAll(fmmain.broker);
		fmmain.broker=0;
		createbroker();
	}

	if(fmmain.cxport) {
		while((cxmsg=(CxMsg*)GetMsg(fmmain.cxport))) {
			msgtype=CxMsgID(cxmsg);
			msgid=CxMsgType(cxmsg);
			ReplyMsg((struct Message*)cxmsg);
			switch(msgid)
			{
			case CXM_IEVENT:
			if(!sleepmode&&fmmain.fmactive==3&&!fmmain.passhotkey&&nostring()) {
				fmmain.passhotkey=msgtype;
				Signal((struct Task*)fmmain.myproc,SIGBREAKF_CTRL_F);
			}
			break;
			case CXM_COMMAND:
				switch(msgtype)
				{
				case CXCMD_APPEAR:
				if(!sleepmode) {
					sleepmode=STARTSLEEP;
				} else {
					ScreenToFront(fmmain.naytto);
					WindowToFront(fmmain.ikkuna);
				}
				break;
				case CXCMD_DISAPPEAR:
				if(!sleepmode) Signal((struct Task*)fmmain.myproc,SIGBREAKF_CTRL_F);
				break;
				case CXCMD_KILL:
				fmmain.kill=1;
				if(!sleepmode) Signal((struct Task*)fmmain.myproc,SIGBREAKF_CTRL_F);
				break;
				case CXCMD_DISABLE:
				ActivateCxObj(fmmain.broker,FALSE);
				break;
				case CXCMD_ENABLE:
				ActivateCxObj(fmmain.broker,TRUE);
				break;
				}
			break;
			}
		}
	}

	if(CheckIO((struct IORequest*)tr)) {
		WaitIO((struct IORequest*)tr);
		SetSignal(0,tpsig);
		fmmain.timer++;
		timeri=0;
		if(fmmain.sourcedir->flags&LUPDATEMSG) fmmessage(fmmain.sourcedir);
		clockformat(clockstr);
		if(swin) {
			SetWindowTitles(swin,clockstr,(UBYTE*)-1);
		}
		if(fmmain.ikkuna&&AttemptSemaphore(&fmmain.gfxsema)) {
			len=TextFit(fmmain.rp,clockstr,strlen(clockstr),&txtext,0,1,fmmain.bottomlinewidth-2,32767);
			SetAPen(fmmain.rp,fmconfig->txtpen);
			Move(fmmain.rp,xx,fmmain.bottomliney+fmmain.txtbaseline);
			Text(fmmain.rp,clockstr,len);
			SetAPen(fmmain.rp,fmconfig->backpen);
			if(txtext.te_Width<fmmain.bottomlinewidth) RectFill(fmmain.rp,txtext.te_Width+xx,fmmain.bottomliney,fmmain.bottomlinewidth+xx-3,fmmain.bottomliney+fmmain.txtysize-1);
			ReleaseSemaphore(&fmmain.gfxsema);
		}
	}
}
if(timeri) {
	AbortIO((struct IORequest*)tr);
	WaitIO((struct IORequest*)tr);
}
error:
remsleep(swin);
removebroker();
remappicon();
closedevice((struct IORequest*)tr);
deinitproc(pm);
fmmain.clock=1;
}
