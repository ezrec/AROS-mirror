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
 * $Id: gtk_style.c,v 1.8 2005/12/12 17:21:10 itix Exp $
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

/* TODO: move gdk functions to gdk/ */

static GdkGC *allocgc() {
  GdkGC *ret;

  ret=g_new0(GdkGC,1);

  ret->colormap=g_new0(GdkColormap,1);

  ret->colormap->colors=g_new0(GdkColor,1);

  #if USE_PENS
  ret->colormap->colors->havepen=0;
  #endif

  return ret;
}

static GdkGC *newgc(struct ColorMap *cm, gint defaultpen) {
  GdkGC *ret;

  ret=allocgc();

  if (ret)
  {
    #if USE_PENS
    ret->colormap->colors->pen=defaultpen;
    ret->colormap->colors->havepen=1;
    #else
    ULONG colors[3];

    GetRGB32(cm, defaultpen, 1, colors);

    ret->colormap->colors->red = colors[0] >> 16;
    ret->colormap->colors->blue = colors[1] >> 16;
    ret->colormap->colors->green = colors[2] >> 16;
    #endif
  }

  return ret;
}

GdkGC *gdk_gc_new(GdkWindow *window) {
  GdkGC *ret;

  ret=allocgc();
  #if USE_PENS
  ret->colormap->colors->havepen=0;
  #endif

  return ret;
}

GtkStyle *gtk_style_new() {

  GtkStyle *ret;
  int i;

  DebOut("gtk_style_new()\n");

  ret=g_new0(GtkStyle,1);
  i=0;
  while(i<6) {
    ret->fg_gc[i]=allocgc();
    ret->bg_gc[i]=allocgc();
    ret->light_gc[i]=allocgc();
    ret->dark_gc[i]=allocgc();
    ret->mid_gc[i]=allocgc();
    ret->text_gc[i]=allocgc();
    ret->base_gc[i]=allocgc();
    ret->text_aa_gc[i]=allocgc();
    ret->bg_pixmap[i]=g_new0(GdkPixmap,1);

    i++;
  }
  ret->white_gc=allocgc();
  ret->black_gc=allocgc();

  ret->xthickness=2;
  ret->ythickness=2;

  ret->colormap=g_new0(GdkColormap,1);
  ret->colormap->colors=g_new0(GdkColor,1);
  #if USE_PENS
  ret->colormap->colors->havepen=0;
  #endif

  ret->attach_count=0;

  return ret;
}

void gtk_style_set_background(GtkStyle *style, GdkWindow *window, GtkStateType state_type) {

  DebOut("gtk_style_set_background(%lx,%lx,%d)\n",style,window,state_type);

  DebOut("  ... nothing to do, we care for styles in MUI?\n");

}

void gtk_style_apply_default_background (GtkStyle *style, GdkWindow *window, gboolean set_bg, GtkStateType state_type, GdkRectangle *area, gint x, gint y, gint width, gint height) {

  DebOut("gtk_style_apply_default_background(%lx,%lx,%d)\n",style,window,set_bg);

  DebOut("  ... nothing to do, we care for styles in MUI?\n");
}

/*
        DETAILPEN
            Pen compatible with V34. Used to render text in the screen's title bar.
        BLOCKPEN
          Pen compatible with V34. Used to fill the screen's title bar.
        TEXTPEN
          Pen for regular text on BACKGROUNDPEN.
        SHINEPEN
          Pen for the bright edge on 3D objects.
        SHADOWPEN
          Pen for the dark edge on 3D objects.
        FILLPEN
          Pen for filling the active window borders and selected gadgets.
        FILLTEXTPEN
          Pen for text rendered over FILLPEN.
        BACKGROUNDPEN
          Pen for the background color.  Currently must be zero.
        HIGHLIGHTTEXTPEN
          Pen for "special color" or highlighted text on BACKGROUNDPEN.k

        or MUI pens ??
          MPEN_SHINE      = 0,
          MPEN_HALFSHINE  = 1,
          MPEN_BACKGROUND = 2,
          MPEN_HALFSHADOW = 3,
          MPEN_SHADOW     = 4,
          MPEN_TEXT       = 5,
          MPEN_FILL       = 6,
          MPEN_MARK       = 7,
          MPEN_COUNT      = 8,

*/

/**********************************************************************
  mgtk_update_default_style()

  Update style according to MUI settings
**********************************************************************/

