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
#include "globals.h"
#include "HTML.h"
#include "mosaic.h"
#include <ctype.h>

/* Grumble grumble... */
#if defined(__sgi) && !defined(__STDC__)
#define __STDC__
#endif

/* libwww includes */
#include "HTUtils.h"
#ifdef __AROS__
#include "libwww2/tcp.h"
#else
#include "tcp.h"
#endif
#include "HTString.h"
#include "HTTCP.h"
#include "HTParse.h"
#include "HTAccess.h"
#include "HText.h"
#include "HTList.h"
#include "HTInit.h"


#define MO_BUFFER_SIZE 8192


/* Bare minimum. */
struct _HText {
  char *expandedAddress;
  char *simpleAddress;

  /* This is what we should parse and display; it is *not*
     safe to free. */
  char *htmlSrc;
  /* This is what we should free. */
  char *htmlSrcHead;
  int srcalloc;    /* amount of space allocated */
  int srclen;	   /* amount of space used */
};

/* Mosaic does NOT use either the anchor system or the style sheet
   system of libwww. */

HText* HTMainText;		    /* Equivalent of main window */

char *HTAppName = "Mosaic for Amiga";
extern char *HTAppVersion;

int force_dump_to_file; 	    /* hook to force dumping binary data
					   straight to file named by... */
char *force_dump_filename;	    /* this filename. */

/* From gui-documents.c */
extern int interrupted;

/* From HTTP.c */
extern __far int do_post;
extern __far char *post_content_type;
extern __far char *post_data;


/****************************************************************************
 * name:    hack_htmlsrc (PRIVATE)
 * purpose: Do really nasty things to a stream of HTML that just got
 *	    pulled over from a server.
 * inputs:
 *   - none (global HTMainText is assumed to contain current
 *	     HText object)
 * returns:
 *   - HTMainText->htmlSrc (char *)
 * remarks:
 *   This is ugly but it gets the job done.
 *   The job is this:
 *     - Filter out repeated instances of <PLAINTEXT>.
 *     - Discover if <PLAINTEXT> has been improperly applied
 *	 to the head of an HTML document (we discover HTML documents
 *	 by checking to see if a <TITLE> element is on the next line).
 *     - Same as above but for <HEAD> and <HTML>.
 *   We advance the character pointer HTMainText->htmlSrc by the
 *   appropriate remark to make adjustments, and keep the original
 *   head of the allocated block of text in HTMainText->htmlSrcHead.
 ****************************************************************************/
static char *hack_htmlsrc (void)
{
  if (!HTMainText)
    return NULL;

  if (!HTMainText->htmlSrc)
    {
      HTMainText->htmlSrcHead = NULL;
      return NULL;
    }

  /* Keep pointer to real head of htmlSrc memory block. */
  HTMainText->htmlSrcHead = HTMainText->htmlSrc;

  if (HTMainText->htmlSrc && HTMainText->srclen > 30)
    {
      if (!strncmp (HTMainText->htmlSrc, "<plaintext>", 11) ||
	  !strncmp (HTMainText->htmlSrc, "<PLAINTEXT>", 11))
	{
	  if (!strncmp (HTMainText->htmlSrc + 11, "<plaintext>", 11) ||
	      !strncmp (HTMainText->htmlSrc + 11, "<PLAINTEXT>", 11))
	    {
	      HTMainText->htmlSrc += 11;
	    }
	  else if (!strncmp (HTMainText->htmlSrc + 11, "\n<plaintext>", 12) ||
		   !strncmp (HTMainText->htmlSrc + 11, "\n<PLAINTEXT>", 12))
	    {
	      HTMainText->htmlSrc += 12;
	    }
	  else if (!strncmp (HTMainText->htmlSrc + 11, "\n<title>", 8) ||
		   !strncmp (HTMainText->htmlSrc + 11, "\n<TITLE>", 8))
	    {
	      HTMainText->htmlSrc += 12;
	    }
	  /* Catch CERN hytelnet gateway, etc. */
	  else if (!strncmp (HTMainText->htmlSrc + 11, "\n<HEAD>", 7) ||
		   !strncmp (HTMainText->htmlSrc + 11, "\n<head>", 7) ||
		   !strncmp (HTMainText->htmlSrc + 11, "\n<HTML>", 7) ||
		   !strncmp (HTMainText->htmlSrc + 11, "\n<html>", 7))
	    {
	      HTMainText->htmlSrc += 12;
	    }
	}
      if (!strncmp (HTMainText->htmlSrc,
		    "<TITLE>Document</TITLE>\n<PLAINTEXT>", 35))
	{
	  if (!strncmp (HTMainText->htmlSrc + 35, "\n<title>", 8) ||
	      !strncmp (HTMainText->htmlSrc + 35, "\n<TITLE>", 8))
	    {
	      HTMainText->htmlSrc += 36;
	    }
	}
    }
  return HTMainText->htmlSrc;
}


