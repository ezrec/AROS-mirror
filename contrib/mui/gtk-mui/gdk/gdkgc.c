/* GDK - The GIMP Drawing Kit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

/* modified by o1i 2005 */

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "../debug.h"
#include <gtk/gtk.h>
#include "../gtk_globals.h"

/**
 * gdk_gc_set_dashes:
 * @gc: a #GdkGC.
 * @dash_offset: the phase of the dash pattern.
 * @dash_list: an array of dash lengths.
 * @n: the number of elements in @dash_list.
 * 
 * Sets the way dashed-lines are drawn. Lines will be
 * drawn with alternating on and off segments of the
 * lengths specified in @dash_list. The manner in
 * which the on and off segments are drawn is determined
 * by the @line_style value of the GC. (This can
 * be changed with gdk_gc_set_line_attributes().)
 *
 * The @dash_offset defines the phase of the pattern, 
 * specifying how many pixels into the dash-list the pattern 
 * should actually begin.
 **/
void
gdk_gc_set_dashes (GdkGC *gc,
		   gint	  dash_offset,
		   gint8  dash_list[],
		   gint   n)
{
  g_return_if_fail (GDK_IS_GC (gc));
  g_return_if_fail (dash_list != NULL);

  DebOut("WARNING: gdk_gc_set_dashes: TODO\n");
#warning gdk_gc_set_dashes: TODO

/*  GDK_GC_GET_CLASS (gc)->set_dashes (gc, dash_offset, dash_list, n);*/
}

/**
 * gdk_gc_set_line_attributes:
 * @gc: a #GdkGC.
 * @line_width: the width of lines.
 * @line_style: the dash-style for lines.
 * @cap_style: the manner in which the ends of lines are drawn.
 * @join_style: the in which lines are joined together.
 * 
 * Sets various attributes of how lines are drawn. See
 * the corresponding members of #GdkGCValues for full
 * explanations of the arguments.
 **/
void
gdk_gc_set_line_attributes (GdkGC	*gc,
			    gint	 line_width,
			    GdkLineStyle line_style,
			    GdkCapStyle	 cap_style,
			    GdkJoinStyle join_style)
{

#warning gdk_gc_set_line_attributes: TODO
  DebOut("WARNING: gdk_gc_set_line_attributes: TODO\n");

#if 0
  GdkGCValues values;

  values.line_width = line_width;
  values.line_style = line_style;
  values.cap_style = cap_style;
  values.join_style = join_style;

  gdk_gc_set_values (gc, &values,
		     GDK_GC_LINE_WIDTH |
		     GDK_GC_LINE_STYLE |
		     GDK_GC_CAP_STYLE |
		     GDK_GC_JOIN_STYLE);
#endif
}

