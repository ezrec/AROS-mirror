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
 
#include <aros/debug.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/reqtools.h>
#include <proto/reqtools.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <libraries/reqtools.h>

#ifdef __AROS__
#include <aros/asmcall.h>
#endif

#define HELPCONFIGSIZE 500

#include "fmlocale.h"
#include "fmgui.h"
#include "fmnode.h"
#include "fieldlist.h"

extern struct GfxBase *GfxBase;
extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;  
extern struct DosLibrary *DOSBase; 

void *parseconfigdefault(struct CMenuConfig*);
void *includeconfigdefault(struct CMenuConfig*);
void *excludeconfigdefault(struct CMenuConfig*);
void *ownexecuteconfigdefault(struct CMenuConfig*);

WORD quitconfig(struct GUIBase*,struct CMenuConfig*);
void *quitconfigdefault(struct CMenuConfig*);
WORD sleepconfig(struct GUIBase*,struct CMenuConfig*);
void *sleepconfigdefault(struct CMenuConfig*);
WORD parentconfig(struct GUIBase*,struct CMenuConfig*);
void *parentconfigdefault(struct CMenuConfig*);
WORD copyconfig(struct GUIBase*,struct CMenuConfig*);
void *copyconfigdefault(struct CMenuConfig*);
WORD deleteconfig(struct GUIBase*,struct CMenuConfig*);
void *deleteconfigdefault(struct CMenuConfig*);
WORD showtextconfig(struct GUIBase*,struct CMenuConfig*);
void *showtextconfigdefault(struct CMenuConfig*);
WORD executeconfig(struct GUIBase*,struct CMenuConfig*);
void *executeconfigdefault(struct CMenuConfig*);
WORD hearconfig(struct GUIBase*,struct CMenuConfig*);
void *hearconfigdefault(struct CMenuConfig*);
WORD diskeditconfig(struct GUIBase*,struct CMenuConfig*);
void *diskeditconfigdefault(struct CMenuConfig*);

extern struct FMMain fmmain;
extern struct FMList fmlist[];
extern struct FMConfig *fmconfig;
struct ReqToolsBase *ReqToolsBase=0;
extern UBYTE reqlib[];

void* rleunpack(void*,void*,LONG);
LONG  rlepack(void*,void*,LONG);

ULONG power2(WORD);

struct FMConfig *newconfig(struct FMConfig *oc)
{
struct FMConfig *fmc;

fmc=allocmem(sizeof(struct FMConfig));
if(!fmc) return(0);
CopyMem(oc,fmc,sizeof(struct FMConfig));
fmc->txtfontattr.ta_Name=fmc->txtfontname;
fmc->listfontattr.ta_Name=fmc->listfontname;
fmc->reqfontattr.ta_Name=fmc->reqfontname;
fmc->txtshowfontattr.ta_Name=fmc->txtshowfontname;
fmc->smallfontattr.ta_Name=fmc->smallfontname;
return(fmc);
}
void freeconfig(struct FMConfig *sc,struct FMConfig *dc)
{
CopyMem(sc,dc,sizeof(struct FMConfig));
dc->txtfontattr.ta_Name=dc->txtfontname;
dc->listfontattr.ta_Name=dc->listfontname;
dc->reqfontattr.ta_Name=dc->reqfontname;
dc->txtshowfontattr.ta_Name=dc->txtshowfontname;
dc->smallfontattr.ta_Name=dc->smallfontname;
freemem(sc);
}

WORD errorreq(WORD,UBYTE*,WORD,WORD);

WORD openreqtools(void)
{
UBYTE text[100];

if(!ReqToolsBase) {
	ReqToolsBase=(struct ReqToolsBase*)OpenLibrary(reqlib,38);
	if(!ReqToolsBase) {
		sformatmsg(text,MSG_FMDOS_OPENERR,reqlib);
		errorreq(MSG_REQ_ERROR,text,0,MSG_OK);
		return(0);
	}
}
return(1);
}

/*
WORD getfilename(UBYTE *dst,WORD len)
{
struct rtFileRequester *rfr;
UBYTE fname[110];
UBYTE path[200];
UBYTE *ptr;

strcpy(path,dst);
if(!openreqtools()) return(0);
if(!(rfr=rtAllocRequest(RT_FILEREQ,0))) return(0);
ptr=FilePart(path);
strcpy(fname,ptr);
*ptr=0;
rtChangeReqAttr(rfr,RTFI_Dir,path,TAG_DONE);
if(rtFileRequest(rfr,fname,getstring(MSG_CONFIG_COMSELECT),TAG_DONE)) {
	strcpy(rfr->Dir,dst);
	AddPart(dst,fname,len);
}
rtFreeRequest(rfr);
return(1);
}
*/

WORD askscreen(struct ScreenConfig *sc,WORD title,WORD scroll)
{
struct rtScreenModeRequester *req;
WORD ret;
ULONG flags;

ret=0;
flags=0;
if(scroll) flags=SCREQF_AUTOSCROLLGAD;
flags|=SCREQF_OVERSCANGAD|SCREQF_SIZEGADS|SCREQF_DEPTHGAD|SCREQF_GUIMODES;

if(!openreqtools()) return(0);

req=(struct rtScreenModeRequester*)rtAllocRequestA(RT_SCREENMODEREQ,0);
if(req) {
	rtChangeReqAttr(req,
	RTSC_DisplayID,sc->screenmode,
	RTSC_DisplayWidth,sc->width,
	RTSC_DisplayHeight,sc->height,
	RTSC_DisplayDepth,sc->depth,
	RTSC_AutoScroll,sc->autoscroll,
	RTSC_OverscanType,sc->overscan,
	TAG_DONE);
	ret=rtScreenModeRequest(req,getstring(title), 
	RTSC_Flags,flags,
	RTSC_MinDepth,1,RTSC_MaxDepth,4,
	TAG_DONE);
	if(ret) {
		sc->screenmode=req->DisplayID;
		sc->width=req->DisplayWidth;
		sc->height=req->DisplayHeight;
		sc->depth=req->DisplayDepth;
		sc->overscan=req->OverscanType;
		sc->autoscroll=0;
		if (scroll) sc->autoscroll=1;
		ret=3;
	}
	rtFreeRequest(req);
}
return(ret);
}
WORD askfont(struct TextAttr *ta,WORD title,WORD type)
{
struct rtFontRequester *req;
WORD ret=0;

if(!openreqtools()) return(0);

req=(struct rtFontRequester*)rtAllocRequestA(RT_FONTREQ,0);
if(req) {
	rtChangeReqAttr(req,
	RTFO_FontName,ta->ta_Name,
	RTFO_FontHeight,ta->ta_YSize,
	RTFO_FontStyle,ta->ta_Style,
	TAG_DONE);
	ret=rtFontRequest(req,getstring(title),
	RTFO_Flags,(type?0:FREQF_FIXEDWIDTH)|FREQF_SCALE,
	TAG_DONE);
	if(ret) {
		strcpy(ta->ta_Name,req->Attr.ta_Name);
		ta->ta_YSize=req->Attr.ta_YSize;
		ta->ta_Style=req->Attr.ta_Style;
		ret=3;
	}
	rtFreeRequest(req);
}
return(ret);
}

/********************************
*		DATATYPES
*********************************/

