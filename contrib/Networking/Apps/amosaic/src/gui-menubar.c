/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#include "includes.h"
#include "globals.h"
#include "HTML.h"
#include "mosaic.h"
#if defined(__AROS__)
#   include "HTML.h"
#else
#   include "libhtmlw/HTML.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>

extern char *home_document;

#define MAX_DOCUMENTS_MENU_ITEMS 120
#define DOCUMENTS_MENU_COUNT_OFFSET 5000
#if 0
/* List of URL's matching items in documents menu. */
static char *urllist[MAX_DOCUMENTS_MENU_ITEMS];

  
/* ------------------------------ menubar_cb ------------------------------ */

do_menubar(mo_window *win, int i)
{
  switch (i)
    {
    case mo_reload_document:
      mo_reload_window_text (win, 0);
      break;
    case mo_reload_document_and_images:
      mo_reload_window_text (win, 1);
      break;
    case mo_refresh_document:
      mo_refresh_window_text (win);
      break;
    case mo_re_init:
      mo_re_init_formats ();
      break;
    case mo_clear_image_cache:
      mo_flush_image_cache (win);
      break;
    case mo_document_source:
      mo_post_source_window (win);
      break;
    case mo_search:
      mo_post_search_window (win);
      break;
    case mo_open_document:
      mo_post_open_window (win);
      break;
    case mo_open_local_document:
      mo_post_open_local_window (win);
      break;
    case mo_save_document:
      mo_post_save_window (win);
      break;
    case mo_mail_document:
      mo_post_mail_window (win);
      break;
    case mo_tex_document:
      mo_latex_document (win);
      break;
    case mo_print_document:
      mo_post_print_window (win);
      break;
    case mo_new_window:
      mo_open_another_window (win, home_document, NULL, NULL);
      break;
    case mo_clone_window:
      mo_duplicate_window (win);
      break;
    case mo_close_window:
      mo_delete_window (win);
      break;
    case mo_exit_program:
      mo_post_exitbox ();
      break;
#ifdef HAVE_DTM
    case mo_dtm_open_outport:
      mo_post_dtmout_window (win);
      break;
    case mo_dtm_send_document:
      mo_send_document_over_dtm (win);
      break;
#endif
    case mo_home_document:
      mo_access_document (win, home_document);
      break;
    case mo_network_starting_points:
      mo_access_document (win, NETWORK_STARTING_POINTS_DEFAULT);
      break;
    case mo_internet_metaindex:
      mo_access_document (win, INTERNET_METAINDEX_DEFAULT);
      break;
    case mo_mosaic_demopage:
      mo_open_another_window
        (win, DEMO_PAGE_DEFAULT,
         NULL, NULL);
      break;
    case mo_mosaic_manual:
      mo_open_another_window
        (win, mo_assemble_help_url ("mosaic-docs.html"),
         NULL, NULL);
      break;

    case mo_back:
      mo_back_node (win);
      break;
    case mo_forward:
      mo_forward_node (win);
      break;
    case mo_history_list:
      mo_post_history_win (win);
      break;
    case mo_clear_global_history:
      XmxSetUniqid (win->id);
      XmxMakeQuestionDialog
        (win->base, "Are you sure you want to clear the global history?",
         "NCSA Mosaic: Clear Global History", clear_history_confirm_cb, 1, 0);
      XtManageChild (Xmx_w);
      break;
    case mo_hotlist_postit:
      mo_post_hotlist_win (win);
      break;
    case mo_register_node_in_default_hotlist:
      if (win->current_node)
        {
          mo_add_node_to_default_hotlist (win->current_node);
          mo_write_default_hotlist ();
        }
      break;
    case mo_fancy_selections:
      win->pretty = 1 - win->pretty;
      mo_set_fancy_selections_toggle (win);
      HTMLClearSelection (win->scrolled_win);
      XmxSetArg (WbNfancySelections, win->pretty ? True : False);
      XmxSetValues (win->scrolled_win);
      break;
    case mo_binary_transfer:
      win->binary_transfer =
        (win->binary_transfer ? 0 : 1);
      break;
    case mo_delay_image_loads:
      win->delay_image_loads =
        (win->delay_image_loads ? 0 : 1);
      XmxSetArg (WbNdelayImageLoads, win->delay_image_loads ? True : False);
      XmxSetValues (win->scrolled_win);
      XmxRSetSensitive (win->menubar, mo_expand_images_current,
                        win->delay_image_loads ? XmxSensitive : XmxNotSensitive);
      break;
    case mo_expand_images_current:
      XmxSetArg (WbNdelayImageLoads, False);
      XmxSetValues (win->scrolled_win);
      mo_refresh_window_text (win);
      XmxSetArg (WbNdelayImageLoads, win->delay_image_loads ? True : False);
      XmxSetValues (win->scrolled_win);
      break;
    case mo_large_fonts:
    case mo_regular_fonts:
    case mo_small_fonts:
    case mo_large_helvetica:
    case mo_regular_helvetica:
    case mo_small_helvetica:
    case mo_large_newcentury:
    case mo_regular_newcentury:
    case mo_small_newcentury:
    case mo_large_lucidabright:
    case mo_regular_lucidabright:
    case mo_small_lucidabright:
      mo_set_fonts (win, i);
      break;
    case mo_default_underlines:
    case mo_l1_underlines:
    case mo_l2_underlines:
    case mo_l3_underlines:
    case mo_no_underlines:
      mo_set_underlines (win, i);
      break;
    case mo_help_about:
      mo_open_another_window
        (win, mo_assemble_help_url ("help-about.html"),
         NULL, NULL);
      break;
    case mo_help_onwindow:
      mo_open_another_window
        (win, mo_assemble_help_url ("help-on-docview-window.html"),
         NULL, NULL);
      break;
    case mo_whats_new:
      mo_open_another_window
        (win, WHATSNEW_PAGE_DEFAULT,
         NULL, NULL);
      break;
    case mo_help_onversion:
      mo_open_another_window
        (win, MO_HELP_ON_VERSION_DOCUMENT,
         NULL, NULL);
      break;
    case mo_help_faq:
      mo_open_another_window (win, mo_assemble_help_url ("mosaic-faq.html"), 
                              NULL, NULL);
      break;
    case mo_help_html:
      mo_open_another_window (win, HTMLPRIMER_PAGE_DEFAULT, 
                              NULL, NULL);
      break;
    case mo_help_url:
      mo_open_another_window (win, URLPRIMER_PAGE_DEFAULT, 
                              NULL, NULL);
      break;
    case mo_whine:
      mo_post_whine_win (win);
      break;
    case mo_annotate:
      mo_post_annotate_win (win, 0, 0, NULL, NULL, NULL, NULL);
      break;
#ifdef HAVE_AUDIO_ANNOTATIONS
    case mo_audio_annotate:
      mo_post_audio_annotate_win (win);
      break;
#endif
    case mo_annotate_edit:
      /* OK, let's be smart.
         If we get here, we know we're viewing an editable
         annotation.
         We also know the filename (just strip the leading
         file: off the URL).
         We also know the ID, by virtue of the filename
         (just look for PAN-#.html. */
      if (win->current_node)
        {
          char *author, *title, *text, *fname;
          int id;
          
          if (win->current_node->annotation_type == mo_annotation_private)
            {
              mo_grok_pan_pieces (win->current_node->url,
                                  win->current_node->text,
                                  &title, &author, &text, 
                                  &id, &fname);
              
              mo_post_annotate_win (win, 1, id, title, author, text, fname);
            }
          else if (win->current_node->annotation_type == mo_annotation_workgroup)
            {
              mo_grok_grpan_pieces (win->current_node->url,
                                    win->current_node->text,
                                    &title, &author, &text, 
                                    &id, &fname);
              mo_post_annotate_win (win, 1, id, title, author, text, fname);
            }
        }
      break;
    case mo_annotate_delete:
      if (Rdata.confirm_delete_annotation)
        {
          XmxSetUniqid (win->id);
          XmxMakeQuestionDialog
            (win->base, "Are you sure you want to delete this annotation?",
             "NCSA Mosaic: Delete Annotation", delete_annotation_confirm_cb, 1, 0);
          XtManageChild (Xmx_w);
        }
      else
        mo_do_delete_annotation (win);
      break;
    default:
      if (i >= DOCUMENTS_MENU_COUNT_OFFSET)
        mo_access_document (win, urllist[i - DOCUMENTS_MENU_COUNT_OFFSET]);
      break;
    }

  return;
}
#endif
/* ---------------------------- file_menuspec ----------------------------- */

