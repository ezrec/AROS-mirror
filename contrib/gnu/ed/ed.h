/* ed.h: type and constant definitions for the ed editor. */
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

 	@(#)$Id$
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

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <sys/types.h>
#include <signal.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#else
char *getenv ();
char *malloc ();
char *realloc ();
long strtol ();
#endif

#include <stdio.h>

#if STDC_HEADERS || HAVE_STRING_H
#include <string.h>
/* An ANSI string.h and pre-ANSI memory.h might conflict.  */
#if !STDC_HEADERS && HAVE_MEMORY_H
#include <memory.h>
#endif /* not STDC_HEADERS and HAVE_MEMORY_H */
#else /* not STDC_HEADERS and not HAVE_STRING_H */
#include <strings.h>
/* memory.h and strings.h conflict on some systems.  */
#define strchr index
#define strrchr rindex
#define memcpy(d, s, n) bcopy ((s), (d), (n))
#define memcmp(s1, s2, n) bcmp ((s1), (s2), (n))
#endif /* not STDC_HEADERS and not HAVE_STRING_H */

/* #include <sys/types.h> */

#include "pathmax.h"

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#include "regex.h"

#define ERR (-2)
#define EMOD (-3)
#define FATAL (-4)

#define ERRSZ (PATH_MAX + 40)	/* size of error message buffer */
#define MINBUFSZ 512		/* minimum buffer size: must be > 0 */
#define SE_MAX 30		/* max subexpressions in a regular expression */

#ifndef LONG_MAX
#define LONG_MAX ((long) (~(unsigned long) 0 >> (unsigned long) 1))
#endif
#ifndef LONG_MIN
#define LONG_MIN (-LONG_MAX - 1)
#endif

#ifndef INT_MAX
#define INT_MAX ((int) (~(unsigned int) 0 >> (unsigned int) 1))
#endif
#define LINECHARS INT_MAX	/* max chars per line */

/* gflags */
#define GLB 001			/* global command */
#define GPR 002			/* print after command */
#define GLS 004			/* list after command */
#define GNP 010			/* enumerate after command */
#define GSG 020			/* global substitute */

typedef regex_t pattern_t;

/* Line node */
typedef struct line
  {
    struct line *q_forw;
    struct line *q_back;
    off_t seek;			/* address of line in scratch buffer */
    int len;			/* length of line */
  }
line_t;


typedef struct undo
  {

/* type of undo nodes */
#define UADD	0
#define UDEL 	1
#define UMOV	2
#define VMOV	3

    int type;			/* command type */
    line_t *h;			/* head of list */
    line_t *t;			/* tail of list */
  }
undo_t;

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#define INC_MOD(l, k)	((l) + 1 > (k) ? 0 : (l) + 1)
#define DEC_MOD(l, k)	((l) - 1 < 0 ? (k) : (l) - 1)

/* SPL1: disable some interrupts (requires reliable signals) */
#define SPL1()		mutex++

/* SPL0: enable all interrupts; check sigflags (requires reliable signals) */
#define SPL0() \
  do 									\
    {									\
      if (--mutex == 0)							\
	{								\
	  if (sigflags & (1 << (SIGHUP - 1))) handle_hup (SIGHUP);	\
	  if (sigflags & (1 << (SIGINT - 1))) handle_int (SIGINT);	\
        }								\
    }									\
  while (0)

/* STRTOL: convert a string to long */
#define STRTOL(i, p) \
  do 									\
    {									\
      if ((((i) = strtol ((p), &(p), 10)) == LONG_MIN 			\
          || (i) == LONG_MAX) && errno == ERANGE)			\
	{								\
	  sprintf (errmsg, "Number out of range");			\
	  (i) = 0;							\
	  return ERR;							\
	}								\
    }									\
  while (0)

/* REALLOC: assure at least a minimum size for buffer b */
#define REALLOC(b, n, i, err) \
  do 									\
    {									\
      if ((i) > (n))							\
        {								\
          int ti = (n);							\
          char *ts;							\
          SPL1 ();							\
          if ((b) != NULL)						\
	    {								\
	      if ((ts = (char *) realloc ((b), ti += max ((i), MINBUFSZ)))	\
		  == NULL)						\
		{							\
		  fprintf (stderr, "%s\n", strerror (errno));		\
		  sprintf (errmsg, "Out of memory");			\
		  SPL0 ();						\
		  return err;						\
		}							\
	    }								\
          else								\
	    {								\
	      if ((ts = (char *) malloc (ti += max ((i), MINBUFSZ)))	\
		  == NULL)						\
		{							\
		  fprintf (stderr, "%s\n", strerror (errno));		\
		  sprintf (errmsg, "Out of memory");			\
		  SPL0 ();						\
		  return err;						\
		}							\
	    }								\
	  (n) = ti;							\
	  (b) = ts;							\
	  SPL0 ();							\
        }								\
    }									\
  while (0)

/* REQUE: link pred before succ */
#define REQUE(pred, succ) \
  ((pred)->q_forw = (succ), (succ)->q_back = (pred))

/* INSQUE: insert elem in circular queue after pred */
#define INSQUE(elem, pred) \
  do									\
    {									\
      REQUE ((elem), (pred)->q_forw);					\
      REQUE ((pred), (elem));						\
    }									\
  while (0)

/* REMQUE: remove elem from circular queue */
#define REMQUE(elem) \
  REQUE ((elem)->q_back, (elem)->q_forw)

/* NUL_TO_NEWLINE: overwrite ASCII NULs with newlines */
#define NUL_TO_NEWLINE(s, l)	translit_text(s, l, '\0', '\n')

/* NEWLINE_TO_NUL: overwrite newlines with ASCII NULs */
#define NEWLINE_TO_NUL(s, l)	translit_text(s, l, '\n', '\0')

#ifndef HAVE_STRERROR
extern const char *sys_errlist[];
extern int sys_nerr;
#define strerror(n) \
  ((n > 0 && n < sys_nerr) ? sys_errlist[n] : "Unknown system error")
#endif

#ifndef __P
#ifndef __STDC__
#define __P(proto) ()
#else
#define __P(proto) proto
#endif
#endif

/* Local Function Declarations */
void add_line_node __P ((line_t *));
int append_lines __P ((long));
int apply_subst_template __P ((char *, regmatch_t *, int, int));
int build_active_list __P ((int));
int cbc_decode __P ((char *, FILE *));
int cbc_encode __P ((char *, int, FILE *));
int check_addr_range __P ((long, long));
void clear_active_list __P ((void));
void clear_undo_stack __P ((void));
int close_sbuf __P ((void));
int copy_lines __P ((long));
int delete_lines __P ((long, long));
void delete_yank_lines __P ((void));
int display_lines __P ((long, long, int));
line_t *dup_line_node __P ((line_t *));
int exec_command __P ((void));
long exec_global __P ((int, int));
int extract_addr_range __P ((void));
char *extract_pattern __P ((int));
int extract_subst_tail __P ((int *, int *));
char *extract_subst_template __P ((void));
int filter_lines __P ((long, long, char *));
line_t *get_addressed_line_node __P ((long));
pattern_t *get_compiled_pattern __P ((void));
char *get_extended_line __P ((int *, int));
char *get_filename __P ((void));
int get_keyword __P ((void));
long get_line_node_addr __P ((line_t *));
long get_matching_node_addr __P ((pattern_t *, int));
long get_marked_node_addr __P ((int));
char *get_sbuf_line __P ((line_t *));
int get_shell_command __P ((void));
int get_stream_line __P ((FILE *));
int get_tty_line __P ((void));
void handle_hup __P ((int));
void handle_int __P ((int));
void handle_winch __P ((int));
int has_trailing_escape __P ((char *, char *));
int hex_to_binary __P ((int, int));
void init_buffers __P ((void));
int is_legal_filename __P ((char *));
int is_regular_file __P ((int));
int join_lines __P ((long, long));
int mark_line_node __P ((line_t *, int));
int move_lines __P ((long));
line_t *next_active_node __P (());
long next_addr __P ((void));
int open_sbuf __P ((void));
char *parse_char_class __P ((char *));
int pop_undo_stack __P ((void));
undo_t *push_undo_stack __P ((int, long, long));
int put_lines __P ((long));
char *put_sbuf_line __P ((char *));
int put_stream_line __P ((FILE *, char *, int));
int put_tty_line __P ((char *, int, long, int));
void quit __P ((int));
long read_file __P ((char *, long));
long read_stream __P ((FILE *, long));
void (*reliable_signal __P ((int, void (*) ()))) ();
int search_and_replace __P ((pattern_t *, int, int));
int set_active_node __P ((line_t *));
void signal_hup __P ((int));
void signal_int __P ((int));
char *strip_escapes __P ((char *));
int substitute_matching_text __P ((pattern_t *, line_t *, int, int));
char *translit_text __P ((char *, int, int, int));
void unmark_line_node __P ((line_t *));
void unset_active_nodes __P ((line_t *, line_t *));
long write_file __P ((char *, char *, long, long));
long write_stream __P ((FILE *, long, long));
int yank_lines __P ((long, long));

/* global buffers */
extern char stdinbuf[];
extern char *errmsg;
extern char *ibuf;
extern char *ibufp;
extern int ibufsz;

/* global flags */
extern int isbinary;
extern int isglobal;
extern int modified;
extern int mutex;
extern int sigflags;
extern int traditional;

/* global vars */
extern long addr_last;
extern long current_addr;
extern long first_addr;
extern int lineno;
extern long second_addr;
