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
 * $Id:
 *
 *****************************************************************************/

#include <stdio.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

/*******************************************
 *mgtk_get_action_object(GtkWidget *widget)
 *
 * in case a MuiObject is packed in a frame or such,
 * this function returns the action object
 * (frame with a checkbox -> checkbox)
 *******************************************/
Object *mgtk_get_mui_action_object(GtkWidget *widget) {

  if(widget->nextObject!=NULL) {
    return((APTR) widget->nextObject);
  }
  if(widget->MuiObject==NULL) {
    ErrOut("mgtk_get_mui_action_object called on NULL pointer..\n");
  }
  return(widget->MuiObject);
}
 
/***************************************************************************/

/* #define MUIA_Prop_Pressed 0x80422cd7*/ /* V6 g BOOL */ /* private */


/*
 * gtk_signal_connect_full is deprecated and should not be used in 
 * newly-written code. Use g_signal_connect_data() instead.
 */
gulong gtk_signal_connect_full  (GtkObject *object,
                                 const gchar *name,
                                 GtkSignalFunc func,
                                 GtkCallbackMarshal unsupported,
                                 gpointer data,
                                 GtkDestroyNotify destroy_func,
                                 gint object_signal,
                                 gint after) {

  return g_signal_connect_data((gpointer) object, name, (GCallback) func, data, (GClosureNotify) destroy_func, (GConnectFlags) object_signal);

}

