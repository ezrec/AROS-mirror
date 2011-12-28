// MDF - PORT FROM NCSA VERSION 2.1

/****************************************************************************
 * NCSA Mosaic for the X Window System					    *
 * Software Development Group						    *
 * National Center for Supercomputing Applications			    *
 * University of Illinois at Urbana-Champaign				    *
 * 605 E. Springfield, Champaign IL 61820				    *
 * mosaic@ncsa.uiuc.edu 						    *
 *									    *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois	    *
 *									    *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois	    *
 * (UI), and ownership remains with the UI.				    *
 *									    *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such	    *
 * copies.								    *
 *									    *
 * Licensee may make derivative works.	However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then	    *
 * Licensee will (1) notify NCSA regarding its distribution of the	    *
 * derivative work, and (2) clearly notify users that such derivative	    *
 * work is a modified version and not the original NCSA Mosaic		    *
 * distributed by the UI.						    *
 *									    *
 * Any Licensee wishing to make commercial use of the Software should	    *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or	    *
 * part of the source code into a product for sale or license by or on	    *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a	    *
 * commercial product sold or licensed by or on behalf of Licensee.	    *
 *									    *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.						    *
 *									    *
 * By using or copying this Software, Licensee agrees to abide by the	    *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.	    *
 * UI shall have the right to terminate this license immediately by	    *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any	    *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.			    *
 *									    *
 * Comments and questions are welcome and can be sent to		    *
 * mosaic-x@ncsa.uiuc.edu.						    *
 ****************************************************************************/

#ifndef __MOSAIC_H__
#define __MOSAIC_H__

/* --------------------------- SYSTEM INCLUDES ---------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if !defined(VMS) && !defined(NeXT) && !defined(_AMIGA) && !defined(__AROS__)
#include <unistd.h>
#endif
#include <stdlib.h>
/* #include <sys/types.h> */

#ifdef __sgi
#include <malloc.h>
#endif

#if !defined(_AMIGA) && !defined(__AROS__)
#include "Xmx.h"
#endif

#if defined(_AMIGA) || defined(__AROS__)
#include "XtoI.h"
typedef void *XmxMenuRecord;
#endif

/* ------------------------------------------------------------------------ */
/* -------------------------------- ICONS --------------------------------- */
/* ------------------------------------------------------------------------ */
#define NUMBER_OF_FRAMES	25

/* ------------------------------------------------------------------------ */
/* -------------------------------- MACROS -------------------------------- */
/* ------------------------------------------------------------------------ */
#if !defined(_AMIGA) && !defined(__AROS__)
#define MO_VERSION_STRING " FROM NCSA Mosaic V2.1 and 2.4 NOT FOR RELEASE"
#else
extern char *amosaic_version;
extern char *amosaic_window_title;
extern char *amosaic_about_version;
extern char *mo_version_string;
extern char *HTAppVersion;
#endif

#define MO_HELP_ON_VERSION_DOCUMENT \
  mo_assemble_help_url ("help-on-version-2.1.html")
#define MO_DEVELOPER_ADDRESS "amosaic@insti.physics.sunysb.edu"
#define MO_INSTRUMENTATION_ADDRESS "amosaic@insti.physics.sunysb.edu"

#ifndef DOCS_DIRECTORY_DEFAULT
#define DOCS_DIRECTORY_DEFAULT \
  "http://www.omnipresence.com/amosaic/2.0/"
#endif

#ifndef HOME_PAGE_DEFAULT
/* This must be a straight string as it is included into a struct;
   no tricks. */
#define HOME_PAGE_DEFAULT \
  "http://www.omnipresence.com/amosaic/2.0/"
#endif /* not HOME_PAGE_DEFAULT */

#ifndef WHATSNEW_PAGE_DEFAULT
#define WHATSNEW_PAGE_DEFAULT \
  mo_assemble_help_url ("whats-new.html")
#endif /* not WHATSNEW_PAGE_DEFAULT */

#ifndef DEMO_PAGE_DEFAULT
#define DEMO_PAGE_DEFAULT \
  "http://www.ncsa.uiuc.edu/demoweb/demo.html"
