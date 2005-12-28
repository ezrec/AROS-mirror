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
 * $Id: radiobutton.c,v 1.3 2005/12/12 15:18:52 itix Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data
{
  GtkWidget *widget;
  Object *box;
  Object *radio;
};

void mgtk_radio_widget_update(GtkWidget *widget) {

  GtkWidget *act;
  GSList *next;

  DebOut("mgtk_radio_widget_update(%lx)\n",widget);

  if(!widget->active) {
    next=gtk_radio_button_get_group(widget);
    DebOut("  group length: %ld\n",g_slist_length(next));

    while(next!=NULL) {
      act=next->data;
      /* clear the previous active object */
      if(act->active) {
        nnset(act->MuiObject,MUIA_Selected,0);
        act->active=0;
      }
      next=g_slist_next(next);
    }
    /* make our object the active one */
    widget->active=1;
  }
  nnset(widget->MuiObject,MUIA_Selected,1);

  DebOut("  emit newstyle toggled signal\n");
  g_signal_emit(widget,g_signal_lookup("toggled",0),0);
}

/*******************************************
 * MUIHook_radiobutton
 *
 * clear/set all other widget objects in
 * our group and emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_radio, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *active;

  DebOut("MUIHook_radio called(%lx)\n",obj);
  active=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) active);

  mgtk_radio_widget_update(active);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_radio, MUIHook_radio);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  Object *box;
  Object *radio;
  Object *space;
  Object *labelobj;
  GtkWidget *widget;
  const char *label;
  struct TagItem *tstate, *tag;

  DebOut("mNew (radiobutton)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("radiobutton: unable to create object!");
    return (ULONG) NULL;
  }
  else
  {
    GETDATA;

    DebOut("  new obj=%lx\n",obj);

    tstate=((struct opSet *)msg)->ops_AttrList;

    widget=NULL;
    label=NULL;
    while ((tag = (struct TagItem *) NextTagItem(&tstate)))
    {
      switch (tag->ti_Tag)
      {
        case MA_Widget:
          widget = (GtkWidget *) tag->ti_Data;
          break;
        case MA_Radio_Label:
          label = (const char *) tag->ti_Data;
          break;
      }
    }

    if(!widget) {
      ErrOut("classes/radiobutton.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    DebOut("  widget=%lx\n",widget);

    box=HGroup,
        End;

    radio=ImageObject,
            NoFrame,
            MUIA_CycleChain       , 1,
            MUIA_InputMode        , MUIV_InputMode_Toggle,
            MUIA_Image_Spec       , MUII_RadioButton,
            MUIA_ShowSelState     , FALSE,
          End;

    labelobj=Label1(label);
    space=HSpace(0);


    DoMethod(box,OM_ADDMEMBER,radio);
    DoMethod(box,OM_ADDMEMBER,labelobj);
    DoMethod(box,OM_ADDMEMBER,space);
    DoMethod(obj,OM_ADDMEMBER,box);

    data->box=box;
    data->radio=radio;
    data->widget=widget;

    /* setup internal hooks */
    DoMethod(radio,MUIM_Notify,MUIA_Selected,MUIV_EveryTime, radio,2,MUIM_CallHook,&MyMuiHook_radio);
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 * MA_Toggle_Value only usefull TAG here
 * MA_Radio_String_Value *private*
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;
  GtkWidget *widget;

	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Toggle_Value:
        DebOut("mSet(%lx) to %d\n",obj,tag->ti_Data);
        widget=mgtk_get_widget_from_obj(obj);
        if(!widget) {
          ErrOut("classes/radiobutton.c: mgtk_get_widget_from_obj return NULL !?\n");
        }
        else {
          if(tag->ti_Data) { /* radio can only be set to TRUE */
            mgtk_radio_widget_update(widget);
//            mgtk_radio_value_update0(widget,data,(int)tag->ti_Data);
          }
          else {
            DebOut("WARNING: MA_Toggle_Value=0 for a radio widget !? (ignored)\n");
          }
        }
        break;
    }
  }
}

/*******************************************
 * mGet
 * 
 * MA_Toggle_Value: value as string
 * MA_Widget: GtkWidget of this radio
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    case MA_Toggle_Value : 
      rc=xget(data->radio, MUIA_Selected);
      DebOut("mGet: MA_Toggle_Value =%d\n", rc);
      break;
    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_radiobutton_class(void)
{
  DebOut("mgtk_create_radiobutton_class()\n");

  CL_RadioButton = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_RadioButton=%lx\n",CL_RadioButton);

  return CL_RadioButton ? 1 : 0;
}

void mgtk_delete_radiobutton_class(void)
{
  if (CL_RadioButton)
  {
    MUI_DeleteCustomClass(CL_RadioButton);
  }
}
