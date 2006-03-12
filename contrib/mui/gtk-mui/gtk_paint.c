/* GTK - The GIMP Toolkit
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

/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Contact information:
 *
 * modified by Oliver Brunner 2005
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id$
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <math.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

#define INDICATOR_PART_SIZE 13


typedef enum {
  CHECK_AA,
  CHECK_BASE,
  CHECK_BLACK,
  CHECK_DARK,
  CHECK_LIGHT,
  CHECK_MID,
  CHECK_TEXT,
  CHECK_INCONSISTENT_TEXT,
  RADIO_BASE,
  RADIO_BLACK,
  RADIO_DARK,
  RADIO_LIGHT,
  RADIO_MID,
  RADIO_TEXT,
  RADIO_INCONSISTENT_AA,
  RADIO_INCONSISTENT_TEXT
} IndicatorPart;

void gtk_paint_shadow (GtkStyle     *style, GdkWindow    *window, GtkStateType  state_type, GtkShadowType shadow_type, GdkRectangle *area, GtkWidget    *widget, const gchar  *detail, gint          x, gint          y, gint          width, gint          height);

void        gdk_gc_set_clip_rectangle       (GdkGC *gc,
                                             GdkRectangle *rectangle) {
#warning gdk_gc_set_clip_rectangle is TODO

  DebOut("WARNING: gdk_gc_set_clip_rectangle is TODO\n");
}


static GdkGC *create_aa_gc (GdkWindow *window, GtkStyle *style, GtkStateType state_type) {

  GdkColor aa_color;
  GdkGC *gc = gdk_gc_new (window);

  aa_color.red = (style->fg[state_type].red + style->bg[state_type].red) / 2;
  aa_color.green = (style->fg[state_type].green + style->bg[state_type].green) / 2;
  aa_color.blue = (style->fg[state_type].blue + style->bg[state_type].blue) / 2;
  
  gdk_gc_set_rgb_fg_color (gc, &aa_color);

  return gc;
}


static void sanitize_size (GdkWindow *window, gint *width, gint *height) {

/*
  if ((*width == -1) && (*height == -1))
    gdk_drawable_get_size (window, width, height);
  else if (*width == -1)
    gdk_drawable_get_size (window, width, NULL);
  else if (*height == -1)
    gdk_drawable_get_size (window, NULL, height);
*/
}

static GtkTextDirection
get_direction (GtkWidget *widget)
{
  GtkTextDirection dir;
  
  /*
  if (widget)
    dir = gtk_widget_get_direction (widget);
  else
    */
  dir = GTK_TEXT_DIR_LTR;
  
  return dir;
}

void gtk_paint_polygon(GtkStyle *style, GdkWindow *window, GtkStateType state_type, GtkShadowType shadow_type, GdkRectangle *area, GtkWidget *widget, const gchar *detail, GdkPoint *points, gint npoints, gboolean fill) {

  static const gdouble pi_over_4 = G_PI_4;
  static const gdouble pi_3_over_4 = G_PI_4 * 3;
  GdkGC *gc1;
  GdkGC *gc2;
  GdkGC *gc3;
  GdkGC *gc4;
  gdouble angle;
  gint xadjust;
  gint yadjust;
  gint i;
  
  DebOut("gtk_paint_polygon(%lx,%lx,%lx,..)\n",style,widget,window);

  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  g_return_if_fail (points != NULL);
  

  /* taken from "gtk/gtkstyle.c" of original gtk sources (gtk_default_draw_polygon) */
  switch (shadow_type)
    {
    case GTK_SHADOW_IN:
      gc1 = style->bg_gc[state_type];
      gc2 = style->dark_gc[state_type]; 
      gc3 = style->light_gc[state_type];
      gc4 = style->black_gc;
      break;
    case GTK_SHADOW_ETCHED_IN:
      gc1 = style->light_gc[state_type];
      gc2 = style->dark_gc[state_type]; 
      gc3 = style->dark_gc[state_type]; 
      gc4 = style->light_gc[state_type];
      break;
    case GTK_SHADOW_OUT:
      gc1 = style->dark_gc[state_type]; 
      gc2 = style->light_gc[state_type]; 
      gc3 = style->black_gc; 
      gc4 = style->bg_gc[state_type]; 
      break;
    case GTK_SHADOW_ETCHED_OUT:
      gc1 = style->dark_gc[state_type]; 
      gc2 = style->light_gc[state_type]; 
      gc3 = style->light_gc[state_type];
      gc4 = style->dark_gc[state_type]; 
      break;
    default:
      return;
    }

#ifdef GDK_GC_SET_CLIP_RECTANGLE
  if (area)
    {
      gdk_gc_set_clip_rectangle (gc1, area);
      gdk_gc_set_clip_rectangle (gc2, area);
      gdk_gc_set_clip_rectangle (gc3, area);
      gdk_gc_set_clip_rectangle (gc4, area);
    }
#endif
  
  /*
  if (fill)
    gdk_draw_polygon (window, style->bg_gc[state_type], TRUE, points, npoints);
    */
  
  npoints--;
  
  for (i = 0; i < npoints; i++)
    {
      if ((points[i].x == points[i+1].x) &&
          (points[i].y == points[i+1].y))
        {
          angle = 0;
        }
      else
        {
          angle = atan2 (points[i+1].y - points[i].y,
                         points[i+1].x - points[i].x);
        }
      
      if ((angle > -pi_3_over_4) && (angle < pi_over_4))
        {
          if (angle > -pi_over_4)
            {
              xadjust = 0;
              yadjust = 1;
            }
          else
            {
              xadjust = 1;
              yadjust = 0;
            }
          
          gdk_draw_line (window, gc1,
                         points[i].x-xadjust, points[i].y-yadjust,
                         points[i+1].x-xadjust, points[i+1].y-yadjust);
          gdk_draw_line (window, gc3,
                         points[i].x, points[i].y,
                         points[i+1].x, points[i+1].y);
        }
      else
        {
          if ((angle < -pi_3_over_4) || (angle > pi_3_over_4))
            {
              xadjust = 0;
              yadjust = 1;
            }
          else
            {
              xadjust = 1;
              yadjust = 0;
            }
          
          gdk_draw_line (window, gc4,
                         points[i].x+xadjust, points[i].y+yadjust,
                         points[i+1].x+xadjust, points[i+1].y+yadjust);
          gdk_draw_line (window, gc2,
                         points[i].x, points[i].y,
                         points[i+1].x, points[i+1].y);
        }
    }

#ifdef GDK_GC_SET_CLIP_RECTANGLE
  if (area)
    {
      gdk_gc_set_clip_rectangle (gc1, NULL);
      gdk_gc_set_clip_rectangle (gc2, NULL);
      gdk_gc_set_clip_rectangle (gc3, NULL);
      gdk_gc_set_clip_rectangle (gc4, NULL);
    }
#endif
}

static void draw_arrow (GdkWindow *window, GdkGC *gc, GdkRectangle *area, GtkArrowType arrow_type, gint x, gint y, gint width, gint height) {
  gint i, j;

  if (area)
    gdk_gc_set_clip_rectangle (gc, area);

  if (arrow_type == GTK_ARROW_DOWN)
    {
      for (i = 0, j = 0; i < height; i++, j++)
	gdk_draw_line (window, gc, x + j, y + i, x + width - j - 1, y + i);
    }
  else if (arrow_type == GTK_ARROW_UP)
    {
      for (i = height - 1, j = 0; i >= 0; i--, j++)
	gdk_draw_line (window, gc, x + j, y + i, x + width - j - 1, y + i);
    }
  else if (arrow_type == GTK_ARROW_LEFT)
    {
      for (i = width - 1, j = 0; i >= 0; i--, j++)
	gdk_draw_line (window, gc, x + i, y + j, x + i, y + height - j - 1);
    }
  else if (arrow_type == GTK_ARROW_RIGHT)
    {
      for (i = 0, j = 0; i < width; i++, j++)
	gdk_draw_line (window, gc, x + i, y + j, x + i, y + height - j - 1);
    }

  if (area)
    gdk_gc_set_clip_rectangle (gc, NULL);
}


