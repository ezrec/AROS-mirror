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
#include <ctype.h>
#ifndef __AROS__
#include <proto/xfdmaster.h>
#endif
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

void makedir(rexx)
int rexx;
{
    struct FileInfoBlock __aligned fileinfo;
    int a,success,win,err,addicon = 0;
    char dirname[FILEBUF_SIZE],new_directory[256];
    BPTR lock;

    win=data_active_window;
    if (/*(dopus_curwin[win] == dopus_specialwin[win]) ||*/ (dopus_curwin[win]->directory[0] == 0)) return;
    dirname[0]=new_directory[0]=0;

    FOREVER {
        if (rexx) {
            for (a=0;a<256;a++) {
                if (!rexx_args[0][a] || rexx_args[0][a]=='/' || rexx_args[0][a]==':') break;
            }
            if (!rexx_args[0][a]) {
                if (!(isvalidwindow(data_active_window))) return;
                strcpy(new_directory,str_pathbuffer[data_active_window]);
                TackOn(new_directory,rexx_args[0],256);
                if (rexx_args[1][0] && (rexx_args[1][0] == '1')) addicon = 1;
            }
            else {
                strcpy(new_directory,rexx_args[0]);
                win=-1;
            }
        }
        else if (status_iconified) {
            if (!(status_flags&STATUS_ISINBUTTONS) ||
                (!(getdummypath(new_directory,STR_ENTER_DIRECTORY_NAME)))) return;
            win=-1;
        }
        else {
            if (!(isvalidwindow(data_active_window))) return;
            strcpy(new_directory,str_pathbuffer[data_active_window]);

            if (!(whatsit(globstring[STR_ENTER_DIRECTORY_NAME],
                config->iconflags&ICONFLAG_MAKEDIRICON ? FILEBUF_SIZE-7 : FILEBUF_SIZE-2,
                dirname,NULL)) ||
                !dirname[0]) {
                myabort();
                return;
            }
            for (a=0;dirname[a];a++) {
                if (dirname[a]=='/' || dirname[a]==':') {
                    dirname[a]=0;
                    break;
                }
            }
            if (!dirname[0]) {
                myabort();
                return;
            }
            TackOn(new_directory,dirname,256);
        }

        if (!(lock=CreateDir(new_directory))) {
            doerror((err=IoErr()));
            if (rexx || (a=checkerror(globstring[STR_CREATING_DIRECTORY],
                BaseName(new_directory),-err))==3) {
                myabort();
                return;
            }
            continue;
        }

        Examine(lock,&fileinfo);
        UnLock(lock);

        if (win>-1) {
            addfile(dopus_curwin[win],win,fileinfo.fib_FileName,-1,
                fileinfo.fib_DirEntryType,&fileinfo.fib_Date,
                NULL,fileinfo.fib_Protection,0,TRUE,NULL,NULL,
                fileinfo.fib_OwnerUID,fileinfo.fib_OwnerGID);
            seename(win);
            update_buffer_stamp(win,1);
        }

        success=1;
        if ((config->iconflags&ICONFLAG_MAKEDIRICON) || addicon) {
            strcat(new_directory,".info");
            if ((iconwrite(ICONTYPE_DRAWER,new_directory))==1 &&
                (lockandexamine(new_directory,&fileinfo))) {
                if (win>-1) {
                    addfile(dopus_curwin[win],win,fileinfo.fib_FileName,
                        fileinfo.fib_Size,fileinfo.fib_DirEntryType,&fileinfo.fib_Date,
                        fileinfo.fib_Comment,fileinfo.fib_Protection,0,TRUE,NULL,NULL,
                        fileinfo.fib_OwnerUID,fileinfo.fib_OwnerGID);
                    seename(win);
                    update_buffer_stamp(win,1);
                }
            }
            else success=0;
        }
        break;
    }
    if (success) dostatustext(globstring[STR_DIRECTORY_CREATED]);
}

