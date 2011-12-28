// MDF - PORT FROM NCSA VERSION 2.1
// Some functions still need work...

#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"
#include "htmlgad.h"
#include "gui.h"
#include "XtoI.h"
#include "protos.h"

extern void ResetAmigaGadgets(void);

extern char *cached_url;
extern int binary_transfer;
extern char *startup_document, *home_document;
extern void ShowHtml_RG(void);
// extern Display *dsp;

int loading_inlined_images = 0;
char *url_base_override = NULL;
int interrupted = 0;
#define mui_version (MUIMasterBase->lib_Version)

extern char *mo_post_pull_er_over (char *url, char *content_type, 
                                   char *post_data, 
                                   char **texthead);

/****************************************************************************
 * name:    mo_snarf_scrollbar_values
 * purpose: Store current viewing state in the current node, in case
 *          we want to return to the same location later.
 * inputs:  
 *   - mo_window *win: Current window.
 * returns: 
 *   mo_succeed
 *   (mo_fail if no current node exists)
 * remarks: 
 *   Snarfs current docid position in the HTML widget.
 ****************************************************************************/
mo_status mo_snarf_scrollbar_values (mo_window *win)
{
  HTMLGadClData *HTML_Data=INST_DATA(HTMLGadClass,HTML_Gad);
  /* Make sure we have a node. */
  if (!win->current_node)
    return mo_fail;

  win->current_node->docid = HTMLPositionToId(win->scrolled_win, 0, HTML_Data->scroll_y);

  /* Do the cached stuff thing. */
  win->current_node->cached_stuff = HTMLGetWidgetInfo (win->scrolled_win);

  return mo_succeed;
}


/* ---------------------- mo_reset_document_headers ----------------------- */

static mo_status mo_reset_document_headers (mo_window *win)
{
	if (win->current_node){
		if(TX_Title)
			set(TX_Title, MUIA_Text_Contents, win->current_node->title);
		else
			set(WI_Main, MUIA_Window_Title, win->current_node->title);

		set(TX_URL, MUIA_String_Contents, win->current_node->url);
		}
	return mo_succeed;
}

/* --------------------------- mo_back_possible --------------------------- */

/* This could be cached, but since it shouldn't take too long... */
static void mo_back_possible (mo_window *win)
{
  set(BT_Back, MUIA_Disabled, FALSE);
  DoMethod(App, MUIM_Application_SetMenuState, mo_back, TRUE);
  return;
}


/****************************************************************************
 * name:    mo_back_impossible
 * purpose: Can't go back (nothing in the history list).
 ****************************************************************************/
mo_status mo_back_impossible (mo_window *win)
{
  set(BT_Back, MUIA_Disabled, TRUE);
  DoMethod(App, MUIM_Application_SetMenuState, mo_back, FALSE);
  return mo_succeed;
}

static void mo_forward_possible (mo_window *win)
{
  set(BT_Forward, MUIA_Disabled, FALSE);
  DoMethod(App, MUIM_Application_SetMenuState, mo_forward, TRUE);
  return;
}


/****************************************************************************
 * name:    mo_forward_impossible
 * purpose: Can't go forward (nothing in the history list).
 ****************************************************************************/
mo_status mo_forward_impossible (mo_window *win)
{
  set(BT_Forward, MUIA_Disabled, TRUE);
  DoMethod(App, MUIM_Application_SetMenuState, mo_forward, FALSE);
  return mo_succeed;
}

/* ---------------------- mo_annotate_edit_possible ----------------------- */

#if 0
static void mo_annotate_edit_possible (mo_window *win)
{
//  XmxRSetSensitive (win->menubar, mo_annotate_edit, XmxSensitive);
//  XmxRSetSensitive (win->menubar, mo_annotate_delete, XmxSensitive);
  return;
}

static void mo_annotate_edit_impossible (mo_window *win)
{
//  XmxRSetSensitive (win->menubar, mo_annotate_edit, XmxNotSensitive);
//  XmxRSetSensitive (win->menubar, mo_annotate_delete, XmxNotSensitive);
  return;
}
#endif

/* ------------------------------------------------------------------------ */

