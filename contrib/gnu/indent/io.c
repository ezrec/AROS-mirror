/* Copyright (c) 1999, 2000 Carlo Wood.  All rights reserved.
   Copyright (c) 1994 Joseph Arceneaux.  All rights reserved.
   Copyright (c) 1992 Free Software Foundation, Inc.  All rights reserved.

   Copyright (c) 1985 Sun Microsystems, Inc. Copyright (c) 1980 The Regents
   of the University of California. Copyright (c) 1976 Board of Trustees of
   the University of Illinois. All rights reserved.

   Redistribution and use in source and binary forms are permitted
   provided that
   the above copyright notice and this paragraph are duplicated in all such
   forms and that any documentation, advertising materials, and other
   materials related to such distribution and use acknowledge that the
   software was developed by the University of California, Berkeley, the
   University of Illinois, Urbana, and Sun Microsystems, Inc.  The name of
   either University or Sun Microsystems may not be used to endorse or
   promote products derived from this software without specific prior written
   permission. THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
   OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "sys.h"
#include <ctype.h>
#include <stdlib.h>
#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <string.h>
#ifdef VMS
#include <file.h>
#include <types.h>
#include <stat.h>
#else /* not VMS */
#include <sys/types.h>
#include <sys/stat.h>
/* POSIX says that <fcntl.h> should exist.  Some systems might need to use
   <sys/fcntl.h> or <sys/file.h> instead.  */
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#endif
#endif /* not VMS */
#include "indent.h"
#include "io.h"
#include "globs.h"

RCSTAG_CC ("$Id$");

/* number of levels a label is placed to left of code */
#define LABEL_OFFSET 2

/* Stuff that needs to be shared with the rest of indent. Documented in
   indent.h.  */
char *in_prog;
char *in_prog_pos;
char *cur_line;
unsigned long in_prog_size;
FILE *output;
char *buf_ptr;
char *buf_end;
int break_line;
int had_eof;
int out_lines;
int com_lines;
int paren_target;

int suppress_blanklines = 0;
static int comment_open;
static int inhibited = 0;


int
count_columns (column, bp, stop_char)
     int column;
     char *bp;
     int stop_char;
{
  while (*bp != stop_char && *bp != NULL_CHAR)
    {
      switch (*bp++)
	{
	case EOL:
	case 014:		/* form feed */
	  column = 1;
	  break;
	case TAB:
	  column += tabsize - (column - 1) % tabsize;
	  break;
	case 010:		/* backspace */
	  --column;
	  break;
#ifdef COLOR_DEBUG
	case '\e':		/* ANSI color */
	  while (*bp++ != 'm');
	  break;
#endif
	default:
	  ++column;
	  break;
	}
    }

  return column;
}

/* Priority mask bits */
const int boolean_operator = 1;

/*
 * Example:
 *                              e_code		(`s_code' buffer, at the moment set_buf_break() is called)
 *                              ptr		(`s_code' buffer)
 *                              corresponds_to  (input buffer)
 * Left most column		col+1		(the column (31 here)).
 * |                             |
 * 1234567890123456789012345678901234567890
 *         if (!(mask[0] == '\\0' 
 *               |
 *             target_col (assuming `lineup_to_parens' is true in this example)
 *            1 2    3 2         | level == 2
 *         <--------------------->
 *          priority_code_length
 */
struct buf_break_st
{
  struct buf_break_st *next;	/* The first possible break point to the right, if any. */
  struct buf_break_st *prev;	/* The first possible break point to the left, if any. */
  int offset;			/* The break point: the first character in the buffer that will
				   not be put on this line any more. */
  char *corresponds_to;		/* If ptr equals s_code and this equals s_code_corresponds_to,
				   then parser_state_toc->procname is valid. */
  int target_col;		/* Indentation column if we would break the line here. */
  int first_level;
  int level;			/* Number of open '(' and '['. */
  int col;			/* The number of columns left of the break point, before the break. */
  int priority_code_length;	/* Used to calculate the priority of this break point: */
  enum bb_code priority_code;
  int priority_newline;		/* Set when in the input file there was a newline at this place. */
  int priority;
};

static struct buf_break_st *buf_break_list;
struct buf_break_st *buf_break;

/*
 * Calculate break priority.
 */
