/* main.c: This file contains the main control and user-interface routines
   for the ed line editor. */
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
char *copyright =
"@(#) Copyright (c) 1993, 1994 Andrew Moore, Talke Studio. \n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char *rcsid = "@(#)$Id$";
#endif /* not lint */

/*
 * CREDITS
 *
 *      This program is based on the editor algorithm described in
 *      Brian W. Kernighan and P. J. Plauger's book "Software Tools
 *      in Pascal," Addison-Wesley, 1981.
 *
 *      The buffering algorithm is attributed to Rodney Ruddock of
 *      the University of Guelph, Guelph, Ontario.
 *
 */

#ifdef HAVE_CONFIG_H
#if defined (CONFIG_BROKETS)
/* We use <config.h> instead of "config.h" so that a compilation
   using -I. -I$srcdir will use ./config.h rather than $srcdir/config.h
   (which it would do because it found this file in $srcdir).  */
#include <config.h>
#else
#include "config.h"
#endif
#endif

#include <ctype.h>
#include <setjmp.h>
#include <pwd.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include "ed.h"
#include "getopt.h"


#ifdef HAVE_SIGSETJMP
sigjmp_buf env;
#else
jmp_buf env;
#endif

/* static buffers */
char *errmsg;			/* error message buffer */
char stdinbuf[1];		/* stdin buffer */
char *shcmd;			/* shell command buffer */
int shcmdsz;			/* shell command buffer size */
int shcmdi;			/* shell command buffer index */
char *ibuf;			/* ed command-line buffer */
int ibufsz;			/* ed command-line buffer size */
char *ibufp;			/* pointer to ed command-line buffer */

/* global flags */
int traditional = 0;		/* if set, be backwards compatible */
int garrulous = 0;		/* if set, print all error messages */
int isbinary;			/* if set, buffer contains ASCII NULs */
int isglobal;			/* if set, doing a global command */
int modified;			/* if set, buffer modified since last write */
int mutex = 0;			/* if set, signals set "sigflags" */
int red = 0;			/* if set, restrict shell/directory access */
int scripted = 0;		/* if set, suppress diagnostics */
int sigactive = 0;		/* if set, signal handlers are enabled */
int sigflags = 0;		/* if set, signals received while mutex set */

char *old_filename;		/* default filename */
long current_addr;		/* current address in editor buffer */
long addr_last;			/* last address in editor buffer */
int lineno;			/* script line number */
char *prompt;			/* command-line prompt */
char *dps = "*";		/* default command-line prompt */
long err_status = 0;		/* program exit status */

/* The name this program was run with. */
char *program_name;

/* If non-zero, display usage information and exit.  */
int show_help = 0;

/* If non-zero, print the version on standard output and exit.  */
int show_version = 0;

/* Long options equivalences.  */
struct option long_options[] =
{
  {"help", no_argument, &show_help, 1},
  {"prompt", required_argument, NULL, 'p'},
  {"quiet", no_argument, NULL, 's'},
  {"silent", no_argument, NULL, 's'},
  {"traditional", no_argument, NULL, 'G'},
  {"version", no_argument, &show_version, 1},
  {0, 0, 0, 0},
};

extern char version_string[];
extern int optind;
extern char *optarg;

/* usage: explain usage */

void
usage (status)
     int status;
{
  if (status != 0)
    fprintf (stderr, "Try `%s --help' for more information.\n", program_name);
  else
    {
      printf ("Usage: %s [OPTION]... [FILE]\n", program_name);
      printf ("\
\n\
  -G, --traditional          use a few backward compatible features\n\
  -p, --prompt=STRING        use STRING as an interactive prompt\n\
  -s, -, --quiet, --silent   suppress diagnostics\n");
      printf ("\
      --help                 display this help\n\
      --version              output version information\n\
\n\
Start edit by reading in FILE if given.  Read output of shell command\n\
if FILE begins with a `!'.\n");
    }
  exit (status);
}


/* ed: line editor */
int
main (argc, argv)
     int argc;
     char **argv;
{
  int c, n;
  long status = 0;

  program_name = argv[0];
  red = (n = strlen (argv[0])) > 2 && argv[0][n - 3] == 'r';
top:
  while ((c = getopt_long (argc, argv, "Gp:s", long_options, NULL)) != EOF)
    switch (c)
      {
      default:
	usage (1);
      case 0:
	break;
      case 'G':			/* backward compatibility */
	traditional = 1;
	break;
      case 'p':			/* set prompt */
	prompt = optarg;
	break;
      case 's':			/* run script */
	scripted = 1;
	break;
      }
  if (show_version)
    {
      printf ("%s\n", version_string);
      exit (0);
    }
  if (show_help)
    usage (0);
  argv += optind;
  argc -= optind;
  if (argc && **argv == '-')
    {
      scripted = 1;
      if (argc > 1)
	{
	  optind = 0;
	  goto top;
	}
      argv++;
      argc--;
    }
#if HAVE_LOCALE_H
  setlocale(LC_CTYPE, "");
#endif
  init_buffers ();

  /* assert: reliable signals! */
#ifdef SIGWINCH
  handle_winch (SIGWINCH);
  if (isatty (0))
    reliable_signal (SIGWINCH, handle_winch);
#endif
  reliable_signal (SIGHUP, signal_hup);
  reliable_signal (SIGQUIT, SIG_IGN);
  reliable_signal (SIGINT, signal_int);
#ifdef HAVE_SIGSETJMP
  if (status = sigsetjmp (env, 1))
#else
  if (status = setjmp (env))
#endif
    {
      fputs ("\n?\n", stderr);
      sprintf (errmsg, "Interrupt");
    }
  else
    {
      sigactive = 1;		/* enable signal handlers */
      if (argc && **argv && is_legal_filename (*argv))
	{
	  if (read_file (*argv, 0) < 0 && is_regular_file (0))
	    quit (2);
	  else if (**argv != '!')
	    strcpy (old_filename, *argv);
	}
      else if (argc)
	{
	  fputs ("?\n", stderr);
	  if (**argv == '\0')
	    sprintf (errmsg, "Invalid filename");
	  if (is_regular_file (0))
	    quit (2);
	}
    }
  for (;;)
    {
      if (status < 0 && garrulous)
	fprintf (stderr, "%s\n", errmsg);
      if (prompt)
	{
	  printf ("%s", prompt);
	  fflush (stdout);
	}
      if ((n = get_tty_line ()) < 0)
	{
	  status = ERR;
	  continue;
	}
      else if (n == 0)
	{
	  if (modified && !scripted)
	    {
	      fputs ("?\n", stderr);
	      sprintf (errmsg, "Warning: file modified");
	      if (is_regular_file (0))
		{
		  fprintf (stderr, garrulous ?
			   "script, line %d: %s\n" :
			   "", lineno, errmsg);
		  quit (2);
		}
	      clearerr (stdin);
	      modified = 0;
	      status = EMOD;
	      continue;
	    }
	  else
	    quit (err_status);
	}
      else if (ibuf[n - 1] != '\n')
	{
	  /* discard line */
	  sprintf (errmsg, "Unexpected end-of-file");
	  clearerr (stdin);
	  status = ERR;
	  continue;
	}
      isglobal = 0;
      if ((status = extract_addr_range ()) >= 0 &&
	  (status = exec_command ()) >= 0)
	if (!status || (status = display_lines (current_addr, current_addr,
						status)) >= 0)
	  continue;
      switch (status)
	{
	case EOF:
	  quit (err_status);
	case EMOD:
	  modified = 0;
	  fputs ("?\n", stderr);	/* give warning */
	  sprintf (errmsg, "Warning: file modified");
	  if (is_regular_file (0))
	    {
	      fprintf (stderr, garrulous ?
		       "script, line %d: %s\n" :
		       "", lineno, errmsg);
	      quit (2);
	    }
	  break;
	case FATAL:
	  if (is_regular_file (0))
	    fprintf (stderr, garrulous ?
		     "script, line %d: %s\n" : "",
		     lineno, errmsg);
	  else
	    fprintf (stderr, garrulous ? "%s\n" : "",
		     errmsg);
	  quit (3);
	default:
	  fputs ("?\n", stderr);
	  if (is_regular_file (0))
	    {
	      fprintf (stderr, garrulous ?
		       "script, line %d: %s\n" : "",
		       lineno, errmsg);
	      quit (4);
	    }
	  break;
	}
	err_status = -status;
    }
  /*NOTREACHED */
}

long first_addr, second_addr, addr_cnt;

/* extract_addr_range: get line addresses from the command buffer until an
   illegal address is seen; return status */
int
extract_addr_range ()
{
  long addr;

  addr_cnt = 0;
  first_addr = second_addr = current_addr;
  while ((addr = next_addr ()) >= 0)
    {
      addr_cnt++;
      first_addr = second_addr;
      second_addr = addr;
      if (*ibufp != ',' && *ibufp != ';')
	break;
      else if (*ibufp++ == ';')
	current_addr = addr;
    }
  if ((addr_cnt = min (addr_cnt, 2)) == 1 || second_addr != addr)
    first_addr = second_addr;
  return (addr == ERR) ? ERR : 0;
}


#define SKIP_BLANKS() \
  while (isspace (*ibufp) && *ibufp != '\n')				\
    ibufp++

#define MUST_BE_FIRST() \
  do 									\
    {									\
      if (!first)							\
	{								\
	  sprintf (errmsg, "Invalid address");				\
	  return ERR;							\
	}								\
    }									\
  while (0)

/*  next_addr: return the next line address in the command buffer */
long
next_addr ()
{
  char *hd;
  long addr = current_addr;
  long n;
  int first = 1;
  int c;

  SKIP_BLANKS ();
  for (hd = ibufp;; first = 0)
    switch (c = *ibufp)
      {
      case '+':
      case '\t':
      case ' ':
      case '-':
      case '^':
	ibufp++;
	SKIP_BLANKS ();
	if (isdigit (*ibufp))
	  {
	    STRTOL (n, ibufp);
	    addr += (c == '-' || c == '^') ? -n : n;
	  }
	else if (!isspace (c))
	  addr += (c == '-' || c == '^') ? -1 : 1;
	break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	MUST_BE_FIRST ();
	STRTOL (addr, ibufp);
	break;
      case '.':
      case '$':
	MUST_BE_FIRST ();
	ibufp++;
	addr = (c == '.') ? current_addr : addr_last;
	break;
      case '/':
      case '?':
	MUST_BE_FIRST ();
	if ((addr = get_matching_node_addr (
				    get_compiled_pattern (), c == '/')) < 0)
	  return ERR;
	else if (c == *ibufp)
	  ibufp++;
	break;
      case '\'':
	MUST_BE_FIRST ();
	ibufp++;
	if ((addr = get_marked_node_addr (*ibufp++)) < 0)
	  return ERR;
	break;
      case '%':
      case ',':
      case ';':
	if (first)
	  {
	    ibufp++;
	    addr_cnt++;
	    second_addr = (c == ';') ? current_addr : 1;
	    addr = addr_last;
	    break;
	  }
	/* FALL THROUGH */
      default:
	if (ibufp == hd)
	  return EOF;
	else if (addr < 0 || addr_last < addr)
	  {
	    sprintf (errmsg, "Invalid address");
	    return ERR;
	  }
	else
	  return addr;
      }
  /* NOTREACHED */
}


/* GET_THIRD_ADDR: get a legal address from the command buffer */
#define GET_THIRD_ADDR(addr) \
  do									\
    {									\
      long ol1, ol2;							\
      ol1 = first_addr, ol2 = second_addr;				\
      if (extract_addr_range () < 0)					\
	return ERR;							\
      else if (traditional && addr_cnt == 0)				\
	{								\
	  sprintf (errmsg, "Destination expected");			\
	  return ERR;							\
	}								\
      else if (second_addr < 0 || addr_last < second_addr)		\
	{								\
	  sprintf (errmsg, "Invalid address");				\
	  return ERR;							\
	}								\
      (addr) = second_addr;						\
      first_addr = ol1, second_addr = ol2;				\
    }									\
  while (0)

/* GET_COMMAND_SUFFIX: verify the command suffix in the command buffer */
#define GET_COMMAND_SUFFIX() \
  do									\
    {									\
      int done = 0;							\
      do								\
	{								\
	  switch (*ibufp)						\
	    {								\
	    case 'p':							\
	      gflag |= GPR, ibufp++;					\
	      break;							\
	    case 'l':							\
	      gflag |= GLS, ibufp++;					\
	      break;							\
	    case 'n':							\
	      gflag |= GNP, ibufp++;					\
	      break;							\
	    default:							\
	      done++;							\
	    }								\
	}								\
      while (!done);							\
      if (*ibufp++ != '\n')						\
	{								\
	  sprintf (errmsg, "Invalid command suffix");			\
	  return ERR;							\
	}								\
    }									\
  while (0)

/* sflags */
#define SGG 001			/* complement previous global substitute suffix */
#define SGP 002			/* complement previous print suffix */
#define SGR 004			/* use last regex instead of last pat */
#define SGF 010			/* repeat last substitution */

int patlock = 0;		/* if set, pattern not freed by get_compiled_pattern() */

long rows = 22;			/* scroll length: ws_row - 2 */

/* exec_command: execute the next command in command buffer; return print
   request, if any */
int
exec_command ()
{
  extern long u_current_addr;
  extern long u_addr_last;

  static pattern_t *pat = NULL;
  static int sgflag = 0;
  static int sgnum = 0;

  pattern_t *tpat;
  char *fnp;
  int gflag = 0;
  int sflags = 0;
  long addr = 0;
  int n = 0;
  int c;

  SKIP_BLANKS ();
  switch (c = *ibufp++)
    {
    case 'a':
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (append_lines (second_addr) < 0)
	return ERR;
      break;
    case 'c':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (delete_lines (first_addr, second_addr) < 0 ||
	  append_lines (current_addr) < 0)
	return ERR;
      break;
    case 'd':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (delete_lines (first_addr, second_addr) < 0)
	return ERR;
      else if ((addr = INC_MOD (current_addr, addr_last)) != 0)
	current_addr = addr;
      break;
    case 'e':
      if (modified && !scripted)
	return EMOD;
      /* fall through */
    case 'E':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      else if (!isspace (*ibufp))
	{
	  sprintf (errmsg, "Unexpected command suffix");
	  return ERR;
	}
      else if ((fnp = get_filename ()) == NULL)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (delete_lines (1, addr_last) < 0)
	return ERR;
      delete_yank_lines ();
      clear_undo_stack ();
      if (close_sbuf () < 0)
	return ERR;
      else if (open_sbuf () < 0)
	return FATAL;
      if (*fnp && *fnp != '!')
	strcpy (old_filename, fnp);
      if (traditional && *fnp == '\0' && *old_filename == '\0')
	{
	  sprintf (errmsg, "No current filename");
	  return ERR;
	}
      else if (read_file (*fnp ? fnp : old_filename, 0) < 0)
	return ERR;
      clear_undo_stack ();
      modified = 0;
      u_current_addr = u_addr_last = -1;
      break;
    case 'f':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      else if (!isspace (*ibufp))
	{
	  sprintf (errmsg, "Unexpected command suffix");
	  return ERR;
	}
      else if ((fnp = get_filename ()) == NULL)
	return ERR;
      else if (*fnp == '!')
	{
	  sprintf (errmsg, "Invalid redirection");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (*fnp)
	strcpy (old_filename, fnp);
      printf ("%s\n", strip_escapes (old_filename));
      break;
    case 'g':
    case 'v':
    case 'G':
    case 'V':
      if (isglobal)
	{
	  sprintf (errmsg, "Cannot nest global commands");
	  return ERR;
	}
      else if (check_addr_range (1, addr_last) < 0)
	return ERR;
      else if (build_active_list (c == 'g' || c == 'G') < 0)
	return ERR;
      else if (n = (c == 'G' || c == 'V'))
	GET_COMMAND_SUFFIX ();
      isglobal++;
      if (exec_global (n, gflag) < 0)
	return ERR;
      break;
    case 'h':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (*errmsg)
	fprintf (stderr, "%s\n", errmsg);
      break;
    case 'H':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if ((garrulous = 1 - garrulous) && *errmsg)
	fprintf (stderr, "%s\n", errmsg);
      break;
    case 'i':
      if (second_addr == 0)
	{
	  sprintf (errmsg, "Invalid address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (append_lines (second_addr - 1) < 0)
	return ERR;
      break;
    case 'j':
      if (check_addr_range (current_addr, current_addr + 1) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (first_addr != second_addr &&
	  join_lines (first_addr, second_addr) < 0)
	return ERR;
      break;
    case 'k':
      c = *ibufp++;
      if (second_addr == 0)
	{
	  sprintf (errmsg, "Invalid address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (mark_line_node (get_addressed_line_node (second_addr), c) < 0)
	return ERR;
      break;
    case 'l':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (display_lines (first_addr, second_addr, gflag | GLS) < 0)
	return ERR;
      gflag = 0;
      break;
    case 'm':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_THIRD_ADDR (addr);
      if (first_addr <= addr && addr < second_addr)
	{
	  sprintf (errmsg, "Invalid destination");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (move_lines (addr) < 0)
	return ERR;
      break;
    case 'n':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (display_lines (first_addr, second_addr, gflag | GNP) < 0)
	return ERR;
      gflag = 0;
      break;
    case 'p':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (display_lines (first_addr, second_addr, gflag | GPR) < 0)
	return ERR;
      gflag = 0;
      break;
    case 'P':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      prompt = prompt ? NULL : optarg ? optarg : dps;
      break;
    case 'q':
    case 'Q':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      gflag = (modified && !scripted && c == 'q') ? EMOD : EOF;
      break;
    case 'r':
      if (!isspace (*ibufp))
	{
	  sprintf (errmsg, "Unexpected command suffix");
	  return ERR;
	}
      else if (addr_cnt == 0)
	second_addr = addr_last;
      if ((fnp = get_filename ()) == NULL)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (*old_filename == '\0' && *fnp != '!')
	strcpy (old_filename, fnp);
      if (traditional && *fnp == '\0' && *old_filename == '\0')
	{
	  sprintf (errmsg, "No current filename");
	  return ERR;
	}
      if ((addr = read_file (*fnp ? fnp : old_filename, second_addr)) < 0)
	return ERR;
      else if (addr && addr != addr_last)
	modified = 1;
      break;
    case 's':
      do
	{
	  switch (*ibufp)
	    {
	    case '\n':
	      sflags |= SGF;
	      break;
	    case 'g':
	      sflags |= SGG;
	      ibufp++;
	      break;
	    case 'p':
	      sflags |= SGP;
	      ibufp++;
	      break;
	    case 'r':
	      sflags |= SGR;
	      ibufp++;
	      break;
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	      STRTOL (sgnum, ibufp);
	      sflags |= SGF;
	      sgflag &= ~GSG;	/* override GSG */
	      break;
	    default:
	      if (sflags)
		{
		  sprintf (errmsg, "Invalid command suffix");
		  return ERR;
		}
	    }
	}
      while (sflags && *ibufp != '\n');
      if (sflags && !pat)
	{
	  sprintf (errmsg, "No previous substitution");
	  return ERR;
	}
      else if (sflags & SGG)
	sgnum = 0;		/* override numeric arg */
      if (*ibufp != '\n' && *(ibufp + 1) == '\n')
	{
	  sprintf (errmsg, "Invalid pattern delimiter");
	  return ERR;
	}
      tpat = pat;
      SPL1 ();
      if ((!sflags || (sflags & SGR)) &&
	  (tpat = get_compiled_pattern ()) == NULL)
	{
	  SPL0 ();
	  return ERR;
	}
      else if (tpat != pat)
	{
	  if (pat)
	    {
	      regfree (pat);
	      free (pat);
	    }
	  pat = tpat;
	  patlock = 1;		/* reserve pattern */
	}
      SPL0 ();
      if (!sflags && extract_subst_tail (&sgflag, &sgnum) < 0)
	return ERR;
      else if (isglobal)
	sgflag |= GLB;
      else
	sgflag &= ~GLB;
      if (sflags & SGG)
	sgflag ^= GSG;
      if (sflags & SGP)
	sgflag ^= GPR, sgflag &= ~(GLS | GNP);
      do
	{
	  switch (*ibufp)
	    {
	    case 'p':
	      sgflag |= GPR, ibufp++;
	      break;
	    case 'l':
	      sgflag |= GLS, ibufp++;
	      break;
	    case 'n':
	      sgflag |= GNP, ibufp++;
	      break;
	    default:
	      n++;
	    }
	}
      while (!n);
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (search_and_replace (pat, sgflag, sgnum) < 0)
	return ERR;
      break;
    case 't':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_THIRD_ADDR (addr);
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (copy_lines (addr) < 0)
	return ERR;
      break;
    case 'u':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (pop_undo_stack () < 0)
	return ERR;
      break;
    case 'w':
    case 'W':
      if ((n = *ibufp) == 'q' || n == 'Q')
	{
	  gflag = EOF;
	  ibufp++;
	}
      if (!isspace (*ibufp))
	{
	  sprintf (errmsg, "Unexpected command suffix");
	  return ERR;
	}
      else if ((fnp = get_filename ()) == NULL)
	return ERR;
      if (addr_cnt == 0 && !addr_last)
	first_addr = second_addr = 0;
      else if (check_addr_range (1, addr_last) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (*old_filename == '\0' && *fnp != '!')
	strcpy (old_filename, fnp);
      if (traditional && *fnp == '\0' && *old_filename == '\0')
	{
	  sprintf (errmsg, "No current filename");
	  return ERR;
	}
      if ((addr = write_file (*fnp ? fnp : old_filename,
		      (c == 'W') ? "a" : "w", first_addr, second_addr)) < 0)
	return ERR;
      else if (addr == addr_last)
	modified = 0;
      else if (modified && !scripted && n == 'q')
	gflag = EMOD;
      break;
    case 'x':
      if (second_addr < 0 || addr_last < second_addr)
	{
	  sprintf (errmsg, "Invalid address");
	  return ERR;
	}
      GET_COMMAND_SUFFIX ();
      if (!isglobal)
	clear_undo_stack ();
      if (put_lines (second_addr) < 0)
	return ERR;
      break;
    case 'y':
      if (check_addr_range (current_addr, current_addr) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (yank_lines (first_addr, second_addr) < 0)
	return ERR;
      break;
    case 'z':
      if (check_addr_range
         (first_addr = 1, current_addr + (traditional ? 1 : !isglobal)) < 0)
	return ERR;
      else if ('0' < *ibufp && *ibufp <= '9')
	STRTOL (rows, ibufp);
      GET_COMMAND_SUFFIX ();
      if (display_lines (second_addr, min (addr_last,
					   second_addr + rows), gflag) < 0)
	return ERR;
      gflag = 0;
      break;
    case '=':
      GET_COMMAND_SUFFIX ();
      printf ("%ld\n", addr_cnt ? second_addr : addr_last);
      break;
    case '!':
      if (addr_cnt > 0)
	{
	  sprintf (errmsg, "Unexpected address");
	  return ERR;
	}
      else if ((sflags = get_shell_command ()) < 0)
	return ERR;
      GET_COMMAND_SUFFIX ();
      if (sflags)
	printf ("%s\n", shcmd + 1);
      system (shcmd + 1);
      if (!scripted)
	printf ("!\n");
      break;
    case '\n':
      if (check_addr_range
         (first_addr = 1, current_addr + (traditional ? 1 : !isglobal)) < 0 ||
         display_lines (second_addr, second_addr, 0) < 0)
	return ERR;
      break;
    case '#':
      while (*ibufp++ != '\n')
	 ;
      break;
    default:
      sprintf (errmsg, "Unknown command");
      return ERR;
    }
  return gflag;
}


/* check_addr_range: return status of address range check */
int
check_addr_range (n, m)
     long n, m;
{
  if (addr_cnt == 0)
    {
      first_addr = n;
      second_addr = m;
    }
  if (first_addr > second_addr || 1 > first_addr ||
      second_addr > addr_last)
    {
      sprintf (errmsg, "Invalid address");
      return ERR;
    }
  return 0;
}


/* get_matching_node_addr: return the address of the next line matching a
   pattern in a given direction.  wrap around begin/end of editor buffer if
   necessary */
long
get_matching_node_addr (pat, dir)
     pattern_t *pat;
     int dir;
{
  char *s;
  long n = current_addr;
  line_t *lp;

  if (!pat)
    return ERR;
  do
    {
      if (n = dir ? INC_MOD (n, addr_last) : DEC_MOD (n, addr_last))
	{
	  lp = get_addressed_line_node (n);
	  if ((s = get_sbuf_line (lp)) == NULL)
	    return ERR;
	  if (isbinary)
	    NUL_TO_NEWLINE (s, lp->len);
	  if (!regexec (pat, s, 0, NULL, 0))
	    return n;
	}
    }
  while (n != current_addr);
  sprintf (errmsg, "No match");
  return ERR;
}


/* get_filename: return pointer to copy of filename in the command buffer */
char *
get_filename ()
{
  static char *file = NULL;
  static int filesz = 0;

  int n;

  if (*ibufp != '\n')
    {
      SKIP_BLANKS ();
      if (*ibufp == '\n')
	{
	  sprintf (errmsg, "Invalid filename");
	  return NULL;
	}
      else if ((ibufp = get_extended_line (&n, 1)) == NULL)
	return NULL;
      else if (*ibufp == '!')
	{
	  ibufp++;
	  if ((n = get_shell_command ()) < 0)
	    return NULL;
	  if (n)
	    printf ("%s\n", shcmd + 1);
	  return shcmd;
	}
      else if (n > PATH_MAX)
	{
	  sprintf (errmsg, "Filename too long");
	  return NULL;
	}
    }
  else if (!traditional && *old_filename == '\0')
    {
      sprintf (errmsg, "No current filename");
      return NULL;
    }
  REALLOC (file, filesz, PATH_MAX + 1, NULL);
  for (n = 0; *ibufp != '\n';)
    file[n++] = *ibufp++;
  file[n] = '\0';
  return is_legal_filename (file) ? file : NULL;
}


/* get_shell_command: read a shell command from stdin; return substitution
   status */
int
get_shell_command ()
{
  static char *buf = NULL;
  static int n = 0;

  char *s;			/* substitution char pointer */
  int i = 0;
  int j = 0;

  if (red)
    {
      sprintf (errmsg, "Shell access restricted");
      return ERR;
    }
  else if ((s = ibufp = get_extended_line (&j, 1)) == NULL)
    return ERR;
  REALLOC (buf, n, j + 1, ERR);
  buf[i++] = '!';		/* prefix command w/ bang */
  while (*ibufp != '\n')
    switch (*ibufp)
      {
      default:
	REALLOC (buf, n, i + 2, ERR);
	buf[i++] = *ibufp;
	if (*ibufp++ == '\\')
	  buf[i++] = *ibufp++;
	break;
      case '!':
	if (s != ibufp)
	  {
	    REALLOC (buf, n, i + 1, ERR);
	    buf[i++] = *ibufp++;
	  }
	else if (shcmd == NULL || (traditional && *(shcmd + 1) == '\0'))
	  {
	    sprintf (errmsg, "No previous command");
	    return ERR;
	  }
	else
	  {
	    REALLOC (buf, n, i + shcmdi, ERR);
	    for (s = shcmd + 1; s < shcmd + shcmdi;)
	      buf[i++] = *s++;
	    s = ibufp++;
	  }
	break;
      case '%':
	if (*old_filename == '\0')
	  {
	    sprintf (errmsg, "No current filename");
	    return ERR;
	  }
	j = strlen (s = strip_escapes (old_filename));
	REALLOC (buf, n, i + j, ERR);
	while (j--)
	  buf[i++] = *s++;
	s = ibufp++;
	break;
      }
  REALLOC (shcmd, shcmdsz, i + 1, ERR);
  memcpy (shcmd, buf, i);
  shcmd[shcmdi = i] = '\0';
  return *s == '!' || *s == '%';
}


/* append_lines: insert text from stdin to after line n; stop when either a
   single period is read or EOF; return status */
int
append_lines (n)
     long n;
{
  int l;
  char *lp = ibuf;
  char *eot;
  undo_t *up = NULL;

  for (current_addr = n;;)
    {
      if (!isglobal)
	{
	  if ((l = get_tty_line ()) < 0)
	    return ERR;
	  else if (l == 0 || ibuf[l - 1] != '\n')
	    {
	      clearerr (stdin);
	      return l ? EOF : 0;
	    }
	  lp = ibuf;
	}
      else if (*(lp = ibufp) == '\0')
	return 0;
      else
	{
	  while (*ibufp++ != '\n')
	    ;
	  l = ibufp - lp;
	}
      if (l == 2 && lp[0] == '.' && lp[1] == '\n')
	{
	  return 0;
	}
      eot = lp + l;
      SPL1 ();
      do
	{
	  if ((lp = put_sbuf_line (lp)) == NULL)
	    {
	      SPL0 ();
	      return ERR;
	    }
	  else if (up)
	    up->t = get_addressed_line_node (current_addr);
	  else if ((up = push_undo_stack (UADD, current_addr,
					  current_addr)) == NULL)
	    {
	      SPL0 ();
	      return ERR;
	    }
	}
      while (lp != eot);
      modified = 1;
      SPL0 ();
    }
  /* NOTREACHED */
}


/* join_lines: replace a range of lines with the joined text of those lines */
int
join_lines (from, to)
     long from;
     long to;
{
  static char *buf = NULL;
  static int n;

  char *s;
  int size = 0;
  line_t *bp, *ep;

  ep = get_addressed_line_node (INC_MOD (to, addr_last));
  bp = get_addressed_line_node (from);
  for (; bp != ep; bp = bp->q_forw)
    {
      if ((s = get_sbuf_line (bp)) == NULL)
	return ERR;
      REALLOC (buf, n, size + bp->len, ERR);
      memcpy (buf + size, s, bp->len);
      size += bp->len;
    }
  REALLOC (buf, n, size + 2, ERR);
  memcpy (buf + size, "\n", 2);
  if (delete_lines (from, to) < 0)
    return ERR;
  current_addr = from - 1;
  SPL1 ();
  if (put_sbuf_line (buf) == NULL ||
      push_undo_stack (UADD, current_addr, current_addr) == NULL)
    {
      SPL0 ();
      return ERR;
    }
  modified = 1;
  SPL0 ();
  return 0;
}


/* move_lines: move a range of lines */
int
move_lines (addr)
     long addr;
{
  line_t *b1, *a1, *b2, *a2;
  long n = INC_MOD (second_addr, addr_last);
  long p = first_addr - 1;
  int done = (addr == first_addr - 1 || addr == second_addr);

  SPL1 ();
  if (done)
    {
      a2 = get_addressed_line_node (n);
      b2 = get_addressed_line_node (p);
      current_addr = second_addr;
    }
  else if (push_undo_stack (UMOV, p, n) == NULL ||
	   push_undo_stack (UMOV, addr, INC_MOD (addr, addr_last)) == NULL)
    {
      SPL0 ();
      return ERR;
    }
  else
    {
      a1 = get_addressed_line_node (n);
      if (addr < first_addr)
	{
	  b1 = get_addressed_line_node (p);
	  b2 = get_addressed_line_node (addr);
	  /* this get_addressed_line_node last! */
	}
      else
	{
	  b2 = get_addressed_line_node (addr);
	  b1 = get_addressed_line_node (p);
	  /* this get_addressed_line_node last! */
	}
      a2 = b2->q_forw;
      REQUE (b2, b1->q_forw);
      REQUE (a1->q_back, a2);
      REQUE (b1, a1);
      current_addr = addr + ((addr < first_addr) ?
			     second_addr - first_addr + 1 : 0);
    }
  if (isglobal)
    unset_active_nodes (b2->q_forw, a2);
  modified = 1;
  SPL0 ();
  return 0;
}


/* copy_lines: copy a range of lines; return status */
int
copy_lines (addr)
     long addr;
{
  line_t *lp, *np = get_addressed_line_node (first_addr);
  undo_t *up = NULL;
  long n = second_addr - first_addr + 1;
  long m = 0;

  current_addr = addr;
  if (first_addr <= addr && addr < second_addr)
    {
      n = addr - first_addr + 1;
      m = second_addr - addr;
    }
  for (; n > 0; n = m, m = 0, np = get_addressed_line_node (current_addr + 1))
    for (; n-- > 0; np = np->q_forw)
      {
	SPL1 ();
	if ((lp = dup_line_node (np)) == NULL)
	  {
	    SPL0 ();
	    return ERR;
	  }
	add_line_node (lp);
	if (up)
	  up->t = lp;
	else if ((up = push_undo_stack (UADD, current_addr,
					current_addr)) == NULL)
	  {
	    SPL0 ();
	    return ERR;
	  }
	modified = 1;
	SPL0 ();
      }
  return 0;
}


/* delete_lines: delete a range of lines */
int
delete_lines (from, to)
     long from, to;
{
  line_t *n, *p;

  if (yank_lines (from, to) < 0)
    return ERR;
  SPL1 ();
  if (push_undo_stack (UDEL, from, to) == NULL)
    {
      SPL0 ();
      return ERR;
    }
  n = get_addressed_line_node (INC_MOD (to, addr_last));
  p = get_addressed_line_node (from - 1);
  /* this get_addressed_line_node last! */
  if (isglobal)
    unset_active_nodes (p->q_forw, n);
  REQUE (p, n);
  addr_last -= to - from + 1;
  current_addr = from - 1;
  modified = 1;
  SPL0 ();
  return 0;
}


int dlcnt = 0;				/* # of lines displayed */

/* display_lines: print a range of lines to stdout */
int
display_lines (from, to, gflag)
     long from;
     long to;
     int gflag;
{
  line_t *bp;
  line_t *ep;
  char *s;

  if (!from)
    {
      sprintf (errmsg, "Invalid address");
      return ERR;
    }
  ep = get_addressed_line_node (INC_MOD (to, addr_last));
  bp = get_addressed_line_node (from);
  for (dlcnt = 0; bp != ep; bp = bp->q_forw)
    {
      if ((s = get_sbuf_line (bp)) == NULL)
	return ERR;
      else if (put_tty_line (s, bp->len, current_addr = from++, gflag) < 0)
	return ERR;
      else if (!traditional && !scripted && !isglobal &&
	       bp->q_forw != ep && ++dlcnt > rows)
	{
	  dlcnt = 0;
	  fputs ("Press <RETURN> to continue... ", stdout);
	  fflush (stdout);
	  if (get_tty_line () < 0)
		return ERR;
	}
    }
  return 0;
}


line_t yank_buffer_head;			/* head of yank buffer */

/* yank_lines: copy a range of lines to the cut buffer */
int
yank_lines (from, to)
     long from;
     long to;
{
  line_t *bp, *cp, *ep, *lp;


  delete_yank_lines ();
  ep = get_addressed_line_node (INC_MOD (to, addr_last));
  bp = get_addressed_line_node (from);
  for (lp = &yank_buffer_head; bp != ep; bp = bp->q_forw, lp = cp)
    {
      SPL1 ();
      if ((cp = dup_line_node (bp)) == NULL)
	{
	  SPL0 ();
	  return ERR;
 	}
      INSQUE (cp, lp);
      SPL0 ();
    }
  return 0;
}


/* delete_yank_lines: delete lines from the yank buffer */
void
delete_yank_lines ()
{
  line_t *cp, *lp;


  for (lp = yank_buffer_head.q_forw; lp != &yank_buffer_head; lp = cp)
    {
      SPL1 ();
      cp = lp->q_forw;
      REQUE (lp->q_back, lp->q_forw);
      free (lp);
      SPL0 ();
    }
}


/* put_lines: append lines from the yank buffer */
int
put_lines (addr)
      long addr;
{
  undo_t *up = NULL;
  line_t *lp, *cp;

  if ((lp = yank_buffer_head.q_forw) == &yank_buffer_head)
    {
      sprintf (errmsg, "Nothing to put");
      return ERR;
    }
  current_addr = addr;
  for (; lp != &yank_buffer_head; lp = lp->q_forw)
    {
      SPL1 ();
        if ((cp = dup_line_node (lp)) == NULL)
        {
          SPL0 ();
          return ERR;
        }
      add_line_node (cp);
      if (up)
        up->t = cp;
      else if ((up = push_undo_stack (UADD, current_addr,
      				current_addr)) == NULL)
        {
          SPL0 ();
          return ERR;
        }
      modified = 1;
      SPL0 ();
    }
  return 0;
}


#define MAXMARK 26		/* max number of marks */

line_t *mark[MAXMARK];		/* line markers */
int markno;			/* line marker count */

/* mark_line_node: set a line node mark */
int
mark_line_node (lp, n)
     line_t *lp;
     int n;
{
  if (!islower (n))
    {
      sprintf (errmsg, "Invalid mark character");
      return ERR;
    }
  else if (mark[n - 'a'] == NULL)
    markno++;
  mark[n - 'a'] = lp;
  return 0;
}


/* get_marked_node_addr: return address of a marked line */
long
get_marked_node_addr (n)
     int n;
{
  if (!islower (n))
    {
      sprintf (errmsg, "Invalid mark character");
      return ERR;
    }
  return get_line_node_addr (mark[n - 'a']);
}


/* unmark_line_node: clear line node mark */
void
unmark_line_node (lp)
     line_t *lp;
{
  int i;

  for (i = 0; markno && i < MAXMARK; i++)
    if (mark[i] == lp)
      {
	mark[i] = NULL;
	markno--;
      }
}


/* dup_line_node: return a pointer to a copy of a line node */
line_t *
dup_line_node (lp)
     line_t *lp;
{
  line_t *np;

  if ((np = (line_t *) malloc (sizeof (line_t))) == NULL)
    {
      fprintf (stderr, "%s\n", strerror (errno));
      sprintf (errmsg, "Out of memory");
      return NULL;
    }
  np->seek = lp->seek;
  np->len = lp->len;
  return np;
}


/* has_trailing_escape:  return the parity of escapes preceding a character
   in a string */
int
has_trailing_escape (s, t)
     char *s;
     char *t;
{
  return (s == t || *(t - 1) != '\\') ? 0 : !has_trailing_escape (s, t - 1);
}


/* strip_escapes: return copy of escaped string of at most length PATH_MAX */
char *
strip_escapes (s)
     char *s;
{
  static char *file = NULL;
  static int filesz = 0;

  int i = 0;

  REALLOC (file, filesz, PATH_MAX + 1, NULL);
  /* assert: no trailing escape */
  while (file[i++] = (*s == '\\') ? *++s : *s)
    s++;
  return file;
}


#ifndef S_ISREG
#define S_ISREG(m) ((m & S_IFMT) == S_IFREG)
#endif

/* is_regular_file: if file descriptor of a regular file, then return true;
   otherwise return false */
int
is_regular_file (fd)
  int fd;
{
  struct stat sb;

  return fstat (fd, &sb) < 0 || S_ISREG (sb.st_mode);
}


/* is_legal_filename: return a legal filename */
int
is_legal_filename (s)
  char *s;
{
  if (red && (*s == '!' || !strcmp (s, "..") || strchr (s, '/')))
    {
      sprintf (errmsg, "Shell access restricted");
      return 0;
    }
  return 1;
}
