/* re.c: This file contains the regular expression interface routines for
   the ed line editor. */
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


extern int patlock;

/* get_compiled_pattern: return pointer to compiled pattern from command
   buffer */
pattern_t *
get_compiled_pattern ()
{
  static pattern_t *exp = NULL;

  char *exps;
  char delimiter;
  int n;

  if ((delimiter = *ibufp) == ' ')
    {
      sprintf (errmsg, "Invalid pattern delimiter");
      return NULL;
    }
  else if (delimiter == '\n' || *++ibufp == '\n' || *ibufp == delimiter)
    {
      if (!exp)
	sprintf (errmsg, "No previous pattern");
      return exp;
    }
  else if ((exps = extract_pattern (delimiter)) == NULL)
    return NULL;
  /* buffer alloc'd && not reserved */
  if (exp && !patlock)
    regfree (exp);
  else if ((exp = (pattern_t *) malloc (sizeof (pattern_t))) == NULL)
    {
      fprintf (stderr, "%s\n", strerror (errno));
      sprintf (errmsg, "Out of memory");
      return NULL;
    }
  patlock = 0;
  if (n = regcomp (exp, exps, 0))
    {
      regerror (n, exp, errmsg, ERRSZ);
      free (exp);
      return exp = NULL;
    }
  return exp;
}


/* extract_pattern: copy a pattern string from the command buffer; return
   pointer to the copy */
char *
extract_pattern (delimiter)
     int delimiter;
{
  static char *lhbuf = NULL;	/* buffer */
  static int lhbufsz = 0;	/* buffer size */

  char *nd;
  int len;

  for (nd = ibufp; *nd != delimiter && *nd != '\n'; nd++)
    switch (*nd)
      {
      default:
	break;
      case '[':
	if ((nd = parse_char_class (++nd)) == NULL)
	  {
	    sprintf (errmsg, "Unbalanced brackets ([])");
	    return NULL;
	  }
	break;
      case '\\':
	if (*++nd == '\n')
	  {
	    sprintf (errmsg, "Trailing backslash (\\)");
	    return NULL;
	  }
	break;
      }
  len = nd - ibufp;
  REALLOC (lhbuf, lhbufsz, len + 1, NULL);
  memcpy (lhbuf, ibufp, len);
  lhbuf[len] = '\0';
  ibufp = nd;
  return (isbinary) ? NUL_TO_NEWLINE (lhbuf, len) : lhbuf;
}


/* parse_char_class: expand a POSIX character class */
char *
parse_char_class (s)
     char *s;
{
  int c, d;

  if (*s == '^')
    s++;
  if (*s == ']')
    s++;
  for (; *s != ']' && *s != '\n'; s++)
    if (*s == '[' && ((d = *(s + 1)) == '.' || d == ':' || d == '='))
      for (s++, c = *++s; *s != ']' || c != d; s++)
	if ((c = *s) == '\n')
	  return NULL;
  return (*s == ']') ? s : NULL;
}
