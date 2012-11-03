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

//#include <fctype.h>
#include <string.h>
//#include <stdlib.h>

//#include <exec/types.h>
//#include <exec/memory.h>
//#include <exec/execbase.h>
//#include <dos/dos.h>
//#include <dos/dosextens.h>
//#include <dos/exall.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfxbase.h>
#include <devices/input.h>
#include <workbench/startup.h>
#include <dopus/dopusbase.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/dopus.h>

#ifndef __AROS__
#include <debug.h>
#endif

#if defined(__PPC__) || defined(__AROS__)
  #undef  __saveds
  #define __saveds
  #define __chip
  #define __aligned __attribute__((__aligned__(4)))
  #define lsprintf sprintf
  #define __asm(A)
#endif

//extern struct ExecBase *SysBase;
//extern struct DosLibrary *DOSBase;
struct DOpusBase *DOpusBase = NULL;

void WBRun(int,char **);
int setarg(struct WBArg *,char *,BPTR);
BPTR CloneCommandDir(const char *);

static const char *version="$VER: DOpusRT 4.8 (" __DATE__ ") by Jacek Rzeuski";

static const char *pathlists[7]={
        "Workbench",
        "Initial CLI",
        "Shell Process",
        "New CLI",
        "AmigaShell",
        "New_WShell",
        "Background CLI"};

struct InputEvent *InputHandler(register struct InputEvent *ie __asm("a0"),register struct Task *task __asm("a1"))
{
 if (ie->ie_Class == IECLASS_RAWMOUSE)
  {
   if (ie->ie_Code & IECODE_LBUTTON)
    {
#if !defined(__AROS__)
     register struct ExecBase *SysBase __asm("a6") = *(struct ExecBase **)4L;
#endif

     Signal(task,(ie->ie_Code & IECODE_UP_PREFIX)?2:1);
    }
  }
 return ie;
}

static inline LONG atoi(char *str)
 {
  LONG i;

  StrToLong((STRPTR)str,&i);

  return i;
 }

