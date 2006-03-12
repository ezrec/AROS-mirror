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

#ifndef MGTK_HAVE_GTK_H
#define MGTK_HAVE_GTK_H 1

#include <stdio.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <stdarg.h>

#include <glib/gmacros.h>
#include <gdk/gdkconfig.h>
#include <gtk/gtkenums.h>

/* internal GTK-MUI defines */
#define BADMAGIC 0xDEADBEEF;

/* Pens used for GTK styles */
#define MGTK_PEN_FG      TEXTPEN
#define MGTK_PEN_BG      BACKGROUNDPEN
#define MGTK_PEN_LIGHT   SHADOWPEN
#define MGTK_PEN_DARK    TEXTPEN
#define MGTK_PEN_MID     TEXTPEN
#define MGTK_PEN_TEXT    TEXTPEN
#define MGTK_PEN_BASE    BACKGROUNDPEN
#define MGTK_PEN_TEXT_AA TEXTPEN

#define MGTK_PEN_WHITE   SHINEPEN
#define MGTK_PEN_BLACK   TEXTPEN

/* GLIB */
#include <glib/gtypes.h>

/*typedef TimeVal GTimeVal ;*/
#define     G_GNUC_CONST
#define GDestroyNotify gint

typedef struct _GClosure GClosure;
struct _GClosure {
  int foo;
  /* only glib private stuff in here */
};

#include <glib/gobject/gsignal.h>

typedef struct _GError GError;
struct _GError
{
  int badmagic;
  char *message;
};

typedef struct _GList           GList;

struct _GList
{
  gpointer data;
  GList *next;
  GList *prev;
};


#ifndef HAVE_GSLIST_DEFINED
#define HAVE_GSLIST_DEFINED 1
typedef struct _GSList		GSList;

struct _GSList
{
  gpointer data;
  GSList *next;
};
#endif


/* GTK */

typedef void  (*GCallback) (void);
typedef void  (*GtkSignalFunc) (void);
typedef void  (*GSourceFunc) (void);
typedef void  (*GtkFunction) (void);
typedef void  (*GClosureNotify) (gpointer        data,
                                 GClosure       *closure);
typedef void  (*GtkDestroyNotify) (gpointer        data,
                                 GClosure       *closure);

typedef gpointer GtkCallbackMarshal; /* dummy only */

gchar* g_strdup (const gchar *str);

APTR mgtk_allocvec(ULONG size, ULONG req);
VOID mgtk_freevec (APTR ptr);
APTR mgtk_allocmem(ULONG size, ULONG req);
VOID mgtk_freemem (APTR ptr, ULONG size);

#define g_new(type, count)      \
    ((type *) mgtk_malloc ((unsigned) sizeof (type) * (count)))

#define g_new0(type, count)      \
    ((type *) mgtk_malloc ((unsigned) sizeof (type) * (count)))


#define g_print printf
#define g_signal_connect(instance, detailed_signal, c_handler, data) \
    g_signal_connect_data ((instance), (detailed_signal), (c_handler), (data), \
    NULL, (GConnectFlags) 0)
#define g_signal_connect_swapped(instance, detailed_signal, c_handler, data) \
        g_signal_connect_data ((instance), (detailed_signal), (c_handler), (data), NULL, G_CONNECT_SWAPPED)
#define G_CALLBACK(f) ((GCallback) (f))

#define gtk_signal_lookup(name,object_type) \
  g_signal_lookup ((name), (object_type))
#define gtk_signal_name(signal_id) \
  g_signal_name (signal_id)
#define gtk_signal_emit_stop(object,signal_id) \
  g_signal_stop_emission ((object), (signal_id), 0)
#define gtk_signal_connect(object,name,func,func_data) \
  gtk_signal_connect_full ((object), (name), (func), 0, (func_data), 0, 0, 0)
#define gtk_signal_connect_after(object,name,func,func_data) \
  gtk_signal_connect_full ((object), (name), (func), 0, (func_data), 0, 0, 1)
#define gtk_signal_connect_object(object,name,func,slot_object) \
  gtk_signal_connect_full ((object), (name), (func), 0, (slot_object), 0, 1, 0)
#define gtk_signal_connect_object_after(object,name,func,slot_object) \
  gtk_signal_connect_full ((object), (name), (func), 0, (slot_object), 0, 1, 1)
#define gtk_signal_disconnect(object,handler_id) \
  g_signal_handler_disconnect ((object), (handler_id))


/* all checks disabled ;) */
#define _G_TYPE_CIC(ip, gt, ct) ((ct*) ip)

#define G_TYPE_CHECK_INSTANCE_CAST(instance, g_type, c_type) (_G_TYPE_CIC ((instance), (g_type), c_type))
#define G_OBJECT(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), G_TYPE_OBJECT, GObject))
#define GTK_CHECK_CAST(tobj, cast_type, cast) ((cast*) (tobj))
#define g_return_if_fail(expr)         { (void)0; }
#define g_return_val_if_fail(expr,val) { (void)0; }

