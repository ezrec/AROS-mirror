%{

#ifndef lint
static char *RCSid = "$Id$";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <time.h>
#include "rexx.h"

#if defined(HAVE_MALLOC_H)
# include <malloc.h>
#endif

#if defined(HAVE_ALLOCA_H)
# include <alloca.h>
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(_MSC_VER) || defined(MAC)
# define __STDC__ 1 /* Hack to allow const since it is not defined */
#endif

#define YYSTYPE nodeptr

/* locals, they are protected by regina_parser (see lexsrc.l) */
static int tmplno, tmpchr ;
static nodeptr current, with = NULL ;
#if 0 /* see below */
static nodeptr *narray=NULL ;
static int narptr=0, narmax=0 ;
#endif

static int gettypeof( nodeptr this ) ;
static void checkconst( nodeptr this ) ;
static void transform( nodeptr this ) ;
static nodeptr create_head( const char *name ) ;
static nodeptr makenode( int type, int numb, ... ) ;
static void checkdosyntax( cnodeptr this ) ;
void newlabel( const tsd_t *TSD, internal_parser_type *ipt, nodeptr this ) ;

%}

%token ADDRESS ARG CALL DO TO BY FOR WHILE UNTIL EXIT IF THEN ELSE
%token ITERATE INTERPRET LEAVE NOP NUMERIC PARSE EXTERNAL SOURCE VAR
%token VALUE WITH PROCEDURE EXPOSE PULL PUSH QUEUE SAY RETURN SELECT
%token WHEN DROP OTHERWISE SIGNAL ON OFF ERROR SYNTAX HALT NOVALUE
%token TRACE END UPPER ASSIGNMENTVARIABLE STATSEP FOREVER DIGITS FORM
%token FUZZ SCIENTIFIC ENGINEERING NOT CONCATENATE MODULUS GTE GT LTE
%token LT DIFFERENT EQUALEQUAL NOTEQUALEQUAL OFFSET SPACE EXP XOR
%token PLACEHOLDER NOTREADY CONSYMBOL SIMSYMBOL EXFUNCNAME INFUNCNAME
%token LABEL DOVARIABLE HEXSTRING STRING VERSION LINEIN WHATEVER NAME
%token FAILURE BINSTRING OPTIONS ENVIRONMENT LOSTDIGITS
%token GTGT LTLT NOTGTGT NOTLTLT GTGTE LTLTE
%token INPUT OUTPUT ERROR NORMAL APPEND REPLACE STREAM STEM LIFO FIFO

%start prog

%left '|' XOR
%left '&'
%nonassoc '=' DIFFERENT GTE GT LT LTE EQUALEQUAL NOTEQUALEQUAL GTGT LTLT NOTGTGT NOTLTLT GTGTE LTLTE
%left CONCATENATE SPACE CCAT
%left '+' '-'
%left '*' '/' '%' MODULUS
%left EXP
%left UMINUS UPLUSS NOT
%nonassoc SYNTOP

%nonassoc THEN
%nonassoc ELSE

%right STATSEP

%{
#ifdef NDEBUG
# define YYDEBUG 0
#else
# define YYDEBUG 1
#endif
%}

%%

prog         : nseps stats { EndProg( $2 ) ; }
             | nseps       { EndProg( NULL ) ; }
             ;

stats        : ystatement stats         { $$->next = $2 ; }
             | ystatement               { $$->next = NULL ; }
             ;

xstats       : statement xstats         { $$->next = $2 ; }
             | statement gruff          { $$->next = NULL ; }
             ;

ystatement   : statement               { $$ = $1 ; }
             | lonely_end        {  exiterror( ERR_UNMATCHED_END, 1 )  ; }
             ;

lonely_end   : gruff end seps
             ;

nxstats      : xstats                  { $$ = $1 ; }
             | gruff                   { $$ = NULL ; }
             ;

nseps        : seps
             |
             ;

seps         : STATSEP seps
             | STATSEP
             ;

statement    : mstatement   { $$=$1; }
             | ex_when_stat { $$=$1; }
             ;

mstatement   : mttstatement { $$=$1; }
             | unexp_then   { $$=$1; }
             ;

gruff        : { tmpchr=parser_data.tstart; tmplno=parser_data.tline; }
             ;

mttstatement : gruff mtstatement { $$=$2; }
             ;

mtstatement  : address_stat
             | expr_stat
             | arg_stat
             | call_stat
             | do_stat
             | drop_stat
             | exit_stat
             | if_stat
             | unexp_else
             | ipret_stat
             | iterate_stat
             | label_stat
             | leave_stat
             | nop_stat
             | numeric_stat
             | options_stat
             | parse_stat
             | proc_stat
             | pull_stat
             | push_stat
             | queue_stat
             | return_stat
             | say_stat
             | select_stat
             | signal_stat
             | trace_stat
             | upper_stat
             | assignment
             ;

