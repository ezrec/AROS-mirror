/****************************************************************************
*                                                                           *
* gui.h -- Lunapaint,                                                       *
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

#ifndef _gui_h_
#define _gui_h_

#include <stdio.h>

#include <exec/types.h>
#include <libraries/mui.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <proto/dos.h>
#include <clib/alib_protos.h>
#include <string.h>

#ifndef __AROS__
#include "aros/aros.h"
#endif

#include "events.h"

/*
    GUI includes
*/
#include "about_window.h"
#include "new_image.h"
#include "export_import.h"
#include "toolbox.h"
#include "layers.h"
#include "animation.h"
#include "palette_editor.h"
#include "canvas.h"
#include "parts.h"
#include "preferences.h"
#include "text_to_brush.h"

#include "../config.h"

#include "locale.h"

struct ObjectBOOL
{
    Object *obj;
    BOOL value;
} ObjectBOOL;

/*
    Program menu
*/
extern IPTR ProgramMenu;
extern Object *mainPulldownMenu;

/*
    - The MUI "Application" runs on *PaintApp
    - The canvases is managed in the canvases list
*/
extern Object *PaintApp;
extern struct WindowList *canvases;
extern Object *testWindow;
extern Object *AlertWindow;
extern Object *AlertText;
extern Object *AlertOk;
extern Object *LunaBackdrop;

/*
    This function returns a pointer to a window by id
*/
Object *GetWindowById ( int id );

/*
    This function starts the application object
*/
void Init_Application ( );

/*
    Clean up and dispose of all MUI objects
*/
void Exit_Application ( );

/*
    Get signals
*/
int getSignals ( ULONG *sigs );

/*
    Check for break
*/
int checkSignalBreak ( ULONG *sigs );

/*
    Setup the lunapaint screen
*/
void InitLunaScreen ( );

/*
    Init the backdrop window
*/
void Init_Backdrop ( );

/*
    Initialize the alert window
*/
void Init_AlertWindow ( );

/*
    Display an alert message
*/
void ShowAlert ( unsigned char *text );

/*
    Hide all open windows and save their state so they can be reopened
    as they were
*/
void HideOpenWindows ( );
void ReopenWindows ( );


#endif
