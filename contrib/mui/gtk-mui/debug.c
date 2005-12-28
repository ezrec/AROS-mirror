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
 * $Id: debug.c,v 1.1 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifndef __MORPHOS__
#include <clib/alib_protos.h>
#endif
#include <proto/intuition.h>
#include <mui.h>

#include "debug.h"
#include <gtk/gtk.h>
#include "gtk_globals.h"

/*****************************************************************************
 * Errors are never nice, but they happen. This functions leaves the user
 * the choice: 
 *
 * He can continue at his own risk, which migth work, if
 * for example an image could not be loaded, or which might crash,
 * if a MuiObject could not be created.
 *
 * He can try to do a clean exit, resources should be released here.
 *
 * He can try to run, leave all resources allocated, but maybe he
 * can avoid a full crash..
 *
 * Default should be to Clean Exit ..
 *****************************************************************************/

void handle_error(const char *format, ...) {

  LONG reply;
  struct Window *w;
  char *text;
  va_list args;
  struct EasyStruct easy = {
    sizeof (struct EasyStruct),
    0,
    "GTK-MUI Error",
    "%s",
    "Ignore|Clean Exit|Hard Exit",
  };

  DebOut("handle_error(..)\n");

  if(mgtk && mgtk->MuiRoot) {
    w=(struct Window *) xget(mgtk->MuiRoot,MUIA_Window_Window);
  }
  else {
    w=NULL;
  }

  va_start (args, format);
  text = g_strdup_vprintf (format, args);
  va_end (args);

  reply=EasyRequest( w, &easy, NULL, text);
  DebOut("  reply=%d\n",reply);

  switch(reply) {
    case 0: exit(1);              /* Hard Exit, quit NOW! */
    case 2: gtk_main_quit();      /* Exit as soon as possible */
            break;
    case 1: DebOut("User wants to continue at own risk!");
            break;
  }

  g_free(text);

}
