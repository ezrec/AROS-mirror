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

#include "dopuslib.h"

char * __regargs getstringcopy(const char *str)
{
    char *newstr=NULL;

//D(bug("getstringcopy(%s)\n",str);
    if (str && (newstr=AllocMem(strlen(str)+1,0)))
        LStrCpy(newstr,str);
    return(newstr);
}

void __regargs freestring(char *str)
{
    if (str) FreeMem(str,strlen(str)+1);
}

int __regargs writestring(BPTR file,char *string)
{
    int b;
    char nl=0;

    if (string) {
        b=strlen(string)+1;
        if ((Write(file,string,b))<b) return(0);
    }
    else Write(file,&nl,1);
    return(1);
}

int __regargs readline(char *buf,int pos,char *buf1,int size)
{
    int a;

    for (a=0;a<4096;a++) {
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

void __regargs DoAssignDrive(struct ConfigStuff *cstuff,int drv,char *name,char *path)
{
    struct Config *config;

    if (!(config=cstuff->config)) return;
    if (name!=(char *)-1) {
        if (!name) config->drive[drv].name[0]=0;
        else LStrCpy(config->drive[drv].name,name);
    }
    if (path!=(char *)-1) {
        freestring(config->drive[drv].function);
        config->drive[drv].function=NULL;
        if (path && path[0]) config->drive[drv].function=getstringcopy(path);
    }
}

void __regargs linkinnewfiletype(struct ConfigStuff *cstuff,struct dopusfiletype *temp)
{
    struct dopusfiletype *pos;

    temp->next=NULL;
    if (!(pos=cstuff->firsttype)) cstuff->firsttype=temp;
    else {
        while (pos->next) {
            if (LStrCmp(pos->next->type,"Default")==0) {
                temp->next=pos->next;
                break;
            }
            pos=pos->next;
        }
        pos->next=temp;
    }
}

int __saveds DoReadConfig(register char *name __asm("a0"),register struct ConfigStuff *cstuff __asm("a1"))
{
    BPTR in;
    int a,size,pos,b,bk,gad,mv;
    UWORD ver,mag;
    char *cbuf,*buf,*tbuf,buf2[102],buf3[102];
    struct dopusfiletype *newtype;
    struct olddopusfiletype otype;
    struct dopusgadgetbanks *bank=NULL,*temp;
    struct Config *config;
    struct DOpusRemember *key;
    struct dopusfunction *tempfunc;
    struct dopusdrive *driveptr;
    struct dopushotkey *hotkey,*curhotkey=NULL;

    if (!(config=cstuff->config)) return(-1);

//D(bug("DoReadConfig:begin\n"));
    DoFreeConfig(cstuff);
    if (DoCheckExist(name,&size)>=0) return(ERROR_OBJECT_NOT_FOUND);
//D(bug("DoReadConfig:1\n"));
    size-=sizeof(struct Config);
    if (!(in=Open(name,MODE_OLDFILE))) return(IoErr());
//D(bug("DoReadConfig:2\n"));
    if ((Read(in,(char *)&ver,2))<2 || (Read(in,(char *)&mag,2))<2 ||
        mag!=CONFIG_MAGIC || ver==0xde) {
        if (mag==CONFIG_MAGIC && ver==0xde) {
            struct DOpusSimpleRequest req;
            char *gads[2];
            int rets[1];

            req.text=string_table[STR_CONFIG_TOO_OLD];
            gads[0]=string_table[STR_CONTINUE]; gads[1]=NULL;
            rets[0]=0;
            req.gads=gads;
            req.rets=rets;
            req.hi=-1; req.lo=-1;
            req.fg=-1; req.bg=-1;
            req.strbuf=NULL;
            req.flags=SRF_BORDERS;
            req.font=GfxBase->DefaultFont;
            req.title=string_table[STR_REQUEST];

            DoDoSimpleRequest(NULL,&req);
        }
        Close(in);
//D(bug("DoReadConfig:3\n"));
        return(ERROR_NOT_CONFIG);
    }
    Seek(in,0,OFFSET_BEGINNING);
    if ((Read(in,(char *)config,sizeof(struct Config)))<sizeof(struct Config)) {
        Close(in);
//D(bug("DoReadConfig:4\n"));
        return(IoErr());
    }
//D(bug("DoReadConfig:7\n"));
    if (config->version<CONFIG_CHANGE_DISPLAY) {
        for (a=0;a<2;a++) {
            for (b=0;b<8;b++) {
                config->displaypos[a][b]=config->old_displaypos[a][b];
                config->displaylength[a][b]=(config->old2_displaylength[a][b])*8;
                config->old_displaypos[a][b]=0;
                config->old2_displaylength[a][b]=0;
            }
            for (b=8;b<16;b++) {
                config->displaypos[a][b]=-1;
                config->displaylength[a][b]=0;
            }
        }
    }
    else if (config->version<CONFIG_BIG_DISPLEN) {
        for (a=0;a<2;a++) {
            for (b=0;b<16;b++) {
                config->displaylength[a][b]=(config->old_displaylength[a][b])*8;
                config->old_displaylength[a][b]=0;
            }
        }
    }

    if (config->version<CONFIG_CHANGE_CONFIGXY) {
        config->config_x=-1;
        config->config_y=-1;
    }

    if (config->version<CONFIG_CHANGE_DIMENS) {
        config->scr_winx=config->wbwinx;
        config->scr_winy=config->wbwiny;
        config->scr_winw=config->scrw;
        config->scr_winh=config->scrh;
        config->pubscreen_name[0]=0;
    }

    if (config->version<CONFIG_NEW_SLIDERS) {
        config->slider_pos=1;
    }

    if (config->version<=OLD_CONFIG_VERSION) {
        config->scrdepth+=2;
        config->dateformat=1;
        for (a=0;a<2;a++) {
            for (b=0;b<5;b++) config->displaypos[a][b]=b;
            for (b=5;b<16;b++) config->displaypos[a][b]=-1;
            config->displaylength[a][DISPLAY_NAME]=32*8;
            config->displaylength[a][DISPLAY_COMMENT]=40*8;
            config->displaylength[a][DISPLAY_FILETYPE]=16*8;
            config->displaylength[a][DISPLAY_OWNER]=16*8;
            config->displaylength[a][DISPLAY_GROUP]=16*8;
            config->sortmethod[a]=0;
            config->separatemethod[a]=1;
        }
        config->sortflags=0;
        config->scrclktype=SCRCLOCK_MEMORY|SCRCLOCK_DATE|SCRCLOCK_TIME;
        if (config->icontype==3) config->icontype=ICON_NOWINDOW;
        else config->icontype=0;
        config->icontype|=ICON_MEMORY|ICON_DATE|ICON_TIME;
        config->showfree=1<<config->showfree;
        config->hotkeyqual=config->hotkeycode&0xff;
        config->hotkeycode>>=8;
        config->stringfgcol=1; config->stringbgcol=0;
        config->stringselfgcol=1; config->stringselbgcol=0;
    }
    if (config->version<=CONFIG_CHANGE_PALETTE) {
        for (a=0,b=0;a<16;a++) {
            config->new_palette[b++]=(((config->Palette[a]>>8)&0xf)<<28)|0x0fffffff;
            config->new_palette[b++]=(((config->Palette[a]>>4)&0xf)<<28)|0x0fffffff;
            config->new_palette[b++]=((config->Palette[a]&0xf)<<28)|0x0fffffff;
        }
    }
    if (config->version<=CONFIG_CHANGE_BUFCOUNT) {
        config->bufcount/=2;
        if (config->bufcount<1) config->bufcount=1;
    }
    if (config->version<CONFIG_LESS_DODRIVES) {
        if (!(tempfunc=AllocMem(sizeof(struct dopusfunction)*DRIVECOUNT,MEMF_CLEAR))) {
            Close(in);
//D(bug("DoReadConfig:5\n"));
            return(ERROR_NO_FREE_STORE);
        }
        driveptr=(struct dopusdrive *)config->drive;
        for (a=0;a<OLDDRIVECOUNT;a++) {
            LStrCpy(tempfunc[a].name,driveptr[a].name);
            tempfunc[a].key=driveptr[a].key;
            tempfunc[a].qual=driveptr[a].qual;
            tempfunc[a].fpen=driveptr[a].fpen;
            tempfunc[a].bpen=driveptr[a].bpen;
            tempfunc[a].function=getstringcopy(driveptr[a].path);
        }
        for (a=OLDDRIVECOUNT;a<DRIVECOUNT;a++) tempfunc[a].fpen=3;
        CopyMem((char *)tempfunc,(char *)driveptr,sizeof(struct dopusfunction)*DRIVECOUNT);
        FreeMem(tempfunc,sizeof(struct dopusfunction)*DRIVECOUNT);
    }
    if (config->version<=CONFIG_CHANGE_DOSREQ) {
        config->errorflags^=1;
        config->hotkeyflags=0;
        config->pad3=config->pad4=config->pad8=0;
// HUX
        for (a=0;a<2;a++) config->pad7[a]=0;
        for ( a = 0; a < 224; a++ ) config->morepadding[a] = 0;
/*
        for (a=0;a<2;a++) config->pad5[a]=config->pad7[a]=0;
        for (a=0;a<232;a++) config->morepadding[a]=0;
*/
        for (a=0;a<2;a++) {
            config->scrollborders[a].MinX=~0;
            config->scrollborders[a].MaxX=~0;
            config->scrollborders[a].MinY=~0;
            config->scrollborders[a].MaxY=~0;
        }
        config->generalscreenflags=2;
    }
    if (config->version<=CONFIG_NEW_FUNCTIONS) {
        for (a=0;a<MENUCOUNT;a++) {
            LStrCpy(buf2,(char *)&config->menu[a]);
            config->menu[a].name=getstringcopy(buf2);
            config->menu[a].pad2[0]=0;
            config->menu[a].pad2[1]=0;
            config->menu[a].pad2[2]=0;
        }
    }
    if (config->version<=CONFIG_CHANGE_ARROWS)
        for (a=0;a<3;a++) config->arrowsize[a]=8;
    if (config->version<=CONFIG_CHANGE_EXTERN) {
// HUX        for (a=0;a<80;a++) config->pad5a[a]=config->configreturnscript[a]=0;
        for (a=0;a<397;a++) config->pad9a[a]=0;
        config->loadexternal=0;
    }
    key=NULL;
    if (!(cbuf=(char *)DoAllocRemember(&key,size,MEMF_CLEAR)) ||
        !(buf=(char *)DoAllocRemember(&key,4096,MEMF_CLEAR)) ||
        !(tbuf=(char *)DoAllocRemember(&key,256,MEMF_CLEAR))) {
        Close(in);
        DoFreeRemember(&key);
//D(bug("DoReadConfig:6\n"));
        return(ERROR_NO_FREE_STORE);
    }
    Read(in,cbuf,size);
    Close(in);
    for (a=0;a<MENUCOUNT;a++) {
        config->menu[a].function=NULL;
        if (config->version>CONFIG_NEW_FUNCTIONS) config->menu[a].name=NULL;
    }
    if (config->version>OLD_CONFIG_VERSION) {
        for (a=0;a<DRIVECOUNT;a++) config->drive[a].function=NULL;
    }

//D(bug("DoReadConfig:8\n"));
    pos=0;
    for (a=0;a<MENUCOUNT;a++) {
        pos=readline(cbuf,pos,buf,size);
        if (buf[0]) DoAssignMenu(cstuff,a,(char *)-1,buf);
        if (pos==-1) goto endthis;
    }
    if (config->version>CONFIG_NEW_FUNCTIONS) {
        for (a=0;a<MENUCOUNT;a++) {
            pos=readline(cbuf,pos,buf,size);
            if (buf[0]) DoAssignMenu(cstuff,a,buf,(char *)-1);
            if (pos==-1) goto endthis;
        }
    }
//D(bug("DoReadConfig:9\n"));
    if (config->version>OLD_CONFIG_VERSION) {
        for (a=0;a<DRIVECOUNT;a++) {
            pos=readline(cbuf,pos,buf,size);
            if (buf[0]) DoAssignDrive(cstuff,a,(char *)-1,buf);
            if (pos==-1) goto endthis;
        }
    }
    else {
        for (a=0;a<ARCHIVECOUNT;a++) pos=readline(cbuf,pos,buf,size);
    }
    bk=-1;
    FOREVER {
        if (pos>=size || pos==-1) break;
        if (cbuf[pos]==5 && cbuf[pos+1]=='H') {
            pos+=2;
            if ((pos+sizeof(struct dopushotkey))>=size) break;
            if ((hotkey=AllocMem(sizeof(struct dopushotkey),MEMF_CLEAR))) {
                CopyMem((char *)&cbuf[pos],(char *)hotkey,sizeof(struct dopushotkey));
                hotkey->func.function=NULL;
                hotkey->next=NULL;
            }
            pos+=sizeof(struct dopushotkey);
            if ((pos=readline(cbuf,pos,buf,size))==-1) break;
            if (hotkey) {
                hotkey->func.function=getstringcopy(buf);
                if (curhotkey) curhotkey->next=hotkey;
                else cstuff->firsthotkey=hotkey;
                curhotkey=hotkey;
            }
        }
        else if (cbuf[pos]==6 && cbuf[pos+1]=='F') {
            pos+=2;
            if ((pos+sizeof(struct wr_dopusfiletype))>=size) break;
            if ((newtype=(struct dopusfiletype *)
                DoAllocRemember(&cstuff->typekey,sizeof(struct dopusfiletype),MEMF_CLEAR))) {
                CopyMem((char *)&cbuf[pos],(char *)newtype,sizeof(struct wr_dopusfiletype));
                if (config->version<=CONFIG_CHANGE_FTYPE) {
                    newtype->type[31]=0;
                    newtype->typeid[0]=0;
                }
                pos+=sizeof(struct wr_dopusfiletype);
                newtype->recognition=NULL;
                for (a=0;a<FILETYPE_FUNCNUM;a++) newtype->function[a]=NULL;
                if ((pos=readline(cbuf,pos,buf,size))==-1) break;
                if ((newtype->recognition=DoAllocRemember(&cstuff->typekey,strlen(buf)+1,0)))
                    LStrCpy(newtype->recognition,buf);
                for (a=0;a<FILETYPE_FUNCNUM;a++) {
                    if ((pos=readline(cbuf,pos,buf,size))==-1) break;
                    if ((newtype->function[a]=DoAllocRemember(&cstuff->typekey,strlen(buf)+1,0)))
                        LStrCpy(newtype->function[a],buf);
                }
                newtype->iconpath=NULL;
                if (config->version>CONFIG_CHANGE_FILETYPE) {
                    pos=readline(cbuf,pos,buf,size);
                    if (buf[0] && (newtype->iconpath=DoAllocRemember(&cstuff->typekey,strlen(buf)+1,0)))
                        LStrCpy(newtype->iconpath,buf);
                }
                linkinnewfiletype(cstuff,newtype);
            }
        }
        else if (cbuf[pos]==1 && cbuf[pos+1]=='F') {
            pos+=2;
            if ((pos+sizeof(struct olddopusfiletype))>=size) break;
            if ((newtype=(struct dopusfiletype *)
                DoAllocRemember(&cstuff->typekey,sizeof(struct dopusfiletype),MEMF_CLEAR))) {
                CopyMem((char *)&cbuf[pos],(char *)&otype,sizeof(struct olddopusfiletype));
                pos+=sizeof(struct olddopusfiletype);
                LStrnCpy(newtype->type,otype.type,32);
                newtype->type[31]=0;
                newtype->typeid[0]=0;
                for (a=0;a<FILETYPE_FUNCNUM;a++) newtype->function[a]=NULL;
                for (a=0;a<4;a++) {
                    LStrnCpy(newtype->actionstring[a],otype.actionstring[a],39);
                    newtype->actionstring[a][39]=0;
                    newtype->which[a]=otype.which[a];
                    newtype->stack[a]=otype.stack[a];
                    newtype->pri[a]=otype.pri[a];
                    newtype->delay[a]=otype.delay[a];
                }
                for (a=4;a<FILETYPE_FUNCNUM;a++) {
                    newtype->stack[a]=4000;
                    newtype->delay[a]=2;
                }
                for (a=0;a<4;a++) {
                    if ((pos=readline(cbuf,pos,buf,size))==-1) break;
                    if ((newtype->function[a]=DoAllocRemember(&cstuff->typekey,strlen(buf)+1,MEMF_CLEAR)))
                        LStrCpy(newtype->function[a],buf);
                }
                buf[0]=0;
                if (otype.filepat[0])
                    LSprintf(buf,"%lc%s%lc",FTYC_MATCHNAME,otype.filepat,
                        ((otype.recogchars[0])?((otype.and)?FTYC_AND:FTYC_OR):FTYC_ENDSECTION));
                if (otype.recogchars[0]) {
                    b=mv=0;
                    for (a=0;a<100;a++) {
                        if (!otype.recogchars[a] || otype.recogchars[a]==',') {
                            buf2[b]=0;
                            if (mv==0) {
                                if ((b=DoAtoh(buf2,0))>0) {
                                    LSprintf(buf3,"%lc%ld%lc",FTYC_MOVETO,b,FTYC_ENDSECTION);
                                    DoStrConcat(buf,buf3,4096);
                                }
                                mv=1;
                            }
                            else {
                                LSprintf(buf3,"%lc$%s%lc",FTYC_MATCH,buf2,FTYC_AND);
                                DoStrConcat(buf,buf3,4096);
                                mv=0;
                            }
                            if (!otype.recogchars[a]) break;
                            b=0;
                        }
                        else {
                            if (mv==1 && otype.recogchars[a]=='.') {
                                buf2[b++]='?'; buf2[b++]='?';
                            }
                            else buf2[b++]=otype.recogchars[a];
                        }
                    }
                    b=strlen(buf);
                    if (buf[b-1]==(char)FTYC_AND) buf[b-1]=0;
                }
                if ((newtype->recognition=DoAllocRemember(&cstuff->typekey,strlen(buf)+1,MEMF_CLEAR)))
                    LStrCpy(newtype->recognition,buf);
                linkinnewfiletype(cstuff,newtype);
            }
        }
        else if ((cbuf[pos]==2 || cbuf[pos]==3) && cbuf[pos+1]=='G') {
            if (cbuf[pos]==2) {
                a=0;
                b=sizeof(struct olddopusgadget)*GADCOUNT;
            }
            else {
                a=1;
                b=sizeof(struct newdopusfunction)*GADCOUNT;
            }
            pos+=2;
            if ((pos+b)>=size) break;
            ++bk;
            if (!(temp=AllocMem(sizeof(struct dopusgadgetbanks),MEMF_CLEAR))) goto endthis;
            if (!cstuff->firstbank) cstuff->firstbank=temp;
            else bank->next=temp;
            bank=temp;
            if (a) {
                CopyMem((char *)&cbuf[pos],(char *)bank->gadgets,sizeof(struct newdopusfunction)*GADCOUNT);
                pos+=(sizeof(struct newdopusfunction)*GADCOUNT);
            }
            else {
                for (a=0;a<GADCOUNT;a++) {
                    CopyMem((char *)&cbuf[pos],(char *)&bank->gadgets[a],10);
                    CopyMem((char *)&cbuf[pos+10],(char *)&bank->gadgets[a].which,16);
                    pos+=sizeof(struct olddopusgadget);
                }
            }
            for (gad=0;gad<GADCOUNT;gad++) {
                bank->gadgets[gad].function=NULL;
                buf[0]=0;
                if (pos>-1) {
                    pos=readline(cbuf,pos,buf,size);
                    DoAssignGadget(cstuff,bk,gad,(char *)-1,buf);
                }
            }
            if (config->version>CONFIG_NEW_FUNCTIONS) {
                for (gad=0;gad<GADCOUNT;gad++) {
                    bank->gadgets[gad].name=NULL;
                    buf[0]=0;
                    if (pos>-1) {
                        pos=readline(cbuf,pos,buf,size);
                        DoAssignGadget(cstuff,bk,gad,buf,(char *)-1);
                    }
                }
            }
            else {
                for (gad=0;gad<GADCOUNT;gad++) {
                    LStrCpy(buf,(char *)&bank->gadgets[gad]);
                    bank->gadgets[gad].name=getstringcopy(buf);
                    bank->gadgets[gad].pad2[0]=0;
                    bank->gadgets[gad].pad2[1]=0;
                    bank->gadgets[gad].pad2[2]=0;
                }
            }
            if (pos==-1) break;
        }
//        else {
//D(bug("pos = $%lx\n",pos++));
//        }
    }
endthis:
    DoFreeRemember(&key);

    if (config->version < CONFIG_USERAW0_HOTKEY)
     {
      for (a = 0; a < MENUCOUNT; a++) if (config->menu[a].key == 0) config->menu[a].key = 0xff;
      for (a = 0; a < DRIVECOUNT; a++) if (config->drive[a].key == 0) config->drive[a].key = 0xff;
      for (bank = cstuff->firstbank; bank; bank = bank->next)
        for (a = 0; a < GADCOUNT; a++) if (bank->gadgets[a].key == 0) bank->gadgets[a].key = 0xff;

      LStrCpy(config->autodirs[1],config->autodirs[0]+30);
     }

    cstuff->curbank=cstuff->firstbank;
//D(bug("DoReadConfig:end\n");Delay(50));
    return(1);
}

int __saveds DoSaveConfig(register char *name __asm("a0"), register struct ConfigStuff *cstuff __asm("a1"))
{
    BPTR out;
    int a,ret=0;
    struct dopusfiletype *type;
    struct dopushotkey *hotkey;
    struct dopusgadgetbanks *bank;
    struct Config *config;

    if (!(config=cstuff->config)) return(0);

    config->version=CONFIG_VERSION;
    config->magic=CONFIG_MAGIC;

    if (!(out=Open(name,MODE_NEWFILE))) return(FALSE);
    if ((Write(out,(char *)config,sizeof(struct Config)))<sizeof(struct Config)) {
        Close(out);
        return(0);
    }
    for (a=0;a<MENUCOUNT;a++)
        if (!writestring(out,config->menu[a].function)) goto error;
    for (a=0;a<MENUCOUNT;a++)
        if (!writestring(out,config->menu[a].name)) goto error;
    for (a=0;a<DRIVECOUNT;a++)
        if (!writestring(out,config->drive[a].function)) goto error;

    type=cstuff->firsttype;
    while (type) {
        if ((Write(out,"\006F",2))<2) goto error;
        if ((Write(out,(char *)type,sizeof(struct wr_dopusfiletype)))<sizeof(struct wr_dopusfiletype))
            goto error;
        if (!writestring(out,type->recognition)) goto error;
        for (a=0;a<FILETYPE_FUNCNUM;a++)
            if (!writestring(out,type->function[a])) goto error;
        if (!writestring(out,type->iconpath)) goto error;
        type=type->next;
    }
    bank=cstuff->firstbank;
    while (bank) {
        if ((Write(out,"\003G",2))<2) goto error;
        if ((Write(out,(char *)bank->gadgets,sizeof(struct dopusfunction)*GADCOUNT))<
            sizeof(struct dopusfunction)*GADCOUNT) goto error;
        for (a=0;a<GADCOUNT;a++)
            if (!writestring(out,bank->gadgets[a].function)) goto error;
        for (a=0;a<GADCOUNT;a++)
            if (!writestring(out,bank->gadgets[a].name)) goto error;
        bank=bank->next;
    }
    hotkey=cstuff->firsthotkey;
    while (hotkey) {
        if ((Write(out,"\005H",2))<2) goto error;
        if ((Write(out,(char *)hotkey,sizeof(struct dopushotkey)))<sizeof(struct dopushotkey))
            goto error;
        if (!writestring(out,hotkey->func.function)) goto error;
        hotkey=hotkey->next;
    }
    ret=1;
error:
    Close(out);
    return(ret);
}

struct DefaultGadFlag {
    char code;
    char qual;
    char fpen;
    char bpen;
};

const static ULONG defaultpalette[48]={
    0xafffffff,0xafffffff,0xafffffff,
    0x00000000,0x00000000,0x00000000,
    0xffffffff,0xffffffff,0xffffffff,
    0x0fffffff,0x5fffffff,0xbfffffff,
    0xefffffff,0xafffffff,0x4fffffff,
    0x7fffffff,0x00000000,0x7fffffff,
    0xffffffff,0xffffffff,0x00000000,
    0xcfffffff,0x2fffffff,0x00000000,
    0xffffffff,0x8fffffff,0x00000000,
    0xffffffff,0x00000000,0xffffffff,
    0x9fffffff,0x6fffffff,0x3fffffff,
    0x00000000,0xffffffff,0x9fffffff,
    0x4fffffff,0xffffffff,0x3fffffff,
    0x00000000,0x00000000,0x00000000,
    0xffffffff,0xffffffff,0xffffffff,
    0x2fffffff,0x5fffffff,0x9fffffff};

const static char
    *defgads[42]={
        "All","Copy","Makedir","Hunt","Run","Comment","Read",
        "None","Move","Assign","Search","","Datestamp","Hex Read",
        "Parent","Rename","Check Fit","","","Protect","Show",
        "Root","","GetSizes","","","Icon Info","Play",
        "","","","","","Arc Ext","Edit",
        "","DELETE","","","","Encrypt","Print"},
    *revgads[35]={
        "Toggle","Copy As","","","","","ANSI Read",
        "","Move As","","","","","",
        "","Clone","","","","","",
        "","","ClearSizes","","","Add Icon","Loop Play",
        "","","","","","","New File"},
    *deffuncs[42]={
        "*All","*Copy","*Makedir","*Hunt","*Run","*Comment","*Read",
        "*None","*Move","*Assign","*Search","","*Datestamp","*HexRead",
        "*Parent","*Rename","*CheckFit","","","*Protect","*Show",
        "*Root","","*GetSizes","","","*IconInfo","*Play",
        "","","","","","*User1","SYS:Tools/Editor {f}",
        "","*DELETE","","","","*Encrypt","*Print"},
    *revfuncs[35]={
        "*Toggle","*CopyAs","","","","","*ANSIRead",
        "","*MoveAs","","","","","",
        "","*Clone","","","","","",
        "","","*ClearSizes","","","*AddIcon","*LoopPlay",
        "","","","","","","SYS:Tools/Editor {RsEnter filename to create new file:Untitled}"},

    *defmenus1[9]={
        "Current dir~","Help!","Error help~",
        "Configure~","About",
        "Version~","Iconify","Button Iconify","Quit"},
    *deffuncs1[9]={
        "*CD","*Help","*ErrorHelp",
        "*Configure","*About",
        "*Version","*Iconify","*ButtonIconify","*Quit"},
    *defmenus2[9]={
        "Disk copy","Format","Install~","Relabel","Print dir","Disk info~",
        "LHA add","Arc add","Zoo add"},
    *deffuncs2[9]={
        "*Diskcopy","*Format","*Install","*Relabel","*PrintDir","*DiskInfo",
        "LHA -x a \"{d}{RsEnter LHA archive name}\" {O}",
        "Arc a \"{d}{RsEnter Arc archive name}\" {O}",
        "Zoo a \"{d}{RsEnter Zoo archive name}\" {O}"},

    *deftype_type[5]={
        "LHA archive","ARC archive","ZOO archive","Icon","Default"},
    *deftype_typeid[5]={
        "LHA","ARC","ZOO","ICON","DEFAULT"},
    *deftype_recog[6]={
        "\002*.(lzh|lha)",
        "\002*.arc",
        "\002*.zoo",
        "\001$E310\376\002*.info",
        "\002*"},
    *deftype_funcs[5][4]={
        {"LHA v {f}","LHA -x -M x {f}","LHA v {f}","LHA -x -M x {f}"},
        {"Arc v {f}","Arc x {f}","Arc v {f}","Arc x {f}"},
        {"Zoo v {f}","Zoo x// {f}","Zoo v {f}","Zoo x// {f}"},
        {"*IconInfo",NULL,NULL,NULL},
        {"*SmartRead","*Copy",NULL,NULL}},
    deftype_funcpos[5][4]={
        {FTFUNC_DOUBLECLICK,FTFUNC_CLICKMCLICK,FTFUNC_READ,FTFUNC_AUTOFUNC1},
        {FTFUNC_DOUBLECLICK,FTFUNC_CLICKMCLICK,FTFUNC_READ,FTFUNC_AUTOFUNC1},
        {FTFUNC_DOUBLECLICK,FTFUNC_CLICKMCLICK,FTFUNC_READ,FTFUNC_AUTOFUNC1},
        {FTFUNC_DOUBLECLICK},
        {FTFUNC_DOUBLECLICK,FTFUNC_CLICKMCLICK}},
    *deftype_action[5][4]={
        {"Listing LHA archive...","Extracting files from LHA archive...",
            "Listing LHA archive...","Extracting files from LHA archive..."},
        {"Listing Arc archive...","Extracting files from Arc archive...",
            "Listing Arc archive...","Extracting files from Arc archive..."},
        {"Listing Zoo archive...","Extracting files from Zoo archive...",
            "Listing Zoo archive...","Extracting files from Zoo archive..."},
        {"Examining icon..."},
        {"Reading file...","Copying file..."}},
    deftype_delay[5][4]={
        {0,2,0,2},
        {0,2,0,2},
        {-1,2,-1,2},
        {2,2,2,2},
        {2,2,2,2}};

const unsigned char
    defmenkeys1[10]={0xff,0x5f,0xff,0x33,0xff,0xff,0xff,0x17,0xff,0x10},
    defmenqual1[10]={0,0,0,0x80,0,0,0,0x80,0,0x80},
    defmenkeys2[9]={0x22,0x23,0x36,0xff,0xff,0xff,0xff,0xff,0xff},
    defmenqual2[9]={0x80,0x80,0x80,0,0,0,0,0,0};

const static struct DefaultGadFlag
    default_gadflags[]={
        {0x20,IEQUALIFIER_CONTROL,6,3},
        {0x33,IEQUALIFIER_CONTROL,6,5},
        {0x37,IEQUALIFIER_CONTROL,5,4},
        {0x25,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,4,1},
        {0x16,IEQUALIFIER_CONTROL,1,4},
        {0x33,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,5,0},
        {0x13,IEQUALIFIER_CONTROL,6,7},

        {0x36,IEQUALIFIER_CONTROL,6,3},
        {0x37,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,6,5},
        {0xff,0,5,4},
        {0x21,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,4,1},
        {0xff,0,1,0},
        {0x22,IEQUALIFIER_CONTROL,5,0},
        {0x25,IEQUALIFIER_CONTROL,6,7},

        {0x19,IEQUALIFIER_CONTROL,6,3},
        {0x13,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,6,5},
        {0x23,IEQUALIFIER_CONTROL,5,4},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0x14,IEQUALIFIER_CONTROL,5,0},
        {0x21,IEQUALIFIER_CONTROL,6,7},

        {0x18,IEQUALIFIER_CONTROL,6,3},
        {0xff,0,1,0},
        {0x24,IEQUALIFIER_CONTROL,5,4},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0x17,IEQUALIFIER_CONTROL,5,0},
        {0x15,IEQUALIFIER_CONTROL,6,7},

        {0xff,0,1,0},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0x12,IEQUALIFIER_CONTROL,5,0},
        {0x12,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,6,7},

        {0xff,0,1,0},
        {0x22,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,2,7},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0xff,0,1,0},
        {0xff,0,5,0},
        {0x19,IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT,6,7}};

