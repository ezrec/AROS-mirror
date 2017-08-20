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
#include <proto/locale.h>

#define EXALL_NUM 2
#define EXALL_BUFSIZE ((sizeof(struct ExAllData)+160)*EXALL_NUM)

void freedir(dir,win)
struct DirectoryWindow *dir;
int win;
{
        struct Directory *free,*work;

        if (dir) {
D(bug("freedir: %lx (directory: %s\tflags: %lx)\n",dir,dir->directory?dir->directory:"<NULL>",dir->flags));
                if (dir->firstentry && dir->total>0) {
                        if (dir->firstentry) {
                                free=dir->firstentry;
                                while (free) {
                                        if (free->next) work=free->next;
                                        else work=NULL;
                                        free->next=NULL;
                                        free->last=NULL;

                                        free_file_memory(free);
                                        free=work;
                                }
                        }
                }
                dir->firstentry =
                dir->firstfile =
                dir->firstdir = NULL;

                dir->offset =
                dir->total =
                dir->filesel =
                dir->dirsel=
                dir->dirtot =
                dir->filetot =
                dir->hoffset =
                dir->bytessel =
                dir->bytestot =
                dir->custhandler[0] =
                dir->realdevice[0] =
                dir->volumename[0] = 0;

                dir->oldoff =
                dir->oldhoff = -1;

                if ((!(dir->flags & DWF_ARCHIVE)) || (win == -1))
                 {
                  dir->flags=0;
                  freearchive(dir);
                 }
                if (Window && win>-1) {
                        fixhorizprop(win);
                        last_selected_entry=NULL;
                }
        }
}

