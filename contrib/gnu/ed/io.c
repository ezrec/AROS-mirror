/* io.c: This file contains the i/o routines for the ed line editor */
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


extern int scripted;

/* read_file: read a named file/pipe into the buffer; return line count */
long
read_file (fn, n)
     char *fn;
     long n;
{
  FILE *fp;
  long size;


  fp = (*fn == '!') ? popen (fn + 1, "r") : fopen (strip_escapes (fn), "r");
  if (fp == NULL)
    {
      fprintf (stderr, "%s: %s\n", fn, strerror (errno));
      sprintf (errmsg, "Cannot open input file");
      return ERR;
    }
  else if ((size = read_stream (fp, n)) < 0)
    return ERR;
  else if (((*fn == '!') ? pclose (fp) : fclose (fp)) < 0)
    {
      fprintf (stderr, "%s: %s\n", fn, strerror (errno));
      sprintf (errmsg, "Cannot close input file");
      return ERR;
    }
  fprintf (stderr, !scripted ? "%lu\n" : "", size);
  return current_addr - n;
}


char *sbuf;			/* file i/o buffer */
int sbufsz;			/* file i/o buffer size */
int newline_added;		/* if set, newline appended to input file */

/* read_stream: read a stream into the editor buffer; return status */
long
read_stream (fp, n)
     FILE *fp;
     long n;
{
  line_t *lp = get_addressed_line_node (n);
  undo_t *up = NULL;
  unsigned long size = 0;
  int o_newline_added = newline_added;
  int o_isbinary = isbinary;
  int o_n = n;
  int appended = n == addr_last;
  int len;

  isbinary = newline_added = 0;
  for (current_addr = n; (len = get_stream_line (fp)) > 0; size += len)
    {
      SPL1 ();
      if (put_sbuf_line (sbuf) == NULL)
	{
	  SPL0 ();
	  return ERR;
	}
      lp = lp->q_forw;
      if (up)
	up->t = lp;
      else if ((up = push_undo_stack (UADD, current_addr,
				      current_addr)) == NULL)
	{
	  SPL0 ();
	  return ERR;
	}
      SPL0 ();
    }
  if (len < 0)
    return ERR;
  if (o_n && appended && size && o_isbinary && o_newline_added)
    fputs ("Newline inserted\n", stderr);
  else if (newline_added && (!appended || !isbinary && !o_isbinary))
    fputs ("Newline appended\n", stderr);
  if (isbinary && newline_added && !appended)
    size += 1;
  if (!size)
    newline_added = 1;
  newline_added = appended ? newline_added : o_newline_added;
  isbinary = isbinary | o_isbinary;
  return size;
}


/* get_stream_line: read a line of text from a stream; return line length */
int
get_stream_line (fp)
     FILE *fp;
{
  register int c;
  register int i = 0;

  while (((c = getc (fp)) != EOF || !feof (fp) &&
	  !ferror (fp)) && c != '\n')
    {
      REALLOC (sbuf, sbufsz, i + 1, ERR);
      if (!(sbuf[i++] = c))
	isbinary = 1;
    }
  REALLOC (sbuf, sbufsz, i + 2, ERR);
  if (c == '\n')
    sbuf[i++] = c;
  else if (ferror (fp))
    {
      fprintf (stderr, "%s\n", strerror (errno));
      sprintf (errmsg, "Cannot read input file");
      return ERR;
    }
  else if (i)
    {
      sbuf[i++] = '\n';
      newline_added = 1;
    }
  sbuf[i] = '\0';
  return (isbinary && newline_added && i) ? --i : i;
}


/* write_file: write a range of lines to a named file/pipe; return line count */
long
write_file (fn, mode, n, m)
     char *fn;
     char *mode;
     long n;
     long m;
{
  FILE *fp;
  long size;

  fp = (*fn == '!') ? popen (fn + 1, "w") : fopen (strip_escapes (fn), mode);
  if (fp == NULL)
    {
      fprintf (stderr, "%s: %s\n", fn, strerror (errno));
      sprintf (errmsg, "Cannot open output file");
      return ERR;
    }
  else if ((size = write_stream (fp, n, m)) < 0)
    return ERR;
  else if (((*fn == '!') ? pclose (fp) : fclose (fp)) < 0)
    {
      fprintf (stderr, "%s: %s\n", fn, strerror (errno));
      sprintf (errmsg, "Cannot close output file");
      return ERR;
    }
  fprintf (stderr, !scripted ? "%lu\n" : "", size);
  return n ? m - n + 1 : 0;
}


/* write_stream: write a range of lines to a stream; return status */
long
write_stream (fp, n, m)
     FILE *fp;
     long n;
     long m;
{
  line_t *lp = get_addressed_line_node (n);
  unsigned long size = 0;
  char *s;
  int len;

  for (; n && n <= m; n++, lp = lp->q_forw)
    {
      if ((s = get_sbuf_line (lp)) == NULL)
	return ERR;
      len = lp->len;
      if (n != addr_last || !isbinary || !newline_added)
	s[len++] = '\n';
      if (put_stream_line (fp, s, len) < 0)
	return ERR;
      size += len;
    }
  return size;
}