#if 0
static XmxMenubarStruct file_menuspec[] =
{
  { "New Window",         'N', menubar_cb, mo_new_window },
  { "Clone Window",       'C', menubar_cb, mo_clone_window },
  { "----" },
  { "Open...",            'O', menubar_cb, mo_open_document },
  { "Open Local...",      'L', menubar_cb, mo_open_local_document },
  { "----" },
  { "Save As...",         'A', menubar_cb, mo_save_document },
  { "Mail To...",         'M', menubar_cb, mo_mail_document },
  { "Print...",           'P', menubar_cb, mo_print_document },
#ifdef HAVE_DTM
  { "----" },
  { "Open DTM Outport...",    'T', menubar_cb, mo_dtm_open_outport },
  { "Broadcast Document",     'B', menubar_cb, mo_dtm_send_document },
#endif /* HAVE_DTM */
  { "----" },
  { "Search...",          'S', menubar_cb, mo_search },
  { "Reload",             'R', menubar_cb, mo_reload_document },
  { "Refresh",            'f', menubar_cb, mo_refresh_document },
  { "Source Document...", 'D', menubar_cb, mo_document_source },
  { "----" },
  { "Reload Config Files", 'n', menubar_cb, mo_re_init },
  { "Clear Image Cache",       'I', menubar_cb, mo_clear_image_cache },
  { "----" },
  { "Close Window",       'W', menubar_cb, mo_close_window },
  { "Exit Program...",    'E', menubar_cb, mo_exit_program },
  { NULL },
};

