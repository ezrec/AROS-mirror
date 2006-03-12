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

GtkProgressBar *GTK_PROGRESS_BAR(GtkWidget* widget) {
    return (GtkProgressBar *) widget;
}

GtkWidget* gtk_progress_bar_new(void) {

  GtkWidget *ret;
  Object *progress;

  DebOut("gtk_progress_bar_new()\n");

  ret=mgtk_widget_new(IS_PROGRESS);

  progress = (APTR) NewObject(CL_ProgressBar->mcc_Class, NULL,MA_Widget,ret,TAG_DONE);

  DebOut(" NewObject=%lx\n",progress);

  ret->MuiObject=progress;

  return ret;
}

gdouble gtk_progress_bar_get_fraction(GtkProgressBar *pbar) {
  int i;

  DebOut("gtk_progress_bar_get_fraction(%lx)\n",pbar);

  i=xget(pbar->MuiObject,MUIA_Gauge_Current);

  return (gdouble) i/100;
}

GtkProgressBarOrientation gtk_progress_bar_get_orientation (GtkProgressBar *pbar) {

  DebOut("TODO: gtk_progress_bar_get_orientation(%lx)\n",pbar);

  return GTK_PROGRESS_LEFT_TO_RIGHT;
}

void gtk_progress_bar_pulse(GtkProgressBar *pbar) {

  DebOut("gtk_progress_bar_pulse(%lx)\n",pbar);

  set(pbar->MuiObject,MA_Pulse,TRUE);

  return;
}

void gtk_progress_bar_set_fraction(GtkProgressBar *pbar, gdouble fraction) {
  int i=0;
  float f=0.0;

  DebOut("gtk_progress_bar_set_fraction(%lx,..)\n",pbar);

  f=fraction*100.0;

  i= 1 * f;

  set(pbar->MuiObject,MA_Pulse,FALSE);

  nnset(pbar->MuiObject,MUIA_Gauge_Current,f);

  return;
}

void gtk_progress_bar_set_orientation (GtkProgressBar *pbar, GtkProgressBarOrientation orientation) {

  DebOut("gtk_progress_bar_set_orientation(%lx,%d)\n",pbar,orientation);

  if(orientation!=GTK_PROGRESS_LEFT_TO_RIGHT) {
    WarnOut("gtk_progress_bar_set_orientation(%d) ignored\n",orientation);
  }

  return;
}

const gchar* gtk_progress_bar_get_text(GtkProgressBar *pbar) {
  gchar *muitext;

  DebOut("gtk_progress_bar_get_text(%lx)\n",pbar);

  muitext=(gchar *) xget(pbar->MuiObject,MUIA_Gauge_InfoText);

  return (const gchar *) muitext;
}

void gtk_progress_bar_set_text(GtkProgressBar *pbar, const gchar *text) {
  gchar *muitext;

  DebOut("gtk_progress_bar_set_text(%lx,%s)\n",pbar,text);

  if(/* no pattern*/ 1) {
    muitext=g_strdup(text);
  }

  set(pbar->MuiObject,MUIA_Gauge_InfoText,muitext);

  g_free(muitext);

  return;
}
