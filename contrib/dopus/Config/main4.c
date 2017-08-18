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

static struct dopusfunction *seldrive;

int dodriveconfig()
{
    ULONG class;
    UWORD code,gadgetid,qual;
    struct ConfigUndo *undo;
    struct DOpusRemember *gadkey;
    struct dopusfunction *seconddrive;
    struct Gadget *gad;
    struct Border *unselborder,*selborder;
    int a,b,x,y,mode=-1,editon=0,dnum=-1,odnum;
    char buf[256];
    unsigned char c;

    gadkey=NULL;
    if (!(drivegads=(struct Gadget *)LAllocRemember(&gadkey,sizeof(struct Gadget)*USEDRIVECOUNT,
        MEMF_CLEAR))) return(0);

    CreateGadgetBorders(&gadkey,
        80,10,
        &selborder,&unselborder,
        0,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);

    seldrive=NULL; seligad=NULL;

    x=x_off+76;
    y=y_off+2;
    for (a=0;a<USEDRIVECOUNT;a++) {
        drivegads[a].NextGadget=(a<USEDRIVECOUNT-1)?&drivegads[a+1]:NULL;
        drivegads[a].LeftEdge=x;
        drivegads[a].TopEdge=y;
        drivegads[a].Width=80;
        drivegads[a].Height=10;
        drivegads[a].Flags=(selborder)?GFLG_GADGHIMAGE:GFLG_GADGHCOMP;
        drivegads[a].Activation=GACT_RELVERIFY;
        drivegads[a].GadgetType=GTYP_BOOLGADGET;
        drivegads[a].GadgetID=a+100;
        drivegads[a].GadgetRender=unselborder;
        drivegads[a].SelectRender=selborder;
        if ((y+=10)>=y_off+62) {
            y=y_off+2;
            x+=100;
        }
    }

    namesinfo.MaxChars=16;
    showconfigscreen(CFG_DRIVE);

    paint_state=0;
    ClearMenuStrip(Window);
    projectmenu.NextMenu=&neatstuffmenu;
    neatstuffitem.Flags&=~CHECKED;
    neatstuffmenu.LeftEdge=140;
    FSSetMenuStrip(Window,&projectmenu);
    undo=makeundo(UNDO_DRIVE);

    showdrives();

    FOREVER {
        Wait(1<<Window->UserPort->mp_SigBit);
        while (IMsg=getintuimsg()) {
            class=IMsg->Class; code=IMsg->Code; x=IMsg->MouseX; y=IMsg->MouseY;
            qual=IMsg->Qualifier;
            if (class==GADGETUP || class==GADGETDOWN) {
                gad=(struct Gadget *)IMsg->IAddress;
                gadgetid=gad->GadgetID;
            }
            ReplyMsg((struct Message *)IMsg);
            switch (class) {
                case IDCMP_MENUPICK:
                    if (code==MENUNULL) break;
                    if (seldrive) {
                        select_gadget(seligad,0);
                        seligad=NULL; seldrive=NULL;
                    }
                    if (editon) {
                        driveeditoff();
                        editon=0;
                    }
                    if (mode>-1) {
                        select_gadget(&maingad[mode],0);
                        mode=-1;
                    }
                    dogadgetinfo(NULL);
                    switch (MENUNUM(code)) {
                        case 0:
                            switch ((a=ITEMNUM(code))) {
                                case 0:
                                case 1:
                                    if (doload(CFG_DRIVE,a)) {
                                        showdrives();
                                        if (a) dogadgetinfo(cfg_string[STR_NEW_DRIVE_BANKS_LOADED]);
                                        else dogadgetinfo(cfg_string[STR_DEFAULT_DRIVE_BANKS_RESET]);
                                    }
                                    break;
                            }
                            break;
                        case 1:
                            if (ITEMNUM(code)==0) {
                                if (neatstuffitem.Flags&CHECKED) {
                                    if (!(paint_state=get_paint_colours(&paint_fg,&paint_bg,CFG_GADGET))) {
                                        ClearMenuStrip(Window);
                                        neatstuffitem.Flags&=~CHECKED;
                                        FSSetMenuStrip(Window,&projectmenu);
                                    }
                                }
                                else paint_state=0;
                                do_menuscreen_title(STR_DRIVE_SCREEN);
                            }
                            break;
                    }
                    break;

                case IDCMP_RAWKEY:
                    if (!seldrive || !editon || (code>=0x60 && code<=0x67) || code&0x80)
                        break;
                    if (editdrivegadgets[1].Flags&SELECTED) {
                        qual&=IEQUALIFIER_LCOMMAND|IEQUALIFIER_RCOMMAND|
                            IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT|
                            IEQUALIFIER_LALT|IEQUALIFIER_RALT;
                        showkey(seldrive->key=code,seldrive->qual=qual,x_off+150,y_off+129,331,10);
                        break;
                    }
                    buf[0]=0;
                    RawkeyToStr(code,qual,NULL,buf,10);
                    c = LToLower(buf[0]);
                    if (c == getkeyshortcut(cfg_string[STR_EDIT_NAME]))
                            ActivateStrGad(&editdrivegadgets[0],Window);
                    else if (c == getkeyshortcut(cfg_string[STR_SHORTCUT_KEY]))
                     {
                            fixrmbimage(&sampleclearrmb,NULL,0,-1,-1);
                            editdrivegadgets[1].Flags|=SELECTED;
                            select_gadget(&editdrivegadgets[1],1);
                            fixrmbimage(&sampleclearrmb,NULL,1,-1,-1);
                     }
                    else if (c == getkeyshortcut(cfg_string[STR_PATH]))
                            ActivateStrGad(&editdrivegadgets[3],Window);
                    break;

                case IDCMP_MOUSEBUTTONS:
                    if (code==SELECTDOWN) {
                        if (seldrive && editon) {
                            a=seldrive->fpen; b=seldrive->bpen;
                            if (docolourgad(rp,x_off+150,y_off+76,x,y,seldrive->name,&a,&b,CFG_DRIVE)) {
                                seldrive->fpen=a; seldrive->bpen=b;
                                showdrivename(seldrive,seligad);
                            }
                        }
                    }
                    else if (code==MENUDOWN) {
                        if (!seldrive || !editon) break;
                        if (fixrmbimage(&sampleclearrmb,NULL,0,x,y)) {
                            a=DoRMBGadget(&sampleclearrmb,Window);
                            fixrmbimage(&sampleclearrmb,NULL,1,-1,-1);
                            if (a==0) showkey(seldrive->key=0xFF,seldrive->qual=0,x_off+150,y_off+129,331,10);
                            editdrivegadgets[1].Flags&=~SELECTED;
                            help_ok=1;
                        }
                    }
                    break;

                case IDCMP_GADGETUP:
                    if (gadgetid<DRIVE_OKAY) {
                        if (seldrive) {
                            select_gadget(seligad,0);
                            seligad=NULL; seldrive=NULL;
                        }
                        if (editon) {
                            driveeditoff();
                            editon=0;
                        }
                        if (mode==gadgetid) {
                            mode=-1;
                            dogadgetinfo(NULL);
                            break;
                        }
                        if (mode>-1) {
                            select_gadget(&maingad[mode],0);
                            mode=-1;
                        }
                    }
                    else if (gadgetid>=100) {
                        a=gadgetid-100;
                        odnum=dnum; dnum=a/6;
                        if (mode>-1) {
                            if (mode==DRIVE_DELETEDRIVE) {
                                deletedrive(&config->drive[a]);
                                seldrive=NULL;
                                dogadgetinfo(cfg_string[STR_DRIVE_GADGET_DELETED]);
                                showdrivename(seldrive,gad);
                            }
                            else if (!seldrive) {
                                seldrive=&config->drive[a];
                                seligad=gad;
                                select_gadget(gad,1);
                                switch (mode) {
                                    case DRIVE_COPYDRIVE:
                                        dogadgetinfo(cfg_string[STR_SELECT_DRIVE_TO_COPY_TO]);
                                        break;
                                    case DRIVE_SWAPDRIVE:
                                        dogadgetinfo(cfg_string[STR_SELECT_DRIVE_TO_SWAP_WITH_FIRST]);
                                        break;
                                    case DRIVE_COPYBANK:
                                        dogadgetinfo(cfg_string[STR_SELECT_DRIVE_BANK_TO_COPY_TO]);
                                        break;
                                    case DRIVE_SWAPBANK:
                                        dogadgetinfo(cfg_string[STR_SELECT_DRIVE_BANK_TO_SWAP_WITH_FIRST]);
                                        break;
                                    case DRIVE_DELETEBANK:
                                        if (request(cfg_string[STR_REALLY_DELETE_DRIVE_BANK])) {
                                            x=dnum*6; b=x+6;
                                            for (a=x;a<b;a++) deletedrive(&config->drive[a]);
                                        }
                                        seldrive=NULL;
                                        showdrives();
                                        doinitdrivetext(mode);
                                        break;
                                    case DRIVE_SORTBANK:
                                        sortdrivebank(dnum);
                                        seldrive=NULL;
                                        showdrives();
                                        doinitdrivetext(mode);
                                        break;
                                }
                            }
                            else {
                                seconddrive=&config->drive[a];
                                if (seldrive==seconddrive) select_gadget(gad,0);
                                else {
                                    switch (mode) {
                                        case DRIVE_COPYDRIVE:
                                            CopyMem((char *)seldrive,(char *)seconddrive,sizeof(struct dopusfunction));
                                            seconddrive->function=getcopy(seldrive->function,-1,NULL);
                                            break;
                                        case DRIVE_SWAPDRIVE:
                                            SwapMem((char *)seldrive,(char *)seconddrive,sizeof(struct dopusfunction));
                                            break;
                                        case DRIVE_COPYBANK:
                                            if (dnum!=odnum) {
                                                for (a=0;a<6;a++) {
                                                    CopyMem((char *)&config->drive[(odnum*6)+a],
                                                        (char *)&config->drive[(dnum*6)+a],sizeof(struct dopusfunction));
                                                    config->drive[(dnum*6)+a].function=getcopy(config->drive[(odnum*6)+a].function,-1,NULL);
                                                }
                                            }
                                            break;
                                        case DRIVE_SWAPBANK:
                                            if (dnum!=odnum)
                                                SwapMem((char *)&config->drive[odnum*6],
                                                    (char *)&config->drive[dnum*6],sizeof(struct dopusfunction)*6);
                                            break;
                                    }
                                    seldrive=NULL;
                                    showdrives();
                                }
                                seldrive=NULL; seligad=NULL;
                                doinitdrivetext(mode);
                            }
                        }
                        else {
                            if (paint_state) {
                                config->drive[a].fpen=paint_fg;
                                config->drive[a].bpen=paint_bg;
                                showdrivename(&config->drive[a],gad);
                            }
                            else {
                                if (seldrive && editon) select_gadget(seligad,0);
                                if (seldrive==&config->drive[a]) {
                                    driveeditoff();
                                    editon=0;
                                    break;
                                }
                                seligad=gad; seldrive=&config->drive[a];
                                showdrivename(seldrive,gad);
                                setupcolourbox(rp,x_off+150,y_off+76,seldrive->fpen,seldrive->bpen);
                                strcpy(edit_namebuf,seldrive->name);
                                if (seldrive->function) strcpy(edit_pathbuf,seldrive->function);
                                else edit_pathbuf[0]=0;
                                SetAPen(rp,screen_pens[1].pen);
                                if (!editon) {
                                    do_gad_label(cfg_string[STR_SHORTCUT_KEY],x_off+139,y_off+136);
                                    do_gad_label(cfg_string[STR_PATH],x_off+139,y_off+122);
                                    editdrivegadgets[1].Flags&=~SELECTED;
                                    AddGadgets(Window,
                                        editdrivegadgets,
                                        drivegadgets2,
                                        4,
                                        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
                                    fixrmbimage(&sampleclearrmb,&editdrivegadgets[1],2,-1,-1);
                                    doglassimage(&editdrivegadgets[2]);
                                }
                                else {
                                    RefreshStrGad(&editdrivegadgets[0],Window);
                                    RefreshStrGad(&editdrivegadgets[3],Window);
                                }
                                if (editdrivegadgets[1].Flags&SELECTED) {
                                    fixrmbimage(&sampleclearrmb,NULL,0,-1,-1);
                                    editdrivegadgets[1].Flags&=~SELECTED;
                                    select_gadget(&editdrivegadgets[1],0);
                                    fixrmbimage(&sampleclearrmb,NULL,1,-1,-1);
                                }
                                showfuncob(rp,seldrive->name,seldrive->fpen,seldrive->bpen,CFG_DRIVE,x_off+244,y_off+66);
                                showkey(seldrive->key,seldrive->qual,x_off+150,y_off+129,331,10);
                                editon=1;
                            }
                        }
                        break;
                    }
                    switch (gadgetid) {
                        case DRIVE_GETDRIVES:
                            {
                                char fpen_back[USEDRIVECOUNT],bpen_back[USEDRIVECOUNT];
                                int a;

                                for (a=0;a<USEDRIVECOUNT;a++) {
                                    fpen_back[a]=config->drive[a].fpen;
                                    bpen_back[a]=config->drive[a].bpen;
                                }
                                fixcstuff(&cstuff);
                                GetDevices(&cstuff);
                                seldrive=NULL;
                                for (a=0;a<USEDRIVECOUNT;a++) {
                                    config->drive[a].fpen=fpen_back[a];
                                    config->drive[a].bpen=bpen_back[a];
                                }
                                showdrives();
                                dogadgetinfo(cfg_string[STR_SYSTEM_DEVICES_LISTED]);
                            }
                            break;

                        case DRIVE_EDITNAME:
                            strcpy(seldrive->name,edit_namebuf);
                            showfuncob(rp,seldrive->name,seldrive->fpen,seldrive->bpen,CFG_DRIVE,x_off+244,y_off+66);
                            showdrivename(seldrive,seligad);
                            if (code!=0x9) getnextgadget(gad);
                            break;

                        case DRIVE_EDITPATH:
                            freestring(seldrive->function);
                            seldrive->function=getcopy(edit_pathbuf,-1,NULL);
                            if (code!=0x9) getnextgadget(gad);
                            break;

                        case DRIVE_EDITREQ:
                            strcpy(dirbuf,edit_pathbuf);
                            if (funcrequester(FREQ_PATHREQ,buf,NULL)) {
                                LStrnCpy(edit_pathbuf,buf,FILEBUF_SIZE); edit_pathbuf[FILEBUF_SIZE]=0;
                                b=strlen(edit_pathbuf);
                                for (a=b;a>=0;a--) {
                                    if (edit_pathbuf[a]=='/' || edit_pathbuf[a]==':') {
                                        edit_pathbuf[a+1]=0;
                                        break;
                                    }
                                }
                                freestring(seldrive->function);
                                seldrive->function=getcopy(edit_pathbuf,-1,NULL);
                                RefreshStrGad(&editdrivegadgets[3],Window);
                            }
                            break;

                        case DRIVE_EDITSAMPLE:
                            help_ok=(editdrivegadgets[1].Flags&SELECTED)?0:1;
                            break;

                        case DRIVE_CANCEL:
                            doundo(undo,UNDO_DRIVE);
                        case DRIVE_OKAY:
                            doundo(undo,0);
                            RemoveGList(Window,drivegads,USEDRIVECOUNT);
                            LFreeRemember(&gadkey); drivegads=NULL;
                            ClearMenuStrip(Window); projectmenu.NextMenu=NULL;
                            FSSetMenuStrip(Window,&projectmenu);
                            help_ok=1;
                            paint_state=0;
                            return((gadgetid==DRIVE_OKAY));
                    }
                    mode=doinitdrivetext(gadgetid);
                    break;
            }
        }
    }
}

void showdrives()
{
    int a;

    for (a=0;a<USEDRIVECOUNT;a++)
        showdrivename(&config->drive[a],&drivegads[a]);
}

void showdrivename(drive,gadget)
struct dopusfunction *drive;
struct Gadget *gadget;
{
    int a,x,y,l;
    char showname[16];

    x=gadget->LeftEdge;
    y=gadget->TopEdge;
    if (drive) SetAPen(rp,screen_pens[drive->bpen].pen);
    else SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,x+2,y+1,x+77,y+8);
    do3dbox(x+2,y+1,76,8);
    if (drive) {
        SetAPen(rp,screen_pens[drive->fpen].pen);
        SetBPen(rp,screen_pens[drive->bpen].pen);
        strcpy(showname,drive->name);
        l = (80-8)/rp->Font->tf_XSize;
        showname[l]=0;
        UScoreText(rp,showname,x+((80-((a=strlen(showname))*rp->Font->tf_XSize))/2),y+7,-1);
        SetBPen(rp,screen_pens[0].pen);
        if (seldrive==drive) select_gadget(seligad,1);
    }
}

