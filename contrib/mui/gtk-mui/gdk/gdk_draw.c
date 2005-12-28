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
 * $Id: gdk_draw.c,v 1.6 2005/12/12 16:18:58 itix Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "../debug.h"
#include <gtk/gtk.h>
#include "../gtk_globals.h"

#include "gdkmui.h"

/* pens (gc) is still TODO! */

void gdk_draw_line(GdkDrawable *drawable, GdkGC *gc, gint x1, gint y1, gint x2, gint y2) {
  GtkWidget *widget;
  Object *obj;

  DebOut("gdk_draw_line(%lx,%lx,%d,%d,%d,%d)\n",drawable,gc,x1,y1,x2,y2);

  if(!drawable) {
    DebOut("ERROR: drawable is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:drawable is NULL!\n");
    return;
  }

  if(!gc) {
    DebOut("ERROR: gc is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:gc is NULL!\n");
    return;
  }

  widget=(GtkWidget *) drawable->mgtk_widget;

  if(!widget) {
    DebOut("ERROR: drawable->mgtk_widget is NULL!\n");
    return;
  }
  obj=widget->MuiObject;

  if(!obj) {
    DebOut("ERROR: obj is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c: obj is NULL!\n");
    return;
  }

  gdkmui_set_color(_rp(obj), gc);
  Move(_rp(obj),_mleft(obj)+x1,_mtop(obj)+y1);
  Draw(_rp(obj),_mleft(obj)+x2,_mtop(obj)+y2);

}

void gdk_draw_lines(GdkDrawable *drawable, GdkGC *gc, GdkPoint *points, gint npoints) {

  gint i;

  DebOut("gdk_draw_lines(%lx,%lx,%lx,%d)\n",drawable,gc,points,npoints);

  if(npoints<2) {
    return;
  }

  for (i=1; i<npoints; i++) {
    gdk_draw_line(drawable,gc,points[i-1].x,points[i-1].y,points[i].x,points[i].y);
  }

}

void gdk_draw_rectangle(GdkDrawable *drawable, GdkGC *gc, gint filled, gint x, gint y, gint width, gint height) {
  GtkWidget *widget;
  Object *obj;


  DebOut("gdk_draw_rectangle(%lx,%lx,%d,%d,%d,%d,%d)\n",drawable,gc,filled,x,y,width,height);


  if(!drawable) {
    DebOut("ERROR: drawable is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:drawable is NULL!\n");
    return;
  }

  if(!gc) {
    DebOut("ERROR: gc is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c:gc is NULL!\n");
    return;
  }

#if 0
  DebOut("  gc:                            %lx\n",gc);
  DebOut("  gc->colormap:                  %lx\n",gc->colormap);
  DebOut("  gc->colormap->colors:          %lx\n",gc->colormap->colors);
  DebOut("  gc->colormap->colors->havepen: %lx\n",gc->colormap->colors->havepen);
#endif

  widget=(GtkWidget *) drawable->mgtk_widget;
  DebOut("  widget=%lx\n",widget);

  if(!widget) {
    DebOut("ERROR: drawable->mgtk_widget is NULL!\n");
    return;
  }
  obj=widget->MuiObject;

  if(!obj) {
    DebOut("ERROR: obj is NULL!\n");
    printf("ERROR: gdk/gdk_draw.c: obj is NULL!\n");
    return;
  }

#if 0
  DebOut("  obj=%lx\n",obj);
  DebOut("  rp(obj)=%lx\n",_rp(obj));
  DebOut("  dri(obj)=%lx\n",_dri(obj));
  DebOut("  mleft(obj)=%lx\n",_mleft(obj));
  DebOut("  mtop(obj)=%lx\n",_mtop(obj));
#endif

  gdkmui_set_color(_rp(obj), gc);
  Move(_rp(obj),_mleft(obj)+x,_mtop(obj)+y);
  Draw(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y);

  Move(_rp(obj),_mleft(obj)+x,_mtop(obj)+y);
  Draw(_rp(obj),_mleft(obj)+x,_mtop(obj)+y+height);
  
  Move(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y+height);
  Draw(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y);
  Move(_rp(obj),_mleft(obj)+x+width,_mtop(obj)+y+height);
  Draw(_rp(obj),_mleft(obj)+x,_mtop(obj)+y+height);

#if 0
  DebOut("  exit gdk_draw_rectangle\n");
#endif
}