static void
set_priority (bb)
     struct buf_break_st *bb;
{
  bb->priority = bb->priority_code_length;
  if (bb->priority_code == bb_semicolon)
    bb->priority += 6000;
  if (bb->priority_code == bb_before_boolean_binary_op
      || (bb->priority_code == bb_after_boolean_binary_op && bb->priority_code_length > 2))
    bb->priority += 5000;
  if (bb->priority_code == bb_after_boolean_binary_op && break_before_boolean_operator)
    bb->priority -= 3;
  if (bb->priority_code == bb_after_equal_sign)
    bb->priority += 4000;
  if (bb->priority_code == bb_attribute)
    bb->priority += 3000;
  if (bb->priority_code == bb_comma)
    bb->priority += 2000;
  if (bb->priority_code == bb_comparisation)
    bb->priority += 1000;
  if (bb->priority_code == bb_proc_call)
    bb->priority -= 1000;
  if (bb->priority_code == bb_operator6)
    bb->priority += 600;
  if (bb->priority_code == bb_operator5)
    bb->priority += 500;
  if (bb->priority_code == bb_operator4)
    bb->priority += 400;
  if (bb->priority_code == bb_operator2)
    bb->priority += 200;
  if (bb->priority_code == bb_doublecolon)
    bb->priority += 100;
}

/*
 * Returns `true' when `b1' is a better place to break the code than `b2'.
 * `b1' must be newer.
 *
 * When `lineup_to_parens' is true, do not break more then 1 level deeper
 * unless that doesn't cost us "too much" indentation.
 * What is "too much" is determined in a fuzzy way as follows:
 * Consider the example,
 *   while (!(!(!(!(!(!(mask
 *                      || (((a_very_long_expression_that_cant_be_broken
 * here we prefer to break after `mask' instead of after `while'.
 * This is because the `target_col' is pretty close to the break point
 * of the `while': "mask"->target_col - "while"->col == 15 == "mask"->level * 2 + 1.
 */
static int
better_break (b1, b2)
     struct buf_break_st *b1;
     const struct buf_break_st *b2;
{
  static int first_level;
  int is_better;

  if (!b2)
    {
      first_level = b1->level;
      b1->first_level = first_level;
      return 1;
    }
  if (b2->target_col >= b2->col + 1)
    is_better = true;
  else if (honour_newlines && b2->priority_newline)
    is_better = false;
  else if (honour_newlines && b1->priority_newline)
    is_better = true;
  else
    {
      int only_parens_till_b2 = 0;

      is_better = (b1->priority > b2->priority);
      if (is_better)
	{
	  char *p;

	  for (p = &s_code[b2->offset]; p >= s_code; --p)
	    {
	      if (*p == '!')
		--p;
	      if (*p != '(')
		break;
	    }
	  if (p < s_code)
	    only_parens_till_b2 = 1;
	}
      if (lineup_to_parens && b1->level > first_level + 1
	  && !(only_parens_till_b2 && b1->target_col <= b2->col + (1 + 2 * b1->level)) && b1->level > b2->level)
	is_better = false;
    }
  if (is_better)
    b1->first_level = first_level;
  return is_better;
}

/*
 * This function is called at every position where we possibly want
 * to break a line (if it gets too long).
 */
void
set_buf_break (code)
     enum bb_code code;
{
  int target_col, level;
  int code_target = compute_code_target ();
  struct buf_break_st *bb;

  /* First, calculate the column that code following e_code would be
     printed in if we'd break the line here.
     This is done quite simular to compute_code_target(). */

  /* Base indentation level (number of open left-braces) */
  target_col = parser_state_tos->i_l_follow + 1;
  /* Did we just parse a brace that will be put on the next line
     by this line break? */
  if (*token == '{')
    target_col -= ind_size;	/* Use `ind_size' because this only happens
				   for the first brace of initializer blocks. */

  /* Number of open brackets */
  level = parser_state_tos->p_l_follow;
  /* Did we just parse a bracket that will be put on the next line
     by this line break? */
  if (*token == '(' || *token == '[')
    --level;			/* then don't take it into account */

  /* Procedure name of function declaration? */
  if (parser_state_tos->procname[0] && token == parser_state_tos->procname)
    target_col = 1;
  else if (level == 0)		/* No open brackets? */
    {
      if (parser_state_tos->in_stmt)	/* Breaking a statement? */
	target_col += continuation_indent;
    }
  else if (!lineup_to_parens)
    {
      target_col += continuation_indent + (paren_indent * (level - 1));
    }
  else
    {
      if (parser_state_tos->paren_indents[level - 1] < 0)
	target_col = -parser_state_tos->paren_indents[level - 1];
      else
	target_col = code_target + parser_state_tos->paren_indents[level - 1];
    }

