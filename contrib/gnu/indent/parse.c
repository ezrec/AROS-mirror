/* Copyright (c) 1999, 2000 Carlo Wood.  All rights reserved.
   Copyright (c) 1994 Joseph Arceneaux.  All rights reserved.
   Copyright (c) 1985 Sun Microsystems, Inc. Copyright (c) 1980 The Regents
   of the University of California. Copyright (c) 1976 Board of Trustees of
   the University of Illinois. All rights reserved.

   Redistribution and use in source and binary forms are permitted provided
   that the above copyright notice and this paragraph are duplicated in all
   such forms and that any documentation, advertising materials, and other
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
#include "parse.h"
#include "globs.h"

RCSTAG_CC ("$Id$");

struct parser_state *parser_state_tos;

#define INITIAL_BUFFER_SIZE 1000
#define INITIAL_STACK_SIZE 2

void
init_parser ()
{
  parser_state_tos = (struct parser_state *) xmalloc (sizeof (struct parser_state));
  /* GDB_HOOK_parser_state_tos */
  parser_state_tos->p_stack_size = INITIAL_STACK_SIZE;
  parser_state_tos->p_stack = (enum codes *) xmalloc (INITIAL_STACK_SIZE * sizeof (enum codes));
  parser_state_tos->il = (int *) xmalloc (INITIAL_STACK_SIZE * sizeof (int));
  parser_state_tos->cstk = (int *) xmalloc (INITIAL_STACK_SIZE * sizeof (int));
  parser_state_tos->paren_indents_size = 8;
  parser_state_tos->paren_indents = (short *) xmalloc (parser_state_tos->paren_indents_size * sizeof (short));

  /* Although these are supposed to grow if we reach the end,
     I can find no place in the code which does this. */
  combuf = (char *) xmalloc (INITIAL_BUFFER_SIZE);
  labbuf = (char *) xmalloc (INITIAL_BUFFER_SIZE);
  codebuf = (char *) xmalloc (INITIAL_BUFFER_SIZE);

  save_com.size = INITIAL_BUFFER_SIZE;
  save_com.end = save_com.ptr = xmalloc (save_com.size);
  save_com.len = save_com.column = 0;

  di_stack_alloc = 2;
  di_stack = (int *) xmalloc (di_stack_alloc * sizeof (*di_stack));

}

void
reset_parser ()
{
  parser_state_tos->next = 0;
  parser_state_tos->tos = 0;
  parser_state_tos->p_stack[0] = stmt;	/* this is the parser's stack */
  parser_state_tos->last_nl = true;	/* this is true if the last thing
					   scanned was a newline */
  parser_state_tos->last_token = start_token;
  parser_state_tos->last_saw_nl = false;
  parser_state_tos->broken_at_non_nl = false;
  parser_state_tos->box_com = false;
  parser_state_tos->cast_mask = 0;
  parser_state_tos->noncast_mask = 0;
  parser_state_tos->sizeof_mask = 0;
  parser_state_tos->block_init = 0;
  parser_state_tos->block_init_level = 0;
  parser_state_tos->col_1 = false;
  parser_state_tos->com_col = 0;
  parser_state_tos->dec_nest = 0;
  parser_state_tos->i_l_follow = 0;
  parser_state_tos->ind_level = 0;
  parser_state_tos->last_u_d = false;
  parser_state_tos->p_l_follow = 0;
  parser_state_tos->paren_level = 0;
  parser_state_tos->paren_depth = 0;
  parser_state_tos->search_brace = false;
  parser_state_tos->use_ff = false;
  parser_state_tos->its_a_keyword = false;
  parser_state_tos->sizeof_keyword = false;
  parser_state_tos->dumped_decl_indent = false;
  parser_state_tos->in_parameter_declaration = false;
  parser_state_tos->just_saw_decl = false;
  parser_state_tos->in_decl = false;
  parser_state_tos->decl_on_line = false;
  parser_state_tos->in_or_st = false;
  parser_state_tos->bl_line = true;
  parser_state_tos->want_blank = false;
  parser_state_tos->in_stmt = false;
  parser_state_tos->ind_stmt = false;
  parser_state_tos->procname = "\0";
  parser_state_tos->procname_end = "\0";
  parser_state_tos->classname = "\0";
  parser_state_tos->classname_end = "\0";
  parser_state_tos->pcase = false;
  parser_state_tos->dec_nest = 0;
  parser_state_tos->can_break = bb_none;
  parser_state_tos->saw_double_colon = false;

  parser_state_tos->il[0] = 0;
  parser_state_tos->cstk[0] = 0;

  save_com.len = save_com.column = 0;

  di_stack[parser_state_tos->dec_nest] = 0;

  l_com = combuf + INITIAL_BUFFER_SIZE - 5;
  l_lab = labbuf + INITIAL_BUFFER_SIZE - 5;
  l_code = codebuf + INITIAL_BUFFER_SIZE - 5;
  combuf[0] = codebuf[0] = labbuf[0] = ' ';
  combuf[1] = codebuf[1] = labbuf[1] = '\0';

  else_or_endif = false;
  s_lab = e_lab = labbuf + 1;
  s_code = e_code = codebuf + 1;
  s_com = e_com = combuf + 1;

  line_no = 1;
  had_eof = false;
  break_comma = false;
  bp_save = 0;
  be_save = 0;
  if (tabsize <= 0)
    tabsize = 1;
  prefix_blankline_requested = 0;
}