/* bad hack, of course ;) */
/* see glib-2.0/gobject/gmarshal.h .. */
#define g_cclosure_marshal_VOID__VOID NULL

#define g_print printf

/* from gaim/internal.h*/
#define N_(String) (String)
#define _(x) (x)

/* normally in limits.h */
#define MB_LEN_MAX 6

typedef enum
{
	CL_UNDEFINED,
	CL_FAMILY,
	/* CL_APPLICATION, */ /* not possible */
	CL_WINDOW,
	CL_AREA,
	CL_OTHER, /* really undefined */
} mgtkMUIClass;

typedef struct {
  char *key;
  gpointer value;
  struct internalGObject *next;
} internalGObject;

typedef struct _mgtk_signal_connect_data mgtk_signal_connect_data;
struct _mgtk_signal_connect_data{
  gchar *detailed_signal;
  guint id;
  GCallback          c_handler; /* callback*/
  gpointer           data;
  GClosureNotify     destroy_data;
  GConnectFlags      connect_flags;
  mgtk_signal_connect_data *next;
};

typedef struct {
  guint type;
  guint id;
  gchar *name;
  APTR next;
} mgtk_SignalType;

/*gdk */

typedef struct _GObject GObject;
typedef struct _GObject GtkObject;

struct _GObject {
  /* GObject parentstuff;*/
  /* internal attributes */
  int badmagic;
  /* GLIB */
  struct internalGObject *gobjs;
  guint ref_count;
};

#include <glib/gobject/gobject.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkvisual.h>
#include <gdk/gdkgc.h>

typedef struct _GdkDevice GdkDevice;
struct _GdkDevice
{
/*    GObject parent_instance;*/
    /* All fields are read-only */

    gchar *name;
    /* don't care about the rest for now ;) */
};


typedef enum
{
  GDK_DRAG_PROTO_MOTIF,
  GDK_DRAG_PROTO_XDND,
  GDK_DRAG_PROTO_ROOTWIN,         /* A root window with nobody claiming
  * drags */
  GDK_DRAG_PROTO_NONE,            /* Not a valid drag window */
  GDK_DRAG_PROTO_WIN32_DROPFILES, /* The simple WM_DROPFILES dnd */
  GDK_DRAG_PROTO_OLE2,            /* The complex OLE2 dnd (not implemented) */
  GDK_DRAG_PROTO_LOCAL            /* Intra-app */
} GdkDragProtocol;

typedef enum
{
  GDK_ACTION_DEFAULT = 1 << 0,
  GDK_ACTION_COPY    = 1 << 1,
  GDK_ACTION_MOVE    = 1 << 2,
  GDK_ACTION_LINK    = 1 << 3,
  GDK_ACTION_PRIVATE = 1 << 4,
  GDK_ACTION_ASK     = 1 << 5
} GdkDragAction;

typedef struct _GdkDragContext        GdkDragContext;
struct _GdkDragContext {
 /* GObject parent_instance;*/
  
  /*< public >*/
  
  GdkDragProtocol protocol;
  
  gboolean is_source;
  
  GdkWindow *source_window;
  GdkWindow *dest_window;
  
  GList *targets;
  GdkDragAction actions;
  GdkDragAction suggested_action;
  GdkDragAction action;
  
  guint32 start_time;

  /*< private >*/
  
  gpointer windowing_data;
};

#include <gdk/gdkwindow.h>

/* GTK */
typedef enum
{
  GTK_TOPLEVEL         = 1 << 4,
  GTK_NO_WINDOW        = 1 << 5,
  GTK_REALIZED         = 1 << 6,
  GTK_MAPPED           = 1 << 7,
  GTK_VISIBLE          = 1 << 8,
  GTK_SENSITIVE        = 1 << 9,
  GTK_PARENT_SENSITIVE = 1 << 10,
  GTK_CAN_FOCUS        = 1 << 11,
  GTK_HAS_FOCUS        = 1 << 12,
  GTK_CAN_DEFAULT      = 1 << 13,
  GTK_HAS_DEFAULT      = 1 << 14,

  GTK_HAS_GRAB         = 1 << 15,
  GTK_RC_STYLE         = 1 << 16,
  GTK_COMPOSITE_CHILD  = 1 << 17,
  GTK_NO_REPARENT      = 1 << 18,
  GTK_APP_PAINTABLE    = 1 << 19,
  GTK_RECEIVES_DEFAULT = 1 << 20,
  GTK_DOUBLE_BUFFERED  = 1 << 21,
  GTK_NO_SHOW_ALL      = 1 << 22
} GtkWidgetFlags;



#include <gtk/gtkstyle.h>

/***************************************************
 * GtkWidget
 *
 * Every GTK Object uses the following struct
 ***************************************************/