#endif /* not DEMO_PAGE_DEFAULT */

#ifndef HTMLPRIMER_PAGE_DEFAULT
#define HTMLPRIMER_PAGE_DEFAULT \
  "http://www.ncsa.uiuc.edu/General/Internet/WWW/HTMLPrimer.html"
#endif /* not HTMLPRIMER_PAGE_DEFAULT */

#ifndef URLPRIMER_PAGE_DEFAULT
#define URLPRIMER_PAGE_DEFAULT \
  "http://www.ncsa.uiuc.edu/demoweb/url-primer.html"
#endif /* not URLPRIMER_PAGE_DEFAULT */

#ifndef NETWORK_STARTING_POINTS_DEFAULT
#define NETWORK_STARTING_POINTS_DEFAULT \
  "http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/StartingPoints/NetworkStartingPoints.html"
#endif /* not NETWORK_STARTING_POINTS_DEFAULT */

#ifndef INTERNET_METAINDEX_DEFAULT
#define INTERNET_METAINDEX_DEFAULT \
  "http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/MetaIndex.html"
#endif /* not INTERNET_METAINDEX_DEFAULT */

#ifndef DOCUMENTS_MENU_SPECFILE
#define DOCUMENTS_MENU_SPECFILE \
  "s:documents.menu"
#endif /* not DOCUMENTS_MENU_SPECFILE */

#ifndef GLOBAL_EXTENSION_MAP
#define GLOBAL_EXTENSION_MAP "/usr/local/lib/mosaic/mime.types"
#endif
#ifndef GLOBAL_TYPE_MAP
#define GLOBAL_TYPE_MAP "/usr/local/lib/mosaic/mailcap"
#endif

#if defined(__hpux)
#define MO_MACHINE_TYPE "HP-UX"
#endif
#if defined(__sgi)
#define MO_MACHINE_TYPE "Silicon Graphics"
#endif
#if defined(ultrix)
#define MO_MACHINE_TYPE "DEC Ultrix"
#endif
#if defined(_IBMR2)
#define MO_MACHINE_TYPE "RS/6000 AIX"
#endif
#if defined(sun)
#define MO_MACHINE_TYPE "Sun"
#endif
#if defined(__alpha)
#define MO_MACHINE_TYPE "DEC Alpha"
#endif
#if defined(NEXT)
#define MO_MACHINE_TYPE "NeXT BSD"
#endif
#if defined(cray)
#define MO_MACHINE_TYPE "Cray"
#endif
#if defined(VMS)
#define MO_MACHINE_TYPE "VMS"
#endif
#if defined(NeXT)
#define MO_MACHINE_TYPE "NeXT"
#endif
#if defined(_AMIGA)
#define MO_MACHINE_TYPE "Amiga"
#endif
#if defined(__AROS__)
#undef  MO_MACHINE_TYPE
#define MO_MACHINE_TYPE "AROS"
#endif
#ifndef MO_MACHINE_TYPE
#define MO_MACHINE_TYPE "Unknown Platform"
#endif

#ifdef __hpux
#define HAVE_AUDIO_ANNOTATIONS
#else
#if defined(__sgi) || defined(sun)
#define HAVE_AUDIO_ANNOTATIONS
#endif /* if */
#endif /* ifdef */

#ifdef __hpux
#define MO_SIGHANDLER_RETURNTYPE int
#else /* not __hpux */
#define MO_SIGHANDLER_RETURNTYPE void
#endif

/* Be safe... some URL's get very long. */
#define MO_LINE_LENGTH 2048

#define MO_MAX(x,y) ((x) > (y) ? (x) : (y))
#define MO_MIN(x,y) ((x) > (y) ? (y) : (x))

/* Use builtin strdup when appropriate -- code duplicated in tcp.h. */
#if defined(ultrix) || defined(VMS) || defined(NeXT)
extern char *strdup ();
#endif

#if defined(SYSV) || defined(SVR4) || defined(__svr4__) || defined(VMS) || (defined(_AMIGA) && defined (_STRICT_ANSI))
#define bcopy(source, dest, count) memcpy(dest, source, count)
#define bzero(b, len) memset(b, 0, len)
#endif