  /* Store the position of `e_code' as the place to break this line. */
  bb = (struct buf_break_st *) xmalloc (sizeof (struct buf_break_st));
  bb->offset = e_code - s_code;
  bb->level = level;
  bb->target_col = target_col;
  bb->corresponds_to = token;
  *e_code = 0;
  bb->col = count_columns (code_target, s_code, NULL_CHAR) - 1;

  /* Calculate default priority. */
  bb->priority_code_length = (e_code - s_code);
  bb->priority_newline = (parser_state_tos->last_saw_nl && !parser_state_tos->broken_at_non_nl);
  if (buf_break)
    bb->first_level = buf_break->first_level;
  switch (parser_state_tos->last_token)
    {
    case binary_op:
      if ((e_code - s_code) >= 3 && e_code[-3] == ' '
	  && ((e_code[-1] == '&' && e_code[-2] == '&') || (e_code[-1] == '|' && e_code[-2] == '|')))
	bb->priority_code = bb_after_boolean_binary_op;
      else if (e_code - s_code >= 2 && e_code[-1] == '='
	       && (e_code[-2] == ' '
		   || (e_code - s_code >= 3 && e_code[-3] == ' '
		       && (e_code[-2] == '%' || e_code[-2] == '^' || e_code[-2] == '&' || e_code[-2] == '*'
			   || e_code[-2] == '-' || e_code[-2] == '+' || e_code[-2] == '|'))))
	bb->priority_code = bb_after_equal_sign;
      else if (((e_code - s_code) >= 2 && e_code[-2] == ' ' && (e_code[-1] == '<' || e_code[-1] == '>'))
	       || ((e_code - s_code) >= 3 && e_code[-3] == ' ' && e_code[-1] == '='
		   && (e_code[-2] == '=' || e_code[-2] == '!' || e_code[-2] == '<' || e_code[-2] == '>')))
	bb->priority_code = bb_comparisation;
      else if (e_code[-1] == '+' || e_code[-1] == '-')
	bb->priority_code = bb_operator6;
      else if (e_code[-1] == '*' || e_code[-1] == '/' || e_code[-1] == '%')
	bb->priority_code = bb_operator5;
      else
	bb->priority_code = bb_binary_op;
      break;
    case comma:
      bb->priority_code = bb_comma;
      break;
    default:
      if (code == bb_binary_op && (*token == '&' || *token == '|') && *token == token[1])
	bb->priority_code = bb_before_boolean_binary_op;
      else if (e_code[-1] == ';')
	bb->priority_code = bb_semicolon;
      else
	{
	  bb->priority_code = code;
	  if (code == bb_struct_delim)	/* . -> .* or ->* */
	    {
	      if (e_code[-1] == '*')
		bb->priority_code = bb_operator4;
	      else
		bb->priority_code = bb_operator2;
	    }
	}
    }
  set_priority (bb);

  /* Add buf_break to the list */
  if (buf_break_list)
    buf_break_list->next = bb;
  bb->prev = buf_break_list;
  bb->next = NULL;
  buf_break_list = bb;

  if (buf_break && bb->col > max_col)
    return;

  if (!better_break (bb, buf_break))
    return;

  /* Found better buf_break.  Get rid of all previous possible breaks. */
  buf_break = bb;
  for (bb = bb->prev; bb;)
    {
      struct buf_break_st *obb = bb;

      bb = bb->prev;
      free (obb);
    }
  buf_break->prev = NULL;
}

void
clear_buf_break_list ()
{
  struct buf_break_st *bb;

  for (bb = buf_break_list; bb;)
    {
      struct buf_break_st *obb = bb;

      bb = bb->prev;
      free (obb);
    }
  buf_break = buf_break_list = NULL;
  break_line = 0;
}

/*
 *        prev_code_target
 *        |                prev_code_target + offset
 *        |                |
 * <----->if ((aaa == bbb) && xxx
 *            && xxx
 *            |
 *            new_code_target
 */