static void
calculate_arrow_geometry (GtkArrowType  arrow_type,
			  gint         *x,
			  gint         *y,
			  gint         *width,
			  gint         *height) {

  gint w = *width;
  gint h = *height;
  
  switch (arrow_type)
    {
    case GTK_ARROW_UP:
    case GTK_ARROW_DOWN:
      w += (w % 2) - 1;
      h = (w / 2 + 1);
      
      if (h > *height)
	{
	  h = *height;
	  w = 2 * h - 1;
	}
      
      if (arrow_type == GTK_ARROW_DOWN)
	{
	  if (*height % 2 == 1 || h % 2 == 0)
	    *height += 1;
	}
      else
	{
	  if (*height % 2 == 0 || h % 2 == 0)
	    *height -= 1;
	}
      break;

    case GTK_ARROW_RIGHT:
    case GTK_ARROW_LEFT:
      h += (h % 2) - 1;
      w = (h / 2 + 1);
      
      if (w > *width)
	{
	  w = *width;
	  h = 2 * w - 1;
	}
      
      if (arrow_type == GTK_ARROW_RIGHT)
	{
	  if (*width % 2 == 1 || w % 2 == 0)
	    *width += 1;
	}
      else
	{
	  if (*width % 2 == 0 || w % 2 == 0)
	    *width -= 1;
	}
      break;
      
    default:
      /* should not be reached */
      break;
    }

  *x += (*width - w) / 2;
  *y += (*height - h) / 2;
  *height = h;
  *width = w;
}

static void
rgb_to_hls (gdouble *r,
            gdouble *g,
            gdouble *b)
{
  gdouble min;
  gdouble max;
  gdouble red;
  gdouble green;
  gdouble blue;
  gdouble h, l, s;
  gdouble delta;
  
  red = *r;
  green = *g;
  blue = *b;
  
  if (red > green)
    {
      if (red > blue)
        max = red;
      else
        max = blue;
      
      if (green < blue)
        min = green;
      else
        min = blue;
    }
  else
    {
      if (green > blue)
        max = green;
      else
        max = blue;
      
      if (red < blue)
        min = red;
      else
        min = blue;
    }
  
  l = (max + min) / 2;
  s = 0;
  h = 0;
  
  if (max != min)
    {
      if (l <= 0.5)
        s = (max - min) / (max + min);
      else
        s = (max - min) / (2 - max - min);
      
      delta = max -min;
      if (red == max)
        h = (green - blue) / delta;
      else if (green == max)
        h = 2 + (blue - red) / delta;
      else if (blue == max)
        h = 4 + (red - green) / delta;
      
      h *= 60;
      if (h < 0.0)
        h += 360;
    }
  
  *r = h;
  *g = l;
  *b = s;
}

static void
hls_to_rgb (gdouble *h,
            gdouble *l,
            gdouble *s)
{
  gdouble hue;
  gdouble lightness;
  gdouble saturation;
  gdouble m1, m2;
  gdouble r, g, b;
  
  lightness = *l;
  saturation = *s;
  
  if (lightness <= 0.5)
    m2 = lightness * (1 + saturation);
  else
    m2 = lightness + saturation - lightness * saturation;
  m1 = 2 * lightness - m2;
  
  if (saturation == 0)
    {
      *h = lightness;
      *l = lightness;
      *s = lightness;
    }
  else
    {
      hue = *h + 120;
      while (hue > 360)
        hue -= 360;
      while (hue < 0)
        hue += 360;
      
      if (hue < 60)
        r = m1 + (m2 - m1) * hue / 60;
      else if (hue < 180)
        r = m2;
      else if (hue < 240)
        r = m1 + (m2 - m1) * (240 - hue) / 60;
      else
        r = m1;
      
      hue = *h;
      while (hue > 360)
        hue -= 360;
      while (hue < 0)
        hue += 360;
      
      if (hue < 60)
        g = m1 + (m2 - m1) * hue / 60;
      else if (hue < 180)
        g = m2;
      else if (hue < 240)
        g = m1 + (m2 - m1) * (240 - hue) / 60;
      else
        g = m1;
      
      hue = *h - 120;
      while (hue > 360)
        hue -= 360;
      while (hue < 0)
        hue += 360;
      
      if (hue < 60)
        b = m1 + (m2 - m1) * hue / 60;
      else if (hue < 180)
        b = m2;
      else if (hue < 240)
        b = m1 + (m2 - m1) * (240 - hue) / 60;
      else
        b = m1;
      
      *h = r;
      *l = g;
      *s = b;
    }
}
static void
gtk_style_shade (GdkColor *a,
                 GdkColor *b,
                 gdouble   k)
{
  gdouble red;
  gdouble green;
  gdouble blue;
  
  red = (gdouble) a->red / 65535.0;
  green = (gdouble) a->green / 65535.0;
  blue = (gdouble) a->blue / 65535.0;
  
  rgb_to_hls (&red, &green, &blue);
  
  green *= k;
  if (green > 1.0)
    green = 1.0;
  else if (green < 0.0)
    green = 0.0;
  
  blue *= k;
  if (blue > 1.0)
    blue = 1.0;
  else if (blue < 0.0)
    blue = 0.0;
  
  hls_to_rgb (&red, &green, &blue);
  
  b->red = red * 65535.0;
  b->green = green * 65535.0;
  b->blue = blue * 65535.0;
}


static GdkGC*
get_darkened_gc (GdkWindow *window,
                 GdkColor  *color,
                 gint       darken_count)
{
  GdkColor src = *color;
  GdkColor shaded = *color;
  GdkGC *gc;
  
  gc = gdk_gc_new (window);

  while (darken_count)
    {
      gtk_style_shade (&src, &shaded, 0.93);
      src = shaded;
      --darken_count;
    }
   
  gdk_gc_set_rgb_fg_color (gc, &shaded);

  return gc;
}

static void gtk_default_draw_arrow (GtkStyle      *style,
			GdkWindow     *window,
			GtkStateType   state,
			GtkShadowType  shadow,
			GdkRectangle  *area,
			GtkWidget     *widget,
			const gchar   *detail,
			GtkArrowType   arrow_type,
			gboolean       fill,
			gint           x,
			gint           y,
			gint           width,
			gint           height) {

  gint original_width, original_x;
  
  sanitize_size (window, &width, &height);

  original_width = width;
  original_x = x;

  calculate_arrow_geometry (arrow_type, &x, &y, &width, &height);

  if (detail && strcmp (detail, "menu_scroll_arrow_up") == 0)
    y++;

  if (state == GTK_STATE_INSENSITIVE)
    draw_arrow (window, style->white_gc, area, arrow_type,
		x + 1, y + 1, width, height);
  draw_arrow (window, style->fg_gc[state], area, arrow_type,
	      x, y, width, height);
}

/**
 * gtk_paint_arrow:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: the type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @arrow_type: the type of arrow to draw
 * @fill: %TRUE if the arrow tip should be filled
 * @x: x origin of the rectangle to draw the arrow in
 * @y: y origin of the rectangle to draw the arrow in
 * @width: width of the rectangle to draw the arrow in
 * @height: height of the rectangle to draw the arrow in
 * 
 * Draws an arrow in the given rectangle on @window using the given 
 * parameters. @arrow_type determines the direction of the arrow.
 */
void
gtk_paint_arrow (GtkStyle      *style,
                 GdkWindow     *window,
                 GtkStateType   state_type,
                 GtkShadowType  shadow_type,
                 GdkRectangle  *area,
                 GtkWidget     *widget,
                 const gchar   *detail,
                 GtkArrowType   arrow_type,
                 gboolean       fill,
                 gint           x,
                 gint           y,
                 gint           width,
                 gint           height) {

  g_return_if_fail (GTK_IS_STYLE (style));
  
  gtk_default_draw_arrow (style, window, state_type, shadow_type, area, widget, detail, arrow_type, fill, x, y, width, height);
}

