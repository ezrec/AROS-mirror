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

/* Data for DOpus functions, including the actual internal command list and
   ARexx data */

#include "dopus.h"

/* List of Internal commands */

struct CommandList commandlist[]={
    {"About",FUNC_ABOUT,0},
    {"AddCustEntry",FUNC_ADDCUSTENTRY,RCL_NOBUTTON},
    {"AddCustHandler",FUNC_ADDCUSTHANDLER,RCL_NOBUTTON},
    {"AddFile",FUNC_ADDFILE,RCL_NOBUTTON},
    {"AddIcon",FUNC_ADDICON,0},
    {"Alarm",FUNC_ALARM,0},
    {"All",FUNC_ALL,RCL_NOBUTTON},
    {"AnsiRead",FUNC_ANSIREAD,0},
    {"ARexx",FUNC_AREXX,0},
    {"Assign",FUNC_ASSIGN,RCL_NOBUTTON},
    {"Auto",FUNC_AUTO,RCL_SYNONYM},
    {"Auto2",FUNC_AUTO2,RCL_SYNONYM},
    {"Beep",FUNC_BEEP,0},
    {"BufferList",FUNC_BUFFERLIST,RCL_NOBUTTON},
    {"Busy",FUNC_BUSY,RCL_NOBUTTON},
    {"ButtonIconify",FUNC_ICONIFYBUTTONS,RCL_NOBUTTON},
    {"Byte",FUNC_BYTE,RCL_NOBUTTON|RCL_SYNONYM},
    {"CD",FUNC_SETCURDIR,0},
    {"CheckAbort",FUNC_CHECKABORT,0},
    {"CheckFit",FUNC_CHECKFIT,RCL_NOBUTTON},
    {"ClearBuffers",FUNC_CLEARBUFFERS,RCL_NOBUTTON},
    {"ClearSizes",FUNC_UNBYTE,RCL_NOBUTTON},
    {"ClearWin",FUNC_CLEARWINDOW,RCL_NOBUTTON},
    {"Clone",FUNC_CLONE,RCL_NEEDDEST},
    {"Comment",FUNC_COMMENT,0},
    {"Configure",FUNC_CONFIGURE,0},
    {"ContST",FUNC_CONTST,0},
    {"Copy",FUNC_COPY,RCL_NEEDDEST},
    {"CopyAs",FUNC_COPYAS,RCL_NEEDDEST},
    {"CopyWindow",FUNC_COPYWINDOW,RCL_NOBUTTON},
    {"DateStamp",FUNC_DATESTAMP,0},
    {"Defaults",FUNC_DEFAULTCONFIG,RCL_NOBUTTON},
    {"Delete",FUNC_DELETE,0},
    {"DirTree",FUNC_DIRTREE,RCL_NOBUTTON},
    {"Diskcopy",FUNC_DISKCOPY,0},
    {"DiskcopyBG",FUNC_BGDISKCOPY,0},
    {"DiskInfo",FUNC_DISKINFO,RCL_NOFILE|RCL_NOBUTTON},
    {"DisplayDir",FUNC_DISPLAYDIR,RCL_NOBUTTON},
    {"DopusToBack",FUNC_DOPUSTOBACK,RCL_NOBUTTON},
    {"DopusToFront",FUNC_DOPUSTOFRONT,RCL_NOBUTTON},
    {"Encrypt",FUNC_ENCRYPT,RCL_NEEDDEST},
    {"EndFunction",FUNC_ENDFUNCTION,RCL_SYNONYM},
    {"ErrorHelp",FUNC_ERRORHELP,0},
    {"Execute",FUNC_RUN,0},
    {"FileInfo",FUNC_FILEINFO,RCL_NOBUTTON},
    {"FinishSection",FUNC_ENDFUNCTION,0},
    {"Format",FUNC_FORMAT,0},
    {"FormatBG",FUNC_BGFORMAT,0},
    {"GetAll",FUNC_GETALL,RCL_NOBUTTON},
    {"GetDevices",FUNC_DEVICELIST,RCL_NOBUTTON},
    {"GetDirs",FUNC_GETDIRS,RCL_NOBUTTON},
    {"GetEntry",FUNC_GETENTRY,RCL_NOBUTTON},
    {"GetFiletype",FUNC_GETFILETYPE,RCL_NOBUTTON},
    {"GetFiles",FUNC_GETFILES,RCL_NOBUTTON},
    {"GetNextSelected",FUNC_GETNEXTSELECTED,RCL_NOBUTTON},
    {"GetSelectedAll",FUNC_GETSELECTEDALL,RCL_NOBUTTON},
    {"GetSelectedDirs",FUNC_GETSELECTEDDIRS,RCL_NOBUTTON},
    {"GetSelectedFiles",FUNC_GETSELECTEDFILES,RCL_NOBUTTON},
    {"GetSizes",FUNC_BYTE,RCL_NOBUTTON},
    {"GetString",FUNC_GETSTRING,0},
    {"Help",FUNC_HELP,0},
    {"HexRead",FUNC_HEXREAD,0},
    {"Hunt",FUNC_HUNT,RCL_NOFILE},
    {"Iconify",FUNC_ICONIFY,RCL_NOBUTTON},
    {"IconInfo",FUNC_ICONINFO,0},
    {"Install",FUNC_INSTALL,0},
    {"InstallBG",FUNC_BGINSTALL,0},
    {"LastSaved",FUNC_LASTSAVEDCONFIG,RCL_NOBUTTON},
    {"LoadConfig",FUNC_LOADCONFIG,RCL_NOBUTTON},
    {"LoadStrings",FUNC_LOADSTRINGFILE,RCL_NOBUTTON},
    {"LoopPlay",FUNC_LOOPPLAY,0},
    {"LPlay",FUNC_LOOPPLAY,RCL_SYNONYM},
    {"MakeDir",FUNC_MAKEDIR,0},
    {"MakeLink",FUNC_MAKELINK,RCL_NEEDDEST},
    {"Modify",FUNC_MODIFY,0},
    {"Move",FUNC_MOVE,RCL_NEEDDEST},
    {"MoveAs",FUNC_MOVEAS,RCL_NEEDDEST},
    {"NewCLI",FUNC_NEWCLI,0},
    {"NewShell",FUNC_NEWCLI,0},
    {"NextDrives",FUNC_NEXTDRIVES,RCL_NOBUTTON},
    {"NNCopy",FUNC_COPYAS,RCL_NEEDDEST|RCL_SYNONYM},
    {"NNMove",FUNC_MOVEAS,RCL_NEEDDEST|RCL_SYNONYM},
    {"Notify",FUNC_NOTIFY,0},
    {"None",FUNC_NONE,RCL_NOBUTTON},
    {"OtherWindow",FUNC_OTHERWINDOW,RCL_NOBUTTON},
    {"Parent",FUNC_PARENT,RCL_NOBUTTON},
    {"ParentList",FUNC_PARENTLIST,RCL_NOBUTTON},
    {"PatternMatch",FUNC_PATTERNMATCH,0},
    {"Play",FUNC_PLAY,0},
    {"PlayST",FUNC_PLAYST,0},
    {"Print",FUNC_PRINT,0},
    {"PrintDir",FUNC_PRINTDIR,RCL_NOBUTTON},
    {"Protect",FUNC_PROTECT,0},
    {"Query",FUNC_QUERY,0},
    {"Quit",FUNC_QUIT,0},
    {"Read",FUNC_READ,0},
    {"Redraw",FUNC_REDRAW,RCL_NOBUTTON},
    {"Relabel",FUNC_RELABEL,RCL_NOFILE},
    {"Remember",FUNC_REMEMBER,0},
    {"RemoveEntry",FUNC_REMOVEENTRY,RCL_NOBUTTON},
    {"RemoveFile",FUNC_REMOVEFILE,RCL_NOBUTTON},
    {"Rename",FUNC_RENAME,0},
    {"Request",FUNC_REQUEST,0},
    {"Rescan",FUNC_RESCAN,RCL_NOBUTTON},
    {"Reselect",FUNC_RESELECT,RCL_NOBUTTON},
    {"Restore",FUNC_RESTORE,0},
    {"Root",FUNC_ROOT,RCL_NOBUTTON},
    {"Run",FUNC_RUN,0},
    {"SaveConfig",FUNC_SAVECONFIG,0},
    {"ScanDir",FUNC_SCANDIR,RCL_NOBUTTON},
    {"ScrollH",FUNC_SCROLLH,RCL_NOBUTTON},
    {"ScrollToShow",FUNC_SCROLLTOSHOW,RCL_NOBUTTON},
    {"ScrollV",FUNC_SCROLLV,RCL_NOBUTTON},
    {"Search",FUNC_SEARCH,0},
    {"Select",FUNC_SELECT,RCL_NOBUTTON},
    {"SelectEntry",FUNC_SELECTENTRY,RCL_NOBUTTON},
    {"SelectFile",FUNC_SELECTFILE,RCL_NOBUTTON},
    {"SetVar",FUNC_SETVAR,0},
    {"SetWinTitle",FUNC_SETWINTITLE,0},
    {"Show",FUNC_SHOW,0},
    {"SmartRead",FUNC_SMARTREAD,0},
    {"Status",FUNC_STATUS,0},
    {"StopST",FUNC_STOPST,0},
    {"SwapWindow",FUNC_SWAPWINDOW,RCL_NOBUTTON},
    {"Toggle",FUNC_TOGGLE,RCL_NOBUTTON},
    {"TopText",FUNC_TOPTEXT,0},
    {"UnByte",FUNC_UNBYTE,RCL_NOBUTTON|RCL_SYNONYM},
    {"UnIconify",FUNC_UNICONIFY,0},
    {"User1",FUNC_AUTO,0},
    {"User2",FUNC_AUTO2,0},
    {"User3",FUNC_AUTO3,0},
    {"User4",FUNC_AUTO4,0},
    {"Verify",FUNC_VERIFY,0},
    {"Version",FUNC_VERSION,0},
    {NULL,0}};

