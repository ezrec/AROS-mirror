#ifndef __GTK_WIDGET_H__
#define __GTK_WIDGET_H__

#include <gtk/gtk.h>
#include <gtk/gtkobject.h>


/* Macro for casting a pointer to a GtkWidget or GtkWidgetClass pointer.
 * Macros for testing whether `widget' or `klass' are of type GTK_TYPE_WIDGET.
 */
#define GTK_TYPE_WIDGET			  (gtk_widget_get_type ())
#define GTK_WIDGET(widget)		  (G_TYPE_CHECK_INSTANCE_CAST ((widget), GTK_TYPE_WIDGET, GtkWidget))
#define GTK_WIDGET_CLASS(klass)		  (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_WIDGET, GtkWidgetClass))
#define GTK_IS_WIDGET(widget)		  (G_TYPE_CHECK_INSTANCE_TYPE ((widget), GTK_TYPE_WIDGET))
#define GTK_IS_WIDGET_CLASS(klass)	  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_WIDGET))
#define GTK_WIDGET_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_WIDGET, GtkWidgetClass))

/* Macros for extracting various fields from GtkWidget and GtkWidgetClass.
 */
#define GTK_WIDGET_TYPE(wid)		  (GTK_OBJECT_TYPE (wid))
#define GTK_WIDGET_STATE(wid)		  (GTK_WIDGET (wid)->state)
#define GTK_WIDGET_SAVED_STATE(wid)	  (GTK_WIDGET (wid)->saved_state)

/* Macros for extracting the widget flags from GtkWidget.
 */
#define GTK_WIDGET_FLAGS(wid)		  (wid->flags)
#define GTK_WIDGET_TOPLEVEL(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_TOPLEVEL) != 0)
#define GTK_WIDGET_NO_WINDOW(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_NO_WINDOW) != 0)
#define GTK_WIDGET_REALIZED(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_REALIZED) != 0)
#define GTK_WIDGET_MAPPED(wid)		  ((GTK_WIDGET_FLAGS (wid) & GTK_MAPPED) != 0)
#define GTK_WIDGET_VISIBLE(wid)		  ((GTK_WIDGET_FLAGS (wid) & GTK_VISIBLE) != 0)
#define GTK_WIDGET_DRAWABLE(wid)	  (GTK_WIDGET_VISIBLE (wid) && GTK_WIDGET_MAPPED (wid))
#define GTK_WIDGET_SENSITIVE(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_SENSITIVE) != 0)
#define GTK_WIDGET_PARENT_SENSITIVE(wid)  ((GTK_WIDGET_FLAGS (wid) & GTK_PARENT_SENSITIVE) != 0)
#define GTK_WIDGET_IS_SENSITIVE(wid)	  (GTK_WIDGET_SENSITIVE (wid) && \
					   GTK_WIDGET_PARENT_SENSITIVE (wid))
#define GTK_WIDGET_CAN_FOCUS(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_CAN_FOCUS) != 0)
#define GTK_WIDGET_HAS_FOCUS(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_HAS_FOCUS) != 0)
#define GTK_WIDGET_CAN_DEFAULT(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_CAN_DEFAULT) != 0)
#define GTK_WIDGET_HAS_DEFAULT(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_HAS_DEFAULT) != 0)
#define GTK_WIDGET_HAS_GRAB(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_HAS_GRAB) != 0)
#define GTK_WIDGET_RC_STYLE(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_RC_STYLE) != 0)
#define GTK_WIDGET_COMPOSITE_CHILD(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_COMPOSITE_CHILD) != 0)
#define GTK_WIDGET_APP_PAINTABLE(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_APP_PAINTABLE) != 0)
#define GTK_WIDGET_RECEIVES_DEFAULT(wid)  ((GTK_WIDGET_FLAGS (wid) & GTK_RECEIVES_DEFAULT) != 0)
#define GTK_WIDGET_DOUBLE_BUFFERED(wid)	  ((GTK_WIDGET_FLAGS (wid) & GTK_DOUBLE_BUFFERED) != 0)
  
/* Macros for setting and clearing widget flags.
 */
#define GTK_WIDGET_UNSET_FLAGS(wid,flag)  G_STMT_START{ (GTK_WIDGET_FLAGS (wid) &= ~(flag)); }G_STMT_END

#define GTK_TYPE_REQUISITION              (gtk_requisition_get_type ())


typedef struct _GtkRequisition     GtkRequisition;
typedef struct _GtkWidgetClass     GtkWidgetClass;

struct _GtkWidgetClass
{
  GtkObjectClass parent_class;

  /* Default signal handler for the ::destroy signal, which is
  *  invoked to request that references to the widget be dropped.
  *  If an object class overrides destroy() in order to perform class
  *  specific destruction then it must still invoke its superclass'
  *  implementation of the method after it is finished with its
  *  own cleanup. (See gtk_widget_real_destroy() for an example of
  *  how to do this).
  */


  /* o1i: I could not find the following stuff in the GLib/GTK includes..? */
  void (*realize)  (GtkObject *object);
  gint (*expose_event)  (GtkObject *object,GdkEventExpose   *event);
  void (*size_request)  (GtkObject *object, GtkRequisition *requisition);
  void (*size_allocate)  (GtkObject *object, GtkAllocation *allocation);
  gint (*button_press_event)  (GtkObject *object, GdkEventButton   *event);
  gint (*button_release_event)  (GtkObject *object, GdkEventButton   *event);
  gint (*motion_notify_event)  (GtkObject *object, GdkEventMotion   *event);

  /* normal stuff continues..*/

  guint activate_signal;
  guint set_scroll_adjustments_signal;

};


#endif /* __GTK_WIDGET_H__ */

