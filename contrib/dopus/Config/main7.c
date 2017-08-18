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

struct Gadget busygadget={
    NULL,
    0,0,0,0,
    GFLG_GADGHNONE,0,GTYP_BOOLGADGET,
    NULL,NULL,NULL,0,NULL,0,NULL};
static int oldidcmp,busyflag;

void busy()
{
    if (!busyflag) {
        busygadget.TopEdge=Window->BorderTop;
        busygadget.LeftEdge=Window->BorderLeft;
        busygadget.Width=Window->Width-Window->BorderLeft-Window->BorderRight;
        busygadget.Height=Window->Height-Window->BorderTop-Window->BorderBottom;
        AddGList(Window,&busygadget,0,1,NULL);
        oldidcmp=Window->IDCMPFlags;
        ModifyIDCMP(Window,IDCMP_CLOSEWINDOW);
        Window->Flags|=WFLG_RMBTRAP;
        SetBusyPointer(Window);
        busyflag=1;
    }
}

void unbusy()
{
    if (busyflag) {
        RemoveGList(Window,&busygadget,1);
        ModifyIDCMP(Window,oldidcmp);
        Window->Flags&=~WFLG_RMBTRAP;
        ClearPointer(Window);
        busyflag=0;
    }
}

void dosave(new)
int new;
{
    char buf[80];

    busy();
    if (new) {
trysave:
        makefilebuffer(configname);
        filereq.flags=DFRF_SAVE;
        filereq.title=cfg_string[STR_ENTER_CONFIGURATION_FILENAME];
        filereq.window=Window;
        if (!(FileRequest(&filereq))) {
            unbusy();
            return;
        }
        strcpy(configname,dirbuf);
        TackOn(configname,filebuf,256);
        if (!(stricmp(configname,"DirectoryOpus.DefCFG"))) if (!(strstri(configname,".CFG"))) StrConcat(configname,".CFG",256);
    }
    fixcstuff(&cstuff);
    if (!SaveConfig(configname,&cstuff)) {
        lsprintf(buf,cfg_string[STR_SAVE_FAILED],IoErr());
        if (request(buf)) goto trysave;
    }
    else lchanged=changed=0;
    unbusy();
}

