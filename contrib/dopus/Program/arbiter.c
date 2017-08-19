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

#include "dopus.h"
#include "view.h"
#include <dos/dostags.h>

struct MsgPort *arbiter_reply_port;
struct MsgPort *arbiter_msg_port;
struct ProcessStart *arbiter_seglist=NULL;
struct Message arbiter_startup;

#ifdef __MORPHOS__
struct EmulLibEntry GATE_arbiter_process = { TRAP_LIB, 0, (void (*)(void))&arbiter_process };

struct TagItem arbiter_tags[] = {
  { NP_Entry, (Tag)&GATE_arbiter_process },
  { NP_Name,  (Tag)"dopus_arbiter" },
  { NP_Priority, 0 },
  { NP_StackSize,4000 },
  { TAG_END } };
#endif

int install_arbiter()
{
    struct Process *proc;

#ifndef __AROS__
    if (!(arbiter_reply_port=LCreatePort(NULL,0)) ||
        !(arbiter_seglist=AllocMem(sizeof(struct ProcessStart),MEMF_CLEAR|MEMF_PUBLIC)))
#else
    if (!(arbiter_reply_port=LCreatePort(NULL,0)))
#endif
        return(0);

#ifndef __AROS__
    arbiter_seglist->ps_NextSeg=NULL;
    arbiter_seglist->ps_JMP=0x4ef9;
    arbiter_seglist->ps_EntryPoint=(void *)arbiter_process;
#endif

    arbiter_startup.mn_Node.ln_Type=NT_MESSAGE;
    arbiter_startup.mn_Node.ln_Pri=0;
    arbiter_startup.mn_ReplyPort=arbiter_reply_port;
    arbiter_startup.mn_Length=(UWORD)sizeof(struct ArbiterMessage);

    proc=
#ifdef __MORPHOS__
        CreateNewProc(arbiter_tags)->pr_MsgPort);
#else
#ifdef __AROS__
        CreateNewProcTags(NP_Name,"dopus_arbiter",NP_Priority,0,
            NP_Entry,arbiter_process,NP_StackSize,AROS_STACKSIZE,
            TAG_END);
#else
        CreateNewProcTags(NP_Name,(Tag)"dopus_arbiter",NP_Priority,0,
            NP_Seglist,MKBADDR(arbiter_seglist),NP_StackSize,4000,
            NP_FreeSeglist,FALSE,TAG_END);
#endif
#endif
    if (!proc) return(0);
    arbiter_msg_port=&proc->pr_MsgPort;
    PutMsg(arbiter_msg_port,&arbiter_startup);
    return(1);
}

void remove_arbiter()
{
    if (arbiter_msg_port) {
        arbiter_command(ARBITER_REMOVE,NULL,0);
        WaitPort(arbiter_reply_port);
        GetMsg(arbiter_reply_port);
    }

    if (arbiter_seglist) FreeMem(arbiter_seglist,sizeof(struct ProcessStart));
    if (arbiter_reply_port) LDeletePort(arbiter_reply_port);
}

int arbiter_command(command,data,flags)
int command;
APTR data;
int flags;
{
    struct ArbiterMessage arbiter_msg;
    struct MsgPort command_port;

    CopyMem((char *)arbiter_reply_port,(char *)&command_port,sizeof(struct MsgPort));
    NewList(&command_port.mp_MsgList);

    arbiter_msg.msg.mn_Node.ln_Type=NT_MESSAGE;
    arbiter_msg.msg.mn_Node.ln_Pri=0;
    arbiter_msg.msg.mn_ReplyPort=&command_port;
    arbiter_msg.msg.mn_Length=(UWORD)sizeof(struct ArbiterMessage);
    arbiter_msg.command=command;
    arbiter_msg.data=data;
    arbiter_msg.flags=flags;
    PutMsg(arbiter_msg_port,(struct Message *)&arbiter_msg);
    WaitPort(&command_port);
    GetMsg(&command_port);

D(bug("arbiter_command() returned %ld\n",arbiter_msg.command));
    return(arbiter_msg.command);
}

struct LaunchList {
    struct LaunchList *next;
    struct ProcessStart *seglist;
    struct ArbiterMessage launch_msg;
    struct ArbiterMessage *reply_msg;
    struct DOpusRemember *memory;
};

