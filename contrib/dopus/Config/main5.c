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

int dofiletypeconfig()
{
    ULONG class;
    UWORD code,gadgetid;
    struct ConfigUndo *undo;
    struct DOpusRemember *key;
    struct DOpusListView *view;
    struct dopusfiletype *type,*tpos,*type1;
    struct fileclass *fclass,classbuf;
    int mode=-1,a,classchange=0;
    char **listptr,buf[256],*ptr;

    key=NULL; type1=NULL; firstclass=NULL;
    filetypeactionlist.items=NULL;
    filetypeactionlist.topitem=0;
    showconfigscreen(CFG_FILETYPE);
    ClearMenuStrip(Window);
    projectmenu.NextMenu=&classmenu;
    FSSetMenuStrip(Window,&projectmenu);
    listptr=makefiletypelist(&key);
    undo=makeundo(UNDO_FILETYPE);
    showtypelist(listptr);
    busy();
    readfileclasses();
    unbusy();

    FOREVER {
        Wait(1<<Window->UserPort->mp_SigBit);
        while (IMsg=getintuimsg()) {
            if ((view=ListViewIDCMP(&filetypeactionlist,IMsg))==(struct DOpusListView *)-1) {
                class=IMsg->Class; code=IMsg->Code;
                if (class==GADGETUP || class==GADGETDOWN)
                    gadgetid=((struct Gadget *)IMsg->IAddress)->GadgetID;
                ReplyMsg((struct Message *)IMsg);
                switch (class) {
                    case MENUPICK:
                        if (code==MENUNULL) break;
                        switch (MENUNUM(code)) {
                            case 0:
                                if (type1) {
                                    showtypelist(listptr);
                                    type1=NULL;
                                }
                                if (mode>-1) {
                                    select_gadget(&maingad[mode],0);
                                    mode=-1;
                                }
                                dogadgetinfo(NULL);
                                switch ((a=ITEMNUM(code))) {
                                    case 0:
                                    case 1:
                                        if (doload(CFG_FILETYPE,a)) {
                                            listptr=makefiletypelist(&key);
                                            showtypelist(listptr);
                                            if (a) dogadgetinfo(cfg_string[STR_NEW_FILETYPES_MERGED]);
                                            else dogadgetinfo(cfg_string[STR_DEFAULT_FILETYPES_MERGED]);
                                        }
                                        break;
                                }
                                break;
                            case 1:
                                switch (ITEMNUM(code)) {
                                    case 0:
                                        classbuf.type[0]=0;
                                        classbuf.typeid[0]=0;
                                        classbuf.recognition=NULL;
                                        classbuf.next=classbuf.last=NULL;
                                        if (editclass(&classbuf,1)) classchange=1;
                                        break;
                                    case 1:
                                        if (!(fclass=getfileclasslist(1))) break;
                                        if (editclass(fclass,0)) classchange=1;
                                        break;
                                    case 2:
                                        if (!(fclass=getfileclasslist(3))) break;
                                        CopyMem((char *)fclass,(char *)&classbuf,sizeof(struct fileclass));
                                        classbuf.recognition=getcopy(fclass->recognition,-1,NULL);
                                        classbuf.next=classbuf.last=NULL;
                                        if (editclass(&classbuf,1)) classchange=1;
                                        break;
                                    case 3:
                                        if (!(fclass=getfileclasslist(2))) break;
                                        lsprintf(buf,"\"%s\"\n%s",
                                            fclass->type,cfg_string[STR_REALLY_DELETE_THIS_CLASS]);
                                        if (request(buf)) {
                                            dogadgetinfo(cfg_string[STR_FILE_CLASS_DELETED]);
                                            removefileclass(fclass);
                                            classchange=1;
                                        }
                                        else dogadgetinfo(NULL);
                                        break;
                                    case 5:
                                        if (importfileclasses()) classchange=1;
                                        break;
                                    case 6:
                                        busy();
                                        if (savefileclasses()) classchange=0;
                                        unbusy();
                                        break;
                                    case 7:
                                        if (classname[0]) strcpy(dirbuf,classname);
                                        else strcpy(dirbuf,"PROGDIR:S/DirectoryOpus.CLA");
                                        if (ptr=BaseName(dirbuf)) {
                                            strcpy(filebuf,ptr);
                                            *ptr=0;
                                        }
D(bug("filebuf: %s\t%s\n",dirbuf,filebuf));
                                        filereq.flags=0;
                                        filereq.title=cfg_string[STR_ENTER_NAME_TO_SAVE_CLASSES];
                                        filereq.window=Window;
                                        if (!(FileRequest(&filereq))) filebuf[0]=0;
                                        if (!filebuf[0]) break;
                                        strcpy(classname,dirbuf);
                                        TackOn(classname,filebuf,256);
                                        busy();
                                        if (savefileclasses()) classchange=0;
                                        unbusy();
                                        break;
                                    case 8:
                                        if (firstclass && (request(cfg_string[STR_ERASE_ALL_CLASSES]))) {
                                            freefileclasses();
                                            classchange=0;
                                        }
                                        break;
                                }
                                break;
                        }
                        break;
                    case GADGETUP:
                        if (gadgetid<FILETYPE_OKAY) {
                            if (type1) {
                                showtypelist(listptr);
                                type1=NULL;
                            }
                            if (mode==gadgetid) {
                                mode=-1;
                                dogadgetinfo(NULL);
                                break;
                            }
                            if (mode>-1) {
                                select_gadget(&maingad[mode],0);
                            }
                        }
                        switch (gadgetid) {
                            case FILETYPE_OKAY:
                            case FILETYPE_CANCEL:
                                if (classchange && !(request(cfg_string[STR_EXIT_WITHOUT_SAVING_CLASSES])))
                                    break;
                                if (gadgetid==FILETYPE_CANCEL) doundo(undo,UNDO_FILETYPE);
                                doundo(undo,0);
                                RemoveListView(&filetypeactionlist,1);
                                LFreeRemember(&key);
                                freefileclasses();
                                ClearMenuStrip(Window); projectmenu.NextMenu=NULL;
                                FSSetMenuStrip(Window,&projectmenu);
                                return((gadgetid==FILETYPE_OKAY));
                            case FILETYPE_NEW:
                                dogadgetinfo(NULL);
                                if (!(fclass=getfileclasslist(0))) break;
                                type=firsttype;
                                while (type) {
                                    if (strcmp(type->type,fclass->type)==0) break;
                                    type=type->next;
                                }
                                if (!type || request(cfg_string[STR_REDEFINE_EXISTING_CLASS_ACTION])) {
                                    if (type=AllocMem(sizeof(struct dopusfiletype),MEMF_CLEAR)) {
                                        strcpy(type->type,fclass->type);
                                        strcpy(type->typeid,fclass->typeid);
                                        type->recognition=getcopy(fclass->recognition,-1,&typekey);
                                        for (a=0;a<4;a++) {
                                            type->stack[a]=4000;
                                            type->delay[a]=2;
                                        }
                                        if (editfiletype(type,&key,1)) {
                                            if (tpos=(struct dopusfiletype *)
                                                    getcopy((char *)type,sizeof(struct dopusfiletype),&typekey)) {
                                                tpos->recognition=getcopy(type->recognition,-1,&typekey);
                                                for (a=0;a<4;a++)
                                                    tpos->function[a]=getcopy(type->function[a],-1,&typekey);
                                                addfiletype(tpos);
                                            }
                                        }
                                        FreeMem(type,sizeof(struct dopusfiletype));
                                        listptr=makefiletypelist(&key);
                                        showtypelist(listptr);
                                    }
                                }
                                break;
                        }
                        mode=doinitfiletypetext(gadgetid);
                        break;
                }
            }
            else if (view) {
                if (view->listid==2) {
                    if (view->itemselected>-1) {
                        type=firsttype;
                        for (a=0;a<view->itemselected;a++,type=type->next)
                            if (!type) break;
                        if (type) {
                            switch (mode) {
                                case -1:
                                    editfiletype(type,&key,0);
                                    listptr=makefiletypelist(&key);
                                    showtypelist(listptr);
                                    break;
                                case FILETYPE_SWAP:
                                    if (type1) {
                                        if (strcmp(type->type,"Default")==0)
                                            dogadgetinfo(cfg_string[STR_DEFAULT_ACTION_CAN_NOT_BE_MOVED]);
                                        else if (type!=type1) {
                                            SwapMem((char *)type->type,(char *)type1->type,sizeof(struct dopusfiletype)-4);
                                            listptr=makefiletypelist(&key);
                                            dogadgetinfo(cfg_string[STR_CLASS_ACTIONS_SWAPPED]);
                                        }
                                        else dogadgetinfo(NULL);
                                        showtypelist(listptr);
                                        type1=NULL;
                                    }
                                    else {
                                        if (strcmp(type->type,"Default")==0) {
                                            showtypelist(listptr);
                                            dogadgetinfo(cfg_string[STR_DEFAULT_ACTION_CAN_NOT_BE_MOVED]);
                                        }
                                        else {
                                            type1=type;
                                            dogadgetinfo(cfg_string[STR_SELECT_ACTION_TO_SWAP_WITH_FIRST]);
                                        }
                                    }
                                    break;
                                case FILETYPE_DELETE:
                                    if (request(cfg_string[STR_REALLY_DELETE_THIS_ACTION])) {
                                        if (type==firsttype) firsttype=type->next;
                                        else {
                                            tpos=firsttype;
                                            while (tpos) {
                                                if (tpos->next==type) {
                                                    tpos->next=type->next;
                                                    break;
                                                }
                                                tpos=tpos->next;
                                            }
                                        }
                                        dogadgetinfo(cfg_string[STR_ACTION_DELETED]);
                                        listptr=makefiletypelist(&key);
                                    }
                                    else dogadgetinfo(NULL);
                                    showtypelist(listptr);
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }
}
    
char **makefiletypelist(key)
struct DOpusRemember **key;
{
    char **list;
    int count,a;
    struct dopusfiletype *type;

    LFreeRemember(key);
    type=firsttype;
    for (count=0;type;count++,type=type->next);
    if (!(list=LAllocRemember(key,(count+1)*4,MEMF_CLEAR))) return(NULL);
    type=firsttype;
    for (a=0;a<count;a++) {
        if (list[a]=LAllocRemember(key,60,MEMF_CLEAR)) {
            if (type->type[0]) lsprintf(list[a],"%-50s%s",type->type,type->typeid);
            else {
                list[a][0]=' '; list[a][1]=0;
            }
        }
        type=type->next;
    }
    return(list);
}

void showtypelist(list)
char **list;
{
    filetypeactionlist.items=list; filetypeactionlist.itemselected=-1;
    RefreshListView(&filetypeactionlist,1);
}

int doinitfiletypetext(id)
int id;
{
    int mode=-1;

    switch (id) {
        case FILETYPE_SWAP:
            mode=FILETYPE_SWAP;
            dogadgetinfo(cfg_string[STR_SELECT_ACTION_TO_SWAP]);
            break;
        case FILETYPE_DELETE:
            mode=FILETYPE_DELETE;
            dogadgetinfo(cfg_string[STR_SELECT_ACTION_TO_DELETE]);
            break;
    }
    return(mode);
}

void filetypetitle(type)
struct dopusfiletype *type;
{
    char title[80];

    lsprintf(title,"%s : %s",cfg_string[STR_FILETYPE_CLASS],type->type);
    if (type->typeid[0]) {
        char extra[16];

        lsprintf(extra," (%s)",type->typeid);
        strcat(title,extra);
    }
    doscreentitle(title);
}

int editfiletype(type,key,new)
struct dopusfiletype *type;
struct DOpusRemember **key;
int new;
{
    struct dopusfunction editbuf;
    char *func[FILETYPE_FUNCNUM],*oldfunc[FILETYPE_FUNCNUM];
    int a,b;

    RemoveListView(&filetypeactionlist,1);
    cleanconfigscreen();
    filetypetitle(type);
    editbuf.function=NULL;
    for (a=0;a<FILETYPE_FUNCNUM;a++) {
        oldfunc[a]=type->function[a];
        func[a]=getcopy(type->function[a],-1,NULL);
        type->function[a]=func[a];
    }
    b=editfunction(&editbuf,CFG_FILETYPE,type);
    if (new) {
        if (!b) {
            for (a=0;a<FILETYPE_FUNCNUM;a++) {
                func[a]=oldfunc[a];
                freestring(type->function[a]);
                type->function[a]=func[a];
            }
        }
    }
    else {
        for (a=0;a<FILETYPE_FUNCNUM;a++) {
            if (b) func[a]=getcopy(type->function[a],-1,&typekey);
            else func[a]=oldfunc[a];
            freestring(type->function[a]);
            type->function[a]=func[a];
        }
    }
    showconfigscreen(CFG_FILETYPE);
    return(b);
}

int editclass(class,new)
struct fileclass *class;
int new;
{
    struct fileclass editbuf;
    int b,c;

    makehelpname(cfg_string[STR_EDIT_CLASS]);
    if (new==-1) {
        new=c=0;
    }
    else {
        c=1;
        RemoveListView(&filetypeactionlist,2);
    }
    cleanconfigscreen();
    doscreentitle(cfg_string[STR_FILE_CLASS_EDIT_SCREEN]);
    strcpy(editbuf.type,class->type);
    strcpy(editbuf.typeid,class->typeid);
    editbuf.recognition=getcopy(class->recognition,-1,NULL);
    if ((b=editfileclass(&editbuf,new))) {
        if (c) {
            if (!new) removefileclass(class);
            addfileclass(editbuf.type,editbuf.typeid,editbuf.recognition);
        }
        else {
            freestring(class->recognition);
            class->recognition=getcopy(editbuf.recognition,-1,NULL);
            strcpy(class->type,editbuf.type);
            strcpy(class->typeid,editbuf.typeid);
        }
    }
    if (c) showconfigscreen(CFG_FILETYPE);
    freestring(editbuf.recognition);
    makehelpname((char *)-1);
    return(b);
}

void addfiletype(type)
struct dopusfiletype *type;
{
    struct dopusfiletype *tpos;

    if (!(tpos=firsttype)) firsttype=type;
    else {
        while (tpos->next) {
            if (strcmp(tpos->next->type,"Default")==0) {
                type->next=tpos->next;
                break;
            }
            tpos=tpos->next;
        }
        tpos->next=type;
    }
}

void readfileclasses()
{
    BPTR in;
    int size,pos,lsize,a;
    char *classbuf,buf[256],*typeid;

    if ((CheckExist(classname,&size))>=0 ||
        !(classbuf=AllocMem(size,MEMF_CLEAR))) return;
    if (in=Open(classname,MODE_OLDFILE)) {
        Read(in,classbuf,size);
        Close(in);
        pos=0;
        FOREVER {
            if ((pos=readline(classbuf,pos,buf,size))==-1) break;
            typeid="";
            for (a=0;buf[a];a++) {
                if (buf[a]==1) {
                    buf[a]=0;
                    typeid=&buf[a+1];
                    break;
                }
            }
            for (lsize=pos;lsize<size;lsize++) if (!classbuf[lsize]) break;
            if (!(addfileclass(buf,typeid,&classbuf[pos]))) break;
            pos=lsize+1;
        }
    }
    FreeMem(classbuf,size);
}

int importfileclasses()
{
    BPTR in;
    int size,pos,lsize,num,a,b,tpos,ret=0;
    char *classbuf,**classlist,*classarray,**classtypeid,**classrecog,buf[256],buf2[256];
    struct DOpusRemember *key;

    filereq.flags=0;
    filereq.title=cfg_string[STR_SELECT_CLASS_FILE_TO_LOAD];
    strcpy(dirbuf,classname);
    if (classbuf=BaseName(dirbuf)) *classbuf=0;
    filebuf[0]=0;
    filereq.window=Window;
    if (!(FileRequest(&filereq)) || !filebuf[0]) return(0);
    strcpy(buf,dirbuf); TackOn(buf,filebuf,256);

    while (!(in=Open(buf,MODE_OLDFILE))) {
        lsprintf(buf2,cfg_string[STR_OPEN_FAILED],IoErr());
        if (!(request(buf2))) return(0);
    }

    key=NULL;

    if ((CheckExist(buf,&size))>=0 ||
        !(classbuf=LAllocRemember(&key,size+2,MEMF_CLEAR))) {
        Close(in);
        return(0);
    }

    Read(in,classbuf,size);
    Close(in);

    for (a=0,num=0;a<size;a++) if (!classbuf[a]) ++num;
    num/=2;

    if ((classlist=LAllocRemember(&key,(num+1)*4,MEMF_CLEAR)) &&
        (classtypeid=LAllocRemember(&key,(num+1)*4,MEMF_CLEAR)) &&
        (classrecog=LAllocRemember(&key,(num+1)*4,MEMF_CLEAR)) &&
        (classarray=LAllocRemember(&key,num+1,MEMF_CLEAR))) {
        pos=0; a=0;
        FOREVER {
            tpos=pos;
            if ((pos=readline(classbuf,pos,buf,size))==-1) break;
            classlist[a]=&classbuf[tpos];
            classtypeid[a]="";
            for (b=0;buf[b];b++) {
                if (buf[b]==1) {
                    classbuf[tpos+b]=0;
                    classtypeid[a]=&classbuf[tpos+b+1];
                    ++b;
                    break;
                }
            }
            for (;buf[b];b++);
            classrecog[a++]=&classbuf[tpos+b+1];
            for (lsize=tpos+b+1;lsize<size;lsize++)
                if (!classbuf[lsize]) break;
            pos=lsize+1;
        }
        if (classlist[0] &&
            (dolistwindow(cfg_string[STR_SELECT_CLASSES_TO_IMPORT],332,72,
                classlist,DLVF_MULTI|DLVF_HIREC,classarray,NULL))) {
            for (a=0;a<num;a++) {
                if (classarray[a]) {
                    if (!(addfileclass(classlist[a],classtypeid[a],classrecog[a]))) break;
                    ret=1;
                }
            }
        }
    }
    LFreeRemember(&key);
    return(ret);
}

int savefileclasses()
{
    struct fileclass *fclass;
    BPTR out;
    int a;
    char f=0,g=1,buf[100];

    if (!firstclass) return(1);
    if (!classname[0]) strcpy(classname,"PROGDIR:S/DirectoryOpus.CLA");
D(bug("classname: %s\n",classname));
    while (!(out=Open(classname,MODE_NEWFILE))) {
        lsprintf(buf,cfg_string[STR_SAVE_FAILED],IoErr());
        if (!(request(buf))) return(0);
    }
    fclass=firstclass;
    while (fclass) {
        if (fclass->type[0]) {
            a=strlen(fclass->type);
            if (Write(out,fclass->type,a)<a) break;
            if (fclass->typeid[0]) {
                Write(out,&g,1);
                a=strlen(fclass->typeid)
                if (Write(out,fclass->typeid,a)<a) break;
            }
        }
        Write(out,&f,1);
        if (fclass->recognition) {
            a=strlen(fclass->recognition)+1;
            if (Write(out,fclass->recognition,a)<a)
                break;
        }
        else Write(out,&f,1);
        fclass=fclass->next;
    }
    Close(out);
    return(1);
}

int addfileclass(type,typeid,recog)
char *type,*typeid,*recog;
{
    struct fileclass *fclass,*newclass,*last;

    fclass=firstclass;
    while (fclass) {
        if (LStrCmpI(fclass->type,type)>=0) break;
        if (!fclass->next) last=fclass;
        fclass=fclass->next;
    }
    if (!(newclass=AllocMem(sizeof(struct fileclass),MEMF_CLEAR))) return(0);
    if (fclass) {
        if (fclass->last) {
            fclass->last->next=newclass;
            newclass->last=fclass->last;
        }
        else if (fclass==firstclass) firstclass=newclass;
        newclass->next=fclass;
        fclass->last=newclass;
    }
    else if (firstclass) {
        last->next=newclass;
        newclass->last=last;
    }
    else firstclass=newclass;
    strcpy(newclass->type,type);
    strcpy(newclass->typeid,typeid);
    if (recog && (newclass->recognition=AllocMem(strlen(recog)+1,MEMF_CLEAR)))
        strcpy(newclass->recognition,recog);
    return(1);
}

void freefileclasses()
{
    struct fileclass *fclass,*newclass;

    fclass=firstclass;
    while (fclass) {
        newclass=fclass->next;
        freestring(fclass->recognition);
        FreeMem(fclass,sizeof(struct fileclass));
        fclass=newclass;
    }
    firstclass=NULL;
}

void removefileclass(fclass)
struct fileclass *fclass;
{
    if (fclass->last) fclass->last->next=fclass->next;
    if (fclass->next) fclass->next->last=fclass->last;
    if (fclass==firstclass) firstclass=fclass->next;
    freestring(fclass->recognition);
    FreeMem(fclass,sizeof(struct fileclass));
}

char **makeclasslist(key)
struct DOpusRemember **key;
{
    char **list;
    int count,a;
    struct fileclass *fclass;

    LFreeRemember(key);
    if (!firstclass) return(NULL);
    fclass=firstclass;
    for (count=0;fclass;count++,fclass=fclass->next);
    if (!(list=LAllocRemember(key,(count+1)*4,MEMF_CLEAR))) return(NULL);
    fclass=firstclass;
    for (a=0;a<count;a++) {
        if (list[a]=LAllocRemember(key,40,MEMF_CLEAR)) strcpy(list[a],fclass->type);
        fclass=fclass->next;
    }
    return(list);
}

int readline(buf,pos,buf1,size)
char *buf;
int pos;
char *buf1;
int size;
{
    int a;

    for (a=0;a<256;a++) {
        if (size==pos || buf[pos]==0) {
            buf1[a]=0;
            if (size==pos) return(-1);
            return(pos+1);
        }
        buf1[a]=buf[pos];
        ++pos;
    }
    buf1[pos]=0;
    return(pos);
}

int editfileclass(fclass,new)
struct fileclass *fclass;
int new;
{
    int a,b,old,selitem,lasta=-1,x,y,mx,my,off,temp,waitbits,remapp=0,char_w;
    ULONG class,sec,mic,oldsec,oldmic;
    UWORD code,gadgetid,qual;
    struct Gadget *gad;
    struct DOpusListView *view;
    struct DOpusRemember *key=NULL;
    char
        *classlist[MAXFUNCS+1],classtype[MAXFUNCS],*displist[MAXFUNCS+1],
        *templist,temptype,buf[16];
    unsigned char c;

    for (a=0;a<MAXFUNCS;a++) {
        displist[a]=LAllocRemember(&key,80,MEMF_CLEAR);
        classlist[a]=NULL;
    }
    makeeditclasslist(fclass,classlist,classtype);
    dispclasslist(classlist,classtype,displist);

    namesinfo.MaxChars=32;
    strcpy(edit_namebuf,fclass->type);
    strcpy(edit_typeidbuf,fclass->typeid);
    edit_funcbuf[0]=0; edit_pathbuf[0]=0;
    editclassgadgets[4].Flags&=~SELECTED;

    AddGadgetBorders(&key,
        &editclassgadgets[2],
        4,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgetBorders(&key,
        &editclassgadgets[6],
        3,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgetBorders(&key,
        &editclassgadgets[10],
        1,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);
    AddGadgetBorders(&key,
        &editclassgadgets[12],
        2,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen);

    SetAPen(rp,screen_pens[1].pen);
    AddGadgets(Window,
        editclassgadgets,
        editclassgads,
        14,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
    doglassimage(&editclassgadgets[6]);
    doglassimage(&editclassgadgets[8]);
    Do3DBox(rp,
        x_off+37,y_off+140,
        116,12,
        screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);

    fileview_buf=NULL; fileview_lines=fileview_topline=0; fileview_type=1;
    char_w = rp->Font->tf_XSize;
    draw_file_view();

    showclassop(0);
    seteditclassgads(0);
    setuplist(&editclasslist,7,88);
    editclasslist.topitem=0; editclasslist.itemselected=-1;
    editclasslist.items=displist;
    AddListView(&editclasslist,1);
    initsidegads(editfuncgads,0,0);
    ClearMenuStrip(Window);
    Window->Flags|=WFLG_RMBTRAP;
    selitem=-1;
    if (new) ActivateStrGad(&editclassgadgets[0],Window);
    oldsec=oldmic=0;

    if (!appobject) {
        add_appobject(0);
        remapp=1;
    }

    waitbits=1<<Window->UserPort->mp_SigBit;
    if (appobject) waitbits|=1<<appport->mp_SigBit;

    FOREVER {
        if (appobject) {
            while (appmsg=(struct AppMessage *)GetMsg(appport)) {
                ActivateWindow(Window);
                if (appmsg->am_ID==MY_APPOBJECT &&
                    appmsg->am_NumArgs>0 &&
                    (*appmsg->am_ArgList[0].wa_Name)) {
                    PathName(appmsg->am_ArgList[0].wa_Lock,edit_pathbuf,256);
                    TackOn(edit_pathbuf,appmsg->am_ArgList[0].wa_Name,256);
                    RefreshStrGad(&editclassgadgets[9],Window);
                    load_file_view();
                }
                ReplyMsg((struct Message *)appmsg);
            }
        }
        while (IMsg=getintuimsg()) {
            if ((view=ListViewIDCMP(&editclasslist,IMsg))==(struct DOpusListView *)-1) {
                class=IMsg->Class; code=IMsg->Code; qual = IMsg->Qualifier;
                mx=IMsg->MouseX; my=IMsg->MouseY;
                sec=IMsg->Seconds; mic=IMsg->Micros;
                if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN) {
                    gad=(struct Gadget *)IMsg->IAddress;
                    gadgetid=gad->GadgetID;
                }
                ReplyMsg((struct Message *)IMsg);
                switch (class) {
                    case IDCMP_MOUSEBUTTONS:
                        if (code!=SELECTDOWN ||
                            mx<x_off+11 || mx>x_off+505 ||
                            my<y_off+21 || my>x_off+68) break;
                        if (oldsec && DoubleClick(oldsec,oldmic,sec,mic)) off=1;
                        else off=0;
                        oldsec=sec; oldmic=mic;
                        FOREVER {
                            if (my>y_off+20 && my<y_off+69) {

                                x=((mx-x_off-11)/char_w)-10;
                                if (x<0) x=0;
                                if (x<36) {
                                    if ((x%9)==8) --x;
                                    a=x/9; x=(a*4)+((x%9)/2);
                                }
                                else if (x>35) {
                                    x-=36;
                                    if (x>15) x=15;
                                }

                                y=((my-y_off-21)/8)+fileview_topline;
                                if (y<fileview_topline) y=fileview_topline;

                                if (off) {
                                    a=(y*16)+x;
                                    if (a==fileview_position) {
                                        old=fileview_offset;
                                        if ((fileview_offset=a)<0 ||
                                            fileview_offset>=fileview_size) fileview_offset=-1;
                                        a=0; b=1;
                                    }
                                    else {
                                        a=-1;
                                        off=0;
                                    }
                                }
                                else {
                                    old=fileview_position;
                                    if ((fileview_position=(y*16)+x)<0 ||
                                        fileview_position>=fileview_size) fileview_position=-1;
                                    a=fileview_position; b=0;
                                    if (fileview_offset>fileview_position) show_view_number(-1,1);
                                    else if (fileview_offset>-1)
                                        show_view_number(fileview_position-fileview_offset,1);
                                }
                                if (a>-1 && a!=old) {
                                    fileview_oldtop=-1;
                                    show_file_view();
                                    show_view_number(a,b);
                                }
                            }
                            if (off) break;
                            class=0;
                            while (IMsg=getintuimsg()) {
                                if (IMsg->Class==IDCMP_MOUSEMOVE || IMsg->Class==IDCMP_MOUSEBUTTONS)
                                    class=IMsg->Class;
                                code=IMsg->Code;
                                ReplyMsg((struct Message *)IMsg);
                                if (class==IDCMP_MOUSEBUTTONS && code==SELECTUP) break;
                            }
                            if (class==IDCMP_MOUSEBUTTONS && code==SELECTUP) break;
                            mx=Window->MouseX;
                            my=Window->MouseY;
                            if (my<y_off+21) {
                                my=y_off+21;
                                if (fileview_topline>0) {
                                    --fileview_topline; fileview_oldtop=fileview_position=-1;
                                    show_file_view();
                                    FixSliderPot(Window,&editclassgadgets[11],fileview_topline,fileview_lines,6,1);
                                }
                            }
                            else if (my>y_off+68) {
                                my=y_off+68;
                                if (fileview_topline<fileview_lines-6) {
                                    ++fileview_topline; fileview_oldtop=fileview_position=-1;
                                    show_file_view();
                                    FixSliderPot(Window,&editclassgadgets[11],fileview_topline,fileview_lines,6,1);
                                }
                            }
                            else if (class!=IDCMP_MOUSEMOVE) Wait(1<<Window->UserPort->mp_SigBit);
                        }
                        break;

                    case IDCMP_RAWKEY:
                        switch (code) {
                            case CURSORUP:
                                if (fileview_position>15) cursor_fileview(-16);
                                break;
                            case CURSORDOWN:
                                if (fileview_position<fileview_size-16) cursor_fileview(16);
                                break;
                            case CURSORLEFT:
                                if (fileview_position) cursor_fileview(-1);
                                break;
                            case CURSORRIGHT:
                                if (fileview_position<fileview_size-1) cursor_fileview(1);
                                break;
                            default:
                             {
                              buf[0]=0;
                              RawkeyToStr(code,qual,NULL,buf,10);
                              c = LToLower(buf[0]);
                              if (c == getkeyshortcut(cfg_string[STR_EDITCLASS_FILECLASS]))
                                ActivateStrGad(&editclassgadgets[0],Window);
                              else if (c == getkeyshortcut(cfg_string[STR_EDITCLASS_CLASSID]))
                                ActivateStrGad(&editclassgadgets[1],Window);
                              else if (c == getkeyshortcut(cfg_string[STR_EDITCLASS_FILEVIEWER]))
                                goto getfileview;
                             }
                        }
                        break;

                    case IDCMP_GADGETDOWN:
                        switch (gadgetid) {
                            case CLASS_VIEWUP:
                            case CLASS_VIEWDOWN:
                            case CLASS_VIEWSLIDER:
                                temp=0;
                                FOREVER {
                                    if (gadgetid==CLASS_VIEWUP) {
                                        if (fileview_topline==0) break;
                                        --fileview_topline;
                                    }
                                    else if (gadgetid==CLASS_VIEWDOWN) {
                                        if (fileview_topline>=fileview_lines-6) break;
                                        ++fileview_topline;
                                    }
                                    else {
                                        fileview_topline=GetSliderPos(&editclassgadgets[11],fileview_lines,6);
                                    }
                                    if (fileview_topline!=fileview_oldtop) show_file_view();
                                    if (gadgetid!=CLASS_VIEWSLIDER) {
                                        FixSliderPot(Window,&editclassgadgets[11],fileview_topline,fileview_lines,6,1);
                                        if (!temp++) Delay(10);
                                    }
                                    while (IMsg=getintuimsg()) {
                                        class=IMsg->Class; code=IMsg->Code;
                                        ReplyMsg((struct Message *)IMsg);
                                        if (class==IDCMP_MOUSEBUTTONS && code==SELECTUP)
                                            class=IDCMP_GADGETUP;
                                        if (class==IDCMP_GADGETUP) break;
                                    }
                                    if (class==IDCMP_GADGETUP) break;
                                }
                                ShowSlider(Window,&editclassgadgets[11]);
                                break;
                        }
                        break;

                    case IDCMP_GADGETUP:
                        switch (gadgetid) {
                            case CLASS_OKAY:
                                makeclassrecog(fclass,classlist,classtype);
                                strcpy(fclass->type,edit_namebuf);
                                strcpy(fclass->typeid,edit_typeidbuf);
                            case CLASS_CANCEL:
                                if (editclassgadgets[3].Flags&GFLG_DISABLED) seteditclassgads(1);
                                RemoveListView(&editclasslist,1);
                                LFreeRemember(&key);
                                freefunclist(classlist);
                                Window->Flags&=~WFLG_RMBTRAP;
                                FSSetMenuStrip(Window,&projectmenu);
                                if (remapp) rem_appobject(0);
                                return((gadgetid==CLASS_OKAY));

                            case CLASS_OPERATION:
                                checkclassswap();
                                if ((a=funcrequester(FREQ_FILETYPE,
                                    NULL,cfg_string[STR_LIST_OF_FILETYPE_COMMANDS]))) {
                                    classtype[selitem]=a;
                                    showclassop(classtype[selitem]);
                                    ActivateStrGad(&editclassgadgets[7],Window);
                                }
                                break;

                            case CLASS_DELETE:
                                edit_funcbuf[0]=0; classtype[selitem]=0;
                            case CLASS_FUNC:
                                lasta=-1;
                            case CLASS_NEWENTRY:
                                checkclassswap();
                                a=-1;
                                if (selitem>-1) {
                                    if (gadgetid==CLASS_NEWENTRY && !edit_funcbuf[0]) {
                                        ActivateStrGad(&editclassgadgets[7],Window);
                                        break;
                                    }
                                    editclasslist.itemselected=-1;
                                    if (!edit_funcbuf[0]) lasta=-1;
                                    endclassedit(selitem,classlist,classtype,displist);
                                    if (lasta==-1) a=selitem;
                                    else a=lasta+1;
                                    selitem=-1;
                                }
                                if (gadgetid==CLASS_NEWENTRY) {
                                    if (editclasslist.count>=MAXFUNCS) break;
                                    if (a>-1) selitem=a;
                                    else {
                                        selitem=editclasslist.count;
                                        seteditclassgads(1);
                                    }
                                    insertnewclass(classlist,classtype,selitem,displist," ",2);
                                    lasta=selitem;
                                }
                                else seteditclassgads(0);
                                break;

                            case CLASS_DUPLICATE:
                                checkclassswap();
                                if (editclasslist.count>=MAXFUNCS || !edit_funcbuf[0]) {
                                    ActivateStrGad(&editclassgadgets[7],Window);
                                    break;
                                }
                                editclasslist.itemselected=-1;
                                endclassedit(selitem,classlist,classtype,displist);
                                a=selitem; selitem=editclasslist.count;
                                insertnewclass(classlist,classtype,selitem,displist,classlist[a],classtype[a]);
                                break;

                            case CLASS_FILEVIEWREQ:
getfileview:
                                if (!(funcrequester(FREQ_GENERIC,edit_pathbuf,NULL))) break;
                                RefreshStrGad(&editclassgadgets[9],Window);
                            case CLASS_FILEVIEW:
                                if (code!=0x9) load_file_view();
                                break;
                            case CLASS_HEXDEC:
                                fileview_type=1-fileview_type;
                                DoCycleGadget(&editclassgadgets[10],Window,fileview_types,fileview_type);
                                show_view_number(fileview_position,0);
                                if (fileview_offset>fileview_position) show_view_number(-1,1);
                                else if (fileview_offset>-1)
                                    show_view_number(fileview_position-fileview_offset,1);
                                break;
                            case CLASS_TYPE:
                                ActivateStrGad(&editclassgadgets[1],Window);
                                break;
                            case CLASS_TYPEID:
                                StrToUpper(edit_typeidbuf,edit_typeidbuf);
                                RefreshStrGad(&editclassgadgets[1],Window);
                                break;
                        }
                        break;
                }
            }
            else if (view) {
                do {
                    if (selitem!=-1) {
                        endclassedit(selitem,classlist,classtype,displist);
                        if (editclassgadgets[4].Flags&SELECTED) {
                            checkclassswap();
                            seteditclassgads(0);
                            templist=classlist[selitem];
                            classlist[selitem]=classlist[view->itemselected];
                            classlist[view->itemselected]=templist;
                            temptype=classtype[selitem];
                            classtype[selitem]=classtype[view->itemselected];
                            classtype[view->itemselected]=temptype;
                            dispclasslist(classlist,classtype,displist);
                            editclasslist.itemselected=-1;
                            RefreshListView(&editclasslist,1);
                            selitem=-1;
                            break;
                        }
                    }
                    else seteditclassgads(1);
                    selitem=view->itemselected;
                    if (classlist[selitem]) strcpy(edit_funcbuf,classlist[selitem]);
                    else edit_funcbuf[0]=0;
//kprintf("selitem: %ld\tedit_funcbuf: %s\n",selitem,edit_funcbuf);
                    RefreshStrGad(&editclassgadgets[7],Window);
                    showclassop(classtype[selitem]);
                    ActivateStrGad(&editclassgadgets[7],Window);
                }
                while (0);
            }
        }
        Wait(waitbits);
    }
}

void makeeditclasslist(class,classlist,classtype)
struct fileclass *class;
char **classlist,*classtype;
{
    int a,b,len,type,pos,num;
    char buf[256];

    if (!class->recognition) return;
    len=strlen(class->recognition);
    type=-1; pos=num=0;
    for (a=0;a<=len;a++) {
        if (type==-1) {
            if (class->recognition[a]<FTYC_COMMANDOK) {
                type=class->recognition[a];
                continue;
            }
            if (class->recognition[a]>FTYC_ENDLIMIT &&
                class->recognition[a]<FTYC_ENDSECTION) {
doandor:
                classlist[num]=getcopy(" ",2,NULL);
                for (b=0;b<12;b++) {
                    if (classopvals[b]==class->recognition[a]) {
                        classtype[num]=b+1;
                        break;
                    }
                }
                if ((++num)==MAXFUNCS) break;
            }
        }
        else if (pos==255 || class->recognition[a]>FTYC_ENDLIMIT ||
            !class->recognition[a]) {
            buf[pos]=0;
            if (classlist[num]=getcopy(buf,-1,NULL)) {
                for (b=0;b<12;b++) {
                    if (classopvals[b]==type) {
                        classtype[num]=b+1;
                        break;
                    }
                }
                if ((++num)==MAXFUNCS) break;
            }
            pos=0; type=-1;
            if (class->recognition[a]>FTYC_ENDLIMIT &&
                class->recognition[a]<FTYC_ENDSECTION) goto doandor;
        }
        else buf[pos++]=class->recognition[a];
    }
    for (a=num;a<MAXFUNCS;a++) classlist[a]=NULL;
}

void dispclasslist(classlist,classtype,displist)
char **classlist,*classtype,**displist;
{
    int a;
    char buf[80],*ptr;

    for (a=0;a<MAXFUNCS;a++) {
        if (!classlist[a]) break;
        strcpy(buf,classopslist[classtype[a]-1]);
        ptr=strchr(buf,' '); *ptr=0;
        StrCombine(displist[a],buf,spacestring,15);
        if (classopvals[classtype[a]-1]<FTYC_COMMANDOK) StrConcat(displist[a],classlist[a],75);
    }
    for (;a<MAXFUNCS;a++) displist[a][0]=0;
}

void seteditclassgads(on)
int on;
{
    int a;

    if (on) {
        for (a=3;a<6;a++) EnableGadget(&editclassgadgets[a],rp,4,2);
        EnableGadget(&editclassgadgets[6],rp,4,2);
        EnableGadget(&editclassgadgets[7],rp,0,0);
    }
    else {
        for (a=3;a<6;a++) DisableGadget(&editclassgadgets[a],rp,4,2);
        DisableGadget(&editclassgadgets[6],rp,4,2);
        DisableGadget(&editclassgadgets[7],rp,0,0);
    }
}

void removeclassentry(classlist,classtype,entry)
char **classlist,*classtype;
int entry;
{
    if (entry<MAXFUNCS-1) {
        CopyMem(&classlist[entry+1],&classlist[entry],(MAXFUNCS-1-entry)*4);
        CopyMem(&classtype[entry+1],&classtype[entry],(MAXFUNCS-1-entry));
    }
    classlist[MAXFUNCS-1]=NULL;
}

void insertnewclass(classlist,classtype,entry,displist,string,type)
char **classlist,*classtype;
int entry;
char **displist,*string;
int type;
{
    char *templist[MAXFUNCS],temptype[MAXFUNCS];
    int a;

    a=MAXFUNCS-entry-1;
    CopyMem(&classlist[entry],templist,a*4);
    CopyMem(templist,&classlist[entry+1],a*4);
    classlist[entry]=getcopy(string,-1,NULL);
    CopyMem(&classtype[entry],temptype,a);
    CopyMem(temptype,&classtype[entry+1],a);
    classtype[entry]=type;
    dispclasslist(classlist,classtype,displist);
    if (entry<editclasslist.topitem ||
        entry>=editclasslist.topitem+editclasslist.lines) editclasslist.topitem=entry;
    editclasslist.itemselected=entry;
    RefreshListView(&editclasslist,1);
    if (string[0]==' ' && string[1]==0) edit_funcbuf[0]=0;
    else strcpy(edit_funcbuf,string);
    RefreshStrGad(&editclassgadgets[7],Window);
    showclassop(type);
    ActivateStrGad(&editclassgadgets[7],Window);
}

void endclassedit(item,classlist,classtype,displist)
int item;
char **classlist,*classtype,**displist;
{
//kprintf("classtype[item]: %ld\tbuf: %s\n",classtype[item],edit_funcbuf);
    freestring(classlist[item]);
    if (classtype[item]<1 || classopvals[classtype[item]-1]<FTYC_COMMANDOK) {
        if (edit_funcbuf[0]) classlist[item]=getcopy(edit_funcbuf,-1,NULL);
        else removeclassentry(classlist,classtype,item);
    }
    else classlist[item]=getcopy(" ",2,NULL);
    dispclasslist(classlist,classtype,displist);
    RefreshListView(&editclasslist,1);
    edit_funcbuf[0]=0;
    RefreshStrGad(&editclassgadgets[7],Window);
    showclassop(0);
}

void makeclassrecog(class,classlist,classtype)
struct fileclass *class;
unsigned char **classlist,*classtype;
{
    int size,num,a;
    unsigned char *buf,buf2[2];

    size=num=0;
    for (a=0;a<MAXFUNCS;a++) {
        if (!classlist[a]) break;
        size+=strlen(classlist[a])+2;
        ++num;
    }
    freestring(class->recognition); class->recognition=NULL;
    if (!size) return;
    if (!(buf=AllocMem(size+2,MEMF_CLEAR))) return;
    buf2[1]=0;
    for (a=0;a<num;a++) {
        buf2[0]=classopvals[classtype[a]-1];
        strcat(buf,buf2);
        if (buf2[0]<FTYC_COMMANDOK) {
            strcat(buf,classlist[a]);
            if (a==(num-1) || classopvals[classtype[a+1]-1]<FTYC_COMMANDOK) {
                buf2[0]=FTYC_ENDSECTION;
                strcat(buf,buf2);
            }
        }
    }
    class->recognition=getcopy(buf,-1,NULL);
    FreeMem(buf,size+2);
}

void checkclassswap()
{
    if (editclassgadgets[4].Flags&SELECTED) {
        editclassgadgets[4].Flags&=~SELECTED;
        select_gadget(&editclassgadgets[4],0);
    }
}

void showclassop(op)
int op;
{
    char str[80],*ptr;
    int x,a;

    SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,
        x_off+37,y_off+140,
        x_off+152,y_off+151);
    SetAPen(rp,screen_pens[1].pen);
    if (op<1) ptr="----";
    else {
        strcpy(str,classopslist[op-1]);
        ptr=strchr(str,' '); *ptr=0;
        ptr=str;
    }
    a=strlen(ptr);
    x=((116-(a*8))/2)+x_off+37;
    Move(rp,x,y_off+148);
    Text(rp,ptr,a);
}

struct fileclass *getfileclasslist(type)
int type;
{
    struct DOpusRemember *key=NULL;
    int a;
    struct fileclass *fclass=NULL;

    fileclasslist=makeclasslist(&key);
    if ((a=funcrequester(FREQ_FILECLASS,NULL,fileclasstype[type]))) {
        fclass=firstclass;
        while ((--a) && fclass && fclass->next) fclass=fclass->next;
    }
    LFreeRemember(&key);
    return(fclass);
}

void draw_file_view()
{
    int a;

    SetAPen(rp,screen_pens[0].pen);
    RectFill(rp,
        y_off+7,y_off+21,
        x_off+511,y_off+68);
    do3dbox(x_off+7,y_off+21,505,48);
    Do3DBox(rp,
        x_off+534,y_off+31,
        91,12,
        screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);
    Do3DBox(rp,
        x_off+534,y_off+57,
        91,12,
        screen_pens[config->gadgetbotcol].pen,screen_pens[config->gadgettopcol].pen);
    SetAPen(rp,screen_pens[1].pen);
    UScoreText(rp,
        cfg_string[STR_POSITION],
        x_off+535,y_off+27,
        -1);
    UScoreText(rp,
        cfg_string[STR_OFFSET],
        x_off+535,y_off+53,
        -1);
    for (a=0;a<2;a++)
        DoArrow(rp,
            editclassgadgets[12+a].LeftEdge+2,editclassgadgets[12+a].TopEdge+1,
            12,6,
            screen_pens[1].pen,screen_pens[0].pen,a);
    fix_slider(&editclassgadgets[11]);
    FixSliderBody(Window,&editclassgadgets[11],fileview_lines,6,0);
    FixSliderPot(Window,&editclassgadgets[11],fileview_topline,fileview_lines,6,0);
    ShowSlider(Window,&editclassgadgets[11]);
    DoCycleGadget(&editclassgadgets[10],Window,fileview_types,fileview_type);
}

void free_file_view()
{
    int a;

    if (fileview_buf) {
        FreeMem(fileview_buf,fileview_size);
        fileview_buf=NULL;
    }
    fileview_size=fileview_lines=fileview_topline=0;
    fileview_oldtop=fileview_offset=fileview_position=-1;
    for (a=0;a<2;a++) show_view_number(-1,a);
}

void load_file_view()
{
    BPTR file;
    int rsize;

    free_file_view();
    busy();
    if ((CheckExist(edit_pathbuf,&fileview_size))<0 &&
        (file=Open(edit_pathbuf,MODE_OLDFILE))) {
        if (fileview_size>4096) fileview_size=4096;
        else if (fileview_size%16) fileview_size=((fileview_size+15)/16)*16;
        if (fileview_buf=AllocMem(fileview_size,MEMF_CLEAR)) {
            rsize=Read(file,fileview_buf,fileview_size);
            fileview_lines=(rsize+15)/16;
        }
        Close(file);
    }
    draw_file_view();
    if (!fileview_buf) {
        DisplayBeep(NULL);
        file_view_text(cfg_string[STR_FILE_NOT_FOUND],0);
    }
    else show_file_view();
    unbusy();
}

void file_view_text(txt,line)
char *txt;
int line;
{
    int len;

    SetAPen(rp,screen_pens[1].pen);
    SetBPen(rp,screen_pens[0].pen);
    Move(rp,x_off+9,y_off+27+(line*8));
    if (txt) Text(rp,txt,(len=strlen(txt)));
    else len=0;
    if (len<62) Text(rp,spacestring,62-len);
}

void show_file_view(void)
{
    int line,a,off,old,top,bottom,scroll,ox,px,aox,apx,char_w;
    char buf[80],buf2[30];

    top=0; bottom=5; scroll=0;
    if (fileview_oldtop>-1) {
        if ((a=fileview_topline-fileview_oldtop)>0 && a<6) {
            top=6-a;
            scroll=a*8;
        }
        else if (a<0 && a>-6) {
            bottom=-a-1;
            scroll=a*8;
        }
    }
    char_w = rp->Font->tf_XSize;
    SetDrMd(rp,JAM2);

    for (line=0;line<6;line++) {
        if (line>=top && line<=bottom) {
            if (scroll) {
                ScrollRaster(rp,0,scroll,x_off+7,y_off+21,x_off+511,y_off+68);
                scroll=0;
            }
            if (fileview_lines<=line+fileview_topline) file_view_text(NULL,line);
            else {
                off=(fileview_topline+line)*16;
                lsprintf(buf,"%08lx: ",(long unsigned int)off);
                old=off; ox=px=-1;
                for (a=0;a<4;a++) {
/*                    lsprintf(buf2,"%02lx%02lx%02lx%02lx ",
                        fileview_buf[off++],fileview_buf[off++],fileview_buf[off++],fileview_buf[off++]);
*/
                    lsprintf(buf2,"%08lx ",*((long *)(fileview_buf+off)));
                    off += 4;

                    strcat(buf,buf2);
                    if (px==-1 && fileview_position>=off-4 && fileview_position<off) {
                        apx=fileview_position-(off-4);
                        px=(a*9)+(apx*2); apx+=(a*4)+36;
                    }
                    if (ox==-1 && fileview_offset>=off-4 && fileview_offset<off) {
                        aox=fileview_offset-(off-4);
                        ox=(a*9)+(aox*2); aox+=(a*4)+36;
                    }
                }
                off=old;
                for (a=0;a<16;a++,off++)
                    buf[46+a]=(isprint(fileview_buf[off]))?fileview_buf[off]:'.';
                buf[62]=0;
                file_view_text(buf,line);
                if (px>-1 && px!=ox) {
                    SetDrMd(rp,COMPLEMENT);
                    RectFill(rp,
                        x_off+9+(10+px)*char_w,y_off+21+(line*8),
                        x_off+8+(12+px)*char_w,y_off+28+(line*8));
                    RectFill(rp,
                        x_off+9+(10+apx)*char_w,y_off+21+(line*8),
                        x_off+8+(11+apx)*char_w,y_off+28+(line*8));
                    SetDrMd(rp,JAM2);
                }
                if (ox>-1) {
                    SetAPen(rp,screen_pens[2].pen);
                    lsprintf(buf2,"%02lx",(long unsigned int)fileview_buf[fileview_offset]);
                    Move(rp,x_off+9+(10+ox)*char_w,y_off+27+(line*8));
                    Text(rp,buf2,2);
                    buf2[0]=(isprint(fileview_buf[fileview_offset])?fileview_buf[fileview_offset]:'.');
                    Move(rp,x_off+9+(10+aox)*char_w,y_off+27+(line*8));
                    Text(rp,buf2,1);
                    SetAPen(rp,screen_pens[1].pen);
                }
            }
        }
    }
    fileview_oldtop=fileview_topline;
}

void show_view_number(num,pos)
int num,pos;
{
    char buf[20];
    int y;

    if (num<0) {
        SetAPen(rp,screen_pens[0].pen);
        RectFill(rp,
            x_off+534,y_off+31+(26*pos),
            x_off+624,y_off+42+(26*pos));
        SetAPen(rp,screen_pens[1].pen);
    }
    else {
        y=y_off+39+(26*pos);
        if (fileview_type==0) lsprintf(buf,"$%08lx",(long unsigned int)num);
        else lsprintf(buf,"%09ld",(long int)num);
        UScoreText(rp,buf,x_off+543,y,-1);
    }
}

void cursor_fileview(delta)
int delta;
{
    int ups=0;

    fileview_position+=delta;
    if (fileview_position<0) fileview_position=0;
    else if (fileview_position>=fileview_size) fileview_position=fileview_size-1;

    if (fileview_position<(fileview_topline*16)) {
        fileview_oldtop=-1;
        while (fileview_position<(fileview_topline*16)) {
            if (!fileview_topline) break;
            --fileview_topline;
        }
        ups=1;
    }
    else if (fileview_position>=((fileview_topline+6)*16)) {
        fileview_oldtop=-1;
        while (fileview_position>=((fileview_topline+6)*16)) {
            if (fileview_topline>=fileview_lines-6) break;
            ++fileview_topline;
        }
        ups=1;
    }
    show_file_view();
    show_view_number(fileview_position,0);
    if (fileview_offset>fileview_position) show_view_number(-1,1);
    else if (fileview_offset>-1)
        show_view_number(fileview_position-fileview_offset,1);
    if (ups) FixSliderPot(Window,&editclassgadgets[11],fileview_topline,fileview_lines,6,1);
}