static void 
gtk_default_draw_box (GtkStyle      *style,
		      GdkWindow     *window,
		      GtkStateType   state_type,
		      GtkShadowType  shadow_type,
		      GdkRectangle  *area,
		      GtkWidget     *widget,
		      const gchar   *detail,
		      gint           x,
		      gint           y,
		      gint           width,
		      gint           height)
{
  GdkGC *gc;
  gboolean is_spinbutton_box = FALSE;
  
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  sanitize_size (window, &width, &height);

#if 0 /* spinbuttons are done different in GTK-MUI anyways */
  if (widget && GTK_IS_SPIN_BUTTON (widget) && detail) {
      if (strcmp (detail, "spinbutton_up") == 0)
	{
	  y += 2;
	  width -= 3;
	  height -= 2;

	  if (get_direction (widget) == GTK_TEXT_DIR_RTL)
	    x += 2;
	  else
	    x += 1;

	  is_spinbutton_box = TRUE;
	}
      else if (strcmp (detail, "spinbutton_down") == 0)
	{
	  width -= 3;
	  height -= 2;

	  if (get_direction (widget) == GTK_TEXT_DIR_RTL)
	    x += 2;
	  else
	    x += 1;

	  is_spinbutton_box = TRUE;
	}
    }
#endif
  
#if 0
  if (!style->bg_pixmap[state_type]  || 
      GDK_IS_PIXMAP (window) ) {

    GdkGC *gc = (GdkGC *) BACKGROUNDPEN; //style->bg_gc[state_type];
      
    if (state_type == GTK_STATE_SELECTED && detail && strcmp (detail, "paned") == 0) {

      if (widget && !GTK_WIDGET_HAS_FOCUS (widget))
        gc = style->base_gc[GTK_STATE_ACTIVE];
    }
#ifdef GDK_GC_SET_CLIP_RECTANGLE
      if (area)
        gdk_gc_set_clip_rectangle (gc, area);
#endif
    gdk_draw_rectangle (window, gc, TRUE, x, y, width, height);
#ifdef GDK_GC_SET_CLIP_RECTANGLE
      if (area)
        gdk_gc_set_clip_rectangle (gc, NULL);
#endif
  }
  else
#endif /* 0 */
    gc = style->bg_gc[state_type];
    gdk_draw_rectangle (window, gc, TRUE, x, y, width, height);

#warning gtk_style_apply_default_background is TODO!
/*
    gtk_style_apply_default_background (style, window,
                                        widget && !GTK_WIDGET_NO_WINDOW (widget),
                                        state_type, area, x, y, width, height);
    */

  if (is_spinbutton_box)
    {
      GdkGC *upper_gc;
      GdkGC *lower_gc;
      
      lower_gc = style->dark_gc[state_type];
      if (shadow_type == GTK_SHADOW_OUT)
	upper_gc = style->light_gc[state_type];
      else
	upper_gc = style->dark_gc[state_type];

#ifdef GDK_GC_SET_CLIP_RECTANGLE
      if (area) {
        gdk_gc_set_clip_rectangle (style->dark_gc[state_type], area);
        gdk_gc_set_clip_rectangle (style->light_gc[state_type], area);
      }
#endif
      
      gdk_draw_line (window, upper_gc, x, y, x + width - 1, y);
      gdk_draw_line (window, lower_gc, x, y + height - 1, x + width - 1, y + height - 1);

#ifdef GDK_GC_SET_CLIP_RECTANGLE
      if (area) {
        gdk_gc_set_clip_rectangle (style->dark_gc[state_type], NULL);
        gdk_gc_set_clip_rectangle (style->light_gc[state_type], NULL);
      }
#endif
      return;
    }

  gtk_paint_shadow (style, window, state_type, shadow_type, area, widget, detail,
                    x, y, width, height);

#ifdef MGTK_HAS_OPTIONMENU
  if (detail && strcmp (detail, "optionmenu") == 0)
    {
      GtkRequisition indicator_size;
      GtkBorder indicator_spacing;
      gint vline_x;

      option_menu_get_props (widget, &indicator_size, &indicator_spacing);

      sanitize_size (window, &width, &height);

      if (get_direction (widget) == GTK_TEXT_DIR_RTL)
	vline_x = x + indicator_size.width + indicator_spacing.left + indicator_spacing.right;
      else 
	vline_x = x + width - (indicator_size.width + indicator_spacing.left + indicator_spacing.right) - style->xthickness;

      gtk_paint_vline (style, window, state_type, area, widget,
		       detail,
		       y + style->ythickness + 1,
		       y + height - style->ythickness - 3,
		       vline_x);
    }
#endif
}

/**
 * gtk_paint_box:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: the type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: x origin of the box
 * @y: y origin of the box
 * @width: the width of the box
 * @height: the height of the box
 * 
 * Draws a box on @window with the given parameters.
 */
void
gtk_paint_box (GtkStyle      *style,
               GdkWindow     *window,
               GtkStateType   state_type,
               GtkShadowType  shadow_type,
               GdkRectangle  *area,
               GtkWidget     *widget,
               const gchar   *detail,
               gint           x,
               gint           y,
               gint           width,
               gint           height)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  
  gtk_default_draw_box (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
}

static void
draw_thin_shadow (GtkStyle      *style,
		  GdkWindow     *window,
		  GtkStateType   state,
		  GdkRectangle  *area,
		  gint           x,
		  gint           y,
		  gint           width,
		  gint           height)
{
  GdkGC *gc1, *gc2;

  sanitize_size (window, &width, &height);
  
  gc1 = style->light_gc[state];
  gc2 = style->dark_gc[state];
  
#ifdef GDK_GC_SET_CLIP_RECTANGLE
  if (area)
    {
      gdk_gc_set_clip_rectangle (gc1, area);
      gdk_gc_set_clip_rectangle (gc2, area);
    }
#endif
  
  gdk_draw_line (window, gc1,
		 x, y + height - 1, x + width - 1, y + height - 1);
  gdk_draw_line (window, gc1,
		 x + width - 1, y,  x + width - 1, y + height - 1);
      
  gdk_draw_line (window, gc2,
		 x, y, x + width - 2, y);
  gdk_draw_line (window, gc2,
		 x, y, x, y + height - 2);

#ifdef GDK_GC_SET_CLIP_RECTANGLE
  if (area)
    {
      gdk_gc_set_clip_rectangle (gc1, NULL);
      gdk_gc_set_clip_rectangle (gc2, NULL);
    }
#endif
}

static void
draw_spinbutton_shadow (GtkStyle        *style,
			GdkWindow       *window,
			GtkStateType     state,
			GtkTextDirection direction,
			GdkRectangle    *area,
			gint             x,
			gint             y,
			gint             width,
			gint             height)
{
  sanitize_size (window, &width, &height);

  /* spinbuttons are done different */

}

static void
draw_menu_shadow (GtkStyle        *style,
		  GdkWindow       *window,
		  GtkStateType     state,
		  GdkRectangle    *area,
		  gint             x,
		  gint             y,
		  gint             width,
		  gint             height)
{

  /* menus are done different */
}

