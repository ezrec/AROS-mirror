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
#include <devices/timer.h>
#include <strings.h>
#include <stdio.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

struct ParentConfig {
	UBYTE parentbuffers;
};

void parentpath(struct FMList*,BPTR);
void dirsort(struct FMList*);
void clearlisti(struct List*);
void readdir(struct FMList*);
extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;    
extern struct GfxBase *GfxBase;

struct FMPathlist pathlist[PATHLISTS];

extern struct FMConfig *fmconfig;
extern UBYTE hsparwed[];
extern struct FMMain fmmain;
extern struct Gadget string1;
extern struct Gadget string2;

//*****

void __saveds getdir(void)
{
struct ProcMsg *pm;
struct FMList *list;

pm=sinitproc();
list=fmmain.sourcedir;
if (!(setalloc(list,1))) {
	DisplayBeep(0);
	Delay(50);
	initproc(0,0);
	goto endi;
}
initproc(list,getstring(MSG_DIRNAME));
readdir(list);
endproc(list);
endi:
deinitproc(pm);
}

void readdir(struct FMList *list)
{
struct FileInfoBlock *fib;
register struct FMNode *node;
ULONG date,protect;
WORD cnt,cnt2;
LONG timer2;
BPTR lock;

clearlist(list);
outputlist(list);
strcpymsg(list->fmmessage1,MSG_DIRRING);
list->fmmessage2[0]=0;
fmmessage(list);

if (!(fib=allocmem(sizeof(struct FileInfoBlock)))) { setalloc(list,0); return; }
if ((lock=fmlock(list,list->path))) {
	if (fmmain.ikkuna&&list->li) RefreshGList(&list->li->string,fmmain.ikkuna,0,1);
	list->flags&=~LDEVLIST; list->flags|=LDIRLIST|LUPDATEMSG;
	updadirmeter(list);
	if (fmexamine(list,lock,fib,list->path)) {
		timer2=fmmain.timer+5;
		while (fmexnext(list,lock,fib)>0) {
			if ((node=allocnode())) {
				strcpy(list->fmmessage1,fib->fib_FileName);
				node->numdate=dstolong(&fib->fib_Date);
				date=dstolong(&fib->fib_Date);
				longtodatestring(NDDATE(node),date);
				siirran(NDFILE(node),fib->fib_FileName,fmmain.filelen);
				siirran(NDCOMM(node),fib->fib_Comment,fmmain.commlen);
				node->numprot=fib->fib_Protection;
				protect=fib->fib_Protection^0x0f; cnt2=128;
				for(cnt=0;cnt<8;cnt++) {
					if(protect&cnt2) *(NDPROT(node)+cnt)=hsparwed[cnt*3+1]; else *(NDPROT(node)+cnt)='-';
					cnt2=cnt2>>1;
				}
				if (fib->fib_DirEntryType>0) {
					node->flags=NDIRECTORY;
				} else {
					node->flags=NFILE;
					node->numlen=fib->fib_Size;
					sformat(NDLENGTH(node),"%lU",fib->fib_Size);
				}
				AddTail((struct List*)list,(struct Node*)node);
			}
			if(fmmain.timer>timer2) {
				dirsort(list);
				outputlist(list);
				timer2=fmmain.timer;
			}
			if (testabort(list)) {
				dirsort(list);
				if (askabort(list)) break;
			}
		}
	}
	UnLock(lock);
}
freemem(fib);
dirsort(list);
}

void dirsort(struct FMList *list)
{
list->fmmessage1[0]=0; list->fmmessage2[0]=0;
fmmessage(list);
csortlist(list);
parselist(list,0);
outputlist(list);
updadirmeter(list);
endofdirtxt(list,0);
}

void parentpath(struct FMList *list,BPTR origlock)
{
BPTR lock=0,lock2=0;
UBYTE *ptr1,*ptr2,*ptr3,*ptr4;
WORD len,aa;
struct FileInfoBlock *fib=0;

if (!(ptr1=allocmem(PATH))) goto pathfail;
if (!(fib=allocmem(sizeof(struct FileInfoBlock)))) goto pathfail;
ptr2=ptr1+(PATH-2); *ptr2='/'; ptr4=ptr2;

if(!origlock) {
	if (!(lock=Lock(list->path,SHARED_LOCK))) goto pathfail;
} else {
	lock=origlock;
}

for(;;) {
	if (!(Examine(lock,fib))) goto pathfail;
	if (fib->fib_DirEntryType<0) goto pathfail;
	len=strlen(fib->fib_FileName);
	ptr2-=len; ptr3=fib->fib_FileName;
	for(aa=0;aa<len;aa++) {
		*(ptr2+aa)=*ptr3++;
	}
	ptr2--; *ptr2='/';
	if (!(lock2=ParentDir(lock))) break;
	UnLock(lock);
	lock=lock2; lock2=0;
}
ptr2++;
ptr3=ptr2;
while(*ptr3++!='/');
*(ptr3-1)=':';
if (*ptr4=='/') *ptr4=0;

strcpy(list->path,ptr2);

pathfail:
if (lock) UnLock(lock);
if (lock2) UnLock(lock2);
freemem(ptr1);
freemem(fib);
}

