
#include <gtk/gtk.h>

int main( int   argc,
          char *argv[] )
{
  /* GtkWidget is the storage type for widgets */
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *button;

  /* Initialise GTK */
  gtk_init (&argc, &argv);
    
  /* Create a new window */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Frame Example");

  /* Here we connect the "destroy" event to a signal handler */ 
  g_signal_connect (G_OBJECT (window), "destroy",
		    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_set_size_request (window, 300, 300);
  /* Sets the border width of the window. */
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Create a Frame */
  //frame = gtk_frame_new (NULL);
  frame = gtk_frame_new ("GTK Frame");
  gtk_container_add (GTK_CONTAINER (window), frame);

  gtk_widget_show (frame);

	button = gtk_button_new_with_label ("dummy button");
	gtk_box_pack_end (GTK_BOX (frame), button, TRUE, TRUE, 2);
  
  /* Display the window */
  gtk_widget_show (window);
    
  /* Enter the event loop */
  gtk_main ();
    
  return 0;
}
