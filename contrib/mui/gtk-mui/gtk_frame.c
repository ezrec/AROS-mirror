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
 * $Id$
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

GtkWidget* gtk_frame_new(const gchar *label) {

  GtkWidget *ret;
  Object *frame;
	gchar *t;
	gchar empty[]="";

  DebOut("gtk_frame_new(%s)\n",label);

  ret=mgtk_widget_new(IS_FRAME);

	if(label) {
		t=g_strdup(label);
    frame=(APTR) HGroup,MUIA_Frame, MUIV_Frame_Group, MUIA_FrameTitle, t, MUIA_Background, MUII_GroupBack,End;
	}
	else {
		t=g_strdup(empty);
    frame=(APTR) HGroup,MUIA_Frame, MUIV_Frame_Group,  MUIA_Background, MUII_GroupBack,End;
	}


  ret->MuiObject=frame;
	ret->title=t;

  return ret;
}

void gtk_frame_set_shadow_type(GtkFrame *frame, GtkShadowType type) {

  DebOut("gtk_frame_set_shadow_type(%lx,%d)\n",frame,type);

  if(type == GTK_SHADOW_NONE) {
    DebOut("TODO: type=GTK_SHADOW_NONE\n");
  }

  /* just ignore it.. */
}

GtkFrame* GTK_FRAME(GtkWidget* widget) {
	return((GtkFrame *) widget);
}

