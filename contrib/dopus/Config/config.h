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

#ifdef DEBUG
  #define bug kprintf
  #define D(x) x
#else
  #define D(x)
#endif

#if defined(__PPC__) || defined(__AROS__)
  #undef  __saveds
  #define __saveds
  #define __chip
  #define __aligned __attribute__((__aligned__(4)))
  #define lsprintf sprintf     
  #define __asm(A)
  #define __stdargs
  #define __regargs
  #define _exit exit
#endif

//#include <fctype.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/exall.h>
#include <intuition/intuitionbase.h>
#include <intuition/iobsolete.h>
#include <intuition/sghooks.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <diskfont/diskfont.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/console.h>
#include <proto/locale.h>
#include <proto/dopus.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/workbench.h>
#include <clib/alib_protos.h>

#include <proto/dopus.h>
#include <dopus/config.h>
#include <dopus/configflags.h>
#include <dopus/dopusmessage.h>
#include <dopus/stringdata.h>
#undef CONFIG_VERSION

#define NUM_MODULES 4

#define OSVER_34 0
#define OSVER_37 1
#define OSVER_38 2
#define OSVER_39 3
#define OSVER_40 4

#define GAD_BASE 1000
#define STRING_BASE 10000
#define REQ_BASE 11000

#define CONFIG_STRUCTURE_SIZE 9200

enum {
    FONT_GENERAL,
    FONT_DIRS,
    FONT_TEXT,
    FONT_GADGETS,
    FONT_MENUS,
    FONT_STATUS,
    FONT_NAMES,
    FONT_CLOCK,
    FONT_REQUEST,
    FONT_STRING,
    FONT_ICONIFY,
    FONT_SCREEN,

    FONT_COUNT};

#define FFLAG_8ONLY   1
#define FFLAG_NOPROP  2

#define NUMFLAGS 16
#define MAXFUNCS 32
#define HIDDENLINES 6

#define FB_OUTPUTWIN    0
#define FB_OUTPUTFILE   1
#define FB_WB2F         2
#define FB_DOPUS2F      3
#define FB_RUNASYNC     4
#define FB_CDSOURCE     5
#define FB_CDDEST       6
#define FB_DOALLFILES   7
#define FB_RECURSEDIR   8
#define FB_RELOADFILE   9
#define FB_AUTOICONIFY  10
#define FB_NOQUOTE      11
#define FB_RESCANSOURCE 12
#define FB_RESCANDEST   13
#define FB_SHELLSTARTUP 14
#define FB_DOPUSSTARTUP 15

#define FF_OUTPUTWIN    (1<<FB_OUTPUTWIN)
#define FF_OUTPUTFILE   (1<<FB_OUTPUTFILE)
#define FF_WB2F         (1<<FB_WB2F)
#define FF_DOPUS2F      (1<<FB_DOPUS2F)
#define FF_RUNASYNC     (1<<FB_RUNASYNC)   
#define FF_CDSOURCE     (1<<FB_CDSOURCE)
#define FF_CDDEST       (1<<FB_CDDEST)
#define FF_DOALLFILES   (1<<FB_DOALLFILES)
#define FF_RECURSEDIR   (1<<FB_RECURSEDIR)
#define FF_RELOADFILE   (1<<FB_RELOADFILE)
#define FF_AUTOICONIFY  (1<<FB_AUTOICONIFY)
#define FF_NOQUOTE      (1<<FB_NOQUOTE)
#define FF_RESCANSOURCE (1<<FB_RESCANSOURCE)
#define FF_RESCANDEST   (1<<FB_RESCANDEST)
#define FF_SHELLSTARTUP (1<<FB_SHELLSTARTUP)
#define FF_DOPUSSTARTUP (1<<FB_DOPUSSTARTUP)

#define SFB_AUTOICONIFY  0
#define SFB_CDDEST       1
#define SFB_CDSOURCE     2
#define SFB_DOPUS2F      3
#define SFB_DOALLFILES   4
#define SFB_DOPUSSTARTUP 5
#define SFB_SHELLSTARTUP 6
#define SFB_NOQUOTE      7
#define SFB_OUTPUTFILE   8
#define SFB_OUTPUTWIN    9
#define SFB_RECURSEDIR   10
#define SFB_RELOADFILE   11
#define SFB_RESCANDEST   12
#define SFB_RESCANSOURCE 13
#define SFB_RUNASYNC     14
#define SFB_WB2F         15

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

