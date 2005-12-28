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
 */

#ifndef __G_OBJECT_H__
#define __G_OBJECT_H__

#include <gtk/gtk.h>
#include <gdk/gobject/gtype.h>

//typedef struct _GTypeClass              GTypeClass;
typedef struct _GObjectConstructParam   GObjectConstructParam;
typedef struct _GObjectClass   GObjectClass;

//struct _GTypeClass
//{
    /*< private >*/
//      GType g_type;
//};

/* --- typedefs & structures --- */
typedef struct _GParamSpec      GParamSpec;
typedef struct _GParamSpecClass GParamSpecClass;
typedef struct _GParameter	GParameter;
typedef struct _GParamSpecPool  GParamSpecPool;

/* dummy */
//typedef unsigned int GTypeInstance ;

struct _GParamSpec
{
  GTypeInstance  g_type_instance;

  gchar         *name;
//  GParamFlags    flags;
  GType		 value_type;
  GType		 owner_type;	/* class or interface using this property */

  /*< private >*/
  gchar         *_nick;
  gchar         *_blurb;
//  GData		*qdata;
  guint          ref_count;
  guint		 param_id;	/* sort-criteria */
};

struct _GParamSpecClass
{
  GTypeClass      g_type_class;

  GType		  value_type;

  void	        (*finalize)		(GParamSpec   *pspec);

  /* GParam methods */
  void          (*value_set_default)    (GParamSpec   *pspec,
					 GValue       *value);
  gboolean      (*value_validate)       (GParamSpec   *pspec,
					 GValue       *value);
  gint          (*values_cmp)           (GParamSpec   *pspec,
					 const GValue *value1,
					 const GValue *value2);
  /*< private >*/
  gpointer	  dummy[4];
};

struct _GObjectConstructParam
{
  GParamSpec *pspec;
  GValue     *value;
};


struct  _GObjectClass
{
  GTypeClass   g_type_class;

  /*< private >*/
  GSList      *construct_properties;

  /*< public >*/
  /* overridable methods */
#if 0
  GObject*   (*constructor)     (GType                  type,
                                 guint                  n_construct_properties,
                                 GObjectConstructParam *construct_properties);
  void       (*set_property)		(GObject        *object,
                                         guint           property_id,
                                         const GValue   *value,
                                         GParamSpec     *pspec);
  void       (*get_property)		(GObject        *object,
                                         guint           property_id,
                                         GValue         *value,
                                         GParamSpec     *pspec);
  void       (*dispose)			(GObject        *object);
  void       (*finalize)		(GObject        *object);
  
  /* seldomly overidden */
  void       (*dispatch_properties_changed) (GObject      *object,
					     guint	   n_pspecs,
					     GParamSpec  **pspecs);

  /* signals */
  void	     (*notify)			(GObject	*object,
					 GParamSpec	*pspec);
#endif
  /*< private >*/
  /* padding */
  gpointer	pdummy[8];
};

#endif /* __G_OBJECT_H__ */