/****************************************************************************
 * name:    doit (PRIVATE)
 * purpose: Given a URL, go fetch information.
 * inputs:
 *   - char	  *url: The URL to fetch.
 *   - char **texthead: Return pointer for the head of the allocated
 *			text block (which may be different than the
 *			return text intended for display).
 * returns:
 *   The text intended for display (char *).
 ****************************************************************************/
static char *doit (char *url, char **texthead)
{
  char *msg;
  int rv;
  extern __far char *use_this_url_instead;

  /* Hmmmmmmm... */
  if (HTMainText)
    {
      free (HTMainText);
      HTMainText = NULL;
    }

  rv = HTLoadAbsolute (url);
  if (rv == 1)
    {
      char *txt = hack_htmlsrc ();
      if (HTMainText)
	*texthead = HTMainText->htmlSrcHead;
      else
	*texthead = NULL;
      return txt;
    }
  else if (rv == -1)
    {
      interrupted = 1;
      *texthead = NULL;
      return NULL;
    }

  /* Return proper error message if we experienced redirection. */
  if (use_this_url_instead)
    url = use_this_url_instead;
  msg = (char *)malloc ((strlen (url) + 200) * sizeof (char));
  sprintf (msg, "<H1>ERROR</H1> Requested document (URL %s) could not be accessed.<p>The information server either is not accessible or is refusing to serve the document to you.<p>", url);
  *texthead = msg;
  return msg;
}


/****************************************************************************
 * name:    mo_pull_er_over
 * purpose: Given a URL, pull 'er over.
 * inputs:
 *   - char	  *url: The URL to pull over.
 *   - char **texthead: Return pointer to head of allocated block.
 * returns:
 *   Text to display (char *).
 * remarks:
 *
 ****************************************************************************/
char *mo_pull_er_over (char *url, char **texthead)
{
  char *rv;
  extern int binary_transfer;

  if (binary_transfer)
    {
      force_dump_to_file = 1;
      force_dump_filename = mo_tmpnam();
    }
  rv = doit (url, texthead);
  if (binary_transfer)
    {
      force_dump_to_file = 0;
      force_dump_filename = NULL;
    }
  return rv;
}


char *mo_post_pull_er_over (char *url, char *content_type, char *data,
			    char **texthead)
{
  char *rv;
  extern int binary_transfer;

  do_post = 1;
  post_content_type = content_type;
  post_data = data;

  if (binary_transfer)
    {
      force_dump_to_file = 1;
      force_dump_filename = mo_tmpnam();
    }
  rv = doit (url, texthead);
  if (binary_transfer)
    {
      force_dump_to_file = 0;
      force_dump_filename = NULL;
    }

  do_post = 0;

  return rv;
}



/****************************************************************************
 * name:    mo_pull_er_over_virgin
 * purpose: Given a URL, pull 'er over in such a way that no format
 *	    handling takes place and the data gets dumped in the filename
 *	    of the calling routine's choice.
 * inputs:
 *   - char  *url: The URL to pull over.
 *   - char *fnam: Filename in which to dump the received data.
 * returns:
 *   mo_succeed on success; mo_fail otherwise.
 * remarks:
 *   This routine is called when we know there's data out there we
 *   want to get and we know we just want it dumped in a file, no
 *   questions asked, by the WWW library.  Appropriate global flags
 *   are set to make this happen.
 *   This must be made cleaner.
 ****************************************************************************/