static void
gtk_default_draw_shadow (GtkStyle      *style,
                         GdkWindow     *window,
                         GtkStateType   state_type,
                         GtkShadowType  shadow_type,
                         GdkRectangle  *area,
                         GtkWidget     *widget,
                         const gchar   *detail,
                         gint           x,
                         gint           y,
                         gint           width,
                         gint           height) {

  GdkGC *gc1 = NULL;
  GdkGC *gc2 = NULL;
  gint thickness_light;
  gint thickness_dark;
  gint i;
  
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);

  if (shadow_type == GTK_SHADOW_IN)
    {
#if 0 /* no buttons ! */
      if (detail && (strcmp (detail, "buttondefault") == 0)) {
	  sanitize_size (window, &width, &height);

	  gdk_draw_rectangle (window, style->black_gc, FALSE,
			      x, y, width - 1, height - 1);
	  
	  return;
	}
#endif
      if (detail && strcmp (detail, "trough") == 0)
	{
	  draw_thin_shadow (style, window, state_type, area,
			    x, y, width, height);
	  return;
	}
     /* if (widget && GTK_IS_SPIN_BUTTON (widget) &&*/
      if (widget && FALSE &&
         detail && strcmp (detail, "spinbutton") == 0)
	{
	  draw_spinbutton_shadow (style, window, state_type, 
				  get_direction (widget), area, x, y, width, height);
	  
	  return;
	}
    }

  if (shadow_type == GTK_SHADOW_OUT && detail && strcmp (detail, "menu") == 0)
    {
      draw_menu_shadow (style, window, state_type, area, x, y, width, height);
      return;
    }
  
  sanitize_size (window, &width, &height);
  
  switch (shadow_type)
    {
    case GTK_SHADOW_NONE:
      return;
    case GTK_SHADOW_IN:
    case GTK_SHADOW_ETCHED_IN:
      gc1 = style->light_gc[state_type];
      gc2 = style->dark_gc[state_type];
      break;
    case GTK_SHADOW_OUT:
    case GTK_SHADOW_ETCHED_OUT:
      gc1 = style->dark_gc[state_type];
      gc2 = style->light_gc[state_type];
      break;
    }
  
  if (area)
    {
#ifdef GDK_GC_SET_CLIP_RECTANGLE
      gdk_gc_set_clip_rectangle (gc1, area);
      gdk_gc_set_clip_rectangle (gc2, area);
#endif
      if (shadow_type == GTK_SHADOW_IN || 
          shadow_type == GTK_SHADOW_OUT)
        {
#ifdef GDK_GC_SET_CLIP_RECTANGLE
          gdk_gc_set_clip_rectangle (style->black_gc, area);
          gdk_gc_set_clip_rectangle (style->bg_gc[state_type], area);
#endif
        }
    }
  
  switch (shadow_type)
    {
    case GTK_SHADOW_NONE:
      break;
      
    case GTK_SHADOW_IN:
      /* Light around right and bottom edge */

      if (style->ythickness > 0)
        gdk_draw_line (window, gc1,
                       x, y + height - 1, x + width - 1, y + height - 1);
      if (style->xthickness > 0)
        gdk_draw_line (window, gc1,
                       x + width - 1, y, x + width - 1, y + height - 1);

      if (style->ythickness > 1)
        gdk_draw_line (window, style->bg_gc[state_type],
                       x + 1, y + height - 2, x + width - 2, y + height - 2);
      if (style->xthickness > 1)
        gdk_draw_line (window, style->bg_gc[state_type],
                       x + width - 2, y + 1, x + width - 2, y + height - 2);

      /* Dark around left and top */

      if (style->ythickness > 1)
        gdk_draw_line (window,style->black_gc,
                       x + 1, y + 1, x + width - 2, y + 1);
      if (style->xthickness > 1)
        gdk_draw_line (window, style->black_gc,
                       x + 1, y + 1, x + 1, y + height - 2);

      if (style->ythickness > 0)
        gdk_draw_line (window, gc2,
                       x, y, x + width - 1, y);
      if (style->xthickness > 0)
        gdk_draw_line (window, gc2,
                       x, y, x, y + height - 1);
      break;
      
    case GTK_SHADOW_OUT:
      /* Dark around right and bottom edge */

      if (style->ythickness > 0)
        {
          if (style->ythickness > 1)
            {
              gdk_draw_line (window, gc1,
                             x + 1, y + height - 2, x + width - 2, y + height - 2);
              gdk_draw_line (window, style->black_gc,
                             x, y + height - 1, x + width - 1, y + height - 1);
            }
          else
            {
              gdk_draw_line (window, gc1,
                             x + 1, y + height - 1, x + width - 1, y + height - 1);
            }
        }

      if (style->xthickness > 0)
        {
          if (style->xthickness > 1)
            {
              gdk_draw_line (window, gc1,
                             x + width - 2, y + 1, x + width - 2, y + height - 2);
              
              gdk_draw_line (window, style->black_gc,
                             x + width - 1, y, x + width - 1, y + height - 1);
            }
          else
            {
              gdk_draw_line (window, gc1,
                             x + width - 1, y + 1, x + width - 1, y + height - 1);
            }
        }
      
      /* Light around top and left */

      if (style->ythickness > 0)
        gdk_draw_line (window, gc2,
                       x, y, x + width - 2, y);
      if (style->xthickness > 0)
        gdk_draw_line (window, gc2,
                       x, y, x, y + height - 2);

      if (style->ythickness > 1)
        gdk_draw_line (window, style->bg_gc[state_type],
                       x + 1, y + 1, x + width - 3, y + 1);
      if (style->xthickness > 1)
        gdk_draw_line (window, style->bg_gc[state_type],
                       x + 1, y + 1, x + 1, y + height - 3);
      break;
      
    case GTK_SHADOW_ETCHED_IN:
    case GTK_SHADOW_ETCHED_OUT:
      if (style->xthickness > 0)
        {
          if (style->xthickness > 1)
            {
              thickness_light = 1;
              thickness_dark = 1;
      
              for (i = 0; i < thickness_dark; i++)
                {
                  gdk_draw_line (window, gc1,
                                 x + width - i - 1,
                                 y + i,
                                 x + width - i - 1,
                                 y + height - i - 1);
                  gdk_draw_line (window, gc2,
                                 x + i,
                                 y + i,
                                 x + i,
                                 y + height - i - 2);
                }
      
              for (i = 0; i < thickness_light; i++)
                {
                  gdk_draw_line (window, gc1,
                                 x + thickness_dark + i,
                                 y + thickness_dark + i,
                                 x + thickness_dark + i,
                                 y + height - thickness_dark - i - 1);
                  gdk_draw_line (window, gc2,
                                 x + width - thickness_light - i - 1,
                                 y + thickness_dark + i,
                                 x + width - thickness_light - i - 1,
                                 y + height - thickness_light - 1);
                }
            }
          else
            {
              gdk_draw_line (window, 
                             style->dark_gc[state_type],
                             x, y, x, y + height);                         
              gdk_draw_line (window, 
                             style->dark_gc[state_type],
                             x + width, y, x + width, y + height);
            }
        }

      if (style->ythickness > 0)
        {
          if (style->ythickness > 1)
            {
              thickness_light = 1;
              thickness_dark = 1;
      
              for (i = 0; i < thickness_dark; i++)
                {
                  gdk_draw_line (window, gc1,
                                 x + i,
                                 y + height - i - 1,
                                 x + width - i - 1,
                                 y + height - i - 1);
          
                  gdk_draw_line (window, gc2,
                                 x + i,
                                 y + i,
                                 x + width - i - 2,
                                 y + i);
                }
      
              for (i = 0; i < thickness_light; i++)
                {
                  gdk_draw_line (window, gc1,
                                 x + thickness_dark + i,
                                 y + thickness_dark + i,
                                 x + width - thickness_dark - i - 2,
                                 y + thickness_dark + i);
          
                  gdk_draw_line (window, gc2,
                                 x + thickness_dark + i,
                                 y + height - thickness_light - i - 1,
                                 x + width - thickness_light - 1,
                                 y + height - thickness_light - i - 1);
                }
            }
          else
            {
              gdk_draw_line (window, 
                             style->dark_gc[state_type],
                             x, y, x + width, y);
              gdk_draw_line (window, 
                             style->dark_gc[state_type],
                             x, y + height, x + width, y + height);
            }
        }
      
      break;
    }

#if 0 /* SPIN_BUTTON are not drawn.. */
  if (shadow_type == GTK_SHADOW_IN &&
      widget && GTK_IS_SPIN_BUTTON (widget) &&
      detail && strcmp (detail, "entry") == 0)
    {
      if (get_direction (widget) == GTK_TEXT_DIR_LTR)
	{
	  gdk_draw_line (window,
			 style->base_gc[state_type],
			 x + width - 1, y + 2,
			 x + width - 1, y + height - 3);
	  gdk_draw_line (window,
			 style->base_gc[state_type],
			 x + width - 2, y + 2,
			 x + width - 2, y + height - 3);
	  gdk_draw_point (window,
			  style->black_gc,
			  x + width - 1, y + 1);
	  gdk_draw_point (window,
			  style->bg_gc[state_type],
			  x + width - 1, y + height - 2);
	}
      else
	{
	  gdk_draw_line (window,
			 style->base_gc[state_type],
			 x, y + 2,
			 x, y + height - 3);
	  gdk_draw_line (window,
			 style->base_gc[state_type],
			 x + 1, y + 2,
			 x + 1, y + height - 3);
	  gdk_draw_point (window,
			  style->black_gc,
			  x, y + 1);
	  gdk_draw_line (window,
			 style->bg_gc[state_type],
			 x, y + height - 2,
			 x + 1, y + height - 2);
	  gdk_draw_point (window,
			  style->light_gc[state_type],
			  x, y + height - 1);
	}
    }
#endif


  if (area)
    {
#ifdef GDK_GC_SET_CLIP_RECTANGLE
      gdk_gc_set_clip_rectangle (gc1, NULL);
      gdk_gc_set_clip_rectangle (gc2, NULL);
#endif
      if (shadow_type == GTK_SHADOW_IN || 
          shadow_type == GTK_SHADOW_OUT)
        {
#ifdef GDK_GC_SET_CLIP_RECTANGLE
          gdk_gc_set_clip_rectangle (style->black_gc, NULL);
          gdk_gc_set_clip_rectangle (style->bg_gc[state_type], NULL);
#endif
        }
    }
}
/**
 * gtk_paint_shadow:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: x origin of the rectangle
 * @y: y origin of the rectangle
 * @width: width of the rectangle 
 * @height: width of the rectangle 
 *
 * Draws a shadow around the given rectangle in @window 
 * using the given style and state and shadow type.
 */
void
gtk_paint_shadow (GtkStyle     *style,
                  GdkWindow    *window,
                  GtkStateType  state_type,
                  GtkShadowType shadow_type,
                  GdkRectangle *area,
                  GtkWidget    *widget,
                  const gchar  *detail,
                  gint          x,
                  gint          y,
                  gint          width,
                  gint          height)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  
  gtk_default_draw_shadow (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
}

