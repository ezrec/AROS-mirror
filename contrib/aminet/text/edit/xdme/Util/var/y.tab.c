
/*  A Bison parser, made from Yacc.y with Bison version GNU Bison version 1.21
Amiga Port V1.00 by Uwe 'Hoover' Schuerkamp 6-May-93
Contact hoover@mathematik.uni-bielefeld.de on port-related Problems.
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	BRA	258
#define	KET	259
#define	ASS	260
#define	SEMI	261
#define	LABEL	262
#define	INNER_LABEL	263
#define	OUTER_LABEL	264
#define	PSEUDO	265
#define	PROJECT	266
#define	GLOBAL	267
#define	BIT	268
#define	STRING	269
#define	HELP	270
#define	SET	271
#define	GET	272
#define	CONST	273
#define	VALUE	274
#define	REFERENCE	275
#define	DEFAULT	276
#define	LOCK	277
#define	UNLOCK	278
#define	CREATE	279
#define	DELETE	280
#define	ADD	281
#define	CHAR	282
#define	ESC	283
#define	FUNC	284
#define	INT	285
#define	TREE	286
#define	ebody	287

#line 4 "Yacc.y"

/* Achtung, dieser Parser vergeudet speicher in rauhen megen ...*/

#define IFOK

/*
	> BISON -y -d Yacc.y
	> Dcc y.tab.c -mD -c
*/


#include <exec/types.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Dyn.h"
/* #include <alloca.h> */
#include <lists.h>
#include "main.h"

#define alloca	malloc


extern DSTR  value;
extern DSTR  structname;
extern DSTR  typename;

#define PTR void *



extern struct carrier *current;
extern struct carrier  defaults[];







#define YYDEBUG 1
/*
%token POINTER
%token WORD
%token UWORD
%token LONG
%token ULONG
%token STRUCT
%token FLAGS
%token F_VAL
*/

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		54
#define	YYFLAG		-32768
#define	YYNTBASE	33

#define YYTRANSLATE(x) ((unsigned)(x) <= 287 ? yytranslate[x] : 42)

static const char yytranslate[] = {     0,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,    10,    12,    15,    18,    19,    22,    25,
    28,    31,    34,    37,    40,    43,    45,    49,    50,    52,
    54,    57,    59,    64,    69,    72,    74,    76,    78,    80,
    82,    84,    86,    88
};

static const short yyrhs[] = {    34,
     0,    34,    33,     0,    35,     5,    37,     6,     0,     6,
     0,    36,     9,     0,    36,    21,     0,     0,    18,    36,
     0,    29,    36,     0,    14,    36,     0,    30,    36,     0,
    13,    36,     0,    31,    36,     0,    12,    36,     0,    11,
    36,     0,     9,     0,     3,    38,     4,     0,     0,    39,
     0,    40,     0,    39,    40,     0,     6,     0,    41,     5,
    32,     6,     0,    41,    26,    32,     6,     0,    41,     6,
     0,    17,     0,    16,     0,    15,     0,    22,     0,    23,
     0,    24,     0,    25,     0,    20,     0,     8,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    98,    99,   102,   103,   106,   107,   110,   111,   112,   113,
   114,   115,   116,   117,   118,   121,   122,   125,   126,   129,
   130,   133,   134,   135,   136,   139,   140,   141,   142,   143,
   144,   145,   146,   147
};

static const char * const yytname[] = {   "$","error","$illegal.","BRA","KET",
"ASS","SEMI","LABEL","INNER_LABEL","OUTER_LABEL","PSEUDO","PROJECT","GLOBAL",
"BIT","STRING","HELP","SET","GET","CONST","VALUE","REFERENCE","DEFAULT","LOCK",
"UNLOCK","CREATE","DELETE","ADD","CHAR","ESC","FUNC","INT","TREE","ebody","file",
"union","header","prefix","body","opt_expr_seq","expr_seq","expr","lval",""
};
#endif

static const short yyr1[] = {     0,
    33,    33,    34,    34,    35,    35,    36,    36,    36,    36,
    36,    36,    36,    36,    36,    37,    37,    38,    38,    39,
    39,    40,    40,    40,    40,    41,    41,    41,    41,    41,
    41,    41,    41,    41
};

static const short yyr2[] = {     0,
     1,     2,     4,     1,     2,     2,     0,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     0,     1,     1,
     2,     1,     4,     4,     2,     1,     1,     1,     1,     1,
     1,     1,     1,     1
};

static const short yydefact[] = {     7,
     4,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     0,     0,    15,    14,    12,    10,     8,     9,    11,    13,
     2,     0,     5,     6,    18,    16,     0,    22,    34,    28,
    27,    26,    33,    29,    30,    31,    32,     0,    19,    20,
     0,     3,    17,    21,     0,    25,     0,     0,     0,    23,
    24,     0,     0,     0
};

static const short yydefgoto[] = {    21,
    10,    11,    12,    27,    38,    39,    40,    41
};

static const short yypact[] = {     9,
-32768,    23,    23,    23,    23,    23,    23,    23,    23,     0,
     2,    -4,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    -1,-32768,-32768,    40,-32768,    -5,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,     5,    40,-32768,
    -2,-32768,-32768,-32768,   -22,-32768,   -16,    13,    19,-32768,
-32768,    26,    28,-32768
};