static XmxMenubarStruct navi_menuspec[] =
{
  { "Back",                    'B', menubar_cb, mo_back },
  { "Forward",                 'F', menubar_cb, mo_forward },
  { "----" },   
  { "Open...",                 'O', menubar_cb, mo_open_document },
  { "Open Local...",           'L', menubar_cb, mo_open_local_document },
  { "----" },
  { "Home Document",           'D', menubar_cb, mo_home_document },
  { "Network Starting Points", 'N', menubar_cb, mo_network_starting_points },
  { "----" },   
  { "Window History...",       'W', menubar_cb, mo_history_list },
  { "Clear Global History...",     'C', menubar_cb, mo_clear_global_history },
  { "----" },
  { "Hotlist...",              'H', menubar_cb, mo_hotlist_postit },
  { "Add Document To Hotlist", 'A', menubar_cb, mo_register_node_in_default_hotlist },
  { NULL },
};

static XmxMenubarStruct fnts_menuspec[] =
{
  { "<Large Times Fonts",          'L', menubar_cb, mo_large_fonts },
  { "<Large Helvetica Fonts",      'H', menubar_cb, mo_large_helvetica },
  { "<Large New Century Fonts",    'N', menubar_cb, mo_large_newcentury },
  { "<Large Lucida Bright Fonts",  'B', menubar_cb, mo_large_lucidabright },
  { "----" },
  { "<Small Times Fonts",          'S', menubar_cb, mo_small_fonts },
  { "<Small Helvetica Fonts",      'e', menubar_cb, mo_small_helvetica },
  { "<Small New Century Fonts",    'C', menubar_cb, mo_small_newcentury },
  { "<Small Lucida Bright Fonts",  'F', menubar_cb, mo_small_lucidabright },
  { NULL },
};

static XmxMenubarStruct undr_menuspec[] =
{
  { "<Default Underlines",         'D', menubar_cb, mo_default_underlines },
  { "<Light Underlines",           'L', menubar_cb, mo_l1_underlines },
  { "<Medium Underlines",          'M', menubar_cb, mo_l2_underlines },
  { "<Heavy Underlines",           'H', menubar_cb, mo_l3_underlines },
  { "<No Underlines",              'N', menubar_cb, mo_no_underlines },
  { NULL },
};

