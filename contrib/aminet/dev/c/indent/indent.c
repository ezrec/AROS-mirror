/* Copyright (c) 1994, Joseph Arceneaux.  All rights reserved.

   Copyright (c) 1992, Free Software Foundation, Inc.  All rights reserved.

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
#include "indent.h"
#include <ctype.h>

void
usage ()
{
  fprintf (stderr, "usage: indent file [-o outfile ] [ options ]\n");
  fprintf (stderr, "       indent file1 file2 ... fileN [ options ]\n");
  exit (1);
}


/* Stuff that needs to be shared with the rest of indent.
   Documented in indent.h.  */
char *labbuf;
char *s_lab;
char *e_lab;
char *l_lab;
char *codebuf;
char *s_code;
char *e_code;
char *l_code;
char *combuf;
char *s_com;
char *e_com;
char *l_com;
struct buf save_com;
char *bp_save;
char *be_save;
int code_lines;
int line_no;
struct fstate keywordf;
struct fstate stringf;
struct fstate boxcomf;
struct fstate blkcomf;
struct fstate scomf;
struct fstate bodyf;
int break_comma;

/* Insure that BUFSTRUC has at least REQ more chars left, if not extend it.
      Note:  This may change bufstruc.ptr.  */
#define need_chars(bufstruc, req) \
  if ((bufstruc.end - bufstruc.ptr + (req)) >= bufstruc.size) \
{\
         int cur_chars = bufstruc.end - bufstruc.ptr;\
         bufstruc.size *= 2;\
         bufstruc.ptr = xrealloc (bufstruc.ptr,bufstruc.size);\
         bufstruc.end = bufstruc.ptr + cur_chars;\
}

/* True if there is an embedded comment on this code line */
int embedded_comment_on_line;

int else_or_endif;

/* structure indentation levels */
int *di_stack;

/* Currently allocated size of di_stack.  */
int di_stack_alloc;

/* when this is positive, we have seen a ? without
   the matching : in a <c>?<s>:<s> construct */
int squest;

#define CHECK_CODE_SIZE \
	if (e_code >= l_code) { \
	    register nsize = l_code-s_code+400; \
	    codebuf = (char *) xrealloc (codebuf, nsize); \
	    e_code = codebuf + (e_code-s_code) + 1; \
	    l_code = codebuf + nsize - 5; \
	    s_code = codebuf + 1; \
	}

#define CHECK_LAB_SIZE \
	if (e_lab >= l_lab) { \
	    register nsize = l_lab-s_lab+400; \
	    labbuf = (char *) xrealloc (labbuf, nsize); \
	    e_lab = labbuf + (e_lab-s_lab) + 1; \
	    l_lab = labbuf + nsize - 5; \
	    s_lab = labbuf + 1; \
	}

static void
indent (this_file)
     struct file_buffer *this_file;
{
  register int i;
  enum codes hd_type;
  register char *t_ptr;
  enum codes type_code;

  /* current indentation for declarations */
  int dec_ind = 0;

  /* used when buffering up comments to remember that
     a newline was passed over */
  int flushed_nl = 0;		
  int force_nl = 0;

  /* true when we've just see a case; determines what to do
     with the following colon */
  int scase = 0;

  /* true when in the expression part of if(...), while(...), etc. */
  int sp_sw = 0;

  /* True if we have just encountered the end of an if (...), etc. (i.e. the
     ')' of the if (...) was the last token).  The variable is set to 2 in
     the middle of the main token reading loop and is decremented at the
     beginning of the loop, so it will reach zero when the second token after
     the ')' is read.  */
  int last_token_ends_sp = 0;

  /* true iff last keyword was an else */
  int last_else = 0;


  in_prog = in_prog_pos = this_file->data;
  in_prog_size = this_file->size;

  hd_type = code_eof;
  dec_ind = 0;
  last_token_ends_sp = false;
  last_else = false;
  sp_sw = force_nl = false;
  scase = false;
  squest = false;

#if 0
  if (com_ind <= 1)
    com_ind = 2;		/* dont put normal comments before column 2 */
#endif

  if (troff)
    {
      if (bodyf.font[0] == 0)
	parsefont (&bodyf, "R");
      if (scomf.font[0] == 0)
	parsefont (&scomf, "I");
      if (blkcomf.font[0] == 0)
	blkcomf = scomf, blkcomf.size += 2;
      if (boxcomf.font[0] == 0)
	boxcomf = blkcomf;
      if (stringf.font[0] == 0)
	parsefont (&stringf, "L");
      if (keywordf.font[0] == 0)
	parsefont (&keywordf, "B");
      writefdef (&bodyf, 'B');
      writefdef (&scomf, 'C');
      writefdef (&blkcomf, 'L');
      writefdef (&boxcomf, 'X');
      writefdef (&stringf, 'S');
      writefdef (&keywordf, 'K');
    }
  if (comment_max_col <= 0)
    comment_max_col = max_col;
  if (decl_com_ind <= 0)	/* if not specified by user, set this */
    decl_com_ind =
      ljust_decl ? (com_ind <= 10 ? 2 : com_ind - 8) : com_ind;
  if (continuation_indent == 0)
    continuation_indent = ind_size;
  fill_buffer ();		/* get first batch of stuff into input buffer */

#if 0
  parse (semicolon);
#endif

  {
    register char *p = buf_ptr;
    register col = 1;

    while (1)
      {
	if (*p == ' ')
	  col++;
	else if (*p == TAB)
	  col = tabsize - (col % tabsize) + 1;
	else if (*p == EOL)
	  col = 1;
	else
	  break;

	p++;
      }

#if 0
    if (col > ind_size)
      parser_state_tos->ind_level = parser_state_tos->i_l_follow = col;
#endif
  }

  if (troff)
    {
      register char *p = in_name, *beg = in_name;

      while (*p)
	if (*p++ == '/')
	  beg = p;
      fprintf (output, ".Fn \"%s\"\n", beg);
    }
  /* START OF MAIN LOOP */