WORD datatypesconfig(struct FMConfig *ofmc)
{
struct GUIBase *gb;
WORD retcode,c;
struct FMConfig *fmc;
WORD dt1,dt2,dt3,dt4,dt5,dt6,dt7,dt8;

retcode=0;
if(!(fmc=newconfig(ofmc))) return(0);

gb=getguibase(getstring(MSG_CONFIG_NAME));
setconnectgroup(gb,9,0,0);

dt1=fmc->usepicturedt;
dt2=fmc->usesampledt;
dt3=fmc->useascdt;
dt4=fmc->usehexdt;
dt5=fmc->usemoddt;
dt6=fmc->useicondt;
dt7=fmc->usefontdt;
dt8=fmc->useexecutedt;
setconnectgroup(gb,1,1,9);
reqinfomsg(gb,MSG_CONFIG_USEDT_PICTURE,110,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_SAMPLE,111,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_ASC,112,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_HEX,113,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_MOD,114,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_ICON,115,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_FONT,116,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_USEDT_EXECUTE,117,guiUC|guiLEFT);
setconnectgroup(gb,2,1,9);
reqcyclemsg(gb,110,&dt1,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,111,&dt2,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,112,&dt3,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,113,&dt4,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,114,&dt5,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,115,&dt6,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,116,&dt7,MSG_NON,MSG_YESN,0);
reqcyclemsg(gb,117,&dt8,MSG_NON,MSG_YESN,0);
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);

c=quickreq(gb);
if(c) {
	fmc->usepicturedt=dt1;
	fmc->usesampledt=dt2;
	fmc->useascdt=dt3;
	fmc->usehexdt=dt4;
	fmc->usemoddt=dt5;
	fmc->useicondt=dt6;
	fmc->usefontdt=dt7;
	fmc->useexecutedt=dt8;
	freeconfig(fmc,ofmc);
	fmc=0;
}
freemem(fmc);
return(retcode);
}

/*********************************
*		GENERAL
*********************************/

WORD generalconfig(struct FMConfig *ofmc)
{
struct GUIBase *gb;
WORD retcode=0;
LONG mpri,pril,prim,prih,dosbuf,dctime;
WORD rbut,c;
struct FMConfig *fmc;

if(!(fmc=newconfig(ofmc))) return(0);

gb=getguibase(getstring(MSG_CONFIG_NAME));
setconnectgroup(gb,9,0,0);

mpri=fmc->mainpriority;
pril=fmc->pri[0];
prim=fmc->pri[1];
prih=fmc->pri[2];
dosbuf=fmc->dosbuffersize;
dctime=fmc->doubleclick;
rbut=fmc->rightmouse;

setconnectgroup(gb,1,1,9);
reqinfomsg(gb,MSG_CONFIG_PRIORITYMAIN,100,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_PRIORITYLOW,101,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_PRIORITYMED,102,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_PRIORITYHIGH,103,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_DOSBUFSIZE,104,guiUC|guiLEFT);	
reqinfomsg(gb,MSG_CONFIG_DOUBLECLICK,105,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_RIGHTMOUSE,106,guiUC|guiLEFT);
setconnectgroup(gb,2,1,9);
reqinteger(gb,100,&mpri,-128,127);
reqinteger(gb,101,&pril,-128,127);
reqinteger(gb,102,&prim,-128,127);
reqinteger(gb,103,&prih,-128,127);
reqinteger(gb,104,&dosbuf,2000,99999999);
reqinteger(gb,105,&dctime,1,100);
reqcyclemsg(gb,106,&rbut,MSG_CONFIG_RIGHTMOUSE_DOUBLECLICK,MSG_CONFIG_RIGHTMOUSE_PARENT,0);
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);

c=quickreq(gb);
if(c) {
	fmc->mainpriority=mpri;
	fmc->pri[0]=pril;
	fmc->pri[1]=prim;
	fmc->pri[2]=prih;
	fmc->dosbuffersize=dosbuf;
	fmc->doubleclick=dctime;
	fmc->rightmouse=rbut;
	freeconfig(fmc,ofmc);
	fmc=0;
}
freemem(fmc);
return(retcode);
}

static WORD palette(struct FMConfig *ofmc)
{
struct GUIBase *gb;
WORD cnt1,retcode,c;
UBYTE *ptr1,*ptr2;
UBYTE txt[100];
UBYTE pens[FMPENS];
struct FMConfig *fmc;

if(!(fmc=newconfig(ofmc))) return(0);
gb=getguibase(getstring(MSG_CONFIG_PALETTE));
setconnectgroup(gb,10,0,0);
setconnectgroup(gb,1,1,10);
setconnectgroup(gb,2,1,10);
ptr1=getstring(MSG_CONFIG_PENS);
ptr2=&fmc->devicepen;
for(cnt1=0;cnt1<FMPENS;cnt1++) {
	setguigroup(gb,1,0);
	reqinfo(gb,ptr1,cnt1+100,guiUC|guiLEFT);
	setguigroup(gb,2,0);
	pens[cnt1]=*ptr2;
	ptr1+=strlen(ptr1)+1;
	reqpalette(gb,cnt1+120,ptr2);
	ptr2++;
}
buttonbarmsg(gb,MSG_OK,2,MSG_CONFIG_EDITPALETTE,1,MSG_CANCEL,0,0);

retcode=0;
if(openinitreq(gb)) {
	retcode=-1;
	while(retcode<0) {
		c=reqmsghandler(gb);
		switch(c)
			{
			case 2:	/*  OK */
			ptr2=&fmc->devicepen;
			for(cnt1=0;cnt1<FMPENS;cnt1++) {
				if(pens[cnt1]!=*ptr2) {
					if(cnt1>=9) retcode=2; else retcode=1;
				}
			}
			freeconfig(fmc,ofmc);
			fmc=0;
			break;
			case 1:
			if(openreqtools()) {
				ptr1=getstring(MSG_CONFIG_EDITPALETTE);
				ptr2=txt;
				while(*ptr1) {
					if(*ptr1=='_') ptr1++; else *ptr2++=*ptr1++;
				}
				*ptr2=0;
				rtPaletteRequestA(txt,0,0);
			}
			break;
			case 0:
			retcode=0;
			break;
		}
	}
}
freereq(gb);
freemem(fmc);
return(retcode);
}

static WORD savecommands(struct FMConfig *fmc,void *moreconfigs[])
{
struct CMenuConfig *cmc;
WORD cnt;

for(cnt=0;cnt<TOTALCOMMANDS;cnt++) {
	cmc=&fmc->cmenuconfig[cnt];
	moreconfigs[cnt]=0;
	if(cmc->moreconfiglen) {
		moreconfigs[cnt]=allocmem(cmc->moreconfiglen);
		if(!moreconfigs[cnt]) return(0);
		CopyMem(cmc->moreconfig,moreconfigs[cnt],cmc->moreconfiglen);
	}
}
return(1);
}
static void restorecommands(struct FMConfig *fmc,void *moreconfigs[])
{
struct CMenuConfig *cmc;
WORD cnt;

for(cnt=0;cnt<TOTALCOMMANDS;cnt++) {
	if(fmc) {
		cmc=&fmc->cmenuconfig[cnt];
		if(moreconfigs[cnt]) {
			CopyMem(moreconfigs[cnt],cmc->moreconfig,cmc->moreconfiglen);
		} else if(cmc->moreconfiglen) {
			freemem(cmc->moreconfig);
			cmc->moreconfiglen=0;
		}
	} else {
		freemem(moreconfigs[cnt]);
	}
	moreconfigs[cnt]=0;
}
}