/* like ++parser_state_tos->tos but checks for stack overflow and extends
   stack if necessary.  */

int
inc_pstack ()
{
  if (++parser_state_tos->tos >= parser_state_tos->p_stack_size)
    {
      parser_state_tos->p_stack_size *= 2;
      parser_state_tos->p_stack =
	(enum codes *) xrealloc ((char *) parser_state_tos->p_stack,
				 parser_state_tos->p_stack_size * sizeof (enum codes));
      parser_state_tos->il =
	(int *) xrealloc ((char *) parser_state_tos->il, parser_state_tos->p_stack_size * sizeof (int));
      parser_state_tos->cstk =
	(int *) xrealloc ((char *) parser_state_tos->cstk, parser_state_tos->p_stack_size * sizeof (int));
    }

  parser_state_tos->cstk[parser_state_tos->tos] = parser_state_tos->cstk[parser_state_tos->tos - 1];
  return parser_state_tos->tos;
}

#ifdef DEBUG
static char **debug_symbol_strings;

void
debug_init ()
{
  int size = ((int) number_of_codes) * sizeof (char *);

  debug_symbol_strings = (char **) xmalloc (size);

  debug_symbol_strings[code_eof] = "code_eof";
  debug_symbol_strings[newline] = "newline";
  debug_symbol_strings[lparen] = "lparen";
  debug_symbol_strings[rparen] = "rparen";
  debug_symbol_strings[start_token] = "start_token";
  debug_symbol_strings[unary_op] = "unary_op";
  debug_symbol_strings[binary_op] = "binary_op";
  debug_symbol_strings[postop] = "postop";
  debug_symbol_strings[question] = "question";
  debug_symbol_strings[casestmt] = "casestmt";
  debug_symbol_strings[colon] = "colon";
  debug_symbol_strings[doublecolon] = "doublecolon";
  debug_symbol_strings[semicolon] = "semicolon";
  debug_symbol_strings[lbrace] = "lbrace";
  debug_symbol_strings[rbrace] = "rbrace";
  debug_symbol_strings[ident] = "ident";
  debug_symbol_strings[overloaded] = "overloaded";
  debug_symbol_strings[cpp_operator] = "cpp_operator";
  debug_symbol_strings[comma] = "comma";
  debug_symbol_strings[comment] = "comment";
  debug_symbol_strings[cplus_comment] = "cplus_comment";
  debug_symbol_strings[swstmt] = "swstmt";
  debug_symbol_strings[preesc] = "preesc";
  debug_symbol_strings[form_feed] = "form_feed";
  debug_symbol_strings[decl] = "decl";
  debug_symbol_strings[sp_paren] = "sp_paren";
  debug_symbol_strings[sp_nparen] = "sp_nparen";
  debug_symbol_strings[sp_else] = "sp_else";
  debug_symbol_strings[ifstmt] = "ifstmt";
  debug_symbol_strings[elseifstmt] = "elseifstmt";
  debug_symbol_strings[whilestmt] = "whilestmt";
  debug_symbol_strings[forstmt] = "forstmt";
  debug_symbol_strings[stmt] = "stmt";
  debug_symbol_strings[stmtl] = "stmtl";
  debug_symbol_strings[elselit] = "elselit";
  debug_symbol_strings[dolit] = "dolit";
  debug_symbol_strings[dohead] = "dohead";
  debug_symbol_strings[dostmt] = "dostmt";
  debug_symbol_strings[ifhead] = "ifhead";
  debug_symbol_strings[elsehead] = "elsehead";
  debug_symbol_strings[struct_delim] = "struct_delim";
  debug_symbol_strings[attribute] = "attribute";
}