  while (1)
    {				/* this is the main loop.  it will go until
				   we reach eof */
      int is_procname;

      type_code = lexi ();	/* lexi reads one token.  "token" points to
				   the actual characters. lexi returns a code
				   indicating the type of token */

      if (last_token_ends_sp > 0)
	last_token_ends_sp--;
      is_procname = parser_state_tos->procname[0];

      /* The following code moves everything following an if (), while (),
         else, etc. up to the start of the following stmt to a buffer. This
         allows proper handling of both kinds of brace placement. */

      flushed_nl = false;
      while (parser_state_tos->search_brace)
	{
	  /* After scanning an if(), while (), etc., it might be necessary to
	     keep track of the text between the if() and the start of the
	     statement which follows.  Use save_com to do so.  */

	  switch (type_code)
	    {
	    case newline:
	      ++line_no;
	      flushed_nl = true;
	    case form_feed:
	      break;		/* form feeds and newlines found here will be
				   ignored */

	    case lbrace:	/* this is a brace that starts the compound
				   stmt */
	      if (save_com.end == save_com.ptr)
		{
		  /* ignore buffering if a comment wasnt stored up */
		  parser_state_tos->search_brace = false;
		  goto check_type;
		}
	      /* We need to put the '{' back into save_com somewhere.  */
	      if (btype_2)
		/* Put it at the beginning, e.g. if (foo) { / * comment here *
		   / */

		save_com.ptr[0] = '{';

	      else		{
		  /* Put it at the end, e.g. if (foo) / * comment here * / { */

		  /* Putting in this newline causes a dump_line to occur
		     right after the comment, thus insuring that it will be
		     put in the correct column.  */
		  *save_com.end++ = EOL;
		  *save_com.end++ = '{';
		}

	      /* Go to common code to get out of this loop.  */
	      goto sw_buffer;

	    case comment:	/* we have a comment, so we must copy it into
				   the buffer */
	      if (!flushed_nl || save_com.end != save_com.ptr)
		{
		  need_chars (save_com, 10);
		  if (save_com.end == save_com.ptr)
		    {		/* if this is the first comment, we must set
				   up the buffer */
		      save_com.ptr[0] = save_com.ptr[1] = ' ';
		      save_com.end = save_com.ptr + 2;
		    }
		  else
		    {
		      *save_com.end++ = EOL;	/* add newline between
						   comments */
		      *save_com.end++ = ' ';
		      --line_no;
		    }
		  *save_com.end++ = '/';	/* copy in start of comment */
		  *save_com.end++ = '*';

		  for (;;)
		    {		/* loop until we get to the end of the
				   comment */
		      /* make sure there is room for this character and
		         (while we're at it) the '/' we might add at the end
		         of the loop. */
		      need_chars (save_com, 2);
		      *save_com.end = *buf_ptr++;
		      if (buf_ptr >= buf_end)
			{
			  fill_buffer ();
			  if (had_eof)
			    {
			      diag (1, "Unclosed comment", 0, 0);
			      exit (1);
			    }
			}

		      if (*save_com.end++ == '*' && *buf_ptr == '/')
			break;	/* we are at end of comment */

		    }
		  *save_com.end++ = '/';	/* add ending slash */
		  if (++buf_ptr >= buf_end)	/* get past / in buffer */
		    fill_buffer ();
		  break;
		}
	    default:		/* it is the start of a normal statment */
	      if (flushed_nl)	/* if we flushed a newline, make sure it is
				   put back */
		force_nl = true;
	      if ((type_code == sp_paren && *token == 'i'
		   && last_else && else_if)
		  ||
		  (type_code == sp_nparen && *token == 'e'
		   && e_code != s_code && e_code[-1] == '}'))
		force_nl = false;

	      if (save_com.end == save_com.ptr)
		{
		  /* ignore buffering if comment wasnt saved up */
		  parser_state_tos->search_brace = false;
		  goto check_type;
		}
	      if (force_nl)
		{		/* if we should insert a nl here, put it into
				   the buffer */
		  force_nl = false;
		  --line_no;	/* this will be re-increased when the nl is
				   read from the buffer */
		  need_chars (save_com, 2);
		  *save_com.end++ = EOL;
		  *save_com.end++ = ' ';
		  if (verbose && !flushed_nl)	/* print error msg if the
						   line was not already
						   broken */
		    diag (0, "Line broken", 0, 0);
		  flushed_nl = false;
		}
	      for (t_ptr = token; t_ptr < token_end; ++t_ptr)
		{
		  need_chars (save_com, 1);
		  *save_com.end++ = *t_ptr;	/* copy token into temp
						   buffer */
		}
	      parser_state_tos->procname = "\0";

	    sw_buffer:
	      parser_state_tos->search_brace = false;	/* stop looking for
							   start of stmt */
	      bp_save = buf_ptr;/* save current input buffer */
	      be_save = buf_end;
	      buf_ptr = save_com.ptr;	/* fix so that subsequent calls to
					   lexi will take tokens out of
					   save_com */
	      need_chars (save_com, 1);
	      *save_com.end++ = ' ';	/* add trailing blank, just in case */
	      buf_end = save_com.end;
	      save_com.end = save_com.ptr;	/* make save_com empty */
	      break;
	    }			/* end of switch */
	  /* we must make this check, just in case there was an unexpected
	     EOF */
	  if (type_code != code_eof)
	    type_code = lexi ();/* read another token */
	  /* if (parser_state_tos->search_brace)
	     parser_state_tos->procname[0] = 0; */
	  if ((is_procname = parser_state_tos->procname[0]) && flushed_nl
	      && !procnames_start_line && parser_state_tos->in_decl
	      && type_code == ident)
	    flushed_nl = 0;
	}			/* end of while (search_brace) */
      last_else = 0;

    check_type:
      if (type_code == code_eof)
	{			/* we got eof */
	  if (s_lab != e_lab || s_code != e_code
	      || s_com != e_com)/* must dump end of line */
	    dump_line ();
	  if (parser_state_tos->tos > 1)	/* check for balanced braces */
	    diag (1, "Stuff missing from end of file.", 0, 0);

	  if (verbose)
	    {
	      printf ("There were %d output lines and %d comments\n",
		      (int) out_lines, (int) out_coms);
	      if (com_lines > 0 && code_lines > 0)
		printf ("(Lines with comments)/(Lines with code): %6.3f\n",
			(1.0 * com_lines) / code_lines);
	    }
	  fflush (output);
	  if (found_err)
	    exit (found_err);

	  return;
	}

      if ((type_code != comment) &&
	  (type_code != cplus_comment) &&
	  (type_code != newline) &&
	  (type_code != preesc) &&
	  (type_code != form_feed))
	{
	  if (force_nl &&
	      (type_code != semicolon) &&
	      (type_code != lbrace || !btype_2))
	    {
	      /* we should force a broken line here */
	      if (verbose && !flushed_nl)
		diag (0, "Line broken", 0, 0);
	      flushed_nl = false;
	      dump_line ();
	      parser_state_tos->want_blank = false;	/* dont insert blank at
							   line start */
	      force_nl = false;
	    }
	  parser_state_tos->in_stmt = true;	/* turn on flag which causes
						   an extra level of
						   indentation. this is
						   turned off by a ; or } */
	  if (s_com != e_com)
	    {			/* the turkey has embedded a comment in a
				   line. Move it from the com buffer to the
				   code buffer.  */
	      /* Do not add a space before the comment if it is the first
	         thing on the line.  */
	      if (e_code != s_code)
		{
		  *e_code++ = ' ';
		  embedded_comment_on_line = 2;
		}
	      else
		embedded_comment_on_line = 1;

	      for (t_ptr = s_com; *t_ptr; ++t_ptr)
		{
		  CHECK_CODE_SIZE;
		  *e_code++ = *t_ptr;
		}
	      *e_code++ = ' ';
	      *e_code = '\0';	/* null terminate code sect */
	      parser_state_tos->want_blank = false;
	      e_com = s_com;
	    }
	}
      else if (type_code != comment
	       && type_code != cplus_comment)
	/* preserve force_nl thru a comment but
	   cancel forced newline after newline, form feed, etc */
	   force_nl = false;



      /*-----------------------------------------------------*\
      |	   do switch on type of token scanned		      |
      \*-----------------------------------------------------*/
      CHECK_CODE_SIZE;
      switch (type_code)
	{			/* now, decide what to do with the token */

	case form_feed:	/* found a form feed in line */
	  parser_state_tos->use_ff = true;	/* a form feed is treated
						   much like a newline */
	  dump_line ();
	  parser_state_tos->want_blank = false;
	  break;

	case newline:
	  if (((parser_state_tos->last_token != comma
		|| !leave_comma || !break_comma
		|| parser_state_tos->p_l_follow > 0
		|| parser_state_tos->block_init
		|| s_com != e_com)
	       && ((parser_state_tos->last_token != rbrace || !btype_2
		    || ! parser_state_tos->in_decl)))
	      || (compute_code_target () + (e_code - s_code)
		  > max_col - tabsize))
	    {
	      dump_line ();
	      parser_state_tos->want_blank = false;
	    }
	  /* If we were on the line with a #else or a #endif, we aren't
	     anymore.  */
	  else_or_endif = false;
	  ++line_no;		/* keep track of input line number */
	  break;

	case lparen:
	  /* Braces in initializer lists should be put on new lines. This is
	     necessary so that -gnu does not cause things like char
	     *this_is_a_string_array[] = { "foo", "this_string_does_not_fit",
	     "nor_does_this_rather_long_string" } which is what happens
	     because we are trying to line the strings up with the
	     parentheses, and those that are too long are moved to the right
	     an ugly amount.
	
	     However, if the current line is empty, the left brace is
	     already on a new line, so don't molest it.  */
	  if (token[0] == '{'
	      && (s_code != e_code || s_com != e_com || s_lab != e_lab))
	    {
	      dump_line ();
	      /* Do not put a space before the '{'.  */
	      parser_state_tos->want_blank = false;
	    }

	  /* Count parens so we know how deep we are.  */
	  if (++parser_state_tos->p_l_follow
	      >= parser_state_tos->paren_indents_size)
	    {
	      parser_state_tos->paren_indents_size *= 2;
	      parser_state_tos->paren_indents = (short *)
		xrealloc ((char *) parser_state_tos->paren_indents,
			  parser_state_tos->paren_indents_size
			  * sizeof (short));
	    }
	  parser_state_tos->paren_depth++;
	  if (parser_state_tos->want_blank && *token != '['
	      && (parser_state_tos->last_token != ident || proc_calls_space
		  || (parser_state_tos->its_a_keyword
		      && (!parser_state_tos->sizeof_keyword
			  || blank_after_sizeof))))
	    *e_code++ = ' ';

	  if (parser_state_tos->in_decl && !parser_state_tos->block_init)
	    {
	      if (troff
		  && !parser_state_tos->dumped_decl_indent
		  && !is_procname
		  && parser_state_tos->last_token == decl)
		{
		  parser_state_tos->dumped_decl_indent = 1;
		  sprintf (e_code, "\n.Du %dp+\200p \"%.*s\"\n",
			   (int) (dec_ind * 7),
			   token_end - token, token);
		  e_code += strlen (e_code);
		}
	      else if (*token != '[')
		{
		  while ((e_code - s_code) < dec_ind)
		    {
		      CHECK_CODE_SIZE;
		      *e_code++ = ' ';
		    }
		  *e_code++ = token[0];
		}
	      else
		*e_code++ = token[0];
	    }
	  else
	    *e_code++ = token[0];

	  parser_state_tos->paren_indents[parser_state_tos->p_l_follow - 1]
	    = e_code - s_code;
	  if (sp_sw && parser_state_tos->p_l_follow == 1
	      && extra_expression_indent
	      && parser_state_tos->paren_indents[0] < 2 * ind_size)
	    parser_state_tos->paren_indents[0] = 2 * ind_size;
	  parser_state_tos->want_blank = false;

	  if (parser_state_tos->in_or_st
	      && *token == '('
	      && parser_state_tos->tos <= 2)
	    {
	      /* this is a kluge to make sure that declarations will be
	         aligned right if proc decl has an explicit type on it, i.e.
	         "int a(x) {..." */
	      parse_lparen_in_decl ();

	      /* Turn off flag for structure decl or initialization.  */
	      parser_state_tos->in_or_st = false;
	    }
	  if (parser_state_tos->sizeof_keyword)
	    parser_state_tos->sizeof_mask |= 1 << parser_state_tos->p_l_follow;

	  /* The '(' that starts a cast can never be preceeded by an
	     indentifier or decl.  */
	  if (parser_state_tos->last_token == decl
	      || (parser_state_tos->last_token == ident
		  && parser_state_tos->last_rw != rw_return))
	    parser_state_tos->noncast_mask |=
	      1 << parser_state_tos->p_l_follow;
	  else
	    parser_state_tos->noncast_mask &=
	      ~(1 << parser_state_tos->p_l_follow);

	  break;

	case rparen:
	  parser_state_tos->paren_depth--;
	  if (parser_state_tos->cast_mask
	      & (1 << parser_state_tos->p_l_follow)
	      & ~parser_state_tos->sizeof_mask)
	    {
	      parser_state_tos->last_u_d = true;
	      parser_state_tos->cast_mask &=
		(1 << parser_state_tos->p_l_follow) - 1;
	      if (!parser_state_tos->cast_mask && cast_space)
		parser_state_tos->want_blank = true;
	      else
		parser_state_tos->want_blank = false;
	    }
	  else if (parser_state_tos->in_decl
		   && ! parser_state_tos->block_init
		   && parser_state_tos->paren_depth == 0)
	    parser_state_tos->want_blank = true;

	  parser_state_tos->sizeof_mask
	    &= (1 << parser_state_tos->p_l_follow) - 1;
	  if (--parser_state_tos->p_l_follow < 0)
	    {
	      parser_state_tos->p_l_follow = 0;
	      diag (0, "Extra %c", (int) *token, 0);
	    }

	  /* if the paren starts the line, then indent it */
	  if (e_code == s_code)
	    {
	      int level = parser_state_tos->p_l_follow;
	      parser_state_tos->paren_level = level;
	      if (level > 0)
		paren_target = -parser_state_tos->paren_indents[level - 1];
	      else
		paren_target = 0;
	    }
	  *e_code++ = token[0];

#if 0
	  if (!parser_state_tos->cast_mask || cast_space)
	    parser_state_tos->want_blank = true;
#endif

	  /* check for end of if (...), or some such */
	  if (sp_sw && (parser_state_tos->p_l_follow == 0))
	    {

	      /* Indicate that we have just left the parenthesized expression
	         of a while, if, or for, unless we are getting out of the
	         parenthesized expression of the while of a do-while loop.
	         (do-while is different because a semicolon immediately
	         following this will not indicate a null loop body).  */
	      if (parser_state_tos->p_stack[parser_state_tos->tos]
		  != dohead)
		last_token_ends_sp = 2;
	      sp_sw = false;
	      force_nl = true;	/* must force newline after if */
	      parser_state_tos->last_u_d = true;	/* inform lexi that a
							   following operator is
							   unary */
	      parser_state_tos->in_stmt = false;	/* dont use stmt
							   continuation
							   indentation */

	      parse (hd_type);	/* let parser worry about if, or whatever */
	    }
	  parser_state_tos->search_brace = btype_2;	/* this should insure
							   that constructs such
							   as main(){...} and
							   int[]{...} have their
							   braces put in the
							   right place */
	  break;

	case unary_op:		/* this could be any unary operation */
	  if (parser_state_tos->want_blank)
	    *e_code++ = ' ';

	  if (troff
	      && !parser_state_tos->dumped_decl_indent
	      && parser_state_tos->in_decl && !is_procname)
	    {
	      sprintf (e_code, "\n.Du %dp+\200p \"%.*s\"\n",
		       (int) (dec_ind * 7),
		       token_end - token, token);
	      parser_state_tos->dumped_decl_indent = 1;
	      e_code += strlen (e_code);
	    }
	  else
	    {
	      char *res = token;
	      char *res_end = token_end;

	      /* if this is a unary op in a declaration, we should
		 indent this token */
	      if (parser_state_tos->paren_depth == 0
		  && parser_state_tos->in_decl
		  && !parser_state_tos->block_init)
		{
		  while ((e_code - s_code) < (dec_ind - (token_end - token)))
		    {
		      CHECK_CODE_SIZE;
		      *e_code++ = ' ';
		    }
		}

	      if (troff && token[0] == '-' && token[1] == '>')
		{
		  static char resval[] = "\\(->";
		  res = resval;
		  res_end = res + sizeof (resval);
		}

	      for (t_ptr = res; t_ptr < res_end; ++t_ptr)
		{
		  CHECK_CODE_SIZE;
		  *e_code++ = *t_ptr;
		}
	    }
	  parser_state_tos->want_blank = false;
	  break;

	case binary_op:	/* any binary operation */
	  if (parser_state_tos->want_blank
	      || (e_code > s_code && *e_code != ' '))
	    *e_code++ = ' ';

	  {
	    char *res = token;
	    char *res_end = token_end;
#define set_res(str) \
	      {\
		static char resval[] = str;\
		res = resval;\
		res_end = res + sizeof(resval);\
	      }

	    if (troff)
	      switch (token[0])
		{
		case '<':
		  if (token[1] == '=')
		    set_res ("\\(<=");
		  break;
		case '>':
		  if (token[1] == '=')
		    set_res ("\\(>=");
		  break;
		case '!':
		  if (token[1] == '=')
		    set_res ("\\(!=");
		  break;
		case '|':
		  if (token[1] == '|')
		    {
		      set_res ("\\(br\\(br");
		    }
		  else if (token[1] == 0)
		    set_res ("\\(br");
		  break;
		}

	    for (t_ptr = res; t_ptr < res_end; ++t_ptr)
	      {
		CHECK_CODE_SIZE;
		*e_code++ = *t_ptr;	/* move the operator */
	      }
	  }
	  parser_state_tos->want_blank = true;
	  break;

	case postop:		/* got a trailing ++ or -- */
	  *e_code++ = token[0];
	  *e_code++ = token[1];
	  parser_state_tos->want_blank = true;
	  break;

	case question:		/* got a ? */
	  squest++;		/* this will be used when a later colon
				   appears so we can distinguish the
				   <c>?<n>:<n> construct */
	  if (parser_state_tos->want_blank)
	    *e_code++ = ' ';
	  *e_code++ = '?';
	  parser_state_tos->want_blank = true;
	  break;

	case casestmt:		/* got word 'case' or 'default' */
	  scase = true;		/* so we can process the later colon
				   properly */
	  goto copy_id;

	case colon:		/* got a ':' */
	  if (squest > 0)
	    {			/* it is part of the <c>?<n>: <n> construct */
	      --squest;
	      if (parser_state_tos->want_blank)
		*e_code++ = ' ';
	      *e_code++ = ':';
	      parser_state_tos->want_blank = true;
	      break;
	    }
	  if (parser_state_tos->in_decl)
	    {
	      *e_code++ = ':';
	      parser_state_tos->want_blank = false;
	      break;
	    }
	  parser_state_tos->in_stmt = false;	/* seeing a label does not
						   imply we are in a stmt */
	  for (t_ptr = s_code; *t_ptr; ++t_ptr)
	    *e_lab++ = *t_ptr;	/* turn everything so far into a label */
	  e_code = s_code;
	  *e_lab++ = ':';
	  *e_lab++ = ' ';
	  *e_lab = '\0';
	  /* parser_state_tos->pcas e will be used by dump_line to decide
	     how to indent the label. force_nl will force a case n: to be
	     on a line by itself */
	  force_nl = parser_state_tos->pcase = scase;
	  scase = false;
	  parser_state_tos->want_blank = false;
	  break;

	case semicolon:
	  /* we are not in an initialization or structure declaration */
	  parser_state_tos->in_or_st = false;
	  scase = false;
	  squest = 0;
	  /* The following code doesn't seem to do much good. Just because
	     we've found something like extern int foo();    or int (*foo)();
	     doesn't mean we are out of a declaration.  Now if it was serving
	     some purpose we'll have to address that.... if
	     (parser_state_tos->last_token == rparen)
	     parser_state_tos->in_parameter_declaration = 0; */
	  parser_state_tos->cast_mask = 0;
	  parser_state_tos->sizeof_mask = 0;
	  parser_state_tos->block_init = 0;
	  parser_state_tos->block_init_level = 0;
	  parser_state_tos->just_saw_decl--;

	  if (parser_state_tos->in_decl
	      && s_code == e_code
	      && !parser_state_tos->block_init)
	    while ((e_code - s_code) < (dec_ind - 1))
	      {
		CHECK_CODE_SIZE;
		*e_code++ = ' ';
	      }

	  /* if we were in a first level structure declaration,
	     we aren't any more */
	  parser_state_tos->in_decl = (parser_state_tos->dec_nest > 0);

#if 0	/* Changed to allow "{}" inside parens, as when
	   passed to a macro.  -jla */
	  if ((!sp_sw || hd_type != forstmt)
	      && parser_state_tos->p_l_follow > 0)
	    {

	      /* This should be true iff there were unbalanced parens in the
	         stmt.  It is a bit complicated, because the semicolon might
	         be in a for stmt */
	      diag (1, "Unbalanced parens", 0, 0);
	      parser_state_tos->p_l_follow = 0;
	      if (sp_sw)
		{		/* this is a check for a if, while, etc. with
				   unbalanced parens */
		  sp_sw = false;
		  parse (hd_type);	/* dont lose the if, or whatever */
		}
	    }
#endif

	  /* If we have a semicolon following an if, while, or for, and the
	     user wants us to, we should insert a space (to show that there
	     is a null statement there).  */
	  if (last_token_ends_sp && space_sp_semicolon)
	    {
	      *e_code++ = ' ';
	    }
	  *e_code++ = ';';
	  parser_state_tos->want_blank = true;
	  /* we are no longer in the middle of a stmt */
	  parser_state_tos->in_stmt = (parser_state_tos->p_l_follow > 0);

	  if (!sp_sw)
	    {			/* if not if for (;;) */
	      parse (semicolon);/* let parser know about end of stmt */
	      force_nl = true;	/* force newline after a end of stmt */
	    }
	  break;

	case lbrace:		/* got a '{' */
	  parser_state_tos->in_stmt = false;	/* dont indent the {} */
	  if (!parser_state_tos->block_init)
	    force_nl = true;	/* force other stuff on same line as '{' onto
				   new line */
	  else if (parser_state_tos->block_init_level <= 0)
	    parser_state_tos->block_init_level = 1;
	  else
	    parser_state_tos->block_init_level++;

	  if (s_code != e_code && !parser_state_tos->block_init)
	    {
	      if (!btype_2)
		{
		  dump_line ();
		  parser_state_tos->want_blank = false;
		}
	      else
		{
		  if (parser_state_tos->in_parameter_declaration
		      && !parser_state_tos->in_or_st)
		    {
		      parser_state_tos->i_l_follow = 0;
		      dump_line ();
		      parser_state_tos->want_blank = false;
		    }
		  else
		    parser_state_tos->want_blank = true;
		}
	    }
	  if (parser_state_tos->in_parameter_declaration)
	    prefix_blankline_requested = 0;

#if 0	/* Changed to allow "{}" inside parens, as when
	   passed to a macro.  -jla */
	  if (parser_state_tos->p_l_follow > 0)
	    {			/* check for preceeding unbalanced parens */
	      diag (1, "Unbalanced parens", 0, 0);
	      parser_state_tos->p_l_follow = 0;
	      if (sp_sw)
		{		/* check for unclosed if, for, etc. */
		  sp_sw = false;
		  parse (hd_type);
		  parser_state_tos->ind_level = parser_state_tos->i_l_follow;
		}
	    }
#endif
	  if (s_code == e_code)
	    parser_state_tos->ind_stmt = false;	/* dont put extra indentation
						   on line with '{' */
	  if (parser_state_tos->in_decl && parser_state_tos->in_or_st)
	    {
	      /* This is a structure declaration.  */
	      if (parser_state_tos->dec_nest >= di_stack_alloc)
		{
		  di_stack_alloc *= 2;
		  di_stack = (int *)
		    xrealloc ((char *) di_stack,
			      di_stack_alloc * sizeof (*di_stack));
		}
	      di_stack[parser_state_tos->dec_nest++] = dec_ind;
	      /* ?		dec_ind = 0; */
	    }
	  else
	    {
	      parser_state_tos->in_decl = false;
	      parser_state_tos->decl_on_line = false;	/* we cant be in the
							   middle of a
							   declaration, so dont
							   do special
							   indentation of
							   comments */

#if 0				/* Doesn't work currently. */
	      if (blanklines_after_declarations_at_proctop
		  && parser_state_tos->in_parameter_declaration)
		postfix_blankline_requested = 1;
#endif
	      parser_state_tos->in_parameter_declaration = 0;
	    }
	  dec_ind = 0;

	  /* We are no longer looking for an initializer or structure. Needed
	     so that the '=' in "enum bar {a = 1" does not get interpreted as
	     the start of an initializer.  */
	  parser_state_tos->in_or_st = false;

	  parse (lbrace);	/* let parser know about this */
	  if (parser_state_tos->want_blank)	/* put a blank before '{' if
						   '{' is not at start of
						   line */
	    *e_code++ = ' ';
	  parser_state_tos->want_blank = false;
	  *e_code++ = '{';
	  parser_state_tos->just_saw_decl = 0;
	  break;

	case rbrace:		/* got a '}' */
	  /* semicolons can be omitted in declarations */
	  if (parser_state_tos->p_stack[parser_state_tos->tos] == decl
	      && !parser_state_tos->block_init)
	    parse (semicolon);
#if 0	/* Changed to allow "{}" inside parens, as when
	   passed to a macro.  -jla */
	  if (parser_state_tos->p_l_follow)
	    {			/* check for unclosed if, for, else. */
	      diag (1, "Unbalanced parens", 0, 0);
	      parser_state_tos->p_l_follow = 0;
	      sp_sw = false;
	    }
#endif

	  parser_state_tos->just_saw_decl = 0;
	  parser_state_tos->block_init_level--;
	  if (s_code != e_code && !parser_state_tos->block_init)
	    {			/* '}' must be first on line */
	      if (verbose)
		diag (0, "Line broken", 0, 0);
	      dump_line ();
	    }
	  *e_code++ = '}';
	  parser_state_tos->want_blank = true;
	  parser_state_tos->in_stmt = parser_state_tos->ind_stmt = false;
	  if (parser_state_tos->dec_nest > 0)
	    {			/* we are in multi-level structure
				   declaration */
	      dec_ind = di_stack[--parser_state_tos->dec_nest];
	      if (parser_state_tos->dec_nest == 0
		  && !parser_state_tos->in_parameter_declaration)
		parser_state_tos->just_saw_decl = 2;
	      parser_state_tos->in_decl = true;
	    }
	  prefix_blankline_requested = 0;
	  parse (rbrace);	/* let parser know about this */
	  parser_state_tos->search_brace
	    = (cuddle_else
	     && parser_state_tos->p_stack[parser_state_tos->tos] == ifhead);

	  if ((parser_state_tos->p_stack[parser_state_tos->tos] == stmtl
	       && ((parser_state_tos->last_rw != rw_struct_like
		    && parser_state_tos->last_rw != rw_decl)
		   || ! btype_2))
	      || (parser_state_tos->p_stack[parser_state_tos->tos] == ifhead)
	      || (parser_state_tos->p_stack[parser_state_tos->tos] == dohead
		  && !btype_2))
	    force_nl = true;
	  else if (parser_state_tos->tos <= 1
		   && blanklines_after_procs
		   && parser_state_tos->dec_nest <= 0)
	    postfix_blankline_requested = 1;

#if 0
	  parser_state_tos->search_brace
	    = (cuddle_else
	       && parser_state_tos->p_stack[parser_state_tos->tos] == ifhead
	       && (parser_state_tos->il[parser_state_tos->tos]
		   >= parser_state_tos->ind_level));
#endif

	  break;

	case swstmt:		/* got keyword "switch" */
	  sp_sw = true;
	  hd_type = swstmt;	/* keep this for when we have seen the
				   expression */
	  parser_state_tos->in_decl = false;
	  goto copy_id;		/* go move the token into buffer */

	case sp_paren:		/* token is if, while, for */
	  sp_sw = true;		/* the interesting stuff is done after the
				   expression is scanned */
	  hd_type = (*token == 'i' ? ifstmt :
		     (*token == 'w' ? whilestmt : forstmt));

	  /* remember the type of header for later use by parser */
	  goto copy_id;		/* copy the token into line */

	case sp_nparen:	/* got else, do */
	  parser_state_tos->in_stmt = false;
	  if (*token == 'e')
	    {
	      if (e_code != s_code && (!cuddle_else || e_code[-1] != '}'))
		{
		  if (verbose)
		    diag (0, "Line broken", 0, 0);
		  dump_line ();	/* make sure this starts a line */
		  parser_state_tos->want_blank = false;
		}
	      force_nl = true;	/* also, following stuff must go onto new
				   line */
	      last_else = 1;
	      parse (elselit);
	    }
	  else
	    {
	      if (e_code != s_code)
		{		/* make sure this starts a line */
		  if (verbose)
		    diag (0, "Line broken", 0, 0);
		  dump_line ();
		  parser_state_tos->want_blank = false;
		}
	      force_nl = true;	/* also, following stuff must go onto new
				   line */
	      last_else = 0;
	      parse (dolit);
	    }
	  goto copy_id;		/* move the token into line */

	case decl:		/* we have a declaration type (int, register,
				   etc.) */

	  if (! parser_state_tos->sizeof_mask)
	    parse (decl);

	  if (parser_state_tos->last_token == rparen
	      && parser_state_tos->tos <= 1)
	    {
	      parser_state_tos->in_parameter_declaration = 1;
	      if (s_code != e_code)
		{
		  dump_line ();
		  parser_state_tos->want_blank = false;
		}
	    }
	  if (parser_state_tos->in_parameter_declaration
	      && indent_parameters
	      && parser_state_tos->dec_nest == 0
	      && parser_state_tos->p_l_follow == 0)
	    {
	      parser_state_tos->ind_level
		= parser_state_tos->i_l_follow = indent_parameters;
	      parser_state_tos->ind_stmt = 0;
	    }

	  /* in_or_st set for struct or initialization decl. Don't set it if
	     we're in ansi prototype */
	  if (!parser_state_tos->paren_depth)
	    parser_state_tos->in_or_st = true;

	  parser_state_tos->in_decl = parser_state_tos->decl_on_line = true;
#if 0
	  if (!parser_state_tos->in_or_st && parser_state_tos->dec_nest <= 0)
#endif
	    if (parser_state_tos->dec_nest <= 0)
	      parser_state_tos->just_saw_decl = 2;
	  if (prefix_blankline_requested
	      && (parser_state_tos->block_init != 0
		  || parser_state_tos->block_init_level != -1
		  || parser_state_tos->last_token != rbrace
		  || e_code != s_code
		  || e_lab != s_lab
		  || e_com != s_com))
	    prefix_blankline_requested = 0;
	  i = token_end - token + 1;	/* get length of token plus 1 */

	  /* dec_ind = e_code - s_code + (parser_state_tos->decl_indent>i ?
	     parser_state_tos->decl_indent : i); */
	  dec_ind = decl_indent > 0 ? decl_indent : i;
	  goto copy_id;

	case ident:		/* got an identifier or constant */
	  /* If we are in a declaration, we must indent identifier. But not
	     inside the parentheses of an ANSI function declaration.  */
	  if (parser_state_tos->in_decl
	      && parser_state_tos->p_l_follow == 0
	      && parser_state_tos->last_token != rbrace)
	    {
	      if (parser_state_tos->want_blank)
		*e_code++ = ' ';
	      parser_state_tos->want_blank = false;
	      if (is_procname == 0 || !procnames_start_line)
		{
		  if (!parser_state_tos->block_init)
		    if (troff && !parser_state_tos->dumped_decl_indent)
		      {
			sprintf (e_code, "\n.De %dp+\200p\n",
				 (int) (dec_ind * 7));
			parser_state_tos->dumped_decl_indent = 1;
			e_code += strlen (e_code);
		      }
		    else
		      while ((e_code - s_code) < dec_ind)
			{
			  CHECK_CODE_SIZE;
			  *e_code++ = ' ';
			}
		}
	      else
		{
		  if (dec_ind && s_code != e_code)
		    dump_line ();
		  dec_ind = 0;
		  parser_state_tos->want_blank = false;
		}
	    }
	  else if (sp_sw && parser_state_tos->p_l_follow == 0)
	    {
	      sp_sw = false;
	      force_nl = true;
	      parser_state_tos->last_u_d = true;
	      parser_state_tos->in_stmt = false;
	      parse (hd_type);
	    }
	copy_id:
	  if (parser_state_tos->want_blank)
	    *e_code++ = ' ';
	  if (troff && parser_state_tos->its_a_keyword)
	    {
	      e_code = chfont (&bodyf, &keywordf, e_code);
	      for (t_ptr = token; t_ptr < token_end; ++t_ptr)
		{
		  CHECK_CODE_SIZE;
		  *e_code++ = keywordf.allcaps && islower (*t_ptr)
		    ? toupper (*t_ptr) : *t_ptr;
		}
	      e_code = chfont (&keywordf, &bodyf, e_code);
	    }
	  else
	    {
	      /* Troff mode requires that strings be processed specially.  */
	      if (troff && (*token == '"' || *token == '\''))
		{
		  char qchar;

		  qchar = *token;
		  *e_code++ = '`';
		  if (qchar == '"')
		    *e_code++ = '`';
		  e_code = chfont (&bodyf, &stringf, e_code);

		  t_ptr = token + 1;
		  while (t_ptr < token_end)
		    {
		      *e_code = *t_ptr++;
		      if (*e_code == '\\')
			{
			  *++e_code = '\\';
			  if (*t_ptr == '\\')
			    *++e_code = '\\';
			  /* Copy char after backslash.  */
			  *++e_code = *t_ptr++;
			  /* Point after the last char we copied.  */
			  e_code++;
			}
		    }
		  e_code = chfont (&stringf, &bodyf, e_code - 1);
		  if (qchar == '"')
		    *e_code++ = '\'';
		}
	      else
		for (t_ptr = token; t_ptr < token_end; ++t_ptr)
		  {
		    CHECK_CODE_SIZE;
		    *e_code++ = *t_ptr;
		  }
	    }
	  parser_state_tos->want_blank = true;

	  /* If the token is va_dcl, it appears without a semicolon, so we
	     need to pretend that one was there.  */
	  if ((token_end - token) == 6
	      && strncmp (token, "va_dcl", 6) == 0)
	    {
	      parser_state_tos->in_or_st = false;
	      parser_state_tos->just_saw_decl--;
	      parser_state_tos->in_decl = 0;
	      parse (semicolon);
	      force_nl = true;
	    }
	  break;

	case period:		/* treat a period kind of like a binary
				   operation */
	  *e_code++ = '.';	/* move the period into line */
	  parser_state_tos->want_blank = false;	/* dont put a blank after a
						   period */
	  break;

	case comma:
	  /* only put blank after comma if comma does not start the line */
	  parser_state_tos->want_blank = (s_code != e_code);
	  if (parser_state_tos->paren_depth == 0
	      && parser_state_tos->in_decl
	      && is_procname == 0
	      && !parser_state_tos->block_init)
	    while ((e_code - s_code) < (dec_ind - 1))
	      {
		CHECK_CODE_SIZE;
		*e_code++ = ' ';
	      }

	  *e_code++ = ',';
	  if (parser_state_tos->p_l_follow == 0)
	    {
	      if (parser_state_tos->block_init_level <= 0)
		parser_state_tos->block_init = 0;
	      /* If we are in a declaration, and either the user wants all
	         comma'd declarations broken, or the line is getting too
	         long, break the line.  */
	      if (break_comma &&
		  (!leave_comma
		   || (compute_code_target () + (e_code - s_code)
		       > max_col - tabsize)))
		force_nl = true;
	    }
	  break;

	case preesc:		/* got the character '#' */
	  if ((s_com != e_com) ||
	      (s_lab != e_lab) ||
	      (s_code != e_code))
	    dump_line ();
	  {
	    int in_comment = 0;
	    int in_cplus_comment = 0;
	    int com_start = 0;
	    char quote = 0;
	    int com_end = 0;

	    /* ANSI allows spaces between '#' and preprocessor directives.
	       Remove such spaces unless user has specified "-lps", in
	       which case also leave any space preceeding the '#'. */
	    if (leave_preproc_space)
	      {
		char *p = cur_line;
		while (p < buf_ptr)
		  *e_lab++ = *p++;

		while (*buf_ptr == ' ' || *buf_ptr == TAB)
		  *e_lab++ = *buf_ptr++;
	      }
	    else
	      {
		*e_lab++ = '#';
		while (*buf_ptr == ' ' || *buf_ptr == TAB)
		  buf_ptr++;
	      }

	    while (*buf_ptr != EOL || (in_comment && !had_eof))
	      {
		CHECK_LAB_SIZE;
		*e_lab = *buf_ptr++;
		if (buf_ptr >= buf_end)
		  fill_buffer ();
		switch (*e_lab++)
		  {
		  case BACKSLASH:
		    if (troff)
		      *e_lab++ = BACKSLASH;
		    if (!in_comment && !in_cplus_comment)
		      {
			*e_lab++ = *buf_ptr++;
			if (buf_ptr >= buf_end)
			  fill_buffer ();
		      }
		    break;
		  case '/':
		    if ((*buf_ptr == '*' || *buf_ptr == '/')
			&& !in_comment && !in_cplus_comment && !quote)
		      {
			if (*buf_ptr == '/')
			  in_cplus_comment = 1;
			else
			  in_comment = 1;
			*e_lab++ = *buf_ptr++;
			com_start = e_lab - s_lab - 2;
		      }
		    break;

		  case '"':
		  case '\'':
		    if (!quote)
		      quote = e_lab[-1];
		    else
		      if (e_lab[-1] == quote)
			quote = 0;
		    break;

		  case '*':
		    if (*buf_ptr == '/' && in_comment)
		      {
			in_comment = 0;
			*e_lab++ = *buf_ptr++;
			com_end = e_lab - s_lab;
		      }
		    break;
		  }
	      }

	    while (e_lab > s_lab && (e_lab[-1] == ' ' || e_lab[-1] == TAB))
	      e_lab--;

	    if (in_cplus_comment)
	      {
		in_cplus_comment = 0;
		*e_lab++ = *buf_ptr++;
		com_end = e_lab - s_lab;
	      }

	    if (e_lab - s_lab == com_end && bp_save == 0)
	      {			/* comment on preprocessor line */
		if (save_com.end != save_com.ptr)
		  {
		    need_chars (save_com, 2);
		    *save_com.end++ = EOL;	/* add newline between
						   comments */
		    *save_com.end++ = ' ';
		    --line_no;
		  }
		need_chars (save_com, com_end - com_start);
		strncpy (save_com.end, s_lab + com_start,
			 com_end - com_start);
		save_com.end += com_end - com_start;

		e_lab = s_lab + com_start;
		while (e_lab > s_lab
		       && (e_lab[-1] == ' ' || e_lab[-1] == TAB))
		  e_lab--;
		bp_save = buf_ptr;	/* save current input buffer */
		be_save = buf_end;
		buf_ptr = save_com.ptr;	/* fix so that subsequent calls to
					   lexi will take tokens out of
					   save_com */
		need_chars (save_com, 1);
		buf_end = save_com.end;
		save_com.end = save_com.ptr;	/* make save_com empty */
	      }
	    *e_lab = '\0';	/* null terminate line */
	    parser_state_tos->pcase = false;
	  }

	  if (strncmp (s_lab + 1, "if", 2) == 0)
	    {
	      if (blanklines_around_conditional_compilation)
		{
		  register c;
		  prefix_blankline_requested++;
		  while ((c = *in_prog_pos++) == EOL);
		  in_prog_pos--;
		}
	      {
		/* Push a copy of the parser_state onto the stack. All
		   manipulations will use the copy at the top of stack, and
		   then we can return to the previous state by popping the
		   stack.  */
		struct parser_state *new;

		new = (struct parser_state *)
		  xmalloc (sizeof (struct parser_state));
		memcpy (new, parser_state_tos, sizeof (struct parser_state));

		/* We need to copy the dynamically allocated arrays in the
		   struct parser_state too.  */
		new->p_stack = (enum codes *)
		  xmalloc (parser_state_tos->p_stack_size
			   * sizeof (enum codes));
		memcpy (new->p_stack, parser_state_tos->p_stack,
		      parser_state_tos->p_stack_size * sizeof (enum codes));

		new->il = (int *)
		  xmalloc (parser_state_tos->p_stack_size * sizeof (int));
		memcpy (new->il, parser_state_tos->il,
			parser_state_tos->p_stack_size * sizeof (int));

		new->cstk = (int *)
		  xmalloc (parser_state_tos->p_stack_size
			   * sizeof (int));
		memcpy (new->cstk, parser_state_tos->cstk,
			parser_state_tos->p_stack_size * sizeof (int));

		new->paren_indents = (short *) xmalloc
		  (parser_state_tos->paren_indents_size * sizeof (short));
		memcpy (new->paren_indents, parser_state_tos->paren_indents,
		     parser_state_tos->paren_indents_size * sizeof (short));

		new->next = parser_state_tos;
		parser_state_tos = new;
	      }
	    }
	  else if (strncmp (s_lab + 1, "else", 4) == 0)
	    {
	      /* When we get #else, we want to restore the parser state to
	         what it was before the matching #if, so that things get
	         lined up with the code before the #if.  However, we do not
	         want to pop the stack; we just want to copy the second to
	         top elt of the stack because when we encounter the #endif,
	         it will pop the stack.  */
	      else_or_endif = true;
	      if (parser_state_tos->next)
		{
		  /* First save the addresses of the arrays for the top of
		     stack.  */
		  enum codes *tos_p_stack = parser_state_tos->p_stack;
		  int *tos_il = parser_state_tos->il;
		  int *tos_cstk = parser_state_tos->cstk;
		  short *tos_paren_indents =
		  parser_state_tos->paren_indents;
		  struct parser_state *second =
		  parser_state_tos->next;

		  memcpy (parser_state_tos, second,
			  sizeof (struct parser_state));
		  parser_state_tos->next = second;

		  /* Now copy the arrays from the second to top of stack to
		     the top of stack.  */
		  /* Since the p_stack, etc. arrays only grow, never shrink,
		     we know that they will be big enough to fit the array
		     from the second to top of stack.  */
		  parser_state_tos->p_stack = tos_p_stack;
		  memcpy (parser_state_tos->p_stack,
			  parser_state_tos->next->p_stack,
			  parser_state_tos->p_stack_size
			  * sizeof (enum codes));

		  parser_state_tos->il = tos_il;
		  memcpy (parser_state_tos->il,
			  parser_state_tos->next->il,
			  parser_state_tos->p_stack_size * sizeof (int));

		  parser_state_tos->cstk = tos_cstk;
		  memcpy (parser_state_tos->cstk,
			  parser_state_tos->next->cstk,
			  parser_state_tos->p_stack_size * sizeof (int));

		  parser_state_tos->paren_indents = tos_paren_indents;
		  memcpy (parser_state_tos->paren_indents,
			  parser_state_tos->next->paren_indents,
			  parser_state_tos->paren_indents_size
			  * sizeof (short));
		}
	      else
		diag (1, "Unmatched #else", 0, 0);
	    }
	  else if (strncmp (s_lab + 1, "endif", 5) == 0)
	    {
	      else_or_endif = true;
	      /* We want to remove the second to top elt on the stack, which
	         was put there by #if and was used to restore the stack at
	         the #else (if there was one). We want to leave the top of
	         stack unmolested so that the state which we have been using
	         is unchanged.  */
	      if (parser_state_tos->next)
		{
		  struct parser_state *second = parser_state_tos->next;

		  parser_state_tos->next = second->next;
		  free (second->p_stack);
		  free (second->il);
		  free (second->cstk);
		  free (second->paren_indents);
		  free (second);
		}
	      else
		diag (1, "Unmatched #endif", 0, 0);
	      if (blanklines_around_conditional_compilation)
		{
		  postfix_blankline_requested++;
		  n_real_blanklines = 0;
		}
	    }

	  /* Normally, subsequent processing of the newline character
	     causes the line to be printed.  The following clause handles
	     a special case (comma-separated declarations separated
	     by the preprocessor lines) where this doesn't happen. */
	  if (parser_state_tos->last_token == comma
	      && parser_state_tos->p_l_follow <= 0
	      && leave_comma && !parser_state_tos->block_init
	      && break_comma && s_com == e_com)
	    {
	      dump_line ();
	      parser_state_tos->want_blank = false;
	    }
	  break;

	  /* A C or C++ comment. */
	case comment:
	case cplus_comment:
	  if (flushed_nl)
	    {
	      flushed_nl = false;
	      dump_line ();
	      parser_state_tos->want_blank = false;
	      force_nl = false;
	    }
	  print_comment ();
	  break;
	}			/* end of big switch stmt */

      *e_code = '\0';		/* make sure code section is null terminated */
      if (type_code != comment
	  && type_code != cplus_comment
	  && type_code != newline
	  && type_code != preesc
	  && type_code != form_feed)
	parser_state_tos->last_token = type_code;

    }				/* end of main while (1) loop */
}