#define public
#define private static

/* ------------------------------------------------------------------------ */
/* ------------------------------ MAIN TYPES ------------------------------ */
/* ------------------------------------------------------------------------ */

/* ------------------------------ mo_window ------------------------------- */

/* mo_window contains everything related to a single Document View
   window, including subwindow details. */
typedef struct mo_window
{
  int id;
  Widget base;

  /* Subwindows. */
  Object *source_win;
  Widget save_win;
  Widget savebinary_win;  /* for binary transfer mode */
  Object *open_win;
  Widget mail_win;
  Widget mailhot_win;
  Widget edithot_win;
  Widget mailhist_win;
  Widget print_win;
  Object *history_win;
  Widget open_local_win;
  Widget hotlist_win;
  Widget whine_win;
  Widget annotate_win;
  Widget search_win;	     /* internal document search */
  Widget searchindex_win;    /* network index search */
#ifdef HAVE_DTM
  Widget dtmout_win;
#endif
#ifdef HAVE_AUDIO_ANNOTATIONS
  Widget audio_annotate_win;
#endif

  XmxMenuRecord *menubar;
  Widget url_text;
  Widget title_text;

  Widget searchindex_button;   /* pushbutton, says "Search Index" */
  Widget searchindex_win_label, searchindex_win_text;
  Widget searchindex_win_searchbut;

  HTMLGadClData *scrolled_win;	/* The data structure for the html gadget */
  Object *HTML_Gad;				/* The actual gadget. Do not use it if you don't have to */
  Object *view;
  Widget bottom_form;
  Widget tracker_label, logo;
  Widget button_rc, back_button, forward_button, save_button,
    clone_button, close_button, reload_button, open_button,
    new_button;

  Widget home_button;

  int last_width;

  struct mo_node *history;
  struct mo_node *current_node;
  int reloading;

  char *target_anchor;

  /* Document source window. */
  Object *source_text;

  int save_format; /* starts at 0 */
  Object *format_optmenu;
  Object *save_fname;
  Object *save_requester;

  Object *open_text;

  /* Macro window */
  Object *macro_win ;
  Object *macro_req ;
  Object *macro_string ;

  Widget mail_to_text;
  Widget mail_subj_text;
  XmxMenuRecord *mail_fmtmenu;
  int mail_format;

  Widget mailhot_to_text;
  Widget mailhot_subj_text;
  Widget mailhist_to_text;
  Widget mailhist_subj_text;

  Widget edithot_text;
  int edithot_pos;

  Widget print_text;
  XmxMenuRecord *print_fmtmenu;
  int print_format;

  Widget history_list;

  Widget hotlist_list;

  Widget whine_text;

  int font_size;
  int pretty;

  int underlines_snarfed;
  int underlines_state;
  /* Default values only, mind you. */
  int underlines;
  int visited_underlines;
  Boolean dashed_underlines;
  Boolean dashed_visited_underlines;

#ifdef HAVE_DTM
  Widget dtmout_text;
#endif /* HAVE_DTM */

#ifdef HAVE_AUDIO_ANNOTATIONS
  Widget audio_start_button;
  Widget audio_stop_button;
  pid_t record_pid;
  char *record_fnam;
#endif

  Widget annotate_author;
  Widget annotate_title;
  Widget annotate_text;
  Widget delete_button;
  Widget include_fsb;
  int annotation_mode;
  int editing_id;

  char *cached_url;

  Widget search_win_text;
  Widget search_caseless_toggle;
  Widget search_backwards_toggle;
  void *search_start;
  void *search_end;

  int binary_transfer;
  int delay_image_loads;

  struct mo_window *next;

#ifdef GRPAN_PASSWD
  Widget passwd_label;
  Widget annotate_passwd;
  Widget passwd_toggle;
#endif
  XmxMenuRecord *pubpri_menu;
  int pubpri;  /* one of mo_annotation_[public,private] */
  XmxMenuRecord *audio_pubpri_menu;
  int audio_pubpri;  /* one of mo_annotation_[public,private] */
#ifdef NOPE_NOPE_NOPE
  XmxMenuRecord *title_menu;
  int title_opt;  /* mo_document_title or mo_document_url */
  Widget annotate_toggle;
  Widget crossref_toggle;
  Widget checkout_toggle;
  Widget checkin_toggle;
#endif
} mo_window;

