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


#include "HTML.h"
#include "mosaic.h"
#include <time.h>
#if !defined(_AMIGA) && !defined(__AROS__)
#include <Xm/List.h>
#endif
#include <pwd.h>
#include <sys/types.h>

#include "bitmaps/hotlist.xbm"

/* This file provides support for hotlists of interesting
   documents within the browser.
   
   Initially there will be a single hotlist, 'Default'.

   Initially the hotlist file format will look like this:
   
   ncsa-mosaic-hotlist-format-1            [identifying string]
   Default                                 [title]
   url Fri Sep 13 00:00:00 1986            [first word is url;
                                            subsequent words are
                                            last-accessed date (GMT)]
   document title cached here              [cached title for above]
   [2-line sequence for single document repeated as necessary]
   ...
   
   Turns out this format is bad for two reasons:
   (1) Document titles can have embedded carriage returns (usually
       on purpose).
   (2) URL's can have embedded carriage returns (usually on accident).

   Also, we should just be using an HTML-derived format for hotlists.
*/

#define NCSA_HOTLIST_FORMAT_COOKIE_ONE \
  "ncsa-xmosaic-hotlist-format-1"
  
static mo_hotlist *default_hotlist = NULL;

/*
 * Given a hotlist and a hotnode, append the node
 * to the hotlist.
 * Change fields nodelist and nodelist_last in the hotlist,
 * and fields next and previous in the hotnode.
 * Also fill in field position in the hotnode.
 * Return nothing.
 */
static void mo_append_hotnode_to_hotlist (mo_hotlist *list, 
                                          mo_hotnode *node)
{
  if (list->nodelist == 0)
    {
      /* Nothing yet. */
      list->nodelist = node;
      list->nodelist_last = node;
      node->next = 0;
      node->previous = 0;
      node->position = 1;
    }
  else
    {
      /* The new node becomes nodelist_last. */
      /* But first, set up node. */
      node->previous = list->nodelist_last;
      node->next = 0;
      node->position = node->previous->position + 1;
      
      /* Now point forward from previous nodelist_last. */
      list->nodelist_last->next = node;
      
      /* Now set up new nodelist_last. */
      list->nodelist_last = node;
    }
  
  return;
}

/* --------------- mo_delete_position_from_default_hotlist ---------------- */

/* Given a hotlist and a hotnode, rip the hotnode out of the hotlist.
   No check is made as to whether the hotnode is actually in the hotlist;
   it better be. */
static void mo_remove_hotnode_from_hotlist (mo_hotlist *list,
                                            mo_hotnode *hotnode)
{
  if (hotnode->previous == NULL)
    {
      /* Node was the first member of the list. */
      if (hotnode->next != NULL)
        {
          /* Node was the first member of the list and had
             a next node. */
          /* The next node is now the first node in the list. */
          hotnode->next->previous = NULL;
          list->nodelist = hotnode->next;
        }
      else
        {
          /* Node was the first member of the list and
             didn't have a next node. */
          /* The list is now empty. */
          list->nodelist = NULL;
          list->nodelist_last = NULL;
        }
    }
  else
    {
      /* Node had a previous. */
      if (hotnode->next != NULL)
        {
          /* Node had a previous and a next. */
          hotnode->previous->next = hotnode->next;
          hotnode->next->previous = hotnode->previous;
        }
      else
        {
          /* Node had a previous but no next. */
          hotnode->previous->next = NULL;
          list->nodelist_last = hotnode->previous;
        }
    }
  
  return;
}

/* Go through a hotlist (sequence of hotnodes) and assign position
   numbers for all of 'em. */
static void mo_recalculate_hotlist_positions (mo_hotlist *list)
{
  mo_hotnode *hotnode;
  int count = 1;
  
  for (hotnode = list->nodelist; hotnode != NULL;
       hotnode = hotnode->next)
    hotnode->position = count++;
  
  return;
}

