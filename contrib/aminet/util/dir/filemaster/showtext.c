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
#include <graphics/gfxbase.h>
#include <dos/dos.h>

#include <stdio.h>
#include <string.h>

#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

struct ShowTxt {
	unsigned showinfo:2;
};
extern struct FMConfig *fmconfig;
extern struct TextFont *txtshowfont;
extern struct FMMain fmmain;

#ifdef AROS
WORD hexconvert(UBYTE*,UBYTE*,WORD,WORD);
#else
WORD __asm hexconvert(register __a0 UBYTE*,register __a1 UBYTE*,register __d0 WORD,register __d1 WORD);
#endif

struct STL;

void showascfile(void);
void showhexfile(void);
WORD showtext(struct FMHandle*,struct CMenuConfig*);
void getjump(struct STL*);
void textsearch(struct STL*,WORD);
void setreqstate(struct STL*);
void resetreqstate(struct STL*);
void convertstring(struct STL*,UBYTE*,UBYTE**,WORD);
void escmode(struct STL*,UBYTE**,UBYTE**,WORD*);
void strtext(struct STL*,struct RastPort*,UBYTE*);
void escstyle(UBYTE**,UBYTE*,UBYTE,UBYTE*,UBYTE,UBYTE*,UBYTE);

void __saveds showtextfile(void)
{
struct ProcMsg *pm;
struct FMList *slist;
struct FMNode *node;
struct FMHandle *h;
WORD ret=0;

pm=sinitproc();
slist=fmmain.sourcedir;
if (!(setalloc(slist,1))) {
	initproc(0,0); 
	goto endi;
}
initproc(slist,pm->cmc->label);
priority(pm->cmc);
if(sselected(slist,1)) {
	while((node=findselnode(slist))) {
		if ((h=openfile(slist,NDFILE(node),OFNORMAL|OFDECRUNCH))) {
			ret=showtext(h,pm->cmc);
			closefile(h);
		}
		node->flags&=~NSELECTED;
		outputlistline(slist,node);
		if (!ret) break;
	}
}
endproc(slist);
endi:
deinitproc(pm);
}

struct STN {
	struct STN *succ;
	struct STN *pred;
	LONG databytes;
	UBYTE buffer;
};
struct STL {
	struct STN *head;
	struct STN *tail;
	struct STN *tailpred;
	struct FMHandle *handle;
	LONG lines;		/* loaded lines */
	LONG bytes;		/* loaded bytes */
	LONG remain;		/* remaining bytes */
	LONG topline;
	WORD numlines;		/* screen lines */
	WORD numrows;		/* screen rows */
	struct Screen *screen;
	struct Window *window;
	struct BitMap *bitmap;
	struct BitMap *vbitmap;
	struct BitMap *cbitmap;
	WORD flag;
	WORD scroll;
	LONG pointer1;		/* downscroll pointer */
	struct STN *usenode1;
	LONG pointer2;		/* upscroll pointer */
	struct STN *usenode2;
	UBYTE *buffer;
	UBYTE *ascbuffer;
	WORD height;		/* total height */
	WORD rows;		/* number of characters per line */
	WORD hexrows;		/* =rows/3-1; */
	WORD scrollvalue;
	WORD xx;		/* start x */
	BYTE loaded;
	BYTE newsearch;
	struct ShowTxt *config;
	UBYTE style;
	UBYTE color1,color2;
	UBYTE searchstring[32];
	UBYTE firstinfo;
};

WORD loadmore(struct FMHandle*,struct STL*);
void textscrollup(struct STL*,WORD);
void textscrolldown(struct STL*,WORD);
WORD textup(struct STL*);
WORD textdown(struct STL*);
void infoblock(struct STL*,WORD,WORD);