static void mo_set_text (Object *HTML_Gad, char *txt, char *ans, int id, 
                         char *target_anchor, void *cached_stuff)
{
  HTMLGadClData *HTML_Data =INST_DATA(HTMLGadClass,HTML_Gad);
  

  /* Any data transfer that takes place in here must be inlined
     image loading. */
  loading_inlined_images = 1;
  interrupted = 0;
  mo_set_image_cache_nuke_threshold ();
  if (Rdata.annotations_on_top)
    HTMLSetText (HTML_Data, txt, ans ? ans : "\0", "\0", id, target_anchor, 
                 cached_stuff);
  else
    HTMLSetText (HTML_Data, txt, "\0", ans ? ans : "\0", id, target_anchor, 
                 cached_stuff);
  loading_inlined_images = 0;
  interrupted = 0;
  mo_gui_done_with_icon ();
}


/****************************************************************************
 * name:    mo_do_window_text (PRIVATE)
 * purpose: Set a window's text and do lots of other housekeeping
 *          and GUI-maintenance things.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - char      *url: The URL for the text; assumed to be canonicalized
 *                     and otherwise ready for inclusion in history lists,
 *                     the window's overhead URL display, etc.
 *   - char      *txt: The new text for the window.
 *   - char  *txthead: The start of the malloc'd block of text corresponding
 *                     to txt.
 *   - int register_visit: If TRUE, then this text should be registerd
 *                         as a new node in the history list.  If FALSE,
 *                         then we're just moving around in the history list.
 *   - char      *ref: Reference (possible title) for this text.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This is the mother of all functions in Mosaic.  Probably should be
 *   rethought and broken down.
 ****************************************************************************/
