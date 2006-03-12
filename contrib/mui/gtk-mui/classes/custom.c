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

/* custom class
**
** works as a basis for custom widgets
*/

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>

#include "classes.h"
#include "gtk_globals.h"
#include "gtk.h"
#include "debug.h"

struct Data {
  LONG is_resizable;
  LONG  defwidth, defheight;
  GtkWidget *widget;

  /* old stuff */
  struct Hook LayoutHook;
  struct MinList ChildList;

  ULONG columns, rows, vertspacing, horizspacing;
};

struct CustomNode {
  struct MinNode node;
  GtkWidget *widget;
  UWORD left, right, top, bottom;
};


static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, ULONG is_new);

#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm) {
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif

  GtkWidget *widget;
  GtkRequisition req;
  void (*callme)(GtkWidget *widget,GtkRequisition *requisition);

  DebOut("LayoutHook(%lx,..)\n",obj);

  switch (lm->lm_Type) {
    case MUILM_MINMAX: {

      widget=mgtk_get_widget_from_obj(obj);

      DebOut("  widget=%lx\n",widget);
      if(widget->class) {
        DebOut("  widget->class=%lx\n",widget->class);
        if(widget->class->size_request) {
          callme=(void *) widget->class->size_request;
          DebOut("  calling widget->class->size_request: %lx\n",callme);
          (*callme)(widget,&req);
          DebOut("  req->width=%d\n",req.width);
          DebOut("  req->height=%d\n",req.height);
        }
      }

      lm->lm_MinMax.MinWidth  = 0;
      lm->lm_MinMax.MinHeight = 0;
      lm->lm_MinMax.DefWidth  = req.width;
      lm->lm_MinMax.DefHeight = req.height;
      lm->lm_MinMax.MaxWidth  = 1024;
      lm->lm_MinMax.MaxHeight = 1024;
    }
    return 0;

#if 0
    case MUILM_LAYOUT: {
      struct Data *data = hook->h_Data;
      struct CustomNode *node;
      ULONG mincw, minch, defcw, defch, maxcw, maxch;

      if (data->columns && data->rows)
      {
        struct CustomNode *node;
        ULONG cw, ch;

        cw = lm->lm_Layout.Width / data->columns; 
        ch = lm->lm_Layout.Height / data->rows; /* +1 ? */

        ForeachNode(&data->ChildList, node)
        {
          if (!MUI_Layout(node->widget->MuiObject,
            node->left * cw,
            node->top * ch ,
            (node->right - node->left) * cw,
            (node->bottom - node->top ) * ch,
            0))
          {
            return FALSE;
          }
        }
      }
    }
#endif
    return TRUE;
  }

  return MUILM_UNKNOWN;
}

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  DebOut("mNew(custom)\n");

  if (!obj) {
    ErrOut("custom: unable to create object!");
    return (ULONG) NULL;
  }
  else
  {
    GETDATA;

    tstate=((struct opSet *)msg)->ops_AttrList;

    widget=NULL;
    while ((tag = (struct TagItem *) NextTagItem(&tstate))) {
      switch (tag->ti_Tag) {
        case MA_Widget:
          widget = (GtkWidget *) tag->ti_Data;
          DebOut("  widget=%lx\n",widget);
          break;
      }
    }

    if(!widget) {
      ErrOut("classes/custom.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    data->widget=widget;

    SETUPHOOK(&data->LayoutHook, LayoutHook, data);
    MGTK_NEWLIST(data->ChildList);

#if 0
    data->vertspacing = 0;
    data->horizspacing = 0;
#endif
    data->defwidth = -1;
    data->defheight = -1;
    data->is_resizable = 1;

    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
    mSet(data, obj, (APTR)msg, TRUE);
  }

  return (ULONG)obj;
}

static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
#if 0
    case MA_Custom_VertSpacing : rc = data->vertspacing; break;
    case MA_Custom_HorizSpacing: rc = data->horizspacing; break;
#endif
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, ULONG is_new)
{
  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem(&tstate))) {
    switch (tag->ti_Tag)
    {
      case MA_Widget : 
        data->widget=(GtkWidget *)tag->ti_Data;
        break;
      case MA_DefWidth:
        if (data->defwidth != tag->ti_Data)
        {
          data->defwidth = tag->ti_Data;
          relayout = 1;
        }
        break;

      case MA_DefHeight:
        if (data->defheight != tag->ti_Data)
        {
          data->defheight = tag->ti_Data;
          relayout = 1;
        }
        break;
    }
  }

  if (relayout) {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
}

#if 0
static VOID mRemMember(struct Data *data, struct opMember *msg)
{
  struct CustomNode *node;

  ForeachNode(&data->ChildList, node)
  {
    if (node->widget->MuiObject == msg->opam_Object)
    {
      REMOVE(node);
      mgtk_freemem(node, sizeof(*node));
      return;
    }
  }
}

struct Custom_Attach {
  GtkWidget *widget;
  ULONG left;
  ULONG right;
  ULONG top;
  ULONG bottom;
};