mo_status mo_pull_er_over_virgin (char *url, char *fnam)
{
  int rv;

  /* Force dump to file. */
  force_dump_to_file = 1;
  force_dump_filename = fnam;

  rv = HTLoadAbsolute (url);
  if (rv == 1)
    {
      force_dump_to_file = 0;
      return mo_succeed;
    }
  else if (rv == -1)
    {
      force_dump_to_file = 0;
      interrupted = 1;
      return mo_fail;
    }
  else
    {
      force_dump_to_file = 0;
      return mo_fail;
    }
}


mo_status mo_re_init_formats (void)
{
  HTReInit ();
  return mo_succeed;
}

/* ------------------------------------------------------------------------ */

HText *HText_new (void)
{
  HText *htObj = (HText *)malloc (sizeof (HText));

  htObj->expandedAddress = NULL;
  htObj->simpleAddress = NULL;
  htObj->htmlSrc = NULL;
  htObj->htmlSrcHead = NULL;
  htObj->srcalloc = 0;
  htObj->srclen = 0;

  /* Free the struct but not the text, as it will be handled
     by Mosaic proper -- apparently. */
  if (HTMainText)
    free (HTMainText);

  HTMainText = htObj;

  return htObj;
}

void HText_free (HText *self)
{
  if (self)
    {
      if (self->htmlSrcHead)
	free (self->htmlSrcHead);
      free (self);
    }
  return;
}

void HText_beginAppend (HText *text)
{
  HTMainText = text;
  return;
}

void HText_endAppend (HText *text)
{
  if (text)
    {
      HText_appendCharacter (text, '\0');
    }
  HTMainText = text;
  return;
}

void HText_doAbort (HText *self)
{
  /* Clean up -- we want to free htmlSrc here because htmlSrcHead
     doesn't get assigned until hack_htmlsrc, and by the time we
     reach that, this should never be called. */
  if (self)
    {
      if (self->htmlSrc)
	free (self->htmlSrc);
      self->htmlSrc = NULL;
      self->htmlSrcHead = NULL;
      self->srcalloc = 0;
      self->srclen = 0;
    }
  return;
}

void HText_clearOutForNewContents (HText *self)
{
  if (self)
    {
      if (self->htmlSrc)
	free (self->htmlSrc);
      self->htmlSrc = NULL;
      self->htmlSrcHead = NULL;
      self->srcalloc = 0;
      self->srclen = 0;
    }
  return;
}

static void new_chunk (HText *text)
{
  if (text->srcalloc == 0)
    {
      text->htmlSrc = (char *)malloc (MO_BUFFER_SIZE);
      text->htmlSrc[0] = '\0';
    }
  else
    {
      text->htmlSrc = (char *)realloc
	(text->htmlSrc, text->srcalloc + MO_BUFFER_SIZE);
    }

  text->srcalloc += MO_BUFFER_SIZE;

  return;
}

void HText_appendCharacter (HText *text, char ch)
{
  if (text->srcalloc < text->srclen + 1)
    new_chunk (text);

  text->htmlSrc[text->srclen++] = ch;

  return;
}

void HText_appendText (HText *text, CONST char *str)
{
  int len;

  if (!str)
    return;

  len = strlen (str);

  while (text->srcalloc < text->srclen + len + 1)
    new_chunk (text);

  bcopy (str, (text->htmlSrc + text->srclen), len);

  text->srclen += len;
  text->htmlSrc[text->srclen] = '\0';

  return;
}

void HText_appendBlock (HText *text, CONST char *data, int len)
{
  if (!data)
    return;

  while (text->srcalloc < text->srclen + len + 1)
    new_chunk (text);

  bcopy (data, (text->htmlSrc + text->srclen), len);

  text->srclen += len;
  text->htmlSrc[text->srclen] = '\0';

  return;
}

void HText_appendParagraph (HText *text)
{
  /* Boy, talk about a misnamed function. */
  char *str = " <p> \n";

  HText_appendText (text, str);

  return;
}

void HText_beginAnchor (HText *text, char *anc)
{
  HText_appendText (text, "<A HREF=\"");
  HText_appendText (text, anc);
  HText_appendText (text, "\">");
  return;
}

void HText_endAnchor (HText * text)
{
  HText_appendText (text, "</A>");
  return;
}

