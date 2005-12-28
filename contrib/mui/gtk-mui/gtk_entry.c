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
 * $Id: gtk_entry.c,v 1.12 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

/* cross platform hooks */
#include <SDI_hook.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

/* set textlenght here */
GtkEntry *GTK_ENTRY(GtkWidget *widget) {

  widget->text_length=strlen(gtk_entry_get_text(widget));

  return (GtkEntry *) widget;
}

GtkTable *GTK_EDITABLE(GtkWidget *widget) {
  return (GtkTable *) widget;
}

GtkWidget *gtk_entry_new() {
  GtkWidget *ret = NULL;
  APTR entry;

  DebOut("gtk_entry_new()\n");

  ret=mgtk_widget_new(IS_ENTRY);

//  entry = (APTR) MUI_MakeObject(MUIO_String, 0, 256);
  entry=NewObject(CL_Entry->mcc_Class, NULL,MA_Widget,ret,MUIA_String_MaxLen,(LONG) 256,TAG_DONE);

  DebOut("ret=%lx\n",ret);
  DebOut("entry=%lx\n",entry);


  if (entry) {
    set(entry, MUIA_CycleChain, 1);

    if (ret) {
      ret->MuiObject = entry;
    }
  }
  else {
    ErrOut("Unable to get new entry object\n");
  }

  return ret;
}

const gchar *gtk_entry_get_text( GtkEntry *entry ) {
  const char *text;

  DebOut("gtk_entry_get_text(%lx)\n",entry);
	DebOut("  MuiObject: %lx\n",entry->MuiObject);

/*  SetAttrs(entry->MuiObject,MUIA_String_Contents,(LONG) text, TAG_DONE);*/
  get( entry->MuiObject, MUIA_String_Contents, &text );

  DebOut("Text is: %s\n",text);

  return text;
}

void gtk_entry_set_max_length (GtkEntry *entry, gint length) {

  DebOut("gtk_entry_set_max_length(entry %lx, length %ld)\n",entry,length);

  SetAttrs(entry->MuiObject,MUIA_String_MaxLen,(LONG) length, TAG_DONE);

  return;
}

void gtk_entry_set_text (GtkEntry *entry, const gchar *text) {

  DebOut("gtk_entry_set_text(entry %lx, text: %s)\n",entry,text);

  SetAttrs(entry->MuiObject,MUIA_String_Contents,(LONG) text, TAG_DONE);

  return;
}

/*gtk_editable_insert_text (GTK_EDITABLE (entry), " world", -1, &tmp_pos);*/

void gtk_editable_insert_text (GtkEditable *editable, const gchar *in_text, gint new_text_length, gint *position) {
  const char *oldtext;
  char *newtext;
  int newlen;

  DebOut("gtk_editable_insert_text (GtkEditable %lx, in_text: %s, new_text_length %ld, position: %ld)\n",editable,in_text,new_text_length,*position);

  oldtext=gtk_entry_get_text(editable);

  DebOut("oldtext: %s\n",oldtext);

  if(new_text_length == -1) {
    new_text_length=strlen(in_text);
  }

  newlen=strlen(oldtext)+new_text_length;

  DebOut("new_text_length: %ld\n",newlen);
  DebOut("newlen:          %ld\n",newlen);

  newtext = mgtk_allocvec(newlen+1, MEMF_ANY);

  if (newtext)
  {
    /* Btw in MUI4 there is a method to insert text directly
    */

    strncpy(newtext, oldtext, *position);
    strncpy(newtext + *position, in_text, new_text_length);
    strcpy(newtext + *position + new_text_length, oldtext + *position);

    *position = *position+new_text_length;

    gtk_entry_set_text(editable, newtext);
    mgtk_freevec(newtext);
  }

  DebOut("gtk_editable_insert_text: exit\n");
}

void gtk_editable_select_region(GtkEditable *editable, gint start, gint end) {

  WarnOut("WARNING: gtk_editable_select_region(): requires MUI4 (%d/%d)\n", start, end);

#if 0
  // according to GTK documentation this is not required
  set(editable->MuiObject, MUIA_String_BufferPos, start);
#endif

  /* Requires MUI4
  **
  ** VaporWare donated TextInput.mcc source code to Stefan Stuntz and it was
  ** integrated to String.mui class.
  */

  set(editable->MuiObject, MUIA_Textinput_MarkStart, start);
  set(editable->MuiObject, MUIA_Textinput_MarkEnd, end < 0 ? end : start + end);

  return;
}

gchar* gtk_editable_get_chars(GtkEditable *editable, gint start_pos, gint end_pos) {
  const gchar *gtext;
  gchar *ret;
  int size;

  DebOut("gtk_editable_get_chars(%lx,%ld,%ld=\n",editable,start_pos,end_pos);

  gtext=gtk_entry_get_text(editable);

  if(start_pos>strlen(gtext)) {
    WarnOut("gtk_editable_get_chars: illegal start_pos\n");
    start_pos=0;
  }

  if(end_pos==-1) {
    end_pos=strlen(gtext);
  }

  size=end_pos-start_pos+1;

  ret=g_strndup(gtext+start_pos,end_pos-start_pos);

  DebOut(" return: >%s<\n",ret);
  return ret;
}

void gtk_editable_set_editable(GtkEditable *editable, gboolean is_editable) {
  LONG disable;

  DebOut("gtk_editable_set_editable(%lx,%ld)\n",editable,is_editable);

  if(is_editable) {
    disable=0;
  }
  else {
    disable=1;
  }


  if(editable->nextObject !=NULL) {
    set(editable->nextObject,MUIA_Disabled,disable); /* there is a label around */
  }
  else {
    set(editable->MuiObject,MUIA_Disabled,disable); /* there is a label around */
  }
  return;
}

/* this does not hide the entry gadget, but sets secret mode 
 * this seems not to work under AROS
 */
void gtk_entry_set_visibility(GtkEditable *editable, gboolean is_visible) {
  ULONG secret;

  DebOut("gtk_entry_set_visibility(%lx,%ld)\n",editable,is_visible);

  if(is_visible) {
    secret=(ULONG) FALSE;
  }
  else {
    secret=(ULONG) TRUE;
  }

  set(mgtk_get_mui_action_object(editable),MUIA_String_Secret,secret); 

  gtk_entry_set_text(editable,gtk_entry_get_text(editable));

  return;
}

