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

GtkWidget *gtk_toolbar_new(void) {

  GtkWidget *ret;
  Object *tb;

  DebOut("gtk_toolbar_new()\n");

  ret=mgtk_widget_new(IS_TOOLBAR);

  tb = (APTR) NewObject(CL_Toolbar->mcc_Class, NULL,MA_Widget,ret,MUIA_Group_Horiz,TRUE,MUIA_Group_Spacing,0,TAG_DONE);

  DebOut(" NewObject=%lx\n",tb);

  ret->MuiObject=tb;
  ((GtkToolbar *)ret)->space_size=12;  /* default value */
  ((GtkToolbar *)ret)->style=GTK_TOOLBAR_BOTH;  /* default value */


  return ret;
}

GtkWidget *gtk_toolbar_append_item(GtkToolbar *toolbar, const char *text, const char *tooltip_text, const char *tooltip_private_text, GtkWidget *icon, GtkSignalFunc callback, gpointer user_data) {

  GtkWidget *button;
  GtkWidget *box;
  GtkWidget *label;
  char *t;

  DebOut("gtk_toolbar_append_item(%lx,%s,%s,%s,%lx,%lx,%lx)\n",toolbar,text,tooltip_text,tooltip_private_text,icon,callback,user_data);

  button=gtk_button_new ();

  if(callback) {
    g_signal_connect (G_OBJECT (button), "clicked",
              G_CALLBACK (callback), user_data);
  }

  box=gtk_vbox_new (FALSE, 0);
//  gtk_container_set_border_width (GTK_CONTAINER (box), 2);
  gtk_box_pack_start (GTK_BOX (box), icon, FALSE, FALSE, 2);
  gtk_widget_show (box);

  if(text) {
    t=g_strdup_printf("%s%s",MUIX_C,text);
    label=gtk_label_new(t);
    gtk_container_add (GTK_CONTAINER (box), label);
    gtk_widget_show (label);
    button->title=t;
  }
  gtk_container_add (GTK_CONTAINER (button), box);

  gtk_container_add (GTK_CONTAINER ((GtkWidget *)toolbar),button);

  /* keep a list of all children of the toolbar */
  toolbar->mgtk_children=g_slist_append(toolbar->mgtk_children,button);
  g_object_set_data(G_OBJECT(button),"mgtk_image_widget",(gpointer) icon);
  g_object_set_data(G_OBJECT(button),"mgtk_text_widget",(gpointer) label);
  g_object_set_data(G_OBJECT(button),"mgtk_parent_object",(gpointer) box);

  if(tooltip_text) {
    if(! toolbar->tooltips) {
      toolbar->tooltips=gtk_tooltips_new ();
    }
    gtk_tooltips_set_tip (toolbar->tooltips, button, tooltip_text,tooltip_private_text);
  }

  return button;

}

static GtkWidget *mgtk_toolbar_append_toggle(GtkToolbar *toolbar, const char *text, const char *tooltip_text, const char *tooltip_private_text, GtkWidget *icon, GtkSignalFunc callback, gpointer user_data) {

  GtkWidget *button;
  GtkWidget *box;
  GtkWidget *label;
  Object *image;
  char *t;

  DebOut("gtk_toolbar_append_toggle(%lx,%s,%s,%s,%lx,%lx,%lx)\n",toolbar,text,tooltip_text,tooltip_private_text,icon,callback,user_data);

  button=gtk_toggle_button_new();

  image=ImageObject,
          MUIA_Image_Spec , icon->MuiObject,
          MUIA_Image_FreeVert , TRUE,
          MUIA_Image_FreeHoriz , TRUE,
        End;

  DoMethod(button->MuiObject,MUIM_Group_InitChange);
  DoMethod(button->MuiObject,OM_ADDMEMBER,image);
  DoMethod(button->MuiObject,MUIM_Group_ExitChange);

  if(callback) {
    g_signal_connect (G_OBJECT (button), "toggled",
              G_CALLBACK (callback), user_data);
  }

  box=gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (box), 2);
  gtk_box_pack_start (GTK_BOX (box), icon, FALSE, FALSE, 3);
  gtk_widget_show (box);

  if(text) {
    t=g_strdup_printf("%s%s",MUIX_C,text);
    label=gtk_label_new(t);
    gtk_container_add (GTK_CONTAINER (box), label);
    gtk_widget_show (label);
    button->title=t;
  }
  gtk_container_add (GTK_CONTAINER (button), box);

  gtk_container_add (GTK_CONTAINER ((GtkWidget *)toolbar),button);

  /* keep a list of all children of the toolbar */
  toolbar->mgtk_children=g_slist_append(toolbar->mgtk_children,button);
  g_object_set_data(G_OBJECT(button),"mgtk_image_widget",(gpointer) icon);
  g_object_set_data(G_OBJECT(button),"mgtk_text_widget",(gpointer) label);
  g_object_set_data(G_OBJECT(button),"mgtk_parent_object",(gpointer) box);

  if(tooltip_text) {
    if(! toolbar->tooltips) {
      toolbar->tooltips=gtk_tooltips_new ();
    }
    gtk_tooltips_set_tip (toolbar->tooltips, button, tooltip_text,tooltip_private_text);
  }

  return button;
}

