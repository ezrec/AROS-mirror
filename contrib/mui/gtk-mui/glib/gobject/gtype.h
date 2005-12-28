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
 * Modified by o1i
 */

#ifndef __G_TYPE_H__
#define __G_TYPE_H__

#include <gtk/gtk.h>

/* Basic Type Macros
 */
#define G_TYPE_FUNDAMENTAL(type)	(g_type_fundamental (type))
#define	G_TYPE_FUNDAMENTAL_MAX		(255 << G_TYPE_FUNDAMENTAL_SHIFT)

/* Constant fundamental types,
 * introduced by g_type_init().
 */
#define G_TYPE_INVALID			G_TYPE_MAKE_FUNDAMENTAL (0)
#define G_TYPE_NONE			G_TYPE_MAKE_FUNDAMENTAL (1)
#define G_TYPE_INTERFACE		G_TYPE_MAKE_FUNDAMENTAL (2)
#define G_TYPE_CHAR			G_TYPE_MAKE_FUNDAMENTAL (3)
#define G_TYPE_UCHAR			G_TYPE_MAKE_FUNDAMENTAL (4)
#define G_TYPE_BOOLEAN			G_TYPE_MAKE_FUNDAMENTAL (5)
#define G_TYPE_INT			G_TYPE_MAKE_FUNDAMENTAL (6)
#define G_TYPE_UINT			G_TYPE_MAKE_FUNDAMENTAL (7)
#define G_TYPE_LONG			G_TYPE_MAKE_FUNDAMENTAL (8)
#define G_TYPE_ULONG			G_TYPE_MAKE_FUNDAMENTAL (9)
#define G_TYPE_INT64			G_TYPE_MAKE_FUNDAMENTAL (10)
#define G_TYPE_UINT64			G_TYPE_MAKE_FUNDAMENTAL (11)
#define G_TYPE_ENUM			G_TYPE_MAKE_FUNDAMENTAL (12)
#define G_TYPE_FLAGS			G_TYPE_MAKE_FUNDAMENTAL (13)
#define G_TYPE_FLOAT			G_TYPE_MAKE_FUNDAMENTAL (14)
#define G_TYPE_DOUBLE			G_TYPE_MAKE_FUNDAMENTAL (15)
#define G_TYPE_STRING			G_TYPE_MAKE_FUNDAMENTAL (16)
#define G_TYPE_POINTER			G_TYPE_MAKE_FUNDAMENTAL (17)
#define G_TYPE_BOXED			G_TYPE_MAKE_FUNDAMENTAL (18)
#define G_TYPE_PARAM			G_TYPE_MAKE_FUNDAMENTAL (19)
#define G_TYPE_OBJECT			G_TYPE_MAKE_FUNDAMENTAL (20)

/* Reserved fundamental type numbers to create new fundamental
 * type IDs with G_TYPE_MAKE_FUNDAMENTAL().
 * Send email to gtk-devel-list@redhat.com for reservations.
 */
#define	G_TYPE_FUNDAMENTAL_SHIFT	(2)
#define	G_TYPE_MAKE_FUNDAMENTAL(x)	((GType) ((x) << G_TYPE_FUNDAMENTAL_SHIFT))
#define G_TYPE_RESERVED_GLIB_FIRST	(21)
#define G_TYPE_RESERVED_GLIB_LAST	(31)
#define G_TYPE_RESERVED_BSE_FIRST	(32)
#define G_TYPE_RESERVED_BSE_LAST	(48)
#define G_TYPE_RESERVED_USER_FIRST	(49)

typedef struct _GTypeInfo               GTypeInfo;
typedef struct _GValue                  GValue;
typedef struct _GTypeValueTable         GTypeValueTable;
typedef union  _GTypeCValue             GTypeCValue;
typedef struct _GTypeClass              GTypeClass;
typedef struct _GTypeInstance           GTypeInstance;


/* already defined in gtk.h
typedef gulong                          GType;
*/

typedef void   (*GBaseInitFunc)              (gpointer         g_class);
typedef void   (*GBaseFinalizeFunc)          (gpointer         g_class);
typedef void   (*GClassInitFunc)             (gpointer         g_class,
                gpointer         class_data);
typedef void   (*GClassFinalizeFunc)         (gpointer         g_class,
                gpointer         class_data);
typedef void   (*GInstanceInitFunc)          (GTypeInstance   *instance,
                gpointer         g_class);
typedef void   (*GInterfaceInitFunc)         (gpointer         g_iface,
                gpointer         iface_data);
typedef void   (*GInterfaceFinalizeFunc)     (gpointer         g_iface,
                gpointer         iface_data);
typedef gboolean (*GTypeClassCacheFunc)      (gpointer         cache_data,
                GTypeClass      *g_class);
typedef void     (*GTypeInterfaceCheckFunc)  (gpointer         check_data,
                gpointer         g_iface);

/* Basic Type Structures
  */
struct _GTypeClass
{
  /*< private >*/
  GType g_type;
};

struct _GTypeInstance
{
  /*< private >*/
  GTypeClass *g_class;
};

#define G_TYPE_FROM_CLASS(g_class) (((GTypeClass*) (g_class))->g_type) 

struct _GTypeValueTable
{
  void     (*value_init)         (GValue       *value);
  void     (*value_free)         (GValue       *value);
  void     (*value_copy)         (const GValue *src_value,
                                  GValue       *dest_value);
  /* varargs functionality (optional) */
  gpointer (*value_peek_pointer) (const GValue *value);
  gchar     *collect_format;
  gchar*   (*collect_value)      (GValue       *value,
  guint      n_collect_values,
  GTypeCValue  *collect_values,
  guint      collect_flags);
  gchar     *lcopy_format;
  gchar*   (*lcopy_value)        (const GValue *value,
                                  guint      n_collect_values,
                                  GTypeCValue  *collect_values,
                                  guint   collect_flags);
};

typedef enum    /*< skip >*/
{
  G_TYPE_FLAG_ABSTRACT    = (1 << 4),
  G_TYPE_FLAG_VALUE_ABSTRACT  = (1 << 5)
} GTypeFlags;

struct _GTypeInfo
{
  /* interface types, classed types, instantiated types */
  guint16                class_size;
  
  GBaseInitFunc          base_init;
  GBaseFinalizeFunc      base_finalize;
  
  /* interface types, classed types, instantiated types */
  GClassInitFunc         class_init;
  GClassFinalizeFunc     class_finalize;
  gconstpointer          class_data;
  
  /* instantiated types */
  guint16                instance_size;
  guint16                n_preallocs;
  GInstanceInitFunc      instance_init;
  
  /* value handling */
  const GTypeValueTable *value_table;
};

#endif /* __G_TYPE_H__ */