//*****

void endofdirtxt(struct FMList *list,struct FMNode *node)
{
UWORD selfils=0,seldirs=0,totfils=0,totdirs=0,mfils=0,mdirs=0;
ULONG selbytes=0,totbytes=0,mtotbytes=0;
register struct FMNode *node2;

if(list->flags&LDIRLIST) {
	for(node2=list->head;node2->succ;node2=node2->succ) {
		if(node2->flags&NMATCHED) {
			totbytes+=node2->numlen;
			if (node2->flags&NSELECTED) {
				selbytes+=node2->numlen;
				if (node2->flags & NFILE) selfils++;
				if (node2->flags & NDIRECTORY) seldirs++;
			}
			if (node2->flags & NFILE) totfils++;
			if (node2->flags & NDIRECTORY) totdirs++;
		}
		mtotbytes+=node2->numlen;
		if (node2->flags & NFILE) mfils++;
		if (node2->flags & NDIRECTORY) mdirs++;
	}
	sformatmsg(list->fmmessage1,MSG_DIRREDLINE1,selfils,totfils,mfils,seldirs,totdirs,mdirs,selbytes,totbytes,mtotbytes);
	list->fmmessage2[0]=0;
	if (node) {
		sformatmsg(list->fmmessage2,MSG_DIRREDLINE2,NDFILE(node),NDPROT(node),NDDATE(node),NDCOMM(node));
	}
} else {
	if(node) {
		sformat(list->fmmessage1,"%s",NDFILE(node));
		sformat(list->fmmessage2,"%s",NDCOMM(node));
	} else {
		list->fmmessage1[0]=0;
		list->fmmessage2[0]=0;
	}
}
fmmessage(list);
}

//*****


void dirmeters(void)
{
WORD cnt;

extern struct FMList fmlist[];

for(cnt=0;cnt<LISTS;cnt++) updadirmeter(&fmlist[cnt]);
}

WORD infodata(UBYTE *file,UBYTE *dst,WORD mode)
{
UBYTE state=getstring(MSG_DIRMETER_RW)[0];
WORD ret=0;
struct InfoData *idata;
struct Process *myproc;
APTR ooldwinptr;
ULONG apu1;
UBYTE used,total;
ULONG usedbytes=0,totalbytes=0;
BPTR lock;
WORD percent=0;

if ((idata=allocmem(sizeof(struct InfoData)))) {
	myproc=(struct Process*)FindTask(0);
	ooldwinptr=myproc->pr_WindowPtr;
	myproc->pr_WindowPtr=(APTR)-1;
	if ((lock=Lock(file,SHARED_LOCK))) {
		if (Info(lock,idata)) {
			usedbytes=idata->id_NumBlocksUsed*idata->id_BytesPerBlock;
			totalbytes=idata->id_NumBlocks*idata->id_BytesPerBlock;
			percent=0;
			apu1=totalbytes>>10;
			if(apu1) percent=(usedbytes>>10)*100/apu1;
			if(idata->id_DiskState==ID_VALIDATED) state=getstring(MSG_DIRMETER_RW)[1];
			ret=1;
		}
		UnLock(lock);
	}
	freemem(idata);
	myproc->pr_WindowPtr=ooldwinptr;
}
if(ret) {
	usedbytes/=1024;
	used='K';
	if(usedbytes>=1024) {
		used='M';
		usedbytes/=1024;
	}
	totalbytes/=1024;
	total='K';
	if(totalbytes>=1024) {
		total='M';
		totalbytes/=1024;
	}
	switch(mode)
	{
	case 1:
	sformat(dst,getstring(MSG_DIRMETER),usedbytes,used,totalbytes,total,percent,state);
	break;
	case 0:
	sformat(dst,"%3ld%% %lc",percent,state);
	break;
	}
}
return(ret);
}

void updadirmeter(struct FMList *list)
{
UBYTE varatxt[40];
WORD len,xx,yy,didit=0;
struct TextExtent txtext;
struct ListInfo *li;


li=list->li;
if(!li||!fmmain.ikkuna) return;

ObtainSemaphore(&fmmain.gfxsema);
SetDrMd(fmmain.rp,JAM1);
xx=li->dirx; yy=li->topliney;
if (list->flags&LSOURCE) {
	SetAPen(fmmain.rp,fmconfig->sourcepen);
} else if (list->flags&LDESTINATION) {
	SetAPen(fmmain.rp,fmconfig->destpen);
} else {
	SetAPen(fmmain.rp,fmconfig->backpen);
}

if (list->flags&LDIRLIST) {
	if(infodata(list->path,varatxt,1)) {
		len=TextFit(fmmain.rp,varatxt,strlen(varatxt),&txtext,0,1,list->li->dirwidth,32767);
		WaitTOF();
		RectFill(fmmain.rp,xx-1,yy-1,xx+li->dirwidth,yy+fmmain.txtysize-1);
		SetAPen(fmmain.rp,fmconfig->txtpen);
		Move(fmmain.rp,xx+(list->li->dirwidth-txtext.te_Width)/2,yy+fmmain.txtbaseline);
		Text(fmmain.rp,varatxt,len);
		didit=1;
	}
}
if(!didit) {
	WaitTOF();
	RectFill(fmmain.rp,xx-1,yy-1,xx+li->dirwidth,yy+fmmain.txtysize-1);
}
SetDrMd(fmmain.rp,JAM2);
ReleaseSemaphore(&fmmain.gfxsema);
}