static void gtk_default_draw_hline (GtkStyle     *style,
                        GdkWindow    *window,
                        GtkStateType  state_type,
                        GdkRectangle  *area,
                        GtkWidget     *widget,
                        const gchar   *detail,
                        gint          x1,
                        gint          x2,
                        gint          y) {
  gint thickness_light;
  gint thickness_dark;
  gint i;
  
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  thickness_light = style->ythickness / 2;
  thickness_dark = style->ythickness - thickness_light;
  
#ifdef GDK_GC_SET_CLIP_RECTANGLE
  if (area)
    {
      gdk_gc_set_clip_rectangle (style->light_gc[state_type], area);
      gdk_gc_set_clip_rectangle (style->dark_gc[state_type], area);
    }
#endif
  
  if (detail && !strcmp (detail, "label"))
    {
      if (state_type == GTK_STATE_INSENSITIVE)
        gdk_draw_line (window, style->white_gc, x1 + 1, y + 1, x2 + 1, y + 1);   
      gdk_draw_line (window, style->fg_gc[state_type], x1, y, x2, y);     
    }
  else
    {
      for (i = 0; i < thickness_dark; i++)
        {
          gdk_draw_line (window, style->dark_gc[state_type], x1, y + i, x2 - i - 1, y + i);
          gdk_draw_line (window, style->light_gc[state_type], x2 - i, y + i, x2, y + i);
        }
      
      y += thickness_dark;
      for (i = 0; i < thickness_light; i++)
        {
          gdk_draw_line (window, style->dark_gc[state_type], x1, y + i, x1 + thickness_light - i - 1, y + i);
          gdk_draw_line (window, style->light_gc[state_type], x1 + thickness_light - i, y + i, x2, y + i);
        }
    }
  
#ifdef GDK_GC_SET_CLIP_RECTANGLE
  if (area)
    {
      gdk_gc_set_clip_rectangle (style->light_gc[state_type], NULL);
      gdk_gc_set_clip_rectangle (style->dark_gc[state_type], NULL);
    }
#endif
}


static void gtk_default_draw_vline (GtkStyle     *style,
                        GdkWindow    *window,
                        GtkStateType  state_type,
                        GdkRectangle  *area,
                        GtkWidget     *widget,
                        const gchar   *detail,
                        gint          y1,
                        gint          y2,
                        gint          x) {
  gint thickness_light;
  gint thickness_dark;
  gint i;
  
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  thickness_light = style->xthickness / 2;
  thickness_dark = style->xthickness - thickness_light;
  
  if (area) {
      gdk_gc_set_clip_rectangle (style->light_gc[state_type], area);
      gdk_gc_set_clip_rectangle (style->dark_gc[state_type], area);
    }

  for (i = 0; i < thickness_dark; i++) { 
      gdk_draw_line (window, style->dark_gc[state_type], x + i, y1, x + i, y2 - i - 1);
      gdk_draw_line (window, style->light_gc[state_type], x + i, y2 - i, x + i, y2);
    }
  
  x += thickness_dark;
  for (i = 0; i < thickness_light; i++) {
      gdk_draw_line (window, style->dark_gc[state_type], x + i, y1, x + i, y1 + thickness_light - i - 1);
      gdk_draw_line (window, style->light_gc[state_type], x + i, y1 + thickness_light - i, x + i, y2);
    }

  if (area) {
      gdk_gc_set_clip_rectangle (style->light_gc[state_type], NULL);
      gdk_gc_set_clip_rectangle (style->dark_gc[state_type], NULL);
    }
}


/**
 * gtk_paint_hline:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @area: rectangle to which the output is clipped
 * @widget: the widget
 * @detail: a style detail
 * @x1: the starting x coordinate
 * @x2: the ending x coordinate
 * @y: the y coordinate
 * 
 * Draws a horizontal line from (@x1, @y) to (@x2, @y) in @window
 * using the given style and state.
 **/ 
void gtk_paint_hline (GtkStyle      *style,
                 GdkWindow     *window,
                 GtkStateType   state_type,
                 GdkRectangle  *area,
                 GtkWidget     *widget,
                 const gchar   *detail,
                 gint          x1,
                 gint          x2,
                 gint          y) {

  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_hline != NULL);
  
  gtk_default_draw_hline (style, window, state_type, area, widget, detail, x1, x2, y);
}

/**
 * gtk_paint_vline:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @area: rectangle to which the output is clipped
 * @widget: the widget
 * @detail: a style detail
 * @y1_: the starting y coordinate
 * @y2_: the ending y coordinate
 * @x: the x coordinate
 * 
 * Draws a vertical line from (@x, @y1_) to (@x, @y2_) in @window
 * using the given style and state.
 */
void gtk_paint_vline (GtkStyle      *style,
                 GdkWindow     *window,
                 GtkStateType   state_type,
                 GdkRectangle  *area,
                 GtkWidget     *widget,
                 const gchar   *detail,
                 gint          y1_,
                 gint          y2_,
                 gint          x) {

  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_vline != NULL);
  
  gtk_default_draw_vline (style, window, state_type, area, widget, detail, y1_, y2_, x);
}

static void gtk_default_draw_slider (GtkStyle      *style,
                         GdkWindow     *window,
                         GtkStateType   state_type,
                         GtkShadowType  shadow_type,
                         GdkRectangle  *area,
                         GtkWidget     *widget,
                         const gchar   *detail,
                         gint           x,
                         gint           y,
                         gint           width,
                         gint           height,
                         GtkOrientation orientation) {
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  sanitize_size (window, &width, &height);
  
  gtk_paint_box (style, window, state_type, shadow_type,
                 area, widget, detail, x, y, width, height);

  if (detail &&
      (strcmp ("hscale", detail) == 0 ||
       strcmp ("vscale", detail) == 0)) {
    if (orientation == GTK_ORIENTATION_HORIZONTAL)
      gtk_paint_vline (style, window, state_type, area, widget, detail, 
                         y + style->ythickness, 
                         y + height - style->ythickness - 1, x + width / 2);
      else
        gtk_paint_hline (style, window, state_type, area, widget, detail, 
                         x + style->xthickness, 
                         x + width - style->xthickness - 1, y + height / 2);
    }
}

void gtk_paint_slider (GtkStyle      *style,
                  GdkWindow     *window,
                  GtkStateType   state_type,
                  GtkShadowType  shadow_type,
                  GdkRectangle  *area,
                  GtkWidget     *widget,
                  const gchar   *detail,
                  gint           x,
                  gint           y,
                  gint           width,
                  gint           height,
                  GtkOrientation orientation) {

  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_slider != NULL);
  
  gtk_default_draw_slider (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, orientation);
}

static void
draw_part (GdkDrawable  *drawable,
	   GdkGC        *gc,
	   GdkRectangle *area,
	   gint          x,
	   gint          y,
	   IndicatorPart part)
{
  if (area)
    gdk_gc_set_clip_rectangle (gc, area);
 /* 
  gdk_gc_set_ts_origin (gc, x, y);
  gdk_gc_set_stipple (gc, get_indicator_for_screen (drawable, part));
  gdk_gc_set_fill (gc, GDK_STIPPLED);
  */

  gdk_draw_rectangle (drawable, gc, TRUE, x, y, INDICATOR_PART_SIZE, INDICATOR_PART_SIZE);
/*
  gdk_gc_set_fill (gc, GDK_SOLID);
  */

  if (area)
    gdk_gc_set_clip_rectangle (gc, NULL);
}

