// MDF - PORT FROM NCSA VERSION 2.1
// In progress.

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

#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"

#include <signal.h>
#include <sys/types.h>

/* don't think we need these - MDF 1/12/94
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
*/

#include "XtoI.h"
//#include "spinner.h"
#include "Boingtransfer.h"

/*
 * Globals used by the pixmaps for the animated icon.
 * Marc, I imagine you might want to do something cleaner
 * with these?
 */
extern Pixmap IconPix[NUMBER_OF_FRAMES];
extern int IconsMade;

/* ------------------------------ variables ------------------------------- */

Widget toplevel;
Widget view = NULL;  /* HORRIBLE HACK @@@@ */
int Vclass;  /* visual class for 24bit support hack */
AppData Rdata;	/* extern'd in mosaic.h */
char *global_xterm_str;  /* required for HTAccess.c now */

char *uncompress_program="gunzip";
char *gunzip_program="gunzip";

int use_default_extension_map = 1;
char *global_extension_map   ="env:mosaic/mime.types";
char *personal_extension_map ="env:mosaic/personal_mime.types";
int use_default_type_map = 1;
char *global_type_map	     ="env:mosaic/mailcap";
char *personal_type_map      ="env:mosaic/personal_mailcap";

int tweak_gopher_types;
int max_wais_responses;
int have_hdf = 0;
int twirl_increment;

/* This is exported to libwww, like altogether too many other
   variables here. */
int binary_transfer;
/* Now we cache the current window right before doing a binary
   transfer, too.  Sheesh, this is not pretty. */
mo_window *current_win;

/* If startup_document is set to anything but NULL, it will be the
   initial document viewed (this is separate from home_document
   below). */
char *startup_document = NULL;
/* If startup_document is NULL home_document will be the initial
   document. */
char *home_document = NULL;
char *machine;
char *shortmachine;
char *machine_with_domain;

XColor fg_color, bg_color;

static void *busy_cursor;
static int busy = 0;
static Widget *busylist = NULL;
char *cached_url = NULL;

/* Forward declaration of test predicate. */
int anchor_visited_predicate (Widget, char *);

/* When we first start the application, we call mo_startup()
   after creating the unmapped toplevel widget.  mo_startup()
   either sets the value of this to 1 or 0.  If 0, we don't
   open a starting window. */
int defer_initial_window;

extern char *HTDescribeURL (char *);
extern mo_status mo_post_access_document (mo_window *win, char *url,
					  char *content_type,
					  char *post_data);

/* ----------------------------- WINDOW LIST ------------------------------ */
/* Amiga Mosaic doesn't support multiple windows yet, so this is empty.     */
/* ----------------------------- WINDOW LIST ------------------------------ */


/****************************************************************************
 * name:    mo_assemble_help_url
 * purpose: Make a temporary, unique filename.
 * inputs:
 *   - char *file: Filename to be appended to Rdata.docs_directory.
 * returns:
 *   The desired help url (a malloc'd string).
 * remarks:
 ****************************************************************************/
char *mo_assemble_help_url (char *file)
{
  char *tmp;

  if (!file)
    return strdup ("http://lose.lose/lose");

  tmp = (char *)malloc ((strlen (file) + strlen (Rdata.docs_directory) + 4) *
			sizeof (char));

  if (Rdata.docs_directory[strlen(Rdata.docs_directory)-1] == '/')
    {
      /* Trailing slash in docs_directory spec. */
      sprintf (tmp, "%s%s", Rdata.docs_directory, file);
    }
  else
    {
      /* No trailing slash. */
      sprintf (tmp, "%s/%s", Rdata.docs_directory, file);
    }

  return tmp;
}


/* ----------------------------- busy cursor ------------------------------ */

mo_status mo_not_busy (void)
{
  if (busy)
    {
//	set(App, MUIA_Application_Sleep, FALSE);
      busy = 0;
    }
  return mo_succeed;
}

mo_status mo_busy (void)
{
  if (!busy)
    {
//	set(App, MUIA_Application_Sleep, TRUE);
      busy = 1;
    }
  return mo_succeed;
}


/****************************************************************************
 * name:    mo_redisplay_window
 * purpose: Cause the current window's HTML widget to be refreshed.
 *	    This causes the anchors to be reexamined for visited status.
 * inputs:
 *   - mo_window *win: Current window.
 * returns:
 *   mo_succeed
 * remarks:
 *
 ****************************************************************************/
mo_status mo_redisplay_window (mo_window *win)
{
  struct Gadget *g;
  struct Window *w;
  char *curl = cached_url;
  cached_url = win->cached_url;

//  HTMLRetestAnchors (win->scrolled_win, anchor_visited_predicate);
  cached_url = curl;

/* AUTO NOW?
  GetAttr(MUIA_Boopsi_Object, HTML_gad, (ULONG *)(&g));
  GetAttr(MUIA_Window_Window, WI_Main, (ULONG *)(&w));
  RefreshGList(g, w, NULL, 1);
*/
  return mo_succeed;
}


/* ---------------------- mo_set_current_cached_win ----------------------- */

mo_status mo_set_current_cached_win (mo_window *win)
{
  current_win = win;
  view = win->view;

  return mo_succeed;
}

static short connect_interrupt = 0; /* 2 Bytes */
short image_interrupt = 0;   /* +2 Bytes = 1 Longword */
void icon_pressed_cb(void)
{
  connect_interrupt = 1;
}

#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    anchor_cb
 * purpose: Callback for triggering anchor in HTML widget.
 * inputs:
 *   - as per XmxCallback
 * returns:
 *   nothing
 * remarks:
 *   This is too complex and should be broken down.
 *   We look at the button event passed in through the callback;
 *   button1 == same window, button2 == new window.
 *   Call mo_open_another_window or mo_load_window_text to get
 *   the actual work done.
 ****************************************************************************/
static XmxCallback (anchor_cb)
{
  char *href, *reftext;
  char *access;
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  XButtonReleasedEvent *event =
    (XButtonReleasedEvent *)(((WbAnchorCallbackData *)call_data)->event);
  int force_newwin = (event->button == Button2 ? 1 : 0);

  if (!win)
    return;

  if (Rdata.protect_me_from_myself)
    {
      int answer = XmxModalYesOrNo
	(win->base, app_context,
	 "BEWARE: Despite our best and most strenuous intentions to the contrary,\n"
         "absolutely anything could be on the other end of this hyperlink,\n"
         "including -- quite possibly -- pornography, or even nudity.\n\n"
         "NCSA disclaims all responsibility regarding your emotional and mental health\n"
         "and specifically all responsibility for effects of viewing salacious material via Mosaic.\n\n"
         "With that in mind, are you *sure* you want to follow this hyperlink???",
	 "Yup, I'm sure, really.", "Ack, no!  Get me outta here.");
      if (!answer)
	return;
    }

  if (((WbAnchorCallbackData *)call_data)->href)
    href = strdup (((WbAnchorCallbackData *)call_data)->href);
  else
    href = strdup ("Unlinked");
  if (((WbAnchorCallbackData *)call_data)->text)
    reftext = strdup (((WbAnchorCallbackData *)call_data)->text);
  else
    reftext = strdup ("Untitled");

  mo_convert_newlines_to_spaces (href);

  if (!force_newwin)
    mo_load_window_text (win, href, reftext);
  else
    {
      char *target = mo_url_extract_anchor (href);
      char *url =
	mo_url_canonicalize_keep_anchor (href, win->current_node->url);
      /* @@@ should we be keeping the anchor here??? */
      if (strncmp (url, "telnet:", 7) && strncmp (url, "tn3270:", 7) &&
	  strncmp (url, "rlogin:", 7))
	{
	  /* Not a telnet anchor. */

	  /* Open the window (generating a new cached_url). */
	  mo_open_another_window (win, url, reftext, target);

	  /* Now redisplay this window. */
	  mo_redisplay_window (win);
	}
      else
	/* Just do mo_load_window_text go get the xterm forked off. */
	mo_load_window_text (win, url, reftext);
    }

  free (href);
  return;
}
#endif


