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
#define VERSION "4.17a"

char *version="$VER: ConfigOpus " VERSION " (" __DATE__ ")";

UWORD drawinfo[]={
    0, /* DETAILPEN */
    1, /* BLOCKPEN */
    1, /* TEXTPEN */
    2, /* SHINEPEN */
    1, /* SHADOWPEN */
    3, /* FILLPEN */
    1, /* FILLTEXTPEN */
    0, /* BACKGROUNDPEN */
    3, /* HIGHLIGHTTEXTPEN */
    1, /* BARDETAILPEN */
    2, /* BARBLOCKPEN */
    1, /* BARTRIMPEN */
    (UWORD)~0};

struct TagItem
    scr_taglist[]={
        {SA_Pens,(IPTR)drawinfo},
        {SA_DisplayID,0},
        {TAG_END,0}};

struct ExtNewScreen configscr={
    0,0,640,200,0,0,1,
    HIRES,CUSTOMSCREEN|SCREENBEHIND|NS_EXTENDED,
    &sfont,"ConfigOpus " VERSION,NULL,NULL,scr_taglist};

struct NewWindow
    configwin={
        0,11,640,0,
        255,255,
        IDCMP_MOUSEBUTTONS|IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MENUPICK|
            IDCMP_MOUSEMOVE|IDCMP_INACTIVEWINDOW|IDCMP_RAWKEY,
        WFLG_ACTIVATE|WFLG_REPORTMOUSE|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_NEWLOOKMENUS,
        NULL,NULL,
        NULL,
        NULL,NULL,
        0,0,0,0,
        CUSTOMSCREEN};

struct DOpusBase *DOpusBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *LayersBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *DiskfontBase = NULL;
struct Library *WorkbenchBase = NULL;
struct Library *IconBase = NULL;
struct muBase *muBase = NULL;
struct Screen *Screen = NULL;
struct Window *Window = NULL;
struct ViewPort *vp;
struct RastPort *rp;
struct Process *myproc = NULL;
struct IntuiMessage *IMsg;
APTR wsave,realwsave;
char *spacestring="                                                                                                                                                      ";

char fontbuf[50];
struct TextAttr
    sfont={(STRPTR)fontbuf,8,0,0},
    bsfont={(STRPTR)fontbuf,8,FSF_BOLD,0};
struct Config *config = NULL,*undoconfig = NULL;
struct dopusfiletype *firsttype = NULL;
struct dopushotkey *firsthotkey = NULL;
struct dopusgadgetbanks *firstbank = NULL,*curbank = NULL;
struct DOpusRemember *typekey = NULL,*fontkey = NULL;

char configname[256],loadnamebuf[256];

int version2;

struct MsgPort *conport = NULL,*cmdport = NULL,*appport = NULL;
/*
struct MsgPort **clip_port;
struct IOClipReq *clip_io;
*/
struct ConfigStuff cstuff;
struct DOpusRemember *mainkey = NULL,*gadgetkey = NULL,*tickkey = NULL,*screenkey,*clipkey= NULL,*buttonkey = NULL;
struct Gadget *maingad = NULL,*tickgad = NULL,*gadgads = NULL,*menugads = NULL,*drivegads = NULL;
struct Clip *firstclip = NULL;
int tickcount=0;
struct dopusconfigmsg msg;

int lchanged=0,changed=0;

struct Image
    *checkonimage,*checkoffimage,
    *buttononimage,*buttonoffimage,
    copy_checkonimage,copy_checkoffimage;

struct RMBGadget
    insertbankrmb={
        NULL,0,0,104,12,RGF_ALTTEXT,1,
        {{NULL,2,1,100,10,1,0},{NULL,2,1,100,10,1,0}}},
    nextbankrmb={
        &insertbankrmb,0,0,104,12,RGF_ALTTEXT,0,
        {{NULL,2,1,100,10,1,0},{NULL,2,1,100,10,1,0}}},
    formatclearrmb={
        NULL,465,137,104,12,RGF_ALTTEXT,0,
        {{NULL,2,1,100,10,1,0},{NULL,2,1,100,10,1,0}}},
    sampleclearrmb={
        NULL,0,0,104,12,RGF_ALTTEXT,0,
        {{NULL,2,1,100,10,1,0},{NULL,2,1,100,10,1,0}}};

