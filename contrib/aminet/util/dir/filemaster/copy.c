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

#include <stdio.h>
#include <string.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct FMConfig *fmconfig;
extern struct FMMain fmmain;
extern UBYTE nformatstring[];
extern struct UtilityBase *UtilityBase;  
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;

struct CopyConfig {
	unsigned overwrite:2;
	unsigned comment:2;
	unsigned flags:2;
	unsigned freespace:2;
	unsigned date:2;
	unsigned update:2;
	LONG bufsize;
};
struct DeleteConfig {
	unsigned askdelete:2;
	unsigned askdir:2;
	unsigned protected:2;
	unsigned update:2;
};

struct DelStr;

WORD deletefile(struct FMList*,UBYTE*,struct DeleteConfig*,WORD);
WORD deletedir(struct FMList*,UBYTE*,struct DelStr*,WORD);
WORD deletefirst(struct FMList*,struct FMNode*,struct DelStr*);

WORD askcopyname(struct FMList*,UBYTE*,struct DelStr*);
void copyroutine(struct CMenuConfig *cmc);
WORD copyfile(struct FMList*,struct FMList*,UBYTE*,struct DelStr*);
WORD copydir(struct FMList*,struct FMList*,UBYTE*,struct DelStr*,WORD  flag);
void copyflags(struct CopyConfig*,struct FMList*,struct FMList*,UBYTE*,UBYTE*);

WORD renamefile(struct FMList*,struct FMList*,UBYTE*);

WORD sizedir(struct FMList*,UBYTE*,struct DelStr*,LONG*);

WORD doubleabort(struct FMList*,struct FMList*);

struct DelStr *createtoken(struct FMList*);
WORD dmatch(struct FileInfoBlock*,struct DelStr*);
WORD askcontinue(struct FMList*);

void clearlisti(struct List*);

struct DelStr {
	ULONG	filecnt;
	ULONG	dircnt;
	ULONG	bytecnt;
	UBYTE	method;
	UBYTE	owflag;
	UBYTE	asflag;
	UBYTE	allsubdelete;
	struct	CopyConfig *cconfig;
	struct	DeleteConfig *dconfig;
	struct	Match match;
	UBYTE	token1[1000];
	UBYTE	token2[1000];
	UBYTE	token3[500];
};

/********* COPY **************/

void __saveds copyfiles(void)
{
struct ProcMsg *pm;
struct CMenuConfig *cmc;
pm=sinitproc();
cmc=pm->cmc;
copyroutine(cmc);
deinitproc(pm);
}

void copyroutine(struct CMenuConfig *cmc)
{
struct FMList *slist;
struct FMList *dlist;
struct FMNode *node;
struct DelStr *delstr;
WORD ret;
UBYTE varaname[32];
UBYTE aputxt[80];

priority(cmc);
slist=fmmain.sourcedir;
dlist=fmmain.destdir;
if (!(setalloc(slist,1))) { initproc(0,0); return; }
if (!(setalloc(dlist,1))) { setalloc(slist,0); initproc(0,0); return; }
slist->pair=dlist;
dlist->pair=slist;
if (!(delstr=createtoken(slist))) {
	initproc(slist,(UBYTE*)1);
	goto delfailure1;
}
sformat(aputxt,getstring(MSG_COPYNAMESOURCE),cmc->label);
initproc(slist,aputxt);
sformat(aputxt,getstring(MSG_COPYNAMEDESTINATION),cmc->label);
strcpy(dlist->workname,aputxt);
if(cmc->cmenucount==COPYASCONFIG) delstr->method=1;
delstr->cconfig=getconfig(cmc);
if(!delstr->cconfig) goto delfailure1;

if(!sselected(slist,-1)) goto delfailure1;
if(!(dlist->flags&LDIRLIST)) {
	strcpymsg(slist->fmmessage1,MSG_MAIN_NODESTINATION);
	goto delfailure1;
}
slist->flags|=LUPDATEMSG; dlist->flags|=LUPDATEMSG;
sformatmsg(slist->fmmessage1,MSG_COPYING,0L,0L,0L);
strcpy(dlist->fmmessage1,slist->fmmessage1);
fmmessage(slist);
fmmessage(dlist);
for(node=slist->head;node->succ;node=node->succ) {
	if(node->flags&NFILE && node->flags&NSELECTED) {
		strcpy(varaname,NDFILE(node));
		ret=copyfile(slist,dlist,varaname,delstr);
		node->flags&=~NSELECTED;
		outputlistline(slist,node);
		disknode(dlist,varaname);
		if (delstr->cconfig->update==0) outputlist(dlist);
		if (!ret) goto delfailure1;
		sformatmsg(slist->fmmessage1,MSG_COPYING,delstr->filecnt,delstr->dircnt,delstr->bytecnt);
		strcpy(dlist->fmmessage1,slist->fmmessage1);
	}
}
for(node=slist->head;node->succ;node=node->succ) {
	if(node->flags&NDIRECTORY && node->flags&NSELECTED) {
		strcpy(varaname,NDFILE(node));
		ret=copydir(slist,dlist,varaname,delstr,0);
		node->flags&=~NSELECTED;
		outputlistline(slist,node);
		disknode(dlist,varaname);
		if(delstr->cconfig->update<=1) outputlist(dlist);
		if (!ret) goto delfailure1;
	}
}
sformatmsg(slist->fmmessage1,MSG_COPIED,delstr->filecnt,delstr->dircnt,delstr->bytecnt);
slist->fmmessage2[0]=0;
strcpy(dlist->fmmessage1,slist->fmmessage1);
strcpy(dlist->fmmessage2,slist->fmmessage2);
delfailure1:
freemem(delstr);
fmmessage(slist);
fmmessage(dlist);
csortlist(dlist);
parselist(dlist,0);
outputlist(slist);
outputlist(dlist);
updadirmeter(dlist);
endproc(slist);
}