#define MENUCOUNT 100
#define GADCOUNT 84
#define DRIVECOUNT 32
#define NUMFONTS 16
#define USEDRIVECOUNT 30

#define FTYC_MATCH        1
#define FTYC_MATCHNAME    2
#define FTYC_MATCHBITS    3
#define FTYC_MATCHCOMMENT 4
#define FTYC_MATCHSIZE    5
#define FTYC_MATCHDATE    6
#define FTYC_MOVETO       7
#define FTYC_MOVE         8
#define FTYC_SEARCHFOR    9
#define FTYC_MATCHI      10
#define FTYC_OR         253
#define FTYC_AND        254
#define FTYC_ENDSECTION 255

#define FTYC_ENDLIMIT   252
#define FTYC_COMMANDOK   11
#define FTYC_CYCLEEND    12

#define FILETYPE_FUNCNUM   16

struct fileclass {
    struct fileclass *last,*next;
    char type[32];
    char typeid[8];
    unsigned char *recognition;
};

extern struct TagItem scr_taglist[];
extern struct ExtNewScreen configscr;
extern struct NewWindow configwin,requestwin;
extern struct ExecBase *SysBase;
extern struct DOpusBase *DOpusBase;
extern struct GfxBase *GfxBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct muBase *muBase;
extern struct Screen *Screen;
extern struct Window *Window;
extern struct ViewPort *vp;
extern struct RastPort *rp;
extern struct Process *myproc;
extern struct IntuiMessage *IMsg;
extern APTR wsave,realwsave;
extern char *spacestring;
extern char fontbuf[50];
extern struct TextAttr sfont,bsfont;
extern struct Config *config,*undoconfig;
extern struct dopusfiletype *firsttype;
extern struct dopushotkey *firsthotkey;
extern struct dopusgadgetbanks *firstbank,*curbank;
extern struct DOpusRemember *typekey,*fontkey;
extern char configname[256],loadnamebuf[256];
extern char filebuf[256],dirbuf[258];
extern struct DOpusFileReq filereq;
extern int version2;
extern struct MsgPort *conport,*cmdport,*appport;
/*
extern struct MsgPort *clip_port;
extern struct IOClipReq *clip_io;
*/
/*
struct ConfigStuff {
    struct Config *config;
    struct DOpusRemember *typekey;
    struct dopusfiletype *firsttype;
    struct dopusgadgetbanks *firstbank,*curbank;
    struct dopushotkey *firsthotkey;
};
*/
extern struct ConfigStuff cstuff;
extern struct DOpusRemember *mainkey,*gadgetkey,*tickkey,*screenkey,*clipkey,*buttonkey;
extern struct Gadget *maingad,*tickgad,*gadgads,*menugads,*drivegads;
extern int tickcount;
extern struct dopusconfigmsg msg;

extern struct TagItem taglist[2];
extern UWORD drawinfo[];
extern int lchanged,changed;

extern struct Image
    *checkonimage,*checkoffimage,
    *buttononimage,*buttonoffimage,
    copy_checkonimage,copy_checkoffimage;

#define CTYPE_MOVEREL -3
#define CTYPE_MOVE -2
#define CTYPE_SKIP -1
#define CTYPE_TEXT 1
#define CTYPE_GAD  2
#define CTYPE_STR  3
#define CTYPE_REQ  4
#define CTYPE_RAD  5
#define CTYPE_MRAD  6

struct ConfigGadget {
    char type;
    char value;
    int nameentry;
    short x,y;
    short w,h;
    int bit;
    int mutualex;
    char *buffer;
    struct Gadget *gad;
};

#define CFG_MAINMENU  11
#define CFG_GADGET    0
#define CFG_DRIVE     1
#define CFG_FILETYPE  2
#define CFG_HOTKEYS   3
#define CFG_MENU      4
#define CFG_OPERATION 5
#define CFG_SCREEN    6
#define CFG_SYSTEM    7
#define CFG_SAVE      8
#define CFG_OKAY      9
#define CFG_CANCEL    10

