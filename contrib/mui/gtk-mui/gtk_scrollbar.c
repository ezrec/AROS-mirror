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
 * $Id: gtk_scrollbar.c,v 1.18 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#ifdef __MORPHOS__
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

/******************************************************************************
 * Problem here is, that MUI only has integers as values for sliders,
 * GTK can use floats. 
 ******************************************************************************/

GtkWidget *mgtk_scale_new(GtkAdjustment *adjustment, gint vert);
GtkWidget *mgtk_scroll_new(GtkAdjustment *adjustment, gint vert);
gchar* g_strdup_printf (const gchar *format, ...);


GtkWidget *gtk_hscale_new( GtkAdjustment *adjustment ) {
  return mgtk_scale_new(adjustment,0);
}

GtkWidget *gtk_vscale_new( GtkAdjustment *adjustment ) {
  return mgtk_scale_new(adjustment,1);
}

GtkWidget *gtk_hscrollbar_new( GtkAdjustment *adjustment ) {
  return mgtk_scroll_new(adjustment,0);
}

GtkWidget *gtk_vscrollbar_new( GtkAdjustment *adjustment ) {
  return mgtk_scroll_new(adjustment,1);
}

/*******************************
 * mgtk_scale_update
 *
 * This hook updates the string 
 * of a scale widget, if there
 * is one
 * and of course sets value
 *******************************/

void mgtk_scale_update( GtkWidget *source, GtkWidget *widget) {
  Object *obj;
  int level;
  gchar *text;
  gchar *format;

  DebOut("mgtk_scale_update(%lx,%lx)\n",source,widget);

  /* set value of GtkWidget */
  level=xget(mgtk_get_mui_action_object(widget),MUIA_Numeric_Value);
  widget->value=level*widget->step_increment;

  if(widget->draw_value == 0) {
    /* nothing to draw */
    return;
  }

  /* which string is visible? also generate format string (position, decimal places)*/
  switch(widget->value_pos) {
    case GTK_POS_TOP:
      obj=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_top");
      format=g_strdup_printf("\033c%%2.%df",widget->decimalplaces);
      break;
    case GTK_POS_BOTTOM:
      obj=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_bottom");
      format=g_strdup_printf("\033c%%2.%df",widget->decimalplaces);
      break;
    case GTK_POS_RIGHT:
      obj=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_right");
      format=g_strdup_printf("\033l%%2.%df",widget->decimalplaces);
      break;
    case GTK_POS_LEFT:
      obj=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_left");
      format=g_strdup_printf("\033r%%2.%df",widget->decimalplaces);
      break;
    default:
      ErrOut("mgtk_scale_update: unknown case: %d\n",widget->value_pos);
      return;
  }

  text=g_strdup_printf(format,widget->value);

  DebOut("  Text MuiObj: %lx\n",obj);
  DebOut("  Action MuiObj: %lx\n",mgtk_get_mui_action_object(widget));
  DebOut("  Level: %ld\n",level); 
  DebOut("  text: >%s<\n",text);

  /* update text */
  set(obj,MUIA_Text_Contents,(LONG) text);

  g_free(format);
  g_free(text);
}

