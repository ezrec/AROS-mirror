#ifndef  DOPUSLIB_H
#define  DOPUSLIB_H

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

/*
 *
 * dopus.library header file
 *
 * Support library for Directory Opus version 3.50+
 *
 * (c) Copyright 1992 Jonathan Potter
 *
 */

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif

#include <graphics/gfx.h>

/* Library base structure */

struct DOpusBase {
    struct Library LibNode;
    UBYTE Flags;
    UBYTE pad;
    struct ExecBase *ExecBase;
    struct DosLibrary *DOSBase;
    struct IntuitionBase *IntuitionBase;
    struct GfxBase *GfxBase;
    struct LayersBase *LayersBase;
    /* Private stuff */
    PLANEPTR pdb_cycletop;
    PLANEPTR pdb_cyclebot;
    PLANEPTR pdb_check;
    ULONG pad1;
    ULONG pad2;
    ULONG pad3;
    ULONG pad4;
    ULONG pdb_Flags;
    BPTR SegList;
};

/* File requester definition structure */

#define DIRBUF_SIZE  256
#define FILEBUF_SIZE 108 // 32

struct DOpusFileReq {
    char *title;            /* Requester title or NULL for default    */
    char *dirbuf,*filebuf;  /* Buffers for directory and filenames    */
    struct Window *window;  /* Window in screen to appear on          */
    int x,y;                /* x & y position of requester window     */
    int lines;              /* Number of files to display at once     */
    ULONG flags;            /* Flags; see below                       */
    struct DOpusRemember
        *filearraykey;        /* Remember structure for multiple files  */
    char **filearray;       /* Array of multiple files (DFRF_MULTI)   */
};

/* Requester flags */

#define DFRB_DIRREQ 0 /* Directory requester - no files        */
#define DFRB_MULTI  1 /* Can select more than one file at once */
#define DFRB_SAVE   2 /* Save requester                        */
#define DFRB_FONT   3 /* Font requester                        */

#define DFRF_DIRREQ (1<<DFRB_DIRREQ)
#define DFRF_MULTI  (1<<DFRB_MULTI)
#define DFRF_SAVE   (1<<DFRB_SAVE)
#define DFRF_FONT   (1<<DFRB_FONT)

/* return values of FileRequest() */

#define RETURN_OKAY    1
#define RETURN_CANCEL  0
#define ERROR_NOMEM   -1

/* return values of CompareLock() */

#define LOCK_DIFFERENT   -1
#define LOCK_SAME         0
#define LOCK_SAME_VOLUME  1

/* return values of Assign() */

#define ASSIGN_OK     0
#define ASSIGN_NODEV  1
#define ASSIGN_NOMEM  2
#define ASSIGN_CANCEL 3

/* DOpusDateTime structure for StampToStr() and StrToStamp() */

struct DOpusDateTime {
    struct DateStamp dat_Stamp; /* DOS Datestamp               */
    UBYTE dat_Format;           /* Sets the date string format */
    UBYTE dat_Flags;            /* Flags; see below            */
    char *dat_StrDay;           /* day of the week string      */
    char *dat_StrDate;          /* date string                 */
    char *dat_StrTime;          /* time string                 */
//    char **custom_weekdays;     /* array of weekday names      */
//    char **custom_shortweekdays;/* array of short weekday names*/
//    char **custom_months;       /* array of month names        */
//    char **custom_shortmonths;  /* array of short month names  */
//    char **custom_special_days; /* array of special days       */
};

#define DDTB_SUBST  0 /* Substitute "Today","Tomorrow" where appropriate */
#define DDTB_FUTURE 1 /* "Monday", etc. refers to next not last day      */

#define DDTB_CUSTOM 6 /* Custom strings */
#define DDTB_12HOUR 7 /* 12-hour time */

#define DDTF_SUBST  (1<<DDTB_SUBST)
#define DDTF_FUTURE (1<<DDTB_FUTURE)

#define DDTF_CUSTOM (1<<DDTB_CUSTOM)
#define DDTF_12HOUR (1<<DDTB_12HOUR)

#define FORMAT_DOS 0 /* dd-mmm-yy */
#define FORMAT_INT 1 /* yy-mm-dd  */
#define FORMAT_USA 2 /* mm-dd-yy  */
#define FORMAT_CDN 3 /* dd-mm-yy  */
#define FORMAT_AUS FORMAT_CDN

/* ListView definition structure */

struct DOpusListView {
    ULONG listid;          /* Unique ID number                           */
    struct Window *window; /* Window to add the ListView to              */
    int x,y;               /* x & y position of ListView                 */
    int w,h;               /* Width and height of ListView               */
    char *title;           /* Title of ListView                          */
    char **items;          /* Null-terminated array of items to display  */
    char *selectarray;     /* Table of selection status used with        */
                           /*   DLVB_MULTI. See docs for more info.      */
    ULONG flags;           /* Flags; see below                           */
    int sliderwidth;       /* Width of slider                            */
    int slidercol;         /* Pen colour of slider, -1 for default       */
    int sliderbgcol;       /* Pen colour of slider background            */
    int textcol;           /* Pen colour of general text, -1 for default */
    int boxhi,boxlo;       /* Pen colours of 3D box, -1 for defaults     */
    int arrowfg,arrowbg;   /* Arrow fg/bg colours, -1 for defaults       */
    int itemfg,itembg;     /* Item fg/bg colours, -1 for defaults        */
    int topitem;           /* First item displayed                       */
    int itemselected;      /* Item just clicked on                       */
    struct DOpusListView
        *next;               /* Next ListView to add to window or NULL     */

    /* Private stuff */

