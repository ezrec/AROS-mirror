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

/* Definitions of any data that relates to the Screen display. Includes
   TextAttrs, NewScreen structures and TagItems, and various global data */

#include "dopus.h"

/* TextAttr structures */

struct TextAttr
    main_screen_attr={NULL,0,0,0},          /* Main screen font */
    screen_attr={NULL,8,0,0},               /* General screen font */
    menu_attr={NULL,0,0,0};                 /* Menu font */

/* Screen tag lists */

UWORD scr_drawinfo[NUMDRIPENS+1];         /* Pen lists */
UWORD zoom_data[]={0,0,640,200};          /* Zoom gadget data */

struct TagItem
    mainscreen_tags[]={                     /* Tags for the main screen */
        {SA_DisplayID,0},
        {SA_Pens,(IPTR)scr_drawinfo},
        {SA_PubName,(IPTR)str_arexx_portname},
        {SA_Overscan,OSCAN_TEXT},
        {SA_AutoScroll,TRUE},
        {SA_Interleaved,TRUE},
        {SA_SharePens,TRUE},
        {SA_SysFont,1},
        {TAG_DONE,0}},

    stdscreen_tags[]={                      /* Tags for a standard HIRES screen */
        {SA_DisplayID,HIRES_KEY},
        {SA_Pens,(IPTR)scr_drawinfo},
        {TAG_DONE,0}},

    mainwindow_tags[]={                     /* Tags for the main window */
        {WA_PubScreen,0},
        {WA_AutoAdjust,TRUE},
        {WA_Zoom,(IPTR)zoom_data},
        {TAG_DONE,0}};

/* NewScreen structures */

struct ExtNewScreen
    main_scr={                              /* Main screen */
        0,0,0,0,0,
        0,1,
        0,
        CUSTOMSCREEN|SCREENBEHIND|NS_EXTENDED,
        &main_screen_attr,
        (STRPTR)str_arexx_portname,
        NULL,NULL,
        mainscreen_tags},

    font_scr={
        0,0,640,0,1,0,1,                      /* Font screen */
        HIRES,
        CUSTOMSCREEN|SCREENBEHIND|SCREENQUIET|NS_EXTENDED,
        NULL,NULL,NULL,NULL,stdscreen_tags},
    blank_scr={                             /* Blank screen */
        0,0,320,STDSCREENHEIGHT,1,0,1,
        0,
        CUSTOMSCREEN|SCREENQUIET,
        NULL,NULL,NULL,NULL,NULL};

/* NewWindow structures */

struct ExtNewWindow
    main_win={                              /* Main window */
        0,0,0,0,255,255,
        IDCMP_MOUSEBUTTONS|IDCMP_MENUPICK|IDCMP_GADGETUP|
            IDCMP_GADGETDOWN|IDCMP_RAWKEY|IDCMP_MOUSEMOVE|
            IDCMP_DISKINSERTED|IDCMP_DISKREMOVED|IDCMP_INACTIVEWINDOW|
            IDCMP_ACTIVEWINDOW|IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE,
        0,
        NULL,NULL,NULL,NULL,NULL,640,200,65535,65535,CUSTOMSCREEN,
        mainwindow_tags};

struct NewWindow
    font_win={                              /* Font window */
        0,0,640,200,255,255,
        IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INACTIVEWINDOW,
        WFLG_RMBTRAP|WFLG_BORDERLESS|WFLG_SIMPLE_REFRESH|WFLG_NOCAREREFRESH,
        NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN},

    disk_win={                              /* DiskInfo window */
        0,0,0,0,255,255,
        IDCMP_RAWKEY|IDCMP_DISKINSERTED|IDCMP_DISKREMOVED|IDCMP_GADGETUP|IDCMP_VANILLAKEY,
        WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_ACTIVATE,
        NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN};

struct AppWindow *dopus_appwindow=NULL;   /* AppWindow when on Workbench screen */


/* Definitions for the custom menus */

struct Menu menu_menus[5];                /* Custom menus */
struct MenuItem  *menu_menuitem;          /* Custom menu items */
struct IntuiText *menu_intuitext;         /* IntuiText for custom menu items */

char *menu_itemnames;                     /* Names of the menu items */
char menu_spaceitemnames[5][80];          /* Space background of menu items */

UBYTE menu_real_number[5];                /* The real ordinal number of each menu */

/* Internal Screen Data */

struct ColourTable screen_pens[16];       /* Colour pen table */

UWORD scrdata_norm_width;                /* Normal width of the display */
UWORD scrdata_norm_height;               /* Normal height of the display */
UWORD scrdata_scr_height;                /* Main screen/window height */