int doload(type,def)
int type,def;
{
    char *path,buf[80],*menulist[6],menuarray[6],**ftypelist,*typearray;
    struct DOpusRemember *fkey;
    struct ConfigStuff *newcstuff;
    struct dopusgadgetbanks *bank,*bank2,*nbank;
    struct dopusfiletype *ftype,*ntype;
    struct dopushotkey *fhotkey,*nhotkey,*curhotkey;
    int a,b,c,freecon=0,ret,m1,m2;

    busy();
    fkey=NULL;
    if (def) {
tryload:
        if (type==CFG_MAINMENU) path=configname;
        else path=loadnamebuf;
        makefilebuffer(path);
        filereq.flags=0;
        filereq.title=cfg_string[STR_SELECT_CONFIGURATION_TO_LOAD];
        filereq.window=Window;
        if (!(FileRequest(&filereq))) {
            unbusy();
            return(0);
        }
        strcpy(loadnamebuf,dirbuf);
        TackOn(loadnamebuf,filebuf,256);
        if (!(strstri(loadnamebuf,".CFG"))) StrConcat(loadnamebuf,".CFG",256);
    }
    if (!(newcstuff=AllocMem(sizeof(struct ConfigStuff),MEMF_CLEAR)) ||
        !(newcstuff->config=AllocMem(sizeof(struct Config),MEMF_CLEAR))) {
        if (newcstuff) FreeMem(newcstuff,sizeof(struct ConfigStuff));
        unbusy();
        return(0);
    }
    fixcstuff(&cstuff);
    if (def) {
        lchanged=changed=0;
        SetBusyPointer(Window);
        if ((a=ReadConfig(loadnamebuf,newcstuff))!=1) {
            ClearPointer(Window);
            if (a==ERROR_NOT_CONFIG) strcpy(buf,cfg_string[STR_FILE_NOT_VALID_CONFIGURATION]);
            else lsprintf(buf,cfg_string[STR_OPEN_FAILED],a);
            if (request(buf)) goto tryload;
        }
        else {
            lchanged=1;
            ClearPointer(Window);
            CheckConfig(newcstuff);
        }
    }
    else {
        DefaultConfig(newcstuff);
        lchanged=1;
    }
    if (lchanged) {
        ret=1;
        switch (type) {
            case CFG_MAINMENU:
                FreeConfig(&cstuff);
                CopyMem((char *)newcstuff,(char *)&cstuff,sizeof(struct ConfigStuff));
                cstuff.config=config;
                CopyMem((char *)newcstuff->config,(char *)config,sizeof(struct Config));
                strcpy(configname,loadnamebuf);
                freecon=1;
                break;
            case CFG_OPERATION:
                CopyMem((char *)&newcstuff->config->copyflags,(char *)&config->copyflags,
                    (offsetof(struct Config,dynamicflags)-offsetof(struct Config,copyflags))+1);
                config->dateformat=newcstuff->config->dateformat;
                config->formatflags=newcstuff->config->formatflags;
                for (a=0;a<2;a++) {
                    config->sortmethod[a]=newcstuff->config->sortmethod[a];
                    config->separatemethod[a]=newcstuff->config->separatemethod[a];
                    CopyMem((char *)newcstuff->config->displaypos[a],(char *)config->displaypos[a],16);
                    CopyMem((UWORD *)newcstuff->config->displaylength[a],(UWORD *)config->displaylength[a],32);
                }
                freecon=3;
                break;
            case CFG_GADGET:
                if (bank=firstbank) {
                    while (bank->next) bank=bank->next;
                }
                bank2=newcstuff->firstbank;
                while (bank2) {
                    if (nbank=(struct dopusgadgetbanks *)
                            getcopy((char *)bank2,sizeof(struct dopusgadgetbanks),NULL)) {
                        nbank->next=NULL;
                        if (bank) bank->next=nbank;
                        else firstbank=nbank;
                        bank=nbank;
                        copygadgets(bank2,bank,NULL);
                    }
                    bank2=bank2->next;
                }
                cstuff.firstbank=firstbank;
                freecon=3;
                break;
            case CFG_MENU:
                freecon=3;
                for (a=0;a<6;a++) {
                    menulist[a]=NULL;
                    menuarray[a]=0;
                }
                for (a=0,b=0;a<5;a++) {
                    if (newcstuff->config->menutit[a][0])
                        menulist[b++]=newcstuff->config->menutit[a];
                }
                if (!menulist[0] ||
                    !(dolistwindow(cfg_string[STR_SELECT_MENUS_TO_IMPORT],
                        332,40,menulist,DLVF_MULTI|DLVF_HIREC,menuarray,NULL))) {
                    ret=0;
                    break;
                }
                for (a=0,b=0;a<5;a++) {
                    m1=a*20; m2=(a+1)*20;
                    for (c=m1;c<m2;c++)
                        if (config->menu[c].name && config->menu[c].name[0]) break;
                    if (c==m2) {
                        while (!menuarray[b]) if ((++b)>4) break;
                        if (b>4) break;
                        for (c=m1;c<m2;c++) freenewfunction(&config->menu[c]);
                        CopyMem((char *)&newcstuff->config->menu[b*20],
                            (char *)&config->menu[m1],sizeof(struct dopusfunction)*20);
                        for (c=m1;c<m2;c++) {
                            config->menu[c].name=getcopy(config->menu[c].name,-1,NULL);
                            config->menu[c].function=getcopy(config->menu[c].function,-1,NULL);
                        }
                        strcpy(config->menutit[a],newcstuff->config->menutit[b]);
                        ++b;
                    }
                }
                if ((a == 5) && (menuarray[b])) ret=2;
                break;
            case CFG_DRIVE:
                for (a=0;a<DRIVECOUNT;a++) freestring(config->drive[a].function);
                CopyMem((char *)newcstuff->config->drive,(char *)config->drive,sizeof(struct dopusfunction)*DRIVECOUNT);
                for (a=0;a<DRIVECOUNT;a++)
                    config->drive[a].function=getcopy(newcstuff->config->drive[a].function,-1,NULL);
                freecon=3;
                break;
            case CFG_FILETYPE:
                freecon=3;
                b=0;
                ftype=newcstuff->firsttype;
                while (ftype) {
                    b++;
                    ftype=ftype->next;
                }
                if (!(ftypelist=LAllocRemember(&fkey,(b+1)*4,MEMF_CLEAR)) ||
                    !(typearray=LAllocRemember(&fkey,b+1,MEMF_CLEAR))) break;
                ftype=newcstuff->firsttype;
                for (a=0;a<b;a++) {
                    ftypelist[a]=ftype->type;
                    ftype=ftype->next;
                }
                if (!ftypelist[0] ||
                    !(dolistwindow(cfg_string[STR_SELECT_FILETYPES_TO_IMPORT],
                        332,72,ftypelist,DLVF_MULTI|DLVF_HIREC,typearray,NULL))) {
                    ret=0;
                    break;
                }
                ftype=newcstuff->firsttype; b=0;
                while (ftype) {
                    if (typearray[b]) {
                        if (ntype=(struct dopusfiletype *)
                            getcopy((char *)ftype,sizeof(struct dopusfiletype),&typekey)) {
                            ntype->next=NULL;
                            ntype->recognition=getcopy(ftype->recognition,-1,&typekey);
                            ntype->iconpath=getcopy(ftype->iconpath,-1,&typekey);
                            for (a=0;a<FILETYPE_FUNCNUM;a++)
                                ntype->function[a]=getcopy(ftype->function[a],-1,&typekey);
                            addfiletype(ntype);
                        }
                    }
                    ftype=ftype->next; ++b;
                }
                cstuff.firsttype=firsttype;
                break;
            case CFG_HOTKEYS:
                freecon=3;
                b=0;
                curhotkey=firsthotkey;
                while (curhotkey && curhotkey->next) curhotkey=curhotkey->next;
                fhotkey=newcstuff->firsthotkey;
                while (fhotkey) {
                    b++;
                    fhotkey=fhotkey->next;
                }
                if (!(ftypelist=LAllocRemember(&fkey,(b+1)*4,MEMF_CLEAR)) ||
                    !(typearray=LAllocRemember(&fkey,b+1,MEMF_CLEAR))) break;
                fhotkey=newcstuff->firsthotkey;
                for (a=0;a<b;a++) {
                    ftypelist[a]=fhotkey->name;
                    fhotkey=fhotkey->next;
                }
                if (!ftypelist[0] ||
                    !(dolistwindow(cfg_string[STR_SELECT_HOTKEYS_TO_IMPORT],
                        332,72,ftypelist,DLVF_MULTI|DLVF_HIREC,typearray,NULL))) {
                    ret=0;
                    break;
                }
                fhotkey=newcstuff->firsthotkey; b=0;
                while (fhotkey) {
                    if (typearray[b]) {
                        if (nhotkey=(struct dopushotkey *)
                            getcopy((char *)fhotkey,sizeof(struct dopushotkey),NULL)) {
                            nhotkey->next=NULL;
                            nhotkey->func.function=getcopy(fhotkey->func.function,-1,NULL);
                            if (curhotkey) curhotkey->next=nhotkey;
                            else firsthotkey=nhotkey;
                            curhotkey=nhotkey;
                        }
                    }
                    fhotkey=fhotkey->next; ++b;
                }
                cstuff.firsthotkey=firsthotkey;
                break;
            case CFG_SCREEN:
/*HUX               CopyMem((char *)&newcstuff->config->gadgettopcol,
                    (char *)&config->gadgettopcol,
                    offsetof(struct Config,pad4)-offsetof(struct Config,gadgettopcol));*/
                CopyMem(newcstuff->config->fontsizes,config->fontsizes,NUMFONTS);
                CopyMem(newcstuff->config->fontbufs,config->fontbufs,NUMFONTS*40);
                config->sliderbgcol=newcstuff->config->sliderbgcol;
                config->stringfgcol=newcstuff->config->stringfgcol;
                config->stringbgcol=newcstuff->config->stringbgcol;
                config->sliderwidth=newcstuff->config->sliderwidth;
                config->sliderheight=newcstuff->config->sliderheight;
                config->stringheight=newcstuff->config->stringheight;
                config->stringselfgcol=newcstuff->config->stringselfgcol;
                config->stringselbgcol=newcstuff->config->stringselbgcol;
                config->generalscreenflags=newcstuff->config->generalscreenflags;
                CopyMem((char *)newcstuff->config->scrollborders,
                    (char *)config->scrollborders,sizeof(struct Rectangle)*2);
                CopyMem((char *)newcstuff->config->new_palette,
                    (char *)config->new_palette,sizeof(ULONG)*48);
                strcpy(config->pubscreen_name,newcstuff->config->pubscreen_name);
                config->slider_pos=newcstuff->config->slider_pos;
                freecon=3;
                break;
            case CFG_SYSTEM:
                CopyMem((char *)newcstuff->config->outputcmd,(char *)config->outputcmd,
                    offsetof(struct Config,gadgetrows)-offsetof(struct Config,outputcmd));
                CopyMem((char *)newcstuff->config->startupscript,(char *)config->startupscript,
                    offsetof(struct Config,pad8)-offsetof(struct Config,startupscript));
                strcpy(config->uniconscript,newcstuff->config->uniconscript);
                config->addiconflags=newcstuff->config->addiconflags;
                strcpy(config->shellstartup,newcstuff->config->shellstartup);
                strcpy(config->language,newcstuff->config->language);
                config->loadexternal=newcstuff->config->loadexternal;
                freecon=3;
                break;
        }
        lchanged=changed=0;
    }
    else {
        freecon=1;
        ret=0;
    }
    if (freecon&2) FreeConfig(newcstuff);
    if (freecon&1) FreeMem(newcstuff->config,sizeof(struct Config));
    FreeMem(newcstuff,sizeof(struct ConfigStuff));
    cstufffix(&cstuff);
    LFreeRemember(&fkey);
    unbusy();
    return(ret);
}