WORD copydir(struct FMList *slist,struct FMList *dlist,UBYTE *dirname,struct DelStr *ds,WORD flag)
{
struct FileInfoBlock *fib;
BPTR lock=0;
WORD ret=0,ret2,ret3,dret=0,dflag=0;
UBYTE name[32];
UBYTE varaname[32];
UBYTE path1[PATH],path2[PATH];

if (!doubleabort(slist,dlist)) return(0);
fib=allocmem(sizeof(struct FileInfoBlock));
if (!fib) return(0);

AddPart(slist->path,dirname,PATH);
ret2=askcopyname(dlist,dirname,ds);
AddPart(dlist->path,dirname,PATH);
if(!ret2) goto dderr;

sformatmsg(slist->fmmessage1,MSG_COPYING,ds->filecnt,ds->dircnt,ds->bytecnt);
strcpy(dlist->fmmessage1,slist->fmmessage1);

createretry:
if (!(lock=CreateDir(dlist->path))) {
	if(checkdoserr()!=203) {
		ret3=dosrequestmsg(dlist,3,MSG_FMDOS_MAKEDIRERR,dirname);
		if(ret3>0) goto createretry;
		if(ret3<0) {
			ret=-1;
			goto dderr;
		}
	}
}
if (lock) UnLock(lock);
lock=0;
strcpy(path1,slist->path);
strcpy(path2,dlist->path);
// *(PathPart(slist->path))=0;
// *(PathPart(dlist->path))=0;
copyflags(ds->cconfig,slist,dlist,dirname,dirname);
strcpy(slist->path,path1);
strcpy(dlist->path,path2);

if (!(lock=fminitdirread(slist,fib,slist->path))) goto dderr;
while(fmexnext(slist,lock,fib)>0) {
	if(dflag) {
		if (!deletefile(slist,name,ds->dconfig,1)) goto dderr;
		dflag=0;
	}
	if(dmatch(fib,ds)) {
		if (fib->fib_DirEntryType>0) {
			strcpy(varaname,fib->fib_FileName);
			dret=copydir(slist,dlist,varaname,ds,flag);
			if(!dret) goto dderr;
		} else {
			strcpy(varaname,fib->fib_FileName);
			dret=copyfile(slist,dlist,varaname,ds);
			if(!dret) goto dderr;
			sformatmsg(slist->fmmessage1,MSG_COPYING,ds->filecnt,ds->dircnt,ds->bytecnt);
			strcpy(dlist->fmmessage1,slist->fmmessage1);
		}
	}
	if (flag&&dret>0) {
		strcpy(name,fib->fib_FileName);
		dflag=1;
	}
}
if(dflag) {
	if (!deletefile(slist,name,ds->dconfig,1)) goto dderr;
}
ret=1;
dderr:
if(ret>0) ds->dircnt++;
// *(PathPart(slist->path))=0;
// *(PathPart(dlist->path))=0;
UnLock(lock);
freemem(fib);
return(ret);
}