const static short
    deftype_which[5][4]={
        {FLAG_DOALL|FLAG_OUTFILE,FLAG_SCANDEST|FLAG_DOALL|FLAG_CDDEST|FLAG_OUTWIND,
            FLAG_DOALL|FLAG_OUTFILE,FLAG_SCANDEST|FLAG_DOALL|FLAG_CDDEST|FLAG_OUTWIND},
        {FLAG_DOALL|FLAG_OUTFILE,FLAG_SCANDEST|FLAG_DOALL|FLAG_CDDEST|FLAG_OUTWIND,
            FLAG_DOALL|FLAG_OUTFILE,FLAG_SCANDEST|FLAG_DOALL|FLAG_CDDEST|FLAG_OUTWIND},
        {FLAG_DOALL|FLAG_OUTWIND,FLAG_SCANDEST|FLAG_DOALL|FLAG_CDDEST|FLAG_OUTWIND,
            FLAG_DOALL|FLAG_OUTWIND,FLAG_SCANDEST|FLAG_DOALL|FLAG_CDDEST|FLAG_OUTWIND},
        {0,0,0,0},
        {0,0,0,0}};

int __saveds DoDefaultConfig(register struct ConfigStuff *cstuff __asm("a0"))
{
    int a,b,h,i;
    struct Config *config;
    struct dopusgadgetbanks *firstbank;
    struct dopusfiletype *type;
    char name[256];

    if (!(config=cstuff->config)) return(0);
    DoFreeConfig(cstuff);

    if ((DoFindSystemFile("DirectoryOpus.DefCFG",name,256,SYSFILE_DATA)) &&
        (DoReadConfig(name,cstuff))==1) return(1);

    /* Operation */

    config->copyflags=COPY_DATE|COPY_PROT|COPY_NOTE;
    config->dateformat=DATE_DOS|DATE_SUBST|DATE_12HOUR;
    config->existflags=REPLACE_ASK;
    config->deleteflags=DELETE_ASK;
    config->errorflags=ERROR_ENABLE_DOS|ERROR_ENABLE_OPUS;
    config->generalflags=GENERAL_DRAG|GENERAL_DOUBLECLICK|GENERAL_ACTIVATE;
    config->iconflags=ICONFLAG_MAKEDIRICON|ICONFLAG_DOUNTOICONS;

    for (h=0;h<2;h++) {
        for (i=0;i<5;i++) config->displaypos[h][i]=i;
        for (i=5;i<16;i++) config->displaypos[h][i]=-1;
        config->displaylength[h][DISPLAY_NAME]=32*8;
        config->displaylength[h][DISPLAY_COMMENT]=40*8;
        config->displaylength[h][DISPLAY_FILETYPE]=16*8;
        config->displaylength[h][DISPLAY_OWNER]=16*8;
        config->displaylength[h][DISPLAY_GROUP]=16*8;
        config->sortmethod[h]=0;
        config->separatemethod[h]=1;
    }
    config->sortflags=0;
    config->formatflags=0;

    config->dynamicflags=UPDATE_FREE|UPDATE_SCROLL|UPDATE_LEFTJUSTIFY|UPDATE_PROGRESSINDICATOR|UPDATE_REDRAW;

    /* System */

    LStrCpy(config->outputcmd,"NewCLI");
    LStrCpy(config->output,"CON:20/10/600/180/Directory Opus Output");
    LStrCpy(config->shellstartup,"Shell-Startup");
    config->priority=0;

    config->icontype=ICON_MEMORY|ICON_DATE|ICON_TIME;
    config->scrclktype=SCRCLOCK_MEMORY|SCRCLOCK_DATE|SCRCLOCK_TIME;

    config->bufcount=10;
    config->dirflags=DIRFLAGS_FINDEMPTY|DIRFLAGS_REREADOLD|
                                        DIRFLAGS_SMARTPARENT|DIRFLAGS_CHECKBUFS|
                                        DIRFLAGS_AUTODISKC|DIRFLAGS_AUTODISKL;
    config->showfree=SHOWFREE_KILO;

    config->hotkeycode=(UWORD)~0;
    config->hotkeyqual=IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT|IEQUALIFIER_LALT;
    config->hotkeyflags=0;

    config->toolicon[0]=0;
    config->projecticon[0]=0;
    config->drawericon[0]=0;
    config->defaulttool[0]=0;
    config->addiconflags=0;

    config->loadexternal=0;
    config->language[0]=0;

    config->hiddenbit=0;
    config->showpat[0]=0;
    config->hidepat[0]=0;
    config->showpatparsed[0]=0;
    config->hidepatparsed[0]=0;

    config->autodirs[0][0]=0;
    config->autodirs[1][0]=0;
    config->startupscript[0]=0;
    config->uniconscript[0]=0;
    config->configreturnscript[0]=0;

    config->viewbits=VIEWBITS_SHOWBLACK|VIEWBITS_TEXTBORDERS;
    config->showdelay=0;
    config->fadetime=2;
    config->tabsize=8;

    /* Screen */

    for (a=0;a<3;a++) {
        config->arrowpos[a]=2;
        config->arrowsize[a]=8;
    }
    config->sliderwidth=10;
    config->sliderheight=7;
    config->stringheight=8;

    config->statusfg=1; config->statusbg=0;
    config->disknameselfg=2; config->disknameselbg=7;
    config->disknamefg=1; config->disknamebg=0;
    config->dirsselfg=2; config->dirsselbg=3;
    config->dirsfg=3; config->dirsbg=0;
    config->filesselfg=2; config->filesselbg=1;
    config->filesfg=1; config->filesbg=0;
    config->slidercol=1; config->sliderbgcol=0;
    config->arrowfg=1; config->arrowbg=0;
    config->littlegadfg=1; config->littlegadbg=0;
    config->clockfg=1; config->clockbg=0;
    config->requestfg=1; config->requestbg=0;
    config->gadgettopcol=2; config->gadgetbotcol=1;
    config->stringfgcol=1; config->stringbgcol=0;
    config->stringselfgcol=1; config->stringselbgcol=4;

    for (i=0;i<NUMFONTS;i++) {
        config->fontsizes[i]=8;
        LStrCpy(config->fontbufs[i],"topaz.font");
    }

    config->generalscreenflags=SCR_GENERAL_TINYGADS|
                               SCR_GENERAL_INDICATERMB|
                               SCR_GENERAL_REQDRAG|
//                               SCR_GENERAL_NEWLOOKMENU|
                               SCR_GENERAL_WINBORDERS;

    CopyMem((char *)defaultpalette,(char *)config->new_palette,sizeof(defaultpalette));

    config->screenmode=MODE_WORKBENCHCLONE;
    config->scrdepth=3;
    config->scrw=-1;
    config->scrh=-1;
    config->screenflags=0;

    config->scr_winx=0;
    config->scr_winx=0;
    config->scr_winw=-1;
    config->scr_winh=-1;

    /* Misc Stuff */

    config->gadgetrows=6;

    config->iconx=252; config->icony=0;
    config->wbwinx=0; config->wbwiny=0;
    config->iconbutx=100; config->iconbuty=40;
    config->config_x=-1; config->config_y=-1;

    for (a=0;a<2;a++) {
        config->scrollborders[a].MinX=~0;
        config->scrollborders[a].MaxX=~0;
        config->scrollborders[a].MinY=~0;
        config->scrollborders[a].MaxY=~0;
    }

    config->windowdelta=0;
    config->slider_pos=1;

    /* Buttons */

    for (i=0;i<42;i++) DoAssignGadget(cstuff,0,i,defgads[i],deffuncs[i]);
    for (i=0;i<35;i++) DoAssignGadget(cstuff,0,i+42,revgads[i],revfuncs[i]);
    for (i=78;i<GADCOUNT;i++) DoAssignGadget(cstuff,0,i,NULL,NULL);

    if (firstbank=cstuff->firstbank) {
        for (i=0;i<GADCOUNT;i++) {
            firstbank->gadgets[i].which=0;
            firstbank->gadgets[i].type=0;
            firstbank->gadgets[i].stack=4000;
            firstbank->gadgets[i].pri=0;
            firstbank->gadgets[i].delay=2;
            if (i<42) {
                firstbank->gadgets[i].key=default_gadflags[i].code;
                firstbank->gadgets[i].qual=default_gadflags[i].qual;
                firstbank->gadgets[i].fpen=default_gadflags[i].fpen;
                firstbank->gadgets[i].bpen=default_gadflags[i].bpen;
            }
            else {
                firstbank->gadgets[i].fpen=1;
                firstbank->gadgets[i].bpen=0;
                if (i==57) {
                    firstbank->gadgets[i].key=0x28;
                    firstbank->gadgets[i].qual=IEQUALIFIER_CONTROL;
                }
                else if (i==68) {
                    firstbank->gadgets[i].key=0x17;
                    firstbank->gadgets[i].qual=IEQUALIFIER_CONTROL|IEQUALIFIER_LSHIFT;
                }
                else {
                    firstbank->gadgets[i].key=0xff;
                    firstbank->gadgets[i].qual=0;
                }
            }
        }
        firstbank->gadgets[34].which=FLAG_ASYNC|FLAG_CDSOURCE;
        firstbank->gadgets[76].which=FLAG_ASYNC|FLAG_CDSOURCE;
    }
    cstuff->curbank=firstbank;

    /* Menus */

    LStrCpy(config->menutit[0],"Project");
    LStrCpy(config->menutit[1],"Function");
    for (i=2;i<5;i++) config->menutit[i][0]=0;

    for (i=0;i<9;i++) {
        DoAssignMenu(cstuff,i,defmenus1[i],deffuncs1[i]);
        config->menu[i].key=defmenkeys1[i];
        config->menu[i].qual=defmenqual1[i];
    }
    for (i=20;i<29;i++) {
        h=i-20;
        DoAssignMenu(cstuff,i,defmenus2[h],deffuncs2[h]);
        config->menu[i].key=defmenkeys2[h];
        config->menu[i].qual=defmenqual2[h];
    }
    for (i=26;i<29;i++) {
        config->menu[i].which=FLAG_OUTWIND|FLAG_CDSOURCE|FLAG_DOALL|FLAG_SCANDEST|((i==27)?0:FLAG_RECURSE);
        config->menu[i].delay=-1;
    }

    /* Drives */

    DoGetDevices(cstuff);

    for (i=0;i<DRIVECOUNT;i++) {
        config->drive[i].key=0xff;
        config->drive[i].qual=0;
        config->drive[i].fpen=1;
        config->drive[i].bpen=0;
    }

    /* Filetypes */

    for (i=0;i<5;i++) {
        if ((type=(struct dopusfiletype *)
            DoAllocRemember(&cstuff->typekey,sizeof(struct dopusfiletype),MEMF_CLEAR))) {
            LStrCpy(type->type,deftype_type[i]);
            LStrCpy(type->typeid,deftype_typeid[i]);
            if (type->recognition=DoAllocRemember(&cstuff->typekey,strlen(deftype_recog[i])+1,0))
                LStrCpy(type->recognition,deftype_recog[i]);
            for (a=0;a<4;a++) {
                if (deftype_funcs[i][a]) {
                    b=deftype_funcpos[i][a];
                    LStrCpy(type->actionstring[b],deftype_action[i][a]);
                    type->which[b]=deftype_which[i][a];
                    type->delay[b]=deftype_delay[i][a];
                    type->stack[b]=4000;
                    if (type->function[b]=DoAllocRemember(&cstuff->typekey,strlen(deftype_funcs[i][a])+1,0))
                    LStrCpy(type->function[b],deftype_funcs[i][a]);
                }
            }
            linkinnewfiletype(cstuff,type);
        }
    }
    return(1);
}