int getdir(dir,win,incmess)
struct DirectoryWindow *dir;
int win,incmess;
{
        struct FileInfoBlock __aligned fileinfo;
        int tot=1,a,use_exall=0,exall_entry,exall_continue,/*exall_bufsize,*/subtype,exall_type=ED_OWNER;
        BPTR mylock;
        char buf[256];
        struct ExAllControl *exall_control;
        struct ExAllData *exall_buffer,*exall_current;
        struct MsgPort *deviceport;

        endnotify(win);
D(bug("dir: %lx\tDWF_ARCHIVE flag: %ld\n",dir,dir->flags&DWF_ARCHIVE));
        freedir(dir,win);

        vert_propinfo[win].VertPot=0;
        if (Window && !status_iconified) {
                refreshwindow(win,1);
                SetAPen(main_rp,screen_pens[(win==data_active_window)?config->disknameselbg:config->disknamebg].pen);
                rectfill(main_rp,
                        scrdata_diskname_xpos[win]+2,
                        scrdata_diskname_ypos,
                        scrdata_diskname_width[win],
                        scrdata_diskname_height-2);
                SetAPen(main_rp,screen_pens[1].pen);
        }

        if (!(mylock=Lock(dir->directory,ACCESS_READ))) {
                if (OpenXADlib()) if (readarchive(dir, win))
                 {
                  unbusy();
                  seename(win);
                  refreshwindow(win,1);
                  startnotify(win);
                  return(1);
                 }
                if (Window) doerror(-1);
                for (a=0;a<30;a++) {
                        if (str_pathbuffer[win][a]==':' || str_pathbuffer[win][a]==0) break;
                }
                if (str_pathbuffer[win][a]==':') LStrnCpy(dir->realdevice,str_pathbuffer[win],a+1);
                return(0);
        }

        strcpy(buf,str_pathbuffer[win]);
        if (getroot(buf,NULL)) {
                strcpy(dir->volumename,buf);
                strcat(buf,":");
                if ((deviceport=(struct MsgPort *)DeviceProc(buf)))
                        get_device_task(mylock,dir->realdevice,deviceport);
        }

        if (config->dirflags&DIRFLAGS_EXPANDPATHS) {
                PathName(mylock,buf,256);
                strcpy(str_pathbuffer[win],buf);
                checkdir(str_pathbuffer[win],Window?&path_strgadget[win]:NULL);
                strcpy(dir->directory,str_pathbuffer[win]);
        }

        Examine(mylock,&fileinfo);
        if (fileinfo.fib_DirEntryType<0) {
                UnLock(mylock);
                if (OpenXADlib()) if (readarchive(dir, win))
                 {
                  unbusy();
                  seename(win);
                  refreshwindow(win,1);
                  startnotify(win);
                  return(1);
                 }
                if (Window) doerror(ERROR_OBJECT_WRONG_TYPE);
                return(0);
        }
        dir->flags=0;
        dir->firstentry=NULL;
        freearchive(dir);

        if (Window) {
                if (incmess) dostatustext(globstring[STR_READING_CHANGED_DIRECTORY]);
                else dostatustext(globstring[STR_READING_DIRECTORY]);
                busy();
        }
        copy_datestamp(&fileinfo.fib_Date,&dir->dirstamp);

        if (config->dirflags&DIRFLAGS_EXALL && system_version2>=OSVER_40) {
                if ((exall_control=AllocDosObject(DOS_EXALLCONTROL,NULL))) {
                        if ((exall_buffer=(struct ExAllData *) AllocMem(EXALL_BUFSIZE,MEMF_CLEAR))) {
                                use_exall=1;
                                exall_entry=0;
                                exall_continue=1;
                                exall_control->eac_Entries=0;
                                exall_control->eac_LastKey=0;
                                exall_control->eac_MatchString=NULL;
                                exall_control->eac_MatchFunc=NULL;
                                exall_current=NULL;
                        }
                        else {
                                FreeDosObject(DOS_EXALLCONTROL,exall_control);
                                exall_control=NULL;
                        }
                }
        }

        FOREVER {
//                fileinfo.fib_OwnerUID=fileinfo.fib_OwnerGID=0;
                if (use_exall) {
                        if (exall_entry>=exall_control->eac_Entries || !exall_current) {
                                if (!exall_continue) break;
                                exall_continue=ExAll(mylock,exall_buffer,EXALL_BUFSIZE,exall_type,exall_control);
                                if (!exall_continue)
                                 {
                                  if (IoErr() == ERROR_BAD_NUMBER)
                                    exall_continue=ExAll(mylock,exall_buffer,EXALL_BUFSIZE,exall_type=ED_COMMENT,exall_control);
                                 }
                                exall_entry=0;
                                exall_current=exall_buffer;
                                continue;
                        }
                        if (exall_current->ed_Name)
                                strcpy(fileinfo.fib_FileName,exall_current->ed_Name);
                        else fileinfo.fib_FileName[0]=0;

                        fileinfo.fib_DirEntryType=exall_current->ed_Type;
                        fileinfo.fib_Size=exall_current->ed_Size;
                        fileinfo.fib_Protection=exall_current->ed_Prot;
                        fileinfo.fib_Date.ds_Days=exall_current->ed_Days;
                        fileinfo.fib_Date.ds_Minute=exall_current->ed_Mins;
                        fileinfo.fib_Date.ds_Tick=exall_current->ed_Ticks;
                        fileinfo.fib_OwnerUID=(exall_type==ED_OWNER)?exall_current->ed_OwnerUID:0;
                        fileinfo.fib_OwnerGID=(exall_type==ED_OWNER)?exall_current->ed_OwnerGID:0;
                        if (exall_current->ed_Comment)
/*
                                BtoCStr((BSTR)exall_current->ed_Comment,fileinfo.fib_Comment,79);
*/
                                strcpy(fileinfo.fib_Comment,exall_current->ed_Comment);
                        else fileinfo.fib_Comment[0]=0;
                        ++exall_entry;
                        exall_current=exall_current->ed_Next;
                }
                else {
                        if (!(ExNext(mylock,&fileinfo))) break;
                }

                subtype=fileinfo.fib_DirEntryType;
//D(bug("name: %s\ttype: %ld\n",fileinfo.fib_FileName,fileinfo.fib_DirEntryType));
                if (subtype == ST_SOFTLINK)
                 {
                  struct FileInfoBlock __aligned fib = fileinfo;
                  struct DevProc *dp;
                  BPTR ld, lf;
                  char linkbuf[512];

                  NameFromLock(mylock,linkbuf,512);
                  do
                   {
D(bug("SoftLink! path: %s/%s\n",linkbuf,fib.fib_FileName));
                    ld = Lock(linkbuf,ACCESS_READ);
                    if ((dp = GetDeviceProc(linkbuf,NULL)))
                     {
//D(bug("DeviceProc: %lX\n",dp));
                      if (ReadLink(dp->dvp_Port,ld,fib.fib_FileName,buf,256))
                       {
                        AddPart(linkbuf,buf,512);
D(bug("Resolved: %s\n",linkbuf));
                        if ((lf = Lock(linkbuf,ACCESS_READ)))
                         {
                          if (Examine(lf,&fib))
                           {
                            switch (fib.fib_DirEntryType)
                             {
                              case ST_ROOT:
                              case ST_USERDIR:
                              case ST_LINKDIR:
                                fileinfo.fib_DirEntryType = ENTRY_DIRECTORY;
                                break;
                              case ST_SOFTLINK:
                                NameFromLock(lf,linkbuf,512);
                                *PathPart(linkbuf) = 0;
                                break;
                              default:
                                fileinfo.fib_DirEntryType = ENTRY_FILE;
//                                  fileinfo.fib_Size = fib.fib_Size;
                                break;
                             }
                           }
                          UnLock(lf);
                         }
                        else
                         {
// D(bug("Lock() failed!\n"));
                          fileinfo.fib_DirEntryType = fib.fib_DirEntryType = ENTRY_FILE;
                          fileinfo.fib_Size = 0;
                         }
                       }
                      FreeDeviceProc(dp);
                      UnLock(ld);
                     }
                   } while (fib.fib_DirEntryType == ST_SOFTLINK);
                 }
                if (status_haveaborted && Window) {
                        myabort();
                        dir->flags|=DWF_ABORTED;
                        tot=-1;
                        break;
                }
                if (!(addfile(dir,win,fileinfo.fib_FileName,
                        (fileinfo.fib_DirEntryType>0)?-1:fileinfo.fib_Size,
                        fileinfo.fib_DirEntryType,
                        &fileinfo.fib_Date,fileinfo.fib_Comment,fileinfo.fib_Protection,subtype,
                        FALSE,NULL,NULL,fileinfo.fib_OwnerUID,fileinfo.fib_OwnerGID))) {
                        if (Window) doerror(-1);
                        tot=0;
                        break;
                }
/* Code deleted by Pavel Cizek, 28. 3. 2000:
 *   This code was disable to speed-up directory reading.
 *   Its deletion causes no proportional-gadget refresh during scanning of a directory.
 *   The final update of directory proportional gadget is done by call refreshwindow(win,1)
 *   below via the same function - fixprop(win).
*/
                else if (Window) if ( config->dynamicflags & UPDATE_QUIETGETDIR ) fixprop(win); //HUX
/* * End of deleted code
 */
        }
        UnLock(mylock);

        if (Window) {
                unbusy();
                if (tot) {
                        seename(win);
                        if (tot==1) okay();
                }
                refreshwindow(win,1);
        }
        if (use_exall) {
                if (exall_control) FreeDosObject(DOS_EXALLCONTROL,exall_control);
                if (exall_buffer) FreeMem(exall_buffer,EXALL_BUFSIZE);
        }
        startnotify(win);
        return(1);
}

void fixprop(win)
int win;
{
        if (win>-1 && !status_iconified) {
                FixSliderBody(Window,&vert_propgad[win],dopus_curwin[win]->total,scrdata_dispwin_lines,0);
                fixvertprop(win);
        }
}

void fixvertprop(win)
int win;
{
        if (win>-1 && !status_iconified) {
                FixSliderPot(Window,&vert_propgad[win],dopus_curwin[win]->offset,
                        dopus_curwin[win]->total,scrdata_dispwin_lines,2);
        }
}

void fixhorizprop(win)
int win;
{
        if (win>-1 && !status_iconified) {
                fixhlen(win);
                if (Window) {
                        FixSliderBody(NULL,&horiz_propgad[win],dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win],0);
                        FixSliderPot(Window,&horiz_propgad[win],dopus_curwin[win]->hoffset,dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win],2);
                }
        }
}

