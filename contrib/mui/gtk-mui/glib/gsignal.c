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
 * $Id: gsignal.c,v 1.4 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <libraries/mui.h>
#include <stdarg.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

/* g_signal_new
 *
 * Creates a new signal. (This is usually done in the class initializer.)
 *
 * A signal name consists of segments consisting of ASCII letters and digits, 
 * separated by either the '-' or '_' character. The first character of a 
 * signal name must be a letter. Names which violate these rules lead to 
 * undefined behaviour of the GSignal system. 
 */

/* example:
  g_signal_new ("tictactoe",
  G_TYPE_FROM_CLASS (object_class),
  G_SIGNAL_RUN_FIRST,
  0,
  NULL,
  NULL,
  g_cclosure_marshal_VOID__VOID,
  G_TYPE_NONE, 0, NULL);
*/

guint g_signal_new(const gchar *signal_name, GType itype, GSignalFlags signal_flags, guint class_offset, GSignalAccumulator accumulator, gpointer accu_data, GSignalCMarshaller c_marshaller, GType return_type, guint n_params, ...) {

  mgtk_SignalType *newsignal;
  mgtk_SignalType *i;

  DebOut("g_signal_new(%s,...)\n",signal_name);

  if(n_params) {
    WarnOut("g_signal_new n_params ignored\n");
  }

  newsignal=g_new(mgtk_SignalType,sizeof(mgtk_SignalType));
  newsignal->type=100; /* ?? TODO */
  newsignal->name=g_strdup(signal_name);
  newsignal->next=NULL;

  if(!mgtk->mgtk_signal) {
    /* first signal, start count with 100 */
    mgtk->mgtk_signal_count=100;
    newsignal->id=mgtk->mgtk_signal_count;

    mgtk->mgtk_signal=newsignal;
  }
  else {
    mgtk->mgtk_signal_count++;
    newsignal->id=mgtk->mgtk_signal_count;
    i=mgtk->mgtk_signal;
  
    while(i->next) {
      i=i->next;
    }
    i->next=newsignal;
  }
  return newsignal->id;
}

/* Given the name of the signal and the type of object 
 * it connects to, gets the signal's identifying integer. 
 * Emitting the signal by number is somewhat faster 
 * than using the name each time.
 */

guint g_signal_lookup(const gchar *name, GType itype) {

  mgtk_SignalType *s;

  DebOut("g_signal_lookup(%s,%d)\n",name,itype);
  DebOut("WARNING: itype ignored\n");

  s=mgtk->mgtk_signal;

  while(s) {
    if(!strcmp(s->name,name)) {
      return s->id;
    }
    s=s->next;
  }
  return 0;
}

  /* g_signal_connect_data may be called on Adjustments, which have no MUI object themselves.
   * So we just store the call and redo it, as soon as the adjustment is connected to
   * a real Object.
   * On the other hand, the adjustment may have been used for a real widget
   * already. In this case, you have to store the widget in the GSList
   * of the adjustment. See spinbutton for an example.
	 */

  /* Menuitems might be used in "real menus", then the applied hook
   * works. But they might end up in an option menu,
   * and the hooks would not work. So remember them additionally
   */

	/* Fileselectors are quite different under AmigaOS than under GTK
	 * So we remember the callbacks for them, too
	 */

  /* if we have a custom/new style signal, we will deal with at 
   * the beginning (we will remember them)
   * In the long run, all signals should be handled in this way..
   *
   * At the moment, the following signals are handled here:
   *
   * - custom signals (of course)
   * - toggled signals
   *
   */

gulong g_signal_connect_data(gpointer           instance, /*widget */
                             const gchar       *detailed_signal,
                             GCallback          c_handler, /* callback*/
                             gpointer           data,
                             GClosureNotify     destroy_data,
                             GConnectFlags      connect_flags) {

  GtkWidget *widget;
  GtkWidget *w;
  mgtk_signal_connect_data *store;
  mgtk_signal_connect_data *newrem;
  mgtk_signal_connect_data *i;
  GSList *gs;

  widget=(GtkWidget *) instance;

  DebOut("g_signal_connect_data(%lx,%s,%lx,%lx,%lx)\n",instance,detailed_signal,c_handler,data,destroy_data,connect_flags);

  if(g_signal_lookup(detailed_signal,0)) {
    DebOut("  custom/newstyle signal %s detected\n",detailed_signal);

    store=widget->mgtk_signal_connect_data;
    DebOut("  store=%lx\n",store);
    if(store) {
      while(store->next) {
        DebOut("    store->next=%lx\n",store->next);
        if(store!=store->next) {
          store=store->next;
        }
        else {
          store->next=NULL;
          WarnOut("gtk_hooks.c: Loop detected (fixed the hard way)\n");
        }
      }
    }

    DebOut("found store: %lx\n",store);

    /* create and init new mgtk_signal_connect_data */
    newrem=g_new0(mgtk_signal_connect_data,sizeof(mgtk_signal_connect_data));
    DebOut("newrem=%lx\n",newrem);
    newrem->detailed_signal=g_strdup(detailed_signal);
    newrem->id=g_signal_lookup(detailed_signal,0);
    newrem->c_handler=c_handler;
    newrem->data=data;
    newrem->destroy_data=destroy_data;
    newrem->connect_flags=connect_flags;
    newrem->next=NULL;

    /* add it to list */
    if(widget->mgtk_signal_connect_data) {
      DebOut("  appending mgtk_signal_connect_data\n");
      store->next=newrem;
    }
    else {
      DebOut("  stored first mgtk_signal_connect_data\n");
      widget->mgtk_signal_connect_data=newrem;
    }
    DebOut("  remembered it for later use\n");

    /* An adjustment may have been used for a real widget
     * already. In this case, you have to store the widget in the GSList
     * of the adjustment. See spinbutton for an example.
     */
    if(widget->type==IS_ADJUSTMENT) {
      if(widget->GSList) {
        /* we already have widgets, which use this adjustment */
        DebOut("  connecting adjustment signals to stored widgets\n");
        gs=widget->GSList;
        while(gs) {
          w=(GtkWidget *) gs->data;
          /* copy all signals */
          /* this should be *one* list, referred from more
           * objects, TODO: introduce counter and just link the
           * list:
           * w->mgtk_signal_connect_data=widget->mgtk_signal_connect_data;
           */
          i=widget->mgtk_signal_connect_data;
          while(i) {
            g_signal_connect_data(w,i->detailed_signal,i->c_handler,i->data,i->destroy_data,i->connect_flags);
            i=i->next;
          }
          gs=gs->next;
        }
      }
    }

    return 0;
  }

  WarnOut("g_signal_connect_data: unknown signal %s ignored\n",detailed_signal);
  return 0;
}

