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

static struct dopusgadgetbanks *currentbank,*selbank;

int dogadgetconfig()
{
    ULONG class;
    UWORD code,gadgetid;
    struct ConfigUndo *undo;
    struct DOpusRemember *gadkey=NULL;
    struct dopusgadgetbanks *bank,*bank2;
    struct newdopusfunction *secondgad;
    struct Gadget *gad;
    struct Border *unselborder,*selborder;
    char buf[80];
    int a,b,x,y,mode=-1;

    if (!(gadgads=(struct Gadget *)LAllocRemember(&gadkey,sizeof(struct Gadget)*GADCOUNT,
        MEMF_CLEAR))) return(0);

    CreateGadgetBorders(&gadkey,
        80,10,
        &selborder,&unselborder,
        0,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);

    selgad=NULL; selbank=NULL; seligad=NULL;

    x=x_off+38; y=y_off+4;
    for (a=0;a<GADCOUNT;a++) {
        gadgads[a].NextGadget=(a<GADCOUNT-1)?&gadgads[a+1]:NULL;
        gadgads[a].LeftEdge=x;
        gadgads[a].TopEdge=y;
        gadgads[a].Width=80;
        gadgads[a].Height=10;
        gadgads[a].Flags=(selborder)?GFLG_GADGHIMAGE:GFLG_GADGHCOMP;
        gadgads[a].Activation=GACT_RELVERIFY;
        gadgads[a].GadgetType=GTYP_BOOLGADGET;
        gadgads[a].GadgetID=a+100;
        gadgads[a].GadgetRender=unselborder;
        gadgads[a].SelectRender=selborder;
        if ((x+=80)>=x_off+598) {
            x=x_off+38;
            if ((y+=10)==y_off+64) y+=14;
        }
    }

    paint_state=0;
    showconfigscreen(CFG_GADGET);
    ClearMenuStrip(Window);
    projectmenu.NextMenu=&gadrowsmenu;
    neatstuffitem.Flags&=~CHECKED;
    neatstuffmenu.LeftEdge=272;

    for (a=0;a<5;a++) {
        if (config->gadgetrows==rowtrans[a]) gadrowsitems[a].Flags|=CHECKED;
        else gadrowsitems[a].Flags&=~CHECKED;
    }
    FSSetMenuStrip(Window,&projectmenu);
    undo=makeundo(UNDO_GADGET);
    currentbank=firstbank;
    showgadgets(currentbank,1);
    nextbankrmb.x=maingad[0].LeftEdge;
    nextbankrmb.y=maingad[0].TopEdge;
    insertbankrmb.x=maingad[1].LeftEdge;
    insertbankrmb.y=maingad[1].TopEdge;
    fixrmbimage(&nextbankrmb,maingad,1,-1,-1);

    FOREVER {
        Wait(1<<Window->UserPort->mp_SigBit);
        while (IMsg=getintuimsg()) {
            class=IMsg->Class;
            code=IMsg->Code;
            x=IMsg->MouseX;
            y=IMsg->MouseY;
            if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN) {
                gad=(struct Gadget *)IMsg->IAddress;
                gadgetid=gad->GadgetID;
            }
            ReplyMsg((struct Message *)IMsg);
            switch (class) {
                case IDCMP_MENUPICK:
                    if (code==MENUNULL) break;
                    switch (MENUNUM(code)) {
                        case 0:
                            if (selgad) {
                                if (selbank==currentbank) select_gadget(seligad,0);
                                selbank=NULL; seligad=NULL; selgad=NULL;
                            }
                            if (mode>-1) {
                                select_gadget(&maingad[mode],0);
                                mode=-1;
                            }
                            dogadgetinfo(NULL);
                            switch ((a=ITEMNUM(code))) {
                                case 0:
                                case 1:
                                    if (doload(CFG_GADGET,a)) {
                                        if (bank=firstbank) {
                                            while (bank->next) bank=bank->next;
                                            currentbank=bank;
                                            showgadgets(currentbank,1);
                                            if (a) dogadgetinfo(cfg_string[STR_NEW_GADGET_BANKS_APPENDED]);
                                            else dogadgetinfo(cfg_string[STR_DEFAULT_GADGET_BANKS_APPENDED]);
                                        }
                                    }
                                    break;
                            }
                            break;
                        case 1:
                            config->gadgetrows=rowtrans[ITEMNUM(code)];
                            break;
                        case 2:
                            if (ITEMNUM(code)==0) {
                                if (neatstuffitem.Flags&CHECKED) {
                                    if (!(paint_state=get_paint_colours(&paint_fg,&paint_bg,CFG_GADGET))) {
                                        ClearMenuStrip(Window);
                                        neatstuffitem.Flags&=~CHECKED;
                                        FSSetMenuStrip(Window,&projectmenu);
                                    }
                                }
                                else paint_state=0;
                                showgadgets(currentbank,0);
                            }
                            break;
                    }
                    break;

                case IDCMP_MOUSEBUTTONS:
                    if (code==MENUDOWN) {
                        fixrmbimage(&nextbankrmb,maingad,0,x,y);
                        if ((a=DoRMBGadget(&nextbankrmb,Window))==0) {
                            if (currentbank) {
                                if (mode==-1) dogadgetinfo(NULL);
                                if (selgad && selbank==currentbank) select_gadget(seligad,0);
                                bank2=currentbank;
                                if (!(bank=lastbank(currentbank))) {
                                    bank=firstbank;
                                    while (bank && bank->next) bank=bank->next;
                                }
                                currentbank=bank;
                                if (currentbank!=bank2) showgadgets(currentbank,1);
                                if (selgad && selbank==currentbank) select_gadget(seligad,1);
                            }
                            else dogadgetinfo(cfg_string[STR_NO_GADGET_BANKS_CONFIGURED]);
                        }
                        else if (a==1) makenewbank(1);
                        fixrmbimage(&nextbankrmb,maingad,1,-1,-1);
                    }
                    break;

                case IDCMP_GADGETUP:
                    if (gadgetid<GAD_OKAY) {
                        if (selgad) {
                            if (gadgetid!=GAD_NEXTBANK) selgad=NULL;
                            if (selbank==currentbank) select_gadget(seligad,0);
                            if (gadgetid!=GAD_NEXTBANK) {
                                selbank=NULL; seligad=NULL;
                            }
                        }
                        if (mode==gadgetid) {
                            mode=-1;
                            dogadgetinfo(NULL);
                            break;
                        }
                        if (gadgetid!=GAD_NEXTBANK && mode>-1) {
                            select_gadget(&maingad[mode],0);
                            mode=-1;
                        }
                        if (gadgetid!=GAD_NEWBANK && !firstbank) {
                            dogadgetinfo(cfg_string[STR_NO_GADGET_BANKS_CONFIGURED]);
                            if (gad->Activation&GACT_TOGGLESELECT) select_gadget(gad,0);
                            break;
                        }
                    }
                    else if (gadgetid>=100) {
                        if (!firstbank) {
                            dogadgetinfo(cfg_string[STR_NO_GADGET_BANKS_CONFIGURED]);
                            break;
                        }
                        a=gadgetid-100;
                        if (mode>-1) {
                            if (mode==GAD_DELETEGADGET) {
                                selgad=&(currentbank->gadgets[a]);
                                clearmenu(selgad);
                                selgad->fpen=1; selgad->bpen=0;
                                freenewfunction(selgad);
                                selgad=NULL;
                                dogadgetinfo(cfg_string[STR_GADGET_DELETED]);
                                showgadgetname(selgad,gad);
                            }
                            else if (!selgad) {
                                selgad=&(currentbank->gadgets[a]);
                                seligad=gad;
                                selbank=currentbank;
                                select_gadget(gad,1);
                                if (mode==GAD_COPYGADGET)
                                    dogadgetinfo(cfg_string[STR_SELECT_GADGET_TO_COPY]);
                                else if (mode==GAD_SWAPGADGET)
                                    dogadgetinfo(cfg_string[STR_SELECT_GADGET_TO_SWAP]);
                            }
                            else {
                                secondgad=&(currentbank->gadgets[a]);
                                if (selgad==secondgad) select_gadget(gad,0);
                                else {
                                    if (mode==GAD_COPYGADGET) {
                                        freenewfunction(secondgad);
                                        CopyMem((char *)selgad,(char *)secondgad,sizeof(struct newdopusfunction));
                                        secondgad->name=getcopy(selgad->name,-1,NULL);
                                        secondgad->function=getcopy(selgad->function,-1,NULL);
                                    }
                                    else if (mode==GAD_SWAPGADGET)
                                        SwapMem((char *)selgad,(char *)secondgad,sizeof(struct dopusfunction));
                                    showgadgetname(secondgad,gad);
                                    if (selbank==currentbank) {
                                        select_gadget(seligad,0);
                                        showgadgetname(selgad,seligad);
                                    }
                                }
                                selgad=NULL; seligad=NULL; selbank=NULL;
                                if (mode==GAD_COPYGADGET)
                                    dogadgetinfo(cfg_string[STR_SELECT_A_GADGET_TO_COPY]);
                                else if (mode==GAD_SWAPGADGET)
                                    dogadgetinfo(cfg_string[STR_SELECT_FIRST_GADGET_TO_SWAP]);
                            }
                        }
                        else {
                            if (paint_state) {
                                currentbank->gadgets[a].fpen=paint_fg;
                                currentbank->gadgets[a].bpen=paint_bg;
                                showgadgetname(&currentbank->gadgets[a],&gadgads[a]);
                            }
                            else {
                                cleanconfigscreen();
                                selgad=&(currentbank->gadgets[a]);
                                doscreentitle(cfg_string[STR_GADGET_EDIT_SCREEN]);
                                editfunction((struct dopusfunction *)selgad,CFG_GADGET,NULL);
                                showconfigscreen(CFG_GADGET);
                                fixrmbimage(&nextbankrmb,maingad,1,-1,-1);
                                selgad=NULL;
                                showgadgets(currentbank,1);
                            }
                        }
                        break;
                    }
                    switch (gadgetid) {
                        case GAD_NEXTBANK:
                            bank2=currentbank;
                            if (!(currentbank=currentbank->next)) currentbank=firstbank;
                            if (currentbank!=bank2) showgadgets(currentbank,1);
                            if (mode==-1) dogadgetinfo(NULL);
                            break;
                        case GAD_NEWBANK:
                            makenewbank(0);
                            break;
                        case GAD_COPYBANK:
                            dogadgetinfo(NULL);
                            buf[0]=0;
                            if (!getstring(cfg_string[STR_ENTER_BANK_TO_COPY],buf,5,SRF_LONGINT|SRF_CENTJUST))
                                break;
                            a=atoi(buf)-1;
                            if (a<0) break;
                            bank2=currentbank;
                            bank=firstbank;
                            for (b=0;b<a;b++) if (!(bank=bank->next)) break;
                            if (bank) {
                                showgadgets(bank,1);
                                if (!(request(cfg_string[STR_BANK_ALREADY_EXISTS]))) {
                                    showgadgets(currentbank,1);
                                    break;
                                }
                                freegadgetbank(bank,0);
                                currentbank=bank;
                                dogadgetinfo(cfg_string[STR_GADGET_BANK_COPIED]);
                            }
                            else if (!makenewbank(0)) break;
                            CopyMem((char *)bank2,(char *)currentbank,sizeof(struct newdopusfunction)*GADCOUNT);
                            copygadgets(bank2,currentbank,NULL);
                            showgadgets(currentbank,1);
                            break;
                        case GAD_SWAPBANK:
                            dogadgetinfo(NULL);
                            buf[0]=0;
                            if (!getstring(cfg_string[STR_ENTER_BANK_TO_SWAP],buf,5,SRF_LONGINT|SRF_CENTJUST))
                                break;
                            a=atoi(buf)-1;
                            if (a<0) break;
                            bank=firstbank;
                            for (b=0;b<a;b++) if (!(bank=bank->next)) break;
                            if (!bank) {
                                dogadgetinfo(cfg_string[STR_BANK_DOES_NOT_EXIST]);
                                break;
                            }
                            if (bank==currentbank) break;
                            SwapMem((char *)bank,(char *)currentbank,sizeof(struct newdopusfunction)*GADCOUNT);
                            showgadgets(currentbank,1);
                            dogadgetinfo(cfg_string[STR_BANKS_SWAPPED]);
                            break;
                        case GAD_DELETEBANK:
                            dogadgetinfo(NULL);
                            if (currentbank==firstbank && !currentbank->next) {
                                if (!request(cfg_string[STR_ONLY_BANK_CONFIGURED])) break;
                            }
                            else if (!request(cfg_string[STR_REALLY_DELETE_BANK]))
                                break;
                            if (bank=lastbank(currentbank)) bank->next=currentbank->next;
                            if (firstbank==currentbank) firstbank=currentbank->next;
                            freegadgetbank(currentbank,1);
                            if (bank && bank->next) currentbank=bank->next;
                            else if (bank) currentbank=bank;
                            else currentbank=firstbank;
                            dogadgetinfo(cfg_string[STR_BANK_DELETED]);
                            showgadgets(currentbank,1);
                            break;
                        case GAD_COPYGADGET:
                            mode=GAD_COPYGADGET;
                            dogadgetinfo(cfg_string[STR_SELECT_A_GADGET_TO_COPY]);
                            break;
                        case GAD_SWAPGADGET:
                            mode=GAD_SWAPGADGET;
                            dogadgetinfo(cfg_string[STR_SELECT_FIRST_GADGET_TO_SWAP]);
                            break;
                        case GAD_DELETEGADGET:
                            mode=GAD_DELETEGADGET;
                            dogadgetinfo(cfg_string[STR_SELECT_GADGET_TO_DELETE]);
                            break;
                        case GAD_CANCEL:
                            doundo(undo,UNDO_GADGET);
                        case GAD_OKAY:
                            doundo(undo,0);
                            RemoveGList(Window,gadgads,GADCOUNT);
                            LFreeRemember(&gadkey); gadgads=NULL;
                            ClearMenuStrip(Window); projectmenu.NextMenu=NULL;
                            FSSetMenuStrip(Window,&projectmenu);
                            paint_state=0;
                            return((gadgetid==GAD_OKAY));
                    }
            }
        }
    }
}