UWORD scrdata_dispwin_lines;             /* Number of lines in each window */
UWORD scrdata_dispwin_width[2];          /* Displayed width in each window */
UWORD scrdata_dispwin_height;            /* Height of directory windows */
UWORD scrdata_dispwin_nchars[2];         /* Width of each window in characters */
UWORD scrdata_dispwin_ypos;              /* Y-Position of directory windows */
UWORD scrdata_dispwin_left[2];           /* Left edge of directory windows */
UWORD scrdata_dispwin_xpos[2];           /* Left edge of text in windows */
UWORD scrdata_dispwin_center;            /* Center of the two windows */

UWORD scrdata_dirwin_xpos[2];            /* Rendered X-Position of each window */
UWORD scrdata_dirwin_ypos[2];            /* Rendered Y-Position of each window */
UWORD scrdata_dirwin_width[2];           /* Rendered width of each window */
UWORD scrdata_dirwin_height;             /* Rendered height of directory windows */
UWORD scrdata_dirwin_bottom;             /* Rendered bottom of each window */
UWORD scrdata_dirwin_center;             /* Rendered center of the two windows */

UWORD scrdata_drive_width;               /* Width of the drive gadgets */
UWORD scrdata_gadget_offset;             /* X-Offset of entire gadget display */
UWORD scrdata_gadget_xpos;               /* X-Position of the custom gadgets */
UWORD scrdata_gadget_ypos;               /* Y-Position of the custom gadgets */
UWORD scrdata_gadget_width;              /* Width of the custom gadgets */
UWORD scrdata_gadget_height;             /* Height of the custom gadgets */

UWORD scrdata_clock_width;               /* Width of the clock bar */
UWORD scrdata_clock_height;              /* Height of the clock bar */
UWORD scrdata_clock_xpos;                /* x-pos of the clock bar */
UWORD scrdata_clock_ypos;                /* y-pos of the clock bar */
UWORD scrdata_status_xpos;               /* X-Position of the status bar */
UWORD scrdata_status_ypos;               /* Y-Position of the status bar */
UWORD scrdata_status_width;              /* Width of the status bar */
UWORD scrdata_status_height;             /* Height of the status bar */
UWORD scrdata_diskname_width[2];         /* Width of each diskname display */
UWORD scrdata_diskname_height;           /* Height of the diskname displays */
UWORD scrdata_diskname_ypos;             /* Y-Position of the diskname displays */
UWORD scrdata_diskname_xpos[2];          /* X-Position of each diskname display */
UWORD scrdata_prop_height;               /* Height of the vertical sliders */
UWORD scrdata_string_height;             /* Height of the path string gadgets */
UWORD scrdata_closegad_width;            /* Width of the screen close gadget */

UWORD scrdata_font_xsize;                /* X-Size of the dir window font */
UWORD scrdata_font_ysize;                /* Y-Size of the dir window font */
UWORD scrdata_font_baseline;             /* Baseline of the dir window font */

UBYTE scrdata_is_pal;                     /* Indicates PAL or NTSC system */
UBYTE scrdata_statustext_pos=0;           /* Status text positioning */
LONG scrdata_old_offset_store=-1;         /* Stored offset in directory window */

UWORD scrdata_xoffset;                   /* General x-offset */
UWORD scrdata_yoffset;                   /* General y-offset */
UWORD scrdata_width;                     /* Display width */
UWORD scrdata_height;                    /* Display height */

/* Definitions of screen objects */

struct PropInfo
    vert_propinfo[2]={                      /* Vertical slider PropInfo */
        {FREEVERT|PROPBORDERLESS,0,0,0,0xffff},
        {FREEVERT|PROPBORDERLESS,0,0,0,0xffff}},

    horiz_propinfo[2]={                     /* Horizontal slider PropInfo */
        {FREEHORIZ|PROPBORDERLESS,0,0,0x4152,0},
        {FREEHORIZ|PROPBORDERLESS,0,0,0x4152,0}},

    drive_propinfo={                        /* Drive banks slider PropInfo */
        FREEVERT|PROPBORDERLESS,0,0,0,0xffff},

    gadget_propinfo={                       /* Gadget banks slider PropInfo */
        FREEVERT|PROPBORDERLESS,0,0,0,0xffff};

struct Image
    vert_propimage[2],                      /* Vertical slider Image */
    horiz_propimage[2],                     /* Horizontal slider Image */
    drive_propimage={                       /* Drive banks slider Image */
        0,0,8,0,1,NULL,0,0,NULL},
    gadget_propimage={                      /* Gadget banks slider Image */
        0,0,8,0,1,NULL,0,0,NULL};

struct StringExtend                       /* Path fields StringExtension */
    string_extend={
        NULL,{0,0},{0,0},SGM_EXITHELP,NULL,NULL,{0,0,0,0}},

    requester_stringex={                    /* Requester StringExtension */
        NULL,{0,0},{0,0},0,NULL,NULL,{0,0,0,0}};

