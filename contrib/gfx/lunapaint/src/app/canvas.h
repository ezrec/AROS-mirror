/****************************************************************************
*                                                                           *
* canvas.h -- Lunapaint,                                                    *
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

#ifndef _aros_canvas_h_
#define _aros_canvas_h_

#define LNCATAGBASE (TAG_USER+300)

#define MUIM_Luna_Canvas_ClearActiveCanvas  (LNCATAGBASE+0)
#define MUIM_Luna_Canvas_SetPaintMode       (LNCATAGBASE+1)
#define MUIM_Luna_Canvas_AlterBrushShape    (LNCATAGBASE+2)
#define MUIM_Luna_Canvas_AlterBrushStep     (LNCATAGBASE+3)
#define MUIM_Luna_Canvas_AlterBrushStrength (LNCATAGBASE+4)
#define MUIM_Luna_Canvas_CanvasAddLayer     (LNCATAGBASE+5)
#define MUIM_Luna_Canvas_CanvasSwapLayer    (LNCATAGBASE+6)
#define MUIM_Luna_Canvas_CanvasDeleteLayer  (LNCATAGBASE+7)
#define MUIM_Luna_Canvas_CanvasMergeLayer   (LNCATAGBASE+8)
#define MUIM_Luna_Canvas_CanvasCopyLayer    (LNCATAGBASE+9)
#define MUIM_Luna_Canvas_ChangeOnionskin    (LNCATAGBASE+10)
#define MUIM_Luna_Canvas_ChangeScrollOffset (LNCATAGBASE+11)
#define MUIM_Luna_Canvas_SetGlobalGrid      (LNCATAGBASE+12)
#define MUIM_Luna_Canvas_SetGridSize        (LNCATAGBASE+13)
#define MUIM_Luna_Canvas_SetColorMode       (LNCATAGBASE+14)
#define MUIM_Luna_Canvas_NextFrame          (LNCATAGBASE+15)
#define MUIM_Luna_Canvas_PrevFrame          (LNCATAGBASE+16)
#define MUIM_Luna_Canvas_GotoFrame          (LNCATAGBASE+17)
#define MUIM_Luna_Canvas_EffectOffset       (LNCATAGBASE+18)
#define MUIM_Luna_Canvas_OpenPaletteEditor  (LNCATAGBASE+19)
#define MUIM_Luna_Canvas_ScrollingNotify    (LNCATAGBASE+20)
#define MUIM_Luna_Canvas_RedrawArea         (LNCATAGBASE+21)
#define MUIM_Luna_Canvas_Redraw             (LNCATAGBASE+22)

// Paint tool keys
#define MUIA_Key_F          1001
#define MUIA_Key_D          1002
#define MUIA_Key_V          1003
#define MUIA_Key_L          1004
#define MUIA_Key_E          1005
#define MUIA_Key_R          1006
#define MUIA_Key_O          1007
#define MUIA_Key_B          1008
// Buffer keys
#define MUIA_Key_J          1050
#define MUIA_Key_Shift_J    1051
#define MUIA_Key_X          1052
#define MUIA_Key_Y          1053
#define MUIA_Key_Up         1054
#define MUIA_Key_Left       1055
#define MUIA_Key_Right      1056
#define MUIA_Key_Down       1057
// Effect keys
#define MUIA_Key_F1         1100
#define MUIA_Key_F2         1101
#define MUIA_Key_F3         1102
#define MUIA_Key_F4         1103
#define MUIA_Key_F5         1104
#define MUIA_Key_F6         1105
#define MUIA_Key_F7         1106
#define MUIA_Key_F8         1107
#define MUIA_Key_F9         1108
// Color keys
#define MUIA_Key_Shift_Comma    1200
#define MUIA_Key_Comma          1201

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <datatypes/datatypes.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <cybergraphx/cybergraphics.h>

#include <math.h>
#include <devices/rawkeycodes.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <clib/alib_protos.h>
#include <string.h>

#ifdef __AROS__
#define DEBUG 1
#include <aros/debug.h>
#endif

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include "parts.h"
#include "toolbox.h"
#include "layers.h"
#include "project.h"

#include "config.h"

#include "core/canvas.h"
#include "core/tools.h"
#include "math/math.h"
#include "common.h"

extern BOOL isZooming;
extern BOOL isScrolling;
extern BOOL fullscreenEditing;
extern int AskMinMaxTimes;

/*
    For editing in fullscreen mode
*/
extern Object *windowFullscreen;
extern Object *fullscreenGroup;