static XmxMenubarStruct opts_menuspec[] =
{
  { "#Fancy Selections",           'F', menubar_cb, mo_fancy_selections },
  { "#Binary Transfer Mode",       'B', menubar_cb, mo_binary_transfer },
  { "----" },
  { "<Times (Default) Fonts",      'T', menubar_cb, mo_regular_fonts },
  { "<Helvetica Fonts",            'H', menubar_cb, mo_regular_helvetica },
  { "<New Century Fonts",          'N', menubar_cb, mo_regular_newcentury },
  { "<Lucida Bright Fonts",        'L', menubar_cb, mo_regular_lucidabright },
  { "More Fonts",                  'M', NULL, NULL, fnts_menuspec },
  { "----" },
  { "Anchor Underlines",           'A', NULL, NULL, undr_menuspec }, 
  { NULL },
};

static XmxMenubarStruct help_menuspec[] =
{
  { "About...",      'A', menubar_cb, mo_help_about },
  { "Manual...",     'M', menubar_cb, mo_mosaic_manual },
  { "----" },
  { "What's New...", 'W', menubar_cb, mo_whats_new },
  { "Demo...",       'D', menubar_cb, mo_mosaic_demopage },
  { "----" },
  { "On Version...", 'V', menubar_cb, mo_help_onversion },
  { "On Window...",  'O', menubar_cb, mo_help_onwindow },
  { "On FAQ...",     'F', menubar_cb, mo_help_faq },
  { "----" },
  { "On HTML...",    'H', menubar_cb, mo_help_html },
  { "On URLs...",    'U', menubar_cb, mo_help_url },
  { "----" },
  { "Mail Developers...",   'M', menubar_cb, mo_whine },
  { NULL },
};

static XmxMenubarStruct anno_menuspec[] =
{
  { "Annotate...",              'A', menubar_cb, mo_annotate },
#ifdef HAVE_AUDIO_ANNOTATIONS
  { "Audio Annotate...",        'u', menubar_cb, mo_audio_annotate },
#endif
  { "----" },
  { "Edit This Annotation...",  'E', menubar_cb, mo_annotate_edit },
  { "Delete This Annotation...",'D', menubar_cb, mo_annotate_delete },
  { NULL },
};

static XmxMenubarStruct menuspec[] =
{
  { "File",      'F', NULL, NULL, file_menuspec },
  { "Navigate",  'N', NULL, NULL, navi_menuspec },
  { "Options",   'O', NULL, NULL, opts_menuspec },
  { "Annotate",  'A', NULL, NULL, anno_menuspec },
  { "Help",      'H', NULL, NULL, help_menuspec },
  /* Dummy submenu. */
  { NULL,       NULL, NULL, NULL, NULL },
  { NULL },
};
#endif

/* ------------------------------------------------------------------------ */
/* --------------------------- Colleen menubar ---------------------------- */
/* ------------------------------------------------------------------------ */

#if 0
static XmxMenubarStruct file_menuspecc[] =
{
  { "New Window",         'N', menubar_cb, mo_new_window },
  { "Clone Window",       'C', menubar_cb, mo_clone_window },
  { "----" },
  { "Open URL...",        'O', menubar_cb, mo_open_document },
  { "Open Local...",      'L', menubar_cb, mo_open_local_document },
  { "----" },
  { "Reload Current",     'R', menubar_cb, mo_reload_document },
  { "Reload Images",      'a', menubar_cb, mo_reload_document_and_images },
  { "Refresh Current",    'f', menubar_cb, mo_refresh_document },
  { "----" },
  { "Find In Current...", 'I', menubar_cb, mo_search },
  { "View Source...",     'V', menubar_cb, mo_document_source },
  { "----" },
  { "Save As...",         'S', menubar_cb, mo_save_document },
  { "Print...",           'P', menubar_cb, mo_print_document },
  { "Mail To...",         'M', menubar_cb, mo_mail_document },
#ifdef HAVE_DTM
  { "----" },
  { "Open DTM Outport...",'D', menubar_cb, mo_dtm_open_outport },
  { "Broadcast Over DTM", 'B', menubar_cb, mo_dtm_send_document },
#endif /* HAVE_DTM */
  { "----" },
  { "Close Window",       'W', menubar_cb, mo_close_window },
  { "Exit Program...",    'E', menubar_cb, mo_exit_program },
  { NULL },
};