void copyconfigonly(config1,config2)
struct Config *config1,*config2;
{
    struct newdopusfunction *menubackup;
    struct dopusfunction *drivebackup;
    struct DOpusRemember *key;

    key=NULL;
    menubackup=(struct newdopusfunction *)
        getcopy((char *)config2->menu,sizeof(struct dopusfunction)*MENUCOUNT,&key);
    drivebackup=(struct dopusfunction *)
        getcopy((char *)config2->drive,sizeof(struct dopusfunction)*DRIVECOUNT,&key);
    CopyMem((char *)config1,(char *)config2,sizeof(struct Config));
    if (menubackup) CopyMem((char *)menubackup,(char *)config2->menu,sizeof(struct newdopusfunction)*MENUCOUNT);
    if (drivebackup) CopyMem((char *)drivebackup,(char *)config2->drive,sizeof(struct dopusfunction)*DRIVECOUNT);
    LFreeRemember(&key);
}

int dolistwindow(title,w,h,items,flags,selarray,item)
char *title;
int w,h;
char **items;
int flags;
char *selarray;
int *item;
{
    struct Window *wind;
    struct DOpusListView *view;
    ULONG class;
    UWORD code;
    int a,gadgetid,all=1,count;
    char **gadtxt;

    listlist.w=w;
    listlist.h=h;

    if (selarray && flags&DLVF_MULTI) {
        listokaygad[0].NextGadget=&listokaygad[2];
        listokaygad[2].NextGadget=&listokaygad[1];
        gadtxt=listviewgads2;
        count=3;
    }
    else {
        listokaygad[0].NextGadget=&listokaygad[1];
        gadtxt=listviewgads;
        count=2;
    }
    listokaygad[1].NextGadget=NULL;

    setup_list_window(&requestwin,&listlist,listokaygad,count);

    listlist.items=items;
    listlist.selectarray=selarray;
    listlist.flags=flags;
    listlist.itemselected=-1;
    setuplist(&listlist,-1,-1);

    requestwin.Title=title;

    if (!(wind=openwindow(&requestwin))) return(0);

    listlist.window=wind;
    if (!(AddListView(&listlist,1))) {
        CloseWindow(wind);
        return(0);
    }

    SetAPen(wind->RPort,screen_pens[1].pen);
    AddGadgets(wind,
        listokaygad,
        gadtxt,
        count,
        screen_pens[config->gadgettopcol].pen,screen_pens[config->gadgetbotcol].pen,1);
    busy();

    FOREVER {
        Wait(1<<wind->UserPort->mp_SigBit);
        while (IMsg=(struct IntuiMessage *)GetMsg(wind->UserPort)) {
            if ((view=ListViewIDCMP(&listlist,IMsg))==(struct DOpusListView *)-1) {
                class=IMsg->Class; code=IMsg->Code;
                if (class==IDCMP_GADGETUP || class==IDCMP_GADGETDOWN)
                    gadgetid=((struct Gadget *)IMsg->IAddress)->GadgetID;
                ReplyMsg((struct Message *)IMsg);
                if (class==IDCMP_VANILLAKEY) {
                    a=-1;
                    if (code=='\r') a=0;
                    else if (code=='') a=1;
                    if (a!=-1) {
                        SelectGadget(wind,&listokaygad[a]);
                        class=GADGETUP; gadgetid=1-a;
                    }
                }
                if (class==IDCMP_GADGETUP) {
                    if (gadgetid<2) {
                        unbusy();
                        RemoveListView(&listlist,1);
                        CloseWindow(wind);
                        return(gadgetid);
                    }
                    else if (gadgetid==2 && flags&DLVF_MULTI) {
                        for (a=0;a<listlist.count;a++) selarray[a]=all;
                        RefreshListView(&listlist,1);
                        all=1-all;
                    }
                }
            }
            else if (view) {
                if (flags&DLVF_MULTI)
                    selarray[view->itemselected]=1-selarray[view->itemselected];
                else {
                    if (item) *item=view->itemselected;
                    if (!(flags&DLVF_LEAVE)) {
                        unbusy();
                        RemoveListView(&cmdlist,1);
                        CloseWindow(wind);
                        return(1);
                    }
                }
            }
        }
    }
}

