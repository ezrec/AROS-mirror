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
#include <dos/dostags.h>
#include <dos/dosextens.h>
#include <strings.h>
#include <stdio.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;

struct ExecuteConfig {
	UBYTE shellwindow[80];
	BYTE timeout;
};

void executefile(struct FMHandle *h,struct CMenuConfig *cmc);
void executeexe(struct FMList*,struct FMList*,UBYTE*,struct CMenuConfig*,struct ExecuteFlags*);
void executeown(struct FMList*,struct FMList*,struct FMNode*,struct CMenuConfig*);
WORD deloldexe(WORD);
WORD txtwrite(BPTR,UBYTE*,WORD);
UBYTE *scanchar(UBYTE*,UBYTE);
void readdir(struct FMList*);

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;

void __saveds execute(void)
{
struct ProcMsg *pm;
struct FMList *list;
struct FMNode *node;
struct FMHandle *h;
struct FMHandle hh;
UBYTE byt=0;

pm=sinitproc();
list=fmmain.sourcedir;
if (!(setalloc(list,1))) {
	initproc(0,0); 
	goto endi;
}
initproc(list,pm->cmc->label);
priority(pm->cmc);
if((node=findselnode(list))) {
	if ((h=openfile(list,NDFILE(node),OFNORMAL))) {
		executefile(h,pm->cmc);
		closefile(h);
	}
	node->flags&=~NSELECTED;
	outputlistline(list,node);
} else {
	hh.filename=&byt;
	hh.owner=list;
	executefile(&hh,pm->cmc);
}
endproc(list);
endi:
deinitproc(pm);
}

void executefile(struct FMHandle *h,struct CMenuConfig *cmc)
{
struct GUIBase *gb;
WORD c;
UBYTE params[128];
LONG timeout;
WORD shellw;
struct ExecuteConfig *ec;
struct ExecuteFlags ef={0};

changename(h->owner,cmc->label);
priority(cmc);
ec=getconfig(cmc);
if(h->filename[0]) sformatmsg(h->owner->fmmessage1,MSG_FILECLICK_EXECUTABLE,h->filename);
fmmessage(h->owner);
strcpy(params,h->filename);
gb=getguibase(cmc->label);
setconnectgroup(gb,1,0,0);
reqinfomsg(gb,MSG_EXE_COMMAND,200,guiUC|guiLEFT);
reqstring(gb,200,params,sizeof(params));
timeout=ec->timeout;
setconnectgroup(gb,2,0,0);
reqinfomsg(gb,MSG_EXE_TIMEOUT,201,guiUC|guiLEFT);
reqinteger(gb,201,&timeout,-1,99);
shellw=0;
setconnectgroup(gb,3,0,0);
reqinfomsg(gb,MSG_EXE_SHELLWINDOW,202,guiUC|guiLEFT);
reqcycle2msg(gb,202,&shellw);
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
c=quickreq(gb);
if (c) {
	ef.shellwindow=shellw;
	ef.stack=10000;
	ef.timeout=timeout;
	executeexe(h->owner,fmmain.destdir,params,cmc,&ef);
}
}