WORD copyfile(struct FMList *slist,struct FMList *dlist,UBYTE *name,struct DelStr *ds)
{
struct GUIBase *gb;
struct FMHandle *hs=0;
struct FMHandle *hd=0;
struct FileInfoBlock *fib;
LONG cnt,cnt2=0;
WORD ret=0,nocont=0;
UBYTE varaname[32];
ULONG size,dstsize = 0;
BPTR lock=0,olock=0;
LONG dstdate=0;
/* struct DeleteConfig dc; */
WORD c=0;
UBYTE text[100];
UBYTE date1[80],date2[80];

struct InfoData *id;
LONG bytesperblock,alreadyblocks=0,freeblocks;

if (!doubleabort(slist,dlist)) return(0);

strcpy(varaname,name);
if ((hs=openfile(slist,varaname,OFBUFFER|(ds->cconfig->bufsize?ds->cconfig->bufsize:60000)))) {
	size=hs->size;

	sformatmsg(slist->fmmessage2,MSG_COPYING2,hs->filename,0L,hs->size);
	strcpy(dlist->fmmessage2,slist->fmmessage2);

	if(askcopyname(dlist,name,ds)) {

		/*  freespace check */
	
		fib=allocmem(sizeof(struct FileInfoBlock));
		if(currentdir(dlist)) olock=Lock(name,SHARED_LOCK);
		if(fib&&olock) {
			if(fmexamine(dlist,olock,fib,name)) {
				alreadyblocks=fib->fib_NumBlocks;
				dstdate=dstolong(&fib->fib_Date);
				dstsize=fib->fib_Size;
			}
		}
		freemem(fib);

		if(ds->cconfig->freespace) {
			id=allocmem(sizeof(struct InfoData));
			lock=fmlock(slist,dlist->path);
			if(id&&lock&&Info(lock,id)) {
				bytesperblock=512;
				freeblocks=id->id_NumBlocks-id->id_NumBlocksUsed;
				if(id->id_DiskType==ID_DOS_DISK||id->id_DiskType==ID_INTER_DOS_DISK) bytesperblock=488;				
				if(((id->id_DiskType&0xffffff00)==(ID_DOS_DISK&0xffffff00)))
					size+=((((size+bytesperblock)/bytesperblock)/72)*512);
				if(freeblocks) freeblocks+=alreadyblocks;
				if(freeblocks&&(freeblocks-1)*bytesperblock<=size) {
					gb=getguibase(slist->workname);
					sformatmsg(text,MSG_COPY_NOFREESPACE,name);
					reqinfo(gb,text,-1,guiCENTER);
					buttonbarmsg(gb,MSG_IGNORE,1,MSG_SKIP,2,MSG_CANCEL,0,0);
					c=quickreq(gb);
					if(!c) {
						ret=0;
						nocont=1;
						goto nocopy;
					}
					if(c==2) {
						ret=-1;
						goto nocopy;
					}
				}
			}
			if(lock) UnLock(lock);
			lock=0;
			freemem(id);
		}

		size=hs->size;

		/*  overwrite check */

		if(!ds->owflag&&ds->cconfig->overwrite&&currentdir(dlist)) {
			if (olock) {
				gb=getguibase(slist->workname);
				longtodatestring(date1,hs->date);
				longtodatestring(date2,dstdate);
				sformatmsg(text,MSG_COPY_OVERWRITE,name,date1,hs->size,date2,dstsize);
				reqinfo(gb,text,-1,guiCENTER);
				buttonbarmsg(gb,MSG_YES,1,MSG_ALL,2,MSG_NO,3,MSG_CANCEL,0,0);
				c=quickreq(gb);
				if(!c) {
					ret=0;
					nocont=1;
					goto nocopy;
				}
				if(c==3) {
					ret=-1;
					goto nocopy;
				}
				if(c==2) {
					ds->owflag=1;
				}
			}
		}

		if(olock) UnLock(olock);
		olock=0;

		if ((hd=openfile(dlist,name,OFWRITE))) {
			ret=1;
			for(;;) {
				sformatmsg(slist->fmmessage2,MSG_COPYING2,hs->filename,cnt2,hs->size);
				strcpy(dlist->fmmessage2,slist->fmmessage2);
				cnt=readfile(hs);
				if(cnt<0) {
					ret=-1;
					break;
				}
				cnt2+=cnt;
				if (cnt==0) break;
				sformatmsg(slist->fmmessage2,MSG_COPYING2,hd->filename,cnt2,hs->size);
				strcpy(dlist->fmmessage2,slist->fmmessage2);
				if(!doubleabort(slist,dlist)) {
					ret=0;
					nocont=1;
					break;
				}
				if (cnt<0||writefile(hd,hs->buffer,cnt)!=cnt) {
					ret=-1;
					break;
				}
			}
			closefile(hd);
			if(ret>0) copyflags(ds->cconfig,slist,dlist,varaname,name);
		}
	}
nocopy:
closefile(hs);
}
if(ret<=0) {
/*
	if(hd) {
		dc.askdelete=0;
		dc.askdir=0;		
		dc.protected=0;
		deletefile(dlist,name,&dc);
	}
*/
	if(!ret&&!ds->method&&!nocont) ret=askcontinue(slist);
} else {
	ds->filecnt++;
	ds->bytecnt+=size;
}
if(olock) UnLock(olock);
if(lock) UnLock(lock);
return(ret);
}

