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
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <proto/datatypes.h>
#include <strings.h>
#include <stdio.h>
#include "fmnode.h"
#include "fmlocale.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;

void showpicture(struct FMHandle*,struct CMenuConfig*);
WORD showdatatypepicture(struct FMHandle*,struct DataType*,struct CMenuConfig*);
void showicon(struct FMHandle*,struct CMenuConfig*);
void showfont(struct FMHandle*,struct CMenuConfig*);
void modplay(struct FMHandle*,struct CMenuConfig*);
void executefile(struct FMHandle*,struct CMenuConfig*);
void showtext(struct FMHandle*,struct CMenuConfig*);
void playsample(struct FMHandle*,struct CMenuConfig*);
WORD playdatatypesample(struct FMHandle*,struct DataType*,struct CMenuConfig*);
extern struct Library *DataTypesBase;

#define MEMSIZE 2048
extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;

void checkfile(struct FMHandle*,UBYTE*,struct FMNode*);
void executeown(struct FMList*,struct FMList*,struct FMNode*,struct CMenuConfig*);

WORD isdatatypes(struct FMHandle *h,LONG dtype)
{
struct DataType *dtn=0;
struct DataTypeHeader *dth;
BPTR lock=0;
WORD ret=-99;

if(!DataTypesBase) return(ret);
if(h->flags&OFDECRUNCH) {
	dtn=ObtainDataTypeA(DTST_RAM,h->decbuffer,NULL);
} else {
	lock=fmlock(h->owner,h->filename);
	if(lock) dtn=ObtainDataTypeA(DTST_FILE,(APTR)lock,NULL);
}
if(dtn) {
	dth = dtn->dtn_Header;
	if(!dtype) {
		if(dth->dth_GroupID==GID_PICTURE) {
			dtype=GID_PICTURE;
		}
		if(dth->dth_GroupID==GID_SOUND) {
			dtype=GID_SOUND;
		}
	}
	if(dth->dth_GroupID==dtype) {
			if(dtype==GID_PICTURE) ret=showdatatypepicture(h,dtn,getconfignumber(SHOWPICCONFIG));
			if(dtype==GID_SOUND) ret=playdatatypesample(h,dtn,getconfignumber(HEARCONFIG));
	}
	ReleaseDataType(dtn);
}
if(lock) UnLock(lock);
return(ret);
}


void __saveds clickfile(void)
{
struct ProcMsg *pm;
struct FMList *list;
register struct FMNode *node;
struct FMHandle *h;
UBYTE *mem;
LONG len;

mem=0;
list=fmmain.sourcedir;
node=list->lastclicked;
pm=sinitproc();
if (!(setalloc(list,1))) {
	initproc(0,0);
	goto endi;
}
initproc(list,getstring(MSG_FILECLICKNAME));
if (node&&(mem=allocvec(list,MEMSIZE,0))) {
	sformatmsg(list->fmmessage1,MSG_FILECLICKING,NDFILE(node));
	list->fmmessage2[0]=0;
	fmmessage(list);
	if ((h=openfile(list,NDFILE(node),OFNORMAL|OFDECRUNCH))) {
		len=h->size;
		if (len>MEMSIZE) len=MEMSIZE;
		if (readbufferfile(h,mem,len)==len) {
			if (seek(h,0,OFFSET_BEGINNING)>=0) {
				list->fmmessage1[0]=0;
				list->fmmessage2[0]=0;
				fmmessage(list);
				node->flags|=NSELECTED;
				checkfile(h,mem,node);
			}
		}
		closefile(h);
		node->flags&=~NSELECTED;
		outputlistline(list,node);
	}
}
freemem(mem);
list->lastclicked=0;
endproc(list);
endi:
deinitproc(pm);
}

