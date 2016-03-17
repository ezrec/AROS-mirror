/* A Bison parser, made from complex.y
   by GNU bison 1.35.  */

#define YYBISON 1  /* Identify Bison output.  */

# define	NUMBER	257
# define	VAR	258
# define	_CONST	259
# define	C_BLTIN	260
# define	R_BLTIN	261
# define	FUNCDEF	262
# define	UFUNC	263
# define	COMMAND	264
# define	UNDEF	265
# define	PARAMETER	266
# define	PRECISION	267
# define	REPEAT	268
# define	UMINUS	269

#line 12 "complex.y"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "complex.h"
#include "memory.h"

extern void cprin(FILE *, char *prefix, char *suffix, Complex z);
extern int precision;

RemKey *remkey;

const Complex	zero = {0.0, 0.0},
		eye = {0.0, 1.0};

Symbol	*ans;		/* set by init.c */
Complex	treeval;	/* what tree evaluates to */
UserFunc *userfunc;	/* symbol for user-function definition */
int	inparamlist;	/* are we at a functions parameter-list? */
int	indefn;		/* are we in function definition? */
int	silent;		/* should we display results? */
int	repeatcnt;	/* number of times to repeat expression */
char	*infile;	/* file currently being read */

#line 37 "complex.y"
#ifndef YYSTYPE
typedef union {
	double	rval;
	Symbol	*sym;
	Node	*node;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
#ifndef YYDEBUG
# define YYDEBUG 0
#endif



#define	YYFINAL		69
#define	YYFLAG		-32768
#define	YYNTBASE	28

/* YYTRANSLATE(YYLEX) -- Bison token number corresponding to YYLEX. */
#define YYTRANSLATE(x) ((unsigned)(x) <= 269 ? yytranslate[x] : 36)

/* YYTRANSLATE[YYLEX] -- Bison token number corresponding to YYLEX. */
static const char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      25,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    22,
      23,    24,    18,    16,     2,    17,     2,    19,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    26,
       2,    15,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    21,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    27,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    20
};

#if YYDEBUG
static const short yyprhs[] =
{
       0,     0,     1,     4,     9,    10,    11,    23,    27,    32,
      36,    38,    40,    42,    44,    46,    47,    50,    52,    55,
      57,    59,    63,    65,    67,    71,    76,    81,    86,    90,
      94,    98,   102,   106,   109,   113
};
static const short yyrhs[] =
{
      -1,    28,    33,     0,    28,    13,     3,    33,     0,     0,
       0,    28,     8,    32,    29,    23,    31,    24,    30,    15,
      35,    33,     0,    28,    10,    33,     0,    28,    34,    35,
      33,     0,    28,     1,    25,     0,    12,     0,     4,     0,
       9,     0,    25,     0,    26,     0,     0,    14,     3,     0,
       3,     0,     3,    27,     0,    27,     0,     5,     0,     5,
      15,    35,     0,     4,     0,    12,     0,     4,    15,    35,
       0,     6,    23,    35,    24,     0,     7,    23,    35,    24,
       0,     9,    23,    35,    24,     0,    35,    16,    35,     0,
      35,    17,    35,     0,    35,    18,    35,     0,    35,    19,
      35,     0,    35,    21,    35,     0,    35,    22,     0,    23,
      35,    24,     0,    17,    35,     0
};

#endif

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined. */
static const short yyrline[] =
{
       0,    59,    60,    61,    62,    62,    62,    94,    95,   110,
     122,   126,   127,   130,   131,   134,   135,   138,   139,   140,
     141,   142,   143,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159
};
#endif


#if (YYDEBUG) || defined YYERROR_VERBOSE

