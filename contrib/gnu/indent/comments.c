/* Copyright (c) 1993 - 1998, Joseph Arceneaux.  All rights reserved.

   This file is subject to the terms of the GNU General Public License as
   published by the Free Software Foundation.  A copy of this license is
   included with this software distribution in the file COPYING.  If you
   do not have a copy, you may obtain a copy by writing to the Free
   Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details. */

#include "sys.h"
#include "indent.h"
#include "io.h"
#include "comments.h"
#include "globs.h"
#include "parse.h"

#include <string.h>

RCSTAG_CC ("$Id$");

/* Check the limits of the comment buffer, and expand as neccessary. */

#define CHECK_COM_SIZE \
	if (e_com >= l_com) \
          { \
	    int nsize = l_com-s_com+400; \
	    combuf = (char *) xrealloc (combuf, nsize); \
	    e_com = combuf + (e_com-s_com) + 1; \
	    l_com = combuf + nsize - 5; \
	    s_com = combuf + 1; \
	  }

/* Output a comment.  `buf_ptr' is pointing to the character after
   the beginning comment delimiter when this is called.  This handles
   both C and C++ comments.

   As far as indent is concerned, there are basically two types
   of comments -- those on lines by themselves and those which are
   on lines with other code.  Variables (and the options specifying them)
   affecting the printing of comments are:

   `format_comments'                ("fca"):  Ignore newlines in the
       comment and perform filling up to `comment_max_col'.  Double
       newlines indicate paragraph breaks.

   `format_col1_comments'           ("fc1"):  Format comments which
       begin in column 1.

   `unindent_displace'              ("d"):  The hanging indentation for
       comments which do not appear to the right of code.

   `comment_delimiter_on_blankline' ("cdb"):  If set, place the comment
       delimiters on lines by themselves.  This only affects comments
       which are not to the right of code.

   `com_ind'                        ("c"):  The column in which to begin
       comments that are to the right of code.

   `decl_com_ind'                   ("cd"):  The column in which to begin
       comments that are to the right of declarations.

   `else_endif_col'                 ("cp"):  The column in which to begin
       comments to the right of preprocessor directives.

   `star_comment_cont'              ("sc"):  Place a star ('*') to the
       left of the comment body.

   `comment_max_col'                ("lc"): The length of a comment line.
       Formatted comments which extend past this column will be continued on
       the following line.  If this option is not specified, `max_col' is
       used.
   */

