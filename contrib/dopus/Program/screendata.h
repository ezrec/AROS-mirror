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

#ifndef DOPUS_SCREEN_DATA
#define DOPUS_SCREEN_DATA

/* Definitions of any data that relates to the Screen display. Includes
   TextAttrs, NewScreen structures and TagItems, and various global data */

#include "dopus.h"

/* TextAttr structures */

extern struct TextAttr
	main_screen_attr,                              /* Main screen font */
	screen_attr,                                   /* General screen font */
	menu_attr;                                     /* Menu font */

/* Screen tag lists */

extern UWORD scr_drawinfo[NUMDRIPENS+1];         /* Pen lists */

extern struct TagItem
	mainscreen_tags[],                             /* Tags for the main screen */
	stdscreen_tags[],                              /* Tags for a standard HIRES screen */
	mainwindow_tags[];                             /* Tags for the main window */

/* NewScreen structures */

extern struct ExtNewScreen
	main_scr,                                      /* Main screen */
	font_scr,                                      /* Font screen */
	blank_scr;                                     /* Blank screen */

/* NewWindow structures */

extern struct ExtNewWindow
	main_win;                                      /* Main window */

extern struct NewWindow
	font_win,                                      /* Font window */
	disk_win;                                      /* DiskInfo window */

extern struct NewWindow icon_win;                /* Iconified window */

extern struct AppWindow *dopus_appwindow;        /* AppWindow when on Workbench screen */

/* Definitions for the custom menus */

extern struct Menu menu_menus[5];                /* Custom menus */
extern struct MenuItem  *menu_menuitem;          /* Custom menu items */
extern struct IntuiText *menu_intuitext;         /* IntuiText for custom menu items */

extern char *menu_itemnames;                     /* Names of the menu items */
extern char menu_spaceitemnames[5][80];          /* Space background of menu items */

extern UBYTE menu_real_number[5];                /* The real ordinal number of each menu */

/* Internal Screen Data */

extern struct ColourTable screen_pens[16];       /* Colour pen table */

extern USHORT scrdata_norm_width;                /* Normal width of the display */
extern USHORT scrdata_norm_height;               /* Normal height of the display */
extern USHORT scrdata_scr_height;                /* Main screen/window height */

extern USHORT scrdata_dispwin_lines;             /* Number of lines in each window */
extern USHORT scrdata_dispwin_width[2];          /* Displayed width in each window */
extern USHORT scrdata_dispwin_height;            /* Height of directory windows */
extern USHORT scrdata_dispwin_nchars[2];         /* Width of each window in characters */
extern USHORT scrdata_dispwin_ypos;              /* Y-Position of directory windows */
extern USHORT scrdata_dispwin_left[2];           /* Left edge of directory windows */
extern USHORT scrdata_dispwin_xpos[2];           /* Left edge of text in windows */
extern USHORT scrdata_dispwin_center;            /* Center of the two windows */

extern USHORT scrdata_dirwin_xpos[2];            /* Rendered X-Position of each window */
extern USHORT scrdata_dirwin_ypos[2];            /* Rendered Y-Position of each window */
extern USHORT scrdata_dirwin_width[2];           /* Rendered width of each window */
extern USHORT scrdata_dirwin_height;             /* Rendered height of directory windows */
extern USHORT scrdata_dirwin_bottom;             /* Rendered bottom of each window */
extern USHORT scrdata_dirwin_center;             /* Rendered center of the two windows */

extern USHORT scrdata_drive_width;               /* Width of the drive gadgets */
extern USHORT scrdata_gadget_offset;             /* X-Offset of entire gadget display */
extern USHORT scrdata_gadget_xpos;               /* X-Position of the custom gadgets */
extern USHORT scrdata_gadget_ypos;               /* Y-Position of the custom gadgets */
extern USHORT scrdata_gadget_width;              /* Width of the custom gadgets */
extern USHORT scrdata_gadget_height;             /* Height of the custom gadgets */

extern USHORT scrdata_clock_width;               /* Width of the clock bar */
extern USHORT scrdata_clock_height;              /* Height of the clock bar */
extern USHORT scrdata_clock_xpos;                /* x-pos of the clock bar */
extern USHORT scrdata_clock_ypos;                /* y-pos of the clock bar */
extern USHORT scrdata_status_xpos;               /* X-Position of the status bar */
extern USHORT scrdata_status_ypos;               /* Y-Position of the status bar */
extern USHORT scrdata_status_width;              /* Width of the status bar */
extern USHORT scrdata_status_height;             /* Height of the status bar */
extern USHORT scrdata_diskname_width[2];         /* Width of each diskname display */
extern USHORT scrdata_diskname_height;           /* Height of the diskname displays */
extern USHORT scrdata_diskname_ypos;             /* Y-Position of the diskname displays */
extern USHORT scrdata_diskname_xpos[2];          /* X-Position of each diskname display */
extern USHORT scrdata_prop_height;               /* Height of the vertical sliders */
extern USHORT scrdata_string_height;             /* Height of the path string gadgets */
extern USHORT scrdata_closegad_width;            /* Width of the screen close gadget */

extern USHORT scrdata_font_xsize;                /* X-Size of the dir window font */
extern USHORT scrdata_font_ysize;                /* Y-Size of the dir window font */
extern USHORT scrdata_font_baseline;             /* Baseline of the dir window font */

