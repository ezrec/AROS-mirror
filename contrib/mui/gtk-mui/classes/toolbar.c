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

/* toolbar class
 * 
 * a lot of useless stuff in here, will clean up, promised ..
 */

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>

#include <mui.h>
#include <MUI/Toolbar_mcc.h>

#include "classes.h"
#include "gtk_globals.h"
#include "gtk.h"
#include "debug.h"

/*********************************************************************/

struct Data {
  GtkWidget *widget;
};

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  DebOut("mNew(toolbar)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (!obj) {
    ErrOut("toolbar: unable to create object!");
    return (ULONG) NULL;
  }
  else
  {
    GETDATA;

    tstate=((struct opSet *)msg)->ops_AttrList;
    widget=NULL;

    while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate))) {
      switch (tag->ti_Tag) {
        case MA_Widget:
          widget = (GtkWidget *) tag->ti_Data;
          break;
      }
    }

    if(!widget) {
      ErrOut("classes/toolbar.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    set(obj,MUIA_Frame, MUIV_Frame_Group);
    set(obj,MUIA_Background, MUII_GroupBack);

    data->widget=widget;
  }

  return (ULONG)obj;
}

static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;

  switch (msg->opg_AttrID)
  {
    case MA_Widget: 
      rc = (ULONG) data->widget; 
      break;

    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;

  return TRUE;
}

BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew           (cl, obj, msg);
    case OM_GET         : return mGet           (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

int mgtk_create_toolbar_class(void) {

  CL_Toolbar=MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Toolbar ? 1 : 0;
}

void mgtk_delete_toolbar_class(void) {

  if (CL_Toolbar) {
    MUI_DeleteCustomClass(CL_Toolbar);
  }
}
