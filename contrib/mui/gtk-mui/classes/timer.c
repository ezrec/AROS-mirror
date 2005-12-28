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
 * $Id: timer.c,v 1.3 2005/12/12 14:50:00 itix Exp $
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data {
  guint32 intervall;
  GSourceFunc function;
  gpointer parameter;
};

/*******************************************
 * MUIHook_timer
 *******************************************/
HOOKPROTO(MUIHook_timer, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT

  DebOut("MUIHook_timer(%x) called\n", (int) obj);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_timer, MUIHook_timer);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  guint32 intervall;
  GSourceFunc function;
  gpointer parameter;

  struct TagItem *tstate, *tag;

  DebOut("mNew (timer)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("timer: unable to create object!");
    return (ULONG) NULL;
  }
  else
  {
    GETDATA;

    DebOut("  new obj=%lx\n",obj);

    tstate=((struct opSet *)msg)->ops_AttrList;

    intervall=0;
    function=0;
    parameter=0;
    while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
    {
      switch (tag->ti_Tag)
      {
        case MA_Timer_Intervall:
          intervall = (guint32) tag->ti_Data;
          break;
        case MA_Timer_Data:
          parameter = (gpointer) tag->ti_Data;
          break;
       case MA_Timer_Function:
          function = (GSourceFunc) tag->ti_Data;
          break;
      }
    }

    if(!intervall || !function) {
      ErrOut("classes/timer.c: mNew: required parameter not supplied!\n");
      DebOut("ERROR: classes/timer.c: mNew: required parameter not supplied (%d,%lx)!\n",intervall,function);
      CoerceMethod(cl, obj, OM_DISPOSE);
      return (ULONG) NULL;
    }

    data->function=function;
    data->parameter=parameter;
    data->intervall=intervall;
  }

  return (ULONG)obj;
}

ULONG mTrigger(struct IClass *cl,Object *obj,Msg msg) {
  void (*callme)(gpointer parameter);
  GETDATA;

  DebOut("mTrigger!\n");

  DebOut("data=%lx\n",data);
  callme=(APTR) data->function;

  DebOut("calling: %lx(%lx)\n",callme,data->parameter);

  (*callme)(data->parameter);

  return(FALSE);
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE

  switch (msg->MethodID)
  {
    case OM_NEW             : return(mNew    (cl,obj,(APTR)msg));
    case MUIM_Class5_Trigger: return(mTrigger(cl,obj,(APTR)msg));
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_timer_class(void)
{
  DebOut("mgtk_create_timer_class()\n");

  CL_Timer = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_Timer=%lx\n",CL_Timer);

  return CL_Timer ? 1 : 0;
}

void mgtk_delete_timer_class(void)
{
  if (CL_Timer)
  {
    MUI_DeleteCustomClass(CL_Timer);
  }
}
