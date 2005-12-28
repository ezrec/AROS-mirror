
/* dummy */

#include <gtk/gtk.h>

#include <gtk/gtkbox.h>

#define GTK_TYPE_VBOX    (gtk_vbox_get_type ())

typedef struct _GtkVBoxClass  GtkVBoxClass;

struct _GtkVBoxClass
{
    GtkBoxClass parent_class;
};

