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
#include <dos/dosextens.h>

#ifndef AROS
#include <proto/xfdmaster.h>
#include <libraries/xfdmaster.h>
#endif

#include <stdio.h>
#include <string.h>
#include "fmnode.h"
#include "fmlocale.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;


extern struct FMConfig *fmconfig;
extern UBYTE nformatstring[];
extern struct FMMain fmmain;
extern UBYTE pplib[];

#ifndef AROS
WORD xfddecrunch(struct FMHandle *h,struct xfdBufferInfo *xfdbi)
{
UBYTE *obuf;

obuf=xfdbi->xfdbi_SourceBuffer;
if(xfdbi->xfdbi_SourceBuffer=allocvec(h->owner,h->size,0)) {
	CopyMem(obuf,xfdbi->xfdbi_SourceBuffer,xfdbi->xfdbi_SourceBufLen);
	freemem(obuf);
	if(readbufferfile(h,(UBYTE*)xfdbi->xfdbi_SourceBuffer+xfdbi->xfdbi_SourceBufLen,h->size-xfdbi->xfdbi_SourceBufLen)==h->size-xfdbi->xfdbi_SourceBufLen) {
		xfdbi->xfdbi_SourceBufLen=h->size;
		xfdbi->xfdbi_TargetBufMemType=MEMF_PUBLIC;
		sformat(h->owner->fmmessage1,getstring(MSG_FMDOS_DECRUNCHING),xfdbi->xfdbi_PackerName,h->filename);
		h->owner->fmmessage2[0]=0;
		fmmessage(h->owner);
		if(xfdRecogBuffer(xfdbi)) {
			if(xfdDecrunchBuffer(xfdbi)) {
				h->owner->fmmessage1[0]=0;
				h->owner->fmmessage2[0]=0;
				fmmessage(h->owner);
				h->flags|=OFDECRUNCH;
				h->decbuffer=xfdbi->xfdbi_TargetBuffer;
				h->decbufsize=xfdbi->xfdbi_TargetBufLen;
				h->size=xfdbi->xfdbi_TargetBufSaveLen;
				h->position=0;
				freemem(xfdbi->xfdbi_SourceBuffer);
				xfdFreeObject(xfdbi);
				return(1);
			}
		}
	}
}
if(xfdbi->xfdbi_SourceBuffer) freemem(xfdbi->xfdbi_SourceBuffer);
xfdFreeObject(xfdbi);
return(0);
}


struct xfdBufferInfo *xfdtest(struct FMHandle *h)
{
struct xfdBufferInfo *xfdbi;
UBYTE *recbuf;
LONG recbuflen;
WORD ret=0;

xfdbi=xfdAllocObject(XFDOBJ_BUFFERINFO);
if(xfdbi) {
	recbuflen=xfdMasterBase->xfdm_MinBufferSize;
	if(h->size<recbuflen) recbuflen=h->size;
	xfdbi->xfdbi_SourceBufLen=recbuflen;
	recbuf=allocvec(h->owner,recbuflen,0);
	xfdbi->xfdbi_SourceBuffer=recbuf;
	xfdbi->xfdbi_Flags|=XFDPFF_EXTERN;
	if(xfdbi->xfdbi_SourceBuffer) {
		if(readbufferfile(h,recbuf,recbuflen)==recbuflen) {
			if(xfdRecogBuffer(xfdbi)) ret=1;
		}
	}
	if(!ret) {
		if(xfdbi->xfdbi_SourceBuffer) freemem(xfdbi->xfdbi_SourceBuffer);
		xfdFreeObject(xfdbi);
		xfdbi=0;
	}
}
if(!xfdbi) seek(h,0,OFFSET_BEGINNING);
return(xfdbi);
}
#endif

LONG fmread(struct FMHandle *h,void *buffer,LONG size)
{
return(Read(h->handle,buffer,size));
}

LONG seek(struct FMHandle *h,LONG p,LONG mode)
{
LONG ret,ret2=0;

if (h->flags&OFDECRUNCH) {
	ret=h->position;
	switch(mode)
	{
		case OFFSET_CURRENT:
		h->position+=p;
		break;
		case OFFSET_BEGINNING:
		h->position=p;
		break;
		case OFFSET_END:
		h->position=h->size+p;
		break;
	}
	if (h->position<0||h->position>h->size) ret=-1;
} else {
	ret=Seek(h->handle,p,mode);
	if(ret>=0) ret2=Seek(h->handle,0,OFFSET_CURRENT);
	if (ret<0||ret2<0) {
		dosrequestmsg(h->owner,0,MSG_FMDOS_SEEKERR,h->filename);
	} else {
		h->position=ret2;
	}
}
return(ret);
}


