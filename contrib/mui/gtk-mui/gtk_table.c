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
 * $Id: gtk_table.c,v 1.16 2005/12/03 23:25:03 o1i Exp $
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
#include "classes.h"

GtkTable *GTK_TABLE(GtkWidget *widget) {
  return (GtkTable *) widget;
}

GtkWidget *gtk_table_new(guint rows, guint columns, gboolean homogeneous) {
  GtkWidget *ret = NULL;
  APTR table;

  DebOut("gtk_table_new(rows %ld, columns %ld, homogeneous %ld)\n",rows,columns,homogeneous);

#if 0
  table=(APTR) GroupObject,
		 MUIA_Group_Columns, columns,
/*		 Child,TextObject,MUIA_Text_Contents,"1",End,
		 Child,TextObject,MUIA_Text_Contents,"2",End,
		 Child,TextObject,MUIA_Text_Contents,"3",End,
		 Child,TextObject,MUIA_Text_Contents,"4",End,*/
	       End;
#endif

  table = (APTR) NewObject(CL_Table->mcc_Class, NULL,TAG_DONE);
  set(table,MA_Table_Columns,columns);
  set(table,MA_Table_Rows,rows);

  if (table)
  {
    /*
    APTR child;
    int i;
    GtkWidget * act;
    GtkWidget * next;
    char text[512];
    */

    ret = mgtk_widget_new(IS_TABLE);

    if (ret)
    {
      ret->MuiObject = table;;
      ret->columns = columns;
      ret->rows = rows;
    }
    else
    {
      MUI_DisposeObject(table);
    }
  }
 
  return ret;
}

/*
void GLADE_HOOKUP_OBJECT(GtkWidget *a,GtkWidget *b,char *foo) {
  DebOut("GLADE_HOOKUP_OBJECT: not done yet\n");
}

void GLADE_HOOKUP_OBJECT_NO_REF(GtkWidget *a,GtkWidget *b,char *foo) {
  DebOut("GLADE_HOOKUP_OBJECT: not done yet\n");
}
*/

void gtk_table_attach(GtkTable *table, GtkWidget *child, guint left_attach, guint right_attach, guint top_attach, guint bottom_attach, GtkAttachOptions xoptions, GtkAttachOptions yoptions, guint xpadding, guint ypadding ) {

  DebOut("WARNING: gtk_table_attach not done yet, just a call to gtk_table_attach_defaults!!\n");
  gtk_table_attach_defaults(table, child, left_attach,right_attach,top_attach,bottom_attach);
}

void gtk_table_attach_defaults  (GtkTable *table, GtkWidget *widget, guint left_attach, guint right_attach, guint top_attach, guint bottom_attach) {
  /*
  int number;
  int i=0;
  GtkWidget *act;
  GtkWidget *replaceme;
  */

  DebOut("gtk_table_attach_defaults(table %lx, GtkWidget %lx, left_attach %ld, right_attach %ld, top_attach %ld, bottom_attach %ld)\n",table,widget,left_attach,right_attach,top_attach,bottom_attach);

  if(widget->parent!=NULL) {
    printf("WARNING: widget %x has already a parent(%x), now: %x\n",(int) widget,(int) widget->parent,(int) table);
  }
  widget->parent=(APTR) table;

  DoMethod(table->MuiObject,MUIM_Group_InitChange);

  DebOut("tableattach=%lx\n",tableattach);
  DoMethod(table->MuiObject,MM_Table_Attach,(ULONG)widget, left_attach, right_attach, top_attach, bottom_attach);

  DoMethod(table->MuiObject,MUIM_Group_ExitChange);

  return;
}

void gtk_table_set_row_spacings(GtkTable *table, guint spacing) {

  DebOut("gtk_table_set_row_spacings(%lx,%ld)\n",table,spacing);

  SetAttrs(table->MuiObject,MUIA_Group_VertSpacing,(ULONG) spacing, TAG_DONE);
  return;
}

void gtk_table_set_col_spacings(GtkTable *table, guint spacing) {

  DebOut("gtk_table_set_row_spacings(%lx,%ld)\n",table,spacing);

  SetAttrs(table->MuiObject,MUIA_Group_HorizSpacing,(ULONG) spacing, TAG_DONE);
  return;
}
