/* Copyright (c) 1994, Joseph Arceneaux.  All rights reserved

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


/* Here we have the token scanner for indent.  It scans off one token and
   puts it in the global variable "token".  It returns a code, indicating the
   type of token scanned. */

#include "sys.h"
#include "indent.h"
#include <ctype.h>

/* Stuff that needs to be shared with the rest of indent. Documented in
   indent.h.  */
char *token;
char *token_end;

#define alphanum 1
#define opchar 3

struct templ
{
  char *rwd;
  enum rwcodes rwcode;
};

/* Pointer to a vector of keywords specified by the user.  */
static struct templ *user_specials = 0;

/* Allocated size of user_specials.  */
static unsigned int user_specials_max;

/* Index in user_specials of the first unused entry.  */
static unsigned int user_specials_idx;

char chartype[128] =
{				/* this is used to facilitate the decision of
				   what type (alphanumeric, operator) each
				   character is */
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 3, 0, 0, 1, 3, 3, 0,
  0, 0, 3, 3, 0, 3, 0, 3,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 3, 3, 3, 3,
  0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 0, 0, 3, 1,
  0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 0, 3, 0, 3, 0
};

/* C code produced by gperf version 2.0 (K&R C version)
   Command-line:
   gperf -c -p -t -T -g -j1 -o -K rwd -N is_reserved indent.gperf  */

#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 42

/* 31 keywords. 39 is the maximum key range */

INLINE
static int
hash (str, len)
     register char *str;
     register unsigned int  len;
{
  static unsigned char hash_table[] =
    {
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
     42, 42, 42, 42, 42, 42, 42, 42,  6,  9,
     10,  0, 16,  5,  4, 24, 42,  0, 20,  4,
     20,  0, 42, 42,  6,  0,  0, 10, 10,  2,
     42, 42, 42, 42, 42, 42, 42, 42,
    };
  return len + hash_table[str[len - 1]] + hash_table[str[0]];
}


INLINE
struct templ*
is_reserved (str, len)
     register char *str;
     register unsigned int len;
{

  static struct templ wordlist[] =
    {
      {"",}, {"",}, {"",}, {"",}, 
      {"else",  rw_sp_nparen,},
      {"short",  rw_decl,},
      {"struct",  rw_struct_like,},
      {"while",  rw_sp_paren,},
      {"enum",  rw_struct_like,},
      {"goto",  rw_break,},
      {"switch",  rw_switch,},
      {"break",  rw_break,},
      {"do",  rw_sp_nparen,},
      {"case",  rw_case,},
      {"const",  rw_decl,},
      {"static",  rw_decl,},
      {"double",  rw_decl,},
      {"default",  rw_case,},
      {"volatile",  rw_decl,},
      {"char",  rw_decl,},
      {"register",  rw_decl,},
      {"float",  rw_decl,},
      {"sizeof",  rw_sizeof,},
      {"typedef",  rw_decl,},
      {"void",  rw_decl,},
      {"for",  rw_sp_paren,},
      {"extern",  rw_decl,},
      {"int",  rw_decl,},
      {"unsigned",  rw_decl,},
      {"long",  rw_decl,},
      {"",}, 
      {"global",  rw_decl,},
      {"return",  rw_return,},
      {"",}, {"",}, 
      {"union",  rw_struct_like,},
      {"va_dcl",  rw_decl,},
      {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"if",  rw_sp_paren,},
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= MIN_HASH_VALUE)
        {
          register char *s = wordlist[key].rwd;

          if (*s == *str && !strncmp (str + 1, s + 1, len - 1))
            return &wordlist[key];
        }
    }
  return 0;
}

extern int squest;

