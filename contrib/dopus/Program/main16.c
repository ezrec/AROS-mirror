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
#include <libraries/amigaguide.h>
//#include <proto/powerpacker.h>

int showfont(name,size,np)
char *name;
int size,np;
{
    int base,y,fred,t,len;
    unsigned char a;
    struct TextFont *font;
    char fontbuf[256],*fontptr;
    static UWORD fcols[2]={0xfff,0};
    struct TextAttr sfattr = { (STRPTR)name, size, 0, 0 };
    struct RastPort *font_rp;

    font = DiskfontBase ? OpenDiskFont(&sfattr) : OpenFont(&sfattr);
    if (!font || !(setupfontdisplay(1,NULL))) {
        doerror(-1);
        return(0);
    }

    base=font->tf_Baseline;
    SetFont(font_rp=fontwindow->RPort,font);
    SetAPen(font_rp,1);
    SetDrMd(font_rp,JAM1);

    y=base;
    a=font->tf_LoChar;
    len=t=0;

    Move(font_rp,0,y);
    fontptr=fontbuf;

    FOREVER {
        len+=TextLength(font_rp,(char *)&a,1);
        if (len>fontscreen->Width || t>254) {
            Text(font_rp,fontbuf,t);
            y+=size;
            Move(font_rp,0,y);
            len=t=0; fontptr=fontbuf;
            if (y-base>fontscreen->Height) break;
        }
        *(fontptr++)=a; ++t;
        if ((++a)>font->tf_HiChar) a=font->tf_LoChar;
    }

    ScreenToFront(fontscreen);
    ActivateWindow(fontwindow);
    FadeRGB4(fontscreen,fcols,2,1,config->fadetime);
    show_global_font=font;

    fred=WaitForMouseClick(0,fontwindow);

    show_global_font=NULL;
    if (fred!=-3) FadeRGB4(fontscreen,fcols,2,-1,config->fadetime);

    cleanup_fontdisplay();
    CloseFont(font);

    return((fred==0)?1:-1);
}
    
int setupfontdisplay(depth,coltab)
int depth;
UWORD *coltab;
{
    struct DimensionInfo dims;
    //struct RastPort *font_rp;

    font_scr.Width=GfxBase->NormalDisplayColumns;
    font_scr.Height=STDSCREENHEIGHT;
    font_scr.Depth=depth;
//    if (system_version2 >= OSVER_39)
     {
      font_scr.Extension[0].ti_Data = BestModeID(BIDTAG_DesiredWidth, font_scr.Width,
                                                 BIDTAG_DesiredHeight, font_scr.Height,
                                                 BIDTAG_Depth, font_scr.Depth,
                                                 BIDTAG_MonitorID, GetVPModeID(&Window->WScreen->ViewPort) & MONITOR_ID_MASK,
                                                 TAG_END);
D(bug("Font screen ModeID: %lx\n",font_scr.Extension[0].ti_Data));
      if (font_scr.Extension[0].ti_Data == INVALID_ID) font_scr.Extension[0].ti_Data = HIRES_KEY;
      if (GetDisplayInfoData(NULL,(UBYTE *)&dims,sizeof(struct DimensionInfo),DTAG_DIMS,font_scr.Extension[0].ti_Data))
       {
        font_scr.Width = dims.Nominal.MaxX-dims.Nominal.MinX+1;
        font_scr.Height = dims.Nominal.MaxY-dims.Nominal.MinY+1;
       }
     }
    if (!(fontscreen=OpenScreen((struct NewScreen *)&font_scr))) return(0);
    font_win.Width=fontscreen->Width;
    font_win.Height=fontscreen->Height;
    font_win.Screen=fontscreen;
    if (!(fontwindow=OpenWindow(&font_win))) {
        CloseScreen(fontscreen);
        return(0);
    }
    //font_rp=fontwindow->RPort;
    if (coltab) {
        int a,num;

        num=1<<depth;
//        if (num>16) num=16;
        for (a=0;a<num;a++) coltab[a]=GetRGB4(fontscreen->ViewPort.ColorMap,a);
    }
    LoadRGB4(&fontscreen->ViewPort,nullpalette,1<<depth);
    setnullpointer(fontwindow);
    return(1);
}