int doposprop(win)
int win;
{
        int i;

        if (win<0 || status_iconified) return(0);
        if (!(status_flags&STATUS_NEWLOOK) &&
                vert_propgad[win].MutualExclude) ShowSlider(Window,&vert_propgad[win]);
        i=GetSliderPos(&vert_propgad[win],dopus_curwin[win]->total,scrdata_dispwin_lines);
        if (dopus_curwin[win]->offset==i) return(0);
        dopus_curwin[win]->offset=i;
        displaydir(win);
        return(1);
}

void doposhprop(win)
int win;
{
        int i;

        if (!status_iconified && win>-1) {
                if (!(status_flags&STATUS_NEWLOOK) &&
                        horiz_propgad[win].MutualExclude) ShowSlider(Window,&horiz_propgad[win]);
                i=GetSliderPos(&horiz_propgad[win],dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win]);
                if (dopus_curwin[win]->hoffset==i) return;
                dopus_curwin[win]->hoffset=i; dopus_curwin[win]->oldoff=-1;
                displaydir(win);
        }
}

void checkdir(str,gad)
char *str;
struct Gadget *gad;
{
        struct StringInfo *sinfo;

        TackOn(str,NULL,256);
        if (gad) {
                sinfo=(struct StringInfo *)gad->SpecialInfo;
                gad->Flags|=GADGHNONE;
/*                if (!system_version2) {
                        sinfo->BufferPos=0; RefreshGList(gad,Window,NULL,1);
                } */
                sinfo->BufferPos=strlen(str); RefreshGList(gad,Window,NULL,1);
                gad->Flags&=~GADGHNONE;
        }
}

void verticalscroll(win,dir)
int win,dir;
{
        if (win<0 || dopus_curwin[win]->total<=scrdata_dispwin_lines) return;

        if (dir<0) {
                --dopus_curwin[win]->offset;
                if (dopus_curwin[win]->offset<0) {
                        dopus_curwin[win]->offset=0;
                        return;
                }
        }
        else {
                ++dopus_curwin[win]->offset;
                if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines) {
                        dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
                        return;
                }
        }
        fixvertprop(win);
        displaydir(win);
}

void horizontalscroll(win,dir)
int win,dir;
{
        if (win<0 || dopus_curwin[win]->total==0 || dopus_curwin[win]->firstentry->type==ENTRY_DEVICE)
                return;
        if (dir<0) {
                if (dopus_curwin[win]->hoffset==0) return;
                dopus_curwin[win]->hoffset-=scrdata_font_xsize;
                if (dopus_curwin[win]->hoffset<0) dopus_curwin[win]->hoffset=0;
        }
        else {
                if (dopus_curwin[win]->hoffset==(dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win])) return;
                dopus_curwin[win]->hoffset+=scrdata_font_xsize;
                if (dopus_curwin[win]->hoffset>(dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win]))
                  dopus_curwin[win]->hoffset=dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win];
        }
        refreshwindow(win,1);
}

void findfirstsel(win,type)
int win,type;
{
        struct Directory *sel;
        int a;

        if (win<0 || dopus_curwin[win]->total<=scrdata_dispwin_lines) return;
        sel=dopus_curwin[win]->firstentry; a=0;
        while (sel) {
                if (sel->selected) {
                        if (type==-2 || ENTRYTYPE(sel->type)==type) break;
                }
                sel=sel->next;
                ++a;
        }
        if (sel) {
                dopus_curwin[win]->offset=a;
                if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
                        dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
                fixvertprop(win);
                displaydir(win);
        }
        return;
}

void findfirstchar(win,c)
int win;
char c;
{
        struct Directory *sel;
        int a=0,b=0,file=1,rev;
        char d;

        if (_isupper(c)) { file=0; c=ToLower(c); }
        if (dopus_curwin[win]->total<=scrdata_dispwin_lines) return;
        rev = (config->sortflags&(win?SORT_RREVERSE:SORT_LREVERSE))?1:0;
        sel=dopus_curwin[win]->firstentry;
        while (sel) {
                d = ToLower(sel->name[0]);
                if ((rev && (d<=c)) || (!rev && (d>=c))) {
                        if (!file || ENTRYTYPE(sel->type)==ENTRY_FILE) {
                                b=1;
                                break;
                        }
                }
                /*if (sel->next)*/ sel=sel->next;
//                else sel=NULL;
                ++a;
        }
        if (!b) dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
        else {
                dopus_curwin[win]->offset=a;
                if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
                        dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
        }
        if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;
        fixvertprop(win);
        displaydir(win);
}

void doinfodisplay(entry,state)
struct Directory *entry;
int state;
{
        char buf[MAXDISPLAYLENGTH],pbuf[10],dbuf[30];

        buf[0]=0;
        if (entry->selected==state) {
                getprot(entry->protection,pbuf);
                nodayseedate(&(entry->date),dbuf);
                lsprintf(buf,"%8s %18s ",pbuf,dbuf);
                if (entry->type!=ENTRY_CUSTOM && entry->comment)
                        strcat(buf,entry->comment);
        }
        dostatustext(buf);
}

void nodayseedate(ds,date)
struct DateStamp *ds;
char *date;
{
        char datebuf[16],timebuf[16];
        struct DOpusDateTime dt;

        copy_datestamp(ds,&dt.dat_Stamp);
        initdatetime(&dt,datebuf,timebuf,0);
//        strcpy(date,datebuf); strcat(date," "); strcat(date,timebuf);
        lsprintf(date,"%s %s",datebuf,timebuf);
}

void displaydirgiven(win,dir,bypass)
int win;
struct Directory *dir;
char bypass;
{
        struct Directory *display;
        int off;

        if (win<0 ||
                (!bypass && !(config->dynamicflags&UPDATE_SCROLL))) return;

        display=dopus_curwin[win]->firstentry;
        off=0;
        while (display) {
                if (display==dir) break;
                display=display->next;
                ++off;
        }
        if (!display) return;
        dopus_curwin[win]->offset=off;
        if (dopus_curwin[win]->oldoff>-1) {
                if (dopus_curwin[win]->offset>=dopus_curwin[win]->oldoff &&
                        dopus_curwin[win]->offset<dopus_curwin[win]->oldoff+scrdata_dispwin_lines)
                        dopus_curwin[win]->offset=dopus_curwin[win]->oldoff;
        }
        if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
                dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
        if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;
        if (dopus_curwin[win]->offset!=dopus_curwin[win]->oldoff) {
                displaydir(win);
                fixvertprop(win);
        }
}