/****************************************************************************
 * name:    anchor_visited_predicate (PRIVATE)
 * purpose: Called by the HTML widget to determine whether a given URL
 *	    has been previously visited.
 * inputs:
 *   - Widget	w: HTML widget that called this routine.
 *   - char *href: URL to test.
 * returns:
 *   1 if href has been visited previously; 0 otherwise.
 * remarks:
 *   All this does is canonicalize the URL and call
 *   mo_been_here_before_huh_dad() to figure out if we've been
 *   there before.
 ****************************************************************************/
int anchor_visited_predicate (Widget w, char *href)
{
  int rv;

  if (!Rdata.track_visited_anchors || !href)
    return 0;

  /* This doesn't do special things for data elements inside
     an HDF file, because it's faster this way. */
  href = mo_url_canonicalize (href, cached_url);

  rv = (mo_been_here_before_huh_dad (href) == mo_succeed ? 1 : 0);

  free (href);
  return rv;
}

#if !defined(_AMIGA) && !defined(__AROS__)
static void pointer_motion_callback (Widget w, char *href)
{
  mo_window *win = NULL;
  XmString xmstr;
  char *to_free = NULL, *to_free_2 = NULL;

  if (!Rdata.track_pointer_motion)
    return;

  while (win = mo_next_window (win))
    if (win->scrolled_win == w)
      goto foundit;

  /* Shit outta luck. */
  return;

 foundit:

  if (href && *href)
    {
      href = mo_url_canonicalize_keep_anchor (href, win->cached_url);
      to_free = href;

      /* Sigh... */
      mo_convert_newlines_to_spaces (href);

      /* This is now the option wherein the URLs are just spit up there;
	 else we put up something more friendly. */
      if (Rdata.track_full_url_names)
	{
	  /* Everything already done... */
	}
      else
	{
	  /* This is where we go get a good description. */
	  href = HTDescribeURL (href);
	  to_free_2 = href;
	}
    }
  else
    href = " ";
  xmstr = XmStringCreateLtoR (href, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues
    (win->tracker_label,
     XmNlabelString, (XtArgVal)xmstr,
     NULL);
  XmStringFree (xmstr);

  if (to_free)
    free (to_free);
  if (to_free_2)
    free (to_free_2);

  return;
}
#endif


#if !defined(_AMIGA) && !defined(__AROS__)
XmxCallback (submit_form_callback)
{
  mo_window *win = NULL;
  char *url = NULL, *method = NULL, *enctype = NULL, *query;
  int len, i;
  WbFormCallbackData *cbdata = (WbFormCallbackData *)call_data;
  int do_post_urlencoded = 0;

  if (!cbdata)
    return;

  while (win = mo_next_window (win))
    if (win->scrolled_win == w)
      goto foundit;

  /* Shit outta luck. */
  return;

 foundit:

  mo_busy ();

  /* Initial query: Breathing space. */
  len = 16;

  /* Add up lengths of strings. */
  for (i = 0; i < cbdata->attribute_count; i++)
    {
      if (cbdata->attribute_names[i])
	{
	  len += strlen (cbdata->attribute_names[i]) * 3;
	  if (cbdata->attribute_values[i])
	    len += strlen (cbdata->attribute_values[i]) * 3;
	}
      len += 2;
    }

  /* Get the URL. */
  if (cbdata->href && *(cbdata->href))
    url = cbdata->href;
  else
    url = win->current_node->url;

  if (cbdata->method && *(cbdata->method))
    method = cbdata->method;
  else
    method = strdup ("GET");

  /* Grab enctype if it's there. */
  if (cbdata->enctype && *(cbdata->enctype))
    enctype = cbdata->enctype;

#if 0
  fprintf (stderr, "[submit_form_callback] method is '%s'\n",
	   method);
  fprintf (stderr, "[submit_form_callback] enctype is '%s'\n",
	   enctype);
#endif

  if (strcmp (method, "POST") == 0)
    do_post_urlencoded = 1;

  len += strlen (url);

  query = (char *)malloc (sizeof (char) * len);

  if (!do_post_urlencoded)
    {
      strcpy (query, url);
      /* Clip out anchor. */
      strtok (query, "#");
      /* Clip out old query. */
      strtok (query, "?");
      if (query[strlen(query)-1] != '?')
	{
	  strcat (query, "?");
	}
    }
  else
    {
      /* Get ready for cats below. */
      query[0] = 0;
    }

  /* Take isindex possibility into account. */
  if (cbdata->attribute_count == 1 &&
      strcmp (cbdata->attribute_names[0], "isindex") == 0)
    {
      if (cbdata->attribute_values[0])
	{
	  char *c = mo_escape_part (cbdata->attribute_values[0]);
	  strcat (query, c);
	  free (c);
	}
    }
  else
    {
      for (i = 0; i < cbdata->attribute_count; i++)
	{
	  /* For all but the first, lead off with an ampersand. */
	  if (i != 0)
	    strcat (query, "&");
	  /* Rack 'em up. */
	  if (cbdata->attribute_names[i])
	    {
	      char *c = mo_escape_part (cbdata->attribute_names[i]);
	      strcat (query, c);
	      free (c);

	      strcat (query, "=");

	      if (cbdata->attribute_values[i])
		{
		  char *c = mo_escape_part (cbdata->attribute_values[i]);
		  strcat (query, c);
		  free (c);
		}
	    }
	}
    }

  if (do_post_urlencoded)
    {
      mo_post_access_document (win, url, "application/x-www-form-urlencoded",
			       query);
    }
  else
    {
      mo_access_document (win, query);
    }

  free (query);

  return;
}
#endif


#if !defined(_AMIGA) && !defined(__AROS__)
/* This only gets called by the widget in the middle of set_text. */
static XmxCallback (link_callback)
{
  mo_window *win = current_win;
  LinkInfo *linfo = (LinkInfo *)call_data;
  extern char *url_base_override;

  if (!linfo)
    return;

  /* Free -nothing- in linfo. */
  if (linfo->href)
    {
      url_base_override = strdup (linfo->href);

      /* Set the URL cache variables to the correct values NOW. */
      cached_url = mo_url_canonicalize (url_base_override, "");
      win->cached_url = cached_url;
    }
  if (linfo->role)
    {
      /* Do nothing with the role crap yet. */
    }

  return;
}
#endif


/* Exported to libwww2. */
void mo_gui_notify_progress(char *msg)
{
//  mo_window *win = current_win;
  char word3[100];
  static char sz[15];
  int page, x, y;

//  if (!Rdata.track_pointer_motion)
//    return;

  if (!msg)
    msg = " ";

  sscanf(msg, "%*s %d %s %d", &x, word3, &y);
  if (!strncmp(word3, "of", 2)) { /* Read X of Y bytes? */
    get(PG_Bottom, MUIA_Group_ActivePage, &page);
    if (page != 1) {
      set(PG_Bottom, MUIA_Group_ActivePage, 1);
    }
    set(GG_Progress, MUIA_Gauge_Current, (100 * x / y));
    sprintf(sz, "%d", y);
    set(TX_Progress, MUIA_Text_Contents, sz);
  } else {			/* Normal message */
    get(PG_Bottom, MUIA_Group_ActivePage, &page);
    if (page != 0) {
      set(PG_Bottom, MUIA_Group_ActivePage, 0);
    }
    set(TX_Dest, MUIA_Text_Contents, msg);
  }

  return;
}

void UpdateButtons (Widget w)
{
  /* MDF - I don't think we need this function */
}


static int logo_count = 0;

int mo_gui_check_icon (int twirl)
{
//  mo_window *win = current_win;
  int ret;

//if (twirl && Rdata.twirling_transfer_icon)
//    {
//	logo_count++;
//	if (logo_count == 8) {
//	logo_count = 0;
//	}
//	set(BT_Interrupt, SPIN_state, logo_count);
//    }
	/* Move the boing ball */
	DoMethod(BT_Interrupt,MUIM_BoingTransfer_Move);

  /* Have MUI check for any user input (i.e. buttons..) */
  DoMethod(App, MUIM_Application_InputBuffered);

  ret = connect_interrupt;
  image_interrupt = connect_interrupt;
  connect_interrupt = 0;

  return(ret);
}


void mo_gui_clear_icon (void)
{
  connect_interrupt = 0;
//  set(BT_Interrupt, MUIA_Image_State, IDS_NORMAL);
}

void mo_gui_done_with_icon (void)
{
//  mo_window *win = current_win;

  if (Rdata.larry_bitmaps)
    {
//	XmxApplyPixmapToLabelWidget
//	  (win->logo, xmosaic_up_pix);
    }
  else
    {
//	XClearArea(XtDisplay(win->logo), XtWindow(win->logo), 0, 0, 0, 0, True);
    }

//  set(BT_Interrupt, MUIA_Image_State, IDS_NORMAL);
  logo_count = 0;
}


void mo_view_keypress_handler(void)
{
  /* MDF - I don't think we need this */
}


#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    mo_fill_window (PRIVATE)
 * purpose: Take a new (empty) mo_window struct and fill in all the
 *	    GUI elements.
 * inputs:
 *   - mo_window *win: The window.
 * returns:
 *   mo_succeed
 * remarks:
 *
 ****************************************************************************/
static mo_status mo_fill_window (mo_window *win)
{
  Widget form;
  Widget frame, rc;
  Widget title_label, url_label;

  form = XmxMakeForm (win->base);

  /* Create the menubar. */
  win->menubar = mo_make_document_view_menubar (form);

  XmxRSetToggleState (win->menubar, win->font_size, XmxSet);

  win->binary_transfer = 0;
  XmxRSetToggleState (win->menubar, mo_binary_transfer,
		      (win->binary_transfer ? XmxSet : XmxNotSet));
  win->delay_image_loads = Rdata.delay_image_loads;
  XmxRSetToggleState (win->menubar, mo_delay_image_loads,
		      (win->delay_image_loads ? XmxSet : XmxNotSet));
  XmxRSetSensitive (win->menubar, mo_expand_images_current,
		    win->delay_image_loads ? XmxSensitive : XmxNotSensitive);

  XmxRSetSensitive (win->menubar, mo_annotate, XmxSensitive);
  XmxRSetSensitive (win->menubar, mo_annotate_edit, XmxNotSensitive);
  XmxRSetSensitive (win->menubar, mo_annotate_delete, XmxNotSensitive);

#ifdef HAVE_AUDIO_ANNOTATIONS
  /* If we're not audio capable, don't provide the menubar entry. */
  if (!mo_audio_capable ())
    XmxRSetSensitive (win->menubar, mo_audio_annotate, XmxNotSensitive);
#endif

#ifdef HAVE_DTM
  mo_set_dtm_menubar_functions (win);
#endif

  title_label = XmxMakeLabel (form, "Document Title:");
  XmxSetArg (XmNcursorPositionVisible, False);
  XmxSetArg (XmNeditable, False);
  win->title_text = XmxMakeTextField (form);

  url_label = XmxMakeLabel (form, "Document URL:");
  XmxSetArg (XmNcursorPositionVisible, False);
  XmxSetArg (XmNeditable, False);
  win->url_text = XmxMakeTextField (form);

  win->logo = XmxMakeNamedPushButton (form, NULL, "logo", icon_pressed_cb, NULL);

  if (Rdata.larry_bitmaps)
    {
      xmosaic_right_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_larry_right_bits, xmosaic_larry_right_width,
	 xmosaic_larry_right_height);
      xmosaic_left_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_larry_left_bits, xmosaic_larry_left_width,
	 xmosaic_larry_left_height);
      xmosaic_up_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_larry_bits, xmosaic_larry_width,
	 xmosaic_larry_height);
      xmosaic_down_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_larry_down_bits, xmosaic_larry_down_width,
	 xmosaic_larry_down_height);
    }
  else
    {
      xmosaic_right_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_right_bits, xmosaic_right_width,
	 xmosaic_right_height);
      xmosaic_left_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_left_bits, xmosaic_left_width,
	 xmosaic_left_height);
      xmosaic_up_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_bits, xmosaic_width,
	 xmosaic_height);
      xmosaic_down_pix = XmxCreatePixmapFromBitmap
	(win->logo, xmosaic_down_bits, xmosaic_down_width,
	 xmosaic_down_height);
    }

  if (IconsMade)
    {
       XmxApplyPixmapToLabelWidget
	    (win->logo, IconPix[0]);
    }
  else
    {
       XmxApplyPixmapToLabelWidget
	    (win->logo, xmosaic_up_pix);
    }

  XmxSetArg (WbNtext, NULL);
  win->last_width = 0;
  XmxSetArg (XmNresizePolicy, XmRESIZE_ANY);
  XmxSetArg (WbNpreviouslyVisitedTestFunction, (long)anchor_visited_predicate);
  XmxSetArg (WbNpointerMotionCallback, (long)pointer_motion_callback);
  XmxSetArg (WbNfancySelections, win->pretty ? True : False);
  XmxSetArg (WbNverticalScrollOnRight, True);
  XmxSetArg (WbNdelayImageLoads, win->delay_image_loads ? True : False);
  XmxSetArg (XmNshadowThickness, 2);
  win->scrolled_win = XtCreateWidget ("view", htmlWidgetClass,
				      form, Xmx_wargs, Xmx_n);
  XtManageChild (win->scrolled_win);
  mo_register_image_resolution_function (win);
  XmxAddCallback (win->scrolled_win, WbNanchorCallback, anchor_cb, 0);
  XmxAddCallback (win->scrolled_win, WbNlinkCallback, link_callback, 0);
  XmxAddCallback (win->scrolled_win, WbNsubmitFormCallback,
		  submit_form_callback, 0);

  Xmx_n = 0;
  XmxSetArg (WbNview, (long)(&win->view));
  XtGetValues (win->scrolled_win, Xmx_wargs, Xmx_n);

  XmxAddEventHandler
    (win->view, KeyPressMask, mo_view_keypress_handler, 0);

  XmxSetArg (XmNresizePolicy, XmRESIZE_ANY);
  XmxSetArg (XmNresizable, True);
  win->bottom_form = XmxMakeForm (form);

  /* Children of win->bottom_form. */
  {
    if (Rdata.track_pointer_motion)
      {
	XmxSetArg (XmNalignment, XmALIGNMENT_BEGINNING);
	win->tracker_label = XmxMakeLabel (win->bottom_form, " ");
      }

    XmxSetArg (XmNmarginWidth, 0);
    XmxSetArg (XmNmarginHeight, 0);
    XmxSetArg (XmNpacking, XmPACK_TIGHT);
    XmxSetArg (XmNresizable, True);
    win->button_rc = XmxMakeHorizontalRowColumn (win->bottom_form);

    /* Children of win->button_rc. */
    {
      win->back_button = XmxMakePushButton
	(win->button_rc, "Back", menubar_cb, mo_back);
      win->forward_button = XmxMakePushButton
	(win->button_rc, "Forwd", menubar_cb, mo_forward);
      win->home_button = XmxMakePushButton
	(win->button_rc, "Home", menubar_cb, mo_home_document);
      if (!Rdata.simple_interface)
	win->reload_button = XmxMakePushButton
	  (win->button_rc, "Reload", menubar_cb, mo_reload_document);
      if (!Rdata.simple_interface)
	win->open_button = XmxMakePushButton
	  (win->button_rc, "Open", menubar_cb, mo_open_document);
      win->save_button = XmxMakePushButton
	(win->button_rc, "Save", menubar_cb, mo_save_document);
      win->tex_button = XmxMakePushButton
	(win->button_rc, "(La)TeX", menubar_cb, mo_tex_document);
      win->clone_button = XmxMakePushButton
	(win->button_rc, "Clone", menubar_cb, mo_clone_window);
      if (!Rdata.simple_interface)
	win->new_button = XmxMakePushButton
	  (win->button_rc, "New", menubar_cb, mo_new_window);
      win->close_button = XmxMakePushButton
	(win->button_rc, "Close", menubar_cb, mo_close_window);
    }

    if (Rdata.track_pointer_motion)
      {
	XmxSetOffsets (win->tracker_label, 0, 0, 10, 10);
	XmxSetConstraints
	  (win->tracker_label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM,
	   XmATTACH_FORM, NULL, NULL, NULL, NULL);
	XmxSetOffsets (win->button_rc, 2, 5, 10, 10);
	XmxSetConstraints
	  (win->button_rc, XmATTACH_WIDGET, XmATTACH_FORM, XmATTACH_FORM,
	   XmATTACH_FORM, win->tracker_label, NULL, NULL, NULL);
      }
    else
      {
	XmxSetOffsets (win->button_rc, 5, 5, 10, 10);
	XmxSetConstraints
	  (win->button_rc, XmATTACH_FORM, XmATTACH_FORM, XmATTACH_FORM,
	   XmATTACH_FORM, NULL, NULL, NULL, NULL);
      }

  }

  /* Constraints for form. */
  XmxSetConstraints
    (win->menubar->base, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM,
     XmATTACH_FORM, NULL, NULL, NULL, NULL);

  /* Top to menubar, bottom to nothing,
     left to form, right to nothing. */
  XmxSetOffsets (title_label, 13, 0, 10, 0);
  XmxSetConstraints
    (title_label,
     XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
     win->menubar->base, NULL, NULL, NULL);
  /* Top to menubar, bottom to nothing,
     Left to title label, right to logo. */
  XmxSetOffsets (win->title_text, 9, 1, 9, 8);
  XmxSetConstraints
    (win->title_text,
     XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_WIDGET, XmATTACH_WIDGET,
     win->menubar->base, NULL, title_label, win->logo);
  /* Top to title text, bottom to nothing,
     left to form, right to nothing. */
  XmxSetOffsets (url_label, 16, 0, 10, 0);
  XmxSetConstraints
    (url_label, XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
     win->title_text, NULL, NULL, NULL);
  /* Top to title text, bottom to nothing,
     Left to url label, right to logo. */
  XmxSetOffsets (win->url_text, 12, 1, 8, 8);
  XmxSetConstraints
    (win->url_text,
     XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_WIDGET, XmATTACH_WIDGET,
     win->title_text, NULL, url_label, win->logo);
  /* Top to menubar, bottom to nothing,
     left to nothing, right to form. */
  XmxSetOffsets (win->logo, 8, 0, 0, 7);
  XmxSetConstraints
    (win->logo,
     XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_NONE, XmATTACH_FORM,
     win->menubar->base, NULL, NULL, NULL);

  /* Top to logo, bottom to bottom form,
     Left to form, right to form. */
  XmxSetOffsets (win->scrolled_win, 7, 2, 7, 7);
  XmxSetConstraints
    (win->scrolled_win,
     XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, XmATTACH_FORM,
     win->logo, win->bottom_form, NULL, NULL);

  XmxSetOffsets (win->bottom_form, 10, 2, 2, 2);
  XmxSetConstraints
    (win->bottom_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM,
     XmATTACH_FORM, NULL, NULL, NULL, NULL);

  /* Can't go back or forward if we haven't gone anywhere yet... */
  mo_back_impossible (win);
  mo_forward_impossible (win);

  return;
}
#endif

