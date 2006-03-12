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
#include <proto/gadtools.h>
#ifdef __AROS__
#include <dos/dos.h>
#endif
#include <mui.h>
/* cross platform hooks */
#include <SDI_hook.h>

#include "classes.h"
#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"
#include "gtk/gtkrange.h"
#include "mui.h"

/* stolen from YAM */
ULONG xget(Object *obj, ULONG attr) { 
  ULONG b = 0;
  GetAttr(attr, obj, &b);
  return b;
}

/**********************************************************************
  mgtk_allocvec, mgtk_freevec, mgtk_allocmem, mgtk_freemem
**********************************************************************/

APTR mgtk_allocvec(ULONG size, ULONG req)
{
  APTR ptr;
#ifdef __MORPHOS__
  ptr = AllocVecTaskPooled(size);

  if (ptr && (req & MEMF_CLEAR))
  {
    memset(ptr, 0, size);
  }
#else
  ptr = AllocVec(size, req);
#endif

	return ptr;
}

VOID mgtk_freevec(APTR ptr)
{
#ifdef __MORPHOS__
  FreeVecTaskPooled(ptr);
#else
  FreeVec(ptr);
#endif
}

APTR mgtk_allocmem(ULONG size, ULONG req)
{
  APTR ptr;
#ifdef __MORPHOS__
  ptr = AllocTaskPooled(size);

  if (ptr && (req & MEMF_CLEAR))
  {
    memset(ptr, 0, size);
  }
#else
  ptr = AllocMem(size, req);
#endif

	return ptr;
}

VOID mgtk_freemem(APTR ptr, ULONG size)
{
#ifdef __MORPHOS__
  FreeTaskPooled(ptr, size);
#else
  FreeMem(ptr, size);
#endif
}

/*********************************
 * private functions
 *********************************/

APTR mgtk_malloc(int size) {
  APTR ret;

  DebOut("mgtk_malloc(%d)\n",size);

  if (!(ret = AllocRemember(&rememberKey, size, MEMF_CLEAR ))) {
    printf("mgtk_malloc: Unable to allocate %d bytes of memory\n",size);
    return(NULL);
  }

  return(ret);
}
 
GtkWidget *mgtk_widget_new(int type) {

  GtkWidget *ret;

  switch(type) {
    case IS_SCROLLBAR:
      ret=(GtkWidget *) g_new0(GtkRange,1);
      break;
    case IS_TOOLBAR:
      ret=(GtkWidget *) g_new0(GtkToolbar,1);
      break;
    default:
      ret=g_new0(GtkWidget,1);
      break;
  }
 
  /* init data structure */
  G_OBJECT(ret)->badmagic=BADMAGIC;
  G_OBJECT(ret)->gobjs=NULL;
  G_OBJECT(ret)->ref_count=0;
  ret->type=type;
  ret->MuiWin=NULL;
  ret->MuiWinMenustrip=NULL;
  ret->MuiGroup=NULL;
  ret->MuiObject=NULL;
  ret->title=NULL;
  ret->rows=0;
  ret->columns=0;
  ret->text_length=0;
  ret->parent=NULL;
  ret->nextObject=NULL;
  ret->GSList=NULL;
  ret->value=0;
  ret->lower=0;
  ret->upper=0;
  ret->step_increment=0;
  ret->page_increment=0;
  ret->page_size=0;
  ret->mgtk_signal_connect_data=NULL;
  ret->style=mgtk_get_default_style(mgtk->MuiRoot);

  switch (type)
  {
    default           : ret->mainclass = CL_AREA; break;
    case IS_WINDOW    : ret->mainclass = CL_WINDOW; break;

    case IS_MENUBAR   :
    case IS_MENU      :
    case IS_OPTIONMENU:
    case IS_MENUITEM  :
      ret->mainclass = CL_FAMILY;
      break;

    case IS_TOOLTIPS      :
    case IS_FILESEL       :
    case IS_FILESEL_BUTTON:
      ret->mainclass = CL_OTHER;
      break;
  }

  return ret;
}