/*
 * Delete an element of the default hotlist.
 * The element is referenced by its position.
 * Algorithm for removal:
 *   Find hotnode with the position.
 *   Remove the hotnode from the hotlist data structure.
 *   Recalculate positions of the hotlist.
 *   Remove the element in the position in the list widgets.
 * Return status.
 */
mo_status mo_delete_position_from_default_hotlist (int position)
{
  mo_hotlist *list = default_hotlist;
  mo_hotnode *hotnode;
  mo_window *win = NULL;
  
  for (hotnode = list->nodelist; hotnode != NULL;
       hotnode = hotnode->next)
    {
      if (hotnode->position == position)
        goto foundit;
    }
  
  return mo_fail;
  
  /* OK, now we have hotnode loaded. */
 foundit:
  /* Pull the hotnode out of the hotlist. */
  mo_remove_hotnode_from_hotlist (list, hotnode);
  free (hotnode);
  /* Recalculate positions in this hotlist. */
  mo_recalculate_hotlist_positions (list);
  
  /* Do the GUI stuff. */
  while (win = mo_next_window (win))
    {
      if (win->hotlist_list)
        XmListDeletePos (win->hotlist_list, position);
    }
  
  return mo_succeed;
}

/* ------------------- mo_edit_title_in_default_hotlist ------------------- */
static XmxCallback (edithot_cb)
{
  mo_window *win = mo_fetch_window_by_id 
    (XmxExtractUniqid ((int)client_data));
  char *title;

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->edithot_win);
      title = XmxTextGetString (win->edithot_text);
      {
        /* OK, now position is still cached in win->edithot_pos. */
        mo_hotlist *list = default_hotlist;
        mo_hotnode *hotnode;
        
        for (hotnode = list->nodelist; hotnode != NULL;
             hotnode = hotnode->next)
          {
            if (hotnode->position == win->edithot_pos)
              goto foundit;
          }
      foundit:
        if (hotnode == NULL)
          goto punt;
        if (hotnode->position != win->edithot_pos)
          goto punt;

        /* OK, now we have the hotnode. */
        hotnode->title = title;
        
        /* Save the hotlist before we screw something up. */
        mo_write_default_hotlist ();

        /* Change the extant hotlists. */
        {
          mo_window *win = NULL;

          while (win = mo_next_window (win))
            if (win->hotlist_list)
              {
                XmString xmstr =
                  XmxMakeXmstrFromString (Rdata.display_urls_not_titles ? 
                                          hotnode->url : hotnode->title);
                XmListDeletePos
                  (win->hotlist_list,
                   hotnode->position);
                XmListAddItemUnselected 
                  (win->hotlist_list, 
                   xmstr, 
                   hotnode->position);
                XmStringFree (xmstr);
              }
        }
        
        /* That's it! */
      }
  
    punt:
      break;
    case 1:
      XtUnmanageChild (win->edithot_win);
      /* Do nothing. */
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("help-on-hotlist-view.html"), 
         NULL, NULL);
      break;
    }

  return;
}