/* ------------------------------- mo_node -------------------------------- */

/* mo_node is a component of the linear history list.  A single
   mo_node will never be effective across multiple mo_window's;
   each window has its own linear history list. */
typedef struct mo_node
{
  char *title;
  char *url;
  char *ref;  /* how the node was referred to from a previous anchor,
		 if such an anchor existed. */
  char *text;
  char *texthead;   /* head of the alloc'd text -- this should
		       be freed, NOT text */
  /* Position in the list, starting at 1; last item is
     effectively 0 (according to the XmList widget). */
  int position;

  /* The type of annotation this is (if any) */
  int annotation_type;

  /* This is returned from HTMLPositionToId. */
  int docid;

  /* This is returned from HTMLGetWidgetInfo. */
  void *cached_stuff;

  struct mo_node *previous;
  struct mo_node *next;
} mo_node;

/* ------------------------------ mo_hotnode ------------------------------ */

/* mo_hotnode is a single item in a mo_hotlist. */
typedef struct mo_hotnode
{
  char *title; /* cached */
  char *url;
  char *lastdate;
  /* Position in the list; starting at 1... */
  int position;

  struct mo_hotnode *previous;
  struct mo_hotnode *next;
} mo_hotnode;

/* ------------------------------ mo_hotlist ------------------------------ */

/* mo_hotlist is a list of URL's and (cached) titles that can be
   added to and deleted from freely, and stored and maintained across
   sessions. */
typedef struct mo_hotlist
{
  mo_hotnode *nodelist;
  /* Point to last element in nodelist for fast appends. */
  mo_hotnode *nodelist_last;

  /* Filename for storing this hotlist to local disk; example is
     $HOME/.mosaic-hotlist-default. */
  char *filename;

  /* Name for this hotlist.  This is plain english. */
  char *name;

  /* Flag set to indicate whether this hotlist has to be written
     back out to disk at some point. */
  int modified;

  /* In case we want to string together multiple hotlists... */
  struct mo_hotlist *next;
} mo_hotlist;

/* ------------------------------------------------------------------------ */
/* ------------------------------ MISC TYPES ------------------------------ */
/* ------------------------------------------------------------------------ */

typedef enum
{
  mo_fail = 0, mo_succeed
} mo_status;

typedef enum
{
  mo_annotation_public = 0, mo_annotation_workgroup, mo_annotation_private
} mo_pubpri_token;

typedef struct
{
  int default_width;
  int default_height;
  char *home_document;
  char *sendmail_command;
  char *xterm_command;
  char *global_history_file;
  char *default_hotlist_file;
  char *private_annotation_directory;
  char *annotation_server;
  char *default_author_name;
  char *download_dir;

  int colors_per_inlined_image;

  char *uncompress_command;
  char *gunzip_command;

  char *record_command_location;
  char *record_command;

  char *tmp_directory;

  char *full_hostname;

  int hdf_max_image_dimension;
  int hdf_max_displayed_datasets;
  int hdf_max_displayed_attributes;

  char *docs_directory;

  char *documents_menu_specfile;

  int max_wais_responses;

  char *global_extension_map;
  char *personal_extension_map;
  char *global_type_map;
  char *personal_type_map;

  int twirl_increment;

  int image_cache_size;

  Boolean use_global_history;
  Boolean display_urls_not_titles;
  Boolean confirm_exit;
  Boolean default_fancy_selections;
  Boolean annotations_on_top;
  Boolean track_visited_anchors;
  Boolean catch_prior_and_next;
  Boolean hdf_power_user;

  Boolean reload_reloads_images;
  Boolean simple_interface;
  Boolean larry_bitmaps;
  Boolean delay_image_loads;

  Boolean use_default_extension_map;
  Boolean use_default_type_map;
  Boolean twirling_transfer_icon;
  Boolean protect_me_from_myself;

  Boolean reverse_inlined_bitmap_colors;

  Boolean confirm_delete_annotation;
  Boolean tweak_gopher_types;

  /* If True, we can't call gethostbyname to find out who we are. */
  Boolean gethostbyname_is_evil;

  Boolean auto_place_windows;
  Boolean initial_window_iconic;

  Boolean track_pointer_motion;
  Boolean track_full_url_names;

  char *mail_filter_command;

  char *font, *italicfont, *boldfont, *h1font, *h2font, *h3font, *h4font;
  char *h5font, *h6font, *fixedfont, *addressfont, *listingfont, *plainfont;

	Boolean image_buttons;
	Boolean isindex_to_form;
	Boolean title_to_window;

	long u_anchor;
	long f_anchor;
	long a_anchor;

} AppData, *AppDataPtr;