#include <gtk/gtkrequisition.h>

#ifndef GtkWidget
typedef struct _GtkWidget GtkWidget;
#endif

#include <gtk/gtkwidget.h>
#include <gdk/gdkevents.h>

struct _GtkWidget {
  GObject object;

  int type;
  GtkWidget *parent;
  APTR MuiWin;     /* if !IS_WINDOW, this points to the MUI root Window (TODO) */
  APTR MuiWinMenustrip;
  APTR MuiGroup; 
  Object *MuiObject; 
  /*** GTK attributes ***/
  /* stuff to clean up ;): */
  GSList *GSList;           /* for radio buttons and more */
  mgtk_signal_connect_data *mgtk_signal_connect_data; /* if you want to add a signal later */
  mgtkMUIClass mainclass; /* main class tree (area, window, application), defines what kind of tags and methods are available */

  /*** GTK attributes ***/
  GtkStateType state;
  GtkWidgetFlags flags;
  /*** official GTK Widget attributes ***/
   /* The style for the widget. The style contains the
    *  colors the widget should be drawn in for each state
    *  along with graphics contexts used to draw with and
    *  the font to use for text.
    */
  GtkStyle *style; 
   /* The widgets allocated size. */
  GtkAllocation allocation;
   /* The widgets window or its parent window if it does
    *  not have a window. (Which will be indicated by the
    *  GTK_NO_WINDOW flag being set).
    */
  GdkWindow *window;
  /* at the moment, not used */
  GdkEventMask gdkevents;
  /* stuff to clean up ;): */
  gchar *title;
	gchar *tip_text;          /* tooltip */
  int rows;                 /* table stuff */
  int columns;
  int text_length;          /* for entry */
  int active;               /* for checkboxes, and windows */
  GtkWidget *nextObject;
  guint draw_value;         /* sliders:	non-zero if the scale's current value is displayed next to the slider.*/
  guint value_pos;          /* sliders:	the position in which the textual value is displayed, selected from GtkPositionType.*/
  guint decimalplaces;      /* sliders: the number of decimal places to display, 1 to display 1.0, 2 to display 1.00 etc.*/
  guint digits;             /* spin buttons */
  gboolean wrap;            /* spin buttons */
  gdouble lower;            /* adjustment start */
  gdouble upper;
  gdouble value;
  gdouble step_increment;
  gdouble page_increment;
  gdouble page_size;        /*adjustment stop */
  guint tab_pos; /* notebook */
  guint show_tabs; /* notebook */
  guint show_border; /* notebook */
	GtkWidget *ok_button;     /* file selection */
	GtkWidget *cancel_button; /* file selection */
  /* internal ones ? */
  GdkVisual *visual;
  GtkWidgetClass *class;
};


typedef struct {
  GType type_id;
  gchar *type_name;
  guint16    instance_size;
  GtkWidget *(*newparentobject) (void);
  GtkWidget *(*newobject) (void);
  GtkWidgetClass *class;
  APTR next;
} mgtk_Fundamental;

/***************************************************
 * global data 
 ***************************************************/
struct mgtk_ObjApp {
  APTR MuiApp;
  APTR MuiRoot;      /* WindowObject */
  APTR MuiRootStrip; /* WindowObject Menu Strip*/
  APTR Group;        /* GroupObject */
  GSList *windows;   /* all GTK windows in a linked list */
  GtkWidget *grab;   /* the widget getting all keystrokes.. TODO */
  guint mgtk_signal_count;
  mgtk_SignalType *mgtk_signal;       /* All GTK (custom) signals  */
  GType mgtk_fundamental_count;
  mgtk_Fundamental *mgtk_fundamental; /* All widget types */
  GtkStyle *default_style;

  #if USE_PENS
  guint white_black_optained;
  LONG white_pen;
  LONG black_pen;
  guint other_pens_optained;
  struct Screen   *screen;
  struct DrawInfo *dri;
  APTR             visualinfo;
  #endif

//  int winnr;
};

/* ATTENTION: take care in hooks.c, when you add!! */
typedef enum {
  IS_NULL,         /* if type=IS_NULL, something is not initialized correctly */
  IS_WINDOW,
  IS_GROUP,
  IS_MENUBAR,      /*  3 top level (not visible under AmigaOS) */
  IS_MENU,         /*  4 menu      (not visible)               */
  IS_OPTIONMENU,   /*  5 menu      (not visible)               */
  IS_MENUITEM,     /*  6 menu item (menu item)                 */
  IS_TABLE,        /*  7 */
  IS_TABLECELL,    /*  8 */
  IS_ENTRY,        /*  9 */
  IS_TOGGLEBUTTON,
  IS_CHECKBUTTON,
  IS_BUTTON,
  IS_LABEL,
  IS_IMAGE,
  IS_RADIO,
  IS_SEPARATOR,
  IS_ARROW,
  IS_SCROLLBAR,    /* 18 */
  IS_ADJUSTMENT,   /* (not visible under AmigaOS) */
	IS_TOOLTIPS,     /* group of tooltips, not visible */
	IS_FILESEL,      /* file selector, no MuiObject attached */
	IS_FILESEL_BUTTON,      /* file selector button, no MuiObject attached */
	IS_FRAME,
  IS_SPINBUTTON,
  IS_NOTEBOOK,
  IS_PROGRESS,
  IS_FIXED,
  IS_HANDLEBOX,
  IS_TOOLBAR,
  IS_SPACE,
  IS_CUSTOM,
  IS_LASTONE       /* type=IS_LASTONE, something is not initialized correctly */
} mgtkWidgetType;

