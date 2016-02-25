/****************************************************************************
*                                                                           *
* parts.h -- Lunapaint,                                                     *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#ifndef _parts_h_
#define _parts_h_

#define LNTAGBASE (TAG_USER+100)

#define MUIM_Luna_ZoomIn                        (LNTAGBASE+0)
#define MUIM_Luna_ZoomOut                       (LNTAGBASE+1)
#define MUIM_Luna_ShowAll                       (LNTAGBASE+2)
#define MUIM_Luna_SetTool_Draw                  (LNTAGBASE+3)
#define MUIM_Luna_SetTool_Fill                  (LNTAGBASE+4)
#define MUIM_Luna_SetTool_Line                  (LNTAGBASE+5)
#define MUIM_Luna_SetTool_Rectangle             (LNTAGBASE+6)
#define MUIM_Luna_SetTool_ClipBrush             (LNTAGBASE+7)
#define MUIM_Luna_SetTool_Circle                (LNTAGBASE+8)
#define MUIM_Luna_SetTool_Colorpicker           (LNTAGBASE+9)
#define MUIM_Luna_ExecuteRevert                 (LNTAGBASE+10)
#define MUIM_Luna_CanvasDeactivate              (LNTAGBASE+11)
#define MUIM_Luna_CanvasActivate                (LNTAGBASE+12)
#define MUIM_Luna_NewProject                    (LNTAGBASE+13)
#define MUIM_Luna_CloseCanvasWin                (LNTAGBASE+14)

// how much time defines a click within a doubleclick
#define MOUSE_DoubleClickTime   5

#include <stdio.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include <exec/types.h>
#include <libraries/asl.h>
#include <libraries/mui.h>

#include <dos/filehandler.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <cybergraphx/cybergraphics.h>
#include <string.h>

#include "config.h"
#include "core/definitions.h"

extern BOOL keyboardEnabled;

/*
    For the pubscreen
*/
extern struct NewScreen *lunaScreen;
extern struct Screen *lunaPubScreen;

struct RectStruct
{
    int x;
    int y;
    int w;
    int h;
} RectStruct;

/*
    A linked list consisting of windowID and a window object
*/
struct WindowList
{
    Object          *win;           // The MUI window

    Object          *projectWin;    // Project window
    Object          *projBtnOk;     // Project ok button
    Object          *projName;      // Project name
    Object          *projAuthor;    // Project author
    Object          *projDesc;      // Project description
    struct Hook     projHook;       // Function hook to change the proj info
    char            *filename;      // The filename

    int             rRectX;         // RedrawRect X
    int             rRectY;         // RedrawRect Y
    int             rRectW;         // RedrawRect W
    int             rRectH;         // RedrawRect H
    unsigned int    contWidth;      // Known container width
    unsigned int    contHeight;     // Known container height

    Object          *infocontainer; // Containing frame info etc
    Object          *container;     // Containing the area
    Object          *area;          // MUI area data
    Object          *scrollgrp;     // Scrollgroup
    Object          *scrollH;       // Horizontal scrollbar
    Object          *scrollV;       // Vertical scrollbar
    Object          *mouse;         // Contains the mouse object
    Object          *btnZoom;       // The zoom button
    Object          *btnShowAll;    // The whole image
    Object          *btnUnZoom;     // The zoom out button
    Object          *txtFrameInf;   // Info of which frame you're on
    Object          *txtLayerInf;   // Info on which layer you're on
    Object          *txtCoordX;     // X coordinates
    Object          *txtCoordY;     // Y coordinates
    Object          *txtZoomLevel;  // How much zoom
    struct oCanvas  *canvas;        // Pixel buffer
    unsigned int    id;             // Window ID
    BOOL            isActive;       // is the window active or not?
    BOOL            isVisible;      // If it is supposed to be visible
    BOOL            layersChg;      // Has the layercount changed?
    struct RectStruct prevBlit;     // previous blitted area
    struct Hook     CanvasKey_hook; // Hook for input keys
    struct WindowList *nextwin;     // Next window in list
} WindowList;

/*
    The data struct for the custom area class
*/
struct RGBitmapData
{
    int             zuneAreaLeft;   // Offset relative to window
    int             zuneAreaTop;    // --||--
    int             rgbAreaLeft;    // Offset relative to draw rect
    int             rgbAreaTop;     //
    int             currentzoom;    // Current zoom level
    int             zuneAreaWidth;  // Zune area width
    int             zuneAreaHeight; // Zune area height
    int             rgbAreaWidth;   // current width of paint area
    int             rgbAreaHeight;  // current height of paint area
    unsigned int    scrollPosH;     // Remembered scroll pos
    unsigned int    scrollPosV;     // Remembered scroll pos
    unsigned int    scrollEntriesH; // Remembered scroll pos
    unsigned int    scrollEntriesV; // Remembered scroll pos
    BOOL            mousepressed;   // If the mouse is pressed
    BOOL            isBusy;         // If the canvas is heavily busy on something
    struct WindowList *window;      // A pointer to the window
    struct RastPort *rp;            // Rastport to draw on
};

extern struct FileRequester *aslfileReq;

extern int mouseClickCount; // registers clicks and doubleclicks etc


/*
    Remember ALWAYS to free a filename gotten from getFilename()
*/
char *getFilename ( );

/*
    Get the filesize
*/
unsigned int getFilesize ( char *filename );

#endif
