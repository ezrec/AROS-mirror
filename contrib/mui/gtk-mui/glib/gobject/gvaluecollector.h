/* GObject - GLib Type, Object, Parameter and Signal Library
 * Copyright (C) 1998-1999, 2000-2001 Tim Janik and Red Hat, Inc.
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
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * gvaluecollector.h: GValue varargs stubs
 *
 * modified by o1i
 */
#ifndef __G_VALUE_COLLECTOR_H__
#define __G_VALUE_COLLECTOR_H__

#include <gtk/gtk.h>


/* we may want to add aggregate types here some day, if requested
 * by users. the basic C types are covered already, everything
 * smaller than an int is promoted to an integer and floats are
 * always promoted to doubles for varargs call constructions.
 */
enum	/*< skip >*/
{
  G_VALUE_COLLECT_INT		= 'i',
  G_VALUE_COLLECT_LONG		= 'l',
  G_VALUE_COLLECT_INT64         = 'q',
  G_VALUE_COLLECT_DOUBLE	= 'd',
  G_VALUE_COLLECT_POINTER	= 'p'
};


/* vararg union holding actuall values collected
 */
union _GTypeCValue
{
  gint     v_int;
  glong    v_long;
  gint64   v_int64;
  gdouble  v_double;
  gpointer v_pointer;
};

#endif /* __G_VALUE_COLLECTOR_H__ */