int __saveds DoGetDevices(register struct ConfigStuff *cstuff __asm("a0"))
{
    struct DeviceList *devlist;
    struct RootNode *rootnode;
    struct DosInfo *dosinfo;
    struct Config *config;
    char devname[16],pathname[256];
    int gap,i,j,k,a,p,l,d;

    if (!(config=cstuff->config)) return(0);
    rootnode=(struct RootNode *) DOSBase->dl_Root;
    dosinfo=(struct DosInfo *) BADDR(rootnode->rn_Info);
    devlist=(struct DeviceList *) BADDR(dosinfo->di_DevInfo);
    a=0;
    while (devlist) {
        if (devlist->dl_Type==DLT_DEVICE && devlist->dl_Task) {
            BtoCStr((BPTR)devlist->dl_Name,pathname,256);
            LStrCat(pathname,":");
            LStrnCpy(devname,pathname,15); devname[15]=0;
            DoAssignDrive(cstuff,a,devname,pathname);
//D(bug("%ld: %s\n",a,cstuff->config->drive[a].name)/*;Delay(50)*/);
            ++a;
            if (a==DRIVECOUNT) break;
        }
        devlist=(struct DeviceList *) BADDR(devlist->dl_Next);
    }
    for (gap=a/2;gap>0;gap/=2) {
        for (i=gap;i<a;i++) {
            for (j=i-gap;j>=0;j-=gap) {
                k=j+gap;
                if (LStrCmpI(config->drive[j].name,config->drive[k].name)<=0) break;
                SwapMem((char *)&config->drive[j],(char *)&config->drive[k],sizeof(struct dopusfunction));
//D(bug("Drives swapped!\n"));
            }
        }
    }
    d=a;
    if (a<DRIVECOUNT) {
        devlist=(struct DeviceList *) BADDR(dosinfo->di_DevInfo);
        while (devlist) {
            if (devlist->dl_Type==DLT_DIRECTORY) {
                BtoCStr((BPTR)devlist->dl_Name,pathname,256);
                LStrCat(pathname,":");
                LStrnCpy(devname,pathname,15); devname[15]=0;
                DoAssignDrive(cstuff,a,devname,pathname);
//D(bug("%ld: %s\n",a,cstuff->config->drive[a].name));
                ++a;
                if (a==DRIVECOUNT) break;
            }
            devlist=(struct DeviceList *) BADDR(devlist->dl_Next);
        }
    }
    p=a-d;
    for (gap=p/2;gap>0;gap/=2) {
        for (i=gap;i<p;i++) {
            for (j=i-gap;j>=0;j-=gap) {
                k=j+gap+d; l=j+d;
                if (LStrCmpI(config->drive[l].name,config->drive[k].name)<=0) break;
                SwapMem((char *)&config->drive[l],(char *)&config->drive[k],sizeof(struct dopusfunction));
//D(bug("Drives swapped!\n"));
            }
        }
    }
    if (a<DRIVECOUNT) {
        for (i=a;i<DRIVECOUNT;i++) DoAssignDrive(cstuff,i,NULL,NULL);
//D(bug("+%ld: %s\n",i,cstuff->config->drive[i].name);
    }
//for (a = 0; a < DRIVECOUNT; a++) D(bug("%ld: %s\n",a,cstuff->config->drive[a].name);

  return 1;
}

