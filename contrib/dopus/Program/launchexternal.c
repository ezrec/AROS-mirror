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

#ifdef __AROS__
#define STACK_SIZE AROS_STACKSIZE
#else
#define STACK_SIZE 8192
#endif

static ULONG data_window_refresh;              /* Windows that need refreshing */

int start_external(func)
struct dopus_func_start *func;
{
    int arg;
    char path[256],*ptr;
    ULONG stacksize = func->stack;

    func->key=NULL;
    func->status=0;
    func->resseg=NULL;

    if (!(func->replyport=LCreatePort(NULL,0))) return(0);
    if (!(func->startup.wbstartup.sm_ArgList=LAllocRemember(&func->key,
        sizeof(struct WBArg)*(func->argcount+1),MEMF_CLEAR|MEMF_PUBLIC)))
        return(0);

    func->startup.wbstartup.sm_Message.mn_Node.ln_Type=NT_MESSAGE;
    func->startup.wbstartup.sm_Message.mn_ReplyPort=func->replyport;
    func->startup.wbstartup.sm_Message.mn_Length=(UWORD)sizeof(struct DOpusStartup);

    func->startup.wbstartup.sm_NumArgs=func->argcount;
    for (arg=0;arg<func->argcount;arg++)
        func->startup.wbstartup.sm_ArgList[arg].wa_Name=func->args[arg];

    strcpy(path,func->segname);
    if ((ptr=BaseName(path))) *ptr=0;
D(bug("start_external: Lock(%s)\n",path));
    if (!(func->startup.wbstartup.sm_ArgList[0].wa_Lock=Lock(path,ACCESS_READ)))
        return(0);

    func->startup.retcode=0;

    func->status=FS_CURDIR;
    func->olddir=CurrentDir(func->startup.wbstartup.sm_ArgList[0].wa_Lock);

    if (!func->segment) {
//        if (system_version2) {
            Forbid();
            if ((func->resseg=FindSegment(func->segname,NULL,0))) {
D(bug("Segment %s found\n",func->segname));
                func->resseg->seg_UC++;
                func->segment=func->resseg->seg_Seg;
            }
            Permit();
//        }
        if (!func->segment &&
            !(func->segment=(BPTR)LoadSeg(func->segname)))
            return(0);
    }

    func->status=FS_SEGMENT;

    func->startup.wbstartup.sm_Segment=func->segment;
D(bug("start_external: CreateProc(%s from %lx)\n",func->procname,func->startup.wbstartup.sm_Segment));
//    if (!(func->startup.wbstartup.sm_Process=CreateProc(func->procname,
//        main_proc->pr_Task.tc_Node.ln_Pri,func->startup.wbstartup.sm_Segment,func->stack)))
// JRZ: begin
    if (stacksize < STACK_SIZE) stacksize = STACK_SIZE;
    if (!(func->startup.wbstartup.sm_Process=&CreateNewProcTags(NP_Name,        (IPTR)func->procname,
                                                                NP_Priority,    main_proc->pr_Task.tc_Node.ln_Pri,
                                                                NP_Seglist,     func->startup.wbstartup.sm_Segment,
                                                                NP_StackSize,  stacksize,
                                                                NP_FreeSeglist, FALSE,
                                                                NP_CloseInput,  FALSE,
                                                                NP_CloseOutput, FALSE,
                                                                TAG_END)->pr_MsgPort))
// JRZ: end
     return(0);

    func->startup.wbstartup.sm_ToolWindow=NULL;
    PutMsg(func->startup.wbstartup.sm_Process,(struct Message *)&func->startup);

    func->status=FS_LAUNCHED;
    return(1);
}

int close_external(func,wait)
struct dopus_func_start *func;
int wait;
{
    if (func->status>=FS_LAUNCHED && wait) {
        WaitPort(func->replyport);
        GetMsg(func->replyport);
    }
    if (func->status>=FS_SEGMENT) {
        Forbid();
        if (func->resseg) {
            func->resseg->seg_UC--;
            func->segment=0;
        }
        else if (!func->flags&FF_SAVESEG && func->segment) {
            UnLoadSeg(func->segment);
            func->segment=0;
        }
        Permit();
    }
    if (func->status>=FS_CURDIR) {
        CurrentDir(func->olddir);
        UnLock(func->startup.wbstartup.sm_ArgList[0].wa_Lock);
    }
    if (func->replyport) LDeletePort(func->replyport);
    LFreeRemember(&func->key);
    return(func->startup.retcode);
}