/* ---------------------------- a few globals ----------------------------- */

extern AppData Rdata;
#if !defined(_AMIGA) && !defined(__AROS__)
extern Display *dsp;
#endif
/* ------------------------------- menubar -------------------------------- */

typedef enum
{
  mo_reload_document = 50, mo_reload_document_and_images,
  mo_refresh_document, mo_clear_image_cache,
  mo_document_source, mo_search,
  mo_open_document, mo_open_local_document, mo_save_document,
  mo_mail_document, mo_tex_document,
  mo_new_window, mo_clone_window,
  mo_close_window, mo_exit_program,
  mo_home_document, mo_ncsa_document,
  mo_mosaic_manual, mo_mosaic_demopage,
  mo_back, mo_forward, mo_history_list,
  mo_clear_global_history,
  mo_network_starting_points, mo_internet_metaindex, mo_search_index,
  mo_large_fonts, mo_regular_fonts, mo_small_fonts,
  mo_large_helvetica, mo_regular_helvetica, mo_small_helvetica,
  mo_large_newcentury, mo_regular_newcentury, mo_small_newcentury,
  mo_large_lucidabright, mo_regular_lucidabright, mo_small_lucidabright,
  mo_help_about, mo_help_onwindow, mo_help_onversion, mo_help_faq,
  mo_whine, mo_help_html, mo_help_url,
  mo_whats_new,
  mo_annotate,
#ifdef HAVE_AUDIO_ANNOTATIONS
  mo_audio_annotate,
#endif
  mo_annotate_edit, mo_annotate_delete,
  mo_checkout, mo_checkin,
  mo_fancy_selections,
  mo_default_underlines, mo_l1_underlines, mo_l2_underlines, mo_l3_underlines,
  mo_no_underlines, mo_binary_transfer,
  mo_re_init, mo_delay_image_loads, mo_expand_images_current, mo_about,
  mo_macro, mo_macro_1, mo_macro_2, mo_macro_3, mo_macro_4, mo_macro_5,
  mo_macro_6, mo_macro_7, mo_macro_8, mo_macro_9, mo_macro_10,
  mo_add_hot_list, mo_jump_hot_list, mo_print_document,
  mo_isindex_to_form, mo_form_notify, mo_image_buttons,
  mo_buttons_text,mo_buttons_image,mo_buttons_imagetext,
  mo_load_amigahome, mo_load_amosaichome,
  mo_url_to_clipboard,
  mo_show_form_images,
} mo_token;


/* ------------------------------------------------------------------------ */
/* ------------------------------ PROTOTYPES ------------------------------ */
/* ------------------------------------------------------------------------ */

/* annotate.c */
extern mo_status mo_post_annotate_win
  (mo_window *win, int, int, char *, char *, char *, char *);
extern char *mo_fetch_annotation_links (char *, int);
extern mo_status mo_is_editable_annotation (mo_window *, char *);
extern mo_status mo_delete_annotation (mo_window *, int);
extern mo_status mo_delete_group_annotation (mo_window *, char *);

#ifdef HAVE_AUDIO_ANNOTATIONS
/* audan.c */
extern mo_status mo_audio_capable (void);
extern mo_status mo_post_audio_annotate_win (mo_window *);
#endif