LONG readbufferfile(struct FMHandle *handle,void *buffer,LONG len)
{
LONG ret=-1;
LONG len2;
LONG oldseek;
UBYTE *src,*dst;

oldseek=handle->position;
rretry:
if(handle->size-handle->position>=len) {
	if (handle->flags&OFDECRUNCH) {
		src=handle->decbuffer+handle->position;
		dst=(UBYTE*)buffer;
		len2=len; ret=0;
		while(len2&&handle->position<handle->size) {
			*dst++=*src++;
			len2--;
			ret++;
		}
		handle->position+=ret;
	} else {
		ret=fmread(handle,buffer,len);
		if (ret==-1) {
			if(dosrequestmsg(handle->owner,1,MSG_FMDOS_READERR,handle->filename)) {
				if(seek(handle,oldseek,OFFSET_BEGINNING)>=0) goto rretry;
			}
			return(ret);
		} else {
			handle->position+=ret;
		}
	}
}
if (ret!=len) {
	requestmsg(handle->owner->workname,0,MSG_OK,MSG_FMDOS_UEOF,handle->filename);
	ret=0;
}
return(ret);
}

LONG readfile(struct FMHandle *handle)
{
LONG ret;
LONG oldseek;

oldseek=handle->position;
rretry:
ret=fmread(handle,handle->buffer,handle->bufsize);
if (ret==-1) {
	if(dosrequestmsg(handle->owner,1,MSG_FMDOS_READERR,handle->filename)) {
		if(seek(handle,oldseek,OFFSET_BEGINNING)>=0) goto rretry;
	}
	return(ret);
}
handle->position+=ret;
return(ret);
}

LONG writefile(struct FMHandle *handle,void *buffer,LONG size)
{
LONG ret;
LONG oldseek;

oldseek=handle->position;
wretry:
if (handle->flags&OFDECRUNCH) {
	ret=-1;
} else {
	ret=Write(handle->handle,buffer,size);
	if (ret==-1||ret!=size) {
		if(dosrequestmsg(handle->owner,1,MSG_FMDOS_WRITEERR,handle->filename)) {
			if(seek(handle,oldseek,OFFSET_BEGINNING)>=0) goto wretry;
		}
	} else {
		handle->position+=size;
	}
}
return(ret);
}

struct FMHandle* openfile(struct FMList *list,UBYTE *name,ULONG flags)
{
struct FMHandle *handle=0;
struct FileInfoBlock *fib=0;
ULONG bufsize;
WORD retry;

if (!currentdir(list)) return(0);

oretry:
retry=0;
if (!(handle=(struct FMHandle*)AllocVec(sizeof(struct FMHandle),MEMF_CLEAR|MEMF_PUBLIC))) goto ofile1;
handle->path=list->path;
handle->owner=list;
handle->filename=name;
bufsize=fmconfig->dosbuffersize&0x00ffffff;
if(flags&OFBUFFER) {
	bufsize=flags&~OFBUFFER;
	flags=OFBUFFER|OFRELAXED;
}
if (flags&OFWRITE) {
	if (!(handle->handle=Open(name,1006))) {
		retry=dosrequestmsg(list,1,MSG_FMDOS_OPENWERR,name);
		goto ofile1;
	}
handle->flags|=OFWRITE;
return(handle);
}
if (!(flags&OFFAKE)) {
	if (!(handle->handle=Open(name,1005))) {
		retry=dosrequestmsg(list,1,MSG_FMDOS_OPENRERR,name);
		goto ofile1;
	}
	if (!(fib=AllocVec(sizeof(struct FileInfoBlock),MEMF_CLEAR))) goto ofile1;
	if (!fmexaminefile(list,fib,name)) goto ofile1;
	handle->size=fib->fib_Size;
	handle->date=dstolong(&fib->fib_Date);
	freemem(fib); fib=0;
}

if (flags&OFNORMAL) {
	handle->bufsize=bufsize;
	handle->buffer=AllocVec(handle->bufsize,MEMF_CLEAR|MEMF_PUBLIC);
	handle->flags|=OFNORMAL;
} else if (flags&OFBUFFER) {
	for(;;) {
		if(bufsize<handle->size&&bufsize<4000) break;
		handle->bufsize=bufsize;
		if ((handle->buffer=AllocVec(bufsize,MEMF_CLEAR|MEMF_PUBLIC))) break;
		bufsize/=2;
	}
}
if (!handle->buffer) {
	retry=requestmsg(list->workname,MSG_RETRY,MSG_CANCEL,MSG_FMDOS_NOBUFMEM);
	goto ofile1;
}
if(!(flags&OFFAKE)) {
	if (handle->size==0&&!(flags&OFRELAXED)) {
		requestmsg(list->workname,0,MSG_OK,MSG_FMDOS_ZEROSIZE,handle->filename);
		goto ofile1;
	}
#ifndef AROS
	if (flags&OFDECRUNCH&&handle->size&&xfdMasterBase) {
		if(xfdbi=xfdtest(handle)) {
			if(!xfddecrunch(handle,xfdbi)) goto ofile1;
		}
	}
} else {
	handle->flags|=OFDECRUNCH|OFFAKE;
#endif
}
return(handle);
ofile1:
freemem(fib);
closefile(handle);
if(retry) goto oretry;
return(0);
}