int mgtk_add_child(GtkWidget *parent, GtkWidget *child) {

  DebOut("mgtk_add_child(parent %lx, child %lx\n",parent,child);

  if(child->parent != NULL) {
    WarnOut("mgtk_add_child: child %x has already a parent (%x), new parent now: %x\n",(int) child,(int) child->parent,(int) parent);
  }

  child->parent=(APTR) parent;

  if((child->MuiWin!=NULL) && (child->MuiWin!=parent->MuiWin)) {
    WarnOut("mgtk_add_child: child %x has already a MuiWin (%x), new MuiWin now: %x\n",(int) child,(int) child->MuiWin,(int) parent->MuiWin);
  }

  if(parent->MuiWin!=NULL) {
    child->MuiWin=parent->MuiWin;
  }
  else {
    DebOut("WARNING: mgtk_add_child: someone forgot to set MuiWin for %x\n",(int) parent);
  }

  return 0;
}

/* to iterate is human .. */
void mgtk_initchange(GtkWidget *change) {

  DebOut("mgtk_initchange(%lx)\n",change);

  if(change==NULL) {
    return;
  }

  /*
  if(change->type != IS_GROUP) {
    mgtk_initchange((APTR) change->parent);
    return;
  }

  mgtk_initchange((APTR) change->parent);*/

  change=(APTR) change->parent;
  DebOut("  mgtk_initchange: DoMethod(%lx,MUIM_Group_InitChange)\n",change);
  DoMethod(change->MuiObject,MUIM_Group_InitChange);
  return;
}

void mgtk_exitchange(GtkWidget *change) {

  DebOut("mgtk_exitchange(%lx)\n",change);

  if(change==NULL) {
    return;
  }

  /*
  if(change->type != IS_GROUP) {
    mgtk_exitchange((APTR) change->parent);
    return;
  }
  */

  change=(APTR) change->parent;

  DebOut("  mgtk_exitchange: DoMethod(%lx,MUIM_Group_ExitChange)\n",change);
  DoMethod(change->MuiObject,MUIM_Group_ExitChange);
/*  mgtk_exitchange((APTR) change->parent);*/
  return;
}


void mgtk_file_selection_show(GtkWidget *widget);

/*********************************
 * gtk_widget_show
 *********************************/

void gtk_widget_show(GtkWidget *widget) {

  int realized;

	DebOut("gtk_widget_show(%lx)\n",widget);

  realized=widget->flags && GTK_REALIZED;

  /* set realized flag */
  widget->flags |= GTK_REALIZED;

  if(widget->type == IS_WINDOW) {
    set(widget->MuiWin,MUIA_Window_Open,TRUE);
    return;
  }
	else if(widget->type == IS_FILESEL) {
		mgtk_file_selection_show(widget);
		return;
	}
  else if (widget->mainclass == CL_AREA)
  {
		DebOut("better not use ShowMe (?)\n");
    SetAttrs(widget->MuiObject, MUIA_Group_Forward, FALSE, MUIA_ShowMe, TRUE, TAG_DONE);
  }
  else
  {
    WarnOut("gtk_widget_show() for non window/area ignored\n");
  }

  if(!realized) {
    g_signal_emit_by_name(widget,"realized");
  }

	DebOut("  exit\n");
}

void gtk_widget_hide(GtkWidget *widget)
{
  if (widget->mainclass == CL_WINDOW)
  {
    set(widget->MuiWin, MUIA_Window_Open, FALSE);
  }
  else if (widget->mainclass == CL_AREA)
  {
    SetAttrs(widget->MuiObject, MUIA_Group_Forward, FALSE, MUIA_ShowMe, FALSE, TAG_DONE);
  }
  else
  {
    WarnOut("gtk_widget_hide() for non window/area ignored\n");
  }
}

void gtk_widget_show_all(GtkWidget *widget) {
  if(widget->type == IS_WINDOW) {
    set(widget->MuiWin,MUIA_Window_Open,TRUE);
    return;
  }
	else if(widget->type == IS_FILESEL) {
		mgtk_file_selection_show(widget);
		return;
	}
  else if (widget->mainclass == CL_AREA)
  {
    // MUIA_ShowMe is forwarded to children

    set(widget->MuiObject, MUIA_ShowMe, TRUE);
  }
  else
  {
    WarnOut("gtk_widget_show_all() for non window/area ignored\n");
  }
}