void doconfig()
{
    char buf[100],buf1[20],replyname[50],portname[50],funcpath[80];
    int old_bufcount,a;
    struct MsgPort *conport,*cmdport;
    struct dopusconfigmsg *repmsg;
    struct configconfig cfg,*rcfg;
    struct ConfigStuff cstuff;
    struct dopus_func_start config_func;
    char *func_args[2];
    char *old_name;
    char old_language[30];

    if (!(checkwindowquit())) return;

    lsprintf(replyname,"%s%ld",config_replyport_basename,(long int)system_dopus_runcount);
    if (!(conport=LCreatePort(replyname,20))) return;

    lsprintf(buf,"%ld",(long int)system_dopus_runcount);
    lsprintf(buf1,"dopus4_config%ld",(long int)system_dopus_runcount);

    strcpy(funcpath,"ConfigOpus");

    if (!configopus_segment) {
        dostatustext(globstring[STR_LOADING_CONFIG]);
        FindSystemFile("ConfigOpus",funcpath,80,SYSFILE_MODULE);
    }

    func_args[0]=funcpath;
    func_args[1]=buf;

    config_func.segment=configopus_segment;
    config_func.procname="ConfigOpus";

    config_func.segname=funcpath;
    config_func.argcount=2;
    config_func.args=func_args;
    config_func.stack=STACK_SIZE;
    config_func.flags=(config->loadexternal&LOAD_CONFIG)?FF_SAVESEG:0;

    if (!(start_external(&config_func))) {
        close_external(&config_func,0);
        dostatustext(globstring[STR_CONFIG_NOT_FOUND]);
        LDeletePort(conport);
        return;
    }

    dostatustext(globstring[STR_WAITING_FOR_PORT]);
    lsprintf(portname,"dopus4_config_port%ld",(long int)system_dopus_runcount);
    cmdport=NULL;

    for (a=0;a<100;a++) {
        Forbid();
        cmdport=FindPort(portname);
        Permit();
        if (cmdport) break;
        Delay(5);
    }

    if (!cmdport) {
        close_external(&config_func,0);
        dostatustext(globstring[STR_CONFIG_NOT_FOUND]);
        LDeletePort(conport);
        return;
    }
    status_configuring=-1;
    shutthingsdown(1);
    old_bufcount=config->bufcount;
    strcpy(old_language,config->language);
    dotaskmsg(hotkeymsg_port,HOTKEY_KILLHOTKEYS,0,0,NULL,0);

    endnotifies();

    Forbid();
    old_name=arexx_port->mp_Node.ln_Name;
    arexx_port->mp_Node.ln_Name="foo";
    Permit();

    FOREVER {
        while ((repmsg=(struct dopusconfigmsg *)GetMsg(conport))) {
            switch (repmsg->command) {
                case CONFIG_ALL_DONE:
                    ReplyMsg((struct Message *)repmsg);
                    goto configdone;
                case CONFIG_GET_CONFIG:
                    cfg.config=config;
                    cfg.firsttype=dopus_firsttype;
                    cfg.typekey=filetype_key;
                    cfg.firstbank=dopus_firstgadbank;
                    cfg.changed=config_changed;
                    cfg.firsthotkey=dopus_firsthotkey;
                    strcpy(cfg.configname,str_config_basename);
D(bug("launchexternal: MainScreen = %lx\n",MainScreen));
                    cfg.Screen=MainScreen;
                    repmsg->buffer=(char *)&cfg;
                    break;
                case CONFIG_HERES_CONFIG:
                    rcfg=(struct configconfig *)repmsg->buffer;
                    dopus_firsttype=rcfg->firsttype;
                    dopus_firstgadbank=rcfg->firstbank;
                    dopus_firsthotkey=rcfg->firsthotkey;
                    filetype_key=rcfg->typekey;
                    config_changed=rcfg->changed;
                    StrCombine(str_config_file,rcfg->configname,".CFG",256);
                    strcpy(str_config_basename,rcfg->configname);
//                    Window=rcfg->Window; MainScreen=rcfg->Screen;
                    status_configuring=1;
                    break;
                case CONFIG_NEW_HOTKEY:
                    dotaskmsg(hotkeymsg_port,HOTKEY_HOTKEYCHANGE,0,0,NULL,0);
                    break;
            }
            ReplyMsg((struct Message *)repmsg);
        }

        Wait(1<<conport->mp_SigBit);
    }

configdone:
    config_func.flags=(config->loadexternal&LOAD_CONFIG)?FF_SAVESEG:0;
    close_external(&config_func,1);
    configopus_segment=config_func.segment;

    dopus_curgadbank=dopus_firstgadbank;
    data_gadgetrow_offset=data_drive_offset=0;
    if (config_changed)
     {
      if (config_changed==2) config_changed=0;
      closedisplay();
      status_configuring=-1;
  //    Window=NULL; MainScreen=NULL;
      dotaskmsg(hotkeymsg_port,HOTKEY_HOTKEYCHANGE,0,0,NULL,0);
      dotaskmsg(hotkeymsg_port,HOTKEY_NEWHOTKEYS,0,0,NULL,0);
      if (config->bufcount!=old_bufcount) {
          allocdirbuffers(config->bufcount);
          for (a=0;a<2;a++) strcpy(str_pathbuffer[a],dopus_curwin[a]->directory);
      }
      if (strcmp(old_language,config->language)) read_data_files(0);

      for (a=0;a<3;a++) {
          if (!(config->loadexternal&(1<<a)) && external_mod_segment[a]) {
              UnLoadSeg(external_mod_segment[a]);
              external_mod_segment[a]=0;
          }
      }

      fixcstuff(&cstuff);
      CheckConfig(&cstuff);
      SetUp(2);
//      config_changed=1;
     }
    else
     {
      initclock();
      fixcstuff(&cstuff);
      setupdisplay(1);
      dostatustext(globstring[STR_WELCOME_BACK_TO_DOPUS]);
      dopustofront();
     }

    LDeletePort(conport);

    Forbid();
    arexx_port->mp_Node.ln_Name=old_name;
    Permit();

    rexx_command(config->configreturnscript,NULL);

    startnotifies();
    status_configuring=0;
}