typedef enum
{
  GTK_MESSAGE_INFO,
  GTK_MESSAGE_WARNING,
  GTK_MESSAGE_QUESTION,
  GTK_MESSAGE_ERROR
} GtkMessageType;

#include <gtk/gtkprogressbar.h>

typedef GtkWidget GtkWindow;
typedef GtkWidget GtkBox;
typedef GtkWidget GtkVBox;
typedef GtkWidget GtkMenu;
typedef GtkWidget GtkMenuShell;
typedef GtkWidget GtkMenuItem;
typedef GtkWidget GtkTable;
typedef GtkWidget GtkEntry;
typedef GtkWidget GtkEditable;
typedef GtkWidget GtkToggleButton;
typedef GtkWidget GtkContainer;
typedef GtkWidget GdkPixbuf;
typedef GtkWidget GtkTextBuffer;
typedef GtkWidget GtkRadioButton;
typedef GtkWidget GtkScale;
typedef GtkWidget GtkAdjustment;
/*typedef GtkWidget GtkRange;*/
typedef GtkWidget GtkOptionMenu;
typedef GtkWidget GtkTooltips;
typedef GtkWidget GtkFileSelection;
typedef GtkWidget GtkFrame;
typedef GtkWidget GtkSpinButton;
typedef GtkWidget GtkNotebook;
typedef GtkWidget GtkProgressBar;
typedef GtkWidget GtkFixed;
typedef GtkWidget GtkHandleBox;
typedef GtkWidget GtkLabel; /*TODO*/
typedef GtkWidget GtkButton; /*TODO*/


#include <gtk/gtkadjustment.h>
#include <gtk/gtkrange.h>
#include <gtk/gtksettings.h>
#include <gtk/gtktoolbar.h>

/* internal functions */
ULONG      xget(Object *obj, ULONG attr);
GtkWidget *mgtk_widget_new(int type);
int        mgtk_add_child(GtkWidget *parent, GtkWidget *child);
void       mgtk_initchange(GtkWidget *w);
void       mgtk_exitchange(GtkWidget *w);
void      *mgtk_malloc(int size);
void       mgtk_radio_button_muify(GtkWidget *button);
Object    *mgtk_get_mui_action_object(GtkWidget *widget);
void       mgtk_call_destroy_hooks(GtkWidget *widget);
GType      mgtk_g_type_register_fundamental(GType type_id, const gchar *type_name, GtkWidget  *(*newobject) (void));
mgtk_Fundamental *mgtk_g_type_get_fundamental(GType type_id);

/* gtk functions */
void       gtk_init(int *argc, char ***argv);
GtkWidget* gtk_window_new   (GtkWindowType   type);
void       gtk_window_set_title   (GtkWindow *window,  const gchar *title);
void       gtk_window_set_resizable        (GtkWindow *window,
                                             gboolean resizable);

gboolean   gtk_window_get_resizable(GtkWindow *window);
GtkWidget* gtk_button_new (void);
GtkWidget* gtk_button_new_with_label    (const char *label);
GtkWidget* gtk_button_new_from_stock       (const gchar *stock_id);
void       gtk_widget_show  (GtkWidget      *widget);
void       gtk_widget_hide  (GtkWidget *widget);
void       gtk_widget_show_all(GtkWidget *widget);
void       gtk_widget_hide_all(GtkWidget *widget);
void       gtk_widget_grab_default (GtkWidget *widget);
void       gtk_widget_set_sensitive(GtkWidget *widget, gboolean sensitive);
gint       gtk_widget_get_events           (GtkWidget *widget);
GdkVisual* gtk_widget_get_visual           (GtkWidget *widget);
GdkColormap* gtk_widget_get_colormap        (GtkWidget *widget);
GType      gtk_widget_get_type(void);
void       gtk_widget_queue_draw(GtkWidget *widget);
void       gtk_widget_style_get            (GtkWidget *widget,
                                             const gchar *first_property_name,
                                            ...);
void       gtk_widget_set_rc_style(GtkWidget *widget);
void       gtk_widget_set_style            (GtkWidget *widget,
                                             GtkStyle *style);
