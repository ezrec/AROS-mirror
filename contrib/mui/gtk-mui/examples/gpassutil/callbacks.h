#include <gtk/gtk.h>

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_c_custom_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_generate_btn_clicked                (GtkButton       *button,
                                        gpointer         user_data);

guint
generating_callback                    (gpointer        data);

void
on_about_btn_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_generate_btn_pressed                (GtkButton       *button,
                                        gpointer         user_data);

void
on_generate_btn_released               (GtkButton       *button,
                                        gpointer         user_data);