WORD askcopyname(struct FMList *list,UBYTE *txt,struct DelStr *ds)
{
struct GUIBase *gb;
WORD ret=0,c;

if(!ds->method||ds->asflag) return(1);

gb=getguibase(list->workname);
setconnectgroup(gb,1,0,0);
reqinfomsg(gb,MSG_COPY_NEWNAME,100,guiUC|guiLEFT);
reqstring(gb,100,txt,31);
buttonbarmsg(gb,MSG_OK,1,MSG_ALL,2,MSG_CANCEL,0,0);
c=quickreq(gb);
if(c==1) {
	ret=1;
}
if(c==2) {
	ds->asflag=1;
	ret=1;
}
return(ret);
}

void copyflags(struct CopyConfig *cconfig,struct FMList *slist,struct FMList *dlist,UBYTE *sname,UBYTE *dname)
{
struct FileInfoBlock *fib;

fib=allocmem(sizeof(struct FileInfoBlock));
if(!fib) return;
if(cconfig->comment|cconfig->flags|cconfig->date) {
	if(fmexaminefile(slist,fib,sname)) {
		if(cconfig->comment&&fib->fib_Comment[0]) fmsetcomment(dlist,dname,fib->fib_Comment);
		if(cconfig->flags&&fib->fib_Protection!=0x0f) fmsetprotection(dlist,dname,fib->fib_Protection);
		if(cconfig->date) fmsetfiledate(dlist,dname,&fib->fib_Date);
	}
}
freemem(fib);
}

/************ DELETE **************/


void __saveds deletefiles(void)
{
struct FMList *list;
struct FMNode *node;
struct FMNode *nxtnode;
struct ProcMsg *pm;
struct DelStr *delstr;
WORD first=1;
WORD apu1;

pm=sinitproc();
priority(pm->cmc);
list=fmmain.sourcedir;
if (!(setalloc(list,1))) { initproc(0,0); return; }
if (!(delstr=createtoken(list))) {
	initproc(list,(UBYTE*)1);
	goto delfailure1;
}
initproc(list,pm->cmc->label);
delstr->dconfig=getconfig(pm->cmc);
if(!delstr->dconfig) goto delfailure1;
if(!sselected(list,-1)) goto delfailure1;

list->flags|=LUPDATEMSG;
sformatmsg(list->fmmessage1,MSG_DELETING,0L,0L,0L);
fmmessage(list);
if (!currentdir(list)) goto delfailure1;
for(node=list->head;node->succ;node=node->succ) {
	if(node->flags&NFILE && node->flags&NSELECTED) {
		if(first) {
			if(!deletefirst(list,node,delstr)) goto delfailure1;
			first=0;
		}
		nxtnode=node->pred;
		apu1=deletefile(list,NDFILE(node),delstr->dconfig,0);
		node->flags&=~NSELECTED;
		if(!apu1) goto delfailure1;
		if(apu1>0) {
			delstr->filecnt++;
			delstr->bytecnt+=node->numlen;
			sformatmsg(list->fmmessage1,MSG_DELETING,delstr->filecnt,delstr->dircnt,delstr->bytecnt);
			sformat(list->fmmessage2,"'%s'",NDFILE(node));
			removenode(list,node);
			node=nxtnode;
		}
		if(delstr->dconfig->update==0) outputlist(list);
	}
}
for(node=list->head;node->succ;node=node->succ) {
	if(node->flags&NDIRECTORY && node->flags&NSELECTED) {
		if(first) {
			if(!deletefirst(list,node,delstr)) goto delfailure1;
			first=0;
		}
		nxtnode=node->pred;
		apu1=deletedir(list,NDFILE(node),delstr,delstr->dconfig->askdir);
		node->flags&=~NSELECTED;
		if(!apu1) goto delfailure1;
		if(apu1>0) {
			if (!(apu1=deletefile(list,NDFILE(node),delstr->dconfig,0))) goto delfailure1;
			if(apu1>0) {
				delstr->dircnt++;
				removenode(list,node);
				node=nxtnode;
			}
		}
		if(delstr->dconfig->update<=1) outputlist(list);
	}
}
sformatmsg(list->fmmessage1,MSG_DELETED,delstr->filecnt,delstr->dircnt,delstr->bytecnt);
list->fmmessage2[0]=0;
delfailure1:
freemem(delstr);
fmmessage(list);
outputlist(list);
updadirmeter(list);
endproc(list);
deinitproc(pm);
}

