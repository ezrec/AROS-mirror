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

/* Defines relating to functions */

#ifndef DOPUS_FUNCTIONS
#define DOPUS_FUNCTIONS

/* Internal Function Flags */

#define FUNCFLAGS_DIRS 1
#define FUNCFLAGS_DEVS 2
#define FUNCFLAGS_FILES 4
#define FUNCFLAGS_ANYTHING 8

#define FUNCFLAGS_COPYISCLONE      256
#define FUNCFLAGS_BYTEISCHECKFIT   256

/* External Function Flags */

#define FLAG_OUTWIND      1
#define FLAG_OUTFILE      2
#define FLAG_WB2F         4
#define FLAG_DOPUSF       8
#define FLAG_ASYNC       16
#define FLAG_CDSOURCE    32
#define FLAG_CDDEST      64
#define FLAG_DOALL      128
#define FLAG_RECURSE    256
#define FLAG_RELOAD     512
#define FLAG_ICONIFY   1024
#define FLAG_NOQUOTE   2048
#define FLAG_SCANSRCE  4096
#define FLAG_SCANDEST  8192
#define FLAG_SHELLUP  16384
#define FLAG_DOPUSUP  32768

/* Recursive Operation Flags */

#define R_DELETE     1
#define R_COPY       2
#define R_HUNT       4
#define R_COMMENT    8
#define R_PROTECT    16
#define R_DATESTAMP  32
#define R_SEARCH     64
#define R_GETNAMES   128
#define R_GETBYTES   256
#define R_STARDIR    512

/* Command List Flags */

#define RCL_NOBUTTON 1
#define RCL_NEEDDEST 2
#define RCL_NOFILE   4
#define RCL_SYNONYM  8

/* FileType Commands */

#define FTYC_MATCH        1
#define FTYC_MATCHNAME    2
#define FTYC_MATCHBITS    3
#define FTYC_MATCHCOMMENT 4
#define FTYC_MATCHSIZE    5
#define FTYC_MATCHDATE    6
#define FTYC_MOVETO       7
#define FTYC_MOVE         8
#define FTYC_SEARCHFOR    9
#define FTYC_OR         253
#define FTYC_AND        254
#define FTYC_ENDSECTION 255

#define FTYC_ENDLIMIT   252
#define FTYC_COMMANDOK  10

/* FileType Functions */

#define FTFUNC_AUTOFUNC1   0
#define FTFUNC_AUTOFUNC2   1
#define FTFUNC_DOUBLECLICK 2
#define FTFUNC_CLICKMCLICK 3
#define FTFUNC_ANSIREAD    4
#define FTFUNC_AUTOFUNC3   5
#define FTFUNC_HEXREAD     6
#define FTFUNC_LOOPPLAY    7
#define FTFUNC_PLAY        8
#define FTFUNC_AUTOFUNC4   9
#define FTFUNC_READ        10
#define FTFUNC_SHOW        11

/* Curly-Bracket Sequences */

#define FUNC_QUERYINFO   235
#define FUNC_VARIABLE    236
#define FUNC_SCREENNAME  237
#define FUNC_REQUESTER   238
#define FUNC_STDARG      239
#define FUNC_NOSOURCE_RR 240
#define FUNC_NODEST_RR   241
#define FUNC_ENDARG      242
#define FUNC_DEST_RR     243
#define FUNC_SOURCE_RR   244
#define FUNC_ONEFILE_NO  245
#define FUNC_ALLFILES_NO 246
#define FUNC_ONEPATH_NO  247
#define FUNC_ALLPATHS_NO 248
#define FUNC_ONEFILE     249
#define FUNC_ALLFILES    250
#define FUNC_SOURCE      251
#define FUNC_DEST        252
#define FUNC_GETARG      253
#define FUNC_ONEPATH     254
#define FUNC_ALLPATHS    255

/* Custom Function Types */

#define FT_INTERNAL     0
#define FT_EXECUTABLE   1
#define FT_WORKBENCH    2
#define FT_BATCH        3
#define FT_AREXX        4
#define FT_REQUESTER    5
#define FT_CHDIR       99

#define FC_INTERNAL     '*'
#define FC_WORKBENCH    '%'
#define FC_BATCH        '$'
#define FC_AREXX        '&'
#define FC_REQUESTER    '@'
#define FC_CHDIR        '!'

/* List of Internal commands */

enum functionnums {
	FUNC_TESTCMD,

