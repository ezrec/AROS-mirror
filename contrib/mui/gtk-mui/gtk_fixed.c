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
#include "classes/classes.h"

GtkFixed *GTK_FIXED(GtkWidget* widget) {
    return (GtkFixed *) widget;
}

GtkWidget *gtk_fixed_new(void) {

  GtkWidget *ret;
  Object *fixed;

  DebOut("gtk_fixed_new()\n");

  ret=mgtk_widget_new(IS_FIXED);

  fixed = (APTR) NewObject(CL_Fixed->mcc_Class, NULL,MA_Widget,ret,TAG_DONE);

  DebOut(" NewObject=%lx\n",fixed);

  ret->MuiObject=fixed;

  return ret;
}

void gtk_fixed_put(GtkFixed *fixed, GtkWidget *widget, gint x, gint y) {

  DebOut("gtk_fixed_put(%lx,%lx,%d,%d)\n",fixed,widget,x,y);

  DoMethod(widget->MuiObject,MUIM_Group_InitChange);
  DoMethod(fixed->MuiObject,OM_ADDMEMBER,widget->MuiObject);
  DoMethod(widget->MuiObject,MUIM_Group_ExitChange);

  gtk_fixed_move(fixed,widget,x,y);
}

void gtk_fixed_move(GtkFixed *fixed, GtkWidget *widget, gint x, gint y) {

  struct MA_Fixed_Move_Data *data;

  DebOut("gtk_fixed_move(%lx,%lx,%d,%d)\n",fixed,widget,x,y);

  data=g_new(struct MA_Fixed_Move_Data,1);

  data->widget=widget;
  data->x=x;
  data->y=y;

  set(fixed->MuiObject,MA_Fixed_Move,data);

  g_free(data);

}