WORD deletedir(struct FMList *list,UBYTE *dirname,struct DelStr *ds,WORD subdir)
{
struct GUIBase *gb;
struct FileInfoBlock *fib=0;
BPTR lock=0;
WORD ret=0,flag=0,apu1,empty=0;
UBYTE name[32];
WORD c;
UBYTE text[100];

AddPart(list->path,dirname,PATH);
if (testabort(list)) {
	if (askabort(list)) goto dderr;
}
sformatmsg(list->fmmessage1,MSG_DELETING,ds->filecnt,ds->dircnt,ds->bytecnt);

fib=allocmem(sizeof(struct FileInfoBlock));
if (!fib) goto dderr;

if (!(currentdir(list))) goto dderr;
if (!(lock=fminitdirread(list,fib,list->path))) goto dderr;
while(fmexnext(list,lock,fib)>0) {
	sformatmsg(list->fmmessage1,MSG_DELETING,ds->filecnt,ds->dircnt,ds->bytecnt);

	if(!empty&&subdir&&!ds->allsubdelete) {
		empty=1;
		sformat(list->fmmessage2,"'%s'",dirname);
		gb=getguibase(list->workname);
		sformatmsg(text,MSG_DELETE_ASKDIRECTORY,dirname);
		reqinfo(gb,text,-1,guiCENTER);
		buttonbarmsg(gb,MSG_YES,1,MSG_ALL,2,MSG_NO,3,MSG_CANCEL,0,0);
		c=quickreq(gb);
		if(!c) {
			ret=0;
			goto dderr;
		}
		if(c==3) {
			ret=-1;
			goto dderr;
		}
		if(c==2) {
			ds->allsubdelete=1;
		}
	}


	if(flag) {
		apu1=deletefile(list,name,ds->dconfig,0);
		flag=0;
		if(!apu1) goto dderr;
		if(apu1>0) ds->dircnt++;
	}
	if(dmatch(fib,ds)) {
		if (fib->fib_DirEntryType>0) {
			if (!(apu1=deletedir(list,fib->fib_FileName,ds,0))) goto dderr;
			if(apu1==1) {
				strcpy(name,fib->fib_FileName);
				flag=1;
			}
		} else {
			if (!(apu1=deletefile(list,fib->fib_FileName,ds->dconfig,0))) goto dderr;
			if(apu1<0) {
				ret=-1;
			} else {
				ds->filecnt++;
				ds->bytecnt+=fib->fib_Size;
			}		
		}
	}
}
if(flag) {
	sformat(list->fmmessage2,"'%s'",fib->fib_FileName);
	apu1=deletefile(list,name,ds->dconfig,0);
	if(!apu1) goto dderr;
	if(apu1>0) ds->dircnt++;
}
if(!ret) ret=1;
dderr:
if (lock) UnLock(lock);
freemem(fib);
// *(PathPart(list->path))=0;
return(ret);
}


WORD deletefile(struct FMList *list,UBYTE *name,struct DeleteConfig *dc,WORD req)
{
WORD error;
WORD ret=0;

dretry:
if (testabort(list)) {
	if (askabort(list)) goto dferr;
}
sformat(list->fmmessage2,"'%s'",name);
if (!currentdir(list)) goto dferr;
if (!DeleteFile(name)) {
	outputlist(list);
	error=checkdoserr();
	if(error==222) {
		if(dc->protected) {
			if(!requestmsg(getstring(MSG_MAIN_AREYOUSURE),MSG_YES,MSG_NO,MSG_DELETE_DELPROTECTED,name)) {
				ret=-1;
				goto dferr;
			}
		}
		do {
			if(SetProtection(name,0)) goto dretry;
		} while(dosrequestmsg(list,1,MSG_FMDOS_REMDELPROT,name));
		goto dferr;
	}
	if(!req) {
		ret=dosrequestmsg(list,3,MSG_FMDOS_DELETEERR,name);
		if(ret>0) goto dretry;
	} else {
		if(ret<0) ret=0;
	}
} else {
	ret=1;
}
dferr:
return(ret);
}

WORD deletefirst(struct FMList *list,struct FMNode *node,struct DelStr *ds)
{
if(ds->dconfig->askdelete) {
	outputlistline(list,node);
	return(requestmsg(getstring(MSG_MAIN_AREYOUSURE),MSG_YES,MSG_NO,MSG_DELETE_ASKDELETE));
}
return(1);
}

/**************** MOVE ****************/