/* If it don't exist, make it... */
static mo_status mo_create_edithot_win (mo_window *win)
{
  Widget dialog_frame;
  Widget dialog_sep, buttons_form;
  Widget eht_form, title_label;
  
  XmxSetUniqid (win->id);
  win->edithot_win = XmxMakeFormDialog 
    (win->hotlist_win, "NCSA Mosaic: Edit Hotlist Entry Title");
  dialog_frame = XmxMakeFrame (win->edithot_win, XmxShadowOut);
  
  /* Constraints for win->edithot_win. */
  XmxSetConstraints 
    (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
     XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
  
  /* Main form. */
  eht_form = XmxMakeForm (dialog_frame);
  
  title_label = XmxMakeLabel (eht_form, "Entry Title: ");
  XmxSetArg (XmNwidth, 335);
  win->edithot_text = XmxMakeTextField (eht_form);
  XmxAddCallbackToText (win->edithot_text, edithot_cb, 0);
  
  dialog_sep = XmxMakeHorizontalSeparator (eht_form);
  
  buttons_form = XmxMakeFormAndThreeButtons
    (eht_form, edithot_cb, "Commit", "Dismiss", "Help...", 0, 1, 2);
  
  /* Constraints for eht_form. */
  XmxSetOffsets (title_label, 14, 0, 10, 0);
  XmxSetConstraints
    (title_label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
     NULL, NULL, NULL, NULL);
  XmxSetOffsets (win->edithot_text, 10, 0, 5, 10);
  XmxSetConstraints
    (win->edithot_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
     XmATTACH_FORM, NULL, NULL, title_label, NULL);
  
  XmxSetArg (XmNtopOffset, 10);
  XmxSetConstraints 
    (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
     XmATTACH_FORM,
     win->edithot_text, buttons_form, NULL, NULL);
  XmxSetConstraints 
    (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
     XmATTACH_FORM,
     NULL, NULL, NULL, NULL);
  
  return mo_succeed;
}

  
static mo_status mo_do_edit_hotnode_title_win (mo_window *win, 
                                               char *title, int position)
{
  /* This shouldn't happen. */
  if (!win->hotlist_win)
    return mo_fail;
  
  if (!win->edithot_win)
    mo_create_edithot_win (win);

  /* Cache the position. */
  win->edithot_pos = position;

  /* Insert this title as a starting point. */
  XmxTextSetString (win->edithot_text, title);

  /* Manage the little sucker. */
  XtManageChild (win->edithot_win);
  
  return mo_succeed;
}


/*
 * Edit the title of an element of the default hotlist.
 * The element is referenced by its position.
 * Algorithm for edit:
 *   Find hotnode with the position.
 *   Change the title.
 *   Cause redisplay.
 * Return status.
 */
mo_status mo_edit_title_in_default_hotlist (mo_window *win, int position)
{
  mo_hotlist *list = default_hotlist;
  mo_hotnode *hotnode;
  
  for (hotnode = list->nodelist; hotnode != NULL;
       hotnode = hotnode->next)
    {
      if (hotnode->position == position)
        goto foundit;
    }
  
  return mo_fail;
  
  /* OK, now we have hotnode loaded. */
 foundit:
  /* hotnode->title is the current title.
     hotnode->position is the current position. */
  mo_do_edit_hotnode_title_win (win, hotnode->title, hotnode->position);
  
  return mo_succeed;
}

/* --------------------------- mo_read_hotlist ---------------------------- */

/*
 * Read a hotlist from a file.
 * Return pointer to a mo_hotlist structure, fully loaded
 * and ready to go.
 * Return NULL if file does not exist or is not readable.
 */
mo_hotlist *mo_read_hotlist (char *filename)
{
  mo_hotlist *list = NULL;
  FILE *fp;
  char line[MO_LINE_LENGTH];
  char *status;
  
  fp = fopen (filename, "r");
  if (!fp)
    goto screwed_no_file;
  
  status = fgets (line, MO_LINE_LENGTH, fp);
  if (!status || !(*line))
    goto screwed_open_file;
  
  /* See if it's our format. */
  if (strncmp (line, NCSA_HOTLIST_FORMAT_COOKIE_ONE,
               strlen (NCSA_HOTLIST_FORMAT_COOKIE_ONE)))
    goto screwed_open_file;
  
  /* Hey, whaddaya know, it is. */
  list = (mo_hotlist *)malloc (sizeof (mo_hotlist));
  list->nodelist = list->nodelist_last = 0;
  list->filename = filename;
  list->modified = 1;
  list->next = 0;
  
  /* Go fetch the name on the next line. */
  status = fgets (line, MO_LINE_LENGTH, fp);
  if (!status || (!*line))
    {
      free (list);
      list = NULL;
      goto screwed_open_file;
    }
  list->name = strtok (line, "\n");
  if (!list->name)
    {
      free (list);
      list = NULL;
      goto screwed_open_file;
    }      
  list->name = strdup (list->name);

  /* Start grabbing documents. */
  while (1)
    {
      mo_hotnode *node;
      
      status = fgets (line, MO_LINE_LENGTH, fp);
      if (!status || !(*line))
        goto done;
      
      /* We've got a new node. */
      node = (mo_hotnode *)malloc (sizeof (mo_hotnode));
      
      node->url = strtok (line, " ");
      if (!node->url)
        goto screwed_open_file;
      node->url = strdup (node->url);
      mo_convert_newlines_to_spaces (node->url);

      node->lastdate = strtok (NULL, "\n");
      if (!node->lastdate)
        goto screwed_open_file;
      node->lastdate = strdup (node->lastdate);
      
      status = fgets (line, MO_LINE_LENGTH, fp);
      if (!status || !(*line))
        {
          /* Oops, something went wrong. */
          free (node->url);
          free (node->lastdate);
          free (node);
          goto done;
        }
      
      node->title = strtok (line, "\n");
      if (!node->title)
        goto screwed_open_file;
      node->title = strdup (node->title);
      mo_convert_newlines_to_spaces (node->url);
      
      mo_append_hotnode_to_hotlist (list, node);
    }
  
 done:
  fclose (fp);
  return list;

 screwed_open_file:
  fclose (fp);
  return list;

 screwed_no_file:
  return NULL;
}

/*
 * Create a new mo_hotlist.
 * Pass in the new filename and the new title.
 */
mo_hotlist *mo_new_hotlist (char *filename, char *title)
{
  mo_hotlist *list;
  
  list = (mo_hotlist *)malloc (sizeof (mo_hotlist));
  list->nodelist = list->nodelist_last = 0;
  list->filename = filename;
  list->modified = 1;
  list->next = 0;
  list->name = strdup (title);
  return list;
}

/*
 * Write a hotlist out to a file.
 * Return mo_succeed if everything goes OK;
 * mo_fail else.
 */
mo_status mo_write_hotlist (mo_hotlist *list)
{
  FILE *fp;
  mo_hotnode *node;
  
  fp = fopen (list->filename, "w");
  if (!fp)
    return mo_fail;
  
  fprintf (fp, "%s\n%s\n", NCSA_HOTLIST_FORMAT_COOKIE_ONE, list->name);
  
  for (node = list->nodelist; node != NULL; node = node->next)
    fprintf (fp, "%s %s\n%s\n", node->url, node->lastdate, node->title);

  fclose (fp);
  
  /* We can consider it unmodified since latest write now. */
  list->modified = 0;
  return mo_succeed;
}

/*
 * Write a hotlist out to stdout.
 * Return mo_succeed if everything goes OK;
 * mo_fail else.
 */
mo_status mo_dump_hotlist (mo_hotlist *list)
{
  FILE *fp;
  mo_hotnode *node;
  
  fp = stdout;
  
  fprintf (fp, NCSA_HOTLIST_FORMAT_COOKIE_ONE);
  fprintf (fp, "\n");
  
  for (node = list->nodelist; node != NULL; node = node->next)
    {
      fprintf (fp, "%s %s\n%s\n", node->url, node->lastdate, node->title);
    }
  
  return mo_succeed;
}

/* ------------------------------------------------------------------------ */
/* ----------------------------- HOTLIST GUI ------------------------------ */
/* ------------------------------------------------------------------------ */

/* Initial GUI support for hotlist will work like this:

   There will be a single hotlist, called 'Default'.
   It will be persistent across all windows.

   Upon program startup an attempt will be made to load it out
   of its file; if this attempt isn't successful, it just plain
   doesn't exist yet.  Bummer.
   
   Upon program exit it will be stored to its file.
*/

/*
 * Called on initialization.  
 * Tries to load the default hotlist.
 */
mo_status mo_setup_default_hotlist (void)
{
  char *home = getenv ("HOME");
  char *default_filename = Rdata.default_hotlist_file;
  char *filename;
  
  /* This shouldn't happen. */
  if (!home)
    home = "/tmp";
  
  filename = (char *)malloc 
    ((strlen (home) + strlen (default_filename) + 8) * sizeof (char));
  sprintf (filename, "%s/%s\0", home, default_filename);
  
  /* Try to load the default hotlist. */
  default_hotlist = mo_read_hotlist (filename);
  /* Doesn't exist?  Bummer.  Make a new one. */
  if (!default_hotlist)
    default_hotlist = mo_new_hotlist (filename, "Default");
  
  return mo_succeed;
}

/*
 * Called on program exit.
 * Tries to write the default hotlist.
 */
mo_status mo_write_default_hotlist (void)
{
  mo_write_hotlist (default_hotlist);
  
  return mo_succeed;
}

/* ------------------------------------------------------------------------ */
/* ------------------------- gui support routines ------------------------- */
/* ------------------------------------------------------------------------ */

/* We've just init'd a new hotlist list widget; look at the default
   hotlist and load 'er up. */
static void mo_load_hotlist_list (mo_window *win, Widget list)
{
  mo_hotnode *node;
  
  for (node = default_hotlist->nodelist; node != NULL; node = node->next)
    {
      XmString xmstr = 
        XmxMakeXmstrFromString (Rdata.display_urls_not_titles ? 
                                node->url : node->title);
      XmListAddItemUnselected 
        (list, xmstr, 0);
      XmStringFree (xmstr);
    }

  return;
}

static void mo_visit_hotlist_position (mo_window *win, int position)
{
  mo_hotnode *hotnode;

  for (hotnode = default_hotlist->nodelist; hotnode != NULL;
       hotnode = hotnode->next)
    {
      if (hotnode->position == position)
        mo_access_document (win, hotnode->url);
    }
  
  return;
}

/* ----------------------------- mail hotlist ----------------------------- */

static XmxCallback (mailhot_win_cb)
{
  mo_window *win = mo_fetch_window_by_id 
    (XmxExtractUniqid ((int)client_data));
  char *to, *subj, *fnam, *cmd;
  FILE *fp;

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->mailhot_win);

      mo_busy ();

      to = XmxTextGetString (win->mailhot_to_text);
      if (!to)
        return;
      if (to[0] == '\0')
        return;

      subj = XmxTextGetString (win->mailhot_subj_text);

      fnam = mo_tmpnam();

      fp = fopen (fnam, "w");
      if (!fp)
        goto oops;

      {
        mo_hotnode *node;
        struct passwd *pw = getpwuid (getuid ());
        char *author;
  
        if (Rdata.default_author_name)
          author = Rdata.default_author_name;
        else
          author = pw->pw_gecos;
        
        fprintf (fp, "<H1>Hotlist From %s</H1>\n", author);
        fprintf (fp, "<DL>\n");
        for (node = default_hotlist->nodelist; node != NULL; node = node->next)
          {
            fprintf (fp, "<DT>%s\n<DD><A HREF=\"%s\">%s</A>\n", 
                     node->title, node->url, node->url);
          }
        fprintf (fp, "</DL>\n");
      }
        
      fclose (fp);

      cmd = (char *)malloc 
        ((strlen (Rdata.mail_command) + strlen (subj) + strlen (to)
                             + strlen (fnam) + 24));
      sprintf (cmd, "%s -s \"%s\" %s < %s", 
               Rdata.mail_command, subj, to, fnam);
      if ((system (cmd)) != 0)
        {
          XmxMakeErrorDialog 
            (win->base,
             "Unable to mail hotlist;\ncheck the value of the X resource\nmailCommand.", 
             "Mail Hotlist Error");
          XtManageChild (Xmx_w);
        }
      free (cmd);

    oops:
      free (to);
      free (subj);

      cmd = (char *)malloc ((strlen (fnam) + 32) * sizeof (char));
      sprintf (cmd, "/bin/rm -f %s &", fnam);
      system (cmd);

      free (fnam);
      free (cmd);

      mo_not_busy ();
            
      break;
    case 1:
      XtUnmanageChild (win->mailhot_win);
      /* Do nothing. */
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("help-on-hotlist-view.html"), 
         NULL, NULL);
      break;
    }

  return;
}