static WORD commandconfig(struct FMConfig *ofmc,struct CMenuConfig *cmc)
{
struct GUIBase *gb;
struct OwnCommand *oc;
struct FMConfig *fmc;
void *moreconfigs[TOTALCOMMANDS];
WORD apu1,retcode;
WORD pri,sw,cd,frs,fre,res;
UBYTE front,back;
LONG stack,timeout;
UBYTE vara1[100];

apu1=-1;
retcode=0;
if(!(fmc=newconfig(ofmc))) return(0);
if(!savecommands(fmc,moreconfigs)) goto error;

sformat(vara1,"%s (%s)",getstring(MSG_CONFIG_NAME),findbuildstring(cmc));
getconfig(cmc);

gb=getguibase(vara1);
setconnectgroup(gb,10,0,0);
if(cmc->cmenucount==100) oc=cmc->moreconfig; else oc=0;

setconnectgroup(gb,1,1,10);
reqinfomsg(gb,MSG_CONFIG_COMKEY,100,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_COMLABEL,101,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_COMFRONTPEN,102,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_COMBACKPEN,103,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_COMPRI,104,guiUC|guiLEFT);
setconnectgroup(gb,2,1,10);
reqstring(gb,100,cmc->shortcut,sizeof(cmc->shortcut));
reqstring(gb,101,cmc->label,sizeof(cmc->label));
front=cmc->frontpen;
reqpalette(gb,102,&front);
back=cmc->backpen;
reqpalette(gb,103,&back);
pri=cmc->priority;
reqcyclemsg(gb,104,&pri,MSG_CONFIG_COMLOWPRI,MSG_CONFIG_COMMEDPRI,MSG_CONFIG_COMHIGHPRI,0);

if(cmc->cmenucount==100) {
	setguigroup(gb,1,0);
	reqinfomsg(gb,MSG_EXE_SHELLWINDOW,110,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_EXE_CONFIG_CD,111,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_EXE_CONFIG_RESCAN,112,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_EXE_CONFIG_STARTSCREEN,113,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_EXE_CONFIG_ENDSCREEN,114,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_CONFIG_COMPATTERN,115,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_CONFIG_COMSIGNATURE,116,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_CONFIG_COMCOMMAND,117,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_CONFIG_COMSTACK,118,guiUC|guiLEFT);
	reqinfomsg(gb,MSG_EXE_TIMEOUT,119,guiUC|guiLEFT);
	setguigroup(gb,2,0);
	sw=oc->ef.shellwindow;
	reqcyclemsg(gb,110,&sw,MSG_NON,MSG_YESN,0);
	cd=oc->ef.cd;
	reqcyclemsg(gb,111,&cd,MSG_EXE_CONFIG_NOCHANGE,MSG_EXE_CONFIG_SOURCE,MSG_EXE_CONFIG_DESTINATION,0);
	res=oc->ef.rescan;
	reqcyclemsg(gb,112,&res,MSG_EXE_CONFIG_NOCHANGE,MSG_EXE_CONFIG_SOURCE,MSG_EXE_CONFIG_DESTINATION,MSG_EXE_CONFIG_SOURCEDEST,0);
	frs=oc->ef.startfront;
	reqcyclemsg(gb,113,&frs,MSG_EXE_CONFIG_NOCHANGE,MSG_EXE_CONFIG_FMFRONT,MSG_EXE_CONFIG_WBFRONT,0);
	fre=oc->ef.endfront;
	reqcyclemsg(gb,114,&fre,MSG_EXE_CONFIG_NOCHANGE,MSG_EXE_CONFIG_FMFRONT,MSG_EXE_CONFIG_WBFRONT,0);
	reqstring(gb,115,oc->namematch,sizeof(oc->namematch));
	reqstring(gb,116,oc->matchbytes,sizeof(oc->matchbytes));
	reqstring(gb,117,oc->command,sizeof(oc->command));
	stack=oc->ef.stack;
	reqinteger(gb,118,&stack,4,999999999);
	timeout=oc->ef.timeout;
	reqinteger(gb,119,&timeout,-1,99);
}

apu1=-1;
switch(cmc->cmenucount)
	{
	case SLEEPCONFIG:
	apu1=sleepconfig(gb,cmc);
	break;
	case QUITCONFIG:
	apu1=quitconfig(gb,cmc);
	break;
	case PARENTCONFIG:
	apu1=parentconfig(gb,cmc);
	break;
	case COPYCONFIG:
	case COPYASCONFIG:
	case MOVECONFIG:
	apu1=copyconfig(gb,cmc);
	break;
	case DELETECONFIG:
	apu1=deleteconfig(gb,cmc);
	break;
	case SHOWASCCONFIG:
	case SHOWHEXCONFIG:
	apu1=showtextconfig(gb,cmc);
	break;
	case EXECUTECONFIG:
	apu1=executeconfig(gb,cmc);
	break;
	case HEARCONFIG:
	apu1=hearconfig(gb,cmc);
	break;
	case DISKEDITCONFIG:
	apu1=diskeditconfig(gb,cmc);
	break;
}
if(apu1==-1) {
	commandanswer(gb);
	apu1=quickreq(gb);
}
if(apu1==1) {
	retcode=1;
	cmc->frontpen=front;
	cmc->backpen=back;
	cmc->priority=pri;
	CopyMem(cmc,&fmc->cmenuconfig[cmc-ofmc->cmenuconfig],sizeof(struct CMenuConfig));
	if(oc) {
		oc->ef.shellwindow=sw;
		oc->ef.cd=cd;
		oc->ef.rescan=res;
		oc->ef.startfront=frs;
		oc->ef.endfront=fre;
		oc->ef.stack=stack;
		oc->ef.timeout=timeout;
	}
}

error:
if(!retcode) {
	restorecommands(fmc,moreconfigs);
} else {
	freeconfig(fmc,ofmc);
	fmc=0;
}
restorecommands(0,moreconfigs);
freemem(fmc);
return(retcode);
}

#include "oldconfig.h"
static void readoldconfig(UBYTE *rletemp)
{
struct CMenuConfig *cmc;
struct OwnCommand *oc;
struct FMConfig3022 *fmcold;
struct OwnCommand3022 *ocold;
UBYTE *ptr;
WORD cnt1;

fmcold=allocmem(sizeof(struct FMConfig3022));
if(!fmcold) goto error;
rleunpack(rletemp,fmcold,sizeof(struct FMConfig3022));
cmc=&fmconfig->cmenuconfig[BUILDCOMMANDS];
/*
ptr=&fmcold->cmenuconfig[0];
for(cnt1=0;cnt1<MAXCMENUS3022;cnt1++) {
	if(cnt1<4) cmc->position=2; else cmc->position=1;
	cmc->frontpen=ptr[3];
	cmc->backpen=ptr[4];
	cmc->priority=ptr[5];
	ptr+=CMENUCONFIGSIZE3022;
	cmc++;
}
cmc++;
*/
ptr=&fmcold->cmenuownconfig[0];
for(cnt1=0;cnt1<MAXCOMMANDS3022;cnt1++) {
	ocold=&fmcold->owncommand[cnt1];
	if(ocold->gadgetname[0]) {
		cmc->moreconfig=allocmem(sizeof(struct OwnCommand));
		if(cmc->moreconfig) {
			oc=(struct OwnCommand*)cmc->moreconfig;
			cmc->moreconfiglen=sizeof(struct OwnCommand);
			cmc->cmenucount=100;
			cmc->position=1;
			cmc->frontpen=ptr[3];
			cmc->backpen=ptr[4];
			cmc->priority=ptr[5];
			strcpy(cmc->label,ocold->gadgetname);
			strcpy(oc->namematch,ocold->namematch);
			strcpy(oc->matchbytes,ocold->matchbytes);
			strcpy(oc->command,ocold->command);
			oc->ef.stack=ocold->stack;
			cmc++;
		}
	}
	ptr+=CMENUCONFIGSIZE3022;
}
error:
freemem(fmcold);
}