static void
set_next_buf_break (prev_code_target, new_code_target, offset)
     int prev_code_target, new_code_target, offset;
{
  struct buf_break_st *bb;

  better_break (buf_break, NULL);	/* Reset first_level */

  if (buf_break_list == buf_break)
    {
      clear_buf_break_list ();
      return;
    }

  /* Correct all elements of the remaining buf breaks: */
  for (bb = buf_break_list; bb; bb = bb->prev)
    {
      if (bb->target_col > buf_break->target_col && lineup_to_parens)
	bb->target_col -= ((prev_code_target + offset) - new_code_target);
      bb->col -= ((prev_code_target + offset) - new_code_target);
      bb->offset -= offset;
      bb->priority_code_length -= offset;
      bb->first_level = buf_break->first_level;
      if (!buf_break->priority_newline)
	bb->priority_newline = false;
      set_priority (bb);
      if (bb->prev == buf_break)
	break;
    }
  free (buf_break);

  /* Set buf_break to first break in the list */
  buf_break = bb;
  /* GDB_HOOK_buf_break */
  buf_break->prev = NULL;

  /* Find a better break of the existing breaks */
  for (bb = buf_break; bb; bb = bb->next)
    {
      if (bb->col > max_col)
	continue;
      if (better_break (bb, buf_break))
	{
	  /* Found better buf_break.  Get rid of all previous possible breaks. */
	  buf_break = bb;
	  for (bb = bb->prev; bb;)
	    {
	      struct buf_break_st *obb = bb;

	      bb = bb->prev;
	      free (obb);
	    }
	  bb = buf_break;
	  buf_break->prev = NULL;
	}
    }
}

#ifdef VMS
/* Folks say VMS requires its own read routine.  Then again, some folks
   say it doesn't.  Different folks have also sent me conflicting versions
   of this function.  Who's right?

   Anyway, this version was sent by MEHRDAD@glum.dev.cf.ac.uk and modified
   slightly by me. */

int
vms_read (int file_desc, char *buffer, int nbytes)
{
  char *bufp;
  int nread, nleft;

  bufp = buffer;
  nread = 0;
  nleft = nbytes;

  nread = read (file_desc, bufp, nleft);
  while (nread > 0)
    {
      bufp += nread;
      nleft -= nread;
      if (nleft < 0)
	fatal ("Internal buffering error", 0);
      nread = read (file_desc, bufp, nleft);
    }

  return nbytes - nleft;
}
#endif /* VMS */

/* Return the column we are at in the input line. */

int
current_column ()
{
  char *p;
  int column;

  /* Use save_com.size here instead of save_com.end, because save_com is
     already emptied at this point. */
  if (buf_ptr >= save_com.ptr && buf_ptr <= save_com.ptr + save_com.len)
    {
      p = save_com.ptr;
      column = save_com.start_column;
    }
  else
    {
      p = cur_line;
      column = 1;
    }

  while (p < buf_ptr)
    {
      switch (*p)
	{
	case EOL:
	case 014:		/* form feed */
	  column = 1;
	  break;

	case TAB:
	  column += tabsize - (column - 1) % tabsize;
	  break;

	case '\b':		/* backspace */
	  column--;
	  break;

	default:
	  column++;
	  break;
	}

      p++;
    }

  return column;
}

/* Fill the output line with whitespace up to TARGET_COLUMN, given that
   the line is currently in column CURRENT_COLUMN.  Returns the ending
   column. */

static int
pad_output (current_column, target_column)
     int current_column;
     int target_column;
{
  if (current_column >= target_column)
    return current_column;

  if (use_tabs && tabsize > 1)
    {
      int offset;

      offset = tabsize - (current_column - 1) % tabsize;
      while (current_column + offset <= target_column)
	{
	  putc (TAB, output);
	  current_column += offset;
	  offset = tabsize;
	}
    }

  while (current_column < target_column)
    {
      putc (' ', output);
      current_column++;
    }

  return current_column;
}

static int prev_target_col_break;
int buf_break_used;