static WORD testclick(struct FMHandle *h,struct FMNode *node,UBYTE *m,WORD usedt)
{
UWORD apu2;
ULONG *apu;
UBYTE *ptr1;
WORD mod,cnt;

apu=(ULONG*)m;

if ((fmconfig->usepicturedt|usedt)&&*apu==ID_FORM&&*(apu+2)==ID_ILBM) {
	showpicture(h,getconfignumber(SHOWPICCONFIG));
	return(1);
}
if ((fmconfig->usesampledt|usedt)&&*apu==ID_FORM&&*(apu+2)==MAKE_ID('8','S','V','X')) {
	playsample(h,getconfignumber(HEARCONFIG));
	return(1);
}
if ((fmconfig->useexecutedt|usedt)&&*apu==0x3f3&&!(node->numprot&2)) {
	executefile(h,getconfignumber(EXECUTECONFIG));
	return(1);
}
if ((fmconfig->useicondt|usedt)&&*m==0xe3&&*(m+1)==0x10&&!Strnicmp(h->filename+strlen(h->filename)-5,".info",5)) {
	showicon(h,getconfignumber(SHOWPICCONFIG));
	return(1);
}
if ((fmconfig->usefontdt|usedt)&&*m==0x0f&&(*(m+1)==0x00||*(m+1)==0x03)&&!Strnicmp(h->filename+strlen(h->filename)-5,".font",5)) {
	showfont(h,getconfignumber(SHOWPICCONFIG));
	return(1);
}

ptr1=(UBYTE*)m; mod=1;
apu2=h->size<1000?h->size:1000;
for(cnt=0;cnt<apu2;cnt++) {
	if(*ptr1++==0) mod=0;
}
if (mod) {
	if((fmconfig->useascdt|usedt)) {
		showtext(h,getconfignumber(SHOWASCCONFIG));
		return(1);
	}
	return(0);
}
ptr1=h->filename; mod=0;
apu=(ULONG*)(m+1080);
if (*apu==MAKE_ID('M','.','K','.')||*apu==MAKE_ID('F','L','T','4')) mod=1;
if (!Strnicmp(ptr1+strlen(ptr1)-4,".mod",4)) mod=1;
if (!Strnicmp(ptr1,"mod.",4)) mod=1;
if (mod) {
	if((fmconfig->usemoddt|usedt)) {
		modplay(h,getconfignumber(HEARCONFIG));
		return(1);
	}
	return(0);
}
if((fmconfig->usehexdt|usedt)) {
	showtext(h,getconfignumber(SHOWHEXCONFIG));
	return(1);
}
return(0);
}

void checkfile(struct FMHandle *h,UBYTE *m,struct FMNode *node)
{
struct CMenuConfig *cmc;
UBYTE *ptr1,*ptr2;
WORD cnt;
WORD apu3,apu4,apu5;
struct OwnCommand *oc;
UBYTE token[256];
LONG longi;

for(cnt=0;cnt<TOTALCOMMANDS;cnt++) {
	cmc=&fmconfig->cmenuconfig[cnt];
	if(cmc->cmenucount!=100||!cmc->moreconfig) continue;
	oc=cmc->moreconfig;
	if(oc->namematch[0]) {
		if(ParsePatternNoCase(oc->namematch,token,256)>=0) {
			if(MatchPatternNoCase(token,h->filename)) {
				executeown(h->owner,fmmain.destdir,node,cmc);
				return;
			}
		}
	}
	ptr1=oc->matchbytes;
	apu3=-1;
	apu4=-2;
	while(*ptr1) {
		if(StrToLong(ptr1,&longi)<0) break;
		while(*ptr1!=','&&*ptr1!=0) ptr1++;
		if(*ptr1++==0) break;
		if(*ptr1=='$') {
			ptr1++;
			ptr2=ptr1;
			while(*ptr1!=','&&*ptr1!=0) ptr1++;
			apu3=0;apu4=0;
			while(ptr2<ptr1) {
				apu5=strtohex(ptr2);
				ptr2+=2;
				if(longi+apu3<MEMSIZE&&m[apu3+longi]==(UBYTE)apu5) apu4++;
				if(apu5<0) break;
				apu3++;
			}
			if(apu3==apu4) {
				apu4=-1;
				break;
			}
		} else if(*ptr1=='"') {
			ptr1++;
			ptr2=ptr1;
			while(*ptr1!='"'&&*ptr1!=0) ptr1++;
			if(*ptr1==0) break;
			apu5=ptr1-ptr2;
			if(!apu5) break;
			apu4=0;
			for(apu3=0;apu3<apu5;apu3++) {
				if(longi+apu3<MEMSIZE&&m[apu3+longi]==ptr2[apu3]) apu4++;
			}
			if(apu5==apu4) {
				apu4=-1;
				break;
			}
		} else {
			break;
		}
		if(*ptr1++!=',') break;
	}
	if(apu4==-1) {
		executeown(h->owner,fmmain.destdir,node,cmc);
		return;
	}
}
if(testclick(h,node,m,0)) return;

if(isdatatypes(h,0)!=-99) return;

testclick(h,node,m,1);
}