void endfollow(win)
int win;
{
        if (win>-1 && scrdata_old_offset_store>-1) {
                dopus_curwin[win]->offset=scrdata_old_offset_store;
                if (dopus_curwin[win]->offset>(dopus_curwin[win]->total-scrdata_dispwin_lines))
                        dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
                if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;
                fixvertprop(win);
                displaydir(win);
                scrdata_old_offset_store=-1;
        }
}

void displaydir(win)
int win;
{
  int d,a,l,tl,bl,b,ds,my,c,/*f,*/pw,px,lpst8,sc,to,y;
  char sbuf[MAXDISPLAYLENGTH];
  struct Directory *entry;
  struct Region *oldreg,*newreg;
  struct Rectangle rect;

  if (win<0 || status_iconified) return;
  l=dopus_curwin[win]->oldoff-dopus_curwin[win]->offset;
  entry=dopus_curwin[win]->firstentry;
  if (l<0) {
          tl=scrdata_dispwin_lines-1+l; bl=scrdata_dispwin_lines; a=-l;
  }
  else {
          tl=-1; bl=l; a=l;
  }
  b=scrdata_dispwin_lines-1;
  if (dopus_curwin[win]->oldoff==-1 || a>b) {
          tl=-1; bl=scrdata_dispwin_lines; ds=0; sc=0;
  }
  else {
          b=ABSI(l);
          if (config->dynamicflags&UPDATE_NOTIFY && b>(scrdata_dispwin_lines/4)) {
                  tl=-1; bl=scrdata_dispwin_lines; ds=0; sc=0;
          }
          else {
                  if (b==0) d=0;
                  else d=-(dopus_curwin[win]->oldoff-dopus_curwin[win]->offset)/b;
                  ds=1; sc=d;
          }
  }
  for (a=0;a<dopus_curwin[win]->offset;a++) {
          if (!entry) break;
          entry=entry->next;
  }
  if (sc<0 && ds) {
          c=b-1;
          for (a=0;a<c;a++) {
                  if (!entry) break;
                  entry=entry->next;
          }
  }
  if (ds) {
          if (sc<0) my=scrdata_font_baseline;
          else my=(scrdata_font_baseline+scrdata_dirwin_height)-scrdata_font_ysize;
  }
  else my=scrdata_font_ysize+scrdata_font_baseline+(tl*scrdata_font_ysize);
  my+=scrdata_dirwin_ypos[win];
  lpst8=scrdata_dirwin_height-scrdata_font_ysize;

  ++tl;
  for (a=0;a<tl;a++) {
          if (!entry) break;
          if (sc<0) entry=entry->last;
          else entry=entry->next;
  }
  pw=scrdata_dispwin_nchars[win]; px=scrdata_dirwin_xpos[win]-dopus_curwin[win]->hoffset+1; to=0;
/*
  if (dopus_curwin[win]->oldhoff!=-1) {
          f=dopus_curwin[win]->oldhoff-dopus_curwin[win]->hoffset;
          if (f && f>-scrdata_dispwin_nchars[win] && f<scrdata_dispwin_nchars[win]) {
                  if (f>0) ClipBlit(&dir_rp[win],scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],&dir_rp[win],
                          (f*scrdata_font_xsize)+scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],scrdata_dirwin_width[win]-(f*scrdata_font_xsize),scrdata_dirwin_height,0xc0);
                  else {
                          f=-f;
                          ClipBlit(&dir_rp[win],(f*scrdata_font_xsize)+scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],&dir_rp[win],
                                  scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],scrdata_dirwin_width[win]-(f*scrdata_font_xsize),scrdata_dirwin_height,0xc0);
                          px=(scrdata_dirwin_width[win]-(f*scrdata_font_xsize))+scrdata_dirwin_xpos[win]; to=scrdata_dispwin_nchars[win]-f;
                  }
                  pw=f;
          }
  }
*/
  if (tl<bl) {
    if ((newreg = NewRegion()))
     {
      rect.MinX=scrdata_dirwin_xpos[win]+1;
      rect.MinY=scrdata_dirwin_ypos[win];
      rect.MaxX=rect.MinX+scrdata_dispwin_width[win]-3;
      rect.MaxY=rect.MinY+scrdata_dispwin_height-1;

      OrRectRegion(newreg,&rect);
      oldreg = InstallClipRegion(Window->WLayer,newreg);
     }
//D(bug("px = %ld\toldhoff = %ld\thoffset = %ld\n",px,dopus_curwin[win]->oldhoff,dopus_curwin[win]->hoffset));
    for (a=tl;a<bl;a++) {
            y=my;
            if (ds) {
                    if (sc>0) ClipBlit(&dir_rp[win],scrdata_dirwin_xpos[win],scrdata_font_ysize+scrdata_dirwin_ypos[win],&dir_rp[win],
                            scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],scrdata_dirwin_width[win],lpst8,0xc0);
                    else ClipBlit(&dir_rp[win],scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],&dir_rp[win],
                            scrdata_dirwin_xpos[win],scrdata_font_ysize+scrdata_dirwin_ypos[win],scrdata_dirwin_width[win],lpst8,0xc0);
            }
            else my+=scrdata_font_ysize;
            //setdispcol(entry,win);
/*
            if (entry->type == ENTRY_CUSTOM)
             {
*/
              builddisplaystring(entry,sbuf,win);
              entry_text(win,entry,&sbuf[to],pw,px,y);
/*
             }
            else
             {
              if (!entry->dispstr)
               {
D(bug("Initializing entry->dispstr\n"));
                entry->dispstr = LibAllocPooled(dir_memory_pool,MAXDISPLAYLENGTH);
                builddisplaystring(entry,entry->dispstr?entry->dispstr:sbuf,win);
               }
              entry_text(win,entry,(entry->dispstr?entry->dispstr:sbuf)+to,pw,px,y);
             }
*/
            if (entry) {
                    if (sc<0) entry=entry->last;
                    else entry=entry->next;
            }
    }
    if (newreg)
     {
      InstallClipRegion(Window->WLayer,oldreg);
      DisposeRegion(newreg);
     }
  }
  dopus_curwin[win]->oldoff=dopus_curwin[win]->offset;
  dopus_curwin[win]->oldhoff=dopus_curwin[win]->hoffset;
}