	FUNC_ALL,
	FUNC_NONE,
	FUNC_PARENT,
	FUNC_ROOT,
	FUNC_BYTE,
	FUNC_MAKEDIR,
	FUNC_DELETE,
	FUNC_CLONE,
	FUNC_COPY,
	FUNC_RENAME,
	FUNC_MOVE,
	FUNC_RUN,
	FUNC_ENCRYPT,
	FUNC_PLAY,
	FUNC_SHOW,
	FUNC_READ,
	FUNC_HEXREAD,
	FUNC_removed0,
	FUNC_ADDICON,
	FUNC_ASSIGN,
	FUNC_COMMENT,
	FUNC_DATESTAMP,
	FUNC_PRINT,
	FUNC_PROTECT,
	FUNC_HUNT,
	FUNC_SEARCH,
	FUNC_AREXX,
	FUNC_QUIT,
	FUNC_ICONIFY,
	FUNC_setdateremoved,
	FUNC_ERRORHELP,
	FUNC_HELP,
	FUNC_SETCURDIR,
	FUNC_CONFIGURE,
	FUNC_FORMAT,
	FUNC_DISKCOPY,
	FUNC_INSTALL,
	FUNC_SAVECONFIG,
	FUNC_LASTSAVEDCONFIG,
	FUNC_DEFAULTCONFIG,
	FUNC_RELABEL,
	FUNC_PRINTDIR,
	FUNC_SELECT,
	FUNC_ABOUT,
	FUNC_OBSOLETE1,
	FUNC_UNBYTE,
	FUNC_COPYAS,
	FUNC_LOOPPLAY,
	FUNC_printremoved1,
	FUNC_printremoved2,
	FUNC_CLEARBUFFERS,
	FUNC_NEWCLI,
	FUNC_PARENT1,
	FUNC_PARENT2,
	FUNC_TOGGLE,
	FUNC_AUTO,
	FUNC_AUTO2,
	FUNC_BUFFERLIST,
	FUNC_RESELECT,
	FUNC_DISKINFO,
	FUNC_MOVEAS,
	FUNC_SMARTREAD,
	FUNC_REDRAW,
	FUNC_removed1,
	FUNC_RESCAN,
	FUNC_PLAYST,
	FUNC_STOPST,
	FUNC_CONTST,
	FUNC_LOADCONFIG,
	FUNC_ANSIREAD,
	FUNC_ICONINFO,
	FUNC_SCANDIR,
	FUNC_removedCUSTREQ,
	FUNC_CLEARWINDOW,
	FUNC_printremoved3,
	FUNC_MODIFY,
	FUNC_ENDFUNCTION,
	FUNC_REMEMBER,
	FUNC_RESTORE,
	FUNC_ALARM,
	FUNC_BEEP,
	FUNC_ICONIFYBUTTONS,
	FUNC_VERIFY,
	FUNC_LOADSTRINGFILE,
	FUNC_UNICONIFY,
	FUNC_GETSTRING,
	FUNC_DOPUSTOFRONT,
	FUNC_DOPUSTOBACK,
	FUNC_REQUEST,
	FUNC_OTHERWINDOW,
	FUNC_STATUS,
	FUNC_FILEINFO,
	FUNC_GETALL,
	FUNC_GETFILES,
	FUNC_GETDIRS,
	FUNC_GETSELECTEDALL,
	FUNC_GETSELECTEDFILES,
	FUNC_GETSELECTEDDIRS,
	FUNC_DEVICELIST,
	FUNC_GETNEXTSELECTED,
	FUNC_GETENTRY,
	FUNC_SCROLLV,
	FUNC_SCROLLH,
	FUNC_NEXTDRIVES,
	FUNC_PATTERNMATCH,
	FUNC_BUSY,
	FUNC_SELECTFILE,
	FUNC_ADDFILE,
	FUNC_REMOVEFILE,
	FUNC_SWAPWINDOW,
	FUNC_COPYWINDOW,
	FUNC_TOPTEXT,
	FUNC_DISPLAYDIR,
	FUNC_DIRTREE,
	FUNC_printremoved4,
	FUNC_printremoved5,
	FUNC_CHECKABORT,
	FUNC_ADDCUSTENTRY,
	FUNC_SETWINTITLE,
	FUNC_ADDCUSTHANDLER,
	FUNC_SELECTENTRY,
	FUNC_REMOVEENTRY,
	FUNC_VERSION,
	FUNC_CHECKFIT,
	FUNC_GETFILETYPE,
	FUNC_SCROLLTOSHOW,
	FUNC_AUTO3,
	FUNC_AUTO4,
	FUNC_BGFORMAT,
	FUNC_BGDISKCOPY,
	FUNC_BGINSTALL,
	FUNC_QUERY,
	FUNC_supportremoved,
	FUNC_SETVAR,
	FUNC_NOTIFY,
	FUNC_PARENTLIST
};


