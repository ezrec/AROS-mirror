/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <fctype.h>
#include <string.h>
#include <stdlib.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>
#include <devices/input.h>
#include <proto/all.h>

#include "dopusbase.h"

extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
struct DOpusBase *DOpusBase;

void main(int,char **);
void WBRun(int,char **);
int setarg(struct WBArg *,char *,BPTR);
BPTR FindPath(char *);

void InputHandler(void);

char *version="$VER: DOpusRT 4.4 (30 Apr 93) Copyright © 1993 Jonathan Potter";

char
	*pathlists[7]={
		"Workbench",
		"Initial CLI",
		"Shell Process",
		"New CLI",
		"AmigaShell",
		"New_WShell",
		"Background CLI"};

void main(argc,argv)
int argc;
char *argv[];
{
	int a,out,x,y,flag;
	struct MsgPort *port,*port1;
	struct Process *myproc;
	struct Message msg;
	struct InfoData __aligned ind;
	struct Window *win;
	struct CommandLineInterface *cli;
	void *cont;
	BPTR path;
	ULONG arg;
	struct IntuitionBase *IntuitionBase;
	struct GfxBase *GfxBase;
	struct Interrupt *interrupt;
	struct IOStdReq *inputreq;

	if (argc<2 ||
		!(DOpusBase=(struct DOpusBase *)OpenLibrary("dopus.library",0))) _exit(0);
	IntuitionBase=DOpusBase->IntuitionBase;
	GfxBase=DOpusBase->GfxBase;

	myproc=(struct Process *)FindTask(NULL);
	cli=(struct CommandLineInterface *)(myproc->pr_CLI<<2);

	win=NULL;
	if ((cont=myproc->pr_ConsoleTask)) {
		arg=(ULONG)(&ind)>>2;
		if (SendPacket(cont,ACTION_DISK_INFO,&arg,1))
			win=(struct Window *)ind.id_VolumeNode;
	}
	out=Output();

	if (LStrnCmp(argv[1],"-s",2)==0) {
		if (win) {
			if ((win->WScreen->Flags&SCREENTYPE)!=WBENCHSCREEN) {
				myproc->pr_WindowPtr=(APTR)win;
				SetFont(win->RPort,GfxBase->DefaultFont);
				Write(out,"\x1b\x63",2);
			}
		}
		if (cli && !cli->cli_CommandDir) {
			for (a=0;a<7;a++) if ((path=FindPath(pathlists[a]))) break;
			cli->cli_CommandDir=path;
		}
	}
	else if (LStrnCmp(argv[1],"-w",2)==0) {
		if ((a=atoi(argv[2]))>60) a=60;
		if (a>0) Delay(a*50);
		else if (a==-1) {
			if (out) {
				if (port=LCreatePort(NULL,0)) {
					if (inputreq=(struct IOStdReq *)LCreateExtIO(port,sizeof(struct IOStdReq))) {
						if (!(OpenDevice("input.device",NULL,(struct IORequest *)inputreq,NULL))) {
							if (interrupt=AllocMem(sizeof(struct Interrupt),MEMF_CLEAR|MEMF_PUBLIC)) {
								interrupt->is_Code=InputHandler;
								interrupt->is_Node.ln_Pri=51;
								interrupt->is_Node.ln_Name="DOpusRT";
								interrupt->is_Data=myproc;
								if (argc>3) {
									Write(out,"\n",1);
									Write(out,argv[3],strlen(argv[3]));
								}
								else Write(out,"\nPress left mouse button to continue...",39);
								inputreq->io_Data=(APTR)interrupt;
								inputreq->io_Command=IND_ADDHANDLER;
								DoIO((struct IORequest *)inputreq);
								flag=0;
								FOREVER {
									a=Wait(3);
									if (!win) break;
									if (IntuitionBase->ActiveWindow==win) {
										if (a&1) {
											x=win->MouseX; y=win->MouseY;
											if (x<=win->BorderLeft || x>=win->Width-win->BorderRight ||
												y<=win->BorderTop && y>=win->Height-win->BorderBottom) flag=0;
											else flag=1;
										}
										else if (a&2 && flag) {
											flag=0;
											if (x>win->BorderLeft && x<win->Width-win->BorderRight &&
												y>win->BorderTop && y<win->Height-win->BorderBottom) break;
										}
									}
								}
								inputreq->io_Data=(APTR)interrupt;
								inputreq->io_Command=IND_REMHANDLER;
								DoIO((struct IORequest *)inputreq);
								FreeMem(interrupt,sizeof(struct Interrupt));
							}
							CloseDevice((struct IORequest *)inputreq);
						}
						LDeleteExtIO((struct IORequest *)inputreq);
					}
					LDeletePort(port);
				}
			}
		}
	}
	else if (LStrnCmp(argv[1],"-p",2)==0) {
		Forbid();
		if ((port=FindPort(argv[2]))) {
			Permit();
			port1=LCreatePort(0,0);
			msg.mn_Node.ln_Type=NT_MESSAGE;
			msg.mn_ReplyPort=port1;
			msg.mn_Length=sizeof(struct Message);
			PutMsg(port,&msg);
			WaitPort(port1);
			LDeletePort(port1);
		}
		else Permit();
	}
	else if (LStrnCmp(argv[1],"-r",2)==0) WBRun(argc-2,&argv[2]);
	CloseLibrary((struct Library *)DOpusBase);
	_exit(0);
}