int main(argc,argv)
int argc;
char *argv[];
{
    int a,out,x,y,flag;
    struct MsgPort *port,*port1,*cont;
    struct Process *myproc;
    struct Message msg;
    struct Window *win=NULL;
    struct CommandLineInterface *cli;
    struct IntuitionBase *IntuitionBase;
    struct GfxBase *GfxBase;
    struct Interrupt *interrupt;
    struct IOStdReq *inputreq;

    if (argc<2 || !(DOpusBase=(struct DOpusBase *)OpenLibrary("dopus.library",0))) return 1000000/*_exit(0)*/;
    IntuitionBase=DOpusBase->IntuitionBase;
    GfxBase=DOpusBase->GfxBase;

    myproc=(struct Process *)FindTask(NULL);
    cli=BADDR(myproc->pr_CLI);
    if (cont=GetConsoleTask()) {
        struct InfoData __aligned ind;
        ULONG arg=MKBADDR(&ind);

        if (SendPacket(cont,ACTION_DISK_INFO,&arg,1))
            win=(struct Window *)ind.id_VolumeNode;
    }
    out=Output();

    if (argv[1][0]=='-') switch(LToLower(argv[1][1]))
     {
      case 's':
        if (win) {
            if ((win->WScreen->Flags&SCREENTYPE)!=WBENCHSCREEN) {
                myproc->pr_WindowPtr=(APTR)win;
                SetFont(win->RPort,GfxBase->DefaultFont);
                Write(out,"\x1b\x63",2);
            }
        }
        if (cli && !cli->cli_CommandDir) {
            BPTR path = NULL;

            for (a=0;a<7;a++) if ((path=CloneCommandDir(pathlists[a]))) break;
            cli->cli_CommandDir=path;
        }
        break;
      case 'w':
        if ((a=atoi(argv[2]))>60) a=60;
        if (a>0) Delay(a*50);
        else if (a==-1) {
            if (out) {
                if (port=LCreatePort(NULL,0)) {
                    if (inputreq=(struct IOStdReq *)LCreateExtIO(port,sizeof(struct IOStdReq))) {
                        if (!(OpenDevice("input.device",NULL,(struct IORequest *)inputreq,NULL))) {
                            if (interrupt=AllocMem(sizeof(struct Interrupt),MEMF_CLEAR|MEMF_PUBLIC)) {
                                interrupt->is_Code=(APTR)InputHandler;
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
                                        x=win->MouseX; y=win->MouseY;
                                        if (a&1) {
                                            if ((x<=win->BorderLeft) || (x>=win->Width-win->BorderRight) ||
                                               (y<=win->BorderTop) || (y>=win->Height-win->BorderBottom)) flag=0;
                                            else flag=1;
                                        }
                                        else if (a&2 && flag) {
                                            flag=0;
                                            if (x>win->BorderLeft && x<win->Width-win->BorderRight &&
                                                y>win->BorderTop && y<win->Height-win->BorderBottom) break;
                                        }
                                    }
                                }
//                                inputreq->io_Data=(APTR)interrupt;
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
        break;
      case 'p':
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
        break;
      case 'r':
        WBRun(argc-2,&argv[2]);
        break;
     }
    CloseLibrary((struct Library *)DOpusBase);
    return 0;
    //_exit(0);
}

struct PathList
 {
  BPTR nextPath;
  BPTR pathLock;
 };

BPTR CloneCommandDir(const char *taskname)
{
    struct Process *teacher;
    struct CommandLineInterface *teachcli;
    struct PathList *wext,*mext,*lastmext=NULL;
    BPTR newpath=NULL;

    Forbid();
    if (teacher=(struct Process *)FindTask(taskname))
     {
      if (teachcli=(struct CommandLineInterface *)BADDR(teacher->pr_CLI))
       {
        for (wext=BADDR(teachcli->cli_CommandDir); wext; wext=BADDR(wext->nextPath))
         {
          if (!(mext = AllocVec(sizeof(struct PathList),MEMF_PUBLIC))) break;

          mext->nextPath=NULL;
          mext->pathLock=DupLock(wext->pathLock);
          if (!newpath) newpath=MKBADDR(mext);

          if (lastmext) lastmext->nextPath=MKBADDR(mext);
          lastmext=mext;
         }
       }
     }
    Permit();
    return newpath;
}

void WBRun(argc,argv)
int argc;
char **argv;
{
    struct WBStartup *WBStartup;
    struct DiskObject *diskobj=NULL;
    char namebuf[256];
    int stacksize,i,ok=1;
    struct Process *ourtask;
    struct MsgPort *replyport=NULL;
    BPTR olddir=-1;
    struct IconBase *IconBase;
    struct DOpusRemember *key=NULL;
    struct CommandLineInterface *cli;

    if (argc<1 || !(IconBase=(struct IconBase *)OpenLibrary("icon.library",0))) return;

    ourtask=(struct Process *)FindTask(NULL);

    if ((WBStartup=LAllocRemember(&key,sizeof(struct WBStartup),MEMF_CLEAR)) &&
        (WBStartup->sm_ArgList=LAllocRemember(&key,sizeof(struct WBArg)*(argc+1),MEMF_CLEAR)) &&
        (replyport=LCreatePort(NULL,0)))
     {
      WBStartup->sm_Message.mn_ReplyPort=replyport;
      WBStartup->sm_NumArgs=argc;

      for (i=0;i<argc;i++)
        if (setarg(&WBStartup->sm_ArgList[i],argv[i],ourtask->pr_CurrentDir))
          break;

      if (i==argc)
       {
        olddir=CurrentDir(WBStartup->sm_ArgList[0].wa_Lock);

        if (!(diskobj=GetDiskObject(WBStartup->sm_ArgList[0].wa_Name)))
         {
          if (!(SearchPathList(argv[0],namebuf,256)))
            ok=0;
         }
        else if (diskobj->do_Type==WBTOOL) LStrCpy(namebuf,argv[0]);
        else if (diskobj->do_Type==WBPROJECT)
         {
          if (SearchPathList(diskobj->do_DefaultTool,namebuf,256))
           {
            for (i = argc - 1; i >= 0; i--)
              CopyMem(&WBStartup->sm_ArgList[i],&WBStartup->sm_ArgList[i+1],sizeof(struct WBArg));
            if (setarg(&WBStartup->sm_ArgList[0],namebuf,WBStartup->sm_ArgList[0].wa_Lock))
                ok=0;
            else ++WBStartup->sm_NumArgs;
           }
          else ok=0;
        }
        else ok=0;

        if (ok)
         {
          if (diskobj) stacksize=diskobj->do_StackSize;
          else
           {
            if (cli = BADDR(ourtask->pr_CLI))
              stacksize = cli->cli_DefaultStack*4;
            else stacksize = 4096;
           }

          stacksize = (stacksize+3)&(~3);
          if (stacksize < 4096) stacksize = 4096;

          WBStartup->sm_ToolWindow=(diskobj)?diskobj->do_ToolWindow:NULL;

          if (WBStartup->sm_Segment = LoadSeg(namebuf))
           {
            char *ptr = BaseName(namebuf);

            Forbid();
            if (WBStartup->sm_Process=CreateProc(ptr,
                ourtask->pr_Task.tc_Node.ln_Pri,WBStartup->sm_Segment,stacksize))
             {
              if (ptr) *ptr=0;
              ((struct Process *)(WBStartup->sm_Process->mp_SigTask))->pr_HomeDir=Lock(ptr,ACCESS_READ);

              PutMsg(WBStartup->sm_Process,(struct Message *)WBStartup);

              Permit();
              WaitPort(replyport);
             }
            else Permit();
           }
         }
       }
     }
    if (replyport) LDeletePort(replyport);
    if (diskobj) FreeDiskObject(diskobj);
    if (olddir!=-1) CurrentDir(olddir);
    if (WBStartup) {
        /*if (WBStartup->sm_Segment)*/ UnLoadSeg(WBStartup->sm_Segment);
        if (WBStartup->sm_ArgList) {
            for (i=0;i<WBStartup->sm_NumArgs;i++) {
//                if (WBStartup->sm_ArgList[i].wa_Lock)
                    UnLock(WBStartup->sm_ArgList[i].wa_Lock);
            }
        }
    }
    LFreeRemember(&key);
    /*if (IconBase)*/ CloseLibrary((struct Library *)IconBase);
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