int doinitdrivetext(id)
int id;
{
    int mode=-1;

    switch (id) {
        case DRIVE_COPYBANK:
            mode=DRIVE_COPYBANK;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_BANK_TO_COPY]);
            break;
        case DRIVE_SWAPBANK:
            mode=DRIVE_SWAPBANK;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_BANK_TO_SWAP]);
            break;
        case DRIVE_DELETEBANK:
            mode=DRIVE_DELETEBANK;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_BANK_TO_DELETE]);
            break;
        case DRIVE_SORTBANK:
            mode=DRIVE_SORTBANK;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_BANK_TO_SORT]);
            break;
        case DRIVE_COPYDRIVE:
            mode=DRIVE_COPYDRIVE;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_GADGET_TO_COPY]);
            break;
        case DRIVE_SWAPDRIVE:
            mode=DRIVE_SWAPDRIVE;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_GADGET_TO_SWAP]);
            break;
        case DRIVE_DELETEDRIVE:
            mode=DRIVE_DELETEDRIVE;
            dogadgetinfo(cfg_string[STR_SELECT_DRIVE_GADGET_TO_DELETE]);
            break;
    }
    return(mode);
}

int docolourgad(r,x,y,mx,my,name,fp,bp,type)
struct RastPort *r;
int x,y,mx,my;
char *name;
int *fp,*bp,type;
{
    int num,box;

    if (my>y && my<y+21) {
        for (box=0;box<2;box++) {
            for (num=0;num<colour_box_num;num++) {
                if (mx>=colour_box_xy[box][num][0] && mx<=colour_box_xy[box][num][2] &&
                    my>=colour_box_xy[box][num][1] && my<=colour_box_xy[box][num][3]) {
                    if ((box==0 && *fp!=num) || (box==1 && *bp!=num)) {
                        docoloursel(r,x+(box*248),y,(box==0)?*fp:*bp);
                        docoloursel(r,x+(box*248),y,num);
                        if (box==0) *fp=num;
                        else *bp=num;
                        if (name) showfuncob(r,name,*fp,*bp,type,x+94,y-10);
                        return(1);
                    }
                    return(0);
                }
            }
        }
    }
    return(0);
}