void showgadgets(bank,show)
struct dopusgadgetbanks *bank;
int show;
{
    int a;
    struct dopusgadgetbanks *bank2;
    char buf[60],*ptr;

    if (show) {
        for (a=0;a<GADCOUNT;a++) {
            if (bank) {
                showgadgetname(&bank->gadgets[a],&gadgads[a]);
                if (selgad==&bank->gadgets[a]) select_gadget(&gadgads[a],1);
            }
            else showgadgetname(NULL,&gadgads[a]);
        }
    }
    if (bank) {
        bank2=firstbank; a=0;
        while (bank2 && bank2!=bank) {
            ++a;
            bank2=bank2->next;
        }
        lsprintf(buf,"%s %ld",cfg_string[STR_GADGET_BANK_NUMBER],(long int)(a+1));
        if (paint_state) strcat(buf,cfg_string[STR_PAINT_STATE]);
        ptr=buf;
    }
    else ptr=cfg_string[STR_GADGET_SCREEN_NO_BANKS];
    doscreentitle(ptr);
}

void showgadgetname(gad,gadget)
struct newdopusfunction *gad;
struct Gadget *gadget;
{
    int a,x,y,l;

    x=gadget->LeftEdge;
    y=gadget->TopEdge;
    l=(80-8)/rp->Font->tf_XSize;
    if (gad) SetAPen(rp,screen_pens[gad->bpen].pen);
    else SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,x+2,y+1,x+77,y+8);
    if (gad) {
        SetAPen(rp,screen_pens[gad->fpen].pen);
        SetBPen(rp,screen_pens[gad->bpen].pen);
        if (gad->name && gad->name[0]) {
            Move(rp,x+((80-(((a=strlen(gad->name))>l?(a=l):a)*rp->Font->tf_XSize))/2),y+7);
            Text(rp,gad->name,a);
        }
    }
    SetBPen(rp,screen_pens[0].pen);
}