static XmxMenubarStruct fnts_menuspec[] =
{
  { "<Times Regular",              'T', menubar_cb, mo_regular_fonts },
  { "<Times Small",                'S', menubar_cb, mo_small_fonts },
  { "<Times Large",                'L', menubar_cb, mo_large_fonts },
  { "----" },
  { "<Helvetica Regular",          'H', menubar_cb, mo_regular_helvetica },
  { "<Helvetica Small",            'e', menubar_cb, mo_small_helvetica },
  { "<Helvetica Large",            'v', menubar_cb, mo_large_helvetica },
  { "----" },
  { "<New Century Regular",        'N', menubar_cb, mo_regular_newcentury },
  { "<New Century Small",          'w', menubar_cb, mo_small_newcentury },
  { "<New Century Large",          'C', menubar_cb, mo_large_newcentury },
  { "----" },
  { "<Lucida Bright Regular",      'L', menubar_cb, mo_regular_lucidabright },
  { "<Lucida Bright Small",        'u', menubar_cb, mo_small_lucidabright },
  { "<Lucida Bright Large",        'i', menubar_cb, mo_large_lucidabright },
  { NULL },
};

static XmxMenubarStruct undr_menuspec[] =
{
  { "<Default Underlines",         'D', menubar_cb, mo_default_underlines },
  { "<Light Underlines",           'L', menubar_cb, mo_l1_underlines },
  { "<Medium Underlines",          'M', menubar_cb, mo_l2_underlines },
  { "<Heavy Underlines",           'H', menubar_cb, mo_l3_underlines },
  { "<No Underlines",              'N', menubar_cb, mo_no_underlines },
  { NULL },
};

static XmxMenubarStruct opts_menuspec[] =
{
  { "#Fancy Selections",           'S', menubar_cb, mo_fancy_selections },
  { "----" },
  { "#Load To Local Disk",         'T', menubar_cb, mo_binary_transfer },
  { "----" },
  { "#Delay Image Loading",        'D', menubar_cb, mo_delay_image_loads },
  { "Load Images In Current",      'L', menubar_cb, mo_expand_images_current },
  { "----" },
  { "Reload Config Files",         'R', menubar_cb, mo_re_init },
  { "----" },
  { "Flush Image Cache",           'I', menubar_cb, mo_clear_image_cache },
  { "Clear Global History...",     'C', menubar_cb, mo_clear_global_history },
  { "----" },
  { "Fonts",                       'F', NULL, NULL, fnts_menuspec },
  { "Anchor Underlines",           'A', NULL, NULL, undr_menuspec }, 
  { NULL },
};

static XmxMenubarStruct navi_menuspec[] =
{
  { "Back",                    'B', menubar_cb, mo_back },
  { "Forward",                 'F', menubar_cb, mo_forward },
  { "----" },   
  { "Home Document",           'D', menubar_cb, mo_home_document },
  { "Window History...",       'W', menubar_cb, mo_history_list },
  { "----" },   
  { "Hotlist...",              'H', menubar_cb, mo_hotlist_postit },
  { "Add Current To Hotlist",  'A', menubar_cb, mo_register_node_in_default_hotlist },
  { "----" },   
  { "Internet Starting Points",      'I', menubar_cb, mo_network_starting_points },
  { "Internet Resources Meta-Index", 'M', menubar_cb, mo_internet_metaindex },
  { NULL },
};

static XmxMenubarStruct help_menuspec[] =
{
  { "About...",      'A', menubar_cb, mo_help_about },
  { "Manual...",     'M', menubar_cb, mo_mosaic_manual },
  { "----" },
  { "What's New...", 'W', menubar_cb, mo_whats_new },
  { "Demo...",       'D', menubar_cb, mo_mosaic_demopage },
  { "----" },
  { "On Version...", 'V', menubar_cb, mo_help_onversion },
  { "On Window...",  'O', menubar_cb, mo_help_onwindow },
  { "On FAQ...",     'F', menubar_cb, mo_help_faq },
  { "----" },
  { "On HTML...",    'H', menubar_cb, mo_help_html },
  { "On URLs...",    'U', menubar_cb, mo_help_url },
  { "----" },
  { "Mail Developers...",   'M', menubar_cb, mo_whine },
  { NULL },
};