struct CommandList {
	char *name;
	ULONG function;
	ULONG flags;
};

/* List of Internal commands */

extern struct CommandList commandlist[];

/* ARexx and Function data */

extern ULONG rexx_signalbit;                     /* Signal bit for ARexx port */
extern SHORT rexx_argcount;                      /* ARexx argument count */
extern char  *rexx_args[16];                     /* ARexx arguments */
extern LONG  rexx_arg_value[16];                 /* Argument values */
extern UBYTE rexx_global_flag;                   /* Signifies an ARexx command */
extern char  *rexx_pathbuffer[2];                /* Path buffer of an ARexx argument */
extern LONG  rexx_result_code;                   /* Return code for ARexx functions */
extern LONG  rexx_return_value;                  /* RC for ARexx functions */

extern ULONG func_global_function;               /* Global function number */
extern char  func_single_file[32];               /* Single file to operate on */
extern char  func_external_file[256];            /* External file to operate on */
extern struct Directory *func_single_entry;      /* Single entry to operate on */
extern UBYTE func_entry_deleted;                 /* Indicates entry was deleted */

extern struct DirWindowPars func_reselection;    /* Reselection data */

/* Parameters recognised by the Modify command */

extern char *modifynames[];

/* Data for Remember/Restore function */

extern struct RememberData *remember_data;       /* Storage for remembered data */

/* Date for Search function */

extern int search_flags;                         /* Search flags */
extern char *search_found_position;              /* Position of match */
extern int search_found_size;                    /* Bytes searched */
extern int search_found_lines;                   /* Lines searched */
extern int search_last_line_pos;                 /* Last line position searched */

/* Data for Show Info routine */

extern struct TextFont *show_global_font;        /* Font info */
extern struct DiskObject *show_global_icon;      /* Icon info */
extern char *show_global_icon_name;              /* Icon name */

/* Directory tree graphics items */

#define DIRTREEGFX_VERT        0                 /* |  */
#define DIRTREEGFX_VERTCROSS   1                 /* |- */
#define DIRTREEGFX_CROSS       2                 /* -  */
#define DIRTREEGFX_BRANCH      3                 /* +  */
#define DIRTREEGFX_FINALCROSS  4                 /*  - */

/* ARexx Status values */

#define RXSTATUS_CURRENT_DIRECTORY          1
#define RXSTATUS_VERSION                    2
#define RXSTATUS_ACTIVE_WINDOW              3
#define RXSTATUS_NUMBER_OF_FILES            4
#define RXSTATUS_NUMBER_OF_DIRS             5
#define RXSTATUS_NUMBER_OF_ENTRIES          6
#define RXSTATUS_NUMBER_SELECTED_FILES      7
#define RXSTATUS_NUMBER_SELECTED_DIRS       8
#define RXSTATUS_NUMBER_SELECTED_ENTRIES    9
#define RXSTATUS_TOTAL_BYTES                10
#define RXSTATUS_TOTAL_SELECTED_BYTES       11
#define RXSTATUS_SELECT_PATTERN             12
#define RXSTATUS_DIRECTORY_NAME             13
#define RXSTATUS_DISK_NAME                  14
#define RXSTATUS_DISK_FREE_BYTES            15
#define RXSTATUS_DISK_TOTAL_BYTES           16
#define RXSTATUS_BUFFERED_DIRECTORY_NAME    17
#define RXSTATUS_BUFFERED_DISKNAME          18
#define RXSTATUS_BUFFERED_DISK_FREE_BYTES   19
#define RXSTATUS_BUFFERED_DISK_TOTAL_BYTES  20
#define RXSTATUS_BUFFER_DISPLAYED           21
#define RXSTATUS_DISPLAY_OFFSET             22
#define RXSTATUS_ENTRIES_PER_PAGE           24
#define RXSTATUS_CONFIGURATION_CHANGED      25
#define RXSTATUS_OKAY_STRING                26
#define RXSTATUS_CANCEL_STRING              27
#define RXSTATUS_ICONIFIED                  28
#define RXSTATUS_TOP_TEXT_JUSTIFY           30
#define RXSTATUS_CONFIGURATION_ADDRESS      31
#define RXSTATUS_FIRST_NODE                 33
#define RXSTATUS_CURRENT_BANK_NUMBER        34

#endif
