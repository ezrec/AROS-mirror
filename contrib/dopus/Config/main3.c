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

#include "config.h"
#define MAXFUNCLISTLEN 100

#define NUM_TYPES 5

int editfunction(func,type,data)
struct dopusfunction *func;
int type;
APTR data;
{
    int a,selitem,x,y,b,ofp,obp,curtype=0,tof,waitbits;
    ULONG class;
    UWORD code,qual,gadgetid;
    struct Gadget *gad;
    char
        flagsel[NUMFLAGS],*funclist[MAXFUNCS+1],functype[MAXFUNCS],
        *displist[MAXFUNCS+1],buf[256],buf1[256],*ptr,t;
    struct DOpusRemember *key=NULL;
    struct DOpusListView *view;
    struct fileclass fclass;
    struct dopusfiletype *ftype;
    struct dopushotkey *hotkey;
    unsigned char c;

    if (type==CFG_FILETYPE) ftype=(struct dopusfiletype *)data;
    else if (type==CFG_HOTKEYS) hotkey=(struct dopushotkey *)data;

    add_appobject(1);

    for (a=0;a<NUMFLAGS;a++) {
        if (type!=CFG_HOTKEYS ||
            gadflagvals[a]==FF_OUTPUTWIN ||
            gadflagvals[a]==FF_NOQUOTE ||
            gadflagvals[a]==FF_SHELLSTARTUP ||
            gadflagvals[a]==FF_DOPUSSTARTUP) flagsel[a]=0;
        else flagsel[a]=LVARRAY_DISABLED;
    }

    selitem=-1;
    displist[0]=LAllocRemember(&key,MAXFUNCLISTLEN*(MAXFUNCS+1),MEMF_CLEAR);
    for (a=1;a<=MAXFUNCS;a++) {
        if (displist[0]) displist[a]=displist[a-1]+MAXFUNCLISTLEN;
        else displist[a]=NULL;
    }
    ofp=func->fpen; obp=func->bpen;
    editfuncgadgets[1].Flags&=~SELECTED;
    edit_actionbuf[0]=0;
    makehelpname(cfg_string[STR_EDIT]);

    AddGadgetBorders(&key,
        &editfuncgadgets[2],
        4,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgetBorders(&key,
        &editfuncgadgets[9],
        1,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgetBorders(&key,
        &editfuncgadgets[10],
        3,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgetBorders(&key,
        &editfuncgadgets[14],
        1,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);

startedit:
    edit_funcbuf[0]=0;
    SetAPen(rp,screen_pens[1].pen);
    ClearMenuStrip(Window);
    FSSetMenuStrip(Window,&editmenu);
    editlists[1].next=NULL;
    if (type==CFG_HOTKEYS) {
        namesinfo.MaxChars=40;
        strcpy(edit_namebuf,hotkey->name);
        editfuncgadgets[1].NextGadget=&editfuncgadgets[2];
        editfuncgadgets[12].NextGadget=NULL;
        editfuncgadgets[0].LeftEdge=x_off+132;
        editfuncgadgets[0].TopEdge=y_off+22;
        editfuncgadgets[0].Width=320;
        AddGadgets(Window,
            editfuncgadgets,
            editfuncgads2,
            13,
            screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
    }
    else if (type==CFG_GADGET || type==CFG_MENU) {
        namesinfo.MaxChars=64;
        if (((struct newdopusfunction *)func)->name)
            strcpy(edit_namebuf,((struct newdopusfunction *)func)->name);
        else edit_namebuf[0]=0;
        editfuncgadgets[1].NextGadget=&editfuncgadgets[2];
        editfuncgadgets[12].NextGadget=NULL;
        editfuncgadgets[0].LeftEdge=x_off+188;
        editfuncgadgets[0].TopEdge=y_off+42;
        editfuncgadgets[0].Width=292;
        AddGadgets(Window,
            editfuncgadgets,
            editfuncgads2,
            13,
            screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
    }
    else if (type==CFG_FILETYPE) {
        curtype=fixfunctypelist(ftype);
        editfuncgadgets[12].NextGadget=&editfuncgadgets[13];
        if (curtype==-1) curtype=0;
        editlists[1].next=&editlists[2];
        editlists[2].topitem=curtype;
        editlists[2].itemselected=curtype;
        AddGadgets(Window,
            &editfuncgadgets[2],
            &editfuncgads2[2],
            13,
            screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
    }
    setupeditdisplay(func,type,data,curtype,displist,funclist,functype,flagsel,1);
    doglassimage(&editfuncgadgets[10]);
    if (type==CFG_GADGET || type==CFG_MENU) {
        setupcolourbox(rp,x_off+150,y_off+15,func->fpen,func->bpen);
        SetAPen(rp,screen_pens[1].pen);
        do_gad_label(cfg_string[STR_SHORTCUT_KEY],x_off+121,y_off+62);
        showfuncob(rp,edit_namebuf,func->fpen,func->bpen,type,x_off+244,y_off+5);
        showkey(func->key,func->qual,x_off+130,y_off+55,374,10);
    }
    else if (type==CFG_HOTKEYS) {
        SetAPen(rp,screen_pens[1].pen);
        do_gad_label(cfg_string[STR_HOTKEY_USCORE],x_off+121,y_off+62);
        showkey(hotkey->code,hotkey->qualifier,x_off+130,y_off+55,374,10);
    }
    if (type!=CFG_FILETYPE)
        fixrmbimage(&sampleclearrmb,&editfuncgadgets[1],2,-1,-1);

    editgadsoff();

    waitbits=1<<Window->UserPort->mp_SigBit;
    if (appobject) waitbits|=1<<appport->mp_SigBit;

    FOREVER {
        if (appobject) {
            tof=0;
            while (appmsg=(struct AppMessage *)GetMsg(appport)) {
                ActivateWindow(Window);
                if (appmsg->am_ID==MY_APPOBJECT && appmsg->am_NumArgs>0) {
                    for (a=0;a<appmsg->am_NumArgs;a++) {
                        if ((*appmsg->am_ArgList[a].wa_Name)) {
                            if (selitem>-1) {
                                freestring(funclist[selitem]); funclist[selitem]=NULL;
                                if (edit_funcbuf[0]) makenewentry(funclist,functype,displist,selitem);
                                else if (selitem<editlists[0].count)
                                    deleteentry(funclist,functype,displist,selitem);
                                endedit(); selitem=-1;
                            }
                            if (editlists[0].count<MAXFUNCS) {
                                selitem=editlists[0].count;
                                editgadson(0);
                                functype[selitem]=2;
                                PathName(appmsg->am_ArgList[a].wa_Lock,edit_funcbuf,256);
                                TackOn(edit_funcbuf,appmsg->am_ArgList[a].wa_Name,256);
                                editlists[0].itemselected=-1;
                                setupforedit(functype[selitem],(a==(appmsg->am_NumArgs-1)));
                                tof=1;
                            }
                            else DisplayBeep(NULL);
                        }
                    }
                }
                ReplyMsg((struct Message *)appmsg);
            }
            if (tof && Screen) ScreenToFront(Screen);
        }
        while (IMsg=getintuimsg()) {
            if ((view=ListViewIDCMP(editlists,IMsg))==(struct DOpusListView *)-1) {
                class=IMsg->Class; code=IMsg->Code; qual=IMsg->Qualifier;
                x=IMsg->MouseX; y=IMsg->MouseY;
                if (class==GADGETUP || class==GADGETDOWN) {
                    gad=(struct Gadget *)IMsg->IAddress;
                    gadgetid=gad->GadgetID;
                }
                ReplyMsg((struct Message *)IMsg);
                switch (class) {
                    case IDCMP_MENUPICK:
                        if (code==MENUNULL) break;
                        switch (ITEMNUM(code)) {
                            case 0:
                            case 1:
                                if (type==CFG_FILETYPE) {
                                    LStrnCpy(edit_namebuf,ftype->type,22);
                                    edit_namebuf[22]=0;
                                    lsprintf(buf," (%ld)",(long int)(curtype+1));
                                    StrConcat(edit_namebuf,buf,28);
                                }
                                copytoclip(func,funclist,functype,flagsel);
                                if (ITEMNUM(code)==1) break;
                            case 2:
                                if (ITEMNUM(code)==2 && !(pasteclip(func,funclist,functype,displist,flagsel)))
                                    break;
                            case 4:
                                if (ITEMNUM(code)!=2)
                                    erasefunction(func,funclist,displist,flagsel);
                                if (type==CFG_GADGET || type==CFG_MENU) {
                                    edit_namebuf[255]=0;
                                    showfuncob(rp,edit_namebuf,func->fpen,func->bpen,type,x_off+244,y_off+5);
                                    showkey(func->key,func->qual,x_off+130,y_off+55,374,10);
                                    setupcolourbox(rp,x_off+150,y_off+15,func->fpen,func->bpen);
                                    RefreshStrGad(&editfuncgadgets[0],Window);
                                }
                                else if (type==CFG_HOTKEYS) {
                                    edit_namebuf[39]=0;
                                    showkey(hotkey->code,hotkey->qualifier,x_off+130,y_off+55,374,10);
                                    RefreshStrGad(&editfuncgadgets[0],Window);
                                }
                                RefreshListView(editlists,3);
                                RefreshGList(&editfuncgadgets[6],Window,NULL,3);
                                break;
                            case 6:
                                if (request(cfg_string[STR_REALLY_CLEAR_CLIPS])) {
                                    LFreeRemember(&clipkey);
                                    firstclip=NULL;
                                    DeleteFile("t:configopus.clip");
                                }
                                break;
                        }
                        break;

                    case IDCMP_RAWKEY:
                        if ((code>=0x60 && code<=0x67) || code&0x80) break;
                        if ((type!=CFG_GADGET && type!=CFG_MENU && type!=CFG_HOTKEYS) ||
                            !(editfuncgadgets[1].Flags&SELECTED)) {
                            buf[0]=0;
                            RawkeyToStr(code,qual,NULL,buf,10);
                            c = LToLower(buf[0]);
                            if (c == getkeyshortcut(cfg_string[STR_EDIT_NAME]))
                             {
                              if (type==CFG_GADGET || type==CFG_MENU || type==CFG_HOTKEYS)
                                  ActivateStrGad(&editfuncgadgets[0],Window);
                             }
                            else if ((c == getkeyshortcut(cfg_string[STR_HOTKEY_USCORE])) || (c == getkeyshortcut(cfg_string[STR_SHORTCUT_KEY])))
                             {
                              if (type==CFG_GADGET || type==CFG_MENU || type==CFG_HOTKEYS) {
                                  fixrmbimage(&sampleclearrmb,NULL,0,-1,-1);
                                  editfuncgadgets[1].Flags|=SELECTED;
                                  select_gadget(&editfuncgadgets[1],1);
                                  fixrmbimage(&sampleclearrmb,NULL,1,-1,-1);
                               }
                             }
                            else if (c == getkeyshortcut(cfg_string[STR_EDIT_STACKSIZE]))
                                    ActivateStrGad(&editfuncgadgets[6],Window);
                            else if (c == getkeyshortcut(cfg_string[STR_EDIT_PRIORITY]))
                                    ActivateStrGad(&editfuncgadgets[7],Window);
                            else if (c == getkeyshortcut(cfg_string[STR_EDIT_CLOSEDELAY]))
                                    ActivateStrGad(&editfuncgadgets[8],Window);
                            else if (c == getkeyshortcut(cfg_string[STR_EDIT_ACTION]))
                                    if (type==CFG_FILETYPE)
                                        ActivateStrGad(&editfuncgadgets[13],Window);
                            break;
                        }
                        qual&=IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|
                            IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT|
                            IEQUALIFIER_LALT|IEQUALIFIER_RALT;
                        if (type==CFG_HOTKEYS) {
                            hotkey->code=code; hotkey->qualifier=qual;
                        }
                        else {
                            func->key=code; func->qual=qual;
                        }
                        showkey(code,qual,x_off+130,y_off+55,374,10);
                        break;

                    case IDCMP_MOUSEBUTTONS:
                        if (code==SELECTDOWN) {
                            if (type==CFG_GADGET || type==CFG_MENU) {
                                a=func->fpen; b=func->bpen;
                                docolourgad(rp,x_off+150,y_off+15,x,y,edit_namebuf,&a,&b,type);
                                func->fpen=a; func->bpen=b;
                            }
                        }
                        else if (code==MENUDOWN) {
                            if (type!=CFG_FILETYPE && (fixrmbimage(&sampleclearrmb,NULL,0,x,y))) {
                                a=DoRMBGadget(&sampleclearrmb,Window);
                                fixrmbimage(&sampleclearrmb,NULL,1,-1,-1);
                                if (a==0) {
                                    if (type==CFG_HOTKEYS) {
                                        hotkey->code=0;
                                        hotkey->qualifier=0;
                                    }
                                    else {
                                        func->key=0xff;
                                        func->qual=0;
                                    }
                                    showkey(~0,0,x_off+130,y_off+55,374,10);
                                }
                                editfuncgadgets[1].Flags&=~SELECTED;
                                help_ok=1;
                                break;
                            }
                        }
                        break;

                    case IDCMP_GADGETUP:
                        switch (gadgetid) {
                            case EDIT_CANCEL:
                            case EDIT_OKAY:
                                if (gadgetid==EDIT_CANCEL) {
                                    func->fpen=ofp; func->bpen=obp;
                                }
                                else fixeditfunction(func,flagsel,funclist,functype,selitem,type,data,curtype);
                                freefunclist(funclist);
                                editgadson(1);
                                if (type==CFG_FILETYPE) freefunction(func);
                                LFreeRemember(&key);
                                RemoveListView(editlists,3);
                                ClearMenuStrip(Window);
                                FSSetMenuStrip(Window,&projectmenu);
                                makehelpname((char *)-1);
                                rem_appobject(1);
                                help_ok=1;
                                return((gadgetid==EDIT_OKAY));

                            case EDIT_EDITCLASS:
                                if (type!=CFG_FILETYPE) break;
                                fixeditfunction(func,flagsel,funclist,functype,selitem,type,data,curtype);
                                freefunclist(funclist);
                                editgadson(1);
                                RemoveListView(editlists,3);
                                fclass.last=fclass.next=NULL;
                                strcpy(fclass.type,ftype->type);
                                strcpy(fclass.typeid,ftype->typeid);
                                if (fclass.recognition=getcopy(ftype->recognition,-1,NULL)) {
                                    if (editclass(&fclass,-1)) {
                                        ftype->recognition=getcopy(fclass.recognition,-1,&typekey);
                                        strcpy(ftype->type,fclass.type);
                                        strcpy(ftype->typeid,fclass.typeid);
                                    }
                                    freestring(fclass.recognition);
                                }
                                cleanconfigscreen();
                                filetypetitle(ftype);
                                goto startedit;

                            case EDIT_NAME:
                                if (type!=CFG_HOTKEYS)
                                    showfuncob(rp,edit_namebuf,func->fpen,func->bpen,type,x_off+244,y_off+5);
                                if (code!=0x9) getnextgadget(gad);
                                break;

                            case EDIT_STACK:
                                CheckNumGad(&editfuncgadgets[6],Window,4000,999999);
                                if (code!=0x9) getnextgadget(gad);
                                break;

                            case EDIT_PRIORITY:
                                CheckNumGad(&editfuncgadgets[7],Window,-127,127);
                                if (code!=0x9) getnextgadget(gad);
                                break;

                            case EDIT_DELAY:
                                CheckNumGad(&editfuncgadgets[8],Window,-1,60);
                                if (code!=0x9) getnextgadget(gad);
                                break;

                            case EDIT_TYPE:
                                checkswapgad();
                                if (qual&IEQUALIFIER_LSHIFT || qual&IEQUALIFIER_RSHIFT) {
                                    if (type==CFG_HOTKEYS) {
                                        if ((--functype[selitem])<1) functype[selitem]=NUM_TYPES-1;
                                    }
                                    else if ((--functype[selitem])<0) functype[selitem]=NUM_TYPES-1;
                                }
                                else if ((++functype[selitem])>=NUM_TYPES)
                                    functype[selitem]=(type==CFG_HOTKEYS)?1:0;
                                SetAPen(rp,screen_pens[1].pen);
                                DoCycleGadget(&editfuncgadgets[9],Window,functypestr,functype[selitem]);
                                ActivateStrGad(&editfuncgadgets[11],Window);
                                break;

                            case EDIT_FUNC:
                            case EDIT_DUPLICATE:
                            case EDIT_NEWENTRY:
                                if (selitem>-1) {
                                    freestring(funclist[selitem]); funclist[selitem]=NULL;
                                    if (edit_funcbuf[0]) makenewentry(funclist,functype,displist,selitem);
                                    else if (selitem<editlists[0].count)
                                        deleteentry(funclist,functype,displist,selitem);
                                }
                                if (gadgetid==EDIT_DUPLICATE && edit_funcbuf[0] &&
                                    editlists[0].count<MAXFUNCS) {
                                    strcpy(edit_funcbuf,funclist[selitem]);
                                    functype[editlists[0].count]=functype[selitem];
                                    selitem=editlists[0].count;
                                    editlists[0].topitem+=MAXFUNCS;
                                    editlists[0].itemselected=-1;
                                    setupforedit(functype[selitem],1);
                                }
                                else if (gadgetid==EDIT_NEWENTRY && editlists[0].count<MAXFUNCS) {
                                    if (selitem>-1) {
                                        for (a=MAXFUNCS-1;a>selitem;a--) {
                                            funclist[a]=funclist[a-1];
                                            functype[a]=functype[a-1];
                                            makedispfunc(funclist[a],functype[a],displist[a]);
                                        }
                                        ++editlists[0].count;
                                        funclist[selitem]=getcopy(" ",2,NULL);
                                        makedispfunc(funclist[selitem],(type==CFG_HOTKEYS)?1:0,displist[selitem]);
                                    }
                                    else selitem=editlists[0].count;
                                    editgadson(0);
                                    functype[selitem]=(type==CFG_HOTKEYS)?1:0;
                                    edit_funcbuf[0]=0;
                                    editlists[0].itemselected=-1;
                                    setupforedit(functype[selitem],1);
                                }
                                else {
                                    endedit(); selitem=-1;
                                }
                                break;

                            case EDIT_DELETE:
                                if (selitem<editlists[0].count)
                                    deleteentry(funclist,functype,displist,selitem);
                                endedit(); selitem=-1;
                                break;

                            case EDIT_ARGREQ:
                                cmdlist.topitem=0;
                            case EDIT_REQUESTER:
                                if (funcrequester(
                                    (gadgetid==EDIT_ARGREQ)?FREQ_ARGREQ:functype[selitem],buf,
                                    (gadgetid==EDIT_ARGREQ)?cfg_string[STR_ARGUMENT_CONTROL_SEQUENCES]:
                                        cfg_string[STR_AVAILABLE_COMMANDS])) {
                                    strcpy(buf1,&edit_funcbuf[funcsinfo.BufferPos]);
                                    edit_funcbuf[funcsinfo.BufferPos]=0;
                                    StrConcat(edit_funcbuf,buf,256);
                                    if (gadgetid==EDIT_ARGREQ || functype[selitem]!=FT_INTERNAL)
                                        StrConcat(edit_funcbuf," ",256);
                                    StrConcat(edit_funcbuf,buf1,256);
                                    a=funcsinfo.BufferPos+strlen(buf)+1;
                                    RefreshStrGad(&editfuncgadgets[11],Window);
                                    funcsinfo.BufferPos=(a>255)?255:a;
                                }
//                                if (!version2) Delay(5);
                                ActivateGadget(&editfuncgadgets[11],Window,NULL);
                                break;

                            case EDIT_FILETYPEACTION:
                                if (code!=0x9) getnextgadget(gad);
                                break;

                            case EDIT_SAMPLE:
                                help_ok=(editfuncgadgets[1].Flags&GFLG_SELECTED)?0:1;
                                break;
                        }
                        break;
                }
            }
            else if (view) {
                switch (view->listid) {
                    case EDIT_FUNCTIONS:
                        if (editfuncgadgets[4].Flags&SELECTED) {
                            a=view->itemselected;
                            SwapMem((char *)displist[selitem],(char *)displist[a],60);
                            ptr=funclist[selitem]; funclist[selitem]=funclist[a]; funclist[a]=ptr;
                            t=functype[selitem]; functype[selitem]=functype[a]; functype[a]=t;
                            RefreshListView(&editlists[1],1);
                            endedit(); selitem=-1;
                        }
                        else {
                            if (selitem>-1) {
                                freestring(funclist[selitem]); funclist[selitem]=NULL;
                                if (edit_funcbuf[0]) makenewentry(funclist,functype,displist,selitem);
                                else if (selitem<editlists[0].count)
                                    deleteentry(funclist,functype,displist,selitem);
                            }
                            selitem=view->itemselected;
                            editgadson(1);
                            strcpy(edit_funcbuf,funclist[selitem]);
                            setupforedit(functype[selitem],1);
                        }
                        break;
                    case EDIT_FLAGS:
                        flagsel[view->itemselected]^=LVARRAY_SELECTED;
                        checkmutflags(flagsel,view->itemselected);
                        RefreshListView(&editlists[1],1);
                        break;
                    case EDIT_FUNCTIONTYPE:
                        if (type!=CFG_FILETYPE) break;
                        if (curtype==view->itemselected || view->itemselected>=FILETYPE_FUNCNUM)
                            break;
                        fixeditfunction(func,flagsel,funclist,functype,selitem,type,data,curtype);
                        if (selitem>-1) {
                            endedit(); selitem=-1;
                        }
                        curtype=view->itemselected;
                        freefunclist(funclist);
                        setupeditdisplay(func,type,data,curtype,displist,funclist,functype,flagsel,0);
                        break;
                }
            }
        }
        Wait(waitbits);
    }
}

void setuplist(list,x,y)
struct DOpusListView *list;
int x,y;
{
    list->window=Window;
    list->slidercol=screen_pens[1].pen;
    list->sliderbgcol=screen_pens[0].pen;
    list->textcol=screen_pens[1].pen;
    list->boxhi=screen_pens[config->gadgettopcol].pen;
    list->boxlo=screen_pens[config->gadgetbotcol].pen;
    list->arrowfg=screen_pens[1].pen;
    list->arrowbg=screen_pens[0].pen;
    list->itemfg=screen_pens[1].pen;
    list->itembg=screen_pens[0].pen;
    if (x>-1) {
        list->x=x_off+x;
        list->y=y_off+y;
    }
}

void makefunclist(func,funclist,functype)
char *func,**funclist,*functype;
{
    int len,a,pos,num,gottype;
    char buf[256],buf1[256];

    gottype=-1; pos=num=0;
    if (func) {
        len=strlen(func);
        for (a=0;a<=len;a++) {
            if (gottype==-1) {
                switch (func[a]) {
                    case FC_CHDIR: gottype=FT_CHDIR; break;
                    case FC_INTERNAL: gottype=FT_INTERNAL; break;
                    case FC_WORKBENCH: gottype=FT_WORKBENCH; break;
                    case FC_BATCH: gottype=FT_BATCH; break;
                    case FC_AREXX: gottype=FT_AREXX; break;
                    case FC_REQUESTER: gottype=FT_REQUESTER; break;
                    default:
                        if (!isspace(func[a]) && func[a]!='|') {
                            gottype=FT_EXECUTABLE;
                            --a;
                        }
                        break;
                }
                continue;
            }
            if ((gottype==FT_CHDIR && func[a]=='!') ||
                pos==255 || func[a]==0 || (func[a]=='|' && func[a+1]=='|')) {
                buf[pos]=0;
                if (gottype==FT_CHDIR) {
                    StrCombine(buf1,"CD ",buf,256);
                    strcpy(buf,buf1);
                    gottype=FT_EXECUTABLE;
                }
                if (funclist[num]=getcopy(buf,-1,NULL)) {
                    functype[num]=gottype;
                    ++num;
                    if (num==MAXFUNCS) break;
                }
                pos=0; gottype=-1;
            }
            else buf[pos++]=func[a];
        }
    }
    for (a=num;a<MAXFUNCS;a++) funclist[a]=NULL;
}

void makedispfunc(func,type,disp)
char *func,type,*disp;
{
    if (!disp) return;
    if (!func || !func[0] || type<0 || type>=NUM_TYPES) disp[0]=0;
    else {
        strcpy(disp,functypestr[type]);
        StrConcat(disp,spacestring,13);
        StrConcat(disp,func,MAXFUNCLISTLEN-2);
    }
}

char *compilefunclist(funclist,functype,key)
char **funclist,*functype;
struct DOpusRemember **key;
{
    int a,size,num;
    char *buf,typec,buf2[2];

    size=1; num=0;
    for (a=0;a<MAXFUNCS;a++) {
        if (!funclist[a] || !funclist[a][0]) break;
        if (a>0) size+=2;
        size+=strlen(funclist[a]);
        if (functype[a]!=FT_EXECUTABLE) ++size;
        ++num;
    }
    if (key) buf=LAllocRemember(key,size,MEMF_CLEAR);
    else buf=AllocMem(size,MEMF_CLEAR);
    if (!buf) return(NULL);
    buf2[1]=0;
    for (a=0;a<num;a++) {
        switch (functype[a]) {
            case FT_INTERNAL: typec=FC_INTERNAL; break;
            case FT_WORKBENCH: typec=FC_WORKBENCH; break;
            case FT_BATCH: typec=FC_BATCH; break;
            case FT_AREXX: typec=FC_AREXX; break;
            case FT_REQUESTER: typec=FC_REQUESTER; break;
            default: typec=0; break;
        }
        if (typec) {
            buf2[0]=typec;
            strcat(buf,buf2);
        }
        strcat(buf,funclist[a]);
        if (a<(num-1)) strcat(buf,"||");
    }
    return(buf);
}

void freefunclist(funclist)
char **funclist;
{
    int a;

    for (a=0;a<MAXFUNCS;a++) {
        freestring(funclist[a]); funclist[a]=NULL;
    }
}

void docolourbox(r,x,y,fg,num)
struct RastPort *r;
int x,y,fg,num;
{
    int a,b,ac,dn,w,h,x1,y1;
    char *ptr;

    switch ((b=(1<<((screen_depth > 4) ? 4 : screen_depth)))) {
        case 4: ac=2; dn=2; break;
        case 8: ac=4; dn=2; break;
        case 16: ac=4; dn=4; break;
        default: return;
    }

    Do3DBox(r,x,y,84,22,screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    SetAPen(r,screen_pens[1].pen);

    a=strlen((ptr=(fg)?cfg_string[STR_FOREGROUND]:cfg_string[STR_BACKGROUND]));
    Move(r,x+42-(a*4),y-4);
    Text(r,ptr,a);

    w=80/ac; h=20/dn;
    x+=2; x1=x; y1=y+1;
    for (a=0;a<b;a++) {
        SetAPen(r,screen_pens[a].pen);
        RectFill(r,x1,y1,x1+w-1,y1+h-1);
        colour_box_xy[num][a][0]=x1;
        colour_box_xy[num][a][1]=y1;
        colour_box_xy[num][a][2]=x1+w-1;
        colour_box_xy[num][a][3]=y1+h-1;
        x1+=w;
        if ((x1-x)==80) {
            x1=x;
            y1+=h;
        }
    }
    colour_box_num=b;
}

void docoloursel(r,x,y,pen)
struct RastPort *r;
int x,y,pen;
{
    int a,ac,dn,w,h,x1,y1;

    a=1<<((screen_depth > 4) ? 4 : screen_depth);
    while (pen>=a && pen>-1) pen-=a;
    if (pen<0) return;

    switch (a) {
        case 4: ac=2; dn=2; break;
        case 8: ac=4; dn=2; break;
        case 16: ac=4; dn=4; break;
        default: return;
    }
    w=80/ac; h=20/dn;
    x+=2; x1=x; y1=y+1;
    for (a=0;a<pen;a++) {
        x1+=w;
        if ((x1-x)==80) {
            x1=x;
            y1+=h;
        }
    }
    drawcompbox(r,x1,y1,w,h);
}

void drawcompbox(r,x1,y1,w,h)
struct RastPort *r;
int x1,y1,w,h;
{
    SetDrMd(r,COMPLEMENT);
    Move(r,x1+2,y1+1); Draw(r,x1+w-3,y1+1);
    Draw(r,x1+w-3,y1+h-2); Draw(r,x1+2,y1+h-2);
    Draw(r,x1+2,y1+2); Draw(r,x1+3,y1+2); Draw(r,x1+3,y1+h-3);
    Move(r,x1+w-4,y1+2); Draw(r,x1+w-4,y1+h-3);
    SetDrMd(r,JAM2);
}

void showkey(key,qual,x,y,w,h)
UWORD key,qual;
int x,y,w,h;
{
    char buf[60];
    int a,l;

    Do3DBox(rp,x,y,w,h,screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);
    SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,x,y,x+w-1,y+h-1);
    SetAPen(rp,screen_pens[1].pen);
//    if (key==0) key=255;
    if (RawkeyToStr(key,qual,buf,NULL,60)) {
        l=w/8;
        Move(rp,x+2,y+((h-8)/2)+6);
        a=strlen(buf);
        Text(rp,buf,(a<l)?a:l);
    }
}

void showfuncob(r,name,fp,bp,type,x,y)
struct RastPort *r;
char *name;
int fp,bp,type,x,y;
{
    int a,op,l;

    op=r->FgPen;
    if (type==CFG_GADGET || type==CFG_DRIVE)
        Do3DBox(r,x+10,y+12,124,8,screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    SetAPen(r,screen_pens[bp].pen);
    a=strlen(name);
    if (type==CFG_GADGET || type==CFG_DRIVE) {
        l = 120/rp->Font->tf_XSize;
        if (a>l) a=l;
        RectFill(r,x+10,y+12,x+133,y+19);
        x+=8+((128-(a*rp->Font->tf_XSize))/2);
    }
    else {
        if (a>14) a=14;
        RectFill(r,x+16,y+12,x+127,y+19);
        x+=16;
    }
    SetDrMd(r,JAM1);
    SetAPen(r,screen_pens[fp].pen);
    Move(r,x,y+18); Text(r,name,a);
    SetDrMd(r,JAM2);
    SetAPen(r,op);
}

void makeselflags(flags,buf)
int flags;
char *buf;
{
    int a;

    for (a=0;a<NUMFLAGS;a++) {
        if (flags&gadflagvals[a]) buf[a]|=LVARRAY_SELECTED;
        else buf[a]&=~LVARRAY_SELECTED;
    }
}

int getselflags(buf)
char *buf;
{
    int a,flags=0;

    for (a=0;a<NUMFLAGS;a++) if (buf[a]&LVARRAY_SELECTED) flags|=gadflagvals[a];
    return(flags);
}

void checkmutflags(flags,sel)
char *flags;
int sel;
{
    switch (sel) {
        case SFB_OUTPUTWIN:
            if (flags[SFB_RUNASYNC]&LVARRAY_SELECTED) flags[SFB_WB2F]|=LVARRAY_SELECTED;
            flags[SFB_OUTPUTFILE]&=~LVARRAY_SELECTED;
            if (!(flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED) && flags[SFB_RUNASYNC]&LVARRAY_SELECTED) {
                flags[SFB_WB2F]&=~LVARRAY_SELECTED;
                flags[SFB_DOPUS2F]&=~LVARRAY_SELECTED;
            }
            if (flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED && flags[SFB_AUTOICONIFY]&LVARRAY_SELECTED)
                flags[SFB_WB2F]|=LVARRAY_SELECTED;
            break;
        case SFB_OUTPUTFILE:
            if (flags[SFB_OUTPUTFILE]&LVARRAY_SELECTED) {
                flags[SFB_OUTPUTWIN]&=~LVARRAY_SELECTED;
                if (flags[SFB_RUNASYNC]&LVARRAY_SELECTED) {
                    flags[SFB_WB2F]&=~LVARRAY_SELECTED;
                    flags[SFB_DOPUS2F]&=~LVARRAY_SELECTED;
                }
            }
            flags[SFB_RUNASYNC]&=~LVARRAY_SELECTED;
            flags[SFB_AUTOICONIFY]&=~LVARRAY_SELECTED;
            break;
        case SFB_WB2F:
            if (!(flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED) && flags[SFB_RUNASYNC]&LVARRAY_SELECTED)
                flags[SFB_DOPUS2F]&=~LVARRAY_SELECTED;
            else if (flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED) {
                if (!(flags[SFB_WB2F]&LVARRAY_SELECTED)) flags[SFB_RUNASYNC]&=~LVARRAY_SELECTED;
                flags[SFB_AUTOICONIFY]&=~LVARRAY_SELECTED;
            }
            break;
        case SFB_DOPUS2F:
            if (!(flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED)) {
                if (flags[SFB_RUNASYNC]&LVARRAY_SELECTED) {
                    flags[SFB_WB2F]&=~LVARRAY_SELECTED;
                    flags[SFB_DOPUS2F]&=~LVARRAY_SELECTED;
                }
                else if (flags[SFB_DOPUS2F]&LVARRAY_SELECTED)
                    flags[SFB_RUNASYNC]&=~LVARRAY_SELECTED;
            }
            break;
        case SFB_RUNASYNC:
            if (flags[SFB_OUTPUTFILE]&LVARRAY_SELECTED) {
                flags[SFB_OUTPUTFILE]&=~LVARRAY_SELECTED;
                flags[SFB_OUTPUTWIN]|=LVARRAY_SELECTED;
            }
            if (flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED) flags[SFB_WB2F]|=LVARRAY_SELECTED;
            flags[SFB_RELOADFILE]&=~LVARRAY_SELECTED;
            if (flags[SFB_RUNASYNC]&LVARRAY_SELECTED && !(flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED)) {
                flags[SFB_WB2F]&=~LVARRAY_SELECTED;
                flags[SFB_DOPUS2F]&=~LVARRAY_SELECTED;
            }
            if (!(flags[SFB_RUNASYNC]&LVARRAY_SELECTED))
                flags[SFB_AUTOICONIFY]&=~LVARRAY_SELECTED;
            flags[SFB_OUTPUTFILE]&=~LVARRAY_SELECTED;
            flags[SFB_RESCANSOURCE]&=~LVARRAY_SELECTED;
            flags[SFB_RESCANDEST]&=~LVARRAY_SELECTED;
            break;
        case SFB_CDSOURCE:
            flags[SFB_CDDEST]&=~LVARRAY_SELECTED;
            break;
        case SFB_CDDEST:
            flags[SFB_CDSOURCE]&=~LVARRAY_SELECTED;
            break;
        case SFB_RELOADFILE:
            flags[SFB_RUNASYNC]&=~LVARRAY_SELECTED;
            flags[SFB_AUTOICONIFY]&=~LVARRAY_SELECTED;
            break;
        case SFB_AUTOICONIFY:
            if (!(flags[SFB_RUNASYNC]&LVARRAY_SELECTED)) flags[SFB_RUNASYNC]|=LVARRAY_SELECTED;
            if (flags[SFB_OUTPUTFILE]&LVARRAY_SELECTED) flags[SFB_OUTPUTWIN]|=LVARRAY_SELECTED;
            if (flags[SFB_OUTPUTWIN]&LVARRAY_SELECTED) flags[SFB_WB2F]|=LVARRAY_SELECTED;
            flags[SFB_OUTPUTFILE]&=~LVARRAY_SELECTED;
            flags[SFB_RESCANSOURCE]&=~LVARRAY_SELECTED;
            flags[SFB_RESCANDEST]&=~LVARRAY_SELECTED;
            flags[SFB_RELOADFILE]&=~LVARRAY_SELECTED;
            break;
        case SFB_RESCANSOURCE:
        case SFB_RESCANDEST:
            flags[SFB_RUNASYNC]&=~LVARRAY_SELECTED;
            flags[SFB_AUTOICONIFY]&=~LVARRAY_SELECTED;
            break;
    }
}

void editgadsoff()
{
    int a;

    SetAPen(rp,screen_pens[1].pen);
    DoCycleGadget(&editfuncgadgets[9],Window,functypestr,NUM_TYPES);
    for (a=2;a<6;a++) {
        if (a>2 || editlists[0].count>=MAXFUNCS)
            DisableGadget(&editfuncgadgets[a],rp,4,2);
    }
    for (a=9;a<13;a++) DisableGadget(&editfuncgadgets[a],rp,(a==10 || a==12)?3:((a==9)?4:0),(a==11)?0:2);
}

void editgadson(sel)
int sel;
{
    int a;

    for (a=2;a<6;a++) {
        if ((a==3 || a==4) && !sel) continue;
        if ((a==2 || a==3) && editlists[0].count>=MAXFUNCS) continue;
        EnableGadget(&editfuncgadgets[a],rp,4,2);
    }
    for (a=9;a<13;a++)
        EnableGadget(&editfuncgadgets[a],rp,(a==10 || a==12)?3:((a==9)?4:0),(a==11)?0:2);
}

void deleteentry(funclist,functype,displist,selitem)
char **funclist,*functype,**displist;
int selitem;
{
    int a;

    if (selitem>-1) {
        if (selitem<MAXFUNCS-1) {
            CopyMem(&funclist[selitem+1],&funclist[selitem],(MAXFUNCS-1-selitem)*4);
            CopyMem(&functype[selitem+1],&functype[selitem],(MAXFUNCS-1-selitem));
            for (a=selitem;a<MAXFUNCS-1;a++)
                makedispfunc(funclist[a],functype[a],displist[a]);
        }
        funclist[MAXFUNCS-1]=NULL;
        displist[MAXFUNCS-1][0]=0;
    }
}

void endedit()
{
    checkswapgad();
    edit_funcbuf[0]=0;
    RefreshStrGad(&editfuncgadgets[11],Window);
    editlists[0].itemselected=-1;
    RefreshListView(&editlists[0],1);
    editgadson(1);
    editgadsoff();
}

void setupforedit(type,act)
int type,act;
{
    checkswapgad();
    RefreshListView(&editlists[0],1);
    SetAPen(rp,screen_pens[1].pen);
    DoCycleGadget(&editfuncgadgets[9],Window,functypestr,type);
    RefreshStrGad(&editfuncgadgets[11],Window);
    if (act) ActivateStrGad(&editfuncgadgets[11],Window);
}

void checkswapgad()
{
    if (editfuncgadgets[4].Flags&SELECTED) {
        editfuncgadgets[4].Flags&=~SELECTED;
        select_gadget(&editfuncgadgets[4],0);
    }
}

int funcrequester(type,buf,title)
int type;
char *buf,*title;
{
    struct Window *wind;
    ULONG class;
    UWORD gadgetid;
    struct DOpusListView *view;
    char *ptr;
    int ret=0;

    switch (type) {
        case FT_INTERNAL:
        case FREQ_ARGREQ:
        case FREQ_FILETYPE:
        case FREQ_FILECLASS:
            setup_list_window(&requestwin,&cmdlist,&cmdcancelgad,1);
            requestwin.Title=title;
            if (!(wind=openwindow(&requestwin))) return(0);
            setuplist(&cmdlist,-1,-1);
            switch (type) {
                case FREQ_FILETYPE:
                    cmdlist.items=classopslist;
                    break;
                case FREQ_FILECLASS:
                    cmdlist.items=fileclasslist;
                    break;
                case FREQ_ARGREQ:
                    cmdlist.items=arglist;
                    break;
                case FT_INTERNAL:
                    cmdlist.items=commandlist;
                    break;
            }
            cmdlist.window=wind;
            if (!(AddListView(&cmdlist,1))) {
                CloseWindow(wind);
                return(0);
            }
            SetAPen(wind->RPort,screen_pens[1].pen);
            AddGadgets(wind,
                &cmdcancelgad,
                &editfuncgads[4],
                1,
                screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);

            busy();

            FOREVER {
                Wait(1<<wind->UserPort->mp_SigBit);
                while (IMsg=(struct IntuiMessage *)GetMsg(wind->UserPort)) {
                    if ((view=ListViewIDCMP(&cmdlist,IMsg))==(struct DOpusListView *)-1) {
                        class=IMsg->Class;
                        if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN)
                            gadgetid=((struct Gadget *)IMsg->IAddress)->GadgetID;
                        ReplyMsg((struct Message *)IMsg);
                        if (class==IDCMP_GADGETUP && gadgetid==0) {
                            RemoveListView(&cmdlist,1);
                            CloseWindow(wind);
                            unbusy();
                            return(0);
                        }
                    }
                    else if (view) {
                        if (buf) {
                            strcpy(buf,cmdlist.items[view->itemselected]);
                            ptr=buf;
                            while (*(ptr)) {
                                if (*(ptr)==' ') {
                                    *ptr=0;
                                    break;
                                }
                                ++ptr;
                            }
                        }
                        RemoveListView(&cmdlist,1);
                        CloseWindow(wind);
                        unbusy();
                        return(view->itemselected+1);
                    }
                }
            }
            break;

        default:
            filereq.flags=0;
            filereq.title=cfg_string[STR_SELECT_DESIRED_FILE];

            switch (type) {
                case FT_EXECUTABLE:
                case FT_WORKBENCH:
                case FT_BATCH:
                case FT_AREXX:
                    filebuf[0]=0;
                    if (CheckExist(defdir[type],NULL)) strcpy(dirbuf,defdir[type]);
                    else dirbuf[0]=0;
                    break;
                case FREQ_GENERIC:
                    strcpy(dirbuf,buf);
                    if ((ptr=BaseName(dirbuf))>dirbuf) {
                        strcpy(filebuf,ptr);
                        *ptr=0;
                    }
                    else {
                        LStrnCpy(filebuf,buf,FILEBUF_SIZE-1);
                        dirbuf[0]=0;
                    }
                    break;
                case FREQ_PATHREQ:
                    filereq.flags=DFRF_DIRREQ;
                    filereq.title=cfg_string[STR_SELECT_DESIRED_DIRECTORY];
                    break;
            }

            filereq.window=Window;
            if (!dirbuf[0]) {
                BPTR lock;

                if (lock=Lock("",ACCESS_READ)) {
                    PathName(lock,dirbuf,256);
                    UnLock(lock);
                }
            }
            busy();
            if (FileRequest(&filereq)) {
                strcpy(buf,dirbuf);
                TackOn(buf,filebuf,256);
                ret=1;
            }
            unbusy();
            break;
    }
    return(ret);
}

void makenewentry(funclist,functype,displist,selitem)
char **funclist,*functype,**displist;
int selitem;
{
    funclist[selitem]=getcopy(edit_funcbuf,-1,NULL);
    if (displist) {
        makedispfunc(funclist[selitem],functype[selitem],displist[selitem]);
        if (selitem>=editlists[0].count && editlists[0].topitem>=editlists[0].count-5)
            ++editlists[0].topitem;
    }
}

void setupcolourbox(r,x,y,fp,bp)
struct RastPort *r;
int x,y,fp,bp;
{
    docolourbox(r,x,y,1,0);
    docoloursel(r,x,y,fp);
    docolourbox(r,x+248,y,0,1);
    docoloursel(r,x+248,y,bp);
    Do3DBox(r,x+94,y-10,144,32,screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);
}

void getfiletypefunc(func,ftype,type)
struct dopusfunction *func;
struct dopusfiletype *ftype;
int type;
{
    type=ftype_funcmap[type];

    func->which=ftype->which[type];
    func->stack=ftype->stack[type];
    func->pri=ftype->pri[type];
    func->delay=ftype->delay[type];
    freestring(func->function);
    func->function=getcopy(ftype->function[type],-1,NULL);
    strcpy(edit_actionbuf,ftype->actionstring[type]);
}

void makefiletypefunc(func,ftype,type)
struct dopusfunction *func;
struct dopusfiletype *ftype;
int type;
{
    type=ftype_funcmap[type];

    ftype->which[type]=func->which;
    ftype->stack[type]=func->stack;
    ftype->pri[type]=func->pri;
    ftype->delay[type]=func->delay;
    freestring(ftype->function[type]);
    ftype->function[type]=getcopy(func->function,-1,NULL);
    strcpy(ftype->actionstring[type],edit_actionbuf);
}

void fixeditfunction(func,flagsel,funclist,functype,selitem,type,data,curtype)
struct dopusfunction *func;
char *flagsel,**funclist,*functype;
int selitem,type;
APTR data;
int curtype;
{
    struct dopusfiletype *ftype;
    struct dopushotkey *hotkey;

    if (type==CFG_FILETYPE) ftype=(struct dopusfiletype *)data;
    else if (type==CFG_HOTKEYS) hotkey=(struct dopushotkey *)data;

    if (selitem>-1 && edit_funcbuf[0]) {
        freestring(funclist[selitem]); funclist[selitem]=NULL;
        makenewentry(funclist,functype,NULL,selitem);
    }
    freefunction(func);
    func->function=compilefunclist(funclist,functype,NULL);
    func->which=getselflags(flagsel);
    func->stack=atoi(edit_stackbuf);
#ifdef __AROS__
    if (func->stack<AROS_STACKSIZE) func->stack=AROS_STACKSIZE;
#else
    if (func->stack<4000) func->stack=4000;
#endif
    func->pri=atoi(edit_prioritybuf);
//    if (func->pri<-127) func->pri=-127;
//    else if (func->pri>127) func->pri=127;
    func->delay=atoi(edit_delaybuf);
    if (func->delay<-1) func->delay=-1;
    else if (func->delay>60) func->delay=60;
    if (type==CFG_FILETYPE) {
        makefiletypefunc(func,ftype,curtype);
        fixfunctypelist(ftype);
        RefreshListView(&editlists[2],1);
    }
    else if (type==CFG_HOTKEYS) strcpy(hotkey->name,edit_namebuf);
    else {
        freestring(((struct newdopusfunction *)func)->name);
        ((struct newdopusfunction *)func)->name=getcopy(edit_namebuf,-1,0);
    }
}

void setupeditdisplay(func,type,data,curtype,displist,funclist,functype,flagsel,add)
struct dopusfunction *func;
int type;
APTR data;
int curtype;
char **displist,**funclist,*functype,*flagsel;
int add;
{
    int a;
    struct dopusfiletype *ftype;

    if (type==CFG_FILETYPE) ftype=(struct dopusfiletype *)data;

    for (a=0;a<2;a++) {
        setuplist(&editlists[a],130,69+(a*60));
        editlists[a].itemselected=-1;
        editlists[a].topitem=0;
    }
    if (type==CFG_FILETYPE) {
        setuplist(&editlists[2],173,4);
        getfiletypefunc(func,ftype,curtype);
    }
    else if (type==CFG_HOTKEYS) editlists[1].topitem=5;
    makefunclist(func->function,funclist,functype);
    for (a=0;a<MAXFUNCS;a++) makedispfunc(funclist[a],functype[a],displist[a]);
    displist[MAXFUNCS]=funclist[MAXFUNCS]=NULL;
    editlists[0].items=displist;
    makeselflags(func->which,flagsel);
    editlists[1].selectarray=flagsel;
    if (add) {
        AddListView(editlists,3);
        initsidegads(editfuncgads,0,0);
    }
    else RefreshListView(editlists,3);
    edit_funcbuf[0]=0;
    lsprintf(edit_stackbuf,"%ld",(long int)func->stack);
    lsprintf(edit_prioritybuf,"%ld",(long int)func->pri);
    lsprintf(edit_delaybuf,"%ld",(long int)func->delay);
    RefreshGList(&editfuncgadgets[6],Window,NULL,3);
    if (type==CFG_FILETYPE) RefreshGList(&editfuncgadgets[13],Window,NULL,1);
}

void erasefunction(func,funclist,displist,flagsel)
struct dopusfunction *func;
char **funclist,**displist,*flagsel;
{
    int a;

    freefunclist(funclist);
    for (a=0;a<MAXFUNCS;a++) displist[a][0]=0;
    strcpy(edit_stackbuf,"4000");
    edit_prioritybuf[0]='0'; edit_prioritybuf[1]=0;
    edit_delaybuf[0]='2'; edit_delaybuf[1]=0;
    edit_namebuf[0]=0;
    if (func) {
        func->key=0; func->qual=0;
    }
    makeselflags(0,flagsel);
}

int fixfunctypelist(ftype)
struct dopusfiletype *ftype;
{
    int a,type=-1,b;

    for (a=0;a<FILETYPE_FUNCNUM;a++) {
        if (!functypelist[a]) break;
        b=ftype_funcmap[a];
        if (ftype->function[b] && ftype->function[b][0]) {
            if (type==-1) type=a;
            functypelist[a][1]='*';
        }
        else functypelist[a][1]=' ';
    }
    if (type==-1) type=0;
    return(type);
}

void add_appobject(type)
int type;
{
    if (WorkbenchBase && appport) {
        if (Screen && type) {
            appobject=
                (void *)AddAppIconA(MY_APPOBJECT,0,
                    cfg_string[STR_DROP_A_TOOL_HERE],
                    appport,
                    NULL,dropboxicon,NULL);
        }
        else if (!Screen) {
            appobject=
                (void *)AddAppWindowA(MY_APPOBJECT,0,
                    Window,appport,NULL);
        }
    }
}

void rem_appobject(type)
int type;
{
    if (WorkbenchBase && appobject) {
        if (Screen && type) RemoveAppIcon(appobject);
        else if (!Screen) RemoveAppWindow(appobject);
        while (appmsg=(struct AppMessage *)GetMsg(appport))
            ReplyMsg((struct Message *)appmsg);
    }
    appobject=NULL;
}

void do_gad_label(txt,x,y)
char *txt;
int x,y;
{
    char buf[140];
    int len,a,uspos=-1;

    for (len=0,a=0;txt[len] && a<139;len++) {
        if (txt[len]=='_' && uspos==-1) {
            uspos=a;
            continue;
        }
        buf[a++]=txt[len];
    }
    buf[a]=0;

    x-=a*8;
    UScoreText(rp,buf,x,y,uspos);
}
