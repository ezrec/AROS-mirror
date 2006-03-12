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

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

GtkWidget *gtk_grab_get_current(void) {

  DebOut("gtk_grab_get_current()\n");

  return mgtk->grab;
}

void gtk_grab_add(GtkWidget *widget) {

  DebOut("gtk_grab_add(%lx)\n");

  if(widget != mgtk->grab) {
    WarnOut("gtk_grab_add is just a dummy\n");
    mgtk->grab=widget;
  }
}
  
void gtk_grab_remove(GtkWidget *widget) {
  DebOut("gtk_grab_remove(%lx)\n");

  WarnOut("gtk_grab_remove is just a dummy\n");
  mgtk->grab=NULL;
}