static char *external_modules[3]={
    "DOpus_Disk",
    "DOpus_Print",
    "DOpus_Icon"};

struct DiskData {
    int function;
    char funcpath[80];
    char *args[16];
    int argcount;
    int background;
};

void dopus_diskop(function,rexx,bg)
int function,rexx,bg;
{
    struct ArbiterLaunch launch;
    struct DiskData *disk_data;
    struct DOpusRemember *memkey=NULL;
    int a,fail=1;

    if ((disk_data=LAllocRemember(&memkey,sizeof(struct DiskData),MEMF_CLEAR))) {
        dostatustext(globstring[STR_STARTING_DISK_MODULE]);

        disk_data->function=function;
        disk_data->background=bg;

        strcpy(disk_data->funcpath,external_modules[SEG_DISK]);
        if (external_mod_segment[SEG_DISK] ||
           (FindSystemFile(external_modules[SEG_DISK],disk_data->funcpath,80,SYSFILE_MODULE)))
        {
D(bug("dopus_diskop: funcpath = %s\n",disk_data->funcpath));
            disk_data->args[0]=disk_data->funcpath;

            switch (function) {
                case FUNC_FORMAT:
                case FUNC_BGFORMAT:
                    disk_data->args[1]="f";
                    break;
                case FUNC_DISKCOPY:
                case FUNC_BGDISKCOPY:
                    disk_data->args[1]="d";
                    break;
                case FUNC_INSTALL:
                case FUNC_BGINSTALL:
                    disk_data->args[1]="i";
                    break;
                default:
                    return;
            }

            disk_data->argcount=3;

            if (rexx) {
                if ((disk_data->argcount+=rexx_argcount)>16) disk_data->argcount=16;
                for (a=3;a<disk_data->argcount;a++) {
                    if ((disk_data->args[a]=LAllocRemember(&memkey,strlen(rexx_args[a-3])+1,0)))
                        strcpy(disk_data->args[a],rexx_args[a-3]);
                }
            }

            for (a=disk_data->argcount;a<16;a++) disk_data->args[a]="";

            launch.launch_code=(void *)launch_diskop;
            launch.launch_name="dopus_disk_launch";
            launch.launch_memory=memkey;
            launch.data=(APTR)disk_data;

            if (!bg) data_window_refresh=0;

            arbiter_command(ARBITER_LAUNCH,(APTR)&launch,(bg)?0:ARB_WAIT);

            if (!bg) {
                for (a=0;a<2;a++) {
                    if (data_window_refresh&(1<<a)) {
                        strcpy(str_pathbuffer[a],dopus_curwin[a]->realdevice);
                        startgetdir(a,1);
                    }
                }
            }

            okay();
            fail=0;
        }
    }
    if (fail) dostatustext(globstring[STR_UNABLE_TO_LOAD_MODULE]);
}

