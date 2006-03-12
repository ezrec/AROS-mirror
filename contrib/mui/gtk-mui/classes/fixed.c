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

/* fixed class
 * 
 * heavily based on itix's table class
 */

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>

#include "classes.h"
#include "gtk_globals.h"
#include "gtk.h"
#include "debug.h"

/*********************************************************************/

struct Data {
  GtkWidget *widget;
  struct Hook LayoutHook;
  struct MinList ChildList;
};

struct FixedNode {
  struct MinNode node;
  GtkWidget *widget;
  int x;
  int y;
};

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, ULONG is_new);

#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm)
{
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif
  switch (lm->lm_Type) {

    case MUILM_MINMAX: {

      struct Data *data = hook->h_Data;
      struct FixedNode *node;
      ULONG mincw, minch, defcw, defch, maxcw, maxch;

      mincw = 0;
      minch = 0;
      defcw = 0; //data->horizspacing;
      defch = 0; //data->vertspacing;
      maxcw = 0;
      maxch = 0;

      /* find minimum/maximum width and height needed */
      ForeachNode(&data->ChildList, node) {

        ULONG w, h;

        w = _minwidth(node->widget->MuiObject);
        h = _minheight(node->widget->MuiObject);

        if(w + node->x > mincw) {
          mincw=w + node->x;
        }
        if(h + node->y > minch) {
          minch=h + node->y;
        }

        // calculate default cell width/height

        w = _defwidth(node->widget->MuiObject);
        h = _defheight(node->widget->MuiObject);

        if(w + node->x > defcw) {
          defcw=w + node->x;
        }
        if(h + node->y > defcw) {
          defch=h + node->y;
        }

        // calculate max cell width/height

        w = _maxwidth(node->widget->MuiObject);
        h = _maxheight(node->widget->MuiObject);

        if(w + node->x > maxcw) {
          maxcw=w + node->x;
        }
        if(h + node->y > defcw) {
          maxch=h + node->y;
        }
      }
      lm->lm_MinMax.MinWidth  = mincw;
      lm->lm_MinMax.MinHeight = minch;
      lm->lm_MinMax.DefWidth  = defcw;
      lm->lm_MinMax.DefHeight = defch;
      lm->lm_MinMax.MaxWidth  = maxcw;
      lm->lm_MinMax.MaxHeight = maxch;
      /*
      DebOut("  mincw=%d\n",mincw);
      DebOut("  minch=%d\n",minch);
      DebOut("  maxcw=%d\n",maxcw);
      DebOut("  maxcw=%d\n",maxcw);
      */
    }
    return 0;

    case MUILM_LAYOUT: {
      struct Data *data = hook->h_Data;

      struct FixedNode *node;

      ForeachNode(&data->ChildList, node) {
        if (!MUI_Layout(node->widget->MuiObject,
          node->x,
          node->y,
          _minwidth(node->widget->MuiObject),
          _minheight(node->widget->MuiObject),
          0)) {
          return FALSE;
        }
      }
    }
    return TRUE;
  }

  return MUILM_UNKNOWN;
}

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {

  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  DebOut("mNew(fixed)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (!obj) {
    ErrOut("fixed: unable to create object!");
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
      ErrOut("classes/fixed.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    data->widget=widget;

    SETUPHOOK(&data->LayoutHook, LayoutHook, data);
    MGTK_NEWLIST(data->ChildList);

    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
    mSet(data, obj, (APTR)msg, TRUE);
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

void fixed_move(struct Data *data,APTR obj,struct MA_Fixed_Move_Data *movedata) {

  struct FixedNode *node;
  struct FixedNode *mynode;

  DebOut("fixed_move(%lx,%lx,%lx)\n",data,obj,movedata);
  DebOut(" x=%d\n",movedata->x);
  DebOut(" y=%d\n",movedata->y);

  /* check, if we already have placed this object before */
  mynode=NULL;
  ForeachNode(&data->ChildList, node) {
    if(node->widget == movedata->widget) {
      mynode=node;
    }
  }
  if(!mynode) {
    mynode = mgtk_allocmem(sizeof(*node), MEMF_ANY);
    ADDTAIL(&data->ChildList, mynode);
    mynode->widget=movedata->widget;
  }

  /* now we have our node in mynode */
  mynode->x=movedata->x;
  mynode->y=movedata->y;

  /* relayout */
  DoMethod(obj, MUIM_Group_InitChange);
  DoMethod(obj, MUIM_Group_ExitChange);

  /* relayout parent (our size might have changed) */
  if(_parent(obj)) {
    DoMethod(_parent(obj), MUIM_Group_InitChange);
    DoMethod(_parent(obj), MUIM_Group_ExitChange);
  }
}

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, ULONG is_new)
{
  struct TagItem *tstate, *tag;

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem(&tstate))) {

    switch (tag->ti_Tag) {
      case MA_Fixed_Move:
        fixed_move(data,obj,(struct MA_Fixed_Move_Data *)tag->ti_Data);
        break;
    }
  }
}

static VOID mRemMember(struct Data *data, struct opMember *msg) {
  struct FixedNode *node;

  ForeachNode(&data->ChildList, node) {
    if (node->widget->MuiObject == msg->opam_Object)
    {
      REMOVE(node);
      mgtk_freemem(node, sizeof(*node));
      return;
    }
  }
}

BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew           (cl, obj, msg);
  	case OM_SET         :        mSet           (data, obj, (APTR)msg, FALSE); break;
    case OM_GET         : return mGet           (data, obj, (APTR)msg, cl);
    case OM_REMMEMBER   :        mRemMember     (data,      (APTR)msg); break;
  }

ENDMTABLE

int mgtk_create_fixed_class(void) {

  CL_Fixed = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Fixed ? 1 : 0;
}

void mgtk_delete_fixed_class(void) {

  if (CL_Fixed) {
    MUI_DeleteCustomClass(CL_Fixed);
  }
}