void __saveds DoAssignGadget(register struct ConfigStuff *cstuff __asm("a0"), register int bk __asm("d0"), register int gad __asm("d1"), register const char *name __asm("a1"), register const char *func __asm("a2"))
{
    struct dopusgadgetbanks *bank,*temp;
    int a;

    bank=cstuff->firstbank;
    for (a=0;a<bk;a++) {
        if (!bank || !bank->next) break;
        bank=bank->next;
    }
    if (a<bk || !bank) {
        if (!(temp=AllocMem(sizeof(struct dopusgadgetbanks),MEMF_CLEAR))) return;
        if (bank) bank->next=temp;
        else cstuff->firstbank=temp;
        bank=temp;
    }

    if (name!=(char *)-1) {
        freestring(bank->gadgets[gad].name);
        bank->gadgets[gad].name=NULL;
        if (name && name[0]) bank->gadgets[gad].name=getstringcopy(name);
    }
    if (func!=(char *)-1) {
        freestring(bank->gadgets[gad].function);
        bank->gadgets[gad].function=NULL;
        if (func && func[0]) bank->gadgets[gad].function=getstringcopy(func);
    }
}

void __saveds DoAssignMenu(register struct ConfigStuff *cstuff __asm("a0"), register int men __asm("d0"), const register char *name __asm("a1"), const register char *func __asm("a2"))
{
    struct Config *config;

//D(bug("DoAssignMenu(%s)\n",name);
    if (!(config=cstuff->config)) return;
    if (name!=(char *)-1) {
        freestring(config->menu[men].name);
        config->menu[men].name=NULL;
        if (name && name[0]) config->menu[men].name=getstringcopy(name);
    }
    if (func!=(char *)-1) {
        freestring(config->menu[men].function);
        config->menu[men].function=NULL;
        if (func && func[0]) config->menu[men].function=getstringcopy(func);
/*
    if (!(men%20))
     {
      config->menu[men].fpen = 1;
      config->menu[men].bpen = 2;
     }
*/
    }
}