int makenewbank(insert)
int insert;
{
    struct dopusgadgetbanks *bank,*bank2;
    int a;

    if (!(bank=AllocMem(sizeof(struct dopusgadgetbanks),MEMF_CLEAR))) {
        dogadgetinfo(cfg_string[STR_NO_MEMORY_FOR_NEW_BANK]);
        return(0);
    }
    for (a=0;a<GADCOUNT;a++) {
        bank->gadgets[a].stack=4000; bank->gadgets[a].delay=2;
        if (paint_state) {
            bank->gadgets[a].fpen=paint_fg;
            bank->gadgets[a].bpen=paint_bg;
        }
        else bank->gadgets[a].fpen=1;
    }
    if (insert && firstbank) {
        if (currentbank==firstbank) {
            bank->next=firstbank;
            firstbank=bank;
        }
        else {
            bank2=firstbank;
            while (bank2->next) {
                if (bank2->next==currentbank) break;
                bank2=bank2->next;
            }
            if (bank2->next==currentbank) {
                bank2->next=bank;
                bank->next=currentbank;
            }
            else bank2->next=bank;
        }
    }
    else {
        if (!firstbank) firstbank=bank;
        else {
            bank2=firstbank;
            while (bank2->next) bank2=bank2->next;
            bank2->next=bank;
        }
    }
    currentbank=bank;
    showgadgets(currentbank,1);
    dogadgetinfo(cfg_string[STR_NEW_BANK_CREATED]);
    return(1);
}