/* globalhist.c */
extern mo_status mo_been_here_before_huh_dad (char *);
extern mo_status mo_here_we_are_son (char *);
extern mo_status mo_init_global_history (void);
extern mo_status mo_wipe_global_history (mo_window *);
extern mo_status mo_setup_global_history (void);
extern mo_status mo_write_global_history (void);
extern void *mo_fetch_cached_image_data (char *);
extern void *mo_fetch_cached_local_name (char *);
extern mo_status mo_cache_data (char *, void *, int);
extern mo_status mo_zap_cached_images_here (mo_window *);
extern mo_status mo_flush_image_cache (mo_window *);
extern mo_status mo_set_image_cache_nuke_threshold (void);

/* grpan.c */
extern char *mo_fetch_grpan_links (char *url);
extern mo_status mo_is_editable_grpan (char *);
extern mo_status mo_audio_grpan (char *url, char *title, char *author,
			       char *data, int len);
extern mo_status mo_new_grpan (char *url, char *title, char *author,
			       char *text);
extern mo_status mo_modify_grpan (char *url, char *title, char *author,
			       char *text);
extern mo_status mo_delete_grpan (char *url);
extern mo_status mo_grok_grpan_pieces
  (char *, char *, char **, char **, char **, int *, char **);

/* grpan-www.c */
extern char *grpan_doit (char *, char *, char *, int, char **);

/* gui.c */
extern mo_window *mo_next_window (mo_window *);
extern mo_window *mo_fetch_window_by_id (int);
extern mo_status mo_add_window_to_list (mo_window *);
extern char *mo_assemble_help_url (char *);
extern mo_status mo_busy (void);
extern mo_status mo_not_busy (void);
extern mo_status mo_redisplay_window (mo_window *);
extern mo_status mo_set_current_cached_win (mo_window *);
extern mo_status mo_set_dtm_menubar_functions (mo_window *);
extern mo_status mo_delete_window (mo_window *);
extern mo_window *mo_open_window (Widget, char *, mo_window *);
extern mo_window *mo_duplicate_window (mo_window *);
extern mo_window *mo_open_another_window (mo_window *, char *, char *, char *);
extern mo_status mo_open_initial_window (void);
extern void mo_gui_notify_progress (char *);
extern int mo_gui_check_icon (int);
extern void mo_gui_clear_icon (void);
extern void mo_gui_done_with_icon (void);
#ifdef HAVE_DTM
extern mo_status mo_register_dtm_blip (void);
#endif

extern void mo_do_gui (int, char **);

/* gui-dialogs.c */
extern mo_status mo_post_save_window (mo_window *);
/* called from libwww */
extern void rename_binary_file (char *);
extern mo_status mo_post_open_local_window (mo_window *);
extern mo_status mo_post_open_window (mo_window *);
#ifdef HAVE_DTM
extern mo_status mo_send_document_over_dtm (mo_window *);
extern mo_status mo_post_dtmout_window (mo_window *);
#endif
extern mo_status mo_post_mail_window (mo_window *);
extern mo_status mo_post_print_window (mo_window *);
extern mo_status mo_post_source_window (mo_window *);
extern mo_status mo_post_search_window (mo_window *);

/* gui-documents.c */
extern mo_status mo_back_impossible (mo_window *win);
extern mo_status mo_forward_impossible (mo_window *win);
extern mo_status mo_set_win_current_node (mo_window *, mo_node *);
extern mo_status mo_reload_window_text (mo_window *, int);
extern mo_status mo_refresh_window_text (mo_window *);
extern mo_status mo_load_window_text (mo_window *, char *, char *);
extern mo_status mo_duplicate_window_text (mo_window *, mo_window *);
extern mo_status mo_access_document (mo_window *, char *);
//added by SB:
extern mo_status mo_post_access_document (mo_window *, char *,char *, char *);

/* gui-menubar.c */
extern mo_status mo_set_fancy_selections_toggle (mo_window *);
extern mo_status mo_set_fonts (mo_window *, int);
extern mo_status mo_set_underlines (mo_window *, int);
extern XmxMenuRecord *mo_make_document_view_menubar (Widget);
extern void do_menubar(mo_window *, int); /* MDF */

