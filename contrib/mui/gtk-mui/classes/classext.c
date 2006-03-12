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

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

#ifdef __MORPHOS__
#include <emul/emulregs.h>
#endif

/**********************************************************************
  mgtk_askminmax

  This is like standard mAskMinMax() but supports user defined
  default width/height
**********************************************************************/

ULONG mgtk_askminmax(struct IClass *cl, APTR obj, struct MUIP_AskMinMax *msg, LONG defwidth, LONG defheight)
{
  struct MUI_MinMax *mm = msg->MinMaxInfo;

  DoSuperMethodA(cl, obj, msg);

  if (defwidth < 0)
    defwidth = mm->DefWidth;
  else if (defwidth == 0)
    defwidth = mm->MinWidth;
  else
  {
    if (defwidth < mm->MinWidth)
      defwidth = mm->MinWidth;
    else if (defwidth > mm->MaxWidth)
      defwidth = mm->MaxWidth;
  }

  if (defheight < 0)
    defheight = mm->DefHeight;
  else if (defheight == 0)
    defheight = mm->MinHeight;
  else
  {
    if (defheight < mm->MinHeight)
      defheight = mm->MinHeight;
    else if (defheight > mm->MaxHeight)
      defheight = mm->MaxHeight;
  }

  mm->DefWidth = defwidth;
  mm->DefHeight = defheight;

  return 0;
}

/****************************
 * mgtk_get_widget_from_obj
 *
 * get the GtkWidget of a 
 * MUI object
 ****************************/
GtkWidget *mgtk_get_widget_from_obj(Object *obj) {
  Object *o;

  o=obj;
  while(!xget(o,MA_Widget)) {
    DebOut("  o=%x\n",o);
    o=_parent(o);
  }
  return (GtkWidget *) xget(o,MA_Widget);
}

