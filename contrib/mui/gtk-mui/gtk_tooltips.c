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
 * $Id: gtk_tooltips.c,v 1.4 2005/12/04 00:08:10 o1i Exp $
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

GtkTooltips* GTK_TOOLTIPS(GtkWidget* widget) {
	return((GtkTooltips*) widget);
}

GtkTooltips* gtk_tooltips_new(void) {
	GtkWidget *ret;

	DebOut("gtk_tooltips_new()\n");

	ret=mgtk_widget_new(IS_TOOLTIPS);
	return ret;
}

void mgtk_tooltip_add(GtkTooltips *tooltips, GtkWidget *widget) {
	GSList *list;

	DebOut("mgtk_tooltip_add(%ld,%ld)\n");

	list=tooltips->GSList;
	list=(GSList *) g_slist_append(list,(gpointer) widget);
	tooltips->GSList=list;
}

void gtk_tooltips_set_tip(GtkTooltips *tooltips, GtkWidget *widget, const gchar *tip_text, const gchar *tip_private) {

	DebOut("gtk_tooltips_set_tip(%lx,%lx,%s,%s)\n",tooltips,widget,tip_text,tip_private);

	/*
	 * tip_private is *not* shown normally, the docs say:
	 *
	 * "tip_private is a string that is not shown as the default tooltip. 
	 * Instead, this message may be more informative and go towards forming 
	 * a context-sensitive help system for your application. 
	 * (FIXME: how to actually "switch on" private tips?)"
	 *
	 * so we ignore tip_private..
	 */

	widget->tip_text=g_strdup(tip_text);
	mgtk_tooltip_add(tooltips,widget);
	set(widget->MuiObject,MUIA_ShortHelp,(ULONG) widget->tip_text);
}

/*
 * gtk_tooltips_disable disables all attached
 * help bubbles. As I did not find any documentation
 * on how to disable a ShortHelp, I do a 
 * set MUIA_ShortHelp,NULL, which seems to work.
 */

void gtk_tooltips_disable(GtkTooltips *tooltips) {
	GSList *list;
	GtkWidget *widget;

	DebOut("gtk_tooltips_disable(%ld)\n",tooltips);

	list=tooltips->GSList;

	while(list!=NULL) {
		widget=(GtkWidget*) list->data;
		DebOut("  disable: widget %lx object %lx\n",widget,widget->MuiObject);
		set(widget->MuiObject,MUIA_ShortHelp,NULL);
		list=list->next;
	}
}

void gtk_tooltips_enable(GtkTooltips *tooltips) {
	GSList *list;
	GtkWidget *widget;

	DebOut("gtk_tooltips_enable(%ld)\n",tooltips);

	list=tooltips->GSList;

	while(list!=NULL) {
		widget=(GtkWidget*) list->data;
		DebOut("  enable: widget %lx object %lx\n",widget,widget->MuiObject);
		set(widget->MuiObject,MUIA_ShortHelp,(ULONG) widget->tip_text);
		list=list->next;
	}
}

/*
 * "Ensures that the window used for displaying the given tooltips is created.
 * Applications should never have to call this function, since GTK+ takes care of this."
 */
void gtk_tooltips_force_window(GtkTooltips *tooltips) {
	return;
}
