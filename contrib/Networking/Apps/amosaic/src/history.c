// MDF - PORT FROM NCSA VERSION 2.1

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
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"
#include "htmlgad.h"
#include "gui.h"
#include "XtoI.h"
#include "protos.h"

extern IPTR HookEntry();

/* ------------------------------------------------------------------------ */
/* ----------------------------- HISTORY LIST ----------------------------- */
/* ------------------------------------------------------------------------ */

/* ---------------------------- kill functions ---------------------------- */

/* Free the data contained in an mo_node.  Currently we only free
   the text itself. */
mo_status mo_free_node_data (mo_node *node)
{
  if (node->texthead != NULL)
    {
      free (node->texthead);
      node->texthead = NULL;
    }
  if (node->title != NULL)
    {
      free (node->title);
      node->title = NULL;
    }
#if 0
  /* Leads to memory getting freed twice in some cases, apparently.
     Not sure why. */
  if (node->url != NULL)
    free (node->url);
#endif
  if (node->ref != NULL)
    {
      free (node->ref);
      node->ref = NULL;
    }

  if (node->cached_stuff)
    {
//      HTMLFreeWidgetInfo (node->cached_stuff);
      node->cached_stuff = NULL;
    }

  return mo_succeed;
}

/* Kill a single mo_node associated with a given mo_window; it
   the history list exists we delete it from that.  In any case
   we call mo_free_node_data and return. */
mo_status mo_kill_node (mo_window *win, mo_node *node)
{
  if (win->history_list)
    DoMethod(win->history_list, MUIM_List_Remove, node->position - 1);

  mo_free_node_data (node);

  return mo_succeed;
}

/* Iterate through all descendents of an mo_node, but not the given
   mo_node itself, and kill them.  This is equivalent to calling
   mo_kill_node on each of those nodes, except this is faster since
   all the Motif list entries can be killed at once. */
mo_status mo_kill_node_descendents (mo_window *win, mo_node *node)
{
  mo_node *foo;
  int count = 0;

  if (node == NULL)
    return mo_fail;
  for (foo = node->next; foo != NULL; foo = foo->next)
    {
      mo_free_node_data (foo);
      count++;
    }

  /* Free count number of items from the end of the list... */
  if (win->history_list && count)
    {
      int i;
      for (i=0; i<count; i++)
	DoMethod(win->history_list, MUIM_List_Remove, MUIV_List_Remove_Last);
    }

  return mo_succeed;
}

/* ------------------------ mo_add_node_to_history ------------------------ */

/* Called from mo_record_visit to insert an mo_node into the history
   list of an mo_window. */
mo_status mo_add_node_to_history (mo_window *win, mo_node *node)
{
  /* If there is no current node, this is our first time through. */
  if (win->history == NULL)
    {
      win->history = node;
      node->previous = NULL;
      node->next = NULL;
      node->position = 1;
      win->current_node = node;
    }
  else
    {
      /* Node becomes end of history list. */
      /* Point back at current node. */
      node->previous = win->current_node;
      /* Point forward to nothing. */
      node->next = NULL;
      node->position = node->previous->position + 1;
      /* Kill descendents of current node, since we'll never
         be able to go forward to them again. */
      mo_kill_node_descendents (win, win->current_node);
      /* Current node points forward to this. */
      win->current_node->next = node;
      /* Current node now becomes new node. */
      win->current_node = node;
    }

  if (win->history_list){
		DoMethod(win->history_list, MUIM_List_Select, node->previous->position - 1,
			MUIV_List_Select_Off, NULL);
		DoMethod(win->history_list, MUIM_List_Insert, &(node->title), 1,
			MUIV_List_Insert_Bottom);
		DoMethod(win->history_list, MUIM_List_Select, node->position - 1,
			MUIV_List_Select_On, NULL);
		}

  return mo_succeed;
}

/* ---------------------------- mo_grok_title ----------------------------- */

/* Make up an appropriate title for a document that does not otherwise
   have one associated with it. */