static ULONG mAttach(struct Data *data, APTR obj, struct MUIP_Custom_Attach *in) {
  struct CustomNode *node;
  struct Custom_Attach *msg;

  DebOut("mAttach(..)\n");

  DebOut("WARNING: BAD HACK INSIDE, TODO!!\n");
  /* for some reason, parameter handling sux here ?? */
  msg=(struct MUIP_Custom_Attach *) in->widget;

  /* but now, we got a correct message */
  DebOut("msg=%lx\n",msg);

  node = mgtk_allocmem(sizeof(*node), MEMF_ANY);

  if (node)
  {
    ADDTAIL(&data->ChildList, node);

    node->widget = msg->widget;
    node->left = msg->left;
    node->right = msg->right;
    node->top = msg->top;
    node->bottom = msg->bottom;

    DebOut("  msg->widget: %lx\n",msg->widget);
    DebOut("  msg->widget->MuiObject: %lx\n",msg->widget->MuiObject);
    DebOut("  obj: %lx\n",obj);
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, OM_ADDMEMBER, msg->widget->MuiObject);
    DoMethod(obj, MUIM_Group_ExitChange);
  }

  return 0;
}
#endif

static ULONG mAskMinMax(struct Data *data, APTR obj, struct MUIP_AskMinMax *msg, struct IClass *cl)
{
	DebOut("mAskMinMax(%lx,%lx,%lx,%lx)\n",data,obj,msg,cl);

  DoSuperMethodA(cl, obj, msg);

  if (!data->is_resizable || data->defwidth >=0 || data->defheight >= 0) {
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
        DebOut("outsch!\n");
        defwidth = mm->MinWidth;
      }
      else if (defwidth > mm->MaxWidth) {
        DebOut("outsch2! %d\n",mm->MaxWidth);
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

    DebOut("defheight=%d\n",defheight);
    DebOut("defwidth=%d\n",defwidth);

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


static ULONG mDraw(struct IClass *cl,struct Data *data,Object *obj,struct MUIP_Draw *msg)
{
  gint ret;
  GtkWidget *widget;
  GdkEventExpose *event;
  static gint (*callme)(GtkWidget *widget, GdkEventExpose *event);
  static void (*callme2) (GtkWidget *widget, GtkAllocation *allocation);

 
  /*
  ** let our superclass draw itself first, area class would
  ** e.g. draw the frame and clear the whole region. What
  ** it does exactly depends on msg->flags.
  */
  
  DoSuperMethodA(cl,obj,msg);
  
  /*
  ** if MADF_DRAWOBJECT isn't set, we shouldn't draw anything.
  ** MUI just wanted to update the frame or something like that.
  */
  
  if (!(msg->flags & MADF_DRAWOBJECT)) {
    return(0);
  }
  
  /*
  ** ok, everything ready to render...
  */

  DebOut("custom.c:mDraw(%lx,..)\n",obj);

  widget=mgtk_get_widget_from_obj(obj);

  widget->allocation.width=_mright(obj)-_mleft(obj);
  widget->allocation.height=_mbottom(obj)-_mtop(obj);

  DebOut("  widget->allocation.width=%d\n",widget->allocation.width);
  DebOut("  widget->allocation.height=%d\n",widget->allocation.height);

  DebOut("  widget=%lx\n",widget);
  if(widget->class) {
    DebOut("  widget->class=%lx\n",widget->class);
    if(widget->class->size_allocate) {
      callme2=widget->class->size_allocate;
      DebOut("  calling widget->class->size_allocate: %lx\n",callme2);
      (*callme2)(widget,&widget->allocation);
    }
    else {
      DebOut("  no expose_event defined in widget class\n");
    }

    if(widget->class->expose_event) {
      callme=widget->class->expose_event;
      DebOut("  calling widget->class->expose_event: %lx\n",callme);
      event=g_new0(GdkEventExpose,1);
      event->count=0;
      ret=(*callme)(widget,event);
      g_free(event);
      DebOut("  ret=%d\n",ret);
    }
    else {
      DebOut("  no expose_event defined in widget class\n");
    }
  }
  else {
    DebOut("  no widget class for this widget\n");
  }

  return(0);
}

BEGINMTABLE
GETDATA;

  switch (msg->MethodID) {
    case OM_NEW         : return mNew           (cl,   obj, msg);
  	case OM_SET         :        mSet           (data, obj, (APTR)msg, FALSE); break;
    case OM_GET         : return mGet           (data, obj, (APTR)msg, cl);
    case MUIM_AskMinMax : return mAskMinMax     (data, obj, (APTR)msg, cl);
    case MUIM_Draw      : return mDraw          (cl,   data, obj, (APTR)msg);
#if 0
    case OM_REMMEMBER   :        mRemMember     (data,      (APTR)msg); break;
    case MM_Custom_Attach: return mAttach        (data, obj, (APTR)msg);
#endif
  }

ENDMTABLE

int mgtk_create_custom_class(void) {
  CL_Custom = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Custom ? 1 : 0;
}

void mgtk_delete_custom_class(void) {
  if (CL_Custom) {
    MUI_DeleteCustomClass(CL_Custom);
  }
}