void readconfig(void)
{
BPTR handle=0;
UBYTE *rletemp=0;
UBYTE *ptr1;
UWORD apu2,apu1;
WORD cnt1;
struct CMenuConfig *cmc;
LONG len;
UBYTE *vara2;

D(bug("config.c 650...........\n"));  
handle=Open("ENV:fm.prefs",1005);
if(!handle) handle=Open("ENVARC:fm.prefs",1005);
if(!handle) handle=Open("S:fm.prefs",1005);
if(!handle) goto error;
D(bug("config.c 655...........\n"));  
Seek(handle,0,OFFSET_END);
len=Seek(handle,0,OFFSET_BEGINNING)-6;
vara2=allocmem(HELPCONFIGSIZE);
rletemp=allocmem(len);
if(!rletemp||!vara2) goto error;

Read(handle,&apu2,2);
if(apu2!=('F' << 8 | 'M')) goto error;
Read(handle,&apu2,2);
if(apu2!=OLDCONFIGVERSION&&apu2!=FMCONFIGVERSION) {
	errorreq(MSG_MAIN_WARNING,getstring(MSG_CONFIG_OLDCONFIG),0,MSG_OK);
	goto error;
}
Read(handle,&apu1,2);
if(Read(handle,rletemp,len)!=len) goto error;
if(apu2==OLDCONFIGVERSION) {
	readoldconfig(rletemp);
} else {
	ptr1=rleunpack(rletemp,fmconfig,apu1);
	for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
		cmc=&fmconfig->cmenuconfig[cnt1];
		ptr1=rleunpack(ptr1,vara2,HELPCONFIGSIZE);
		if(cmc->moreconfiglen) {
			cmc->moreconfig=allocvec(0,cmc->moreconfiglen,MEMF_CLEAR|MEMF_PUBLIC);
			if(!cmc->moreconfig) goto error;
			CopyMem(vara2,cmc->moreconfig,cmc->moreconfiglen);
		}
	}
}
error:
D(bug("config.c 686...........\n"));  
if(handle) Close(handle);
freemem(rletemp);
#ifndef AROS
freemem(vara2); //broken wall??
#endif
}

void writeconfig(struct FMConfig *fmc)
{
BPTR handle;
LONG size;
UBYTE *rletemp,*ptr1;
UBYTE ident[6];
WORD apu1,cnt1;
void *moreconfigs[TOTALCOMMANDS];
struct CMenuConfig *cmc;
UBYTE empty[HELPCONFIGSIZE];
UBYTE vara2[HELPCONFIGSIZE];

ident[0]='F';
ident[1]='M';
ident[2]=0;
ident[3]=FMCONFIGVERSION;
ident[4]=(UBYTE)((sizeof(struct FMConfig)>>8)&0xff);
ident[5]=(UBYTE)((sizeof(struct FMConfig))&0xff);

memseti(empty,0,HELPCONFIGSIZE);
fmmain.myproc->pr_WindowPtr=(APTR)-1;
for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
	cmc=&fmc->cmenuconfig[cnt1];
	moreconfigs[cnt1]=cmc->moreconfig;
	cmc->moreconfig=0;
}
fmc->txtfontattr.ta_Name=0;
fmc->listfontattr.ta_Name=0;
fmc->reqfontattr.ta_Name=0;
fmc->txtshowfontattr.ta_Name=0;
fmc->smallfontattr.ta_Name=0;

rletemp=allocmem(sizeof(struct FMConfig)+TOTALCOMMANDS*HELPCONFIGSIZE);
if(!rletemp) goto error;
size=rlepack(fmc,rletemp,sizeof(struct FMConfig));
for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
	cmc=&fmc->cmenuconfig[cnt1];
	if(cmc->moreconfiglen) {
		memseti(vara2,0,HELPCONFIGSIZE);
		CopyMem(moreconfigs[cnt1],vara2,cmc->moreconfiglen);
		ptr1=vara2;
	} else {
		ptr1=empty;
	}
	size+=rlepack(ptr1,rletemp+size,HELPCONFIGSIZE);
}
apu1=0;
DeleteFile("ENV:fm.prefs");
DeleteFile("ENVARC:fm.prefs");
DeleteFile("S:fm.prefs");
handle=Open("ENV:fm.prefs",1006);
if(handle) {
	Write(handle,ident,sizeof(ident));
	Write(handle,rletemp,size);
	Close(handle);
}
handle=Open("ENVARC:fm.prefs",1006);
if(handle) {
	Write(handle,ident,sizeof(ident));
	Write(handle,rletemp,size);
	Close(handle);
	apu1=1;
}
if(!apu1) {
	handle=Open("S:fm.prefs",1006);
	if(handle) {
		Write(handle,ident,sizeof(ident));
		Write(handle,rletemp,size);
		Close(handle);
	}
}
error:
for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
	fmc->cmenuconfig[cnt1].moreconfig=moreconfigs[cnt1];
}
fmc->txtfontattr.ta_Name=fmc->txtfontname;
fmc->listfontattr.ta_Name=fmc->listfontname;
fmc->reqfontattr.ta_Name=fmc->reqfontname;
fmc->txtshowfontattr.ta_Name=fmc->txtshowfontname;
fmc->smallfontattr.ta_Name=fmc->smallfontname;
fmmain.myproc->pr_WindowPtr=fmmain.ikkuna;
freemem(rletemp);
}

void commandanswer(struct GUIBase *gb)
{
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
}

WORD allocconfig(struct CMenuConfig *cmc,WORD len)
{
if(cmc->moreconfig) return(-1);
cmc->moreconfig=allocvec(0,len,MEMF_CLEAR|MEMF_PUBLIC);
if(!cmc->moreconfig) return(0);
cmc->moreconfiglen=len;
return(1);
}

struct CMenuConfig *getconfignumber(WORD num)
{
WORD cnt;
struct CMenuConfig *cmc;
for(cnt=0;cnt<TOTALCOMMANDS;cnt++) {
	cmc=&fmconfig->cmenuconfig[cnt];
	if(cmc->cmenucount==num) {
		getconfig(cmc);
		return(cmc);
	}
}
return(0);
}

void *getconfig(struct CMenuConfig *cmc)
{
if(!cmc->moreconfig) {
	switch(cmc->cmenucount)
	{
	case 100:
	ownexecuteconfigdefault(cmc);
	break;
	case INCLUDECONFIG:
	case PARSECONFIG:
	case EXCLUDECONFIG:
	parseconfigdefault(cmc);
	break;
	case SLEEPCONFIG:
	sleepconfigdefault(cmc);
	break;
	case QUITCONFIG:
	quitconfigdefault(cmc);
	break;
	case PARENTCONFIG:
	parentconfigdefault(cmc);
	break;
	case COPYCONFIG:
	case COPYASCONFIG:
	case MOVECONFIG:
	copyconfigdefault(cmc);
	break;
	case DELETECONFIG:
	deleteconfigdefault(cmc);
	break;
	case SHOWASCCONFIG:
	case SHOWHEXCONFIG:
	showtextconfigdefault(cmc);
	break;
	case EXECUTECONFIG:
	executeconfigdefault(cmc);
	break;
	case HEARCONFIG:
	hearconfigdefault(cmc);
	break;
	case DISKEDITCONFIG:
	diskeditconfigdefault(cmc);
	break;
	}
}
return(cmc->moreconfig);
}
#ifdef __AROS__
AROS_UFH3(APTR, dlldisplayhookfunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct lvRender *, lvr, A1))
    
#else
__saveds __asm APTR dlldisplayhookfunc(register __a0 struct Hook *hook,register __a2 Object *obj,register __a1 struct lvRender *lvr)
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
   
UBYTE *str = NULL;
struct DirListLayout *dll=(struct DirListLayout*)lvr->lvr_Entry;
switch(dll->type)
{
case 0:
str=getstring(MSG_PARSE_SIZEMATCH);
break;
case 1:
str=getstring(MSG_PARSE_NAMEMATCH);
break;
case 2:
str=getstring(MSG_PARSE_PROTMATCH);
break;
case 3:
str=getstring(MSG_PARSE_DATEMATCH);
break;
case 4:
str=getstring(MSG_PARSE_COMMENTMATCH);
break;
}
return(str);

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifdef __AROS__
AROS_UFH3(APTR, dllresourcehookfunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct lvResource *, lvr, A1))
#else
__saveds __asm APTR dllresourcehookfunc(register __a0 struct Hook *hook,register __a2 Object *obj,register __a1 struct lvResource *lvr)
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif

struct DirListLayout *dll=0;

switch(lvr->lvr_Command)
{
case LVRC_MAKE:
dll=AllocVec(sizeof(struct DirListLayout),0);
if(dll) {
	CopyMem(lvr->lvr_Entry,dll,sizeof(struct DirListLayout));
}
break;
case LVRC_KILL:
FreeVec(lvr->lvr_Entry);
break;
}
return(dll);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