void closefile(struct FMHandle *handle)
{
if (!handle) return;
if (handle->flags&OFDECRUNCH&&!(handle->flags&OFFAKE)) FreeMem(handle->decbuffer,handle->decbufsize);
freemem(handle->buffer);
if (handle->handle) Close(handle->handle);
freemem(handle);
}

WORD fmexaminefile(struct FMList *list,struct FileInfoBlock *fib,UBYTE *name)
{
BPTR lock;
WORD ret;

if (!(lock=fmlock(list,name))) return(0);
ret=fmexamine(list,lock,fib,name);
UnLock(lock);
return(ret);
}

WORD fmexamine(struct FMList *list,BPTR lock,struct FileInfoBlock *fib,UBYTE *name)
{
do {
	if (Examine(lock,fib)) return(1);
} while(dosrequestmsg(list,1,MSG_FMDOS_EXAMINEERR,name));
return(0);
}

WORD fmexnext(struct FMList *list,BPTR lock,struct FileInfoBlock *fib)
{
struct FileInfoBlock vfib;

CopyMem(fib,&vfib,sizeof(struct FileInfoBlock));
do {
	CopyMem(&vfib,fib,sizeof(struct FileInfoBlock));
	if (ExNext(lock,fib)) return(1);
	if(checkdoserr()==232) return(0);
} while(dosrequestmsg(list,1,MSG_FMDOS_EXNEXTERR));
return(-1);
}
BPTR fmlock(struct FMList *list,UBYTE *name)
{
BPTR lock;

/* AROS Hack ************************************************/
if(name!=list->path) if(!currentdir(list)) return(0);
do {
	if ((lock=Lock(name,SHARED_LOCK))) return(lock);
} while(strcat(name, ":"));
/**********************************************************/

if(name!=list->path) if(!currentdir(list)) return(0);
do {
        if ((lock=Lock(name,SHARED_LOCK))) return(lock);
} while(dosrequestmsg(list,1,MSG_FMDOS_LOCKERR,name));

return(0);
}

WORD currentdir(struct FMList *list)
{
BPTR lock;
BPTR curri;

if (!(lock=Lock(list->path,SHARED_LOCK))) return(0);
curri=CurrentDir(lock);
if (list->pair) {
	if (list->flags&LSUBPROC) {
		if(list->oldcur==(BPTR)1) { list->oldcur=curri; curri=0; }
		goto curend;
	}
	if (list->pair->flags&LSUBPROC) { 
		if(list->pair->oldcur==(BPTR)1) { list->pair->oldcur=curri; curri=0; }
		goto curend;
	}
}
if (list->flags&LSUBPROC) {
	if (list->oldcur==(BPTR)1) { list->oldcur=curri; curri=0; }
	goto curend;
}
if (!(list->flags&LSUBPROC)) {
	if (fmmain.oldcur==(BPTR)1) { fmmain.oldcur=curri; curri=0; }
}
curend:
UnLock(curri);
return(1);
}