void gtk_widget_hide_all(GtkWidget *widget)
{
  if (widget->mainclass == CL_WINDOW)
  {
    set(widget->MuiWin, MUIA_Window_Open, FALSE);
  }
  else if (widget->mainclass == CL_AREA)
  {
    set(widget->MuiObject, MUIA_ShowMe, FALSE);
  }
  else
  {
    WarnOut("gtk_widget_hide_all() for non window/area ignored\n");
  }
}

void gtk_widget_set_sensitive(GtkWidget *widget, gboolean sensitive)
{
  if (widget->mainclass == CL_AREA)
  {
    set(widget->MuiObject, MUIA_Disabled, !sensitive);
  }
  else
  {
    WarnOut("gtk_widget_set_sensitive() - only area class objects supported\n");
  }
}

/* Creates the GDK (windowing system) resources associated with a widget. 
 * For example, widget->window will be created when a widget is realized. 
 * Normally realization happens implicitly; if you show a widget and all 
 * its parent containers, then the widget will be realized and mapped 
 * automatically.

 * Realizing a widget requires all the widget's parent widgets to be 
 * realized; calling gtk_widget_realize() realizes the widget's parents 
 * in addition to widget itself. If a widget is not yet inside a toplevel 
 * window when you realize it, bad things will happen.

 * This function is primarily used in widget implementations, and isn't 
 * very useful otherwise. Many times when you think you might need it, 
 * a better approach is to connect to a signal that will be called after 
 * the widget is realized automatically, such as "expose_event". Or simply 
 * g_signal_connect_after() to the "realize" signal.
 */
void gtk_widget_realize(GtkWidget *widget) {
  GdkWindow *win;

  DebOut("gtk_widget_realize(%lx\n",widget);

  if(!widget) {
    DebOut("WARNING: widget is NULL\n");
    return;
  }

  if(!widget->window) {
    win=g_new0(GdkWindow,1);
    /* and store it dirty */
    win->mgtk_widget=(APTR) widget;
  }
}

void gtk_container_add (GtkContainer *container, GtkWidget *button) {

  DebOut("gtk_container_add(window %lx, button %lx)\n",container,button);

  if(container == NULL) {
    ErrOut("gtk_container_add: window is NULL!?\n");
  }

/*
  DoMethod(container,MUIM_Group_InitChange);
  DoMethod(container,OM_ADDMEMBER,button->MuiObject);
  DoMethod(container,MUIM_Group_ExitChange);
  */

  mgtk_add_child((APTR) container,button);

  mgtk_initchange(button);
  DebOut("DoMethod(%lx,OM_ADDMEMBER,%lx)\n",container->MuiObject,button->MuiObject);
  DoMethod(container->MuiObject,OM_ADDMEMBER,button->MuiObject);
  mgtk_exitchange(button);

  return;
}

void Close_Libs(void);

void gtk_main(void) {
  ULONG sigs = 0;

  while ((DoMethod(mgtk->MuiApp,MUIM_Application_NewInput,&sigs) != MUIV_Application_ReturnID_Quit) && (!gtk_do_main_quit)) {
    if (sigs)
    {
      sigs = Wait(sigs | SIGBREAKF_CTRL_C);
      if (sigs & SIGBREAKF_CTRL_C) break;
    }
  }

  set(mgtk->MuiRoot,MUIA_Window_Open,FALSE);

  #if USE_PENS
  if(mgtk->white_black_optained) {
    ReleasePen(mgtk->screen->ViewPort.ColorMap, mgtk->white_pen);
    ReleasePen(mgtk->screen->ViewPort.ColorMap, mgtk->black_pen);
    mgtk->white_black_optained=0;
  }
    
  if (mgtk->visualinfo) {
    FreeVisualInfo(mgtk->visualinfo);
  }
  if (mgtk->screen) {
    if (mgtk->dri) {
      FreeScreenDrawInfo(mgtk->screen, mgtk->dri);
    }
    UnlockPubScreen(0, mgtk->screen);
  }
  #endif

/*
  ReleasePen(ViewAddress()->ViewPort->ColorMap,mgtk->black_pen);
  ReleasePen(ViewAddress()->ViewPort->ColorMap,mgtk->white_pen);
  */

  MUI_DisposeObject(mgtk->MuiApp);

  FreeRemember(&rememberKey,TRUE);
  Close_Libs();
#if defined ENABLE_RT
  RT_Exit();
#endif
}

