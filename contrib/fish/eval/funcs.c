/*
**
** FUNCS.C     A module to be included with EVAL.C that handles
**             the manipulation of functions.
**
** Originally written 5/89 in ANSI C
**
** To add/change the functions in Eval:
** --------------------------------------------------------------
** 1. If the name of your function is longer than MAXFLEN, or the
**    argument list string is longer than MAXALEN, then increase the
**    size of these defines (MAXFLEN in eval.h, MAXALEN in funcs.c).
** 1. Update the value of NUMFUNCS in eval.h (total number of functions).
** 2. Add a #define for any new functions to the list of #defines
**    below.  You do not need to keep the list in alphabetic order.
** 3. Add the function ID (#define tag), name, argument list, and a
**    brief descripton to the flist[] array below.  Please keep the list
**    in alphabetic order by function name.
** 4. Add the actual function code to the func_eval function below.
**    Follow the examples of other functions.
** 5. If necessary, add the C-code for new math functions to emath.c,
**    and add prototypes for new C math functions to eval.h
**    NOTICE:  Read the disclaimer about adding code to emath.c in
**             the comments of emath.c.
** 6. Recompile ALL modules.
**
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

#include   "eval.h"

#define MAXALEN     3
#define MAXDLEN     80

typedef struct {
               int     id;
               char    name[MAXFLEN+1];
               char    argspec[MAXALEN+1];
               char    desc[MAXDLEN+1];
               } FUNC, *FUNCPTR;

#define    ABSVAL      1
#define    ACOS        2
#define    ACOSH       3
#define    ASIN        4
#define    ASINH       5
#define    ATAN        6
#define    ATAN2       7
#define    ATANH       8
#define    BESSI       9
#define    BESSJ       10
#define    BESSK       11
#define    CEIL        12
#define    COS         13
#define    COSH        14
#define    DBESSI      15
#define    DBESSJ      16
#define    DBESSK      17
#define    DJROOT      18
#define    EXP         19
#define    FACT        20
#define    FLOOR       21
#define    JROOT       22
#define    LN          23
#define    LOG         24
#define    SIGN        25
#define    SIN         26
#define    SINH        27
#define    SQRT        28
#define    TAN         29
#define    TANH        30

static FUNC    flist[NUMFUNCS] =
{
{ABSVAL,"abs",    "x",  "absolute value"                              },
{ACOS,  "acos",   "x",  "arccosine, return value in radians"          },
{ACOSH, "acosh",  "x",  "inverse hyperbolic cosine"                   },
{ASIN,  "asin",   "x",  "arcsine, return value in radians"            },
{ASINH, "asinh",  "x",  "inverse hyperbolic sine"                     },
{ATAN,  "atan",   "x",  "arctangent, return value in radians"         },
{ATAN2, "atan2",  "y,x","arctangent of y/x, return value in radians"  },
{ATANH, "atanh",  "x",  "inverse hyperbolic tangent"                  },
{BESSI, "bessi",  "m,x","bessel function Im(x)"                       },
{BESSJ, "bessj",  "m,x","bessel function Jm(x)"                       },
{BESSK, "bessk",  "m,x","bessel function Km(x)"                       },
{CEIL,  "ceil",   "x",  "round up"                                    },
{COS,   "cos",    "x",  "cosine, x in radians"                        },
{COSH,  "cosh",   "x",  "hyperbolic cosine"                           },
{DBESSI,"dbessi", "m,x","derivative of bessel function:  Im'(x)"      },
{DBESSJ,"dbessj", "m,x","derivative of bessel function:  Jm'(x)"      },
{DBESSK,"dbessk", "m,x","derivative of bessel function:  Km'(x)"      },
{DJROOT,"djroot", "m,n","nth non-zero root of Jm'(x)"                 },
{EXP,   "exp",    "x",  "e (2.718..) raised to the power of x"        },
{FACT,  "fact",   "n",  "factorial (n!)"                              },
{FLOOR, "floor",  "x",  "round down"                                  },
{JROOT, "jroot",  "m,n","nth non-zero root of Jm(x)"                  },
{LN,    "ln",     "x",  "natural logarithm (base e)"                  },
{LOG,   "log",    "x",  "logarithm to the base 10"                    },
{SIGN,  "sgn",    "x",  "-1 if x<0, 0 if x=0, +1 if x>0"              },
{SIN,   "sin",    "x",  "sine, x in radians"                          },
{SINH,  "sinh",   "x",  "hyperbolic sine"                             },
{SQRT,  "sqrt",   "x",  "squareroot"                                  },
{TAN,   "tan",    "x",  "tangent, x in radians"                       },
{TANH,  "tanh",   "x",  "hyperbolic tangent"                          }
};


static BOOLEAN func_error(int,int);
static int     maxfunclen(FUNCPTR);

/*
** print_funclist(FILE *s,char *input,int d)
**
** Prints a list of the functions
**
*/
int print_funclist(FILE *s,char *input,int d)

    {
    char    line[MAXALEN+MAXFLEN+MAXDLEN+10];
    int     i,j,k,l,n,m1,nc,cw,nr,li,c;

    m1=maxfunclen(flist)+2;
    cw=m1+2;
    if (d)
        nc=1;
    else
        {
        nc=(SCRWIDTH+3)/cw;
        if (nc<=0)
            nc=1;
        }
    nr=(NUMFUNCS+nc-1)/nc;
    for (i=0;i<nr;i++)
        {
        li=0;
        for (j=0;j<nc;j++)
            {
            k=i+j*nr;
            if (k>=NUMFUNCS)
                break;
            for (l=0;(c=flist[k].name[l])!=EOS;l++)
                line[li++]=c;
            line[li++]='(';
            for (l++,n=l;(c=flist[k].argspec[l-n])!=EOS;l++)
                line[li++]=c;
            line[li++]=')';
            for (l++;l<m1;l++)
                line[li++]=' ';
            if (d)
                {
                for (l=0;l<4;l++)
                    line[li++]=' ';
                line[li]=EOS;
                strcat(line,flist[k].desc);
                li=strlen(line);
                }
            else
                if (j<nc-1)
                    for (;l<cw;l++)
                        line[li++]=' ';
            }
        line[li]=EOS;
        printf("%s\n",line);
        if (s==stdin && i!=nr-1 && ((i+3)%NLINES)==0)
            {
            printf("\nPress <ENTER> for more...\n");
            printf("%s",PROMPT);
            input[0]=EOS;
            fgets(input,MAXINPUT,s);
            fixup(input);
            if (input[0]!=EOS)
                return(1);
            printf("\n");
            }
        }
    printf("\n");
    return(0);
    }