#endif

enum exit_values
parse (tk)
     enum codes tk;		/* the code for the construct scanned */
{
  int i;

#ifdef DEBUG
  if (debug)
    {
      if (tk >= code_eof && tk < number_of_codes)
	printf ("Parse: %s\n", debug_symbol_strings[tk]);
      else
	printf ("Parse: Unknown code: %d for %s\n", (int) tk, token ? token : "NULL");
    }
#endif

  while (parser_state_tos->p_stack[parser_state_tos->tos] == ifhead && tk != elselit)
    {
      /* true if we have an if without an else */

      /* apply the if(..) stmt ::= stmt reduction */
      parser_state_tos->p_stack[parser_state_tos->tos] = stmt;
      reduce ();		/* see if this allows any reduction */
    }


  switch (tk)
    {				/* go on and figure out what to do with the
				   input */

    case decl:			/* scanned a declaration word */
      parser_state_tos->search_brace = braces_on_struct_decl_line;
      /* indicate that following brace should be on same line */
      if (parser_state_tos->p_stack[parser_state_tos->tos] != decl && parser_state_tos->block_init == 0)
	{			/* only put one declaration onto stack */
	  break_comma = true;	/* while in declaration, newline should be
				   forced after comma */
	  inc_pstack ();
	  parser_state_tos->p_stack[parser_state_tos->tos] = decl;
	  parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->i_l_follow;

	  if (ljust_decl)
	    {			/* only do if we want left justified
				   declarations */
	      parser_state_tos->ind_level = 0;
	      for (i = parser_state_tos->tos - 1; i > 0; --i)
		if (parser_state_tos->p_stack[i] == decl)
		  /* indentation is number of declaration levels deep we are
		     times spaces per level */
		  parser_state_tos->ind_level += ind_size;
	      parser_state_tos->i_l_follow = parser_state_tos->ind_level;
	    }
	}
      break;

    case ifstmt:		/* scanned if (...) */
      if (parser_state_tos->p_stack[parser_state_tos->tos] == elsehead)
	parser_state_tos->i_l_follow = parser_state_tos->il[parser_state_tos->tos];
    case dolit:		/* 'do' */
    case forstmt:		/* for (...) */
    case casestmt:		/* case n: */
      inc_pstack ();
      parser_state_tos->p_stack[parser_state_tos->tos] = tk;
      parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->ind_level = parser_state_tos->i_l_follow;
      if (tk != casestmt)
	parser_state_tos->i_l_follow += ind_size;	/* subsequent statements
							   should be indented */
      parser_state_tos->search_brace = btype_2;
      break;

    case lbrace:		/* scanned { */
      break_comma = false;	/* don't break comma in an initial list */
      if (parser_state_tos->p_stack[parser_state_tos->tos] == stmt
	  || parser_state_tos->p_stack[parser_state_tos->tos] == stmtl)
	/* it is a random, isolated stmt group or a declaration */
	parser_state_tos->i_l_follow += ind_size;
      else if (parser_state_tos->p_stack[parser_state_tos->tos] == decl)
	{
	  parser_state_tos->i_l_follow += ind_size;
	  if ((parser_state_tos->last_rw == rw_struct_like || parser_state_tos->last_rw == rw_enum)
	      && (parser_state_tos->block_init != 1 || parser_state_tos->block_init_level == 0)
	      && parser_state_tos->last_token != rparen && !braces_on_struct_decl_line)
	    {
	      parser_state_tos->ind_level += struct_brace_indent;
	      parser_state_tos->i_l_follow += struct_brace_indent;
	    }
	}
      else if (parser_state_tos->p_stack[parser_state_tos->tos] == casestmt)
	{
	  parser_state_tos->ind_level += case_brace_indent - ind_size;
	  parser_state_tos->i_l_follow += case_brace_indent;
	}
      else
	{
	  /* It is a group as part of a while, for, etc. */

	  /* Only do this if there is nothing on the line */
	  if (s_code == e_code)
	    parser_state_tos->ind_level -= ind_size;

	  /* For -bl formatting, indent by brace_indent additional spaces
	     e.g. if (foo == bar) { <--> brace_indent spaces (in this
	     example, 4) */
	  if (!btype_2)
	    {
	      parser_state_tos->ind_level += brace_indent;
	      parser_state_tos->i_l_follow += brace_indent;
	    }

	  if (parser_state_tos->p_stack[parser_state_tos->tos] == swstmt)
	    parser_state_tos->i_l_follow += case_indent;
	}

      inc_pstack ();
      parser_state_tos->p_stack[parser_state_tos->tos] = lbrace;
      parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->ind_level;
      inc_pstack ();
      parser_state_tos->p_stack[parser_state_tos->tos] = stmt;
      /* allow null stmt between braces */
      parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->i_l_follow;
      break;

    case whilestmt:		/* scanned while (...) */
      if (parser_state_tos->p_stack[parser_state_tos->tos] == dohead)
	{
	  /* it is matched with do stmt */
	  parser_state_tos->ind_level = parser_state_tos->i_l_follow = parser_state_tos->il[parser_state_tos->tos];
	  inc_pstack ();
	  parser_state_tos->p_stack[parser_state_tos->tos] = whilestmt;
	  parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->ind_level = parser_state_tos->i_l_follow;
	}
      else
	{			/* it is a while loop */
	  inc_pstack ();
	  parser_state_tos->p_stack[parser_state_tos->tos] = whilestmt;
	  parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->i_l_follow;
	  parser_state_tos->i_l_follow += ind_size;
	  parser_state_tos->search_brace = btype_2;
	}

      break;

    case elselit:		/* scanned an else */

      if (parser_state_tos->p_stack[parser_state_tos->tos] != ifhead)
	{
	  ERROR ("Unmatched 'else'", 0, 0);
	}
      else
	{
	  /* indentation for else should be same as for if */
	  parser_state_tos->ind_level = parser_state_tos->il[parser_state_tos->tos];
	  /* everything following should be in 1 level */
	  parser_state_tos->i_l_follow = (parser_state_tos->ind_level + ind_size);

	  parser_state_tos->p_stack[parser_state_tos->tos] = elsehead;
	  /* remember if with else */
	  parser_state_tos->search_brace = true;
	}
      break;

    case rbrace:		/* scanned a } */
      /* stack should have <lbrace> <stmt> or <lbrace> <stmtl> */
      if (parser_state_tos->p_stack[parser_state_tos->tos - 1] == lbrace)
	{
	  parser_state_tos->ind_level = parser_state_tos->i_l_follow = parser_state_tos->il[--parser_state_tos->tos];
	  parser_state_tos->p_stack[parser_state_tos->tos] = stmt;
	}
      else
	{
	  ERROR ("Stmt nesting error.", 0, 0);
	}
      break;

    case swstmt:		/* had switch (...) */
      inc_pstack ();
      parser_state_tos->p_stack[parser_state_tos->tos] = swstmt;
#if 0
      parser_state_tos->cstk[parser_state_tos->tos] = case_ind;
#endif
      parser_state_tos->cstk[parser_state_tos->tos] = case_indent + parser_state_tos->i_l_follow;
      if (!btype_2)
	parser_state_tos->cstk[parser_state_tos->tos] += brace_indent;

      /* save current case indent level */
      parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->i_l_follow;
      /* case labels should be one level down from switch, plus
         `case_indent' if any.  Then, statements should be the `ind_size'
         further. */
#if 0
      case_ind = parser_state_tos->i_l_follow + case_indent;
      parser_state_tos->i_l_follow += case_indent + ind_size;
#endif
      parser_state_tos->i_l_follow += ind_size;
      parser_state_tos->search_brace = btype_2;
      break;

    case semicolon:		/* this indicates a simple stmt */
      break_comma = false;	/* turn off flag to break after commas in a
				   declaration */
      if (parser_state_tos->p_stack[parser_state_tos->tos] == dostmt)
	{
	  parser_state_tos->p_stack[parser_state_tos->tos] = stmt;
	}
      else
	{
	  inc_pstack ();
	  parser_state_tos->p_stack[parser_state_tos->tos] = stmt;
	  parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->ind_level;
	}
      break;

      /* This is a fatal error which cases the program to exit. */
    default:
      fatal ("Unknown code to parser", 0);
    }