#define OP_COPY       0
#define OP_DATEFORMAT 1
#define OP_DELETE     2
#define OP_ERRORCHECK 3
#define OP_GENERAL    4
#define OP_ICONS      5
#define OP_FORMAT     6
#define OP_UPDATE     7
#define OP_OKAY       8
#define OP_CANCEL     9

#define SYS_AMIGADOS    0
#define SYS_CLOCK       1
#define SYS_DIRECTORIES 2
#define SYS_HOTKEYS     3
#define SYS_ICONS       4
#define SYS_MODULES     5
#define SYS_SHOWPATTERN 6
#define SYS_STARTUP     7
#define SYS_VIEWPLAY    8
#define SYS_OKAY        9
#define SYS_CANCEL      10

#define GAD_NEXTBANK     0
#define GAD_NEWBANK      1
#define GAD_COPYBANK     2
#define GAD_SWAPBANK     3
#define GAD_DELETEBANK   4
#define GAD_COPYGADGET   5
#define GAD_SWAPGADGET   6
#define GAD_DELETEGADGET 7
#define GAD_OKAY         8
#define GAD_CANCEL       9

#define MENU_COPYMENU    0
#define MENU_SWAPMENU    1
#define MENU_DELETEMENU  2
#define MENU_SORTMENU    3
#define MENU_INSERTITEM  4
#define MENU_COPYITEM    5
#define MENU_SWAPITEM    6
#define MENU_DELETEITEM  7
#define MENU_OKAY        8
#define MENU_CANCEL      9
#define MENU_MOVEUP     10
#define MENU_MOVEDOWN   11
#define MENU_SLIDER     12

#define DRIVE_GETDRIVES   0
#define DRIVE_COPYBANK    1
#define DRIVE_SWAPBANK    2
#define DRIVE_DELETEBANK  3
#define DRIVE_SORTBANK    4
#define DRIVE_COPYDRIVE   5
#define DRIVE_SWAPDRIVE   6
#define DRIVE_DELETEDRIVE 7
#define DRIVE_OKAY        8
#define DRIVE_CANCEL      9
#define DRIVE_EDITNAME   10
#define DRIVE_EDITSAMPLE 11
#define DRIVE_EDITREQ    12
#define DRIVE_EDITPATH   13

#define FILETYPE_NEW          0
#define FILETYPE_SWAP         1
#define FILETYPE_DELETE       2
#define FILETYPE_OKAY         3
#define FILETYPE_CANCEL       4

#define HOTKEYS_NEWHOTKEY     0
#define HOTKEYS_DELETE        1
#define HOTKEYS_DUPLICATE     2
#define HOTKEYS_OKAY          3
#define HOTKEYS_SWAP          4
#define HOTKEYS_CANCEL        5

#define EDIT_OKAY             0
#define EDIT_CANCEL           1
#define EDIT_EDITCLASS        2
#define EDIT_NAME             3
#define EDIT_SAMPLE           4
#define EDIT_NEWENTRY         5
#define EDIT_DUPLICATE        6
#define EDIT_SWAP             7
#define EDIT_DELETE           8
#define EDIT_STACK            9
#define EDIT_PRIORITY        10
#define EDIT_DELAY           11
#define EDIT_TYPE            12
#define EDIT_FUNC            13
#define EDIT_REQUESTER       14
#define EDIT_PATH            15
#define EDIT_ARGREQ          16
#define EDIT_FILETYPEACTION  17

#define CLASS_OKAY           0
#define CLASS_CANCEL         1
#define CLASS_TYPE           3
#define CLASS_NEWENTRY       4
#define CLASS_DUPLICATE      5
#define CLASS_SWAP           6
#define CLASS_DELETE         7
#define CLASS_OPERATION      8
#define CLASS_FUNC           9
#define CLASS_FILEVIEWREQ    10
#define CLASS_FILEVIEW       11
#define CLASS_HEXDEC         12
#define CLASS_VIEWSLIDER     13
#define CLASS_VIEWUP         14
#define CLASS_VIEWDOWN       15
#define CLASS_TYPEID         16

#define EDIT_FUNCTIONS    1
#define EDIT_FLAGS        2
#define EDIT_FUNCTIONTYPE 3