char
    *commandlist[95],
    *arglist[18],
    *classopslist[13];

int classopvals[]={
    FTYC_AND,FTYC_MATCH,FTYC_MATCHBITS,FTYC_MATCHCOMMENT,FTYC_MATCHDATE,
    FTYC_MATCHNAME,FTYC_MATCHSIZE,FTYC_MOVE,FTYC_MOVETO,FTYC_OR,FTYC_SEARCHFOR,FTYC_MATCHI};

struct DOpusListView
    cmdlist={
        1,NULL,0,0,332,88,NULL,commandlist,NULL,DLVF_HIREC,8,
        0,0,0,0,0,0,0,0,0,0,-1,NULL},
    listlist={
        1,NULL,0,0,0,0,NULL,NULL,NULL,DLVF_HIREC,8,
        0,0,0,0,0,0,0,0,0,0,-1,NULL},
    helplist={
        1,NULL,0,0,516,131,NULL,NULL,NULL,DLVF_HIREC,8,
        0,0,0,0,0,0,0,0,0,0,-1,NULL},
    modulelist={
        5,NULL,0,0,338,56,NULL,NULL,NULL,
        DLVF_MULTI|DLVF_TTOP|DLVF_CHECK|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,/*&languagelist},
    languagelist={
        6,NULL,0,0,338,56,NULL,NULL,NULL,
        DLVF_CHECK|DLVF_LEAVE|DLVF_TTOP|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,*/NULL};

char *external_module_list[]={
    "ConfigOpus",
    "Diskcopy, Format, Install",
    "IconInfo",
    "Print, Printdir",
    NULL};

char *external_module_name[]={
    "ConfigOpus",
    "DOpus_Disk",
    "DOpus_Icon",
    "DOpus_Print",
    NULL};

int external_module_map[]={
    65536,
    1,
    4,
    2};

struct Gadget
    cmdcancelgad={
        NULL,124,102,104,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
        NULL,NULL,NULL,0,NULL,0,NULL},
    helpcancelgad={
        NULL,226,147,104,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
        NULL,NULL,NULL,0,NULL,0,NULL},
    listokaygad[3]={
        {&listokaygad[1],10,0,104,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
        NULL,NULL,NULL,0,NULL,1,NULL},
        {NULL,0,0,104,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
        NULL,NULL,NULL,0,NULL,0,NULL},
        {NULL,0,0,104,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
        NULL,NULL,NULL,0,NULL,2,NULL}};

struct NewWindow
    requestwin={
        0,0,0,0,
        255,255,
        IDCMP_MOUSEBUTTONS|IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE|IDCMP_VANILLAKEY,
        WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_DRAGBAR|WFLG_DEPTHGADGET,
        NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN};

char filebuf[256],dirbuf[258];
struct DOpusFileReq filereq={
    NULL,dirbuf,filebuf,NULL,162,22,10,0,NULL,NULL};

char *defdir[5]={"","SYS:","SYS:","S:","REXX:"};

struct newdopusfunction *selgad;
struct Gadget *seligad;

struct ColourSel coloursel[]={
    {COLBIT_STATUS,149,57,598,76},
    {COLBIT_SELDISKNAME,149,77,373,86},
    {COLBIT_DISKNAME,374,75,598,86},
    {COLBIT_SELDIRECTORY,151,88,353,95},
    {COLBIT_DIRECTORY,151,96,353,103},
    {COLBIT_SELFILE,151,104,353,111},
    {COLBIT_FILE,151,112,353,128},
    {COLBIT_FILE,394,88,596,128},
    {COLBIT_SLIDERS,356,87,391,111},
    {COLBIT_SLIDERS,149,130,319,140},
    {COLBIT_SLIDERS,428,130,599,140},
    {COLBIT_ARROWS,320,130,352,153},
    {COLBIT_ARROWS,392,130,427,153},
    {COLBIT_ARROWS,356,112,391,129},
    {COLBIT_TINYGADS,356,130,391,153},
    {COLBIT_TINYGADS,491,153,598,162},
    {COLBIT_CLOCK,149,153,490,162},
    {COLBIT_3DBOXES,160,167,371,178},
    {COLBIT_REQUESTER,376,167,587,178},
    {COLBIT_STRINGS,149,141,319,152},
    {COLBIT_SELSTRINGS,428,141,599,152},
    {-1,0,0,0,0}};