BPTR FindPath(taskname)
char *taskname;
{
	struct Process *teacher;
	struct CommandLineInterface *teachcli;
	BPTR *wext,*mext,*lastmext,newpath=0;

	if (!(teacher=(struct Process *)FindTask(taskname))) return(0);
	lastmext=&newpath;
	if (!(teachcli=(struct CommandLineInterface *)(teacher->pr_CLI<<2)))
		return(0);
	Forbid();
	for (wext=(BPTR *)(teachcli->cli_CommandDir<<2);wext;wext=(BPTR *)(*wext<<2)) {
		if (!(mext=AllocMem(3*sizeof(BPTR),MEMF_PUBLIC))) break;
		*lastmext=(long)(mext+1)>>2;
		lastmext=mext+1;
		mext[2]=DupLock(wext[1]);
		mext[1]=0;
		*mext=3*sizeof(BPTR);
	}
	Permit();
	return(newpath);
}

void WBRun(argc,argv)
int argc;
char **argv;
{
	struct WBStartup *WBStartup;
	struct DiskObject *diskobj;
	char namebuf[256];
	int stacksize,i;
	struct Process *ourtask;
	struct MsgPort *replyport;
	BPTR olddir;
	struct IconBase *IconBase;
	struct DOpusRemember *key;
	struct CommandLineInterface *cli;

	diskobj=NULL; replyport=NULL; olddir=-1; key=NULL;

	if (argc<1 ||
		!(IconBase=(struct IconBase *)OpenLibrary("icon.library",0))) return;

	ourtask=(struct Process *)FindTask(NULL);
	if (!(WBStartup=LAllocRemember(&key,sizeof(struct WBStartup),MEMF_CLEAR)) ||
		!(WBStartup->sm_ArgList=LAllocRemember(&key,sizeof(struct WBArg)*(argc+1),MEMF_CLEAR)) ||
		!(replyport=LCreatePort(NULL,0))) goto done;

	WBStartup->sm_Message.mn_ReplyPort=replyport;
	WBStartup->sm_NumArgs=argc;

	for (i=0;i<argc;i++)
		if (setarg(&WBStartup->sm_ArgList[i],argv[i],ourtask->pr_CurrentDir))
			goto done;

	olddir=CurrentDir(WBStartup->sm_ArgList[0].wa_Lock);
	if (!(diskobj=GetDiskObject(WBStartup->sm_ArgList[0].wa_Name))) {
		if (!(SearchPathList(argv[0],namebuf,256))) goto done;
	}
	else if (diskobj->do_Type==WBTOOL) strcpy(namebuf,argv[0]);
	else if (diskobj->do_Type==WBPROJECT) {
		if (!(SearchPathList(diskobj->do_DefaultTool,namebuf,256))) goto done;
		for (i=argc-1;i>=0;i--)
			CopyMem(&WBStartup->sm_ArgList[i],&WBStartup->sm_ArgList[i+1],
				sizeof(struct WBArg));
		if (setarg(&WBStartup->sm_ArgList[0],namebuf,WBStartup->sm_ArgList[0].wa_Lock))
			goto done;
		++WBStartup->sm_NumArgs;
	}
	else goto done;

	if (!diskobj) {
		if ((cli=(struct CommandLineInterface *)(ourtask->pr_CLI<<2)))
			stacksize=cli->cli_DefaultStack*4;
		else stacksize=4096;
	}
	else stacksize=diskobj->do_StackSize;

	stacksize=(stacksize+3)&(~3);
	if (stacksize<4096) stacksize=4096;

	if (!(WBStartup->sm_Segment=(BPTR)LoadSeg(namebuf))) goto done;

	Forbid();
	if (!(WBStartup->sm_Process=CreateProc(BaseName(namebuf),
		ourtask->pr_Task.tc_Node.ln_Pri,WBStartup->sm_Segment,stacksize))) {
		Permit();
		goto done;
	}

	if (DOSBase->dl_lib.lib_Version>=36) {
		struct Process *proc;
		char *ptr;

		if (ptr=BaseName(namebuf)) *ptr=0;
		proc=(struct Process *)WBStartup->sm_Process->mp_SigTask;
		proc->pr_HomeDir=Lock(ptr,ACCESS_READ);
	}

	WBStartup->sm_ToolWindow=(diskobj)?diskobj->do_ToolWindow:NULL;
	Permit();

	PutMsg(WBStartup->sm_Process,(struct Message *)WBStartup);
	WaitPort(replyport);
done:
	if (replyport) LDeletePort(replyport);
	if (diskobj) FreeDiskObject(diskobj);
	if (olddir!=-1) CurrentDir(olddir);
	if (WBStartup) {
		if (WBStartup->sm_Segment) UnLoadSeg(WBStartup->sm_Segment);
		if (WBStartup->sm_ArgList) {
			for (i=0;i<WBStartup->sm_NumArgs;i++) {
				if (WBStartup->sm_ArgList[i].wa_Lock)
					UnLock(WBStartup->sm_ArgList[i].wa_Lock);
			}
		}
	}
	LFreeRemember(&key);
	if (IconBase) CloseLibrary((struct Library *)IconBase);
}

int setarg(WBArg,name,curdir)
struct WBArg *WBArg;
char *name;
BPTR curdir;
{
	char *p,*lastc;
	unsigned char c;

	if (!name || !name[0]) return(1);

	lastc=NULL;
	for (p=name;*p;p++)
		if (*p==':' || *p=='/') lastc=p+1;

	if (!lastc) {
		WBArg->wa_Lock=DupLock(curdir);
		WBArg->wa_Name=name;
	}
	else {
		if (!lastc[0]) return(1);
		WBArg->wa_Name=lastc;
		c=lastc[0];
		lastc[0]=0;
		if (!(WBArg->wa_Lock=Lock(name,ACCESS_READ))) return(1);
		*lastc=c;
	}
	return(0);
}