void
print_comment ()
{
  register int column, format;
  enum codes comment_type;

  int start_column, found_column;
  int first_comment_line, right_margin;
  int boxed_comment, stars, blankline_delims, paragraph_break, merge_blank_comment_lines;

  int two_contiguous_comments = 0;
  int save_length = 0;
  char *save_ptr = 0;
  char *text_on_line = 0;
  char *line_break_ptr = 0;
  char *start_delim;

  char *line_preamble;
  int line_preamble_length, visible_preamble;
  int suppress_cdb = 0;

  /* GDB_HOOK_print_comment() */

  /* Increment the parser stack, as we will store some things
     there for dump_line to use. */
  inc_pstack ();

  /* Have to do it this way because this piece of shit program doesn't
     always place the last token code on the stack. */
  if (*(token + 1) == '/')
    comment_type = cplus_comment;
  else
    comment_type = comment;

  /* First, decide what kind of comment this is: C++, C, or boxed C.
     Even if this appears to be a normal C comment, we may change our
     minds if we find a star in the right column of the second line,
     in which case that's a boxed comment too. */
  if (comment_type == cplus_comment)
    {
      start_delim = "//";
      line_preamble = "// ";
      line_preamble_length = 3;
      visible_preamble = 1;
      boxed_comment = 0;
      stars = 0;
      blankline_delims = 0;
    }
  else if (*buf_ptr == '*' || *buf_ptr == '-' || *buf_ptr == '=' || *buf_ptr == '_'
	   || (parser_state_tos->col_1 && !format_col1_comments))
    /* Boxed comment.  This block of code will return. */
    {
      int comment_lines_count = 1;

      found_column = start_column = current_column () - 2;
      parser_state_tos->box_com = 1;
      parser_state_tos->com_col = found_column;

      if (blanklines_before_blockcomments)
	prefix_blankline_requested = 1;

      *e_com++ = '/';
      *e_com++ = '*';
      while (1)
	{
	  do
	    {
	      if (*buf_ptr == EOL)	/* Count line numbers within comment blocks */
		++line_no;
	      *e_com++ = *buf_ptr++;
	      CHECK_COM_SIZE;
	    }
	  while (*buf_ptr != '*' && buf_ptr < buf_end);

	  /* We have reached the end of the comment, and it's all on
	     this line. */
	  if (*buf_ptr == '*' && *(buf_ptr + 1) == '/')
	    {
	      if (buf_ptr == buf_end)
		fill_buffer ();

	      buf_ptr += 2;
	      if (buf_ptr == buf_end)
		fill_buffer ();

	      *e_com++ = '*';
	      *e_com++ = '/';
	      *e_com = '\0';
	      parser_state_tos->tos--;

	      /* If this is the only line of a boxed comment, it may
	         be after some other text (e.g., #if foo <comment>),
	         in which case we want to specify the correct column.
	         In the other cases, the leading spaces account for
	         the columns and we start it in column 1. */

	      if (comment_lines_count > 1)
		parser_state_tos->com_col = 1;
	      else
		parser_state_tos->com_col = found_column;
	      return;
	    }

	  /* End of the line, or end of file. */
	  if (buf_ptr == buf_end)
	    {
	      if (*(buf_ptr - 1) == EOL)
		{
		  *(--e_com) = '\0';
		  dump_line (true);
		  comment_lines_count++;
		  parser_state_tos->com_col = 1;
		}

	      fill_buffer ();
	      if (had_eof)
		{
		  *e_com++ = '\0';
		  parser_state_tos->tos--;
		  parser_state_tos->com_col = start_column;
		  return;
		}
	    }
	}
    }
  else
    {
      start_delim = "/*";
      line_preamble = 0;
      line_preamble_length = 0;
      visible_preamble = 0;
      boxed_comment = 0;
      stars = star_comment_cont;
      blankline_delims = comment_delimiter_on_blankline;
    }

  paragraph_break = 0;
  merge_blank_comment_lines = 0;
  first_comment_line = com_lines;
  right_margin = comment_max_col;

  /* Now, compute the correct indentation for this comment
     and whether or not it should be formatted. */
  found_column = current_column () - 2;

  if ((s_lab == e_lab) && (s_code == e_code))
    /* First handle comments which begin the line. */
    {
      if (parser_state_tos->col_1 && !format_col1_comments)
	{
	  format = format_col1_comments;
	  start_column = 1;
	}
      else if (s_com != e_com)
	/* The fool has a line consisting of two contiguous comments.
	   In this case, we don't try too hard, 'cause nothing will
	   look good. */
	{
	  format = 0;
	  start_column = found_column;
	  two_contiguous_comments = 1;
	}
      else
	{
	  format = format_comments;

	  if (parser_state_tos->ind_level <= 0
	      && (!parser_state_tos->in_stmt || (parser_state_tos->in_decl && parser_state_tos->paren_level == 0)))
	    start_column = found_column;
	  else
	    /* This comment is within a procedure or other code. */
	    {
	      start_column = compute_code_target () - unindent_displace;
	      if (start_column < 0)
		start_column = 1;
	    }
	}
    }
  else
    /* This comment follows code of some sort. */
    {
      int target;

      suppress_cdb = 1;

      /* First, compute where the comment SHOULD go. */
      if (parser_state_tos->decl_on_line)
	target = decl_com_ind;
      else if (else_or_endif)
	target = else_endif_col;
      else
	target = com_ind;

      /* Now determine if the code on the line is short enough
         to allow the comment to begin where it should. */
      if (s_code != e_code)
	start_column = count_columns (compute_code_target (), s_code, NULL_CHAR);
      else
	/* s_lab != e_lab : there is a label here. */
	start_column = count_columns (compute_label_target (), s_lab, NULL_CHAR);

      if (start_column < target)
	start_column = target;
      else
	{
	  /* If the too-long code is a pre-processor command,
	     start the comment 1 space afterwards, otherwise
	     start at the next tab mark. */
	  if (else_or_endif)
	    {
	      start_column++;
	      else_or_endif = false;
	    }
	  else
	    start_column += tabsize - ((start_column - 1) % tabsize);
	}

      format = format_comments;
    }

  if (!line_preamble)
    {
      line_preamble_length = 3;
      if (stars)
	{
	  line_preamble = " * ";
	  visible_preamble = 1;
	}
      else
	{
	  line_preamble = "   ";
	  visible_preamble = 0;
	}
    }

  /* These are the parser stack variables used to communicate
     formatting information to dump_line (). */
  parser_state_tos->com_col = (two_contiguous_comments ? 1 : start_column);
  parser_state_tos->box_com = boxed_comment;

  /* Output the beginning comment delimiter.  They are both two
     characters long. */
  *e_com++ = *start_delim;
  *e_com++ = *(start_delim + 1);
  column = start_column + 2;

  /* If the user specified -cdb, put the delimiter on one line. */
  if (blankline_delims && !suppress_cdb)
    {
      char *p = buf_ptr;

      *e_com = '\0';
      dump_line (true);

      /* Check if the delimiter was already on a line by itself,
         and skip whitespace if formating. */
      while (*p == ' ' || *p == TAB)
	p++;
      if (*p == EOL)
	buf_ptr = p + 1;
      else if (format)
	buf_ptr = p;
      if (buf_ptr >= buf_end)
	fill_buffer ();

      column = start_column;
      goto begin_line;
    }
  else if (format)
    {
      *e_com++ = ' ';
      column = start_column + 3;
      while (*buf_ptr == ' ' || *buf_ptr == TAB)
	if (++buf_ptr >= buf_end)
	  fill_buffer ();
    }

  /* Iterate through the lines of the comment */
  while (!had_eof)
    {
      /* Iterate through the characters on one line */
      while (!had_eof)
	{
	  CHECK_COM_SIZE;

	  switch (*buf_ptr)
	    {
	    case ' ':
	    case TAB:
	      /* If formatting, and previous break marker is
	         nonexistant, or before text on line, reset
	         it to here. */
	      if (format && line_break_ptr < text_on_line)
		line_break_ptr = e_com;

	      if (format)
		{
		  /* Don't write two spaces after another, unless the first space it preceeded by a dot. */
		  if (e_com == s_com || e_com[-1] != ' ' || e_com - 1 == s_com || e_com[-2] == '.')
		    {
		      *e_com++ = ' ';
		      column++;
		    }
		}
	      else if (*buf_ptr == ' ')
		{
		  *e_com++ = ' ';
		  column++;
		}
	      else
		{
		  /* Convert the tab to the appropriate number of spaces,
		     based on the column we found the comment in, not
		     the one we're printing in. */
		  int tab_width = (tabsize - ((column + found_column - start_column - 1) % tabsize));
		  column += tab_width;
		  while (tab_width--)
		    *e_com++ = ' ';
		}
	      break;

	    case EOL:
	      /* We may be at the end of a C++ comment */
	      if (comment_type == cplus_comment)
		{
		cplus_exit:
		  parser_state_tos->tos--;
		  parser_state_tos->com_col = (two_contiguous_comments ? 1 : start_column);
		  parser_state_tos->box_com = boxed_comment;
		  *e_com = 0;
		  return;
		}

	      if (format)
		{
		  /* Newline and null are the two characters which
		     end an input line, so check here if we need to
		     get the next line. */
		  if (*buf_ptr == EOL)
		    ++line_no;
		  buf_ptr++;
		  if (buf_ptr >= buf_end)
		    fill_buffer ();

		  /* If there are any spaces between the text and this
		     newline character, remove them. */
		  if (e_com > line_break_ptr && text_on_line < line_break_ptr)
		    e_com = line_break_ptr;

		  /* If this is "\n\n", or "\n<whitespace>\n",
		     it's a paragraph break. */
		  while (*buf_ptr == TAB || *buf_ptr == ' ')
		    if (++buf_ptr >= buf_end)
		      fill_buffer ();
		  if (*buf_ptr == EOL || !text_on_line)
		    {
		      paragraph_break = 1;
		      goto end_line;
		    }

		  /* Also need to eat the preamble. */
		  if (!boxed_comment && current_column () == found_column + 1 && buf_ptr[0] == '*' && buf_ptr[1] != '/')
		    {
		      if (++buf_ptr >= buf_end)
			fill_buffer ();
		      if (*buf_ptr == ' ' && ++buf_ptr >= buf_end)
			fill_buffer ();
		    }

		  /* This is a single newline.  Transform it (and any
		     following whitespace) into a single blank. */
		  if (e_com[-1] != ' ')
		    {
		      line_break_ptr = e_com;
		      *e_com++ = ' ';
		      column++;
		    }
		  continue;
		}

	      /* We are printing this line "as is", so output it
	         and continue on to the next line. */
	      goto end_line;

	    case '*':
	      /* Check if we've reached the end of the comment. */
	      if (comment_type == comment)
		{
		  if (*(buf_ptr + 1) == '/')
		    {
		      /* If it's not a boxed comment, put some whitespace
		         before the ending delimiter.  Otherwise, simply
		         insert the delimiter. */
		      if (!boxed_comment)
			{
			  if (text_on_line)
			    {
			      if (blankline_delims && !suppress_cdb)
				{
				  *e_com = '\0';
				  dump_line (true);
				  *e_com++ = ' ';
				}
			      else
				/* Insert space before closing delim */
			      if (*(e_com - 1) != ' ' && *(e_com - 1) != TAB)
				*e_com++ = ' ';
			    }
			  /* If no text on line, then line is completely empty
			     or starts with preamble, or is beginning of
			     comment and starts with beginning delimiter. */
			  else if (s_com == e_com || *s_com != '/')
			    {
			      e_com = s_com;
			      *e_com++ = ' ';
			    }
			  else
			    /* This is case of first comment line.  Test
			       with:
			       if (first_comment_line != com_lines)
			       abort (); */
			  if (*(e_com - 1) != ' ' && *(e_com - 1) != TAB)
			    *e_com++ = ' ';
			}

		      /* Now insert the ending delimiter */
		      *e_com++ = '*';
		      *e_com++ = '/';
		      *e_com = '\0';

		      /* Skip any whitespace following the comment.  If
		         there is only whitespace after it, print the line.

		         NOTE:  We're not printing the line: TRY IT! */
		      buf_ptr += 2;
		      while (*buf_ptr == ' ' || *buf_ptr == TAB)
			buf_ptr++;
		      if (buf_ptr >= buf_end)
			fill_buffer ();

		      parser_state_tos->tos--;
		      parser_state_tos->com_col = (two_contiguous_comments ? 1 : start_column);
		      parser_state_tos->box_com = boxed_comment;
		      return;
		    }

		  /* If this star is on the second line of the
		     comment in the same column as the star of the
		     beginning delimiter, then consider it
		     a boxed comment. */
		  if (first_comment_line == com_lines - 1 && e_com == s_com + line_preamble_length
		      && current_column () == found_column + 1)
		    {
		      /* Account for change in line_preamble_length: */
		      column -= line_preamble_length - 1;
		      line_preamble = " ";
		      line_preamble_length = 1;
		      boxed_comment = 1;
		      format = 0;
		      blankline_delims = 0;
		      *s_com = ' ';
		      *(s_com + 1) = '*';
		      text_on_line = e_com = s_com + 2;
		      column++;
		      break;
		    }
		}
	      /* If it was not the end of the comment, drop through
	         and insert the star on the line. */

	    default:
	      /* Some textual character. */
	      text_on_line = e_com;
	      *e_com++ = *buf_ptr;
	      column++;
	      break;
	    }


	  /* If we are formatting, check that we haven't exceeded the
	     line length.  If we haven't set line_break_ptr, keep going. */
	  if (format && column > right_margin && line_break_ptr)
	    {
	      if (line_break_ptr < e_com - 1)
		/* Here if we are really "breaking" the line:  the line
		   break is before some text we've seen. */
		{
		  *line_break_ptr = '\0';
		  save_ptr = line_break_ptr + 1;
		  save_length = e_com - save_ptr;
		  e_com = line_break_ptr;

		  /* If we had to go past `right_margin' to print stuff out,
		     extend `right_margin' out to this point. */
		  if ((column - save_length) > right_margin)
		    right_margin = column - save_length;
		}
	      else
		/* The line break is after the last text;  we're really
		   truncating the line. */
		{
		  if (comment_type == cplus_comment)
		    {
		      while (*buf_ptr == TAB || *buf_ptr == ' ')
			buf_ptr++;
		      buf_ptr--;
		      if (*buf_ptr == EOL)
			goto cplus_exit;
		    }
		  else
		    {
		      while (*buf_ptr == TAB || *buf_ptr == ' ' || *buf_ptr == EOL)
			buf_ptr++;

		      buf_ptr--;
		    }

		  *e_com = '\0';
		}
	      goto end_line;
	    }

	  if (*buf_ptr == EOL)
	    ++line_no;
	  buf_ptr++;
	  if (buf_ptr == buf_end)
	    fill_buffer ();
	}


    end_line:
      /* Compress pure whitespace lines into newlines. */
      if (!text_on_line && !visible_preamble && !(first_comment_line == com_lines))
	e_com = s_com;
      *e_com = '\0';
      dump_line (true);
      /* We're in the middle of a C-comment, don't add blank lines! */
      prefix_blankline_requested = 0;

      /* If formatting (paragraph_break is only used for formatted
         comments) and user wants blank lines merged, kill all white
         space after the "\n\n" indicating a paragraph break. */
      if (paragraph_break)
	{
	  if (merge_blank_comment_lines)
	    while (*buf_ptr == EOL || *buf_ptr == ' ' || *buf_ptr == TAB)
	      {
		if (*buf_ptr == EOL)
		  ++line_no;
		if (++buf_ptr >= buf_end)
		  fill_buffer ();
	      }
	  paragraph_break = 0;
	}
      else
	{
	  /* If it was a paragraph break (`if' clause), we scanned ahead
	     one character.  So, here in the `else' clause, advance buf_ptr. */
	  if (*buf_ptr == EOL)
	    ++line_no;
	  buf_ptr++;
	  if (buf_ptr >= buf_end)
	    fill_buffer ();
	}

    begin_line:
      if (had_eof)
	break;

      /* Indent the line properly.  If it's a boxed comment, align with
         the '*' in the beginning slash-star and start inserting there.
         Otherwise, insert blanks for alignment, or a star if the
         user specified -sc. */
      if (line_preamble)
	{
	  (void) memcpy (e_com, line_preamble, line_preamble_length);
	  e_com += line_preamble_length;
	  column = start_column + line_preamble_length;
	}
      else
	column = start_column;
      line_break_ptr = 0;

      /* If we have broken the line before the end for formatting,
         copy the text after the break onto the beginning of this
         new comment line. */
      if (save_ptr)
	{
	  while ((*save_ptr == ' ' || *save_ptr == TAB) && save_length)
	    {
	      save_ptr++;
	      save_length--;
	    }
	  (void) memcpy (e_com, save_ptr, save_length);
	  text_on_line = e_com;
	  e_com += save_length;
	  /* We only break if formatting, in which cases there
	     are no tabs, only spaces. */
	  column += save_length;
	  save_ptr = 0;
	  save_length = 0;
	}
      else
	{
	  while (*buf_ptr == ' ' || *buf_ptr == TAB)
	    if (++buf_ptr >= buf_end)
	      fill_buffer ();
	  text_on_line = 0;
	}
    }

  parser_state_tos->tos--;
  parser_state_tos->com_col = (two_contiguous_comments ? 1 : start_column);
  parser_state_tos->box_com = boxed_comment;
}