static const short yypgoto[] = {    32,
-32768,-32768,    64,-32768,-32768,-32768,    -6,-32768
};


#define	YYLAST		73


static const short yytable[] = {    -1,
    42,    25,    45,    46,    23,     1,    22,    26,    43,    48,
     2,     3,     4,     5,     1,    49,    24,     6,    50,     2,
     3,     4,     5,    47,    51,    53,     6,    54,     7,     8,
     9,    52,    44,     2,     3,     4,     5,     7,     8,     9,
     6,     0,     0,     0,     0,    28,     0,    29,     0,     0,
     0,     7,     8,     9,    30,    31,    32,     0,     0,    33,
     0,    34,    35,    36,    37,    13,    14,    15,    16,    17,
    18,    19,    20
};

static const short yycheck[] = {     0,
     6,     3,     5,     6,     9,     6,     5,     9,     4,    32,
    11,    12,    13,    14,     6,    32,    21,    18,     6,    11,
    12,    13,    14,    26,     6,     0,    18,     0,    29,    30,
    31,     0,    39,    11,    12,    13,    14,    29,    30,    31,
    18,    -1,    -1,    -1,    -1,     6,    -1,     8,    -1,    -1,
    -1,    29,    30,    31,    15,    16,    17,    -1,    -1,    20,
    -1,    22,    23,    24,    25,     2,     3,     4,     5,     6,
     7,     8,     9
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "S:bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca (unsigned int);
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		yylex(&yylval, &yylloc)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 184 "bison.simple"
int
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
#ifdef YYLSP_NEEDED
		 &yyls1, size * sizeof (*yylsp),
#endif
		 &yystacksize);

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
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
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 3:
#line 102 "Yacc.y"
{IFOK{ carrier_new(); };
    break;}
case 5:
#line 106 "Yacc.y"
{IFOK{ carrier_name(current, (PTR)yyvsp[0]); };
    break;}
case 6:
#line 107 "Yacc.y"
{IFOK{ int tp = current->type; carrier_cpy (&defaults[tp], current); carrier_drop(current); current = &defaults[tp]; };
    break;}
case 8:
#line 111 "Yacc.y"
{IFOK{ carrier_const(current);    };
    break;}
case 9:
#line 112 "Yacc.y"
{IFOK{ carrier_type (current, CT_FNC);  };
    break;}
case 10:
#line 113 "Yacc.y"
{IFOK{ carrier_type (current, CT_STR);  };
    break;}
case 11:
#line 114 "Yacc.y"
{IFOK{ carrier_type (current, CT_INT);  };
    break;}
case 12:
#line 115 "Yacc.y"
{IFOK{ carrier_type (current, CT_BIT);  };
    break;}
case 13:
#line 116 "Yacc.y"
{IFOK{ carrier_type (current, CT_TREE); };
    break;}
case 16:
#line 121 "Yacc.y"
{IFOK{ struct carrier *l; assert(l = carrier_lock((PTR)yyvsp[0])); carrier_cpy(current, l); };
    break;}
case 23:
#line 134 "Yacc.y"
{IFOK{ struct node *n;    if (n = node_lock((PTR)yyvsp[-3])) node_drop(n);          node_add (current, (PTR)yyvsp[-3], (PTR)yyvsp[-1]); };
    break;}
case 24:
#line 135 "Yacc.y"
{IFOK{ struct node *n; assert(n = node_lock((PTR)yyvsp[-3])); node_paste(n,(PTR)yyvsp[-1]); node_add (current, (PTR)yyvsp[-3], (PTR)yyvsp[-1]); };
    break;}
case 25:
#line 136 "Yacc.y"
{IFOK{ struct node *n;  if (!(n = node_lock((PTR)yyvsp[-1])))                       node_add (current, (PTR)yyvsp[-1], NULL); };
    break;}
case 26:
#line 139 "Yacc.y"
{IFOK{ yyval = strdup("GET");       };
    break;}
case 27:
#line 140 "Yacc.y"
{IFOK{ yyval = strdup("SET");       };
    break;}
case 28:
#line 141 "Yacc.y"
{IFOK{ yyval = strdup("HELP");      };
    break;}
case 29:
#line 142 "Yacc.y"
{IFOK{ yyval = strdup("LOCK");      };
    break;}
case 30:
#line 143 "Yacc.y"
{IFOK{ yyval = strdup("UNLOCK");    };
    break;}
case 31:
#line 144 "Yacc.y"
{IFOK{ yyval = strdup("CREATE");    };
    break;}
case 32:
#line 145 "Yacc.y"
{IFOK{ yyval = strdup("DELETE");    };
    break;}
case 33:
#line 146 "Yacc.y"
{IFOK{ yyval = strdup("REFERENCE"); };
    break;}
case 34:
#line 147 "Yacc.y"
{IFOK{ yyval = (yyvsp[0]);                };
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 457 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 169 "Yacc.y"






int yyerror (const char *x)
{
    extern int lineno;
    fprintf (stderr, "%s\n (line %ld)\n", x, lineno);
    exit (-1);
    return 0;
} /* yyerror */






