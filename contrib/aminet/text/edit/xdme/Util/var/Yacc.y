


%{
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
/*#include <lists.h>*/
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
%}

%token BRA
%token KET
%token ASS
%token SEMI
%token LABEL
%token INNER_LABEL
%token OUTER_LABEL
%token PSEUDO
%token PROJECT
%token GLOBAL_TOKEN
%token BIT
%token STRING
%token HELP
%token SET
%token GET
%token CONST
/* %token VALUE	    / * addes since 21-08-94 */
%token REFERENCE
%token DEFAULT
%token LOCK
%token UNLOCK
%token CREATE
%token DELETE
%token ADD

%token CHAR
%token ESC
%token FUNC
%token INT
%token TREE
%token ebody




%start file

%%

file : union
     | union file
     ;

union : header ASS body SEMI {IFOK{ carrier_new(); }}
      | 		SEMI
      ;

header : prefix OUTER_LABEL {IFOK{ carrier_name(current, (PTR)$2); }}
       | prefix DEFAULT     {IFOK{ int tp = current->type; carrier_cpy (&defaults[tp], current); carrier_drop(current); current = &defaults[tp]; }}
       ;

prefix : /* -/- */
       | CONST	 prefix     {IFOK{ carrier_const(current);    }}
       | FUNC	 prefix     {IFOK{ carrier_type (current, CT_FNC);  }}
       | STRING  prefix     {IFOK{ carrier_type (current, CT_STR);  }}
       | INT	 prefix     {IFOK{ carrier_type (current, CT_INT);  }}
       | BIT	 prefix     {IFOK{ carrier_type (current, CT_BIT);  }}
       | TREE	 prefix     {IFOK{ carrier_type (current, CT_TREE); }}
       | GLOBAL_TOKEN  prefix
       | PROJECT prefix
       ;

body : OUTER_LABEL	    {IFOK{ struct carrier *l; assert(l = carrier_lock((PTR)$1)); carrier_cpy(current, l); }}
     | BRA opt_expr_seq KET
     ;

opt_expr_seq : /* -/- */
	     | expr_seq
	     ;

expr_seq : expr
	 | expr_seq expr
	 ;

expr : SEMI
     | lval ASS ebody SEMI  {IFOK{ struct node *n;    if (n = node_lock((PTR)$1)) node_drop(n);          node_add (current, (PTR)$1, (PTR)$3); }}
     | lval ADD ebody SEMI  {IFOK{ struct node *n; assert(n = node_lock((PTR)$1)); node_paste(n,(PTR)$3); node_add (current, (PTR)$1, (PTR)$3); }}
     | lval	      SEMI  {IFOK{ struct node *n;  if (!(n = node_lock((PTR)$1)))                       node_add (current, (PTR)$1, NULL); }}
     ;

lval : GET		    {IFOK{ $$ = strdup("GET");       }}
     | SET		    {IFOK{ $$ = strdup("SET");       }}
     | HELP		    {IFOK{ $$ = strdup("HELP");      }}
     | LOCK		    {IFOK{ $$ = strdup("LOCK");      }}
     | UNLOCK		    {IFOK{ $$ = strdup("UNLOCK");    }}
     | CREATE		    {IFOK{ $$ = strdup("CREATE");    }}
     | DELETE		    {IFOK{ $$ = strdup("DELETE");    }}
     | REFERENCE	    {IFOK{ $$ = strdup("REFERENCE"); }}
     | INNER_LABEL	    {IFOK{ $$ = ($1);                }}
     ;

/*
ebody : BRA text_seq KET
      | char_seq
      ;

text_seq :		  char_seq
      | text_seq SEMI	  char_seq
      | text_seq ESC KET  char_seq
      ;

char_seq :
	 | char_seq CHAR
	 | char_seq lval
	 | char_seq prefix
	 | char_seq ASS
	 | char_seq ADD
	 ;
*/

%%





int yyerror (const char *x)
{
    extern int lineno;
    fprintf (stderr, "%s\n (line %ld)\n", x, lineno);
    exit (-1);
    return 0;
} /* yyerror */