void executeexe(struct FMList *list,struct FMList *dlist,UBYTE *command,struct CMenuConfig *cmc,struct ExecuteFlags *ef2)
{
struct Process *proc,*proc2;
APTR window=0;
BPTR lock,tlock;
WORD c,apu1;
UBYTE exename[32];
UBYTE para[PATH];
struct CommandLineInterface *cli;
BPTR *fl;
WORD len;
UBYTE *shellwindow;
UBYTE fmexe[32];
WORD signal=-1;
struct ExecuteFlags ef;

CopyMem(ef2,&ef,sizeof(struct ExecuteFlags));
if(!deloldexe(0)) goto xerror;

proc=(struct Process*)FindTask(0);
window=proc->pr_WindowPtr;
proc->pr_WindowPtr=(APTR)-1;

shellwindow=((struct ExecuteConfig*)(getconfig(getconfignumber(EXECUTECONFIG))))->shellwindow;

if(!command[0]) ef.shellwindow=1;
for(c=0;c<10;c++) {
	sformat(exename,"t:fmexe.tmp%ld",c);
	if(!(lock=Lock(exename,SHARED_LOCK))) break;
	UnLock(lock);
}
if(c>=10) goto xerror;

lock=Open(exename,1006);
if(!lock) goto xerror;

apu1=0;len=101;

cli=(struct CommandLineInterface*)BADDR(fmmain.myproc->pr_CLI); 

fl=(BPTR *)BADDR(cli->cli_CommandDir); 
if(!fl) {
	proc2=(struct Process*)FindTask("Workbench");
	if(proc2) {
		cli=(struct CommandLineInterface*)BADDR(proc2->pr_CLI); 
		fl=(BPTR*)BADDR(cli->cli_CommandDir); 
	}
}
while(fl) {
	if(NameFromLock(fl[1],para,PATH)) {
		if(len>100) {
			if(apu1) txtwrite(lock," add",1);
			txtwrite(lock,"Path >NIL:",0);
			len=0;apu1=1;
		}
		txtwrite(lock," \"",0);
		txtwrite(lock,para,0);
		txtwrite(lock,"\"",0);
		len+=strlen(para)+3;
	}
	fl=(BPTR*)BADDR(fl[0]);
}
if(apu1) txtwrite(lock," add",1);

sformat(para,"NewCLI \"%s\" from %s",shellwindow,exename);
if(command[0]) {
	txtwrite(lock,"Failat 10000000",1);
	txtwrite(lock,command,1);
	if(ef.timeout>0) {
		sformat(fmexe,"Wait %ld SEC",ef.timeout);
		txtwrite(lock,fmexe,1);
	}
	txtwrite(lock,"t:fmexe ",0);
	if(ef.timeout<0) txtwrite(lock,"K ",0);
	if(ef.rescan) {
		signal=AllocSignal(-1);
		sformat(fmexe,"S:%ld:%ld ",FindTask(0),signal);
		txtwrite(lock,fmexe,0);
	}
	if(ef.endfront) {
		sformat(fmexe,"P:%ld ",ef.endfront==1?fmmain.naytto:0);
		txtwrite(lock,fmexe,0);
	}
	txtwrite(lock," ",1);
	if(ef.shellwindow) {
		txtwrite(lock,"EndCLI >NIL:",1);
	} else {
		sformat(para,"Run >NIL: Execute \"%s\"",exename);
	}
} else {
	tlock=Lock("S:Shell-StartUp",SHARED_LOCK);
	if(tlock) {
		txtwrite(lock,"Execute S:Shell-StartUp",1);
		UnLock(tlock);
	}
}
Close(lock);
if(window) proc->pr_WindowPtr=window;
window=0;
lock=Open("NIL:",1005);
if(!lock) goto xerror;
if(ef.startfront==2) WBenchToFront();
if(ef.startfront==1) ScreenToFront(fmmain.naytto);
apu1=Execute(para,0,lock);
Close(lock);

if(signal>=0) {
	Wait(1L<<signal);
	FreeSignal(signal);
	if(ef.rescan&1) readdir(list);
	if(ef.rescan&2) readdir(dlist);
}

if(apu1) return;
xerror:
if(window) proc->pr_WindowPtr=window;
requestmsg(cmc->label,0,MSG_OK,MSG_EXE_FAILED);
}

WORD deloldexe(WORD ide)
{
UBYTE exename[32];
WORD ret=0;
WORD aa;
BPTR lock;
APTR win;
struct Process *proc;

extern UBYTE fmexe[];
extern UBYTE fmexee[];

proc=(struct Process*)FindTask(0);
win=proc->pr_WindowPtr;
proc->pr_WindowPtr=(APTR)-1;
if(!(lock=Lock("t:",SHARED_LOCK))) goto endi;
UnLock(lock);
for(aa=0;aa<10;aa++) {
	sformat(exename,"t:fmexe.tmp%ld",aa);
	DeleteFile(exename);
}
if((lock=Lock("t:fmexe",SHARED_LOCK))) {
	UnLock(lock);
	return(1);
}
if(ide) {
	DeleteFile("t:fmexe");
} else {
	lock=Open("t:fmexe",1006);
	if(!lock) goto endi;
	Write(lock,fmexe,fmexee-fmexe);
	Close(lock);
}
ret=1;
endi:
proc->pr_WindowPtr=win;
return(ret);
}