void cleanup_fontdisplay()
{
    ScreenToFront(blankscreen ? blankscreen : Window->WScreen);
    if (fontwindow) {
        CloseWindow(fontwindow);
        fontwindow=NULL;
    }
    if (fontscreen) {
        CloseScreen(fontscreen);
        fontscreen=NULL;
    }
    ActivateWindow(Window);
}

void readhelp(file)
char *file;
{
//D(bug("readhelp(%s)\n",file);Delay(50));
    if (file) doreadhelp(file);
    if (dopus_firsthelp) screen_gadgets[SCRGAD_TINYHELP].Flags&=~GFLG_DISABLED;
    else screen_gadgets[SCRGAD_TINYHELP].Flags|=GFLG_DISABLED;
    if (Window && config->generalscreenflags&SCR_GENERAL_TINYGADS)
        dolittlegads(&screen_gadgets[SCRGAD_TINYHELP],&globstring[STR_TINY_BUTTONS][4],1);
}

void doreadhelp(file)
char *file;
{
    int a,b,helpsize,inmsg;
    char *helpbuf,*buf;
    struct Help *temph,*curhelp;

//D(bug("doreadhelp(%s)\n",file);Delay(50));
    freehelp();
    if (!file[0] || !(buf=AllocMem(10000,MEMF_CLEAR))) return;
    if (readfile(file,&helpbuf,&helpsize)) {
        FreeMem(buf,10000);
        return;
    }

    curhelp=NULL;
    inmsg=0;
    for (a=0;a<helpsize;a++) {
        if (helpbuf[a]=='*' && !inmsg) {
            inmsg=1; b=0;
            continue;
        }
        else {
            if (inmsg==1) {
                if (b==9999 || helpbuf[a]=='\n') {
                    buf[b]=0;
                    if (!(temph=LAllocRemember(&help_key,sizeof(struct Help),MEMF_CLEAR))) break;
                    if (!(temph->lookup=LAllocRemember(&help_key,b+1,MEMF_CLEAR))) break;
                    CopyMem(buf,temph->lookup,b);
                    if (curhelp) curhelp->next=temph;
                    else dopus_firsthelp=temph;
                    curhelp=temph;
                    inmsg=2; b=0;
                    continue;
                }                   
                buf[b]=helpbuf[a];
                ++b;
                continue;
            }
            else if (inmsg==2) {
                if (b==9999 || helpbuf[a]=='^') {
                    if (curhelp) {
                        buf[b]=0;
                        if (!(curhelp->message=LAllocRemember(&help_key,b+1,MEMF_CLEAR))) break;
                        CopyMem(buf,curhelp->message,b);
                    }
                    inmsg=0; b=0;
                    continue;
                }
                buf[b]=helpbuf[a];
                ++b;
                continue;
            }
        }
    }
    FreeMem(buf,10000);
    FreeVec(helpbuf);
}

static STRPTR helpcontext[] = {
    "Main",
    "About",
    "AddCustEntry",
    "AddCustHandler",
    "AddFile",
    "AddIcon",
    "Alarm",
    "All",
    "AnsiRead",
    "ARexx",
    "Assign",
    "Beep",
    "BufferList",
    "Busy",
    "ButtonIconify",
    "CD",
    "CheckAbort",
    "CheckFit",
    "ClearBuffers",
    "ClearSizes",
    "ClearWin",
    "Clone",
    "Comment",
    "Configure",
    "ContST",
    "Copy",
    "CopyAs",
    "CopyWindow",
    "DateStamp",
    "Defaults",
    "Delete",
    "DirTree",
    "DiskCopy",
    "DiskCopyBG",
    "DiskInfo",
    "DisplayDir",
    "DOpusToBack",
    "DOpusToFront",
    "Encrypt",
    "ErrorHelp",
    "Execute",
    "FileInfo",
    "FinishSection",
    "Format",
    "FormatBG",
    "GetAll",
    "GetDevices",
    "GetDirs",
    "GetEntry",
    "GetFileType",
    "GetFiles",
    "GetNextSelected",
    "GetSelectedAll",
    "GetSelectedDirs",
    "GetSelectedFiles",
    "GetSizes",
    "GetString",
    "HexRead",
    "Hunt",
    "Iconify",
    "IconInfo",
    "Install",
    "InstallBG",
    "LastSaved",
    "LoadConfig",
    "LoopPlay",
    "MakeDir",
    "Modify",
    "Move",
    "MoveAs",
    "NewCLI",
    "NextDrives",
    "None",
    "notify",
    "OtherWindow",
    "Parent",
    "ParentList",
    "PatternMatch",
    "Play",
    "PlayST",
    "Print",
    "PrintDir",
    "Protect",
    "Query",
    "Quit",
    "Read",
    "Redraw",
    "Relabel",
    "Remember",
    "RemoveEntry",
    "RemoveFile",
    "Rename",
    "Request",
    "Rescan",
    "Reselect",
    "Restore",
    "Root",
    "Run",
    "SaveConfig",
    "ScanDir",
    "ScrollH",
    "ScrollToShow",
    "ScrollV",
    "Search",
    "Select",
    "SelectEntry",
    "SelectFile",
    "SetVar",
    "SetWinTitle",
    "Show",
    "SmartRead",
    "Status",
    "StopST",
    "SwapWindow",
    "Toggle",
    "TopText",
    "Uniconify",
    "User1",
    "Verify",
    "Version",
     NULL };