struct dopusgadgetbanks *lastbank(bank)
struct dopusgadgetbanks *bank;
{
    struct dopusgadgetbanks *temp;

    temp=firstbank;
    while (temp && temp->next!=bank) temp=temp->next;
    return(temp);
}

void dogadgetinfo(str)
char *str;
{
    int tx,ex,len,x;

    x=(Window->Width-580)/2;

    Do3DBox(rp,
        x,y_off+144,
        580,10,
        screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);
    SetDrMd(rp,JAM2);

    tx=x;
    if (str) {
        SetAPen(rp,screen_pens[1].pen);
        tx+=(580-((len=strlen(str))*8))/2;
        UScoreText(rp,str,tx,y_off+151,-1);
        ex=rp->cp_x;
    }
    else ex=x;
    SetAPen(rp,screen_pens[0].pen);
    if (tx>x) RectFill(rp,x,y_off+145,tx-1,y_off+153);
    if (ex<x+579) RectFill(rp,ex,y_off+145,x+579,y_off+153);
}

static struct newdopusfunction *selmenu;
static int menuoffset;

int domenuconfig()
{
    ULONG class;
    UWORD code,gadgetid;
    struct ConfigUndo *undo;
    struct Gadget *gad;
    struct DOpusRemember *menukey=NULL;
    struct newdopusfunction *secondmenu;
    int a,b,x,y,mode=-1,gid,mnum=-1,omnum;
    char buf[100];

    if (!(menugads=(struct Gadget *)LAllocRemember(&menukey,sizeof(struct Gadget)*
        (MENUCOUNT+5-(HIDDENLINES*5)),MEMF_CLEAR))) return(0);
    selgad=NULL; selbank=NULL;
    menuoffset=0;

    a=0; gid=100;
    for (x=0;x<5;x++,gid+=HIDDENLINES) {
        for (y=0;y<20-HIDDENLINES;y++,a++) {
            menugads[a].NextGadget=(a<MENUCOUNT+4-(HIDDENLINES*5))?&menugads[a+1]:NULL;
            menugads[a].LeftEdge=(x*120)+x_off+12;
            menugads[a].TopEdge=(y*8)+y_off+21;
            menugads[a].Width=112;
            menugads[a].Height=8;
            menugads[a].Flags=GADGHCOMP; menugads[a].Activation=RELVERIFY;
            menugads[a].GadgetType=BOOLGADGET;
            menugads[a].GadgetID=gid++;
        }
    }
    gid=200;
    for (x=0;x<5;x++,a++) {
        menugads[a].NextGadget=(x<4)?&menugads[a+1]:NULL;
        menugads[a].LeftEdge=(x*120)+x_off+12;
        menugads[a].TopEdge=y_off+8;
        menugads[a].Width=112;
        menugads[a].Height=8;
        menugads[a].Flags=GADGHCOMP; menugads[a].Activation=RELVERIFY;
        menugads[a].GadgetType=BOOLGADGET;
        menugads[a].GadgetID=gid++;
    }

    fix_slider(&menuslidergads[0]);
    AddGadgetBorders(&menukey,
        &menuslidergads[1],
        2,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);

    showconfigscreen(CFG_MENU);
    ClearMenuStrip(Window);
    projectmenu.NextMenu=&neatstuffmenu;
    neatstuffitem.Flags&=~CHECKED;
    neatstuffmenu.LeftEdge=140;
    FSSetMenuStrip(Window,&projectmenu);

    paint_state=0;
    undo=makeundo(UNDO_MENU);
    showmenus(7);

    FOREVER {
        Wait(1<<Window->UserPort->mp_SigBit);
        while (IMsg=getintuimsg()) {
            class=IMsg->Class; code=IMsg->Code;
            if (class==GADGETUP || class==GADGETDOWN) {
                gad=(struct Gadget *)IMsg->IAddress;
                gadgetid=gad->GadgetID;
            }
            ReplyMsg((struct Message *)IMsg);
            switch (class) {
                case IDCMP_MENUPICK:
                    if (code==MENUNULL) break;
                    switch (MENUNUM(code)) {
                        case 0:
                            if (selmenu) {
                                selmenu=NULL;
                                showmenus(5);
                            }
                            dogadgetinfo(NULL);
                            if (mode>-1) {
                                select_gadget(&maingad[mode],0);
                                mode=-1;
                            }
                            switch ((a=ITEMNUM(code))) {
                                case 0:
                                case 1:
                                    if (b = doload(CFG_MENU,a)) {
                                        showmenus(5);
                                        if (a) dogadgetinfo(cfg_string[(b==1)?STR_NEW_MENUS_LOADED:STR_MENU_MAX_REACHED]);
                                        else dogadgetinfo(cfg_string[(b==1)?STR_DEFAULT_MENUS_LOADED:STR_MENU_MAX_REACHED]);
                                    }
                                    break;
                            }
                            break;
                        case 1:
                            if (ITEMNUM(code)==0) {
                                if (neatstuffitem.Flags&CHECKED) {
                                    if (!(paint_state=get_paint_colours(&paint_fg,&paint_bg,CFG_MENU))) {
                                        ClearMenuStrip(Window);
                                        neatstuffitem.Flags&=~CHECKED;
                                        FSSetMenuStrip(Window,&projectmenu);
                                    }
                                }
                                else paint_state=0;
                                do_menuscreen_title(STR_MENU_SCREEN);
                            }
                            break;
                    }
                    break;

                case IDCMP_GADGETUP:
                    if (gadgetid<MENU_OKAY) {
                        if (selmenu) {
                            selmenu=NULL;
                            showmenus(5);
                        }
                        dogadgetinfo(NULL);
                        if (mode==gadgetid) {
                            mode=-1;
                            break;
                        }
                        if (mode>-1) {
                            select_gadget(&maingad[mode],0);
                            mode=-1;
                        }
                    }
                    if (gadgetid>199) {
                        if ((a=gadgetid-200)<0 || a>4) break;
                        if (mode==MENU_DELETEMENU || mode==MENU_SORTMENU ||
                            mode==MENU_COPYMENU || mode==MENU_SWAPMENU) {
                            gadgetid=100+(a*20);
                        }
                        else {
                            if (getstring(cfg_string[STR_ENTER_MENU_TITLE],config->menutit[a],14,SRF_CENTJUST))
                                showmenus(4);
                            break;
                        }
                    }
                    if (gadgetid>99 && gadgetid<200) {
                        a=(gadgetid-100)+menuoffset;
                        omnum=mnum; mnum=a/20;
                        if (mode>-1) {
                            switch (mode) {
                                case MENU_DELETEITEM:   
                                    clearmenu(&config->menu[a]);
                                    freenewfunction(&config->menu[a]);
                                    if (((a+1)%20)!=0) {
                                        x=((a+20)/20)*20-1;
                                        y=x-a;
                                        CopyMem((char *)&config->menu[a+1],&config->menu[a],y*sizeof(struct dopusfunction));
                                        clearmenu(&config->menu[x]);
                                        config->menu[x].function=NULL;
                                        config->menu[x].name=NULL;
                                    }
                                    showmenus(1);
                                    dogadgetinfo(cfg_string[STR_MENU_ITEM_DELETED]);
                                    break;
                                case MENU_INSERTITEM:
                                    x=((a+20)/20)*20-1;
                                    freenewfunction(&config->menu[x]);
                                    for (b=x-1;b>=a;b--)
                                        CopyMem((char *)&config->menu[b],&config->menu[b+1],sizeof(struct dopusfunction));
                                    clearmenu(&config->menu[a]);
                                    config->menu[a].name=NULL;
                                    config->menu[a].function=NULL;
                                    showmenus(1);
                                    dogadgetinfo(cfg_string[STR_MENU_ITEM_INSERTED]);
                                    break;
                                case MENU_DELETEMENU:
                                    lsprintf(buf,"%s\n%s",
                                        config->menutit[mnum],
                                        cfg_string[STR_REALLY_DELETE_THIS_MENU]);
                                    if (request(buf)) {
                                        b=(mnum*20)+20;
                                        for (a=mnum*20;a<b;a++) {
                                            clearmenu(&config->menu[a]);
                                            freenewfunction(&config->menu[a]);
                                        }
                                        config->menutit[mnum][0]=0;
                                        showmenus(5);
                                        dogadgetinfo(cfg_string[STR_MENU_DELETED]);
                                    }
                                    break;
                                case MENU_SORTMENU:
                                    sortmenustrip(mnum);
                                    showmenus(5);
                                    dogadgetinfo(cfg_string[STR_MENU_SORTED]);
                                    break;
                                default:
                                    if (!selmenu) {
                                        selmenu=&config->menu[a];
                                        HiliteGad(gad,rp);
                                        switch (mode) {
                                            case MENU_COPYMENU:
                                                dogadgetinfo(cfg_string[STR_SELECT_MENU_TO_COPY]);
                                                break;
                                            case MENU_SWAPMENU:
                                                dogadgetinfo(cfg_string[STR_SELECT_MENU_TO_SWAP]);
                                                break;
                                            case MENU_COPYITEM:
                                                dogadgetinfo(cfg_string[STR_SELECT_ITEM_TO_COPY]);
                                                break;
                                            case MENU_SWAPITEM:
                                                dogadgetinfo(cfg_string[STR_SELECT_ITEM_TO_SWAP]);
                                                break;
                                        }
                                    }
                                    else {
                                        secondmenu=&config->menu[a];
                                        if (selmenu==secondmenu) {
                                            HiliteGad(gad,rp);
                                            selmenu=NULL;
                                        }
                                        else {
                                            switch (mode) {
                                                case MENU_COPYITEM:
                                                    freenewfunction(secondmenu);
                                                    CopyMem((char *)selmenu,(char *)secondmenu,sizeof(struct newdopusfunction));
                                                    secondmenu->function=getcopy(selmenu->function,-1,NULL);
                                                    secondmenu->name=getcopy(selmenu->name,-1,NULL);
                                                    break;
                                                case MENU_SWAPITEM:
                                                    SwapMem((char *)selmenu,(char *)secondmenu,sizeof(struct newdopusfunction));
                                                    break;
                                                case MENU_COPYMENU:
                                                    if (omnum<0 || mnum==omnum) break;
                                                    CopyMem((char *)&config->menu[omnum*20],(char *)&config->menu[mnum*20],
                                                        sizeof(struct newdopusfunction)*20);
                                                    b=(mnum*20)+20;
                                                    for (a=mnum*20;a<b;a++) {
                                                        config->menu[a].name=getcopy(config->menu[a].name,-1,NULL);
                                                        config->menu[a].function=getcopy(config->menu[a].function,-1,NULL);
                                                    }
                                                    strcpy(config->menutit[mnum],config->menutit[omnum]);
                                                    break;
                                                case MENU_SWAPMENU:
                                                    if (omnum<0 || mnum==omnum) break;
                                                    SwapMem((char *)&config->menu[omnum*20],(char *)&config->menu[mnum*20],
                                                        sizeof(struct newdopusfunction)*20);
                                                    SwapMem(config->menutit[mnum],config->menutit[omnum],16);
                                                    break;
                                            }
                                            selmenu=NULL;
                                            showmenus(5);
                                        }
                                        doinitmenutext(mode);
                                    }
                                    break;
                            }
                        }
                        else {
                            if (paint_state) {
                                config->menu[a].fpen=paint_fg;
                                config->menu[a].bpen=paint_bg;
                                showmenus(1);
                            }
                            else {
                                cleanconfigscreen();
                                selmenu=&config->menu[a];
                                doscreentitle(cfg_string[STR_MENU_ITEM_EDIT_SCREEN]);
                                editfunction((struct dopusfunction *)selmenu,CFG_MENU,NULL);
                                showconfigscreen(CFG_MENU);
                                FixSliderPot(Window,&menuslidergads[0],menuoffset,20,20-HIDDENLINES,2);
                                selmenu=NULL;
                                showmenus(7);
                            }
                        }
                        break;
                    }
                    switch (gadgetid) {
                        case MENU_CANCEL:
                            doundo(undo,UNDO_MENU);
                        case MENU_OKAY:
                            doundo(undo,0);
                            RemoveGList(Window,menugads,MENUCOUNT);
                            LFreeRemember(&menukey); menugads=NULL;
                            ClearMenuStrip(Window); projectmenu.NextMenu=NULL;
                            FSSetMenuStrip(Window,&projectmenu);
                            paint_state=0;
                            return((gadgetid==MENU_OKAY));
                    }
                    if (gadgetid<MENU_OKAY) mode=doinitmenutext(gadgetid);
                    break;

                case IDCMP_GADGETDOWN:
                    switch (gadgetid) {
                        case MENU_SLIDER:
                            a=menuoffset;
                            menuoffset=GetSliderPos(&menuslidergads[0],20,20-HIDDENLINES);
                            if (a!=menuoffset) showmenus(1);
                            FOREVER {
                                Wait(1<<Window->UserPort->mp_SigBit);
                                while (IMsg=getintuimsg()) {
                                    class=IMsg->Class;
                                    ReplyMsg((struct Message *)IMsg);
                                    if (class==IDCMP_GADGETUP) break;
                                    if (class==IDCMP_MOUSEMOVE) {
                                        a=menuoffset;
                                        menuoffset=GetSliderPos(&menuslidergads[0],20,20-HIDDENLINES);
                                        if (a!=menuoffset) showmenus(1);
                                    }
                                }
                                if (class==IDCMP_GADGETUP) break;
                            }
                            ShowSlider(Window,&menuslidergads[0]);
                            break;
                        case MENU_MOVEUP:
                            menuoffset-=2;
                            if (menuoffset<-1) {
                                menuoffset=0;
                                break;
                            }
                        case MENU_MOVEDOWN:
                            ++menuoffset;
                            if (menuoffset>HIDDENLINES) {
                                menuoffset=HIDDENLINES;
                                break;
                            }
                            showmenus(1);
                            FixSliderPot(Window,&menuslidergads[0],menuoffset,20,20-HIDDENLINES,2);
                            Delay(5);
                            FOREVER {
                                while (IMsg=getintuimsg()) {
                                    class=IMsg->Class;
                                    ReplyMsg((struct Message *)IMsg);
                                    if (class==IDCMP_GADGETUP) break;
                                }
                                if (class==IDCMP_GADGETUP) break;
                                if (gadgetid==MENU_MOVEUP) {
                                    --menuoffset;
                                    if (menuoffset<0) {
                                        menuoffset=0;
                                        break;
                                    }
                                }
                                else {
                                    ++menuoffset;
                                    if (menuoffset>HIDDENLINES) {
                                        menuoffset=HIDDENLINES;
                                        break;
                                    }
                                }
                                FixSliderPot(Window,&menuslidergads[0],menuoffset,20,20-HIDDENLINES,2);
                                showmenus(1);
                            }
                            break;
                    }
                    break;
            }
        }
    }
}