void copytoclip(func,funclist,functype,flagsel)
struct dopusfunction *func;
char **funclist,*functype,*flagsel;
{
    struct Clip *newclip,*pos;

    busy();
    if (newclip=LAllocRemember(&clipkey,sizeof(struct Clip),MEMF_CLEAR)) {
        newclip->func.function=compilefunclist(funclist,functype,&clipkey);
        newclip->func.which=getselflags(flagsel);
        newclip->func.stack=atoi(edit_stackbuf);
        newclip->func.pri=atoi(edit_prioritybuf);
        newclip->func.delay=atoi(edit_delaybuf);
        newclip->func.fpen=func->fpen;
        newclip->func.bpen=func->bpen;
        strcpy(newclip->name,edit_namebuf);
        pos=firstclip;
        while (pos && pos->next) pos=pos->next;
        if (pos) pos->next=newclip;
        else firstclip=newclip;
        ++clipcount;
    }
    unbusy();
}

int pasteclip(func,funclist,functype,displist,flagsel)
struct dopusfunction *func;
char **funclist,*functype,**displist,*flagsel;
{
    char **cliplist;
    struct DOpusRemember *key=NULL;
    struct Clip *clip;
    int a,b,ret=0;

    if (!clipcount) return(0);
    if (!(cliplist=LAllocRemember(&key,(clipcount+1)*4,MEMF_CLEAR))) return(0);
    busy();
    clip=firstclip;
    for (a=0;a<clipcount;a++) {
        if (!clip) break;
        if (!(cliplist[a]=LAllocRemember(&key,40,MEMF_CLEAR))) break;
        strcpy(cliplist[a],clip->name);
        clip=clip->next;
    }
    cliplist[a]=NULL;
    if (dolistwindow(cfg_string[STR_SELECT_FUNCTION_TO_PASTE],
        212,40,cliplist,DLVF_LEAVE|DLVF_HIREC,NULL,&a)) {
        clip=firstclip;
        for (b=0;b<a;b++) if (!(clip=clip->next)) break;
        if (clip) {
            erasefunction(func,funclist,displist,flagsel);
            lsprintf(edit_stackbuf,"%ld",(long int)clip->func.stack);
            lsprintf(edit_prioritybuf,"%ld",(long int)clip->func.pri);
            lsprintf(edit_delaybuf,"%ld",(long int)clip->func.delay);
            strcpy(edit_namebuf,clip->name);
            func->key=0; func->qual=0;
            func->fpen=clip->func.fpen; func->bpen=clip->func.bpen;
            makefunclist(clip->func.function,funclist,functype);
            for (a=0;a<MAXFUNCS;a++) makedispfunc(funclist[a],functype[a],displist[a]);
            makeselflags(clip->func.which,flagsel);
            ret=1;
        }
    }
    LFreeRemember(&key);
    unbusy();
    return(ret);
}

void makefilebuffer(buf)
char *buf;
{
    char *ptr;

    strcpy(dirbuf,buf);
    ptr=BaseName(dirbuf);
    if (ptr>dirbuf) {
        strcpy(filebuf,ptr);
        *ptr=0;
    }
    else if (CheckExist(dirbuf,NULL)<1) {
        dirbuf[0]=0;
        strcpy(filebuf,buf);
    }
}

char *strstri(string,substring)
char *string,*substring;
{
    int a,len,sublen;

    len=(strlen(string)-(sublen=strlen(substring)))+1;
    if (len<1) return(NULL);

    for (a=0;a<len;a++) {
        if (LStrnCmpI(&string[a],substring,sublen)==0)
            return(string+a);
    }
    return(NULL);
}
