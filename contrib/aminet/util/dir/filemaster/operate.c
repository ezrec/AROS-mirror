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
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <devices/trackdisk.h>
#include <intuition/intuition.h>
#include <workbench/workbench.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <dos/rdargs.h>
#include <libraries/iffparse.h>
#include <proto/dos.h>
#include <stdio.h>
#include <string.h>

#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

struct OperateConfig {
	unsigned fffs:1;
	unsigned fint:1;
	unsigned fdir:1;
	unsigned fver:1;
};
extern UBYTE hsparwed[];

WORD format(struct FMList*,struct FMNode*,UBYTE*,struct CMenuConfig *cmc);
WORD openicon(struct FMList*);
WORD diskio(struct FMList*,struct IOStdReq*);
#ifdef __AROS__
void bootsum(UBYTE *);
#else
void __asm bootsum(register __a0 UBYTE *);
#endif
void drives(struct FMList*);

static ULONG bootblock[]=
	{
	0x00000370,0x43fa003e,0x70254eae,0xfdd84a80,
	0x670c2240,0x08e90006,0x00224eae,0xfe6243fa,0x00184eae,0xffa04a80,
	0x670a2040,0x20680016,0x70004e75,0x70ff4e75,0x646f732e,0x6c696272,
	0x61727900,0x65787061,0x6e73696f,0x6e2e6c69,0x62726172,0x79000000
	};

extern struct FMMain fmmain;
extern struct FMList fmlist[];
extern struct FMConfig *fmconfig;

static WORD createoperatewindow(UBYTE *title,struct FMNode *node,UBYTE *string1,UBYTE *string2,UBYTE *string3,UWORD cdir,BOOL bprot[])
{
struct GUIBase *gb;
WORD cnt2,cnt3,dev=0;
ULONG prot;
struct DosList *doslist;
struct FileSysStartupMsg *fssm;
struct DosEnvec *de;
UBYTE devname[32];

gb=getguibase(title);

strcpy(string1,NDFILE(node));
strcpy(string2,NDCOMM(node));
if(node->flags&NDEVICE) dev=1;

if (dev) string2[strlen(string2)-1]=0;
setconnectgroup(gb,9,0,0);

setconnectgroup(gb,1,1,9);
reqinfomsg(gb,MSG_OPERATE_ASKNAME,100,guiUC|guiLEFT);
if(!dev) {
	reqinfomsg(gb,MSG_OPERATE_ASKFLAGS,101,guiLEFT|guiUC);
	reqinfomsg(gb,MSG_OPERATE_ASKCOMMENT,102,guiLEFT|guiUC);
	reqinfomsg(gb,MSG_OPERATE_ASKDATE,103,guiLEFT|guiUC);
}

setconnectgroup(gb,2,1,9);
reqstring(gb,100,dev?string2:string1,30);
if(!dev) {
	setconnectgroup(gb,3,0,2);
	cnt3=128; prot=node->numprot^0x0f;
	for (cnt2=0;cnt2<8;cnt2++) {
		bprot[cnt2]=prot&cnt3?1:0;
		reqtoggle(gb,&hsparwed[cnt2*3],101,guiUC,&bprot[cnt2]);
		cnt3=cnt3>>1;
	}
	setguigroup(gb,2,0);
	reqstring(gb,102,string2,80);
	reqstring(gb,103,string3,32);
}
startbuttonbar(gb);
reqbuttonmsg(gb,MSG_OK,1,guiUC);
if(!dev) {
	if(cdir&1) reqbuttonmsg(gb,MSG_OPERATE_CREATEICON,2,guiUC);
	if(cdir&2) reqbuttonmsg(gb,MSG_ALL,3,guiUC);
} else {
	strcpy(devname,NDFILE(node));
	devname[strlen(devname)-1]=0;
	doslist=LockDosList(LDF_DEVICES|LDF_READ);
	doslist=FindDosEntry(doslist,devname,LDF_DEVICES);

	fssm=(struct FileSysStartupMsg*)BADDR((doslist->dol_misc.dol_handler.dol_Startup));

	if(fssm) {
		de=(struct DosEnvec*)BADDR(fssm->fssm_Environ);

		if(de->de_Surfaces*de->de_BlocksPerTrack*(de->de_HighCyl+1)*de->de_SizeBlock<786432) {
			reqbuttonmsg(gb,MSG_OPERATE_FORMATINSTALL,4,guiUC);
		}
	UnLockDosList(LDF_DEVICES|LDF_READ);
	}
}
reqbuttonmsg(gb,MSG_CANCEL,0,guiUC);
return(quickreq(gb));
}

