/****************************************************************************
*                                                                           *
* drawtools.h -- Lunapaint,                                                 *
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

#ifndef _drawtools_h_
#define _drawtools_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "math/math.h"
#include "app/toolbox.h"
#include "canvas.h"

#define PI 3.14159265358979323846264f
#define RAD 0.01745329250000000116f
#define DEG 57.29577957855229514f

// Some array tools
extern int fStackPointer;
extern BOOL isFilling;

BOOL fPop ( int *x, int *y, int pitch, int *fStack );
void fPush ( int x, int y, int limit, int pitch, int *fStack );
void fEmptyStack ( int *fStack );

// Draw a line from x1,y1 to x2,y2 on defined buffer
// endpoint is if you are doing this in a draw operation
// where you don't want to draw over a coord two times
struct Affrect drawLine (
    double x1, double y1, double x2, double y2,
    int bufferwidth, int bufferheight,
    unsigned long long int *buffer, BOOL subtract_endpoint
);
// Draws a line used on an unsigned char buffer (used for internal stuff)
void drawLineCharbuf (
    double x1, double y1, double x2, double y2,
    int bufferwidth, int bufferheight,
    unsigned char *buffer, unsigned char value
);

struct Affrect plotBrush (
    double x, double y,
    int bufferwidth, int bufferheight,
    unsigned long long int *buffer
);

void pixelAntialias (
    double x, double y, struct rgba64 paintcol, int bw, int bh, unsigned long long int *buf
);

void pixelPlain (
    int x, int y, struct rgba64 paintcol, int bw, int bh, unsigned long long int *buf
);


unsigned long long int processPixel (
    struct rgba64 origCol, struct rgba64 paintCol, double x, double y,
    double diffx, double diffy
);

struct Affrect floodFill (
    int x, int y,
    int bufferwidth, int bufferheight,
    unsigned long long int color,
    unsigned long long int clickColor,
    unsigned long long int *buffer,
    unsigned int threshold
);

void fillCharbuf (
    int x, int y, int bufferwidth, int bufferheight,
    unsigned char *buffer, unsigned char value
);

struct Affrect drawCircle (
    double x, double y, double w, double h,
    unsigned int bufferwidth, unsigned int bufferheight,
    unsigned long long int *buffer
);

/*
    Snap a color to the palette
*/
struct rgba64 snapToPalette ( struct rgba64 color, BOOL selectIndex );

#endif