void display_entry(entry,win,x,y)
struct Directory *entry;
int win,x,y;
{
  char dispbuf[MAXDISPLAYLENGTH];
  struct Region *oldreg,*newreg;
  struct Rectangle rect;

  //setdispcol(entry,win);
  builddisplaystring(entry,dispbuf,win);
  if ((newreg = NewRegion()))
   {
    rect.MinX=scrdata_dirwin_xpos[win]+1;
    rect.MinY=y-scrdata_font_baseline;
    rect.MaxX=rect.MinX+scrdata_dispwin_width[win]-3;
    rect.MaxY=rect.MinY+scrdata_font_ysize-1;

    OrRectRegion(newreg,&rect);
    oldreg = InstallClipRegion(Window->WLayer,newreg);
   }
  entry_text(win,entry,dispbuf,scrdata_dispwin_nchars[win],x,y);
  if (newreg)
   {
    InstallClipRegion(Window->WLayer,oldreg);
    DisposeRegion(newreg);
   }
}

#define TEXT_END  0x00 // End of TextCoded string
#define TEXT_MOVE 0x01 // Move cursor
#define TEXT_PENS 0x02 // Text pens (fgpen<<8 | bgpen)
#define TEXT_STYL 0x03 // Font style

struct TextCode
 {
  char  type; // 0 = control code
  UBYTE code; // TEXT_xxxx defined above
  union
   {
    WORD skip;       // TEXT_MOVE
    struct
     {
      UBYTE fg, bg;
     } tc_pens;      // TEXT_PENS
    struct
     {
      UBYTE style,stylemask;
     } tc_style;     // TEXT_STYL
   } mode;
 };
#ifndef __AROS__
static void inline PUTCODE(char **c, UWORD x, WORD y)
 {
  ULONG *tc = (ULONG *)(*c);

  *tc = (x<<16)|(y&0xFFFF);
  *c += sizeof(struct TextCode);
 }
#else
static void inline PUTCODE(char **c, UWORD x, WORD y)
 {
  struct TextCode *tc = (struct TextCode *)*c;

  tc->type = 0;
  tc->code = x;
  if (x == TEXT_MOVE)
  {
    tc->mode.skip = y;
  }
  else
  {
    tc->mode.tc_pens.fg = y >> 8;
    tc->mode.tc_pens.bg = y & 0xff;
  }
  *c += sizeof(struct TextCode);
 }
#endif

/* text should start with TEXT_PENS! */
void drawentry(char *text, int win)
{
  struct RastPort *rp = &dir_rp[win];
  int len=0,/*x0=rp->cp_x,*/x,y0,y1,fg/*=GetAPen(rp)*/,bg/*=GetBPen(rp)*/;
  char *t=NULL;
  struct TextCode *c=(struct TextCode *)text;
  WORD skip;
  BOOL quit=FALSE;

//D(KDump(text,32));
  y0=rp->cp_y-scrdata_font_baseline;
  y1=y0+rp->Font->tf_YSize-1;

  while(!quit)
   {
    switch(c->type)
     {
      case 0: // control code - flush string and perform action
        if (len) Text(rp,t,len);
        t = NULL;
        len = 0;

        switch(c->code) // action code
         {
          case TEXT_PENS:
            SetABPenDrMd(rp,fg=screen_pens[c->mode.tc_pens.fg].pen,bg=screen_pens[c->mode.tc_pens.bg].pen,JAM2);
            break;
          case TEXT_STYL:
            SetSoftStyle(rp,c->mode.tc_style.style,c->mode.tc_style.stylemask);
            break;
          case TEXT_END:
            x = scrdata_dirwin_xpos[win]+scrdata_dispwin_width[win]-1;
            SetAPen(rp,bg);
            if (rp->cp_x < x) RectFill(rp,rp->cp_x,y0,x,y1);
            SetAPen(rp,fg);
            Move(rp,x,rp->cp_y);
            quit = TRUE;
            break;
          case TEXT_MOVE:
            SetAPen(rp,bg);
            skip = c->mode.skip;
            x=rp->cp_x+skip;
            if (skip < 0)
             {
              RectFill(rp,x,y0,x+scrdata_font_xsize,y1);
              x+=scrdata_font_xsize;
             }
            if (rp->cp_x < x) RectFill(rp,rp->cp_x,y0,x,y1);
            SetAPen(rp,fg);
            Move(rp,x,rp->cp_y);
            break;
         }
        c++;
        break;
      default:
        if (!t) t = &(c->type);
        len++;
        c=(struct TextCode *)(&(c->code));
        break;
     }
   }
}

void entry_text(win,entry,buf,len,x,y)
int win;
struct Directory *entry;
char *buf;
int len,x,y;
{
        struct RastPort *rp = &dir_rp[win];
        int a,final=0;

        Move(rp,x,y);
        if (entry && entry->type==ENTRY_CUSTOM && entry->subtype==CUSTOMENTRY_DIRTREE) {
                char pbuf[MAXDISPLAYLENGTH],*c=pbuf;

                //SetAPen(rp,screen_pens[config->dirsselfg].pen);
                //SetAPen(rp,screen_pens[config->dirsfg].pen);
                PUTCODE(&c,((struct TextCode *)buf)->code,((struct TextCode *)buf)->mode.skip);

                for (a=sizeof(struct TextCode);buf[a];a++) if (strchr("+-| ",buf[a])==NULL) break;
                PUTCODE(&c,TEXT_MOVE,(a-sizeof(struct TextCode))*scrdata_font_xsize);
                while(buf[a]) *c++=buf[a++];
                *c++=0;*c++=TEXT_END;

                drawentry(pbuf,win);

                y-=scrdata_font_baseline;
                for (a=sizeof(struct TextCode);buf[a];a++) {
                        if (buf[a]=='|') {
                                if (buf[a+1]=='-') draw_dirtree_gfx(rp,x,y,DIRTREEGFX_VERTCROSS);
                                else draw_dirtree_gfx(rp,x,y,DIRTREEGFX_VERT);
                        }
                        else if (buf[a]=='-') {
                                draw_dirtree_gfx(rp,x,y,(final)?DIRTREEGFX_FINALCROSS:DIRTREEGFX_CROSS);
                                final=1-final;
                        }
                        else if (buf[a]=='+') draw_dirtree_gfx(rp,x,y,DIRTREEGFX_BRANCH);
                        else if (buf[a]!=' ') break;
                        x+=scrdata_font_xsize;
                }
        }
        else drawentry(buf,win);
}