void __saveds arbiter_process()
{
    struct Process *my_process;
    struct Message *my_startup_message;
    struct ArbiterMessage *arb_msg,*remove_msg=NULL;
    struct LaunchList *first_launch=NULL,*launch,*launchpos;
    struct MsgPort *replyport;
    char ret,remove=0;
    int wait_mask;

    my_process=(struct Process *)FindTask(NULL);
    my_process->pr_WindowPtr=(APTR)-1;
    WaitPort(&my_process->pr_MsgPort);
    my_startup_message=GetMsg(&my_process->pr_MsgPort);

    wait_mask=1<<my_process->pr_MsgPort.mp_SigBit;

    if ((replyport=LCreatePort(NULL,0)))
        wait_mask|=1<<replyport->mp_SigBit;

    FOREVER {
        if (replyport) {
            while ((arb_msg=(struct ArbiterMessage *)GetMsg(replyport))) {
                launch=first_launch;
                launchpos=NULL;
                while (launch) {
                    if (arb_msg==&launch->launch_msg) {
                        if (launchpos) launchpos->next=launch->next;
                        else first_launch=launch->next;
                        if (launch->reply_msg) {
                            launch->reply_msg->command=arb_msg->command;
                            ReplyMsg((struct Message *)launch->reply_msg);
                        }
                        LFreeRemember(&launch->memory);
                        FreeMem(launch,sizeof(struct LaunchList));
                        break;
                    }
                    launchpos=launch;
                    launch=launch->next;
                }
            }
            if (remove_msg && !first_launch) {
                ReplyMsg((struct Message *)remove_msg);
                break;
            }
        }
        while ((arb_msg=(struct ArbiterMessage *)GetMsg(&my_process->pr_MsgPort))) {
            ret=0;
            switch (arb_msg->command) {
                case ARBITER_REMOVE:
                    if (first_launch) {
                        remove_msg=arb_msg;
//                        arb_msg=NULL;
                    }
                    else remove=1;
                    break;
                case ARBITER_LAUNCH:
                    if (replyport && (launch=AllocMem(sizeof(struct LaunchList),MEMF_CLEAR))) {
                        struct ArbiterLaunch *arb_launch;
                        struct MsgPort *port;

                        if ((launchpos=first_launch)) for (;launchpos->next; launchpos=launchpos->next);

                        if (launchpos) launchpos->next=launch;
                        else first_launch=launch;

                        arb_launch=(struct ArbiterLaunch *)arb_msg->data;

                        launch->memory=arb_launch->launch_memory;

                        if ((launch->seglist=LAllocRemember(&launch->memory,
                            sizeof(struct ProcessStart),MEMF_PUBLIC|MEMF_CLEAR))) {
#ifdef __MORPHOS__
                            struct EmulLibEntry GATE_arbiterlaunch_process = { TRAP_LIB, 0, (void (*)(void))&arbiter_process };
#endif

#ifndef __AROS__
                            launch->seglist->ps_JMP=0x4ef9;
                            launch->seglist->ps_EntryPoint=arb_launch->launch_code;
#endif

                            launch->launch_msg.msg.mn_Node.ln_Type=NT_MESSAGE;
                            launch->launch_msg.msg.mn_ReplyPort=replyport;
                            launch->launch_msg.msg.mn_Length=sizeof(struct ArbiterMessage);
                        
                            launch->launch_msg.command=arb_msg->command;
                            launch->launch_msg.data=arb_launch->data;
                            launch->launch_msg.flags=arb_msg->flags;

D(bug("ARBITER_LAUNCH: %s, flags: %lx\n",arb_launch->launch_name,arb_msg->flags));
                            if ((port=(struct MsgPort *)
//                                CreateProc(arb_launch->launch_name,0,(long)launch->seglist>>2,4000))) {
#ifdef __MORPHOS__
                                &CreateNewProcTags(NP_Name,        (Tag)arb_launch->launch_name,
                                                   NP_Priority,    0,
                                                   NP_Entry, (Tag)&GATE_arbiterlaunch_process,
                                                   NP_StackSize,   8192,
                                                   NP_FreeSeglist, FALSE,
                                                   NP_CloseInput,  FALSE,
                                                   NP_CloseOutput, FALSE,
                                                   TAG_END)->pr_MsgPort)) {
#else
#ifdef __AROS__
                                &CreateNewProcTags(NP_Name,        (IPTR)arb_launch->launch_name,
                                                   NP_Priority,    0,
                                                   NP_Entry, arb_launch->launch_code,
                                                   NP_StackSize,   8192,
                                                   NP_CloseInput,  FALSE,
                                                   NP_CloseOutput, FALSE,
                                                   TAG_END)->pr_MsgPort)) {
#else
                                &CreateNewProcTags(NP_Name,        (Tag)arb_launch->launch_name,
                                                   NP_Priority,    0,
                                                   NP_Seglist,     (long)launch->seglist>>2,
                                                   NP_StackSize,   8192,
                                                   NP_FreeSeglist, FALSE,
                                                   NP_CloseInput,  FALSE,
                                                   NP_CloseOutput, FALSE,
                                                   TAG_END)->pr_MsgPort)) {
#endif
#endif
                                PutMsg(port,(struct Message *)&launch->launch_msg);
                                if (arb_msg->flags&ARB_WAIT) {
                                    launch->reply_msg=arb_msg;
                                    arb_msg=NULL;
                                }
                                else ret=1;
                            }
                        }
                        else {
                            LFreeRemember(&launch->memory);
                            FreeMem(launch,sizeof(struct ProcessStart));
                        }
                    }
                    break;
            }
            if (arb_msg) {
                arb_msg->command=ret;
                ReplyMsg((struct Message *)arb_msg);
            }
        }
        if (remove) break;
        Wait(wait_mask);
    }

    if (replyport) LDeletePort(replyport);
    Forbid();
    ReplyMsg(my_startup_message);
    return;
}

