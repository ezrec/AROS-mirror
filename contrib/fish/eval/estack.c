/*
**
** ESTACK.C     Manipulates a stack of tokens.
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

static  TOKEN   tstack[MAXINPUT+1];
static  int     tsp;


void clear_stack(void)

    {
    tsp=0;
    }


BOOLEAN push_token(TOKENPTR t)

    {
    if (tsp>MAXINPUT)
        {
        printf("Out of expression stack space.\n");
        return(0);
        }
    tokcpy(&tstack[tsp],t);
    tsp++;
    return(1);
    }


BOOLEAN pop_token(TOKENPTR t)

    {
    if (!tsp)
        return(0);
    tokcpy(t,&tstack[--tsp]);
    return(1);
    }



BOOLEAN top_of_stack(TOKENPTR t)

    {
    if (!tsp)
        return(0);
    tokcpy(t,&tstack[tsp-1]);
    return(1);
    }
