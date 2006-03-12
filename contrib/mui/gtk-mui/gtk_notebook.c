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

GtkNotebook *GTK_NOTEBOOK(GtkWidget* widget) {
    return (GtkNotebook *) widget;
}

GtkWidget* gtk_notebook_new(void) {

  GtkWidget *ret;
  Object *note;

  DebOut("gtk_notebook_new()\n");

  ret=mgtk_widget_new(IS_NOTEBOOK);

  note = (APTR) NewObject(CL_NoteBook->mcc_Class, NULL,MA_Widget,ret,TAG_DONE);

  DebOut(" NewObject=%lx\n",note);

  ret->MuiObject=note;

  return ret;
}

gint gtk_notebook_append_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label) {
  LONG last;
  LONG act;

  DebOut("gtk_notebook_append_page(%lx,%lx,%lx)\n",notebook,child,tab_label);
  DebOut("  label=%s\n",tab_label->title);

  child->title=g_strdup(tab_label->title);

  set(notebook->MuiObject,MA_Note_Append_Child,child);

  mgtk_add_child(notebook,child);

  /* return position */
  act=xget(notebook->MuiObject,MUIA_Group_ActivePage);
  set(notebook->MuiObject,MUIA_Group_ActivePage,MUIV_Group_ActivePage_Last);
  last=xget(notebook->MuiObject,MUIA_Group_ActivePage);
  set(notebook->MuiObject,MUIA_Group_ActivePage,act);

  return last;
}

gint gtk_notebook_prepend_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label) {

  DebOut("gtk_notebook_append_page(%lx,%lx,%lx)\n",notebook,child,tab_label);
  DebOut("  label=%s\n",tab_label->title);

  child->title=g_strdup(tab_label->title);

  set(notebook->MuiObject,MA_Note_Prepend_Child,child);

  mgtk_add_child(notebook,child);

  return 0;
}

void gtk_notebook_next_page(GtkNotebook *notebook) {
  DebOut("gtk_notebook_next_page(%lx)\n",notebook);

  set(notebook->MuiObject,MUIA_Group_ActivePage,MUIV_Group_ActivePage_Next);
}

void gtk_notebook_prev_page(GtkNotebook *notebook) {
  DebOut("gtk_notebook_prev_page(%lx)\n",notebook);

  set(notebook->MuiObject,MUIA_Group_ActivePage,MUIV_Group_ActivePage_Prev);
}