void mgtk_scale_update_show(GtkWidget *widget) {
  Object *top;
  Object *bottom;
  Object *left;
  Object *right;
  gint width;
  gint height;

  DebOut("mgtk_scale_update_show(%lx)\n");

  top=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_top");
  bottom=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_bottom");
  left=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_left");
  right=(Object *) g_object_get_data(G_OBJECT(widget),"mui_text_right");
  width=(gint) g_object_get_data(G_OBJECT(widget),"MUIA_FixWidth");
  height=(gint) g_object_get_data(G_OBJECT(widget),"MUIA_FixHeight");


  if(widget->draw_value == 0) {
    DoMethod(widget->MuiObject,MUIM_Group_InitChange);
    set(top,MUIA_ShowMe,FALSE);
    set(bottom,MUIA_ShowMe,FALSE);
    set(left,MUIA_ShowMe,FALSE);
    set(right,MUIA_ShowMe,FALSE);
    DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
    return;
  }

  /* seems magic, that you *have* to do it in this
   * order? seting it to true and then hiding the
   * others seems problematic, at least..?
   * Will be interesting, how zune can do that stuff ..
   */
  switch(widget->value_pos) {
    case GTK_POS_TOP:
      DoMethod(widget->MuiObject,MUIM_Group_InitChange);
      set(bottom,MUIA_ShowMe,FALSE);
      set(left,MUIA_ShowMe,FALSE);
      set(right,MUIA_ShowMe,FALSE);
      set(top,MUIA_ShowMe,TRUE);
      DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
      break;
    case GTK_POS_BOTTOM:
      DoMethod(widget->MuiObject,MUIM_Group_InitChange);
      set(top,MUIA_ShowMe,FALSE);
      set(left,MUIA_ShowMe,FALSE);
      set(right,MUIA_ShowMe,FALSE);
      set(bottom,MUIA_ShowMe,TRUE);
      DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
      break;
    case GTK_POS_LEFT:
      DoMethod(widget->MuiObject,MUIM_Group_InitChange);
      set(top,MUIA_ShowMe,FALSE);
      set(bottom,MUIA_ShowMe,FALSE);
      set(right,MUIA_ShowMe,FALSE);
      set(left,MUIA_Weight,0);
      set(left,MUIA_ShowMe,TRUE);
      DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
      break;
    case GTK_POS_RIGHT:
      DoMethod(widget->MuiObject,MUIM_Group_InitChange);
      set(top,MUIA_ShowMe,FALSE);
      set(bottom,MUIA_ShowMe,FALSE);
      set(left,MUIA_ShowMe,FALSE);
      set(right,MUIA_ShowMe,TRUE);
      set(right,MUIA_Weight,0);
      DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
      break;
    default:
      WarnOut("mgtk_scale_update_show: unknown case: %d\n",widget->value_pos);
      break;
  }
  mgtk_scale_update(widget,widget);

  /* if it had a fixed width/height, this might have been lost, so redo it */
  if(width>0) {
    DebOut("  we have a fixed width\n");
    SetAttrs(widget->MuiObject,MUIA_FixWidth,width, TAG_DONE);
  }
  if(height>0) {
    SetAttrs(widget->MuiObject,MUIA_FixHeight,height, TAG_DONE);
  }
}

/* starting text values are overwritten through the hook automatically */