WORD fmsetrename(struct FMList *list,UBYTE *oldname,UBYTE *newname)
{
if(!currentdir(list)) return(0);
do {
	if(Rename(oldname,newname)) return(1);
} while(dosrequestmsg(list,1,MSG_FMDOS_RENAMEERR,oldname));
return(0);
}
WORD fmrename(struct FMList *list,struct FMNode *node,UBYTE *name)
{
if (strcmp(NDFILE(node),name)) {
	if(fmsetrename(list,NDFILE(node),name)) {
		siirran(NDFILE(node),name,fmmain.filelen);
		return(1);
	}
	return(0);
}
return(1);
}

WORD fmsetfiledate(struct FMList *list,UBYTE *name,struct DateStamp *ds)
{
if(!currentdir(list)) return(0);
do {
	if(SetFileDate(name,ds)) return(1);
} while(dosrequestmsg(list,1,MSG_FMDOS_DATEERR,name));
return(0);
}
WORD fmfiledate(struct FMList *list,struct FMNode *node,ULONG date)
{
struct DateStamp ds;
UBYTE dstring[32];

if(date==0xffffffff) return(1);
longtodatestring(dstring,node->numdate);
if(datestringtolong(dstring)!=date) {
	longtods(&ds,date);
	if(fmsetfiledate(list,NDFILE(node),&ds)) {
		node->numdate=date;
		longtodatestring(NDDATE(node),date);
		return(1);
	}
	return(0);
}
return(1);
}

WORD fmsetcomment(struct FMList *list,UBYTE *name,UBYTE *comm)
{
if(!currentdir(list)) return(0);
do {
	if (SetComment(name,comm)) return(1);
} while(dosrequestmsg(list,1,MSG_FMDOS_COMMENTERR,name));
return(0);
}
WORD fmcomment(struct FMList *list,struct FMNode *node,UBYTE *comm)
{
if (strcmp(NDCOMM(node),comm)) {
	if(fmsetcomment(list,NDFILE(node),comm)) {
		siirran(NDCOMM(node),comm,fmmain.commlen);
		return(1);
	}
	return(0);
}
return(1);
}

WORD fmsetprotection(struct FMList *list,UBYTE *name,ULONG prot)
{
if(!currentdir(list)) return(0);
do {
	if(SetProtection(name,prot)) return(1);
} while(dosrequestmsg(list,1,MSG_FMDOS_PROTERR,name));
return(0);
}

UBYTE hsparwed[]={"_H\0_S\0_P\0_A\0_R\0_W\0_E\0_D\0"};

WORD fmprotect(struct FMList *list,struct FMNode *node,ULONG prot)
{
WORD cnt,cnt2;

if (node->numprot!=prot) {
	if(fmsetprotection(list,NDFILE(node),prot)) {
		node->numprot=prot;
		prot=prot^0x0f; cnt2=128;
		for(cnt=0;cnt<8;cnt++) {
			if(prot&cnt2) *(NDPROT(node)+cnt)=hsparwed[cnt*3+1]; else *(NDPROT(node)+cnt)='-';
			cnt2=cnt2>>1;
		}
		return(1);
	}
	return(0);
}
return(1);
}

struct FMNode* fmcreatedir(struct FMList *list,UBYTE *name)
{
BPTR lock;
struct FMNode *node;

if (!(node=allocnode())) return(0);
if (currentdir(list)) {
	do {
		if ((lock=CreateDir(name))) {
			UnLock(lock);
			node->flags|=NDIRECTORY;
			siirran(NDFILE(node),name,fmmain.filelen);
			examinefile(list,node);
			return(node);
		}
	} while(dosrequestmsg(list,1,MSG_FMDOS_MAKEDIRERR,name));
}
freevecpooled(node);
return(0);
}

