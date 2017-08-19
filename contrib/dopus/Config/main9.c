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
#ifndef __AROS__
#include <proto/powerpacker.h>
#endif

void readhelp()
{
    BPTR in;
    char helpname[40],helpfilename[256];
    ULONG id;


    LFreeRemember(&helpkey);
    helpbuffer=NULL;
    helpsize=0;

    lsprintf(helpname,"CO_%s.HLP",config->language);
    if (!config->language[0] ||
        !(FindSystemFile(helpname,helpfilename,256,SYSFILE_DATA)))
        FindSystemFile("ConfigOpus.HLP",helpfilename,256,SYSFILE_DATA);

    if (CheckExist(helpfilename,&helpsize)>=0) return;
    if (in=Open(helpfilename,MODE_OLDFILE))
     {
      Read(in, &id, 4);
      if ((id == MAKE_ID('P','X','2','0')) || (id == MAKE_ID('P','P','1','1')) || (id == MAKE_ID('P','P','2','0'))) // FIXME: endiannes?
       {
#ifndef __AROS__
        struct PPBase *PPBase;
        char *pptemp;
#endif

        Close(in);
#ifndef __AROS__
        if (PPBase = OpenLibrary("powerpacker.library",0))
          {
           if (!(ppLoadData(helpfilename,DECR_NONE,MEMF_CLEAR,&pptemp,&helpsize,NULL)))
            {
             if (helpbuffer=LAllocRemember(&helpkey,helpsize+1,MEMF_CLEAR))
               CopyMem(pptemp,helpbuffer,helpsize);
             FreeMem(pptemp,helpsize);
            }
           CloseLibrary(PPBase);
          }
#endif
       }
      else
       {
        Seek(in,0,OFFSET_BEGINNING);
        if (helpbuffer=LAllocRemember(&helpkey,helpsize+1,MEMF_CLEAR)) Read(in,helpbuffer,helpsize);
        Close(in);
       }
     }
}

void makehelpname(path)
char *path;
{
    int a,b;

    if (path==(char *)-1) {
        b=strlen(currenthelpname)-2;
        for (a=b;a>=0;a--) {
            if (currenthelpname[a]=='/') {
                currenthelpname[a]=0;
                break;
            }
        }
    }
    else TackOn(currenthelpname,path,256);
}

void doconfighelp()
{
    int a,test=1;

    if (!helpbuffer) return;
    for (a=0;a<helpsize;a++) {
        if (helpbuffer[a]==0) {
            if (test==-1) {
                busy();
                dohelpmsg(&helpbuffer[a+1]);
                unbusy();
                return;
            }
            if (test) test=0;
            else test=1;
        }
        else if (test==1) {
D(bug("doconfighelp: LStrCmpI(\"%s\",\"%s\")\n",&helpbuffer[a],currenthelpname));
            if (LStrCmpI(&helpbuffer[a],currenthelpname)==0) test=-1;
            else ++test;
        }
    }
}

