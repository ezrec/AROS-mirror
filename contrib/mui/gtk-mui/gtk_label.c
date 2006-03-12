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
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>
#ifdef __MORPHOS__
  #include <proto/alib.h>
#endif

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"


GtkLabel *GTK_LABEL(GtkWidget *widget) {
  return (GtkLabel *) widget;
}

GtkWidget *gtk_label_new( const char *str ) {

  GtkWidget *ret;
  Object *label;
  char *s;

  DebOut("gtk_label_new(%s)\n",str);

  ret = mgtk_widget_new(IS_LABEL);

  if(str) {
    s=g_strdup_printf("%s%s",MUIX_L,str);
    label = (APTR) MUI_MakeObject(MUIO_Label, s,MUIO_Label_Centered,0);
    ret->title=s;
  }
  else {
    label = (APTR) MUI_MakeObject(MUIO_Label, "",MUIO_Label_Centered,0);
    ret->title=(char *) NULL;
  }

  ret->MuiObject = label;

  return ret;
}

/*******************
 * as far as I see, changing the text of a text label is
 * not possible in MUI!?
 * ok, the we delete the label and create a new one ;)
 *******************/

void gtk_label_set_text(GtkLabel *label, const char *str) {
  Object *parent;
  Object *old;
  Object *new;
  Object *o;
  struct List *list;
  struct Node *state;
  int i;
  ULONG *msg;

  DebOut("gtk_label_set_text(%x,%s)\n",(int) label,str);

  if(!strcmp(str,label->title)) {
    DebOut("  same strings, nothing to do\n");
    return;
  }

  /* delete old string.. */
  if(label->title) {
    g_free(label->title);
    label->title=(char *) NULL;
  }

  if(str) {
    label->title=g_strdup(str);
  }
  else {
    label->title=g_strdup("");
  }

  old=label->MuiObject;
  new = (APTR) MUI_MakeObject(MUIO_Label, label->title ,MUIO_Label_Centered, 0);
  label->MuiObject=new;

  parent=(Object *) xget(old,MUIA_Parent);
  DebOut(" parent: %x\n",parent);

  if(parent) {
    list=(struct List *) xget(parent,MUIA_Group_ChildList);
    DebOut(" list: %x\n",list);

    state = list->lh_Head;
    i=0;
    DebOut("  old object: %x\n",old);
    while ( (o = NextObject( &state )) ) {
      DebOut("  old childs: %x\n",o);
      i++;
    }

    /* thanks, Georg ;) */
    msg=(APTR) g_new(char,sizeof(struct MUIP_Group_Sort) + sizeof(Object *) * (i+1));

    /* create new sort order */
    msg[0]=MUIM_Group_Sort;

    state = list->lh_Head;
    i=1;
    DebOut("  new object: %x\n",new);
    while ( (o = NextObject( &state )) ) {
      if(o!=old) {
        msg[i]=(ULONG) o;
      }
      else {
        msg[i]=(ULONG) new;
      }
      DebOut("  new childs: %x\n",msg[i]);
      i++;
    }
    msg[i]=0;

    DebOut("  remove/add/sort childs\n");

    DoMethod(parent,MUIM_Group_InitChange);
    DoMethod(parent,OM_REMMEMBER,old);
    DoMethod(parent,OM_ADDMEMBER,new);
    DoMethodA(parent, msg);
    DoMethod(parent,MUIM_Group_ExitChange);

    g_free(msg);
  }
  DisposeObject(old);

  return;
}

