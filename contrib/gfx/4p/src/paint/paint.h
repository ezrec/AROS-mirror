/*
 *  paint.h
 *  4pX
 *
 *  Created by jerry on Sat Sep 01 2001.
 *  Copyright (c) 2001 __CompanyName__. All rights reserved.
 *
 */

#include "jsui.h"

/* controlling variables */
extern Uint8  video_bpp;
extern Uint32 video_flags;
extern Uint32 video_width;
extern Uint32 video_height;
extern Uint32 image_width;
extern Uint32 image_height;

/* the rubberband page overlays the primary, getting redrawn every frame */
extern IPAGE * page_rubberband;

/* the primary is the page that gets drawn */
extern IPAGE * page_primary;

/* the swap page is the page that is the backup page */
extern IPAGE * page_swap;

/* this points to either primary or swap */
extern IPAGE * page_active;

/* this pen gets changed in color every few clock ticks */
extern PEN * pen_cycle;

extern FONT_DEFINITION * current_font;

/* our event filter thingy */
void PaintFilterEvents(int e, int vx, int vy, int vb);


/* for setting up all of the paint related tasks... */
void paint_init( void );
void paint_deinit( void );
void paint_update( void );

/* messages for the mechanisms and such... */

#define PAINT_MSG_DOTTEDDRAW		(JSUI_MSG_USER + 1)
#define PAINT_MSG_CONTINUOUSDRAW	(JSUI_MSG_USER + 2)
#define PAINT_MSG_VECTORDRAW		(JSUI_MSG_USER + 3)
#define PAINT_MSG_CURVEDRAW		(JSUI_MSG_USER + 3 + 1024)
#define PAINT_MSG_FLOODFILL		(JSUI_MSG_USER + 4)
#define PAINT_MSG_AIRBRUSH		(JSUI_MSG_USER + 5)
#define PAINT_MSG_SETAIRBRUSH		(JSUI_MSG_USER + 5 + 1024)
#define PAINT_MSG_RECTANGLE		(JSUI_MSG_USER + 6)
#define PAINT_MSG_FILLEDRECTANGLE	(JSUI_MSG_USER + 7)
#define PAINT_MSG_CIRCLE		(JSUI_MSG_USER + 8)
#define PAINT_MSG_FILLEDCIRCLE		(JSUI_MSG_USER + 9)
#define PAINT_MSG_OVAL			(JSUI_MSG_USER + 10)
#define PAINT_MSG_FILLEDOVAL		(JSUI_MSG_USER + 11)
#define PAINT_MSG_POLYLINE		(JSUI_MSG_USER + 12)
#define PAINT_MSG_FILLEDPOLYLINE	(JSUI_MSG_USER + 13)
#define PAINT_MSG_TEXT			(JSUI_MSG_USER + 14)

#define PAINT_MSG_GRID			(JSUI_MSG_USER + 15)
#define PAINT_MSG_SETGRID		(JSUI_MSG_USER + 16)
#define PAINT_MSG_MIRROR		(JSUI_MSG_USER + 17)
#define PAINT_MSG_SETMIRROR		(JSUI_MSG_USER + 18)
#define PAINT_MSG_MAGNIFY		(JSUI_MSG_USER + 19)
#define PAINT_MSG_ZOOMIN		(JSUI_MSG_USER + 20)
#define PAINT_MSG_ZOOMOUT		(JSUI_MSG_USER + 21)

#define PAINT_MSG_SNAGBRUSH		(JSUI_MSG_USER + 22)
#define PAINT_MSG_OLDBRUSH		(JSUI_MSG_USER + 23)

#define PAINT_MSG_DOTDRAW		(JSUI_MSG_USER + 24)
#define PAINT_MSG_BRUSHBIGGER		(JSUI_MSG_USER + 25)
#define PAINT_MSG_BRUSHSMALLER		(JSUI_MSG_USER + 26)

#define PAINT_MSG_CIRCLEBRUSHSIZE	(JSUI_MSG_USER + 27)
#define PAINT_MSG_SQUAREBRUSHSIZE	(JSUI_MSG_USER + 28)
#define PAINT_MSG_BRUSH3		(JSUI_MSG_USER + 29)
#define PAINT_MSG_BRUSH5		(JSUI_MSG_USER + 30)

#define PAINT_MSG_BRUSHFLIPHORIZ	(JSUI_MSG_USER + 31)
#define PAINT_MSG_BRUSHFLIPVERT		(JSUI_MSG_USER + 32)
#define PAINT_MSG_BRUSHHALVE		(JSUI_MSG_USER + 33)
#define PAINT_MSG_BRUSHDOUBLE		(JSUI_MSG_USER + 34)
#define PAINT_MSG_BRUSHDOUBLEH		(JSUI_MSG_USER + 35)
#define PAINT_MSG_BRUSHDOUBLEV		(JSUI_MSG_USER + 36)

#define PAINT_MSG_UNDO			(JSUI_MSG_USER + 37)
#define PAINT_MSG_CLEARSCREEN		(JSUI_MSG_USER + 38)

#define PAINT_MSG_SPARESWAP		(JSUI_MSG_USER + 39)
#define PAINT_MSG_COPYTOSPARE		(JSUI_MSG_USER + 40)
#define PAINT_MSG_SAVEPAGE		(JSUI_MSG_USER + 41)
#define PAINT_MSG_LOADPAGE		(JSUI_MSG_USER + 42)

#define PAINT_MSG_EYEDROPPER		(JSUI_MSG_USER + 43)
#define PAINT_MSG_PALETTEEDIT		(JSUI_MSG_USER + 43 + 1024)

#define PAINT_MSG_FGPALETTEINC		(JSUI_MSG_USER + 44)
#define PAINT_MSG_FGPALETTEDEC		(JSUI_MSG_USER + 45)
#define PAINT_MSG_FGPALETTESET		(JSUI_MSG_USER + 46)

#define PAINT_MSG_BGPALETTEINC		(JSUI_MSG_USER + 47)
#define PAINT_MSG_BGPALETTEDEC		(JSUI_MSG_USER + 48)
#define PAINT_MSG_BGPALETTESET		(JSUI_MSG_USER + 49)

#define PAINT_MSG_TOGGLECYCLE		(JSUI_MSG_USER + 50)

#define PAINT_MSG_MODEMATTE		(JSUI_MSG_USER + 51)
#define PAINT_MSG_MODECOLOR		(JSUI_MSG_USER + 52)
#define PAINT_MSG_MODEREPLACE		(JSUI_MSG_USER + 53)
#define PAINT_MSG_MODESMEAR		(JSUI_MSG_USER + 54)
#define PAINT_MSG_MODESHADE		(JSUI_MSG_USER + 55)
#define PAINT_MSG_MODEBLEND		(JSUI_MSG_USER + 56)
#define PAINT_MSG_MODECYCLE		(JSUI_MSG_USER + 57)
#define PAINT_MSG_MODESMOOTH		(JSUI_MSG_USER + 58)

#define PAINT_MSG_TOGGLEMENU		(JSUI_MSG_USER + 59)
#define PAINT_MSG_TOGGLETOOLS		(JSUI_MSG_USER + 60)
#define PAINT_MSG_TOGGLECURSOR		(JSUI_MSG_USER + 61)
