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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

void copyus(UBYTE*,UBYTE*);
void outputfmmessage(struct FMList*,UBYTE*,WORD);
UBYTE *scanchar(UBYTE*,UBYTE);
void fittext(struct RastPort*,UBYTE*,WORD,WORD,WORD,WORD,WORD);

extern struct FMConfig *fmconfig;
extern struct FMMain fmmain;

WORD errorreq(WORD title,UBYTE *txt,WORD pos,WORD neg)
{
UBYTE text[100];
UBYTE *ptr1;
struct EasyStruct es;

es.es_StructSize=sizeof(struct EasyStruct);
es.es_Flags=0;
es.es_Title=getstring(title);
es.es_TextFormat=txt;
ptr1=text;
if(pos) {
	copyus(ptr1,getstring(pos));
	ptr1+=strlen(ptr1);
	*ptr1++='|';
}
copyus(ptr1,getstring(neg));
es.es_GadgetFormat=text;
return((WORD)EasyRequest(0,&es,0,0));
}

void fmmessage(struct FMList *list)
{
UBYTE msg1[256],msg2[256];

if (!(list->flags&LSOURCE)) return;
if(AttemptSemaphore(&fmmain.msgsema)) {
	strcpy(msg1,list->fmmessage1);
	strcpy(msg2,list->fmmessage2);
	ReleaseSemaphore(&fmmain.msgsema);
	outputfmmessage(list,msg1,fmmain.messageliney1);
	outputfmmessage(list,msg2,fmmain.messageliney2);
}
}

void outputfmmessage(struct FMList *list,UBYTE *ptr,WORD yy)
{
WORD width;
UBYTE *ptr1;

ptr1=scanchar(ptr,'\n');
if(ptr1) *ptr1=32;
if (fmmain.ikkuna) {
	width=fmmain.ikkuna->Width-fmmain.ikkuna->BorderLeft-fmmain.ikkuna->BorderRight-2*fmconfig->spacew;
	fittext(fmmain.rp,ptr,-1,fmconfig->spacew,yy,width,1);
}
}

WORD request2(UBYTE *title,WORD pos,WORD neg,UBYTE *reqtxt)
{
struct GUIBase *gb;
WORD ret;

gb=getguibase(title);
reqinfo(gb,reqtxt,-1,guiCENTER);
if (pos) {
	buttonbarmsg(gb,pos,1,neg,0,0);
} else {
	buttonbarmsg(gb,neg,0,0);
}
ret=quickreq(gb);
if (pos&&ret==1) return(1);
return(0);
}

WORD requestmsg(UBYTE *title,WORD pos,WORD neg,LONG msg,...)
{
UBYTE reqtxt[200];

va_list args;
va_start(args,msg);
sformatti(reqtxt,getstring(msg),args);
va_end(args);
return(request2(title,pos,neg,reqtxt));
}

WORD request(UBYTE *title,WORD pos,WORD neg,UBYTE *txt,...)
{
UBYTE reqtxt[200];

va_list args;
va_start(args,txt);
sformatti(reqtxt,txt,args);
va_end(args);
return(request2(title,pos,neg,reqtxt));
}

WORD dosrequesti(struct FMList *list,WORD flag,UBYTE *dostxt1)
{
struct GUIBase *gb;
ULONG ioerr;
UBYTE dostxt2[100];
WORD ret;

outputlist(list);
gb=getguibase(getstring(MSG_REQ_DOSERR));
ioerr=IoErr();
Fault(ioerr,0,dostxt2,256);
reqinfo(gb,dostxt1,-1,guiCENTER);
reqinfo(gb,dostxt2,-1,guiCENTER);
startbuttonbar(gb);
if (flag&1) reqbuttonmsg(gb,MSG_RETRY,2,guiUC);
if (flag&2) reqbuttonmsg(gb,MSG_IGNORE,3,guiUC);
if (!flag) reqbuttonmsg(gb,MSG_OK,1,guiUC); else reqbuttonmsg(gb,MSG_CANCEL,0,guiUC);
ret=quickreq(gb);
if (ret==2) return(1);
if (ret==3) return(-1);
return(0);
}

WORD dosrequestmsg(struct FMList *list,WORD flag,SIPTR msg,...)
{
UBYTE dostxt1[200];

va_list args;
va_start(args,msg);
sformatti(dostxt1,getstring(msg),args);
va_end(args);
return(dosrequesti(list,flag,dostxt1));
}

WORD dosrequest(struct FMList *list,WORD flag,UBYTE *txt,...)
{
UBYTE dostxt1[200];

va_list args;
va_start(args,txt);
sformatti(dostxt1,txt,args);
va_end(args);
return(dosrequesti(list,flag,dostxt1));
}


WORD asklong(struct Screen *scr,UBYTE *title,UBYTE *text,LONG *value,LONG min,LONG max)
{
struct GUIBase *gb;
WORD c;

gb=getguibase(title);
if(gb) gb->screen=scr;
setconnectgroup(gb,1,0,0);
reqinfo(gb,text,100,guiLEFT|guiUC);
reqinteger(gb,100,value,min,max);
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
c=quickreq(gb);
return(c);
}

WORD askstring(struct Screen *scr,UBYTE *title,UBYTE *text,UBYTE *string,WORD len)
{
struct GUIBase *gb;
WORD c;

gb=getguibase(title);
if(gb) gb->screen=scr;
setconnectgroup(gb,1,0,0);
reqinfo(gb,text,100,guiLEFT|guiUC);
reqstring(gb,100,string,len);
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
c=quickreq(gb);
if(!c) return(0);
return(1);
}
