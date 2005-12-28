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
 * $Id: gtk_window.c,v 1.12 2005/12/03 09:14:25 o1i Exp $
 *
 *****************************************************************************/

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <mui.h>

/* cross platform hooks */
#include <SDI_hook.h>

#include "classes.h"
#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

/*********************************
 * gtk_window_new
 *********************************/
GtkWidget* gtk_window_new   (GtkWindowType   type) {

  GtkWidget *win;
  GSList *gl;

  DebOut("gtk_window_new: TODO: check parameter\n");

  win=mgtk_widget_new(IS_WINDOW);

  gl=mgtk->windows;

  if(!g_slist_length(gl)) {
    /* first window */
    DebOut("  first window\n");

    win->MuiWin=mgtk->MuiRoot;
    win->MuiWinMenustrip=mgtk->MuiRootStrip;
    win->MuiGroup=mgtk->Group;
    win->MuiObject=mgtk->Group;
    /* mgtk->winnr++; */
    gl=g_slist_append(gl,win);
    mgtk->windows=gl;
    /* active: 
     * 0: we are inactive (another window is modal)
     * 1: everything normal, all are active
     * 2: we are the modal one
     */
    win->active=1;
    return win;
  }

  WarnOut("gtk_window_new: second window not done yet\n!");
  return(NULL);
}

void gtk_window_set_title   (GtkWindow *window, const gchar *title) {
  DebOut("gtk_window_set_title(win %lx,%s)\n",window,title);

  SetAttrs(window->MuiWin,MUIA_Window_Title,(ULONG) title, TAG_DONE); /* ouch, casting it to ULONG.. */
}

void gtk_window_set_resizable(GtkWindow *window, gboolean resizable) {
#if 0
  ULONG is_open, was_resizable;
#endif

  DebOut("gtk_window_set_resizable(%lx,%d)\n",window,resizable);

#if 0
  was_resizable = xget(window->MuiWin, MUIA_Window_SizeGadget);
  is_open = xget(window->MuiWin, MUIA_Window_Open);

  SetAttrs(window->MuiWin,MUIA_Window_SizeGadget,(ULONG) resizable, TAG_DONE);

  if (is_open && !(was_resizable & resizable))
  {
    /* We must reopen window to toggle sizing gadget
    **
    ** This causes unavoidable flickery. It could be avoided by rootgroup subclass
    ** (by making group fixed size) but the sizing gadget remains.
    **
    ** Hmm...
    */

    SetAttrs(window->MuiWin, MUIA_Window_Open, FALSE, TAG_DONE);
    SetAttrs(window->MuiWin, MUIA_Window_Open, TRUE, TAG_DONE);
  }
#else
	/* Drawback in this method: sizing gadget is still there
	**
	** OTOH is there any good reason to disable resizing? Because GTK coders are lame?
  **
  ** Disable this
	*/

  set(window->MuiObject, MA_RootGroup_Resizable, resizable);
#endif

  return;
}

gboolean gtk_window_get_resizable(GtkWindow *window)
{
	return xget(window->MuiWin, MUIA_Window_SizeGadget);
}

void gtk_window_set_modal(GtkWindow *window, gboolean modal) {

  GSList *windows;
  GtkWidget *w;

  DebOut("gtk_window_set_modal(%lx,%d)\n",window,modal);

  if((window->active == 2) && modal) {
    DebOut("  we are already modal.\n");
    return;
  }

  windows=mgtk->windows;
  if(modal) {
    /* set all other windows to sleep */
    while(windows) {
      w=(GtkWidget *)windows->data;

      if(w==window) {
        DebOut("  set %lx to modal-active\n",w);
        w->active=2;
        if(w->type == IS_WINDOW) {
          set(w->MuiObject,MUIA_Window_Sleep,FALSE);
        }
      }
      else {
        DebOut("  set %lx to modal-inactive\n",w);
        w->active=0;
        if(w->type == IS_WINDOW) {
          set(w->MuiObject,MUIA_Window_Sleep,TRUE);
        }
      }
      windows=g_slist_next(windows);
    }
  }
  else {
    /* wake all other windows */
    while(windows) {
      w=(GtkWidget *)windows->data;
      if(w->active==2) {
        DebOut("  set %lx to normal (was modal)\n",w);
        w->active=1;
      }
      else {
        DebOut("  set %lx to normal (was modal-inactive)n",w);
        w->active=1;
        if(w->type == IS_WINDOW) {
          set(w->MuiObject,MUIA_Window_Sleep,FALSE);
        }
      }
      windows=g_slist_next(windows);
    }
  }
}

/* Dialog windows should be set transient for the main application window 
 * they were spawned from. This allows window managers to e.g. keep the 
 * dialog on top of the main window, or center the dialog over the main window. 
 * gtk_dialog_new_with_buttons() and other convenience functions in 
 * GTK+ will sometimes call gtk_window_set_transient_for() on your behalf.
 *
 * On Windows, this function will and put the child window on top of 
 * the parent, much as the window manager would have done on X.
 * In MUI we make the parent window the RefWindow of the child.
 * The child won't stay at top, but it should be sufficient.
 */
void gtk_window_set_transient_for(GtkWindow *window, GtkWindow *parent) {

  DebOut("gtk_window_set_transient_for(%lx,%lx)\n");

  if((!window) || (!parent) || (!window->MuiObject) || (!parent->MuiObject)) {
    WarnOut("  unable to gtk_window_set_transient_for, something is NULL!\n");
    return;
  }

  set(window->MuiObject, MUIA_Window_RefWindow,parent->MuiObject);
  set(window->MuiObject, MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered);
  set(window->MuiObject, MUIA_Window_TopEdge,  MUIV_Window_TopEdge_Centered);
}