void __saveds launch_diskop()
{
    struct Process *my_process;
    struct ArbiterMessage *my_startup_message;
    struct DiskData *disk_data;
    struct MsgPort *control_port;
    struct dopus_func_start disk_func;
    struct DOpusMessage *dopusmsg;
    struct Screen *screen;
    char portname[22],portbuf[22];
    int retcode=-1;

    my_process=(struct Process *)FindTask(NULL);
    my_process->pr_WindowPtr=(APTR)-1;
    WaitPort(&my_process->pr_MsgPort);
    my_startup_message=(struct ArbiterMessage *)GetMsg(&my_process->pr_MsgPort);

    disk_data=(struct DiskData *)(my_startup_message->data);

    if ((control_port=CreateUniquePort("DOPUS_DISK",portbuf,NULL))) {
        disk_func.segment=external_mod_segment[SEG_DISK];
        disk_func.procname=external_modules[SEG_DISK];
        disk_func.segname=disk_data->funcpath;

        StrCombine(portname,"&",portbuf,20);
        disk_data->args[2]=portname;

        disk_func.argcount=disk_data->argcount;
        disk_func.args=disk_data->args;
        disk_func.stack=STACK_SIZE;
        disk_func.flags=(config->loadexternal&LOAD_DISK)?FF_SAVESEG:0;

        if (start_external(&disk_func)) {
            struct DOpusRemember *memory=NULL;
//            int newscreen=0;

            screen=NULL;
/*
            if (disk_data->background && MainScreen &&
                (screen=open_subprocess_screen(globstring[STR_DISK_OP_TITLE],
                    scr_font[FONT_REQUEST],&memory,NULL))) newscreen=1;
*/
            FOREVER {
                if (GetMsg(disk_func.replyport)) break;
                while ((dopusmsg=(struct DOpusMessage *)GetMsg(control_port))) {
                    switch (dopusmsg->command) {
                        case DOPUSMSG_GETVIS:
                            fill_out_visinfo((struct VisInfo *)dopusmsg->data,screen);
                            break;
                        case DOPUSMSG_UPDATEDRIVE:
                            if (!disk_data->background) {
                                BPTR lock,testlock;
                                int win,ret;
                                char *drive;

                                drive=(char *)dopusmsg->data;
                                if (drive && drive[0]) {
                                    if ((lock=Lock(drive,ACCESS_READ))) {
                                        for (win=0;win<2;win++) {
                                            if (dopus_curwin[win]->realdevice[0] &&
                                                (testlock=Lock(dopus_curwin[win]->realdevice,ACCESS_READ))) {
                                                ret=CompareLock(lock,testlock);
                                                if (ret!=LOCK_DIFFERENT) data_window_refresh|=1<<win;
                                                UnLock(testlock);
                                            }
                                        }
                                        UnLock(lock);
                                    }
                                }
                            }
                            break;
                    }
                    ReplyMsg((struct Message *)dopusmsg);
                }
                Wait(1<<disk_func.replyport->mp_SigBit|1<<control_port->mp_SigBit);
            }
//            if (newscreen) CloseScreen(screen);
            LFreeRemember(&memory);
        }

        close_external(&disk_func,0);
        external_mod_segment[SEG_DISK]=disk_func.segment;
        LDeletePort(control_port);
        retcode=0;
    }

    my_startup_message->command=retcode;
    Forbid();
    ReplyMsg((struct Message *)my_startup_message);
    return;
}

