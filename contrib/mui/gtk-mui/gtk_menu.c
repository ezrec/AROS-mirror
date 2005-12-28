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
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

/* cross platform hooks */
#ifndef __AROS__
  #define MGTK_HOOK_OBJECT_POINTER Object *
  #define MGTK_HOOK_APTR APTR
  #define MGTK_USERFUNC_INIT
  #define MGTK_USERFUNC_EXIT
  #include <SDI_hook.h>
#else
  #include "mgtk_aros.h"
#endif

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "classes/classes.h"

GtkWidget *gtk_menu_new( void ) {
  GtkWidget *ret;

  /* gtk_menu_new creates a IS_MENU Object, which is *not* displayed in MUI
   * it just contains IS_MENUITEM Objects, which are visible 
   */

  ret=mgtk_widget_new(IS_MENU);
  
  return ret;
}

#if 0
HOOKPROTO(MUIHookFunc_menu,ULONG,MGTK_HOOK_OBJECT_POINTER obj,MGTK_HOOK_APTR bla) {
  MGTK_USERFUNC_INIT

  GtkWidget *widget;

  DebOut("MUIHookFunc_menu called for obj %lx (%lx)\n",obj,bla);

  widget=(GtkWidget *) xget(obj,MA_Widget);

  if(!widget) {
    DebOut("ERROR: widget for obj %lx not found!\n",obj);
  }
  else {
    g_signal_emit_by_name(widget,"activate");
  }
//  call_gtk_hooks_activated(obj);
  return(0);
  MGTK_USERFUNC_EXIT
}

MakeHook(MyMuiHook_menu, MUIHookFunc_menu);
#endif

GtkWidget *gtk_menu_item_new_with_label( const char *label ) {
  APTR menu;
  GtkWidget *ret;
  char *s;

  DebOut("gtk_menu_item_new_with_label(%s)\n",label);

  ret=mgtk_widget_new(IS_MENUITEM);

  s=g_strdup(label);

//  menu = (APTR) MUI_MakeObject(MUIO_Menuitem,s ,0,0,0);
  menu = (APTR) NewObject(CL_Menu->mcc_Class, NULL,MA_Widget,ret,MUIA_Menuitem_Title,s,TAG_DONE);

 // DoMethod(menu,MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, menu, 2, MUIM_CallHook, &MyMuiHook_menu);

  ret->MuiObject=menu;
  ret->title=s;
 
  return ret;
}

/* not sure, if this is usefull ..*/
GtkWidget *gtk_menu_item_new(void) {

  return gtk_menu_item_new_with_label("");
}

GtkWidget *gtk_menu_bar_new (void) {
  GtkWidget *ret;

  ret=mgtk_widget_new(IS_MENUBAR);
  
  return ret;
}

void gtk_menu_shell_append (GtkMenuShell *menu_shell, GtkWidget *child) {
  GtkWidget *i;

  DebOut("gtk_menu_shell_append (GtkMenuShell %lx,GtkWidget %lx)\n",menu_shell,child);

  DebOut("  gtk_menu_shell_append: menu_shell->type: %lx\n",menu_shell->type);
  DebOut("  gtk_menu_shell_append: child->type: %lx\n",child->type);

  if(child->type == IS_MENU) {
    /* as a IS_MENU is invisible, we add it's children */
    DebOut("  gtk_menu_shell_append: adding children of IS_MENU\n");
    i=(GtkWidget *) child->nextObject;
    while(i!=NULL) {
      DoMethod(menu_shell->MuiObject,MUIM_Family_AddTail,i->MuiObject);
      i=(GtkWidget *) i->nextObject;
    }
    return;
  }

  if(menu_shell->type == IS_MENUBAR) {
    /* level 1 (in screen bar) */

    DebOut("  gtk_menu_shell_append: target is MENUBAR\n");
    DebOut("  mgtk->MuiRootStrip: %lx\n",mgtk->MuiRootStrip);
    DebOut("  child->MuiObject: %lx\n",child->MuiObject);
    DebOut("  menu_shell->MuiObject: %lx\n",menu_shell->MuiObject);
    DebOut("  menu_shell->MuiWinMenustrip: %lx\n",menu_shell->MuiWinMenustrip);
  
    /* attach menu */
    DoMethod(menu_shell->MuiWinMenustrip,MUIM_Family_AddHead,child->MuiObject);
    return;
  }

  if((menu_shell->type == IS_MENU) && (child->type == IS_MENUITEM)) {
    /* as menu_shell- is invisible, it does not have a MUI Object..*/
    DebOut("  gtk_menu_shell_append: remember this child in menu_shell\n");
    i=menu_shell;
    while(i->nextObject != NULL) {
      i=(GtkWidget *) i->nextObject;
    }
    i->nextObject=(APTR) child;
    return;
  }

  if((menu_shell->type == IS_MENU) && (child->type == IS_MENU)) {
    /* add to level 1 */
    DebOut("  IS_MENU: menu_shell->MuiObject: %lx\n",menu_shell->MuiObject);
    DebOut("  IS_MENU: child->MuiObject: %lx\n",child->MuiObject);
    DoMethod(menu_shell->MuiObject,MUIM_Family_AddTail,child->MuiObject);
    return;
  }

  WarnOut("gtk_menu_shell_append with non strange objects (%d,%d) ignored !?\n",menu_shell->type,child->type);
  return;
}