WORD showtext(struct FMHandle *h,struct CMenuConfig *cmc)
{
struct STL stl;
struct BitMap *bitmap=0,*vbitmap=0,*cbitmap=0;
struct FMList *list;
struct STN *stn;
struct IntuiMessage *im;
struct InputEvent ie;

WORD err=0, outputted=0;
WORD apu1;
#ifndef V39
WORD apu2;
#endif
WORD quit=0,mode=0,omode=0,ignorebutton=0,started=1,keypressed=0;
UBYTE chartab[2];
ULONG class;
UWORD code;
UWORD qualifier = 0;
ULONG point;
WORD penlist[2];

penlist[0]=-1;penlist[1]=-1;
list=h->owner;
ie.ie_Class=IECLASS_RAWKEY;
ie.ie_SubClass=0;
priority(cmc);
changename(list,cmc->label);
memseti(&stl,0,sizeof(struct STL));
NewList((struct List*)&stl);

if(cmc->cmenucount==SHOWASCCONFIG) {
	sformat(list->fmmessage1,getstring(MSG_FILECLICK_ASC),h->filename);
	stl.flag=1;
} else {
	sformat(list->fmmessage1,getstring(MSG_FILECLICK_HEX),h->filename);
}
fmmessage(list);
stl.color1=1;
stl.remain=h->size;
fmconfig->textscreen.height=(fmconfig->textscreen.height/fmmain.txtshowysize)*fmmain.txtshowysize;
stl.numlines=fmconfig->textscreen.height/fmmain.txtshowysize;
stl.numrows=fmconfig->textscreen.width/fmmain.txtshowxsize;
stl.scroll=fmmain.txtshowysize; stl.height=fmconfig->textscreen.height*2+3*fmmain.txtshowysize;
stl.scrollvalue=1; stl.handle=h; stl.config=getconfig(cmc);
if(!stl.config) goto sterr;
if(!(stl.ascbuffer=allocvec(h->owner,h->bufsize*4,0))) goto sterr;
stl.rows=stl.numrows;
if (stl.flag) {
	stl.xx=0;
} else {
	stl.hexrows=stl.numrows/3-1;
	if(stl.hexrows&1) stl.hexrows--;
	stl.xx=(stl.numrows-stl.hexrows*3+1)/2*fmmain.txtshowxsize;
}
if (!(stl.buffer=allocvec(h->owner,stl.numrows,0))) goto sterr;

#ifdef V39
if(!(bitmap=AllocBitMap(fmconfig->textscreen.width,stl.height,fmconfig->textscreen.depth,BMF_CLEAR|BMF_DISPLAYABLE,0))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_ERRNOBITMAP);
	goto sterr;
}
if(!(vbitmap=AllocBitMap(fmconfig->textscreen.width,fmmain.txtshowysize,fmconfig->textscreen.depth,BMF_CLEAR,0))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_ERRNOBITMAP);
	goto sterr;
}
if(!(cbitmap=AllocBitMap(fmconfig->textscreen.width,fmconfig->textscreen.height,fmconfig->textscreen.depth,BMF_CLEAR,0))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_ERRNOBITMAP);
	goto sterr;
}
#else
if (!(bitmap=allocvec(h->owner,sizeof(struct BitMap),0))) goto sterr;
if (!(vbitmap=allocvec(h->owner,sizeof(struct BitMap),0))) goto sterr;
if (!(cbitmap=allocvec(h->owner,sizeof(struct BitMap),0))) goto sterr;
InitBitMap(bitmap,fmconfig->textscreen.depth,fmconfig->textscreen.width,stl.height);
InitBitMap(vbitmap,fmconfig->textscreen.depth,fmconfig->textscreen.width,fmmain.txtshowysize);
InitBitMap(cbitmap,fmconfig->textscreen.depth,fmconfig->textscreen.width,fmconfig->textscreen.height);
apu2=1;
for(apu1=0;apu1<fmconfig->textscreen.depth;apu1++) {
	if(!(bitmap->Planes[apu1]=AllocRaster(fmconfig->textscreen.width,stl.height))) 
		apu2=0;
		else
		BltClear(bitmap->Planes[apu1],RASSIZE(fmconfig->textscreen.width,stl.height),1);
	if(!(vbitmap->Planes[apu1]=AllocRaster(fmconfig->textscreen.width,fmmain.txtshowysize))) apu2=0;
	if(!(cbitmap->Planes[apu1]=AllocRaster(fmconfig->textscreen.width,fmconfig->textscreen.height))) apu2=0;
}
if (!apu2) goto sterr;
#endif

if (!(stl.screen=OpenScreenTags(0,
	SA_Left,0,SA_Top,0,
	SA_Width,fmconfig->textscreen.width,SA_Height,fmconfig->textscreen.height,
	SA_Depth,fmconfig->textscreen.depth,SA_Font,&fmconfig->txtshowfontattr,
	SA_Type,CUSTOMSCREEN,SA_DisplayID,fmconfig->textscreen.screenmode,
	SA_BitMap,bitmap,SA_ShowTitle,0,SA_Quiet,1,SA_Pens,penlist,
	SA_AutoScroll,0,SA_Interleaved,1,
	TAG_DONE))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_SCRERR1);
	goto sterr;
}
stl.bitmap=bitmap; stl.vbitmap=vbitmap; stl.cbitmap=cbitmap;
stl.screen->ViewPort.RasInfo->RyOffset=stl.scroll;
ScrollVPort(&stl.screen->ViewPort);
SetAPen(&stl.screen->RastPort,fmconfig->txtpen);