char *mo_grok_alternate_title (char *url, char *ref)
{
  char *title, *foo1, *foo2;

  if (!strncmp (url, "gopher:", 7))
    {
      /* It's a gopher server. */
      /* Do we have a ref? */
      if (ref)
        {
          char *tmp = ref;
          while (*tmp && (*tmp == ' ' || *tmp == '\t'))
            tmp++;
          title = strdup (tmp);
          goto done;
        }
      else
        {
          /* Nope, no ref.  Make up a title. */
          foo1 = url + 9;
          foo2 = strstr (foo1, ":");
          /* If there's a trailing colon (always should be.. ??)... */
          if (foo2)
            {
              char *server = (char *) malloc ((foo2 - foo1 + 2));
              
              bcopy (foo1, server, (foo2 - foo1));
              server[(foo2 - foo1)] = '\0';
              
              title = (char *) malloc ((strlen (server) + 32) * sizeof (char));
              sprintf (title, "Gopher server at %s\0", server);
              
              /* OK, we got a title... */
              free (server);

              goto done;
            }
          else
            {
              /* Aw hell... */
              title = strdup ("Gopher server");
              goto done;
            }
        }
    }

  /* If we got here, assume we should use 'ref' if possible
     for the WAIS title. */
  if (!strncmp (url, "wais:", 5) || 
      !strncmp (url, "http://info.cern.ch:8001/", 25) ||
      !strncmp (url, "http://info.cern.ch.:8001/", 26) ||
      !strncmp (url, "http://www.ncsa.uiuc.edu:8001/", 30))
    {
      /* It's a WAIS server. */
      /* Do we have a ref? */
      if (ref)
        {
          title = strdup (ref);
          goto done;
        }
      else
        {
          /* Nope, no ref.  Make up a title. */
          foo1 = url + 7;
          foo2 = strstr (foo1, ":");
          /* If there's a trailing colon (always should be.. ??)... */
          if (foo2)
            {
              char *server = (char *) malloc ((foo2 - foo1 + 2));
              
              bcopy (foo1, server, (foo2 - foo1));
              server[(foo2 - foo1)] = '\0';
              
              title = (char *) malloc ((strlen (server) + 32) * sizeof (char));
              sprintf (title, "WAIS server at %s\0", server);
              
              /* OK, we got a title... */
              free (server);

              goto done;
            }
          else
            {
              /* Aw hell... */
              title = strdup ("WAIS server");
              goto done;
            }
        }
    }

  if (!strncmp (url, "news:", 5))
    {
      /* It's a news source. */
      if (strstr (url, "@"))
        {
          /* It's a news article. */
          foo1 = url + 5;
          
          title = (char *)malloc ((strlen (foo1) + 32) * sizeof (char));
          sprintf (title, "USENET article %s\0", foo1);

          goto done;
        }
      else
        {
          /* It's a newsgroup. */
          foo1 = url + 5;
          
          title = (char *)malloc ((strlen (foo1) + 32) * sizeof (char));
          sprintf (title, "USENET newsgroup %s\0", foo1);

          goto done;
        }
    }

  if (!strncmp (url, "file:", 5))
    {
      /* It's a file. */
      if (strncmp (url, "file:///", 8) == 0)
        {
          /* It's a local file. */
          foo1 = url + 7;
          
          title = (char *)malloc ((strlen (foo1) + 32) * sizeof (char));
          sprintf (title, "Local file %s\0", foo1);
          
          goto done;
        }
      else if (strncmp (url, "file://localhost/", 17) == 0)
        {
          /* It's a local file. */
          foo1 = url + 16;
          
          title = (char *)malloc ((strlen (foo1) + 32) * sizeof (char));
          sprintf (title, "Local file %s\0", foo1);
          
          goto done;
        }
      else
        {
          /* It's a remote file. */
          foo1 = url + 7;
          
          title = (char *)malloc ((strlen (foo1) + 32) * sizeof (char));
          sprintf (title, "Remote file %s\0", foo1);
          
          goto done;
        }
    }
  
  if (!strncmp (url, "ftp:", 4))
    {
      {
        /* It's a remote file. */
        foo1 = url + 6;
        
        title = (char *)malloc ((strlen (foo1) + 32) * sizeof (char));
        sprintf (title, "Remote file %s\0", foo1);
        
        goto done;
      }
    }
  
  /* Punt... */
  title = (char *) malloc ((strlen (url) + 24) * sizeof (char));
  sprintf (title, "Untitled, URL %s\0", url);
  
 done:
  return title;
}

