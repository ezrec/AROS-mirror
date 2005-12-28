
#include <gtk/gtk.h>

typedef struct _GtkObjectClass  GtkObjectClass;
typedef struct _GtkArg  GtkArg;

struct _GtkArg
{
  GtkType type;
  gchar *name;

  /* union skipped ..*/
};

struct _GtkObjectClass
{
  GObjectClass parent_class;
  /* Non overridable class methods to set and get per class arguments */
#if 0
  void (*set_arg) (GtkObject *object,
                   GtkArg    *arg,
                   guint      arg_id);
  void (*get_arg) (GtkObject *object,
                   GtkArg    *arg,
                   guint      arg_id);

#endif
  void (*destroy)  (GtkObject *object);
  
  int foo;
};