static XmxMenubarStruct anno_menuspec[] =
{
  { "Annotate...",              'A', menubar_cb, mo_annotate },
#ifdef HAVE_AUDIO_ANNOTATIONS
  { "Audio Annotate...",        'u', menubar_cb, mo_audio_annotate },
#endif
  { "----" },
  { "Edit This Annotation...",  'E', menubar_cb, mo_annotate_edit },
  { "Delete This Annotation...",'D', menubar_cb, mo_annotate_delete },
  { NULL },
};

static XmxMenubarStruct menuspec[] =
{
  { "File",      'F', NULL, NULL, file_menuspec },
  { "Options",   'O', NULL, NULL, opts_menuspec },
  { "Navigate",  'N', NULL, NULL, navi_menuspec },
  { "Annotate",  'A', NULL, NULL, anno_menuspec },
  { "Help",      'H', NULL, NULL, help_menuspec },
  /* Dummy submenu. */
  { NULL,       NULL, NULL, NULL, NULL },
  { NULL },
};
#endif

/* ----------------------- simple menubar interface ----------------------- */

#if 0

static XmxMenubarStruct file_simple_menuspec[] =
{
  { "Clone Window",       'C', menubar_cb, mo_clone_window },
  { "----" },
  { "Search...",          'S', menubar_cb, mo_search },
  { "----" },
  { "Close Window",       'W', menubar_cb, mo_close_window },
  { "Exit Program...",    'E', menubar_cb, mo_exit_program },
  { NULL },
};

static XmxMenubarStruct navi_simple_menuspec[] =
{
  { "Back",                    'B', menubar_cb, mo_back },
  { "Forward",                 'F', menubar_cb, mo_forward },
  { "----" },   
  { "Home Document",           'D', menubar_cb, mo_home_document },
  { "----" },   
  { "Window History...",       'W', menubar_cb, mo_history_list },
  { "----" },
  { "Hotlist...",              'H', menubar_cb, mo_hotlist_postit },
  { "Add Document To Hotlist", 'A', menubar_cb, mo_register_node_in_default_hotlist },
  { NULL },
};

static XmxMenubarStruct opts_simple_menuspec[] =
{
  { "#Binary Transfer Mode",       'B', menubar_cb, mo_binary_transfer },
  { NULL },
};

static XmxMenubarStruct help_simple_menuspec[] =
{
  { "About...",      'A', menubar_cb, mo_help_about },
  { "Manual...",     'M', menubar_cb, mo_mosaic_manual },
  { "----" },
  { "What's New...", 'W', menubar_cb, mo_whats_new },
  { "Demo...",       'D', menubar_cb, mo_mosaic_demopage },
  { "----" },
  { "On Version...", 'V', menubar_cb, mo_help_onversion },
  { "On Window...",  'O', menubar_cb, mo_help_onwindow },
  { "On FAQ...",     'F', menubar_cb, mo_help_faq },
  { NULL },
};

static XmxMenubarStruct anno_simple_menuspec[] =
{
  { "Annotate...",              'A', menubar_cb, mo_annotate },
#ifdef HAVE_AUDIO_ANNOTATIONS
  { "Audio Annotate...",        'u', menubar_cb, mo_audio_annotate },
#endif
  { "----" },
  { "Edit This Annotation...",  'E', menubar_cb, mo_annotate_edit },
  { "Delete This Annotation...",'D', menubar_cb, mo_annotate_delete },
  { NULL },
};

static XmxMenubarStruct simple_menuspec[] =
{
  { "File",      'F', NULL, NULL, file_simple_menuspec },
  { "Navigate",  'N', NULL, NULL, navi_simple_menuspec },
  { "Options",   'O', NULL, NULL, opts_simple_menuspec },
  { "Annotate",  'A', NULL, NULL, anno_simple_menuspec },
  { "Help",      'H', NULL, NULL, help_simple_menuspec },
  /* Dummy submenu. */
  { NULL,       NULL, NULL, NULL, NULL },
  { NULL },
};

#endif

#ifndef __AROS__
/* -------------------- mo_make_document_view_menubar --------------------- */