struct Hook dllresourcehook = { {NULL, NULL}, (HOOKFUNC)dllresourcehookfunc, NULL, NULL };
struct Hook dlldisplayhook  = { {NULL, NULL}, (HOOKFUNC)dlldisplayhookfunc,  NULL, NULL };

struct DirListLayout *setdllgads(struct GUIBase *gb,struct GUISlot *gs)
{
IPTR data;
struct DirListLayout *dll;

GetAttr(LISTV_LastClicked,gs->obj,&data);
dll=(struct DirListLayout*)data;
setobject(gb,10,dll->width);
setobject(gb,11,dll->rightaligned);
setobject(gb,12,dll->rightlock);
return(dll);
}

WORD dirlayoutconfig(struct FMConfig *ofmc)
{
struct FMConfig *fmc;
struct GUIBase *gb;
struct GUISlot *gs1;
struct DirListLayout *dll;
Class *ddlistclass;
WORD cnt1;
WORD lock,align;
LONG fw;
WORD ret;

if(!(fmc=newconfig(ofmc))) return(0);

gb=getguibase(getstring(MSG_CONFIG_NAME));
setconnectgroup(gb,1,0,0);

ddlistclass=InitFLClass();

gs1=getguislot(gb,NewObject(ddlistclass,0,
	GA_ID,20,
	LAB_Label,getstring(MSG_CONFIG_LAYOUT_DIRLIST),
	LAB_Place,PLACE_ABOVE,
	LISTV_ShowDropSpot,TRUE,
	LISTV_MinEntriesShown,8,
	LISTV_SortEntryArray,FALSE,
	LISTV_ListFont,&fmc->listfontattr,
	LISTV_ResourceHook,&dllresourcehook,
	LISTV_DisplayHook,&dlldisplayhook,
	BT_DragObject,TRUE,
	BT_DropObject,TRUE,
	TAG_DONE),20,LISTVIEW_KIND,0);

setconnectgroup(gb,2,1,1);
if(!gs1) goto error;

fw=0;align=0;lock=0;

reqinfomsg(gb,MSG_CONFIG_LAYOUT_WIDTH,10,guiUC|guiCENTER);
reqinteger(gb,10,&fw,1,99);
reqinfomsg(gb,MSG_CONFIG_LAYOUT_RIGHTALIGNMENT,11,guiUC|guiCENTER);
reqcycle2msg(gb,11,&align);
reqinfomsg(gb,MSG_CONFIG_LAYOUT_RIGHTLOCK,12,guiUC|guiCENTER);
reqcycle2msg(gb,12,&lock);

for(cnt1=0;cnt1<5;cnt1++) {
	dll=&fmc->dl[cnt1];
	BGUI_DoGadgetMethod(gs1->obj,0,0,LVM_ADDSINGLE,0,dll,LVAP_TAIL,0);
}
SetAttrs(gs1->obj,LISTV_Select,0,TAG_DONE);

buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
if(openinitreq(gb)) {
	dll=setdllgads(gb,gs1);
	for(;;) {
		ret=reqmsghandler(gb);
		if(ret==1) {
			closereq(gb);
			dll=(struct DirListLayout*)DoMethod(gs1->obj,LVM_FIRSTENTRY,0,0,TAG_DONE);
			cnt1=0;
			do {
				CopyMem(dll,&fmc->dl[cnt1],sizeof(struct DirListLayout));
				dll=(struct DirListLayout*)DoMethod(gs1->obj,LVM_NEXTENTRY,(IPTR)dll,0,TAG_DONE);
				cnt1++;
			} while(dll);
			freeconfig(fmc,ofmc);
			fmc=0;
		}
		if(ret<2) break;
		switch(ret)
		{
		case 20:
		dll=setdllgads(gb,gs1);
		break;
		case 10:
		dll->width=fw;
		break;
		case 11:
		dll->rightaligned=align;
		break;
		case 12:
		dll->rightlock=lock;
		break;
		}
	}
}
error:
freereq(gb);
if(ddlistclass) FreeClass(ddlistclass);
freemem(fmc);
return(0);
}


/****************************
** 	SCREEN
****************************/

WORD screenconfig(struct FMConfig *ofmc)
{
struct GUIBase *gb;
WORD c;
WORD apu1,apu2;
WORD refresh=0;
UBYTE *ptr1,*ptr2;
WORD stype,windowed;
LONG flags;
UBYTE *buf=0;
struct FMConfig *fmc=0;

if(!(fmc=newconfig(ofmc))) goto error;;
if(!(buf=allocmem(1000))) goto error;

for(;;) {
	ptr1=buf;
	gb=getguibase(getstring(MSG_CONFIG_NAME));

	sformatmsg(ptr1,MSG_CONFIG_SCRFORMAT,getstring(MSG_CONFIG_MAINSCREEN),fmc->mainscreen.width,fmc->mainscreen.height,fmc->mainscreen.depth,fmc->mainscreen.screenmode);
	reqbutton(gb,ptr1,10,guiUC|guiCENTER);
	ptr1+=strlen(ptr1)+1;
	sformatmsg(ptr1,MSG_CONFIG_SCRFORMAT,getstring(MSG_CONFIG_TEXTSCREEN),fmc->textscreen.width,fmc->textscreen.height,fmc->textscreen.depth,fmc->textscreen.screenmode);
	reqbutton(gb,ptr1,11,guiUC|guiCENTER);
	ptr1+=strlen(ptr1)+1;

	sformatmsg(ptr1,MSG_CONFIG_FONTFORMAT,getstring(MSG_CONFIG_TEXTFONT),fmc->txtfontattr.ta_Name,fmc->txtfontattr.ta_YSize);
	reqbutton(gb,ptr1,12,guiUC|guiCENTER);
	ptr1+=strlen(ptr1)+1;
	sformatmsg(ptr1,MSG_CONFIG_FONTFORMAT,getstring(MSG_CONFIG_LISTFONT),fmc->listfontattr.ta_Name,fmc->listfontattr.ta_YSize);
	reqbutton(gb,ptr1,13,guiUC|guiCENTER);
	ptr1+=strlen(ptr1)+1;
	sformatmsg(ptr1,MSG_CONFIG_FONTFORMAT,getstring(MSG_CONFIG_REQFONT),fmc->reqfontattr.ta_Name,fmc->reqfontattr.ta_YSize);
	reqbutton(gb,ptr1,14,guiUC|guiCENTER);
	ptr1+=strlen(ptr1)+1;
	sformatmsg(ptr1,MSG_CONFIG_FONTFORMAT,getstring(MSG_CONFIG_SHOWFONT),fmc->txtshowfontattr.ta_Name,fmc->txtshowfontattr.ta_YSize);
	reqbutton(gb,ptr1,15,guiUC|guiCENTER);
	ptr1+=strlen(ptr1)+1;

	ptr2=ptr1;

	ptr1=stpcpy(ptr1,getstring(MSG_CONFIG_LAYOUT));

	*ptr1++=32;
	apu1=0;
	while(fmc->listinfo[apu1][0]) {
		apu2=1;
		while(fmc->listinfo[apu1][apu2]&0x7f) apu2++;
		if(apu1) *ptr1++='/';
		*ptr1++=apu2-1+'0';
		apu1++;
	}
	*ptr1++=0;
	reqbutton(gb,ptr2,16,guiUC|guiCENTER);

	reqbuttonmsg(gb,MSG_CONFIG_DIRLAYOUT,17,guiUC|guiCENTER);

	reqbuttonmsg(gb,MSG_CONFIG_PALETTE,18,guiUC|guiCENTER);

	setconnectgroup(gb,9,0,0);

	setconnectgroup(gb,1,1,9);
	reqinfomsg(gb,MSG_CONFIG_SCREENWINDOW,90,guiUC|guiLEFT);
	stype=fmc->screentype;
	reqcyclemsg(gb,90,&stype,MSG_CONFIG_SCREENTYPE1,MSG_CONFIG_SCREENTYPE2,MSG_CONFIG_SCREENTYPE3,0);
	setconnectgroup(gb,2,1,9);
	windowed=(fmc->flags&MWINDOWED)?1:0;
	reqcycle2msg(gb,100,&windowed);
	reqstring(gb,101,fmc->pubscreen,sizeof(fmc->pubscreen));

	buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);

	c=0;
	if(openinitreq(gb)) {
		do {
			apu1=getobject(gb,90);
			if(apu1==2) denaobject(gb,101,0); else denaobject(gb,101,1);
			c=reqmsghandler(gb);
		} while(c>=90);
		closereq(gb);
	}

	switch(c)
	{
	case 10:
	refresh=askscreen(&fmc->mainscreen,MSG_CONFIG_MAINSCREEN,TRUE);
	break;
	case 11:
	refresh=askscreen(&fmc->textscreen,MSG_CONFIG_TEXTSCREEN,FALSE);
	break;
	case 12:
	refresh=askfont(&fmc->txtfontattr,MSG_CONFIG_TEXTFONT,TRUE);
	break;
	case 13:
	refresh=askfont(&fmc->listfontattr,MSG_CONFIG_LISTFONT,FALSE);
	break;
	case 14:
	refresh=askfont(&fmc->reqfontattr,MSG_CONFIG_REQFONT,TRUE);
	break;
	case 15:
	refresh=askfont(&fmc->txtshowfontattr,MSG_CONFIG_SHOWFONT,FALSE);
	break;
	case 16:
/*	refresh=layoutconfig(fmc); */
	break;
	case 17:
	refresh=dirlayoutconfig(fmc);
	break;
	case 18:
	refresh=palette(fmc);
	break;
	}
	flags=fmc->flags;
	fmc->flags&=~(MWINDOWED|MVSCROLL|MHSCROLL);
	if (windowed) fmc->flags|=MWINDOWED;

	if(flags!=fmc->flags) refresh=3;
	if(fmc->screentype!=stype) refresh=3;
	fmc->screentype=stype;
	if(c==1) {
		if(!refresh) refresh=1;
		freeconfig(fmc,ofmc);
		fmc=0;
	}
	if(c<10) break;
}
error:
freemem(fmc);
freemem(buf);
return(refresh);
}