GtkContainer* GTK_CONTAINER(GtkWidget *widget) {
  DebOut("GTK_CONTAINER(GtkWidget %lx)\n",widget);
  return widget;
}

GtkWindow* GTK_WINDOW(GtkWidget *widget) {
  DebOut("GTK_WINDOW(GtkWidget %lx)\n",widget);
  return widget;
}

GtkBox* GTK_BOX(GtkWidget *widget) {
  DebOut("GTK_BOX(GtkWidget %lx)\n",widget);
  return widget;
}

/*
GtkWidget* GTK_WIDGET(APTR widget) {
  DebOut("GTK_WIDGET(GtkWidget %lx)\n",widget);
  return (GtkWidget *) widget;
}
*/

GtkMenu* GTK_MENU(GtkWidget *widget) {
  DebOut("GTK_MENU(GtkWidget %lx)\n",widget);
  return widget;
}

GtkMenuShell* GTK_MENU_SHELL(GtkWidget *widget) {
  DebOut("GTK_MENU_SHELL(GtkWidget %lx)\n",widget);
  return widget;
}

GtkMenuItem  *GTK_MENU_ITEM(GtkWidget *widget) {
  DebOut("GTK_MENU_ITEM(GtkWidget %lx)\n",widget);
  return widget;
}

void GTK_WIDGET_SET_FLAGS (GtkWidget *widget, gint flags) {
  DebOut("GTK_WIDGET_SET_FLAGS(%lx,%ld)\n",widget,flags);

  if(flags && GTK_CAN_DEFAULT) {
    DebOut("  GTK_CAN_DEFAULT: nothing required todo.\n");
    flags=flags && (!GTK_CAN_DEFAULT);
  }

  if(flags) {
    widget->flags|=flags;
  }
  return;
}

gboolean  GTK_IS_MENU(GtkWidget *widget) {
  DebOut("GTK_ISMENU(GtkWidget %lx)\n",widget);

  if(widget->type==IS_MENU) {
    DebOut("  IS_MENU\n");
    return(1);
  }
  if(widget->type==IS_MENUITEM) {
    DebOut("  IS_MENUITEM\n");
    return(1);
  }
  if(widget->type==IS_MENUBAR) {
    DebOut("  IS_MENUBAR\n");
    return(1);
  }

  return(0);
}


void gtk_container_set_border_width (GtkContainer *container, guint border_width) {

  WarnOut("gtk_container_set_border_width: ignored (MUI does not need this?)\n");
}

GtkWidget *gtk_hbox_new( gint homogeneous, gint spacing ) {

  Object *box;
  GtkWidget *ret;
  
  ret=mgtk_widget_new(IS_GROUP);
  
  box = HGroup,
	  GroupSpacing(spacing),
 	End;

  /* does this work!? */
  if(box && homogeneous) {
    SetAttrs(box,MUIA_Group_SameWidth,(LONG) 1, TAG_DONE);
  }
  
  DebOut("gtk_hbox_new: %lx\n",box);
  
  ret->MuiObject=box;
  ret->columns=1;
  
  return ret;
}

GtkWidget *mgtk_hbox_new_noparams() {
  return gtk_hbox_new(0,0);
}

GType gtk_vbox_get_type (void) {

  return (GType) IS_GROUP;
}

GtkWidget *gtk_vbox_new( gint homogeneous, gint spacing ) {

  Object *box;
  GtkWidget *ret;
  
  ret=mgtk_widget_new(IS_GROUP);
  
  box = VGroup,
	  GroupSpacing(spacing),
 	End;

  /* does this work!? */
  if(homogeneous) {
    SetAttrs(box,MUIA_Group_SameHeight,(LONG) 1, TAG_DONE);
  }

  DebOut("gtk_vbox_new: %lx\n",box);
  
  ret->MuiObject=box;
  ret->rows=1;
  
  return ret;
}