//#define DISPLAYSIZELENGTH     10
//#define DISPLAYSIZEFORMAT     "%9qd "
#define DISPLAYSIZEFORMAT     "%qd"

void buildkmgstring(char *buf, unsigned long long size, int kmgmode)
{
 if (kmgmode)
  {
   if (size > 1024)
    {
/*
     char tmp[116],c;

     if (size > 1024*1024)
      {
       if (size > 1024*1024*1024)
        {
         sprintf(tmp,"%.1f",size/(float)(1024*1024*1024));
         c = 'G';
        }
       else
        {
         sprintf(tmp,"%.1f",size/(float)(1024*1024));
         c = 'M';
        }
      }
     else
      {
       sprintf(tmp,"%.1f",size/(float)1024);
       c = 'K';
      }
*/
     char tmp[16],c;
     float div;

     if (size > 1024*1024)
      {
       if (size > 1024*1024*1024)
        {
         div = 1024*1024*1024;
         c = 'G';
        }
       else
        {
         div = 1024*1024;
         c = 'M';
        }
      }
     else
      {
       div = 1024;
       c = 'K';
      }
     sprintf(tmp,"%.1f",size/div);
//D(bug("buildkmgstring: %s\n",tmp));
     if (tmp[3] == '.') tmp[3] = 0;
     else if (tmp[4] == '.') tmp[4] = 0;
     lsprintf(buf,"%4s%lc",tmp,c);
    }
   else sprintf(buf,"%4qd",size);
  }
 else sprintf(buf, DISPLAYSIZEFORMAT,size);
}

void builddisplaystring(display,sbuf,win)
 struct Directory *display;
 char *sbuf;
 int win;
 {
  char sizebuf[20];
  int fg,bg;
  char *sptr,*c;
  int  a,b,w,l,w1;
  struct TextExtent te;
//D(char *dbg=sbuf;)

  if (display) switch (ENTRYTYPE(display->type)) {
    case ENTRY_CUSTOM:
      if (display->comment)
       {
        if (display->subtype==CUSTOMENTRY_USER) {
                if (display->selected) {
                        fg=config->filesselfg;
                        bg=config->filesselbg;
                }
                else {
                        if ((((int)display->size)&0xff)==0xff) fg=config->filesfg;
                        else fg=((int)display->size)&0xff;
                        if ((((int)display->size)&0xff00)==0xff00) bg=config->filesbg;
                        else bg=(((int)display->size)&0xff00)>>8;
                }
        }
        else {
                if (display->selected) {
                        fg=config->dirsselfg;
                        bg=config->dirsselbg;
                }
                else {
                        fg=config->dirsfg;
                        bg=config->dirsbg;
                }
        }
        PUTCODE(&sbuf,TEXT_PENS,fg<<8|bg);

        for (sptr=display->comment; *sptr;) *sbuf++=*sptr++;
       }
      break;
    case ENTRY_DEVICE:
      if (display->size==DLT_DEVICE || display->size==DLT_VOLUME) {
              if (display->selected) {
                      fg=config->filesselfg;
                      bg=config->filesselbg;
              }
              else {
                      fg=config->filesfg;
                      bg=config->filesbg;
              }
      }
      else {
              if (display->selected) {
                      fg=config->dirsselfg;
                      bg=config->dirsselbg;
              }
              else {
                      fg=config->dirsfg;
                      bg=config->dirsbg;
              }
      }
      PUTCODE(&sbuf,TEXT_PENS,fg<<8|bg);

      sptr=display->name;
      w=TextLength(&dir_rp[win],sptr,b=strlen(sptr))+scrdata_font_xsize;
      if (w > config->displaylength[win][DISPLAY_NAME])
       {
        b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_NAME]-scrdata_font_xsize,0x7FFF);
        w=te.te_Width+scrdata_font_xsize;
       }
      for (;*sptr && b; b--) *sbuf++=*sptr++;
      PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_NAME]-w);
      sptr=display->comment;
      w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
      if (w > config->displaylength[win][DISPLAY_SIZE])
       {
        b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_SIZE],0x7FFF);
        w=te.te_Width;
       }
      for (;*sptr && b; b--) *sbuf++=*sptr++;
      break;
    default:
      if (ENTRYTYPE(display->type)==ENTRY_FILE) {
        if (display->selected) {
                fg=config->filesselfg;
                bg=config->filesselbg;
        }
        else {
                fg=config->filesfg;
                bg=config->filesbg;
        }
      }
      else {
        if (display->selected) {
                fg=config->dirsselfg;
                bg=config->dirsselbg;
        }
        else {
                fg=config->dirsfg;
                bg=config->dirsbg;
        }
      }