enum codes
lexi ()
{
  int unary_delim;		/* this is set to 1 if the current token
				   forces a following operator to be unary */
  static enum codes last_code;	/* the last token type returned */
  static int l_struct;		/* set to 1 if the last token was 'struct' */
  enum codes code;		/* internal code to be returned */
  char qchar;			/* the delimiter character for a string */

  unary_delim = false;
  /* tell world that this token started in column 1 iff the last
     thing scanned was nl */
  parser_state_tos->col_1 = parser_state_tos->last_nl;
  parser_state_tos->last_nl = false;

  while (*buf_ptr == ' ' || *buf_ptr == TAB)
    {				/* get rid of blanks */
      parser_state_tos->col_1 = false;	/* leading blanks imply token is not
					   in column 1 */
      if (++buf_ptr >= buf_end)
	fill_buffer ();
    }

  token = buf_ptr;

  /* Scan an alphanumeric token */
  if (chartype[*buf_ptr] == alphanum
      || (buf_ptr[0] == '.' && isdigit (buf_ptr[1])))
    {
      /* we have a character or number */
      register struct templ *p;

      if (isdigit (*buf_ptr) || (buf_ptr[0] == '.' && isdigit (buf_ptr[1])))
	{
	  int seendot = 0, seenexp = 0;
	  if (*buf_ptr == '0' &&
	      (buf_ptr[1] == 'x' || buf_ptr[1] == 'X'))
	    {
	      buf_ptr += 2;
	      while (isxdigit (*buf_ptr))
		buf_ptr++;
	    }
	  else
	    while (1)
	      {
		if (*buf_ptr == '.')
		  if (seendot)
		    break;
		  else
		    seendot++;
		buf_ptr++;
		if (!isdigit (*buf_ptr) && *buf_ptr != '.')
		  if ((*buf_ptr != 'E' && *buf_ptr != 'e') || seenexp)
		    break;
		  else
		    {
		      seenexp++;
		      seendot++;
		      buf_ptr++;
		      if (*buf_ptr == '+' || *buf_ptr == '-')
			buf_ptr++;
		    }
	      }
	  /* Accept unsigned, unsigned long, and float constants
	     (U, UL, and F suffixes).  I'm not sure if LU is ansii.
	     11Jun93 - rdh@key.amdahl.com tells me LL and ULL are also
	     acceptable. */
	  if (*buf_ptr == 'F' || *buf_ptr == 'f')
	    buf_ptr++;
	  else
	    {
	      if (*buf_ptr == 'U' || *buf_ptr == 'u')
		buf_ptr++;
	      if (*buf_ptr == 'L' || *buf_ptr == 'l')
		buf_ptr++;
	      if (*buf_ptr == 'L' || *buf_ptr == 'l')
		buf_ptr++;
	    }
	}
      else
	while (chartype[*buf_ptr] == alphanum)
	  {			/* copy it over */
	    buf_ptr++;
	    if (buf_ptr >= buf_end)
	      fill_buffer ();
	  }
      token_end = buf_ptr;
      while (*buf_ptr == ' ' || *buf_ptr == TAB)
	{
	  if (++buf_ptr >= buf_end)
	    fill_buffer ();
	}
      parser_state_tos->its_a_keyword = false;
      parser_state_tos->sizeof_keyword = false;

      /* if last token was 'struct', then this token should be treated
	 as a declaration */
      if (l_struct)
	{
	  l_struct = false;
	  last_code = ident;
	  parser_state_tos->last_u_d = true;
	  return (decl);
	}

      /* Operator after indentifier is binary */
      parser_state_tos->last_u_d = false;
      last_code = ident;

      /* Check whether the token is a reserved word.  Use perfect hashing... */
      p = is_reserved (token, token_end - token);

      if (!p && user_specials != 0)
	{
	  for (p = &user_specials[0];
	       p < &user_specials[0] + user_specials_idx;
	       p++)
	    {
	      char *q = token;
	      char *r = p->rwd;

	      /* This string compare is a little nonstandard because token
	         ends at the character before token_end and p->rwd is
	         null-terminated.  */
	      while (1)
		{
		  /* If we have come to the end of both the keyword in
		     user_specials and the keyword in token they are equal.  */
		  if (q >= token_end && !*r)
		    goto found_keyword;

		  /* If we have come to the end of just one, they are not
		     equal.  */
		  if (q >= token_end || !*r)
		    break;

		  /* If the characters in corresponding characters are not
		     equal, the strings are not equal.  */
		  if (*q++ != *r++)
		    break;
		}
	    }
	  /* Didn't find anything in user_specials.  */
	  p = 0;
	}

      if (p)
	{			/* we have a keyword */
	found_keyword:
	  parser_state_tos->its_a_keyword = true;
	  parser_state_tos->last_u_d = true;
	  parser_state_tos->last_rw = p->rwcode;
	  switch (p->rwcode)
	    {
	    case rw_switch:	/* it is a switch */
	      return (swstmt);
	    case rw_case:	/* a case or default */
	      return (casestmt);

	    case rw_struct_like:	/* a "struct" */
	      if (parser_state_tos->p_l_follow
		  && !(parser_state_tos->noncast_mask
		       & 1 << parser_state_tos->p_l_follow))
		/* inside parens: cast */
		{
		  parser_state_tos->cast_mask
		    |= 1 << parser_state_tos->p_l_follow;
		  break;
		}
	      l_struct = true;

	      /* Next time around, we will want to know that we have had a
	         'struct' */
	    case rw_decl:	/* one of the declaration keywords */
	      if (parser_state_tos->p_l_follow
		  && !(parser_state_tos->noncast_mask
		       & 1 << parser_state_tos->p_l_follow))
		/* inside parens: cast */
		{
		  parser_state_tos->cast_mask
		    |= 1 << parser_state_tos->p_l_follow;
		  break;
		}
	      last_code = decl;
	      return (decl);

	    case rw_sp_paren:	/* if, while, for */
	      return (sp_paren);

	    case rw_sp_nparen:	/* do, else */
	      return (sp_nparen);

	    case rw_sizeof:
	      parser_state_tos->sizeof_keyword = true;
	      return (ident);

	    case rw_return:
	    case rw_break:
	    default:		/* all others are treated like any other
				   identifier */
	      return (ident);
	    }			/* end of switch */
	}			/* end of if (found_it) */

      if (*buf_ptr == '('
	  && parser_state_tos->tos <= 1
	  && parser_state_tos->ind_level == 0
	  && parser_state_tos->paren_depth == 0)
	{
	  /* We have found something which might be the name in a function
	     definition.  */
	  register char *tp;
	  int paren_count = 1;

	  /* Skip to the matching ')'.  */
	  for (tp = buf_ptr + 1;
	       paren_count > 0 && tp < in_prog + in_prog_size;
	       tp++)
	    {
	      if (*tp == '(')
		paren_count++;
	      if (*tp == ')')
		paren_count--;
	      /* Can't occur in parameter list; this way we don't search the
	         whole file in the case of unbalanced parens.  */
	      if (*tp == ';')
		goto not_proc;
	    }

	  if (paren_count == 0)
	    {
	      while (isspace (*tp))
		tp++;
	      /* If the next char is ';' or ',' or '(' we have a function
	         declaration, not a definition.

		 I've added '=' to this list to keep from breaking
		 a non-valid C macro from libc.  -jla */
	      if (*tp != ';' && *tp != ',' && *tp != '(' && *tp != '=')
		{
		  parser_state_tos->procname = token;
		  parser_state_tos->procname_end = token_end;
		  parser_state_tos->in_parameter_declaration = 1;
		}
	    }

	not_proc:;
	}

      /* The following hack attempts to guess whether or not the
	 current token is in fact a declaration keyword -- one that
	 has been typedef'd */
      if (((*buf_ptr == '*' && buf_ptr[1] != '=')
	   || isalpha (*buf_ptr) || *buf_ptr == '_')
	  && !parser_state_tos->p_l_follow
	  && !parser_state_tos->block_init
	  && (parser_state_tos->last_token == rparen
	      || parser_state_tos->last_token == semicolon
	      || parser_state_tos->last_token == decl
	      || parser_state_tos->last_token == lbrace
	      || parser_state_tos->last_token == rbrace))
	{
	  parser_state_tos->its_a_keyword = true;
	  parser_state_tos->last_u_d = true;
	  last_code = decl;
	  return decl;
	}

      if (last_code == decl)	/* if this is a declared variable, then
				   following sign is unary */
	parser_state_tos->last_u_d = true;	/* will make "int a -1" work */
      last_code = ident;
      return (ident);		/* the ident is not in the list */
    }				/* end of procesing for alpanum character */
  /* Scan a non-alphanumeric token */

  /* If it is not a one character token, token_end will get changed later.  */
  token_end = buf_ptr + 1;

  if (++buf_ptr >= buf_end)
    fill_buffer ();

  switch (*token)
    {
    case '\0':
      code = code_eof;
      break;

    case EOL:
      unary_delim = parser_state_tos->last_u_d;
      parser_state_tos->last_nl = true;
      code = newline;
      break;

    case '\'':			/* start of quoted character */
    case '"':			/* start of string */
      qchar = *token;

      /* Find out how big the literal is so we can set token_end.  */

      /* Invariant:  before loop test buf_ptr points to the next */
      /* character that we have not yet checked. */
      while (*buf_ptr != qchar && *buf_ptr != 0 && *buf_ptr != EOL)
	{
	  if (*buf_ptr == '\\')
	    {
	      buf_ptr++;
	      if (buf_ptr >= buf_end)
		fill_buffer ();
	      if (*buf_ptr == EOL)
		++line_no;
	      if (*buf_ptr == 0)
		break;
	    }
	  buf_ptr++;
	  if (buf_ptr >= buf_end)
	    fill_buffer ();
	}
      if (*buf_ptr == EOL || *buf_ptr == 0)
	{
	  diag (1, (qchar == '\''
		    ? "Unterminated character constant"
		    : "Unterminated string constant"),
		0, 0);
	}
      else
	{
	  /* Advance over end quote char.  */
	  buf_ptr++;
	  if (buf_ptr >= buf_end)
	    fill_buffer ();
	}

      code = ident;
      break;

    case ('('):
    case ('['):
      unary_delim = true;
      code = lparen;
      break;

    case (')'):
    case (']'):
      code = rparen;
      break;

    case '#':
      unary_delim = parser_state_tos->last_u_d;
      code = preesc;
      break;

    case '?':
      unary_delim = true;
      code = question;
      break;

    case (':'):
      code = colon;
      unary_delim = true;
      if (squest && *e_com != ' ')
	{
	  if (e_code == s_code)
	    parser_state_tos->want_blank = false;
	  else
	    parser_state_tos->want_blank = true;
	}
      break;

    case (';'):
      unary_delim = true;
      code = semicolon;
      break;

    case ('{'):
      unary_delim = true;

      /* This check is made in the code for '='.  No one who writes
         initializers without '=' these days deserves to have indent work on
         their code (besides which, uncommenting this would screw up anything
         which assumes that parser_state_tos->block_init really means you are
         in an initializer.  */
      /* if (parser_state_tos->in_or_st) parser_state_tos->block_init = 1; */

      /* The following neat hack causes the braces in structure
         initializations to be treated as parentheses, thus causing
         initializations to line up correctly, e.g. struct foo bar = {{a, b,
         c}, {1, 2}}; If lparen is returned, token can be used to distinguish
         between '{' and '(' where necessary.  */

      code = parser_state_tos->block_init ? lparen : lbrace;
      break;

    case ('}'):
      unary_delim = true;
      /* The following neat hack is explained under '{' above.  */
      code = parser_state_tos->block_init ? rparen : rbrace;

      break;

    case 014:			/* a form feed */
      unary_delim = parser_state_tos->last_u_d;
      parser_state_tos->last_nl = true;	/* remember this so we can set
					   'parser_state_tos->col_1' right */
      code = form_feed;
      break;

    case (','):
      unary_delim = true;
      code = comma;
      break;

    case '.':
      unary_delim = false;
      code = period;
      break;

    case '-':
    case '+':			/* check for -, +, --, ++ */
      code = (parser_state_tos->last_u_d ? unary_op : binary_op);
      unary_delim = true;

      if (*buf_ptr == token[0])
	{
	  /* check for doubled character */
	  buf_ptr++;
	  /* buffer overflow will be checked at end of loop */
	  if (last_code == ident || last_code == rparen)
	    {
	      code = (parser_state_tos->last_u_d ? unary_op : postop);
	      /* check for following ++ or -- */
	      unary_delim = false;
	    }
	}
      else if (*buf_ptr == '=')
	/* check for operator += */
	buf_ptr++;
      else if (*buf_ptr == '>')
	{
	  /* check for operator -> */
	  buf_ptr++;
	  if (!pointer_as_binop)
	    {
	      unary_delim = false;
	      code = unary_op;
	      parser_state_tos->want_blank = false;
	    }
	}
      break;			/* buffer overflow will be checked at end of
				   switch */

    case '=':
      if (parser_state_tos->in_or_st)
	parser_state_tos->block_init = 1;

      if (*buf_ptr == '=')	/* == */
	buf_ptr++;
      else if (*buf_ptr == '-'
	       || *buf_ptr == '+'
	       || *buf_ptr == '*'
	       || *buf_ptr == '&')
	{
	  /* Something like x=-1, which can mean x -= 1 ("old style" in K&R1)
	     or x = -1 (ANSI).  Note that this is only an ambiguity if the
	     character can also be a unary operator.  If not, just produce
	     output code that produces a syntax error (the theory being that
	     people want to detect and eliminate old style assignments but
	     they don't want indent to silently change the meaning of their
	     code).  */
	  diag (0,
	  "old style assignment ambiguity in \"=%c\".  Assuming \"= %c\"\n",
		(int) *buf_ptr, (int) *buf_ptr);
	}

      code = binary_op;
      unary_delim = true;
      break;
      /* can drop thru!!! */

    case '>':
    case '<':
    case '!':
      /* ops like <, <<, <=, !=, <<=, etc */
      /* This will of course scan sequences like "<=>", "!=>", "<<>", etc. as
         one token, but I don't think that will cause any harm.  */
      while (*buf_ptr == '>' || *buf_ptr == '<' || *buf_ptr == '=')
	{
	  if (++buf_ptr >= buf_end)
	    fill_buffer ();
	  if (*buf_ptr == '=')
	    {
	      if (++buf_ptr >= buf_end)
		fill_buffer ();
	    }
	}

      code = (parser_state_tos->last_u_d ? unary_op : binary_op);
      unary_delim = true;
      break;

    default:
      if (token[0] == '/' && (*buf_ptr == '*' || *buf_ptr == '/'))
	{
	  /* A C or C++ comment */

	  if (*buf_ptr == '*')
	    code = comment;
	  else
	    code = cplus_comment;

	  if (++buf_ptr >= buf_end)
	    fill_buffer ();

	  unary_delim = parser_state_tos->last_u_d;
	  break;
	}

      while (*(buf_ptr - 1) == *buf_ptr || *buf_ptr == '=')
	{
	  /* handle ||, &&, etc, and also things as in int *****i */
	  if (++buf_ptr >= buf_end)
	    fill_buffer ();
	}
      code = (parser_state_tos->last_u_d ? unary_op : binary_op);
      unary_delim = true;


    }				/* end of switch */

  if (code != newline)
    {
      l_struct = false;
      last_code = code;
    }
  token_end = buf_ptr;
  if (buf_ptr >= buf_end)	/* check for input buffer empty */
    fill_buffer ();
  parser_state_tos->last_u_d = unary_delim;

  return (code);
}

/* Add the given keyword to the keyword table, using val as the keyword type */
addkey (key, val)
     char *key;
     enum rwcodes val;
{
  register struct templ *p;

  /* Check to see whether key is a reserved word or not. */
  if (is_reserved (key, strlen (key)) != 0)
    return;

  if (user_specials == 0)
    {
      user_specials = (struct templ *) xmalloc (5 * sizeof (struct templ));
      user_specials_max = 5;
      user_specials_idx = 0;
    }
  else if (user_specials_idx == user_specials_max)
    {
      user_specials_max += 5;
      user_specials = (struct templ *) xrealloc ((char *) user_specials,
						 user_specials_max
						 * sizeof (struct templ));
    }

  p = &user_specials[user_specials_idx++];
  p->rwd = key;
  p->rwcode = val;
  return;
}