int iconwrite(type,name)
int type;
char *name;
{
    int suc,err;
    struct DiskObject *diskobj;
    char namebuf[256],*ptr,*originalicon,sourcebuf[256];
    struct dopusfiletype *ftype;

    if (!IconBase) return(0);

    strcpy(namebuf,name);
    if ((ptr=strstri(namebuf,".info"))) *ptr=0;

    if (type==ICONTYPE_DRAWER) originalicon=config->drawericon;
    else if (type==ICONTYPE_TRASH) originalicon=NULL;
    else {
        ftype=checkfiletype(namebuf,-2,0);
        if (ftype && ftype->iconpath) originalicon=ftype->iconpath;
        else if (type==ICONTYPE_TOOL) originalicon=config->toolicon;
        else originalicon=config->projecticon;
    }

    if (originalicon) {
        strcpy(sourcebuf,originalicon);
        if (!(strstri(sourcebuf,".info"))) strcat(sourcebuf,".info");
    }

    FOREVER {
        if (originalicon && originalicon[0] && (CheckExist(sourcebuf,NULL)<0)) {
            if ((copyicon(originalicon,namebuf,&err))>0) return(1);
        }
        else {
            if (/*system_version2 &&*/ (diskobj=GetDefDiskObject(type+2))) {
                if (diskobj->do_Type==WBPROJECT && config->defaulttool[0]) {
                    ptr=diskobj->do_DefaultTool;
                    diskobj->do_DefaultTool=config->defaulttool;
                }
                else ptr=NULL;
                suc=PutDiskObject(namebuf,diskobj);
                if (ptr) diskobj->do_DefaultTool=ptr;
                FreeDiskObject(diskobj);
                if (suc) return(1);
                err=IoErr();
            }
            else {
                if (type==ICONTYPE_PROJECT)
                    icontable[ICONTYPE_PROJECT]->do_DefaultTool=config->defaulttool;
                if (PutDiskObject(namebuf,icontable[type])) return(1);
                err=IoErr();
            }
        }
        if ((suc=checkerror(globstring[STR_ADDING_ICON],BaseName(namebuf),err))==3)
            return(-1);
        if (suc==0 || suc==2) return(0);
    }
}

int copyicon(srce,dest,err)
char *srce,*dest;
int *err;
{
    int suc;
    struct DiskObject *diskobj;
    char /*buf[256],*/buf1[256],*ptr;

    strcpy(buf1,srce);
    if ((ptr=strstri(buf1,".info"))) *ptr=0;

    if ((diskobj=GetDiskObject(buf1))) {
        suc=PutDiskObject(dest,diskobj);
        FreeDiskObject(diskobj);
        if (!suc || CheckExist(dest,NULL)>=0) *err=IoErr();
    }
    else {
//        StrCombine(buf,dest,".info",256);
//        suc=copyfile(srce,buf,err,/*-1,*/NULL,0);
        SetIoErr(ERROR_OBJECT_NOT_FOUND);
        suc = 0;
    }
    return(suc);
}

char *isicon(name)
char *name;
{
    int a;

    if ((a=strlen(name))<6) return(NULL);
    if (Stricmp(&name[a-5],".info")==0) return(&name[a-5]);
    return(NULL);
}

char *getarexxpath(rexx,win,num,argnum)
int rexx,win,num,argnum;
{
    struct FileInfoBlock __aligned fblock;
    int a,b;
    char *ptr;
    APTR save;

    if (!rexx || !rexx_argcount) return(str_pathbuffer[win]);
    if ((strchr(rexx_args[argnum],':') || strchr(rexx_args[argnum],'/')) &&
        (a=CheckExist(rexx_args[argnum],&b))) {
        if (!num) {
            save=main_proc->pr_WindowPtr;
            main_proc->pr_WindowPtr=(APTR)-1;
            dos_global_entry.subtype=0;
            if (lockandexamine(rexx_args[argnum],&fblock)) {
                dos_global_entry.protection=fblock.fib_Protection;
                dos_global_entry.comment=dos_copy_comment;
                strcpy(dos_copy_comment,fblock.fib_Comment);
                dos_global_entry.dispstr=NULL;
                getprot(dos_global_entry.protection,dos_global_entry.protbuf);
                CopyMem(&fblock.fib_Date,&dos_global_entry.date,sizeof(struct DateStamp));
                seedate(&dos_global_entry.date,dos_global_entry.datebuf,1);
                dos_global_entry.selected=0;
            }
            main_proc->pr_WindowPtr=save;
            ptr=BaseName(rexx_args[argnum]);
            strcpy(dos_global_entry.name,ptr);
            if (ptr>rexx_args[argnum]) {
                *ptr=0;
                strcpy(rexx_pathbuffer[win],rexx_args[argnum]);
                TackOn(rexx_pathbuffer[win],NULL,256);
            }
            else rexx_pathbuffer[win][argnum]=0;
            dos_global_entry.last=dos_global_entry.next=NULL;
            dos_global_entry.type=a;
            dos_global_entry.size=b;
            func_single_entry=&dos_global_entry;
            removeargstring(argnum);
            return(rexx_pathbuffer[win]);
        }
        else {
            strcpy(rexx_pathbuffer[win],rexx_args[argnum]);
            TackOn(rexx_pathbuffer[win],NULL,256);
            removeargstring(argnum);
            return(rexx_pathbuffer[win]);
        }
    }
    return(str_pathbuffer[win]);
}

