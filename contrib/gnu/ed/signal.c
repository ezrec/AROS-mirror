/* signal.c: This file contains the signal routines for the ed line editor. */
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
#endif


#include <setjmp.h>

#include "ed.h"


#ifndef SIG_ERR
# define SIG_ERR ((void (*)()) -1)
#endif /* !SIG_ERR */

void
(*reliable_signal (sno, hndlr)) ()
     int sno;
     void (*hndlr) ();
{
#ifndef HAVE_SIGACTION
  signal (sno, hndlr);
#else
  struct sigaction sa, osa;

  sa.sa_handler = hndlr;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
#ifdef SA_RESTART
      sa.sa_flags |= SA_RESTART;
#endif
  return (sigaction (sno, &sa, &osa) < 0) ?  SIG_ERR : osa.sa_handler;
#endif /* HAVE_SIGACTION */
}


void
signal_hup (signo)
     int signo;
{
  if (mutex)
    sigflags |= (1 << (signo - 1));
  else
    handle_hup (signo);
}


void
signal_int (signo)
     int signo;
{
  if (mutex)
    sigflags |= (1 << (signo - 1));
  else
    handle_int (signo);
}


#ifdef HAVE_SIGSETJMP
extern sigjmp_buf env;
#else
extern jmp_buf env;
#endif
extern int sigactive;

void
handle_hup (signo)
     int signo;
{
  char *hup = NULL;		/* hup filename */
  char *s;
  int n;

  if (!sigactive)
    quit (1);
  sigflags &= ~(1 << (signo - 1));
  if (addr_last && write_file ("ed.hup", "w", 1, addr_last) < 0 &&
      (s = getenv ("HOME")) != NULL &&
      (n = strlen (s)) + 7 <= PATH_MAX &&	/* "ed.hup" + '/' */
      (hup = (char *) malloc (n + 8)) != NULL)
    {
      strcpy (hup, s);
      if (n && hup[n - 1] != '/')
	  hup[n++] = '/';
      strcpy (hup + n, "ed.hup");
      write_file (hup, "w", 1, addr_last);
    }
  quit (2);
}


void
handle_int (signo)
     int signo;
{
  if (!sigactive)
    quit (1);
  sigflags &= ~(1 << (signo - 1));
#ifdef HAVE_SIGSETJMP
  siglongjmp (env, -1);
#else
  longjmp (env, -1);
#endif
}


extern long rows;
int cols = 72;			/* wrap column */

void
handle_winch (signo)
     int signo;
{
#ifdef TIOCGWINSZ
  struct winsize ws;		/* window size structure */
#endif

  sigflags &= ~(1 << (signo - 1));
#ifdef TIOCGWINSZ
  if (ioctl (0, TIOCGWINSZ, (char *) &ws) >= 0)
    {
      if (ws.ws_row > 2)
	rows = ws.ws_row - 2;
      if (ws.ws_col > 8)
	cols = ws.ws_col - 8;
    }
#endif
}