  reduce ();			/* see if any reduction can be done */

#ifdef DEBUG
  if (debug)
    {
      printf ("\nParseStack [%d]:\n", (int) parser_state_tos->p_stack_size);
      for (i = 1; i <= parser_state_tos->tos; ++i)
	printf ("  stack[%d] =>   stack: %d   ind_level: %d\n", (int) i, (int) parser_state_tos->p_stack[i],
		(int) parser_state_tos->il[i]);
      printf ("\n");
    }
#endif

  return total_success;
}

/* NAME: reduce

FUNCTION: Implements the reduce part of the parsing algorithm

ALGORITHM: The following reductions are done.  Reductions are repeated until
   no more are possible.

Old TOS		     New TOS <stmt> <stmt>	     <stmtl> <stmtl> <stmt>
   <stmtl> do <stmt>		     dohead <dohead> <whilestmt>
   <dostmt> if <stmt>		     "ifstmt" switch <stmt>	     <stmt>
   decl <stmt>		     <stmt> "ifelse" <stmt>	     <stmt> for
   <stmt>		     <stmt> while <stmt>		     <stmt>
   "dostmt" while	     <stmt>

On each reduction, parser_state_tos->i_l_follow (the indentation for the
   following line) is set to the indentation level associated with the old
   TOS.

PARAMETERS: None

RETURNS: Nothing

GLOBALS: parser_state_tos->cstk parser_state_tos->i_l_follow =
   parser_state_tos->il parser_state_tos->p_stack = parser_state_tos->tos =

CALLS: None

CALLED BY: parse

HISTORY: initial coding 	November 1976	D A Willcox of CAC

*/
/*----------------------------------------------*\
|   REDUCTION PHASE				    |
\*----------------------------------------------*/