void gtk_menu_item_set_submenu(GtkMenuItem *menu_item, GtkWidget *submenu) {
  GtkWidget *i;

  DebOut("gtk_menu_item_set_submenu(GtkMenuItem %lx, GtkWidget %lx)\n",menu_item,submenu);

  if(menu_item->type != IS_MENUITEM) {
    WarnOut("gtk_menu_item_set_submenu with non menuitem!?\n");
  }

  DebOut("gtk_menu_item_set_submenu: menu_item->type: %lx\n",menu_item->type);
  DebOut("gtk_menu_item_set_submenu: submenu->type: %lx\n",submenu->type);

  if(submenu->type == IS_MENU) {
    /* as a IS_MENU is invisible, we add it's children */
    DebOut("gtk_menu_item_set_submenu: adding children of IS_MENU\n");
    i=(GtkWidget *) submenu->nextObject;
    while(i!=NULL) {
      DebOut("gtk_menu_item_set_submenu: add %lx\n",i);
      
      DoMethod(menu_item->MuiObject,MUIM_Family_AddTail,i->MuiObject);
      i=(APTR) i->nextObject;
    }
    return;
  }

  /* attach submenu */
  DoMethod(menu_item->MuiObject,MUIM_Family_AddTail,submenu->MuiObject);

  return;
}

GtkWidget *gtk_menu_get_attach_widget(GtkMenu *menu) {

  DebOut("gtk_menu_get_attach_widget(%lx)\n",menu);

  if(menu->parent==NULL) {
    WarnOut("gtk_menu_get_attach_widget: nobody set parent for menu %x\n",(int) menu);
  }

  return((GtkWidget *)menu->parent);
}

/****************************************************************************************
 * option_menu
 *
 * gtk_option_menu_set_menu is deprecated and should not be used in newly-written code. 
 *
 * an option menu is the MUI equivalent of MUIO_Cycle, just with a better API ;)
 *
 ****************************************************************************************/
GtkWidget *gtk_option_menu_new ( void ) {
  GtkWidget *ret;

  /* gtk_option_menu_new () creates a IS_OPTIONMENU Object, which is *not* displayed in MUI
   * it just contains IS_MENUITEM Objects, which are visible 
   */

  ret=mgtk_widget_new(IS_OPTIONMENU);
  
  return ret;
}

void gtk_option_menu_set_menu(GtkOptionMenu *option_menu, GtkWidget *menu) {

  DebOut("gtk_option_menu_set_menu(%lx,%lx)\n",option_menu,menu);

  option_menu->nextObject=(APTR) menu;

  return;
}

GtkOptionMenu *GTK_OPTION_MENU(GtkWidget *widget) {

  return((GtkOptionMenu *) widget);
}