call         : CALL                    { $$ = makenode(X_CALL,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
do           : DO                      { $$ = makenode(X_DO,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
exit         : EXIT                    { $$ = makenode(X_EXIT,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
if           : IF                      { $$ = makenode(X_IF,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
iterate      : ITERATE                 { $$ = makenode(X_ITERATE,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
leave        : LEAVE                   { $$ = makenode(X_LEAVE,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
say          : SAY                     { $$ = makenode(X_SAY,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
return       : RETURN                  { $$ = makenode(X_RETURN,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
address      : ADDRESS                 { $$ = makenode(X_ADDR_N,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
arg          : ARG                     { $$ = makenode(X_PARSE_ARG_U,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
drop         : DROP                    { $$ = makenode(X_DROP,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
interpret    : INTERPRET               { $$ = makenode(X_IPRET,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
label        : LABEL                   { $$ = makenode(X_LABEL,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
nop          : NOP                     { $$ = makenode(X_NULL,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; }
numeric      : NUMERIC                 { $$ = makenode(0,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
options      : OPTIONS                 { $$ = makenode(X_OPTIONS,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
parse        : PARSE                   { $$ = makenode(0,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
proc         : PROCEDURE               { $$ = makenode(X_PROC,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
pull         : PULL                    { $$ = makenode(X_PULL,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
push         : PUSH                    { $$ = makenode(X_PUSH,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
queue        : QUEUE                   { $$ = makenode(X_QUEUE,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
select       : SELECT                  { $$ = makenode(X_SELECT,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
signal       : SIGNAL                  { $$ = makenode(X_SIG_LAB,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
when         : WHEN                    { $$ = makenode(X_WHEN,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
otherwise    : OTHERWISE               { $$ = makenode(X_OTHERWISE,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
trace        : TRACE                   { $$ = makenode(X_TRACE,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
upper        : UPPER                   { $$ = makenode(X_UPPER_VAR,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; } ;
address_stat : address                 { $$ = current = $1 ; }
               address_stat2
             ;

address_stat2: VALUE expr naddr_with   { current->type = X_ADDR_V ;
                                         current->p[0] = $2 ;
                                         current->p[1] = $3 ; }
             | addr_with               { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
             | seps                    { current->type = X_ADDR_S ; }
             | error                   { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
               naddr_with
             | nvir nexpr naddr_with   { current->name = (streng *)$1 ;
                                         current->type = X_ADDR_N ;
                                         current->p[0] = $2 ;
                                         current->p[1] = $3 ; }
             | '(' expr ')' nspace naddr_with { current->type = X_ADDR_V ;
                                         current->p[0] = $2 ;
                                         current->p[1] = $5 ;
                                         current->u.nonansi = 1 ; }
             ;

arg_stat     : arg templs seps         { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

call_stat    : call asymbol exprs seps { $$ = $1 ;
                                         $$->p[0] = $3 ;
                                         $$->name = (streng *) $2 ; }
             | call string exprs seps  { $$ = $1 ;
                                         $$->type = X_EX_FUNC ;
                                         $$->p[0] = $3 ;
                                         $$->name = (streng *) $2 ; }
             | call function seps      { $$ = $1 ;
                                         $$->name = $2->name ;
                                         $$->p[0] = $2->p[0] ;
                                         RejectNode( $2 ); }
             | call on error { exiterror( ERR_INV_SUBKEYWORD, 1, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
                       seps
             | call off error { exiterror( ERR_INV_SUBKEYWORD, 2, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
                        seps
             | call on c_action error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                                seps
             | call on c_action namespec error { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
                                         seps
             | call on c_action namespec seps
                                       { $$ = $1 ;
                                         $$->type = X_CALL_SET ;
                                         $$->p[0] = $2 ;
                                         $$->name = (streng *)$4 ;
                                         $$->p[1] = $3 ; }
             | call on c_action seps   { $$ = $1 ;
                                         $$->type = X_CALL_SET ;
                                         $$->p[0] = $2 ;
                                         $$->name = NULL ;
                                         $$->p[1] = $3 ; }
             | call off c_action error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                                 seps
             | call off c_action seps  { $$ = $1 ;
                                         $$->type = X_CALL_SET ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $3 ; }
             ;

expr_stat    : expr seps
                                       { $$ = makenode(X_COMMAND,0) ;
                                         $$->charnr = tmpchr ;
                                         $$->lineno = tmplno;
                                         $$->p[0] = $1 ; }
             ;

end_stat     : END                     { $$ = makenode(X_END,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; }
             ;

end          : end_stat simsymb       { $$ = $1 ;
                                         $$->name = (streng*)($2) ; }
             | end_stat                { $$ = $1 ; }
             | end_stat simsymb error {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
             ;

do_stat      :  do repetitor conditional seps nxstats end seps
                                       { $$ = $1 ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $3 ;
                                         $$->p[2] = $5 ;
                                         $$->p[3] = $6 ;
                          if (($$->p[0]==NULL || $$->p[0]->name==NULL)
                              && $$->p[3]->name)
                                            exiterror( ERR_UNMATCHED_END, 0 ) ;
                          if (($$->p[0])&&($$->p[0]->name)&&
                              ($$->p[3]->name)&&
                              (($$->p[3]->name->len != $$->p[0]->name->len)||
                               (strncmp($$->p[3]->name->value,
                                        $$->p[0]->name->value,
                                        $$->p[0]->name->len))))
                                            exiterror( ERR_UNMATCHED_END, 0 )  ;
                                       }
             ;

repetitor    : dovar '=' expr tobyfor tobyfor tobyfor
                                       { $$ =makenode(X_REP,4,$3,$4,$5,$6) ;
                                         $$->name = (streng *)$1 ;
                                         checkdosyntax($$) ; }
             | dovar '=' expr tobyfor tobyfor
                                       { $$ =makenode(X_REP,3,$3,$4,$5) ;
                                         $$->name = (streng *)$1 ;
                                         checkdosyntax($$) ; }
             | dovar '=' expr tobyfor  { $$ =makenode(X_REP,2,$3,$4) ;
                                         $$->name = (streng *)$1 ;
                                         checkdosyntax($$) ; }
             | dovar '=' expr          { $$ =makenode(X_REP,1,$3) ;
                                         $$->name = (streng *)$1 ;
                                         checkdosyntax($$) ; }
             | FOREVER                 { $$ = makenode(X_REP_FOREVER,0) ; }
             | FOREVER error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                       seps
             | expr                    { $1 = makenode(X_DO_EXPR,1,$1) ;
                                         $$ = makenode(X_REP,2,NULL,$1) ; }
             |                         { $$ = NULL ; }
             ;

nvir         : CONSYMBOL               { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
             | SIMSYMBOL               { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
             | STRING                  { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
             ;

naddr_with   :                         { SymbolDetect |= SD_ADDRWITH ;
                                         $$ = with = makenode(X_ADDR_WITH,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; }
               addr_with               { with = NULL ;
                                         SymbolDetect &= ~SD_ADDRWITH ; }
             | seps                    { $$ = NULL ; }
             ;

addr_with    : WITH connection seps    { $$ = $2 ; /* pro forma */ }
             | WITH connection error seps { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
             | WITH nspace seps        { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
             ;

connection   : inputstmts
             | outputstmts
             | errorstmts
             | error                   { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
             ;

inputstmts   : inputstmt
               adeo
             ;

outputstmts  : outputstmt
               adei
             ;

errorstmts   : errorstmt
               adio
             ;

adeo         : outputstmt nspace
             | outputstmt errorstmt nspace
             | errorstmt nspace
             | errorstmt outputstmt nspace
             | nspace
             ;

adei         : inputstmt nspace
             | inputstmt errorstmt nspace
             | errorstmt nspace
             | errorstmt inputstmt nspace
             | nspace
             ;

adio         : inputstmt nspace
             | inputstmt outputstmt nspace
             | outputstmt nspace
             | outputstmt inputstmt nspace
             | nspace
             ;

inputstmt    : nspace INPUT nspace resourcei { with->p[0] = $4; }
             | nspace INPUT error      { exiterror( ERR_INV_SUBKEYWORD, 6, __reginatext ) ; }
             ;

outputstmt   : nspace OUTPUT nspace resourceo { with->p[1] = $4; }
             | nspace OUTPUT error     { exiterror( ERR_INV_SUBKEYWORD, 7, __reginatext ) ; }
             ;

errorstmt    : nspace ERROR nspace resourceo { with->p[2] = $4; }
             | nspace ERROR error      { exiterror( ERR_INV_SUBKEYWORD, 14, __reginatext ) ; }
             ;

resourcei    : resources               { $$ = $1 ; }
             | NORMAL                  { $$ = makenode(X_ADDR_WITH, 0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; }
             ;

resourceo    : resources               { $$ = $1 ; }
             | APPEND resources        { $$ = $2 ;
                                         $$->u.of.append = 1 ; }
             | APPEND error            { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
             | REPLACE resources       { $$ = $2 ; }
             | REPLACE error           { exiterror( ERR_INV_SUBKEYWORD, 9, __reginatext ) ; }
             | NORMAL                  { $$ = makenode(X_ADDR_WITH, 0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; }
             ;

resources    : STREAM nnvir            { /* ANSI extension: nsimsymb is
                                          * used by the standard, but I think
                                          * there are no reasons why using
                                          * it here as a must. FGC
                                          */
                                         $$ = makenode(X_ADDR_WITH, 0) ;
                                         $$->name = (streng *) $2 ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ;
                                         $$->u.of.awt = isSTREAM;
                                         SymbolDetect |= SD_ADDRWITH ; }
             | STREAM error            { exiterror( ERR_INVALID_OPTION, 1, __reginatext ) ; }
             | STEM nsimsymb           {
                                         streng *tmp=(streng *)$2;
                                         if ( tmp->value[(tmp->len)-1] != '.' )
                                            exiterror( ERR_INVALID_OPTION, 3, __reginatext );
                                         $$ = makenode(X_ADDR_WITH, 0, 0) ;
                                         $$->name = (streng *) $2 ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ;
                                         $$->u.of.awt = isSTEM;
                                         SymbolDetect |= SD_ADDRWITH ; }
             | STEM error              { exiterror( ERR_INVALID_OPTION, 2, __reginatext ) ; }
             | LIFO nnvir              {
                                         $$ = makenode(X_ADDR_WITH, 0) ;
                                         $$->name = (streng *) $2 ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ;
                                         $$->u.of.awt = isLIFO;
                                         SymbolDetect |= SD_ADDRWITH ; }
             | LIFO error              { exiterror( ERR_INVALID_OPTION, 100, __reginatext ) ; }
             | FIFO nnvir              {
                                         $$ = makenode(X_ADDR_WITH, 0) ;
                                         $$->name = (streng *) $2 ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ;
                                         $$->u.of.awt = isFIFO;
                                         SymbolDetect |= SD_ADDRWITH ; }
             | FIFO error              { exiterror( ERR_INVALID_OPTION, 101, __reginatext ) ; }
             ;

nsimsymb     :                         { SymbolDetect &= ~SD_ADDRWITH ; }
               nspace xsimsymb         { $$ = $3 ; }
             ;

nnvir        :                         { SymbolDetect &= ~SD_ADDRWITH ; }
               nspace nvir             { $$ = $3 ; }
             ;

nspace       : SPACE
             |
             ;

dovar        : DOVARIABLE              { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
             ;

tobyfor      : TO expr                 { $$ = makenode(X_DO_TO,1,$2) ; }
             | FOR expr                { $$ = makenode(X_DO_FOR,1,$2) ; }
             | BY expr                 { $$ = makenode(X_DO_BY,1,$2) ; }
             ;

conditional  : WHILE expr              { $$ = makenode(X_WHILE,1,$2) ; }
             | UNTIL expr              { $$ = makenode(X_UNTIL,1,$2) ; }
             |                         { $$ = NULL ; }
             ;

drop_stat    : drop anyvars error { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
             | drop anyvars seps       { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

upper_stat   : upper anyvars error { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
             | upper anyvars seps       { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

exit_stat    : exit nexpr seps         { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

if_stat      : if expr nseps THEN nseps ystatement
                                       { $$ = $1 ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $6 ; }
             | if expr nseps THEN nseps ystatement ELSE nseps ystatement
                                       { $$ = $1 ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $6 ;
                                         $$->p[2] = $9 ; }
             | if expr nseps THEN nseps ystatement ELSE nseps error
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 4 ) ;}
             | if expr nseps THEN nseps error
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 3 ) ;}
             | if seps                 {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
             | if expr nseps error     {  exiterror( ERR_THEN_EXPECTED, 1, parser_data.if_linenr, __reginatext )  ; }
             ;

unexp_then   : gruff THEN              {  exiterror( ERR_THEN_UNEXPECTED, 1 )  ; }
             ;

unexp_else   : gruff ELSE              {  exiterror( ERR_THEN_UNEXPECTED, 2 )  ; }
             ;

ipret_stat   : interpret expr seps     { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;


iterate_stat : iterate simsymb seps    { $$ = $1 ;
                                         $$->name = (streng *) $2 ; }
             | iterate simsymb error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                               seps
             | iterate seps            { $$ = $1 ; }
             ;

label_stat   : labelname nseps         { $$ = $1 ;
                                         newlabel( (const tsd_t *)parser_data.TSD,
                                                   &parser_data,
                                                   $1 ) ; }
             ;

labelname    : label                   { $$ = $1 ;
                                         $$->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
             ;

leave_stat   : leave simsymb seps      { $$ = $1 ;
                                         $$->name = (streng *) $2 ; }
             | leave simsymb error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                             seps
             | leave seps              { $$ = $1 ; }
             ;

nop_stat     : nop seps                { $$ = $1 ; }
             | nop error {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                   seps
             ;

numeric_stat : numeric DIGITS expr seps { $$ = $1 ;
                                         $$->type = X_NUM_D ;
                                         $$->p[0] = $3 ; }
             | numeric DIGITS seps     { $$ = $1; $$->type = X_NUM_DDEF ; }
             | numeric FORM form_expr error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                                      seps
             | numeric FORM form_expr seps
                                       { $$ = $1 ;
                                         $$->type = X_NUM_F ;
                                         $$->p[0] = $3 ; }
             | numeric FORM seps       { /* NOTE! This clashes ANSI! */
                                         $$ = $1 ; $$->type=X_NUM_FRMDEF ;}
             | numeric FORM VALUE expr seps  { $$ = $1 ; $$->type=X_NUM_V ;
                                         $$->p[0] = $4 ; }
             | numeric FORM error { exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", __reginatext ) ;}
                            seps
             | numeric FUZZ seps       { $$ = $1; $$->type = X_NUM_FDEF ;}
             | numeric FUZZ expr seps  { $$ = $1 ;
                                         $$->type = X_NUM_FUZZ ;
                                         $$->p[0] = $3 ; }
             | numeric error { exiterror( ERR_INV_SUBKEYWORD, 15, "DIGITS FORM FUZZ", __reginatext ) ;}
                       seps
             ;

form_expr    : SCIENTIFIC              { $$ = makenode(X_NUM_SCI,0) ; }
             | ENGINEERING             { $$ = makenode(X_NUM_ENG,0) ; }
             ;

options_stat : options nexpr seps      { ($$=$1)->p[0]=$2 ; }
             ;

parse_stat   : parse parse_param template seps
                                       { $$ = $1 ;
                                         $$->type = X_PARSE ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $3 ; }
             | parse UPPER parse_param template seps
                                       { $$ = $1 ;
                                         $$->type = X_PARSE_U ;
                                         $$->p[0] = $3 ;
                                         $$->p[1] = $4 ; }
             | parse ARG templs seps   { $$ = $1 ;
                                         $$->type = X_PARSE_ARG ;
                                         $$->p[0] = $3 ; }
             | parse UPPER ARG templs seps
                                       { $$ = $1 ;
                                         $$->type = X_PARSE_ARG_U ;
                                         $$->p[0] = $4 ; }
             | parse UPPER error { exiterror( ERR_INV_SUBKEYWORD, 13, "ARG EXTERNAL LINEIN PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
             | parse error { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG EXTERNAL LINEIN PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
                     seps
             ;

templs       : template ',' templs     { $$ = $1 ; $$->next = $3 ; }
             | template                { $$ = $1 ; }
             ;

parse_param  : LINEIN                  { $$ = makenode(X_PARSE_EXT,0) ; }
             | EXTERNAL                { $$ = makenode(X_PARSE_EXT,0) ; }
             | VERSION                 { $$ = makenode(X_PARSE_VER,0) ; }
             | PULL                    { $$ = makenode(X_PARSE_PULL,0) ; }
             | SOURCE                  { $$ = makenode(X_PARSE_SRC,0) ; }
             | VAR simsymb             { $$ = makenode(X_PARSE_VAR,0) ;
                                         $$->name = (streng *) $2 ; }
             | VALUE nexpr WITH        { $$ = makenode(X_PARSE_VAL,1,$2) ; }
             | VALUE error seps        { exiterror( ERR_INVALID_TEMPLATE, 3 ) ;}
             ;

proc_stat    : proc seps               { $$ = $1 ; }
             | proc error { exiterror( ERR_INV_SUBKEYWORD, 17, __reginatext ) ;}
                    seps
             | proc EXPOSE error { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
                           seps
             | proc EXPOSE anyvars error { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
                                   seps
             | proc EXPOSE anyvars seps { $$ = $1 ;
                                         $$->p[0] = $3 ; }
             ;

pull_stat    : pull template seps      { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

push_stat    : push nexpr seps         { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

queue_stat   : queue nexpr seps        { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

say_stat     : say nexpr seps          { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

return_stat  : return nexpr seps       { $$ = $1 ;
                                         $$->p[0] = $2 ; }
             ;

sel_end      : END simsymb             {  exiterror( ERR_UNMATCHED_END, 0 ) ;}
             | END simsymb error       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
             | END
             ;

select_stat  : select seps when_stats otherwise_stat sel_end seps
                                       { $$ = $1 ;
                                         $$->p[0] = $3 ;
                                         $$->p[1] = $4 ; }
             | select seps END error   {  exiterror( ERR_WHEN_EXPECTED, 0 ) ;}
             | select seps otherwise error
                                       {  exiterror( ERR_WHEN_EXPECTED, 0 ) ;}
             | select error            {  exiterror( ERR_EXTRA_DATA, 1, __reginatext )  ;}
             | select seps THEN        {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
             | select seps when_stats otherwise error
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
             ;

when_stats   : when_stat when_stats    { $$->next = $2 ; }
             | when_stat               { $$ = $1 ; }
             | error                   {  exiterror( ERR_WHEN_EXPECTED, 0 )  ;}
             ;

when_stat    : when expr nseps THEN nseps statement
                                       { $$ = $1 ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $6 ; }
             | when expr nseps THEN nseps statement THEN
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
             | when expr seps
                                       {  exiterror( ERR_THEN_EXPECTED, 2, parser_data.when_linenr, __reginatext )  ; }
             | when error              {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
             ;

when_or_other: when
             | otherwise
             ;

ex_when_stat : gruff when_or_other    {  exiterror( ERR_WHEN_UNEXPECTED, 0 ) ;}
             ;

otherwise_stat : otherwise nseps nxstats {
                                         $$ = $1 ;
                                         $$->p[0] = $3 ; }
             |                         { $$ = makenode(X_NO_OTHERWISE,0) ;
                                         $$->lineno = parser_data.tline ;
                                         $$->charnr = parser_data.tstart ; }
             ;


signal_stat : signal VALUE expr seps   { $$ = $1 ;
                                         $$->type = X_SIG_VAL ;
                                         $$->p[0] = $3 ; }
            | signal asymbol error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                             seps
            | signal asymbol seps      { $$ = $1 ;
                                         $$->name = (streng *)$2 ; }
            | signal on error { exiterror( ERR_INV_SUBKEYWORD, 3, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
                        seps
            | signal off error { exiterror( ERR_INV_SUBKEYWORD, 4, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
                         seps
            | signal on s_action error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                                 seps
            | signal on s_action namespec error { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
                                          seps
            | signal on s_action namespec seps
                                       { $$ = $1 ;
                                         $$->type = X_SIG_SET ;
                                         $$->p[0] = $2 ;
                                         $$->name = (streng *)$4 ;
                                         $$->p[1] = $3 ; }
            | signal on s_action seps
                                       { $$ = $1 ;
                                         $$->type = X_SIG_SET ;
                                         $$->p[0] = $2 ;
                                         $$->name = (streng *)$4 ;
                                         $$->p[1] = $3 ; }
            | signal off s_action error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                                  seps
            | signal off s_action seps { $$ = $1 ;
                                         $$->type = X_SIG_SET ;
                                         $$->p[0] = $2 ;
                                         $$->p[1] = $3 ; }
            ;

namespec    : NAME SIMSYMBOL           { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue);}
            | NAME error               { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
            ;

asymbol     : CONSYMBOL                { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
            | SIMSYMBOL                { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
            | error                    { exiterror( ERR_STRING_EXPECTED, 0 ) ;}
            ;

on          : ON                       { $$ = makenode(X_ON,0) ; }
            ;

off         : OFF                      { $$ = makenode(X_OFF,0) ; }
            ;

c_action    : ERROR                    { $$ = makenode(X_S_ERROR,0) ; }
            | HALT                     { $$ = makenode(X_S_HALT,0) ; }
            | NOTREADY                 { $$ = makenode(X_S_NOTREADY,0) ; }
            | FAILURE                  { $$ = makenode(X_S_FAILURE,0) ; }
            ;

s_action    : c_action                 { $$ = $1 ; }
            | NOVALUE                  { $$ = makenode(X_S_NOVALUE,0) ; }
            | SYNTAX                   { $$ = makenode(X_S_SYNTAX,0) ; }
            | LOSTDIGITS               { $$ = makenode(X_S_LOSTDIGITS,0) ; }

trace_stat  : trace VALUE expr seps    { $$ = $1 ;
                                         $$->p[0] = $3 ; }
            | trace expr seps          { $$ = $1 ;
                                         $$->p[0] = $2 ; }
            | trace whatever error { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
                             seps
            | trace whatever seps      { $$ = $1 ;
                                         $$->name = (streng *) $2 ; }
            ;

whatever    : WHATEVER                 { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
            ;


assignment  : ass_part nexpr seps      { $$ = $1 ;
                                         $$->p[1] = $2 ;
                                         if (($2) && gettypeof($$->p[1])==1)
                                            $$->type = X_NASSIGN ; }
            ;

ass_part    : ASSIGNMENTVARIABLE       { $$ = makenode(X_ASSIGN,0) ;
                                         $$->charnr = parser_data.tstart ;
                                         $$->lineno = parser_data.tline ;
                                         $$->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
            ;


expr        : '(' expr ')'             { $$ = $2 ; }
            | NOT expr                 { $$ = makenode(X_LOG_NOT,1,$2) ; }
            |      NOT                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "NOT" ) ;}
            | expr '+' expr            { $$ = makenode(X_PLUSS,2,$1,$3) ; }
            | expr '=' expr            { $$ = makenode(X_EQUAL,2,$1,$3) ;
                                         transform( $$ ) ; }
            |      '='                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "=" ) ;}
            | expr '-' expr            { $$ = makenode(X_MINUS,2,$1,$3) ; }
            | expr '*' expr            { $$ = makenode(X_MULT,2,$1,$3) ; }
            |      '*'                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "*" ) ;}
            | expr '/' expr            { $$ = makenode(X_DEVIDE,2,$1,$3) ; }
            |      '/'                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "/" ) ;}
            | expr '%' expr            { $$ = makenode(X_INTDIV,2,$1,$3) ; }
            |      '%'                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "%" ) ;}
            | expr '|' expr            { $$ = makenode(X_LOG_OR,2,$1,$3) ; }
            |      '|'                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "|" ) ;}
            | expr '&' expr            { $$ = makenode(X_LOG_AND,2,$1,$3) ; }
            |      '&'                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "&" ) ;}
            | expr XOR expr            { $$ = makenode(X_LOG_XOR,2,$1,$3) ; }
            |      XOR                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "&&" ) ;}
            | expr EXP expr            { $$ = makenode(X_EXP,2,$1,$3) ; }
            |      EXP                 {  exiterror( ERR_INVALID_EXPRESSION, 1, "**" ) ;}
            | expr SPACE expr          { $$ = makenode(X_SPACE,2,$1,$3) ; }
            |      SPACE               {  exiterror( ERR_INVALID_EXPRESSION, 1, " " ) ;}
            | expr GTE expr            { $$ = makenode(X_GTE,2,$1,$3) ;
                                         transform( $$ ) ; }
            |      GTE                 {  exiterror( ERR_INVALID_EXPRESSION, 1, ">=" ) ;}
            | expr LTE expr            { $$ = makenode(X_LTE,2,$1,$3) ;
                                         transform( $$ ) ; }
            |      GT                  {  exiterror( ERR_INVALID_EXPRESSION, 1, ">" ) ;}
            | expr GT expr             { $$ = makenode(X_GT,2,$1,$3) ;
                                         transform( $$ ) ; }
            | expr MODULUS expr        { $$ = makenode(X_MODULUS,2,$1,$3) ; }
            |      MODULUS             {  exiterror( ERR_INVALID_EXPRESSION, 1, "//" ) ;}
            | expr LT expr             { $$ = makenode(X_LT,2,$1,$3) ;
                                         transform( $$ ) ; }
            |      LT                  {  exiterror( ERR_INVALID_EXPRESSION, 1, "<" ) ;}
            | expr DIFFERENT expr      { $$ = makenode(X_DIFF,2,$1,$3) ;
                                         transform( $$ ) ; }
            |      DIFFERENT           {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | expr EQUALEQUAL expr     { $$ = makenode(X_S_EQUAL,2,$1,$3) ; }
            |      EQUALEQUAL          {  exiterror( ERR_INVALID_EXPRESSION, 1, "==" ) ;}
            | expr NOTEQUALEQUAL expr  { $$ = makenode(X_S_DIFF,2,$1,$3) ; }
            |      NOTEQUALEQUAL       {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | expr GTGT expr           { $$ = makenode(X_S_GT,2,$1,$3) ; }
            |      GTGT                {  exiterror( ERR_INVALID_EXPRESSION, 1, ">>" ) ;}
            | expr LTLT expr           { $$ = makenode(X_S_LT,2,$1,$3) ; }
            |      LTLT                {  exiterror( ERR_INVALID_EXPRESSION, 1, "<<" ) ;}
            | expr NOTGTGT expr        { $$ = makenode(X_S_NGT,2,$1,$3) ; }
            |      NOTGTGT             {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | expr NOTLTLT expr        { $$ = makenode(X_S_NLT,2,$1,$3) ; }
            |      NOTLTLT             {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | expr GTGTE expr          { $$ = makenode(X_S_GTE,2,$1,$3) ; }
            |      GTGTE               {  exiterror( ERR_INVALID_EXPRESSION, 1, ">>=" ) ;}
            | expr LTLTE expr          { $$ = makenode(X_S_LTE,2,$1,$3) ; }
            |      LTLTE               {  exiterror( ERR_INVALID_EXPRESSION, 1, "<<=" ) ;}
            | symbtree                 { $$ = $1 ; }
            | CONSYMBOL                { $$ = makenode(X_STRING,0) ;
                                         $$->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
            | HEXSTRING                { $$ = makenode(X_STRING,0) ;
                                         $$->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy($$->name->value,retvalue,
                                                    $$->name->len=retlength); }
            | BINSTRING                { $$ = makenode(X_STRING,0) ;
                                         $$->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy($$->name->value,retvalue,
                                                    $$->name->len=retlength); }
            | STRING                   { $$ = makenode(X_STRING,0) ;
                                         $$->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
            | function                 { $$ = $1 ; }
            | '+' expr %prec UPLUSS    { $$ = makenode(X_U_PLUSS,1,$2) ; }
            | '-' expr %prec UMINUS    { $$ = makenode(X_U_MINUS,1,$2) ; }
            | '+'      %prec SYNTOP    {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | '-'      %prec SYNTOP    {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | expr CONCATENATE expr    { $$ = makenode(X_CONCAT,2,$1,$3) ; }
            |      CONCATENATE         {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            | expr CCAT expr           { $$ = makenode(X_CONCAT,2,$1,$2) ; }
            |      CCAT                {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
            ;

symbtree    : SIMSYMBOL                { $$ = (nodeptr)create_head( (const char *)retvalue ) ; }
            ;


function    : extfunc  exprs ')'       { $$ = makenode(X_EX_FUNC,1,$2) ;
                                         $$->name = (streng *)$1 ; }
            | intfunc  exprs ')'       { $$ = makenode(X_IN_FUNC,1,$2) ;
                                         $$->name = (streng *)$1 ; }
            ;

intfunc     : INFUNCNAME               { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
            ;

extfunc     : EXFUNCNAME               { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
            ;

template    : pv solid template        { $$ =makenode(X_TPL_SOLID,3,$1,$2,$3);}
            | pv                       { $$ =makenode(X_TPL_SOLID,1,$1) ; }
            | error { exiterror( ERR_INVALID_TEMPLATE, 1, __reginatext ) ;}
              seps
            ;

solid       : '-' offset               { $$ = makenode(X_NEG_OFFS,0) ;
                                         $$->name = (streng *) $2 ; }
            | '+' offset               { $$ = makenode(X_POS_OFFS,0) ;
                                         $$->name = (streng *) $2 ; }
            | offset                   { $$ = makenode(X_ABS_OFFS,0) ;
                                         $$->name = (streng *) $1 ; }
            | '=' offset               { $$ = makenode(X_ABS_OFFS,0) ;
                                         $$->name = (streng *) $2 ; }
            | '(' symbtree ')'         { $$ = makenode(X_TPL_VAR,0) ;
                                         $$->p[0] = $2 ; }
            | '-' '(' symbtree ')'     { $$ = makenode(X_NEG_OFFS,0) ;
                                         $$->p[0] = $3 ; }
            | '+' '(' symbtree ')'     { $$ = makenode(X_POS_OFFS,0) ;
                                         $$->p[0] = $3 ; }
            | '=' '(' symbtree ')'     { $$ = makenode(X_ABS_OFFS,0) ;
                                         $$->p[0] = $3 ; }
            | string                   { $$ = makenode(X_TPL_MVE,0) ;
                                         $$->name = (streng *) $1 ; }
            ;

offset      : OFFSET                   { $$ = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
            | CONSYMBOL                { streng *sptr = Str_cre_TSD(parser_data.TSD,retvalue) ;
                                          if (myisnumber(sptr))
                                          {
                                             exiterror( ERR_INVALID_INTEGER, 4, sptr->value ) ;
                                          }
                                          else
                                             exiterror( ERR_INVALID_TEMPLATE, 0 ) ;}
            ;

string      : STRING                   { $$ = (nodeptr) Str_cre_TSD(parser_data.TSD,retvalue) ; }
            | HEXSTRING                { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         $$ = (nodeptr) sptr ; }
            | BINSTRING                { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         $$ = (nodeptr) sptr ; }
            ;

pv          : PLACEHOLDER pv           { $$ = makenode(X_TPL_POINT,1,$2) ; }
            | symbtree pv              { $$ = makenode(X_TPL_SYMBOL,1,$2) ;
                                         $$->p[1] = $1 ; }
            |                          { $$ = NULL ; }
            ;

exprs       : nexpr ',' exprs          { $$ = makenode(X_EXPRLIST,2,$1,$3) ;
                                         checkconst( $$ ) ; }

            | nexpr                    { $$ = makenode(X_EXPRLIST,1,$1) ;
                                         checkconst( $$ ) ; }
            ;

nexpr       : expr                     { $$ = $1 ; }
            |                          { $$ = NULL ; }
            ;

anyvars     : xsimsymb anyvars          { $$ = makenode(X_SIM_SYMBOL,1,$2) ;
                                         $$->name = (streng *) $1 ; }
            | xsimsymb                  { $$ = makenode(X_SIM_SYMBOL,0) ;
                                         $$->name = (streng *) $1 ; }
            | '(' xsimsymb ')' anyvars  { $$ = makenode(X_IND_SYMBOL,1,$4) ;
                                         $$->name = (streng *) $2 ; }
            | '(' xsimsymb ')'          { $$ = makenode(X_IND_SYMBOL,0) ;
                                         $$->name = (streng *) $2 ; }
            ;

xsimsymb    : SIMSYMBOL                { $$ = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
            ;

simsymb     : SIMSYMBOL                { $$ = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
            | error                    { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
            ;

%%

#if 0
static void checkin( nodeptr ptr )
{
   nodeptr *new ;
   tsd_t *TSD = parser_data.TSD;

   if (!narray)
   {
      narray = MallocTSD( sizeof(nodeptr)* 100 ) ;
      narmax = 100 ;
      narptr = 0 ;
   }

   if (narptr==narmax)
   {
      new = MallocTSD( sizeof(nodeptr)*narmax*3 ) ;
      memcpy( new, narray, sizeof(nodeptr)*narmax ) ;
      narmax *= 3 ;
      FreeTSD( narray ) ;
      narray = new ;
   }

   narray[narptr++] = ptr ;
}
#endif

static nodeptr makenode( int type, int numb, ... )
{
   nodeptr thisleave ;
   va_list argptr ;
   int i ;

   assert(numb <= sizeof(thisleave->p)/sizeof(thisleave->p[0]));
#ifdef REXXDEBUG
   printf("makenode: making new node, type: %d\n",type) ;
#endif /* REXXDEBUG */

   thisleave = FreshNode( ) ;
   /* thisleave is initialized to zero except for nodeindex */
   va_start( argptr, numb ) ;
   thisleave->type = type ;
   thisleave->lineno = -1 ;
   thisleave->charnr = -1 ;
   for (i=0;i<numb;i++)
      thisleave->p[i]=va_arg(argptr, nodeptr) ;

   va_end( argptr ) ;

   return( thisleave ) ;
}

static char *getdokeyword( int type )
{
   char *ptr;
   switch( type )
   {
      case X_DO_TO: ptr="TO";break;
      case X_DO_BY: ptr="BY";break;
      case X_DO_FOR: ptr="FOR";break;
      default: ptr="";break;
   }
   return ptr;
}

static void checkdosyntax( cnodeptr this )
{
   if ((this->p[1]!=NULL)&&(this->p[2]!=NULL))
   {
      if ((this->p[1]->type)==(this->p[2]->type))
      {
         exiterror( ERR_INVALID_DO_SYNTAX, 1, getdokeyword(this->p[1]->type) )  ;
      }
   }
   if ((this->p[2]!=NULL)&&(this->p[3]!=NULL))
   {
      if ((this->p[2]->type)==(this->p[3]->type))
      {
         exiterror( ERR_INVALID_DO_SYNTAX, 1, getdokeyword(this->p[2]->type) )  ;
      }
   }
   if ((this->p[1]!=NULL)&&(this->p[3]!=NULL))
   {
      if ((this->p[1]->type)==(this->p[3]->type))
      {
         exiterror( ERR_INVALID_DO_SYNTAX, 1, getdokeyword(this->p[1]->type) )  ;
      }
   }
   return ;
}


void newlabel( const tsd_t *TSD, internal_parser_type *ipt, nodeptr this )
{
   labelboxptr new ;

   assert( this ) ;

   new = MallocTSD(sizeof(labelbox)) ;

   new->next = NULL ;
   new->entry = this ;
   if (ipt->first_label == NULL)
   {
      ipt->first_label = new ;
      ipt->last_label = new ; /* must be NULL, too */
   }
   else
   {
      ipt->last_label->next = new ;
      ipt->last_label = new ;
   }
   ipt->numlabels++;
}

static nodeptr create_tail( const char *name )
{
   const char *cptr ;
   nodeptr node ;
   int constant ;
   streng *tname ;
   tsd_t *TSD = parser_data.TSD;

   if (!*name)
   {
      node = makenode( X_CTAIL_SYMBOL, 0 ) ;
      node->name = Str_make_TSD( parser_data.TSD, 0) ;
      return node ;
   }

   cptr = name ;
   constant = isdigit(*cptr) || *cptr=='.' || (!*cptr) ;
   node = makenode( (constant) ? X_CTAIL_SYMBOL : X_VTAIL_SYMBOL, 0 ) ;

   for (;*cptr && *cptr!='.'; cptr++) ;
   node->name = Str_ncre_TSD( parser_data.TSD, name, cptr-name ) ;

   if (*cptr)
   {
      node->p[0] = create_tail( ++cptr ) ;
      if (constant && node->p[0]->type==X_CTAIL_SYMBOL)
      {
         streng *first, *second ;
         nodeptr tptr ;

         first = node->name ;
         second = node->p[0]->name ;
         node->name = NULL;
         node->p[0]->name = NULL;
         tname = Str_makeTSD( first->len + second->len + 1) ;
         memcpy( tname->value, first->value, first->len ) ;
         tname->value[first->len] = '.' ;
         memcpy( tname->value+first->len+1, second->value, second->len) ;
         tname->len = first->len + second->len + 1 ;

         Free_stringTSD( first ) ;
         Free_stringTSD( second ) ;
         node->name = tname ;
         tptr = node->p[0] ;
         node->p[0] = tptr->p[0] ;
         RejectNode(tptr);
      }
   }

   return node ;
}

static nodeptr create_head( const char *name )
{
   const char *cptr ;
   nodeptr node ;

   for (cptr=name; *cptr && *cptr!='.'; cptr++) ;
   node = makenode( X_SIM_SYMBOL, 0 ) ;
   node->name = Str_ncre_TSD( parser_data.TSD, name, cptr-name+(*cptr=='.')) ;

   if (*cptr)
   {
      if (*(++cptr))
         node->p[0] = create_tail( cptr ) ;
      else
         node->p[0] = NULL ;

      node->type = (node->p[0]) ? X_HEAD_SYMBOL : X_STEM_SYMBOL ;
   }

   return node ;
}

#define IS_UNKNOWN   0
#define IS_A_NUMBER  1
#define IS_NO_NUMBER 2
#define IS_IRREG_NUMBER 10

static int gettypeof( nodeptr this )
{
   tsd_t *TSD = parser_data.TSD;

   switch(this->type)
   {
      case X_PLUSS:
      case X_MINUS:
      case X_MULT:
      case X_U_PLUSS:
      case X_U_MINUS:
      case X_DEVIDE:
      case X_INTDIV:
      case X_MODULUS:
      case X_EQUAL:
      case X_DIFF:
      case X_GTE:
      case X_GT:
      case X_LTE:
      case X_LT:
      case X_SEQUAL:
      case X_SDIFF:
      case X_SGTE:
      case X_SGT:
      case X_SLTE:
      case X_SLT:
      case X_NEQUAL:
      case X_NDIFF:
      case X_NGTE:
      case X_NGT:
      case X_NLTE:
      case X_NLT:
         return IS_A_NUMBER ;


      case X_SIM_SYMBOL:
         return 3 ;

      case X_HEAD_SYMBOL:
         return 4 ;

      case X_STRING:
      case X_CON_SYMBOL:
      {
         if (this->u.number)
            return IS_A_NUMBER ;

         this->u.number = is_a_descr( TSD, this->name ) ;
         if (this->u.number)
         {
            streng *stmp = str_norm( TSD, this->u.number, NULL ) ;
            if (Str_cmp(stmp,this->name))
            {
               Free_stringTSD( stmp ) ;
               return IS_UNKNOWN ;
            }
            Free_stringTSD( stmp ) ;
         }
         return (this->u.number) ? IS_A_NUMBER : IS_NO_NUMBER ;
      }
   }
   return IS_UNKNOWN ;
}



static void transform( nodeptr this )
{
   int left, rght, type ;

   left = gettypeof( this->p[0] ) ;
   rght = gettypeof( this->p[1] ) ;
   type = this->type ;

   if (left==1 && rght==1)
   {
      if (type==X_EQUAL)
         this->type = X_NEQUAL ;
      else if (type==X_DIFF)
         this->type = X_NDIFF ;
      else if (type==X_GTE)
         this->type = X_NGTE ;
      else if (type==X_GT)
         this->type = X_NGT ;
      else if (type==X_LTE)
         this->type = X_NLTE ;
      else if (type==X_LT)
         this->type = X_NLT ;
   }
   else if (left==2 || rght==2)
   {
      if (type==X_EQUAL)
         this->type = X_SEQUAL ;
      else if (type==X_DIFF)
         this->type = X_SDIFF ;
      else if (type==X_GTE)
         this->type = X_SGTE ;
      else if (type==X_GT)
         this->type = X_SGT ;
      else if (type==X_LTE)
         this->type = X_SLTE ;
      else if (type==X_LT)
         this->type = X_SLT ;
   }
   else
   {
      type = this->p[0]->type ;
      if (left==1 && (type==X_STRING || type==X_CON_SYMBOL))
         this->u.flags.lnum = 1 ;
      else if (left==3)
         this->u.flags.lsvar = 1 ;
      else if (left==4)
         this->u.flags.lcvar = 1 ;

      type = this->p[1]->type ;
      if (rght==1 && (type==X_STRING || type==X_CON_SYMBOL))
         this->u.flags.rnum = 1 ;
      else if (rght==3)
         this->u.flags.rsvar = 1 ;
      else if (rght==4)
         this->u.flags.rcvar = 1 ;
   }
}


static int is_const( cnodeptr this )
{
   if (!this)
      return 1 ;

   switch (this->type)
   {
      case X_STRING:
      case X_CON_SYMBOL:
         return 1 ;

      case X_U_PLUSS:
      case X_U_MINUS:
         return is_const( this->p[0] ) ;

      case X_PLUSS:
      case X_MINUS:
      case X_MULT:
/*    case X_DEVIDE: Bug 20000807-41821 */
      case X_INTDIV:
      case X_MODULUS:
      case X_EQUAL:
      case X_DIFF:
      case X_GTE:
      case X_GT:
      case X_LTE:
      case X_LT:
      case X_SEQUAL:
      case X_SDIFF:
      case X_SGTE:
      case X_SGT:
      case X_SLTE:
      case X_SLT:
      case X_NEQUAL:
      case X_NDIFF:
      case X_NGTE:
      case X_NGT:
      case X_NLTE:
      case X_NLT:
      case X_SPACE:
      case X_CONCAT:
         return is_const( this->p[0] ) && is_const( this->p[1] ) ;
   }
   return 0 ;
}


static void checkconst( nodeptr this )
{
   tsd_t *TSD = parser_data.TSD;

   assert( this->type == X_EXPRLIST ) ;
   if (is_const(this->p[0]))
   {
      if (this->p[0])
         this->u.strng = evaluate( TSD, this->p[0], NULL ) ;
      else
         this->u.strng = NULL ;

      this->type = X_CEXPRLIST ;
   }
}
