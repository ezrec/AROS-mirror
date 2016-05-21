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
#include <dos/dos.h>
#include <workbench/workbench.h>
#include <libraries/diskfont.h>
#include <intuition/iobsolete.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>  
#include <proto/gadtools.h>

#include <stdio.h>
#include <string.h>

#include "fmnode.h"
#include "ilbm.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

ULONG power2(WORD);
WORD openicon(struct FMList*);
WORD opendisk(struct FMList*);
WORD showiconfont(struct FMHandle*,WORD,struct CMenuConfig *cmc);
WORD selectfont(struct FMHandle*,struct FontContentsHeader*,struct TextAttr*);

extern struct Library *IconBase;
extern struct Library *GadToolsBase;
extern struct Library *DiskfontBase;
extern struct FMMain fmmain;
extern UBYTE icolib[];
extern UBYTE dflib[];
extern UBYTE workbench[];

WORD showicon(struct FMHandle *h,struct CMenuConfig *cmc)
{
return(showiconfont(h,0,cmc));
}
WORD showfont(struct FMHandle *h,struct CMenuConfig *cmc)
{
return(showiconfont(h,1,cmc));
}

WORD showiconfont(struct FMHandle *h,WORD type,struct CMenuConfig *cmc)
{
WORD ret,quitti,ignorebutton,started;
WORD x,y,width,height,pagew,pageh,depth=0,retry=0,apu1,apu2,apu3,txtcolor;
ULONG modeid=INVALID_ID;
UWORD mask,code,numcols=0;
ULONG class;
ULONG *coltable=0;
#ifndef V39
UBYTE *coltable2=0;
#endif
struct Image *i1;
struct Image *i2;
struct Gadget *igadg=NULL;
struct DiskObject *dob=0;
struct DrawInfo *di;
struct Screen *scr=0;
struct Window *win=0;
struct RastPort *rp;
struct VisualInfo *vi=NULL;
struct FMList *list;
UBYTE name[PATH];
UBYTE name2[PATH];
UBYTE text[100];
WORD penlist[2];
UBYTE chars[256];

struct TextFont *tfont=0;
struct ColorTextFont *ctfont=0;
struct TextAttr tattr;
struct FontContentsHeader *fch=0;
UWORD *cfptr;
WORD depthset=0;

txtcolor=1;
list=h->owner;
penlist[0]=-1;penlist[1]=-1;
ret=0;
changename(list,cmc->label);
if(type) {
	sformat(list->fmmessage1,getstring(MSG_FILECLICK_FONT),h->filename);
	fmmessage(list);
	if(!opendisk(list)) goto sierr;
	if(!(fch=allocvec(list,h->size,0))) goto sierr;
	if(readbufferfile(h,fch,h->size)==0) goto sierr;
} else {
	sformat(list->fmmessage1,getstring(MSG_FILECLICK_WBICON),h->filename);
	fmmessage(list);
	if(!openicon(list)) goto sierr;
	strcpy(name,h->filename);
	name[strlen(name)-5]=0;
	if (!(dob=GetDiskObject(name))) {
		dosrequestmsg(h->owner,0,MSG_FMDOS_OPENERR,h->filename);
		goto sierr;
	}
	igadg=&dob->do_Gadget;
}


if((scr=LockPubScreen(workbench))) {
	di=GetScreenDrawInfo(scr);
	if(di) {
		txtcolor=di->dri_Pens[TEXTPEN];
		FreeScreenDrawInfo(scr,di);
	}
	depth=scr->RastPort.BitMap->Depth;
	modeid=GetVPModeID(&scr->ViewPort);
	numcols=power2(depth);
	if(scr->ViewPort.ColorMap) {
		coltable=allocvec(h->owner,numcols*4*3+8,MEMF_CLEAR);
		if(coltable) {
#ifdef V39
			GetRGB32(scr->ViewPort.ColorMap,0,numcols,coltable+1);
#else
			coltable2=(UBYTE*)coltable;
			for(apu1=0;apu1<numcols;apu1++) {
				apu2=GetRGB4(scr->ViewPort.ColorMap,apu1);
				*coltable2++=(apu2>>8)&0x0f;
				*coltable2++=(apu2>>4)&0x0f;
				*coltable2++=(apu2)&0x0f;
			}
#endif
		}
	}
	UnlockPubScreen(0,scr);
}
if(modeid==INVALID_ID) modeid=0x8000;

newicon:

if(type==0) {
	sformat(text,getstring(MSG_SHOWICON_TITLE),h->filename,depth,(WORD)power2(depth));
} else {
	text[0]=0;
	if(!depthset) {
		if(!selectfont(h,fch,&tattr)) {
			ret=1;
			quitti=1;
			retry=0;
			goto sierr;
		}
	}
	strcpy(name2,list->path);
	AddPart(name2,h->filename,PATH);
	tattr.ta_Name=name2;
	if(!(tfont=OpenDiskFont(&tattr))) {
		requestmsg(list->workname,0,MSG_OK,MSG_FMDOS_OPENERR,tattr.ta_Name);
		goto sierr;
	}
	if(tattr.ta_Style&FSF_COLORFONT)
		ctfont=(struct ColorTextFont*)tfont;
		else
		ctfont=0;
	if(ctfont&&!retry) depth=ctfont->ctf_Depth;
}

vi=0;ret=0;retry=0;depthset=0;
ignorebutton=0;started=1;quitti=0;

scr=OpenScreenTags(0,
		SA_Title,text,
		SA_Width,STDSCREENWIDTH,SA_Height,STDSCREENHEIGHT,
		SA_DisplayID,modeid,SA_Depth,depth,
		SA_Pens,penlist,SA_Interleaved,1,
		TAG_DONE);
if(!scr) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_SCRERR1);
	goto sierr;
}
if(ctfont) {
	for(apu1=0;apu1<ctfont->ctf_ColorFontColors->cfc_Count;apu1++) {
		cfptr=ctfont->ctf_ColorFontColors->cfc_ColorTable+apu1;
		SetRGB4(&scr->ViewPort,apu1+ctfont->ctf_Low,((*cfptr)>>8)&0x0f,((*cfptr)>>4)&0x0f,((*cfptr))&0x0f);
	}
} else {
	if(coltable) { 
		apu1=power2(depth)-numcols;
#ifdef V39
		if(apu1>0) {
			*coltable=(ULONG)numcols<<16|apu1;
			LoadRGB32(&scr->ViewPort,coltable);
		}
		*coltable=(ULONG)numcols<<16;
		LoadRGB32(&scr->ViewPort,coltable);
#else
		if(apu1>0) {
			coltable2=(UBYTE*)coltable;
			for(apu2=0;apu2<numcols;apu2++) SetRGB4(&scr->ViewPort,apu2+apu1,*coltable2++,*coltable2++,*coltable2++);
		}
		coltable2=(UBYTE*)coltable;
		for(apu2=0;apu2<numcols;apu2++) SetRGB4(&scr->ViewPort,numcols,*coltable2++,*coltable2++,*coltable2++);
#endif
	}
}