void dopus_print(rexx,arglist,printdir,port,vdata)
int rexx;
struct DOpusArgsList *arglist;
int printdir;
char *port;
struct ViewData *vdata;
{
    char *args[16],portname[21],arglistbuf[20],funcpath[80];
    struct dopus_func_start print_func;
    int a,argcount,waitbits,abase=3;

    if (!vdata) dostatustext(globstring[STR_STARTING_PRINT_MODULE]);

    print_func.segment=external_mod_segment[SEG_PRINT];
    print_func.procname=external_modules[SEG_PRINT];

    strcpy(funcpath,external_modules[SEG_PRINT]);

    if (!print_func.segment)
      FindSystemFile(external_modules[SEG_PRINT],funcpath,80,SYSFILE_MODULE);

    print_func.segname=funcpath;

    args[0]=print_func.segname;

    StrCombine(portname,"&",port,20);
    args[1]=portname;

    if (printdir) {
        int win;

        if (!rexx || rexx_argcount<1 ||
            (win=atoi(rexx_args[0]))<0 || win>1) win=data_active_window;
        lsprintf(arglistbuf,"@%ld",(long int)win);
        abase=2;
        --rexx_argcount;
    }
    else lsprintf(arglistbuf,"!%ld",(long int)arglist);

    args[2]=arglistbuf;
    argcount=3;

    if (rexx && rexx_argcount>0) {
        if ((argcount+=rexx_argcount)>16) argcount=16;
        for (a=3;a<argcount;a++) args[a]=rexx_args[a-abase];
    }

    for (a=argcount;a<16;a++) args[a]="";

    print_func.argcount=argcount;
    print_func.args=args;
    print_func.stack=STACK_SIZE;
    print_func.flags=(config->loadexternal&LOAD_PRINT)?FF_SAVESEG:0;

    if (!(start_external(&print_func))) {
        close_external(&print_func,0);
        if (!vdata) dostatustext(globstring[STR_UNABLE_TO_LOAD_MODULE]);
        return;
    }

    waitbits=1<<print_func.replyport->mp_SigBit;
    if (vdata) waitbits|=1<<vdata->view_port->mp_SigBit;
    else waitbits|=rexx_signalbit;

    FOREVER {
        if ((Wait(waitbits))&rexx_signalbit && !vdata) {
            rexx_dispatch(0);
            continue;
        }
        if (vdata) {
            struct DOpusMessage *dmsg;
            struct DOpusArgsList *arg;

            while ((dmsg=(struct DOpusMessage *)GetMsg(vdata->view_port))) {
                switch (dmsg->command) {
                    case DOPUSMSG_GETVIS:
                        CopyMem((char *)&vdata->view_vis_info,(char *)dmsg->data,
                            sizeof(struct VisInfo));
                        break;
                    case DOPUSMSG_GETNEXTFILE:
                        arg=(struct DOpusArgsList *)dmsg->data;
                        if (arg->single_file) {
                            strcpy(arg->file_data,arg->single_file);
                            arg->single_file=NULL;
                        }
                        else arg->file_data[0]=0;
                        break;
                }
                ReplyMsg((struct Message *)dmsg);
            }
        }
        if (GetMsg(print_func.replyport)) break;
    }

    close_external(&print_func,0);
    external_mod_segment[SEG_PRINT]=print_func.segment;

    if (!vdata) okay();
}