static void 
gtk_default_draw_check (GtkStyle      *style,
  GdkWindow     *window,
  GtkStateType   state_type,
  GtkShadowType  shadow_type,
  GdkRectangle  *area,
  GtkWidget     *widget,
  const gchar   *detail,
  gint           x,
  gint           y,
  gint           width,
  gint           height) {

  if (detail && strcmp (detail, "cellcheck") == 0) {
    if (area) {
      gdk_gc_set_clip_rectangle (widget->style->base_gc[state_type], area);
    }

    gdk_draw_rectangle (window,
                        widget->style->base_gc[state_type],
                        TRUE,
                        x, y,
                        width, height);
    if (area) {
      gdk_gc_set_clip_rectangle (widget->style->base_gc[state_type], NULL);
      gdk_gc_set_clip_rectangle (widget->style->text_gc[state_type], area);
    }
    gdk_draw_rectangle (window,
                        widget->style->text_gc[state_type],
                        FALSE,
                        x, y,
                        width, height);
    if (area) {
        gdk_gc_set_clip_rectangle (widget->style->text_gc[state_type], NULL);
    }

    x -= (1 + INDICATOR_PART_SIZE - width) / 2;
    y -= (((1 + INDICATOR_PART_SIZE - height) / 2) - 1);

    if (shadow_type == GTK_SHADOW_IN) {
      draw_part (window, style->text_gc[state_type], area, x, y, CHECK_TEXT);
      draw_part (window, style->text_aa_gc[state_type], area, x, y, CHECK_AA);
    }
    else if (shadow_type == GTK_SHADOW_ETCHED_IN) { /* inconsistent */
      draw_part (window, style->text_gc[state_type], area, x, y, CHECK_INCONSISTENT_TEXT);
    }
  }
  else {
    GdkGC *free_me = NULL;
      
    GdkGC *base_gc;
    GdkGC *text_gc;
    GdkGC *aa_gc;

    x -= (1 + INDICATOR_PART_SIZE - width) / 2;
    y -= (1 + INDICATOR_PART_SIZE - height) / 2;

    if (detail && strcmp (detail, "check") == 0) { /* Menu item */
      text_gc = style->fg_gc[state_type];
      base_gc = style->bg_gc[state_type];
      aa_gc = free_me = create_aa_gc (window, style, state_type);
    }
    else {
      if (state_type == GTK_STATE_ACTIVE) {
        text_gc = style->fg_gc[state_type];
        base_gc = style->bg_gc[state_type];
        aa_gc=create_aa_gc (window, style, state_type);
        free_me=aa_gc;
      }
      else {
        text_gc = style->text_gc[state_type];
        base_gc = style->base_gc[state_type];
        aa_gc = style->text_aa_gc[state_type];
      }
      draw_part (window, base_gc, area, x, y, CHECK_BASE);
      draw_part (window, style->black_gc, area, x, y, CHECK_BLACK);
      draw_part (window, style->dark_gc[state_type], area, x, y, CHECK_DARK);
      draw_part (window, style->mid_gc[state_type], area, x, y, CHECK_MID);
      draw_part (window, style->light_gc[state_type], area, x, y, CHECK_LIGHT);
    }
    if (shadow_type == GTK_SHADOW_IN) {
      draw_part (window, text_gc, area, x, y, CHECK_TEXT);
      draw_part (window, aa_gc, area, x, y, CHECK_AA);
    }
    else if (shadow_type == GTK_SHADOW_ETCHED_IN) { /* inconsistent */
      draw_part (window, text_gc, area, x, y, CHECK_INCONSISTENT_TEXT);
    }

    if (free_me) {
      g_object_unref ((GObject *) free_me);
    }
  }
}

/**
 * gtk_paint_check:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: the type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: x origin of the rectangle to draw the check in
 * @y: y origin of the rectangle to draw the check in
 * @width: the width of the rectangle to draw the check in
 * @height: the height of the rectangle to draw the check in
 * 
 * Draws a check button indicator in the given rectangle on @window with 
 * the given parameters.
 */
void
gtk_paint_check (GtkStyle      *style,
                 GdkWindow     *window,
                 GtkStateType   state_type,
                 GtkShadowType  shadow_type,
                 GdkRectangle  *area,
                 GtkWidget     *widget,
                 const gchar   *detail,
                 gint           x,
                 gint           y,
                 gint           width,
                 gint           height)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_check != NULL);
  
  gtk_default_draw_check (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
}


static void 
gtk_default_draw_extension (GtkStyle       *style,
                            GdkWindow      *window,
                            GtkStateType    state_type,
                            GtkShadowType   shadow_type,
                            GdkRectangle   *area,
                            GtkWidget      *widget,
                            const gchar    *detail,
                            gint            x,
                            gint            y,
                            gint            width,
                            gint            height,
                            GtkPositionType gap_side)
{
  GdkGC *gc1 = NULL;
  GdkGC *gc2 = NULL;
  GdkGC *gc3 = NULL;
  GdkGC *gc4 = NULL;
  
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  gtk_style_apply_default_background (style, window,
                                      widget && !GTK_WIDGET_NO_WINDOW (widget),
                                      GTK_STATE_NORMAL, area, x, y, width, height);
  
  sanitize_size (window, &width, &height);
  
  switch (shadow_type)
    {
    case GTK_SHADOW_NONE:
      return;
    case GTK_SHADOW_IN:
      gc1 = style->dark_gc[state_type];
      gc2 = style->black_gc;
      gc3 = style->bg_gc[state_type];
      gc4 = style->light_gc[state_type];
      break;
    case GTK_SHADOW_ETCHED_IN:
      gc1 = style->dark_gc[state_type];
      gc2 = style->light_gc[state_type];
      gc3 = style->dark_gc[state_type];
      gc4 = style->light_gc[state_type];
      break;
    case GTK_SHADOW_OUT:
      gc1 = style->light_gc[state_type];
      gc2 = style->bg_gc[state_type];
      gc3 = style->dark_gc[state_type];
      gc4 = style->black_gc;
      break;
    case GTK_SHADOW_ETCHED_OUT:
      gc1 = style->light_gc[state_type];
      gc2 = style->dark_gc[state_type];
      gc3 = style->light_gc[state_type];
      gc4 = style->dark_gc[state_type];
      break;
    }

  if (area)
    {
      gdk_gc_set_clip_rectangle (gc1, area);
      gdk_gc_set_clip_rectangle (gc2, area);
      gdk_gc_set_clip_rectangle (gc3, area);
      gdk_gc_set_clip_rectangle (gc4, area);
    }

  switch (shadow_type)
    {
    case GTK_SHADOW_NONE:
    case GTK_SHADOW_IN:
    case GTK_SHADOW_OUT:
    case GTK_SHADOW_ETCHED_IN:
    case GTK_SHADOW_ETCHED_OUT:
      switch (gap_side)
        {
        case GTK_POS_TOP:
          gtk_style_apply_default_background (style, window,
                                              widget && !GTK_WIDGET_NO_WINDOW (widget),
                                              state_type, area,
                                              x + style->xthickness, 
                                              y, 
                                              width - (2 * style->xthickness), 
                                              height - (style->ythickness));
          gdk_draw_line (window, gc1,
                         x, y, x, y + height - 2);
          gdk_draw_line (window, gc2,
                         x + 1, y, x + 1, y + height - 2);
          
          gdk_draw_line (window, gc3,
                         x + 2, y + height - 2, x + width - 2, y + height - 2);
          gdk_draw_line (window, gc3,
                         x + width - 2, y, x + width - 2, y + height - 2);
          gdk_draw_line (window, gc4,
                         x + 1, y + height - 1, x + width - 2, y + height - 1);
          gdk_draw_line (window, gc4,
                         x + width - 1, y, x + width - 1, y + height - 2);
          break;
        case GTK_POS_BOTTOM:
          gtk_style_apply_default_background (style, window,
                                              widget && !GTK_WIDGET_NO_WINDOW (widget),
                                              state_type, area,
                                              x + style->xthickness, 
                                              y + style->ythickness, 
                                              width - (2 * style->xthickness), 
                                              height - (style->ythickness));
          gdk_draw_line (window, gc1,
                         x + 1, y, x + width - 2, y);
          gdk_draw_line (window, gc1,
                         x, y + 1, x, y + height - 1);
          gdk_draw_line (window, gc2,
                         x + 1, y + 1, x + width - 2, y + 1);
          gdk_draw_line (window, gc2,
                         x + 1, y + 1, x + 1, y + height - 1);
          
          gdk_draw_line (window, gc3,
                         x + width - 2, y + 2, x + width - 2, y + height - 1);
          gdk_draw_line (window, gc4,
                         x + width - 1, y + 1, x + width - 1, y + height - 1);
          break;
        case GTK_POS_LEFT:
          gtk_style_apply_default_background (style, window,
                                              widget && !GTK_WIDGET_NO_WINDOW (widget),
                                              state_type, area,
                                              x, 
                                              y + style->ythickness, 
                                              width - (style->xthickness), 
                                              height - (2 * style->ythickness));
          gdk_draw_line (window, gc1,
                         x, y, x + width - 2, y);
          gdk_draw_line (window, gc2,
                         x + 1, y + 1, x + width - 2, y + 1);
          
          gdk_draw_line (window, gc3,
                         x, y + height - 2, x + width - 2, y + height - 2);
          gdk_draw_line (window, gc3,
                         x + width - 2, y + 2, x + width - 2, y + height - 2);
          gdk_draw_line (window, gc4,
                         x, y + height - 1, x + width - 2, y + height - 1);
          gdk_draw_line (window, gc4,
                         x + width - 1, y + 1, x + width - 1, y + height - 2);
          break;
        case GTK_POS_RIGHT:
          gtk_style_apply_default_background (style, window,
                                              widget && !GTK_WIDGET_NO_WINDOW (widget),
                                              state_type, area,
                                              x + style->xthickness, 
                                              y + style->ythickness, 
                                              width - (style->xthickness), 
                                              height - (2 * style->ythickness));
          gdk_draw_line (window, gc1,
                         x + 1, y, x + width - 1, y);
          gdk_draw_line (window, gc1,
                         x, y + 1, x, y + height - 2);
          gdk_draw_line (window, gc2,
                         x + 1, y + 1, x + width - 1, y + 1);
          gdk_draw_line (window, gc2,
                         x + 1, y + 1, x + 1, y + height - 2);
          
          gdk_draw_line (window, gc3,
                         x + 2, y + height - 2, x + width - 1, y + height - 2);
          gdk_draw_line (window, gc4,
                         x + 1, y + height - 1, x + width - 1, y + height - 1);
          break;
        }
    }

  if (area)
    {
      gdk_gc_set_clip_rectangle (gc1, NULL);
      gdk_gc_set_clip_rectangle (gc2, NULL);
      gdk_gc_set_clip_rectangle (gc3, NULL);
      gdk_gc_set_clip_rectangle (gc4, NULL);
    }
}