    int oldoffset,count,lines,columns;
    struct DOpusRemember *key;
    struct Gadget *listgads;
    struct Image *listimage;
    struct PropInfo *listprop;
    struct Region *listregion;
    int gadadd,fw,fh,fb,xo,yo,mx,my,chk,ty;
    char ofg,obg,odm;
    char *selected;
};

/* ListView flags */

#define DLVB_MULTI  0 /* Can multi-select items                              */
#define DLVB_LEAVE  1 /* Leave item selected                                 */
#define DLVB_CHECK  2 /* Select with check marks, used with MULTI and LEAVE  */
#define DLVB_TOGGLE 3 /* Used with LEAVE to toggle item on/off               */
#define DLVB_SLOW   4 /* Specify slow scrolling                              */
#define DLVB_DUMB   5 /* Dumb list; display only - no mouse selection        */
#define DLVB_TTOP   6 /* Title at the top of the list (default to the left)  */
#define DLVB_ENDNL  7 /* End list with a null-string as well as NULL pointer */
#define DLVB_HIREC  8 /* Highlight by recessed borders                       */

#define DLVF_MULTI  (1<<DLVB_MULTI)
#define DLVF_LEAVE  (1<<DLVB_LEAVE)
#define DLVF_CHECK  (1<<DLVB_CHECK)
#define DLVF_TOGGLE (1<<DLVB_TOGGLE)
#define DLVF_SLOW   (1<<DLVB_SLOW)
#define DLVF_DUMB   (1<<DLVB_DUMB)
#define DLVF_TTOP   (1<<DLVB_TTOP)
#define DLVF_ENDNL  (1<<DLVB_ENDNL)
#define DLVF_HIREC  (1<<DLVB_HIREC)

/* DOpusListView->selectarray flags */

#define LVARRAY_SELECTED     1   /* Item is selected */
#define LVARRAY_DISABLED     2   /* Item is disabled */

/* DOpusRemember structure, used by LAllocRemember() and LFreeRemember() */

struct DOpusRemember {
    struct DOpusRemember *next,*current;
    ULONG size;
};

/* Right mouse button gadget text */

struct RMBGadgetText {
    char *text;                    /* Text string                      */
    int x,y;                       /* x and y coordinates of text      */
    int w,h;                       /* Width and Height of text area    */
    int fg,bg;                     /* Fg and Bg pens                   */
};

/* Right mouse button gadget structure, used by RMBGadget */

struct RMBGadget {
    struct RMBGadget *next;        /* Next RMBGadget                 */
    int x,y;                       /* x and y coordinates            */
    int w,h;                       /* Width and Height               */
    int flags;                     /* Flags; see below               */
    int id;                        /* ID number of this RMBGadget    */
    struct RMBGadgetText txt[2];   /* Alternate gadget text displays */
    struct Border *high_bord[2];   /* Borders to highlight with      */
};

#define RGB_ALTTEXT 0  /* This gadget has alternate text displays */
#define RGB_ALTBORD 1  /* Highlight with borders */

#define RGF_ALTTEXT (1<<RGB_ALTTEXT)
#define RGF_ALTBORD (1<<RGB_ALTBORD)

/* Defines used for AddGadgets */

#define GAD_NONE  -1
#define GAD_CYCLE 1
#define GAD_CHECK 2
#define GAD_RADIO 3
#define GAD_GLASS 4

/* SimpleRequest structure */

struct DOpusSimpleRequest {
    char *text;                 /* Text to display            */
    char **gads;                /* Gadget names               */
    int *rets;                  /* Gadget return values       */
    int hi,lo;                  /* 3D hi and lo pen colours   */
    int fg,bg;                  /* Text Fg and Bg pen colours */
    char *strbuf;               /* Buffer for string gadget   */
    int strlen;                 /* Maximum string length      */
    int flags;                  /* Flags, see below           */
    struct TextFont *font;      /* Font to use, must be open  */
    long value;                 /* Generic data               */
    char *title;                /* Title for req with borders */
};

#define SRB_LONGINT   0  /* Integer gadget                          */
#define SRB_CENTJUST  1  /* Center justify string (left is default) */
#define SRB_RIGHTJUST 2  /* Right justify string (left is default)  */
#define SRB_GLASS     3  /* Glass gadget                            */
#define SRB_DIRGLASS  4  /* Glass gadget is for directory requester */
#define SRB_BORDERS   5  /* Requester borders                       */
#define SRB_NOCENTER  6  /* Don't center text in requester          */
#define SRB_RECESSHI  7  /* Highlight gadgets by recessing          */
#define SRB_EXTEND    8  /* StringExtend passed in value            */

#define SRF_LONGINT   (1<<SRB_LONGINT)
#define SRF_CENTJUST  (1<<SRB_CENTJUST)
#define SRF_RIGHTJUST (1<<SRB_RIGHTJUST)
#define SRF_GLASS     (1<<SRB_GLASS)
#define SRF_DIRGLASS  (1<<SRB_DIRGLASS)
#define SRF_BORDERS   (1<<SRB_BORDERS)
#define SRF_NOCENTER  (1<<SRB_NOCENTER)
#define SRF_RECESSHI  (1<<SRB_RECESSHI)
#define SRF_EXTEND    (1<<SRB_EXTEND)

#define SYSFILE_COMMAND    0
#define SYSFILE_DATA       1
#define SYSFILE_LIBRARY    2
#define SYSFILE_REXX       3
#define SYSFILE_MODULE     4
#define SYSFILE_REQUESTER  5

#define ERROR_NOT_CONFIG -1

#endif /* DOPUSLIB_H */