void mgtk_update_group(GtkWidget *widget, GSList *group);

static GtkWidget *mgtk_toolbar_append_radio(GtkToolbar *toolbar, GtkWidget *widget, const char *text, const char *tooltip_text, const char *tooltip_private_text, GtkWidget *icon, GtkSignalFunc callback, gpointer user_data) {

  GtkWidget *toggle;
  
  toggle=mgtk_toolbar_append_toggle(toolbar, text, tooltip_text, tooltip_private_text, icon, callback, user_data);

  if(widget && widget->GSList) {
    /* get radio group from widget*/
    toggle->GSList=widget->GSList;
  }
  else {
    /* new radio group */
    DebOut("  first element in this radio group.\n");
  }
  toggle->GSList=g_slist_append(toggle->GSList,toggle);
  mgtk_update_group(toggle,toggle->GSList);

  return toggle;
}

GtkWidget *gtk_toolbar_append_element(GtkToolbar *toolbar, GtkToolbarChildType type, GtkWidget *widget, const char *text, const char *tooltip_text, const char *tooltip_private_text, GtkWidget *icon, GtkSignalFunc callback, gpointer user_data) {


  DebOut("gtk_toolbar_append_element(%lx,%d,%lx,%s,%s,%s,%lx,%lx,%lx)\n",toolbar,type,widget,text,tooltip_text,tooltip_private_text,icon,callback,user_data);

  if(type == GTK_TOOLBAR_CHILD_TOGGLEBUTTON) {
    return mgtk_toolbar_append_toggle(toolbar, text, tooltip_text, tooltip_private_text, icon, callback, user_data);
  }
  else if(type == GTK_TOOLBAR_CHILD_BUTTON) {
    return gtk_toolbar_append_item(toolbar, text, tooltip_text, tooltip_private_text, icon, callback, user_data);
  }
  else if(type == GTK_TOOLBAR_CHILD_RADIOBUTTON ){
    return mgtk_toolbar_append_radio(toolbar, widget, text, tooltip_text, tooltip_private_text, icon, callback, user_data);
  }

  DebOut("WARNING: other types : %d NOT DONE YET\n",type); /*TODO*/
  return gtk_toolbar_append_item(toolbar, text, tooltip_text, tooltip_private_text, icon, callback, user_data);

}

void gtk_toolbar_append_widget(GtkToolbar *toolbar, GtkWidget *widget, const char *tooltip_text, const char *tooltip_private_text) {
  DebOut("gtk_toolbar_append_widget(%lx,%lx,%s,%s)\n",toolbar,widget,tooltip_text,tooltip_private_text);

  gtk_container_add (GTK_CONTAINER ((GtkWidget *)toolbar),widget);

  /* keep a list of all children of the toolbar */
  toolbar->mgtk_children=g_slist_append(toolbar->mgtk_children,widget);

  if(tooltip_text) {
    if(! toolbar->tooltips) {
      toolbar->tooltips=gtk_tooltips_new ();
    }
    gtk_tooltips_set_tip (toolbar->tooltips, widget, tooltip_text,tooltip_private_text);
  }

}