void HText_dump (HText *me)
{
  return;
}

char *HText_getText (HText *me)
{
  if (me)
    return me->htmlSrc;
  else
    return NULL;
}

char **HText_getPtrToText (HText *me)
{
  if (me)
    return &(me->htmlSrc);
  else
    return NULL;
}

int HText_getTextLength (HText *me)
{
  if (me)
    return me->srclen;
  else
    return NULL;
}

BOOL HText_select (HText *text)
{
  return FALSE;
}



/****************************************************************************
 * name:    mo_url_canonicalize
 * purpose: Turn a URL into its canonical form, based on the previous
 *	    URL in this context (if appropriate).
 *	    INTERNAL ANCHORS ARE STRIPPED OFF.
 * inputs:
 *   - char    *url: URL to canonicalize.
 *   - char *oldurl: The previous URL in this context.
 * returns:
 *   The canonical representation of the URL.
 * remarks:
 *   All we do is call HTParse.
 ****************************************************************************/
char *mo_url_canonicalize (char *url, char *oldurl)
{
  /* We LOSE anchor information. */
  return HTParse (url, oldurl,
		  PARSE_ACCESS | PARSE_HOST | PARSE_PATH |
		  PARSE_PUNCTUATION);
}


/****************************************************************************
 * name:    mo_url_canonicalize_keep_anchor
 * purpose: Turn a URL into its canonical form, based on the previous
 *	    URL in this context (if appropriate).
 *	    INTERNAL ANCHORS ARE *NOT* STRIPPED OFF.
 * inputs:
 *   - char    *url: URL to canonicalize.
 *   - char *oldurl: The previous URL in this context.
 * returns:
 *   The canonical representation of the URL.
 * remarks:
 *   All we do is call HTParse.
 ****************************************************************************/
char *mo_url_canonicalize_keep_anchor (char *url, char *oldurl)
{
  char *rv;
  /* We KEEP anchor information already present in url,
     but NOT in oldurl. */
  oldurl = HTParse (oldurl, "", PARSE_ACCESS | PARSE_HOST | PARSE_PATH |
		    PARSE_PUNCTUATION);
  rv = HTParse (url, oldurl,
		PARSE_ACCESS | PARSE_HOST | PARSE_PATH |
		PARSE_PUNCTUATION | PARSE_ANCHOR);
  /* We made a new copy of oldurl, so free the new copy. */
  free (oldurl);
  return rv;
}


/****************************************************************************
 * name:    mo_url_to_unique_document
 * purpose: Given a URL that may or may not contain an internal anchor,
 *	    return a form that corresponds to a unique document -- i.e.,
 *	    a URL that has annotations different than all other
 *	    URL's, etc.  Generally this will be the URL without the
 *	    target anchor, except for automatically generated representations
 *	    of internal parts of HDF files.
 * inputs:
 *   - char *url: The URL.
 * returns:
 *   URL corresponding to a unique document.
 * remarks:
 *
 ****************************************************************************/
char *mo_url_to_unique_document (char *url)
{
  char *target = mo_url_extract_anchor (url), *rv;

  if (target && !strncmp (target, "hdfref;", 7))
    rv = strdup (url);
  else
    rv = mo_url_canonicalize (url, "");

  if (target)
    free (target);

  return rv;
}


/****************************************************************************
 * name:    mo_url_extract_anchor
 * purpose: Given a URL (presumably in canonical form), extract
 *	    the internal anchor, if any.
 * inputs:
 *   - char *url: URL to use.
 * returns:
 *   Internal anchor, if one exists in the URL; else NULL.
 * remarks:
 *
 ****************************************************************************/
char *mo_url_extract_anchor (char *url)
{
  return HTParse (url, "", PARSE_ANCHOR);
}


/****************************************************************************
 * name:    mo_url_extract_access
 * purpose: Given a URL (presumably in canonical form), extract
 *	    the access method, if any.
 * inputs:
 *   - char *url: URL to use.
 * returns:
 *   Access method, if one exists in the URL; else NULL.
 * remarks:
 *
 ****************************************************************************/
char *mo_url_extract_access (char *url, char *oldurl)
{
  return HTParse (url, oldurl, PARSE_ACCESS);
}