/* We now allow a single customizable menu.  

   First choice for the spec file is the value of the resource
   documentsMenuSpecfile.
   If that doesn't exist, second choice is the value of the
   environment variable MOSAIC_DOCUMENTS_MENU_SPECFILE.
   If *that* doesn't exist, third choice is specified in 
   #define DOCUMENTS_MENU_SPECFILE. */

static mo_status mo_file_exists (char *filename)
{
  struct stat buf;
  int r;

  r = stat (filename, &buf); 
  if (r != -1)
    return mo_succeed;
  else
    return mo_fail;
}

static void mo_grok_menubar (char *filename)
{
  FILE *fp;
  char line[MO_LINE_LENGTH];
  char *status;
  XmxMenubarStruct *menu;
  int count;

  fp = fopen (filename, "r");
  if (!fp)
    goto screwed_no_file;

  /* Make the menu. */
  menu = (XmxMenubarStruct *) malloc 
    (MAX_DOCUMENTS_MENU_ITEMS * sizeof (XmxMenubarStruct));
  count = 0;
  
  /* File consists of alternating titles and URL's.
     A title consisting of at least two leading dashes
     is a separator. */
  while (1)
    {
      status = fgets (line, MO_LINE_LENGTH, fp);
      if (!status || !(*line))
        goto done;

      if (strlen (line) >= 2 &&
          line[0] == '-' &&
          line[1] == '-')
        {
          /* It's a separator. */
          menu[count].namestr = "----";
          menu[count].mnemonic = 0;
          menu[count].func = 0;
          menu[count].data = 0;
          menu[count].sub_menu = 0;
        }
      else
        {
          /* That's the title. */
          menu[count].namestr = strdup (line);
          /* Wipe out trailing newline. */
          menu[count].namestr[strlen(line)-1] = '\0';
          menu[count].mnemonic = 0;
          menu[count].func = (void (*)())menubar_cb;
          menu[count].data = count + DOCUMENTS_MENU_COUNT_OFFSET;
          menu[count].sub_menu = 0;
          
          status = fgets (line, MO_LINE_LENGTH, fp);
          if (!status || !(*line))
            {
              /* Oops, something went wrong. */
              menu[count].namestr = 0;
              
              goto done;
            }
          
          /* There's a URL. */
          urllist[count] = strdup (line);
          urllist[count][strlen(line)-1] = '\0';
        }

      /* Count increases. */
      count++;
    }

 done:
  /* Make sure we get a NULL in the right place. */
  menu[count].namestr = 0;
  fclose (fp);
  if (count > 0)
    {
      if (Rdata.simple_interface)
        {
          simple_menuspec[5].namestr = "Documents";
          simple_menuspec[5].mnemonic = 'D';
          simple_menuspec[5].func = 0;
          simple_menuspec[5].data = 0;
          simple_menuspec[5].sub_menu = menu;
        }
      else
        {
          menuspec[5].namestr = "Documents";
          menuspec[5].mnemonic = 'D';
          menuspec[5].func = 0;
          menuspec[5].data = 0;
          menuspec[5].sub_menu = menu;
        }
    }
  return;

 screwed_open_file:
  fclose (fp);
  return;

 screwed_no_file:
  return;
}

static void mo_try_to_grok_menubar (void)
{
  char *filename = Rdata.documents_menu_specfile;
  
  if (filename && mo_file_exists (filename))
    {
      mo_grok_menubar (filename);
    }
  else
    {
      filename = getenv ("MOSAIC_DOCUMENTS_MENU_SPECFILE");
      if (filename && mo_file_exists (filename))
        {
          mo_grok_menubar (filename);
        }
      else
        {
          filename = DOCUMENTS_MENU_SPECFILE;
          if (filename && mo_file_exists (filename))
            {
              mo_grok_menubar (filename);
            }
        }
    }

  return;
}

static int grokked_menubar = 0;

XmxMenuRecord *mo_make_document_view_menubar (Widget form)
{
  /* If we've never tried to set up the user's configurable menubar by
     loading menuspec[5], give it a shot now. */
  if (!grokked_menubar)
    {
      grokked_menubar = 1;

      mo_try_to_grok_menubar ();
    }

  return XmxRMakeMenubar 
    (form, Rdata.simple_interface ? simple_menuspec : menuspec);
}
#endif