char **screenmodelist;
struct ScreenMode *firstmode,*screenmode;

struct Image
    open_barimage={0,0,0,0,0,NULL,0,0,NULL},
    paste_barimage={0,0,0,0,0,NULL,0,0,NULL},
    erase_barimage={0,0,0,0,0,NULL,0,0,NULL},
    deleteclass_barimage={0,0,0,0,0,NULL,0,0,NULL};

struct IntuiText
    newtext={0,0,JAM1,1,1,NULL,NULL,NULL},
    opentext={0,0,JAM1,1,1,NULL,NULL,NULL},
    savetext={0,0,JAM1,1,1,NULL,NULL,NULL},
    saveastext={0,0,JAM1,1,1,NULL,NULL,NULL},

    cuttext={0,0,JAM1,1,1,NULL,NULL,NULL},
    copytext={0,0,JAM1,1,1,NULL,NULL,NULL},
    pastetext={0,0,JAM1,1,1,NULL,NULL,NULL},
    erasetext={0,0,JAM1,1,1,NULL,NULL,NULL},
    clearcliptext={0,0,JAM1,1,1,NULL,NULL,NULL},

    newclasstext={0,0,JAM1,1,1,NULL,NULL,NULL},
    editclasstext={0,0,JAM1,1,1,NULL,NULL,NULL},
    duplicateclasstext={0,0,JAM1,1,1,NULL,NULL,NULL},
    deleteclasstext={0,0,JAM1,1,1,NULL,NULL,NULL},
    clearclasstext={0,0,JAM1,1,1,NULL,NULL,NULL},

    gadrowstext[5],

    paintmodetext={0,0,JAM1,1,1,NULL,NULL,NULL};

struct MenuItem
    projectitems[]={
        {&projectitems[1],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
        (APTR)&newtext,NULL,'D',NULL,0xffff},
        {&projectitems[2],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
        (APTR)&opentext,NULL,'O',NULL,0xffff},
        {&projectitems[3],0,0,0,2,HIGHNONE,0,
        (APTR)&open_barimage,NULL,0,NULL,0xffff},
        {&projectitems[4],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
        (APTR)&savetext,NULL,'S',NULL,0xffff},
        {NULL,0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ,0,
        (APTR)&saveastext,NULL,'A',NULL,0xffff}},
    edititems[]={
        {&edititems[1],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&cuttext,NULL,0,NULL,0xffff},
        {&edititems[2],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&copytext,NULL,0,NULL,0xffff},
        {&edititems[3],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&pastetext,NULL,0,NULL,0xffff},
        {&edititems[4],0,0,0,2,HIGHNONE,0,
        (APTR)&paste_barimage,NULL,0,NULL,0xffff},
        {&edititems[5],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&erasetext,NULL,0,NULL,0xffff},
        {&edititems[6],0,0,0,2,HIGHNONE,0,
        (APTR)&erase_barimage,NULL,0,NULL,0xffff},
        {NULL,0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&clearcliptext,NULL,0,NULL,0xffff}},
    classitems[]={
        {&classitems[1],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&newclasstext,NULL,0,NULL,0xffff},
        {&classitems[2],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&editclasstext,NULL,0,NULL,0xffff},
        {&classitems[3],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&duplicateclasstext,NULL,0,NULL,0xffff},
        {&classitems[4],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&deleteclasstext,NULL,0,NULL,0xffff},
        {&classitems[5],0,0,0,2,HIGHNONE,0,
        (APTR)&deleteclass_barimage,NULL,0,NULL,0xffff},
        {&classitems[6],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&opentext,NULL,0,NULL,0xffff},
        {&classitems[7],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&savetext,NULL,0,NULL,0xffff},
        {&classitems[8],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&saveastext,NULL,0,NULL,0xffff},
        {NULL,0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP,0,
        (APTR)&clearclasstext,NULL,0,NULL,0xffff}},
    gadrowsitems[]={
        {&gadrowsitems[1],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT,30,
        (APTR)&gadrowstext[0],NULL,0,NULL,0xffff},
        {&gadrowsitems[2],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT,29,
        (APTR)&gadrowstext[1],NULL,0,NULL,0xffff},
        {&gadrowsitems[3],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT,27,
        (APTR)&gadrowstext[2],NULL,0,NULL,0xffff},
        {&gadrowsitems[4],0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT,23,
        (APTR)&gadrowstext[3],NULL,0,NULL,0xffff},
        {NULL,0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT,15,
        (APTR)&gadrowstext[4],NULL,0,NULL,0xffff}},
    neatstuffitem={
        NULL,0,0,0,0,ITEMTEXT|ITEMENABLED|HIGHCOMP|CHECKIT|MENUTOGGLE,0,
        (APTR)&paintmodetext,NULL,0,NULL,0xffff};