char str_pathbuffer[2][256];              /* Path fields buffers */
char str_undobuffer[256];                 /* General undo buffer for string gads */

struct StringInfo                         /* Path fields StringInfos */
    path_stringinfo[2]={
        {(STRPTR)str_pathbuffer[0],(STRPTR)str_undobuffer,0,256,0},
        {(STRPTR)str_pathbuffer[1],(STRPTR)str_undobuffer,0,256,0}};

struct Gadget
    *main_gadgets,                          /* Custom gadgets */
    *drive_gadgets,                         /* Drive gadgets */

    path_strgadget[2]={                     /* Path field gadgets */
        {&path_strgadget[1],2,0,0,0,
            GFLG_GADGHCOMP|GFLG_STRINGEXTEND|GFLG_TABCYCLE,
            GACT_IMMEDIATE|GACT_RELVERIFY|GACT_TOGGLESELECT,
            GTYP_STRGADGET,NULL,NULL,NULL,0,
            (APTR)&path_stringinfo[0],SCRGAD_LEFTSTRING,NULL},
        {NULL,0,0,0,0,
            GFLG_GADGHCOMP|GFLG_STRINGEXTEND|GFLG_TABCYCLE,
            GACT_IMMEDIATE|GACT_RELVERIFY|GACT_TOGGLESELECT,
            GTYP_STRGADGET,NULL,NULL,NULL,0,
            (APTR)&path_stringinfo[1],SCRGAD_RIGHTSTRING,NULL}},

    vert_propgad[2]={                       /* Vertical slider gadgets */
        {&vert_propgad[1],0,0,8,0,
            GFLG_GADGHNONE,
            GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE,
            GTYP_PROPGADGET,(APTR)&vert_propimage[0],NULL,NULL,0,
            (APTR)&vert_propinfo[0],SCRGAD_LEFTPROP,NULL},
        {NULL,0,0,8,0,
            GFLG_GADGHNONE,
            GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE,
            GTYP_PROPGADGET,(APTR)&vert_propimage[1],NULL,NULL,0,
            (APTR)&vert_propinfo[1],SCRGAD_RIGHTPROP,NULL}},

    horiz_propgad[2]={                      /* Horizontal slider gadgets */
        {&horiz_propgad[1],0,0,0,5,
            GFLG_GADGHNONE,
            GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE,
            GTYP_PROPGADGET,(APTR)&horiz_propimage[0],NULL,NULL,0,
            (APTR)&horiz_propinfo[0],SCRGAD_LEFTHPROP,NULL},
        {NULL,0,0,0,5,
            GFLG_GADGHNONE,
            GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE,
            GTYP_PROPGADGET,(APTR)&horiz_propimage[1],NULL,NULL,0,
            (APTR)&horiz_propinfo[1],SCRGAD_RIGHTHPROP,NULL}},

    drive_propgad={                         /* Drive banks slider gadget */
        NULL,0,0,8,0,
        GFLG_GADGHNONE,
        GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE,
        GTYP_PROPGADGET,(APTR)&drive_propimage,NULL,NULL,0,
        (APTR)&drive_propinfo,SCRGAD_DRIVEPROP,NULL},

    gadget_propgad={                        /* Gadget banks slider gadget */
        NULL,0,0,8,0,
        GFLG_GADGHNONE,
        GACT_IMMEDIATE|GACT_RELVERIFY|GACT_FOLLOWMOUSE,
        GTYP_PROPGADGET,(APTR)&gadget_propimage,NULL,NULL,0,
        (APTR)&gadget_propinfo,SCRGAD_GADGETPROP,NULL},

    screen_gadgets[24]={                    /* General screen gadgets */
        {&screen_gadgets[1],0,0,2,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_PARENT1,NULL},
        {&screen_gadgets[2],0,0,2,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_PARENT2,NULL},
        {&screen_gadgets[3],0,0,0,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_BUFFERLIST,NULL},
        {&screen_gadgets[4],0,0,0,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_RESELECT,NULL},
        {&screen_gadgets[5],0,0,0,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_SELECT,NULL},
        {&screen_gadgets[6],0,0,0,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_AREXX,NULL},
        {&screen_gadgets[7],0,0,19,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY|GACT_TOGGLESELECT,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_HELP,NULL},
        {&screen_gadgets[8],0,0,19,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_ERRORHELP,NULL},
        {&screen_gadgets[9],0,0,19,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY|GACT_TOGGLESELECT,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_TINYFILTER,NULL},
        {&screen_gadgets[10],0,0,19,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_CONFIGURE,NULL},
        {&screen_gadgets[11],0,0,19,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_ICONIFY,NULL},
        {&screen_gadgets[12],0,0,19,0,
            GFLG_GADGHCOMP,
            GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,FUNC_QUIT,NULL},
        {&screen_gadgets[13],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVELEFT1,NULL},
        {&screen_gadgets[14],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVELEFT2,NULL},
        {&screen_gadgets[15],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVERIGHT1,NULL},
        {&screen_gadgets[16],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVERIGHT2,NULL},
        {&screen_gadgets[17],0,0,0,9,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVEUP1,NULL},
        {&screen_gadgets[18],0,0,0,9,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVEUP2,NULL},
        {&screen_gadgets[19],0,0,0,9,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVEDOWN1,NULL},
        {&screen_gadgets[20],0,0,0,9,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_MOVEDOWN2,NULL},
        {&screen_gadgets[21],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_BUFFERLEFT1,NULL},
        {&screen_gadgets[22],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_BUFFERLEFT2,NULL},
        {&screen_gadgets[23],0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_BUFFERRIGHT1,NULL},
        {NULL,0,0,16,0,
            GFLG_GADGHCOMP,
            GACT_IMMEDIATE|GACT_RELVERIFY,
            GTYP_BOOLGADGET,NULL,NULL,NULL,0,
            NULL,SCRGAD_BUFFERRIGHT2,NULL}},

    size_gadgets[]={
        {&size_gadgets[1],0,0,0,0,
        GFLG_GADGHNONE,
        GACT_IMMEDIATE|GACT_RELVERIFY,
        GTYP_SIZING,NULL,NULL,NULL,0,
        NULL,65000,NULL},
        {NULL,0,0,0,0,
        GFLG_GADGHNONE,
        GACT_IMMEDIATE|GACT_RELVERIFY,
        GTYP_SIZING,NULL,NULL,NULL,0,
        NULL,65000,NULL}};

