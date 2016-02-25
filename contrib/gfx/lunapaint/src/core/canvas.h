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

#ifndef _canvas_h_
#define _canvas_h_
#define MAXLAYERS 8

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define AROS 1
#ifdef AROS
#include <proto/dos.h>
#endif

#define DEBUG 1
#include <aros/debug.h>

#include "definitions.h"
#include "color.h"
#include "common.h"
#include "math/math.h"
#include "../app/locale.h"

// Use this to interrupt redrawing
extern BOOL abortRedraw;
extern int redrawTimes; // how many redrawtimes

// Linked value list
struct ValueList
{
    double x, y;
    struct ValueList *Next;
};


// Tool Line
struct toolLineData
{
    int                     mode;           // The mode
    double                  x;              // Draw from
    double                  y;              // -||-
    double                  dx, dy;         // Destination x, y (draw to)
    double                  ox;             // start offset x
    double                  oy;             // start offset y
    double                  w;              // Width of buffer rect
    double                  h;              // Height of buffer rect
    BOOL                    initialized;    // Are we initialized or not?
    unsigned long long int  *buffer;        // pixelbuffer
} toolLineData;

extern struct toolLineData lineTool;

// Tool Brush
struct toolBrushData
{
    int                     width;      // Brush width
    int                     height;     // Brush height
    int                     opacity;    // Brush opacity
    int                     power;      // Brush draw power
    int                     step;       // Brush steps in between draw
    int                     paintmode;  // The current paintmode
    char                    opacitymode;// How to use opacity
    struct ValueList        *ContourBuffer; // For filled freehand drawing
    BOOL                    RecordContour;  // Record pixels for contour or not?
    double                  ContourMinX, ContourMinY;   // Smallest x,y
    double                  ContourMaxX, ContourMaxY;   // Largest x,y
    BOOL                    feather;    // Using feather or not?
    BOOL                    antialias;  // Using antialiasing or not?
    BOOL                    initialized;// Are we initialized?
    unsigned long long int  *buffer;    // Buffer for storing the brush
    unsigned long long int  *tmpbuf;    // Generic buffer for different things
};

extern struct toolBrushData brushTool;

// Tool Circle
struct toolCircleData
{
    int                     mode;       // The mode
    int                     x, y;       // Draw from
    int                     ox;         // start offset x
    int                     oy;         // start offset y
    int                     w;          // Width of buffer rect
    int                     h;          // Height of buffer rect
    int                     bufwidth;   // Buffer width
    int                     bufheight;  // buffer height
    BOOL                    initialized;// Are we initialized or not?
    unsigned long long int  *buffer;    // pixelbuffer
};

extern struct toolCircleData circleTool;

// Tool Rectangle
struct toolRectData
{
    int                     mode;           // The mode
    double                  x;              // Draw from
    double                  y;              // -||-
    double                  dx, dy;         // Destination x, y (draw to)
    double                  ox;             // start offset x
    double                  oy;             // start offset y
    double                  w;              // Width of buffer rect
    double                  h;              // Height of buffer rect
    BOOL                    initialized;    // Are we initialized or not?
    unsigned long long int  *buffer;        // pixelbuffer
};

extern struct toolRectData rectangleTool;

// Tool Clipbrush
struct toolClipBrushData
{
    int                     mode;       // The mode
    double                  x;          // Draw from
    double                  y;          // -||-
    double                  dx, dy;     // Destination x, y (draw to)
    double                  ox;         // start offset x
    double                  oy;         // start offset y
    double                  w;          // Width of buffer rect
    double                  h;          // Height of buffer rect
    BOOL                    initialized;// Are we initialized or not?
    unsigned long long int  *buffer;    // pixelbuffer
};

extern struct toolClipBrushData clipbrushTool;

/*
    Initialize a canvas and return a struct with a canvas in it
*/
struct oCanvas* Init_Canvas (
    unsigned int w, unsigned int h, unsigned int layers,
    unsigned int frames, BOOL generateBuffers
);

/*
    Control the frames on a canvas
*/
void NextFrame ( struct oCanvas *canvas );
void PrevFrame ( struct oCanvas *canvas );
int GotoFrame ( struct oCanvas *canvas, unsigned int frame );
int NextLayer ( struct oCanvas *canvas );
int PrevLayer ( struct oCanvas *canvas );
int GotoLayer ( struct oCanvas *canvas, unsigned int layer );

/*
    Get a pixel from the canvas
*/
unsigned long long int GetPixelFromCanvas ( unsigned int x, unsigned int y );

/*
    Get a pixel from all current layers combined ( flattened )
*/
unsigned long long int GetFlatPixelFromCanvas ( unsigned int x, unsigned int y );

/*
    Check which layer / frame we are using and set the pointer to the active
    buffer...
*/
void setActiveBuffer ( struct oCanvas *canvas );

/*
    Return a pointer to the next frame (wrap around)
*/
unsigned long long int *getNextFrame ( struct oCanvas *canvas );

/*
    Return a pointer to the prev frame (wrap around)
*/
unsigned long long int *getPrevFrame ( struct oCanvas *canvas );

/*
    Return a pointer to the current gfxbuffer struct
*/
struct gfxbuffer *getActiveGfxbuffer ( struct oCanvas *canvas );

/*
    Return a pointer to a specified gfxbuffer
*/
struct gfxbuffer *getGfxbuffer ( struct oCanvas *canvas, int layer, int frame );
struct gfxbuffer *getGfxbufferFromList ( struct gfxbuffer *buf, int layer, int frame, int totallayers, int totalframes );

/*
    Free the memory in a canvas
*/
void Destroy_Canvas ( struct oCanvas *canvas );

/*
    Free memory in a canvas buffer
*/
void Destroy_Buffer ( struct oCanvas *canv );

/*
    Draw the layers etc and return an unsigned int buffer for use by
    redrawScreenbufferRect and others
*/
unsigned int *renderCanvas (
    struct oCanvas *canvas, unsigned int rx, unsigned int ry,
    unsigned int rw, unsigned int rh, BOOL Transparent
);

/*
    Scroll the canvas screen storage by x,y (destructive moving)
*/
void scrollScreenStorage ( struct oCanvas *canvas, int x, int y );

/*
    Draw all layers onto the screenbuffer constrained by rect
*/
BOOL redrawScreenbufferRect (
    struct oCanvas *canvas, unsigned int rx, unsigned int ry, unsigned int rw,
    unsigned int rh, BOOL updateStorage
);

/*
    Draw all layers onto the screenbuffer in specified
    canvas data
*/
BOOL redrawScreenbuffer ( struct oCanvas *canvas );

/*
    Initialize the palette with standard colors...
*/
void initPalette ( );

/*
    Load the default palette
*/
void loadDefaultPalette ( );

/*
    Insert a new layer on each frame
*/
void addLayer ( struct oCanvas *canv );

/*
    Swap two layers
*/
void swapLayers ( struct oCanvas *canv );

/*
    Copy one layer to another
*/
void copyLayers ( struct oCanvas *canv );

/*
    Merge two laywers
*/
void mergeLayers ( struct oCanvas *canv );

/*
    Delete a layer
*/
void deleteLayer ( struct oCanvas *canv );

/*
    Copy data to swap
*/
void copyToSwap ( struct oCanvas *canv );

/*
    Swap with spare
*/
void swapCanvasBuffers ( struct oCanvas *canv );


/*
    Draw preview of current tool / hover version
*/
unsigned int drawToolPreview ( int x, int y );

/*
    Use the value list
*/
void addListValue ( double x, double y, struct ValueList **lst );
void freeValueList ( struct ValueList **lst );

#endif