#define SCREEN_ARROWS     0
#define SCREEN_COLOURS    1
#define SCREEN_FONTS      2
#define SCREEN_GENERAL    3
#define SCREEN_PALETTE    4
#define SCREEN_SCREENMODE 5
#define SCREEN_SLIDERS    6
#define SCREEN_OKAY       7
#define SCREEN_CANCEL     8

#define PALETTE_SLIDER    100
#define PALETTE_STRING    103

#define COLOURS_SELECT    200

#define SCREENMODE_WIDTH      300
#define SCREENMODE_HEIGHT     301
#define SCREENMODE_DEPTH      302
#define SCREENMODE_DEFWIDTH   303
#define SCREENMODE_DEFHEIGHT  304
#define SCREENMODE_HALFHEIGHT 305
#define SCREENMODE_SLIDER     306

#define HOTKEY_SAMPLE     400
#define HOTKEY_MMB        401

#define FORMAT_MAXNUM 9

#define FORMAT_WINDOW      500
#define FORMAT_RESET       501
#define FORMAT_CLEAR       502
#define FORMAT_LENGTH      503
#define FORMAT_SEPARATE    510
#define FORMAT_REVERSESORT 511
#define FORMAT_SIZEKMG     512 // HUX

#define ICON_ICONREQ       600
#define ICON_ICONPATH      601
#define ICON_TOOLREQ       602
#define ICON_TOOLPATH      603

#define ARROWBASE          600
#define ARROWCYCLE         610

#define FONT_SIZE 700

#define SCREEN_SLIDER_LEFTCYCLE 800
#define SCREEN_SLIDER_RIGHTCYCLE 801

#define COLBIT_STATUS        0
#define COLBIT_SELDISKNAME   1
#define COLBIT_DISKNAME      2
#define COLBIT_SELDIRECTORY  3
#define COLBIT_DIRECTORY     4
#define COLBIT_SELFILE       5
#define COLBIT_FILE          6
#define COLBIT_SLIDERS       7
#define COLBIT_ARROWS        8
#define COLBIT_TINYGADS      9
#define COLBIT_CLOCK         10
#define COLBIT_REQUESTER     11
#define COLBIT_3DBOXES       12
#define COLBIT_STRINGS       13
#define COLBIT_SELSTRINGS    14

#define COLOURS_STATUS       (1<<COLBIT_STATUS)
#define COLOURS_SELDISKNAME  (1<<COLBIT_SELDISKNAME)
#define COLOURS_DISKNAME     (1<<COLBIT_DISKNAME)
#define COLOURS_SELDIRECTORY (1<<COLBIT_SELDIRECTORY)
#define COLOURS_DIRECTORY    (1<<COLBIT_DIRECTORY)
#define COLOURS_SELFILE      (1<<COLBIT_SELFILE)
#define COLOURS_FILE         (1<<COLBIT_FILE)
#define COLOURS_SLIDERS      (1<<COLBIT_SLIDERS)
#define COLOURS_ARROWS       (1<<COLBIT_ARROWS)
#define COLOURS_TINYGADS     (1<<COLBIT_TINYGADS)
#define COLOURS_CLOCK        (1<<COLBIT_CLOCK)
#define COLOURS_REQUESTER    (1<<COLBIT_REQUESTER)
#define COLOURS_3DBOXES      (1<<COLBIT_3DBOXES)
#define COLOURS_STRINGS      (1<<COLBIT_STRINGS)
#define COLOURS_SELSTRINGS   (1<<COLBIT_SELSTRINGS)

#define COLOURS_ALL          ~0

struct ColourSel {
    char item;
    short x1,y1,x2,y2;
};

extern struct ColourSel coloursel[];