static mo_status mo_post_mailhot_win (mo_window *win)
{
  /* This shouldn't happen. */
  if (!win->hotlist_win)
    return mo_fail;

  if (!win->mailhot_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form;
      Widget mailhot_form, to_label, subj_label;
      
      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      win->mailhot_win = XmxMakeFormDialog 
        (win->hotlist_win, "NCSA Mosaic: Mail Hotlist");
      dialog_frame = XmxMakeFrame (win->mailhot_win, XmxShadowOut);

      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      mailhot_form = XmxMakeForm (dialog_frame);
      
      to_label = XmxMakeLabel (mailhot_form, "Mail To: ");
      XmxSetArg (XmNwidth, 335);
      win->mailhot_to_text = XmxMakeTextField (mailhot_form);
      
      subj_label = XmxMakeLabel (mailhot_form, "Subject: ");
      win->mailhot_subj_text = XmxMakeTextField (mailhot_form);

      dialog_sep = XmxMakeHorizontalSeparator (mailhot_form);
      
      buttons_form = XmxMakeFormAndThreeButtons
        (mailhot_form, mailhot_win_cb, "Mail", "Dismiss", "Help...", 0, 1, 2);

      /* Constraints for mailhot_form. */
      XmxSetOffsets (to_label, 14, 0, 10, 0);
      XmxSetConstraints
        (to_label, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_NONE,
         NULL, NULL, NULL, NULL);
      XmxSetOffsets (win->mailhot_to_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->mailhot_to_text, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, NULL, NULL, to_label, NULL);

      XmxSetOffsets (subj_label, 14, 0, 10, 0);
      XmxSetConstraints
        (subj_label, XmATTACH_WIDGET, XmATTACH_NONE, XmATTACH_FORM, 
         XmATTACH_NONE,
         win->mailhot_to_text, NULL, NULL, NULL);
      XmxSetOffsets (win->mailhot_subj_text, 10, 0, 5, 10);
      XmxSetConstraints
        (win->mailhot_subj_text, XmATTACH_WIDGET, 
         XmATTACH_NONE, XmATTACH_WIDGET,
         XmATTACH_FORM, win->mailhot_to_text, NULL, subj_label, NULL);

      XmxSetArg (XmNtopOffset, 10);
      XmxSetConstraints 
        (dialog_sep, XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, 
         XmATTACH_FORM,
         win->mailhot_subj_text, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
    }
  
  XtManageChild (win->mailhot_win);
  
  return mo_succeed;
}

/* ---------------------------- hotlist_win_cb ---------------------------- */

static XmxCallback (hotlist_win_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));

  switch (XmxExtractToken ((int)client_data))
    {
    case 0:
      XtUnmanageChild (win->hotlist_win);
      /* Dismissed -- do nothing. */
      break;
    case 1:
      mo_post_mailhot_win (win);
      break;
    case 2:
      mo_open_another_window
        (win, 
         mo_assemble_help_url ("help-on-hotlist-view.html"),
         NULL, NULL);
      break;
    case 3:
      /* Add current. */
      mo_add_node_to_default_hotlist (win->current_node);
      mo_write_default_hotlist ();
      break;
    case 4:
      /* Goto selected. */
      {
        Boolean rv;
        int *pos_list;
        int pos_cnt;
        rv = XmListGetSelectedPos (win->hotlist_list, &pos_list, &pos_cnt);
        if (rv && pos_cnt)
          {
            mo_visit_hotlist_position (win, pos_list[0]);
          }
        else
          {
            XmxMakeErrorDialog
              (win->hotlist_win, "No entry in the hotlist is currently selected.\n\nTo go to an entry in the hotlist,\nselect it with a single mouse click\nand press the Go To button again.", "Error: Nothing Selected");
            XtManageChild (Xmx_w);
          }
      }
      break;
    case 5:
      /* Remove selected. */
      {
        Boolean rv;
        int *pos_list;
        int pos_cnt;
        rv = XmListGetSelectedPos (win->hotlist_list, &pos_list, &pos_cnt);
        if (rv && pos_cnt)
          {
            mo_delete_position_from_default_hotlist (pos_list[0]);
            mo_write_default_hotlist ();
          }
        else
          {
            XmxMakeErrorDialog
              (win->hotlist_win, "No entry in the hotlist is currently selected.\n\nTo remove an entry in the hotlist,\nselect it with a single mouse click\nand press the Remove button again.", "Error: Nothing Selected");
            XtManageChild (Xmx_w);
          }
      }
      break;
    case 6:
      /* Edit title of selected. */
      {
        Boolean rv;
        int *pos_list;
        int pos_cnt;
        rv = XmListGetSelectedPos (win->hotlist_list, &pos_list, &pos_cnt);
        if (rv && pos_cnt)
          {
            mo_edit_title_in_default_hotlist (win, pos_list[0]);
            /* Writing the default hotlist should take place in the callback. */
            /* mo_write_default_hotlist (); */
          }
        else
          {
            XmxMakeErrorDialog
              (win->hotlist_win, "No entry in the hotlist is currently selected.\n\nTo edit the title of an entry in the hotlist,\nselect it with a single mouse click\nand press the Edit Title button again.", "Error: Nothing Selected");
            XtManageChild (Xmx_w);
          }
      }
      break;
    }

  return;
}