struct Menu
    projectmenu={NULL,8,0,0,0,MENUENABLED,NULL,projectitems},
    gadrowsmenu={&neatstuffmenu,0,0,0,0,MENUENABLED,NULL,gadrowsitems},
    editmenu={NULL,8,0,0,0,MENUENABLED,NULL,edititems},
    classmenu={NULL,0,0,0,0,MENUENABLED,NULL,classitems},
    neatstuffmenu={NULL,0,0,0,0,MENUENABLED,NULL,&neatstuffitem};

int curoperation=-1,clipcount=0;

UWORD
    defpalettes[11][4]={
        {0xaaa,0x000,0xfff,0x68b},
        {0,0,0,0},
        {0xba9,0x002,0xfff,0x68b},
        {0x8ac,0x002,0xfff,0xfc9},
        {0x8ac,0x002,0xfff,0xe97},
        {0x5ba,0x002,0xeef,0x57a},
        {0x9bd,0x002,0xfff,0x68b},
        {0xa98,0x321,0xfee,0xfdb},
        {0xccb,0x003,0xfff,0x9ab},
        {0xc99,0x002,0xfee,0xb67},
        {0xa96,0x002,0xfff,0x779}},

    def_dopus_palette[16]={
        0xaaa,0x000,0xfff,0x05b,0xea4,0x707,0xff0,0xc20,
        0xf80,0xf0f,0x963,0x0f9,0x4f3,0x000,0xfff,0x259};

char *palettenames[14];

char *fontplacenames[FONT_COUNT+1];

int fontplaceflags[]={
    FFLAG_8ONLY,0,FFLAG_NOPROP,0,0,0,0,0,FFLAG_NOPROP,0,0,0};

int fontplacevals[]={
    FONT_CLOCK,FONT_GADGETS,FONT_MENUS,FONT_NAMES,FONT_DIRS,    
    FONT_GENERAL,FONT_ICONIFY,FONT_STRING,FONT_REQUEST,
    FONT_SCREEN,FONT_STATUS,FONT_TEXT};

struct DOpusListView
    palettelist={
        2,NULL,0,0,250,56,NULL,palettenames,NULL,
        DLVF_SLOW|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,NULL},
    fontplacelist={
        1,NULL,0,0,176,64,NULL,fontplacenames,NULL,
        DLVF_SLOW|DLVF_LEAVE|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,&fontlistview},
    fontlistview={
        2,NULL,0,0,176,64,NULL,NULL,NULL,
        DLVF_SLOW|DLVF_LEAVE|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,&fontsizelistview},
    fontsizelistview={
        3,NULL,0,0,32,64,NULL,NULL,NULL,
        DLVF_SLOW|DLVF_LEAVE|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,NULL},
    listformatlists[2]={
        {1,NULL,0,0,196,56,NULL,formatnames,NULL,
        DLVF_SLOW|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,&listformatlists[1]},
        {2,NULL,0,0,196,56,NULL,selectedformatnames,NULL,
        DLVF_SLOW|DLVF_LEAVE|DLVF_CHECK|DLVF_TOGGLE|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,NULL}},
    iconlistview={
        3,NULL,0,0,452,104,NULL,NULL,NULL,
        DLVF_TTOP|DLVF_LEAVE|DLVF_HIREC,8,0,0,0,0,0,0,0,0,0,0,-1,NULL};