int readfile(name,buf,size)
char *name,**buf;
int *size;
{
    BPTR in;
    int retval /*loadpp*/ = 0;
/*
    if (PPBase) {
        if ((in=ppLoadData(name,DECR_POINTER,MEMF_CLEAR,buf,size,NULL))) {
            if (in==PP_OPENERR || in==PP_READERR) return(-1);
            if (in==PP_NOMEMORY) return(-2);
        }
        else loadpp=1;
    }
    if (!loadpp)
*/
    if (CheckExist(name,size)>=0 || !(in=Open(name,MODE_OLDFILE))) return(-1);
    if ((*buf=AllocVec(*size,MEMF_ANY)))
     {
      Read(in,*buf,*size);

#ifndef __AROS__
      if (OpenXFDlib())
       {
        struct xfdBufferInfo *xfdbi;

        if ((xfdbi = xfdAllocObject(XFDOBJ_BUFFERINFO)))
         {
          xfdbi->xfdbi_SourceBuffer = *buf;
          xfdbi->xfdbi_SourceBufLen = *size;
   //                     xfdbi->xfdbi_Flags = XFDFB_RECOGEXTERN;
          if (xfdRecogBuffer(xfdbi))
           {
            xfdbi->xfdbi_TargetBufMemType = MEMF_ANY;
            if (xfdDecrunchBuffer(xfdbi))
             {
              FreeVec(xfdbi->xfdbi_SourceBuffer);
//              *size = xfdbi->xfdbi_TargetBufLen;
              *size   = xfdbi->xfdbi_TargetBufSaveLen;
              if ((*buf = AllocVec(*size, MEMF_ANY))) CopyMem(xfdbi->xfdbi_TargetBuffer,*buf,*size);
              else *buf = xfdbi->xfdbi_TargetBuffer;
             }
            else retval = -2;
           }
          xfdFreeObject(xfdbi);
         }
       }
#endif
     }
    else retval = -2;
    Close(in);
//D(bug("readfile() ends\n");Delay(50);)
    return(retval);
}

int getnewprot(old,new,mask)
int old,new,mask;
{
    int val,a;

    if (!mask) return(new);
    val=old&(~255);
    for (a=0;a<8;a++) {
        if (mask&(1<<a)) {
            if (old&(1<<a)) val|=(1<<a);
        }
        else if (new&(1<<a)) val|=(1<<a);
    }
    return(val);
}

int dateformat(flag)
int flag;
{
    int a;

    for (a=0;a<4;a++) {
        if (flag&(1<<a)) return(a);
    }
    return(a);
}

int checkscreenmode(mode)
int mode;
{
    DisplayInfoHandle *handle;
    struct DimensionInfo *dimension;
    char buf[256];
    int flags,minw,defw;

    if (mode==MODE_WORKBENCHUSE || mode==MODE_WORKBENCHCLONE || mode==MODE_PUBLICSCREEN)
        return(mode);
/*    if (!system_version2) {
        if (mode==HIRES_KEY || mode==HIRESLACE_KEY) return(mode);
        return(MODE_WORKBENCHCLONE);
    } */
    if (!(handle=FindDisplayInfo(mode)) ||
        !(GetDisplayInfoData(handle,buf,256,DTAG_DISP,0)) ||
        ((struct DisplayInfo *)buf)->NotAvailable)
        return(MODE_WORKBENCHCLONE);
    if (CyberGfxBase)
     {
      if (IsCyberModeID(mode))
       {
        if (GetCyberIDAttr(CYBRIDATTR_WIDTH,mode) >= 640) return (mode);
       }
     }
    flags=((struct DisplayInfo *)buf)->PropertyFlags;
    if (flags&DIPF_IS_DUALPF || flags&DIPF_IS_PF2PRI || flags&DIPF_IS_HAM ||
        !(GetDisplayInfoData(handle,buf,256,DTAG_DIMS,0)))
        return(MODE_WORKBENCHCLONE);
    dimension=(struct DimensionInfo *)buf;
    if (mode&HIRES_KEY || mode&HIRESLACE_KEY) minw=640;
    else {
        minw=dimension->MinRasterWidth;
        if (minw<160) minw*=10;
    }
    defw=(dimension->TxtOScan.MaxX-dimension->TxtOScan.MinX)+1;
    if ((minw<640 || defw<640) && dimension->MaxDepth>=5)
        return(MODE_WORKBENCHCLONE);
    return(mode);
}