/* put_stream_line: write a line of text to a stream; return status */
int
put_stream_line (fp, s, len)
     FILE *fp;
     char *s;
     int len;
{
  while (len--)
    if (fputc (*s++, fp) < 0)
      {
	fprintf (stderr, "%s\n", strerror (errno));
	sprintf (errmsg, "Cannot write file");
	return ERR;
      }
  return 0;
}

/* get_extended_line: get an extended line from stdin */
char *
get_extended_line (sizep, nonl)
     int *sizep;
     int nonl;
{
  static char *cvbuf = NULL;	/* buffer */
  static int cvbufsz = 0;	/* buffer size */

  int l, n;
  char *t = ibufp;

  while (*t++ != '\n')
    ;
  if ((l = t - ibufp) < 2 || !has_trailing_escape (ibufp, ibufp + l - 1))
    {
      *sizep = l;
      return ibufp;
    }
  *sizep = -1;
  REALLOC (cvbuf, cvbufsz, l, NULL);
  memcpy (cvbuf, ibufp, l);
  *(cvbuf + --l - 1) = '\n';	/* strip trailing esc */
  if (nonl)
    l--;			/* strip newline */
  for (;;)
    {
      if ((n = get_tty_line ()) < 0)
	return NULL;
      else if (n == 0 || ibuf[n - 1] != '\n')
	{
	  sprintf (errmsg, "Unexpected end-of-file");
	  return NULL;
	}
      REALLOC (cvbuf, cvbufsz, l + n, NULL);
      memcpy (cvbuf + l, ibuf, n);
      l += n;
      if (n < 2 || !has_trailing_escape (cvbuf, cvbuf + l - 1))
	break;
      *(cvbuf + --l - 1) = '\n';	/* strip trailing esc */
      if (nonl)
	l--;			/* strip newline */
    }
  REALLOC (cvbuf, cvbufsz, l + 1, NULL);
  cvbuf[l] = '\0';
  *sizep = l;
  return cvbuf;
}


/* get_tty_line: read a line of text from stdin; return line length */
int
get_tty_line ()
{
  register int oi = 0;
  register int i = 0;
  int c;

  /* Read stdin one character at a time to avoid i/o contention
     with shell escapes invoked by nonterminal input, e.g.,
     ed - <<EOF
     !cat
     hello, world
     EOF */
  for (;;)
    switch (c = getchar ())
      {
      default:
	oi = 0;
	REALLOC (ibuf, ibufsz, i + 2, ERR);
	if (!(ibuf[i++] = c))
	  isbinary = 1;
	if (c != '\n')
	  continue;
	lineno++;
	ibuf[i] = '\0';
	ibufp = ibuf;
	return i;
      case EOF:
	if (ferror (stdin))
	  {
	    fprintf (stderr, "stdin: %s\n", strerror (errno));
	    sprintf (errmsg, "Cannot read stdin");
	    clearerr (stdin);
	    ibufp = NULL;
	    return ERR;
	  }
	else
	  {
	    clearerr (stdin);
	    if (i != oi)
	      {
		oi = i;
		continue;
	      }
	    else if (i)
	      ibuf[i] = '\0';
	    ibufp = ibuf;
	    return i;
	  }
      }
}



#define ESCAPES "\a\b\f\n\r\t\v\\"
#define ESCCHARS "abfnrtv\\"

extern long rows;
extern int cols;
extern int dlcnt;

/* put_tty_line: print text to stdout */
int
put_tty_line (s, l, n, gflag)
     char *s;
     int l;
     long n;
     int gflag;
{
  int col = 0;
  char *cp;

  if (gflag & GNP)
    {
      printf ("%ld\t", n);
      col = 8;
    }
  for (; l--; s++)
    {
      if ((gflag & GLS) && ++col > cols)
	{
	  fputs ("\\\n", stdout);
	  col = 1;
          if (!traditional && !scripted && !isglobal && ++dlcnt > rows)
	    {
	      dlcnt = 0;
	      fputs ("Press <RETURN> to continue... ", stdout);
	      fflush (stdout);
	      if (get_tty_line () < 0)
	        return ERR;
	    }
	}
      if (gflag & GLS)
	{
	  if (31 < *s && *s < 127 && *s != '\\')
	    putchar (*s);
	  else
	    {
	      putchar ('\\');
	      col++;
	      if (*s && (cp = strchr (ESCAPES, *s)) != NULL)
		putchar (ESCCHARS[cp - ESCAPES]);
	      else
		{
		  putchar ((((unsigned char) *s & 0300) >> 6) + '0');
		  putchar ((((unsigned char) *s & 070) >> 3) + '0');
		  putchar (((unsigned char) *s & 07) + '0');
		  col += 2;
		}
	    }

	}
      else
	putchar (*s);
    }
  if (!traditional && (gflag & GLS))
    putchar ('$');
  putchar ('\n');
  return 0;
}