void __saveds movefiles(void)
{
struct ProcMsg *pm;
struct FMList *slist;
struct FMList *dlist;
struct FMNode *node,*nxtnode;
struct DelStr *delstr;
LONG rename;
WORD apu1;
UBYTE *string=0;
struct CopyConfig fakecconfig;
struct DeleteConfig fakedconfig;
struct CopyConfig *moveconfig;
BPTR lock1=0,lock2=0;
UBYTE varaname[32];

pm=sinitproc();
priority(pm->cmc);
slist=fmmain.sourcedir;
dlist=fmmain.destdir;
if (!(setalloc(slist,1))) { initproc(0,0); return; }
if (!(setalloc(dlist,1))) { setalloc(slist,0); initproc(0,0); return; }
slist->pair=dlist;
dlist->pair=slist;
if (!(delstr=createtoken(slist))) {
	initproc(slist,(UBYTE*)1);
	goto delfailure1;
}
initproc(slist,pm->cmc->label);
moveconfig=getconfig(pm->cmc);
if(!moveconfig) goto delfailure1;
delstr->cconfig=&fakecconfig;
delstr->dconfig=&fakedconfig;
fakedconfig.askdelete=0;
fakedconfig.askdir=0;
fakedconfig.protected=0;
fakecconfig.overwrite=moveconfig->overwrite;
fakecconfig.comment=moveconfig->comment;
fakecconfig.date=moveconfig->date;
fakecconfig.flags=moveconfig->flags;
fakecconfig.freespace=moveconfig->freespace;
if(!sselected(slist,-1)) goto delfailure1;
if(!(dlist->flags&LDIRLIST)) {
	strcpymsg(slist->fmmessage1,MSG_MAIN_NODESTINATION);
	goto delfailure1;
}
slist->flags|=LUPDATEMSG; dlist->flags|=LUPDATEMSG;
sformatmsg(slist->fmmessage1,MSG_MOVING,0,string,0,string,0,string);
strcpy(dlist->fmmessage1,slist->fmmessage1);
fmmessage(slist);
fmmessage(dlist);
lock1=fmlock(slist,slist->path);
lock2=fmlock(dlist,dlist->path);
if(!lock1||!lock2) goto delfailure1;
rename=SameLock(lock1,lock2);
if (rename==LOCK_DIFFERENT) rename=0; else rename=1;

for(node=slist->head;node->succ;node=node->succ) {
	if(node->flags&NFILE && node->flags&NSELECTED) {
		nxtnode=node->pred;
		strcpy(varaname,NDFILE(node));
		if (rename) {
			if (!(apu1=renamefile(slist,dlist,varaname))) goto delfailure1;
			node->flags&=~NSELECTED;
			disknode(dlist,varaname);
			if(moveconfig->update==0) outputlist(dlist);
			if(apu1>0) {
				delstr->filecnt++;
				delstr->bytecnt+=node->numlen;
			}
		} else {
			apu1=copyfile(slist,dlist,varaname,delstr);
			node->flags&=~NSELECTED;
			disknode(dlist,varaname);
			if(moveconfig->update==0) outputlist(dlist);
			if(!apu1) goto delfailure1;
			if(apu1>0) if(deletefile(slist,varaname,delstr->dconfig,1)<=0) goto delfailure1;
		}
		sformatmsg(slist->fmmessage1,MSG_MOVING,delstr->filecnt,string,delstr->dircnt,string,delstr->bytecnt,string);
		strcpy(dlist->fmmessage1,slist->fmmessage1);
		removenode(slist,node);
		if(moveconfig->update==0) outputlist(slist);
		node=nxtnode;
	}
}
for(node=slist->head;node->succ;node=node->succ) {
	if(node->flags&NDIRECTORY && node->flags&NSELECTED) {
		nxtnode=node->pred;
		strcpy(varaname,NDFILE(node));
		if (rename) {
			apu1=renamefile(slist,dlist,varaname);
			node->flags&=~NSELECTED;
			outputlistline(slist,node);
			disknode(dlist,varaname);
			if(moveconfig->update<=1) outputlist(dlist);
			if(!apu1) goto delfailure1;
			if (node->flags&(NDIRECTORY|NDEVICE|NASSIGN)) string="+";
			if(apu1>0) delstr->dircnt++;
		} else {
			apu1=copydir(slist,dlist,varaname,delstr,1);
			node->flags&=~NSELECTED;
			outputlistline(slist,node);
			disknode(dlist,varaname);
			if(moveconfig->update<=1) outputlist(dlist);
			if (!apu1) goto delfailure1;
			apu1=deletefile(slist,NDFILE(node),delstr->dconfig,1);
			if (!apu1) goto delfailure1;
		}
		sformatmsg(slist->fmmessage1,MSG_MOVING,delstr->filecnt,string,delstr->dircnt,string,delstr->bytecnt,string);
		sformat(slist->fmmessage2,"'%s'",slist->path);
		strcpy(dlist->fmmessage1,slist->fmmessage1);
		strcpy(dlist->fmmessage2,slist->fmmessage2);
		removenode(slist,node);
		if(moveconfig->update<=1) outputlist(slist);
		node=nxtnode;
	}
}
sformatmsg(slist->fmmessage1,MSG_MOVED,delstr->filecnt,string,delstr->dircnt,string,delstr->bytecnt,string);
slist->fmmessage2[0]=0;
strcpy(dlist->fmmessage1,slist->fmmessage1);
strcpy(dlist->fmmessage2,slist->fmmessage2);
delfailure1:
UnLock(lock1);
UnLock(lock2);
freemem(delstr);
fmmessage(slist);
fmmessage(dlist);
csortlist(dlist);
outputlist(slist);
outputlist(dlist);
updadirmeter(slist);
updadirmeter(dlist);
endproc(slist);
deinitproc(pm);
}

