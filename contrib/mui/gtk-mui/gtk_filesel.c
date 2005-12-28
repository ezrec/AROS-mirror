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
#include <libraries/asl.h>
#include <mui.h>

#include "debug.h"
#include "gtk.h"
#include "gtk_globals.h"

#define OK_WIDGET 1;
#define CANCEL_WIDGET 2;

GtkWidget* gtk_file_selection_new(const gchar *title){

  GtkWidget *ret;
  GtkWidget *ok;
  GtkWidget *cancel;
	GSList *gl;

  DebOut("gtk_file_selection_new(%s)\n",title);

  ret=mgtk_widget_new(IS_FILESEL);
  ok=mgtk_widget_new(IS_FILESEL);
  cancel=mgtk_widget_new(IS_FILESEL);

	ret->title=g_strdup(title);

	/* we store in draw_value, which button this is */
	ok->draw_value=OK_WIDGET;
	cancel->draw_value=CANCEL_WIDGET;

	ret->ok_button=(APTR) ok;
	ret->cancel_button=(APTR) cancel;
	ret->active=1;  /* if 0, someone called a destroy on us */

	mgtk_add_child(ret,ok);
	mgtk_add_child(ret,cancel);

	gl=mgtk->windows;
	gl=g_slist_append(gl,ret);
	mgtk->windows=gl;

  return ret;
}

GtkFileSelection* GTK_FILE_SELECTION(GtkWidget *widget) {
	return((GtkFileSelection *) widget);
}

const gchar* gtk_file_selection_get_filename(GtkFileSelection *filesel) {

	DebOut("gtk_file_selection_get_filename(%lx)\n",filesel);

	return filesel->tip_text;
}

void gtk_file_selection_set_filename (GtkFileSelection *filesel, const gchar *filename) {

	DebOut("gtk_file_selection_set_filename(%lx,%s)\n",filesel,filename);

	filesel->tip_text=g_strdup(filename);

	return;
}

/*
 * this is the main function:
 * - show file selector with all stored parameters
 * - call the stored callbacks for ok/cancel
 */
void mgtk_file_selection_show(GtkWidget *widget) {
	struct FileRequester *req;
	char *dir;
	char *file;
	char empty[]="";
	GtkWidget *button;
	void (*callme)(GtkWidget *widget,gpointer data);
	mgtk_signal_connect_data *callback;
	GSList *gl;

	DebOut("mgtk_file_selection_show(%lx)\n",widget);

	if(widget->tip_text) {   /* we already have a filename */
		file=g_strdup(widget->tip_text);
	}
	else {
		file=g_strdup(empty);
	}

	while(widget->active) {
		if ((req=MUI_AllocAslRequestTags(ASL_FileRequest,
									ASLFR_Window,mgtk->MuiRoot ,
									ASLFR_TitleText, widget->title,
	/*                ASLFR_InitialLeftEdge, left,
										ASLFR_InitialTopEdge , top,
										ASLFR_InitialWidth   , width,
										ASLFR_InitialHeight  , height,*/
									ASLFR_InitialDrawer  , "PROGDIR:",
									ASLFR_InitialFile, file,
	/*                ASLFR_InitialPattern , "#?.iff",*/
	/*                ASLFR_DoSaveMode     , save,
									ASLFR_DoPatterns     , TRUE,*/
									ASLFR_RejectIcons    , TRUE,
									ASLFR_UserData       , widget,
	/*                ASLFR_IntuiMsgFunc   , &IntuiMsgHook,*/
									TAG_DONE))) {

			set(mgtk->MuiApp,MUIA_Application_Sleep,TRUE);

			if (MUI_AslRequestTags(req,TAG_DONE))
			{
				if (*req->fr_File)
				{
					DebOut(" selected dir:  >%s<\n",req->fr_Drawer);
					DebOut(" selected file: >%s<\n",req->fr_File);
					dir=g_strdup(req->fr_Drawer);
					if(strlen(dir)>0) {
						if((dir[strlen(dir)-1] == ':')||(dir[strlen(dir)-1] == '/')) {
							widget->tip_text=g_strdup_printf("%s%s",dir,req->fr_File);
						}
						else {
							widget->tip_text=g_strdup_printf("%s/%s",dir,req->fr_File);
						}
					}
					else {
						widget->tip_text=g_strdup(req->fr_File);
					}
					DebOut("  full filename: %s\n",widget->tip_text);
					g_free(dir);
				}
				button=(GtkWidget *) widget->ok_button;
			}
			else { /* cancel */
				button=(GtkWidget *) widget->cancel_button;
			}
#ifndef __AROS__
			MUI_FreeAslRequest(req); /*??*/
#endif
			set(mgtk->MuiApp,MUIA_Application_Sleep,FALSE);

      /* now call all callbacks of this button */
      if(button->mgtk_signal_connect_data) {
        DebOut("  found remember structure\n");
        callback=(mgtk_signal_connect_data *) button->mgtk_signal_connect_data;
        /* call all remembered functions */
        while(callback) {
          callme=(void *) callback->c_handler;
          DebOut("  calling callback %lx\n",callme);
          if(callback->connect_flags == G_CONNECT_SWAPPED) {
            (*callme)(callback->data,button);
          }
          else {
            (*callme)(button,callback->data);
          }
          callback=(mgtk_signal_connect_data *)callback->next;
        }
      }
		}
    else {
      ErrOut("gtk_filesel.c: ERROR: unable to AllocAslRequest!\n");
    }
	}

	g_free(file);

	/* now check, if there is still a window left */

	gl=mgtk->windows;
	if(!g_slist_length(gl)) {
		DebOut("  request was the last open window, so quit\n");
		gtk_main_quit();
	}

  /* we did it, phew. */
	return;
}