//D(bug("builddisplaystring: fg=%ld,bg=%ld\n",fg,bg));
      PUTCODE(&sbuf,TEXT_PENS,fg<<8|bg);

      if (display->type<ENTRY_DEVICE || (display->type>ENTRY_DEVICE && display->size>=0))
        buildkmgstring(sizebuf,display->size,config->listerdisplayflags[win] & SIZE_KMG);
      else
        sizebuf[0]=0;

      for (a=0;a<DISPLAY_LAST+1;a++) {
        w=0;
        switch (config->displaypos[win][a]) {
          case DISPLAY_NAME:
            if ((sptr=display->name)) {
              switch (display->subtype)
               {
                case ST_SOFTLINK:
                case ST_LINKDIR:
                case ST_LINKFILE:
                  l = 1;
                  break;
                default:
                  l = 0;
               }
              if (l) PUTCODE(&sbuf,TEXT_STYL,FSF_UNDERLINED<<8|FSF_UNDERLINED);

              w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
              if (w > (config->displaylength[win][DISPLAY_NAME]-scrdata_font_xsize))
               {
                b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_NAME]-scrdata_font_xsize,0x7FFF);
                w=te.te_Width;
  //                            l=-1;
               }
              for (;*sptr && b; b--) *sbuf++=*sptr++;
  //                          if (l==-1) *sbuf++=0xBB;
              if (l) PUTCODE(&sbuf,TEXT_STYL,FS_NORMAL<<8|FSF_UNDERLINED);
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_NAME]-w);
            break;
          case DISPLAY_SIZE:
            sptr=sizebuf;
            w=TextLength(&dir_rp[win],sptr,b=strlen(sptr))+scrdata_font_xsize;
            l = config->displaylength[win][DISPLAY_SIZE]-w;
            if (l<0)
             {
              PUTCODE(&sbuf,TEXT_MOVE,l-scrdata_font_xsize/*TextLength(&dir_rp[win]," ",1)*/);
//              *sbuf++=' ';
//              for (;*sptr && b; b--) *sbuf++=*sptr++;
//              b=TextFit(&dir_rp[win],sptr+l-1,l,&te,NULL,-1,config->displaylength[win][DISPLAY_SIZE]-scrdata_font_xsize,0x7FFF);
//              w=te.te_Width+scrdata_font_xsize;
//              sptr+=l-b;
             }
            else PUTCODE(&sbuf,TEXT_MOVE,l);
            for (;*sptr && b; b--) *sbuf++=*sptr++;
            PUTCODE(&sbuf,TEXT_MOVE,scrdata_font_xsize);
            break;
          case DISPLAY_PROTECT:
            w=0;
            if ((sptr=display->protbuf)) {
              w=TextLength(&dir_rp[win],sptr,b=8/*strlen(sptr)*/);
              for (;*sptr/* && b*/; /*b--*/) *sbuf++=*sptr++;
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_PROTECT]-w+scrdata_font_xsize);
            break;
          case DISPLAY_DATE:
            if ((sptr=display->datebuf)) {
              for(c=sptr;*c;c++) if (*c==' ') break;
              w=TextLength(&dir_rp[win],sptr,b=(long)c-(long)sptr)+scrdata_font_xsize;
  /*
              if (w > column[3])
               {
                b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,column[3]-scrdata_font_xsize,0x7FFF);
                w=te.te_Width+scrdata_font_xsize;
               }
  */
              for (;*sptr && b; b--) *sbuf++=*sptr++;
  //            if (w < column[3])
               {
                for(sptr=c;*sptr;sptr++) if (*sptr!=' ') break;
  //D(bug("datebuf:%s\n",sptr));
                w1=config->displaylength[win][DISPLAY_DATE]-w;
                w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
  /*              if (w > w1)
                 {
                  b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,w1,0x7FFF);
                  w=te.te_Width;
                 }
  */
  //D(bug("sptr = %s, b = %ld, w = %ld, w1 = %ld\n",sptr,b,w,w1));
                PUTCODE(&sbuf,TEXT_MOVE,w1-w+scrdata_font_xsize);
                for (;*sptr/* && b*/; /*b--*/) *sbuf++=*sptr++;
               }
              PUTCODE(&sbuf,TEXT_MOVE,scrdata_font_xsize);
            }
            else PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_DATE]-w+scrdata_font_xsize);
            break;
          case DISPLAY_COMMENT:
            if ((sptr=display->comment)) {
              w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
              if (w > (config->displaylength[win][DISPLAY_COMMENT]-scrdata_font_xsize))
               {
                b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_COMMENT]-scrdata_font_xsize,0x7FFF);
                w=te.te_Width;
               }
              for (;*sptr && b; b--) *sbuf++=*sptr++;
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_COMMENT]-w);
            break;
          case DISPLAY_FILETYPE:
            if ((sptr=display->description)) {
              w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
              if (w > (config->displaylength[win][DISPLAY_FILETYPE]-scrdata_font_xsize))
               {
                b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_FILETYPE]-scrdata_font_xsize,0x7FFF);
                w=te.te_Width;
               }
              for (;*sptr && b; b--) *sbuf++=*sptr++;
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_FILETYPE]-w);
            break;
          case DISPLAY_OWNER:
            if (AccountsBase || muBase) {
              if (!display->network || !(sptr=display->network->owner)) sptr=globstring[STR_NETWORK_NO_OWNER];
              w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
              if (w > (config->displaylength[win][DISPLAY_OWNER]-scrdata_font_xsize))
               {
                b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_OWNER]-scrdata_font_xsize,0x7FFF);
                w=te.te_Width;
               }
              for (;*sptr && b; b--) *sbuf++=*sptr++;
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_OWNER]-w);
            break;
          case DISPLAY_GROUP:
            if (AccountsBase || muBase) {
              if (!display->network || !(sptr=display->network->group)) sptr=globstring[STR_NETWORK_NO_GROUP];
              w=TextLength(&dir_rp[win],sptr,b=strlen(sptr));
              if (w > (config->displaylength[win][DISPLAY_GROUP]-scrdata_font_xsize))
               {
                b=TextFit(&dir_rp[win],sptr,b,&te,NULL,1,config->displaylength[win][DISPLAY_GROUP]-scrdata_font_xsize,0x7FFF);
                w=te.te_Width;
               }
              for (;*sptr && b; b--) *sbuf++=*sptr++;
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_GROUP]-w);
            break;
          case DISPLAY_NETPROT:
            if ((AccountsBase || muBase) && display->network) {
              sptr=display->network->net_protbuf;
              w=TextLength(&dir_rp[win],sptr,b=9/*strlen(sptr)*/);
              for (;*sptr && b; b--) *sbuf++=*sptr++;
            }
            PUTCODE(&sbuf,TEXT_MOVE,config->displaylength[win][DISPLAY_NETPROT]-w);
            break;
         }
      }
      break;
  }
  else PUTCODE(&sbuf,TEXT_PENS,config->filesbg<<8|config->filesbg);
  *sbuf++=0;*sbuf++=TEXT_END;
//D(KDump(dbg,64);)
 }