WORD examinefile(struct FMList *list,struct FMNode *node)
{
BPTR lock;
struct FileInfoBlock *fib;
struct Process *proc;
void *oldwinptr;
ULONG protect;
WORD cnt,cnt2;

if (!(fib=AllocVec(sizeof(struct FileInfoBlock),MEMF_CLEAR))) return(0);
proc=(struct Process*)FindTask(0);
oldwinptr=proc->pr_WindowPtr;
proc->pr_WindowPtr=(APTR)-1;
if (currentdir(list)) {
	if ((lock=fmlock(list,NDFILE(node)))) {
		if (fmexamine(list,lock,fib,NDFILE(node))) {
			node->numdate=dstolong(&fib->fib_Date);
			longtodatestring(NDDATE(node),node->numdate);
			siirran(NDFILE(node),fib->fib_FileName,fmmain.filelen);
			siirran(NDCOMM(node),fib->fib_Comment,fmmain.commlen);
			node->numprot=fib->fib_Protection;
			protect=fib->fib_Protection^0x0f; cnt2=128;
			for(cnt=0;cnt<8;cnt++) {
				if(protect&cnt2) *(NDPROT(node)+cnt)=hsparwed[cnt*3+1]; else *(NDPROT(node)+cnt)='-';
				cnt2=cnt2>>1;
			}
			if(fib->fib_DirEntryType<0) {
				node->numlen=fib->fib_Size;
				node->flags|=NFILE;
				sformat(NDLENGTH(node),nformatstring,fib->fib_Size);
			} else {
				node->flags|=NDIRECTORY;
			}
			UnLock(lock);
			freemem(fib);
			proc->pr_WindowPtr=oldwinptr;
			return(1);
		}
		UnLock(lock);
	}
}
proc->pr_WindowPtr=oldwinptr;
freemem(fib);
return(0);
}

BPTR fminitdirread(struct FMList *list,struct FileInfoBlock *fib,UBYTE *name)
{
BPTR lock;

if ((lock=fmlock(list,name))) {
	if (fmexamine(list,lock,fib,name)) {
		return(lock);
	}
UnLock(lock);
}
return(0);
}

void *allocvecpooled(LONG a)
{
SIPTR *ret;

if(fmmain.pool) {
	ObtainSemaphore(&fmmain.poolsema);
	a+=4;
	if ((ret=(SIPTR*)AllocPooled(fmmain.pool,a))) *ret++=a;
	ReleaseSemaphore(&fmmain.poolsema);
} else {
	ret=AllocVec(a,MEMF_PUBLIC|MEMF_CLEAR);
}
return((void*)ret);
}
void freevecpooled(void *a)
{
SIPTR *b;

if(fmmain.pool) {
	ObtainSemaphore(&fmmain.poolsema);
	b=((SIPTR *)a)-1;
	FreePooled(fmmain.pool,b,*b);
	ReleaseSemaphore(&fmmain.poolsema);
} else {
	freemem(a);
}
}
void *allocmem(LONG size)
{
return(AllocVec(size,MEMF_CLEAR));
}
void freemem(void *ptr)
{
FreeVec(ptr);
}
void *allocvec(struct FMList *l,LONG a,LONG b)
{
void *ret;
UBYTE *ptr;

do {
	if((ret=AllocVec(a,b))) return((void*)ret);
	if(!l) return(0);
	if (b&MEMF_CHIP) ptr=getstring(MSG_FMDOS_CHIPMEM); else ptr=getstring(MSG_FMDOS_PUBLICMEM);
} while(requestmsg(l->workname,MSG_RETRY,MSG_CANCEL,MSG_FMDOS_ALLOCATEERR,a,ptr));
return(0);
}

struct IORequest* opendevice(struct FMList *l,UBYTE *name,WORD unit,ULONG flags,WORD iolen)
{
struct MsgPort *mp;
struct IORequest *io;

if ((mp=CreateMsgPort())) {
	if ((io=CreateIORequest(mp,iolen))) {
		if (!OpenDevice(name,unit,io,flags)) return(io);
		DeleteIORequest(io);
	}
	DeleteMsgPort(mp);
}
requestmsg(l->workname,0,MSG_OK,MSG_FMDOS_OPENERR,name);
return(0);
}

void closedevice(struct IORequest *ioreq)
{
struct MsgPort *mp;

if(!ioreq) return;
CloseDevice(ioreq);
mp=ioreq->io_Message.mn_ReplyPort;
DeleteIORequest(ioreq);
DeleteMsgPort(mp);
}

WORD checkdoserr(void)
{
LONG error;

error=IoErr();
SetIoErr(error);
return((WORD)error);
}