int dopus_iconinfo(filename)
char *filename;
{
    char *args[3],portname[14],funcpath[80];
    struct dopus_func_start icon_func;
    int a;

    icon_func.segment=external_mod_segment[SEG_ICON];
    icon_func.procname=external_modules[SEG_ICON];

    strcpy(funcpath,external_modules[SEG_ICON]);

    if (!icon_func.segment)
        FindSystemFile(external_modules[SEG_ICON],funcpath,80,SYSFILE_MODULE);

    icon_func.segname=funcpath;

    StrCombine(portname,"&",str_arexx_portname,13);

    args[0]=icon_func.segname;
    args[1]=portname;
    args[2]=filename;

    icon_func.argcount=3;
    icon_func.args=args;
    icon_func.stack=STACK_SIZE;
    icon_func.flags=(config->loadexternal&LOAD_ICON)?FF_SAVESEG:0;

    if (!(start_external(&icon_func))) {
        close_external(&icon_func,0);
        dostatustext(globstring[STR_UNABLE_TO_LOAD_MODULE]);
        return(-4);
    }

    FOREVER {
        if ((Wait(1<<icon_func.replyport->mp_SigBit|rexx_signalbit))&rexx_signalbit) {
            rexx_dispatch(0);
            continue;
        }
        if (GetMsg(icon_func.replyport)) break;
    }

    a=close_external(&icon_func,0);
    external_mod_segment[SEG_ICON]=icon_func.segment;
    return(a);
}

void setup_externals()
{
    int a;
    char funcbuf[256];
    APTR wsave;

    wsave=main_proc->pr_WindowPtr;
    main_proc->pr_WindowPtr=(APTR)-1;

    if (config->loadexternal&LOAD_CONFIG) {
        FindSystemFile("ConfigOpus",funcbuf,256,SYSFILE_MODULE);
        configopus_segment=LoadSeg(funcbuf);
    }
    else configopus_segment=NULL;

    for (a=0;a<3;a++) {
        if (config->loadexternal&(1<<a)) {
            FindSystemFile(external_modules[a],funcbuf,256,SYSFILE_MODULE);
            external_mod_segment[a]=LoadSeg(funcbuf);
        }
        else external_mod_segment[a]=NULL;
    }

    main_proc->pr_WindowPtr=wsave;
}

void fill_out_visinfo(vis,scr)
struct VisInfo *vis;
struct Screen *scr;
{
    if (scr && (!Window || Window->WScreen!=scr)) {
        vis->vi_flags&=~VISF_WINDOW;
        vis->vi_screen=scr;

        vis->vi_fg=config->requestfg;
        vis->vi_bg=config->requestbg;
        vis->vi_shine=config->gadgettopcol;
        vis->vi_shadow=config->gadgetbotcol;
        vis->vi_stringcol[0]=config->stringfgcol;
        vis->vi_stringcol[1]=config->stringbgcol;
        vis->vi_activestringcol[0]=config->stringselfgcol;
        vis->vi_activestringcol[1]=config->stringselbgcol;
    }
    else {
        if (vis->vi_flags&VISF_WINDOW) vis->vi_screen=(struct Screen *)Window;
        else if (Window) vis->vi_screen=Window->WScreen;
        else vis->vi_screen=NULL;

        vis->vi_fg=screen_pens[config->requestfg].pen;
        vis->vi_bg=screen_pens[config->requestbg].pen;
        vis->vi_shine=screen_pens[config->gadgettopcol].pen;
        vis->vi_shadow=screen_pens[config->gadgetbotcol].pen;
        vis->vi_stringcol[0]=screen_pens[config->stringfgcol].pen;
        vis->vi_stringcol[1]=screen_pens[config->stringbgcol].pen;
        vis->vi_activestringcol[0]=screen_pens[config->stringselfgcol].pen;
        vis->vi_activestringcol[1]=screen_pens[config->stringselbgcol].pen;
    }

    if (vis->vi_flags&VISF_8POINTFONT) vis->vi_font=scr_font[FONT_GENERAL];
    else vis->vi_font=scr_font[FONT_REQUEST];

    if (config->generalscreenflags&SCR_GENERAL_REQDRAG)
        vis->vi_flags|=VISF_BORDERS;
    else vis->vi_flags&=~VISF_BORDERS;

    vis->vi_language=config->language;
}