void gtk_toolbar_set_tooltips(GtkToolbar *toolbar, gboolean enable) {
  DebOut("gtk_toolbar_set_tooltips(%lx,%d)\n",toolbar,enable);

  if(toolbar->tooltips) {
    if(enable) {
      gtk_tooltips_enable(toolbar->tooltips);
    }
    else {
      gtk_tooltips_disable(toolbar->tooltips);
    }
  }
}

void gtk_toolbar_append_space(GtkToolbar *toolbar) {

  GtkWidget *space;

  DebOut("gtk_toolbar_append_space(%lx)\n",toolbar);

  space=mgtk_widget_new(IS_SPACE);

  space->MuiObject=HSpace(toolbar->space_size);

  gtk_toolbar_append_widget(toolbar,space,NULL,NULL);
}

/************* STYLE *****************/

void mgtk_toolitem_restyle(GtkToolbar *toolbar,GtkWidget *widget,GtkToolbarStyle style) {
  GtkWidget *label;
  GtkWidget *image;
  Object *box;
  DebOut("mgtk_toolitem_restyle(%ld,%d)\n",widget,style);

  if((widget->type==IS_TOGGLEBUTTON)||(widget->type==IS_BUTTON)) {
    DebOut("  (Toggle)Button: %ld\n",widget);

    image=(GtkWidget *)g_object_get_data(G_OBJECT(widget),"mgtk_image_widget");
    label=(GtkWidget *)g_object_get_data(G_OBJECT(widget),"mgtk_text_widget");

    box=widget->MuiObject;

    DebOut("  mgtk_image_widget=%lx\n",image);
    DebOut("  mgtk_text_widget=%lx\n",label);
    DebOut("  mgtk_parent_object=%lx\n",box);

    DoMethod(widget->MuiObject,MUIM_Group_InitChange);

    if(style==GTK_TOOLBAR_ICONS) {
      DebOut("  set to GTK_TOOLBAR_ICONS\n");
      DoMethod(box,OM_REMMEMBER,label->MuiObject);
      if(toolbar->style==GTK_TOOLBAR_TEXT) {
        /* old style was text only */
        DoMethod(box,OM_ADDMEMBER,image->MuiObject);
      }
    }
    else if(style==GTK_TOOLBAR_TEXT) {
      DebOut("  set to GTK_TOOLBAR_TEXT\n");
      DoMethod(box,OM_REMMEMBER,image->MuiObject);
      if(toolbar->style==GTK_TOOLBAR_ICONS) {
        /* old style was text only */
        DoMethod(box,OM_ADDMEMBER,label->MuiObject);
      }
    }
    else if((style==GTK_TOOLBAR_BOTH)||(style==GTK_TOOLBAR_BOTH_HORIZ))  {
      DebOut("  set to GTK_TOOLBAR_BOTH\n");
      if(toolbar->style==GTK_TOOLBAR_TEXT) {
        /* old style was text only */
        DoMethod(box,OM_REMMEMBER,label->MuiObject);
        DoMethod(box,OM_ADDMEMBER,image->MuiObject);
      }
      DoMethod(box,OM_ADDMEMBER,label->MuiObject);
    }

    DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
  }
}

void gtk_toolbar_set_style(GtkToolbar *toolbar, GtkToolbarStyle style) {

  GSList *children;

  DebOut("gtk_toolbar_set_style(%lx,%d)\n",toolbar,style);
  DebOut("  mgtk type: %lx\n",((GtkWidget *)toolbar)->type);
  DebOut("  old style: %lx\n",toolbar->style);

  if(style == toolbar->style) {
    DebOut("  nothing to do (keep old style)\n");
    return;
  }

  children=toolbar->mgtk_children;

  while(children) {
    mgtk_toolitem_restyle(toolbar,(GtkWidget *)children->data,style);
    children=g_slist_next(children);
  }

  DoMethod(mgtk->Group,MUIM_Group_InitChange);
  DoMethod(mgtk->Group,MUIM_Group_ExitChange);

  toolbar->style=style;
}