WORD txtwrite(BPTR handle,UBYTE *ptr,WORD eof)
{
Write(handle,ptr,strlen(ptr));
if(eof) Write(handle,"\n",1);
return(1);
}

void __saveds executeownnum(void)
{
struct ProcMsg *pm;
struct FMList *list;

pm=sinitproc();
list=fmmain.sourcedir;
if (!(setalloc(list,1))) {
	initproc(0,0);
	goto endi;
}
initproc(list,pm->cmc->label);
priority(pm->cmc);
executeown(list,fmmain.destdir,0,pm->cmc);
endproc(list);
endi:
deinitproc(pm);
}

void executeown(struct FMList *slist,struct FMList *dlist,struct FMNode *cnode,struct CMenuConfig *cmc)
{
struct OwnCommand *oc;
UBYTE command[1000];
UBYTE varapath[PATH],vara[PATH];
UBYTE *ptr1,*ptr2,*ptr3;
WORD apu1,apu2,apu3;
struct FMNode *node;
WORD dalloc=0;

oc=getconfig(cmc);
changename(slist,cmc->label);
priority(cmc);

if(oc->ef.rescan&2) {
	if(!(setalloc(dlist,1))) goto endi;
	changename(dlist,cmc->label);
	dalloc=1;
}

ptr1=oc->command;
if(*ptr1==0) goto endi;
ptr2=command;

*ptr2=0;
if(oc->ef.cd==2) {
	if(!dlist->path[0]) {
		strcpymsg(slist->fmmessage1,MSG_MAIN_NODESTINATION);
		fmmessage(slist);
		goto endi;
	}
	sformat(ptr2,"CD %lc%s%lc\n",'"',dlist->path,'"');
}
if(oc->ef.cd==1) {
	if(!slist->path[0]) {
		strcpymsg(slist->fmmessage1,MSG_MAIN_NOSOURCE);
		fmmessage(slist);
	}
	sformat(ptr2,"CD %lc%s%lc\n",'"',slist->path,'"');
}
ptr2+=strlen(ptr2);
sformat(ptr2,"Stack %ld\n",oc->ef.stack);
ptr2+=strlen(ptr2);

while(*ptr1) {
	if(*ptr1=='{') {
		ptr1++;
		*ptr2=0;
		switch(*ptr1)
		{
		case 'd':
		*ptr2++=34;
		strcpy(ptr2,dlist->path);
		ptr2+=strlen(ptr2);
		*ptr2++=34;
		break;
		case 's':
		*ptr2++=34;
		strcpy(ptr2,slist->path);
		ptr2+=strlen(ptr2);
		*ptr2++=34;
		break;
		case 'f':
		case 'F':
		case 'o':
		case 'O':
		apu1=0;apu2=0;
		if(*ptr1=='f'||*ptr1=='F') apu1=1;
		if(*ptr1=='F'||*ptr1=='O') apu2=1;
		if(cnode&&!apu2) {
			node=cnode;
			node->flags&=~NSELECTED;
			*ptr2++=34;
			if(apu1) {
				strcpy(varapath,slist->path);
				AddPart(varapath,NDFILE(node),PATH);
				strcpy(ptr2,varapath);
			} else {
				strcpy(ptr2,NDFILE(node));
			}
			ptr2+=strlen(ptr2);
			*ptr2++=34;
		} else {	
			apu3=0;
			for(node=slist->head;node->succ;node=node->succ) {
				if(node->flags&NSELECTED) {
					node->flags&=~NSELECTED;
					if(apu3) *ptr2++=32;
					*ptr2++=34;
					if(apu1) {
						strcpy(varapath,slist->path);
						AddPart(varapath,NDFILE(node),PATH);
						strcpy(ptr2,varapath);
					} else {
						strcpy(ptr2,NDFILE(node));
					}
					ptr2+=strlen(ptr2);
					*ptr2++=34;
					apu3=1;
					if(!apu2) break;
				}
			}
		}
		break;
		case 'r':
		ptr1=scanchar(ptr1,',');
		if(!ptr1) goto serror;
		ptr1++;
		ptr3=scanchar(ptr1,'}');
		if(!ptr3) goto serror;
		memseti(vara,0,PATH);
		strncpy(vara,ptr1,ptr3-ptr1);
		if(!askstring(0,cmc->label,vara,varapath,80)) goto endi;
		strcpy(ptr2,varapath);
		ptr2+=strlen(ptr2);
		break;
		}
		ptr1=scanchar(ptr1,'}');
		if(!ptr1) goto serror;
		ptr1++;
	} else {
		*ptr2++=*ptr1++;
	}
}
*ptr2=0;

sformatmsg(slist->fmmessage1,MSG_EXE_EXECUTING,cmc->label);
fmmessage(slist);
executeexe(slist,dlist,command,cmc,&oc->ef);
goto endi;
serror:
requestmsg(cmc->label,0,MSG_OK,MSG_EXE_SYNTAXERROR,cmc->label);
endi:
if(dalloc) setalloc(dlist,0);
outputlist(slist);
}