/* YYTNAME[TOKEN_NUM] -- String name of the token TOKEN_NUM. */
static const char *const yytname[] =
{
  "$", "error", "$undefined.", "NUMBER", "VAR", "CONST", "C_BLTIN", 
  "R_BLTIN", "FUNCDEF", "UFUNC", "COMMAND", "UNDEF", "PARAMETER", 
  "PRECISION", "REPEAT", "'='", "'+'", "'-'", "'*'", "'/'", "UMINUS", 
  "'^'", "'\\''", "'('", "')'", "'\\n'", "';'", "'i'", "list", "@1", "@2", 
  "parlist", "symbol", "separator", "opt_repeat", "expr", 0
};
#endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives. */
static const short yyr1[] =
{
       0,    28,    28,    28,    29,    30,    28,    28,    28,    28,
      31,    32,    32,    33,    33,    34,    34,    35,    35,    35,
      35,    35,    35,    35,    35,    35,    35,    35,    35,    35,
      35,    35,    35,    35,    35,    35
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN. */
static const short yyr2[] =
{
       0,     0,     2,     4,     0,     0,    11,     3,     4,     3,
       1,     1,     1,     1,     1,     0,     2,     1,     2,     1,
       1,     3,     1,     1,     3,     4,     4,     4,     3,     3,
       3,     3,     3,     2,     3,     2
};

/* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
   doesn't specify something else to do.  Zero means the default is an
   error. */
static const short yydefact[] =
{
       1,     0,     0,     0,     0,     0,     0,    13,    14,     2,
       0,     9,    11,    12,     4,     7,     0,    16,    17,    22,
      20,     0,     0,     0,    23,     0,     0,    19,     0,     0,
       3,    18,     0,     0,     0,     0,     0,    35,     0,     0,
       0,     0,     0,     0,    33,     8,     0,    24,    21,     0,
       0,     0,    34,    28,    29,    30,    31,    32,    10,     0,
      25,    26,    27,     5,     0,     0,     0,     6,     0,     0
};

static const short yydefgoto[] =
{
       1,    29,    64,    59,    14,     9,    10,    28
};

static const short yypact[] =
{
  -32768,    38,   -20,     4,   -11,     6,    17,-32768,-32768,-32768,
      63,-32768,-32768,-32768,-32768,-32768,   -11,-32768,    -6,     7,
       8,    14,    26,    36,-32768,    63,    63,-32768,   -15,    37,
  -32768,-32768,    63,    63,    63,    63,    63,    -5,    60,    63,
      63,    63,    63,    63,-32768,-32768,    18,   102,   102,    75,
      84,    93,-32768,    35,    35,    -5,    -5,    -5,-32768,    16,
  -32768,-32768,-32768,-32768,    56,    63,   -15,-32768,    31,-32768
};

static const short yypgoto[] =
{
  -32768,-32768,-32768,-32768,-32768,    -4,-32768,    -7
};


#define	YYLAST		124


static const short yytable[] =
{
      15,    39,    40,    41,    42,    11,    43,    44,    12,    16,
       7,     8,    30,    13,     7,     8,    43,    44,    37,    38,
      17,    31,    32,    33,    45,    47,    48,    49,    50,    51,
      58,    69,    53,    54,    55,    56,    57,    34,    68,     2,
      63,   -15,   -15,   -15,   -15,   -15,     3,   -15,     4,    35,
     -15,     5,     6,    41,    42,   -15,    43,    44,    66,    36,
      46,   -15,    67,     7,     8,   -15,    18,    19,    20,    21,
      22,    65,    23,     0,     0,    24,    39,    40,    41,    42,
      25,    43,    44,     0,    52,     0,    26,     0,     0,     0,
      27,    39,    40,    41,    42,     0,    43,    44,     0,    60,
      39,    40,    41,    42,     0,    43,    44,     0,    61,    39,
      40,    41,    42,     0,    43,    44,     0,    62,    39,    40,
      41,    42,     0,    43,    44
};

static const short yycheck[] =
{
       4,    16,    17,    18,    19,    25,    21,    22,     4,     3,
      25,    26,    16,     9,    25,    26,    21,    22,    25,    26,
       3,    27,    15,    15,    28,    32,    33,    34,    35,    36,
      12,     0,    39,    40,    41,    42,    43,    23,     0,     1,
      24,     3,     4,     5,     6,     7,     8,     9,    10,    23,
      12,    13,    14,    18,    19,    17,    21,    22,    65,    23,
      23,    23,    66,    25,    26,    27,     3,     4,     5,     6,
       7,    15,     9,    -1,    -1,    12,    16,    17,    18,    19,
      17,    21,    22,    -1,    24,    -1,    23,    -1,    -1,    -1,
      27,    16,    17,    18,    19,    -1,    21,    22,    -1,    24,
      16,    17,    18,    19,    -1,    21,    22,    -1,    24,    16,
      17,    18,    19,    -1,    21,    22,    -1,    24,    16,    17,
      18,    19,    -1,    21,    22
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison/bison.simple"

/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser when
   the %semantic_parser declaration is not specified in the grammar.
   It was written by Richard Stallman by simplifying the hairy parser
   used when %semantic_parser is specified.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

#if ! defined (yyoverflow) || defined (YYERROR_VERBOSE)

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || defined (YYERROR_VERBOSE) */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYLTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
# if YYLSP_NEEDED
  YYLTYPE yyls;
# endif
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAX (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# if YYLSP_NEEDED
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE) + sizeof (YYLTYPE))	\
      + 2 * YYSTACK_GAP_MAX)