void       gtk_widget_realize              (GtkWidget *widget);
gpointer   gtk_type_class                  (GtkType type);
#define    GTK_TYPE_WIDGET (gtk_widget_get_type ())
void       gtk_grab_add                    (GtkWidget *widget);
void       gtk_grab_remove                 (GtkWidget *widget);
GtkWidget* gtk_grab_get_current            (void);
void       gtk_main         (void);
void       gtk_main_quit    (void);
void       gtk_widget_destroy( GtkWidget *widget/*, GtkWidget *param*/ );
void       gtk_widget_unref(GtkWidget *widget);
GtkWidget *gtk_widget_ref(GtkWidget *widget);
void       gtk_widget_set_size_request    (GtkWidget           *widget,
                                           gint                 width,
                                            gint                 height);
void       gtk_widget_set_usize           (GtkWidget *widget,
                                            gint width,
                                            gint height);
void       gtk_widget_set_state            (GtkWidget *widget,
                                             GtkStateType state);
void       gtk_container_add(GtkContainer  *container, GtkWidget *widget);
void       gtk_container_set_border_width (GtkContainer *container, guint border_width); 
GtkWidget *gtk_hbox_new     (gint homogeneous, gint spacing );
GtkWidget *mgtk_hbox_new_noparams();
GtkWidget *gtk_vbox_new     (gint homogeneous, gint spacing );
GType      gtk_vbox_get_type (void);
void       gtk_box_pack_start( GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding );
void       gtk_box_pack_end (GtkBox *box, GtkWidget *child, gint expand, gint fill, gint padding );

GtkWidget *gtk_menu_new      (void);
GtkWidget *gtk_menu_bar_new      (void);
GtkWidget *gtk_option_menu_new             (void);
GtkWidget *gtk_menu_item_new_with_label (const char *label );
GtkWidget *gtk_menu_item_new ();
void       gtk_menu_shell_append        (GtkMenuShell *menu_shell, GtkWidget *child);
void       gtk_menu_item_set_submenu    (GtkMenuItem         *menu_item,
                                         GtkWidget           *submenu);
GtkWidget *gtk_menu_get_attach_widget   (GtkMenu *menu);
void       gtk_option_menu_set_menu(GtkOptionMenu *option_menu, GtkWidget *menu);


GtkWidget *gtk_table_new(guint rows, guint columns, gboolean homogeneous);
void       gtk_table_attach_defaults  (GtkTable        *table,
                                       GtkWidget       *widget,
				       guint            left_attach,
				       guint            right_attach,
				       guint            top_attach,
				       guint            bottom_attach);
void      gtk_table_attach( GtkTable         *table,
				      GtkWidget        *child,
				      guint            left_attach,
				      guint            right_attach,
				      guint            top_attach,
				      guint            bottom_attach,
				      GtkAttachOptions xoptions,
				      GtkAttachOptions yoptions,
				      guint            xpadding,
				      guint            ypadding );
void       gtk_table_set_row_spacings      (GtkTable *table,
                                              guint spacing);
void       gtk_table_set_col_spacings      (GtkTable *table,
                                              guint spacing);
GtkWidget *gtk_entry_new( void );
void       gtk_entry_set_text( GtkEntry    *entry,
                                              const gchar *text );
const gchar *gtk_entry_get_text( GtkEntry *entry );
void gtk_entry_set_visibility( GtkEntry *entry,
                                              gboolean  visible );
void        gtk_entry_set_max_length        (GtkEntry *entry,
                                              gint max);
void gtk_editable_select_region( GtkEditable *entry,
                                              gint         start,
                                              gint         end );
void        gtk_editable_insert_text        (GtkEditable *editable,
                                              const gchar *new_text,
                                              gint new_text_length,
                                              gint *position);

GtkWidget *gtk_toggle_button_new( void );
GtkWidget *gtk_toggle_button_new_with_label( const gchar *label );
GtkWidget *gtk_toggle_button_new_with_mnemonic( const gchar *label );
gboolean   gtk_toggle_button_get_active	(GtkToggleButton *toggle_button);
void       gtk_toggle_button_set_active( GtkToggleButton *toggle_button,
                                         gboolean        is_active );

GtkWidget* gtk_check_button_new               (void);
GtkWidget* gtk_check_button_new_with_label    (const gchar *label);
GtkWidget* gtk_check_button_new_with_mnemonic (const gchar *label);
void       gtk_editable_set_editable         (GtkEditable *editable,
                                            gboolean     is_editable);
gboolean    gtk_editable_get_editable         (GtkEditable *editable);
gchar*      gtk_editable_get_chars (GtkEditable *editable,
																		gint start_pos,
																		gint end_pos);

