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
 * $Id: gtk_arrow.c,v 1.13 2005/12/03 09:14:25 o1i Exp $
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

GtkWidget *gtk_arrow_new(GtkArrowType arrow_type, GtkShadowType shadow_type) {

  GtkWidget *ret = NULL;
  Object *arr;
  ULONG spec;

  DebOut("gtk_arrow_new(%ld,%ld)\n",arrow_type,shadow_type);

  if(shadow_type!=GTK_SHADOW_OUT) {
    WarnOut("gtk_arrow_new: only GTK_SHADOW_OUT supported (type: %d)\n",shadow_type);
  }

  switch (arrow_type)
  {
    case GTK_ARROW_UP   : spec = MUII_ArrowUp; break;
    case GTK_ARROW_DOWN : spec = MUII_ArrowDown; break;
    case GTK_ARROW_LEFT : spec = MUII_ArrowLeft; break;
    case GTK_ARROW_RIGHT: spec = MUII_ArrowRight; break;
    default :
      ErrOut("gtk_arrow_new: unknown type: %d\n",arrow_type);
      return(NULL);
      break;
  }

  arr = ImageObject, ImageButtonFrame,
    MUIA_InputMode  , MUIV_InputMode_RelVerify,
    MUIA_Image_Spec , spec,
#ifdef __MORPHOS__
    MUIA_Background , MUII_ImageButtonBack,
#else
    MUIA_Background , MUII_ButtonBack,
#endif
    MUIA_CycleChain , 1,
  End;

  if (arr)
  {
    ret = mgtk_widget_new(IS_ARROW);

    if (ret)
      ret->MuiObject = arr;
  }

  return ret;
}