void *ownexecuteconfigdefault(struct CMenuConfig *cmc)
{
WORD ret;
struct OwnCommand *oc;

ret=allocconfig(cmc,sizeof(struct OwnCommand));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
oc=(struct OwnCommand*)cmc->moreconfig;
cmc->cmenucount=100;
oc->ef.stack=8000;
oc->ef.timeout=2;
return(cmc->moreconfig);
}

void *executeconfigdefault(struct CMenuConfig *cmc)
{
struct ExecuteConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct ExecuteConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=cmc->moreconfig;
strcpy(config->shellwindow,"CON:////FileMaster Shell/SCREEN FM");
config->timeout=2;
return(cmc->moreconfig);
}

void initexes(WORD cnt1)
{
struct OwnCommand *oc;
struct CMenuConfig *cmc;

executeconfigdefault(&fmconfig->cmenuconfig[EXECUTECONFIG]);
cmc=&fmconfig->cmenuconfig[cnt1++];
cmc->position=1;
oc=ownexecuteconfigdefault(cmc);
if(oc) {
	strcpy(cmc->label,"LhA x");
	strcpy(oc->namematch,"*.(lha|lzh)");
	strcpy(oc->command,"lha x {f}");
	strcpy(oc->matchbytes,"2,$2d6c68");
	oc->ef.shellwindow=1;
	oc->ef.cd=2;
	oc->ef.rescan=2;
	oc->ef.stack=8000;
}
cmc=&fmconfig->cmenuconfig[cnt1++];
cmc->position=1;
oc=ownexecuteconfigdefault(cmc);
if(oc) {
	strcpy(cmc->label,"LhA a");
	strcpy(oc->command,"lha a {r,Archive name} {F}");
	oc->ef.shellwindow=1;
	oc->ef.cd=2;
	oc->ef.rescan=2;
	oc->ef.stack=8000;
}

cmc=&fmconfig->cmenuconfig[cnt1++];
cmc->position=1;
oc=ownexecuteconfigdefault(cmc);
if(oc) {
	strcpy(cmc->label,"AGuide");
	if(fmmain.kick>=39)
		strcpy(oc->command,"sys:utilities/multiview {f}");
		else
		strcpy(oc->command,"sys:utilities/amigaguide {f}");
	strcpy(oc->namematch,"#?.guide");
	oc->ef.startfront=1;
	oc->ef.endfront=1;
	oc->ef.timeout=0;
	oc->ef.stack=8000;
}
}

WORD executeconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
struct ExecuteConfig *ec;
LONG timeout;

ec=executeconfigdefault(cmc);
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_EXE_SHELLWINDOW,200,guiUC|guiLEFT);
reqinfomsg(gb,MSG_EXE_TIMEOUT,201,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqstring(gb,200,ec->shellwindow,sizeof(ec->shellwindow));
timeout=ec->timeout;
reqinteger(gb,201,&timeout,-1,99);
commandanswer(gb);
c=quickreq(gb);
ec->timeout=timeout;
return(c);
}