GtkWidget*  gtk_image_new                   (void);
GtkWidget*  gtk_image_new_from_file         (const gchar *filename);
GSList     *gtk_radio_button_group(GtkWidget *widget);
GSList     *gtk_radio_button_get_group(GtkWidget *widget);
GtkWidget  *gtk_radio_button_new_with_label_from_widget( GtkRadioButton *group, const gchar    *label );
GtkWidget  *gtk_radio_button_new_with_label( GSList *group, const gchar  *label );
GtkWidget  *gtk_arrow_new(GtkArrowType arrow_type, GtkShadowType shadow_type);
GtkWidget  *gtk_hseparator_new(void);
GtkWidget  *gtk_hscrollbar_new(GtkAdjustment *adjustment);
GtkWidget  *gtk_hscale_new( GtkAdjustment *adjustment );
GtkWidget  *gtk_vscale_new( GtkAdjustment *adjustment );
void        gtk_scale_set_digits            (GtkScale *scale,
                                             gint digits);
void        gtk_scale_set_value_pos(GtkScale *scale, GtkPositionType pos);
GtkAdjustment *gtk_adjustment_new(gdouble value, 
																	gdouble lower, 
																	gdouble upper, 
																	gdouble step_increment, 
																	gdouble page_increment, 
																	gdouble page_size );
void        gtk_adjustment_set_value(GtkAdjustment *adjustment, gdouble value);
void        gtk_adjustment_changed          (GtkAdjustment *adjustment);
void        gtk_range_set_update_policy     (GtkRange *range, GtkUpdateType policy);
GtkTooltips* gtk_tooltips_new(void);
void        gtk_tooltips_set_tip(GtkTooltips *tooltips, 
																	GtkWidget *widget, 
																	const gchar *tip_text, 
																	const gchar *tip_private);
void        gtk_tooltips_disable(GtkTooltips *tooltips);
void        gtk_tooltips_enable(GtkTooltips *tooltips);
void        gtk_tooltips_force_window(GtkTooltips *tooltips);
GtkWidget*  gtk_file_selection_new          (const gchar *title);
void        gtk_file_selection_set_filename (GtkFileSelection *filesel,
                                             const gchar *filename);
const gchar* gtk_file_selection_get_filename (GtkFileSelection *filesel);

GtkWidget*  gtk_frame_new                   (const gchar *label);
void        gtk_frame_set_label             (GtkFrame *frame,
                                             const gchar *label);
void        gtk_frame_set_label_align       (GtkFrame *frame,
                                             gfloat xalign,
																						 gfloat yalign);
void        gtk_frame_set_shadow_type       (GtkFrame *frame,
                                             GtkShadowType type);
GtkWidget*  gtk_spin_button_new             (GtkAdjustment *adjustment,
                                              gdouble climb_rate,
                                              guint digits);
gint        gtk_spin_button_get_value_as_int
                                            (GtkSpinButton *spin_button);
void        gtk_spin_button_set_digits      (GtkSpinButton *spin_button,
                                              guint digits);
gdouble     gtk_spin_button_get_value       (GtkSpinButton *spin_button);
gdouble     gtk_spin_button_get_value_as_float(GtkSpinButton *spin_button);
void        gtk_spin_button_set_wrap        (GtkSpinButton *spin_button,
                                             gboolean wrap);
void        gtk_spin_button_set_numeric     (GtkSpinButton *spin_button,
                                             gboolean numeric);
GtkWidget*  gtk_label_new                   (const char *str);
void        gtk_label_set_text              (GtkLabel *label,
                                             const char *str);
GtkWidget*  gtk_notebook_new                (void);
gint        gtk_notebook_append_page        (GtkNotebook *notebook,
                                             GtkWidget *child,
                                             GtkWidget *tab_label);
gint        gtk_notebook_prepend_page       (GtkNotebook *notebook,
                                             GtkWidget *child,
                                             GtkWidget *tab_label);
void        gtk_notebook_next_page          (GtkNotebook *notebook);
void        gtk_notebook_prev_page          (GtkNotebook *notebook);
GtkWidget*  gtk_progress_bar_new            (void);
const gchar* gtk_progress_bar_get_text      (GtkProgressBar *pbar);
void        gtk_progress_bar_set_text       (GtkProgressBar *pbar,
                                             const gchar *text);
gdouble     gtk_progress_bar_get_fraction   (GtkProgressBar *pbar);
GtkProgressBarOrientation gtk_progress_bar_get_orientation
                                            (GtkProgressBar *pbar);
void        gtk_progress_bar_pulse          (GtkProgressBar *pbar);
void        gtk_progress_bar_set_fraction   (GtkProgressBar *pbar,
                                             gdouble fraction);
void        gtk_progress_bar_set_orientation
                                            (GtkProgressBar *pbar,
                                             GtkProgressBarOrientation orientation);