void showmenus(disp)
int disp;
{
    int a,b,x,y;

    a=0;
    SetDrMd(rp,JAM2);
    for (x=0;x<5;x++) {
        if (disp&1) {
            b=(x*20)+menuoffset;
            for (y=0;y<20-HIDDENLINES;y++,a++,b++)
                showmenuname(&config->menu[b],&menugads[a]);
        }
        if (disp&2) {
            do3dbox((x*120)+x_off+12,y_off+21,112,160-(HIDDENLINES*8));
        }
        if (disp&4) {
            showmenutext(config->menutit[x],(x*120)+x_off+12,y_off+8,1,2);
            do3dbox((x*120)+x_off+12,y_off+8,112,8);
        }
    }
}

void showmenuname(menu,gadget)
struct newdopusfunction *menu;
struct Gadget *gadget;
{
    int x,y;

    x=gadget->LeftEdge;
    y=gadget->TopEdge;
    if (menu) {
        showmenutext(menu->name,x,y,menu->fpen,menu->bpen);
        if (menu==selmenu) HiliteGad(gadget,rp);
    }
    else {
        SetAPen(rp,screen_pens[1].pen);
        RectFill(rp,x,y,x+111,y+7);
    }
}

void showmenutext(txt,x,y,fp,bp)
char *txt;
int x,y,fp,bp;
{
    int a,l;

    l = (14*8)/rp->Font->tf_XSize;
    SetAPen(rp,screen_pens[fp].pen);
    SetBPen(rp,screen_pens[bp].pen);
    Move(rp,x,y+6);
    if (txt) {
        if ((a=strlen(txt))>0) {
            if (a>l) a=l;
            Text(rp,txt,a);
        }
    }
    else a=0;
    if (a<l) Text(rp,spacestring,l-a);
    SetAPen(rp,screen_pens[bp].pen);
    RectFill(rp,x+(l*rp->Font->tf_XSize),y,x+111,y+7);
    SetBPen(rp,screen_pens[0].pen);
}

