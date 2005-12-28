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
 * $Id: togglebutton.c,v 1.2 2005/12/12 15:15:53 itix Exp $
 *
 *****************************************************************************/

 /* this one is quite similiar to the checkbutton class */

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data {
  GtkWidget *widget;
  Object *toggle;
  LONG defwidth;
  LONG defheight;
  struct Hook LayoutHook;
  int have_layout_hook;
};

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg);

/******************************
 * LayoutHook
 *
 * Hopefully will work both with
 * a set fixed size and without
 * one
 ******************************/
#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm) {
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif

  struct Data *data = hook->h_Data;
  Object *cstate;
  Object *child;

  DebOut("LayoutHook(%lx,..)\n",obj);

  switch (lm->lm_Type) {
    case MUILM_MINMAX: 
      DebOut("  data->defwidth: %d\n",data->defwidth);
      DebOut("  data->defheight: %d\n",data->defheight);

      lm->lm_MinMax.MinWidth  = 0;
      lm->lm_MinMax.MinHeight = 0;
      lm->lm_MinMax.DefWidth  = data->defwidth;
      lm->lm_MinMax.DefHeight = data->defheight;
      lm->lm_MinMax.MaxWidth  = MUI_MAXMAX;;
      lm->lm_MinMax.MaxHeight = MUI_MAXMAX;;
      DebOut("return FALSE\n");
      return 0;
    case MUILM_LAYOUT:
        cstate = (Object *)lm->lm_Children->mlh_Head;
        while ((child = NextObject(&cstate))) {
          if (!MUI_Layout(child,0,0,lm->lm_Layout.Width,lm->lm_Layout.Height,0)) {
            return(FALSE);
          }
        }
        return(TRUE);
  }
  return(MUILM_UNKNOWN);
}

/*******************************************
 * mgtk_toggle_value_update(GtkWidget *widget)
 *
 *******************************************/
 /*internal */
static void mgtk_toggle_value_update0(GtkWidget *widget,struct Data *data, int b) {

  DebOut("mgtk_toggle_value_update0(%x,%x,%d)\n",widget,data,b);

  DebOut("  data->toggle=%lx\n",data->toggle);
  nnset(data->toggle,MUIA_Selected,b);
  widget->active=b;
}

