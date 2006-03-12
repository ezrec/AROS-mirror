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

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

/* a lot of the following is taken form the InputHandler
 * example of MUI, but as I attach the handler to an
 * invisible/not attached Object, I do all the
 * Setup/Cleanup stuff here. Just the trigger is
 * called through the object.
 */

typedef struct {
  int badmagic;
  struct MsgPort     *port;
  struct timerequest *req;
  struct MUI_InputHandlerNode ihnode;
  Object *obj;
} mgtk_timer;

guint g_timeout_add(guint interval, GSourceFunc function, gpointer parameter) {
  Object *ret;
  mgtk_timer *data;

  DebOut("g_timeout_add(%d,%lx,%lx)\n",interval,function,parameter);

  ret = (APTR) NewObject(CL_Timer->mcc_Class, NULL,MA_Timer_Intervall,(ULONG) interval,MA_Timer_Function,(ULONG) function, MA_Timer_Data,(ULONG) parameter,TAG_DONE);

  if(!ret) {
    ErrOut("g_timeout_add: unable to create NewObject\n");
  }

  data=g_new(mgtk_timer,1);

  data->badmagic=BADMAGIC;

  if ((data->port = CreateMsgPort())) {
    if ((data->req = (struct timerequest *) CreateIORequest(data->port,sizeof(struct timerequest)))) {
      if ((!OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)data->req,0))) {
        data->ihnode.ihn_Object  = ret;
        data->ihnode.ihn_Millis  = interval;
        data->ihnode.ihn_Method  = MUIM_Class5_Trigger;
        data->ihnode.ihn_Flags   = MUIIHNF_TIMER;

        data->req->tr_node.io_Command = TR_ADDREQUEST;
        data->req->tr_time.tv_secs    = 1;
        data->req->tr_time.tv_micro   = 0;

        SendIO((struct IORequest *)data->req);

        DoMethod(mgtk->MuiApp,MUIM_Application_AddInputHandler,&data->ihnode);

        data->obj=ret;
      }
    }
  }

  return (guint) data;
}

/* 
 * g_source_remove needs to cleanup the mess,
 * I caused above ;)
 */

gboolean g_source_remove(guint tag) {

  mgtk_timer *data;

  DebOut("g_source_remove(%d)\n",tag);
  data=(mgtk_timer *)tag;

  if(!data) {
    DebOut("WARNING: g_source_remove called with NULL pointer!\n");
    return FALSE;
  }

  DoMethod(mgtk->MuiApp,MUIM_Application_RemInputHandler,&data->ihnode);

  if(!data->req) {
    DebOut("WARNING: g_source_remove data->req== NULL!\n");
    return FALSE;
  }

  if (!CheckIO((struct IORequest *) data->req)) {
    AbortIO((struct IORequest *) data->req);
  }
  WaitIO((struct IORequest *) data->req);

  if (data->req->tr_node.io_Device) {
    CloseDevice((struct IORequest *)data->req);
  }
  DeleteIORequest((struct IORequest *) data->req);
  data->req=NULL;

  if (data->port) {
    DeleteMsgPort(data->port);
    data->port=NULL;
  }

  if(data->obj) {
    DisposeObject(data->obj);
  }

  g_free(data);

  return TRUE;
}

/*
 * gtk_timeout_add is deprecated and should not be 
 * used in newly-written code. Use g_timeout_add() instead.
 */
guint gtk_timeout_add(guint32 interval, GtkFunction function, gpointer data) {

  return g_timeout_add(interval,function,data);
}

/* 
 * gtk_timeout_remove is deprecated and should not be 
 * used in newly-written code. Use g_source_remove() instead.
 */
void gtk_timeout_remove(guint timeout_handler_id) {

  g_source_remove(timeout_handler_id);
}