void
dump_line (force_nl)
     int force_nl;
{				/* dump_line is the routine that actually
				   effects the printing of the new source. It
				   prints the label section, followed by the
				   code section with the appropriate nesting
				   level, followed by any comments */
  int cur_col;
  int target_col = 0;
  int not_truncated = 1;
  int target_col_break = -1;

  if (buf_break_used)
    {
      buf_break_used = 0;
      target_col_break = prev_target_col_break;
    }
  else if (force_nl)
    parser_state_tos->broken_at_non_nl = false;

  if (parser_state_tos->procname[0] && !parser_state_tos->classname[0]
      && s_code_corresponds_to == parser_state_tos->procname)
    parser_state_tos->procname = "\0";
  else if (parser_state_tos->procname[0] && parser_state_tos->classname[0]
	   && s_code_corresponds_to == parser_state_tos->classname)
    {
      parser_state_tos->procname = "\0";
      parser_state_tos->classname = "\0";
    }

  /* A blank line */
  if (s_code == e_code && s_lab == e_lab && s_com == e_com)
    {
      /* If we have a formfeed on a blank line, we should just output it,
         rather than treat it as a normal blank line.  */
      if (parser_state_tos->use_ff)
	{
	  putc ('\014', output);
	  parser_state_tos->use_ff = false;
	}
      else
	{
	  if (suppress_blanklines > 0)
	    suppress_blanklines--;
	  else
	    {
	      parser_state_tos->bl_line = true;
	      n_real_blanklines++;
	    }
	}
    }
  else
    {
      suppress_blanklines = 0;
      parser_state_tos->bl_line = false;
      if (prefix_blankline_requested && n_real_blanklines == 0)
	n_real_blanklines = 1;
      else if (swallow_optional_blanklines && n_real_blanklines > 1)
	n_real_blanklines = 1;

      while (--n_real_blanklines >= 0)
	putc (EOL, output);
      n_real_blanklines = 0;

      if (e_lab != s_lab || e_code != s_code)
	++code_lines;		/* keep count of lines with code */

      if (e_lab != s_lab)
	{			/* print lab, if any */
	  if (comment_open)
	    {
	      comment_open = 0;
	      fprintf (output, ".*/\n");
	    }
	  while (e_lab > s_lab && (e_lab[-1] == ' ' || e_lab[-1] == TAB))
	    e_lab--;
	  cur_col = pad_output (1, compute_label_target ());
	  if (s_lab[0] == '#' && (strncmp (s_lab, "#else", 5) == 0 || strncmp (s_lab, "#endif", 6) == 0))
	    {
	      /* Treat #else and #endif as a special case because any text
	         after #else or #endif should be converted to a comment.  */
	      char *s = s_lab;

	      if (e_lab[-1] == EOL)	/* Don't include EOL in the comment */
		e_lab--;
	      do
		{
		  putc (*s++, output);
		  ++cur_col;
		}
	      while (s < e_lab && 'a' <= *s && *s <= 'z');
	      while ((*s == ' ' || *s == TAB) && s < e_lab)
		s++;
	      if (s < e_lab)
		{
		  if (tabsize > 1)
		    cur_col = pad_output (cur_col, cur_col + tabsize - (cur_col - 1) % tabsize);
		  else
		    cur_col = pad_output (cur_col, cur_col + 2);
		  if (s[0] == '/' && (s[1] == '*' || s[1] == '/'))
		    fprintf (output, "%.*s", e_lab - s, s);
		  else
		    fprintf (output, "/* %.*s */", e_lab - s, s);
		  /* no need to update cur_col: the very next thing will
		     be a new-line (or end of file) */
		}
	    }
	  else
	    {
	      fprintf (output, "%.*s", (int) (e_lab - s_lab), s_lab);
	      cur_col = count_columns (cur_col, s_lab, NULL_CHAR);
	    }
	}
      else
	cur_col = 1;		/* there is no label section */

      parser_state_tos->pcase = false;

      /* Remove trailing spaces */
      while (*(e_code - 1) == ' ' && e_code > s_code)
	*(--e_code) = NULL_CHAR;
      if (s_code != e_code)
	{			/* print code section, if any */
	  char *p;
	  int i;

	  if (comment_open)
	    {
	      comment_open = 0;
	      fprintf (output, ".*/\n");
	    }

	  /* If a comment begins this line, then indent it to the right
	     column for comments, otherwise the line starts with code,
	     so indent it for code. */
	  if (embedded_comment_on_line == 1)
	    target_col = parser_state_tos->com_col;
	  else if (target_col_break != -1)
	    target_col = target_col_break;
	  else
	    target_col = compute_code_target ();

	  /* If a line ends in an lparen character, the following line should
	     not line up with the parenthesis, but should be indented by the
	     usual amount.  */
	  if (parser_state_tos->last_token == lparen)
	    {
	      parser_state_tos->paren_indents[parser_state_tos->p_l_follow - 1] += ind_size - 1;
	    }

	  cur_col = pad_output (cur_col, target_col);

	  if (break_line && s_com == e_com && buf_break->target_col <= buf_break->col)
	    {
	      int offset, len;
	      char c;
	      char *ptr = &s_code[buf_break->offset];

	      if (*ptr != ' ')
		--ptr;

	      /* Add target_col (and negate) the brackets that are
	         actually printed.  The remaining brackets must
	         be given an offset of . */
	      offset = ptr - s_code + 1;

	      for (i = 0; i < parser_state_tos->p_l_follow; i++)
		if (parser_state_tos->paren_indents[i] >= 0)
		  {
		    if (parser_state_tos->paren_indents[i] < ptr - s_code)
		      parser_state_tos->paren_indents[i] = -(parser_state_tos->paren_indents[i] + target_col);
		    else
		      parser_state_tos->paren_indents[i] -= offset;
		  }
	      for (i = parser_state_tos->p_l_follow; i < parser_state_tos->paren_indents_size; ++i)
		if (parser_state_tos->paren_indents[i] >= (ptr - s_code))
		  parser_state_tos->paren_indents[i] -= offset;

	      for (p = s_code; p < s_code + buf_break->offset; p++)
		putc (*p, output);

	      c = s_code[buf_break->offset];
	      s_code[buf_break->offset] = '\0';
	      cur_col = count_columns (cur_col, s_code, NULL_CHAR);
	      s_code[buf_break->offset] = c;

	      not_truncated = 0;
	      len = (e_code - ptr - 1);
	      memmove (s_code, ptr + 1, len);
	      e_code = s_code + len;
#if COLOR_DEBUG
	      fputs (" \e[31m", output);
	      for (p = s_code; p < e_code; p++)
		putc (*p, output);
	      fputs (" \e[0m", output);
#endif
	      *e_code = '\0';
	      s_code_corresponds_to = buf_break->corresponds_to;
	      prev_target_col_break = buf_break->target_col;
	      if (!buf_break->priority_newline)
		parser_state_tos->broken_at_non_nl = true;
	      set_next_buf_break (target_col, buf_break->target_col, offset);
	      buf_break_used = 1;
	      break_line = (buf_break != NULL) && (output_line_length () > max_col);
	    }
	  else
	    {
	      for (i = 0; i < parser_state_tos->p_l_follow; i++)
		if (parser_state_tos->paren_indents[i] >= 0)
		  parser_state_tos->paren_indents[i] = -(parser_state_tos->paren_indents[i] + target_col);

	      for (p = s_code; p < e_code; p++)
		putc (*p, output);
	      cur_col = count_columns (cur_col, s_code, NULL_CHAR);
	      clear_buf_break_list ();
	    }
	}

      if (s_com != e_com)
	{
	  {
	    /* Here for comment printing. */
	    int target = parser_state_tos->com_col;
	    char *com_st = s_com;

	    if (cur_col > target)
	      {
		putc (EOL, output);
		cur_col = 1;
		++out_lines;
	      }

	    cur_col = pad_output (cur_col, target);
	    fwrite (com_st, e_com - com_st, 1, output);
	    cur_col += e_com - com_st;
	    com_lines++;
	  }
	}
      else if (embedded_comment_on_line)
	com_lines++;
      embedded_comment_on_line = 0;

      if (parser_state_tos->use_ff)
	{
	  putc ('\014', output);
	  parser_state_tos->use_ff = false;
	}
      else
	putc (EOL, output);

      ++out_lines;
      if (parser_state_tos->just_saw_decl == 1 && blanklines_after_declarations)
	{
	  prefix_blankline_requested = 1;
	  prefix_blankline_requested_code = decl;
	  parser_state_tos->just_saw_decl = 0;
	}
      else
	{
	  prefix_blankline_requested = postfix_blankline_requested;
	  prefix_blankline_requested_code = postfix_blankline_requested_code;
	}
      postfix_blankline_requested = 0;
    }

  /* if we are in the middle of a declaration, remember that fact
     for proper comment indentation */
  parser_state_tos->decl_on_line = parser_state_tos->in_decl;

  /* next line should be indented if we have not completed this stmt */
  parser_state_tos->ind_stmt = parser_state_tos->in_stmt;

  parser_state_tos->dumped_decl_indent = 0;
  *(e_lab = s_lab) = '\0';	/* reset buffers */
  if (not_truncated)
    {
      *(e_code = s_code) = '\0';
      s_code_corresponds_to = NULL;
    }

  *(e_com = s_com) = '\0';
  parser_state_tos->ind_level = parser_state_tos->i_l_follow;
  parser_state_tos->paren_level = parser_state_tos->p_l_follow;
  if (parser_state_tos->paren_level > 0)
    {
      /* If we broke the line and the following line will
         begin with a rparen, the indentation is set for
         the column of the rparen *before* the break - reset
         the column to the position after the break. */
      if (!not_truncated && (*s_code == '(' || *s_code == '[') && parser_state_tos->paren_level >= 2)
	{
	  paren_target = -parser_state_tos->paren_indents[parser_state_tos->paren_level - 2];
	}
      else
	paren_target = -parser_state_tos->paren_indents[parser_state_tos->paren_level - 1];
    }
  else
    paren_target = 0;

  if (inhibited)
    {
      char *p = cur_line;

      while (--n_real_blanklines >= 0)
	putc (EOL, output);
      n_real_blanklines = 0;

      do
	{
	  while (*p != '\0' && *p != EOL)
	    putc (*p++, output);
	  if (*p == '\0' && (unsigned long) (p - current_input->data) == current_input->size)
	    {
	      buf_ptr = buf_end = in_prog_pos = p;
	      had_eof = 1;
	      return;
	    }

	  if (*p == EOL)
	    {
	      cur_line = p + 1;
	      line_no++;
	    }
	  putc (*p++, output);
	  while (*p == ' ' || *p == TAB)
	    putc (*p++, output);
	  if (*p == '/' && (*(p + 1) == '*' || *(p + 1) == '/'))
	    {
	      /* We've hit a comment.  See if turns formatting
	         back on. */
	      putc (*p++, output);
	      putc (*p++, output);
	      while (*p == ' ' || *p == TAB)
		putc (*p++, output);
	      if (!strncmp (p, "*INDENT-ON*", 11))
		{
		  do
		    {
		      while (*p != '\0' && *p != EOL)
			putc (*p++, output);
		      if (*p == '\0' && ((unsigned long) (p - current_input->data) == current_input->size))
			{
			  buf_ptr = buf_end = in_prog_pos = p;
			  had_eof = 1;
			  return;
			}
		      else
			{
			  if (*p == EOL)
			    {
			      inhibited = 0;
			      cur_line = p + 1;
			      line_no++;
			    }
			  putc (*p++, output);
			}
		    }
		  while (inhibited);
		}
	    }
	}
      while (inhibited);

      buf_ptr = buf_end = in_prog_pos = cur_line;
      fill_buffer ();
    }

  /* Output the rest already if we really wanted a new-line after this code. */
  if (buf_break_used && s_code != e_code && force_nl)
    {
      prefix_blankline_requested = 0;
      dump_line (true);
    }

  return;
}

