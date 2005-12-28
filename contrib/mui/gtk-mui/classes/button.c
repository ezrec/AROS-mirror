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
 * $Id: button.c,v 1.5 2005/12/12 15:03:36 itix Exp $
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
  Object *button;
  struct MUI_EventHandlerNode HandlerNode;
};

/*******************************************
 * MUIHook_button
 *
 * emit the right signal(s)
 *******************************************/
HOOKPROTO(MUIHook_button, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;
  guint pressed;

  DebOut("MUIHook_button called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) widget);

  pressed=xget(obj,MUIA_Selected);

  if(pressed) {
    DebOut("  emit newstyle pressed signal \n");
    g_signal_emit(widget,g_signal_lookup("pressed",0),0);
  }
  else { /* released */
    DebOut("  emit newstyle released signal \n");
    g_signal_emit(widget,g_signal_lookup("released",0),0);
    DebOut("  emit newstyle clicked signal\n");
    g_signal_emit(widget,g_signal_lookup("clicked",0),0);
  }

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_button, MUIHook_button);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  Object *button;
  GtkWidget *widget;
  const char *label;
  struct TagItem *tstate, *tag;

  DebOut("mNew (button)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("button: unable to create object!");
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
        case MA_Button_Label:
          label = (const char *) tag->ti_Data;
          break;
      }
    }

    if(!widget) {
      ErrOut("classes/button.c: mNew: MA_Widget not supplied!\n");
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    DebOut("  widget=%d\n",widget);

    if(label) {
      /* MUI buttons are Text.mui objects which copies label to an internal
       * buffer always unless MUIA_Text_Copy, FALSE. We can retrieve label
       * by reading MUIA_Text_Contents. -itix
       */
      button=MUI_MakeObject(MUIO_Button, (ULONG)label);
    }
    else {
      button=VGroup,
            GroupSpacing(0),
            MUIA_Background, MUII_ButtonBack,
            ButtonFrame,
            MUIA_InputMode , MUIV_InputMode_RelVerify,
          End;
    }

    /* setup internal hooks */

    if (button)
    {
      SetAttrs(button, MUIA_CycleChain, 1, MUIA_Weight, 0, TAG_DONE);

      DoMethod(button,MUIM_Notify,MUIA_Selected,MUIV_EveryTime, button,2,MUIM_CallHook,&MyMuiHook_button);

      data->widget=widget;

      data->button=NULL;
      DoMethod(obj,OM_ADDMEMBER,button);
      data->button=button;
    }
    else
    {
      CoerceMethod(cl, obj, OM_DISPOSE);
      obj = NULL;
    }
  }

  return (ULONG)obj;
}

/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget of this radio
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

static int mAddMember(struct Data *data, Object *obj,struct opMember *msg,struct IClass *cl) {

  DebOut("button.c: mAddMember(%lx,%lx)\n",obj,msg->opam_Object);

  if(data->button) {
    /* now someone added a new object to us, so don't add it to the
     * group object, but to the button!*/
    DebOut("  data->button,OM_ADDMEMBER,msg->opam_Object\n");
    DoMethod(data->button,OM_ADDMEMBER,msg->opam_Object);
    return TRUE;
  }
  else {
    /* if !data->button, we want to add the button to the object,
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
    case OM_NEW          : return mNew        (cl, obj, msg);
    case OM_GET          : return mGet        (data, obj, (APTR)msg, cl);
    case OM_ADDMEMBER    : return mAddMember  (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_button_class(void)
{
  DebOut("mgtk_create_button_class()\n");

  CL_Button = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Button=%lx\n",CL_Button);

  return CL_RadioButton ? 1 : 0;
}

void mgtk_delete_button_class(void)
{
  if (CL_Button)
  {
    MUI_DeleteCustomClass(CL_Button);
  }
}