char
    *formatnames[FORMAT_MAXNUM+1],
    *selectedformatnames[FORMAT_MAXNUM+1];

int rowtrans[5]={0,1,2,3,6};
struct TextFont *tfont;
char
    *fontdatabuf=NULL,**fontlist,*(**fontsizelist),
    **noproplist,*(**nopropsizelist),
    **only8list,*only8sizelist[]={"8",NULL};
struct fileclass *firstclass;
char classname[256];
struct DOpusRemember *helpkey = NULL;
char *helpbuffer = NULL;
int helpsize;
char currenthelpname[256];

char **fileclasslist;

char *fileclasstype[4];

//char maxlength[5]={64,80,40,32,32};

extern UBYTE pageflip_data1[];
extern UBYTE pageflip_data2[];
extern UBYTE dropbox_data[];

struct StringExtend stringex={NULL,{0,0},{0,0},0,NULL,NULL,{0,0,0,0}};
struct Image dropboximage={0,0,83,42,2,(UWORD *)dropbox_data,3,0,NULL};

struct DiskObject *dropboxicon = NULL;
struct DiskObject dropboxobj={
    0,0, /* WB_DISKMAGIC,WB_DISKVERSION, */
        {NULL,0,0,83,42,0,0,0,
        (APTR)&dropboximage,NULL,NULL,0,NULL,0,NULL},
    0,NULL,NULL,NO_ICON_POSITION,NO_ICON_POSITION,NULL,NULL,0};

void *appobject = NULL;
struct AppMessage *appmsg;

unsigned char *fileview_buf;
int fileview_lines,fileview_size,fileview_topline,fileview_type,fileview_oldtop;
int fileview_offset,fileview_position;
char *fileview_types[2];
char *icontypes[3];

struct StringData stringdata;

int network;

char *ftype_funcs[]={
    "AnsiRead",
    "HexRead",
    "LoopPlay",
    "Play",
    "Read",
    "Show",
    "User1",
    "User2",
    "User3",
    "User4"};