/* Return the column in which we should place the code in s_code. */

int
compute_code_target ()
{
  int target_col;

  if (buf_break_used)
    return prev_target_col_break;

  if (parser_state_tos->procname[0] && s_code_corresponds_to == parser_state_tos->procname)
    {
      target_col = 1;
      if (!parser_state_tos->paren_level)
	return target_col;
    }
  else
    target_col = parser_state_tos->ind_level + 1;

  if (!parser_state_tos->paren_level)
    {
      if (parser_state_tos->ind_stmt)
	target_col += continuation_indent;
      return target_col;
    }

  if (!lineup_to_parens)
    return target_col + continuation_indent + (paren_indent * (parser_state_tos->paren_level - 1));

  return paren_target;
}

int
compute_label_target ()
{
  /* maybe there should be some option to tell indent where to put public:,
     private: etc. ? */
  if (*s_lab == '#')
    return 1;
  if (parser_state_tos->pcase)
    return parser_state_tos->cstk[parser_state_tos->tos] + 1;
  if (c_plus_plus && parser_state_tos->in_decl)
    {
      /* FIXME: does this belong here at all? */
      return 1;
    }
  else
    return parser_state_tos->ind_level - LABEL_OFFSET + 1;
}

/* VMS defines it's own read routine, `vms_read' */
#ifndef INDENT_SYS_READ
#define INDENT_SYS_READ read
#endif