void mgtk_optionmenu_pack(GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding );

void gtk_box_pack_start(GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding ) {

  DebOut("gtk_box_pack_start: box %lx, widget %lx..)\n",box,child);
  DebOut("  box->MuiObject, child->MuiObject: %lx,%lx\n",box->MuiObject,child->MuiObject);

  /* argl,== not = ..*/
  if(child->type == IS_MENUBAR) {
    DebOut("  gtk_box_pack_start for IS_MENUBAR: faked\n");
    /* remember in child, to which menustrip it will belong */
    child->MuiWinMenustrip=mgtk->MuiRootStrip; /* TODO! */
    return;
  }

  if(child->type == IS_OPTIONMENU) {
    DebOut("  gtk_box_pack_start for IS_OPTIONMENU\n");
    mgtk_optionmenu_pack(box, child, expand, fill, padding);
    return;
  }

  /* expand: TRUE if the new child is to be given extra space 
             allocated to box. The extra space will be divided 
             evenly between all children of box that use this option.  
     fill.:  TRUE if space given to child by the expand option is 
             actually allocated to child, rather than just padding it. 
             This parameter has no effect if expand is set to FALSE. 
             A child is always allocated the full height of a GtkHBox 
             and the full width of a GtkVBox. 
             This option affects the other dimension.
     padding.: extra space in pixels to put between this child 
             and its neighbors, over and above the global amount 
             specified by spacing in GtkBox-struct. If child is a 
             widget at one of the reference ends of box, then 
             padding pixels are also put between child and the 
             reference edge of box.
  */

  if(!expand) {
    fill=TRUE;
  }

  if((!fill) || (!expand)) {
    WarnOut("gtk_box_pack_start: expand=%d, fill=%d ignored\n",expand, fill);
  }

  if(box->type != IS_GROUP) {
    WarnOut("gtk_box_pack_start called on non box !?\n");
  }

  DoMethod(box->MuiObject,MUIM_Group_InitChange);
  if(padding) {
    if(box->columns) {
      DoMethod(box->MuiObject,OM_ADDMEMBER,HSpace(padding));
    }
    else {
      DoMethod(box->MuiObject,OM_ADDMEMBER,VSpace(padding));
    }
  }
  DoMethod(box->MuiObject,OM_ADDMEMBER,child->MuiObject);
  if(padding) {
    if(box->columns) {
      DoMethod(box->MuiObject,OM_ADDMEMBER,HSpace(padding));
    }
    else {
      DoMethod(box->MuiObject,OM_ADDMEMBER,VSpace(padding));
    }
  }
  DoMethod(box->MuiObject,MUIM_Group_ExitChange);

  mgtk_add_child(box,child);
  return;
}

void gtk_box_pack_end(GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding ) {

  DebOut("gtk_box_pack_end: box %lx, widget %lx ..\n",box,child);

#warning gtk_box_pack_end just calls gtk_box_pack_start

  WarnOut("gtk_box_pack_end just calls gtk_box_pack_start\n");

  gtk_box_pack_start(box,child,expand,fill,padding);

  return;
}

void gtk_main_quit (void) {
  gtk_do_main_quit=1;
}

