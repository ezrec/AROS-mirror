%option noyywrap

%{
/*
	> FLEX -d -i -I lex.l
	> Dcc lex.yy.c -mD -c
**
    21-08-94 - added FLAG, REFERENCE

*/

#include "Dyn.h"
#include "Yacc.tab.h"

#define VALUE ebody

#define _ESC "\\"
#define _KET "%}"

#define _ERROR 33333
#define ERROR fprintf(stderr, "Scanner Error! line %d\n", lineno); return _ERROR;

int lineno = 0;

#define LINENO getlineno()

#define unget(c) ungetc((c),stdin)

DSTR typename = EmptyDyn;
DSTR structname=EmptyDyn;
DSTR value    = EmptyDyn;
DSTR nodename = EmptyDyn;

#if 1
#define SHOW printf ("<%s>\n", yytext);
#else
#define SHOW ;
#endif

char lline[256];
char *getlineno(void) {
    sprintf (lline, "\n#line %d \"%s\"\n\t", lineno+1, "STDIN");
    return "\t"; /* Das sieht mit #line _echt_ _haesslich_ aus */
}
%}

BRA ([%][{])
KET ([%][}])
ESC "\\"

/* HELP_STR  (([?]) | ([Hh][Ee][Ll][Pp])) */
HELP_STR	   ([Hh][Ee][Ll][Pp])
SET_STR 	   ([Ss][Ee][Tt])
GET_STR 	   ([Gg][Ee][Tt])
FLG_STR 	   ([Ff][Ll][Aa][Gg][Ss])
REF_STR 	   ([Rr][Ee][Ff][Ee][Rr][Ee][Nn][Cc][Ee])
DEF_STR 	   ([Dd][Ee][Ff][Aa][Uu][Ll][Tt])
LOCK_STR	   ([Ll][Oo][Cc][Kk])
ULCK_STR	   ([Uu][Nn][Ll][Oo][Cc][Kk])
CRTE_STR	   ([Cc][Rr][Ee][Aa][Tt][Ee])
DLTE_STR	   ([Dd][Ee][Ll][Ee][Tt][Ee])