/* An MUI Cycle Menu is only one object. For GTK we have
 * a single object per item. So we have one MUI callback,
 * which has to call the GTK objects correct callback.
 *
 * With the help of this not really trivial stuff, you
 * can add callbacks in GTK on option menus at
 * any time (before and after you really show
 * the option menu.
 */
void mgtk_option_menu_hook( GtkWidget *source, GtkWidget *widget) {
  GtkWidget *i;
  GtkWidget *item;
  int num;
  int c;
  void (*callme)(GtkWidget *widget,gpointer data);
  mgtk_signal_connect_data *callback;

  DebOut("mgtk_option_menu_hook(%lx,%lx)\n");

  /* which one is active? */
  num=xget(widget->MuiObject,MUIA_Cycle_Active);

  DebOut("  num=%ld\n",num);

  i=(APTR) widget->nextObject;

  if(!i) {
    WarnOut("mgtk_option_menu_hook: widget->nextObject is NULL !?\n");
    return;
  }

  /* get correct menu item */
  c=0;
  item=NULL;
  while(i!=NULL) {
    if(i->type==IS_MENUITEM) {
      if(c==num) {
	item=i;
	break;
      }
      c++;
    }
    i=(APTR) i->nextObject;
  }

  if(!item) {
    WarnOut("mgtk_option_menu_hook: no matching item found!\n");
    return;
  }

  /* so at least we know, what was selected ;) */
  DebOut("  found matching item %lx (number %ld)\n",item,num);

  /* now call all callbacks of this item */
  if(item->mgtk_signal_connect_data) {
    DebOut("  found remember structure\n");
    callback=(mgtk_signal_connect_data *) item->mgtk_signal_connect_data;
    /* call all remembered functions */
    while(callback) {
      callme=(void *) callback->c_handler;
      DebOut("  calling callback %lx\n",callme);
      if(callback->connect_flags == G_CONNECT_SWAPPED) {
	(*callme)(callback->data,item);
      }
      else {
	(*callme)(item,callback->data);
      }
      callback=(mgtk_signal_connect_data *)callback->next;
    }
  }
  /* we did it, phew. */
}

/* mgtk_optionmenu_pack assumes, that the menu has only
 * one level, so no submenus. This is according to the
 * GTK recommendation, but maybe not perfect..
 */

void mgtk_optionmenu_pack(GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding ) {
  Object *menu;
  GtkWidget *i;
  GtkWidget *item;
  char **menuitems;
  int count;

  DebOut("mgtk_optionmenu_pack(%lx,%lx,%ld,%ld,%ld)\n");

  /* even if the menu has quite some MUI Objects, we can't use them here
   * MUI is far from beeing orthogonal :(
   */
  i=(APTR) child->nextObject;

  if(!i) {
    WarnOut("child->nextObject is NULL !?\n");
    return;
  }

  /* get menu items */
  while(i->type!=IS_MENUITEM) {
    i=(APTR) i->nextObject;
    if(!i) {
      WarnOut("nextObject is NULL, but no MENUITEM found before !?\n");
      return;
    }
  }
  item=i;
  /* now item is the first menuitem */

  /* count the items */
  count=0;
  while(i!=NULL) {
    DebOut("  menu item: %s\n",i->title);
    count++;
    i=(APTR) i->nextObject;
  }

  /* generate MUI parameter */
  menuitems=g_new(char *,count+2);

  i=item;
  count=0;
  while(i!=NULL) {
    menuitems[count]=i->title;
    count++;
    i=(APTR) i->nextObject;
  }
  menuitems[count]=NULL;

  menu=(Object *) MUI_MakeObject(MUIO_Cycle,NULL,menuitems);

  child->MuiObject=menu;

  DoMethod(box->MuiObject,MUIM_Group_InitChange);
  DoMethod(box->MuiObject,OM_ADDMEMBER,child->MuiObject);
  DoMethod(box->MuiObject,MUIM_Group_ExitChange);

  mgtk_add_child(box,child);

  /* connect a hook to the menu */
  g_signal_connect (child, "activate", G_CALLBACK (mgtk_option_menu_hook), child);

  return;
}