guint       g_timeout_add                   (guint interval, GSourceFunc function, gpointer data);
guint       gtk_timeout_add                 (guint32 interval, GtkFunction function, gpointer data);
void        gtk_timeout_remove              (guint timeout_handler_id);
gboolean    g_source_remove                 (guint tag);
GtkWidget*  gtk_fixed_new                   (void);
void        gtk_fixed_put                   (GtkFixed *fixed,
                                             GtkWidget *widget,
                                              gint x,
                                              gint y);
void        gtk_fixed_move                  (GtkFixed *fixed,
                                             GtkWidget *widget,
                                              gint x,
                                              gint y);
gpointer    gtk_type_class                  (GtkType type);

GtkStyle*   gtk_style_new                   (void);
GtkStyle*   gtk_style_attach                (GtkStyle *style,
                                             GdkWindow *window);
void        gtk_style_set_background        (GtkStyle *style,
                                             GdkWindow *window,
                                              GtkStateType state_type);
GtkStyle*   gtk_rc_get_style_by_paths       (GtkSettings *settings,
                                              const char *widget_path,
                                              const char *class_path,
                                              GType type);
void        mgtk_update_default_style       (APTR obj);
GtkStyle*   mgtk_get_default_style          (Object *obj);
void        gtk_paint_polygon               (GtkStyle *style,
                                              GdkWindow *window,
                                              GtkStateType state_type,
                                              GtkShadowType shadow_type,
                                              GdkRectangle *area,
                                              GtkWidget *widget,
                                              const gchar *detail,
                                              GdkPoint *points,
                                              gint npoints,
                                              gboolean fill);
void        gtk_paint_box                   (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_arrow                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             GtkArrowType arrow_type,
                                             gboolean fill,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_shadow                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_check                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_extension             (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height,
                                             GtkPositionType gap_side);
void        gtk_paint_flat_box              (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_focus                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_option                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height);
void        gtk_paint_slider                (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GtkShadowType shadow_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x,
                                             gint y,
                                             gint width,
                                             gint height,
                                             GtkOrientation orientation);
void        gtk_paint_vline                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint y1_,
                                             gint y2_,
                                             gint x);
void        gtk_paint_hline                 (GtkStyle *style,
                                             GdkWindow *window,
                                             GtkStateType state_type,
                                             GdkRectangle *area,
                                             GtkWidget *widget,
                                             const gchar *detail,
                                             gint x1,
                                             gint x2,
                                             gint y);
GtkWidget*  gtk_handle_box_new              (void);
GtkWidget *mgtk_custom_new();

#define GTK_OBJECT_CLASS(n)     ((GtkObjectClass *) n)
#define GTK_HANDLE_BOX(n)       ((GtkHandleBox *) n)
#define GTK_SIGNAL_FUNC(f)      ((GtkSignalFunc) (f))

GtkObject         *GTK_OBJECT(APTR widget);
GtkContainer      *GTK_CONTAINER(GtkWidget *widget);
GtkWindow         *GTK_WINDOW(GtkWidget *widget);
GtkBox            *GTK_BOX(GtkWidget *widget);
/*GtkWidget         *GTK_WIDGET(APTR widget);*/
GtkMenu           *GTK_MENU(GtkWidget *widget);
GtkMenuShell      *GTK_MENU_SHELL(GtkWidget *widget);
GtkMenuItem       *GTK_MENU_ITEM(GtkWidget *widget);
GtkTable          *GTK_TABLE(GtkWidget *widget);
GtkEditable       *GTK_EDITABLE(GtkWidget *widget);
GtkEntry          *GTK_ENTRY(GtkWidget *widget);
GtkToggleButton   *GTK_TOGGLE_BUTTON(GtkWidget *widget);
GtkRadioButton    *GTK_RADIO_BUTTON(GtkWidget *widget);
GtkScale          *GTK_SCALE(GtkWidget *widget);
GtkOptionMenu     *GTK_OPTION_MENU(GtkWidget *widget);
//GtkRange          *GTK_RANGE(GtkWidget *widget);
GtkTooltips       *GTK_TOOLTIPS(GtkWidget* widget);
GtkFileSelection  *GTK_FILE_SELECTION(GtkWidget* widget);
GtkFrame          *GTK_FRAME(GtkWidget* widget);
GtkSpinButton     *GTK_SPIN_BUTTON(GtkWidget* widget);
GtkLabel          *GTK_LABEL(GtkWidget* widget);
GtkNotebook       *GTK_NOTEBOOK(GtkWidget* widget);
GtkProgressBar    *GTK_PROGRESS_BAR(GtkWidget* widget);
GtkFixed          *GTK_FIXED(GtkWidget* widget);

void          GTK_WIDGET_SET_FLAGS (GtkWidget *widget, gint flags);

int           GTK_IS_MENU(GtkWidget        *widget);

/*#define GTK_ENTRY(a) a*/

/*
void GLADE_HOOKUP_OBJECT(GtkWidget *a,GtkWidget *b,gchar *foo);
void GLADE_HOOKUP_OBJECT_NO_REF(GtkWidget *a,GtkWidget *b,gchar *foo);
*/