# else
#  define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAX)
# endif

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAX;	\
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif


#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");			\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).

   When YYLLOC_DEFAULT is run, CURRENT is set the location of the
   first token.  By default, to implement support for ranges, extend
   its range to the last symbol.  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)       	\
   Current.last_line   = Rhs[N].last_line;	\
   Current.last_column = Rhs[N].last_column;
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#if YYPURE
# if YYLSP_NEEDED
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, &yylloc, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval, &yylloc)
#  endif
# else /* !YYLSP_NEEDED */
#  ifdef YYLEX_PARAM
#   define YYLEX		yylex (&yylval, YYLEX_PARAM)
#  else
#   define YYLEX		yylex (&yylval)
#  endif
# endif /* !YYLSP_NEEDED */
#else /* !YYPURE */
# define YYLEX			yylex ()
#endif /* !YYPURE */


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)
/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
#endif /* !YYDEBUG */

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

#ifdef YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif
#endif

#line 315 "/usr/share/bison/bison.simple"


/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
#  define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL
# else
#  define YYPARSE_PARAM_ARG YYPARSE_PARAM
#  define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
# endif
#else /* !YYPARSE_PARAM */
# define YYPARSE_PARAM_ARG
# define YYPARSE_PARAM_DECL
#endif /* !YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
# ifdef YYPARSE_PARAM
int yyparse (void *);
# else
int yyparse (void);
# endif
#endif

/* YY_DECL_VARIABLES -- depending whether we use a pure parser,
   variables are global, or local to YYPARSE.  */

#define YY_DECL_NON_LSP_VARIABLES			\
/* The lookahead symbol.  */				\
int yychar;						\
							\
/* The semantic value of the lookahead symbol. */	\
YYSTYPE yylval;						\
							\
/* Number of parse errors so far.  */			\
int yynerrs;

#if YYLSP_NEEDED
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES			\
						\
/* Location data for the lookahead symbol.  */	\
YYLTYPE yylloc;
#else
# define YY_DECL_VARIABLES			\
YY_DECL_NON_LSP_VARIABLES
#endif


/* If nonreentrant, generate the variables here. */

#if !YYPURE
YY_DECL_VARIABLES
#endif  /* !YYPURE */