/* history.c */
extern mo_status mo_free_node_data (mo_node *);
extern mo_status mo_kill_node (mo_window *, mo_node *);
extern mo_status mo_kill_node_descendents (mo_window *, mo_node *);
extern mo_status mo_add_node_to_history (mo_window *, mo_node *);
extern char *mo_grok_title (mo_window *, char *, char *);
extern mo_status mo_record_visit (mo_window *, char *, char *,
				  char *, char *);
extern mo_status mo_back_node (mo_window *);
extern mo_status mo_forward_node (mo_window *);
extern mo_status mo_visit_position (mo_window *, int);
extern mo_status mo_dump_history (mo_window *);
extern mo_status mo_post_history_win (mo_window *);

/* hotlist.c */
extern mo_hotlist *mo_read_hotlist (char *);
extern mo_hotlist *mo_new_hotlist (char *, char *);
extern mo_status mo_write_hotlist (mo_hotlist *);
extern mo_status mo_dump_hotlist (mo_hotlist *);
extern mo_status mo_setup_default_hotlist (void);
extern mo_status mo_write_default_hotlist (void);
extern mo_status mo_post_hotlist_win (mo_window *);
extern mo_status mo_add_node_to_default_hotlist (mo_node *);

/* img.c */
extern mo_status mo_free_image_data (void *);
extern mo_status mo_register_image_resolution_function (mo_window *);

/* main.c */
extern void mo_exit (void);

/* mo-www.c */
extern char *mo_pull_er_over (char *, char **);
extern mo_status mo_pull_er_over_virgin (char *, char *);
extern char *mo_tmpnam (void);
extern char *mo_get_html_return (char **);
extern char *mo_convert_newlines_to_spaces (char *);
extern mo_status mo_re_init_formats (void);

extern char *mo_url_canonicalize (char *, char *);
extern char *mo_url_canonicalize_keep_anchor (char *, char *);
extern char *mo_url_canonicalize_local (char *);
extern char *mo_url_to_unique_document (char *);
extern char *mo_url_extract_anchor (char *);
extern char *mo_url_extract_access (char *, char *);

extern void application_user_feedback (char *);
extern char *mo_escape_part (char *);
extern char *mo_unescape_part (char *);

#ifdef HAVE_DTM
/* mo-dtm.c */
extern mo_status mo_dtm_in (char *);
extern mo_status mo_dtm_out (char *);
extern mo_status mo_dtm_out_active_p (void);
extern mo_status mo_dtm_poll_and_read (void);
extern mo_status mo_dtm_send_text (mo_window *, char *, char *);
extern mo_status mo_dtm_disconnect (void);
#endif

#ifdef HAVE_HDF
/* mo-hdf.c */
extern char *mo_hdf_fetch_local_filename (char *);
extern char *mo_decode_internal_reference (char *, char *, char *);
extern char *mo_decode_hdfref (char *url, char *target);
#ifdef HAVE_DTM
extern mo_status mo_do_hdf_dtm_thang (char *, char *);
#endif /* HAVE_DTM */
#endif /* HAVE_HDF */

/* pan.c */
extern mo_status mo_setup_pan_list (void);
extern mo_status mo_write_pan_list (void);
extern mo_status mo_new_pan (char *, char *, char *, char *);
extern char *mo_fetch_pan_links (char *url, int);
extern mo_status mo_delete_pan (int);
extern mo_status mo_modify_pan (int, char *, char *, char *);
extern mo_status mo_is_editable_pan (char *);
extern mo_status mo_grok_pan_pieces
  (char *, char *, char **, char **, char **, int *, char **);
extern int mo_next_pan_id (void);

/* picread.c */
// extern unsigned char *ReadBitmap (char *, int *, int *, XColor *, int *);

/* pixmaps.c */
// extern void AnimatePixmapInWidget(Widget, Pixmap);
// extern void MakeAnimationPixmaps(Widget);

/* whine.c */
extern mo_status mo_post_whine_win (mo_window *);
extern mo_status mo_send_mail_message (char *, char *, char *, char *, char *);
extern FILE *mo_start_sending_mail_message (char *, char *, char *, char *);
extern mo_status mo_finish_sending_mail_message (void);

/* ----------------------------- END OF FILE ------------------------------ */

extern void fire_er_up (int, char **);

#endif /* not __MOSAIC_H__ */