/* Read file FILENAME into a `fileptr' structure, and return a pointer to
   that structure. */

static struct file_buffer fileptr;

struct file_buffer *
read_file (filename, file_stats)
     char *filename;
     struct stat *file_stats;
{
  int fd;

  /* Required for MSDOS, in order to read files larger than 32767
     bytes in a 16-bit world... */
  unsigned int size;

  int namelen = strlen (filename);

  fd = open (filename, O_RDONLY, 0777);
  if (fd < 0)
    fatal ("Can't open input file %s", filename);

  if (fstat (fd, file_stats) < 0)
    fatal ("Can't stat input file %s", filename);

  if (file_stats->st_size == 0)
    ERROR ("Warning: Zero-length file %s", filename, 0);

#if !defined(__DJGPP__)
  if (sizeof (int) == 2)	/* Old MSDOS */
    {
      if (file_stats->st_size < 0 || file_stats->st_size > (0xffff - 1))
	fatal ("File %s is too big to read", filename);
    }
  else
#endif
  if (file_stats->st_size < 0)
    fatal ("System problem reading file %s", filename);

  fileptr.size = file_stats->st_size;
  if (fileptr.data != 0)
    fileptr.data = (char *) xrealloc (fileptr.data, (unsigned) file_stats->st_size + 1);
  else
    fileptr.data = (char *) xmalloc ((unsigned) file_stats->st_size + 1);

  size = INDENT_SYS_READ (fd, fileptr.data, fileptr.size);
  if (size == (unsigned int) -1)
    fatal ("Error reading input file %s", filename);
  if (close (fd) < 0)
    fatal ("Error closeing input file %s", filename);