GtkWidget *mgtk_scale_new(GtkAdjustment *adjustment, gint vert) {

  GtkWidget *ret;
  Object *scroll;
  Object *box;
  Object *top;
  Object *bottom;
  Object *left;
  Object *right;
  gint min;
  gint max;
  gint level;
  mgtk_signal_connect_data *i;
  
  DebOut("gtk_scale_new(%lx,%ld)\n",adjustment,vert);

  ret=mgtk_widget_new(IS_SCROLLBAR);

  if(adjustment->step_increment) {
    max=(int) (adjustment->upper / adjustment->step_increment);
    if(((int) (adjustment->upper / adjustment->step_increment) * adjustment->step_increment) != adjustment->upper) {
      /* correct rounding error */
      max++;
    }
    min=(int) adjustment->lower / adjustment->step_increment;
    DebOut("  min=%ld\n",min);
    DebOut("  max=%ld\n",max);
  }
  else {
    DebOut("WARNING: adjustment->step_increment == 0 !?\n");
    max=adjustment->upper;
    min=adjustment->lower;
  }

  level=(int) adjustment->value/adjustment->step_increment;

  box=(APTR) 
	VGroup,
          Child,
	    top=(APTR) TextObject,
	      MUIA_Text_Contents,/*MUIX_C*/"\033ctop",
	      MUIA_ShowMe,FALSE,
	    End,
	  Child,
	    HGroup,
	      Child,
		left=(APTR) TextObject,
    		    MUIA_Text_Contents,"left",
		    MUIA_Weight,0,
		End,
	      Child,
    		scroll=(APTR) SliderObject,
		    MUIA_Numeric_Min  , min,
		    MUIA_Numeric_Max  , max,
		    MUIA_Numeric_Value, level,
		    MUIA_Weight,100,
		End,
	      Child,
    		right=(APTR) TextObject,
	      	    MUIA_Text_Contents,/*MUIX_L*/"\033lright",
		    MUIA_ShowMe,FALSE,
		    MUIA_Weight,0,
    		End,
	    End,
	  Child,
	    bottom=(APTR) TextObject,
	      MUIA_Text_Contents,/*MUIX_C*/"\033cbottom",
	      MUIA_ShowMe,FALSE,
	  End,
	End;

  /* remember the text objects */
  /* maybe we should use this more often.. would make the GtkWidget struct a lot smaller..? */
  g_object_set_data(G_OBJECT(ret),"mui_text_top",(gpointer) top);
  g_object_set_data(G_OBJECT(ret),"mui_text_left",(gpointer) left);
  g_object_set_data(G_OBJECT(ret),"mui_text_right",(gpointer) right);
  g_object_set_data(G_OBJECT(ret),"mui_text_bottom",(gpointer) bottom);

  if(vert) {
    set(scroll, MUIA_Slider_Horiz , FALSE);
  }

  set(scroll,MUIA_Numeric_Format,(ULONG) ""); /* MUI can only do integers, we need floats.. :( */

  ret->MuiObject=box;
  ret->nextObject=(APTR) scroll;
  ret->value=adjustment->value;
  ret->lower=adjustment->lower;
  ret->upper=adjustment->upper;
  ret->step_increment=adjustment->step_increment;
  /*
  ret->page_increment=adjustment->page_increment;
  ret->page_size=adjustment->page_size;
  */
  /* this is a slider! */
  ret->page_increment=0;
  ret->page_size=0;
  /* defaults */
  ret->draw_value=FALSE;
  ret->value_pos=GTK_POS_LEFT;

  /* special Range properties */
  GTK_RANGE(ret)->adjustment=adjustment;
  if(vert) {
    GTK_RANGE(ret)->orientation=GTK_ORIENTATION_VERTICAL;
  }
  else {
    GTK_RANGE(ret)->orientation=GTK_ORIENTATION_HORIZONTAL;
  }
  

  /* connect our own string update hook */
  g_signal_connect (G_OBJECT (ret), "value_changed", G_CALLBACK (mgtk_scale_update), ret);
 
  if(adjustment->mgtk_signal_connect_data) {
    DebOut("  we have signals to connect remembered..\n");
    i=(APTR) adjustment->mgtk_signal_connect_data;
    while(i!=NULL) {
      /* care for data !? */
      DebOut("    i=%lx\n",i);
      DebOut("    ret=%lx\n",ret);
      DebOut("    ret->MuiObject=%lx\n",ret->MuiObject);
      g_signal_connect_data(ret,i->detailed_signal,i->c_handler,i->data,i->destroy_data,i->connect_flags);

      i=(APTR) i->next;
    }
  }

  /* remember all scales created with this adjustment */
  g_slist_append(adjustment->GSList,ret);

  return ret;
}

/*gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0); value,lower,upper,step_increment,page_increment,page_size*/
GtkWidget *mgtk_scroll_new(GtkAdjustment *adjustment, gint vert) {

  GtkWidget *ret;
  Object *scroll;
  gint min;
  gint max;
  mgtk_signal_connect_data *i;

  DebOut("gtk_hscrollbar_new(%lx)\n",adjustment);

  ret=mgtk_widget_new(IS_SCROLLBAR);

  max=(int) adjustment->upper - adjustment->page_size;
  min=(int) adjustment->lower;
  DebOut("  min=%ld\n",min);
  DebOut("  max=%ld\n",max);

/*
  level=(int) adjustment->value/adjustment->step_increment;
*/

  scroll = (APTR) MUI_NewObject( MUIC_Scrollbar,
			  MUIA_Group_Horiz, vert ? FALSE : TRUE,
			  TAG_DONE);

  set(scroll,MUIA_Prop_Entries,(ULONG) max-min);

  ret->MuiObject=scroll;
  ret->value=adjustment->value;
  ret->lower=adjustment->lower;
  ret->upper=adjustment->upper;
/*  ret->step_increment=adjustment->step_increment;*/
  /* this is a scrollbar */
  ret->step_increment=0;
  ret->page_increment=adjustment->page_increment;
  ret->page_size=adjustment->page_size;

  if(adjustment->mgtk_signal_connect_data!=NULL) {
    DebOut("  we have signals to connect remembered..\n");
    i=(APTR) adjustment->mgtk_signal_connect_data;
    while(i!=NULL) {
      /* care for data !? */
      g_signal_connect_data(ret,i->detailed_signal,i->c_handler,i->data,i->destroy_data,i->connect_flags);

      i=(APTR) i->next;
    }
  }

  /* remember all scrollbars created with this adjustment */
  g_slist_append(adjustment->GSList,ret);

  return ret;
}