static mo_status mo_do_window_text (mo_window *win, char *url, char *txt,
                                    char *txthead,
                                    int register_visit, char *ref)
{
  char *ans;

  mo_set_current_cached_win (win);

  if (Rdata.track_pointer_motion)
    {
/*
      XmString xmstr = XmStringCreateLtoR (" ", XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues
        (win->tracker_label,
         XmNlabelString, (XtArgVal)xmstr,
         NULL);
      XmStringFree (xmstr);
*/
    }
      
  /* If !register_visit, we're just screwing around with current_node
     already, so don't bother snarfing scrollbar values. */
  if (register_visit)
    mo_snarf_scrollbar_values (win);

  /* cached_url HAS to be set here, since Resolve counts on it. */
  cached_url = mo_url_canonicalize (url, "");
  win->cached_url = cached_url;

  mo_here_we_are_son (url);
  
  {
    /* Since mo_fetch_annotation_links uses the communications code,
       we need to play games with binary_transfer. */
    int tmp = binary_transfer;
    binary_transfer = 0;
    ans = NULL;
//    ans = mo_fetch_annotation_links (url, Rdata.annotations_on_top);
    binary_transfer = tmp;
  }

  /* If there is a BASE tag in the document that contains a "real"
     URL, this will be non-NULL by the time we exit and base_callback
     will have been called. */
  url_base_override = NULL;

  {
    int id = 0, freeta = 0;
    void *cached_stuff = NULL;
    char *target_anchor = win->target_anchor;

    if ((!register_visit) && win->current_node)
      {
        id = win->current_node->docid;
        cached_stuff = win->current_node->cached_stuff;
      }

    /* If the window doesn't have a target anchor already,
       see if there's one in this node's URL. */
    if ((!target_anchor || !(*target_anchor)) && win->current_node)
      {
        target_anchor = mo_url_extract_anchor (win->current_node->url);
        freeta = 1;
      }

    if (!txt || !txthead)
      {
#if 0
        txt = strdup ("<h1>Uh oh, I'm stumped</h1> Mosaic has encountered a situation in which it doesn't know what to do.  Please back up and try again, or send a bug report including full details about what you're trying to do to <b>mosaic-x@ncsa.uiuc.edu</b>.  We thank you for your support.");
#endif
        /* Just make it look OK...  band-aid city. */
        txt = strdup ("\0");
        txthead = txt;
      }

    mo_set_text (win->HTML_Gad, txt, ans, id, target_anchor,
                 cached_stuff);

    if (win->target_anchor)
      free (win->target_anchor);

    win->target_anchor = NULL;

    if (freeta)
      free (target_anchor);
  }

  if (url_base_override)
    {
      /* Get the override URL -- this should be all we need to do here. */
      url = url_base_override;
      mo_here_we_are_son (url);
    }

  /* Every time we view the document, we reset the search_start
     struct so searches will start at the beginning of the document. */
  TO_HTML_reset_search(win);
  /* CURRENT_NODE ISN'T SET UNTIL HERE (assuming register_visit is 1). */
  /* Now that WbNtext has been set, we can pull out WbNtitle. */
  /* First, check and see if we have a URL.  If not, we probably
     are only jumping around inside a document. */
  if (url && *url)
    {
      if (register_visit)
        mo_record_visit (win, url, txt, txthead, ref);
      else
        {
          /* At the very least we want to pull out the new title,
             if one exists. */
          if (win->current_node)
            {
              if (win->current_node->title)
                free (win->current_node->title);
              win->current_node->title = mo_grok_title (win, url, ref);
            }
        }
    }
  
  mo_reset_document_headers (win);

  if (win->history_list && win->current_node)
    {
      int n = 0, i;

      get(win->history_list, MUIA_List_Entries, &n);
      for (i=0; i<n; i++) {
	DoMethod(win->history_list, MUIM_List_Select, i,
		 MUIV_List_Select_Off, NULL);
      }
      set(win->history_list, MUIA_List_Active, MUIV_List_Active_Off);
      DoMethod(win->history_list, MUIM_List_Select,
	       win->current_node->position - 1, MUIV_List_Select_On, NULL);
    }

  /* Update source text if necessary. */
  if (win->source_text && win->current_node)
    {
      set(win->source_text, MUIA_Floattext_Text, win->current_node->text);
    }

  if (win->current_node && win->current_node->previous != NULL)
    mo_back_possible (win);
  else
    mo_back_impossible (win);
  
  if (win->current_node && win->current_node->next != NULL)
    mo_forward_possible (win);
  else
    mo_forward_impossible (win);

#if 0
  if (win->current_node && 
      mo_is_editable_annotation (win, win->current_node->text))
    mo_annotate_edit_possible (win);
  else
    mo_annotate_edit_impossible (win);
#endif

  mo_not_busy ();

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_set_win_current_node
 * purpose: Given a window and a node, set the window's current node.
 *          This assumes node is already all put together, in the history
 *          list for the window, etc.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - mo_node  *node: The node to use.
 * returns: 
 *   Result of calling mo_do_window_text.
 * remarks: 
 *   This routine is meant to be used to move forward, backward,
 *   and to arbitrarily locations in the history list.
 ****************************************************************************/

mo_status mo_set_win_current_node (mo_window *win, mo_node *node)
{
//  void *to_free = NULL;
  mo_status r;

  mo_snarf_scrollbar_values (win);

  if (win->current_node && win->reloading)
    {
//      to_free = win->current_node->cached_stuff;

      win->current_node->cached_stuff = NULL;
    }

  win->current_node = node;

  mo_busy ();
  mo_set_current_cached_win (win);
  r = mo_do_window_text (win, win->current_node->url, 
                         win->current_node->text, 
			 win->current_node->texthead,
			 FALSE, win->current_node->ref);

  if (win->reloading)
    {
//      if (to_free)
//        HTMLFreeWidgetInfo (to_free);
      
      win->reloading = 0;
    }

  ShowHtml_RG();

  return r;
}


/****************************************************************************
 * name:    mo_reload_window_text
 * purpose: Reload the current window's text by pulling it over the
 *          network again.
 * inputs:  
 *   - mo_window *win: The current window.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This frees the current window's texthead.  This calls mo_pull_er_over
 *   directly, and needs to be smarter about handling HDF, etc.
 ****************************************************************************/
mo_status mo_reload_window_text (mo_window *win, int reload_images_also)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  /* Uh oh, this is trouble... */
  if (!win->current_node)
    return mo_load_window_text 
      (win, startup_document ? startup_document : home_document, NULL);

  /* Free all images in the current document. */
  if (Rdata.reload_reloads_images || reload_images_also)
    mo_zap_cached_images_here (win);

  /* Free the current document's text. */
  /* REALLY we shouldn't do this until we've verified we have new text that's
     actually good here -- for instance, if we have a document on display,
     then go to binary transfer mode, then do reload, we should pick up the
     access override here and keep the old copy up on screen. */
  if (win->current_node->texthead != NULL)
    {
      free (win->current_node->texthead);
      win->current_node->texthead = NULL;
    }

  /* Set binary_transfer as per current window. */
  binary_transfer = win->binary_transfer;
  mo_set_current_cached_win (win);
  interrupted = 0;
  win->current_node->text = mo_pull_er_over (win->current_node->url, 
                                             &win->current_node->texthead);
  {
    /* Check use_this_url_instead from HTAccess.c. */
    /* IS THIS GOOD ENOUGH FOR THIS CASE??? */
    extern __far  char *use_this_url_instead;
    if (use_this_url_instead)
      {
        win->current_node->url = use_this_url_instead;
      }
  }
#ifdef HAVE_HDF
  if (win->current_node->text && 
      strncmp (win->current_node->text, "<mosaic-internal-reference", 26) == 0)
    {
      char *text = mo_decode_internal_reference 
        (win->current_node->url, win->current_node->text,
         mo_url_extract_anchor (win->current_node->url));
      win->current_node->text = text;
      win->current_node->texthead = text;
    }
#endif 
  
  /* Clear out the cached stuff, if any exists. */
  win->reloading = 1;

  mo_set_win_current_node (win, win->current_node);

  win->reloading = 0;

//  ResetAmigaGadgets();

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_refresh_window_text
 * purpose: Reload the current window's text without pulling it over the net.
 * inputs:  
 *   - mo_window *win: The current window.
 * returns: 
 *   mo_succeed
 * remarks: 
 ****************************************************************************/
mo_status mo_refresh_window_text (mo_window *win)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  if (!win->current_node)
    {
      mo_not_busy ();
      return mo_fail;
    }

  /* Clear out the cached stuff, if any exists. */
  win->reloading = 1;

  mo_set_win_current_node (win, win->current_node);

  mo_not_busy ();

  return mo_succeed;
}

