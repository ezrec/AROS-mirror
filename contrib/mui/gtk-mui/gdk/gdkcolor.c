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
 * gdk_color_copy:
 * @color: a #GdkColor.
 * 
 * Makes a copy of a color structure. The result
 * must be freed using gdk_color_free().
 * 
 * Return value: a copy of @color.
 **/
GdkColor*
gdk_color_copy (const GdkColor *color)
{
  GdkColor *new_color;
  
  g_return_val_if_fail (color != NULL, NULL);

  new_color = g_new (GdkColor,1);

  *new_color = *color;

  return new_color;
}

/**
 * gdk_color_free:
 * @color: a #GdkColor.
 * 
 * Frees a color structure created with 
 * gdk_color_copy().
 **/
void
gdk_color_free (GdkColor *color)
{
  g_return_if_fail (color != NULL);

  g_free (color);
}