struct RMBGadget
    mainrmbgads[7]={                        /* Screen right mouse button gadgets */
        {&mainrmbgads[1],
            0,0,0,0,0,FUNC_NEXTDRIVES},         /* Drive banks (NextDrives) */
        {&mainrmbgads[2],
            0,0,0,0,0,FUNC_CLEARBUFFERS},       /* B tiny gadget (clear buffers) */
        {&mainrmbgads[3],
            0,0,0,0,0,FUNC_RESCAN},             /* R tiny gadget (rescan) */
        {&mainrmbgads[4],
            0,0,0,0,0,FUNC_DEVICELIST},         /* S tiny gadget (device list) */
        {&mainrmbgads[5],
            0,0,0,0,0,FUNC_DIRTREE},            /* A tiny gadget (dir tree) */
        {&mainrmbgads[6],
            0,0,0,0,0,FUNC_ROOT},               /* Left parent gadget (root) */
        {NULL,
            0,0,0,0,0,FUNC_ROOT}};              /* Right parent gadget (root) */

/* Click-M-Click dragging data */

struct VSprite drag_sprite={              /* VSprite used for dragging */
    NULL,NULL,NULL,NULL,0,0,
    SAVEBACK,0,0,0,0,0,0,0,
    NULL,NULL,NULL,NULL,&drag_bob,0,0};

struct Bob drag_bob={                     /* BOB used for dragging */
    0,NULL,NULL,NULL,NULL,
    &drag_sprite,NULL,NULL};

struct BitMap drag_bob_bitmap;            /* Bitmap used to render BOB */
struct RastPort drag_bob_rastport;        /* Rastport used to render BOB */

PLANEPTR drag_bob_buffer=NULL;            /* BOB buffer */
PLANEPTR drag_bob_savebuffer=NULL;        /* BOB background save buffer */

struct GelsInfo drag_gelsinfo={0};        /* GelsInfo structure for BOB */
struct VSprite drag_dummy_sprite1={0};    /* Dummy sprite 1 for GELs list */
struct VSprite drag_dummy_sprite2={0};    /* Dummy sprite 2 for GELs list */

/* General screen data */

struct Rectangle scr_scroll_borders[2];   /* Scroll borders for dir windows */

/* Borders for gadget highlighting */

struct DOpusRemember *border_key=NULL;    /* Memory key for border data */

struct Border *gadget_sel_border;         /* Borders for main gadgets */
struct Border *gadget_unsel_border;

struct Border *gadget_dog_sel_border;     /* Main gadgets with dog-ears*/
struct Border *gadget_dog_unsel_border;

struct Border *drive_sel_border;          /* Borders for drive gadgets */
struct Border *drive_unsel_border;

struct Border *drive_bank_sel_border;     /* Borders for the drive bank */
struct Border *drive_bank_unsel_border;