void __saveds DoFreeConfig(register struct ConfigStuff *cstuff __asm("a0"))
{
    int a;
    struct Config *config;
    struct dopusgadgetbanks *bank,*temp;
    struct dopushotkey *hotkey,*temphot;

    if (!(config=cstuff->config)) return;

    for (a=0;a<MENUCOUNT;a++) {
        DoAssignMenu(cstuff,a,NULL,NULL);
        config->menu[a].which=0; config->menu[a].type=0;
        config->menu[a].stack=4000; config->menu[a].pri=0;
        config->menu[a].delay=2;
        config->menu[a].fpen=1; config->menu[a].bpen=2;
        config->menu[a].key=0; config->menu[a].qual=0;
    }
    for (a=0;a<DRIVECOUNT;a++) {
        DoAssignDrive(cstuff,a,NULL,NULL);
        config->drive[a].key=0xff;
        config->drive[a].qual=0;
        config->drive[a].fpen=3; config->drive[a].bpen=0;
    }

    bank=cstuff->firstbank;
    while (bank) {
        for (a=0;a<GADCOUNT;a++) {
            freestring(bank->gadgets[a].name);
            freestring(bank->gadgets[a].function);
        }
        temp=bank->next;
        FreeMem(bank,sizeof(struct dopusgadgetbanks));
        bank=temp;
    }
    cstuff->firstbank=cstuff->curbank=NULL;

    hotkey=cstuff->firsthotkey;
    while (hotkey) {
        temphot=hotkey->next;
        freestring(hotkey->func.function);
        FreeMem(hotkey,sizeof(struct dopushotkey));
        hotkey=temphot;
    }
    cstuff->firsthotkey=NULL;

    DoFreeRemember(&cstuff->typekey);
    cstuff->firsttype=NULL;
}

