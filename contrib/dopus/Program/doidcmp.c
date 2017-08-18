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

#ifndef __AROS__
#include <devices/newmouse.h>
#else
#include <devices/rawkeycodes.h>
#endif
#include "dopus.h"
#ifndef __AROS__
#include <libraries/screennotify.h>
#endif

void doidcmp()
{
    int x,y,a,function,wmes,stringgd=0,waitbits,b,x1,y1,c,win,dir,num;
    ULONG class;
    UWORD code,gadgetid,menunum,itemnum,qual;
    UBYTE old_change_state;
    char buf[80],buf2[30],ch;
    struct dopusgadgetbanks *bank,*bank1;
    struct AppMessage *apmsg;
    struct dopushotkey *hotkey;
    struct dopusfuncpar par;
    struct Gadget *gad;
    struct MenuItem *item;

//foreverloop:
    FOREVER {
        waitbits=1<<Window->UserPort->mp_SigBit|1<<count_port->mp_SigBit|rexx_signalbit|INPUTSIG_HOTKEY|1<<snm_port->mp_SigBit;
        if (/*system_version2 &&*/ WorkbenchBase && dopus_appwindow)
            waitbits|=1<<appmsg_port->mp_SigBit;
        if ((wmes=Wait(waitbits))&INPUTSIG_HOTKEY) {
            if (dopus_globalhotkey==(struct dopushotkey *)-1) {
                function=FUNC_QUIT; dopus_globalhotkey=NULL;
                goto foobarbaz;
            }
            else if (dopus_globalhotkey==(struct dopushotkey *)-2) {
                function=FUNC_ICONIFY; dopus_globalhotkey=NULL;
                goto foobarbaz;
            }
            else if (!dopus_globalhotkey) continue;
            hotkey=dopus_globalhotkey; dopus_globalhotkey=NULL;
            CopyMem((char *)&hotkey->func.which,(char *)&par,sizeof(struct dopusfuncpar));
            par.which&=FLAG_OUTWIND|FLAG_NOQUOTE|FLAG_SHELLUP|FLAG_DOPUSUP;
            par.which|=FLAG_ASYNC;
            if (par.which&FLAG_OUTWIND) par.which|=FLAG_WB2F;
            status_flags|=STATUS_FROMHOTKEY;
            dofunctionstring(hotkey->func.function,hotkey->name,NULL,&par);
            status_flags&=~STATUS_FROMHOTKEY;
            continue;
        }
        if (wmes&rexx_signalbit) {
            rexx_dispatch(1);
            if (func_global_function)
                internal_function(func_global_function,rexx_global_flag,NULL,NULL);
            status_haveaborted=0;
            continue;
        }
/// "AppMessage"
        if (WorkbenchBase && dopus_appwindow &&
            (wmes&(1<<appmsg_port->mp_SigBit))) {
            ActivateWindow(Window);
            while ((apmsg=(struct AppMessage *)GetMsg(appmsg_port))) {
                if (apmsg->am_Type==AMTYPE_APPWINDOW && apmsg->am_ID==APPWINID) {
                    for (a=0;a<apmsg->am_NumArgs;a++) {
                        if (apmsg->am_ArgList[a].wa_Lock) {
                            if ((b=isinwindow(apmsg->am_MouseX,apmsg->am_MouseY))!=-1) { // lister area
                                if (!(*apmsg->am_ArgList[a].wa_Name)) {
                                    PathName(apmsg->am_ArgList[a].wa_Lock,str_pathbuffer[b],256);
                                    checkdir(str_pathbuffer[b],&path_strgadget[b]);
                                    startgetdir(b,SGDFLAGS_CANMOVEEMPTY);
                                    break;
                                }
                                else {
                                    c=data_active_window;
                                    makeactive(b,0);
                                    PathName(apmsg->am_ArgList[a].wa_Lock,func_external_file,256);
                                    TackOn(func_external_file,apmsg->am_ArgList[a].wa_Name,256);
                                    dofunctionstring("*copy",NULL,NULL,NULL);
                                    makeactive(c,0);
                                }
                            }
                            else if (dopus_curgadbank &&
                                (b=gadgetfrompos(apmsg->am_MouseX,apmsg->am_MouseY))!=-1) { // gadget bank area
                                b+=(data_gadgetrow_offset*7);
                                if (isvalidgad(&dopus_curgadbank->gadgets[b])) {
                                    PathName(apmsg->am_ArgList[a].wa_Lock,func_external_file,256);
                                    if (func_external_file[0] &&
                                        func_external_file[(strlen(func_external_file)-1)]==':' &&
                                        !apmsg->am_ArgList[a].wa_Name[0])
                                        TackOn(func_external_file,"Disk.info",256);
                                    else TackOn(func_external_file,apmsg->am_ArgList[a].wa_Name,256);
                                    if (!(CheckExist(func_external_file,NULL)))
                                        StrConcat(func_external_file,".info",256);
                                    dofunctionstring(dopus_curgadbank->gadgets[b].function,
                                        dopus_curgadbank->gadgets[b].name,
                                        NULL,(struct dopusfuncpar *)&dopus_curgadbank->gadgets[b].which);
                                }
                            }
                            else if (apmsg->am_MouseY<scrdata_yoffset+scrdata_status_height) {
                                if (apmsg->am_ArgList[a].wa_Name[0]) {
                                    char pathbuf[256];

                                    PathName(apmsg->am_ArgList[a].wa_Lock,pathbuf,256);
                                    strcpy(func_external_file,pathbuf);
                                    TackOn(func_external_file,apmsg->am_ArgList[a].wa_Name,256);
                                    ftype_doubleclick(pathbuf,apmsg->am_ArgList[a].wa_Name,0);
                                    unbusy();
                                }
                            }
                        }
                    }
                    func_external_file[0]=0;
                }
                ReplyMsg((struct Message *)apmsg);
            }
        }
///
/// "DOS notify"
        if (wmes&(1<<count_port->mp_SigBit)) {
            struct NotifyMessage *note;
            int got=0;

            while ((note=(struct NotifyMessage *)GetMsg(count_port))) {
D(bug("DOS notification message\n"));
                if (/*system_version2 &&*/ config->dynamicflags&UPDATE_NOTIFY) {
                    a=note->nm_NReq->nr_UserData;
                    ReplyMsg((struct Message *)note);
                    if (!(got&(1<<a))) {
                        got|=1<<a;

                        if (a==0 || a==1) {
                            if (!(config->dirflags&DIRFLAGS_REREADOLD) ||
                                IntuitionBase->ActiveWindow==Window) {
                                struct DirWindowPars notifypars;

                                notifypars.reselection_list=NULL;
                                makereselect(&notifypars,a);
                                startgetdir(a,SGDFLAGS_CANMOVEEMPTY);
                                doreselect(&notifypars,0);
                                makereselect(&notifypars,-1);
                            }
                        }
                    }
                }
                else ReplyMsg((struct Message *)note);
            }
        }
#ifndef __AROS__
///
/// "Screen notify"
        if (wmes & (1 << snm_port->mp_SigBit))
         {
          BOOL do_iconify;

          while ((snm = (struct ScreenNotifyMessage *) GetMsg(snm_port)))
           {
            do_iconify = FALSE;
            if (snm->snm_Type == SCREENNOTIFY_TYPE_WORKBENCH)
             {
              if (snm->snm_Value == FALSE) /* Workbench close notification */
               {
                if (! status_iconified) if (config->screenmode == MODE_WORKBENCHUSE) do_iconify = TRUE;
               }
             }
            if (do_iconify) internal_function(FUNC_SCREENNOTIFY,0,NULL,NULL);
            else ReplyMsg((struct Message *) snm);
           }
         }
#endif
///
        while (getintuimsg()) {
            x=IMsg->MouseX;
            y=IMsg->MouseY;
            time_current_sec=IMsg->Seconds;
            time_current_micro=IMsg->Micros;
            class=IMsg->Class;
            code=IMsg->Code;
            qual=IMsg->Qualifier;
            if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN) {
                gad=(struct Gadget *) IMsg->IAddress;
                gadgetid=gad->GadgetID;
            }
            ReplyMsg((struct Message *) IMsg);
            if (Window->FirstRequest) continue;

            function=status_haveaborted=0;
            switch (class) {
/// "IDCMP_ACTIVEWINDOW"
                case IDCMP_ACTIVEWINDOW:
                    for (a=0;a<2;a++) check_old_buffer(a);
                    break;

///
/// "IDCMP_NEWSIZE"
                case IDCMP_NEWSIZE:
                    config->scr_winw=Window->Width;
                    config->scr_winh=Window->Height;
                    if (!MainScreen) {
                        config->scrw=config->scr_winw;
                        config->scrh=config->scr_winh;
                    }
                    config->screenflags&=~(SCRFLAGS_DEFWIDTH|SCRFLAGS_DEFHEIGHT);
                    remclock();
                    SetUp(0);
                    dostatustext(str_last_statustext);
                    break;

///
/// "IDCMP_CLOSEWINDOW"
                case IDCMP_CLOSEWINDOW:
                    busy();
                    if (!(config->generalflags&GENERAL_FORCEQUIT)) a=simplerequest(globstring[STR_REALLY_QUIT],globstring[STR_QUIT],str_cancelstring,globstring[STR_ICONIFY],NULL);
                    else a = 1;

                    if (a==2) function=FUNC_ICONIFY;
                    else if (a==1)
                     {
                      function=FUNC_QUIT;
                      rexx_argcount=1;
                      strcpy(rexx_args[0],"force");
                     }
                    unbusy();
                    break;

///
/// "IDCMP_DISKREMOVED"
                case IDCMP_DISKREMOVED:
                    setupchangestate();
                    break;

///
/// "IDCMP_DISKINSERTED"
                case IDCMP_DISKINSERTED:
                    old_change_state=disk_change_state;
                    setupchangestate();
                    if (config->dirflags&DIRFLAGS_AUTODISKC) {
                        if (IntuitionBase->ActiveWindow==Window
#ifdef _USE_SMALL_Q
                             || !(status_flags&STATUS_IANSCRAP)
#endif
                           ) {
                            c=0;
                            for (a=0;a<4;a++) {
                                if ((old_change_state&(1<<a))!=(disk_change_state&(1<<a))) {
                                    lsprintf(buf,"DF%ld:",(long int)a);
                                    lsprintf(buf2,"PC%ld:",(long int)a);
                                    for (b=0;b<2;b++) {
                                        if ((Stricmp(dopus_curwin[b]->realdevice,buf)==0) ||
                                            (Stricmp(dopus_curwin[b]->realdevice,buf2)==0)) {
                                            identify_and_load(b,a);
                                            c=1;
                                        }
                                    }
                                }
                                if (c) break;
                            }
                            if (c) break;
                        }
                    }
                    if (IntuitionBase->ActiveWindow==Window) {
                        if (config->dirflags&DIRFLAGS_AUTODISKL) {
                            for (a=0;a<4;a++) {
                                if ((old_change_state&(1<<a))!=(disk_change_state&(1<<a)) &&
                                    identify_and_load(-1,a)) {
                                    lsprintf(buf,globstring[STR_NEW_DISK_INSERTED],a);
                                    if ((b=simplerequest(buf,globstring[STR_LEFT_WINDOW],
                                        str_cancelstring,globstring[STR_RIGHT_WINDOW],NULL))) {
                                        identify_and_load(b-1,a);
                                    }
                                }
                            }
                        }
                    }
                    break;

///
/// "IDCMP_MENUPICK"
                case IDCMP_MENUPICK:
                    Window->Flags|=RMBTRAP;
                    if (!Window->MenuStrip) break;
                    if (stringgd) {
                        checkstringgads(stringgd);
                        stringgd=0;
                    }
                    if (code==MENUNULL) break;
                    menunum=menu_real_number[MENUNUM(code)];
                    num=ITEMNUM(code);
                    item=menu_menus[menunum].FirstItem;
                    for (a=0,itemnum=0;a<num && item;a++) {
                        if (item->Flags&ITEMTEXT) ++itemnum;
                        item=item->NextItem;
                    }
                    if (item) {
                        a=(menunum*20)+itemnum;
                        if (!isvalidgad(&config->menu[a])) break;
                        dofunctionstring(config->menu[a].function,config->menu[a].name,
                            NULL,(struct dopusfuncpar *)&config->menu[a].which);
                    }
                    break;
///
/// "IDCMP_GADGETDOWN"
                case IDCMP_GADGETDOWN:
                    if (stringgd) {
                        checkstringgads(stringgd);
                        stringgd=0;
                    }
                    switch (gadgetid) {
                        case SCRGAD_MOVEUP1:
                        case SCRGAD_MOVEDOWN1:
                        case SCRGAD_MOVEUP2:
                        case SCRGAD_MOVEDOWN2:
                            if (gadgetid==SCRGAD_MOVEUP1 || gadgetid==SCRGAD_MOVEUP2) dir=-1;
                            else dir=1;
                            if (gadgetid==SCRGAD_MOVEUP1 || gadgetid==SCRGAD_MOVEDOWN1) win=0;
                            else win=1;
                            if (config->generalflags&GENERAL_ACTIVATE) makeactive(win,0);
                            verticalscroll(win,dir);
//                            Delay(5);
                            while (!getintuimsg())
                                if (gad->Flags&SELECTED) verticalscroll(win,dir);
                            ReplyMsg((struct Message *) IMsg);
                            break;

                        case SCRGAD_MOVELEFT1:
                        case SCRGAD_MOVERIGHT1:
                        case SCRGAD_MOVELEFT2:
                        case SCRGAD_MOVERIGHT2:
                            if (gadgetid==SCRGAD_MOVELEFT1 || gadgetid==SCRGAD_MOVELEFT2) dir=-1;
                            else dir=1;
                            if (gadgetid==SCRGAD_MOVELEFT1 || gadgetid==SCRGAD_MOVERIGHT1) win=0;
                            else win=1;
                            if (config->generalflags&GENERAL_ACTIVATE) makeactive(win,0);
                            horizontalscroll(win,dir);
//                            Delay(5);
                            while (!getintuimsg())
                                if (gad->Flags&SELECTED) horizontalscroll(win,dir);
                            ReplyMsg((struct Message *) IMsg);
                            break;

                        case SCRGAD_LEFTPROP:
                        case SCRGAD_RIGHTPROP:
                            if (gadgetid==SCRGAD_LEFTPROP) a=0;
                            else a=1;
                            if (config->generalflags&GENERAL_ACTIVATE) makeactive(a,0);
                            doposprop(a);
                            FOREVER {
                                Wait(1<<Window->UserPort->mp_SigBit);
                                class=0;
                                while (getintuimsg()) {
                                    class=IMsg->Class;
                                    ReplyMsg((struct Message *) IMsg);
                                    if (class==IDCMP_MOUSEMOVE) doposprop(a);
                                    else if (class==IDCMP_GADGETUP) break;
                                }
                                if (class==IDCMP_GADGETUP) break;
                            }
                            dopus_curwin[a]->offset=-1;
                            doposprop(a);
                            break;

                        case SCRGAD_LEFTHPROP:
                        case SCRGAD_RIGHTHPROP:
                            if (gadgetid==SCRGAD_LEFTHPROP) a=0;
                            else a=1;
                            if (config->generalflags&GENERAL_ACTIVATE) makeactive(a,0);
                            doposhprop(a);
                            FOREVER {
                                Wait(1<<Window->UserPort->mp_SigBit);
                                class=0;
                                while (getintuimsg()) {
                                    class=IMsg->Class;
                                    ReplyMsg((struct Message *) IMsg);
                                    if (class==IDCMP_MOUSEMOVE) doposhprop(a);
                                    else if (class==IDCMP_GADGETUP) break;
                                }
                                if (class==IDCMP_GADGETUP) break;
                            }
                            dopus_curwin[a]->hoffset=-1;
                            doposhprop(a);
                            break;

                        case SCRGAD_DRIVEPROP:
                            if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS) {
                                doposdriveprop();
                                FOREVER {
                                    Wait(1<<Window->UserPort->mp_SigBit);
                                    class=0;
                                    while (getintuimsg()) {
                                        class=IMsg->Class;
                                        ReplyMsg((struct Message *) IMsg);
                                        if (class==IDCMP_MOUSEMOVE) doposdriveprop();
                                        else if (class==IDCMP_GADGETUP) break;
                                    }
                                    if (class==IDCMP_GADGETUP) break;
                                }
                                doposdriveprop();
                            }
                            break;

                        case SCRGAD_GADGETPROP:
                            if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS) {
                                FOREVER {
                                    doposgadgetprop(0);
                                    if (getintuimsg()) {
                                        class=IMsg->Class;
                                        ReplyMsg((struct Message *) IMsg);
                                        if (class==IDCMP_MOUSEMOVE) continue;
                                        else if (class==IDCMP_GADGETUP) break;
                                    }
                                    Wait(1<<Window->UserPort->mp_SigBit);
                                }
                                doposgadgetprop(1);
                            }
                            break;

                        case SCRGAD_LEFTSTRING:
                        case SCRGAD_RIGHTSTRING:
                            stringgd=gadgetid+1;
                            break;
                    }
                    break;

///
/// "IDCMP_GADGETUP"
                case IDCMP_GADGETUP:
                    if (stringgd) {
                        if (gadgetid!=SCRGAD_LEFTSTRING && gadgetid!=SCRGAD_RIGHTSTRING)
                            checkstringgads(stringgd);
                        stringgd=0;
                    }
                    if (gadgetid>=MAIN_GAD_BASE && gadgetid<MAIN_GAD_BASE+GADCOUNT) {
                        a=(gadgetid-MAIN_GAD_BASE)+(data_gadgetrow_offset*7);
#ifndef __AROS__
D(bug("doidcmp: a = %ld\n",a);KDump(&dopus_curgadbank->gadgets[a],sizeof(struct newdopusfunction)));
#endif
                        if (!dopus_curgadbank || !(isvalidgad(&dopus_curgadbank->gadgets[a]))) break;
                        dofunctionstring(dopus_curgadbank->gadgets[a].function,
                            dopus_curgadbank->gadgets[a].name,
                            NULL,(struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
                        break;
                    }
                    switch (gadgetid) {
                        case SCRGAD_LEFTSTRING:
                        case SCRGAD_RIGHTSTRING:
                            if (code==0x5f) do_path_completion(gadgetid,qual);
                            else if (code!=0x9) {
                                makeactive(gadgetid,0);
                                startgetdir(gadgetid,SGDFLAGS_CANMOVEEMPTY);
                            }
                            break;

                        case SCRGAD_BUFFERLEFT1:
                            incrementbuf(0,-1,1);
                            break;

                        case SCRGAD_BUFFERRIGHT1:
                            incrementbuf(0,1,1);
                            break;

                        case SCRGAD_BUFFERLEFT2:
                            incrementbuf(1,-1,1);
                            break;

                        case SCRGAD_BUFFERRIGHT2:
                            incrementbuf(1,1,1);
                            break;

                        case FUNC_PARENT1:
                            makeactive(0,0);
                            function=FUNC_PARENT;
                            break;

                        case FUNC_PARENT2:
                            makeactive(1,0);
                            function=FUNC_PARENT;
                            break;

                        case SCRGAD_TINYFILTER:
                            if (status_flags&STATUS_HELP) {
                                dohelp(NULL,"*Filter",0,0,NULL);
                                screen_gadgets[SCRGAD_TINYFILTER].Flags^=GFLG_SELECTED;
                                dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER],&globstring[STR_TINY_BUTTONS][6],1);
                            }
                            else if (screen_gadgets[SCRGAD_TINYFILTER].Flags&GFLG_SELECTED) {
                                busy();
                                if (!(whatsit(globstring[STR_ENTER_FILTER],40,str_filter,NULL)) ||
                                    !str_filter[0]) {
                                    screen_gadgets[SCRGAD_TINYFILTER].Flags&=~GFLG_SELECTED;
                                    dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER],&globstring[STR_TINY_BUTTONS][6],1);
                                    str_filter_parsed[0]=0;
                                }
                                else LParsePatternI(str_filter,str_filter_parsed);
                                unbusy();
                            }
                            else str_filter_parsed[0]=0;
                            break;

                        default:
                            function=gadgetid;