void clearmenu(menu)
struct newdopusfunction *menu;
{
    menu->which=0;
    menu->stack=4000;
    menu->key=0;
    menu->qual=0;
    menu->type=0;
    menu->pri=0;
    menu->delay=0;
    if (paint_state) {
        menu->fpen=paint_fg;
        menu->bpen=paint_bg;
    }
    else {
        menu->fpen=1;
        menu->bpen=2;
    }
}

int doinitmenutext(id)
int id;
{
    int mode=-1;

    switch (id) {
        case MENU_COPYMENU:
            mode=MENU_COPYMENU;
            dogadgetinfo(cfg_string[STR_SELECT_ITEM_IN_MENU_TO_COPY]);
            break;
        case MENU_SWAPMENU:
            mode=MENU_SWAPMENU;
            dogadgetinfo(cfg_string[STR_SELECT_ITEM_IN_MENU_TO_SWAP]);
            break;
        case MENU_DELETEMENU:
            mode=MENU_DELETEMENU;
            dogadgetinfo(cfg_string[STR_SELECT_ITEM_IN_MENU_TO_DELETE]);
            break;
        case MENU_SORTMENU:
            mode=MENU_SORTMENU;
            dogadgetinfo(cfg_string[STR_SELECT_ITEM_IN_MENU_TO_SORT]);
            break;
        case MENU_INSERTITEM:
            mode=MENU_INSERTITEM;
            dogadgetinfo(cfg_string[STR_SELECT_ITEM_TO_INSERT_BEFORE]);
            break;
        case MENU_COPYITEM:
            mode=MENU_COPYITEM;
            dogadgetinfo(cfg_string[STR_SELECT_MENU_ITEM_TO_COPY]);
            break;
        case MENU_SWAPITEM:
            mode=MENU_SWAPITEM;
            dogadgetinfo(cfg_string[STR_SELECT_MENU_ITEM_TO_SWAP]);
            break;
        case MENU_DELETEITEM:
            mode=MENU_DELETEITEM;
            dogadgetinfo(cfg_string[STR_SELECT_MENU_ITEM_TO_DELETE]);
            break;
    }
    return(mode);
}

void sortmenustrip(m)
int m;
{
    int gap,i,j,k;
    char *ptr1,*ptr2,*zstr="\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f";

    m*=20;
    for (gap=10;gap>0;gap/=2)
        for (i=gap;i<20;i++)
            for (j=i-gap;j>=0;j-=gap) {
                k=j+gap;
                ptr1=(config->menu[j+m].name && config->menu[j+m].name[0])?config->menu[j+m].name:zstr;
                ptr2=(config->menu[k+m].name && config->menu[k+m].name[0])?config->menu[k+m].name:zstr;
                if (LStrCmpI(ptr1,ptr2)<=0) break;
                SwapMem((char *)&config->menu[j+m],(char *)&config->menu[k+m],
                    sizeof(struct newdopusfunction));
            }
}

void do_menuscreen_title(title)
int title;
{
    if (!paint_state) doscreentitle(cfg_string[title]);
    else {
        char temp[80];

        StrCombine(temp,cfg_string[title],cfg_string[STR_PAINT_STATE],80);
        doscreentitle(temp);
    }
}