/* ARexx and Function data */

ULONG rexx_signalbit;                     /* Signal bit for ARexx port */
WORD rexx_argcount;                      /* ARexx argument count */
char  *rexx_args[16];                     /* ARexx arguments */
LONG  rexx_arg_value[16];                 /* Argument values */
UBYTE rexx_global_flag=0;                 /* Signifies an ARexx command */
char  *rexx_pathbuffer[2];                /* Path buffer of an ARexx argument */
LONG  rexx_result_code;                   /* Return code for ARexx functions */
LONG  rexx_return_value;                  /* RC for ARexx functions */

ULONG func_global_function=0;             /* Global function number */
char  func_single_file[FILEBUF_SIZE]={0}; /* Single file to operate on */
char  func_external_file[256]={0};        /* External file to operate on */
struct Directory *func_single_entry=NULL; /* Single entry to operate on */
UBYTE func_entry_deleted=0;               /* Indicates entry was deleted */

struct DirWindowPars func_reselection;    /* Reselection data */

/* Parameters recognised by the Modify command */

char *modifynames[]={
    "CopyFlags",                            /* copyflags */
    "DeleteFlags",                          /* deleteflags */
    "ErrorFlags",                           /* errorflags */
    "ListFormat",                           /* displaypos */
    "GeneralFlags",                         /* generalflags */
    "IconFlags",                            /* iconflags */
    "ReplaceFlags",                         /* existflags */
    "SortFlags",                            /* sortflags */
    "UpdateFlags",                          /* dynamicflags */
    "OutputCmd",                            /* outputcmd */
    "OutputWindow",                         /* output */
    "ScrDepth",                             /* scrdepth */
    "ScreenFlags",                          /* screenflags */
    "ScreenMode",                           /* screenmode */
    "ScrW",                                 /* scrw */
    "ScrH",                                 /* scrh */
    "DirFlags",                             /* dirflags */
    "Helpfile",                             /* helpfile */
    "DefaultTool",                          /* defaulttool */
    "ShowDelay",                            /* showdelay */
    "ViewPlayFlags",                        /* viewbits */
    "FadeDelay",                            /* fadetime */
    "ShowPatBits",                          /* hiddenbit */
    "ShowPattern",                          /* showpat */
    "HidePattern",                          /* hidepat */
    "IconifyFlags",                         /* icontype */
    "ScrClockFlags",                        /* scrclktype */
    "ShowFreeFlags",                        /* showfree */
    "Font",                                 /* fontsizes/fontbufs */
    "BankNumber",                           /* current gadget bank */
    "DateFormat",                           /* dateformat */
    "ButtonRows",                           /* gadgetrows */
    "DisplayLength",                        /* displaylength */
    "WindowSize",                           /* windowdelta */
    "SortMethod",                           /* sortmethod */
    "SeparateMethod",                       /* separatemethod */
    "Filter",                               /* file filter */
    "WindowXY",                             /* wbwinx, wbwiny */
    "WindowWH",                             /* scr_winw, scr_winh */
    "PubScreen",                            /* pubscreen_name */
    "WindowXYWH",                           /* wbwinx, wbwiny, scr_winw, scr_winh */
    "ScreenName",                           /* Screen we are open on */
    "PortName",                             /* Rexx port name */
    ""};

/* Data for Remember/Restore function */

struct RememberData *remember_data;       /* Storage for remembered data */

/* Date for Search function */

int search_flags=SEARCH_NOCASE;           /* Search flags */
char *search_found_position;              /* Position of match */
int search_found_size;                    /* Bytes searched */
int search_found_lines;                   /* Lines searched */
int search_last_line_pos;                 /* Last line position searched */

/* Data for Show Info routine */

struct TextFont *show_global_font=NULL;   /* Font info */
struct DiskObject *show_global_icon=NULL; /* Icon info */
char *show_global_icon_name;              /* Icon name */
