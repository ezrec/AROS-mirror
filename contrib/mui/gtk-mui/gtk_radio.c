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
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include <glib.h>
#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

/*****************************************************************************
 * The MUI Radio stuff is quite limited. I tried to use it, it
 * was *more than complicated* to update the lists dynamically
 * and adding hooks to the single radio buttons was not even
 * done. 
 *
 * After rewriting this thing for three times, I choose
 * to implement radio buttons on my own.. much better ;).
 *
 * After seeing, how ugly the hook code gets over the time,
 * I made a MUI custom class for radio widgets.
 *****************************************************************************/

void mgtk_update_group(GtkWidget *widget, GSList *group) {

  DebOut("mgtk_update_group(%lx,%lx)\n",widget,group);

  widget->GSList=(APTR) group;
}

GtkRadioButton *GTK_RADIO_BUTTON(GtkWidget *widget) {
  return((GtkRadioButton *) widget);
}

GtkWidget *gtk_radio_button_new_with_label( GSList *group, const gchar  *label ) {
  GtkWidget *ret;
  /*
  GSList *last;
  GtkWidget *w;
  */
  Object *radio;

  DebOut("==============================================\n");
  DebOut("gtk_radio_button_new_with_label(%lx,%s)\n",group,label);

  ret=mgtk_widget_new(IS_RADIO);

  ret->title=g_strdup(label);

  if(group==NULL) {
    DebOut("  first element in this radio group.\n");
  }
  else {
    DebOut("  add another element to group %lx\n",group);
    DebOut("  group length: %ld\n",g_slist_length(group));
/*    last=g_slist_last (group);
    w=(GtkWidget *) last->data;
*/
  }

  radio=NewObject(CL_RadioButton->mcc_Class, NULL,MA_Widget,ret,MA_Radio_Label,label,TAG_DONE);
      
  ret->MuiObject=(APTR) radio;

  group=g_slist_append(group,ret);
  mgtk_update_group(ret,group);

  return ret;
}

/* Deprecated compatibility macro. Use gtk_radio_button_get_group() instead. */
GSList *gtk_radio_button_group(GtkWidget *widget) {

  DebOut("gtk_radio_button_group(%lx)\n",widget);

  return(gtk_radio_button_get_group(widget));
}

GSList *gtk_radio_button_get_group(GtkWidget *widget) {

  DebOut("gtk_radio_button_get_group(%lx): %lx\n",widget,widget->GSList);

  return((GSList *) widget->GSList);
}

GtkWidget* gtk_radio_button_new_with_label_from_widget( GtkRadioButton *group, const gchar    *label ) {

  DebOut("gtk_radio_button_new_with_label_from_widget(%lx,%s)\n",group,label);

  return(gtk_radio_button_new_with_label((GSList *)group->GSList,label));
}