void dohelpmsg(text)
char *text;
{
    struct Window *wind;
    char **helpbuf,*buf;
    int size,lines,a,b,pos;
    struct DOpusListView *view;
    ULONG class;
    UWORD gadgetid;
    struct DOpusRemember *key=NULL;

D(bug("dohelpmsg: %s\n",text));
    size=strlen(text);
    if (!(buf=LAllocRemember(&key,size+1,MEMF_CLEAR))) return;
    for (a=0,lines=0;a<size;a++) {
        if (text[a]=='\n') {
            buf[a]=0;
            ++lines;
        }
        else buf[a]=text[a];
    }
    if (!(helpbuf=LAllocRemember(&key,(lines+2)*4,MEMF_CLEAR))) {
        LFreeRemember(&key);
        return;
    }
    for (a=0,b=0,pos=0;a<size;a++) {
        if (pos==0) {
            helpbuf[(b++)]=&buf[a];
            if (b>lines) break;
        }
        if (buf[a]==0) pos=0;
        else ++pos;
    }

    setup_list_window(&requestwin,&helplist,&helpcancelgad,1);

    requestwin.Title=currenthelpname;
    if (wind=openwindow(&requestwin)) {
        setuplist(&helplist,-1,-1);
        helplist.items=helpbuf;
        helplist.flags|=DLVF_DUMB;
        helplist.topitem=0;
        helplist.window=wind;
        if (AddListView(&helplist,1)) {
            SetAPen(wind->RPort,screen_pens[1].pen);
            AddGadgets(wind,
                &helpcancelgad,
                editfuncgads,
                1,
                screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
            FOREVER {
                Wait(1<<wind->UserPort->mp_SigBit);
                while (IMsg=(struct IntuiMessage *)GetMsg(wind->UserPort)) {
                    if ((view=ListViewIDCMP(&helplist,IMsg))==(struct DOpusListView *)-1) {
                        class=IMsg->Class;
                        if (class==IDCMP_GADGETUP)
                            gadgetid=((struct Gadget *)IMsg->IAddress)->GadgetID;
                        ReplyMsg((struct Message *)IMsg);
                        if (class==IDCMP_GADGETUP && gadgetid==0) goto endhelp;
                    }
                }
            }
endhelp:
            RemoveListView(&helplist,1);
        }
        CloseWindow(wind);
    }
    LFreeRemember(&key);
}

/*
struct clipboard_data {
    long form;
    long length;
    long docp;
    long size;
};

void load_clips()
{
    struct Clip clip,*curclip=NULL,*newclip;
    struct clipboard_data cdata;
    char *funcbuf;

    if (!clip_io ||
        (OpenDevice("clipboard.device",0,(struct IORequest *)clip_io,0))) return;

    clip_io->io_Error=0;
    clip_io->io_ClipID=0;

    clip_io->io_Command=CMD_READ;
    clip_io->io_Data=(char *)&cdata;
    clip_io->io_Length=sizeof(struct clipboard_data);
    clip_io->io_Offset=0;

    if (!(DoIO((struct IORequest *)clip_io)) &&
        cdata.form=='FORM' && cdata.docp=='DOCP') {

        FOREVER {
            clip_io->io_Command=CMD_READ;
            clip_io->io_Data=(char *)&clip;
            clip_io->io_Length=sizeof(struct Clip);
            if (DoIO((struct IORequest *)clip_io) ||
                clip_io->io_Actual<sizeof(struct Clip)) break;
            if (clip.func.function &&
                (funcbuf=LAllocRemember(&clipkey,(int)clip.func.function,0))) {
                clip_io->io_Command=CMD_READ;
                clip_io->io_Data=funcbuf;
                clip_io->io_Length=(int)clip.func.function;
                if (DoIO((struct IORequest *)clip_io) ||
                    clip_io->io_Actual<(int)clip.func.function) break;
            }
            else funcbuf=NULL;
            clip.func.function=funcbuf;
            if (newclip=LAllocRemember(&clipkey,sizeof(struct Clip),0)) {
                CopyMem((char *)&clip,(char *)newclip,sizeof(struct Clip));
                if (curclip) curclip->next=newclip;
                else firstclip=newclip;
                curclip=newclip;
                ++clipcount;
            }
        }
    }

    CloseDevice((struct IORequest *)clip_io);
}

void save_clips()
{
    int size=0;
    struct Clip *clip;
    struct clipboard_data cdata;
    char *function;

    if (!clip_io ||
        (OpenDevice("clipboard.device",0,(struct IORequest *)clip_io,0))) return;

    clip=firstclip;
    while (clip) {
        size+=sizeof(struct Clip);
        if (clip->func.function) size+=strlen(clip->func.function)+1;
        clip=clip->next;
    }

    cdata.form='FORM';
    cdata.length=size+8;
    cdata.docp='DOCP';
    cdata.size=size;

    clip_io->io_Error=0;
    clip_io->io_ClipID=0;

    clip_io->io_Data=(char *)&cdata;
    clip_io->io_Length=sizeof(struct clipboard_data);
    clip_io->io_Command=CMD_WRITE;
    clip_io->io_Offset=0;

    if (!(DoIO((struct IORequest *)clip_io))) {
        clip=firstclip;
        while (clip) {
            if ((function=clip->func.function))
                clip->func.function=(char *)strlen(function)+1;

            clip_io->io_Data=(char *)clip;
            clip_io->io_Length=sizeof(struct Clip);
            clip_io->io_Command=CMD_WRITE;
            if (DoIO((struct IORequest *)clip_io)) break;
            if (function) {
                clip_io->io_Data=function;
                clip_io->io_Length=(int)clip->func.function;
                clip_io->io_Command=CMD_WRITE;
                if (DoIO((struct IORequest *)clip_io)) break;
            }
            clip=clip->next;
        }
    }

    clip_io->io_Command=CMD_UPDATE;
    DoIO((struct IORequest *)clip_io);

    CloseDevice((struct IORequest *)clip_io);
}
*/

void load_clips()
{
    struct Clip clip,*curclip=NULL,*newclip;
    char *funcbuf;
    BPTR file;

    if (!(file=Open("t:configopus.clip",MODE_OLDFILE))) return;

    FOREVER {
        if ((Read(file,(char *)&clip,sizeof(struct Clip)))<sizeof(struct Clip))
            break;
        if (clip.func.function &&
            (funcbuf=LAllocRemember(&clipkey,(int)clip.func.function,0))) {
            if ((Read(file,funcbuf,(long)clip.func.function))<(long)clip.func.function)
                break;
        }
        else funcbuf=NULL;
        clip.func.function=funcbuf;
        if (newclip=LAllocRemember(&clipkey,sizeof(struct Clip),0)) {
            CopyMem((char *)&clip,(char *)newclip,sizeof(struct Clip));
            if (curclip) curclip->next=newclip;
            else firstclip=newclip;
            curclip=newclip;
            ++clipcount;
        }
    }

    Close(file);
}

void save_clips()
{
    BPTR file;
    struct Clip *clip;
    char *function;

    if (!(file=Open("t:configopus.clip",MODE_NEWFILE))) return;

    clip=firstclip;
    while (clip) {
        if ((function=clip->func.function))
            clip->func.function=(char *)strlen(function)+1;
        if ((Write(file,(char *)clip,sizeof(struct Clip)))<sizeof(struct Clip))
            break;
        if (function &&
            ((Write(file,(char *)function,(int)clip->func.function))<(int)clip->func.function))
            break;
        clip=clip->next;
    }

    Close(file);
}

int readfile(name,buf,size)
char *name,**buf;
int *size;
{
    BPTR in;

    if (CheckExist(name,size)>=0 || !(in=Open(name,MODE_OLDFILE))) return(-1);
    if (!(*buf=AllocMem(*size,MEMF_CLEAR))) {
        Close(in);
        return(-2);
    }
    Read(in,*buf,*size);
    Close(in);
    return(0);
}

void init_strings()
{
    int a;

    insertbankrmb.txt[0].text=cfg_string[STR_NEW_BANK];
    insertbankrmb.txt[1].text=cfg_string[STR_INSERT_BANK];
    nextbankrmb.txt[0].text=cfg_string[STR_NEXT_BANK];
    nextbankrmb.txt[1].text=cfg_string[STR_LAST_BANK];
    formatclearrmb.txt[0].text=cfg_string[STR_CLEAR];
    formatclearrmb.txt[1].text=cfg_string[STR_CLEAR_LAST];
    sampleclearrmb.txt[0].text=cfg_string[STR_EDIT_SAMPLE];
    sampleclearrmb.txt[1].text=cfg_string[STR_CLEAR];

    newtext.IText=cfg_string[STR_MENU_DEFAULT];
    opentext.IText=cfg_string[STR_MENU_OPEN];
    savetext.IText=cfg_string[STR_MENU_SAVE];
    saveastext.IText=cfg_string[STR_MENU_SAVE_AS];
    cuttext.IText=cfg_string[STR_MENU_CUT];
    copytext.IText=cfg_string[STR_MENU_COPY];
    pastetext.IText=cfg_string[STR_MENU_PASTE];
    erasetext.IText=cfg_string[STR_MENU_ERASE];
    clearcliptext.IText=cfg_string[STR_MENU_CLEARCLIPS];
    newclasstext.IText=cfg_string[STR_MENU_NEW];
    editclasstext.IText=cfg_string[STR_MENU_EDIT];
    duplicateclasstext.IText=cfg_string[STR_MENU_DUPLICATE];
    deleteclasstext.IText=cfg_string[STR_MENU_DELETE];
    clearclasstext.IText=cfg_string[STR_CLEAR];

    for (a=0;a<5;a++) {
        gadrowstext[a].FrontPen=0;
        gadrowstext[a].DrawMode=JAM1;
        gadrowstext[a].LeftEdge=1;
        gadrowstext[a].TopEdge=1;
        gadrowstext[a].ITextFont=NULL;
        gadrowstext[a].IText=cfg_string[STR_GADGETROWS_NONE+a];
        gadrowstext[a].NextText=NULL;
    }

    paintmodetext.IText=cfg_string[STR_NEAT_PAINT_MODE];

    projectmenu.MenuName=cfg_string[STR_MENUNAME_CONFIGURE];
    gadrowsmenu.MenuName=cfg_string[STR_MENUNAME_GADGETROWS];
    editmenu.MenuName=cfg_string[STR_EDIT_MENU];
    classmenu.MenuName=cfg_string[STR_MENUNAME_CLASSES];
    neatstuffmenu.MenuName=cfg_string[STR_MENUNAME_NEATSTUFF];

    for (a=0;a<3;a++) fileclasstype[a]=cfg_string[STR_FILECLASS_DEFINE+a];
    fileclasstype[3]=cfg_string[STR_FILECLASS_DUPLICATE];

    for (a=0;a<9;a++) fontplacenames[a]=cfg_string[STR_FONTPLACE_CLOCK+a];
    fontplacenames[a++]=cfg_string[STR_FONTPLACE_SCREEN];
    fontplacenames[a++]=cfg_string[STR_FONTPLACE_STATUS];
    fontplacenames[a++]=cfg_string[STR_FONTPLACE_TEXTVIEWER];
    fontplacenames[a]=NULL;

    palettelist.title=cfg_string[STR_PALETTE_PRESETS];
    iconlistview.title=cfg_string[STR_ICONLIST_TITLE];

    for (a=0;a<6;a++) formatnames[a]=cfg_string[STR_FORMAT_NAME+a];
    if (network) {
        formatnames[6]=cfg_string[STR_FORMAT_OWNER];
        formatnames[7]=cfg_string[STR_FORMAT_GROUP];
        formatnames[8]=cfg_string[STR_FORMAT_GROUPBITS];
        formatnames[9]=NULL;
    }
    else formatnames[6]=NULL;

    for (a=0;a<3;a++) icontypes[a]=cfg_string[STR_ICONS_DRAWER+a];

    for (a=0;a<8;a++) mainmenugads[a]=cfg_string[STR_MAINMENU_GADGETS+a];
    mainmenugads[8]="";
    mainmenugads[9]="";
    mainmenugads[10]=cfg_string[STR_MENU_SAVE];
    mainmenugads[11]=cfg_string[STR_USE];
    mainmenugads[12]=cfg_string[STR_CANCEL];
    mainmenugads[13]=NULL;

    listviewgads[0]=cfg_string[STR_OKAY];
    listviewgads[1]=cfg_string[STR_CANCEL];
    listviewgads[2]=NULL;

    listviewgads2[0]=cfg_string[STR_OKAY];
    listviewgads2[1]=cfg_string[STR_ALL];
    listviewgads2[2]=cfg_string[STR_CANCEL];
    listviewgads2[3]=NULL;

    operationgads[0]=cfg_string[STR_MENU_COPY];
    for (a=0;a<7;a++) operationgads[a+1]=cfg_string[STR_OPERATION_DATEFORMAT+a];
    for (a=8;a<11;a++) operationgads[a]="";
    operationgads[11]=cfg_string[STR_OKAY];
    operationgads[12]=cfg_string[STR_CANCEL];
    operationgads[13]=NULL;

    for (a=0;a<3;a++) systemgads[a]=cfg_string[STR_SYSTEM_AMIGADOS+a];
    systemgads[3]=cfg_string[STR_HOTKEY];
    for (a=3;a<8;a++) systemgads[a+1]=cfg_string[STR_SYSTEM_AMIGADOS+a];
    systemgads[9]="";
    systemgads[10]="";
    systemgads[11]=cfg_string[STR_OKAY];
    systemgads[12]=cfg_string[STR_CANCEL];
    systemgads[13]=NULL;

    gadgetgads[0]=cfg_string[STR_NEXT_BANK];
    gadgetgads[1]=cfg_string[STR_NEW_BANK];
    for (a=0;a<6;a++) gadgetgads[a+2]=cfg_string[STR_GADGET_COPYBANK+a];
    gadgetgads[8]=cfg_string[STR_OKAY];
    gadgetgads[9]=cfg_string[STR_CANCEL];
    gadgetgads[10]=NULL;

    for (a=0;a<16;a++) gadflaglist[a]=cfg_string[STR_FLAG_AUTOICONIFY+a];
    gadflaglist[16]=NULL;

    editfuncgads[0]=cfg_string[STR_OKAY];
    for (a=1;a<4;a++) editfuncgads[a]="";
    editfuncgads[4]=cfg_string[STR_CANCEL];
    editfuncgads[5]=NULL;

    for (a=0;a<5;a++) editfuncgads2[a]=cfg_string[STR_EDIT_NAME+a];
    editfuncgads2[5]=cfg_string[STR_OPERATION_DELETE];
    for (a=5;a<8;a++) editfuncgads2[a+1]=cfg_string[STR_EDIT_NAME+a];
    for (a=9;a<12;a++) editfuncgads2[a]=(char *)-1;
    editfuncgads2[12]="{}";
    editfuncgads2[13]=cfg_string[STR_EDIT_ACTION];
    editfuncgads2[14]=cfg_string[STR_EDIT_CLASS];
    editfuncgads2[15]=NULL;

    editclassgads[0]=cfg_string[STR_EDITCLASS_FILECLASS];
    editclassgads[1]=cfg_string[STR_EDITCLASS_CLASSID];
    editclassgads[2]=cfg_string[STR_EDIT_NEWENTRY];
    editclassgads[3]=cfg_string[STR_EDIT_DUPLICATE];
    editclassgads[4]=cfg_string[STR_EDIT_SWAP];
    editclassgads[5]=cfg_string[STR_OPERATION_DELETE];
    editclassgads[6]=(char *)-1;
    editclassgads[7]=(char *)-1;
    editclassgads[8]=cfg_string[STR_EDITCLASS_FILEVIEWER];
    editclassgads[9]=NULL;

    for (a=0;a<8;a++) menugadgets[a]=cfg_string[STR_MENU_COPYMENU+a];
    menugadgets[8]=cfg_string[STR_OKAY];
    menugadgets[9]=cfg_string[STR_CANCEL];
    menugadgets[10]=NULL;

    drivegadgets[0]=cfg_string[STR_DRIVES_GETDRIVES];
    for (a=0;a<3;a++) drivegadgets[a+1]=cfg_string[STR_GADGET_COPYBANK+a];
    for (a=0;a<5;a++) drivegadgets[a+4]=cfg_string[STR_DRIVES_SORTBANK+a];
    drivegadgets[8]=cfg_string[STR_OKAY];
    drivegadgets[9]=cfg_string[STR_CANCEL];
    drivegadgets[10]=NULL;

    drivegadgets2[0]=cfg_string[STR_EDIT_NAME];
    drivegadgets2[1]=cfg_string[STR_EDIT_SAMPLE];
    drivegadgets2[2]="";
    drivegadgets2[3]="";
    drivegadgets2[4]=NULL;

    hotkeysgadgets[0]=cfg_string[STR_HOTKEYS_NEWHOTKEY];
    hotkeysgadgets[1]="";
    hotkeysgadgets[2]=cfg_string[STR_OPERATION_DELETE];
    hotkeysgadgets[3]="";
    hotkeysgadgets[4]=cfg_string[STR_EDIT_DUPLICATE];
    hotkeysgadgets[5]=cfg_string[STR_OKAY];
    hotkeysgadgets[6]="";
    hotkeysgadgets[7]=cfg_string[STR_EDIT_SWAP];
    hotkeysgadgets[8]="";
    hotkeysgadgets[9]=cfg_string[STR_CANCEL];
    hotkeysgadgets[10]=NULL;

    screengadgets[0]=cfg_string[STR_COLOR_ARROWS];
    for (a=0;a<2;a++) screengadgets[a+1]=cfg_string[STR_SCREEN_COLORS+a];
    screengadgets[3]=cfg_string[STR_SCREEN_GENERAL];
    screengadgets[4]=cfg_string[STR_SCREEN_PALETTE];
    screengadgets[5]=cfg_string[STR_SCREEN_SCREENMODE];
    screengadgets[6]=cfg_string[STR_SCREEN_SLIDERS];
    for (a=7;a<11;a++) screengadgets[a]="";
    screengadgets[11]=cfg_string[STR_OKAY];
    screengadgets[12]=cfg_string[STR_CANCEL];
    screengadgets[13]=NULL;

    for (a=0;a<3;a++) palettegadgets[a]=(char *)-1;
    for (a=0;a<3;a++) palettegadgets[a+3]=cfg_string[STR_PALETTE_RED+a];
    palettegadgets[6]=NULL;

    for (a=0;a<15;a++) coloursgadgets[a]=cfg_string[STR_COLOR_STATUS+a];
    coloursgadgets[15]=NULL;

    for (a=0;a<4;a++) screenmodegadgets[a]=cfg_string[STR_SCREENMODE_WIDTH+a];
    screenmodegadgets[4]=cfg_string[STR_SCREENMODE_DEFAULT];
    screenmodegadgets[5]=cfg_string[STR_SCREENMODE_HALFHEIGHT];
    screenmodegadgets[6]=NULL;

    formatgadgets[0]=cfg_string[STR_LISTFORMAT_TITLE];
    formatgadgets[1]=cfg_string[STR_SIZE_KMGMODE];
    formatgadgets[2]=cfg_string[STR_LISTFORMAT_RESET]; //HUX was 1
    formatgadgets[3]=cfg_string[STR_CLEAR]; //HUX was 2
    formatgadgets[4]=(char *)-1; //HUX was 3
    formatgadgets[5]=cfg_string[STR_LISTFORMAT_REVERSE]; //HUX was 4
    formatgadgets[6]=cfg_string[STR_FORMAT_NAME]; //HUX was 5
    formatgadgets[7]=cfg_string[STR_FORMAT_COMMENT]; //HUX was 6
    formatgadgets[8]=cfg_string[STR_FORMAT_TYPE]; //HUX was 7
    formatgadgets[9]=cfg_string[STR_FORMAT_OWNER]; //HUX was 8
    formatgadgets[10]=cfg_string[STR_FORMAT_GROUP]; //HUX was 9

    hotkeygadgets[0]=cfg_string[STR_EDIT_SAMPLE];
    hotkeygadgets[1]=cfg_string[STR_USE_MMB];
    hotkeygadgets[2]=NULL;

    for (a=0;a<3;a++) arrowgadtxt[a]=cfg_string[STR_ARROWS_INSIDE+a];
    arrowgadtxt[3]=NULL;

    for (a=0;a<3;a++) arrowtypetxt[a]=cfg_string[STR_ARROWTYPE_UPDOWN+a];

    filetypeactiongadgets[0]=cfg_string[STR_NEW];
    filetypeactiongadgets[1]="";
    filetypeactiongadgets[2]=cfg_string[STR_EDIT_SWAP];
    filetypeactiongadgets[3]="";
    filetypeactiongadgets[4]=cfg_string[STR_OPERATION_DELETE];
    filetypeactiongadgets[5]=cfg_string[STR_OKAY];
    for (a=6;a<9;a++) filetypeactiongadgets[a]="";
    filetypeactiongadgets[9]=cfg_string[STR_CANCEL];
    filetypeactiongadgets[10]=NULL;

    editlists[1].title=cfg_string[STR_EDITLIST_TITLE1];
    editlists[2].title=cfg_string[STR_EDITLIST_TITLE2];
    filetypeactionlist.title=cfg_string[STR_FILETYPEACTIONLIST_TITLE];
    hotkeyslist.title=cfg_string[STR_HOTKEYSLIST_TITLE];
    screenmodeview.title=cfg_string[STR_SCREENMODELIST_TITLE];
    modulelist.title=cfg_string[STR_MODULE_LIST_TITLE];
//    languagelist.title=cfg_string[STR_LANGUAGE_LIST_TITLE];

    windownames[0]=cfg_string[STR_LEFT_WINDOW];
    windownames[1]=cfg_string[STR_RIGHT_WINDOW];

    for (a=0;a<3;a++) separatenames[a]=cfg_string[STR_SEP_MIX_FILES+a];

    for (a=0;a<94;a++) commandlist[a]=cfg_string[func_list_items[a]];
    commandlist[94]=NULL;

    for (a=0;a<16;a++) arglist[a]=cfg_string[STR_COMMANDSEQ_DESTINATION+a];
    arglist[16]=cfg_string[STR_COMMANDSEQ_VARIABLE];
    arglist[17]=NULL;

    for (a=0;a<11;a++) classopslist[a]=cfg_string[STR_CLASSOPS_AND+a];
    classopslist[11]=cfg_string[STR_CLASSOPS_MATCHI];
    classopslist[12]=NULL;

    for (a=0;a<4;a++) palettenames[a]=cfg_string[STR_PALETTE_RESET+a];
    for (a=0;a<9;a++) palettenames[a+4]=cfg_string[STR_PALETTE_TINT+a];
    palettenames[13]=NULL;

    for (a=0;a<3;a++)
        lsprintf(functypelist[a],"   %-11s %s",
            cfg_string[STR_FTYPE_ACTION],cfg_string[STR_FTYPE_CLICKMCLICK+a]);
    for (a=0;a<10;a++)
        lsprintf(functypelist[a+3],"   %-11s %s",
            cfg_string[STR_FTYPE_COMMAND],ftype_funcs[a]);

    fileview_types[0]=cfg_string[STR_FILEVIEW_HEX];
    fileview_types[1]=cfg_string[STR_FILEVIEW_DEC];

    for (a=0;a<5;a++) functypestr[a]=cfg_string[STR_FUNCTION_COMMAND+a];
    functypestr[5]="----";

    left_right_cycle[0]=cfg_string[STR_SYS_STARTUP_LEFT];
    left_right_cycle[1]=cfg_string[STR_SYS_STARTUP_RIGHT];
    left_right_cycle[2]=NULL;
}

void setup_list_window(win,list,gadget,count)
struct NewWindow *win;
struct DOpusListView *list;
struct Gadget *gadget;
int count;
{
    struct Screen *screen;

    screen=Window->WScreen;
    list->x=screen->WBorLeft+6;
    list->y=screen->WBorTop+screen->Font->ta_YSize+4;
    win->Width=screen->WBorLeft+screen->WBorRight+list->w+28;
    win->Height=screen->WBorTop+screen->WBorBottom+screen->Font->ta_YSize+list->h+22;
    win->Screen=screen;
    win->LeftEdge=Window->LeftEdge+((Window->Width-win->Width)/2);
    win->TopEdge=Window->TopEdge+((Window->Height-win->Height)/2);

    if (gadget) {
        int gspace,goffset,a;

        gspace=(win->Width-screen->WBorLeft-screen->WBorRight)/count;
        goffset=((gspace-gadget->Width)/2)+screen->WBorLeft;

        for (a=0;a<count && gadget;a++,goffset+=gspace) {
            gadget->LeftEdge=goffset;
            gadget->TopEdge=win->Height-screen->WBorBottom-gadget->Height-2;
            gadget=gadget->NextGadget;
        }
    }
}