/* COMMAND CONFIG */

struct cmenudata {
	UBYTE name[32];
	struct CMenuConfig *cmc;
};

static void rescanbuttonconfig(struct FMConfig *fmc,struct GUISlot *gs[])
{
struct cmenudata *cmd;
struct CMenuConfig *cmcbuf;
struct CMenuConfig *cmc;
WORD cnt1;
APTR entry;

cmcbuf=allocmem(TOTALCOMMANDS*sizeof(struct CMenuConfig));
if(!cmcbuf) return;
cnt1=0;
cmc=cmcbuf;
/*  CORNER */
if((entry=(APTR)DoMethod(gs[2]->obj,LVM_FIRSTENTRY,NULL,0L))) {
	do {
		cmd=(struct cmenudata*)entry;
		CopyMem(cmd->cmc,cmc,sizeof(struct CMenuConfig));
		cmc->position=2;
		cmc++;
		cnt1++;
		entry=(APTR)DoMethod(gs[2]->obj,LVM_NEXTENTRY,(IPTR)entry,0L);
	} while(entry);
}
/*  BAR */
if((entry=(APTR)DoMethod(gs[1]->obj,LVM_FIRSTENTRY,NULL,0L))) {
	do {
		cmd=(struct cmenudata*)entry;
		CopyMem(cmd->cmc,cmc,sizeof(struct CMenuConfig));
		cmc->position=1;
		cmc++;
		cnt1++;
		entry=(APTR)DoMethod(gs[1]->obj,LVM_NEXTENTRY,(IPTR)entry,0L);
	} while(entry);
}
/*  HIDDEN */
if((entry=(APTR)DoMethod(gs[3]->obj,LVM_FIRSTENTRY,NULL,0L))) {
	do {
		cmd=(struct cmenudata*)entry;
		CopyMem(cmd->cmc,cmc,sizeof(struct CMenuConfig));
		cmc->position=9;
		cmc++;
		cnt1++;
		entry=(APTR)DoMethod(gs[3]->obj,LVM_NEXTENTRY,(IPTR)entry,0L);
	} while(entry);
}
while(cnt1<TOTALCOMMANDS) {
	memseti(cmc,0,sizeof(struct CMenuConfig));
	cmc->position=0xff;
	cmc++;
	cnt1++;
}
CopyMem(cmcbuf,fmc->cmenuconfig,TOTALCOMMANDS*sizeof(struct CMenuConfig));
freemem(cmcbuf);
}

static struct CMenuConfig *findnewcmc(struct FMConfig *fmc)
{
struct CMenuConfig *cmc;
WORD cnt1;

for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
	cmc=&fmc->cmenuconfig[cnt1];
	if(cmc->position==0xff) break;
}
if(cnt1==TOTALCOMMANDS) {
	cmc=0;
	request(getstring(MSG_CONFIG_NAME),MSG_OK,0,getstring(MSG_CONFIG_COMSLOTSFULL));
}
if(cmc) {
	cmc->priority=1;
	cmc->frontpen=fmc->txtpen;
	cmc->backpen=fmc->backpen;
}
return(cmc);
}

static void newbuildin(struct FMConfig *fmc,struct CMenuConfig *cmc)
{
struct CMenuConfig *newcmc;
WORD cnt1;

newcmc=findnewcmc(fmc);
if(!newcmc) return;
cnt1=findbuild(cmc);
if(cnt1) {
	newcmc->position=cmc->position;
	newcmc->cmenucount=cnt1;
	memcpy(newcmc->label,cmc->label,sizeof(cmc->label));
	newcmc->label[sizeof(cmc->label)-1]=0;
	commandconfig(fmc,newcmc);
} else {
	newcmc->position=0xff;
}
}

WORD findbuild(struct CMenuConfig *cmc)
{
WORD cnt1;
UBYTE *ptr1;

ptr1=getstring(MSG_MAIN_CMENULABEL);
cnt1=1;
while(*ptr1) {
	if(!strcmp(ptr1,cmc->label)) return(cnt1);
	cnt1++;
	ptr1+=strlen(ptr1)+1;
}
return(0);
}
UBYTE *findbuildstring(struct CMenuConfig *cmc)
{
UBYTE *ptr1;
WORD cnt1;

ptr1=getstring(MSG_MAIN_CMENULABEL);
cnt1=cmc->cmenucount;
if(cnt1>=100) return(getstring(MSG_CONFIG_COMUSERCOMMAND));
cnt1--;
while(cnt1--) ptr1+=strlen(ptr1)+1;
return(ptr1);
}

static void newuser(struct FMConfig *fmc)
{
struct CMenuConfig *newcmc;

newcmc=findnewcmc(fmc);
if(!newcmc) return;
if(!ownexecuteconfigdefault(newcmc)) return;
newcmc->cmenucount=100;
newcmc->position=1;
commandconfig(fmc,newcmc);
}