void driveeditoff()
{
    seldrive=NULL; seligad=NULL;
    SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,
        x_off+2,y_off+65,
        x_bot,y_off+140);
    RemoveGList(Window,editdrivegadgets,4);
}

void deletedrive(drive)
struct dopusfunction *drive;
{
    drive->name[0]=0;
    freestring(drive->function); drive->function=NULL;
    drive->key=0xff;
    drive->qual=0;
    if (paint_state) {
        drive->fpen=paint_fg;
        drive->bpen=paint_bg;
    }
    else {
        drive->fpen=1;
        drive->bpen=0;
    }
}

void sortdrivebank(m)
int m;
{
    int gap,i,j,k;
    char *ptr1,*ptr2,*zstr="\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f";

    m*=6;
    for (gap=3;gap>0;gap/=2)
        for (i=gap;i<6;i++)
            for (j=i-gap;j>=0;j-=gap) {
                k=j+gap;
                ptr1=(config->drive[j+m].name[0])?config->drive[j+m].name:zstr;
                ptr2=(config->drive[k+m].name[0])?config->drive[k+m].name:zstr;
                if (LStrCmpI(ptr1,ptr2)<=0) break;
                SwapMem((char *)&config->drive[j+m],(char *)&config->drive[k+m],
                    sizeof(struct dopusfunction));
            }
}