if (!(stl.window=OpenWindowTags(0,
	WA_Left,0,WA_Top,4,
	WA_Width,fmconfig->textscreen.width,WA_Height,fmconfig->textscreen.height-4,
	WA_IDCMP,IDCMP_MOUSEMOVE|IDCMP_RAWKEY|IDCMP_MOUSEBUTTONS|IDCMP_ACTIVEWINDOW|IDCMP_INACTIVEWINDOW,
	WA_Flags,WFLG_BACKDROP|WFLG_BORDERLESS|WFLG_ACTIVATE|WFLG_NOCAREREFRESH|WFLG_RMBTRAP|WFLG_REPORTMOUSE,
	WA_CustomScreen,stl.screen,WA_RptQueue,1
	,TAG_DONE))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_WINERR1);
	goto sterr;
}
err=1;

while(!quit) {
	if (!mode&&!omode&&stl.loaded==0) {
		stl.loaded=loadmore(h,&stl);
		if(stl.loaded) {
			freemem(stl.ascbuffer);
			stl.ascbuffer=0;
		}
		mode=10;
	}
	if (!outputted&&stl.numlines&&stl.lines>0) {
		stl.usenode2=stl.head; stl.usenode1=stl.head;
		stl.pointer2=0;stl.pointer1=0;
		stl.topline=-stl.numlines;
		stn=stl.usenode2; point=stl.pointer2;
		stl.firstinfo=0;
		for(apu1=0;apu1<stl.numlines;apu1++) textscrollup(&stl,-1);
		stl.usenode2=stn; stl.pointer2=point;
		outputted=1;
	}
	im=(struct IntuiMessage*)GetMsg(stl.window->UserPort);
	if(!im&&!mode&&stl.loaded) {
		Wait(1L<<stl.window->UserPort->mp_SigBit);
	}
	if(im) {
		class=im->Class;
		code=im->Code;
		qualifier=im->Qualifier;
		if(ignorebutton) ignorebutton--;

		switch(class)
		{
		case IDCMP_INACTIVEWINDOW:
		mode=0;
		break;
		case IDCMP_ACTIVEWINDOW:
		if(!started) ignorebutton=2; else started=0;
		break;
		case IDCMP_MOUSEBUTTONS:
		if(code==SELECTDOWN&&!ignorebutton) quit=1;
		if(code==MENUDOWN&&!ignorebutton) { quit=1; err=0; }
		break;
		case IDCMP_MOUSEMOVE:
		if(keypressed) break;
		if(im->MouseY<64) { mode=1;break; }
		if(im->MouseY>stl.window->Height-64) { mode=2;break;}
		mode=0;
		break;
		case IDCMP_RAWKEY:
			switch(code)
			{
			case 0x2e:
			keypressed=1;
			quit=1;
			break;
			case 0x4c:
			case 0x3e:
			keypressed=1;
			mode=1;
			break;
			case 0x4d:
			case 0x1e:
			keypressed=1;
			mode=2;
			break;
			case 0x80+0x4c:
			case 0x80+0x3e:
			case 0x80+0x4d:
			case 0x80+0x1e:
			case 0x80+0x3f:
			case 0x80+0x1f:
			case 0x80+0x3d:
			case 0x80+0x1d:
			keypressed=0;
			mode=0;
			break;
			case 0x4f:
			case 0x2d:
			keypressed=1;
			if (mode==3||mode==4) mode=0; else mode=3;
			break;
			case 0x4e:
			case 0x2f:
			keypressed=1;
			if (mode==4||mode==3) mode=0; else mode=4;
			break;
			case 0x3f:	/* ageup */
			mode=5;
			break;
			case 0x1f:	/* pagedown */
			mode=6;
			break;
			case 0x3d:	/* home */
			mode=7;
			break;
			case 0x1d:	/* end */
			mode=8;
			break;
			default:
			ie.ie_Code=code;
			ie.ie_Qualifier=qualifier;
			ie.ie_EventAddress=(APTR*)*((ULONG*)im->IAddress);
			if (MapRawKey(&ie,chartab,2,0)==1) {
				switch(ToUpper(chartab[0]))
				{
				case 'J':
				mode=9;
				break;
				case 'S':
				mode=11;
				break;
				case 27:
				err=0;
				case 0x0d:
				quit=1;
				break;
				}
			}
			break;
			}
		}
	ReplyMsg((struct Message*)im); im=0;
	}
	if (mode!=omode) {
		if(!mode) infoblock(&stl,stl.scroll,1);
		if(mode&&!omode) {
			WaitTOF();
			infoblock(&stl,stl.scroll,0);
		}
		omode=mode;
	}
	switch(mode)
	{
	case 1:	/* ast down */
	textscrolldown(&stl,0);
	break;
	case 2:	/* fast up */
	textscrollup(&stl,0);
	break;
	case 3:	/* slow down */
	textscrolldown(&stl,1);
	break;
	case 4:	/* slow up */
	textscrollup(&stl,1);
	break;
	case 5: /* page down */
	for(apu1=1;apu1<stl.numlines;apu1++) textscrolldown(&stl,-1);
	break;
	case 6: /* page up */
	for(apu1=1;apu1<stl.numlines;apu1++) textscrollup(&stl,-1);
	break;
	case 7: /* home */
	stl.usenode2=stl.head; stl.usenode1=stl.head;
	stl.pointer2=0;stl.pointer1=0;
	stl.topline=-stl.numlines;
	for(apu1=0;apu1<stl.numlines;apu1++) textscrollup(&stl,-1);
	stl.usenode2=stl.head; stl.pointer2=0;
	stl.firstinfo=0;
	break;
	case 8: /* end */
	stl.usenode1=stl.usenode2=(struct STN*)&stl.tail;
	stl.pointer1=stl.pointer2=0;
	stl.topline=stl.lines;
	for(apu1=0;apu1<stl.numlines;apu1++) textscrolldown(&stl,-1);
	stl.usenode1=(struct STN*)&stl.tail; stl.pointer1=0;
	stl.firstinfo=0;
	break;
	case 9: /* jump */
	getjump(&stl);
	mode=0;
	break;
	case 10: /* infoblock */
	mode=0;
	break;
	case 11: /* search */
	textsearch(&stl,qualifier);
	mode=0;
	break;
	}	
}