void gtk_widget_destroy( GtkWidget *widget/*, GtkWidget *causingwidget*/ ) {
	GSList *gl;
	gl=mgtk->windows;

  DebOut("gtk_widget_destroy(%lx)\n",widget);
  if(widget->type == IS_WINDOW) {
    DebOut("  widget->type == IS_WINDOW\n");
    /* if more than one window is possible, check if this is the last.\n");*/
		if(g_slist_length(gl)>1) {
			DebOut("WARNING: gtk_widget_destroy: we only hide the window (TODO)\n");
			set(mgtk->MuiRoot,MUIA_Window_Open,FALSE);
			/*MUI_DisposeObject(mgtk->MuiApp);*/
			gl=g_slist_remove(gl,widget);
			mgtk->windows=gl;
		}
		else {
			DebOut("  this was the last window\n");
/*      mgtk_call_destroy_hooks(widget);*/
      g_signal_emit(widget,g_signal_lookup("destroy",0),0);
			gtk_main_quit();
		}
    return;
  }

	if(widget->type == IS_FILESEL) {
		DebOut("  widget->type == IS_FILESEL\n");
		widget->active=0;
		gl=g_slist_remove(gl,widget);
		mgtk->windows=gl;

		return;
	}

	if (widget->mainclass == CL_AREA)
	{
		APTR parent = _parent(widget->MuiObject);

		if (parent)
		{
			DoMethod(parent, MUIM_Group_InitChange);
			DoMethod(parent, OM_REMMEMBER, parent);
			DoMethod(parent, MUIM_Group_ExitChange);
		}

		MUI_DisposeObject(widget->MuiObject);
		#warning how to destroy widget? how? damn...
	}

  DebOut("widget->type == %ld\n",widget->type);
  /*DebOut("causingwidget->type == %ld\n",causingwidget->type);*/
}

/* does not work !? */
void gtk_widget_set_size_request(GtkWidget           *widget,
                                 gint                 width,
                                 gint                 height) {
  DebOut("gtk_widget_set_size_request(%lx,%ld,%ld)\n",widget,width,height);

/*gtk_widget_set_size_request is disabled in parts.. */
#if 0
  if(widget->type == IS_WINDOW) {
    DebOut("MuiWin: %lx\n",widget->MuiWin);

    SetAttrs(widget->MuiObject,
      MA_RootGroup_DefWidth, width,
      MA_RootGroup_DefHeight, height,
    TAG_DONE);
  }
  else {
    printf("  gtk_widget_set_size_request on non window!?\n");
    if(width>0) {
      SetAttrs(widget->MuiObject,MUIA_FixWidth,width, TAG_DONE);
      g_object_set_data(widget,"MUIA_FixWidth",GINT_TO_POINTER(width));
    }
    if(height>0) {
      SetAttrs(widget->MuiObject,MUIA_FixHeight,height, TAG_DONE);
      g_object_set_data(widget,"MUIA_FixHeight",GINT_TO_POINTER(height));
    }
  }
#endif

  /* if MUI object doesnt support this... too bad
  ** however using MUIA_FixWidth/MUIA_FixHeight is not sufficient
  */

  SetAttrs(widget->MuiObject,
    MA_DefWidth, width,
    MA_DefHeight, height,
  TAG_DONE);
}


/* Deprecated: Use gtk_widget_set_size_request() instead. */
void gtk_widget_set_usize(GtkWidget *widget, gint width, gint height) {

  DebOut("gtk_widget_set_usize(%lx,%d,%d)\n",widget,width,height);

  gtk_widget_set_size_request(widget,width,height);
}

/* not perfect.. */
void gtk_widget_grab_default (GtkWidget *widget) {

  DebOut("gtk_widget_grab_default(%lx)\n",widget);
	DebOut("  disabled\n");

  // only area class objects can become default objects
/*
	DebOut("  muiobj: %lx\n",widget->MuiObject);

  if (widget->mainclass == CL_AREA && _win(widget->MuiObject))
  {
    set(_win(widget->MuiObject), MUIA_Window_DefaultObject, (ULONG)widget->MuiObject);
  }
  else
  {
    WarnOut("gtk_widget_grab_default(): not in window or object is wrong type\n");
  }
	*/

  return;
}

void gtk_widget_set_state(GtkWidget *widget, GtkStateType state) {

  DebOut("gtk_widget_set_state(%lx,%d)\n",widget,state);

  switch(state) {
    case GTK_STATE_NORMAL:
      if(widget->MuiObject) {
        set(widget->MuiObject,MUIA_Selected,(ULONG) FALSE);
      }
      return;
    case GTK_STATE_ACTIVE:
    case GTK_STATE_SELECTED:
      if(widget->MuiObject) {
        set(widget->MuiObject,MUIA_Selected,(ULONG) TRUE);
      }
      return;
    case GTK_STATE_PRELIGHT:
      return;
    case GTK_STATE_INSENSITIVE:
      gtk_widget_set_sensitive(widget,FALSE);
      return;
  }
}

