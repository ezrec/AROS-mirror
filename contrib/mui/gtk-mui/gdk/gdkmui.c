/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner, Ilkka Lehtoranta
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
 * $Id: gdkmui.c,v 1.2 2005/12/12 16:22:44 itix Exp $
 *
 *****************************************************************************/

#include <gtk/gtk.h>
#include <../gtk_globals.h>

#include <graphics/rpattr.h>
#include <proto/graphics.h>

#include "gdkconfig.h"

#if USE_PENS

void gdkmui_set_color(struct RastPort *rp, GdkGC *gc)
{
  if(gc->colormap->colors->havepen==1) {
    pen=(LONG) gc->colormap->colors->pen;
    DebOut("  found pen: %d\n",(int) pen);
  }
  else {
    DebOut("WARNING: assuming gc pointer is a pen (%d)..\n",gc); /* This should not be the case any more! */
    pen=(LONG) gc;
  }

  SetAPen(_rp(obj),_dri(obj)->dri_Pens[pen]);
}

#else

void gdkmui_set_color(struct RastPort *rp, GdkGC *gc)
{
  ULONG	r, g, b;

  r = gc->colormap->colors->red >> 8;
  g = gc->colormap->colors->green >> 8;
  b = gc->colormap->colors->blue >> 8;

  SetRPAttrs(rp, RPTAG_PenMode, FALSE, RPTAG_FgColor, (r << 16) | (g << 8) | (b << 0), TAG_DONE);
}

#endif