static void gsdoubleclick(struct FMConfig *fmc,struct GUISlot *gs[],struct GUISlot *obj,void *data)
{
struct CMenuConfig cmc2,*cmc3,*cmc;
struct GUIBase *gb;
WORD flags,ret,buildin;
UBYTE vara[80];

flags=buildin=0;
if(obj==gs[0]) {
	memseti(&cmc2,0,sizeof(struct CMenuConfig));
	strcpy(cmc2.label,(UBYTE*)data);
	cmc=&cmc2;
	flags=8|16;
	buildin=1;
} else {
	cmc=((struct cmenudata*)data)->cmc;
}
if(obj==gs[1]) flags=2|4|128|32;
if(obj==gs[2]) flags=1|4|128|32;
if(obj==gs[3]) flags=1|2|128|32;

sformat(vara,"%s ('%s')",getstring(MSG_CONFIG_NAME),cmc->label);
gb=getguibase(vara);
setconnectgroup(gb,1,1,0);
if(!buildin) reqbuttonmsg(gb,MSG_CONFIG_COMEDITCONFIG,4,0);
if(flags&1) reqbuttonmsg(gb,MSG_CONFIG_COMMOVEGADGETBAR,5,0);
if(flags&2) reqbuttonmsg(gb,MSG_CONFIG_COMMOVECORNERMENU,6,0);
if(flags&4) reqbuttonmsg(gb,MSG_CONFIG_COMMOVEHIDDEN,7,0);
if(flags&8) reqbuttonmsg(gb,MSG_CONFIG_COMCOPYGADGETBAR,5,0);
if(flags&16) reqbuttonmsg(gb,MSG_CONFIG_COMCOPYCORNERMENU,6,0);
if(flags&32) reqbuttonmsg(gb,MSG_CONFIG_COMDUPLICATECOMMAND,3,0);
if(flags&128) reqbuttonmsg(gb,MSG_CONFIG_COMREMOVECOMMAND,8,0);
reqbuttonmsg(gb,MSG_CONFIG_COMNEWUSERCOMMAND,9,0);
buttonbarmsg(gb,MSG_OK,1,0);
ret=quickreq(gb);
switch(ret)
	{
	case 3:
	cmc3=findnewcmc(fmc);
	if(cmc3) {
		CopyMem(cmc,cmc3,sizeof(struct CMenuConfig));
		if(cmc->moreconfig) {
			cmc3->moreconfig=allocmem(cmc->moreconfiglen);
			if(cmc3->moreconfig) CopyMem(cmc->moreconfig,cmc3->moreconfig,cmc->moreconfiglen);
		}
	}
	break;
	case 4:
	commandconfig(fmc,cmc);
	break;
	case 5:
	cmc->position=1;
	if(buildin) newbuildin(fmc,cmc);
	break;
	case 6:
	cmc->position=2;
	if(buildin) newbuildin(fmc,cmc);
	break;
	case 7:
	cmc->position=9;
	break;
	case 8:
	cmc->position=0xff;
	freemem(cmc->moreconfig);
	cmc->moreconfig=0;
	cmc->moreconfiglen=0;
	break;
	case 9:
	newuser(fmc);
	break;
}
}

static void refreshcommands(struct FMConfig *fmc,struct GUIBase *gb,struct GUISlot *gs[])
{
struct CMenuConfig *cmc;
WORD cnt1;
UBYTE *ptr1;
UBYTE bap1;

BGUI_DoGadgetMethod(gs[0]->obj,0,0,LVM_CLEAR,0);
BGUI_DoGadgetMethod(gs[1]->obj,0,0,LVM_CLEAR,0);
BGUI_DoGadgetMethod(gs[2]->obj,0,0,LVM_CLEAR,0);
BGUI_DoGadgetMethod(gs[3]->obj,0,0,LVM_CLEAR,0);
ptr1=getstring(MSG_MAIN_CMENULABEL);
for(cnt1=0;cnt1<BUILDCOMMANDS;cnt1++) {
	BGUI_DoGadgetMethod(gs[0]->obj,0,0,LVM_ADDSINGLE,0,ptr1,LVAP_SORTED,0);
	ptr1+=strlen(ptr1)+1;
}
for(cnt1=0;cnt1<TOTALCOMMANDS;cnt1++) {
	cmc=&fmc->cmenuconfig[cnt1];
	bap1=cmc->position;
	if(bap1==1) {
		BGUI_DoGadgetMethod(gs[1]->obj,0,0,LVM_ADDSINGLE,0,cmc,LVAP_TAIL,0);
	} else if(bap1==2) {
		BGUI_DoGadgetMethod(gs[2]->obj,0,0,LVM_ADDSINGLE,0,cmc,LVAP_TAIL,0);
	} else if(bap1==9) {
		BGUI_DoGadgetMethod(gs[3]->obj,0,0,LVM_ADDSINGLE,0,cmc,LVAP_TAIL,0);
	}
}
BGUI_DoGadgetMethod(gs[0]->obj,gb->win,0,LVM_REFRESH,0);
BGUI_DoGadgetMethod(gs[1]->obj,gb->win,0,LVM_REFRESH,0);
BGUI_DoGadgetMethod(gs[2]->obj,gb->win,0,LVM_REFRESH,0);
BGUI_DoGadgetMethod(gs[3]->obj,gb->win,0,LVM_REFRESH,0);
}
#ifdef __AROS__
AROS_UFH3(APTR, comdisplayhookfunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct lvRender *, lvr, A1))
#else
__saveds __asm APTR comdisplayhookfunc(register __a0 struct Hook *hook,register __a2 Object *obj,register __a1 struct lvRender *lvr)
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
struct cmenudata *cmd=(struct cmenudata*)lvr->lvr_Entry;
return(cmd->name);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifdef __AROS__
AROS_UFH3(APTR, comresourcehookfunc,
    AROS_UFHA(struct Hook *, hook, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(struct lvResource *, lvr, A1))
#else
__saveds __asm APTR comresourcehookfunc(register __a0 struct Hook *hook,register __a2 Object *obj,register __a1 struct lvResource *lvr)
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif

struct CMenuConfig *cmc;
struct cmenudata *cmd = NULL;
UBYTE *vara;

switch(lvr->lvr_Command)
{
case LVRC_MAKE:
cmd=AllocVec(sizeof(struct cmenudata),0);
if(cmd) {
	cmc=(struct CMenuConfig*)lvr->lvr_Entry;
	cmd->cmc=cmc;
	vara=cmd->name;
	vara[0]='(';
	if(cmc->cmenucount==100) {
		vara[1]=getstring(MSG_CONFIG_COMBUILDUSERKEY)[1];
	} else {
		vara[1]=getstring(MSG_CONFIG_COMBUILDUSERKEY)[0];
	}
	vara[2]=')';
	vara[3]=' ';
	strcpy(vara+4,cmc->label);
}
break;
case LVRC_KILL:
FreeVec(lvr->lvr_Entry);
break;
}
return(cmd);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}


struct Hook comresourcehook = { {NULL, NULL}, (HOOKFUNC)comresourcehookfunc, NULL, NULL };
struct Hook comdisplayhook  = { {NULL, NULL}, (HOOKFUNC)comdisplayhookfunc,  NULL, NULL };