WORD getparent(struct FMList *list)
{
WORD cnt;
struct FMNode *node2,*node;
struct FMPathlist *pathlist;

for(cnt=0;cnt<fmmain.numpathlist;cnt++) {
	pathlist=fmmain.pathlist+cnt;
	if (pathlist->path[0]&&strcmp(pathlist->path,list->path)==0) {
		clearlist(list);
		for(node=pathlist->head;node->succ;node=node->succ) {
			node2=dupnode(node);
			if (node2) {
				node2->flags&=~NSELECTED;
				AddTail((struct List*)list,(struct Node*)node2);
			}
		}		
		list->firstline=pathlist->firstline;
		list->firstlinew=pathlist->firstlinew;
		pathlist->cnt++;
		if (list->li&&fmmain.ikkuna) RefreshGList(&list->li->string,fmmain.ikkuna,0,1);
		csortlist(list);
		parselist(list,0);
		endofdirtxt(list,0);
		outputlist(list);
		return(1);
	}
}
return(0);
}

void shareware(void);

void setparent(struct FMList *list)
{
LONG cnt2,cnt3,cnt4;
WORD cnt;
struct FMPathlist *pathlist;

if (list->flags&LDIRLIST&&fmmain.numpathlist) {
	cnt3=0x7fffffff; cnt4=0; cnt2=0;
	for(cnt=0;cnt<fmmain.numpathlist;cnt++) {
		pathlist=fmmain.pathlist+cnt;
		if(Stricmp(pathlist->path,list->path)==0) {
			cnt3=0x80000000;
			cnt2=cnt;
		}
		if(pathlist->cnt<cnt3) {
			cnt3=pathlist->cnt;
			cnt2=cnt;
		}
		if(pathlist->cnt>cnt4) cnt4=pathlist->cnt;
	}
	pathlist=fmmain.pathlist+cnt2;
	pathlist->cnt=0;
	pathlist->path[0]=0;
	clearlisti((struct List*)pathlist);
	if (duplist(list,(struct FMList*)pathlist)) strcpy(pathlist->path,list->path);
	pathlist->firstline=list->firstline;
	pathlist->firstlinew=list->firstlinew;
	pathlist->cnt=cnt4+1;
}
}

void freepathlist(void)
{
struct FMPathlist *pathlist;
WORD cnt1;

pathlist=fmmain.pathlist;
if(!pathlist) return;
for(cnt1=0;cnt1<fmmain.numpathlist;cnt1++) {
	clearlisti((struct List*)(pathlist+cnt1));
}
freemem(pathlist);
fmmain.pathlist=0;
fmmain.numpathlist=0;
}
void initpathlist(void)
{
struct ParentConfig *pc;
struct FMPathlist *pathlist=0;
WORD cnt1;
D(bug("dir.c 429...........\n"));  
freepathlist();
D(bug("dir.c 431...........\n"));  
pc=getconfignumber(PARENTCONFIG)->moreconfig;
D(bug("dir.c 433...........\n"));  
if(!pc->parentbuffers) return;
D(bug("dir.c 435...........\n"));  
while(pc->parentbuffers>0) {
	pathlist=allocvec(0,sizeof(struct FMPathlist)*pc->parentbuffers,MEMF_PUBLIC|MEMF_CLEAR);
	if(pathlist) break;
	pc->parentbuffers--;
}
D(bug("dir.c 440...........\n"));  
if(pathlist) {
	for(cnt1=0;cnt1<pc->parentbuffers;cnt1++) NewList((struct List*)(pathlist+cnt1));
}
fmmain.numpathlist=pc->parentbuffers;
fmmain.pathlist=pathlist;

}

void *parentconfigdefault(struct CMenuConfig *cmc)
{
struct ParentConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct ParentConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=cmc->moreconfig;
config->parentbuffers=10;
return(cmc->moreconfig);
}

WORD parentconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
struct ParentConfig *pc;
LONG bufs;
pc=getconfig(cmc);
bufs=pc->parentbuffers;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_PARENT_CONFIG_BUFFERS,200,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqinteger(gb,200,&bufs,0,99);
commandanswer(gb);
c=quickreq(gb);
if(c) {
	pc->parentbuffers=bufs;
	initpathlist();
}
return(c);

}