#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    mo_delete_window
 * purpose: Shut down a window.
 * inputs:
 *   - mo_window *win: The window.
 * returns:
 *   mo_succeed
 * remarks:
 *   This can be called, among other places, from the WM_DELETE_WINDOW
 *   handler.  By the time we get here, we must assume the window is already
 *   in the middle of being shut down.
 *   We must explicitly close every dialog that be open as a child of
 *   the window, because window managers too stupid to do that themselves
 *   will otherwise allow them to stay up.
 ****************************************************************************/
#define POPDOWN(x) \
  if (win->x) XtUnmanageChild (win->x)

mo_status mo_delete_window (mo_window *win)
{
  mo_node *node;

  if (!win)
    return mo_fail;

  node = win->history;

  POPDOWN (source_win);
  POPDOWN (save_win);
  POPDOWN (savebinary_win);
  POPDOWN (open_win);
  POPDOWN (mail_win);
  POPDOWN (mailhot_win);
  POPDOWN (edithot_win);
  POPDOWN (mailhist_win);
  POPDOWN (print_win);
  POPDOWN (history_win);
  POPDOWN (open_local_win);
  POPDOWN (hotlist_win);
  POPDOWN (whine_win);
  POPDOWN (annotate_win);
  POPDOWN (search_win);
  POPDOWN (searchindex_win);
#ifdef HAVE_DTM
  POPDOWN (dtmout_win);
#endif
#ifdef HAVE_AUDIO_ANNOTATIONS
  POPDOWN (audio_annotate_win);
#endif
  XtPopdown (win->base);

  /* Free up some of the HTML Widget's state */
  HTMLFreeImageInfo (win->scrolled_win);

  while (node)
    {
      mo_node *tofree = node;
      node = node->next;
      mo_free_node_data (tofree);
      free (tofree);
    }

  free (win->search_start);
  free (win->search_end);

  /* This will free the win structure (but none of its elements
     individually) and exit if this is the last window in the list. */
  mo_remove_window_from_list (win);

  /* Go get another current_win. */
  mo_set_current_cached_win (mo_next_window (NULL));

  return mo_succeed;
}
#endif