/*
void setdispcol(display,win)
struct Directory *display;
int win;
{
        int fg,bg;

        if (display && !status_iconified) {
                switch (ENTRYTYPE(display->type)) {
                        case ENTRY_FILE:
                                if (display->selected) {
                                        fg=config->filesselfg;
                                        bg=config->filesselbg;
                                }
                                else {
                                        fg=config->filesfg;
                                        bg=config->filesbg;
                                }
                                break;
                        case ENTRY_DEVICE:
                                if (display->size==DLT_DEVICE || display->size==DLT_VOLUME) {
                                        if (display->selected) {
                                                fg=config->filesselfg;
                                                bg=config->filesselbg;
                                        }
                                        else {
                                                fg=config->filesfg;
                                                bg=config->filesbg;
                                        }
                                        break;
                                }
                        case ENTRY_DIRECTORY:
                                if (display->selected) {
                                        fg=config->dirsselfg;
                                        bg=config->dirsselbg;
                                }
                                else {
                                        fg=config->dirsfg;
                                        bg=config->dirsbg;
                                }
                                break;
                        case ENTRY_CUSTOM:
                                if (display->subtype==CUSTOMENTRY_USER) {
                                        if (display->selected) {
                                                fg=config->filesselfg;
                                                bg=config->filesselbg;
                                        }
                                        else {
                                                if ((((int)display->size)&0xff)==0xff) fg=config->filesfg;
                                                else fg=((int)display->size)&0xff;
                                                if ((((int)display->size)&0xff00)==0xff00) bg=config->filesbg;
                                                else bg=(((int)display->size)&0xff00)>>8;
                                        }
                                }
                                else {
                                        if (display->selected) {
                                                fg=config->dirsselfg;
                                                bg=config->dirsselbg;
                                        }
                                        else {
                                                fg=config->dirsfg;
                                                bg=config->dirsbg;
                                        }
                                }
                                break;
                }
        }
        else {
                fg=bg=config->filesbg;
        }
        SetAPen(&dir_rp[win],screen_pens[fg].pen);
        SetBPen(&dir_rp[win],screen_pens[bg].pen);
}
*/
static const UWORD testdays[] = {7695,7726,7754,7785,7815,7846,7876,7907,7938,7968,7999,8029};

void getprotdatelengths(struct RastPort *rp)
 {
  struct DateTime dt;
  char day[LEN_DATSTRING],date[LEN_DATSTRING];
  int a,b,l;

  config->displaylength[0][DISPLAY_DATE] = 0;
  DateStamp(&dt.dat_Stamp);
  dt.dat_Format=dateformat(config->dateformat);
  dt.dat_Flags = 0;
  if (config->dateformat&DATE_12HOUR) dt.dat_Flags |= DDTF_12HOUR;
  if (config->dateformat&DATE_SUBST)  dt.dat_Flags |= DDTF_SUBST;
  if (dt.dat_Flags&DDTF_SUBST)
   {
    dt.dat_Stamp.ds_Days--;
    dt.dat_StrDay=day;
    dt.dat_StrDate=date;
    dt.dat_StrTime=NULL;
    for(a=0;a<7;a++)
     {
      DateToStr(&dt);

      for(l=0; day[l] && (day[l]!=' '); l++);
      b = TextLength(rp,day,l);
D(bug("WeekDay: %s (%ld px)\n",day,b));
      if (b > config->displaylength[0][DISPLAY_DATE]) config->displaylength[0][DISPLAY_DATE] = b;

      if (a < 4)
       {
        for(l=0; date[l] && (date[l]!=' '); l++);
        b = TextLength(rp,date,l);
D(bug("Date: %s (%ld px)\n",date,b));
        if (b > config->displaylength[0][DISPLAY_DATE]) config->displaylength[0][DISPLAY_DATE] = b;
       }
      dt.dat_Stamp.ds_Days++;
     }
D(bug("max = %ld\n",config->displaylength[0][DISPLAY_DATE]));
   }
  dt.dat_StrDay=NULL;
  dt.dat_StrDate=date;
  dt.dat_StrTime=NULL;
  for(a=0;a<12;a++)
   {
    dt.dat_Stamp.ds_Days = testdays[a];
    DateToStr(&dt);
    for(l=0; date[l] && (date[l]!=' '); l++);
    b = TextLength(rp,date,l);
D(bug("Date: %s (%ld px)\n",date,b));
    if (b > config->displaylength[0][DISPLAY_DATE]) config->displaylength[0][DISPLAY_DATE] = b;
   }
D(bug("max = %ld\n",config->displaylength[0][DISPLAY_DATE]));
  dt.dat_StrDay=NULL;
  dt.dat_StrDate=NULL;
  dt.dat_StrTime=date;
  dt.dat_Stamp.ds_Minute = 488;
  dt.dat_Stamp.ds_Tick = 400;
  StampToStr((struct DOpusDateTime *)&dt);
  for(l=0; date[l] /*&& (date[l]!=' ')*/; l++);
  b = TextLength(rp,date,l);
D(bug("Time: %s (%ld px)\n",date,b));
  config->displaylength[0][DISPLAY_DATE] += b+scrdata_font_xsize;
D(bug("sum = %ld\n",config->displaylength[0][DISPLAY_DATE]));

  config->displaylength[0][DISPLAY_PROTECT] = TextLength(rp,str_protbits,8);
D(bug("Protect: %ld px\n",config->displaylength[0][DISPLAY_PROTECT]));
  a = TextLength(rp,"RWED RWED",9);
  b = TextLength(rp,"---- ----",9);
  config->displaylength[0][DISPLAY_NETPROT] = (a>b)?a:b;
D(bug("NetProtect: %ld px\n",config->displaylength[0][DISPLAY_NETPROT]));

  config->displaylength[1][DISPLAY_DATE]    = config->displaylength[0][DISPLAY_DATE];
  config->displaylength[1][DISPLAY_PROTECT] = config->displaylength[0][DISPLAY_PROTECT];
  config->displaylength[1][DISPLAY_NETPROT] = config->displaylength[0][DISPLAY_NETPROT];

  setsizedisplen(rp);
 }

void setsizedisplen(struct RastPort *rp)
{
  char a,b;

  a = TextLength(rp,"9999M",5)+scrdata_font_xsize;
  b = TextLength(rp,"9999999",7)+scrdata_font_xsize;

  config->displaylength[0][DISPLAY_SIZE] = (config->listerdisplayflags[0] & SIZE_KMG) ? a : b;
  config->displaylength[1][DISPLAY_SIZE] = (config->listerdisplayflags[1] & SIZE_KMG) ? a : b;

D(bug("SizeL: %ld px\n",config->displaylength[0][DISPLAY_SIZE]));
D(bug("SizeR: %ld px\n",config->displaylength[1][DISPLAY_SIZE]));
}