/**
 * gtk_paint_extension: 
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: x origin of the extension
 * @y: y origin of the extension
 * @width: width of the extension 
 * @height: width of the extension 
 * @gap_side: the side on to which the extension is attached
 * 
 * Draws an extension, i.e. a notebook tab.
 **/
void
gtk_paint_extension (GtkStyle       *style,
                     GdkWindow      *window,
                     GtkStateType    state_type,
                     GtkShadowType   shadow_type,
                     GdkRectangle   *area,
                     GtkWidget      *widget,
                     gchar          *detail,
                     gint            x,
                     gint            y,
                     gint            width,
                     gint            height,
                     GtkPositionType gap_side)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_extension != NULL);
  
  gtk_default_draw_extension (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side);
}

static void 
gtk_default_draw_flat_box (GtkStyle      *style,
                           GdkWindow     *window,
                           GtkStateType   state_type,
                           GtkShadowType  shadow_type,
                           GdkRectangle  *area,
                           GtkWidget     *widget,
                           const gchar   *detail,
                           gint           x,
                           gint           y,
                           gint           width,
                           gint           height)
{
  GdkGC *gc1;
  GdkGC *freeme = NULL;
  
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  sanitize_size (window, &width, &height);
  
  if (detail)
    {
      if (state_type == GTK_STATE_SELECTED)
        {
          if (!strcmp ("text", detail))
            gc1 = style->bg_gc[GTK_STATE_SELECTED];
          else if (!strncmp ("cell_even", detail, strlen ("cell_even")) ||
		   !strncmp ("cell_odd", detail, strlen ("cell_odd")))
            {
	      /* This has to be really broken; alex made me do it. -jrb */
	      if (GTK_WIDGET_HAS_FOCUS (widget))
		gc1 = style->base_gc[state_type];
	      else 
		gc1 = style->base_gc[GTK_STATE_ACTIVE];
            }
          else
            {
              gc1 = style->bg_gc[state_type];
            }
        }
      else
        {
          if (!strcmp ("viewportbin", detail))
            gc1 = style->bg_gc[GTK_STATE_NORMAL];
          else if (!strcmp ("entry_bg", detail))
            gc1 = style->base_gc[state_type];

          /* For trees: even rows are base color, odd rows are a shade of
           * the base color, the sort column is a shade of the original color
           * for that row.
           */

          else if (!strcmp ("cell_even", detail) ||
                   !strcmp ("cell_odd", detail) ||
                   !strcmp ("cell_even_ruled", detail))
            {
	      GdkColor *color = NULL;

	      gtk_widget_style_get (widget,
		                    "even_row_color", &color,
				    NULL);

	      if (color)
	        {
		  freeme = get_darkened_gc (window, color, 0);
		  gc1 = freeme;

		  gdk_color_free (color);
		}
	      else
	        gc1 = style->base_gc[state_type];
            }
	  else if (!strcmp ("cell_odd_ruled", detail))
	    {
	      GdkColor *color;

	      gtk_widget_style_get (widget,
		                    "odd_row_color", &color,
				    NULL);

	      if (color)
	        {
		  freeme = get_darkened_gc (window, color, 0);
		  gc1 = freeme;

		  gdk_color_free (color);
		}
	      else
	        {
		  gtk_widget_style_get (widget,
		                        "even_row_color", &color,
					NULL);

		  if (color)
		    {
		      freeme = get_darkened_gc (window, color, 1);
		      gdk_color_free (color);
		    }
		  else
		    freeme = get_darkened_gc (window, &style->base[state_type], 1);
		  gc1 = freeme;
		}
	    }
          else if (!strcmp ("cell_even_sorted", detail) ||
                   !strcmp ("cell_odd_sorted", detail) ||
                   !strcmp ("cell_even_ruled_sorted", detail))
            {
	      GdkColor *color = NULL;

	      if (!strcmp ("cell_odd_sorted", detail))
	        gtk_widget_style_get (widget,
		                      "odd_row_color", &color,
				      NULL);
	      else
	        gtk_widget_style_get (widget,
		                      "even_row_color", &color,
				      NULL);

	      if (color)
	        {
		  freeme = get_darkened_gc (window, color, 1);
		  gc1 = freeme;

		  gdk_color_free (color);
		}
	      else
		{
	          freeme = get_darkened_gc (window, &style->base[state_type], 1);
                  gc1 = freeme;
		}
            }
          else if (!strcmp ("cell_odd_ruled_sorted", detail))
            {
	      GdkColor *color = NULL;

	      gtk_widget_style_get (widget,
		                    "odd_row_color", &color,
				    NULL);

	      if (color)
	        {
		  freeme = get_darkened_gc (window, color, 1);
		  gc1 = freeme;

		  gdk_color_free (color);
		}
	      else
	        {
		  gtk_widget_style_get (widget,
		                        "even_row_color", &color,
					NULL);

		  if (color)
		    {
		      freeme = get_darkened_gc (window, color, 2);
		      gdk_color_free (color);
		    }
		  else
                    freeme = get_darkened_gc (window, &style->base[state_type], 2);
                  gc1 = freeme;
	        }
            }
          else
            gc1 = style->bg_gc[state_type];
        }
    }
  else
    gc1 = style->bg_gc[state_type];
  
  if (!style->bg_pixmap[state_type] || gc1 != style->bg_gc[state_type] /*|| GDK_IS_PIXMAP (window)*/)
    {
      if (area)
	gdk_gc_set_clip_rectangle (gc1, area);

      gdk_draw_rectangle (window, gc1, TRUE,
                          x, y, width, height);

      if (detail && !strcmp ("tooltip", detail))
        gdk_draw_rectangle (window, style->black_gc, FALSE,
                            x, y, width - 1, height - 1);

      if (area)
	gdk_gc_set_clip_rectangle (gc1, NULL);
    }
  else
    gtk_style_apply_default_background (style, window,
                                        widget && !GTK_WIDGET_NO_WINDOW (widget),
                                        state_type, area, x, y, width, height);


  if (freeme) {
    g_object_unref ((GObject *) freeme);
  }
}
/**
 * gtk_paint_flat_box:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: the type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: x origin of the box
 * @y: y origin of the box
 * @width: the width of the box
 * @height: the height of the box
 * 
 * Draws a flat box on @window with the given parameters.
 */
void
gtk_paint_flat_box (GtkStyle      *style,
                    GdkWindow     *window,
                    GtkStateType   state_type,
                    GtkShadowType  shadow_type,
                    GdkRectangle  *area,
                    GtkWidget     *widget,
                    const gchar   *detail,
                    gint           x,
                    gint           y,
                    gint           width,
                    gint           height)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_flat_box != NULL);
  
  gtk_default_draw_flat_box (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
}

