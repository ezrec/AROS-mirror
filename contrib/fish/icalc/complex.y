/*
*	Yacc input for complex-number parser, version 1.
*	Handles only simple arithmetical expressions: +, -, *, /, unary -.
*	Now added: exponentation w^z and conjugate, z'.
*	Arbitrary variable names now...and builtin functions.
*	And comments...constants - lots in fact. These things grow and grow!
*	Coming soon: user-defined functions...
*	Thank God for yacc - these things are a nightmare to hand-code...
*
*	Martin W.Scott, March 16, 1991.
*/
%{
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
%}

%union {
	double	rval;
	Symbol	*sym;
	Node	*node;
}
%start	list

%token	<rval>	NUMBER
%token	<sym>	VAR CONST C_BLTIN R_BLTIN FUNCDEF UFUNC COMMAND UNDEF
%token	<sym>	PARAMETER PRECISION REPEAT

%type	<node>	expr
%type	<sym>	parlist symbol

%right	'='			/* assignment */
%left	'+'  '-'		/* standard arithmetic operators */
%left	'*'  '/'
%left	UMINUS			/* unary minus */
%right	'^'			/* exponentation */
%left	'\''			/* conjugate operator z' = conj(z) */
%%

list:	  /* nothing */
	| list separator
	| list PRECISION NUMBER	separator	{ precision = (int)($3); }
	| list FUNCDEF symbol
		{
			if ($3->type == UFUNC)
			{
				clear_ufunc(&$3->u.ufunc);
			}
			else if ($3->type != UNDEF)
			{
				execerror($3->name, "already defined");
			}
			inparamlist = 1;
			$3->type = UFUNC;
			userfunc = &$3->u.ufunc;
			rem_freeall();
			userfunc->remkey = NULL;
			(void)rem_setkey(&userfunc->remkey);
		}
	  '(' parlist ')'
		{
			inparamlist = 0;
			indefn = 1;
			userfunc->param = $6;
		}
	  '=' expr separator
		{
			userfunc->tree = $10;
			(void)rem_setkey(&remkey);
			indefn = 0;
			if (!silent)
				fprintf(stdout, "\t%s(%s) defined\n", $3->name, userfunc->param->name);
		}

	| list COMMAND separator	{ (*($2->u.vptr))(); }
	| list opt_repeat expr separator
		{
			while (repeatcnt)
			{
				--repeatcnt;
				treeval = eval_tree($3);

				if (!silent)
					cprin(stdout, "\t", "\n", treeval);
				if (ans)		/* allocated successfully */
					ans->u.val = treeval;/* set 'last answer' const */
			}
			/* free all mem associated with this tree */
			rem_freeall();
		}
	| list error '\n'
		{
			if (indefn)
			{
				indefn = 0;
				rem_setkey(&remkey);
			}			
			inparamlist = 0;
			yyerrok;
		}
	;

parlist:  PARAMETER

/*	| parlist ',' PARAMETER	*/

symbol:	  VAR
	| UFUNC
	;

separator: '\n'		/* allows multiple commands/expressions on one line */
	| ';'
	;

opt_repeat:	/* nothing */		{ repeatcnt = 1; }
	| REPEAT NUMBER		{ repeatcnt = (int)($2); }
	;

expr:	  NUMBER		{ $$ = n_number($1, 0.0); }
	| NUMBER 'i'		{ $$ = n_number(0.0, $1); }
	| 'i'			{ $$ = n_number(0.0, 1.0); }
	| CONST			{ $$ = n_symbol(CONST, $1); }
	| CONST '=' expr	{ execerror("invalid assignment to constant", $1->name); }
	| VAR			{ if ($1->type == UNDEF)
					warning("using zero for undefined symbol", $1->name);
				  $$ = n_symbol(VAR, $1);
				}
	| PARAMETER		{ $$ = n_symbol(PARAMETER, $1); }
	| VAR '=' expr		{ $1->type = VAR; $$ = n_asgn($1, $3); }
	| C_BLTIN '(' expr ')'	{ $$ = n_func(C_BLTIN, $1, $3); }
	| R_BLTIN '(' expr ')'	{ $$ = n_func(R_BLTIN, $1, $3); }
	| UFUNC '(' expr ')'	{ $$ = n_func(UFUNC, $1, $3); }
	| expr '+' expr		{ $$ = n_binop('+', $1, $3); }
	| expr '-' expr		{ $$ = n_binop('-', $1, $3); }
	| expr '*' expr		{ $$ = n_binop('*', $1, $3); }
	| expr '/' expr		{ $$ = n_binop('/', $1, $3); }
	| expr '^' expr		{ $$ = n_binop('^', $1, $3); }
	| expr '\''		{ $$ = n_unop('\'', $1); }
	| '(' expr ')'		{ $$ = n_unop('(', $2); }
	| '-' expr %prec UMINUS { $$ = n_unop(UMINUS , $2); }
	;

%%

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

int yylex(void)
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