#ifdef _USE_SMALL_Q
                            if (function==FUNC_QUIT && status_flags&STATUS_IANSCRAP) {
                                rexx_argcount=1;
                                strcpy(rexx_args[0],"force");
                            }
#endif
                            break;
                    }
                    break;

///
/// "IDCMP_RAWKEY"
                case IDCMP_RAWKEY:
/*
                    code&=0x7f;
                    readkeys(var_key_matrix);
                    a=code/8; b=code%8;
                    if (!(var_key_matrix[a]&(1<<b)))
*/
                    if (code & IECODE_UP_PREFIX)
                     {
                      flushidcmp();
                      break;
                     }
                    qual&=VALID_QUALIFIERS;
                    bank=dopus_firstgadbank;
                    while (bank) {
                        for (a=0;a<GADCOUNT;a++) {
                            if (check_key_press((struct dopusfunction *)&bank->gadgets[a],code,qual)) {
                                dofunctionstring(bank->gadgets[a].function,bank->gadgets[a].name,
                                    NULL,(struct dopusfuncpar *)&bank->gadgets[a].which);
                                goto foobarbaz;
                            }
                        }
                        bank=bank->next;
                    }
                    for (a=0;a<MENUCOUNT;a++) {
                        if (check_key_press((struct dopusfunction *)&config->menu[a],code,qual)) {
                            dofunctionstring(config->menu[a].function,config->menu[a].name,
                                NULL,(struct dopusfuncpar *)&config->menu[a].which);
                            goto foobarbaz;
                        }
                    }
                    for (a=0;a<USEDRIVECOUNT;a++) {
                        if (check_key_press(&config->drive[a],code,qual)) {
                            strcpy(str_pathbuffer[data_active_window],config->drive[a].function);
                            startgetdir(data_active_window,SGDFLAGS_CANMOVEEMPTY);
                            goto foobarbaz;
                        }
                    }
                    if (qual&(IEQUALIFIER_RCOMMAND|IEQUALIFIER_LCOMMAND)) {
                        RawkeyToStr(code,qual,NULL,buf,0);
                        switch (ToUpper(buf[0])) {
                            case 'R': function=FUNC_RESELECT; break;
                            case 'A': function=FUNC_AREXX; break;
                            case 'S': function=FUNC_SELECT; break;
                            case 'B': function=FUNC_BUFFERLIST; break;
                            case ' ':
                                if (qual&IEQUALIFIER_LCOMMAND) findfirstsel(data_active_window,ENTRY_FILE);
                                else findfirstsel(data_active_window,ENTRY_DIRECTORY);
                                break;
/*
                        switch (code) {
                            case 0x13: function=FUNC_RESELECT; break;  // R
                            case 0x20: function=FUNC_AREXX; break;     // A
                            case 0x21: function=FUNC_SELECT; break;    // S
                            case 0x35: function=FUNC_BUFFERLIST; break;// B
                            case 0x40:                                 // SPACE
                                if (qual&IEQUALIFIER_LCOMMAND) findfirstsel(data_active_window,ENTRY_FILE);
                                else findfirstsel(data_active_window,ENTRY_DIRECTORY);
                                break;
*/
                        }
                    }
                    else {
                        win = -1;
                        switch (code) {
                            case RAWKEY_NM_WHEEL_UP:
                                if ((y>=scrdata_gadget_ypos) && (y<=scrdata_gadget_ypos+(scr_gadget_rows*scrdata_gadget_height))) // gadget bank area
                                 {
                                  if ((x>=scrdata_gadget_offset) && (x<scrdata_gadget_xpos) && scr_gadget_rows) //drive bank
                                   {
                                    if (scrdata_drive_width)
                                     {
                                      data_drive_offset--;
                                      if (data_drive_offset < 0) data_drive_offset = 0;
                                      if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS) {
                                          FixSliderPot(Window,&drive_propgad,data_drive_offset,
                                              USEDRIVECOUNT,scr_gadget_rows,!status_iconified);
                                      }
                                      if (!status_iconified) fixdrivegadgets();
                                     }
                                   }
                                  else // button bank
                                   {
                                    int max = (getgadbankcount()*6)/scr_gadget_rows,
                                        pos = GetSliderPos(&gadget_propgad,max,1);

                                    if (--pos < 0) pos = 0;
                                    else
                                     {
                                      FixSliderPot(Window,&gadget_propgad,pos,max,1,0);
                                      doposgadgetprop(0);
                                     }
                                   }
                                  break;
                                 }
                                else
                                  win = isinwindow(x,y); // lister area
                                
                                if (win == -1) break;
                            case CURSOR_UP:
                                if (win == -1) win = data_active_window;
                                if (dopus_curwin[win]->total<scrdata_dispwin_lines) break;
                                if (qual&(IEQUALIFIER_CONTROL|IEQUALIFIER_ANYSHIFT)) {
                                    if (qual&IEQUALIFIER_CONTROL) dopus_curwin[win]->offset=0;
                                    else {
                                        dopus_curwin[win]->offset-=scrdata_dispwin_lines;
                                        if (dopus_curwin[win]->offset<0)
                                            dopus_curwin[win]->offset=0;
                                    }
                                    fixvertprop(win);
                                    displaydir(win);
                                    break;
                                }
                                verticalscroll(win,-1);
                                break;
                            case RAWKEY_NM_WHEEL_DOWN:
                                if ((y>=scrdata_gadget_ypos) && (y<=scrdata_gadget_ypos+(scr_gadget_rows*scrdata_gadget_height))) // gadget bank area
                                 {
                                  if ((x>=scrdata_gadget_offset) && (x<scrdata_gadget_xpos) && scr_gadget_rows) //drive bank
                                   {
                                    if (scrdata_drive_width)
                                     {
                                      data_drive_offset++;
                                      if (data_drive_offset > (USEDRIVECOUNT-scr_gadget_rows)) data_drive_offset = USEDRIVECOUNT - scr_gadget_rows;
                                      if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS) {
                                          FixSliderPot(Window,&drive_propgad,data_drive_offset,
                                              USEDRIVECOUNT,scr_gadget_rows,!status_iconified);
                                      }
                                      if (!status_iconified) fixdrivegadgets();
                                     }
                                   }
                                  else // button bank
                                   {
                                    int max = (getgadbankcount()*6)/scr_gadget_rows,
                                        pos = GetSliderPos(&gadget_propgad,max,1);

                                    if (++pos == max) pos = max-1;
                                    else
                                     {
                                      FixSliderPot(Window,&gadget_propgad,pos,max,1,0);
                                      doposgadgetprop(0);
                                     }
                                   }
                                  break;
                                 }
                                else
                                  win = isinwindow(x,y); // lister area
                                
                                if (win == -1) break;
                            case CURSOR_DOWN:
                                if (win == -1) win = data_active_window;
                                if (dopus_curwin[win]->total<scrdata_dispwin_lines) break;
                                if (qual&(IEQUALIFIER_CONTROL|IEQUALIFIER_ANYSHIFT)) {
                                    if (qual&IEQUALIFIER_CONTROL)
                                        dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
                                    else {
                                        dopus_curwin[win]->offset+=scrdata_dispwin_lines;
                                        if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
                                            dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
                                    }
                                    fixvertprop(win);
                                    displaydir(win);
                                    break;
                                }
                                verticalscroll(win,1);
                                break;
                            case CURSOR_LEFT:
                                if (qual&(IEQUALIFIER_LALT|IEQUALIFIER_RALT)) {
                                    incrementbuf(data_active_window,-1,1);
                                    break;
                                }
                            case RAWKEY_NM_WHEEL_LEFT:
                                if (code == RAWKEY_NM_WHEEL_LEFT)
                                 {
                                  win = isinwindow(x,y); // lister area
                                  if (win == -1) break;
                                 }
                                if (win == -1) win = data_active_window;
                                if (dopus_curwin[win]->total==0) break;
                                if (qual&(IEQUALIFIER_CONTROL|IEQUALIFIER_ANYSHIFT)) {
                                    if (qual&IEQUALIFIER_CONTROL) dopus_curwin[win]->hoffset=0;
                                    else {
                                        dopus_curwin[win]->hoffset-=scrdata_dispwin_nchars[win];
                                        if (dopus_curwin[win]->hoffset<0) dopus_curwin[win]->hoffset=0;
                                    }
                                    refreshwindow(win,1);
                                    break;
                                }
                                horizontalscroll(win,-1);
                                break;
                            case CURSOR_RIGHT:
                                if (qual&(IEQUALIFIER_LALT|IEQUALIFIER_RALT)) {
                                    incrementbuf(data_active_window,1,1);
                                    break;
                                }
                            case RAWKEY_NM_WHEEL_RIGHT:
                                if (code == RAWKEY_NM_WHEEL_RIGHT)
                                 {
                                  win = isinwindow(x,y); // lister area
                                  if (win == -1) break;
                                 }
                                if (win == -1) win = data_active_window;
                                if (dopus_curwin[win]->total==0) break;
                                if (qual&(IEQUALIFIER_CONTROL|IEQUALIFIER_ANYSHIFT)) {
                                    if (qual&IEQUALIFIER_CONTROL) {
                                        dopus_curwin[win]->hoffset=dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win];
                                        if (dopus_curwin[win]->hoffset<0) dopus_curwin[win]->hoffset=0;
                                    }
                                    else {
                                        dopus_curwin[win]->hoffset+=scrdata_dispwin_nchars[win];
                                        if (dopus_curwin[win]->hoffset>=(dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win]))
                                            dopus_curwin[win]->hoffset=dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win];
                                    }
                                    refreshwindow(win,1);
                                    break;
                                }
                                horizontalscroll(win,1);
                                break;
                        }
                        switch (code) {
                            case 0x5f: // HELP
                                function=FUNC_HELP;
                                break;
                            case 0x40: // SPACE
                            case 0x42: // TAB
                                makeactive(1-data_active_window,1);
                                break;
                            case 0x44: // RETURN
                                if (qual&IEQUALIFIER_ANYSHIFT) function=FUNC_BUFFERLIST;
                                else if (qual&(IEQUALIFIER_LALT|IEQUALIFIER_RALT)) function=FUNC_DEVICELIST;
                                else ActivateStrGad(&path_strgadget[data_active_window],Window);
                                break;
                            case 0x0b: // - _
                                dosizedirwindows(-60000);
                                break;
                            case 0x0c: // = +
                                dosizedirwindows(0);
                                break;
                            case 0x0d: // \ |
                                dosizedirwindows(60000);
                                break;
                            case 0x1a: // [ {
                                goto prevgadgetbank;
                                break;
                            case 0x1b: // ] }
                                goto nextgadgetbank;
                                break;
                            default:
                                if (code<0x40 && !(qual&IEQUALIFIER_CONTROL)) { // a digit or char
                                    RawkeyToStr(code,qual,NULL,buf,0);
                                    ch=buf[0];
                                    if (_isprint(ch)) findfirstchar(data_active_window,ch);
                                }
                            break;
                      }
                    }
                    unbusy();
                    break;