void
reduce ()
{
  int i;

  for (;;)
    {				/* keep looping until there is nothing left
				   to reduce */

      switch (parser_state_tos->p_stack[parser_state_tos->tos])
	{

	case stmt:
	  switch (parser_state_tos->p_stack[parser_state_tos->tos - 1])
	    {

	    case stmt:
	    case stmtl:
	      /* stmtl stmt or stmt stmt */
	      parser_state_tos->p_stack[--parser_state_tos->tos] = stmtl;
	      break;

	    case dolit:	/* <do> <stmt> */
	      parser_state_tos->p_stack[--parser_state_tos->tos] = dohead;
	      parser_state_tos->i_l_follow = parser_state_tos->il[parser_state_tos->tos];
	      break;

	    case ifstmt:
	      /* <if> <stmt> */
	      parser_state_tos->p_stack[--parser_state_tos->tos] = ifhead;
	      for (i = parser_state_tos->tos - 1;
		   (parser_state_tos->p_stack[i] != stmt && parser_state_tos->p_stack[i] != stmtl
		    && parser_state_tos->p_stack[i] != lbrace); --i);
	      parser_state_tos->i_l_follow = parser_state_tos->il[i];
	      /* for the time being, we will assume that there is no else on
	         this if, and set the indentation level accordingly. If an
	         else is scanned, it will be fixed up later */
	      break;

	    case swstmt:
	      /* <switch> <stmt> */
#if 0
	      case_ind = parser_state_tos->cstk[parser_state_tos->tos - 1];
#endif

	    case decl:		/* finish of a declaration */
	    case elsehead:
	      /* <<if> <stmt> else> <stmt> */
	    case forstmt:
	      /* <for> <stmt> */
	    case casestmt:
	      /* <case n:> <stmt> */
	    case whilestmt:
	      /* <while> <stmt> */
	      parser_state_tos->p_stack[--parser_state_tos->tos] = stmt;
	      parser_state_tos->i_l_follow = parser_state_tos->il[parser_state_tos->tos];
	      break;

	    default:		/* <anything else> <stmt> */
	      return;

	    }			/* end of section for <stmt> on top of stack */
	  break;

	case whilestmt:	/* while (...) on top */
	  if (parser_state_tos->p_stack[parser_state_tos->tos - 1] == dohead)
	    {
	      /* it is termination of a do while */
#if 0
	      parser_state_tos->p_stack[--parser_state_tos->tos] = stmt;
#endif
	      parser_state_tos->p_stack[--parser_state_tos->tos] = dostmt;
	      break;
	    }
	  else
	    return;

	default:		/* anything else on top */
	  return;

	}
    }
}

/* This kludge is called from main.  It is just like parse(semicolon) except
   that it does not clear break_comma.  Leaving break_comma alone is
   necessary to make sure that "int foo(), bar()" gets formatted correctly
   under -bc.  */

void
parse_lparen_in_decl ()
{
  inc_pstack ();
  parser_state_tos->p_stack[parser_state_tos->tos] = stmt;
  parser_state_tos->il[parser_state_tos->tos] = parser_state_tos->ind_level;

  reduce ();
}