int __saveds DoCheckConfig(register struct ConfigStuff *cstuff __asm("a0"))
{
    struct Config *config;

    if (!(config=cstuff->config)) return(0);

    if (config->sliderheight<5) config->sliderheight=5;
    else if (config->sliderheight>49) config->sliderheight=49;
    if (config->sliderwidth<8) config->sliderwidth=8;
    else if (config->sliderwidth>108) config->sliderwidth=108;

    return 1;
}

const static char *look_dirs[]={
/* JRZ
    "C:","DOpus:C/",
    "S:","DOpus:S/",
    "LIBS:","DOpus:Libs/",
    "REXX:","DOpus:Rexx/",
    "C:","DOpus:Modules/",
    "S:","DOpus:Requesters/"
*/
    "PROGDIR:C/",         "C:",   "DOpus:C/",         "C/",
    "PROGDIR:S/",         "S:",   "DOpus:S/",         "S/",
    "PROGDIR:Libs/",      "LIBS:","DOpus:Libs/",      "Libs/",
    "PROGDIR:Rexx/",      "REXX:","DOpus:Rexx/",      "Rexx/",
    "PROGDIR:Modules/",   "C:",   "DOpus:Modules/",   "Modules/",
    "PROGDIR:Requesters/","S:",   "DOpus:Requesters/","Requesters/"
};

