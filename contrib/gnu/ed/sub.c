/* sub.c: This file contains the substitution routines for the ed
   line editor */
/* ed line editor.
   Copyright (C) 1993, 1994 Andrew Moore, Talke Studio
   All Rights Reserved

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef lint
static char *rcsid = "@(#)$Id$";
#endif /* not lint */

#include "ed.h"


char *rhbuf;			/* rhs substitution buffer */
int rhbufsz;			/* rhs substitution buffer size */
int rhbufi;			/* rhs substitution buffer index */

/* extract_subst_tail: extract substitution tail from the command buffer */
int
extract_subst_tail (flagp, np)
     int *flagp;
     int *np;
{
  char delimiter;

  *flagp = *np = 0;
  if ((delimiter = *ibufp) == '\n')
    {
      rhbufi = 0;
      *flagp = GPR;
      return 0;
    }
  else if (extract_subst_template () == NULL)
    return ERR;
  else if (*ibufp == '\n')
    {
      *flagp = GPR;
      return 0;
    }
  else if (*ibufp == delimiter)
    ibufp++;
  if ('1' <= *ibufp && *ibufp <= '9')
    {
      STRTOL (*np, ibufp);
      return 0;
    }
  else if (*ibufp == 'g')
    {
      ibufp++;
      *flagp = GSG;
      return 0;
    }
  return 0;
}


/* extract_subst_template: return pointer to copy of substitution template
   in the command buffer */
char *
extract_subst_template ()
{
  int n = 0;
  int i = 0;
  char c;
  char delimiter = *ibufp++;

  if (*ibufp == '%' && *(ibufp + 1) == delimiter)
    {
      ibufp++;
      if (!rhbuf)
	sprintf (errmsg, "No previous substitution");
      return rhbuf;
    }
  while (*ibufp != delimiter)
    {
      REALLOC (rhbuf, rhbufsz, i + 2, NULL);
      if ((c = rhbuf[i++] = *ibufp++) == '\n' && *ibufp == '\0')
	{
	  i--, ibufp--;
	  break;
	}
      else if (c != '\\')
	;
      else if ((rhbuf[i++] = *ibufp++) != '\n')
	;
      else if (!isglobal)
	{
	  while ((n = get_tty_line ()) == 0 ||
		 n > 0 && ibuf[n - 1] != '\n')
	    clearerr (stdin);
	  if (n < 0)
	    return NULL;
	}
    }
  REALLOC (rhbuf, rhbufsz, i + 1, NULL);
  rhbuf[rhbufi = i] = '\0';
  return rhbuf;
}


char *rbuf;			/* substitute_matching_text buffer */
int rbufsz;			/* substitute_matching_text buffer size */

/* search_and_replace: for each line in a range, change text matching a pattern
   according to a substitution template; return status  */
int
search_and_replace (pat, gflag, kth)
     pattern_t *pat;
     int gflag;
     int kth;
{
  undo_t *up;
  char *txt;
  char *eot;
  long lc;
  int nsubs = 0;
  line_t *lp;
  int len;

  current_addr = first_addr - 1;
  for (lc = 0; lc <= second_addr - first_addr; lc++)
    {
      lp = get_addressed_line_node (++current_addr);
      if ((len = substitute_matching_text (pat, lp, gflag, kth)) < 0)
	return ERR;
      else if (len)
	{
	  up = NULL;
	  if (delete_lines (current_addr, current_addr) < 0)
	    return ERR;
	  txt = rbuf;
	  eot = rbuf + len;
	  SPL1 ();
	  do
	    {
	      if ((txt = put_sbuf_line (txt)) == NULL)
		{
		  SPL0 ();
		  return ERR;
		}
	      else if (up)
		up->t = get_addressed_line_node (current_addr);
	      else if ((up = push_undo_stack (UADD,
				       current_addr, current_addr)) == NULL)
		{
		  SPL0 ();
		  return ERR;
		}
	    }
	  while (txt != eot);
	  SPL0 ();
	  nsubs++;
	}
    }
  if (nsubs == 0 && !(gflag & GLB))
    {
      sprintf (errmsg, "No match");
      return ERR;
    }
  else if ((gflag & (GPR | GLS | GNP)) &&
	   display_lines (current_addr, current_addr, gflag) < 0)
    return ERR;
  return 0;
}


