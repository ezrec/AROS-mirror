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
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <aros/debug.h>
#include <aros/bigendianio.h>
#include <graphics/gfxbase.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <devices/inputevent.h>
#include <libraries/gadtools.h>

#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/keymap.h>

#include <proto/bgui.h>
#include <string.h>
#include <stdarg.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "gadgets.h"

int GADGDISABLED, SELECTED, TOGGLESELECT, STRGADGET;

extern struct UtilityBase *UtilityBase;
extern struct Library *GadToolsBase;
extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *KeymapBase;

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
void textextent(struct RastPort*,UBYTE*,WORD*,WORD*);
void textextentuc(struct RastPort*,UBYTE*,WORD*,WORD*);
ULONG power2(WORD);
void supercheck(struct Window*,ULONG*,ULONG*,ULONG*);
void copyus(UBYTE*,UBYTE*);
void getwinposition(struct Screen*,struct NewWindow*,WORD,WORD,WORD);

static UBYTE gadnumbers[]={'0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0};

UBYTE *scanchar(UBYTE*,UBYTE);

#ifndef AROS
void draw3dbox(struct RastPort *rp,WORD x,WORD y,WORD w,WORD h,WORD type)
{
UBYTE col1,col2;

/* 1=raised 0=lowered */

SetAPen(rp,fmconfig->backpen);
RectFill(rp,x,y,x+w,y+h);
if (type) {
	col1=fmconfig->whitepen; col2=fmconfig->blackpen;
	} else {
	col1=fmconfig->blackpen; col2=fmconfig->whitepen;
}
x--;y--;w++;h++;
if (fmconfig->flags&MDOUBLED) { x--; w+=2; }
Move(rp,x,y);
SetAPen(rp,col1);
Draw(rp,x+w,y);
SetAPen(rp,col2);
Draw(rp,x+w,y+h);
Draw(rp,x,y+h);
SetAPen(rp,col1);
Draw(rp,x,y);
if (fmconfig->flags&MDOUBLED) {
	Move(rp,x+1,y);
	Draw(rp,x+1,y+h-1);
	SetAPen(rp,col2);
	Move(rp,x+w-1,y+1);
	Draw(rp,x+w-1,y+h);
}
}
#endif

void reqwindowtext(UBYTE *txt)
{
struct Window *window;
struct NewWindow newwindow;
struct RastPort rp;
WORD lines,apu1,apu2,quit,rows;
UBYTE *txtptr;
UBYTE title[40];
UBYTE extstring[100];
UBYTE *ptr1;
ULONG class;
UWORD code;
WORD trycnt;
WORD tfh,tfb;

if(SetSignal(0,0)&SIGBREAKF_CTRL_C) return;

InitRastPort(&rp);
trycnt=0;
gretry:
switch(trycnt) {
	case 0:
	SetFont(&rp,fmmain.listfont);
	tfh=fmmain.reqysize;
	tfb=fmmain.reqbaseline;
	break;
	case 1:
	SetFont(&rp,fmmain.smallfont);
	tfh=fmmain.smallfont->tf_YSize;
	tfb=fmmain.smallfont->tf_Baseline;
	break;
}
txtptr=txt;
ptr1=title;
while(*txtptr!='\n') *ptr1++=*txtptr++;
*ptr1=0; txtptr++;
rows=0; lines=0;
while(*txtptr) {
	ptr1=extstring;
	while(*txtptr!='\n') *ptr1++=*txtptr++;
	textextent(&rp,extstring,&apu1,&apu2);
	if (apu1>rows) rows=apu1;
	lines++;
	while(*txtptr++!='\n');
}

newwindow.Height=lines*(tfh+1)+fmmain.naytto->WBorTop+fmmain.naytto->Font->ta_YSize+1+2*fmconfig->spaceh;
newwindow.Width=rows+fmconfig->spacew*4;
newwindow.Flags=WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_RMBTRAP|WFLG_ACTIVATE,
newwindow.IDCMPFlags=IDCMP_CLOSEWINDOW|IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS,
newwindow.DetailPen=fmconfig->txtpen;
newwindow.BlockPen=fmconfig->hilipen;
newwindow.FirstGadget=0;
/* newwindow.CheckMark=0; */
newwindow.Title=title;
newwindow.Screen=fmmain.naytto;
newwindow.Type=CUSTOMSCREEN;
newwindow.BitMap=0;

if(newwindow.Height>fmmain.naytto->Height||newwindow.Width>fmmain.naytto->Width) {
	trycnt++;
	if(trycnt==1) goto gretry;
}
getwinposition(fmmain.naytto,&newwindow,0,newwindow.Width/2,newwindow.Height/2);

txtptr=txt;
while(*txtptr++!='\n');
if ((window=OpenWindow(&newwindow))) {
	SetAPen(window->RPort,fmconfig->backfillpen);
	RectFill(window->RPort,window->BorderLeft,window->BorderTop,window->Width-window->BorderRight-1,window->Height-window->BorderBottom-1);
	if(trycnt==0) SetFont(window->RPort,fmmain.listfont); else SetFont(window->RPort,fmmain.smallfont);
	SetAPen(window->RPort,fmconfig->txtpen);
	SetDrMd(window->RPort,JAM1);
	apu1=fmmain.naytto->WBorTop+fmmain.naytto->Font->ta_YSize+1+fmconfig->spaceh+tfb;
	while(lines--) {
		Move(window->RPort,2*fmconfig->spacew,apu1);
		Text(window->RPort,txtptr,scanchar(txtptr,'\n')-txtptr);
		while(*txtptr++!='\n');
		apu1+=tfh+1;
	}
	quit=-2;
	while (quit==-2) {
		if(smallevent(window,&class,&code)) quit=0;
		if(class==IDCMP_CLOSEWINDOW||class==IDCMP_VANILLAKEY||(class==IDCMP_MOUSEBUTTONS&&code==SELECTDOWN)) quit=0;
	}
	CloseWindow(window);
}
}

void getwinposition(struct Screen *scr,struct NewWindow *win,WORD flag,WORD apu1,WORD apu2)
{
WORD mousex,mousey;
struct Rectangle rect;
WORD dwidth,dheight,dxoffset,dyoffset;

QueryOverscan(GetVPModeID(&scr->ViewPort),&rect,OSCAN_TEXT);

dwidth=rect.MaxX-rect.MinX;
dheight=rect.MaxY-rect.MinY;
dxoffset=-scr->ViewPort.DxOffset;
dyoffset=-scr->ViewPort.DyOffset;

if(!flag) {
	mousex=scr->MouseX-apu1;
	mousey=scr->MouseY-apu2;
} else {
	mousex=(dwidth-win->Width)/2+dxoffset;
	mousey=(dheight-win->Height)/2+dyoffset;
}
if(mousex+win->Width>dwidth+dxoffset) mousex=dwidth+dxoffset-win->Width;
if(mousey+win->Height>dheight+dyoffset) mousey=dheight+dyoffset-win->Height;
if(mousex<dxoffset) mousex=dxoffset;
if(mousey<dyoffset) mousey=dyoffset;
if(mousex+win->Width>scr->Width) mousex=scr->Width-win->Width;
if(mousey+win->Height>scr->Height) mousey=scr->Height-win->Height;
if(mousex<0) mousex=0;
if(mousey<0) mousey=0;
win->LeftEdge=mousex;
win->TopEdge=mousey;
}

/*
taulu[0]==0 screen in taulu[1]
taulu[0]==1 center all requesters
taulu[0]==2 super list in taulu[1]

taulu[0]=window title
taulu[1]=#kind,taulu[2]=number of kind values taulu[3]=kind values....
taulu[..]=next #kind....
endofline=100,endoftaulu=101
*/


WORD reqwindow(ULONG *taulu)
{
struct NewWindow newwindow;
struct NewGadget ngad;
struct Window *window;
struct Gadget *glist,*gtgad,*vgad,*fstringgad;
struct IntuiMessage *message;
struct Screen *screen,*oldscreen;
struct RastPort rp;

UWORD tfh=0,tfw=0,spaceh=4,spacew=8;
WORD gadhnum;
WORD kplcnt1,kplcnt2;
WORD quit=0,gadgetnum,txoffset,maxgadw,valikoko=0;
WORD apu1,apu2,apu3,apu4=0,apu5,center=0;
struct IntString *is;
ULONG class;
UWORD code;
ULONG *tauluptr,*realtaulu,*super=0;
ULONG gadgettype;
UBYTE asciicode;
WORD txtlentab[72];		/* gadget widths */
WORD txtmaxlentab[64];		/* maximum widths (cleared) */
WORD rowwidth[64];
UBYTE shortcuts[72];
ULONG addressi[144];
ULONG tags[9];
ULONG array[64];
WORD arrayptr,retgadget;
UBYTE *vptr;
UBYTE chartab[2];
struct InputEvent ie;
ULONG ilock;
WORD opened=0;
WORD trycnt=0;
ULONG sigs;

if(SetSignal(0,0)&SIGBREAKF_CTRL_C) return(0);

memseti(txtmaxlentab,0,32*2);
ie.ie_Class=IECLASS_RAWKEY;
ie.ie_SubClass=0;
InitRastPort(&rp);
ilock=LockIBase(0);
oldscreen=IntuitionBase->FirstScreen;
UnlockIBase(ilock);
screen=fmmain.naytto;
while(*taulu<4) {
	switch(*taulu)
	{
	case GADSCREEN:
	screen=(struct Screen*)*(taulu+1);
	taulu+=2;
	break;
	case GADCENTER:
	center=1;
	taulu++;
	break;
	case GADSUPER:
	super=(ULONG*)*(taulu+1);
	taulu+=2;
	break;
	}
}
realtaulu=taulu;

gretry:
glist=0; gadhnum=0; fstringgad=0;
retgadget=-1;
window=0;
switch(trycnt) {
	case 0:
	SetFont(&rp,fmmain.reqfont);
	tfh=fmmain.reqysize;
	tfw=fmmain.reqxsize;
	valikoko=spaceh+2;
	break;
	case 1:
	valikoko=spaceh;
	break;
	case 2:
	SetFont(&rp,fmmain.smallfont);
	tfh=fmmain.smallfont->tf_YSize;
	tfw=fmmain.smallfont->tf_XSize;
	break;
}
kplcnt1=0;
tauluptr=realtaulu+1;
while(*tauluptr!=101) {
	while(*tauluptr!=100) {
		switch(*tauluptr) {
			case STRING_KIND:
			apu1=20;
			if(*(tauluptr+3)<20) apu1=*(tauluptr+3)+1;
			apu1=apu1*tfw+2*spacew;
			break;
			case INTEGER_KIND:
			apu1=tfw*8+2*spacew;
			break;
			case TEXT_KIND:
			if((*(tauluptr+1)&GADGTEXTNUC)==0)
				textextent(&rp,(UBYTE*)(*(tauluptr+2)),&apu1,&txoffset);
				else
				textextentuc(&rp,(UBYTE*)(*(tauluptr+2)),&apu1,&txoffset);
			break;
			case BUTTON_KIND:
			textextentuc(&rp,(UBYTE*)(*(tauluptr+2)),&apu1,&txoffset);
			apu1+=4*spacew;
			break;
			case CHECKBOX_KIND:
			apu1=32;
			break;
			case TOGGLE_KIND:
			textextentuc(&rp,(UBYTE*)(*(tauluptr+2)),&apu1,&txoffset);
			apu1+=2*spacew;
			break;
			case CYCLE_KIND:
			apu1=0;
			for(apu2=0;apu2<((*(tauluptr+1))&0xff)-1;apu2++) {
				textextentuc(&rp,(UBYTE*)(*(tauluptr+2+apu2)),&apu3,&txoffset);
				apu3+=8*spacew;
				if(apu3>apu1) apu1=apu3;
			}
			break;
			case PALETTE_KIND:
			apu1=power2(fmconfig->mainscreen.depth)*14+2*spacew;
			break;
		}
		txtlentab[kplcnt1++]=apu1;
		tauluptr+=((*(tauluptr+1))&0xff)+2;
	}
	txtlentab[kplcnt1++]=0;
	gadhnum++;
	tauluptr++;
}
txtlentab[kplcnt1]=-1;


kplcnt1=0;
tauluptr=realtaulu+1;
while(*tauluptr!=101) {
	while(*tauluptr!=100) {
		apu1=(*(tauluptr+1)>>16)&0xff;
		if(apu1) {
			if(txtlentab[kplcnt1]>txtmaxlentab[apu1]) txtmaxlentab[apu1]=txtlentab[kplcnt1];
		}
		kplcnt1++;
		tauluptr+=((*(tauluptr+1))&0xff)+2;
	}
	kplcnt1++;
	tauluptr++;
}
maxgadw=0;
kplcnt1=0;kplcnt2=0;
tauluptr=realtaulu+1;
while(*tauluptr!=101) {
	apu2=0;
	rowwidth[kplcnt2]=0;
	while(*tauluptr!=100) {
		apu1=(*(tauluptr+1)>>16)&0xff;
		if(apu1) {
			txtlentab[kplcnt1]=txtmaxlentab[apu1];
		}
		apu2+=txtlentab[kplcnt1];
		rowwidth[kplcnt2]+=txtlentab[kplcnt1];
		kplcnt1++;
		tauluptr+=((*(tauluptr+1))&0xff)+2;
	}
	if(apu2>maxgadw) maxgadw=apu2;
	kplcnt1++;
	tauluptr++;
	kplcnt2++;
}
do {
	apu2=0;
	kplcnt1=0; kplcnt2=0;
	tauluptr=realtaulu+1;
	while(*tauluptr!=101) {
		while(*tauluptr!=100) {
			if(*(tauluptr+1)&GADGMAXSIZE) {
				apu1=maxgadw-rowwidth[kplcnt2];
				if(apu1) {
					if(apu1>tfw) apu1=tfw;
					txtlentab[kplcnt1]+=apu1;
					rowwidth[kplcnt2]+=apu1;
					apu2=1;
				}
			}
			kplcnt1++;
			tauluptr+=((*(tauluptr+1))&0xff)+2;
		}
		kplcnt1++;
		kplcnt2++;
		tauluptr++;
	}
} while(apu2);

newwindow.Height=fmmain.naytto->WBorTop+fmmain.naytto->Font->ta_YSize+1+gadhnum*(tfh+valikoko)+2*spaceh;
newwindow.Width=maxgadw+3*spacew;
newwindow.Flags=WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_SMART_REFRESH|WFLG_ACTIVATE|WFLG_RMBTRAP,
newwindow.IDCMPFlags=IDCMP_REFRESHWINDOW|IDCMP_GADGETDOWN|IDCMP_GADGETUP|IDCMP_CLOSEWINDOW|IDCMP_ACTIVEWINDOW|IDCMP_RAWKEY,
newwindow.DetailPen=fmconfig->txtpen; newwindow.BlockPen=fmconfig->hilipen;
newwindow.FirstGadget=0;
/* newwindow.CheckMark=0; */
newwindow.Title=(UBYTE*)(*realtaulu);
newwindow.Screen=screen;
newwindow.Type=CUSTOMSCREEN;
newwindow.BitMap=0;

if(newwindow.Height>fmmain.naytto->Height||newwindow.Width>fmmain.naytto->Width) {
	trycnt++;
	if(trycnt<=2) goto gretry;
	if(trycnt==3) goto endreq;
}
if (!(gtgad=CreateContext(&glist))) goto endreq;

arrayptr=0;
gadhnum=fmmain.naytto->WBorTop+fmmain.naytto->Font->ta_YSize+1+spaceh;
kplcnt1=0; kplcnt2=0;
gadgetnum=0;
tauluptr=realtaulu+1;
while(txtlentab[kplcnt1]!=-1) {
	apu5=(maxgadw-rowwidth[kplcnt2]+1)/2+2*spacew;
	while(txtlentab[kplcnt1]) {
		gadgettype=*tauluptr;
		memseti(tags,0,9*4);
		switch(gadgettype) {
			case INTEGER_KIND:
			tags[0]=GTIN_Number;
			tags[1]=((struct IntString*)*(tauluptr+2))->value;
			tags[2]=STRINGA_Justification;
			tags[3]=GACT_STRINGCENTER;
			break;
			case STRING_KIND:
			tags[0]=GTST_String;
			tags[1]=*(tauluptr+2);
			tags[2]=GTST_MaxChars;
			tags[3]=*(tauluptr+3);
			tags[4]=STRINGA_Justification;
			tags[5]=GACT_STRINGCENTER;
			break;
			case TEXT_KIND:
			if((*(tauluptr+1)&GADGTEXTNUC)==0) {
				tags[0]=GT_Underscore;
				tags[1]='_';
			}
			vptr=scanchar((UBYTE*)*(tauluptr+2),'_');
			shortcuts[gadgetnum]=0;
			if(vptr) shortcuts[gadgetnum]=ToUpper(*(vptr+1));
			break;
			case CHECKBOX_KIND:
			tags[0]=GT_Underscore;
			tags[1]='_';
			tags[2]=GTCB_Scaled;
			tags[3]=1;
			vptr=scanchar((UBYTE*)*(tauluptr+2),'_');
			shortcuts[gadgetnum]=0;
			if(vptr) shortcuts[gadgetnum]=ToUpper(*(vptr+1));
			break;
			case TOGGLE_KIND:
			case BUTTON_KIND:
			tags[0]=GT_Underscore;
			tags[1]='_';
			vptr=scanchar((UBYTE*)*(tauluptr+2),'_');
			shortcuts[gadgetnum]=0;
			if(vptr) shortcuts[gadgetnum]=ToUpper(*(vptr+1));
			break;
			case CYCLE_KIND:
			tags[0]=GTCY_Labels;
			tags[1]=(ULONG)&array[arrayptr];
			tags[2]=GTCY_Active;
			tags[3]=*(tauluptr+((*(tauluptr+1))&0xff)+1);
			for(apu1=0;apu1<((*(tauluptr+1))&0xff)-1;apu1++) {
				array[arrayptr++]=*(tauluptr+2+apu1);
			}
			array[arrayptr++]=0;
			break;
			case PALETTE_KIND:
			tags[0]=GTPA_Depth;
			tags[1]=fmconfig->mainscreen.depth;
			tags[2]=GTPA_Color;
			*(tauluptr+2)&=(power2(fmconfig->mainscreen.depth)-1);
			tags[3]=*(tauluptr+2);
			tags[4]=GTPA_IndicatorWidth;
			tags[5]=16;
			tags[6]=GTPA_IndicatorHeight;
			tags[7]=tfh+5;
			break;
		}
		ngad.ng_TopEdge=apu1=gadhnum;
		ngad.ng_LeftEdge=apu2=apu5;
		ngad.ng_Height=apu3=tfh+spaceh;
		ngad.ng_Width=apu4=txtlentab[kplcnt1]-spacew;
		if (gadgettype!=TEXT_KIND) {
			ngad.ng_GadgetID=shortcuts[gadgetnum];
			if((*(tauluptr+1))>>24) ngad.ng_GadgetID=((*(tauluptr+1))>>24)|0x100;
		} else {
			ngad.ng_GadgetID=0;
			if((*(tauluptr+1))&(GADGTEXTRIGHT|GADGTEXTLEFT)) {
				textextentuc(&rp,(UBYTE*)(*(tauluptr+2)),&ngad.ng_Width,&txoffset);
				if((*(tauluptr+1))&GADGTEXTRIGHT) ngad.ng_LeftEdge+=apu4-ngad.ng_Width;
			}
		}
		if (trycnt<=1) ngad.ng_TextAttr=&fmconfig->reqfontattr; else ngad.ng_TextAttr=&fmconfig->smallfontattr;
		if(gadgettype==BUTTON_KIND&&(*(tauluptr+1))&GADGDEFAULT) {
			ngad.ng_TextAttr->ta_Style=FSF_BOLD;
			retgadget=ngad.ng_GadgetID;
		}
		ngad.ng_UserData=0;
		if (gadgettype==TEXT_KIND||gadgettype==BUTTON_KIND||gadgettype==TOGGLE_KIND) {
			ngad.ng_GadgetText=(UBYTE*)(*(tauluptr+2));
		} else {
			ngad.ng_GadgetText=0;
		}
		ngad.ng_Flags=PLACETEXT_IN;
		ngad.ng_VisualInfo=fmmain.gtvisual;
		if(gadgettype==TOGGLE_KIND) {
			if(!(gtgad=CreateGadgetA(BUTTON_KIND,gtgad,&ngad,(struct TagItem*)tags))) goto endreq;
			gtgad->Activation|=TOGGLESELECT;
			if (*(tauluptr+3)) gtgad->Flags|=SELECTED;
		} else {
			if(!(gtgad=CreateGadgetA(gadgettype,gtgad,&ngad,(struct TagItem*)tags))) goto endreq;
		}
		ngad.ng_TextAttr->ta_Style=0;
		addressi[gadgetnum*2]=(ULONG)tauluptr;
		addressi[gadgetnum*2+1]=(ULONG)gtgad;
		if(gadgettype!=TEXT_KIND) gadgetnum++;
		if ((gadgettype==STRING_KIND||gadgettype==INTEGER_KIND)&&!fstringgad) fstringgad=gtgad;
		tauluptr+=((*(tauluptr+1))&0xff)+2;
		apu5+=txtlentab[kplcnt1++];
	}
	gadhnum+=tfh+valikoko;
	tauluptr++;
	kplcnt1++;
	kplcnt2++;
}

getwinposition(screen,&newwindow,center,apu2+apu4/2,apu1+apu3/2);
if (!(window=OpenWindowTagList(&newwindow,0))) goto endreq;

SetAPen(window->RPort,fmconfig->backfillpen);
RectFill(window->RPort,window->BorderLeft,window->BorderTop,window->Width-window->BorderRight-1,window->Height-window->BorderBottom-1);
/*
SetAPen(window->RPort,fmconfig->backpen);
gadhnum=fmmain.naytto->WBorTop+fmmain.naytto->Font->ta_YSize+1+spaceh;
kplcnt1=0; kplcnt2=0;
while(txtlentab[kplcnt1]!=-1) {
	apu5=(maxgadw-rowwidth[kplcnt2]+1)/2+2*spacew;
	while(txtlentab[kplcnt1]) {
		RectFill(window->RPort,apu5,gadhnum,txtlentab[kplcnt1]-spacew+apu5-1,tfh+spaceh+gadhnum-1);
		apu5+=txtlentab[kplcnt1++];
	}
	gadhnum+=tfh+valikoko;
	kplcnt1++;
	kplcnt2++;
}
*/

if(trycnt==2) SetFont(window->RPort,fmmain.smallfont);

//fmconfig->dosbuffersize|=(hcsum&0xff000000);

ScreenToFront(screen);
AddGList(window,glist,-1,-1,0);
RefreshGList(glist,window,0,-1);
GT_RefreshWindow(window,0);
supercheck(window,super,taulu,addressi);
opened=1;

quit=-2;
while (quit==-2) {
	while(!(message=GT_GetIMsg(window->UserPort))) {
			sigs=Wait((1L<<window->UserPort->mp_SigBit)|SIGBREAKF_CTRL_C);
			if(sigs&SIGBREAKF_CTRL_C) {
				SetSignal(SIGBREAKF_CTRL_C,SIGBREAKF_CTRL_C);
				quit=0;
				continue;
			}
	}
	class=message->Class;
	code=message->Code;
	switch(class) {
		case IDCMP_ACTIVEWINDOW:
		if (fstringgad&&!(fstringgad->Flags&GADGDISABLED))  ActivateGadget(fstringgad,window,0);
		break;
		case IDCMP_RAWKEY:
		if(code&0x80) break;
		ie.ie_Code=code;
		ie.ie_Qualifier=message->Qualifier;
		ie.ie_EventAddress=(APTR*)*((ULONG*)message->IAddress);
		if (MapRawKey(&ie,chartab,2,0)==1) asciicode=ToUpper(chartab[0]); else break;

		if(asciicode==27) {
			quit=0;
			break;
		}
		for(apu1=0;apu1<gadgetnum;apu1++) {
			if(shortcuts[apu1]==asciicode||(asciicode==13&&retgadget==shortcuts[apu1])) {
				asciicode=shortcuts[apu1];
				tauluptr=(ULONG*)addressi[apu1*2];
				vgad=(struct Gadget*)addressi[apu1*2+1];
				if(*tauluptr!=BUTTON_KIND&&(vgad->Flags&GADGDISABLED)) break;
				switch(*tauluptr)
				{
					case STRING_KIND:
					case INTEGER_KIND:
					if (vgad->Flags&GADGDISABLED) {
						fstringgad=vgad;
						ActivateGadget(fstringgad,window,0);
					}
					break;
					case BUTTON_KIND:
					quit=asciicode;
					break;
					case TOGGLE_KIND:
					RemoveGadget(window,vgad);
					vgad->Flags^=SELECTED;				
					if (vgad->Flags&SELECTED) *(tauluptr+3)=1; else *(tauluptr+3)=0;
					AddGadget(window,vgad,-1);
					RefreshGList(vgad,window,0,1);
					break;
					case CYCLE_KIND:
					apu1=*(tauluptr+((*(tauluptr+1))&0xff)+1);
					if(message->Qualifier&0x03)
						apu1--;
						else
						apu1++;
					if(apu1<0) apu1=((*(tauluptr+1))&0xff)-2;
					if(apu1>=((*(tauluptr+1))&0xff)-1) apu1=0;
					GT_SetGadgetAttrs(vgad,window,0,GTCY_Active,apu1,TAG_DONE);
					*(tauluptr+((*(tauluptr+1))&0xff)+1)=apu1;
					break;
					case PALETTE_KIND:
					apu1=*(tauluptr+2);
					if(message->Qualifier&0x03)
						apu1--;
						else
						apu1++;
					apu1&=(power2(fmconfig->mainscreen.depth)-1);
					GT_SetGadgetAttrs(vgad,window,0,GTPA_Color,apu1,TAG_DONE);
					*(tauluptr+2)=apu1;
					break;
				}
				supercheck(window,super,taulu,addressi);
				break;
			}
		}
		break;
		case IDCMP_REFRESHWINDOW:
		GT_BeginRefresh(window);
		GT_EndRefresh(window,1);
		break;
		case IDCMP_CLOSEWINDOW:
		quit=0;
		break;
		case IDCMP_GADGETUP:		
		vgad=(struct Gadget*)message->IAddress;
		for(apu1=0;apu1<gadgetnum;apu1++) {
			if(addressi[apu1*2+1]==(ULONG)vgad) {
				tauluptr=(ULONG*)addressi[apu1*2];
				switch(*tauluptr)
				{
					case INTEGER_KIND:
					is=((struct IntString*)(*(tauluptr+2)));
					is->value=((struct StringInfo*)vgad->SpecialInfo)->LongInt;
					if(is->value<is->min) is->value=is->min;
					if(is->value>is->max) is->value=is->max;
					((struct StringInfo*)vgad->SpecialInfo)->LongInt=is->value;
					sformat(((struct StringInfo*)vgad->SpecialInfo)->Buffer,"%ld",is->value);
					RefreshGList(vgad,window,0,1);
					case STRING_KIND:
					if(code!=0x09) quit=retgadget;
					break;
/*
					apu2=apu1;
					for(;;) {
						if (message->Qualifier&0x03) apu2--; else apu2++;
						if(apu2>=gadgetnum) {
							apu2=apu1;
							break;
						}
						if(apu2<0) apu2=gadgetnum-1;
						if(*((ULONG*)addressi[apu2*2])==INTEGER_KIND||*((ULONG*)addressi[apu2*2])==STRING_KIND) {
							fstringgad=(struct Gadget*)addressi[apu2*2+1];
							ActivateGadget(fstringgad,window,0);
							break;
						}
					}
					if(apu2==apu1) quit=retgadget;
*/
					case TOGGLE_KIND:
					if (*(tauluptr+3)) *(tauluptr+3)=0; else *(tauluptr+3)=1;
					break;
					case BUTTON_KIND:
					quit=vgad->GadgetID; /* shortcuts[apu1]; */
					break;
					case CYCLE_KIND:
					*(tauluptr+((*(tauluptr+1))&0xff)+1)=code;
					break;
					case PALETTE_KIND:
					*(tauluptr+2)=code;
					break;
				}
				supercheck(window,super,taulu,addressi);
				if(fstringgad&&!(fstringgad->Flags&GADGDISABLED)) ActivateGadget(fstringgad,window,0);
				break;
			}
		}
		break;
	}
	GT_ReplyIMsg(message);

}

for(apu1=0;apu1<gadgetnum;apu1++) {
	vgad=(struct Gadget*)addressi[apu1*2+1];
	tauluptr=(ULONG*)addressi[apu1*2];
	switch(*tauluptr) {
		case STRING_KIND:
		memseti((void*)*(tauluptr+2),0,*(tauluptr+3));
		strcpy((UBYTE*)*(tauluptr+2),(UBYTE*)(((struct StringInfo*)vgad->SpecialInfo)->Buffer));
		break;
		case INTEGER_KIND:
		is=((struct IntString*)(*(tauluptr+2)));
		is->value=((struct StringInfo*)vgad->SpecialInfo)->LongInt;
		if(is->value<is->min) is->value=is->min;
		if(is->value>is->max) is->value=is->max;
		break;
	}
}

endreq:
ilock=LockIBase(0);
if (IntuitionBase->FirstScreen==screen) {
	UnlockIBase(ilock);
	ScreenToFront(oldscreen);
} else {
	UnlockIBase(ilock);
}
if (window) CloseWindow(window);
if (glist) FreeGadgets(glist);
if(!opened) DisplayBeep(screen);
return(quit);
}

WORD pressgadget(struct Window *win,struct Gadget *gad)
{
WORD pos;

if(gad<(struct Gadget*)4) return(1);

if(!(gad->Activation&TOGGLESELECT)&&!(gad->Flags&GADGDISABLED)) {
	pos=RemoveGadget(win,gad);
	gad->Flags|=SELECTED;
	gad->Activation|=TOGGLESELECT;
	AddGadget(win,gad,pos);
	RefreshGList(gad,win,0,1);
	pos=RemoveGadget(win,gad);
	gad->Flags|=GADGDISABLED;
	AddGadget(win,gad,pos);
	return(1);
}
return(0);
}
void unpressgadget(struct Window *win,struct Gadget *gad)
{
WORD pos;

if(gad<(struct Gadget*)4) return;
if(gad->Activation&TOGGLESELECT) {
	pos=RemoveGadget(win,gad);
	gad->Flags&=~(SELECTED|GADGDISABLED);
	gad->Activation&=~TOGGLESELECT;
	AddGadget(win,gad,pos);
	RefreshGList(gad,win,0,1);
}
}

UBYTE *scanchar(UBYTE *ptr,UBYTE ch)
{
while(*ptr) {
	if (*ptr==ch) return(ptr);
	ptr++;
}
return(0);
}

#ifndef AROS
void textextent(struct RastPort *rp,UBYTE *ptr,WORD *width,WORD *offset)
{
struct TextExtent te;

TextExtent(rp,ptr,strlen(ptr),&te);
*offset=te.te_Extent.MinX;
*width=te.te_Extent.MaxX+te.te_Extent.MinX;
}
void textextentuc(struct RastPort *rp,UBYTE *ptr,WORD *width,WORD *offset)
{
struct TextExtent te;
UBYTE varaptr[100];

copyus(varaptr,ptr);
TextExtent(rp,varaptr,strlen(varaptr),&te);
*width=te.te_Extent.MaxX+te.te_Extent.MinX;
*offset=te.te_Extent.MinX;
}
#endif

void dotext(WORD *c,ULONG *t,WORD msg,ULONG type)
{
t+=*c;
*t++=TEXT_KIND;
*t++=1|(type&0xffffff00);
*t++=(ULONG)getstring(msg);
(*c)+=3;
if(type&1) { *t=100; (*c)++; }
}
void dostring(WORD *c,ULONG *t,UBYTE *msg,ULONG type)
{
UBYTE *ptr;
ULONG *tt;

tt=t;
t+=*c;
*t++=TEXT_KIND;
*t++=1|(type&0xffffff00);
*t++=(ULONG)msg;
(*c)+=3;
if(type&1) { *t=100; (*c)++; }
ptr=scanchar(msg,'\n');
if(ptr) {
	*ptr++=0;
	dostring(c,tt,ptr,type);
}
}
void dobutton(WORD *c,ULONG *t,WORD msg,ULONG type)
{
dobuttonstring(c,t,getstring(msg),type);
}
void dobuttonstring(WORD *c,ULONG *t,UBYTE *msg,ULONG type)
{
t+=*c;
*t++=BUTTON_KIND;
*t++=1|(type&0xffffff00);
*t++=(ULONG)msg;
(*c)+=3;
if(type&GADGEOF) { *t=100; (*c)++; }
}
void doletterbutton(WORD *c,ULONG *t,WORD msg,WORD letter,ULONG type)
{
donumbuttonstring(c,t,getstring(msg),letter+'A'-'0',type);
}
void doletterbuttonstring(WORD *c,ULONG *t,UBYTE *msg,WORD letter,ULONG type)
{
donumbuttonstring(c,t,msg,letter+'A'-'0',type);
}
void donumbutton(WORD *c,ULONG *t,WORD msg,WORD num,ULONG type)
{
donumbuttonstring(c,t,getstring(msg),num,type);
}
void donumbuttonstring(WORD *c,ULONG *t,UBYTE *msg,WORD num,ULONG type)
{
UBYTE *txt;

t+=*c;
*t++=BUTTON_KIND;
*t++=15L<<16|2;
*t=(ULONG)(t+1);
t++;
txt=(UBYTE*)t++;
*txt++='_';
*txt++=num+'0';
*txt=0;

*t++=BUTTON_KIND;
*t++=(LONG)(num+'0')<<24|14L<<16|1|GADGTEXTNUC|(type&0xffffff00);
*t++=(ULONG)msg;

(*c)+=7;
if(type&GADGEOF) { *t=100; (*c)++; }
}


ULONG *docyclenumber(WORD *c,ULONG *t,ULONG type,WORD sel,WORD s,WORD e)
{
WORD cnt,num;
ULONG *dat;

num=e-s+1;
t+=*c;
*t++=CYCLE_KIND;
*t++=(num+1)|(type&0xffffff00);
for(cnt=s;cnt<=e;cnt++) *t++=(ULONG)(&gadnumbers[cnt*2]);
dat=t;
if(sel>=num) sel=num-1;
*t++=sel;
(*c)+=3+num;
if(type&GADGEOF) { *t=100; (*c)++; }
return(dat);
}

ULONG *docyclestring(WORD *c,ULONG *t,WORD num,ULONG type,WORD sel,UBYTE *txt1,...)
{
WORD cnt;
ULONG *dat;

t+=*c;
*t++=CYCLE_KIND;
*t++=(num+1)|(type&0xffffff00);
for(cnt=0;cnt<num;cnt++) *t++=(ULONG)(*(&txt1+cnt));
dat=t;
if(sel>=num) sel=num-1;
*t++=sel;
(*c)+=3+num;
if(type&GADGEOF) { *t=100; (*c)++; }
return(dat);
}

ULONG *docycle(WORD *c,ULONG *t,WORD num,ULONG type,WORD sel,WORD msg1,...)
{
WORD cnt;
ULONG *dat;

t+=*c;
*t++=CYCLE_KIND;
*t++=(num+1)|(type&0xffffff00);
for(cnt=0;cnt<num;cnt++) *t++=(ULONG)getstring(*(&msg1+cnt));
dat=t;
if(sel>=num) sel=num-1;
*t++=sel;
(*c)+=3+num;
if(type&GADGEOF) { *t=100; (*c)++; }
return(dat);
}
ULONG *doswitch(WORD *c,ULONG *t,WORD msg,WORD num,ULONG type)
{
return(doswitchstring(c,t,getstring(msg),num,type));
}
ULONG *doswitchstring(WORD *c,ULONG *t,UBYTE *msg,WORD num,ULONG type)
{
ULONG *dat;

if(num) num=1;
t+=*c;
*t++=TOGGLE_KIND;
*t++=2;
*t++=(ULONG)msg;
dat=t;
*t++=num;
(*c)+=4;
if(type&GADGEOF) { *t=100; (*c)++; }
return(dat);
}
ULONG *dotoggle(WORD *c,ULONG *t,WORD msg,WORD num,ULONG type)
{
return(dotogglestring(c,t,getstring(msg),num,type));
}
ULONG *dotogglestring(WORD *c,ULONG *t,UBYTE *msg,WORD num,ULONG type)
{
ULONG *dat;
WORD numi=num?1:0;

dostring(c,t,msg,(type&0xffffff00)|GADGTEXTRIGHT);
dat=docycle(c,t,2,(type&1)|GADGMAXSIZE,numi,MSG_NON,MSG_YESN);
return(dat);
}

ULONG *dointegergad(WORD *c,ULONG *t,struct IntString *is,ULONG type)
{
ULONG *dat;

t+=*c;
*t++=INTEGER_KIND;
*t++=1|(type&0xffffff00);
dat=t;
*t++=(ULONG)is;
(*c)+=3;
if(type&GADGEOF) { *t=100; (*c)++; }
return(dat);
}
void dostringgad(WORD *c,ULONG *t,UBYTE *string,WORD maxlen,ULONG type)
{
t+=*c;
*t++=STRING_KIND;
*t++=2|(type&0xffffff00);
*t++=(ULONG)string;
*t++=maxlen;
(*c)+=4;
if(type&GADGEOF) { *t=100; (*c)++; }
}

/*

1.ulong gadgetnum<<16 | gadget value <<8 | number of modified gadgets

number of modified gadgets amount of ulongs
2.ulong
	bit 31, 1=disable,0=enable
	bit 30, 1=change gadget value,0=don't change
	bits 15-8 new gadget value  (only if bit 30 = 1)
	bits 0-7 gadget number to be set
3.ulong -- : --

*/

void supercheck(struct Window *win,ULONG *super,ULONG *taulu,ULONG *addressi)
{
struct Gadget *vgad;
ULONG *tauluptr;
WORD gadnum,gadvalue,listnum;
WORD cnt=0,apu1,apu2;
UBYTE selected,sel2;
ULONG lapu;

while(super&&super[cnt]) {
	selected=0;
	lapu=super[cnt++];
	gadnum=(lapu>>16)&0xff;
	gadvalue=(lapu>>8)&0xff;
	sel2=gadvalue&0x80; gadvalue&=~0x80;
	listnum=(lapu)&0xff;
	tauluptr=(ULONG*)addressi[gadnum*2];
	vgad=(struct Gadget*)addressi[gadnum*2+1];
	switch(*tauluptr)
	{
	case TOGGLE_KIND:
	if((vgad->Flags&SELECTED&&gadvalue)||(!(vgad->Flags&SELECTED)&&!gadvalue)) selected=1;
	break;
	case CYCLE_KIND:
	if (*(tauluptr+((*(tauluptr+1))&0xff)+1)==gadvalue) selected=1;
	break;
	case PALETTE_KIND:
	if (*(tauluptr+2)==gadvalue) selected=1;
	break;
	}
	if((selected&&!sel2)||(!selected&&sel2)) {
		for(apu1=0;apu1<listnum;apu1++) {
			lapu=super[cnt++];
			gadnum=lapu&0xff;
			apu2=(lapu>>8)&0xff;
			vgad=(struct Gadget*)addressi[gadnum*2+1];
			tauluptr=(ULONG*)addressi[gadnum*2];
			if(lapu&SDISABLE && !(vgad->Flags&GADGDISABLED)) {
				RemoveGadget(win,vgad);
				vgad->Flags|=GADGDISABLED;
				AddGadget(win,vgad,-1);
				RefreshGList(vgad,win,0,1);
			} else if(!(lapu&SDISABLE) && (vgad->Flags&GADGDISABLED)) {
				RemoveGadget(win,vgad);
				vgad->Flags&=~GADGDISABLED;
				AddGadget(win,vgad,-1);
				RefreshGList(vgad,win,0,1);
				if((vgad->GadgetType&7)==STRGADGET) ActivateGadget(vgad,win,0);
			}
			if(lapu&SCHANGE) {
				switch(*tauluptr)
				{
				case TOGGLE_KIND:
				if(*(tauluptr+3)!=apu2) {
					RemoveGadget(win,vgad);
					*(tauluptr+3)=apu2;
					if(apu2) vgad->Flags|=SELECTED; else vgad->Flags&=~SELECTED;
					AddGadget(win,vgad,-1);
					RefreshGList(vgad,win,0,1);
				}
				break;
				case CYCLE_KIND:
				if(*(tauluptr+((*(tauluptr+1))&0xff)+1)!=apu2) {
					*(tauluptr+((*(tauluptr+1))&0xff)+1)=apu2;
					GT_SetGadgetAttrs(vgad,win,0,GTCY_Active,apu2,TAG_DONE);
				}
				break;
				case PALETTE_KIND:
				if(*(tauluptr+2)!=apu2) {
					*(tauluptr+2)=apu2;
					GT_SetGadgetAttrs(vgad,win,0,GTPA_Color,apu2,TAG_DONE);
				}
				break;
				}
			}
		}
	} else {
		cnt+=listnum;
	}
}
}

WORD simplebguirequest(Object *o)
{
struct Window *w;
ULONG signal,sigs,rc;
WORD ret=0;

if(o) {
	w=WindowOpen(o);
	if(w) {
		ret=-1;
		GetAttr(WINDOW_SigMask,o,&signal);
		while(ret==-1) {
			sigs=Wait(signal|SIGBREAKF_CTRL_C);
			if(sigs&SIGBREAKF_CTRL_C) ret=0;
			while((rc=HandleEvent(o))!=WMHI_NOMORE) {
				if(rc==WMHI_CLOSEWINDOW) ret=0;
				if(rc>=20&&rc<=30) ret=rc-19;
			}						
		}
	}
	DisposeObject(o);
}
return(ret);
}


