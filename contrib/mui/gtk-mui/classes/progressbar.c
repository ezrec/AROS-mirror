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
 * $Id: progressbar.c,v 1.6 2005/12/03 23:54:18 o1i Exp $
 *
 *****************************************************************************/

/* This class ca be both, busy or progress bar.
 * To switch use MA_Pulse:
 * 1: busy mode
 * 0: activity mode
 *
 * TODO: 
 * in GTK you can display the current value of a progress bar with
 * the string pattern %P, in MUI it is %ld, so if you want it
 * to work, you will have to do a conversion.
 */

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>
#include <MUI/Busy_mcc.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data
{
  GtkWidget *widget;
  Object *progressbar;
  Object *busy;
  gchar *text;
};

/*******************************************
 * MUIHook_progressbar
 *******************************************/
HOOKPROTO(MUIHook_progressbar, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_progressbar(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_progressbar, MUIHook_progressbar);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  GtkWidget *widget;
  Object *progressbar;
  struct TagItem *tstate, *tag;
  gchar *text;

  DebOut("mNew (progressbar)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("progressbar: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("classes/progressbar.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%d\n",widget);

  text=g_strdup("");

  progressbar=GaugeObject,
            GaugeFrame,
/*            MUIA_Gauge_InfoText, "%ld %%",*/
            MUIA_Gauge_InfoText, text,
            MUIA_Gauge_Horiz, TRUE,
          End;

  DoMethod(obj,OM_ADDMEMBER,progressbar);

  if (obj)
  {
    GETDATA;

    data->widget=widget;
    data->progressbar=progressbar;
    data->busy=NULL;
    data->text=text;
  }

  return (ULONG)obj;
}

static void pulse(struct Data *data,ULONG busy) {
  Object *obj;

  DebOut("pulse(%lx,%d)\n",data,busy);

  obj=data->widget->MuiObject;

  /* do we have to switch !? */
  if((!data->busy) && busy) {
    /* yes, replace with busy bar */
    DoMethod(obj,MUIM_Group_InitChange);

    DoMethod(obj,OM_REMMEMBER,data->progressbar);
    DisposeObject(data->progressbar);
    data->progressbar=NULL;

    data->busy=BusyBar;
    /* no automatic movement busy bar */
    set(data->busy,MUIA_Busy_Speed,MUIV_Busy_Speed_Off);
    DoMethod(obj,OM_ADDMEMBER,data->busy);

    DoMethod(obj,MUIM_Group_ExitChange);
  }
  else if((!data->progressbar) && (!busy)) {
    /* yes, replace with progress bar */
    DoMethod(obj,MUIM_Group_InitChange);

    DoMethod(obj,OM_REMMEMBER,data->busy);
    DisposeObject(data->busy);
    data->busy=NULL;

    data->progressbar=GaugeObject,
                        GaugeFrame,
                        MUIA_Gauge_InfoText, data->text,
                        MUIA_Gauge_Horiz, TRUE,
                      End;

    DoMethod(obj,OM_ADDMEMBER,data->progressbar);

    DoMethod(obj,MUIM_Group_ExitChange);
    return;
  }

  if(data->busy) {
    /* we got a pulse, so move it! */
    DoMethod(data->busy,MUIM_Busy_Move,TRUE);
  }

}
/*******************************************
 * mSet
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {

  struct TagItem *tstate, *tag;

  DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);
  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate))) {
    switch (tag->ti_Tag) {
      case MA_Pulse:
        pulse(data,tag->ti_Data);
        break;
      case MUIA_Gauge_InfoText:
        if(data->progressbar) {
          set(data->progressbar,MUIA_Gauge_InfoText,tag->ti_Data);
        }
        g_free(data->text);
        data->text=g_strdup((gchar *) tag->ti_Data);
        break;
    }

  }
}

/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget of this spin
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl) {

  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    case MUIA_Gauge_InfoText : 
      DebOut("mGet: MUIA_Gauge_InfoText=%s\n",data->text);
      rc = (ULONG) data->text;
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  switch (msg->MethodID) {
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_progressbar_class(void) {

  DebOut("mgtk_create_notebook_class()\n");

  CL_ProgressBar = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_ProgressBar=%lx\n",CL_ProgressBar);

  return CL_ProgressBar ? 1 : 0;
}

void mgtk_delete_progressbar_class(void) {

  if (CL_ProgressBar) {
    MUI_DeleteCustomClass(CL_ProgressBar);
  }
}
