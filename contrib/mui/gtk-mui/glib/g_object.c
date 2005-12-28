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
 * $Id: g_object.c,v 1.7 2005/12/04 00:25:53 o1i Exp $
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
#include "classes/classes.h"

/*
 * This stuff contains a lot of bad magic.
 * If someone wants to write his own custom widget, he
 * - calls g_type_register_static with a parent widget type
 * - and with the returned new id he calls g_object_new
 *
 * so we try to emulate this here ;)
 * 
 * g_object_new calls the parent class new method and the 
 * custom class init function, resizes the class instance to
 * the correct size and returns the new class instance.
 */
gpointer g_object_new(GType object_type, const gchar *first_property_name, ...) {
  GtkWidget *ret;
  GtkWidget *tmpret;
  mgtk_Fundamental *f;
  GtkWidget *(*callme)();
  void (*callme2)(GtkWidget *widget);

  DebOut("g_object_new(%d,..)\n",object_type);
  WarnOut("g_object_new: some parameters ignored\n"); /* TODO */

  f=mgtk_g_type_get_fundamental(object_type);

  if(f) {
    if(f->newparentobject) {
      callme=(APTR) f->newparentobject;
      DebOut(" calling constructor of fundamental parent object_type\n");
      ret=(APTR) (*callme)(); /* TODO, parameters..!? */
      DebOut("f->instance_size: %d\n",f->instance_size);
      DebOut("sizeof(GtkWidget): %d\n",sizeof(GtkWidget));
      if(f->instance_size > sizeof(GtkWidget)) {
        /* copy ret to a new mem area with new size */
        tmpret=g_new(GtkWidget,f->instance_size);
        DebOut("  resize object\n");
        memcpy(tmpret,ret,sizeof(GtkWidget));
        g_free(ret);
        ret=tmpret;
        if(ret->MuiObject) {
          /* correct the reverse pointer ..*/
          set(ret->MuiObject,MA_Widget,ret);
        }
      }

      callme2=(APTR) f->newobject;
      DebOut("  calling constructor of object_type\n");
      (*callme2)(ret);
      /* now set up signal handlers of class, if present */
      if(f->class) {
        if(f->class->realize) {
          DebOut("  setting up realize handler\n");
          g_signal_connect_data(ret,"realized",G_CALLBACK(f->class->realize),ret,NULL,0);
          DebOut("==============> f->class->realize=%lx\n",f->class->realize);
        }
        DebOut("ret->class=f->class: %lx->class:=%lx\n",ret,f->class);
        ret->class=f->class;
      }

    }
    else {
      DebOut("  there is no newparentobject, may cause problems..?\n");
      callme=(APTR) f->newobject;
      DebOut(" calling constructor of object_type\n");
      ret=(APTR) (*callme)();
    }
  }
  else {
    DebOut("WARNING: g_object_new returns NULL, we will soon crash..\n");
    ret=NULL;
  }

  return (gpointer) ret;
}

GType g_type_register_static(GType parent_type, const gchar *type_name, const GTypeInfo *info, GTypeFlags flags) {

  GType new_type;
  mgtk_Fundamental *newf;
  mgtk_Fundamental *oldf;
  GtkWidgetClass *class;
  void (*call_class_init)(APTR class);

  DebOut("g_type_register_static(%d,%s,%ld,%d)\n",parent_type,type_name,info,flags);

  if(!mgtk->mgtk_fundamental_count) {
    /* init */
    mgtk->mgtk_fundamental_count=IS_LASTONE;
  }
  mgtk->mgtk_fundamental_count++;
  new_type=mgtk->mgtk_fundamental_count;

  if(!mgtk_g_type_register_fundamental(new_type,type_name,NULL)) {
    ErrOut("g_type_register_static failed!\n");
    return 0;
  }

  newf=mgtk_g_type_get_fundamental(new_type);
  oldf=mgtk_g_type_get_fundamental(parent_type);

  /* call class initializer */
  call_class_init=(APTR) info->class_init;
  if(call_class_init) {

    /* create empty class */
    class=g_new0(GtkWidgetClass,sizeof(GtkWidgetClass));

    DebOut("  calling class init 2\n");
    /*(*callme)(); * TODO, parameters..!? */
    (*call_class_init)(class); 
    DebOut("..\n");
    newf->class=class;
    DebOut("  calling class init was successful\n");

    /* the class has all signalhandlers, which are needed by
     * the widgets afterwards
     */
  }

  if(oldf) {
    newf->newparentobject=oldf->newobject;
  }
  else {
    DebOut("WARNING: no parent fundamental found!\n");
  }
  newf->newobject=(APTR) info->instance_init;
  newf->instance_size=info->instance_size;

  return new_type;
}

GType mgtk_g_type_register_fundamental(GType type_id, const gchar *type_name, GtkWidget  *(*newobject) (void)) {

  mgtk_Fundamental *newf;
  mgtk_Fundamental *i;

  DebOut("mgtkg_type_register_fundamental(%d,%s,..)\n",type_id,type_name);

  newf=g_new(mgtk_Fundamental,sizeof(mgtk_Fundamental));

  if(!mgtk->mgtk_fundamental) {
    mgtk->mgtk_fundamental=newf;
  }
  else {
    i=mgtk->mgtk_fundamental;
    while(i->next != NULL) {
      i=i->next;
    }
    i->next=newf;
  }

  newf->type_id=type_id;
  newf->type_name=g_strdup(type_name);
  newf->newobject=newobject;

  return type_id;
}

mgtk_Fundamental *mgtk_g_type_get_fundamental(GType type_id) {
  mgtk_Fundamental *i;

  DebOut("mgtk_g_type_get_fundamental(%d)\n",type_id);

  if(type_id==0) {
    DebOut("  type_id is 0 => use type_id IS_CUSTOM!\n");
    type_id=IS_CUSTOM;
  }

  if(!mgtk->mgtk_fundamental) {
    DebOut("  no registered fundamentals, returning NULL\n");
    return NULL;
  }

  i=mgtk->mgtk_fundamental;

  while((i!=NULL) && (i->type_id != type_id)) {
/*    DebOut("i->type_id: %d==%d\n",type_id,i->type_id);*/
    i=i->next;
  }

  if((i) && (i->type_id == type_id)) {
    DebOut("  found fundamental %d: %lx\n",type_id,i);
    return i;
  }

  DebOut("WARNING: fundamental type not found, returning NULL!\n");
  return NULL;
}