extern char
    *mainmenugads[14],
    *listviewgads[3],
    *listviewgads2[4],
    *operationgads[14],
    *systemgads[14],
    *gadgetgads[11],
    *functypelist[],
    ftype_funcmap[],
    *functypestr[],
    *editfuncgads[6],
    *editfuncgads2[16],
    *gadflaglist[17],
    *editclassgads[10],
    *menugadgets[11],
    *drivegadgets[11],
    *drivegadgets2[5],
    *screengadgets[14],
    *formatgadgets[11], // JRZ: was 10
    *hotkeygadgets[3],
    *arrowgadtxt[4],
    *arrowtypetxt[3],
    *filetypeactiongadgets[11],

    *commandlist[95],
    *arglist[18],
    *classopslist[], // JRZ: was 12

    *formatnames[FORMAT_MAXNUM+1],
    *selectedformatnames[FORMAT_MAXNUM+1],

    *defdir[],
    *palettegadgets[],*coloursgadgets[],
    *screenmodegadgets[],
    *hotkeysgadgets[],
    *windownames[2],*errorcheckingtxt[],*separatenames[3];

extern struct ConfigGadget
    *operationgadgets[8],*systemgadgets[9],scr_generalgadgets[];


#define UNDO_MAIN      1
#define UNDO_GADGET    2
#define UNDO_MENU      4
#define UNDO_FILETYPE  8
#define UNDO_DRIVE     16
#define UNDO_HOTKEYS   32
#define UNDO_ALL       (UNDO_MAIN|UNDO_GADGET|UNDO_MENU|UNDO_FILETYPE|UNDO_DRIVE|UNDO_HOTKEYS)

struct ConfigUndo {
    struct Config *config;
    struct newdopusfunction *menu;
    struct dopusfunction *drive;
    struct dopusfiletype *firsttype;
    struct dopusgadgetbanks *firstbank;
    struct DOpusRemember *key;
    struct dopushotkey *firsthotkey;
    char menutit[5][16];
};

extern struct ConfigUndo *makeundo();
extern char *getcopy(),*compilefunclist(),**makefiletypelist(),**makeclasslist();
extern struct dopusgadgetbanks *lastbank();

extern int fontplaceflags[],fontplacevals[];
extern struct RMBGadget nextbankrmb,insertbankrmb,formatclearrmb,sampleclearrmb;
extern struct DOpusListView
    editlists[3],cmdlist,screenmodeview,listlist,iconlistview,
    fontsizelistview,fontlistview,fontplacelist,editclasslist,hotkeyslist,
    helplist,filetypeactionlist,
    modulelist/*,languagelist*/;
extern char *external_module_list[],*external_module_name[];
extern int external_module_map[];

extern struct Gadget
    editfuncgadgets[15],cmdcancelgad,menuslidergads[3],editdrivegadgets[4],
    palettegads[6],coloursgad,screenmodegads[7],editclassgadgets[14],
    listokaygad[3],formatgads[11 /* HUX: was 10 */],hotkeygad,hotkeymmbgad,arrowgadgets[8],
    fontsizegadget,helpgad,helpcancelgad,draggad,icongads[4],depthgads[2],
    screen_sliders_gadgets[];

extern struct Image menusliderimage;
extern struct PropInfo menusliderprop,paletteprop[3];

extern char
    edit_namebuf[256],edit_stackbuf[7],edit_prioritybuf[5],edit_delaybuf[3],
    edit_funcbuf[256],edit_pathbuf[256],edit_actionbuf[40],
    palette_buf[3][4],screenwidth_buf[6],screenheight_buf[6],screendepth_buf[4],
    formatlen_buf[5][8],fontsize_buf[12],edit_typeidbuf[8];

extern struct StringInfo namesinfo,funcsinfo;

#ifdef __SASC_60
extern __chip UBYTE
    glass_data[2][18],uparrow_data[6],downarrow_data[6],
    pageflip_data1[5],pageflip_data2[3];
#else
extern UBYTE
    __chip glass_data[2][36],
    __chip uparrow_data[12],
    __chip downarrow_data[12],
    __chip pageflip_data1[10],
    __chip pageflip_data2[6];
#endif

extern struct IntuiMessage *getintuimsg();

extern struct newdopusfunction *selgad;
extern struct Gadget *seligad;

struct ScreenMode {
    struct ScreenMode *next;
    char name[80];
    UWORD minw,minh;
    UWORD maxw,maxh;
    UWORD defw,defh;
    UWORD maxdepth;
    ULONG mode;
};

#define MODE_WORKBENCHUSE        1
#define MODE_WORKBENCHCLONE      2
#define MODE_PUBLICSCREENUSE     3
#define MODE_PUBLICSCREENCLONE   4