/* 22.4.95 (The day after the sale of Amiga Inc) */
/* SB's note: I ran a diff on the next function vs v2.4 of XMosaic and I */
/* found no obvious changes. They have just added pgp encryption, and I  */
/* am leaving that out! I won't implement that. */

/****************************************************************************
 * name:    mo_load_window_text
 * purpose: Given a window and a raw URL, load the window.  The window
 *          is assumed to already exist with a document inside, etc.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - char      *url: The URL to load.
 *   - char      *ref: The reference ("parent") URL.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This is getting ugly.
 ****************************************************************************/
mo_status mo_load_window_text (mo_window *win, char *url, char *ref)
{
  char *newtext = NULL, *newtexthead = NULL;

  mo_busy ();

  win->target_anchor = mo_url_extract_anchor (url);

  /* If we're just referencing an anchor inside a document,
     do the right thing. */
  if (url && *url == '#')
    {
      /* Now we make a copy of the current text and make sure we ask
         for a new mo_node and entry in the history list. */
      /* IF we're not dealing with an internal reference. */
      if (strncmp (url, "#hdfref;", 8) &&
          strncmp (url, "#hdfdtm;", 8))
        {
          if (win->current_node)
            {
              newtext = strdup (win->current_node->text);
              newtexthead = newtext;
            }
          else
            {
              newtext = strdup ("lose");
              newtexthead = newtext;
            }
        }
      url = mo_url_canonicalize_keep_anchor 
        (url, win->current_node ? win->current_node->url : "");
    }
  else
    {
      /* Get a full address for this URL. */
      /* Under some circumstances we may not have a current node yet
         and may wish to just run with it... so check for that. */
      if (win->current_node && win->current_node->url)
        {
          url = mo_url_canonicalize_keep_anchor (url, win->current_node->url);
        }

      /* Set binary_transfer as per current window. */
      binary_transfer = win->binary_transfer;
      mo_set_current_cached_win (win);

      {
        char *canon = mo_url_canonicalize (url, "");
        interrupted = 0;
        newtext = mo_pull_er_over (canon, &newtexthead);
        free (canon);
      }

      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            
            /* Go get another target_anchor. */
            if (win->target_anchor)
              free (win->target_anchor);
            win->target_anchor = mo_url_extract_anchor (url);
          }
      }
    }