static WORD operoi(struct FMList *list,struct FMNode *node,UBYTE *name,UBYTE *comm,ULONG date,BOOL bprot[],WORD pmode)
{
WORD cnt3,cnt2;
ULONG prot;

node->flags&=~NSELECTED;
if (name&&!(fmrename(list,node,name))) return(0);
if (comm&&!(fmcomment(list,node,comm))) return(0);
if (!fmfiledate(list,node,date)) return(0);
if (!pmode) return(1);
if(pmode==1)
	prot=0;
	else
	prot=node->numprot^0x0f;
cnt3=128;
for(cnt2=0;cnt2<8;cnt2++) {
	if (bprot[cnt2]) {
		switch(pmode) {
			case 1:
			case 2:
			prot|=cnt3;
			break;
			case 3:
			prot&=~cnt3;
			break;
		}
	}
	cnt3>>=1;
}
prot=prot^0x0f;
if (!(fmprotect(list,node,prot))) return(0);
return(1);
}


/*  combined Rename/Comment/Protect */

void __saveds operate(void)
{
struct ProcMsg *pm;
struct GUIBase *gb;
BOOL bprot[8];
struct FMList *list;
struct FMNode *node;
struct FMNode *node2;
UBYTE string1[32];
UBYTE string2[80];
UBYTE string3[32];
WORD ccom,cflags,cdate;
ULONG date;
WORD cnt2;
WORD ret;

pm=sinitproc();
list=fmmain.sourcedir;
node2=list->lastclicked;
list->lastclicked=0;
if (!(setalloc(list,1))) {
	initproc(0,0); 
	goto endi;
}
initproc(list,pm->cmc->label);
if(!node2&&!findselnodeall(list)) {
	strcpymsg(list->fmmessage1,MSG_MAIN_NOSOURCEFILE);
	fmmessage(list);
}
for(;;) {
	if (node2) {
		node=node2;
	} else {
		node=findselnodeall(list);
	}
	if(!node) break;
	endofdirtxt(list,node);
	if(node->flags&NASSIGN) {
		node->flags&=~NSELECTED;
		outputlistline(list,node);
		break;
	}
	longtodatestring(string3,node->numdate);
	cnt2=createoperatewindow(list->workname,node,string1,string2,string3,2,bprot);
	if(!cnt2) break;
	date=datestringtolong(string3);

	if (cnt2==4) {
		format(list,node,string2,pm->cmc);
		node->flags&=~NSELECTED;
		break;
	}
	if (cnt2==3) {
			gb=getguibase(list->workname);
			cflags=0;ccom=0;cdate=0;
			setconnectgroup(gb,1,0,0);
			reqinfomsg(gb,MSG_OPERATE_ASKFLAGS,100,guiUC|guiLEFT);
			reqcyclemsg(gb,100,&cflags,MSG_NON,MSG_OPERATE_CHANGE,MSG_OPERATE_ADD,MSG_OPERATE_SUB,0);
			setconnectgroup(gb,2,0,0);
			reqinfomsg(gb,MSG_OPERATE_ASKCOMMENT,101,guiUC|guiLEFT);
			reqcyclemsg(gb,101,&ccom,MSG_NON,MSG_OPERATE_CHANGE,0);
			setconnectgroup(gb,3,0,0);
			reqinfomsg(gb,MSG_OPERATE_ASKDATE,102,guiUC|guiLEFT);
			reqcyclemsg(gb,102,&cdate,MSG_NON,MSG_OPERATE_CHANGE,0);
			buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
			ret=quickreq(gb);
			if(ret==1) {
				while((node=findselnodeall(list))) {
					node->flags&=~NSELECTED;
					if(!operoi(list,node,0,ccom?string2:0,cdate?date:0xffffffff,bprot,cflags)) goto opend;
					outputlistline(list,node);
				}
				endofdirtxt(list,node);
			}
			goto opend;
	}
	if (cnt2==1) {
		if (list->flags&LDIRLIST) {
			if(!operoi(list,node,string1,string2,date,bprot,1)) goto opend;
		} else if (node->flags&NDEVICE) {
			if (!Relabel(NDFILE(node),string2)) {
				dosrequestmsg(list,0,MSG_FMDOS_RELABELERR,NDCOMM(node));
				break;
			} else {
				sformat(NDCOMM(node),"%s:",string2);
			}
		}
	}
	node->flags&=~NSELECTED;
	endofdirtxt(list,node);
	outputlistline(list,node);
	if (node2) break;
}
opend:
csortlist(list);
fmmessage(list);
outputlist(list);
endproc(list);
endi:
deinitproc(pm);
}

