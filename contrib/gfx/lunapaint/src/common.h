/****************************************************************************
*                                                                           *
* common.h -- Lunapaint,                                                    *
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

#ifndef _common_h_
#define _common_h_

#define LUNA_MOUSEDOWN              1001
#define LUNA_TOOL_BRUSH             1
#define LUNA_TOOL_FILL              2
#define LUNA_TOOL_LINE              3
#define LUNA_TOOL_RECTANGLE         4
#define LUNA_TOOL_CLIPBRUSH         5
#define LUNA_TOOL_CIRCLE            6
#define LUNA_TOOL_COLORPICKER       7
#define LUNA_PAINTMODE_NORMAL       1
#define LUNA_PAINTMODE_ERASE        2
#define LUNA_PAINTMODE_UNERASE      3
#define LUNA_PAINTMODE_BLUR         4
#define LUNA_PAINTMODE_SMUDGE       5
#define LUNA_PAINTMODE_LIGHTEN      6
#define LUNA_PAINTMODE_DARKEN       7
#define LUNA_PAINTMODE_COLORIZE     8
#define LUNA_PAINTMODE_COLOR        9

#define LUNA_COLORMODE_64BIT        1
#define LUNA_COLORMODE_SNAP         2

#include <stdio.h>
#include <stdlib.h>
#include "core/definitions.h"

#ifdef __AROS__
#include "app/parts.h"
#endif

struct Affrect
{
    int x;
    int y;
    int w;
    int h;
};

struct WindowList*          globalActiveWindow;
struct oCanvas*             globalActiveCanvas;
int                         globalWindowIncrement;
int                         globalEvents;
int                         globalCurrentTool;
int                         globalGrid; // Using grid?
int                         globalCurrentGrid; // Grid size

int                         eMouseX; // event mouse coordinate
int                         eMouseY; // --||--
double                      cMouseX; // Registered coordinates..
double                      cMouseY; // --||--
double                      dMouseX; // Coordinate for drawing
double                      dMouseY; // --||--
double                      pMouseX; // Prev coordinate for drawing
double                      pMouseY; // --||--

BOOL                        MouseButtonL;
BOOL                        MouseButtonR;
BOOL                        globalAntialiasing;
BOOL                        globalFeather;
int                         globalBrushMode; // Normal generated or clipbrush
int                         globalColorMode; // How to use colors
UWORD                       lastKeyPressed;
BOOL                        MouseHasMoved;

#endif