/* Figure out a title for the given URL.  'ref', if it exists,
   was the text used for the anchor that pointed us to this URL;
   it is not required to exist. */
char *mo_grok_title (mo_window *win, char *url, char *ref)
{
  char *title = NULL, *t;

#if defined(_AMIGA) || defined(__AROS__)
  get(HTML_Gad, HTMLA_title, (ULONG *)(&title));
#else
  XtVaGetValues (win->scrolled_win, WbNtitleText, &title, NULL);
#endif /* _AMIGA */

  if (!title)
    t = mo_grok_alternate_title (url, ref);
  else if (!strcmp (title, "Document"))
    t = mo_grok_alternate_title (url, ref);
  else
    {
      char *tmp = title;
      while (*tmp && (*tmp == ' ' || *tmp == '\t'))
        tmp++;
      if (*tmp)
        t = strdup (tmp);
      else
        t = mo_grok_alternate_title (url, ref);
    }

  mo_convert_newlines_to_spaces (t);

  return t;
}

/* --------------------------- mo_record_visit ---------------------------- */

/* Called when we visit a new node (as opposed to backing up or
   going forward).  Create an mo_node entry, call mo_grok_title
   to figure out what the title is, and call mo_node_to_history
   to add the new mo_node to both the window's data structures and
   to its Motif history list. */
mo_status mo_record_visit (mo_window *win, char *url, char *newtext, 
                           char *newtexthead, char *ref)
{
  mo_node *node = (mo_node *)malloc (sizeof (mo_node));
  node->url = url;
  node->text = newtext;
  node->texthead = newtexthead;
  node->ref = ref;
  /* Figure out what the title is... */
  node->title = mo_grok_title (win, url, ref);
  
  /* This will be recalc'd when we leave this node. */
  node->docid = 1;
  node->cached_stuff = NULL;

  mo_add_node_to_history (win, node);

  return mo_succeed;
}

/* ------------------------- navigation functions ------------------------- */

/* Back up a node. */
mo_status mo_back_node (mo_window *win)
{
  /* If there is no previous node, choke. */
  if (!win->current_node || win->current_node->previous == NULL)
    return mo_fail;

  mo_set_win_current_node (win, win->current_node->previous);

//  ResetAmigaGadgets();


#if !defined(_AMIGA) && !defined(__AROS__)
  {
    int doc, view, y;
    get(HTML_Gad, HTMLA_view_height, &view);
    get(HTML_Gad, HTMLA_doc_height, &doc);
    get(HTML_Gad, HTMLA_scroll_y, &y);
    set(SB_Vert, MUIA_Prop_Entries, doc);
    set(SB_Vert, MUIA_Prop_Visible, view);
    set(SB_Vert, MUIA_Prop_First, y);
    get(HTML_Gad, HTMLA_view_width, &view);
    get(HTML_Gad, HTMLA_doc_width, &doc);
    set(SB_Horiz, MUIA_Prop_Entries, doc);
    set(SB_Horiz, MUIA_Prop_Visible, view);
    set(SB_Horiz, MUIA_Prop_First, 0);
//  set(LV_Source, MUIA_Floattext_Text, t);
  }
#endif

  return mo_succeed;
}

/* Go forward a node. */
mo_status mo_forward_node (mo_window *win)
{
  /* If there is no next node, choke. */
  if (!win->current_node || win->current_node->next == NULL)
    return mo_fail;

  mo_set_win_current_node (win, win->current_node->next);
//  ResetAmigaGadgets();

#if !defined(_AMIGA) && !defined(__AROS__)
  {
    int doc, view, y;
    get(HTML_Gad, HTMLA_view_height, &view);
    get(HTML_Gad, HTMLA_doc_height, &doc);
    get(HTML_Gad, HTMLA_scroll_y, &y);
    set(SB_Vert, MUIA_Prop_Entries, doc);
    set(SB_Vert, MUIA_Prop_Visible, view);
    set(SB_Vert, MUIA_Prop_First, y);
    get(HTML_Gad, HTMLA_view_width, &view);
    get(HTML_Gad, HTMLA_doc_width, &doc);
    set(SB_Horiz, MUIA_Prop_Entries, doc);
    set(SB_Horiz, MUIA_Prop_Visible, view);
    set(SB_Horiz, MUIA_Prop_First, 0);
//  set(LV_Source, MUIA_Floattext_Text, t);
  }
#endif

  return mo_succeed;
}

