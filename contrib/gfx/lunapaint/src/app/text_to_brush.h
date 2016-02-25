/****************************************************************************
*                                                                           *
* text_to_brush.h -- Lunapaint,                                             *
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

#ifndef _text_to_brush_h
#define _text_to_brush_h

#define MAX_FONTNUM         1024
#define FONTREAD_BUFSIZE    4096

#include <stdio.h>
#include <stdlib.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include <exec/types.h>
#include <datatypes/datatypes.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <cybergraphx/cybergraphics.h>

#include <math.h>
#include <diskfont/diskfont.h>
#include <proto/diskfont.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/muimaster.h>
#include <proto/cybergraphics.h>
#include <clib/alib_protos.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string.h>

#define DEBUG 1
#include <aros/debug.h>

#include "parts.h"
#include "toolbox.h"
#include "layers.h"
#include "project.h"
#include "events.h"

extern Object *textToBrushWindow;
extern Object *ttbw_TextString;
extern Object *ttbw_RenderButton;
extern Object *ttbw_FontListview;
extern Object *ttbw_FontList;
extern Object *ttbw_FontSizeListview;
extern Object *ttbw_FontSizeList;

extern STRPTR FontList[ MAX_FONTNUM ];

extern struct Hook RenderTextToBrush_hook;

void Init_TextToBrushWindow ( );
void Init_TextToBrushMethods ( );
void Exit_TextToBrushWindow ( );
void RenderTextToBrushBuffer ( );
struct Affrect RenderTextToBuffer (
    unsigned char *text, char *font,
    int size, unsigned int **buffer
);

#endif