static XmxCallback (hotlist_list_cb)
{
  mo_window *win = mo_fetch_window_by_id (XmxExtractUniqid ((int)client_data));
  XmListCallbackStruct *cs = (XmListCallbackStruct *)call_data;
  
  mo_visit_hotlist_position (win, cs->item_position);
  
  /* Don't unmanage the list. */
  
  return;
}

/* ------------------------- mo_post_hotlist_win -------------------------- */

/*
 * Pop up a hotlist window for an mo_window.
 */
mo_status mo_post_hotlist_win (mo_window *win)
{
  if (!win->hotlist_win)
    {
      Widget dialog_frame;
      Widget dialog_sep, buttons_form, buttons1_form;
      Widget hotlist_form, buttons1_frame;
      Widget label, logo;
      XtTranslations listTable;
      static char listTranslations[] =
	"~Shift ~Ctrl ~Meta ~Alt <Btn2Down>: ListBeginSelect() \n\
	 ~Shift ~Ctrl ~Meta ~Alt <Btn2Up>:   ListEndSelect() ListKbdActivate()";

      listTable = XtParseTranslationTable(listTranslations);

      /* Create it for the first time. */
      XmxSetUniqid (win->id);
      XmxSetArg (XmNwidth, 475);
      XmxSetArg (XmNheight, 352);
      win->hotlist_win = XmxMakeFormDialog 
        (win->base, "NCSA Mosaic: Hotlist View");
      dialog_frame = XmxMakeFrame (win->hotlist_win, XmxShadowOut);
      
      /* Constraints for base. */
      XmxSetConstraints 
        (dialog_frame, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM, XmATTACH_FORM, NULL, NULL, NULL, NULL);
      
      /* Main form. */
      hotlist_form = XmxMakeForm (dialog_frame);

      label = XmxMakeLabel (hotlist_form, "Default Hotlist:");

      buttons1_form = XmxMakeFormAndFourButtons
        (hotlist_form, hotlist_win_cb, "Add Current", "Go To",
         "Remove", "Edit Title", 3, 4, 5, 6);

      logo = XmxMakeNamedLabel (hotlist_form, NULL, "logo");
      XmxApplyBitmapToLabelWidget
        (logo, hotlist_bits, hotlist_width, hotlist_height);
      
      /* Hotlist list itself. */
      XmxSetArg (XmNresizable, False);
      XmxSetArg (XmNscrollBarDisplayPolicy, XmSTATIC);
      XmxSetArg (XmNlistSizePolicy, XmCONSTANT);
      win->hotlist_list = 
        XmxMakeScrolledList (hotlist_form, hotlist_list_cb, 0);
      XtAugmentTranslations (win->hotlist_list, listTable);

      dialog_sep = XmxMakeHorizontalSeparator (hotlist_form);
      
      buttons_form = XmxMakeFormAndThreeButtons
        (hotlist_form, hotlist_win_cb, "Dismiss", "Mail To...", "Help...", 
         0, 1, 2);
      
      /* Constraints for hotlist_form. */
      /* Label: top to nothing, bottom to buttons1_form,
         left to form, right to nothing. */
      XmxSetOffsets (label, 0, 5, 7, 10);
      XmxSetConstraints
        (label, XmATTACH_NONE, XmATTACH_WIDGET, XmATTACH_FORM,
         XmATTACH_NONE, NULL, buttons1_form, NULL, NULL);
      /* buttons1_form: top to nothing, bottom to hotlist_list,
         left to form, right to logo. */
      XmxSetOffsets (buttons1_form, 0, 2, 2, 0);
      XmxSetConstraints
        (buttons1_form, XmATTACH_NONE, XmATTACH_WIDGET, XmATTACH_FORM,
         XmATTACH_WIDGET, 
         NULL, XtParent (win->hotlist_list), NULL, logo);
      /* Logo: top to form, bottom to nothing,
         left to nothing, right to form. */
      XmxSetOffsets (logo, 2, 0, 0, 4);
      XmxSetConstraints
        (logo, XmATTACH_FORM, XmATTACH_NONE, XmATTACH_NONE, XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
      /* list: top to logo, bottom to dialog_sep,
         etc... */
      XmxSetOffsets (XtParent (win->hotlist_list), 10, 10, 10, 10);
      XmxSetConstraints
        (XtParent (win->hotlist_list), 
         XmATTACH_WIDGET, XmATTACH_WIDGET, XmATTACH_FORM, XmATTACH_FORM, 
         logo, dialog_sep, NULL, NULL);
      XmxSetConstraints 
        (dialog_sep, 
         XmATTACH_NONE, XmATTACH_WIDGET, XmATTACH_FORM, XmATTACH_FORM,
         NULL, buttons_form, NULL, NULL);
      XmxSetConstraints 
        (buttons_form, XmATTACH_NONE, XmATTACH_FORM, XmATTACH_FORM, 
         XmATTACH_FORM,
         NULL, NULL, NULL, NULL);
      
      /* Go get the hotlist up to this point set up... */
      mo_load_hotlist_list (win, win->hotlist_list);
    }
  
  XtManageChild (win->hotlist_win);
  
  return mo_succeed;
}

/* -------------------- mo_add_node_to_default_hotlist -------------------- */

mo_status mo_add_node_to_hotlist (mo_hotlist *list, mo_node *node)
{
  mo_hotnode *hotnode = (mo_hotnode *)malloc (sizeof (mo_hotnode));
  mo_window *win = NULL;
  time_t foo = time (NULL);
  char *ts = ctime (&foo);
  
  ts[strlen(ts)-1] = '\0';

  if (node->title)
    hotnode->title = strdup (node->title);
  else
    hotnode->title = strdup ("Unnamed");
  mo_convert_newlines_to_spaces (hotnode->title);

  hotnode->url = strdup (node->url);
  mo_convert_newlines_to_spaces (hotnode->url);

  hotnode->lastdate = strdup (ts);
  
  mo_append_hotnode_to_hotlist (list, hotnode);
  
  /* Now we've got to update all active hotlist_list's. */
  while (win = mo_next_window (win))
    {
      if (win->hotlist_list)
        {
          XmString xmstr = 
            XmxMakeXmstrFromString (Rdata.display_urls_not_titles ? 
                                    hotnode->url : hotnode->title);
          XmListAddItemUnselected 
            (win->hotlist_list, 
             xmstr, 
             hotnode->position);
          XmStringFree (xmstr);
          XmListSetBottomPos (win->hotlist_list, 0);
        }
    }
  
  return mo_succeed;
}

mo_status mo_add_node_to_default_hotlist (mo_node *node)
{
  return mo_add_node_to_hotlist (default_hotlist, node);
}
