/*
**
** ETABLE.C     Manipulates a table of tokens.
**
** Originally written 6/89 in ANSI C
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
**
*/

#include   "eval.h"

static  TOKEN   ttable[MAXINPUT+1];
static  int     ttp;

static BOOLEAN is_operator (int type);
#ifdef DEBUG
static void    print_table (void);
#endif

void clear_table(void)

    {
    ttp=0;
    }


BOOLEAN add_token(TOKENPTR t)

    {
    if (ttp>MAXINPUT)
        {
        printf("Out of expression table space.\n");
        return(0);
        }
    tokcpy(&ttable[ttp],t);
    ttp++;
    return(1);
    }


BOOLEAN table_value(double *ret_val)

    {
    int     i,j,k,nargs,max;
    double  args[MAXARGS];

    max=ttp;
    while (ttp>1)
        {
        for (i=0;i<max && !is_operator((int)ttable[i].type);i++);
        if (i==max)
            return(eerror("Expression table lacking in operators."));
        if (ttable[i].type==UNARY)
            nargs=1;
        else if (ttable[i].type==BINARY)
            nargs=2;
        else
            nargs=func_nargs(ttable[i].code);
        if (nargs>MAXARGS)
            return(eerror("Maximum number of function arguments exceeded."));
        for (j=i,k=nargs-1;k>=0;k--)
            {
            for (j--;j>=0 && ttable[j].type==VOID;j--);
            if (j<0)
                return(eerror("Expression table lacking in arguments."));
            args[k]=ttable[j].value;
            ttable[j].type=VOID;
            ttp--;
            }
        if (!result((int)ttable[i].type,ttable[i].code,args,&ttable[i].value))
            return(0);
        ttable[i].type=NUMBER;
        }
    if (ttp!=1)
        return(eerror("Expression table unexpectedly empty."));
    for (i=0;i<max && ttable[i].type==VOID;i++);
    if (i==max)
        return(eerror("Internal inconsistency error."));
    if (is_operator((int)ttable[i].type))
        return(eerror("Final expression table result is not a number."));
    (*ret_val)=ttable[i].value;
    return(1);
    }


BOOLEAN result(int type,int code,double *args,double *ret_val)

    {
    switch (type)
        {
        case BINARY:
            switch (code)
                {
                case ADD:
                    (*ret_val)=args[0]+args[1];
                    return(1);
                case SUBTRACT:
                    (*ret_val)=args[0]-args[1];
                    return(1);
                case DIVIDE:
                case MOD:
                    if (args[1]==0.)
                        return(eerror("Divide by zero in expression."));
                    if (code==DIVIDE)
                        (*ret_val)=args[0]/args[1];
                    else
                        (*ret_val)=args[1]*(args[0]/args[1]
                                                -(int)(args[0]/args[1]));
                    return(1);
                case MULTIPLY:
                    (*ret_val)=args[0]*args[1];
                    return(1);
                case POWER:
                    if (args[0]==0. && args[1]<0)
                        return(eerror("Cannot raise zero to a negative power."));
                    if (args[0]<0 && args[1]!=((int)args[1]))
                        return(eerror("Cannot raise a negative number to "
                                      "a non-integer power."));
                    (*ret_val)=pow(args[0],args[1]);
                    return(1);
                case OR:
                    (*ret_val)=or(args[0],args[1]);
                    return(1);
                case AND:
                    (*ret_val)=and(args[0],args[1]);
                    return(1);
                case XOR:
                    (*ret_val)=xor(args[0],args[1]);
                    return(1);
                case SHRIGHT:
                    (*ret_val)=args[0]*pow(2.,-args[1]);
                    return(1);
                case SHLEFT:
                    (*ret_val)=args[0]*pow(2.,args[1]);
                    return(1);
                }
            break;
        case UNARY:
            (*ret_val)=(code==POSITIVE) ? args[0] :
                       (code==NEGATIVE ? -args[0] : not(args[0]));
            return(1);
        case FUNCTION:
            return(func_eval(code,args,ret_val));
        }
    return(0);
    }


static BOOLEAN is_operator(int type)

    {
    return(type==BINARY || type==UNARY || type==FUNCTION);
    }


#ifdef DEBUG
static void print_table(void)

    {
    int     i;

    for (i=0;i<ttp;i++)
        {
        printf("(%d,",ttable[i].type);
        if (ttable[i].type==NUMBER || ttable[i].type==CONSTANT ||
            ttable[i].type==VARIABLE || ttable[i].type==QUOTE)
            printf("%g) ",ttable[i].value);
        else
            printf("%d) ",ttable[i].code);
        }
    printf("\n");
    }
#endif