char *mo_url_canonicalize_local (char *url)
{
  char *cwd = getcwd (NULL, 128);
  char *tmp;

  if (!url)
    return NULL;

  tmp = (char *)malloc ((strlen (url) +
			 strlen (cwd) + 32));
  if (url[0] == '/')
    sprintf (tmp, "file://localhost%s\0",
	     url);
  else
     /* file always returned relative to a volume on the amiga */
     sprintf (tmp, "file://localhost/%s/%s\0",cwd,url);

  free (cwd);

  return tmp;
}


/****************************************************************************
 * name:    mo_tmpnam
 * purpose: Make a temporary, unique filename.
 * inputs:
 *   none
 * returns:
 *   The new temporary filename.
 * remarks:
 *   We call tmpnam() to get the actual filename, and use the value
 *   of Rdata.tmp_directory, if any, for the directory.
 ****************************************************************************/
#ifndef L_tmpnam
#define L_tmpnam 32
#endif
char *mo_tmpnam (void)
{
  char *tmp = (char *)malloc (sizeof (char) * L_tmpnam);
  char c;
  tmpnam (tmp);

  if (!Rdata.tmp_directory)
    {
      /* Fast path. */
      return tmp;
    }
  else
    {
      /* OK, reconstruct to go in the directory of our choice. */
      char *oldtmp = tmp;
      int i;

      /* Start at the back and work our way forward. */
      for (i = strlen(oldtmp)-1; i >= 0; i--)
	{
	  if (((c = oldtmp[i]) == '/') || (c == ':'))
	    goto found_it;
	}

      /* No luck, just punt. */
      return tmp;

    found_it:
      tmp = (char *)malloc (sizeof (char) * (strlen (Rdata.tmp_directory) +
					     strlen (&(oldtmp[i])) + 8));
      c = Rdata.tmp_directory[strlen(Rdata.tmp_directory)-1];
      if ((c == '/') || (c == ':'))
	{
	  /* Trailing slash in tmp_directory spec. */
	  sprintf (tmp, "%s%s", Rdata.tmp_directory, &(oldtmp[i])+1);
	}
      else
	{
	  /* No trailing slash. */
	  sprintf (tmp, "%s/%s", Rdata.tmp_directory, &(oldtmp[i]));
	}

      free (oldtmp);
      return tmp;
    }
}


/* ------------------------------ dumb stuff ------------------------------ */

/* Feedback from the library. */
void application_user_feedback (char *str)
{
  MUI_RequestA(App, WI_Main, 0, "Amiga Mosaic: Application Feedback", "OK", str, NULL);
}

static long paswd=0;

char *prompt_for_string (char *questionstr)
{
/*---*/
  extern mo_window *current_win;
  char *str;
  ULONG signal;
  int done=0;
  Object *w, *s, *ok, *cancel;
  enum { RET_OK = 777, RET_CANCEL };

	w = WindowObject,
		MUIA_Window_Title, "String Request",
		MUIA_Window_RefWindow, current_win->view,
		MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered,
		MUIA_Window_TopEdge, MUIV_Window_TopEdge_Centered,
		MUIA_Window_Width, 400,
		WindowContents, VGroup,
			Child, VGroup,
				Child, TextObject,MUIA_Text_Contents,questionstr, End,
				Child, s = StringObject, StringFrame,
							MUIA_String_Secret,paswd,
						MUIA_String_MaxLen, 512, End,
				End,
			Child, HGroup,
				Child, ok = KeyButton("OK",'o'),
				Child, cancel = KeyButton("Cancel",'c'),
			End,
		End,
	End;

  DoMethod(App, OM_ADDMEMBER, w);

  DoMethod(w, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, App, 2,
	   MUIM_Application_ReturnID, RET_CANCEL);
  DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE, App, 2,
	   MUIM_Application_ReturnID, RET_OK);
  DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE, App, 2,
	   MUIM_Application_ReturnID, RET_CANCEL);
  DoMethod(s, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
	   App, 2, MUIM_Application_ReturnID, RET_OK);

  set(w, MUIA_Window_Open, TRUE);

  while (done == 0) {

    switch (DoMethod(App, MUIM_Application_Input, &signal)) {
    case RET_OK:
      get(s, MUIA_String_Contents, &str);
      done = 1;
      break;
    case RET_CANCEL:
      str = NULL;
      done = 1;
      break;
    }
    if (signal) {
      Wait(signal);
    }
  }

  set(w, MUIA_Window_Open, FALSE);
  DoMethod(App, OM_REMMEMBER, w);
  MUI_DisposeObject(w);

  return str;
}