/* Visit an arbitrary position.  This is called when a history
   list entry is double-clicked upon.

   Iterate through the window history; find the mo_node associated
   with the given position.  Call mo_set_win_current_node. */
mo_status mo_visit_position (mo_window *win, int pos)
{
  mo_node *node;
  
  for (node = win->history; node != NULL; node = node->next)
    {
      if (node->position == pos)
        {
          mo_set_win_current_node (win, node);
          goto done;
        }
    }

  fprintf (stderr, "UH OH BOSS, asked for position %d, ain't got it.\n",
           pos);

 done:
  return mo_succeed;
}

/* ---------------------------- misc functions ---------------------------- */

mo_status mo_dump_history (mo_window *win)
{
  mo_node *node;
  fprintf (stderr, "----------------- history -------------- \n");
  fprintf (stderr, "HISTORY is %p\n", win->history);
  for (node = win->history; node != NULL; node = node->next)
    {
      fprintf (stderr, "NODE %d %s\n", node->position, node->url);
      fprintf (stderr, "     TITLE %s\n", node->title);
    }
  fprintf (stderr, "CURRENT NODE %d %s\n", win->current_node->position,
           win->current_node->url);
  fprintf (stderr, "----------------- history -------------- \n");
  return mo_succeed;
}  

/* ------------------------------------------------------------------------ */
/* ----------------------------- HISTORY GUI ------------------------------ */
/* ------------------------------------------------------------------------ */

/* We've just init'd a new history list widget; look at the window's
   history and load 'er up. */
void mo_load_history_list (mo_window *win, Object *list)
{
	mo_node *node;

	DoMethod(list, MUIM_List_Clear);
	for (node = win->history; node != NULL; node = node->next){
		DoMethod(list, MUIM_List_Insert, &(node->title), 1,
			MUIV_List_Insert_Bottom);
		}
  
	DoMethod(list, MUIM_List_Select, win->current_node->position - 1,
		MUIV_List_Select_On, NULL);
}

/* stuff about mailing the history file... */

/* ---------------------------- history_win_cb ---------------------------- */

static unsigned long history_list_cb(struct Hook *h, void *o, void *msg)
{
  mo_window *win = (mo_window *)h->h_Data;
  Object *list = (Object *)o;
  int x = 0;

  get(list, MUIA_List_Active, &x);
  mo_visit_position (win, x+1);

  return (unsigned long)0;
}


mo_status mo_post_history_win (mo_window *win)
{
  if (!win->history_win)
    {
      struct Hook *history_list_cb_hook = malloc(sizeof(struct Hook));

      history_list_cb_hook->h_Entry = HookEntry;
      history_list_cb_hook->h_SubEntry = history_list_cb;
      history_list_cb_hook->h_Data = win;
      
      /* Create it for the first time. */
      win->history_win = WindowObject,
        MUIA_Window_Title, "Window History",
        MUIA_Window_ID, MAKE_ID('H','S','T','Y'),
        MUIA_Window_RefWindow, win->view,
//rs        MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered,
//rs        MUIA_Window_TopEdge, MUIV_Window_TopEdge_Centered,
//rs        MUIA_Window_Width, 300,
//rs        MUIA_Window_Height, 150,
        WindowContents, VGroup,
          Child, win->history_list = ListviewObject,
            MUIA_Listview_DoubleClick, TRUE,
            MUIA_Listview_List, ListObject,
              InputListFrame,
              MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
              MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
            End,
          End,
        End,
      End;

		DoMethod(win->history_win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
			win->history_win, 3, MUIM_Set, MUIA_Window_Open, FALSE);

		DoMethod(win->history_list, MUIM_Notify, MUIA_Listview_DoubleClick, TRUE,
			win->history_list, 2, MUIM_CallHook, history_list_cb_hook);

		DoMethod(App, OM_ADDMEMBER, win->history_win);

		/* Go get the history up to this point set up... */
		   mo_load_history_list (win, win->history_list);
		}
  
  set(win->history_win, MUIA_Window_Open, TRUE);
  
  return mo_succeed;
}
