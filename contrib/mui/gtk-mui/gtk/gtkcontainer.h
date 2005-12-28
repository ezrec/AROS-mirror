#ifndef MGTK_HAVE_CONTAINER_H
#define MGTK_HAVE_CONTAINER_H 1

#include <gtk/gtk.h>

#include <gtk/gtkwidget.h>

typedef struct _GtkContainerClass  GtkContainerClass;

struct _GtkContainerClass
{
    GtkWidgetClass parent_class;

    /* TODO ? */
};

#endif