sterr:
while(stl.head->succ) {
	stn=stl.head;
	Remove((struct Node*)stn);
	freemem((struct Node*)stn);
}
freemem(stl.buffer);
freemem(stl.ascbuffer);
WaitBlit();
if (stl.window) CloseWindow(stl.window);
if (stl.screen) CloseScreen(stl.screen);
#ifdef V39
FreeBitMap(bitmap);
FreeBitMap(vbitmap);
FreeBitMap(cbitmap);
#else
if (bitmap) {
	for(apu1=0;apu1<fmconfig->textscreen.depth;apu1++) {
		if(bitmap->Planes[apu1]) FreeRaster(bitmap->Planes[apu1],fmconfig->textscreen.width,stl.height);
		if(vbitmap->Planes[apu1]) FreeRaster(vbitmap->Planes[apu1],fmconfig->textscreen.width,fmmain.txtshowysize);
		if(cbitmap->Planes[apu1]) FreeRaster(cbitmap->Planes[apu1],fmconfig->textscreen.width,fmconfig->textscreen.height);
	}
	freemem(bitmap);
	freemem(vbitmap);
	freemem(cbitmap);
}
#endif
return(err);
}


WORD loadmore(struct FMHandle *h,struct STL *stl)
{
LONG len,apu1;
struct STN *stn;
WORD apu12;
UBYTE *ptr1,*ptr2,*ptr3;

len=h->bufsize; if(stl->remain<len) len=stl->remain;
if (readbufferfile(h,h->buffer,len)!=len||!len) return(1);

ptr1=h->buffer;
ptr3=stl->ascbuffer;
while(len>0) {
	if(stl->flag) {
		ptr2=ptr1;
		while(--len>0&&*ptr2!=0&&*ptr2!=0x0a) ptr2++;
		ptr2++;
		apu12=ptr2-ptr1;
		if(apu12>stl->handle->bufsize) return(1);
		if(len<=0&&stl->remain>apu12) break;
		*ptr3++=255;
		convertstring(stl,ptr1,&ptr3,apu12);
		ptr1=ptr2;
	} else {
	 	apu12=len;
		if(apu12>stl->hexrows) apu12=stl->hexrows;
		if(apu12>stl->handle->bufsize) return(1);
		if(apu12<stl->hexrows&&stl->remain>apu12) break;
		*ptr3++=255;
		ptr3+=hexconvert(ptr1,ptr3,apu12,stl->hexrows);
		*ptr3++=0;
		ptr1+=apu12;
		len-=apu12;
	}
	stl->remain-=apu12;
	stl->lines++;
	stl->bytes+=apu12;
	if(!stl->remain) {
		*ptr3++=255; *ptr3++=0;
		apu1=stl->lines+1;
		while(apu1<stl->numlines) {
			apu1++;
			*ptr3++=255; *ptr3++=0;
		}
	}
	apu12=0;
}	
apu1=ptr3-stl->ascbuffer;
if (!(stn=allocvec(h->owner,sizeof(struct STN)+apu1,MEMF_CLEAR))) return(1);
stn->databytes=apu1;
CopyMem(stl->ascbuffer,&(stn->buffer),stn->databytes);
if (seek(h,-apu12,OFFSET_CURRENT)<0) {
	freemem(stn);
	return(1);
}
if((struct List*)stl->head->succ&&!stl->usenode1->succ) {
	stl->usenode1=stl->usenode1->pred;
	AddTail((struct List*)stl,(struct Node*)stn);
	stl->usenode1=stl->usenode1->succ;
} else {
	AddTail((struct List*)stl,(struct Node*)stn);
}
return(0);
}

