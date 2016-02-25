/****************************************************************************
*                                                                           *
* toolbox.h -- Lunapaint,                                                   *
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

#ifndef _toolbox_h_
#define _toolbox_h_

#include <stdio.h>

#include <exec/types.h>
#include <libraries/mui.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <proto/cybergraphics.h>
#include <clib/alib_protos.h>
#include <string.h>

#include "canvas.h"
#include "about_window.h"
#include "palette_editor.h"
#include "export_import.h"
#include "new_image.h"
#include "project.h"
#include "layers.h"
#include "preferences.h"

#include "config.h"
#include "core/effects.h"

#define DEBUG 1
#include <aros/debug.h>

#define LUNAPUBSCREEN TRUE
#define SET_ANTIALIAS 0
#define SET_ANTIALIASOFF 1
#define SET_FEATHERED 2
#define SET_FEATHEREDOFF 3

/*
	Toolbox object elements
*/
extern struct Hook getMenu_hook;
extern struct Hook brushOptions_hook;
extern struct Hook getFill_hook;
extern struct Hook getOpacMode_hook;

extern int ToolboxRunning;
extern Object *toolbox;
extern Object *toolboxTopGroup;
extern Object *tbxAreaPalette;
extern Object *tbxAreaPreview;
extern Object *tbxBtn_draw;
extern Object *tbxBtn_line;
extern Object *tbxBtn_rectangle;
extern Object *tbxBtn_polygon;
extern Object *tbxBtn_circle;
extern Object *tbxBtn_fill;
extern Object *tbxBtn_getbrush;
extern Object *tbxBtn_pickcolor;

extern char tbxPaletteClickMode;

extern Object *antiImage, *antiOffImage, *solidImage, *featherImage;
extern Object *brushOpGroup;

extern Object *tbxCyc_GridSize;
extern Object *tbxCycColorCtrl;

extern Object *tbxSlider_Brushdiameter;
extern Object *tbxSlider_Brushopacity;
extern Object *tbxSlider_Brushincrement;
extern Object *tbxSlider_Brushstep;
extern Object *tbxCycGrid;
extern Object *tbxCycFillmode;

extern Object *tbxCyc_PaletteSnap;

extern Object *tbxCycPaintMode;

extern Object *offsetWindow, *offsetWindowOk, *offsetWindowCancel, *offsetWindowX, *offsetWindowY;

extern unsigned int *PreviewRectData;

/*
	Color things
*/
void nextPaletteColor ( );
void prevPaletteColor ( );

/*
	This function checks which menu entries have been selected
*/
void checkMenuEvents ( int udata );

/*
	Redraws the brush/fill preview
*/
IPTR tbxPaintPreview ( );

/*
	This function redraws the mini palette on the toolbox...
*/
IPTR tbxPaletteRedraw ();

/*
	This function "thinks" for the toolbox palette
*/
IPTR tbxPaletteThink ( struct MUIP_HandleInput *msg );

/*
	This function initializes the application window and brings up
	the toolbox...
*/
void Init_ToolboxWindow ();

/*
	This function sets up the methods on the toolbox
*/
void Init_ToolboxMethods ();

/*
	This frees up any unfreed mem
*/
void Exit_ToolboxWindow ( );

/*
	set toolbar buttons that aren't active to normal
	frames and the active button to a recessed frame
*/
void setToolbarActive ( );

/*
	Get if we're using a filled shape or not
*/
void GetDrawFillState ( );

#endif