struct Screen *open_subprocess_screen(title,font,memkey,pens)
char *title;
struct TextFont *font;
struct DOpusRemember **memkey;
short *pens;
{
    struct ExtNewScreen newscreen;
    struct TagItem *screentags;
    struct TextAttr *screenattr;
    struct Screen *screen;
    UWORD *screen_drawinfo;

    if (!(screenattr=LAllocRemember(memkey,sizeof(struct TextAttr),MEMF_CLEAR)) ||
        !(screentags=LAllocRemember(memkey,sizeof(struct TagItem)*5,MEMF_CLEAR)) ||
        !(screen_drawinfo=LAllocRemember(memkey,sizeof(scr_drawinfo),0)))
        return(NULL);

    CopyMem((char *)scr_drawinfo,(char *)screen_drawinfo,sizeof(scr_drawinfo));

    if (pens) {
        screen_drawinfo[DETAILPEN]=pens[ARB_PEN_DETAIL];
        screen_drawinfo[BLOCKPEN]=pens[ARB_PEN_BLOCK];
    }

    screentags[0].ti_Tag=SA_DisplayID;
    screentags[0].ti_Data=clone_screen(MainScreen,&newscreen);
    screentags[1].ti_Tag=SA_Pens;
    screentags[1].ti_Data=(IPTR)screen_drawinfo;
    screentags[2].ti_Tag=SA_AutoScroll;
    screentags[2].ti_Data=TRUE;
    screentags[3].ti_Tag=SA_Interleaved;
    screentags[3].ti_Data=TRUE;
    screentags[4].ti_Tag=TAG_END;
    screentags[4].ti_Data=0;

/*  if (system_version2)*/ {
        struct DimensionInfo dimbuf;
        int width,height;

        if ((GetDisplayInfoData(NULL,
            (char *)&dimbuf,
            sizeof(struct DimensionInfo),
            DTAG_DIMS,
            screentags[0].ti_Data))) {

            width=(dimbuf.TxtOScan.MaxX-dimbuf.TxtOScan.MinX)+1;
            height=(dimbuf.TxtOScan.MaxY-dimbuf.TxtOScan.MinY)+1;
            if (newscreen.Width>width) newscreen.Width=width;
            if (newscreen.Height>height) newscreen.Height=height;
        }
  }

    screenattr->ta_Name=font->tf_Message.mn_Node.ln_Name;
    screenattr->ta_YSize=font->tf_YSize;

    newscreen.LeftEdge=0;
    newscreen.TopEdge=0;
    if (pens) {
        newscreen.DetailPen=pens[ARB_PEN_DETAIL];
        newscreen.BlockPen=pens[ARB_PEN_BLOCK];
    }
    else {
        newscreen.DetailPen=0;
        newscreen.BlockPen=1;
    }
    newscreen.Type=CUSTOMSCREEN|NS_EXTENDED;
    newscreen.Font=screenattr;
    newscreen.DefaultTitle=title;
    newscreen.Gadgets=NULL;
    newscreen.Extension=screentags;
    newscreen.Depth=config->scrdepth;

    if (!(screen=OpenScreen((struct NewScreen *)&newscreen))) return(NULL);
    load_palette(screen,config->new_palette);
    return(screen);
}
