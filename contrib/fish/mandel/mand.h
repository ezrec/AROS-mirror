/*
                    MAND.H -- The Constants
             Mandelbrot Self-Squared Dragon Generator
                    For the Commodore Amiga
                         Version 1.00

                      Accompanies MAND.C

             Copyright (c) 1985, Robert S. French
                  Placed in the Public Domain

     Assorted Goodies and Intuition-stuff by =RJ Mical=  1985

This program may be distributed free of charge as long as the above
notice is retained.

*/


/*-------------------*/
/* Lots of includes! */

#include <aros/oldprograms.h>

#define USE_MATHFFP 0

#if USE_MATHFFP

#define KLUDGE_INT int

#else

#undef KLUDGE_INT
#define KLUDGE_INT float

#undef SPDiv
#define SPDiv(a,b) ((b) / (a))

#undef SPFlt
#define SPFlt(a)   ((float)(a))

#undef SPSub
#define SPSub(a,b) ((b) - (a))

#undef SPAdd
#define SPAdd(a,b) ((a) + (b))

#undef SPMul
#define SPMul(a,b) ((a) * (b))

#undef SPNeg
#define SPNeg(a)   (-(a))

#undef SPFix
#define SPFix(a)   ((LONG)(a))

#undef SPFieee
#define SPFieee(a) (a)

#endif

#include <exec/types.h>
#include <exec/tasks.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <devices/keymap.h>
#include <graphics/copper.h>
/*
  #include <graphics/display.h>
*/
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/view.h>
#include <graphics/gels.h>
#include <graphics/regions.h>
#include <hardware/blit.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <stdio.h>
#include <ctype.h>


/*-------------------*/
/* Misc. definitions */
#define VERSION "2.00"

#define F_INTUITION 0x000001
#define F_GRAPHICS  0x000002
#define F_MATH      0x000004
#define F_MATHTRANS 0x000008
#define F_CONSOLE   0x000010
#define F_COLORTAB  0x000020
#define F_SETSTORE  0x000040
#define F_DOS       0x000080

#define EVER ;;
#define MAXX 640
#define MAXY 400
#define MAXMY 100
#define STARTY 0

/*---------------------------------*/
/* Fast-Floating-Point Definitions */

/*
extern   int     SPFix();
extern   int     SPFlt();
extern   int     SPCmp();
extern   int     SPTst();
extern   int     SPAbs();
extern   int     SPNeg();
extern   int     SPAdd();
extern   int     SPSub();
extern   int     SPMul();
extern   int     SPDiv();
 
extern   int     SPAtan();
extern   int     SPSin();
extern   int     SPCos();
extern   int     SPTan();
extern   int     SPSincos();
extern   int     SPSinh();
extern   int     SPCosh();
extern   int     SPTanh();
extern   int     SPExp();
extern   int     SPLog();
extern   int     SPPow();
extern   int     SPSqrt();
extern   int     SPTieee();
extern   int     SPFieee();
*/

#define TOPW2 w2->BorderTop
#define BOTTOMW2 (w2->Height - w2->BorderBottom)
#define LEFTW2 w2->BorderLeft
#define RIGHTW2 (w2->Width - w2->BorderRight)


/* === these definitions are for the color_mode variable ================== */
#define NOT_HOLDANDMODIFY	0x0001
#define INTERLACE_MODE		0x0002
#define HIRES_MODE		0x0004

/* === these definitions are for the menu strips ========================== */#define ITEM_HEIGHT 10
#define MENU_OPTIONS 0	/* first menu */
#define MENU_ZOOM 1     /* next menu */
#define MENU_COUNT 2

/* for the OPTIONS Menu ... */
#define OPTIONS_WIDTH   176
#define OPTIONS_STOP	0
#define OPTIONS_QUARTER 1
#define OPTIONS_FULL    2
#define OPTIONS_GENERATE 3
#define OPTIONS_CLOSE	4
#define OPTIONS_COUNT   5

/* for the ZOOM Menu ... */
#define ZOOM_WIDTH   174
#define ZOOM_SETCENTER    0
#define ZOOM_SIZEBOX      1
#define ZOOM_ZOOMIN       2
#define ZOOM_ZOOMIN10     3
#define ZOOM_ZOOMOUT2     4
#define ZOOM_ZOOMOUT10    5
#define ZOOM_COUNT        6

void ZoomAlongDarling(KLUDGE_INT rzoom, KLUDGE_INT izoom);