#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    mo_open_window_internal (PRIVATE)
 * purpose: Make a mo_window struct and fill up the GUI.
 * inputs:
 *   - Widget	    base: The dialog widget on which this window is
 *			  to be based.
 *   - mo_window *parent: The parent mo_window struct for this window,
 *			  if one exists; this can be NULL.
 * returns:
 *   The new window (mo_window *).
 * remarks:
 *   This routine must set to 0 all elements in the mo_window struct
 *   that can be tested by various routines to see if various things
 *   have been done yet (popup windows created, etc.).
 ****************************************************************************/
static mo_window *mo_open_window_internal (Widget base, mo_window *parent)
{
  mo_window *win;

  win = (mo_window *)malloc (sizeof (mo_window));
  win->id = XmxMakeNewUniqid ();
  XmxSetUniqid (win->id);

  win->base = base;

  win->source_win = 0;
  win->save_win = 0;
  win->savebinary_win = 0;
  win->open_win = win->open_text = win->open_local_win = 0;
  win->mail_win = win->mailhot_win = win->edithot_win = win->mailhist_win = 0;
  win->print_win = 0;
  win->history_win = win->history_list = 0;
  win->hotlist_win = win->hotlist_list = 0;
  win->whine_win = win->whine_text = 0;
  win->annotate_win = 0;
  win->search_win = win->search_win_text = 0;
  win->searchindex_win = win->searchindex_win_label = win->searchindex_win_text = 0;
#ifdef HAVE_DTM
  win->dtmout_win = win->dtmout_text = 0;
#endif
#ifdef HAVE_AUDIO_ANNOTATIONS
  win->audio_annotate_win = 0;
#endif

  win->history = NULL;
  win->current_node = 0;
  win->reloading = 0;
  win->source_text = 0;
  win->format_optmenu = 0;
  win->save_format = 0;
  if (!parent)
    win->font_size = mo_regular_fonts;
  else
    win->font_size = parent->font_size;

  win->underlines_snarfed = 0;
  if (!parent)
    win->underlines_state = mo_default_underlines;
  else
    win->underlines_state = parent->underlines_state;

  win->pretty = Rdata.default_fancy_selections;

  win->mail_format = 0;

#ifdef HAVE_AUDIO_ANNOTATIONS
  win->record_fnam = 0;
  win->record_pid = 0;
#endif

  win->print_text = 0;
  win->print_format = 0;

  win->target_anchor = 0;
  /* Create search_start and search_end. */
  win->search_start = (void *)malloc (sizeof (ElementRef));
  win->search_end = (void *)malloc (sizeof (ElementRef));

#ifdef ISINDEX
  /* We don't know yet. */
  win->keyword_search_possible = -1;
#endif

  /* Install all the GUI bits & pieces. */
  mo_fill_window (win);

  /* Register win with internal window list. */
  mo_add_window_to_list (win);

  /* Pop the window up. */
  XtPopup (win->base, XtGrabNone);
  XFlush (dsp);
  XSync (dsp, False);

  if ((!IconsMade)&&(!Rdata.larry_bitmaps))
    {
      MakeAnimationPixmaps(win->logo);
      IconsMade = 1;
      XmxApplyPixmapToLabelWidget
	    (win->logo, IconPix[0]);
      logo_count = 0;
    }

  /* Set the font size. */
  if (win->font_size != mo_regular_fonts)
    mo_set_fonts (win, win->font_size);

  /* Set the underline state. */
  mo_set_underlines (win, win->underlines_state);

  /* Set the fancy selections toggle to the starting value. */
  mo_set_fancy_selections_toggle (win);

  return win;
}
#endif