STARTLINE	   [\n][#]if(def)*[\t ]+SPC_VAR
ENDLINE 	   [\n][#]endif

ALNUM		   [A-Za-z0-9]

LABEL [A-Za-z_][A-Za-z_0-9]*

%start OUTER TYPE INNER INFRA INTER INTRA

%%

<OUTER>PROJECT	    {		   SHOW; return PROJECT; }
<OUTER>GLOBAL	    {		   SHOW; return GLOBAL_TOKEN; }

<OUTER>CONST	    {		   SHOW; return CONST; }

<OUTER>BIT	    {		   SHOW; return BIT; }
<OUTER>INT	    {		   SHOW; return INT; }
<OUTER>FNC	    {		   SHOW; return FUNC; }
<OUTER>STR	    {		   SHOW; return STRING; }
<OUTER>TREE	    {		   SHOW; return TREE; }

<OUTER>[;]	    {		   SHOW; return SEMI; }



<OUTER>{DEF_STR}    {	     ECHO; SHOW;			      return DEFAULT; }
<OUTER>{LABEL}	    {		   SHOW; yylval = (int)strdup(yytext); return OUTER_LABEL; }
<OUTER>[=]	    {		   SHOW;			      return ASS; }
<OUTER>{BRA}	    { BEGIN INNER; SHOW;			      return BRA; }

<INNER>SET	    {		   SHOW;			      return SET; }
<INNER>GET	    {		   SHOW;			      return GET; }
<INNER>LOCK	    {		   SHOW;			      return LOCK; }
<INNER>UNLOCK	    {		   SHOW;			      return UNLOCK; }
<INNER>CREATE	    {		   SHOW;			      return CREATE; }
<INNER>DELETE	    {		   SHOW;			      return DELETE; }
<INNER>REFERENCE    {		   SHOW;			      return REFERENCE; }
<INNER>{HELP_STR}   {		   SHOW;			      return HELP; }
<INNER>{LABEL}	    {		   SHOW; yylval = (int)strdup(yytext); return INNER_LABEL; }
<INNER>[=]	    { BEGIN INTER; SHOW; DynClear(&value);            return ASS; }
<INNER>[+][=]	    { BEGIN INTER; SHOW; DynClear(&value);            return ADD; }
<INNER>{KET}	    { BEGIN OUTER; SHOW;			      return KET; }
<INNER>[;]	    {		   SHOW;			      return SEMI; }


<INTER>[\n]	    { ++lineno; }
<INTER>[\t ]+	    { }
<INTER>{BRA}	    { BEGIN INFRA; SHOW; DynSet(&value, 1, LINENO);               }
<INTER>[;]	    { BEGIN INNER; SHOW;			    return SEMI;  }
<INTER>.	    { BEGIN INTRA;	 DynSet(&value, 2, LINENO, yytext);       }

<INFRA>[$][$]	    {		   SHOW; DynAppend(&value, 1, "value"); }
<INFRA>{ESC}{ESC}   {		   SHOW; DynAppend(&value, 1, _ESC);    }
<INFRA>{ESC}{KET}   {		   SHOW; DynAppend(&value, 1, _KET);    }
<INFRA>{KET}	    { BEGIN INTER; SHOW; yylval = (int)strdup(DynValue(&value)); return VALUE;  }
<INFRA>[\n]	    { ++ lineno;   SHOW  DynAppend(&value, 1, yytext); }
<INFRA>{ALNUM}+     {		   SHOW; DynAppend(&value, 1, yytext); }
<INFRA>.	    {		   SHOW; DynAppend(&value, 1, yytext);  }
<INFRA>[\t ]+	    {		   SHOW; DynAppend(&value, 1, yytext);  }

<INTRA>[$][$]	    {		   SHOW; DynAppend(&value, 1, "value"); }
<INTRA>[^;]+	    { BEGIN INNER; SHOW; DynAppend(&value, 1, yytext); yylval = (int)strdup(DynValue(&value)); return VALUE; }
<INTRA>[\n]	    { ++ lineno;   SHOW  DynAppend(&value, 1, yytext); }
<INTRA>{ALNUM}+     {		   SHOW; DynAppend(&value, 1, yytext); }
<INTRA>.	    {		   SHOW; DynAppend(&value, 1, yytext); }
<INTRA>[\t ]+	    {		   SHOW; DynAppend(&value, 1, yytext);  }

{STARTLINE}	    { BEGIN OUTER; lineno ++; }
<OUTER>{ENDLINE}    { BEGIN 0;	   lineno ++; return 0; }

[/][/][^\n]*[\n]    { ++lineno; /* Kommentare */ }

[ \t]+		    {  }
[\n]		    { ++lineno; }

<INNER>.	    {		   SHOW; ERROR; }
<OUTER>.	    {		   SHOW; ERROR; }
.		    { /* C-source ... nop */ }

%%

/*
[;]		    { BEGIN OUTER; SHOW; }
.		    { SHOW; ERROR; }
<OUTER>[\t ]	    { ; }
<OUTER>[\n]	    { ++ lineno; }
<OUTER>.	    { SHOW; ERROR; }
<TYPE>[\t ]	    { ; }
<TYPE>[\n]	    { ++ lineno; }
<TYPE>. 	    { SHOW; ERROR; }
<INNER>[\t ]	    { ; }
<INNER>[\n]	    { ++ lineno; }

<OUTER>PSEUDO	    {		   SHOW; return PSEUDO; }

<OUTER>WORD	    {		   SHOW; return WORD; }
<OUTER>UWORD	    {		   SHOW; return UWORD; }
<OUTER>LONG	    {		   SHOW; return LONG; }
<OUTER>ULONG	    {		   SHOW; return ULONG; }
<OUTER>STRING	    {		   SHOW; return STRING; }
<OUTER>STRUCT	    { BEGIN TYPE;  SHOW; return STRUCT; }
<TYPE>{LABEL}	    { BEGIN OUTER; SHOW; DynSet(&typename, 1, yytext);   return TYPE_LABEL; }

<OUTER>[*]	    {		   SHOW;				 return POINTER; }

   Nach Assign sind wir in INTER,
   das ueberprueft, ob eine Klammer folgt -> INFRA,
		       ein Semikolon	  -> INNER,
		 oder ein anderes Zeichen -> INTRA

<INNER>{FLG_STR}    {		   SHOW;			      return FLAGS; }

 */