void gtk_widget_unref(GtkWidget *widget) {
  DebOut("gtk_widget_unref(%lx) is just a fake\n",widget);
  return;
}

GtkWidget *gtk_widget_ref(GtkWidget *widget) {
  DebOut("gtk_widget_ref(%lx) is just a fake\n",widget);
  return (GtkWidget *) widget;
}

#include <gtk/gtkwidget.h>
gpointer gtk_type_class(GtkType type) {

  GtkWidgetClass *ret;

  DebOut("gtk_type_class(%ld)\n",type);

  DebOut("WARNING: gtk_type_class is just a dummy returning empty Class\n");

  if(type != 0) {
    DebOut("WARNING: unkown widget type %d supplied to gtk_type_class\n");
  }
  ret=g_new(GtkWidgetClass,1);

  return ret;
}

/* gtk_widget_get_type() returns the type code for GtkWidget. */
GType gtk_widget_get_type(void) {

  DebOut("gtk_widget_get_type()\n");
  DebOut("WARNING: gtk_widget_get_type is just a dummy,but should be ok \n");

  return (GType) 0; /* we only have one GType at the moment, widgets */

  /* "The GType API is the foundation of the GObject system. It 
   * provides the facilities for registering and managing all 
   * fundamental data types, user-defined object and interface types. "
   */
}

GtkObject* GTK_OBJECT(APTR widget) {
    return (GtkObject *) widget;
}

/* well, maybe a function to set the events would be
 * more usefull ;)
 */
gint gtk_widget_get_events(GtkWidget *widget) {

  DebOut("gtk_widget_get_events(%lx)\n",widget);

  return widget->gdkevents;
}

GdkVisual *gtk_widget_get_visual(GtkWidget *widget) {

  DebOut("gtk_widget_get_visual(%lx)\n",widget);

  if(!widget->visual) {
    widget->visual=g_new(GdkVisual,1);
  }

  /* we might need that! */
  widget->visual->mgtk_widget=(int *) widget;

  return widget->visual;
}

GdkColormap *gtk_widget_get_colormap(GtkWidget *widget) {

  DebOut("gtk_widget_get_colormap(%lx)\n",widget);

  if(!widget->style) {
    DebOut("WARNING: widget has no style\n");
    return NULL;
  }

  return widget->style->colormap;
}

void gtk_widget_queue_draw(GtkWidget *widget) {
  DebOut("gtk_widget_queue_draw(%lx)\n");

  if(widget->MuiObject) {
    DoMethod(widget->MuiObject,MUIM_Group_InitChange);
    DoMethod(widget->MuiObject,MUIM_Group_ExitChange);
  }
  else {
    DebOut(" WARNING: gtk_widget_queue_draw called on non MUI widget, what to do?n");
  }
}

void gtk_widget_style_get(GtkWidget *widget, const gchar *first_property_name, ...) {

  DebOut("gtk_widget_style_get(%lx,..)\n",widget);

  DebOut("WARNING: Just a dummy\n");
}

/* Sets the GtkStyle for a widget (widget->style). You probably 
 * don't want to use this function; it interacts badly with 
 * themes, because themes work by replacing the GtkStyle.
 */
void gtk_widget_set_style(GtkWidget *widget, GtkStyle *style) {

  DebOut("gtk_widget_set_style(%lx,%lx)\n",widget,style);

  if(!style) {
    DebOut("  style is NULL => set default style\n");
    widget->style=mgtk_get_default_style(mgtk->MuiRoot);
  }
  else {
    widget->style=style;
    g_object_ref(G_OBJECT(style));
  }
}

/* gtk_widget_set_rc_style is deprecated and should not be 
 * used in newly-written code. 
 */
void gtk_widget_set_rc_style(GtkWidget *widget) {

  DebOut("gtk_widget_set_rc_style(%lx)\n",widget);

  gtk_widget_set_style(widget,NULL);
}