///
/// "IDCMP_MOUSEBUTTONS"
                case IDCMP_MOUSEBUTTONS:
                    if (stringgd) {
                        checkstringgads(stringgd);
                        stringgd=0;
                    }
                    if (code==SELECTDOWN) {
                        if (y>=scrdata_diskname_ypos && y<scrdata_diskname_ypos+scrdata_diskname_height) { // disk name bar
                            a=(x<scrdata_dispwin_center)?0:1;
                            if (data_active_window==1-a) {
                                 if (DoubleClick(time_previous_sec,time_previous_micro,
                                     time_current_sec,time_current_micro)) {
                                    copydirwin(dopus_curwin[1-a],dopus_curwin[a],a);
                                }
                                makeactive(a,1);
                            }
                            else {
                                if (DoubleClick(time_previous_sec,time_previous_micro,
                                    time_current_sec,time_current_micro)) {
                                    busy();
                                    if (str_pathbuffer[data_active_window][0] &&
                                        do_parent_multi(str_pathbuffer[data_active_window]))
                                        startgetdir(data_active_window,SGDFLAGS_CANMOVEEMPTY|SGDFLAGS_CANCHECKBUFS);
                                    unbusy();
                                }
                                time_previous_sec=time_current_sec;
                                time_previous_micro=time_current_micro;
                            }
                            checksize(a);
                            break;
                        }
                        else if (x>scrdata_dispwin_center-3 && x<scrdata_dispwin_center+2 &&
                            y>=scrdata_dispwin_ypos && y<scrdata_dispwin_height+scrdata_dispwin_ypos) { // lister inner edge
                            dosizedirwindows(65536);
                        }
                        else if ((a=isinwindow(x,y))!=-1) doselection(a,TRUE);                          // lister area
                        else if (x>=scrdata_xoffset && x<scrdata_xoffset+scrdata_clock_width &&         // button bank area
                            y>scrdata_clock_ypos-3) {
nextgadgetbank:
                            if ((data_gadgetrow_offset+=scr_gadget_rows)>=6) {
                                data_gadgetrow_offset=0;
                                bank=dopus_curgadbank;
                                if (dopus_curgadbank && dopus_curgadbank->next)
                                    dopus_curgadbank=dopus_curgadbank->next;
                                else dopus_curgadbank=dopus_firstgadbank;
                                if (bank!=dopus_curgadbank || scr_gadget_rows<6) drawgadgets(0,0);
                            }
                            else drawgadgets(0,0);
                            fixgadgetprop();
                        }
                    }
                    else if (code==MENUDOWN) {
                        if ((a=DoRMBGadget(mainrmbgads,Window))>-1) {		// check buttons reacting on RMB click
                            if (a==FUNC_ROOT) {
                               char buf[256];
                               int w;

                               if (x<scrdata_xoffset+3) w = 0;
                               else w=1;
                               makeactive(w,0);

                               if (!(dopus_curwin[w]->flags & DWF_ARCHIVE))
                                 {
                                  strcpy(buf,dopus_curwin[w]->directory);
                                  if (!(doroot(buf)))
                                   {
                                    function = FUNC_DEVICELIST;
                                    break;
                                   }
                                 }
                            }
                            function=a;
                        }
                        else if (y<scrdata_diskname_ypos ||
                            y>scrdata_yoffset+scrdata_height ||
                            x<scrdata_xoffset ||
                            x>scrdata_xoffset+scrdata_width)		// window frame
                              quickfixmenus();

                        else if (y>=scrdata_diskname_ypos && y<=scrdata_dispwin_ypos) {  /* diskname bar */
                            if (x<scrdata_dispwin_center) a=1;
                            else a=0;
                            if (data_active_window==a) {
                                if (DoubleClick(time_previous_sec,time_previous_micro,
                                    time_current_sec,time_current_micro)) {
                                    swapdirwin();
                                    time_previous_sec=0;
                                }
                                makeactive(1-a,1);
                            }
                            else {
                                time_previous_sec=time_current_sec;
                                time_previous_micro=time_current_micro;

#ifndef __AROS__
                                handlelistermenu(1-a);
#endif
                            }
                            checksize(1-a);
                        }
                        else if (x>=scrdata_xoffset && x<scrdata_xoffset+scrdata_clock_width &&
                            y>scrdata_clock_ypos-3) {												// clock bar
                            if (qual&IEQUALIFIER_ANYSHIFT) {
                                drawgadgets(0,GADCOUNT/2);
                                FOREVER {
                                    while (getintuimsg()) {
                                        class=IMsg->Class; code=IMsg->Code;
                                        ReplyMsg((struct Message *) IMsg);
                                        if (class==MOUSEBUTTONS && code==MENUUP) break;
                                    }
                                    if (class==MOUSEBUTTONS && code==MENUUP) break;
                                    Wait(1<<Window->UserPort->mp_SigBit);
                                }
                                drawgadgets(0,0);
                            }
                            else {
prevgadgetbank:
                                if ((data_gadgetrow_offset-=scr_gadget_rows)<0) {
                                    data_gadgetrow_offset=6-scr_gadget_rows;
                                    bank1=dopus_curgadbank;
                                    bank=dopus_firstgadbank;
                                    while (bank && bank->next && bank->next!=dopus_curgadbank) bank=bank->next;
                                    if (!bank) dopus_curgadbank=dopus_firstgadbank;
                                    else dopus_curgadbank=bank;
                                    if (bank1!=dopus_curgadbank || scr_gadget_rows<6) drawgadgets(0,0);
                                }
                                else drawgadgets(0,0);
                                fixgadgetprop();
                            }
                        }
                        else if (y>scrdata_dispwin_ypos+1 &&
                            y<scrdata_dispwin_height+scrdata_dispwin_ypos+2) {        // lister area
                            if (x>1 && x<scrdata_dispwin_center-13) {
                              if (config->generalflags & GENERAL_FMPARENT) function = FUNC_PARENT;
                              else {
                                dormbscroll(0);
                                doselinfo(0);
                              }
                            }
                            else if (x>scrdata_dispwin_center+12 &&
                                x<screen_gadgets[SCRGAD_RIGHTPARENT].LeftEdge) {
                                if (config->generalflags & GENERAL_FMPARENT) function = FUNC_PARENT;
                                else {
                                  doselinfo(1);
                                  dormbscroll(1);
                                }
                            }
                        }
                        else if (x>=scrdata_gadget_xpos && y>=scrdata_gadget_ypos &&             // gadget bank area
                            y<scrdata_gadget_ypos+(scr_gadget_rows*scrdata_gadget_height)+1 &&
                            dopus_curgadbank) {

                            if ((c=gadgetfrompos(x,y))>-1) {
                                a=c+(GADCOUNT/2)+(data_gadgetrow_offset*7);
                                if (isvalidgad(&dopus_curgadbank->gadgets[a])) {
                                    x1=scrdata_gadget_xpos+((c%7)*scrdata_gadget_width);
                                    y1=scrdata_gadget_ypos+((c/7)*scrdata_gadget_height)-1;
                                    if (!(dormbgadget(x1,y1,
                                        &dopus_curgadbank->gadgets[c+(data_gadgetrow_offset*7)],
                                        &dopus_curgadbank->gadgets[a]))) {
                                        dofunctionstring(dopus_curgadbank->gadgets[a].function,
                                            dopus_curgadbank->gadgets[a].name,NULL,
                                            (struct dopusfuncpar *)&dopus_curgadbank->gadgets[a].which);
                                    }
                                }
                            }
                        }
                    }
                    else if (code == MIDDLEUP)
                     {
                      int win;

                      if (config->hotkeyflags & HOTKEY_USEMMB) break;

                      if ((win=isinwindow(x,y))!=-1)	// lister area
                       {
                        if (DoubleClick(time_previous_sec,time_previous_micro,time_current_sec,time_current_micro))
                         {
                          int a, owin;
                          owin = data_active_window;
                          data_active_window = win;
                          a = (y - scrdata_dirwin_ypos[win]) / scrdata_font_ysize;
                          a += dopus_curwin[win]->offset;
                          if (a < dopus_curwin[win]->total)
                           {
                            char buf[256];
                            struct dopusfiletype *type;
                            struct Directory *file;

                            select(win,a - dopus_curwin[win]->offset);
                            for (file = dopus_curwin[win]->firstentry; a--; file=file->next);
                            strcpy(buf,str_pathbuffer[win]);
                            TackOn(buf,file->name,256);
  D(bug("MMB double click on \"%s\"\n",buf));

                            if ((type=checkfiletype(buf,FTFUNC_MMBCLICK,0)))
                             {
                              struct dopusfuncpar par;

  D(bug("filetype %s matched\n",type->type));
                              par.which=type->which[FTFUNC_MMBCLICK];
                              par.stack=type->stack[FTFUNC_MMBCLICK];
                              par.pri=type->pri[FTFUNC_MMBCLICK];
                              par.delay=type->delay[FTFUNC_MMBCLICK];
                              par.key=par.qual=0; par.type=3;

                              if (type->actionstring[FTFUNC_MMBCLICK][0]) {
                                  do_title_string(type->actionstring[FTFUNC_MMBCLICK],buf,0,file->name);
                                  dostatustext(buf);
                              }
                              else buf[0]=0;
                              strcpy(func_single_file,file->name);
                              dofunctionstring(type->function[FTFUNC_MMBCLICK],file->name,buf,&par);
                              func_single_file[0]=0;
                             }
                            unselect(win,file);
                           }
                          data_active_window = owin;
                          time_previous_sec=0;
                         }
                        else
                         {
                          if (config->generalflags & GENERAL_MMBSELECTS) makeactive(win,0);
                          time_previous_sec=time_current_sec;
                          time_previous_micro=time_current_micro;
                         }
                       }
                     }
                    break;
///
            }
foobarbaz:
            if (function) internal_function(function,0,NULL,NULL);
            status_haveaborted=0;
        }
    }
//    goto foreverloop; // JRZ: Just in case???
}

struct IntuiMessage *getintuimsg(void)
{
    return((IMsg=(struct IntuiMessage *)GetMsg(Window->UserPort)));
}

void flushidcmp()
{
    while (getintuimsg()) ReplyMsg((struct Message *)IMsg);
}

int isinwindow(x,y)
int x,y;
{
    int win;

    for (win=0;win<2;win++) {
        if (x>=scrdata_dirwin_xpos[win] && x<scrdata_dirwin_xpos[win]+scrdata_dirwin_width[win] &&
            y>=scrdata_dirwin_ypos[win] && y<scrdata_dirwin_ypos[win]+scrdata_dirwin_height) return(win);
    }
    return(-1);
}


