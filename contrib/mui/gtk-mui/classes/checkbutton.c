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
 * $Id: checkbutton.c,v 1.2 2005/12/12 15:12:31 itix Exp $
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
  Object *check;
  Object *labelobj;
};

/*******************************************
 * mgtk_check_value_update(GtkWidget *widget)
 *
 *******************************************/
 /*internal */
static void mgtk_check_value_update0(GtkWidget *widget,struct Data *data, int b) {

  DebOut("mgtk_check_value_update0(%x,%x,%d)\n",widget,data,b);

  DebOut("  data->check=%lx\n",data->check);
  setcheckmark(data->check,b);
  widget->active=b;
}

/*******************************************
 * MUIHook_checkbutton
 *******************************************/
HOOKPROTO(MUIHook_check, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_check called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) widget);

  widget->active=xget(widget->MuiObject,MA_Toggle_Value);

  DebOut("  emit toggled signal\n");
  g_signal_emit(widget,g_signal_lookup("toggled",0),0);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_check, MUIHook_check);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  Object *box;
  Object *check;
  Object *labelobj;
  GtkWidget *widget;
  const char *label;
  struct TagItem *tstate, *tag;

  DebOut("mNew (checkbutton)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("checkbutton: unable to create object!");
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
        case MA_Check_Label:
          label = (const char *) tag->ti_Data;
          break;
      }
    }

    if(!widget) {
      ErrOut("classes/checkbutton.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    DebOut("  widget=%d\n",widget);

    box=(APTR) HGroup, 
            MUIA_CycleChain, 1,
            End;

    check=(APTR) MUI_MakeObject(MUIO_Checkmark, label);

    labelobj=Label1(label);

    DoMethod(obj,OM_ADDMEMBER,box);
    DoMethod(box,OM_ADDMEMBER,check);
    DoMethod(box,OM_ADDMEMBER,labelobj);
    DoMethod(box,OM_ADDMEMBER,HSpace(0));

    /* setup internal hooks */
    DoMethod(check,MUIM_Notify,MUIA_Selected,MUIV_EveryTime, check,2,MUIM_CallHook,&MyMuiHook_check);

    data->box=box;
    data->check=check;
    data->labelobj=labelobj;
    data->widget=widget;
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 * MA_Toggle_Value only usefull TAG here
 * MA_Check_String_Value *private*
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
        mgtk_check_value_update0(widget,data,(int)tag->ti_Data);
        break;
    }
  }
}

/*******************************************
 * mGet
 * 
 * MA_Toggle_Value: value as string
 * MA_Widget: GtkWidget of this check
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    case MA_Toggle_Value : 
      rc=xget(data->check, MUIA_Selected);
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
int mgtk_create_checkbutton_class(void)
{
  DebOut("mgtk_create_checkbutton_class()\n");

  CL_CheckButton = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_CheckButton=%lx\n",CL_CheckButton);

  return CL_CheckButton ? 1 : 0;
}

void mgtk_delete_checkbutton_class(void)
{
  if (CL_CheckButton)
  {
    MUI_DeleteCustomClass(CL_CheckButton);
  }
}