#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    delete_cb (PRIVATE)
 * purpose: Callback for the WM_DELETE_WINDOW protocol.
 * inputs:
 *   - as per XmxCallback
 * returns:
 *   nothing
 * remarks:
 *   By the time we get called here, the window has already been popped
 *   down.  Just call mo_delete_window to clean up.
 ****************************************************************************/
static XmxCallback (delete_cb)
{
  mo_window *win = (mo_window *)client_data;
  mo_delete_window (win);
  return;
}
#endif


#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    mo_make_window (PRIVATE)
 * purpose: Make a new window from scratch.
 * inputs:
 *   - Widget	   parent: Parent for the new window shell.
 *   - mo_window *parentw: Parent window, if one exists (may be NULL).
 * returns:
 *   The new window (mo_window *).
 * remarks:
 *   The 'parent window' parentw is the window being cloned, or the
 *   window in which the 'new window' command was triggered, etc.
 *   Some GUI properties are inherited from it, if it exists (fonts,
 *   anchor appearance, etc.).
 ****************************************************************************/
static mo_window *mo_make_window (Widget parent, mo_window *parentw)
{
  Widget base;
  mo_window *win;
  Atom WM_DELETE_WINDOW;

  XmxSetArg (XmNtitle, (long)"NCSA Mosaic: Document View");
  XmxSetArg (XmNiconName, (long)"Mosaic");
  XmxSetArg (XmNallowShellResize, False);
  base = XtCreatePopupShell ("shell", topLevelShellWidgetClass,
			     toplevel, Xmx_wargs, Xmx_n);
  Xmx_n = 0;

#ifdef EDITRES_SUPPORT
  XtAddEventHandler(base, (EventMask) 0, TRUE,
		    (XtEventHandler) _XEditResCheckMessages, NULL);
#endif

  win = mo_open_window_internal (base, parentw);

  WM_DELETE_WINDOW = XmInternAtom(dsp, "WM_DELETE_WINDOW", False);
  XmAddWMProtocolCallback(base, WM_DELETE_WINDOW, delete_cb, (XtPointer)win);

  return win;
}


/****************************************************************************
 * name:    mo_open_another_window_internal (PRIVATE)
 * purpose: Open a new window from an existing window.
 * inputs:
 *   - mo_window *win: The old window.
 * returns:
 *   The new window (mo_window *).
 * remarks:
 *   This routine handles (optional) autoplace of new windows.
 ****************************************************************************/
static mo_window *mo_open_another_window_internal (mo_window *win)
{
  Dimension oldx, oldy;
  Dimension scrx = WidthOfScreen (XtScreen (win->base));
  Dimension scry = HeightOfScreen (XtScreen (win->base));
  Dimension x, y;
  Dimension width, height;
  mo_window *newwin;

  XtVaGetValues (win->base, XmNx, &oldx, XmNy, &oldy,
		 XmNwidth, &width, XmNheight, &height, NULL);

  /* Ideally we open down and over 40 pixels... is this possible? */
  x = oldx + 40; y = oldy + 40;
  /* If not, deal with it... */
  while (x + width > scrx)
    x -= 80;
  while (y + height > scry)
    y -= 80;

  XmxSetArg (XmNdefaultPosition, False);
  if (Rdata.auto_place_windows)
    {
      char geom[20];
      sprintf (geom, "+%d+%d\0", x, y);
      XmxSetArg (XmNgeometry, (long)geom);
    }
  XmxSetArg (XmNwidth, width);
  XmxSetArg (XmNheight, height);

  newwin = mo_make_window (toplevel, win);
  mo_set_current_cached_win (newwin);
  return newwin;
}


/****************************************************************************
 * name:    mo_open_window
 * purpose: Open a new window to view a given URL.
 * inputs:
 *   - Widget	   parent: The parent Widget for the new window's shell.
 *   - char	     *url: The URL to view in the new window.
 *   - mo_window *parentw: The (optional) parent window of the new window.
 * returns:
 *   The new window.
 * remarks:
 *
 ****************************************************************************/
mo_window *mo_open_window (Widget parent, char *url, mo_window *parentw)
{
  mo_window *win;

  win = mo_make_window (parent, parentw);

  mo_set_current_cached_win (win);

  mo_load_window_text (win, url, NULL);

  return win;
}


/****************************************************************************
 * name:    mo_duplicate_window
 * purpose: Clone a existing window as intelligently as possible.
 * inputs:
 *   - mo_window *win: The existing window.
 * returns:
 *   The new window.
 * remarks:
 *
 ****************************************************************************/
mo_window *mo_duplicate_window (mo_window *win)
{
  mo_window *neww = mo_open_another_window_internal (win);

  mo_duplicate_window_text (win, neww);

  return neww;
}



/****************************************************************************
 * name:    mo_open_another_window
 * purpose: Open another window to view a given URL, unless the URL
 *	    indicates that it's pointless to do so
 * inputs:
 *   - mo_window      *win: The existing window.
 *   - char	      *url: The URL to view in the new window.
 *   - char	      *ref: The reference (hyperlink text contents) for this
 *			    URL; can be NULL.
 *   - char *target_anchor: The target anchor to view open opening the
 *			    window, if any.
 * returns:
 *   The new window.
 * remarks:
 *
 ****************************************************************************/
mo_window *mo_open_another_window (mo_window *win, char *url, char *ref,
				   char *target_anchor)
{
  mo_window *neww;

  /* Check for reference to telnet.  Never open another window
     if reference to telnet exists; instead, call mo_load_window_text,
     which knows how to manage current window's access to telnet. */
  if (!strncmp (url, "telnet:", 7) || !strncmp (url, "tn3270:", 7) ||
      !strncmp (url, "rlogin:", 7))
    {
      mo_load_window_text (win, url, NULL);
      return NULL;
    }

  mo_busy ();

  neww = mo_open_another_window_internal (win);
  /* Set it here; hope it gets handled in mo_load_window_text_first
     (it probably won't, now. */
  neww->target_anchor = target_anchor;
  mo_load_window_text (neww, url, ref);

  return neww;
}
#endif /* not amiga */


/* ------------------------------------------------------------------------ */

char **gargv;
int gargc;

