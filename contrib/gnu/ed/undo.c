/* undo.c: This file contains the undo routines for the ed line editor */
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


#define USIZE 100		/* undo stack size */
undo_t *ustack = NULL;		/* undo stack */
long usize = 0;			/* stack size variable */
long u_p = 0;			/* undo stack pointer */

/* push_undo_stack: return pointer to intialized undo node */
undo_t *
push_undo_stack (type, from, to)
     int type;
     long from;
     long to;
{
  undo_t *t;

  if ((t = ustack) == NULL &&
   (t = ustack = (undo_t *) malloc ((usize = USIZE) * sizeof (undo_t))) == NULL)
    {
      fprintf (stderr, "%s\n", strerror (errno));
      sprintf (errmsg, "Out of memory");
      return NULL;
    }
  else if (u_p >= usize &&
      (t = (undo_t *) realloc (ustack, (usize += USIZE) * sizeof (undo_t))) == NULL)
    {
      /* out of memory - release undo stack */
      fprintf (stderr, "%s\n", strerror (errno));
      sprintf (errmsg, "Out of memory");
      clear_undo_stack ();
      free (ustack);
      ustack = NULL;
      usize = 0;
      return NULL;
    }
  ustack = t;
  ustack[u_p].type = type;
  ustack[u_p].t = get_addressed_line_node (to);
  ustack[u_p].h = get_addressed_line_node (from);
  return ustack + u_p++;
}


/* USWAP: swap undo nodes */
#define USWAP(x, y) \
  do									\
    {									\
      undo_t utmp;							\
      utmp = (x), (x) = (y), (y) = utmp;				\
    }									\
  while (0)


long u_current_addr = -1;	/* if >= 0, undo enabled */
long u_addr_last = -1;		/* if >= 0, undo enabled */

/* pop_undo_stack: undo last change to the editor buffer */
int
pop_undo_stack ()
{
  long n;
  long o_current_addr = current_addr;
  long o_addr_last = addr_last;

  if (u_current_addr == -1 || u_addr_last == -1)
    {
      sprintf (errmsg, "Nothing to undo");
      return ERR;
    }
  else if (u_p)
    modified = 1;
  get_addressed_line_node (0);	/* this get_addressed_line_node last! */
  SPL1 ();
  for (n = u_p; n-- > 0;)
    {
      switch (ustack[n].type)
	{
	case UADD:
	  REQUE (ustack[n].h->q_back, ustack[n].t->q_forw);
	  break;
	case UDEL:
	  REQUE (ustack[n].h->q_back, ustack[n].h);
	  REQUE (ustack[n].t, ustack[n].t->q_forw);
	  break;
	case UMOV:
	case VMOV:
	  REQUE (ustack[n - 1].h, ustack[n].h->q_forw);
	  REQUE (ustack[n].t->q_back, ustack[n - 1].t);
	  REQUE (ustack[n].h, ustack[n].t);
	  n--;
	  break;
	default:
	  /*NOTREACHED */
	  ;
	}
      ustack[n].type ^= 1;
    }
  /* reverse undo stack order */
  for (n = u_p; n-- > (u_p + 1) / 2;)
    USWAP (ustack[n], ustack[u_p - 1 - n]);
  if (isglobal)
    clear_active_list ();
  current_addr = u_current_addr, u_current_addr = o_current_addr;
  addr_last = u_addr_last, u_addr_last = o_addr_last;
  SPL0 ();
  return 0;
}


/* clear_undo_stack: clear the undo stack */
void
clear_undo_stack ()
{
  line_t *lp, *ep, *tl;

  while (u_p--)
    if (ustack[u_p].type == UDEL)
      {
	ep = ustack[u_p].t->q_forw;
	for (lp = ustack[u_p].h; lp != ep; lp = tl)
	  {
	    unmark_line_node (lp);
	    tl = lp->q_forw;
	    free (lp);
	  }
      }
  u_p = 0;
  u_current_addr = current_addr;
  u_addr_last = addr_last;
}