void escmode(struct STL *stl,UBYTE **ssrc,UBYTE **ddst,WORD *slen)
{
UBYTE *src,*dst;
UBYTE cha1,cha2;
UBYTE vstyle,vcolor1,vcolor2;
WORD len;

/*  first byte = 1 | style<<1 */
/*  second byte = 1 | color1<<5|color2<<1 */

src=*ssrc;
dst=*ddst;
len=*slen;
vstyle=stl->style;
vcolor1=stl->color1;
vcolor2=stl->color2;
cha1=*src++; len--;
if(cha1>='0'&&cha1<='9'&&(*src==';'||*src=='m')) {
	switch(cha1)
	{
	case '0':
	vstyle=0;
	vcolor1=1;
	vcolor2=0;
	break;
	case '1':
	vstyle|=2;
	break;
	case '3':
	vstyle|=4;
	break;
	case '4':
	vstyle|=1;
	break;
	case '7':
	vstyle|=8;
	break;
	}
	cha1=*src++; len--;
	if(cha1=='m') {
		escstyle(&dst,&stl->style,vstyle,&stl->color1,vcolor1,&stl->color2,vcolor2);
		goto escend;
	}
	if(cha1!=';') return;
	cha1=*src++; len--;
}
if(cha1=='3'||cha1=='4') {
	cha2=*src++; len--;
	if(cha2=='9') {
		if(cha1=='3')
			vcolor1=1;
			else
			vcolor2=0;
	} else {
		if(cha2<'0'||cha2>'7') return;
		if(cha1=='3') 
			vcolor1=cha2-'0';
			else
			vcolor2=cha2-'0';
	}
	cha1=*src++; len--;
	if(cha1=='m') {
		escstyle(&dst,&stl->style,vstyle,&stl->color1,vcolor1,&stl->color2,vcolor2);
		goto escend;
	}
} else {
	return;
}
if(cha1!=';') return;
cha1=*src++; len--;
if(cha1=='3'||cha1=='4') {
	cha2=*src++; len--;
	if(cha2<'0'||cha2>'7') return;
	if(cha1=='3') 
		vcolor1=cha2-'0';
		else
		vcolor2=cha2-'0';
	cha1=*src++; len--;
	if(cha1!='m') return;
	escstyle(&dst,&stl->style,vstyle,&stl->color1,vcolor1,&stl->color2,vcolor2);
}
escend:
*ssrc=src;
*ddst=dst;
*slen=len;
}

void escstyle(UBYTE **ddst,UBYTE *s,UBYTE vs,UBYTE *c1,UBYTE vc1,UBYTE *c2,UBYTE vc2)
{
UBYTE *dst;

dst=*ddst;
if(*s==vs&&*c1==vc1&&*c2==vc2) {
	return;
} else {
	*s=vs;
	*c1=vc1;
	*c2=vc2;
	*dst++=1|vs<<1;
	*dst++=1|vc2<<5|vc1<<1;
	*dst=0;
}
*ddst=dst;
}

void convertstring(struct STL *stl,UBYTE *src,UBYTE **ddst,WORD len)
{
UBYTE *dst;
UWORD cha1;
WORD tabsize=8;
WORD tabber=tabsize;

dst=*ddst;
while(len>0) {
	cha1=*src++; len--;
	if(cha1>=32) {
		*dst++=cha1;
		tabber--;
		if(!tabber) tabber=tabsize;
		continue;
	}
	if(cha1==0x0d) continue;
	if(cha1==0x09) {
		while(tabber--) *dst++=0x20;
		tabber=tabsize;
		continue;
	}
	if(cha1==0x1b&&*src=='[') {
		len--; src++;
		escmode(stl,&src,&dst,&len);
		continue;
	}
}
*dst++=32;
*dst++=0; *ddst=dst;
}

WORD textup(struct STL *stl)
{
LONG cnt1,cnt2;
struct STN *un1,*un2;
LONG uptr1,uptr2;

un1=stl->usenode1;
un2=stl->usenode2;
uptr1=stl->pointer1;
uptr2=stl->pointer2;

if(!un1->succ) return(0);

cnt1=strlen(&(un1->buffer)+uptr1)+1;
cnt2=strlen(&(un2->buffer)+uptr2)+1;

uptr1+=cnt1;
if(uptr1>=un1->databytes) {
	un1=un1->succ;
	uptr1=0;
}
uptr2+=cnt2;
if(uptr2>=un2->databytes) {
	un2=un2->succ;
	uptr2=0;
}
stl->pointer1=uptr1;
stl->pointer2=uptr2;
stl->usenode1=un1;
stl->usenode2=un2;
stl->topline++;

return(1);
}