GtkAdjustment *gtk_adjustment_new( gdouble value, gdouble lower, gdouble upper, gdouble step_increment, gdouble page_increment, gdouble page_size ) {

  GtkAdjustment *ret;

  DebOut("gtk_adjustment_new(%f,%f,%f,%f,%f,%f)\n",value,lower,upper,step_increment,page_increment,page_size);

  ret=mgtk_widget_new(IS_ADJUSTMENT);

  ret->lower=lower;
  ret->upper=upper;
  ret->value=value;
  ret->step_increment=step_increment;
  ret->page_increment=page_increment;
  ret->page_size=page_size;
  
  return ret;
}

void gtk_adjustment_set_value(GtkAdjustment *adjustment, gdouble value) {
  int v;
  GtkWidget *w;
  GSList *g;

  DebOut("gtk_adjustment_set_value(%lx,%f)\n",adjustment,value);

  /* an adjustment is a quite abstract object, it has no MuiObject..
   * but we have all real Objects in the GSList */

  g=(GSList *)adjustment->GSList;

  while(g) {
    w=g->data;
    w->value=value;

    if(w->step_increment) {
      /* slider */
      DebOut("  set_value on slider\n");
      v=(int) value / w->step_increment;

      /* this should call the appropiate hooks, too */
    }
    else {
      /* scrollbar */
      DebOut("  set_value on scollbar\n");
      v=(int) value;
      /*??*/
    }
    set(mgtk_get_mui_action_object(w),MUIA_Numeric_Value,v);
/*    g=g_slist_next(g);*/
    g=g->next;
  }
}

/* set the number of decimal points displayed */
void gtk_scale_set_digits(GtkScale *scale, gint digits) {

  DebOut("gtk_scale_set_digits(%lx,%ld)\n",scale,digits);

  scale->decimalplaces=digits;

  if(scale->type == IS_SCROLLBAR) {
    GTK_RANGE(scale)->round_digits=digits;
  }

  mgtk_scale_update(scale,scale);
}

GtkScale* GTK_SCALE(GtkWidget *widget) {
  return (GtkScale*) widget;
}

void gtk_scale_set_draw_value(GtkScale *scale, gboolean draw_value) {

  DebOut("gtk_scale_set_draw_value(%lx,%ld)\n",scale,draw_value);

  scale->draw_value=draw_value;
  mgtk_scale_update_show(scale);
}

void gtk_scale_set_value_pos(GtkScale *scale, GtkPositionType pos) {

  DebOut("gtk_scale_set_value_pos(%lx,%ld)\n",scale,pos);

  scale->value_pos=pos;
  mgtk_scale_update_show(scale);
}

void gtk_adjustment_changed(GtkAdjustment *adjustment) {
  DebOut("gtk_adjustment_changed(%lx)\n",adjustment);

  g_signal_emit_by_name(adjustment,"changed");
}

/********************
 * GtkRange
 ********************/

void gtk_range_set_update_policy(GtkRange *range, GtkUpdateType policy) {

  DebOut("gtk_range_set_update_policy(%lx,%ld)\n",range,policy);

  if(policy!=GTK_UPDATE_CONTINUOUS) {
    DebOut("  WARNING: gtk_range_set_update_policy: policy %ld not supported (only GTK_UPDATE_CONTINUOUS)\n");
  }

  range->update_policy=policy;

  return;
}

GtkAdjustment* gtk_range_get_adjustment(GtkRange *range) {

  DebOut("gtk_range_get_adjustment(%lx)\n",range);

  if(!range->adjustment) {
    DebOut("WARNING: range has no linked adjustment!!\n");
  }

  return range->adjustment;
}

/* not tested: */
void gtk_range_set_adjustment(GtkRange *range, GtkAdjustment *adjustment) {

  DebOut("gtk_range_set_adjustment(%lx,%lx)\n",range,adjustment);
  DebOut("WARNING: this function has not beed tested so far!\n");


  if(range->adjustment == adjustment) {
    DebOut("  range had already this adjustment\n");
    return;
  }

  if(range->adjustment) {
    DebOut("  range had already an adjustment\n");
    g_object_unref(G_OBJECT(range->adjustment));
  }

  range->adjustment=adjustment;
  g_object_ref(G_OBJECT(adjustment));
  range->round_digits=adjustment->digits;

  /* TODO: set orientation etc.. */

}
