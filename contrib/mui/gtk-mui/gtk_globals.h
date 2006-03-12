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
 * $Id$
 *
 *****************************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H 1
/* globals */

#include <gtk/gtk.h>

#ifndef va_copy
#define va_copy __va_copy
#endif

#ifndef __AMIGAOS4__
extern struct GfxBase *GfxBase;
extern struct IntuitionBase *IntuitionBase;
#else
extern struct GadToolsIFace *IGadTools;
#endif
extern struct Library  *MUIMasterBase;

extern struct mgtk_ObjApp *mgtk;
extern struct Remember *rememberKey;

extern struct MUI_CustomClass *CL_RootGroup;
extern struct MUI_CustomClass *CL_Table;
extern struct MUI_CustomClass *CL_Custom;
extern struct MUI_CustomClass *CL_SpinButton;
extern struct MUI_CustomClass *CL_NoteBook;
extern struct MUI_CustomClass *CL_ProgressBar;
extern struct MUI_CustomClass *CL_Timer;
extern struct MUI_CustomClass *CL_Fixed;
extern struct MUI_CustomClass *CL_CheckButton;
extern struct MUI_CustomClass *CL_ToggleButton;
extern struct MUI_CustomClass *CL_RadioButton;
extern struct MUI_CustomClass *CL_Button;
extern struct MUI_CustomClass *CL_Menu;
extern struct MUI_CustomClass *CL_Entry;
extern struct MUI_CustomClass *CL_Toolbar;

extern int gtk_do_main_quit;

#endif
