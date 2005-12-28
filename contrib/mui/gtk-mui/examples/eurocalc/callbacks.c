#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

// Definetions
#define D_TO_E 0
#define E_TO_D 1

// These are the declarations for the function and the flags
void compute_result(GtkWidget *this_widget, const char *fraga, int type);

void
on_euro_entry_activate                 (GtkEntry        *entry,
                                        gpointer         user_data)
{

compute_result(GTK_WIDGET(entry), gtk_editable_get_chars(GTK_EDITABLE(entry), 0, -1) , E_TO_D);
}

void
on_drx_entry_activate                  (GtkEntry        *entry,
                                        gpointer         user_data)
{
compute_result(GTK_WIDGET(entry), gtk_editable_get_chars(GTK_EDITABLE(entry), 0, -1), D_TO_E);
}

// Main Code
void compute_result(GtkWidget *this_widget, const char *fraga, int type)
{
   GtkWidget *other_entry = NULL;
   double result = 0.0;
   gchar *result_string = NULL;

   switch(type) {
     case D_TO_E:
			 result = (atof(fraga) / 340.75);
       other_entry = lookup_widget(this_widget, "euro_entry");
       break;
     case E_TO_D:
			 result  = (atof(fraga) * 340.75);
       other_entry = lookup_widget(this_widget, "drx_entry");
       break;
   }
   result_string = g_strdup_printf("%5.2f",result);
   gtk_entry_set_text(GTK_ENTRY(other_entry), result_string);
   g_free(result_string);
}