static void 
gtk_default_draw_focus (GtkStyle      *style,
			GdkWindow     *window,
			GtkStateType   state_type,
			GdkRectangle  *area,
			GtkWidget     *widget,
			const gchar   *detail,
			gint           x,
			gint           y,
			gint           width,
			gint           height)
{
  GdkPoint points[5];
  GdkGC    *gc;
  gboolean free_dash_list = FALSE;
  gint line_width = 1;
  gint8 *dash_list = "\1\1";
  gint dash_len;

  if (widget)
    {
      gtk_widget_style_get (widget,
			    "focus-line-width", &line_width,
			    "focus-line-pattern", (gchar *)&dash_list,
			    NULL);

      free_dash_list = TRUE;
  }

  sanitize_size (window, &width, &height);
  
  if (detail && !strcmp (detail, "colorwheel_light"))
    gc = style->black_gc;
  else if (detail && !strcmp (detail, "colorwheel_dark"))
    gc = style->white_gc;
  else 
    gc = style->fg_gc[state_type];

  gdk_gc_set_line_attributes (gc, line_width,
			      dash_list[0] ? GDK_LINE_ON_OFF_DASH : GDK_LINE_SOLID,
			      GDK_CAP_BUTT, GDK_JOIN_MITER);
  
  if (area)
    gdk_gc_set_clip_rectangle (gc, area);

  if (detail && !strcmp (detail, "add-mode"))
    {
      if (free_dash_list)
	g_free (dash_list);
      
      dash_list = "\4\4";
      free_dash_list = FALSE;
    }

  points[0].x = x + line_width / 2;
  points[0].y = y + line_width / 2;
  points[1].x = x + width - line_width + line_width / 2;
  points[1].y = y + line_width / 2;
  points[2].x = x + width - line_width + line_width / 2;
  points[2].y = y + height - line_width + line_width / 2;
  points[3].x = x + line_width / 2;
  points[3].y = y + height - line_width + line_width / 2;
  points[4] = points[0];

  if (!dash_list[0])
    {
      gdk_draw_lines (window, gc, points, 5);
    }
  else
    {
      /* We go through all the pain below because the X rasterization
       * rules don't really work right for dashed lines if you
       * want continuity in segments that go between top/right
       * and left/bottom. For instance, a top left corner
       * with a 1-1 dash is drawn as:
       *
       *  X X X 
       *  X
       *
       *  X
       *
       * This is because pixels on the top and left boundaries
       * of polygons are drawn, but not on the bottom and right.
       * So, if you have a line going up that turns the corner
       * and goes right, there is a one pixel shift in the pattern.
       *
       * So, to fix this, we drawn the top and right in one call,
       * then the left and bottom in another call, fixing up
       * the dash offset for the second call ourselves to get
       * continuity at the upper left.
       *
       * It's not perfect since we really should have a join at
       * the upper left and lower right instead of two intersecting
       * lines but that's only really apparent for no-dashes,
       * which (for this reason) are done as one polygon and
       * don't to through this code path.
       */
      
      dash_len = strlen (dash_list);
      
      if (dash_list[0])
	gdk_gc_set_dashes (gc, 0, dash_list, dash_len);
      
      gdk_draw_lines (window, gc, points, 3);
      
      /* We draw this line one farther over than it is "supposed" to
       * because of another rasterization problem ... if two 1 pixel
       * unjoined lines meet at the lower right, there will be a missing
       * pixel.
       */
      points[2].x += 1;
      
      if (dash_list[0])
	{
	  gint dash_pixels = 0;
	  gint i;
	  
	  /* Adjust the dash offset for the bottom and left so we
	   * match up at the upper left.
	   */
	  for (i = 0; i < dash_len; i++)
	    dash_pixels += dash_list[i];
      
	  if (dash_len % 2 == 1)
	    dash_pixels *= 2;
	  
	  gdk_gc_set_dashes (gc, dash_pixels - (width + height - 2 * line_width) % dash_pixels, dash_list, dash_len);
	}
      
      gdk_draw_lines (window, gc, points + 2, 3);
    }

  gdk_gc_set_line_attributes (gc, 0, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);

  if (area)
    gdk_gc_set_clip_rectangle (gc, NULL);

  if (free_dash_list)
    g_free (dash_list);
}

/**
 * gtk_paint_focus:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: the x origin of the rectangle around which to draw a focus indicator
 * @y: the y origin of the rectangle around which to draw a focus indicator
 * @width: the width of the rectangle around which to draw a focus indicator
 * @height: the height of the rectangle around which to draw a focus indicator
 *
 * Draws a focus indicator around the given rectangle on @window using the
 * given style.
 */
void
gtk_paint_focus (GtkStyle      *style,
                 GdkWindow     *window,
		 GtkStateType   state_type,
                 GdkRectangle  *area,
                 GtkWidget     *widget,
                 const gchar   *detail,
                 gint           x,
                 gint           y,
                 gint           width,
                 gint           height)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_focus != NULL);
  
  gtk_default_draw_focus (style, window, state_type, area, widget, detail, x, y, width, height);
}

static void 
gtk_default_draw_option (GtkStyle      *style,
			 GdkWindow     *window,
			 GtkStateType   state_type,
			 GtkShadowType  shadow_type,
			 GdkRectangle  *area,
			 GtkWidget     *widget,
			 const gchar   *detail,
			 gint           x,
			 gint           y,
			 gint           width,
			 gint           height)
{
  if (detail && strcmp (detail, "cellradio") == 0)
    {

      ErrOut("gtk_paint.c: cellradio was commented out..\n");
#if 0

      /* have no cellradios.. */
      if (area)               
	gdk_gc_set_clip_rectangle (widget->style->fg_gc[state_type], area);
      gdk_draw_arc (window,
		    widget->style->fg_gc[state_type],
		    FALSE,
                    x, y,
		    width,
		    height,
		    0, 360*64);

      if (shadow_type == GTK_SHADOW_IN)
	{
	  gdk_draw_arc (window,
			widget->style->fg_gc[state_type],
			TRUE,
                        x + 2,
                        y + 2,
			width - 4,
			height - 4,
			0, 360*64);
	}
      else if (shadow_type == GTK_SHADOW_ETCHED_IN) /* inconsistent */
        {
          draw_part (window, widget->style->fg_gc[state_type],
	             area, x, y, CHECK_INCONSISTENT_TEXT);
	}
      if (area)               
	gdk_gc_set_clip_rectangle (widget->style->fg_gc[state_type], NULL);
#endif
    }
  else {
    GdkGC *free_me = NULL;
      
    GdkGC *base_gc;
    GdkGC *text_gc;
    GdkGC *aa_gc;

    x -= (1 + INDICATOR_PART_SIZE - width) / 2;
    y -= (1 + INDICATOR_PART_SIZE - height) / 2;

    if (detail && strcmp (detail, "option") == 0)	{ /* Menu item */
      text_gc = style->fg_gc[state_type];
      base_gc = style->bg_gc[state_type];
      aa_gc = free_me = create_aa_gc (window, style, state_type);
    }
    else {
      if (state_type == GTK_STATE_ACTIVE) {
        text_gc = style->fg_gc[state_type];
        base_gc = style->bg_gc[state_type];
        aa_gc = free_me = create_aa_gc (window, style, state_type);
      }
      else {
        text_gc = style->text_gc[state_type];
        base_gc = style->base_gc[state_type];
        aa_gc = style->text_aa_gc[state_type];
      }

      draw_part (window, base_gc, area, x, y, RADIO_BASE);
      draw_part (window, style->black_gc, area, x, y, RADIO_BLACK);
      draw_part (window, style->dark_gc[state_type], area, x, y, RADIO_DARK);
      draw_part (window, style->mid_gc[state_type], area, x, y, RADIO_MID);
      draw_part (window, style->light_gc[state_type], area, x, y, RADIO_LIGHT);
    }
    
    if (shadow_type == GTK_SHADOW_IN) {
      draw_part (window, text_gc, area, x, y, RADIO_TEXT);
    }
    else if (shadow_type == GTK_SHADOW_ETCHED_IN) { /* inconsistent */
      if (detail && strcmp (detail, "option") == 0) { /* Menu item */
        draw_part (window, text_gc, area, x, y, CHECK_INCONSISTENT_TEXT);
      }
      else {
        draw_part (window, text_gc, area, x, y, RADIO_INCONSISTENT_TEXT);
        draw_part (window, aa_gc, area, x, y, RADIO_INCONSISTENT_AA);
      }
    }

    if (free_me) {
      g_object_unref ((GObject *) free_me);
    }
  }
}

/**
 * gtk_paint_option:
 * @style: a #GtkStyle
 * @window: a #GdkWindow
 * @state_type: a state
 * @shadow_type: the type of shadow to draw
 * @area: clip rectangle
 * @widget: the widget
 * @detail: a style detail
 * @x: x origin of the rectangle to draw the option in
 * @y: y origin of the rectangle to draw the option in
 * @width: the width of the rectangle to draw the option in
 * @height: the height of the rectangle to draw the option in
 *
 * Draws a radio button indicator in the given rectangle on @window with 
 * the given parameters.
 */
void
gtk_paint_option (GtkStyle      *style,
                  GdkWindow     *window,
                  GtkStateType   state_type,
                  GtkShadowType  shadow_type,
                  GdkRectangle  *area,
                  GtkWidget     *widget,
                  const gchar   *detail,
                  gint           x,
                  gint           y,
                  gint           width,
                  gint           height)
{
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (GTK_STYLE_GET_CLASS (style)->draw_option != NULL);
  
  gtk_default_draw_option (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
}