pagew=scr->Width; pageh=scr->Height;
apu1=scr->WBorTop+scr->Font->ta_YSize+1;
win=OpenWindowTags(0,
		WA_Top,apu1,WA_Height,scr->Height-apu1,
		WA_IDCMP,IDCMP_MOUSEBUTTONS|IDCMP_VANILLAKEY|IDCMP_ACTIVEWINDOW,
		WA_Flags,WFLG_NOCAREREFRESH|WFLG_RMBTRAP|WFLG_ACTIVATE,
		WA_CustomScreen,scr,
		TAG_DONE);
if(!win) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_WINERR1);
	goto sierr;
}
rp=win->RPort;

if(type) {
	SetDrMd(rp,JAM1);
	SetBPen(rp,0);
	sformat(text,getstring(MSG_SHOWFONT_TITLE),h->filename,tattr.ta_YSize,depth,power2(depth));
	SetWindowTitles(win,0,text);
	for(apu1=0;apu1<256;apu1++) chars[apu1]=apu1;
	SetAPen(rp,0);
	RectFill(rp,win->BorderLeft,win->BorderTop,pagew-win->BorderRight,pageh-win->BorderBottom);
	SetFont(rp,tfont);
	SetAPen(rp,txtcolor);	
	apu1=tfont->tf_Baseline+4;
	apu2=4+tfont->tf_YSize;
	for(apu3=32;apu3<256;apu3+=32) {
		Move(rp,24,apu1);
		Text(rp,&chars[apu3],32);
		apu1+=apu2;
	}
	CloseFont(tfont);
	retry=1;
} else {
	vi=GetVisualInfo(scr,0);
	i1=(struct Image*)igadg->GadgetRender;
	i2=(struct Image*)igadg->SelectRender;
	sformatmsg(list->fmmessage2,MSG_SHOWPIC_INFO2,i1->Width,i1->Height,i1->Depth,power2(i1->Depth)," "," "," ");
	fmmessage(list);
	if((igadg->Flags&GADGHIGHBITS)==GADGHIMAGE) {
		width=i2->Width; height=i2->Height;
		x=(pagew-i1->Width-width)/2-32;
		y=i1->Height>height?i1->Height:height;
		y=(pageh-y)/2;
		DrawImage(rp,i1,x,y);
		DrawImage(rp,i2,x+i1->Width+32,y);
	} else {
		width=i1->Width; height=i1->Height;
		x=(pagew-i1->Width*2)/2-32;
		y=(pageh-i1->Height)/2;
		DrawImage(rp,i1,x,y);
		DrawImage(rp,i1,x+i1->Width+32,y);
		if((igadg->Flags&GADGHIGHBITS)==GADGHCOMP) {
			SetDrMd(rp,COMPLEMENT);
			mask=power2(i1->Depth);
			rp->Mask=mask-1;
			RectFill(rp,x+i1->Width+32,y,width+x+i1->Width+32,height+y);
		}
	}
	if (vi) {
		DrawBevelBox(rp,x-6,y-3,i1->Width+12,i1->Height+6,GT_VisualInfo,vi);
		DrawBevelBox(rp,x+i1->Width+32-6,y-3,width+12,height+6,GTBB_Recessed,1,GT_VisualInfo,vi);
	}
}