void textscrollup(struct STL *stl,WORD smooth)
{
struct STN *stn;
struct RastPort *rp;
WORD yy,apu1,apu2,apu3;

rp=&(stl->screen->RastPort);

if(!stl->usenode1->succ) return;

stn=stl->usenode1;
yy=stl->scroll-fmmain.txtshowysize;
Move(rp,stl->xx,yy+fmmain.txtshowbaseline);
strtext(stl,rp,&(stn->buffer)+stl->pointer1);
BltBitMap(rp->BitMap,0,yy,rp->BitMap,0,stl->scroll+fmconfig->textscreen.height,fmconfig->textscreen.width,fmmain.txtshowysize,0xc0,0xff,0);
textup(stl);
apu2=fmmain.txtshowysize; if (smooth==1) apu2=stl->scrollvalue;
for(apu1=0;apu1!=fmmain.txtshowysize;apu1+=apu2) {
	if(stl->config&&stl->config->showinfo&&smooth>=0) infoblock(stl,stl->scroll,1);
	apu3=stl->scroll;
	stl->scroll+=apu2;
	if(stl->scroll==fmconfig->textscreen.height+2*fmmain.txtshowysize) stl->scroll=fmmain.txtshowysize;
	stl->screen->ViewPort.RasInfo->RyOffset=stl->scroll;
	if(smooth>=0) while(VBeamPos()<32);
	ScrollVPort(&(stl->screen->ViewPort));
	if(smooth>=0) {
		WaitTOF();
		if(stl->config->showinfo) infoblock(stl,apu3,0);
	}
}
stl->newsearch=0;
}

WORD textdown(struct STL *stl)
{
LONG cnt1,cnt2;
UBYTE *ptr1,*ptr2;
struct STN *un1,*un2;
LONG uptr1,uptr2;

un1=stl->usenode1;
un2=stl->usenode2;
uptr1=stl->pointer1;
uptr2=stl->pointer2;

if(!un2->pred->pred&&!uptr2) return(0);


if (!uptr2) {
	un2=un2->pred;
	uptr2=un2->databytes;
}
if (!uptr1) {
	un1=un1->pred;
	uptr1=un1->databytes;
}
ptr2=&(un1->buffer);
ptr1=ptr2+uptr1-1;
while(ptr1!=ptr2&&*--ptr1);
cnt1=ptr2+uptr1-ptr1;
if(ptr1!=ptr2) cnt1--;
ptr2=&(un2->buffer);
ptr1=ptr2+uptr2-1;
while(ptr1!=ptr2&&*--ptr1);
cnt2=ptr2+uptr2-ptr1;
if(ptr1!=ptr2) cnt2--;

uptr2-=cnt2;
uptr1-=cnt1;
stl->pointer1=uptr1;
stl->pointer2=uptr2;
stl->usenode1=un1;
stl->usenode2=un2;
stl->topline--;
return(1);
}

void textscrolldown(struct STL *stl,WORD smooth)
{
struct STN *stn;
struct RastPort *rp;
WORD yy,apu1,apu2,apu3;

rp=&(stl->screen->RastPort);


if(!stl->usenode2->pred->pred&&!stl->pointer2) return;

textdown(stl);
stn=stl->usenode2;
yy=stl->scroll-fmmain.txtshowysize;
Move(rp,stl->xx,yy+fmmain.txtshowbaseline);
strtext(stl,rp,&(stn->buffer)+stl->pointer2);
BltBitMap(rp->BitMap,0,yy,rp->BitMap,0,stl->scroll+fmconfig->textscreen.height,fmconfig->textscreen.width,fmmain.txtshowysize,0xc0,0xff,0);

apu2=fmmain.txtshowysize; if (smooth==1) apu2=stl->scrollvalue;
for(apu1=0;apu1!=fmmain.txtshowysize;apu1+=apu2) {
	if(stl->config->showinfo&&smooth>=0) infoblock(stl,stl->scroll,1);
	apu3=stl->scroll;
	stl->scroll-=apu2;
	if(!stl->scroll) stl->scroll=fmconfig->textscreen.height+fmmain.txtshowysize;
	stl->screen->ViewPort.RasInfo->RyOffset=stl->scroll;
	if(smooth>=0) while(VBeamPos()<32);
	ScrollVPort(&(stl->screen->ViewPort));
	if(smooth>=0) {
		WaitTOF();
		if(stl->config->showinfo) infoblock(stl,apu3,0);
	}
}
stl->newsearch=0;
}