/* 
 * g_signal_emit just has to look through the callbacks
 * stored in widget->mgtk_signal_connect_data, if it
 * finds matching signal_ids. If yes, call the
 * hooks according to the connect_flags.
 * This seems to be more clean, than the mess in
 * gtk_hooks. Maybe I'll have to rewrite gtk_hooks..
 */
void g_signal_emit(gpointer instance, guint signal_id, GQuark detail, ...) {

  GtkWidget *widget;
  mgtk_signal_connect_data *s;
  void (*callme)(GtkWidget *widget,gpointer data);

  DebOut("g_signal_emit(%lx,%d,..)\n",instance,signal_id);

  widget=(GtkWidget *) instance;

  s=widget->mgtk_signal_connect_data;

  while(s) {
    /* DebOut("  compare s->id %d == signal_id %d\n",s->id,signal_id); */
    if(s->id == signal_id) {
      /* now call all matching callbacks of this item */
      callme=(void *) s->c_handler;
      DebOut("  calling custom/newstyle signal hook %lx..\n",callme);
      if(s->connect_flags == G_CONNECT_SWAPPED) {
        (*callme)(s->data,widget);
      }
      else {
        (*callme)(widget,s->data);
      }
    }
    if(s == s->next) {
      WarnOut("loop in mgtk_signal_connect_data detected! (fixed hard)\n");
      /* fix the loop in a hard way and pray ;) */
      s->next=NULL;
    }
    s=s->next;
  }
  /* we did it, phew. */
}

/* Emits a signal.
 *
 * TODO:
 * Note that g_signal_emit_by_name() resets the return 
 * value to the default if no handlers are connected, 
 * in contrast to g_signal_emitv().
 * instance :  the instance the signal is being emitted on.
 * detailed_signal :   a string of the form "signal-name::detail".
 * ... :   parameters to be passed to the signal, followed 
 * by a location for the return value. If the return type of 
 * the signal is G_TYPE_NONE, the return value location can be omitted.
*/
void g_signal_emit_by_name(gpointer instance, const gchar *detailed_signal, ...) {

  DebOut("g_signal_emit_by_name(%lx,%s,..)\n",instance,detailed_signal);

  g_signal_emit(instance,g_signal_lookup(detailed_signal,0),0);
}

/* Disconnects all handlers on an instance that match func and data.
 * instance :  The instance to remove handlers from.
 * func :  The C closure callback of the handlers (useless for non-C closures).
 * data :  The closure data of the handlers' closures.
 * Returns :   The number of handlers that got disconnected.
 */

static void remove_signal_handler(GtkWidget *widget,mgtk_signal_connect_data *deleteme) {
  mgtk_signal_connect_data *s;

  if(deleteme==widget->mgtk_signal_connect_data) {
    widget->mgtk_signal_connect_data=NULL;
  }
  else {
    s=widget->mgtk_signal_connect_data;
    while(s->next != deleteme) {
      /* this should terminate, match was found before */
      s=s->next;
    }
    /* skip deleteme */
    s->next=deleteme->next;
  }
  g_free(deleteme);
}

guint g_signal_handlers_disconnect_by_func(gpointer instance, GCallback func, gpointer data) {
  GtkWidget *widget;
  mgtk_signal_connect_data *s;
  guint count;

  DebOut("g_signal_handlers_disconnect_by_func(%lx,%lx,%lx)\n",instance,func,data);

  widget=(GtkWidget *) instance;

  s=widget->mgtk_signal_connect_data;

  count=0;
  while(s) {
    if((s->data == data) && (s->c_handler == func)){
      /* now we have to remove this item */
      count++;
      remove_signal_handler(widget,s);
    }
    if(s == s->next) {
      WarnOut("loop in mgtk_signal_connect_data detected!\n");
      /* fix the loop in a hard way and pray ;) */
      s->next=NULL;
    }
    s=s->next;
  }
  return count;
}
