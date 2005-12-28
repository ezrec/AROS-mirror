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
#include <string.h>
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

GtkWidget* gtk_button_new (void) {
  Object *button;
  GtkWidget *ret;

  ret=mgtk_widget_new(IS_BUTTON);

/*
  button = (Object *) VGroup,
          GroupSpacing(0),
          MUIA_Weight, 0,
          ButtonFrame,
          MUIA_InputMode , MUIV_InputMode_RelVerify,
          End;
          */
  if(ret) {
    button=NewObject(CL_Button->mcc_Class, NULL,MA_Widget,ret,TAG_DONE);
    DebOut("gtk_button_new: %lx\n",button);
    ret->MuiObject=button;
  }

  return ret;
}

GtkWidget* gtk_button_new_with_label(const char *label) {
  Object *button;
  GtkWidget *ret = NULL;

  /* Note: MUI_MakeObject() uses underscored char for keyboard shortcuts
  **
  ** If this is not desired then we must use taglist way
  */

  ret = mgtk_widget_new(IS_BUTTON);

  if(ret) {
    /*button = MUI_MakeObject(MUIO_Button, (ULONG)label);*/
    button=NewObject(CL_Button->mcc_Class, NULL,MA_Widget,ret,MA_Button_Label,(ULONG) label,TAG_DONE);
    DebOut("gtk_button_new_with_label: %lx\n",button);
    ret->MuiObject=button;
  }

  return ret;
}

GtkWidget*  gtk_button_new_from_stock       (const gchar *stock_id){

  DebOut("gtk_button_new_from_stock(%s)\n",stock_id);

  if(!strcmp(stock_id,GTK_STOCK_CLOSE)) {
    return gtk_button_new_with_label("\033cClose");
  }

  ErrOut("Unknown stock object: %s\n",stock_id);
  return NULL;
}