void mgtk_update_default_style(APTR obj)
{
  struct MUI_RenderInfo *mri;
  struct ColorMap *cm;
  ULONG i;

  #if DEBUG
  if (obj != mgtk->MuiRoot)
  {
    DebOut("mgtk_update_default_style(): Only MuiRoot is allowed to call this function\n");
  }

  if (!mgtk->default_style)
  {
    DebOut("mgtk_update_default_style(): BANG! mgtk->default_style is still NULL.\n");
    return;
  }
  #endif

  mri = muiRenderInfo(obj);
  cm = _screen(obj)->ViewPort.ColorMap;

  for (i = 0; i < 6; i++)
  {
#if USE_PENS
  #define INTUI_PENS 0
  #if INTUI_PENS
    mgtk->default_style->fg_gc[i]=newgc(cm, MGTK_PEN_FG);  
    mgtk->default_style->bg_gc[i]=newgc(cm, MGTK_PEN_BG);
    mgtk->default_style->light_gc[i]=newgc(cm, MGTK_PEN_LIGHT);
    mgtk->default_style->dark_gc[i]=newgc(cm, MGTK_PEN_DARK);
    mgtk->default_style->mid_gc[i]=newgc(cm, MGTK_PEN_MID); /* hmm..*/
    mgtk->default_style->text_gc[i]=newgc(cm, MGTK_PEN_TEXT);
    mgtk->default_style->base_gc[i]=newgc(cm, MGTK_PEN_BASE);
    mgtk->default_style->text_aa_gc[i]=newgc(cm, MGTK_PEN_TEXT_AA); /* hmm..*/
  #else
    mri = muiRenderInfo(obj);
    mgtk->default_style->fg_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_TEXT]));  
    mgtk->default_style->bg_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_BACKGROUND]));
    mgtk->default_style->light_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_SHINE]));
    mgtk->default_style->mid_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_HALFSHADOW])); 
    mgtk->default_style->dark_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_SHADOW]));
    mgtk->default_style->text_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_TEXT]));
    mgtk->default_style->base_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_BACKGROUND]));
    mgtk->default_style->text_aa_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_TEXT])); 
  #endif
#else
    mgtk->default_style->fg_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_TEXT]));  
    mgtk->default_style->bg_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_BACKGROUND]));
    mgtk->default_style->light_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_SHINE]));
    mgtk->default_style->mid_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_HALFSHADOW])); 
    mgtk->default_style->dark_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_SHADOW]));
    mgtk->default_style->text_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_TEXT]));
    mgtk->default_style->base_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_BACKGROUND]));
    mgtk->default_style->text_aa_gc[i]=newgc(cm, MUIPEN(mri->mri_Pens[MPEN_TEXT])); 
#endif

    DebOut("mgtk->default_style->fg_gc[i]=%d\n",mgtk->default_style->fg_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->bg_gc[i]=%d\n",mgtk->default_style->bg_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->light_gc[i]=%d\n",mgtk->default_style->light_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->mid_gc[i]=%d\n",mgtk->default_style->mid_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->dark_gc[i]=%d\n",mgtk->default_style->dark_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->text_gc[i]=%d\n",mgtk->default_style->text_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->base_gc[i]=%d\n",mgtk->default_style->base_gc[i]->colormap->colors->pen);
    DebOut("mgtk->default_style->text_aa_gc[i]=%d\n",mgtk->default_style->text_aa_gc[i]->colormap->colors->pen);
  }

  #if USE_PENS
  mgtk->default_style->white_gc=newgc(cm, mgtk->white_pen); 
  mgtk->default_style->black_gc=newgc(cm, mgtk->black_pen); 
  #endif
}

GtkStyle *mgtk_get_default_style(Object *obj) {
  /* Warning: current implementation leaves structure uninitialized.
   * If widgets are caching values (they shouldnt) they get bad gfx...
   * Should we use hardcoded defaults?  -itix
   */

  unsigned int i;

  DebOut("mgtk_get_default_style(%lx)\n");

  if(!mgtk->default_style) {
    mgtk->default_style=gtk_style_new();
    i=0;
    while(i<6) {
      mgtk->default_style->bg_pixmap[i]=g_new0(GdkPixmap,1);

      i++;
    }
  }
  else {
    DebOut("  just return mgtk->default_style\n");
  }

  mgtk->default_style->attach_count++;
  return mgtk->default_style;
}

/* Attaches a style to a window; this process allocates 
 * the colors and creates the GC's for the style - it 
 * specializes it to a particular visual and colormap. 
 * The process may involve the creation of a new style if 
 * the style has already been attached to a window with a 
 * different style and colormap.
 */

GtkStyle *gtk_style_attach(GtkStyle *style, GdkWindow *window) {
  GtkWidget *widget;

  DebOut("gtk_style_attach(%lx,%lx)\n",style,window);

  if(!window) {
    DebOut("WARNING: window is NULL, so I cannot attach style..\n");
    return NULL;
  }

  if(style) {
    window->style=style;
  }
  else {
    widget=(GtkWidget *) window->mgtk_widget;
    window->style=mgtk_get_default_style(widget->MuiObject);
  }

  return window->style;
}

void gdk_gc_set_rgb_fg_color(GdkGC *gc, const GdkColor *color) {

  DebOut("gdk_gc_set_rgb_fg_color(%lx,%lx)",gc,color);

  if(!gc->colormap) {
    ErrOut("gdk_gc_set_rgb_fg_color: gc has no colormap!\n");
    return;
  }

  gc->colormap->colors->red=color->red;
  gc->colormap->colors->green=color->green;
  gc->colormap->colors->blue=color->blue;
  #if USE_PENS
  gc->colormap->colors->havepen=0;

  /* TODO: ObtainPen..? */
  #endif
}

/* A style created by matching with the supplied paths, or NULL 
 * if nothing matching was specified and the default style 
 * should be used.
 */
GtkStyle* gtk_rc_get_style_by_paths(GtkSettings *settings, const char *widget_path, const char *class_path, GType type) {
  DebOut("gtk_rc_get_style_by_paths(%lx,%s,%s,%d)\n",settings,widget_path,class_path,type);

  /* as we ignore styles more or less completely, a default style is fine here */

  return NULL;
}