void __saveds createdir(void)
{
struct ProcMsg *pm;
struct FMList *list;
struct FMNode *node=0,*node2=0;
struct DiskObject *dob;
UBYTE string1[32];
UBYTE string2[80];
UBYTE string3[32];
ULONG prot,date;
WORD cnt2,cnt3;
UBYTE vstring[40];
BOOL bprot[8];

pm=sinitproc();
list=fmmain.sourcedir;
if (!(setalloc(list,1))) {
	initproc(0,0); 
	goto endi;
}
initproc(list,pm->cmc->label);
priority(pm->cmc);
if(!sselected(list,0)) goto crerr;
if (!(node=allocnode())) {
	endproc(list);
	goto endi;
}
string3[0]=0;
cnt2=createoperatewindow(list->workname,node,string1,string2,string3,1,bprot);
date=datestringtolong(string3);
freevecpooled(node); node=0;
if (cnt2&&string1[0]) {
	if ((node=fmcreatedir(list,string1))) {
		if(cnt2==2) {
			if(openicon(list)) {
				dob=GetDefDiskObject(WBDRAWER);
				if(!dob) {
					requestmsg(list->workname,0,MSG_OK,MSG_FMDOS_OPENERR,getstring(MSG_OPERATE_DEFAULTDRAWER));
				} else {
					if(PutDiskObject(string1,dob)) {
						node2=dupnode(node);
						if(node2) {
							node2->flags|=NFILE;
							node2->flags&=~NDIRECTORY;
							strcpy(vstring,string1);
							strcpy(vstring+strlen(vstring),".info");
							strncpy(NDFILE(node2),vstring,fmmain.filelen);
						}
					} else {
						dosrequestmsg(list,0,MSG_FMDOS_WRITEERR,getstring(MSG_OPERATE_DEFAULTDRAWER));
					}
					FreeDiskObject(dob);
				}
			}
		}
		cnt3=128; prot=0;
		for(cnt2=0;cnt2<8;cnt2++) {
			if (bprot[cnt2]) prot|=cnt3;
			cnt3>>=1;
		}
		prot=prot^0x0f;
		fmprotect(list,node,prot);
		fmcomment(list,node,string2);
		fmfiledate(list,node,date);
		if(node2) {
			fmprotect(list,node2,prot);
			fmcomment(list,node2,string2);
			fmfiledate(list,node2,date);
		}
		node->flags&=~NSELECTED;
		if(node2) node2->flags&=~NSELECTED;
	}
}
fmmessage(list);
crerr:
if(node) {
	examinefile(list,node);
	AddTail((struct List*)list,(struct Node*)node);
}
if(node2) {
	examinefile(list,node2);
	AddTail((struct List*)list,(struct Node*)node2);
}
if(node) endofdirtxt(list,node);
csortlist(list);
parselist(list,0);
outputlist(list);
updadirmeter(list);
endproc(list);
endi:
deinitproc(pm);
}

