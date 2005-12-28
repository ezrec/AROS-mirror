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
 * $Id: gtypes.h,v 1.3 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#ifndef __G_TYPES_H__
#define __G_TYPES_H__

typedef APTR gpointer;
typedef LONG guint32;
typedef char gchar;
typedef char gint8;
typedef unsigned char guint8;
typedef short gint16;
typedef unsigned short guint16;
typedef short gshort;
typedef unsigned short gushort;
typedef int gint;
typedef unsigned int guint;
typedef double  gdouble;
typedef int gboolean;
typedef long glong;
typedef unsigned long gulong;
typedef float gfloat;
typedef long int gint64; // is this correct? -oli
typedef unsigned long int guint64; // is this correct? -oli
typedef gulong GType;  // is this correct? -itix
typedef gulong GtkType;  // is this correct? -oli ;)
typedef guint32 GQuark;
typedef const void *gconstpointer;
typedef char GString;

#define G_PI_4  0.78539816339744830961566084581987572104929234984378

#endif /* __G_TYPES_H__ */

