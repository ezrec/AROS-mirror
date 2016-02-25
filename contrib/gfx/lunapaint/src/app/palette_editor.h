/****************************************************************************
*                                                                           *
* palette_editor.h -- Lunapaint,                                            *
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

#ifndef _palette_editor_h_
#define _palette_editor_h_

#include <stdio.h>
#include <stdlib.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include <exec/types.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <cybergraphx/cybergraphics.h>

#include <dos/filehandler.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <proto/cybergraphics.h>
#include <clib/alib_protos.h>
#include <string.h>

#include "gui.h"
#include "parts.h"
#include "toolbox.h"

#include "config.h"

// Some defines for palette actions
#define LNPATAGBASE (TAG_USER+200)

#define MUIM_Luna_Palette_Copy      (LNPATAGBASE+0)
#define MUIM_Luna_Palette_Paste     (LNPATAGBASE+1)
#define MUIM_Luna_Palette_Swap      (LNPATAGBASE+2)
#define MUIM_Luna_Palette_Reverse   (LNPATAGBASE+3)
#define MUIM_Luna_Palette_Spread    (LNPATAGBASE+4)
#define MUIM_Luna_Palette_Clear     (LNPATAGBASE+5)


extern Object *paletteWindow;
extern Object *paletteRect;
extern Object *palSlideR;
extern Object *palSlideG;
extern Object *palSlideB;
extern Object *palColRect;
extern Object *palBtnUse;
extern Object *palBtnSave;
extern Object *palBtnLoad;
extern Object *palStrFileSave;

// Action buttons
extern Object *palBtnCopy;
extern Object *palBtnSwap;
extern Object *palBtnSpread;
extern Object *palBtnReverse;
extern Object *palBtnClear;
extern Object *palBtnPaste;

extern unsigned int tempCopiedColor;
extern BOOL tempCopiedColorExists;

extern struct Hook rgbslider_hook;
extern struct Hook paletteSave_hook;
extern struct Hook paletteLoad_hook;
extern struct Hook paletteClose_hook;

/*
    Function to redraw the palette area
*/
IPTR PaletteRedraw ( int colorIndex );

/*
    Function to handle input events on area
*/
IPTR PaletteEvents ( Class *CLASS, Object *self, struct MUIP_HandleInput *msg );

/*
    Do different palette actions
*/
IPTR PaletteActions ( ULONG action );

/*
    update the little preview rect over the rgb sliders
*/
void updateColorPreview ( );

/*
    Initialize the palette window
*/
void Init_PaletteWindow ( );

/*
    Initialize the events on the palette
*/
void Init_PaletteMethods ( );

/*
    Save a palette to disk using RAW binary format
*/
ULONG savePalette ( );

/*
    Load a palette from disk using RAW binary format
*/
ULONG loadPalette ( );

#endif
