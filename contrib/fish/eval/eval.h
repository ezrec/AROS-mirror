/*
**
** EVAL.H  Master include file for Eval.
**         Contains function definitions and global definitions
**         and typecasts
**
** originally written 5/89
**
** Eval is a floating point expression evaluator.
** This is version 1.12
** Copyright (C) 1993  Will Menninger
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or any
** later version.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 675 Mass Ave, Cambridge, MA 02139, USA.
**
** The author until 9/93 can be contacted at:
** e-mail:     willus@ilm.pfc.mit.edu
** U.S. mail:  Will Menninger, 45 River St., #2, Boston, MA 02108-1124
**
*/

/* Global includes */

#include   <stdio.h>
#include   <math.h>
#include   <string.h>
#include   <ctype.h>
#include   <stdlib.h>
#include   <float.h>

/* For math functions */
#define    SGN(x)      ((x)<0 ? -1 : ((x)>0 ? 1 : 0))
#define    ABS(x)      ((x)<0 ? -(x) : (x))
#define    SWAP(a,b)   {float temp=(a);(a)=(b);(b)=temp;}


/* Global definitions */
#define    VERSION     "1.12"
#define    TRUE        1
#define    FALSE       0
#define    MAXC        56
#define    MAXV        100
#define    MAXNAMELEN  16
#define    MAXINPUT    160
#define    NUMFUNCS    30
#define    MAXFLEN     6
#define    SCRWIDTH    80
#define    MAXARGS     10
#define    EOS          '\0'
#define    NLINES      21
#define    PROMPT      "Ex>"

#define    MAXOUTLEN   1100
/* I removed the original definition of MAXOUTLEN because it was
   too big on some compilers */
/*
#define    MAXOUTLEN   (DBL_MAX_EXP+100)
*/

/* types of tokens */
#define    BINARY      1
#define    UNARY       2
#define    FUNCTION    3
#define    VARIABLE    4
#define    NUMBER      5
#define    LEFT_PAREN  6
#define    RIGHT_PAREN 7
#define    COMMA       8
#define    ILLEGAL     9
#define    CONSTANT    10
#define    VOID        11
#define    QUOTE       12

/* binary operators */
#define    ADD         1
#define    SUBTRACT    2
#define    MULTIPLY    3
#define    DIVIDE      4
#define    POWER       5
#define    AND         6
#define    OR          7
#define    XOR         8
#define    MOD         9
#define    SHLEFT      10
#define    SHRIGHT     11
#define    BSTRING     "+-*/^&|?%"

/* unary operators */
#define    POSITIVE    1
#define    NEGATIVE    2
#define    NOT         3
#define    USTRING     "+-~"


/* Global typecasts */

typedef int    BOOLEAN;
typedef struct {
               char    name[MAXNAMELEN+1];
               double  value;
               } VAR, *VARPTR;
typedef struct {
               double  value;
               char    type;
               int     code;
               } TOKEN, *TOKENPTR;

/* Function definitions */

/* ----- EVAL.C functions ---------------------------------------------- */

BOOLEAN insert_var      (VARPTR new,VARPTR vlist);
BOOLEAN search_varlist  (VARPTR var,VARPTR vlist,int *n,int max);
void    fixup           (char *s);

/* ----- FUNCS.C functions --------------------------------------------- */

int     func_code       (char *);
BOOLEAN func_eval       (int,double *,double *);
char   *func_name       (int);
int     func_nargs      (int);
int     print_funclist  (FILE *s,char *input,int d);

/* ----- PARSE.C functions -------------------------------------------- */

void    evaluate        (char *,int,VARPTR,VARPTR);
BOOLEAN eerror          (char *message);
void    tokcpy          (TOKENPTR dest,TOKENPTR source);

/* ----- ESTACK.C functions ------------------------------------------- */

void    clear_stack     (void);
BOOLEAN pop_token       (TOKENPTR);
BOOLEAN push_token      (TOKENPTR);
BOOLEAN top_of_stack    (TOKENPTR);

/* ----- ETABLE.C functions ------------------------------------------- */

BOOLEAN add_token       (TOKENPTR);
void    clear_table     (void);
BOOLEAN result          (int,int,double *,double *);
BOOLEAN table_value     (double *);

/* ----- BASE.C functions --------------------------------------------- */

void    baseconv        (double val,char *buf);
void    set_scinote     (int val);
void    set_sigfig      (int val);
void    set_dplace      (int val);
void    set_maxexp      (int val);
void    set_fix         (int val);
void    setobase        (int base);
void    setibase        (int base);
int     getobase        (void);
int     getibase        (void);
int     get_scinote     (void);
int     get_fix         (void);
int     precision       (int base);
void    print_outtype   (void);
double  asciiconv       (int base,char *s);

/* ----- BITWISE.C functions --------------------------------------------- */

double  not             (double val);
double  or              (double val1,double val2);
double  and             (double val1,double val2);
double  xor             (double val1,double val2);

/* ----- EMATH.C functions ----------------------------------------------- */

double  bessi           (int,double);
double  bessj           (int,double);
double  bessk           (int,double);
double  dbessi          (int,double);
double  dbessj          (int,double);
double  dbessk          (int,double);
double  jroot           (int,int);
double  djroot          (int,int);
double  factorial       (int);
double  root            (double(*F)(double),double,double,double,int *);
double  root2           (double(*F)(int,double),int,double,double,double,
                         int *);
double  atanh           (double x);
double  asinh           (double x);
double  acosh           (double x);