int __saveds DoFindSystemFile(register char *name __asm("a0"), register char *buf __asm("a1"), register int size __asm("d0"), register int type __asm("d1"))
{
    char temp[256];

//D(bug("DoFindSystemFile(%s)\n",name));
tryloop:
    LStrnCpy(temp,name,256);
    if (/*DOSBase->dl_lib.lib_Version<36 ||*/ //JRZ
        !(FindSegment(temp,NULL,0))) {
        if (DoCheckExist(temp,NULL)>=0) {
            int a;
            BPTR lock;

            for (a=0;a<4;a++) {                                  // JRZ: was 2
                DoStrCombine(temp,look_dirs[(type*4)+a],name,256); // JRZ: was 2

                if (DoCheckExist(temp,NULL)<0) break;
            }
            if (a==4) temp[0]=0;                                 // JRZ: was 2
            else if (lock = Lock(temp,ACCESS_READ))
             {
              NameFromLock(lock,temp,256);
              UnLock(lock);
             }
        }
        else {
            BPTR lock;

            if (lock=Lock("",ACCESS_READ)) {
                DoPathName(lock,temp,256);
                UnLock(lock);
                DoTackOn(temp,name,256);
            }
        }
    }
    if (temp[0]) {
        LStrnCpy(buf,temp,size);
//D(bug("DoFindSystemFile:end\n");Delay(50));
        return(1);
    }
    else if (type==SYSFILE_MODULE) {
        struct DOpusSimpleRequest req;
        char reqbuf[300];
        static char *req_gads[3];
        static int req_rets[2]={1,0};

        req_gads[0]=string_table[STR_TRY_AGAIN];
        req_gads[1]=string_table[STR_CANCEL];
        req_gads[2]=NULL;
// JRZ begin
        LSprintf(reqbuf,string_table[STR_CANNOT_FIND_FILE_REQ],name,req_gads[0],req_gads[1]);
// JRZ end
/* HUX
        LSprintf( reqbuf, "Unable to find the file \"%s\"\n\
in %s\n\n\
Check your configuration and then\nTry Again, or Cancel to abort the operation", name, defpath );
*/

        req.text=reqbuf;
        req.gads=req_gads;
        req.rets=req_rets;
        req.hi=-1; req.lo=-1;
        req.fg=-1; req.bg=-1;
        req.strbuf=NULL;
        req.flags=0;
        req.font=NULL;
        if (DoDoSimpleRequest(NULL,&req)) goto tryloop;
    }
    LStrnCpy(buf,name,size);
//D(bug("DoFindSystemFile:end\n");Delay(50));
    return(0);
}