#define GTK_CHECK_VERSION(major,minor,micro) 1
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)  ((gint)   (p))

/* GLIB */
#define g_warning printf

gulong g_signal_connect_data(gpointer           instance,
                             const gchar       *detailed_signal,
                             GCallback          c_handler,
                             gpointer           data,
                             GClosureNotify     destroy_data,
                             GConnectFlags      connect_flags);

guint g_signal_handlers_disconnect_by_func(gpointer instance, GCallback func, gpointer data);

void g_object_set_data(GObject *object,
			      const gchar *key,
			      gpointer data);

void g_object_set_data_full(GObject *object,
			      const gchar *key,
			      gpointer data,
			      GDestroyNotify destroy);

gpointer    g_object_get_data(GObject *object,
			      const gchar *key);
GObject*    g_object_ref                    (GObject *object);
void        g_object_unref (GObject *object);

void g_free                   (gpointer mem);
void g_error_free(GError **error);
gchar*      g_build_filename                (const gchar *first_element, va_list args);

/*
typedef struct _GdkPixbuf GdkPixbuf;
struct GdkPixbuf
{
  int  priv;
};
*/


#include <gdk/gdkevents.h>

#include <gtk/gtkstock.h>
#include <glib/gslist.h>
#include <glib/gstrfuncs.h>
#include <glib/gobject/gobject.h>

GdkPixbuf*  gdk_pixbuf_new_from_file(const char *filename, GError **error);

/* gnome */
#define PACKAGE "GTK-MUI"
#define VERSION "0.0666"
#define LIBGNOMEUI_MODULE NULL
#define GNOME_PARAM_APP_DATADIR "PROGDIR:"
#define GNOME_FILE_DOMAIN_APP_PIXMAP "PROGDIR:"
#define PACKAGE_DATA_DIR "PROGDIR"

typedef struct {
  const char *name;
  const char *version;
  const char *description;
/*  GnomeModuleRequirement *requirements;

  GnomeModuleHook instance_init;
  GnomeModuleHook pre_args_parse, post_args_parse;

  struct poptOption *options;
  
  GnomeModuleInitHook init_pass;
  
  GnomeModuleClassInitHook class_init;*/
  
  const char *opt_prefix;
  gpointer    expansion1;
} GnomeModuleInfo;

/*
void gnome_program_init            (const char *app_id,
                                             const char *app_version,
					      const GnomeModuleInfo *module_info,
					      int argc,
					      char **argv,
					      const char *first_property_name,
					      ...);
*/

/* maybe wrong parameters..*/
#define gnome_program_init(a,b,c,argc,argv,f,g,h) gtk_init(&argc,&argv)

/*ATK*/
typedef struct _AtkAction AtkAction;
typedef struct {
  int badmagic;
} _AtkAction;


/* mui */
#ifndef __AROS__
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#include <gdk/gobject/gvalue.h>
#include <gdk/gobject/gvaluecollector.h>
#include <gdk/gobject/gtype.h>

gpointer    g_object_new                    (GType object_type,
                                             const gchar *first_property_name,
                                              ...);
GType       g_type_register_static          (GType parent_type,
                                             const gchar *type_name,
                                              const GTypeInfo *info,
                                              GTypeFlags flags);
void        g_signal_emit                    (gpointer instance, 
                                              guint signal_id, 
                                              GQuark detail, 
                                              ...);
void        g_signal_emit_by_name           (gpointer instance,
                                             const gchar *detailed_signal,
                                              ...);
#define     g_signal_handlers_unblock_by_func(instance, func, data) /*?*/
#define     g_signal_handlers_block_by_func(instance, func, data)
typedef void  (*GClosureMarshal)  (GClosure *closure,
                                    GValue         *return_value,
                                    guint           n_param_values,
                                    const GValue   *param_values,
                                    gpointer        invocation_hint,
                                    gpointer  marshal_data);

typedef GClosureMarshal      GSignalCMarshaller;

typedef struct _GSignalInvocationHint  GSignalInvocationHint;

struct _GSignalInvocationHint
{
  int foo; /* TODO */
};


typedef gboolean (*GSignalAccumulator)  (GSignalInvocationHint *ihint,
                      GValue          *return_accu,
                      const GValue        *handler_return,
                      gpointer               data);

guint       g_signal_new                     (const gchar *signal_name, 
                                              GType itype, 
                                              GSignalFlags signal_flags, guint class_offset, 
                                              GSignalAccumulator accumulator, 
                                              gpointer accu_data, 
                                              GSignalCMarshaller c_marshaller, 
                                              GType return_type, 
                                              guint n_params, 
                                              ...);
guint       g_signal_lookup                 (const gchar *name,
                                             GType itype);

#endif /*MGTK_HAVE_GTK_H */