#ifdef HAVE_HDF
  /* If a target anchor exists, and if it's an HDF reference, then
     go decode the HDF reference and call mo_do_window_text on the
     resulting text. */
  if (win->target_anchor &&
      strncmp (win->target_anchor, "hdfref;", 7) == 0 &&
      strlen (win->target_anchor) > 8)
    {
      char *text;
      text = (char *)mo_decode_hdfref (url, win->target_anchor);
      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            mo_load_window_text(win, url, ref);
            return;
          }
      }
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  /* An hdfdtm reference means that we should blast the referenced
     HDF data object over the output DTM port to Collage.  Currently
     this can only be an image; in the future we'll do SDS's, etc. */
  else if (win->target_anchor &&
           strncmp (win->target_anchor, "hdfdtm;", 7) == 0 &&
           strlen (win->target_anchor) > 8)
    {
#ifdef HAVE_DTM
      /* We specifically want to make sure that the anchor is allowed
         to stay in the URL, so we don't canonicalize to strip it out. */
      mo_do_hdf_dtm_thang (url, &(win->target_anchor[7]));
#endif

      if (win->target_anchor)
        free (win->target_anchor);
      win->target_anchor = NULL;

      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  /* Assuming we have HDF, the only thing mosaic-internal-reference
     currently can be is pointer to an HDF file. */
  else if (newtext && strncmp (newtext, "<mosaic-internal-reference", 26) == 0)
    {
      char *text;
      text = mo_decode_internal_reference (url, newtext, win->target_anchor);
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  else
#endif
  /* Now, if it's a telnet session, there should be no need
     to do anything else.  Also check for override in text itself. */
  if (strncmp (url, "telnet:", 7) == 0 || strncmp (url, "tn3270:", 7) == 0 ||
      strncmp (url, "rlogin:", 7) == 0 ||
      (newtext && strncmp (newtext, "<mosaic-access-override>", 24) == 0))
    {
      /* We don't need this anymore. */
      free (newtext);

      /* We still want a global history entry but NOT a 
         window history entry. */
      mo_here_we_are_son (url);
      /* ... and we want to redisplay the current window to
         get the effect of the history entry today, not tomorrow. */
      mo_redisplay_window (win);
      /* We're not busy anymore... */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  else if (newtext)
    {
      /* Not a telnet session and not an override, but text present
         (the "usual" case): */

      /* Set the window text. */
      mo_do_window_text (win, url, newtext, newtexthead, 1, ref);
    }
  else
    {
      /* No text at all. */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }

//	if(!inst->have_forms)
		ShowHtml_RG();

  if (win->delay_image_loads == 0) {
    LoadInlinedImages();
  }

//	if(inst->have_forms)
//		ShowHtml_RG();

  return mo_succeed;
}


/* 22.4.95 The following function is taken from NCSA XMosaic 2.4 */
/* I have removed the code dealing with PGP encryption as I see no */
/* obvious way in implementing it in the rest of the code */
/* /SB */

mo_status mo_post_load_window_text (mo_window *win, char *url, 
                                    char *content_type, char *post_data, 
                                    char *ref)
{
  char *newtext = NULL, *newtexthead = NULL;
  HTMLGadClData *inst = INST_DATA(HTMLGadClass,HTML_Gad);

  mo_busy ();

  win->target_anchor = mo_url_extract_anchor (url);

  /* If we're just referencing an anchor inside a document,
     do the right thing. */
  if (url && *url == '#')
    {
      /* Now we make a copy of the current text and make sure we ask
         for a new mo_node and entry in the history list. */
      /* IF we're not dealing with an internal reference. */
      if (strncmp (url, "#hdfref;", 8) &&
          strncmp (url, "#hdfdtm;", 8))
        {
          if (win->current_node)
            {
              newtext = strdup (win->current_node->text);
              newtexthead = newtext;
            }
          else
            {
              newtext = strdup ("lose");
              newtexthead = newtext;
            }
        }
      url = mo_url_canonicalize_keep_anchor 
        (url, win->current_node ? win->current_node->url : "");
    }
  else
    {
      /* Get a full address for this URL. */
      /* Under some circumstances we may not have a current node yet
         and may wish to just run with it... so check for that. */
      if (win->current_node && win->current_node->url)
        {
          url = mo_url_canonicalize_keep_anchor (url, win->current_node->url);
        }

      /* Set binary_transfer as per current window. */
      binary_transfer = win->binary_transfer;
      mo_set_current_cached_win (win);

      {
        char *canon = mo_url_canonicalize (url, "");
        interrupted = 0;

        newtext = mo_post_pull_er_over (canon, content_type, post_data, 
                                        &newtexthead);
        free (canon);
      }

      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
          }
      }
    }

#ifdef HAVE_HDF
  /* If a target anchor exists, and if it's an HDF reference, then
     go decode the HDF reference and call mo_do_window_text on the
     resulting text. */
  if (win->target_anchor &&
      strncmp (win->target_anchor, "hdfref;", 7) == 0 &&
      strlen (win->target_anchor) > 8)
    {
      char *text;
      text = (char *)mo_decode_hdfref (url, win->target_anchor);
      {
        /* Check use_this_url_instead from HTAccess.c. */
        extern char *use_this_url_instead;
        if (use_this_url_instead)
          {
            mo_here_we_are_son (url);
            url = use_this_url_instead;
            mo_load_window_text(win, url, ref);
            return;
          }
      }
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  /* An hdfdtm reference means that we should blast the referenced
     HDF data object over the output DTM port to Collage.  Currently
     this can only be an image; in the future we'll do SDS's, etc. */
  else if (win->target_anchor &&
           strncmp (win->target_anchor, "hdfdtm;", 7) == 0 &&
           strlen (win->target_anchor) > 8)
    {
#ifdef HAVE_DTM
      /* We specifically want to make sure that the anchor is allowed
         to stay in the URL, so we don't canonicalize to strip it out. */
      mo_do_hdf_dtm_thang (url, &(win->target_anchor[7]));
#endif

      if (win->target_anchor)
        free (win->target_anchor);
      win->target_anchor = NULL;

      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  /* Assuming we have HDF, the only thing mosaic-internal-reference
     currently can be is pointer to an HDF file. */
  else if (newtext && strncmp (newtext, "<mosaic-internal-reference", 26) == 0)
    {
      char *text;
      text = mo_decode_internal_reference (url, newtext, win->target_anchor);
      mo_do_window_text (win, url, text, text, 1, ref);
    }
  else
#endif
  /* Now, if it's a telnet session, there should be no need
     to do anything else.  Also check for override in text itself. */
  if (strncmp (url, "telnet:", 7) == 0 || strncmp (url, "tn3270:", 7) == 0 ||
      strncmp (url, "rlogin:", 7) == 0 ||
      (newtext && strncmp (newtext, "<mosaic-access-override>", 24) == 0))
    {
      /* We don't need this anymore. */
      free (newtext);

      /* We still want a global history entry but NOT a 
         window history entry. */
      mo_here_we_are_son (url);
      /* ... and we want to redisplay the current window to
         get the effect of the history entry today, not tomorrow. */
      mo_redisplay_window (win);
      /* We're not busy anymore... */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }
  else if (newtext)
    {
      /* Not a telnet session and not an override, but text present
         (the "usual" case): */

      /* Set the window text. */
      mo_do_window_text (win, url, newtext, newtexthead, 1, ref);
    }
  else
    {
      /* No text at all. */
      mo_gui_done_with_icon ();
      mo_not_busy ();
    }

	if(!inst->have_forms)
		ShowHtml_RG();


  if (win->delay_image_loads == 0) {
    LoadInlinedImages();
  }

	if(inst->have_forms)
		ShowHtml_RG();

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_duplicate_window_text
 * purpose: Given an old window and a new window, make a copy of the text
 *          in the old window and install it in the new window.
 * inputs:  
 *   - mo_window *oldw: The old window.
 *   - mo_window *neww: The new window.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This is how windows are cloned: a new window is created and this
 *   call sets up its contents.
 ****************************************************************************/
mo_status mo_duplicate_window_text (mo_window *oldw, mo_window *neww)
{
#if !defined(_AMIGA) && !defined(__AROS__)
  /* We can get away with just cloning text here and forgetting
     about texthead, obviously, since we're making a new copy. */
  char *newtext;

  if (!oldw->current_node)
    return mo_fail;

  newtext = strdup (oldw->current_node->text);

  mo_do_window_text 
    (neww, strdup (oldw->current_node->url), 
     newtext, newtext, TRUE, 
     oldw->current_node->ref ? strdup (oldw->current_node->ref) : NULL);

#endif
  return mo_succeed;
}


/****************************************************************************
 * name:    mo_access_document
 * purpose: Given a URL, access the document by loading the current 
 *          window's text.
 * inputs:  
 *   - mo_window *win: The current window.
 *   - char      *url: URL to access.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   This should be the standard call for accessing a document.
 ****************************************************************************/
mo_status mo_access_document (mo_window *win, char *url)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  mo_load_window_text (win, url, NULL);

  return mo_succeed;
}


mo_status mo_post_access_document (mo_window *win, char *url,
                                   char *content_type, char *post_data)
{
  mo_busy ();

  mo_set_current_cached_win (win);

  mo_post_load_window_text (win, url, content_type, post_data, NULL);

  return mo_succeed;
}