void strtext(struct STL *stl,struct RastPort *rp,UBYTE *txt)
{
WORD len2=0,len,yy;
UBYTE *ptr;
UBYTE *hili;
extern UBYTE space[];

hili=txt; txt++;
while(*txt) {
	if((BYTE)*txt<32&&(BYTE)*txt>=0) {
		stl->style=*txt>>1;
		if(stl->style&8) SetDrMd(rp,JAM2|INVERSVID); else SetDrMd(rp,JAM2);
		SetSoftStyle(rp,stl->style,7);
		txt++;
		stl->color1=(*txt>>1)&0x07;
		SetAPen(rp,stl->color1);
		stl->color2=*txt>>5;
		SetBPen(rp,stl->color2);
		txt++;
	} else {
		ptr=txt;
		while(*ptr>=32) ptr++;
		len=ptr-txt;
		if((len+len2)<stl->rows)
			Text(rp,txt,ptr-txt);
			else
			Text(rp,txt,stl->rows-len2);
		len2+=len;
		txt=ptr;
		if(len2>=stl->rows) break;
	}
}
if(len2<stl->rows) {
	SetDrMd(rp,JAM2);
	Text(rp,space,stl->rows-len2);
	if(stl->style&8) SetDrMd(rp,JAM2|INVERSVID);
}
if(*hili!=255) {
	yy=stl->scroll-fmmain.txtshowysize;
	SetDrMd(rp,COMPLEMENT);
	RectFill(rp,0,yy,stl->numrows*fmmain.txtshowxsize,yy+fmmain.txtshowysize);
	SetDrMd(rp,JAM2);
	*hili=255;
}
}


void infoblock(struct STL *stl,WORD yy,WORD type)
{
UBYTE text[200];
WORD cnt;
struct RastPort *rp;
LONG apu,prose;

yy+=stl->screen->Height-fmmain.txtshowysize;
rp=&(stl->screen->RastPort);
if (type) {
	BltBitMap(stl->bitmap,0,yy,stl->vbitmap,0,0,fmconfig->textscreen.width,fmmain.txtshowysize,0xc0,0xff,0);
	apu=stl->topline;
	if (stl->topline<0) apu=0;
	prose=100;
	if(stl->lines) prose=(LONG)(apu+stl->numlines)*(LONG)100/(LONG)stl->lines;
	if(prose>100) prose=100;
	if (!stl->loaded) {
		if(stl->handle->size)
			sformat(text,getstring(MSG_SHOWTEXTINFO1),stl->handle->filename,apu,stl->lines,prose,stl->bytes*100/stl->handle->size);
			else
			sformat(text,getstring(MSG_SHOWTEXTINFO1),stl->handle->filename,apu,stl->lines,prose,100);
	} else {
		sformat(text,getstring(MSG_SHOWTEXTINFO2),stl->handle->filename,apu,stl->lines,prose);
	}
	cnt=strlen(text);
	while(cnt<stl->numrows) text[cnt++]=32;
	SetAPen(rp,fmconfig->whitepen);
	SetBPen(rp,fmconfig->blackpen);
	SetSoftStyle(rp,0,7);
	SetDrMd(rp,JAM2|INVERSVID);
	Move(rp,0,yy+fmmain.txtshowbaseline);
	cnt=strlen(text);
	if(cnt>stl->numrows) cnt=stl->numrows;
	Text(rp,text,cnt);	
	if(stl->style&8) SetDrMd(rp,JAM2|INVERSVID); else SetDrMd(rp,JAM2);
	SetAPen(rp,stl->color1);
	SetBPen(rp,stl->color2);
	stl->firstinfo=1;
} else {
	if(stl->firstinfo) {
		BltBitMap(stl->vbitmap,0,0,stl->bitmap,0,yy,fmconfig->textscreen.width,fmmain.txtshowysize,0xc0,0xff,0);
	}
}
}

void setreqstate(struct STL *stl)
{
WaitTOF();
BltBitMap(stl->bitmap,0,0,stl->cbitmap,0,0,fmconfig->textscreen.width,fmconfig->textscreen.height,0xc0,0xff,0);
BltBitMap(stl->bitmap,0,stl->scroll,stl->bitmap,0,0,fmconfig->textscreen.width,fmconfig->textscreen.height,0xc0,0xff,0);
WaitBlit();
stl->screen->ViewPort.RasInfo->RyOffset=0;
ScrollVPort(&(stl->screen->ViewPort));
infoblock(stl,0,1);
}
void resetreqstate(struct STL *stl)
{
infoblock(stl,0,0);
WaitTOF();
BltBitMap(stl->cbitmap,0,0,stl->bitmap,0,0,fmconfig->textscreen.width,fmconfig->textscreen.height,0xc0,0xff,0);
WaitBlit();
stl->screen->ViewPort.RasInfo->RyOffset=stl->scroll;
ScrollVPort(&(stl->screen->ViewPort));
}

