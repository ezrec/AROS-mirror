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
 * $Id: gtk_checkbutton.c,v 1.15 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

/* cross platform hooks */
#include <SDI_hook.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

GtkToggleButton *GTK_TOGGLE_BUTTON(GtkWidget *widget) {

  return (GtkToggleButton *) widget;
}

/* to be tested.. */
GtkWidget *gtk_toggle_button_new(void) {
  GtkWidget *button;

  DebOut("gtk_toggle_button_new()\n");

  button=mgtk_widget_new(IS_TOGGLEBUTTON);

  button->MuiObject=(APTR) NewObject(CL_ToggleButton->mcc_Class, NULL,MA_Widget,button,TAG_DONE);

  //return gtk_check_button_new_with_label ("");

  return button;

}

GtkWidget *gtk_check_button_new_with_label (const gchar *label) {
  GtkWidget *ret = NULL;
  Object *check;

  DebOut("gtk_check_button_new_with_label(%s)\n",label);

  ret=mgtk_widget_new(IS_CHECKBUTTON);

  check = NewObject(CL_CheckButton->mcc_Class, NULL,MA_Widget,ret,MA_Check_Label,label,TAG_DONE);

  ret->MuiObject = (APTR) check;

  return ret;
}

void gtk_toggle_button_set_active( GtkToggleButton *toggle_button, gboolean is_active) {

  DebOut("gtk_toggle_button_set_active(%lx,%ld)\n",toggle_button,is_active);

  set(toggle_button->MuiObject,MA_Toggle_Value,(ULONG) is_active);

  return;
}

gboolean gtk_toggle_button_get_active(GtkToggleButton *toggle_button) {

  DebOut("gtk_toggle_button_get_active(%lx)\n",toggle_button);

  return xget(toggle_button->MuiObject,MA_Toggle_Value);
}
