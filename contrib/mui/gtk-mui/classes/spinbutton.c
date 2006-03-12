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
#include "gtk_globals.h"

struct Data
{
  GtkWidget *widget;
  Object *text;
  Object *up;
  Object *down;
};

/*******************************************
 * mgtk_spin_value_update(GtkWidget *widget)
 *
 * update shown text to widget->value
 *******************************************/
 /*internal */
static void mgtk_spin_value_update0(GtkWidget *widget, gdouble f) {
  char format[]="%%4.%xxf";
  const char *text;
  gdouble old;

  DebOut("mgtk_spin_value_update0(%x,..)\n",widget);

  old=widget->value;

  /* care for boundaries */
  if(f < widget->lower) {
    if(widget->wrap) {
      f=widget->upper;
    }
    else {
      f=widget->lower;
    }
  }
  else if(f > widget->upper) {
    if(widget->wrap) {
      f=widget->lower;
    }
    else {
      f=widget->upper;
    }
  }

  /* new value */
  widget->value=f;

  sprintf(format,"%%4.%df",widget->decimalplaces);
  text=g_strdup_printf(format,widget->value);

  set(widget->MuiObject,MA_Spin_String_Value,text);

  if(widget->value != old) {
    g_signal_emit(widget,g_signal_lookup("value_changed",0),0);
  }
  g_free((APTR) text);
}

static void mgtk_spin_value_update(GtkWidget *widget) {
  const char *text;
  gdouble f;

  DebOut("mgtk_spin_value_update(%x)\n",(int) widget);

  text=(const char *) xget(widget->MuiObject,MA_Spin_Value);
  DebOut("  New value: %s\n",text);
  f=atof(text);

  /* DebOut seems to be unable to print %f ..?
  DebOut("===>Float: %2.2f<====\n",f);
  */

  if(widget->value != f) {
    mgtk_spin_value_update0(widget,f);
  }
}

/*******************************************
 * MUIHook_spinbutton
 *******************************************/
HOOKPROTO(MUIHook_spinbutton, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_spinbutton(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  g_signal_emit(widget,g_signal_lookup("activate",0),0);

  mgtk_spin_value_update(widget);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_spinbutton, MUIHook_spinbutton);

/*******************************************
 * up arrow
 *******************************************/
HOOKPROTO(MUIHook_up, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_up called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) widget);

  mgtk_spin_value_update0(widget,widget->value + widget->step_increment);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_up, MUIHook_up);

/*******************************************
 * down arrow
 *******************************************/
HOOKPROTO(MUIHook_down, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_down called\n");
  widget=mgtk_get_widget_from_obj(obj);
  DebOut("  widget=%x\n",(int) widget);

  mgtk_spin_value_update0(widget,widget->value - widget->step_increment);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_down, MUIHook_down);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  Object *text;
  Object *updown;
  Object *up;
  Object *down;
  Object *hgroup;
  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  DebOut("mNew (spinbutton)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("spinbutton: unable to create object!");
    return (ULONG) NULL;
  }

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
    ErrOut("classes/spinbutton.c: mNew: MA_Widget not supplied!\n");
    CoerceMethod(cl, obj, OM_DISPOSE);
    return (ULONG) NULL;
  }

  DebOut("  widget=%d\n",widget);

  hgroup=HGroup, 
          MUIA_Group_HorizSpacing, 0,
          End;

  text=StringObject,
        StringFrame,
        MUIA_CycleChain, 1,
        MUIA_String_MaxLen, 256,
        MUIA_String_Contents, "",
        MUIA_String_Accept, "0123456789.",
  End;

  updown=VGroup,
    MUIA_Group_VertSpacing, 0,
    Child,
      up=ImageObject, 
            ImageButtonFrame,
            MUIA_CycleChain, 1,
            MUIA_InputMode,MUIV_InputMode_RelVerify,
            MUIA_Image_Spec, MUII_ArrowUp,
            /* itix ..? */
/*                      MUIA_FixHeight, 4,
            MUIA_Image_FreeVert, TRUE,
            MUIA_Image_FreeHoriz, TRUE,*/
            MUIA_Background , MUII_ImageButtonBack,
      End,
    Child,
      down=ImageObject, 
            ImageButtonFrame,
            MUIA_CycleChain, 1,
            MUIA_InputMode,MUIV_InputMode_RelVerify,
            MUIA_Image_Spec, MUII_ArrowDown,
            MUIA_Background , MUII_ImageButtonBack,
      End,
    End;

  DoMethod(obj,OM_ADDMEMBER,hgroup);
  DoMethod(hgroup,OM_ADDMEMBER,text);
  DoMethod(hgroup,OM_ADDMEMBER,updown);

  /* setup internal hooks */
  DoMethod(text,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime, text,2,MUIM_CallHook,&MyMuiHook_spinbutton);
  DoMethod(up,MUIM_Notify,MUIA_Pressed,FALSE, up,2,MUIM_CallHook,&MyMuiHook_up,1);
  DoMethod(down,MUIM_Notify,MUIA_Pressed,FALSE, down,2,MUIM_CallHook,&MyMuiHook_down);

  if (obj)
  {
    GETDATA;

    data->text=text;
    data->up=up;
    data->down=down;
    data->widget=widget;
  }

  return (ULONG)obj;
}

/*******************************************
 * mNew
 * 
 * MA_Spin_Value only usefull TAG here
 * MA_Spin_String_Value *private*
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
      case MA_Spin_Value:
        DebOut("mSet(%lx) to %s\n",obj,tag->ti_Data);
        widget=mgtk_get_widget_from_obj(obj);
        mgtk_spin_value_update0(widget,atof((const char *)tag->ti_Data));
        break;
      case MA_Spin_String_Value:
        DebOut("mSet string (%lx) to %s\n",obj,tag->ti_Data);
        set(data->text,MUIA_String_Contents,(LONG) tag->ti_Data);
        break;
      case MUIA_String_Accept:
        set(data->text,MUIA_String_Accept,(LONG) tag->ti_Data);
        break;
    }
  }
}

/*******************************************
 * mGet
 * 
 * MA_Spin_Value: value as string
 * MA_Widget: GtkWidget of this spin
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;
  const char *text;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    case MA_Spin_Value : 
      DebOut("mGet: data->text obj =%x\n",(int) data->text);
      get(data->text, MUIA_String_Contents, &text );
      DebOut("mGet: text=%s\n",text);
      rc = (ULONG) text; 
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
/*    case MUIM_Draw     : return(mDraw     (cl,obj,(APTR)msg));*/
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_spinbutton_class(void)
{
  DebOut("mgtk_create_spinbutton_class()\n");

  CL_SpinButton = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_SpinButton=%lx\n",CL_SpinButton);

  return CL_SpinButton ? 1 : 0;
}

void mgtk_delete_spinbutton_class(void)
{
  if (CL_SpinButton)
  {
    MUI_DeleteCustomClass(CL_SpinButton);
  }
}
