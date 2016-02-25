/****************************************************************************
*                                                                           *
* tools.h -- Lunapaint,                                                     *
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

#ifndef _lunatools_h_
#define _lunatools_h_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "canvas.h"

#ifdef AROS
#include "app/canvas.h"
#include "app/toolbox.h"
#endif

#include "drawtools.h"

/*
    Draw with the brush!
*/
BOOL toolBrush ( );
void toolBrushFill ( ); // Fill the last drawn shape
void makeToolBrush ( ); // <- generate pixel buffer

/*
    Fill!
*/
BOOL toolFill ( );

/*
    Line drawing tool
*/
BOOL toolLine ( );

/*
    Rectangle drawing tool
*/
BOOL toolRectangle ( );

/*
    Rectangle drawing tool
*/
BOOL toolCircle ( );

/*
    Rectangle drawing tool
*/
BOOL toolClipBrush ( );

/*
    Pick a color
*/
BOOL toolColorPicker ( );

/*
    Tool tools =)
*/
void flipBrushVert ( );
void flipBrushHoriz ( );
void rotateBrush90 ( );

#endif