void getjump(struct STL *stl)
{
WORD apu1;
LONG linecnt;
struct STN *stn;
LONG point;
LONG value;

setreqstate(stl);
value=stl->topline;
if(asklong(stl->screen,stl->handle->owner->workname,getstring(MSG_SHOWTEXT_LINENUMBER),&value,0,stl->lines)) {
#ifdef V39
	SetWindowPointer(stl->window,WA_BusyPointer,TRUE,WA_PointerDelay,TRUE,TAG_DONE);
#endif
	linecnt=value;
	stl->topline=-stl->numlines;
	if(linecnt>stl->lines-stl->numlines+1) linecnt=stl->lines-stl->numlines+1;
	if(linecnt<0) linecnt=0;
	stl->usenode2=stl->head; stl->usenode1=stl->head;
	stl->pointer2=0;stl->pointer1=0;
	stl->topline=-stl->numlines;
	while(linecnt) {
		linecnt--;
		textup(stl);
	}
	resetreqstate(stl);
	stn=stl->usenode2; point=stl->pointer2;
	for(apu1=0;apu1<stl->numlines;apu1++) textscrollup(stl,-1);
	stl->usenode2=stn; stl->pointer2=point;
	stl->firstinfo=0;
#ifdef V39
	SetWindowPointer(stl->window,TAG_DONE);
#endif
	return;
}
resetreqstate(stl);
}

void textsearch(struct STL *stl,WORD qualifier)
{
WORD apu1,len,ret;
struct STN *stn;
LONG point;
UBYTE *varabuf;
UBYTE *ptr1,*ptr2,*ptr3;
LONG topline;
#ifdef V39
WORD mouse=0;
#endif

varabuf=allocvec(stl->handle->owner,stl->handle->bufsize,0);
if(!varabuf) return;
ret=0;
if(!(qualifier&0x80)||stl->searchstring[0]==0) {
	setreqstate(stl);
	ret=askstring(stl->screen,stl->handle->owner->workname,getstring(MSG_SHOWTEXT_SEARCHSTRING),stl->searchstring,sizeof(stl->searchstring));
	resetreqstate(stl);
}
if((qualifier&0x80)||(ret&&stl->searchstring[0])) {
#ifdef V39
	SetWindowPointer(stl->window,WA_BusyPointer,TRUE,WA_PointerDelay,TRUE,TAG_DONE);
	mouse=1;
#endif
	len=strlen(stl->searchstring);
	if(stl->newsearch) {
		for(apu1=0;apu1<stl->numlines;apu1++) textup(stl);
		stl->newsearch=0;
	}
	stn=stl->usenode2;
	point=stl->pointer2;
	for(;;) {
		ptr1=ptr3=&(stn->buffer)+point;
		point+=strlen(ptr1)+1;
		ptr2=varabuf;
		while(*ptr1) {
			if((BYTE)*ptr1<0||(BYTE)*ptr1>31) *ptr2++=*ptr1++; else ptr1+=2;
		}
		*ptr2=0; ptr2-=len;
		ptr1=varabuf;
		while(ptr1<ptr2) {
			if (Strnicmp(stl->searchstring,ptr1,len)==0) break;
			ptr1++;
		}	
		if(ptr1<ptr2) {
			*ptr3=10;
			break;
		}
		stl->topline++;
		if(point>=stn->databytes) {
			WaitTOF();
			infoblock(stl,0,0);
			infoblock(stl,0,1);
			stn=stn->succ; point=0;
		}
		if(!stn->succ) {
			DisplayBeep(stl->screen);
			break;
		}
	}
	topline=stl->topline-(stl->numlines+1)/2;
	if(topline>stl->lines-stl->numlines+1) topline=stl->lines-stl->numlines+1;
	if(topline<0) topline=0;
	stl->topline=-stl->numlines;
	stl->usenode2=stl->head; stl->usenode1=stl->head;
	stl->pointer2=0;stl->pointer1=0;
	while(topline) {
		topline--;
		textup(stl);
	}
	stn=stl->usenode2; point=stl->pointer2;
	for(apu1=0;apu1<stl->numlines;apu1++) textscrollup(stl,-1);
	stl->usenode2=stn; stl->pointer2=point;
	stl->newsearch=1;
	stl->firstinfo=0;
}
#ifdef V39
	if(mouse) SetWindowPointer(stl->window,TAG_DONE);
#endif
freemem(varabuf);
}

void *showtextconfigdefault(struct CMenuConfig *cmc)
{
struct ShowTxt *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct ShowTxt));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct ShowTxt*)cmc->moreconfig;
config->showinfo=1;
return(cmc->moreconfig);
}

WORD showtextconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD si,c;
struct ShowTxt *config;

config=(struct ShowTxt*)showtextconfigdefault(cmc);
si=config->showinfo;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_SHOWTEXT_CONFIG_INFOLINE,200,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqcycle2msg(gb,200,&si);
commandanswer(gb);
c=quickreq(gb);
config->showinfo=si;
return(c);
}
