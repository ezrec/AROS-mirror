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
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

#ifndef __AROS__
#define MUIA_Dtpic_Name 0x80423d72
#else
#warning I hacked this in here - Jack
#define MUIA_Dtpic_Name     (MUIB_MUI|0x00423d72) /* i.. STRPTR */
#endif

GtkWidget* gtk_image_new(void) {
  Object *pic;
  GtkWidget*ret;

  DebOut("gtk_image_new()\n");

  ret=mgtk_widget_new(IS_IMAGE);

  pic=(APTR) ImageObject,
    End;

  ret->MuiObject=pic;

  return ret;
}

GtkWidget* gtk_image_new_from_file(const gchar *filename) {

  Object *pic;
  GtkWidget*ret;
#ifdef __AROS__
  BPTR lock;
#else
  APTR lock;
#endif

  DebOut("gtk_image_new_from_file(%s)\n",filename);

  ret=mgtk_widget_new(IS_IMAGE);

  DebOut("Loading %s..\n",filename);

  lock=NULL;
  pic=NULL;
  if ((lock = Lock(filename, ACCESS_READ)) != NULL) {
    pic=(APTR) MUI_NewObject("Dtpic.mui",MUIA_Dtpic_Name,(ULONG) filename,TAG_DONE);
    UnLock(lock);
    DebOut("Loaded %s successfully\n",filename);
  }

  if(!pic) {
    pic=HVSpace;
    WarnOut("gtk_image_new_from_file: pic (%s) can't be loaded, we use a HVSpace instead.\n",filename);
  }

  ret->MuiObject=pic;

  return ret;
}

GdkPixbuf*  gdk_pixbuf_new_from_file(const char *filename, GError **error) {

  GError *newerror;

  newerror=(GError *) g_new(GError *,1);

  DebOut("gdk_pixbuf_new_from_file(%s)\n",filename);

  return (GdkPixbuf*) gtk_image_new_from_file(filename);
}