int
yyparse (YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  /* If reentrant, generate the variables here. */
#if YYPURE
  YY_DECL_VARIABLES
#endif  /* !YYPURE */

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yychar1 = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack. */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;

#if YYLSP_NEEDED
  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
#endif

#if YYLSP_NEEDED
# define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
# define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  YYSIZE_T yystacksize = YYINITDEPTH;


  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
#if YYLSP_NEEDED
  YYLTYPE yyloc;
#endif

  /* When reducing, the number of symbols on the RHS of the reduced
     rule. */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
#if YYLSP_NEEDED
  yylsp = yyls;
#endif
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  */
# if YYLSP_NEEDED
	YYLTYPE *yyls1 = yyls;
	/* This used to be a conditional around just the two extra args,
	   but that might be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
# else
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);
# endif
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
# if YYLSP_NEEDED
	YYSTACK_RELOCATE (yyls);
# endif
# undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
#if YYLSP_NEEDED
      yylsp = yyls + yysize - 1;
#endif

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yychar1 = YYTRANSLATE (yychar);

#if YYDEBUG
     /* We have to keep this `#if YYDEBUG', since we use variables
	which are defined only if `YYDEBUG' is set.  */
      if (yydebug)
	{
	  YYFPRINTF (stderr, "Next token is %d (%s",
		     yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise
	     meaning of a token, for further debugging info.  */
# ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
# endif
	  YYFPRINTF (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %d (%s), ",
	      yychar, yytname[yychar1]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to the semantic value of
     the lookahead token.  This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

#if YYLSP_NEEDED
  /* Similarly for the default location.  Let the user run additional
     commands if for instance locations are ranges.  */
  yyloc = yylsp[1-yylen];
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
#endif

#if YYDEBUG
  /* We have to keep this `#if YYDEBUG', since we use variables which
     are defined only if `YYDEBUG' is set.  */
  if (yydebug)
    {
      int yyi;

      YYFPRINTF (stderr, "Reducing via rule %d (line %d), ",
		 yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (yyi = yyprhs[yyn]; yyrhs[yyi] > 0; yyi++)
	YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
      YYFPRINTF (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif

  switch (yyn) {

case 3:
#line 61 "complex.y"
{ precision = (int)(yyvsp[-1].rval); ;
    break;}
case 4:
#line 63 "complex.y"
{
			if (yyvsp[0].sym->type == UFUNC)
			{
				clear_ufunc(&yyvsp[0].sym->u.ufunc);
			}
			else if (yyvsp[0].sym->type != UNDEF)
			{
				execerror(yyvsp[0].sym->name, "already defined");
			}
			inparamlist = 1;
			yyvsp[0].sym->type = UFUNC;
			userfunc = &yyvsp[0].sym->u.ufunc;
			rem_freeall();
			userfunc->remkey = NULL;
			(void)rem_setkey(&userfunc->remkey);
		;
    break;}
case 5:
#line 80 "complex.y"
{
			inparamlist = 0;
			indefn = 1;
			userfunc->param = yyvsp[-1].sym;
		;
    break;}
case 6:
#line 86 "complex.y"
{
			userfunc->tree = yyvsp[-1].node;
			(void)rem_setkey(&remkey);
			indefn = 0;
			if (!silent)
				fprintf(stdout, "\t%s(%s) defined\n", yyvsp[-8].sym->name, userfunc->param->name);
		;
    break;}
case 7:
#line 94 "complex.y"
{ (*(yyvsp[-1].sym->u.vptr))(); ;
    break;}
case 8:
#line 96 "complex.y"
{
			while (repeatcnt)
			{
				--repeatcnt;
				treeval = eval_tree(yyvsp[-1].node);

				if (!silent)
					cprin(stdout, "\t", "\n", treeval);
				if (ans)		/* allocated successfully */
					ans->u.val = treeval;/* set 'last answer' const */
			}
			/* free all mem associated with this tree */
			rem_freeall();
		;
    break;}
case 9:
#line 111 "complex.y"
{
			if (indefn)
			{
				indefn = 0;
				rem_setkey(&remkey);
			}			
			inparamlist = 0;
			yyerrok;
		;
    break;}
case 15:
#line 134 "complex.y"
{ repeatcnt = 1; ;
    break;}
case 16:
#line 135 "complex.y"
{ repeatcnt = (int)(yyvsp[0].rval); ;
    break;}
case 17:
#line 138 "complex.y"
{ yyval.node = n_number(yyvsp[0].rval, 0.0); ;
    break;}
case 18:
#line 139 "complex.y"
{ yyval.node = n_number(0.0, yyvsp[-1].rval); ;
    break;}
case 19:
#line 140 "complex.y"
{ yyval.node = n_number(0.0, 1.0); ;
    break;}
case 20:
#line 141 "complex.y"
{ yyval.node = n_symbol(_CONST, yyvsp[0].sym); ;
    break;}
case 21:
#line 142 "complex.y"
{ execerror("invalid assignment to constant", yyvsp[-2].sym->name); ;
    break;}
case 22:
#line 143 "complex.y"
{ if (yyvsp[0].sym->type == UNDEF)
					warning("using zero for undefined symbol", yyvsp[0].sym->name);
				  yyval.node = n_symbol(VAR, yyvsp[0].sym);
				;
    break;}
case 23:
#line 147 "complex.y"
{ yyval.node = n_symbol(PARAMETER, yyvsp[0].sym); ;
    break;}
case 24:
#line 148 "complex.y"
{ yyvsp[-2].sym->type = VAR; yyval.node = n_asgn(yyvsp[-2].sym, yyvsp[0].node); ;
    break;}
case 25:
#line 149 "complex.y"
{ yyval.node = n_func(C_BLTIN, yyvsp[-3].sym, yyvsp[-1].node); ;
    break;}
case 26:
#line 150 "complex.y"
{ yyval.node = n_func(R_BLTIN, yyvsp[-3].sym, yyvsp[-1].node); ;
    break;}
case 27:
#line 151 "complex.y"
{ yyval.node = n_func(UFUNC, yyvsp[-3].sym, yyvsp[-1].node); ;
    break;}
case 28:
#line 152 "complex.y"
{ yyval.node = n_binop('+', yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 29:
#line 153 "complex.y"
{ yyval.node = n_binop('-', yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 30:
#line 154 "complex.y"
{ yyval.node = n_binop('*', yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 31:
#line 155 "complex.y"
{ yyval.node = n_binop('/', yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 32:
#line 156 "complex.y"
{ yyval.node = n_binop('^', yyvsp[-2].node, yyvsp[0].node); ;
    break;}
case 33:
#line 157 "complex.y"
{ yyval.node = n_unop('\'', yyvsp[-1].node); ;
    break;}
case 34:
#line 158 "complex.y"
{ yyval.node = n_unop('(', yyvsp[-1].node); ;
    break;}
case 35:
#line 159 "complex.y"
{ yyval.node = n_unop(UMINUS , yyvsp[0].node); ;
    break;}
}

#line 705 "/usr/share/bison/bison.simple"


  yyvsp -= yylen;
  yyssp -= yylen;
#if YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;
#if YYLSP_NEEDED
  *++yylsp = yyloc;
#endif

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("parse error, unexpected ") + 1;
	  yysize += yystrlen (yytname[YYTRANSLATE (yychar)]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "parse error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[YYTRANSLATE (yychar)]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exhausted");
	}
      else
#endif /* defined (YYERROR_VERBOSE) */
	yyerror ("parse error");
    }
  goto yyerrlab1;


/*--------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action |
`--------------------------------------------------*/
yyerrlab1:
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;
      YYDPRINTF ((stderr, "Discarding token %d (%s).\n",
		  yychar, yytname[yychar1]));
      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;


/*-------------------------------------------------------------------.
| yyerrdefault -- current state does not do anything special for the |
| error token.                                                       |
`-------------------------------------------------------------------*/
yyerrdefault:
#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */

  /* If its default is to accept any token, ok.  Otherwise pop it.  */
  yyn = yydefact[yystate];
  if (yyn)
    goto yydefault;
#endif


/*---------------------------------------------------------------.
| yyerrpop -- pop the current state because it cannot handle the |
| error token                                                    |
`---------------------------------------------------------------*/
yyerrpop:
  if (yyssp == yyss)
    YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#if YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG
  if (yydebug)
    {
      short *yyssp1 = yyss - 1;
      YYFPRINTF (stderr, "Error: state stack now");
      while (yyssp1 != yyssp)
	YYFPRINTF (stderr, " %d", *++yyssp1);
      YYFPRINTF (stderr, "\n");
    }
#endif

/*--------------.
| yyerrhandle.  |
`--------------*/
yyerrhandle:
  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;
#if YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

/*---------------------------------------------.
| yyoverflowab -- parser overflow comes here.  |
`---------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}
#line 162 "complex.y"


#include <signal.h>
#include <setjmp.h>

#ifdef AMIGA
#include <libraries/dos.h>
#include <workbench/startup.h>
#include <proto/dos.h>
extern struct WBStartup *WBenchMsg;
int workbench;
struct WBArg *wbargv;
char *nextarg(void);
#endif

#define	BANNER	"\033[33micalc expression parser v1.0, by mws\033[31m\n"

jmp_buf	begin;			/* error start */
int	lineno;			/* current line-number of input file */
FILE	*fin;			/* current input file */
char	**gargv;		/* global argument array */
int	gargc;			/* global argument count */

int yylex()
{
	int	c;

	/*chkabort();*/
	while ((c = getc(fin)) == ' ' || c == '\t')	/* skip blanks */
		;

	if (c == EOF)				/* end of input */
		return 0;

	if (c == '.' || isdigit(c))		/* number */
	{
		ungetc(c, fin);
		fscanf(fin, "%lf", &yylval.rval);
		return NUMBER;
	}
	if (c == 'i')				/* possibly imaginary part */
	{
		if (!isalnum(c = getc(fin))) 	/* yes, it is */
		{
			ungetc(c, fin);
			return 'i';
		}
		ungetc(c, fin);			/* no, fall through to next */
		c = 'i';			/* restore c to proper value */
	}
	if (isalpha(c))				/* constant, var or builtin */
	{
		Symbol *s;
		char sbuf[100], *p = sbuf;
		
		do {
			*p++ = c;
		} while ((c = getc(fin)) != EOF && isalnum(c));
		ungetc(c, fin);	
		*p = '\0';

		if (inparamlist)
			s = allocsym(sbuf, PARAMETER);
		else
		{
			/* if in function definition, check it's argument
			   list for variable references BEFORE symtree	  */
			if (indefn && !strcmp(sbuf, userfunc->param->name))
				s = userfunc->param;
			else if (!(s = lookup(sbuf)))
				s = install(sbuf, UNDEF, zero);
		}

		yylval.sym = s;
		
		return s->type == UNDEF ? VAR : s->type;
	}
	if (c == '#')
	{
		while ((c = getc(fin)) != '\n' && c != EOF)
			;
		if (c == EOF)				/* end of input */
			return 0;
	}
	if (c == '\n')
		lineno++;

	return c;
}

void warning(s, t)
	char *s, *t;
{
	fprintf(stderr,"icalc: %s", s);
	if (t)
		fprintf(stderr," %s", t);
	if (infile)
	{
		fprintf(stderr," in %s,", infile);
		fprintf(stderr," near line %d\n", lineno);
	}
	fprintf(stderr,"\n");
}

void yyerror(s)
	char *s;
{
	warning(s, NULL);
}

void execerror(s, t)
	char *s;
	char *t;
{
	warning(s, t);
	longjmp(begin, 0);
}

void fpecatch()
{
	execerror("floating point exception", NULL);
}

int moreinput()
{
#ifdef AMIGA
	if (workbench)
	{
		infile = nextarg();
		if (infile == NULL)
			return 0;
	}
	else {
#endif
	if (gargc-- <= 0)
		return 0;
	if (fin && fin != stdin)
		fclose(fin);
	infile = *gargv++;
#ifdef AMIGA
	}
#endif
	lineno = 1;
	if (!strcmp(infile, "-"))
	{
		fin = stdin;
		infile = NULL;
		fprintf(stderr, "ready\n");
	}
	else if (!(fin = fopen(infile, "r")))
	{
		fprintf(stderr, "icalc: can't open %s\n", infile);
		return moreinput();
	}
	return 1;
}

int main(argc,argv)
	int argc;
	char **argv;
{
	fprintf(stderr,BANNER);			/* hello... */

#ifdef AMIGA
	if (argc == 0)		/* ran from workbench */
	{
		workbench = 1;	/* set workbench flag */
		gargc = WBenchMsg->sm_NumArgs-1;
		wbargv = &(WBenchMsg->sm_ArgList)[1];
	}
	else 
#endif
	if (argc == 1)	/* stdin only - fake argument list */
	{
		static char *stdinonly[] = { "-" };
		
		gargv = stdinonly;
		gargc = 1;
	}
	else
	{
		gargv = &argv[1];
		gargc = argc-1;
	}

	(void)rem_setkey(&remkey);		/* set 'remember' key */
	init();					/* build initial symbol tree */

	while (moreinput())			/* something to rea */
	{
		setjmp(begin);			/* where to come back to */
		signal(SIGFPE, fpecatch);	/* catch math errors */
		yyparse();			/* start parsing */
	}

	return 0;
}

#ifdef AMIGA

char *nextarg()
{
	static BPTR olddir = (BPTR)-1L;

	if (gargc > 0)
	{
		olddir = CurrentDir(wbargv->wa_Lock);
		gargc--;
		return (wbargv++)->wa_Name;
	}
	else if (gargc-- == 0)
	{
		if (olddir != ((BPTR)-1L))
			CurrentDir(olddir);
		return "-";
	}
	else return NULL;
}

#endif