/****************************************************************************
 * name:    fire_er_up (PRIVATE)
 * purpose: Callback from timer that actually starts up the application,
 *	    i.e., opens the first window.
 * inputs:
 *   - as per XmxCallback
 * returns:
 *   Nothing.
 * remarks:
 *   This routine figures out what the home document should be
 *   and then calls mo_open_window().
 ****************************************************************************/
void fire_er_up(int gargc, char **gargv)
{
  char *home_opt;
  mo_window *win;

  /* Pick up default or overridden value out of X resources. */
  home_document = Rdata.home_document;

  /* Value of environment variable WWW_HOME overrides that. */
  if ((home_opt = getenv ("WWW_HOME")) != NULL)
    home_document = home_opt;

  /* Value of argv[1], if it exists, sets startup_document.
     (All other command-line flags will have been picked up by
     the X resource mechanism.) */
  if (gargc > 1 && gargv[1] && *gargv[1])
    startup_document = gargv[1];

  /* Check for proper home document URL construction. */
  if (!strstr (home_document, ":"))
    home_document = mo_url_canonicalize_local (home_document);
  if (startup_document && !strstr (startup_document, ":"))
    startup_document = mo_url_canonicalize_local (startup_document);

//  win = mo_open_window
//    (toplevel, startup_document ? startup_document : home_document, NULL);

  return;
}


#if !defined(_AMIGA) && !defined(__AROS__)
/****************************************************************************
 * name:    mo_open_initial_window
 * purpose: This routine is called when we know we want to open the
 *	    initial Document View window.
 * inputs:
 *   none
 * returns:
 *   mo_succeed
 * remarks: This routine is simply a stub that sets a timeout that
 *	    calls fire_er_up() after 10 milliseconds, which does the
 *	    actual work.
 ****************************************************************************/
mo_status mo_open_initial_window (void)
{
  /* Set a timer that will actually cause the window to open. */
  XtAppAddTimeOut (app_context, 10,
		   (XtTimerCallbackProc)fire_er_up, (XtPointer)True);

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_error_handler (PRIVATE)
 * purpose: Handle X errors.
 * inputs:
 *   - Display	     *dsp: The X display.
 *   - XErrorEvent *event: The error event to handle.
 * returns:
 *   0, if it doesn't force an exit.
 * remarks:
 *   The main reason for this handler is to keep the application
 *   from crashing on BadAccess errors during calls to XFreeColors().
 ****************************************************************************/
static int mo_error_handler (Display *dsp, XErrorEvent *event)
{
  char buf[128];

  XUngrabPointer (dsp, CurrentTime);   /* in case error occurred in Grab */

  /* BadAlloc errors (on a XCreatePixmap() call)
     and BadAccess errors on XFreeColors are 'ignoreable' errors */
  if (event->error_code == BadAlloc ||
      (event->error_code == BadAccess && event->request_code == 88))
    return 0;
  else
    {
      /* All other errors are 'fatal'. */
      XGetErrorText (dsp, event->error_code, buf, 128);
      fprintf (stderr, "X Error: %s\n", buf);
      fprintf (stderr, "  Major Opcode:  %d\n", event->request_code);

      /* Try to close down gracefully. */
      mo_exit ();
    }
}
#endif


/****************************************************************************
 * name:    mo_do_gui
 * purpose: This is basically the real main routine of the application.
 * inputs:
 *   - int    argc: Number of arguments.
 *   - char **argv: The argument vector.
 * returns:
 *   nothing
 * remarks:
 *
 ****************************************************************************/
void mo_do_gui (int argc, char **argv)
{
  /* actually some important stuff in here! */
#if !defined(_AMIGA) && !defined(__AROS__)
  int use_color = 1;
  int no_defaults = 0;

  int i;

  /* Loop through the args before passing them off to
     XtAppInitialize() in case we need to catch something first. */
  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-mono"))
	use_color = 0;
      if (!strcmp (argv[i], "-color"))
	use_color = 1;
      if (!strcmp (argv[i], "-nd"))
	no_defaults = 1;
    }

  /* Motif setup. */
  XmxStartup ();
  XmxSetArg (XmNmappedWhenManaged, False);
  if (no_defaults)
    {
      toplevel = XtAppInitialize
	(&app_context, "Mosaic", options, XtNumber (options),
	 &argc, argv, NULL, Xmx_wargs, Xmx_n);
    }
  else
    {
      if (use_color)
	{
	  toplevel = XtAppInitialize
	    (&app_context, "Mosaic", options, XtNumber (options),
	     &argc, argv, color_resources, Xmx_wargs, Xmx_n);
	}
      else
	{
	  toplevel = XtAppInitialize
	    (&app_context, "Mosaic", options, XtNumber (options),
	     &argc, argv, mono_resources, Xmx_wargs, Xmx_n);
	}
    }

  /* Needed for picread.c, right now. */
  dsp = XtDisplay (toplevel);
  {
    XVisualInfo vinfo, *vptr;
    int cnt;

    vinfo.visualid =
      XVisualIDFromVisual
	(DefaultVisual (dsp,
			DefaultScreen (dsp)));
    vptr = XGetVisualInfo (dsp, VisualIDMask, &vinfo, &cnt);
    Vclass = vptr->class;
    XFree((char *)vptr);
  }

  XtVaGetApplicationResources (toplevel, (XtPointer)&Rdata, resources,
			       XtNumber (resources), NULL);

  XSetErrorHandler (mo_error_handler);

  /* Kill group annotation capability with libwww2 -- IT DOES NOT WORK,
     so don't even try to change this... */
  Rdata.annotation_server = NULL;

#ifdef __sgi
  /* Turn on debugging malloc if necessary. */
  if (Rdata.debugging_malloc)
    mallopt (M_DEBUG, 1);
#endif

  global_xterm_str = Rdata.xterm_command;

  uncompress_program = Rdata.uncompress_command;
  gunzip_program = Rdata.gunzip_command;

  tweak_gopher_types = Rdata.tweak_gopher_types;
  max_wais_responses = Rdata.max_wais_responses;

  use_default_extension_map = Rdata.use_default_extension_map;
  global_extension_map = Rdata.global_extension_map;
  if (Rdata.personal_extension_map)
    {
      char *home = getenv ("HOME");

      if (!home)
	home = "/tmp";

      personal_extension_map = (char *)malloc
	(strlen (home) + strlen (Rdata.personal_extension_map) + 8);
      sprintf (personal_extension_map, "%s/%s\0", home,
	       Rdata.personal_extension_map);
    }
  else
    personal_extension_map = "\0";

  use_default_type_map = Rdata.use_default_type_map;
  global_type_map = Rdata.global_type_map;
  if (Rdata.personal_type_map)
    {
      char *home = getenv ("HOME");

      if (!home)
	home = "/tmp";

      personal_type_map = (char *)malloc
	(strlen (home) + strlen (Rdata.personal_type_map) + 8);
      sprintf (personal_type_map, "%s/%s\0", home,
	       Rdata.personal_type_map);
    }
  else
    personal_type_map = "\0";

#ifdef HAVE_HDF
  have_hdf = 1;
#else
  have_hdf = 0;
#endif

  twirl_increment = Rdata.twirl_increment;

  /* First get the hostname. */
  machine = (char *)malloc (sizeof (char) * 64);
  gethostname (machine, 64);

  /* Then make a copy of the hostname for shortmachine.
     Don't even ask. */
  shortmachine = strdup (machine);

  /* Then find out the full name, if possible. */
  if (Rdata.full_hostname)
    {
      free (machine);
      machine = Rdata.full_hostname;
    }
  else if (!Rdata.gethostbyname_is_evil)
    {
      struct hostent *phe;

      phe = gethostbyname (machine);
      if (phe && phe->h_name)
	{
	  free (machine);
	  machine = strdup (phe->h_name);
	}
    }
  /* (Otherwise machine just remains whatever gethostname returned.) */

  machine_with_domain = (strlen (machine) > strlen (shortmachine) ?
			 machine : shortmachine);

  /* If there's no tmp directory assigned by the X resource, then
     look at TMPDIR. */
  if (!Rdata.tmp_directory)
    {
      Rdata.tmp_directory = getenv ("TMPDIR");
      /* It can still be NULL when we leave here -- then we'll just
	 let tmpnam() do what it does best. */
    }

  /* If there's no docs directory assigned by the X resource,
     then look at MOSAIC_DOCS_DIRECTORY environment variable
     and then at hardcoded default. */
  if (!Rdata.docs_directory)
    {
      Rdata.docs_directory = getenv ("MOSAIC_DOCS_DIRECTORY");
      if (!Rdata.docs_directory)
	Rdata.docs_directory = DOCS_DIRECTORY_DEFAULT;
      if (!Rdata.docs_directory || !*(Rdata.docs_directory))
	{
	  fprintf (stderr, "fatal error: nonexistent docs directory\n");
	  exit (-1);
	}
    }

  mo_setup_default_hotlist ();
  if (Rdata.use_global_history)
    mo_setup_global_history ();
  else
    mo_init_global_history ();
  mo_setup_pan_list ();

  /* Write pid into "~/.mosaicpid". */
  {
    char *home = getenv ("HOME"), *fnam;
    FILE *fp;

    if (!home)
      home = "/tmp";

    fnam = (char *)malloc (strlen (home) + 32);
    sprintf (fnam, "%s/.mosaicpid", home);

    fp = fopen (fnam, "w");
    if (fp)
      {
	fprintf (fp, "%d\n", getpid());
	fclose (fp);
      }

    free (fnam);
  }

  {
    Widget foo = XmxMakeForm (toplevel);
    XmString xmstr = XmxMakeXmstrFromString ("NCSA Mosaic");
    XmxSetArg (XmNlabelString, (long)xmstr);
    Xmx_w = XtCreateManagedWidget ("blargh", xmLabelGadgetClass,
				   foo, Xmx_wargs, Xmx_n);
    XmStringFree (xmstr);
    Xmx_n = 0;
  }

  busy_cursor = XCreateFontCursor (dsp, XC_watch);

  XtRealizeWidget (toplevel);

  gargv = argv;
  gargc = argc;

#ifndef VMS
  signal (SIGUSR1, ProcessExternalDirective);
#endif

  defer_initial_window = 0;

  if (!defer_initial_window)
    mo_open_initial_window ();

  XtAppMainLoop (app_context);
#endif /* not amiga */
}