/* substitute_matching_text: replace text matched by a pattern according to
   a substitution template; return pointer to the modified text */
int
substitute_matching_text (pat, lp, gflag, kth)
     pattern_t *pat;
     line_t *lp;
     int gflag;
     int kth;
{
  int off = 0;
  int changed = 0;
  int matchno = 0;
  int i = 0;
  regmatch_t rm[SE_MAX];
  char *txt;
  char *eot;

  if ((txt = get_sbuf_line (lp)) == NULL)
    return ERR;
  if (isbinary)
    NUL_TO_NEWLINE (txt, lp->len);
  eot = txt + lp->len;
  if (!regexec (pat, txt, SE_MAX, rm, 0))
    {
      do
	{
	  if (!kth || kth == ++matchno)
	    {
	      changed++;
	      i = rm[0].rm_so;
	      REALLOC (rbuf, rbufsz, off + i, ERR);
	      if (isbinary)
		NEWLINE_TO_NUL (txt, rm[0].rm_eo);
	      memcpy (rbuf + off, txt, i);
	      off += i;
	      if ((off = apply_subst_template (txt, rm, off,
					       pat->re_nsub)) < 0)
		return ERR;
	    }
	  else
	    {
	      i = rm[0].rm_eo;
	      REALLOC (rbuf, rbufsz, off + i, ERR);
	      if (isbinary)
		NEWLINE_TO_NUL (txt, i);
	      memcpy (rbuf + off, txt, i);
	      off += i;
	    }
	  txt += rm[0].rm_eo;
	}
      while (*txt && (!changed || (gflag & GSG) && rm[0].rm_eo) &&
	     !regexec (pat, txt, SE_MAX, rm, REG_NOTBOL));
      i = eot - txt;
      REALLOC (rbuf, rbufsz, off + i + 2, ERR);
      if (i > 0 && !rm[0].rm_eo && (gflag & GSG))
	{
	  sprintf (errmsg, "Infinite substitution loop");
	  return ERR;
	}
      if (isbinary)
	NEWLINE_TO_NUL (txt, i);
      memcpy (rbuf + off, txt, i);
      memcpy (rbuf + off + i, "\n", 2);
    }
  return changed ? off + i + 1 : 0;
}


/* apply_subst_template: modify text according to a substitution template;
   return offset to end of modified text */
int
apply_subst_template (boln, rm, off, re_nsub)
     char *boln;
     regmatch_t *rm;
     int off;
     int re_nsub;
{
  int j = 0;
  int k = 0;
  int n;
  char *sub = rhbuf;

  for (; sub - rhbuf < rhbufi; sub++)
    if (*sub == '&')
      {
	j = rm[0].rm_so;
	k = rm[0].rm_eo;
	REALLOC (rbuf, rbufsz, off + k - j, ERR);
	while (j < k)
	  rbuf[off++] = boln[j++];
      }
    else if (*sub == '\\' && '1' <= *++sub && *sub <= '9' &&
	     (n = *sub - '0') <= re_nsub)
      {
	j = rm[n].rm_so;
	k = rm[n].rm_eo;
	REALLOC (rbuf, rbufsz, off + k - j, ERR);
	while (j < k)
	  rbuf[off++] = boln[j++];
      }
    else
      {
	REALLOC (rbuf, rbufsz, off + 1, ERR);
	rbuf[off++] = *sub;
      }
  REALLOC (rbuf, rbufsz, off + 1, ERR);
  rbuf[off] = '\0';
  return off;
}
