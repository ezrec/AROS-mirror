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

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk_globals.h"

struct Data
{
	LONG is_resizable;
  LONG defwidth, defheight;
};

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (obj)
  {
    GETDATA;

    data->defwidth = -1;
    data->defheight = -1;
    data->is_resizable = -1;
  }

  return (ULONG)obj;
}

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg)
{
  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_RootGroup_Resizable:
        if (!(data->is_resizable & tag->ti_Data))
        {
          relayout = 1;
          data->is_resizable = tag->ti_Data;
        }
        break;

      case MA_DefWidth:
        if (data->defwidth != tag->ti_Data)
        {
          relayout = 1;
          data->defwidth = tag->ti_Data;
        }
        break;

      case MA_DefHeight:
        if (data->defheight != tag->ti_Data)
        {
          relayout = 1;
          data->defheight = tag->ti_Data;
        }
        break;
    }
  }

  if (relayout)
  {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
}

static ULONG mAskMinMax(struct Data *data, APTR obj, struct MUIP_AskMinMax *msg, struct IClass *cl)
{
	DebOut("mAskMinMax(%lx,%lx,%lx,%lx)\n",data,obj,msg,cl);

  DoSuperMethodA(cl, obj, msg);

  if (!data->is_resizable || data->defwidth >=0 || data->defheight >= 0)
  {
    struct MUI_MinMax *mm = msg->MinMaxInfo;
    struct Window *window = _window(obj);
    LONG defwidth, defheight;
    LONG bl,br,bt,bb;

    /* during window_show, _window(obj) is NULL,
       on AROS accessing window->BorderLeft crashes
       with a core dump
    */
    DebOut("  window: %lx\n",window);

    if(window) {
      bl=window->BorderLeft;
      br=window->BorderRight;
      bt=window->BorderTop;
      bb=window->BorderBottom;
    }
    else {
      /* ? */
      bl=0;
      br=0;
      bt=0;
      bb=0;
    }

    if (data->defwidth < 0) {
      defwidth = mm->DefWidth;
    }
    else if (data->defwidth == 0) {
      defwidth = mm->MinWidth;
    }
    else {
      defwidth = data->defwidth - bl - br;

      if (defwidth < mm->MinWidth) {
        defwidth = mm->MinWidth;
      }
      else if (defwidth > mm->MaxWidth) {
        defwidth = mm->MaxWidth;
      }
    }

    if (data->defheight < 0)
      defheight = mm->DefHeight;
    else if (data->defheight == 0)
      defheight = mm->MinHeight;
    else
    {
      defheight = data->defheight - bt - bb;

      if (defheight < mm->MinHeight)
        defheight = mm->MinHeight;
      else if (defheight > mm->MaxHeight)
        defheight = mm->MaxHeight;
    }

    if (!data->is_resizable)
    {
      mm->MinWidth = mm->DefWidth = mm->MaxWidth = defwidth;
      mm->MinHeight = mm->DefHeight = mm->MaxHeight = defheight;
    }
    else
    {
      mm->DefWidth = defwidth;
      mm->DefHeight = defheight;
    }
  }

  return 0;
}

static ULONG mSetup(struct IClass *cl, APTR obj, Msg msg)
{
  ULONG rc = DoSuperMethodA(cl, obj, msg);

  if (rc)
  {
    mgtk_update_default_style(obj);
  }

  return rc;
}

BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case MUIM_AskMinMax : return mAskMinMax (data, obj, (APTR)msg, cl);
    case MUIM_Setup     : return mSetup     (cl, obj, (APTR)msg);
  }

ENDMTABLE

int mgtk_create_rootgroup_class(void)
{
  CL_RootGroup = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_RootGroup ? 1 : 0;
}

void mgtk_delete_rootgroup_class(void)
{
  if (CL_RootGroup)
  {
    MUI_DeleteCustomClass(CL_RootGroup);
  }
}