WORD format(struct FMList *list,struct FMNode *node,UBYTE *name,struct CMenuConfig *cmc)
{
struct GUIBase *gb;
struct OperateConfig *operateconfig;
WORD ret;
struct DosList *doslist;
struct FileSysStartupMsg *fssm;
struct DosEnvec *de;
struct Resident *res1;
struct Resident *res2;
struct IOStdReq *ioreq=0;
UBYTE devname[32];
APTR seglist;
WORD romfs=0,jatko=0;
ULONG dostype;
UBYTE dt;
WORD mver,mffs,mint,mdir;
WORD trackcnt,surfacecnt;
UBYTE *formatdata=0,*verifydata=0;
LONG formatlen,cylcnt;
UBYTE *apuptr;
LONG apu1;

mffs=mint=0;

operateconfig=getconfig(cmc);
if(*name==0) strcpymsg(name,MSG_OPERATE_DEFAULTNAME);
gb=getguibase(cmc->label);

setconnectgroup(gb,1,0,0);
reqinfomsg(gb,MSG_OPERATE_ASKNAME,101,guiUC|guiLEFT);
reqstring(gb,101,name,30);
mver=operateconfig->fver;
reqtogglemsg(gb,MSG_OPERATE_VERIFY,102,guiUC,&mver);

doslist=LockDosList(LDF_DEVICES|LDF_READ);
strcpy(devname,NDFILE(node));
devname[strlen(devname)-1]=0;
doslist=FindDosEntry(doslist,devname,LDF_DEVICES);
res1=FindResident("filesystem");
res2=FindResident("fastfilesystem");

fssm=(struct FileSysStartupMsg*)BADDR(doslist->dol_misc.dol_handler.dol_Startup);
de=(struct DosEnvec*)BADDR(fssm->fssm_Environ);

dostype=de->de_DosType;

#ifndef AROS
seglist=(APTR)BADDR(doslist->dol_misc.dol_handler.dol_SegList);
#else
seglist=(APTR)(doslist->dol_misc.dol_handler.dol_Handler);
#endif

mdir=0;
if((res1&&seglist==res1->rt_Init)||(res2&&seglist==res2->rt_Init)) {
	setconnectgroup(gb,2,0,0);
	romfs=1;
	mffs=operateconfig->fffs;
	reqtogglemsg(gb,MSG_OPERATE_FFS,103,guiUC,&mffs);
	mint=operateconfig->fint;
	reqtogglemsg(gb,MSG_OPERATE_INT,104,guiUC,&mint);
	#ifdef V39
	mdir=operateconfig->fdir;
	reqtogglemsg(gb,MSG_OPERATE_DIRCACHE,105,guiUC,&mdir);
	#endif
}
UnLockDosList(LDF_DEVICES|LDF_READ);
buttonbarmsg(gb,MSG_OPERATE_DOFORMAT,1,MSG_OPERATE_DOQFORMAT,2,MSG_OPERATE_DOINSTALL,3,MSG_CANCEL,0,0);

ret=quickreq(gb);
if(!ret) return(0);
operateconfig->fver=mver;
operateconfig->fffs=mffs;
operateconfig->fint=mint;
operateconfig->fdir=mdir;
if(!Inhibit(NDFILE(node),DOSTRUE)) {
	dosrequestmsg(list,0,MSG_OPERATE_INHIBITERR);
	return(0);
}
list->fmmessage2[0]=0;

if(ret==1) {
	formatlen=de->de_SizeBlock*de->de_BlocksPerTrack*4;
	apuptr=formatdata=allocvec(list,formatlen,MEMF_PUBLIC);
	if(!formatdata) goto fend;
	if(operateconfig->fver) {
		verifydata=allocvec(list,formatlen,MEMF_PUBLIC);
		if(!verifydata) goto fend;
	}
	apu1=formatlen;
	while(apu1!=0) {
		*(ULONG*)apuptr=MAKE_ID('F','M','3','0');
		apuptr+=4;
		apu1-=4;
	}	
	*(ULONG*)formatdata=0x42414400;	/* BAD0 */

	ioreq=(struct IOStdReq*)opendevice(list,(UBYTE*)BADDR(fssm->fssm_Device)+1,fssm->fssm_Unit,fssm->fssm_Flags,sizeof(struct IOStdReq));

	if(!ioreq) goto fend;
	list->fmmessage2[0]=0;
	ioreq->io_Command=TD_FORMAT;
	ioreq->io_Offset=0;
	ioreq->io_Data=formatdata;
	ioreq->io_Length=formatlen;
	if(!diskio(list,ioreq)) goto fend;
	*((ULONG*)formatdata)=MAKE_ID('F','M','3','0');
	surfacecnt=de->de_Surfaces; trackcnt=de->de_HighCyl;
	cylcnt=(surfacecnt*trackcnt+1)*formatlen;
	surfacecnt--;
	while(cylcnt>=0) {
		sformat(list->fmmessage1,getstring(MSG_OPERATE_FORMATTING),trackcnt,surfacecnt);
		fmmessage(list);
		ioreq->io_Offset=cylcnt;
		ioreq->io_Data=formatdata;
		ioreq->io_Length=formatlen;
		ioreq->io_Command=TD_FORMAT;
		if(!diskio(list,ioreq)) goto fend;
		if(operateconfig->fver) {
			sformat(list->fmmessage1,getstring(MSG_OPERATE_VERIFYING),trackcnt,surfacecnt);
			fmmessage(list);
			ioreq->io_Offset=cylcnt;
			ioreq->io_Data=verifydata;
			ioreq->io_Length=formatlen;
			ioreq->io_Command=CMD_READ;
			if(!diskio(list,ioreq)) goto fend;
		}
		if(testabort(list)) {
			ioreq->io_Command=TD_MOTOR;
			ioreq->io_Length=0;
			if (!diskio(list,ioreq)) goto fend;
			if (askabort(list)) {
				ret=0;
				break;
			}
		}
		cylcnt-=formatlen;
		if(--surfacecnt<0) {
			surfacecnt=de->de_Surfaces-1;
			trackcnt--;
		}
	}
	ioreq->io_Command=CMD_UPDATE;
	if (!diskio(list,ioreq)) goto fend;
	jatko=1;
}

if(ret==1||ret==2) {
	closedevice((struct IORequest*)ioreq); ioreq=0;
	if(romfs) {
		dt=operateconfig->fffs|(operateconfig->fint<<1)|(operateconfig->fdir<<2);
		if(dt>=6) dt-=2;
		dostype=0x444f5300|dt;
	}
	strcpymsg(list->fmmessage1,MSG_OPERATE_INITIALIZING);
	fmmessage(list);
	if (!Format(NDFILE(node),name,dostype)) {
		dosrequestmsg(list,0,MSG_OPERATE_INITIALIZEERR);
		goto fend;
	}
	jatko=1;
}

if(ret==3||jatko) {
	strcpymsg(list->fmmessage1,MSG_OPERATE_INSTALLING);
	fmmessage(list);
	formatlen=de->de_SizeBlock*4;
	formatdata=allocvec(list,formatlen,MEMF_PUBLIC);
	if(!formatdata) goto fend;

	ioreq=(struct IOStdReq*)opendevice(list,(UBYTE*)BADDR(fssm->fssm_Device)+1,fssm->fssm_Unit,fssm->fssm_Flags,48);

	if(!ioreq) goto fend;
	ioreq->io_Command=CMD_READ;
	ioreq->io_Offset=0;
	ioreq->io_Data=formatdata;
	ioreq->io_Length=formatlen;
	if (diskio(list,ioreq)) {
		memseti(formatdata+4,0,formatlen-4);
		if(formatdata[0]=='D'&&formatdata[1]=='O'&&formatdata[2]=='S') {
			CopyMem(bootblock,formatdata+8,sizeof(bootblock));
			bootsum(formatdata);
			ioreq->io_Command=CMD_WRITE;
			ioreq->io_Offset=0;
			ioreq->io_Data=formatdata;
			ioreq->io_Length=formatlen;
			diskio(list,ioreq);
		} else {
			if(!jatko) {
				requestmsg(list->workname,0,MSG_OK,MSG_OPERATE_INSTALLERR);
			}
		}
	}
}
fend:
freemem(formatdata);
freemem(verifydata);
ioreq->io_Command=CMD_UPDATE;
diskio(list,ioreq);
closedevice((struct IORequest*)ioreq);
Inhibit(NDFILE(node),FALSE);
list->fmmessage1[0]=0;
list->fmmessage2[0]=0;
fmmessage(list);
updadirmeter(list);
for(ret=0;ret<LISTS;ret++) {
	if (fmlist[ret].flags&LDEVLIST) drives(&fmlist[ret]);
}
return(1);
}

WORD diskio(struct FMList *list,struct IOStdReq *req)
{
UBYTE ret;
WORD cnt;
WORD ocommand;
LONG olength;
UBYTE *errtxt;

ocommand=req->io_Command;
olength=req->io_Length;
do {
	req->io_Command=ocommand;
	req->io_Length=olength;
	DoIO((struct IORequest*)req);
	ret=req->io_Error;
	if(!ret) return(1);
	req->io_Command=TD_MOTOR;
	req->io_Length=0;
	DoIO((struct IORequest*)req);
	errtxt=getstring(MSG_DEVICE_ERRORS);
	for(cnt=20;cnt<36;cnt++) {
		if(cnt==ret) break;
		errtxt+=strlen(errtxt)+1;
	}
	if(ret==252) errtxt+=strlen(errtxt)+1;
} while(requestmsg(list->workname,MSG_RETRY,MSG_CANCEL,MSG_DEVICE_ERROR,errtxt,ret));
return(0);
}

void *operateconfigdefault(struct CMenuConfig *cmc)
{
struct OperateConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct OperateConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct OperateConfig*)cmc->moreconfig;
config->fffs=1;
return(cmc->moreconfig);
}