  /* Apparently, the DOS stores files using CR-LF for newlines, but
     then the DOS `read' changes them into '\n'.  Thus, the size of the
     file on disc is larger than what is read into memory.  Thanks, Bill. */
  if (size < fileptr.size)
    fileptr.size = size;

  if (fileptr.name != 0)
    fileptr.name = (char *) xrealloc (fileptr.name, (unsigned) namelen + 1);
  else
    fileptr.name = (char *) xmalloc (namelen + 1);
  (void) memcpy (fileptr.name, filename, namelen);
  fileptr.name[namelen] = '\0';

  fileptr.data[fileptr.size] = '\0';

  return &fileptr;
}

/* This should come from stdio.h and be some system-optimal number */
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

/* Suck the standard input into a file_buffer structure, and
   return a pointer to that structure. */

struct file_buffer stdinptr;

struct file_buffer *
read_stdin ()
{
  unsigned int size = 15 * BUFSIZ;
  int ch;
  char *p;

  if (stdinptr.data != 0)
    free (stdinptr.data);

  stdinptr.data = (char *) xmalloc (size + 1);
  stdinptr.size = 0;
  p = stdinptr.data;
  do
    {
      while (stdinptr.size < size)
	{
	  ch = getc (stdin);
	  if (ch == EOF)
	    break;

	  *p++ = ch;
	  stdinptr.size++;
	}

      if (ch != EOF)
	{
	  size += (2 * BUFSIZ);
	  stdinptr.data = xrealloc (stdinptr.data, (unsigned) size);
	  p = stdinptr.data + stdinptr.size;
	}
    }
  while (ch != EOF);

  stdinptr.name = "Standard Input";

  stdinptr.data[stdinptr.size] = '\0';

  return &stdinptr;
}

/* Advance `buf_ptr' so that it points to the next line of input.

   If the next input line contains an indent control comment turning
   off formatting (a comment, C or C++, beginning with *INDENT-OFF*),
   disable indenting by settting `inhibited' to true; which will cause
   `dump_line ()' to simply print out all input lines without formatting
   until it finds a corresponding comment containing *INDENT-0N* which
   re-enables formatting.

   Note that if this is a C comment we do not look for the closing
   delimiter.  Note also that older version of this program also
   skipped lines containing *INDENT** which represented errors
   generated by indent in some previous formatting.  This version does
   not recognize such lines. */

void
fill_buffer ()
{
  char *p;
  int finished_a_line;

  /* indent() may be saving the text between "if (...)" and the following
     statement.  To do so, it uses another buffer (`save_com').  Switch
     back to the previous buffer here. */
  if (bp_save != 0)
    {
      buf_ptr = bp_save;
      buf_end = be_save;
      bp_save = be_save = 0;

      /* only return if there is really something in this buffer */
      if (buf_ptr < buf_end)
	return;
    }

  if (*in_prog_pos == '\0')
    {
      cur_line = buf_ptr = in_prog_pos;
      had_eof = true;
      return;
    }

  /* Here if we know there are chars to read.  The file is
     NULL-terminated, so we can always look one character ahead
     safely. */
  p = cur_line = in_prog_pos;
  finished_a_line = 0;
  do
    {
      while (*p == ' ' || *p == TAB)
	p++;

      /* If we are looking at the beginning of a comment, see
         if it turns off formatting with off-on directives. */
      if (*p == '/' && (*(p + 1) == '*' || *(p + 1) == '/'))
	{
	  p += 2;
	  while (*p == ' ' || *p == TAB)
	    p++;

	  /* Skip all lines between the indent off and on directives. */
	  if (!strncmp (p, "*INDENT-OFF*", 12))
	    inhibited = 1;
	}

      while (*p != '\0' && *p != EOL)
	p++;

      /* Here for newline -- finish up unless formatting is off */
      if (*p == EOL)
	{
	  finished_a_line = 1;
	  in_prog_pos = p + 1;
	}
      /* Here for embedded NULLs */
      else if ((unsigned int) (p - current_input->data) < current_input->size)
	{
	  WARNING ("Warning: File %s contains NULL-characters\n", current_input->name, 0);
	  p++;
	}
      /* Here for EOF with no terminating newline char. */
      else
	{
	  in_prog_pos = p;
	  finished_a_line = 1;
	}
    }
  while (!finished_a_line);

  buf_ptr = cur_line;
  buf_end = in_prog_pos;
  if (buf_break && (buf_break->offset >= e_code - s_code || buf_break->offset <= 0))
    clear_buf_break_list ();
}
