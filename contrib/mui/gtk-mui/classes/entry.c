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
};

/*******************************************
 * MUIHook_entry
 *******************************************/
HOOKPROTO(MUIHook_entry, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_entry(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  g_signal_emit_by_name(widget,"activate",0);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_entry, MUIHook_entry);

/*******************************************
 * MUIHook_entry2
 *******************************************/
/*
HOOKPROTO(MUIHook_entry2, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_entry2(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  g_signal_emit_by_name(widget,"changed",0);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_entry2, MUIHook_entry2);
*/

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg) {
  GtkWidget *widget;
  struct TagItem *tstate, *tag;

  DebOut("mNew (entry)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("entry: unable to create object!");
    return (ULONG) NULL;
  }

  set(obj,MUIA_Frame, MUIV_Frame_String);
  set(obj,MUIA_CycleChain, 1);

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  while ((tag = (struct TagItem *) NextTagItem(&tstate))) {
    switch (tag->ti_Tag) {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("classes/entry.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%lx\n",widget);

  /* setup internal hooks */
  DoMethod(obj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime, obj,2,MUIM_CallHook,&MyMuiHook_entry);
  /*
  DoMethod(obj,MUIM_Notify,MUIA_String_Contents,MUIV_EveryTime, obj,2,MUIM_CallHook,&MyMuiHook_entry2);
  */

  if (obj) {
    GETDATA;

    data->widget=widget;
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
	DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);
#if 0
  struct TagItem *tstate, *tag;
  GtkWidget *widget;

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
    }
  }
#endif
}

/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
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
int mgtk_create_entry_class(void)
{
  DebOut("mgtk_create_entry_class()\n");

  CL_Entry = MUI_CreateCustomClass(NULL, MUIC_String, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Entry=%lx\n",CL_Entry);

  return CL_Entry ? 1 : 0;
}

void mgtk_delete_entry_class(void)
{
  if (CL_Entry)
  {
    MUI_DeleteCustomClass(CL_Entry);
  }
}