short func_list_items[]={
    STR_FUNCTIONLIST_ABOUT,
    STR_FUNCTIONLIST_ADDICON,
    STR_FUNCTIONLIST_ALARM,
    STR_FUNCTIONLIST_ALL,
    STR_FUNCTIONLIST_ANSIREAD,
    STR_FUNCTIONLIST_AREXX,
    STR_FUNCTIONLIST_ASSIGN,
    STR_FUNCTIONLIST_BEEP,
    STR_FUNCTIONLIST_BUFFERLIST,
    STR_FUNCTIONLIST_BUTTONICONIFY,
    STR_FUNCTIONLIST_CD,
    STR_FUNCTIONLIST_CHECKFIT,
    STR_FUNCTIONLIST_CLEARBUFFERS,
    STR_FUNCTIONLIST_CLEARSIZES,
    STR_FUNCTIONLIST_CLEARWIN,
    STR_FUNCTIONLIST_CLONE,
    STR_FUNCTIONLIST_COMMENT,
    STR_FUNCTIONLIST_CONFIGURE,
    STR_FUNCTIONLIST_CONTST,
    STR_FUNCTIONLIST_COPY,
    STR_FUNCTIONLIST_COPYAS,
    STR_FUNCTIONLIST_COPYWINDOW,
    STR_FUNCTIONLIST_DATESTAMP,
    STR_FUNCTIONLIST_DEFAULTS,
    STR_FUNCTIONLIST_DELETE,
    STR_FUNCTIONLIST_DIRTREE,
    STR_FUNCTIONLIST_DISKCOPY,
    STR_FUNCTIONLIST_DISKCOPYBG,
    STR_FUNCTIONLIST_DISKINFO,
    STR_FUNCTIONLIST_DISPLAYDIR,
    STR_FUNCTIONLIST_DOPUSTOBACK,
    STR_FUNCTIONLIST_DOPUSTOFRONT,
    STR_FUNCTIONLIST_ENCRYPT,
    STR_FUNCTIONLIST_ERRORHELP,
    STR_FUNCTIONLIST_EXECUTE,
    STR_FUNCTIONLIST_ENDFUNCTION,
    STR_FUNCTIONLIST_FORMAT,
    STR_FUNCTIONLIST_FORMATBG,
    STR_FUNCTIONLIST_GETDEVICES,
    STR_FUNCTIONLIST_GETSIZES,
    STR_FUNCTIONLIST_HELP,
    STR_FUNCTIONLIST_HEXREAD,
    STR_FUNCTIONLIST_HUNT,
    STR_FUNCTIONLIST_ICONIFY,
    STR_FUNCTIONLIST_ICONINFO,
    STR_FUNCTIONLIST_INSTALL,
    STR_FUNCTIONLIST_INSTALLBG,
    STR_FUNCTIONLIST_LASTSAVED,
    STR_FUNCTIONLIST_LOADCONFIG,
    STR_FUNCTIONLIST_LOADSTRINGS,
    STR_FUNCTIONLIST_LOOPPLAY,
    STR_FUNCTIONLIST_MAKEDIR,
    STR_FUNCTIONLIST_MAKELINK,
    STR_FUNCTIONLIST_MODIFY,
    STR_FUNCTIONLIST_MOVE,
    STR_FUNCTIONLIST_MOVEAS,
    STR_FUNCTIONLIST_NEWCLI,
    STR_FUNCTIONLIST_NONE,
    STR_FUNCTIONLIST_NOTIFY,
    STR_FUNCTIONLIST_OTHERWINDOW,
    STR_FUNCTIONLIST_PARENT,
    STR_FUNCTIONLIST_PARENTLIST,
    STR_FUNCTIONLIST_PLAY,
    STR_FUNCTIONLIST_PLAYST,
    STR_FUNCTIONLIST_PRINT,
    STR_FUNCTIONLIST_PRINTDIR,
    STR_FUNCTIONLIST_PROTECT,
    STR_FUNCTIONLIST_QUIT,
    STR_FUNCTIONLIST_READ,
    STR_FUNCTIONLIST_REDRAW,
    STR_FUNCTIONLIST_RELABEL,
    STR_FUNCTIONLIST_REMEMBER,
    STR_FUNCTIONLIST_RENAME,
    STR_FUNCTIONLIST_RESCAN,
    STR_FUNCTIONLIST_RESELECT,
    STR_FUNCTIONLIST_RESTORE,
    STR_FUNCTIONLIST_ROOT,
    STR_FUNCTIONLIST_RUN,
    STR_FUNCTIONLIST_SAVECONFIG,
    STR_FUNCTIONLIST_SCANDIR,
    STR_FUNCTIONLIST_SEARCH,
    STR_FUNCTIONLIST_SELECT,
    STR_FUNCTIONLIST_SETVAR,
    STR_FUNCTIONLIST_SHOW,
    STR_FUNCTIONLIST_SMARTREAD,
    STR_FUNCTIONLIST_STOPST,
    STR_FUNCTIONLIST_SWAPWINDOW,
    STR_FUNCTIONLIST_TOGGLE,
    STR_FUNCTIONLIST_UNICONIFY,
    STR_FUNCTIONLIST_USER_FTYPE1,
    STR_FUNCTIONLIST_USER_FTYPE2,
    STR_FUNCTIONLIST_USER_FTYPE3,
    STR_FUNCTIONLIST_USER_FTYPE4,
    STR_FUNCTIONLIST_VERIFY,
    STR_FUNCTIONLIST_VERSION};

struct Border *gadget_dog_sel_border,*gadget_dog_unsel_border;
struct Border *gadget_sel_border,*gadget_unsel_border;

int x_off,y_off,x_bot,y_bot;
char help_ok;
int paint_state,paint_fg=1,paint_bg=0;

int screen_depth;

struct ColourTable screen_pens[16];

short colour_box_xy[2][16][4],colour_box_num;

struct ScreenMode *curmode;
ULONG palette_backup[48];
struct DOpusRemember *borderkey;
short palettegad_xy[16][4];
int bpg;

char *left_right_cycle[3];