WORD commands(struct FMConfig *ofmc)
{
struct FMConfig *fmc;
Object *obj;
ULONG ds[2],dm[2],last = 0;
IPTR clicked;
Class *ddlistclass;
struct GUIBase *gb = NULL;
struct GUISlot *gs[4];
WORD ret = 0,cnt1;
WORD cmenutype,cmenucolumns;
void *moreconfigs[TOTALCOMMANDS];
UBYTE msg[20];

if(!(fmc=newconfig(ofmc))) return(0);
if(!savecommands(fmc,moreconfigs)) goto error;

ddlistclass=InitFLClass();

gb=getguibase(getstring(MSG_CONFIG_NAME));

setconnectgroup(gb,1,0,0);

gs[0]=getguislot(gb,NewObject(ddlistclass,0,
	GA_ID,20,
	LAB_Label,getstring(MSG_CONFIG_COMBUILDINCOMMANDS),
	LAB_Place,PLACE_ABOVE,
	LISTV_MinEntriesShown,20,
	LISTV_SortEntryArray,TRUE,
	LISTV_ListFont,&fmc->listfontattr,
	BT_DragObject,TRUE,
	BT_DropObject,FALSE,
	TAG_DONE),0,LISTVIEW_KIND,0);
gs[1]=getguislot(gb,NewObject(ddlistclass,0,
	GA_ID,21,
	LAB_Label,getstring(MSG_CONFIG_COMBARCOMMANDS),
	LAB_Place,PLACE_ABOVE,
	LISTV_ShowDropSpot,TRUE,
	LISTV_MinEntriesShown,20,
	LISTV_SortEntryArray,FALSE,
	LISTV_ListFont,&fmc->listfontattr,
	LISTV_ResourceHook,&comresourcehook,
	LISTV_DisplayHook,&comdisplayhook,
	BT_DragObject,TRUE,
	BT_DropObject,TRUE,
	TAG_DONE),0,LISTVIEW_KIND,0);
gs[2]=getguislot(gb,NewObject(ddlistclass,0,
	GA_ID,22,
	LAB_Label,getstring(MSG_CONFIG_COMCORNERCOMMANDS),
	LAB_Place,PLACE_ABOVE,
	LISTV_ShowDropSpot,TRUE,
	LISTV_MinEntriesShown,20,
	LISTV_SortEntryArray,FALSE,
	LISTV_ListFont,&fmc->listfontattr,
	LISTV_ResourceHook,&comresourcehook,
	LISTV_DisplayHook,&comdisplayhook,
	BT_DragObject,TRUE,
	BT_DropObject,TRUE,
	TAG_DONE),0,LISTVIEW_KIND,0);
gs[3]=getguislot(gb,NewObject(ddlistclass,0,
	GA_ID,23,
	LAB_Label,getstring(MSG_CONFIG_COMHIDDENCOMMANDS),
	LAB_Place,PLACE_ABOVE,
	LISTV_ShowDropSpot,TRUE,
	LISTV_MinEntriesShown,20,
	LISTV_SortEntryArray,FALSE,
	LISTV_ListFont,&fmc->listfontattr,
	LISTV_ResourceHook,&comresourcehook,
	LISTV_DisplayHook,&comdisplayhook,
	BT_DragObject,TRUE,
	BT_DropObject,TRUE,
	FL_SortDrops,TRUE,
	TAG_DONE),0,LISTVIEW_KIND,0);

if(!gs[0]||!gs[1]||!gs[2]||!gs[3]) goto error;

setconnectgroup(gb,14,0,0);
setconnectgroup(gb,2,1,14);
reqinfomsg(gb,MSG_CONFIG_LAYOUT_CMENUALIGNMENT,110,guiUC|guiLEFT);
reqinfomsg(gb,MSG_CONFIG_LAYOUT_CMENUSIZE,111,guiUC|guiLEFT);
setconnectgroup(gb,3,1,14);
cmenutype=fmconfig->cmenutype;
reqcyclemsg(gb,110,&cmenutype,MSG_CONFIG_LAYOUTVER,MSG_CONFIG_LAYOUTHOR,0);
for(cnt1=0;cnt1<9;cnt1++) {
	msg[cnt1*2+0]=cnt1+'1';
	msg[cnt1*2+1]=0;
}
cmenucolumns=fmconfig->cmenucolumns-1;
reqcycle(gb,111,&cmenucolumns,&msg[0],&msg[2],&msg[4],&msg[6],&msg[8],&msg[10],&msg[12],&msg[14],&msg[16],0);

buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
last=0xffffffff;
if(openinitreq(gb)) {
	refreshcommands(fmc,gb,gs);
	for(;;) {
		ret=reqmsghandler(gb);
		if(ret<10) break;
		if(ret>=20&&ret<=23) {
			obj=gs[ret-20]->obj;
			GetAttr(LISTV_LastClicked,obj,&clicked );
			if (clicked==last ) {
				CurrentTime(&ds[1],&dm[1]);
				if (DoubleClick(ds[0],dm[0],ds[1],dm[1])) {
					sleepreq(gb);
					rescanbuttonconfig(fmc,gs);
					gsdoubleclick(fmc,gs,gs[ret-20],(void*)clicked);
					refreshcommands(fmc,gb,gs);
					wakereq(gb);
				}
			}
			CurrentTime(&ds[0],&dm[0]);
			last = clicked;
		}
	}
}
rescanbuttonconfig(fmc,gs);
refreshcommands(fmc,gb,gs);
error:
freereq(gb);
if(ddlistclass) FreeClass(ddlistclass);
if(!ret) {
	restorecommands(fmc,moreconfigs);
} else {
	fmc->cmenutype=cmenutype;
	fmc->cmenucolumns=cmenucolumns+1;
	freeconfig(fmc,ofmc);
	fmc=0;
}
restorecommands(0,moreconfigs);
freemem(fmc);
return(ret);
}


WORD config(void)
{
struct GUIBase *gb;
struct FMConfig *fmc;
WORD c=0;
WORD apu1=0,apu2;
#ifndef V39
WORD apu3,apu4;
#endif
WORD refresh=0,retcode=0;
ULONG coltable[COLORS*3];
UBYTE *ptr1,*ptr2;

fmconfig->windowtop=fmmain.ikkuna->TopEdge;
fmconfig->windowleft=fmmain.ikkuna->LeftEdge;
fmconfig->windowheight=fmmain.ikkuna->Height;
fmconfig->windowwidth=fmmain.ikkuna->Width;
if(!(fmc=newconfig(fmconfig))) return(0);

gb=getguibase(getstring(MSG_CONFIG_NAME));
reqbuttonmsg(gb,MSG_CONFIG_SCREEN,10,guiUC|guiCENTER);
reqbuttonmsg(gb,MSG_CONFIG_COMMANDS,11,guiUC|guiCENTER);
reqbuttonmsg(gb,MSG_CONFIG_GENERAL,12,guiUC|guiCENTER);
reqbuttonmsg(gb,MSG_CONFIG_DATATYPES,13,guiUC|guiCENTER);
buttonbarmsg(gb,MSG_OK,1,MSG_SAVE,2,MSG_CANCEL,0,0);
if(openinitreq(gb)) {
	for(;;) {
		c=reqmsghandler(gb);
		if(c==2) {
			apu1=power2(fmc->mainscreen.depth);
			ptr1=fmc->colors;
			ptr2=(UBYTE*)coltable;
	#ifdef V39
			GetRGB32(fmmain.naytto->ViewPort.ColorMap,0,apu1,coltable);
			for(apu2=0;apu2<apu1;apu2++) {
				*ptr1++=*ptr2;
				ptr2+=4;
				*ptr1++=*ptr2;
				ptr2+=4;
				*ptr1++=*ptr2;
				ptr2+=4;
			}	
	#else
			for(apu2=0;apu2<apu1;apu2++) {
				apu3=GetRGB4(fmmain.naytto->ViewPort.ColorMap,apu2);
				apu4=(apu3>>8)&0x0f;
				*ptr1++=apu4<<4|apu4;
				apu4=(apu3>>4)&0x0f;
				*ptr1++=apu4<<4|apu4;
				apu4=(apu3)&0x0f;
				*ptr1++=apu4<<4|apu4;
			}
	#endif
			writeconfig(fmc);
		}
		if(c<10) break;
		sleepreq(gb);
		switch(c)
		{
		case 10:
		refresh=screenconfig(fmc);
		break;
		case 11:
		refresh=commands(fmc);
		break;
		case 12:
		refresh=generalconfig(fmc);
		break;
		case 13:
		refresh=datatypesconfig(fmc);
		break;
		}
		wakereq(gb);
		if(refresh>retcode) retcode=refresh;
	}
	apu1=0;
	if(!c) {
		retcode=0;
	} else {
		apu2=fmconfig->screentype;
		apu1=fmc->screentype;
		freeconfig(fmc,fmconfig);
		fmc=0;
		fmconfig->screentype=apu2;
		fmmain.maxdirlistwidth=fmconfig->dl[0].width+fmconfig->dl[1].width+fmconfig->dl[2].width+fmconfig->dl[3].width+fmconfig->dl[4].width+4;
	}
}
freereq(gb);
freemem(fmc);
fmc=0;
if(ReqToolsBase) CloseLibrary((struct Library*)ReqToolsBase);
ReqToolsBase=0;
return((WORD)(apu1<<8|retcode)); /* 0=no refresh,1=redraw,2=reopen window,3=screen/window reopen,bit7=recalculate all */
}

