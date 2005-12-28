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
 * $Id: gtk_spinbutton.c,v 1.9 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

GtkSpinButton     *GTK_SPIN_BUTTON(GtkWidget* widget) {
  return (GtkSpinButton *) widget;
}

GtkWidget* gtk_spin_button_new(GtkAdjustment *adjustment, gdouble climb_rate, guint digits) {

  GtkWidget *ret;
  Object *spin;
  char *value;
  GSList *gs;

  DebOut("gtk_spin_button_new(%lx,,)\n",adjustment);

  ret=mgtk_widget_new(IS_SPINBUTTON);

  DebOut(" NewObject\n");
  DebOut("CL_SpinButton=%x\n",CL_SpinButton);
  spin = (APTR) NewObject(CL_SpinButton->mcc_Class, NULL,MA_Widget,ret,TAG_DONE);
  DebOut(" NewObject=%lx\n",spin);

  ret->MuiObject=spin;

  if(digits) {
    ret->decimalplaces=digits;
    ret->digits=digits;
  }
  else {
    ret->decimalplaces=adjustment->decimalplaces;
    ret->digits=digits;
  }
  ret->lower=adjustment->lower;
  ret->upper=adjustment->upper;
  if(climb_rate) {
    ret->step_increment=climb_rate;
  }
  else {
    ret->step_increment=adjustment->step_increment;
  }
  ret->value=adjustment->value;

  value=g_strdup_printf("%f",ret->value);
  set(spin,MA_Spin_Value,value);
  g_free(value);

  /* add this Widget to the adjustment */
  gs=adjustment->GSList;
  gs=g_slist_append(gs,ret);
  adjustment->GSList=gs;

  return ret;
}

gint gtk_spin_button_get_value_as_int (GtkSpinButton *spin_button) {
  const char *value_string;
  gdouble f;
  int i;

  DebOut("gtk_spin_button_get_value_as_int(%x)\n",(int) spin_button);

  value_string=(const char *) xget(spin_button->MuiObject,MA_Spin_Value);
  f=atof(value_string);
  i=(int) f;

  return i;
}

void gtk_spin_button_set_digits(GtkSpinButton *spin_button, guint digits) {
  char *value;

  DebOut("gtk_spin_button_set_digits(%x,%d)\n",(int) spin_button, digits);

  spin_button->decimalplaces=digits;
  spin_button->digits=digits;
  value=g_strdup_printf("%f",spin_button->value);
  set(spin_button->MuiObject,MA_Spin_Value,value);
  g_free(value);
}

gdouble gtk_spin_button_get_value(GtkSpinButton *spin_button) {

  DebOut("gtk_spin_button_get_value(%x)\n",(int) spin_button);

  return spin_button->value;
}

gdouble gtk_spin_button_get_value_as_float(GtkSpinButton *spin_button) {

  return gtk_spin_button_get_value(spin_button);
}

void gtk_spin_button_set_wrap(GtkSpinButton *spin_button, gboolean wrap) {
  DebOut("gtk_spin_button_set_wrap(%x,%d)\n",(int) spin_button,wrap);

  spin_button->wrap=wrap;
}

void gtk_spin_button_set_numeric(GtkSpinButton *spin_button, gboolean numeric) {
  char *all;
  int i;

  DebOut("gtk_spin_button_set_numeric(%x,%d)\n",spin_button,numeric);

  if(numeric) {
    set(spin_button->MuiObject,MUIA_String_Accept , "-0123456879.");
  }
  else {
    all=g_new(char,256);
    i=0;
    while(i<(256-32)) {
      all[i]=(char) i+32;
      i++;
    }
    all[i]=(char) 0;
    set(spin_button->MuiObject,MUIA_String_Accept , all);
    g_free(all);
  }
}