extern char **screenmodelist;
extern struct ScreenMode *firstmode,*screenmode;
extern struct ScreenMode *showdisplaydesc(),*getscreenmode();

#define SCRFLAGS_DEFWIDTH   1
#define SCRFLAGS_DEFHEIGHT  2
#define SCRFLAGS_HALFHEIGHT 4

extern struct MenuItem
    projectitems[],edititems[],gadrowsitems[],classitems[],neatstuffitem;
extern struct Menu projectmenu,editmenu,gadrowsmenu,classmenu,neatstuffmenu;

extern int changed,curoperation,clipcount;

struct Clip {
    struct Clip *next;
    char name[256];
    struct dopusfunction func;
};

extern struct Clip *firstclip;

extern UWORD defpalettes[11][4],def_dopus_palette[16];
extern struct DOpusListView palettelist,listformatlists[2];
extern int rowtrans[5];

extern struct Library *DiskfontBase;
extern struct Library *WorkbenchBase;
extern struct Library *IconBase;
extern struct TextFont *tfont;
extern char
    *fontdatabuf,**fontlist,*(**fontsizelist),
    **noproplist,*(**nopropsizelist),
    **only8list,*only8sizelist[];

extern int gadflagvals[];

extern struct fileclass *firstclass;
extern int classopvals[];
extern char classname[256];
extern struct DOpusRemember *helpkey;
extern char *helpbuffer;
extern int helpsize;
extern char currenthelpname[256];

extern char **fileclasslist;
extern char *fileclasstype[];
extern char maxlength[5];

extern struct StringExtend stringex;

extern struct DiskObject dropboxobj;
extern struct DiskObject *dropboxicon;
extern void *appobject;
extern struct AppMessage *appmsg;

#define MY_APPOBJECT 1

extern unsigned char *fileview_buf;
extern int fileview_lines,fileview_size,fileview_topline,fileview_type,fileview_oldtop;
extern int fileview_offset,fileview_position;
extern char *fileview_types[];

extern char *icontypes[],*palettenames[14],*fontplacenames[FONT_COUNT+1];

extern struct IntuiText
    newtext,opentext,savetext,saveastext,cuttext,copytext,
    pastetext,erasetext,newclasstext,editclasstext,deleteclasstext,
    clearclasstext,duplicateclasstext,clearcliptext,
    gadrowstext[5],paintmodetext;

extern int network;

extern char *ftype_funcs[];

extern struct StringData stringdata;

extern short func_list_items[];

extern struct Border *gadget_dog_sel_border,*gadget_dog_unsel_border;
extern struct Border *gadget_sel_border,*gadget_unsel_border;

extern int x_off,y_off,x_bot,y_bot;
extern char help_ok;
extern int paint_state,paint_fg,paint_bg;

extern int screen_depth;

extern short
    editfuncgadgets_xy[15][2],
    editdrivegadgets_xy[4][2],
    menuslidergads_xy[3][2],
    palettegads_xy[6][2],
    coloursgad_xy[2],
    screenmodegads_xy[7][2],
    formatgads_xy[11 /* HUX: was 10 */][2],
    icongads_xy[4][2],
    hotkeymmbgad_xy[2],
    hotkeygad_xy[2],
    arrowgadgets_xy[8][2],
    fontsizegadget_xy[2],
    editclassgadgets_xy[14][2],
    screen_sliders_gadgets_xy[2][2];
/*
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
*/
#define FREQ_FILETYPE  10
#define FREQ_FILECLASS 11
#define FREQ_ARGREQ    12
#define FREQ_PATHREQ   20
#define FREQ_GENERIC   21

struct ColourTable {
    ULONG red;
    ULONG green;
    ULONG blue;
    unsigned char pen;
    char alloc;
};

extern struct ColourTable screen_pens[16];

extern short colour_box_xy[2][16][4],colour_box_num;

extern struct ScreenMode *curmode;
extern ULONG palette_backup[48];
extern struct DOpusRemember *borderkey;
extern short palettegad_xy[16][4];
extern int bpg;

extern char *left_right_cycle[3];

#include "configstrings.h"

#include "functions.h"