char *getAGnode(char *buf,char *func)
 {
  int i;

  for (i = 0; func[i] && func[i] != ' '; i++) buf[i] = func[i];
  buf[i] = 0;

  for(i = 0; helpcontext[i]; i++) if (stricmp(buf,helpcontext[i])==0) break;

  if (helpcontext[i] == NULL) strcpy(buf,"Main");

  return buf;
 }

void dohelp(name,func,key,qual,defmsg)
char *name,*func;
int key,qual;
char *defmsg;
{
    if (dopus_firsthelp)
     {
      char *msg=NULL,*buf=NULL,buf1[40],buf2[30];
      struct Help *help=dopus_firsthelp;
      int s,s1;

      while (help) {
          if (help->lookup) {
              if (help->lookup[0]=='*' && func) {
                  if (Stricmp(help->lookup,func)==0) {
                      msg=help->message;
                      break;
                  }
              }
              else if (name) {
                  if (Stricmp(help->lookup,name)==0) {
                      msg=help->message;
                      break;
                  }
              }
          }
          help=help->next;
      }
      if (!msg) msg = (defmsg ? defmsg : globstring[STR_HELP_NOT_AVAILABLE]);

      if (key>0) {
          RawkeyToStr(key,qual,buf2,NULL,30);
          strcpy(buf1,globstring[STR_KEY]); strcat(buf1,buf2); strcat(buf1,"\n\n");
          s1=strlen(buf1);
          s=strlen(msg)+1;
          if ((buf=AllocMem(s+s1,MEMF_CLEAR))) {
              CopyMem(buf1,buf,s1);
              CopyMem(msg,&buf[s1],s);
              s+=s1;
              msg=buf;
          }
      }
      simplerequest(msg,globstring[STR_CONTINUE],NULL);
      if (buf) FreeMem(buf,s);
     }
    else if (AmigaGuideBase && func)
     {
      struct NewAmigaGuide nag = {0};
      AMIGAGUIDECONTEXT agc;
      char buf[32];

      nag.nag_Name = "DirectoryOpus.guide";
      nag.nag_Screen = Window->WScreen;
      nag.nag_Flags = 0 /*HTF_NOACTIVATE*/;
      nag.nag_Context = helpcontext;
      nag.nag_Node = getAGnode(buf,func[0]=='*'?func+1:func);

D(bug("Trying to display Amigaguide node \"%s\"\n",nag.nag_Node));
      if ((agc = OpenAmigaGuide(&nag,NULL)))
       {
        CloseAmigaGuide(agc);
       }
      else simplerequest(defmsg ? defmsg : globstring[STR_HELP_NOT_AVAILABLE],globstring[STR_CONTINUE],NULL);
     }
    unbusy();
    return;
}

void checkstringgads(a)
int a;
{
    --a;
    checkdir(str_pathbuffer[a],&path_strgadget[a]);
    strcpy(dopus_curwin[a]->directory,str_pathbuffer[a]);
}

void setdirsize(dir,byte,win)
struct Directory *dir;
long long byte;
int win;
{
    if (status_iconified || win==-1) return;

    if (dir->size>0) {
        dopus_curwin[win]->bytestot-=dir->size;
        if (dir->selected) dopus_curwin[win]->bytessel-=dir->size;
    }
    if (byte<-1) byte=-1;
    dir->size=byte;
    if (dir->size!=-1) {
        dopus_curwin[win]->bytestot+=byte;
        if (dir->selected) dopus_curwin[win]->bytessel+=byte;
    }
}