WORD renamefile(struct FMList *slist,struct FMList *dlist,UBYTE *name)
{
WORD ret=0;

AddPart(slist->path,name,PATH);
AddPart(dlist->path,name,PATH);
if(fmsetrename(slist,slist->path,dlist->path)) {
	ret=1;
} else {
	ret=askcontinue(slist);
}
// *(PathPart(slist->path))=0;
// *(PathPart(dlist->path))=0;
return(ret);
}

WORD doubleabort(struct FMList *slist,struct FMList *dlist)
{
if (testabort(slist)) {
	if (askabort(slist)) return(0);
}
if (testabort(dlist)) {
	if (askabort(dlist)) return(0);
}
return(1);
}


/************** DISKSIZE ******************/

void __saveds disksize(void)
{
struct ProcMsg *pm;
struct FMList *list;
struct FMNode *node;
struct DelStr *delstr;
LONG dsize;
WORD apu1;

pm=sinitproc();
priority(pm->cmc);
list=fmmain.sourcedir;
if (!(setalloc(list,1))) { initproc(0,0); return; }
if (!(delstr=createtoken(list))) {
	initproc(list,(UBYTE*)1);
	goto sizfailure1;
}
initproc(list,pm->cmc->label);
if(!sselected(list,-1)) goto sizfailure1;
sformatmsg(list->fmmessage1,MSG_DISKSIZING,0L,0L,0L);
fmmessage(list);
list->flags|=LUPDATEMSG;
for(node=list->head;node->succ;node=node->succ) {
	if(node->flags&NFILE&&node->flags&NSELECTED) {
		delstr->filecnt++;
		delstr->bytecnt+=node->numlen;
		node->flags&=~NSELECTED;
		sformatmsg(list->fmmessage1,MSG_DISKSIZING,delstr->filecnt,delstr->dircnt,delstr->bytecnt);
		sformat(list->fmmessage2,"'%s'",list->path);
		outputlistline(list,node);
	}
}
for(node=list->head;node->succ;node=node->succ) {
	if(node->flags&(NDIRECTORY|NASSIGN|NDEVICE)&&node->flags&NSELECTED) {
		dsize=0;
		apu1=sizedir(list,NDFILE(node),delstr,&dsize);
		node->flags&=~NSELECTED;
		if(apu1<=0) goto sizfailure1;
		sformat(NDLENGTH(node),nformatstring,dsize);
		outputlistline(list,node);
	}
}
sformatmsg(list->fmmessage1,MSG_DISKSIZED,delstr->filecnt,delstr->dircnt,delstr->bytecnt);
list->fmmessage2[0]=0;
sizfailure1:
freemem(delstr);
fmmessage(list);	
outputlist(list);
endproc(list);
}

WORD sizedir(struct FMList *list,UBYTE *name,struct DelStr *ds,LONG *dsize)
{
struct FileInfoBlock *fib;
BPTR lock=0;
WORD ret=0;

sformatmsg(list->fmmessage1,MSG_DISKSIZING,ds->filecnt,ds->dircnt,ds->bytecnt);
if (testabort(list)) {
	list->fmmessage2[0]=0;
	fmmessage(list);
	if (askabort(list)) return(-1);
}
if (!(fib=allocmem(sizeof(struct FileInfoBlock)))) return(0);
AddPart(list->path,name,PATH);
if (currentdir(list)) {
	if ((lock=fminitdirread(list,fib,list->path))) {
		while(fmexnext(list,lock,fib)>0) {
			if(dmatch(fib,ds)) {
				if (fib->fib_DirEntryType>0) {
					ret=sizedir(list,fib->fib_FileName,ds,dsize);
					if(ret<=0) goto dderr;
				} else {
					ds->filecnt++;
					ds->bytecnt+=fib->fib_Size;
					*dsize+=fib->fib_Size;
				}
			}
			sformatmsg(list->fmmessage1,MSG_DISKSIZING,ds->filecnt,ds->dircnt,ds->bytecnt);
			sformat(list->fmmessage2,"'%s'",list->path);
		}
		UnLock(lock); lock=0;
	}
}
ret=1;
dderr:
*(PathPart(list->path))=0;
if (lock) UnLock(lock);
freemem(fib);
if(!ret) {
	ret=askcontinue(list);
} else {
	ds->dircnt++;
}
return(ret);
}

WORD dmatch(struct FileInfoBlock *fib,struct DelStr *ds)
{
struct FMNode *node;
WORD ret=0;

node=allocnode();
if(node) {
	strcpy(NDFILE(node),fib->fib_FileName);
	node->numprot=fib->fib_Protection;
	node->numlen=fib->fib_Size;
	node->numdate=dstolong(&fib->fib_Date);
	node->flags=fib->fib_DirEntryType>0?NDIRECTORY:NFILE;
	ret=checkmatch(ds->token1,ds->token2,ds->token3,node,&ds->match,0);
}
freevecpooled(node);
return(ret);
}