ret=1;
while(!quitti) {
	if(smallevent(win,&class,&code)) {
		quitti=1;
		continue;
	}
	if(ignorebutton) ignorebutton--;
	switch(class)
	{
	case IDCMP_ACTIVEWINDOW:
	if(!started) ignorebutton=2; else started=0;
	break;
	case IDCMP_MOUSEBUTTONS:
	if(type) ignorebutton=0;
	if(code==SELECTDOWN&&!ignorebutton) quitti=1;
	if(code==MENUDOWN&&!ignorebutton) { quitti=1; ret=0; }
	break;
	case IDCMP_VANILLAKEY:
	if(code==13||code==27) quitti=1;
	if(code>='1'&&code<='8') {
		depth=code-'0';
		retry=1;
		depthset=1;
		quitti=1;
	}
	}
}
sierr:
if(win) CloseWindow(win);
if(scr) CloseScreen(scr);
freemem(coltable);
if(type==0) FreeVisualInfo(vi);
win=0; scr=0; tfont=0; coltable=0; vi=0;
if(retry&&!type) goto newicon;
if(retry&&type) goto newicon;
if(dob) FreeDiskObject(dob);
freemem(fch);
return(ret);
}

WORD openicon(struct FMList *list)
{
if (!IconBase) IconBase=OpenLibrary(icolib,37);
if (!IconBase) {
	requestmsg(list->workname,0,MSG_OK,MSG_FMDOS_OPENERR,icolib);
	return(0);
}
return(1);
}
WORD opendisk(struct FMList *list)
{
if (!DiskfontBase) DiskfontBase=OpenLibrary(dflib,37);
if (!DiskfontBase) {
	requestmsg(list->workname,0,MSG_OK,MSG_FMDOS_OPENERR,dflib);
	return(0);
}
return(1);
}

WORD selectfont(struct FMHandle *h,struct FontContentsHeader *fch,struct TextAttr *ta)
{
struct GUIBase *gb;
struct FMList *list;
struct FontContents *fc1,*fc2,*fc3 = NULL;
struct FontContents *fc4[10];
UWORD numsizes;
WORD c=0;
WORD apu1,apu2,apu4;
UWORD apu3;
UBYTE *ptr2=0;
UBYTE *ptr1;
WORD ret=0;
LONG issize;
UBYTE used[10];
UBYTE *useptr = NULL;

memseti(used,0,10);
gb=getguibase(h->filename);
list=h->owner;
switch(fch->fch_FileID)
	{
	case OFCH_ID:
	issize=20;
	setconnectgroup(gb,1,0,0);
	reqinfomsg(gb,MSG_SHOWFONT_ASKSIZE,100,guiUC|guiLEFT);
	reqinteger(gb,100,&issize,1,999);
	buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
	c=quickreq(gb);
	if(!c) goto ferr;
	ta->ta_YSize=issize;
	ta->ta_Style=0;
	ta->ta_Flags=0;
	ret=1;
	goto ferr;
	case FCH_ID:
	numsizes=fch->fch_NumEntries;
	if(!(ptr2=allocvec(list,numsizes*(MAXFONTPATH+10),MEMF_CLEAR))) goto ferr;
	ptr1=ptr2; apu4=0;
	for(apu1=0;apu1<numsizes;apu1++) {
		apu3=65535;
		for(apu2=0;apu2<numsizes;apu2++) {
			fc2=(struct FontContents*)((UBYTE*)fch+4+apu2*sizeof(struct FontContents));
			if(used[apu2]==0&&fc2->fc_YSize<apu3) {
				apu3=fc2->fc_YSize;
				fc3=fc2;
				useptr=&used[apu2];
			}
		}
		if(*useptr==0&&apu4<10) {
			sformat(ptr1,"'%s' %3ld",fc3->fc_FileName,fc3->fc_YSize);
			reqbutton(gb,ptr1,apu1+10,guiUC|guiLEFT);
			ptr1+=strlen(ptr1)+1;
			*useptr=1;
			fc4[apu4]=fc3;
			apu4++;
		}
	}
	buttonbarmsg(gb,MSG_OK,1,0);
	c=quickreq(gb);
	if(c>=10&&c<=19) {
		fc1=fc4[c-10];
		ta->ta_YSize=fc1->fc_YSize;
		ta->ta_Style=fc1->fc_Style;
		ta->ta_Flags=fc1->fc_Flags;
		ret=1;
	}
}
ferr:
freemem(ptr2);
return(ret);
}

