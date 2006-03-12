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

/* GLADE_HOOKUP_OBJECT (window1, table1, "table1");*/
/*GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
      gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref) */

/*
typedef struct {
    char *key;
      gpointer value;
        struct internalGObject *next;
} internalGObject;

*/
void g_object_set_data(GObject *object,
                              const gchar *key,
                              gpointer data) {
  internalGObject *new;
  internalGObject *act;
  char *string;

  DebOut("g_object_set_data(%lx,%s,%lx)\n",object,key,data);

  new=g_new0(internalGObject,1);

  string=g_strdup(key);

  new->key=string;
  new->value=data;
  new->next=NULL;

  /* add new key/value pair */
  if(object->gobjs == NULL) {
    /*first object*/
    DebOut(" added first key value pair\n");
    object->gobjs=(struct internalGObject *) new;
    return;
  }

  /* append */
  act=object->gobjs;
  while(act->next != NULL) {
    DebOut("  act->next=%lx\n",act->next);
    act=(internalGObject *) act->next;
  }

  DebOut("  append %lx to %lx\n",new,act);

  act->next=(struct internalGObject *) new;
}
/* g_object_set_data_full(window1,"table1",gtk_widget_ref (widget),GDestroyNotify) gtk_widget_unref */

void g_object_set_data_full(GObject *object,
                              const gchar *key,
                              gpointer data,
                              GDestroyNotify destroy) {
  DebOut("WARNING: g_object_set_data_full just a wrapper to g_object_set_data ( NOT DONE!)\n");

  g_object_set_data(object,key,data);
}

gpointer    g_object_get_data(GObject *object,
                              const gchar *key) {

  internalGObject *act;

  DebOut("g_object_get_data(%lx,%s)\n",object,key);

  /* this one is normally not set manually, but present for GObjects */
  if(!strcmp(key,"GladeParentKey")) {
    return (gpointer) ((GtkWidget *)object)->parent;
  }

  DebOut("  object->gobjs=%lx\n",object->gobjs);
  DebOut("  searching for %s..\n",key);
  act=(internalGObject *) object->gobjs;
  DebOut("  act=%lx\n",act);
  DebOut("  act->key=%s\n",act->key);
  DebOut("  act->next=%lx\n",act->next);

  DebOut("  loop:\n");
  while((act != NULL) && strcmp(act->key,key)) {
    DebOut("    act: %lx\n",act);
    DebOut("    key: %s\n",act->key);
    DebOut("    next: %lx\n",act->next);
    act=(internalGObject *) act->next;
  }
  DebOut("  loop end\n");

  if (act)
  {
    DebOut(" found value: %lx for key %s (%s)\n",act->value,act->key,key);
    return(act->value);
  }

  DebOut("  value not found!\n");
  return NULL;
}

/* g_object_unref might be called on everey object. Bad thing is, that
 * not all GTK-MUI Object do have the base class GObject, so you
 * cannot assume, that there is a ref_count in GObject.
 * Bad, needs to be fixed on various places.. TODO!
 */
void g_object_unref (GObject *object) {

  DebOut("g_object_unref(%lx)\n",object);

  if(!object) {
    DebOut("WARNING: g_object_unref called on NULL object ..?\n");
    return;
  }

#warning g_object_unref is not completely implemented
  if(!object->ref_count) {
    DebOut("WARNING: g_object_unref not done yet (ref==0)!\n");
    /* TODO: free widget resources..*/
  }
  else {
    object->ref_count--;
  }
}

GObject *g_object_ref (GObject *object) {

  DebOut("g_object_ref(%lx)\n",object);

  object->ref_count++;

  return object;
}


void g_free (gpointer mem) {
  DebOut("faked g_free called\n");
  return;
}

void g_error_free(GError **error) {
  DebOut("faked g_error_free called\n");
  return;
}

/*
gchar* g_strdup_vprintf (const gchar *format, va_list      args) {
  gchar *string = NULL;

  g_vasprintf (&string, format, args);

  return string;
}
*/

/*
gchar* g_strdup_printf (const gchar *format, ...)
{
  gchar *buffer;
  va_list args;

  va_start (args, format);
  buffer = g_strdup_vprintf (format, args);
  va_end (args);

  return buffer;
}
*/
/**********************/

gchar* g_strdup (const gchar *str) {
  gchar *new_str;

  if (str)
    {
      new_str = g_new (char, strlen (str) + 1);
      strcpy (new_str, str);
    }
  else
    new_str = NULL;

  return new_str;
}

gchar* g_strndup (const gchar *str, guint        n) {
  gchar *new_str;

  if (str)
    {
      new_str = g_new (gchar, n + 1);
      strncpy (new_str, str, n);
      new_str[n] = '\0';
    }
  else
    new_str = NULL;

  return new_str;
}

gchar* g_strnfill (guint length, gchar fill_char) {
  register gchar *str, *s, *end;

  str = g_new (gchar, length + 1);
  s = str;
  end = str + length;
  while (s < end)
    *(s++) = fill_char;
  *s = 0;

  return str;
}

