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
#ifndef __AROS__
#include <proto/inovamusic.h>
#include <proto/screennotify.h>
#include <proto/sysinfo.h>
#include <proto/multiuser.h>
#include <proto/pm.h>
#else
#include "music.h"
#endif
#include <proto/locale.h>
#define NO_INLINE_STDARG

void quit()
{
    int a;

    remclock();
    removehotkeys();
    endnotifies();
/*
    if (keyboard_req) {
        CloseDevice((struct IORequest *)keyboard_req);
        LDeleteExtIO((struct IORequest *)keyboard_req);
    }
*/
    if (input_req) {
        CloseDevice((struct IORequest *)input_req);
        LDeleteExtIO((struct IORequest *)input_req);
    }

#ifndef __AROS__
    if (ScreenNotifyBase && snm_port) while (! RemWorkbenchClient(snm_handle)) Delay(10);
    if (snm_port) LDeletePort(snm_port);
    CloseLibrary(ScreenNotifyBase);
#endif

    close_rexx_port();

    main_proc->pr_WindowPtr=windowptr_save;
    closedisplay();

    remove_arbiter();

    allocdirbuffers(0);
D(bug("Freeing special dirs\n"));
    for (a=0;a<2;a++) {
        freedir(dopus_specialwin[a],-1);
        if ( completion[a].memory ) LFreeRemember(&completion[a].memory); // HUX
    }
    if (dir_memory_pool) LibDeletePool(dir_memory_pool);
    for (a=0;a<NUMFONTS;a++) freefont(a);
    freedynamiccfg();
    freehelp();
    LFreeRemember(&general_key);
    LFreeRemember(&border_key);
    LFreeRemember(&menu_key);
    FreeStringFile(&stringdata);

    if (configopus_segment) UnLoadSeg(configopus_segment);
    for (a=0;a<3;a++) {
        if (external_mod_segment[a]) UnLoadSeg(external_mod_segment[a]);
    }

        if ( user_appicon ) FreeDiskObject ( user_appicon ); // HUX

    if (nil_file_handle) Close(nil_file_handle);
    UnLock(ramdisk_lock);
    if (func_reselection.reselection_list)
        FreeMem(func_reselection.reselection_list,func_reselection.reselection_size);
    if (count_port) LDeletePort(count_port);
    if (arexx_port) LDeletePort(arexx_port);
    if (general_port) LDeletePort(general_port);
    if (appmsg_port) LDeletePort(appmsg_port);

#ifndef __AROS__
    if (sortmenu) PM_FreePopupMenu(sortmenu);
#endif
    /*if (IconBase)*/ CloseLibrary(IconBase);
    /*if (DiskfontBase)*/ CloseLibrary(DiskfontBase);
#ifdef USE_POWERPACKER
    /*if (PPBase)*/ CloseLibrary((struct Library *)PPBase);
#endif
    /*if (WorkbenchBase)*/ CloseLibrary(WorkbenchBase);
    /*if (CxBase)*/ CloseLibrary(CxBase);
    /*if (RexxSysBase)*/ CloseLibrary((struct Library *)RexxSysBase);
    /*if (UtilityBase)*/ CloseLibrary((struct Library *)UtilityBase);
    CloseLibrary(CyberGfxBase);
#ifndef __AROS__
    CLOSE_PM_LIB;
#endif
    CloseLibrary((struct Library *)xfdMasterBase);
#ifdef USE_XADMASTER
    CloseLibrary((struct Library *)xadMasterBase);
#endif
#ifndef __AROS__    
    if (sysinfo)
     {
      FreeSysInfo(sysinfo);
      CloseLibrary(SysInfoBase);
     }
    if (AccountsBase) {
        if (user_info) FreeUserInfo(user_info);
        if (group_info) FreeGroupInfo(group_info);
        CloseLibrary(AccountsBase);
    }
    if (muBase) {
        if (mu_userinfo) muFreeUserInfo(mu_userinfo);
        if (mu_groupinfo) muFreeGroupInfo(mu_groupinfo);
        CloseLibrary((struct Library *)muBase);
    }
    if (MUSICBase) {
        FlushModule();
        CloseLibrary((struct Library *)MUSICBase);
    }
#endif
    CloseLocale(locale);
    CloseLibrary(LocaleBase);
    CloseLibrary(DataTypesBase);
    CloseLibrary(AmigaGuideBase);
    CloseLibrary(GadToolsBase);

    /*if (DOpusBase)*/ CloseLibrary((struct Library *) DOpusBase);

    main_proc->pr_CIS=old_pr_cis;
    main_proc->pr_COS=old_pr_cos;
    main_proc->pr_ConsoleTask=old_pr_consoletask;

#ifdef __SASC_60
        __exit(0);
#else
        exit(0);
#endif
}