char *prompt_for_password (char *questionstr)
{
	char *res;
	paswd=TRUE; /* Quite dirty, but it works! */
	res = prompt_for_string(questionstr);
	paswd=FALSE;
	return res;
}

int prompt_for_yes_or_no (char *questionstr)
{
#if !defined(_AMIGA) && !defined(__AROS__)
  extern XtAppContext app_context;
  extern mo_window *current_win;

  return XmxModalYesOrNo (current_win->base, app_context,
			  questionstr, "Yes", "No");
#endif
  return 0;
}

char *mo_get_html_return (char **texthead)
{
  char *txt = hack_htmlsrc ();
  *texthead = HTMainText->htmlSrcHead;
  return txt;
}

/* Simply loop through a string and convert all newlines to spaces. */
char *mo_convert_newlines_to_spaces (char *str)
{
  int i;

  if (!str)
    return NULL;

  for (i = 0; i < strlen (str); i++)
    if (str[i] == '\n')
      str[i] = ' ';

  return str;
}

/* ---------------------------- escaping code ----------------------------- */

static unsigned char isAcceptable[96] =
/*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
{    0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,	/* 2x	!"#$%&'()*+,-./  */
     1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,	/* 3x  0123456789:;<=>?  */
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/* 4x  @ABCDEFGHIJKLMNO  */
     1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,	/* 5x  PQRSTUVWXYZ[\]^_  */
     0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	/* 6x  `abcdefghijklmno  */
     1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 }; /* 7x  pqrstuvwxyz{\}~	DEL */

#define MO_HEX(i) (i < 10 ? '0'+i : 'A'+ i - 10)

/* The string returned from here, if any, can be free'd by caller. */
char *mo_escape_part (char *part)
{
  char *q /* , *u */;
  char *p /* , *s, *e */;		/* Pointers into keywords */
  char *escaped;

  if (!part)
    return NULL;

  escaped = (char *)malloc (strlen (part) * 3 + 1);

  for (q = escaped, p = part; *p != '\0'; p++)
    {
      /*
       * This makes sure that values 128 and over don't get
       * converted to negative values.
       */
      int c = (int)((unsigned char)(*p));
      if (*p == ' ')
	{
	  *q++ = '+';
	}
      else if (c >= 32 && c <= 127 && isAcceptable[c-32])
	{
	  *q++ = *p;
	}
      else
	{
	  *q++ = '%';
	  *q++ = MO_HEX(c / 16);
	  *q++ = MO_HEX(c % 16);
	}
    }

  *q=0;

  return escaped;
}


static char mo_from_hex (char c)
{
  return (char)((c >= '0' && c <= '9') ? (c - '0') :
	  ((c >= 'A' && c <= 'F') ? (c - 'A' + 10) :
	   (c - 'a' + 10)));
}

char *mo_unescape_part (char *str)
{
  char *p = str, *q = str;

  while (*p)
    {
      /* Plus's turn back into spaces. */
      if (*p == '+')
	{
	  *q++ = ' ';
	  p++;
	}
      else if (*p == '%')
	{
	  p++;
	  if (*p)
	    *q = mo_from_hex(*p++) * 16;
	  if (*p)
	    *q += mo_from_hex(*p++);
	  q++;
	}
      else
	{
	  *q++ = *p++;
	}
    }

  *q++ = 0;
  return str;
}


#ifdef TIMING
/* ----------------------------- Timing stuff ----------------------------- */

#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>

/* Time globals. */
static struct tms tbuf;
static int gtime;

void StartClock (void)
{
  gtime = times (&tbuf);

  return;
}

void StopClock ()
{
  int donetime;

  donetime = times(&tbuf);

  fprintf (stderr, "Elapsed time %d\n", donetime - gtime);

  return;
}
#endif