gchar* g_strdup_vprintf (const gchar *format, va_list      args1) {
  gchar *buffer;
  va_list args2;

  va_copy(args2, args1);

  buffer = g_new (gchar, g_printf_string_upper_bound (format, args2));

  if (buffer)
    vsprintf (buffer, format, args1);
  va_end (args2);

  return buffer;
}

gchar* g_strdup_printf (const gchar *format, ...) {
  gchar *buffer;
  va_list args;

  va_start (args, format);
  buffer = g_strdup_vprintf (format, args);
  va_end (args);

  return buffer;
}

/*
gchar* g_strconcat (const gchar *string1, ...) {
  guint   l;
  va_list args;
  gchar   *s;
  gchar   *concat;

  g_return_val_if_fail (string1 != NULL, NULL);

  l = 1 + strlen (string1);
  va_start (args, string1);
  s = va_arg (args, gchar*);
  while (s)
    {
      l += strlen (s);
      s = va_arg (args, gchar*);
    }
  va_end (args);

  concat = g_new (gchar, l);
  concat[0] = 0;

  strcat (concat, string1);
  va_start (args, string1);
  s = va_arg (args, gchar*);
  while (s)
    {
      strcat (concat, s);
      s = va_arg (args, gchar*);
    }
  va_end (args);

  return concat;
}

gdouble
g_strtod (const gchar *nptr,
      gchar **endptr)
{
  gchar *fail_pos_1;
  gchar *fail_pos_2;
  gdouble val_1;
  gdouble val_2 = 0;

  g_return_val_if_fail (nptr != NULL, 0);

  fail_pos_1 = NULL;
  fail_pos_2 = NULL;

  val_1 = strtod (nptr, &fail_pos_1);

  if (fail_pos_1 && fail_pos_1[0] != 0)
    {
#ifndef NATIVE_AMIGA
      gchar *old_locale;

      old_locale = g_strdup (setlocale (LC_NUMERIC, NULL));
      setlocale (LC_NUMERIC, "C");
      val_2 = strtod (nptr, &fail_pos_2);
      setlocale (LC_NUMERIC, old_locale);
      g_free (old_locale);
#else 
        val_2 = strtod(nptr, &fail_pos_2);
#endif
    }

  if (!fail_pos_1 || fail_pos_1[0] == 0 || fail_pos_1 >= fail_pos_2)
    {
      if (endptr)
    *endptr = fail_pos_1;
      return val_1;
    }
  else
    {
      if (endptr)
    *endptr = fail_pos_2;
      return val_2;
    }
}

*/

gchar* g_strerror (gint errnum) {
  char *msg;

  msg=g_strdup_printf ("unknown error (%d)", errnum);

  return msg;
}

guint g_printf_string_upper_bound (const gchar* format, va_list      args) {
  guint len = 1;

  while (*format)
    {
      gboolean long_int = FALSE;
      gboolean extra_long = FALSE;
      gchar c;

      c = *format++;

      if (c == '%')
    {
      gboolean done = FALSE;

      while (*format && !done)
        {
          switch (*format++)
        {
          gchar *string_arg;

        case '*':
          len += va_arg (args, int);
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          /* add specified format length, since it might exceed the
           * size we assume it to have.
           */
          format -= 1;
          len += strtol (format, (char**) &format, 10);
          break;
        case 'h':
          /* ignore short int flag, since all args have at least the
           * same size as an int
           */
          break;
        case 'l':
          if (long_int)
            extra_long = TRUE; /* linux specific */
          else
            long_int = TRUE;
          break;
        case 'q':
        case 'L':
          long_int = TRUE;
          extra_long = TRUE;
          break;
        case 's':
          string_arg = va_arg (args, char *);
          if (string_arg)
            len += strlen (string_arg);
          else
            {
              /* add enough padding to hold "(null)" identifier */
              len += 16;
            }
          done = TRUE;
          break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'X':
#ifdef  G_HAVE_GINT64
          if (extra_long)
            (void) va_arg (args, gint64);
          else
#endif  /* G_HAVE_GINT64 */
            {
              if (long_int)
            (void) va_arg (args, long);
              else
            (void) va_arg (args, int);
            }
          len += extra_long ? 64 : 32;
          done = TRUE;
          break;
        case 'D':
        case 'O':
        case 'U':
          (void) va_arg (args, long);
          len += 32;
          done = TRUE;
          break;
        case 'e':
        case 'E':
        case 'f':
        case 'g':
#ifdef HAVE_LONG_DOUBLE
          if (extra_long)
            (void) va_arg (args, long double);
          else
#endif  /* HAVE_LONG_DOUBLE */
            (void) va_arg (args, double);
          len += extra_long ? 128 : 64;
          done = TRUE;
          break;
        case 'c':
          (void) va_arg (args, int);
          len += 1;
          done = TRUE;
          break;
        case 'p':
        case 'n':
          (void) va_arg (args, void*);
          len += 32;
          done = TRUE;
          break;
        case '%':
          len += 1;
          done = TRUE;
          break;
        default:
          /* ignore unknow/invalid flags */
          break;
        }
        }
    }
      else
    len += 1;
    }

  return len;
}


