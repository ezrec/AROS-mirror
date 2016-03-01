/****************************************************************************
*                                                                           *
* definitions.h -- Lunapaint,                                               *
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

#ifndef _ldefinitions_h_
#define _ldefinitions_h_

#define DRAW_DRAW       0
#define DRAW_FILLED     1
#define DRAW_DRAWFILLED 2

#define LUNA_OPACITYMODE_ADD    0
#define LUNA_OPACITYMODE_KEEP   1

#include <stdlib.h>
#include <stdio.h>
#include <exec/types.h>

/*
    The lunapaint fileformat header
    794 bytes
*/
struct LunapaintHeader
{
    char version[ 16 ];
    char projectName[ 128 ];
    char author[ 128 ];
    short width;
    short height;
    short layerCount;
    short frameCount;
    short objectCount;
};

/*
    A lunapaint object can be connected to
    a project
*/
struct LunapaintObject
{
    char type[ 32 ];
    unsigned int layer;
    unsigned int frame;
    void *data;
};

// This one is to be found in the file format
// before the object data
#define LunaObj_LayerOpacity        1
#define LunaObj_LayerVisibility     2
#define LunaObj_LayerName           3

struct LunaObjDesc
{
    unsigned int type;
    unsigned int layer;
    unsigned int frame;
    unsigned int datalength;
};


/*
    The lunapaint fileformat
*/
struct LunapaintFormat
{
    struct LunapaintHeader header;
    char *description;
    struct LunapaintObject *objects;
    unsigned long long int *layers;
};

/*
    A gfxbuffer is a 16 bits pr color buffer that holds
    graphics in nodes. Internally it's just a linked list
    of graphics. But externally, it does group buffers in
    layers and frames.
    The list with 2 frames and three layers is like this for example:
    fr1_L1 - fr1_L2 - fr1_L3 - fr2_L1 - fr2_L2 - fr2_L3
    So first comes layers then frames. When resizing the animation
    or the layers, one has 4to restructure the list.
*/
struct gfxbuffer {
    struct gfxbuffer *nextbuf;
    BOOL visible;
    char opacity;
    char *name;
    unsigned long long int *buf;
};

/*
    A simple 24bpp palette
*/
extern char filledDrawing;
extern unsigned int *globalPalette;
extern unsigned int globalColor; // Current color value
extern unsigned int currColor; // current color index
extern unsigned int prevColor; // previous color index

/*
    A canvas holds all the information about a surface. A surface
    can be shown in a Window, can have frames (it always starts with 1
    frames), can have layers on the frames etc.
*/
struct oCanvas
{
    unsigned int            width;
    unsigned int            height;
    int                     offsetx;
    int                     offsety;
    unsigned int            currentFrame;
    unsigned int            totalFrames;
    unsigned int            currentLayer;
    unsigned int            previousLayer;      // Previously selected layer
    unsigned int            totalLayers;
    unsigned char           onion;              // Modes of onion skin

    unsigned int            *screenbuffer;       // Holds temp data for displaying
    unsigned int            *screenstorage;      // Stores the screenbuffer
    BOOL                    winHasChanged;      // If the window has changed
    unsigned int            scrStorageWidth;    // Width of storage window
    unsigned int            scrStorageHeight;   // Height of storage window
    unsigned int            layerScrollPosV;    // Vertical scroll position in layers window

    unsigned int            winEdgeWidth;       // Edge from canvas to window edge
    unsigned int            winEdgeHeight;      // --||--
    unsigned int            visibleWidth;       // Visible part of image
    unsigned int            visibleHeight;      // Visible part of image
    double                  zoom;               // Amout of zoom on canvas
    struct gfxbuffer        *buffer;            // Linked list
    unsigned long long int  *activebuffer;      // Points to the current layer/frame
    unsigned long long int  *swapbuffer;        // A swap buffer :-)
    unsigned int            *tmpBuf;            // Stores temp info if needed
} oCanvas;

#endif