extern UBYTE scrdata_is_pal;                     /* Indicates PAL or NTSC system */
extern UBYTE scrdata_statustext_pos;             /* Status text positioning */
extern LONG scrdata_old_offset_store;            /* Stored offset in directory window */

extern USHORT scrdata_xoffset;                   /* General x-offset */
extern USHORT scrdata_yoffset;                   /* General y-offset */
extern USHORT scrdata_width;                     /* Display width */
extern USHORT scrdata_height;                    /* Display height */

/* Definitions of screen objects */

extern struct PropInfo
	vert_propinfo[2],                              /* Vertical slider PropInfo */
	horiz_propinfo[2],                             /* Horizontal slider PropInfo */
	drive_propinfo,                                /* Drive banks slider PropInfo */
	gadget_propinfo;                               /* Gadget banks slider PropInfo */

extern struct Image
	vert_propimage[2],                             /* Vertical slider Image */
	horiz_propimage[2],                            /* Horizontal slider Image */
	drive_propimage,                               /* Drive banks slider Image */
	gadget_propimage;                              /* Gadget banks slider Image */

extern struct StringExtend string_extend;        /* Path fields StringExtension */
extern struct StringExtend requester_stringex;   /* Path fields StringExtension */

extern char str_pathbuffer[2][256];              /* Path fields buffers */
extern char str_undobuffer[256];                 /* General undo buffer for string gads */

extern struct StringInfo                         /* Path fields StringInfos */
	path_stringinfo[2];

extern struct Gadget
	*main_gadgets,                                 /* Custom gadgets */
	*drive_gadgets,                                /* Drive gadgets */
	path_strgadget[2],                             /* Path field gadgets */
	vert_propgad[2],                               /* Vertical slider gadgets */
	horiz_propgad[2],                              /* Horizontal slider gadgets */
	drive_propgad,                                 /* Drive banks slider gadget */
	gadget_propgad,                                /* Gadget banks slider gadget */
	screen_gadgets[24];                            /* General screen gadgets */

extern struct Gadget size_gadgets[];

#define SCRGAD_LEFTSTRING       0
#define SCRGAD_RIGHTSTRING      1
#define SCRGAD_LEFTPARENT       0
#define SCRGAD_RIGHTPARENT      1

#define SCRGAD_TINYBUFFERLIST   2
#define SCRGAD_TINYRESELECT     3
#define SCRGAD_TINYSELECT       4
#define SCRGAD_TINYAREXX        5

#define SCRGAD_TINYHELP         6
#define SCRGAD_TINYERRORHELP    7
#define SCRGAD_TINYFILTER       8
#define SCRGAD_TINYCONFIGURE    9
#define SCRGAD_TINYICONIFY     10
#define SCRGAD_TINYQUIT        11

#define SCRGAD_MOVELEFT1       12
#define SCRGAD_MOVELEFT2       13
#define SCRGAD_MOVERIGHT1      14
#define SCRGAD_MOVERIGHT2      15
#define SCRGAD_MOVEUP1         16
#define SCRGAD_MOVEUP2         17
#define SCRGAD_MOVEDOWN1       18
#define SCRGAD_MOVEDOWN2       19
#define SCRGAD_BUFFERLEFT1     20
#define SCRGAD_BUFFERLEFT2     21
#define SCRGAD_BUFFERRIGHT1    22
#define SCRGAD_BUFFERRIGHT2    23

#define SCRGAD_LEFTPROP        30
#define SCRGAD_RIGHTPROP       31
#define SCRGAD_LEFTHPROP       32
#define SCRGAD_RIGHTHPROP      33
#define SCRGAD_DRIVEPROP       34
#define SCRGAD_GADGETPROP      35

extern struct Gadget iconbutpropgad;             /* Button iconify slider */

extern struct RMBGadget
	mainrmbgads[7];                                /* Screen right mouse button gadgets */


/* Click-M-Click dragging data */

extern struct VSprite drag_sprite;               /* VSprite used for dragging */
extern struct Bob drag_bob;                      /* BOB used for dragging */

extern struct BitMap drag_bob_bitmap;            /* Bitmap used to render BOB */
extern struct RastPort drag_bob_rastport;        /* Rastport used to render BOB */

extern PLANEPTR drag_bob_buffer;                 /* BOB buffer */
extern PLANEPTR drag_bob_savebuffer;             /* BOB background save buffer */

extern struct GelsInfo drag_gelsinfo;            /* GelsInfo structure for BOB */
extern struct VSprite drag_dummy_sprite1;        /* Dummy sprite 1 for GELs list */
extern struct VSprite drag_dummy_sprite2;        /* Dummy sprite 2 for GELs list */

/* General screen data */

extern struct Rectangle scr_scroll_borders[2];   /* Scroll borders for dir windows */

/* Borders for gadget highlighting */

extern struct DOpusRemember *border_key;         /* Memory key for border data */

extern struct Border *gadget_sel_border;         /* Borders for main gadgets */
extern struct Border *gadget_unsel_border;

extern struct Border *gadget_dog_sel_border;     /* Main gadgets with dog-ears*/
extern struct Border *gadget_dog_unsel_border;

extern struct Border *drive_sel_border;          /* Borders for drive gadgets */
extern struct Border *drive_unsel_border;

extern struct Border *drive_bank_sel_border;     /* Borders for the drive bank */
extern struct Border *drive_bank_unsel_border;

#define MAIN_GAD_BASE   1000                     /* GadgetID base of custom gadgets */

#endif