/* stuff about external directives left out... */

long atorgb(char *s)
{
	long r,g,b;
	sscanf(s,"%d/%d/%d\n",&r,&g,&b);
	return (r<<16)+(g<<8)+b;
}

/* MDF 6-10-94
   The global Rdata structure contains all Mosaic-wide preferences.
   We first set all fields with our built-in defaults, then see if the
   user has any prefs files which should override these.
*/

char *amiga_editor;	/* not in Rdata, because needed by libwww2 */
char *amiga_sendmail;	/* not in Rdata, because needed by libwww2 */
char *amiga_telnet;	/* not in Rdata, because needed by libwww2 */
char *amiga_rlogin;	/* not in Rdata, because needed by libwww2 */

void amiga_load_prefs(void)
{
  FILE *fp;
  char *s, *p;
  char line[1004], param[100], *value ;
  void RexxMenu(int, char *) ;

  Rdata.use_global_history = TRUE;
  Rdata.track_visited_anchors = TRUE;
#ifndef __AROS__
  Rdata.font = "times/13";
  Rdata.boldfont = "times/13/bold";
  Rdata.italicfont = "times/13/italic";
  Rdata.h1font = "times/24";
  Rdata.h2font = "times/18";
  Rdata.h3font = "times/15";
  Rdata.h4font = "times/13";
  Rdata.h5font = "times/11";
  Rdata.h6font = "times/11";
  Rdata.fixedfont = "courier/13";
  Rdata.addressfont = "times/13/italic";
  Rdata.listingfont = "courier/11";
  Rdata.plainfont = "courier/11";
#else
  Rdata.font = "arial/13";
  Rdata.boldfont = "arial/13/bold";
  Rdata.italicfont = "arial/13/italic";
  Rdata.h1font = "arial/24";
  Rdata.h2font = "arial/18";
  Rdata.h3font = "arial/15";
  Rdata.h4font = "arial/13";
  Rdata.h5font = "arial/11";
  Rdata.h6font = "arial/11";
  Rdata.fixedfont = "ttcourier/13";
  Rdata.addressfont = "arial/13/italic";
  Rdata.listingfont = "ttcourier/11";
  Rdata.plainfont = "ttcourier/11";
#endif
  Rdata.home_document = "http://www.omnipresence.com/amosaic/2.0/";
  Rdata.use_global_history = True;
  Rdata.display_urls_not_titles = False;
  Rdata.global_history_file = "s:mosaic-global-history";
  Rdata.default_hotlist_file = "s:mosaic-hotlist-default";
  Rdata.tmp_directory = "T:";
  Rdata.track_pointer_motion = True;
  Rdata.track_full_url_names = True;
  Rdata.reload_reloads_images = False;
  Rdata.delay_image_loads = True;
  Rdata.use_default_extension_map = True;
  Rdata.twirling_transfer_icon = True;
  Rdata.image_cache_size = 400;
	Rdata.isindex_to_form=TRUE;
	Rdata.image_buttons=2;
	Rdata.title_to_window=FALSE;
	Rdata.u_anchor=0x2020FF;
	Rdata.f_anchor=0x000080;
	Rdata.a_anchor=0xCC0000;

  /* First check the ENV:MOSAIC/PREFS file for prefs information */

  fp = fopen("env:mosaic/prefs", "r");
  if (!fp) goto check_env;		/* we have no prefs file. */

  while (fgets(line, 1000, fp)) {

    value = stpsym(line, param, 100) ;
    if (value) value = stpblk(value) ;
    else continue ;
    if (*value) value[strlen(value) - 1] = '\0' ;

    if (stricmp (param, "TempDir") == 0)
      Rdata.tmp_directory = strdup(value);
    if (stricmp (param, "Sendmail") == 0)
      amiga_sendmail = strdup(value);
    if (stricmp (param, "Editor") == 0)
      amiga_editor = strdup(value);
    if (stricmp (param, "Telnet") == 0)
      amiga_telnet = strdup(value);
    if (stricmp (param, "Rlogin") == 0)
      amiga_rlogin = strdup(value);

    if (!stricmp(param, "ImageCacheSize"))
      Rdata.image_cache_size = atoi(value);
    if (!stricmp(param, "DelayImageLoads"))
      Rdata.delay_image_loads = (stricmp(value, "true"))?0:1;
    if (!stricmp(param, "TitleToWindow"))
      Rdata.title_to_window = (stricmp(value, "true"))?0:1;
    if (!stricmp(param, "IsindexToForm"))
      Rdata.isindex_to_form = (stricmp(value, "true"))?0:1;
    if (!stricmp(param, "Buttons")){
	if(!strnicmp(value, "text",5)) Rdata.image_buttons = 0;
		else if(!strnicmp(value, "images",7)) Rdata.image_buttons = 1;
		else if(!strnicmp(value, "images&text",11)) Rdata.image_buttons = 2;
		}

    if (!stricmp(param, "Anchor"))
      Rdata.u_anchor = atorgb(value);
    if (!stricmp(param, "FollowedAnchor"))
      Rdata.f_anchor = atorgb(value);
    if (!stricmp(param, "ActiveAnchor"))
      Rdata.a_anchor = atorgb(value);

    if (!stricmp(param, "HomeDocument"))
      Rdata.home_document = strdup(value);
    if (!stricmp(param, "DownloadDir"))
      Rdata.download_dir = strdup(value);
#ifdef HAVE_REXX
    if (!stricmp(param, "AddToHotList"))
      RexxMenu(mo_add_hot_list, value) ;
    if (!stricmp(param, "JumpToHotList"))
      RexxMenu(mo_jump_hot_list, value) ;
    if (!stricmp(param, "PrintCommand"))
      RexxMenu(mo_print_document, value) ;
#endif

    if (!stricmp(param, "Font"))
      Rdata.font = strdup(value);
    if (!stricmp(param, "italicFont"))
      Rdata.italicfont = strdup(value);
    if (!stricmp(param, "boldFont"))
      Rdata.boldfont = strdup(value);
    if (!stricmp(param, "h1Font"))
      Rdata.h1font = strdup(value);
    if (!stricmp(param, "h2Font"))
      Rdata.h2font = strdup(value);
    if (!stricmp(param, "h3Font"))
      Rdata.h3font = strdup(value);
    if (!stricmp(param, "h4Font"))
      Rdata.h4font = strdup(value);
    if (!stricmp(param, "h5Font"))
      Rdata.h5font = strdup(value);
    if (!stricmp(param, "h6Font"))
      Rdata.h6font = strdup(value);
    if (!stricmp(param, "fixedFont"))
      Rdata.fixedfont = strdup(value);
    if (!stricmp(param, "addressFont"))
      Rdata.addressfont = strdup(value);
    if (!stricmp(param, "listingFont"))
      Rdata.listingfont = strdup(value);
    if (!stricmp(param, "plainFont"))
      Rdata.plainfont = strdup(value);
#ifdef HAVE_REXX
    if (!stricmp(param, "rexxmacro1"))
      RexxMenu(mo_macro_1, value) ;
    if (!stricmp(param, "rexxmacro2"))
      RexxMenu(mo_macro_2, value) ;
    if (!stricmp(param, "rexxmacro3"))
      RexxMenu(mo_macro_3, value) ;
    if (!stricmp(param, "rexxmacro4"))
      RexxMenu(mo_macro_4, value) ;
    if (!stricmp(param, "rexxmacro5"))
      RexxMenu(mo_macro_5, value) ;
    if (!stricmp(param, "rexxmacro6"))
      RexxMenu(mo_macro_6, value) ;
    if (!stricmp(param, "rexxmacro7"))
      RexxMenu(mo_macro_7, value) ;
    if (!stricmp(param, "rexxmacro8"))
      RexxMenu(mo_macro_8, value) ;
    if (!stricmp(param, "rexxmacro9"))
      RexxMenu(mo_macro_9, value) ;
    if (!stricmp(param, "rexxmacro10"))
      RexxMenu(mo_macro_10, value) ;
#endif
  }
  fclose(fp);

check_env:
  /* Now check the individual environment variables for overrides. */

  if ((s = getenv ("Mosaic/TempDir")))
    Rdata.tmp_directory = s;
  if (strcmp (Rdata.tmp_directory, "T:") == 0)
    if ((s = getenv ("HOME")))
      Rdata.tmp_directory = s;

  if ((s = getenv ("Mosaic/Sendmail")))
    amiga_sendmail = s;
  if (!amiga_sendmail)
    amiga_sendmail = getenv ("Sendmail");

  if ((s = getenv ("Mosaic/Editor")))
    amiga_editor = s;
  if (!amiga_editor)
    amiga_editor = getenv ("Editor");

  if ((s = getenv ("Mosaic/Telnet")))
    amiga_telnet = s;
  if (!amiga_telnet)
    amiga_telnet = getenv ("Telnet");

  if ((s = getenv ("Mosaic/Rlogin")))
    amiga_rlogin = s;
  if (!amiga_telnet)
    amiga_telnet = getenv ("Rlogin");

  if ((s = getenv("Mosaic/ImageCacheSize")))
    Rdata.image_cache_size = atoi(s);
  if ((s = getenv("Mosaic/DelayImageLoads")))
    Rdata.delay_image_loads = (stricmp(s, "true"))?0:1;
  if ((s = getenv("Mosaic/TitleToWindow")))
    Rdata.title_to_window = (stricmp(s, "true"))?0:1;

	if ((s = getenv("Mosaic/IsindexToForm")))
		Rdata.isindex_to_form = (stricmp(s, "true"))?0:1;
	if ((s = getenv("Mosaic/Buttons"))){
	if(!strnicmp(s, "text",5)) Rdata.image_buttons = 0;
		else if(!strnicmp(s, "images",7)) Rdata.image_buttons = 1;
		else if(!strnicmp(s, "images&text",11)) Rdata.image_buttons = 2;
		}


	if ((s = getenv("Mosaic/Anchor")))
		Rdata.u_anchor = atorgb(s);
	if ((s = getenv("Mosaic/FollowedAnchor")))
		Rdata.f_anchor = atorgb(s);
	if ((s = getenv("Mosaic/ActiveAnchor")))
		Rdata.a_anchor = atorgb(s);

  if ((s = getenv("Mosaic/HomeDocument")))
    Rdata.home_document = s;
  if ((s = getenv("Mosaic/DownloadDir")))
    Rdata.download_dir = s;
#ifdef HAVE_REXX
  if ((s = getenv("Mosaic/AddToHotList")))
    RexxMenu(mo_add_hot_list, s) ;
  if ((s = getenv("Mosaic/JumpToHotList")))
    RexxMenu(mo_jump_hot_list, s) ;
  if ((s = getenv("Mosaic/PrintCommand")))
    RexxMenu(mo_print_document, s) ;
#endif

  if ((s = getenv("Mosaic/Font")))
      Rdata.font = s;
  if ((s = getenv("Mosaic/italicFont")))
      Rdata.italicfont = s;
  if ((s = getenv("Mosaic/boldFont")))
      Rdata.boldfont = s;
  if ((s = getenv("Mosaic/h1Font")))
      Rdata.h1font = s;
  if ((s = getenv("Mosaic/h2Font")))
      Rdata.h2font = s;
  if ((s = getenv("Mosaic/h3Font")))
      Rdata.h3font = s;
  if ((s = getenv("Mosaic/h4Font")))
      Rdata.h4font = s;
  if ((s = getenv("Mosaic/h5Font")))
      Rdata.h5font = s;
  if ((s = getenv("Mosaic/h6Font")))
      Rdata.h6font = s;
  if ((s = getenv("Mosaic/fixedFont")))
      Rdata.fixedfont = s;
  if ((s = getenv("Mosaic/addressFont")))
      Rdata.addressfont = s;
  if ((s = getenv("Mosaic/listingFont")))
      Rdata.listingfont = s;
  if ((s = getenv("Mosaic/plainFont")))
      Rdata.plainfont = s;
#ifdef HAVE_REXX
  if ((s = getenv("Mosaic/rexxmacro1")))
      RexxMenu(mo_macro_1, s) ;
  if ((s = getenv("Mosaic/rexxmacro2")))
      RexxMenu(mo_macro_2, s) ;
  if ((s = getenv("Mosaic/rexxmacro3")))
      RexxMenu(mo_macro_3, s) ;
  if ((s = getenv("Mosaic/rexxmacro4")))
      RexxMenu(mo_macro_4, s) ;
  if ((s = getenv("Mosaic/rexxmacro5")))
      RexxMenu(mo_macro_5, s) ;
  if ((s = getenv("Mosaic/rexxmacro6")))
      RexxMenu(mo_macro_6, s) ;
  if ((s = getenv("Mosaic/rexxmacro7")))
      RexxMenu(mo_macro_7, s) ;
  if ((s = getenv("Mosaic/rexxmacro8")))
      RexxMenu(mo_macro_8, s) ;
  if ((s = getenv("Mosaic/rexxmacro9")))
      RexxMenu(mo_macro_9, s) ;
  if ((s = getenv("Mosaic/rexxmacro10")))
      RexxMenu(mo_macro_10, s) ;
#endif
}