/*
** maxfunclen(FUNCPTR flist)
**
** returns the length of the longest function name plus arguments
** (used for printing)
**
*/

static int maxfunclen(FUNCPTR flist)

   {
   int     i,len,max;

   for (max=strlen(flist[0].name)+strlen(flist[0].argspec),i=1;i<NUMFUNCS;i++)
       if ((len=strlen(flist[i].name)+strlen(flist[i].argspec))>max)
           max=len;
   return(max);
   }


int func_code(char *s)

    {
    int    i,step,c,count;

    i=step=NUMFUNCS>>1;
    count=0;
    while (1)
        {
        if (step>1)
            step>>=1;
        else
            {
            count++;
            if (count>5)
                break;
            }
        if (!(c=strcmp(s,flist[i].name)))
            return(i+1);
        if (c>0)
                {
                if (i==NUMFUNCS-1)
                    break;
                i=i+step;
                continue;
                }
        if (i==0)
            break;
        i=i-step;
        }
    return(0);
    }



int func_nargs(int code)

    {
    return((int)((strlen(flist[code-1].argspec)+1)/2));
    }


char *func_name(int code)

    {
    return(flist[code-1].name);
    }


BOOLEAN func_eval(int code,double *args,double *ret_val)

    {
    switch (flist[code-1].id)
        {
        case ABSVAL:
            (*ret_val)=fabs(args[0]);
            return(1);
        case ACOS:
            if (args[0]<-1 || args[0]>1)
                return(func_error(code,0));
            (*ret_val)=acos(args[0]);
            return(1);
        case ACOSH:
            if (args[0]<1)
                return(func_error(code,0));
            (*ret_val)=acosh(args[0]);
            return(1);
        case ASIN:
            if (args[0]<-1 || args[0]>1)
                return(func_error(code,0));
            (*ret_val)=asin(args[0]);
            return(1);
        case ASINH:
            (*ret_val)=asinh(args[0]);
            return(1);
        case ATAN:
            (*ret_val)=atan(args[0]);
            return(1);
        case ATAN2:
            (*ret_val)=atan2(args[0],args[1]);
            return(1);
        case ATANH:
            if (args[0]<=-1 || args[0]>=1)
                return(func_error(code,0));
            (*ret_val)=atanh(args[0]);
            return(1);
        case BESSI:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            (*ret_val)=bessi((int)args[0],args[1]);
            return(1);
        case BESSJ:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            (*ret_val)=bessj((int)args[0],args[1]);
            return(1);
        case BESSK:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            (*ret_val)=bessk((int)args[0],args[1]);
            return(1);
        case CEIL:
            (*ret_val)=ceil(args[0]);
            return(1);
        case COS:
            (*ret_val)=cos(args[0]);
            return(1);
        case COSH:
            (*ret_val)=cosh(args[0]);
            return(1);
        case DBESSI:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            (*ret_val)=dbessi((int)args[0],args[1]);
            return(1);
        case DBESSJ:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            (*ret_val)=dbessj((int)args[0],args[1]);
            return(1);
        case DBESSK:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            (*ret_val)=dbessk((int)args[0],args[1]);
            return(1);
        case DJROOT:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            if (args[1]!=((int)args[1]) || args[1]<1)
                return(func_error(code,2));
            (*ret_val)=djroot((int)args[0],(int)args[1]);
            return(1);
        case EXP:
            (*ret_val)=exp(args[0]);
            return(1);
        case FACT:
            if (args[0]<0)
                return(func_error(code,5));
            if (args[0]!=((int)args[0]))
                return(func_error(code,3));
            (*ret_val)=factorial((int)args[0]);
            return(1);
        case FLOOR:
            (*ret_val)=floor(args[0]);
            return(1);
        case JROOT:
            if (args[0]!=((int)args[0]))
                return(func_error(code,1));
            if (args[1]!=((int)args[1]) || args[1]<1)
                return(func_error(code,2));
            (*ret_val)=jroot((int)args[0],(int)args[1]);
            return(1);
        case LN:
            if (args[0]<=0)
                return(func_error(code,4));
            (*ret_val)=log(args[0]);
            return(1);
        case LOG:
            if (args[0]<=0)
                return(func_error(code,4));
            (*ret_val)=log10(args[0]);
            return(1);
        case SIGN:
            (*ret_val)=SGN(args[0]);
            return(1);
        case SIN:
            (*ret_val)=sin(args[0]);
            return(1);
        case SINH:
            (*ret_val)=sinh(args[0]);
            return(1);
        case SQRT:
            if (args[0]<0)
                return(func_error(code,5));
            (*ret_val)=sqrt(args[0]);
            return(1);
        case TAN:
            (*ret_val)=tan(args[0]);
            return(1);
        case TANH:
            (*ret_val)=tanh(args[0]);
            return(1);
        }
    printf("Unknown function code.\n");
    return(0);
    }


static BOOLEAN func_error(int code,int error)

    {
    static char *error_code[] = {
                "argument out of range",
                "first argument must be an integer",
                "second argument must be a positive integer",
                "argument must be an integer",
                "argument must be positive",
                "argument must be non-negative",
                };
    printf("%s:  %s.\n",flist[code-1].name,error_code[error]);
    return(0);
    }