/*******************************************
 * MUIHook_togglebutton
 *
 * clear/set all other widget objects in
 * our group and emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_toggle, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *active;
  GtkWidget *act;
  GSList *next;
  int toggled;

  DebOut("MUIHook_toggle called\n");
  active=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) active);

  toggled=FALSE;
  /* active->active was our old state */
  if(!active->active) {
    next=gtk_radio_button_get_group(active);
    DebOut("  group length: %ld\n",g_slist_length(next));

    while(next!=NULL) {
      /* if we are in a radio group, clear all others */
      act=next->data;
      /* clear the previous active object */
      if(act->active) {
        nnset(act->MuiObject,MA_Toggle_Value,0);
        act->active=0;
      }
      next=g_slist_next(next);
    }
    /* make our object the active one */
    active->active=1;
    /* we have changed our status */
    toggled=TRUE;
  }
  else { /* active */
    if(gtk_radio_button_get_group(active)) {
      /* we are part of a radio group, then we stay active */
      nnset(active->MuiObject,MA_Toggle_Value,1);
    }
    else {
      /* if we are stand alone, we go inactive and we have changed our status*/
      active->active=0;
      nnset(active->MuiObject,MA_Toggle_Value,0);
      toggled=TRUE;
    }
  }

  if(toggled) {
    DebOut("  emit newstyle toggled signal\n");
    g_signal_emit(active,g_signal_lookup("toggled",0),0);
  }

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_toggle, MUIHook_toggle);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  GtkWidget *widget;
  Object *toggle;
  struct TagItem *tstate, *tag;

  DebOut("mNew (togglebutton)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("togglebutton: unable to create object!");
    return (ULONG) NULL;
  }
  else
  {
    GETDATA;

    DebOut("  new obj=%lx\n",obj);

    tstate=((struct opSet *)msg)->ops_AttrList;

    widget=NULL;
    while ((tag = (struct TagItem *) NextTagItem(&tstate)))
    {
      switch (tag->ti_Tag)
      {
        case MA_Widget:
          widget = (GtkWidget *) tag->ti_Data;
          break;
      }
    }

    if(!widget) {
      ErrOut("classes/togglebutton.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    DebOut("  widget=%d\n",widget);

    /* why another group?
     * Because the normal button does it the same way, and we can hide
     * all parameters in here.
     */

    toggle=VGroup,
            GroupSpacing(0),
            ButtonFrame,
            MUIA_InputMode , MUIV_InputMode_Toggle,
            MUIA_Background , MUII_ButtonBack,
            MUIA_ShowSelState , TRUE,
            MUIA_Selected , FALSE,
            MUIA_CycleChain, TRUE,
          End;

    if (toggle)
    {
      DoMethod(obj,OM_ADDMEMBER,toggle);

      /* setup internal hooks */
      DoMethod(obj,MUIM_Notify,MUIA_Selected,MUIV_EveryTime, obj,2,MUIM_CallHook,&MyMuiHook_toggle);

      SETUPHOOK(&data->LayoutHook, LayoutHook, data);

      widget->active=FALSE;
      data->widget=widget;
      data->toggle=toggle;
      data->defwidth =-1;
      data->defheight=-1;

      mSet(data, obj, (APTR)msg);
    }
    else
    {
      /* We are out of memory or something */
      CoerceMethod(cl, obj, OM_DISPOSE);
      obj = NULL;
    }
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 * MA_Toggle_Value only usefull TAG here
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
  GtkWidget *widget;
  ULONG relayout = 0;

	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Toggle_Value:
        DebOut("mSet(%lx) to %d\n",obj,tag->ti_Data);
        widget=mgtk_get_widget_from_obj(obj);
        mgtk_toggle_value_update0(widget,data,(int)tag->ti_Data);
        break;
      case MA_DefWidth:
        if(!data->have_layout_hook) {
          set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
          data->have_layout_hook=1;
        }
        if (data->defwidth != tag->ti_Data) {
          data->defwidth = tag->ti_Data;
          relayout = 1;
        }
        break;
      case MA_DefHeight:
        if(!data->have_layout_hook) {
          set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
          data->have_layout_hook=1;
        }
        if (data->defheight != tag->ti_Data) {
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

/*******************************************
 * mGet
 * 
 * MA_Toggle_Value: value as string
 * MA_Widget: GtkWidget of this toggle
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    case MA_Toggle_Value : 
      rc=xget(data->toggle, MUIA_Selected);
      DebOut("mGet: MA_Toggle_Value =%d\n", rc);
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

static int mRemMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {

  DebOut("togglebutton.c: mRemMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(data->toggle) {
    DebOut("  data->toggle,OM_REMMEMBER,msg->opam_Object\n");
    DoMethod(data->toggle,OM_REMMEMBER,msg->opam_Object);
    return TRUE;
  }
  else {
    /* if !data->toggle just pass it to the original method */
    DebOut("  DoSuperMethodA(cl, obj, (Msg)msg)\n");
    return DoSuperMethodA(cl, obj, (Msg)msg);
  }
}

static int mAddMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {

  DebOut("togglebutton.c: mAddMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(data->toggle) {
    /* now someone added a new object to us, so don't add it to the
     * group object, but to the inner group!*/
    DebOut("  data->toggle,OM_ADDMEMBER,msg->opam_Object\n");
    DoMethod(data->toggle,OM_ADDMEMBER,msg->opam_Object);
    return TRUE;
  }
  else {
    /* if !data->toggle, we want to add the button to the object,
     * so just pass it to the original method */
    DebOut("  DoSuperMethodA(cl, obj, (Msg)msg)\n");
    return DoSuperMethodA(cl, obj, (Msg)msg);
  }
}


/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew           (cl, obj, msg);
    case OM_SET         :        mSet           (data, obj, (APTR)msg); break;
    case OM_GET         : return mGet           (data, obj, (APTR)msg, cl);
    case OM_ADDMEMBER   : return mAddMember     (data, obj, (APTR)msg, cl);
    case OM_REMMEMBER   : return mRemMember     (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_togglebutton_class(void)
{
  DebOut("mgtk_create_togglebutton_class()\n");

  CL_ToggleButton = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_ToggleButton=%lx\n",CL_ToggleButton);

  return CL_ToggleButton ? 1 : 0;
}

void mgtk_delete_togglebutton_class(void)
{
  if (CL_ToggleButton)
  {
    MUI_DeleteCustomClass(CL_ToggleButton);
  }
}