char *set_profile ();
void set_defaults ();
int set_option ();

/* Points to current input file name */
char *in_name = 0;

/* Points to the current input buffer */
struct file_buffer *current_input = 0;

/* Points to the name of the output file */
char *out_name = 0;

/* How many input files were specified */
int input_files;

/* Names of all input files */
char **in_file_names;

/* Initial number of input filenames to allocate. */
int max_input_files = 128;


#ifdef DEBUG
int debug;
#endif

main (argc, argv)
     int argc;
     char **argv;
{
  register int i;
  char *profile_pathname = 0;
  int using_stdin = false;

#ifdef DEBUG
  if (debug)
    debug_init ();
#endif

  init_parser ();
  initialize_backups ();

  output = 0;
  input_files = 0;
  in_file_names = (char **) xmalloc (max_input_files * sizeof (char *));

  set_defaults ();
  for (i = 1; i < argc; ++i)
    if (strcmp (argv[i], "-npro") == 0
	|| strcmp (argv[i], "--ignore-profile") == 0
	|| strcmp (argv[i], "+ignore-profile") == 0)
      break;
  if (i >= argc)
    profile_pathname = set_profile ();

  for (i = 1; i < argc; ++i)
    {
      if (argv[i][0] != '-' && argv[i][0] != '+')	/* Filename */
	{
	  if (expect_output_file == true)	/* Last arg was "-o" */
	    {
	      if (out_name != 0)
		{
		  fprintf (stderr, "indent: only one output file (2nd was %s)\n", argv[i]);
		  exit (1);
		}

	      if (input_files > 1)
		{
		  fprintf (stderr, "indent: only one input file when output file is specified\n");
		  exit (1);
		}

	      out_name = argv[i];
	      expect_output_file = false;
	      continue;
	    }
	  else
	    {
	      if (using_stdin)
		{
		  fprintf (stderr, "indent: can't have filenames when specifying standard input\n");
		  exit (1);
		}

	      input_files++;
	      if (input_files > 1)
		{
		  if (out_name != 0)
		    {
		      fprintf (stderr, "indent: only one input file when output file is specified\n");
		      exit (1);
		    }

		  if (use_stdout != 0)
		    {
		      fprintf (stderr, "indent: only one input file when stdout is used\n");
		      exit (1);
		    }

		  if (input_files > max_input_files)
		    {
		      max_input_files = 2 * max_input_files;
		      in_file_names
			= (char **) xrealloc ((char *) in_file_names,
					      (max_input_files
					       * sizeof (char *)));
		    }
		}

	      in_file_names[input_files - 1] = argv[i];
	    }
	}
      else
	{
	  /* '-' as filename means stdin. */
	  if (argv[i][0] == '-' && argv[i][1] == '\0')
	    {
	      if (input_files > 0)
		{
		  fprintf (stderr, "indent: can't have filenames when specifying standard input\n");
		  exit (1);
		}

	      using_stdin = true;
	    }
	  else
	    i += set_option (argv[i], (i < argc ? argv[i + 1] : 0), 1);
	}
    }

  if (verbose && profile_pathname)
    fprintf (stderr, "Read profile %s\n", profile_pathname);

  if (input_files > 1)
    {
      /* When multiple input files are specified, make a backup copy
	 and then output the indented code into the same filename. */

      for (i = 0; input_files; i++, input_files--)
	{
	  current_input = read_file (in_file_names[i]);
	  in_name = out_name = in_file_names[i];
	  output = fopen (out_name, "w");
	  if (output == 0)
	    {
	      fprintf (stderr, "indent: can't create %s\n", out_name);
	      exit (1);
	    }

	  make_backup (current_input);
	  reset_parser ();
	  indent (current_input);
	  if (fclose (output) != 0)
	    sys_error (out_name);
	}
    }
  else
    {
      /* One input stream -- specified file, or stdin */

      if (input_files == 0 || using_stdin)
	{
	  input_files = 1;
	  in_file_names[0] = "Standard input";
	  current_input = read_stdin ();
	}
      else
	/* 1 input file */
	{
	  current_input = read_file (in_file_names[0]);
	  if (!out_name && !use_stdout)
	    {
	      out_name = in_file_names[0];
	      make_backup (current_input);
	    }
	}
      in_name = in_file_names[0];

      /* Uset stdout if it was specified ("-st"), or neither input
         nor output file was specified, or we're doing troff. */
      if (use_stdout || !out_name || troff)
	output = stdout;
      else
	{
	  output = fopen (out_name, "w");
	  if (output == 0)
	    {
	      fprintf (stderr, "indent: can't create %s\n", out_name);
	      exit (1);
	    }
	}

      reset_parser ();
      indent (current_input);
    }

  exit (0);
}