void doarrowgadgets(gad,dir)
struct Gadget *gad;
int dir;
{
    int a;

    for (a=0;a<2;a++) {
        if (!gad) break;
        do3dbox(main_rp,
            gad->LeftEdge+2,
            gad->TopEdge+1,
            gad->Width-4,
            gad->Height-2);
        DoArrow(main_rp,
            gad->LeftEdge+2,
            gad->TopEdge+1,
            gad->Width-4,
            gad->Height-2,
            screen_pens[config->arrowfg].pen,
            screen_pens[config->arrowbg].pen,dir);
        gad=gad->NextGadget;
    }
}

void makermbgad(rmbgad,gad,high)
struct RMBGadget *rmbgad;
struct Gadget *gad;
int high;
{
    rmbgad->x=gad->LeftEdge;
    rmbgad->y=gad->TopEdge;
    rmbgad->w=gad->Width;
    rmbgad->h=gad->Height;
    if (high && gad->GadgetRender) {
        rmbgad->flags|=RGF_ALTBORD;
        rmbgad->high_bord[0]=gad->GadgetRender;
        rmbgad->high_bord[1]=gad->SelectRender;
    }
    else rmbgad->flags&=~RGF_ALTBORD;
}

void dolittlegads(gad,txt,num)
struct Gadget *gad;
char *txt;
int num;
{
    int a,y;
    struct TextFont *font;

    if (status_iconified || !Window) return;

    font=main_rp->Font;
    SetFont(main_rp,scr_font[FONT_GENERAL]);
    for (a=0;a<num;a++) {
        do3dbox(main_rp,gad->LeftEdge+2,gad->TopEdge+1,gad->Width-4,gad->Height-2);
        SetAPen(main_rp,screen_pens[config->littlegadbg].pen);
        rectfill(main_rp,gad->LeftEdge+2,gad->TopEdge+1,gad->Width-4,gad->Height-2);
        SetAPen(main_rp,screen_pens[config->littlegadfg].pen);
        SetBPen(main_rp,screen_pens[config->littlegadbg].pen);
        y=gad->TopEdge+1+(((gad->Height-2)-scr_font[FONT_GENERAL]->tf_YSize)/2);
        if (y<gad->TopEdge+1) y=gad->TopEdge+1;
        y+=scr_font[FONT_GENERAL]->tf_Baseline;
        Move(main_rp,gad->LeftEdge+((gad->Width-scr_font[FONT_GENERAL]->tf_XSize)/2),y);
        Text(main_rp,&txt[a],1);
        RefreshGList(gad,Window,NULL,1);
        if (!(gad=gad->NextGadget)) break;
    }
    SetFont(main_rp,font);
}

void rectfill(r,x,y,w,h)
struct RastPort *r;
int x,y,w,h;
{
    if (w<0 || h<0) return;
    RectFill(r,x,y,x+w-1,y+h-1);
}

int isvalidwindow(win)
int win;
{
    if (!str_pathbuffer[win]) return(0);
    if (dopus_curwin[win]->total>0 && dopus_curwin[win]->firstentry->type==ENTRY_CUSTOM)
        return(0);
    return(1);
}

int _isdigit (unsigned char c)
{
 return locale?IsDigit(locale,c):isdigit(c);
}

int _isxdigit (unsigned char c)
{
 return locale?IsXDigit(locale,c):isxdigit(c);
}

int _isprint (unsigned char c)
{
 return locale?IsPrint(locale,c):isprint(c);
}

int _isspace (unsigned char c)
{
 return locale?IsSpace(locale,c):isspace(c);
}

int _isupper (unsigned char c)
{
 return locale?IsUpper(locale,c):isupper(c);
}

int _ispunct (unsigned char c)
{
 return locale?IsPunct(locale,c):ispunct(c);
}