void remclock()
{
    if (clockmsg_port) {
        dotaskmsg(clockmsg_port,TASK_QUIT,0,0,NULL,0);
        /*DeleteTask(clock_task);*/ clock_task=NULL;
    }
}

void removehotkeys()
{
    if (hotkeymsg_port) {
        dotaskmsg(hotkeymsg_port,TASK_QUIT,0,0,NULL,0);
        DeleteTask(hotkey_task); hotkey_task=NULL;
    }
}

void freefont(num)
int num;
{
    if (scr_font[num]) CloseFont(scr_font[num]);
    scr_font[num]=NULL;
}

void freehelp()
{
    LFreeRemember(&help_key);
    dopus_firsthelp=NULL;
}

void freedynamiccfg()
{
    struct ConfigStuff cstuff;

    fixcstuff(&cstuff);
    FreeConfig(&cstuff);
    cstufffix(&cstuff);
}

int checkwindowquit()
{
    struct Window *wind,*temp;
//    int a;

    if (!MainScreen) return(1);
/*
    for (a = 1, wind = MainScreen->FirstWindow; wind; wind = wind->NextWindow)
     {
      if (wind != Window)
       {
        a = 0;
        break;
       }
     }
    if (a) return(1);
*/
    if ((PubScreenStatus(MainScreen, PSNF_PRIVATE) & PSNF_PRIVATE) == 0)
     {
      busy();
      if (!(simplerequest(globstring[STR_ALIEN_WINDOWS],globstring[STR_CLOSE],
          str_cancelstring,NULL))) {
          unbusy();
          PubScreenStatus(MainScreen, 0);
          return(0);
      }
      for (wind = MainScreen->FirstWindow; wind; )
       {
        temp = wind->NextWindow;
        if (wind != Window) CloseWindow(wind);
        wind = temp;
       }
      unbusy();
     }
    return(1);
}

void dotaskmsg(port,command,value,total,data,flag)
struct MsgPort *port;
int command,value,total;
char *data;
int flag;
{
    if (port) {
        struct dopustaskmsg taskmsg;

        taskmsg.msg.mn_Node.ln_Type=NT_MESSAGE;
        taskmsg.msg.mn_ReplyPort=general_port;
        taskmsg.msg.mn_Length=(UWORD)sizeof(struct dopustaskmsg);
        taskmsg.command=command;
        taskmsg.value=value;
        taskmsg.total=total;
        taskmsg.data=data;
        taskmsg.flag=flag;
        Forbid();
        PutMsg(port,(struct Message *)&taskmsg);
        Permit();
        WaitPort(general_port);
/* cut by Jacek Rzeuski (31.03.2000) to avoid on-copy-exit crashes
        GetMsg(general_port);
*/
/* inserted by Jacek Rzeuski (31.03.2000) to avoid on-copy-exit crashes */
        while(GetMsg(general_port) != (struct Message *)&taskmsg);
/* end of */
    }
}

void closedisplay()
{
    if (status_iconified) cleanupiconify();
    else {
        if (MainScreen) ScreenToBack(MainScreen);
        freedragbuffers();
        Forbid();
        if (Window) {
            if (Window->MenuStrip) ClearMenuStrip(Window);
            if (WorkbenchBase && dopus_appwindow) {
                RemoveAppWindow(dopus_appwindow);
                dopus_appwindow=NULL;
            }
            free_colour_table();
            CloseWindow(Window);
            Window=NULL;
        }
        if (MainScreen) {
            if (! CloseScreen(MainScreen))
             {
              simplerequest(globstring[STR_CANNOT_CLOSE_SCREEN],str_okaystring,NULL);
              while(!(CloseScreen(MainScreen))) Delay(50);
             }
            MainScreen=NULL;
        }
        Permit();
    }
}