/*
    For our dispatcher:
*/
IPTR MUIM_RGB_Draw ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_Redraw ( );
IPTR MUIM_RGB_RedrawArea ( );
IPTR MUIM_RGB_HandleInput ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_Setup ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_Cleanup ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_ScrollingNotify ( );
IPTR MUIM_RGB_CanvasActivate ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_CanvasDeactivate ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_ZoomIn ( );
IPTR MUIM_RGB_ZoomOut ( );
IPTR MUIM_RGB_ShowAll ( );
IPTR MUIM_RGB_AskMinMax ( Class *CLASS, Object *self, Msg message );
IPTR MUIM_RGB_CloseCanvasWin ( Class *CLASS, Object *self, Msg message );

/*
    Setup keyboard shortcuts on canvas and toolbox
*/
void checkKeyboardShortcuts ( UWORD stri );

/*
    Move canvas
*/
void moveScrollbarUp ( );
void moveScrollbarDown ( );
void moveScrollbarLeft ( );
void moveScrollbarRight ( );

/*
    Redraw the area object with canvas data
    Private function!!
*/
IPTR _RGBitmapRedraw ( Class *CLASS, Object *self );

/*
    Update Frame: 1/1 etc
*/
void UpdateCanvasInfo ( struct WindowList *win );

/*
    Scroll the active canvas
*/
IPTR ScrollCanvas ( int x, int y );

/*
    Snap offset coords to zoom (globalactivecanvas)
*/
void SnapOffsetToZoom ( struct oCanvas *canv );

/*
    Check if active window has changed and
    redraw if it has
*/
void winHasChanged ( );

/*
    Store mouse coordinates into the global variables
*/
void getMouseCoordinates ( );

/*
    This function adds a canvas window to the canvas window list
    ( canwinlist )
*/
void addCanvaswindow (
    unsigned int width, unsigned int height,
    unsigned int layers, unsigned int frames,
    BOOL generateCanvas
);

/*
    Show/Hide the fullscreen editing window
*/
void showFullscreenWindow ( struct oCanvas *canvas );
void hideFullscreenWindow ( );

/*
    Contstrain offset values within scope of canvas
*/
void constrainOffset ( struct oCanvas *canvas );

/*
    Test if we're hitting "walls"
*/
void snapToBounds ( int *x, int *y );

/*
    This function sends back a pointer to a window object
*/
Object *getCanvaswindowById ( unsigned int id );

/*
    This function sends back a pointer to the window struct
*/
struct WindowList *getCanvasDataById ( unsigned int id );

/*
    Import an image rawly! :-D
*/
void importImageRAW ( unsigned int w, unsigned int h, unsigned long long int *buffer );

/*
    Load an image through datatypes
*/
BOOL loadDatatypeImage ( );

/*
    Free memory
*/
void deleteCanvaswindows ( );

/*
    Remove a single canvas window and it's structure
*/
IPTR removeActiveWindow ( Class *CLASS, Object *self );

/*
    Blit over a rect to the area from a canvas
*/
void blitAreaRect (
    int x, int y, int w, int h,
    struct oCanvas* canvas, struct RastPort *rp
);

/*
    Special mode, blit overlay toolpreview
*/
void blitToolPreview ( int x, int y, int w, int h );

/*
    Removes previous tool preview blit from the painting
*/
void removePrevToolPreview ( );

/*
    Calls blitAreaRect over a hovered over part of the screen
    to show the current brush or a clipbrush
*/
void callToolPreview ( );

/*
    Get geometry of the canvas area
    PRIVATE functions
*/
ULONG _getAreaWidth ( );
ULONG _getAreaHeight ( );
ULONG _getAreaTop ( );
ULONG _getAreaLeft ( );
ULONG _getAreaOffsetX ( );
ULONG _getAreaOffsetY ( );

#endif
