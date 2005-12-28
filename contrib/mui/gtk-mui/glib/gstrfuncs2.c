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
 * $Id: gstrfuncs2.c,v 1.4 2005/12/04 00:31:00 o1i Exp $
 *
 *****************************************************************************/

/* just a warning, this is a quick hack.. */

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

GString *g_string_new(const gchar *init) {

  if(!init) {
    return g_strdup("");
  }

  return g_strdup(init);
}

gchar *g_string_free(GString *string, gboolean free_segment) {

  if(free_segment) {
    g_free(string);
    return NULL;
  }
  return string;
}

GString *g_string_append_c(GString *string, gchar c) {

  char *tmp;

  DebOut("g_string_append_c(%s,%c)\n",string,c);

  if(string) {
    tmp=g_strdup_printf("%s%c",string,c);
    g_free(string);
  }
  else {
    tmp=g_strdup_printf("%c",c);
  }

  return tmp;
}

GString *g_string_append(GString *string, const gchar *val) {

  char *tmp;

  DebOut("g_string_append(%s,%s)\n",string,val);

  if(string) {
    if(val) {
      tmp=g_strdup_printf("%s%s",string,val);
      g_free(string);
    }
    else {
      tmp=string;
    }
  }
  else {
    tmp=(char *)val;
  }

  return tmp;
}




