#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

//#include <gnome.h>
#include <stdlib.h>
#include <unistd.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

GString *alpha;
GString *numbers;
GString *symbols;
GString *custom;
GString *buffer;
GtkWidget *pass_lbl;
int pass_length;
gint timeout;

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_c_custom_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	extern GtkWidget *window;
	GtkWidget *details;
	
	if (!(details = lookup_widget (window, "c_custom_details")))
			return;
	
	gtk_widget_set_sensitive (details, gtk_toggle_button_get_active (togglebutton));
}


void
on_generate_btn_pressed                (GtkButton       *button,
                                        gpointer         user_data)
{
	extern GtkWidget *window;
	GtkWidget *c_alpha          = lookup_widget (window, "c_alpha");
	GtkWidget *c_numbers        = lookup_widget (window, "c_numbers");
	GtkWidget *c_symbols        = lookup_widget (window, "c_symbols");
	GtkWidget *c_custom         = lookup_widget (window, "c_custom");
	GtkWidget *c_custom_details = lookup_widget (window, "c_custom_details");
	GtkWidget *length           = lookup_widget (window, "length");
	int i;
	
begin:
	/* Initialize values */
	buffer   = g_string_new (NULL);
	pass_lbl = lookup_widget (window, "pass_lbl");
	alpha    = g_string_new (NULL);
	numbers  = g_string_new (NULL);
	symbols  = g_string_new (NULL);
	custom   = g_string_new (NULL);
	
	/* Create alpha */
	for (i = 'A'; i <= 'Z'; i++)
			alpha = g_string_append_c (alpha, i);
	
	for (i = 'a'; i <= 'z'; i++)
			alpha = g_string_append_c (alpha, i);
	
	/* Create numbers */
	for (i = '0'; i <= '9'; i++)
			numbers = g_string_append_c (numbers, i);
	
	/* Create symbols */
	symbols = g_string_append (symbols, "!@#$%&=?.;|+*~'\"");
	
	/* Create custom */
	custom = g_string_append (custom, gtk_entry_get_text (GTK_ENTRY (c_custom_details)));

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (c_alpha)))   buffer=g_string_append (buffer, alpha);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (c_numbers))) buffer=g_string_append (buffer, numbers);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (c_symbols))) buffer=g_string_append (buffer, symbols);
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (c_custom)))  buffer=g_string_append (buffer, custom);

/*  buffer=g_string_append (buffer, alpha);*/
	
	if (!strlen(buffer)) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c_alpha), TRUE);
		goto begin;
	}
	
	/* Get desired length */
	pass_length = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (length));
	
	/* Create the timeout */
	timeout = g_timeout_add (50, generating_callback, NULL);
	generating_callback (NULL);
	
	g_string_free (alpha, TRUE);
	g_string_free (numbers, TRUE);
	g_string_free (symbols, TRUE);
	g_string_free (custom, TRUE);
}

void
on_generate_btn_released               (GtkButton       *button,
                                        gpointer         user_data)
{
	g_source_remove (timeout);
	
	
}

guint generating_callback (gpointer data) {
	gchar *password = g_string_new (NULL);
	static int v = 0;
	int i;
	
	/* Randomize */
	srand (time (NULL)+(++v));
	
	/* Create password */
	for (i = 0; i < pass_length; i++) {
		password=g_string_append_c (password, buffer[(int)(rand()%strlen(buffer))]);
	}
	
	gtk_label_set_text (GTK_LABEL (pass_lbl), password);
	
	return TRUE;
}

void
on_about_btn_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  /*
	GnomeAbout *about;
	
	about = create_about ();
	gtk_widget_show (about);
  */
}
