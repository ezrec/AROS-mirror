/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>
/* cross platform hooks */
#include <SDI_hook.h>

#include <gtk/gtk.h>
#include "debug.h"

/* globals */
#ifndef __AMIGAOS4__
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library *UtilityBase;
#else
struct Library *GadToolsBase ;
struct GadToolsIFace *IGadTools;
struct MUIMasterIFace *IMUIMaster;
#endif
struct Library  *MUIMasterBase;

struct mgtk_ObjApp *mgtk;
struct Remember *rememberKey = NULL;

struct MUI_CustomClass *CL_RootGroup;
struct MUI_CustomClass *CL_Custom;
struct MUI_CustomClass *CL_Table;
struct MUI_CustomClass *CL_SpinButton;
struct MUI_CustomClass *CL_NoteBook;
struct MUI_CustomClass *CL_ProgressBar;
struct MUI_CustomClass *CL_Timer;
struct MUI_CustomClass *CL_Fixed;
struct MUI_CustomClass *CL_CheckButton;
struct MUI_CustomClass *CL_ToggleButton;
struct MUI_CustomClass *CL_RadioButton;
struct MUI_CustomClass *CL_Button;
struct MUI_CustomClass *CL_Menu;
struct MUI_CustomClass *CL_Entry;
struct MUI_CustomClass *CL_Toolbar;

int gtk_do_main_quit=0;