struct DelStr *createtoken(struct FMList *list)
{
struct DelStr *ds;
UBYTE patname[PATTERN];
UBYTE patflags[32];
struct Match *match;
UBYTE *ptr1,*ptr2;

ds=allocmem(sizeof(struct DelStr));
match=getconfignumber(PARSECONFIG)->moreconfig;
if(fmmain.kick<39&&match->mcase==1) {
	ptr1=match->patname;
	ptr2=patname;
	while(*ptr1) *ptr2++=ToUpper(*ptr1++);
	*ptr2=0;
} else {
	strcpy(patname,match->patname);
}
strcpy(patflags,match->patflags);
CopyMem(match,&ds->match,sizeof(struct Match));
ParsePatternNoCase(patname,ds->token1,1000);
ParsePattern(patname,ds->token2,1000);
ParsePatternNoCase(patflags,ds->token3,500);
return(ds);
}

WORD askcontinue(struct FMList *list)
{
WORD ret;

ret=requestmsg(getstring(MSG_MAIN_AREYOUSURE),MSG_YES,MSG_NO,MSG_MAIN_CONTINUE);
if(ret) ret=-1;
return(ret);
}

void *copyconfigdefault(struct CMenuConfig *cmc)
{
struct CopyConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct CopyConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct CopyConfig*)cmc->moreconfig;
config->bufsize=60000;
config->overwrite=1;
config->comment=1;
config->flags=1;
config->freespace=1;
return(cmc->moreconfig);
}

WORD copyconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
WORD ov,co,fl,da,fr,upd;
LONG buf;
struct CopyConfig *config;

config=(struct CopyConfig*)copyconfigdefault(cmc);
if(!config) return(0);
upd=config->update;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_REFRESH,200,guiUC|guiLEFT);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_OVERWRITE,201,guiUC|guiLEFT);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_COMMENT,202,guiUC|guiLEFT);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_PROTECTION,203,guiUC|guiLEFT);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_DATE,204,guiUC|guiLEFT);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_CHECKFREESPACE,205,guiUC|guiLEFT);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_COPYBUFFER,206,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqcyclemsg(gb,200,&upd,MSG_COPYMOVE_CONFIG_REFRESH1,MSG_COPYMOVE_CONFIG_REFRESH2,MSG_COPYMOVE_CONFIG_REFRESH3,0);
ov=config->overwrite;
reqcyclemsg(gb,201,&ov,MSG_COPYMOVE_CONFIG_OVERWRITE1,MSG_COPYMOVE_CONFIG_OVERWRITE2,0);
co=config->comment;
reqcycle2msg(gb,202,&co);
fl=config->flags;
reqcycle2msg(gb,203,&fl);
da=config->date;
reqcycle2msg(gb,204,&da);
fr=config->freespace;
reqcycle2msg(gb,205,&fr);
buf=config->bufsize;
reqinteger(gb,206,&buf,4000,99999999);
commandanswer(gb);
c=quickreq(gb);
config->overwrite=ov;
config->date=da;
config->comment=co;
config->flags=fl;
config->freespace=fr;
config->bufsize=buf;
config->update=upd;
return(c);
}

void *deleteconfigdefault(struct CMenuConfig *cmc)
{
struct DeleteConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct DeleteConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct DeleteConfig*)cmc->moreconfig;
config->askdelete=1;
config->askdir=1;
return(cmc->moreconfig);
}

WORD deleteconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
WORD adel,adir,prot,upd;
struct DeleteConfig *dc;

dc=getconfig(cmc);
upd=dc->update;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_COPYMOVE_CONFIG_REFRESH,200,guiUC|guiLEFT);
reqinfomsg(gb,MSG_DELETE_CONFIG_ASKDELETE,201,guiUC|guiLEFT);
reqinfomsg(gb,MSG_DELETE_CONFIG_ASKDIR,202,guiUC|guiLEFT);
reqinfomsg(gb,MSG_DELETE_CONFIG_DELPROT,203,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqcyclemsg(gb,200,&upd,MSG_COPYMOVE_CONFIG_REFRESH1,MSG_COPYMOVE_CONFIG_REFRESH2,MSG_COPYMOVE_CONFIG_REFRESH3,0);
adel=dc->askdelete;
reqcycle2msg(gb,201,&adel);
adir=dc->askdir;
reqcycle2msg(gb,202,&adir);
prot=dc->protected;
reqcycle2msg(gb,203,&prot);
commandanswer(gb);
c=quickreq(gb);
dc->askdelete=adel;
dc->askdir=adir;
dc->protected=prot;
dc->update=upd;
return(c);
}
