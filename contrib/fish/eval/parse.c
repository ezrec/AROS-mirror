/*
**
** PARSE.C     Divides an input string into tokens and evaluates an
**             expression.
**
** Originally written 5/89 in ANSI C
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


/* modes */
#define     M_UNARY     1
#define     M_BINARY    2
#define     M_FUNCTION  3

static double   last_value=0.0;
static int      base;

static int     is_binary     (char *s);
static int     is_unary      (char c);
static int     instr         (char c,char *s);
static BOOLEAN legal         (char *s);
static BOOLEAN is_digit      (char c);
static BOOLEAN is_exp        (char c);
static BOOLEAN get_token     (int mode,char *s,int *n,TOKENPTR t,VARPTR vlist,
                              VARPTR clist,char *newname);
static BOOLEAN assign_as_var (char *s,int *n,TOKENPTR t,VARPTR vlist,
                              VARPTR clist,char *newname);
static BOOLEAN assign_as_num (char *s,int *n,TOKENPTR t,int max,int base);
static BOOLEAN get_value     (char *s,VARPTR vlist,VARPTR clist,
                              double *ret_val);
static int     base_override (char *s,int *n);
static int     rank          (int operator);


static int is_binary(char *s)

    {
    if (s[0]=='<' && s[1]=='<')
        return(SHLEFT);
    if (s[0]=='>' && s[1]=='>')
        return(SHRIGHT);
    return(instr(s[0],BSTRING));
    }


static int is_unary(char c)

    {
    return(instr(c,USTRING));
    }


static int instr(char c,char *s)

    {
    int    i;

    for (i=0;s[i]!=EOS && s[i]!=c;i++);
    return(s[i]==EOS ? 0 : i+1);
    }


static BOOLEAN legal(char *s)

    {
    int     c;

    c=s[0];
    return(c!=EOS && c!='(' && c!=')' && c!=','
            && c!='=' && !isspace(c) && !is_binary(s));
    }


static BOOLEAN is_digit(char c)

    {
    c=tolower((int)c);
    if (base>10)
        return((c>='0' && c<='9') || (c>='a' && c<='a'+base-11));
    return(c>='0' && c<='0'+base-1);
    }


static BOOLEAN is_exp(char c)

    {
    return((base > 14) ? (c=='\\') : (c=='\\' || c=='e' || c=='E'));
    }

/*
** get_token(int mode,char *s,int *n,TOKENPTR t,VARPTR vlist,
**           VARPTR clist,char *newname)
**
** Figures out what the next token in the string s is, starting at (*n).
** (*n) is returned pointing to the part of the string just after the
** evaluated token.  vlist and clist are used to search through in case
** the token is a variable or constant.  newname is assigned if the token
** is a variable name not found in any of the lists.
**
** Returns 0 for end of string, 1 for valid token found
**
*/

static BOOLEAN get_token(int mode,char *s,int *n,TOKENPTR t,VARPTR vlist,
                         VARPTR clist,char *newname)

    {
    int     i,c,h;

    t->type=0;
    t->code=0;
    t->value=0;
    for (i=(*n);isspace(s[i]);i++);
    (*n)=i;
    if (s[i]==EOS)
        {
        (*n)=i;
        return(0);
        }
    t->type=0;
    if (mode==M_UNARY && is_unary(s[i]))
        {
        t->type=UNARY;
        t->code=is_unary(s[i]);
        (*n)=(*n)+1;
        return(1);
        }
    if (is_binary(&s[i]) && (mode!=M_UNARY || s[i]!='&'))
        {
        t->type=BINARY;
        t->code=is_binary(&s[i]);
        if (t->code==SHLEFT || t->code==SHRIGHT)
            (*n)=(*n)+1;
        }
    else
        {
        t->type=instr(s[i],"(),=");
        if (t->type>0)
                t->type+=LEFT_PAREN-1;
        }
    if (t->type!=0)
        {
        (*n)=(*n)+1;
        return(1);
        }
    if (s[i]=='!')
        return(assign_as_var(s,n,t,vlist,clist,newname));
    base=getibase();
    c=base_override(&s[i],&h);
    if (c)
        {
        base=c;
        i+=h;
        }
    if (!is_digit(s[i]) && s[i]!='.')
        return(assign_as_var(s,n,t,vlist,clist,newname));
    if (s[i]=='.' && !is_digit(s[i+1]))
        return(assign_as_var(s,n,t,vlist,clist,newname));
    for (;legal(&s[i]);i++)
        {
        if (s[i]=='.' || is_exp(s[i]))
            break;
        if (!is_digit(s[i]))
                return(assign_as_var(s,n,t,vlist,clist,newname));
        }
    if (!legal(&s[i]))
        {
        (*n)=(*n)+h;
        return(assign_as_num(s,n,t,i,base));
        }
    if (s[i]=='.')
        for (i++;legal(&s[i]);i++)
                {
                if (is_exp(s[i]))
                    break;
                if (!is_digit(s[i]))
                    return(assign_as_var(s,n,t,vlist,clist,newname));
                }
    if (!legal(&s[i]))
        {
        (*n)=(*n)+h;
        return(assign_as_num(s,n,t,i,base));
        }
    if (s[i+1]=='+' || s[i+1]=='-')
        i++;
    if (!is_digit(s[i+1]))
        return(assign_as_var(s,n,t,vlist,clist,newname));
    for (i++;legal(&s[i]);i++)
        {
        if (!is_digit(s[i]))
                return(assign_as_var(s,n,t,vlist,clist,newname));
        }
    (*n)=(*n)+h;
    return(assign_as_num(s,n,t,i,base));
    }


static BOOLEAN assign_as_var(char *s,int *n,TOKENPTR t,VARPTR vlist,
                             VARPTR clist,char *newname)

    {
    VAR     v;
    char    vname[MAXINPUT+1];
    int     l,i,j;

    i=(*n);
    l=(s[i]=='!');
    if (l)
        i++;
    for (j=0;legal(&s[i]) || (!j && s[i]=='&');i++,j++)
        vname[j]=s[i];
    if (!j && l)
        vname[j++]='!';
    (*n)=i;
    vname[j]=EOS;
    if (strlen(vname)>MAXNAMELEN)
        vname[MAXNAMELEN]=EOS;
    strcpy(newname,vname);
    if (vname[0]=='\"' && vname[1]==EOS)
        {
        t->value=last_value;
        t->type=QUOTE;
        return(1);
        }
    if (strlen(vname)<=MAXFLEN && (i=func_code(vname))!=0)
        {
        t->code=i;
        t->type=FUNCTION;
        }
    else
        {
        strcpy(v.name,vname);
        if (search_varlist(&v,clist,&i,MAXC))
                {
                t->code=i;
                t->type=CONSTANT;
                t->value=clist[i].value;
                }
        else
                {
                t->type=VARIABLE;
                if (search_varlist(&v,vlist,&i,MAXV))
                    {
                    t->code=i;
                    t->value=vlist[i].value;
                    }
                else
                    t->code=-1;
                }
        }
    return(1);
    }


static BOOLEAN assign_as_num(char *s,int *n,TOKENPTR t,int max,int base)

    {
    char    num[MAXINPUT+1];
    int     i,j;

    for (i=(*n),j=0;i<max;i++,j++)
        num[j]=s[i];
    (*n)=i;
    num[j]=EOS;
    t->type=NUMBER;
    t->value=asciiconv(base,num);
    return(1);
    }


/*
** evaluate(char *s,int showout,VARPTR vlist,VARPTR clist)
**
** Evaluate parses the input string looking for help queries or an "equals"
** sign that divides the string into an assignment variable and its
** assigned expression.  To evaluate the actual expression, get_value is
** called.
**
** Returns nothing
**
*/

void evaluate(char *s,int showout,VARPTR vlist,VARPTR clist)

    {
    char    varname[MAXINPUT+1];
    int     i,j,n,vn;
    VAR     x;
    TOKEN   t;
    int     checked,outbase,oldbase;
    char    bigbuf[MAXOUTLEN];


    oldbase=getobase();
    outbase=base_override(s,&j);
    checked=0;
    if (outbase && isspace(s[j]))
        {
        j++;
        checked=1;
        }
    else
        {
        j=0;
        outbase=oldbase;
        }
    for (i=j;s[i]!=EOS && s[i]!='=';i++);
    if (s[i]!=EOS)
        {
        strcpy(varname,&s[j]);
        varname[i++]=EOS;
        fixup(varname);
        n=0;
        j=get_token(M_UNARY,varname,&n,&t,vlist,clist,x.name);
        if (!j)
                {
                printf("There must be valid variable name on the left side of the '='.\n");
                return;
                }
        if (varname[n]!=EOS || t.type!=VARIABLE && t.type!=CONSTANT)
                {
                printf("\"%s\" is not a valid variable name.\n",varname);
                return;
                }
        if (t.type==CONSTANT)
                {
                printf("\"%s\" is a pre-assigned constant.  It cannot be reassigned.\n",x.name);
                return;
                }
        vn=(t.type==VARIABLE);
        }
    else
        {
        vn=0;
        i=j;
        }
    if (!checked)
        {
        outbase=base_override(&s[i],&j);
        if (outbase && isspace(s[i+j]))
            j++;
        else
            {
            j=0;
            outbase=oldbase;
            }
        i+=j;
        }
    if (!get_value(&s[i],vlist,clist,&x.value))
        return;
    last_value=x.value;
    if (outbase!=oldbase)
        setobase(outbase);
    baseconv(x.value,bigbuf);
    if (showout)
        printf("%s\n",bigbuf);
    if (outbase!=oldbase)
        setobase(oldbase);
    if (vn)
        {
        if (!insert_var(&x,vlist))
            printf("No more variables can be assigned!  Limit = %d.\n",MAXV);
        }
    }


/*
** get_value(char *s,VARPTR vlist,VARPTR clist)
**
** get_value evaluates an expression string by parsing it into tokens
** (using get_token) and converting the string of tokens to a table that
** contains tokens in reverse polish order.  table_value is then called
** to evaluate the final value of the reverse polish table of tokens.
** That value is returned by get_value.
**
*/

static BOOLEAN get_value(char *s,VARPTR vlist,VARPTR clist,double *ret_val)

    {
    char    nn[MAXNAMELEN+1];
    TOKEN   t,t2,t3;
    int     spos,mode;
    char    argcount[MAXINPUT+1];
    int     argptr;

    clear_stack();
    clear_table();
    spos=0;
    argptr=-1;
    mode=M_UNARY;
    while (get_token(mode,s,&spos,&t,vlist,clist,nn))
        {
        if (t.type==ILLEGAL)
            return(eerror("The '=' character is not allowed in expressions."));
        if (mode==M_UNARY && (t.type==BINARY || t.type==COMMA ||
                              t.type==RIGHT_PAREN))
            return(eerror("Operand expected in expression."));
        if (mode==M_BINARY && (t.type==UNARY || t.type==CONSTANT ||
                               t.type==VARIABLE || t.type==FUNCTION ||
                               t.type==NUMBER || t.type==LEFT_PAREN ||
                               t.type==QUOTE))
            return(eerror("Operator expected in expression."));
        if (t.type==VARIABLE && t.code<0)
            {
            printf("\"%s\" is an unassigned variable.\n",nn);
            return(0);
            }
        if (mode==M_FUNCTION && t.type!=LEFT_PAREN)
            return(eerror("Function names must be immediately followed "
                          "by a left parenthesis."));
        switch(t.type)
            {
            case NUMBER:
            case VARIABLE:
            case CONSTANT:
            case QUOTE:
                if (!add_token(&t))
                    return(0);
                mode=M_BINARY;
                break;
            case LEFT_PAREN:
                if (!push_token(&t))
                    return(0);
                mode=M_UNARY;
                break;
            case UNARY:
                if (!push_token(&t))
                    return(0);
                mode=M_UNARY;
                break;
            case BINARY:
                while (top_of_stack(&t2))
                    {
                    if (t2.type==BINARY && rank(t.code) > rank(t2.code))
                        break;
                    if (t2.type==LEFT_PAREN)
                        break;
                    pop_token(&t2);
                    if (!add_token(&t2))
                        return(0);
                    }
                if (!push_token(&t))
                    return(0);
                mode=M_UNARY;
                break;
            case FUNCTION:
                if (!push_token(&t))
                    return(0);
                argptr++;
                argcount[argptr]=0;
                mode=M_FUNCTION;
                break;
            case RIGHT_PAREN:
                while (pop_token(&t2) && t2.type!=LEFT_PAREN)
                    if (!add_token(&t2))
                        return(0);
                if (t2.type!=LEFT_PAREN)
                    return(eerror("Unmatched parentheses in expression."));
                if (top_of_stack(&t2) && t2.type==FUNCTION)
                    {
                    pop_token(&t2);
                    if (!add_token(&t2))
                        return(0);
                    if (func_nargs(t2.code)!=argcount[argptr]+1)
                        {
                        printf("Incorrect number of arguments "
                               "specified for function %s.\n",
                               func_name(t2.code));
                        return(0);
                        }
                    argptr--;
                    }
                mode=M_BINARY;
                break;
            case COMMA:
                while (pop_token(&t2) && t2.type!=LEFT_PAREN)
                    if (!add_token(&t2))
                        return(0);
                if (t2.type!=LEFT_PAREN || !top_of_stack(&t3) ||
                    t3.type!=FUNCTION)
                    return(eerror("Misplaced comma in expression."));
                if (!push_token(&t2))
                    return(0);
                mode=M_UNARY;
                argcount[argptr]++;
                break;
            }
        }
    if (mode==M_UNARY)
        return(eerror("Expression is improperly terminated."));
    while (pop_token(&t2))
        {
        if (t2.type==LEFT_PAREN)
            return(eerror("Unmatched parentheses in expression."));
        if (!add_token(&t2))
            return(0);
        }
    return(table_value(ret_val));
    }


BOOLEAN eerror(char *message)

    {
    printf("%s\n",message);
    return(0);
    }


static int rank(int operator)

    {
    switch (operator)
        {
        case POWER:
            return(10);
        case DIVIDE:
        case MULTIPLY:
        case MOD:
            return(9);
        case ADD:
        case SUBTRACT:
            return(8);
        case SHRIGHT:
        case SHLEFT:
            return(7);
        case AND:
            return(6);
        case XOR:
            return(5);
        case OR:
            return(4);
        }
    return(0);
    }


static int base_override(char *s,int *n)

    {
    int     base;
    int     c;

    (*n)=0;
    base=0;
    if (s[0]=='&')
        {
        if (s[1]!=EOS)
            {
            c=tolower((int)s[1]);
            if (c=='h' || c=='o' || c=='b' || c=='d')
                {
                base= c=='h' ? 16 : (c=='o' ? 8 :(c=='b' ? 2 : 10));
                (*n)=2;
                }
            }
        }
    else if (s[0]=='0' && (s[1]=='x' || s[1]=='X'))
        {
        base=16;
        (*n)=2;
        }
/* Assuming base 8 for a leading zero removed because of
   confusion with floating point numbers */
    else if (s[0]=='\\')
        {
        c=tolower((int)s[1]);
        if (c=='0' || (c>='2' && c<='9') || (c>='a' && c<='z'))
            {
            base= c=='0' ? 10:((c>='2'&&c<='9')?c-'0':c-'a'+11);
            (*n)=2;
            }
        }
    else if (s[0]=='$')
        {
        base=16;
        (*n)=1;
        }
    return(base);
    }


void tokcpy(TOKENPTR dest,TOKENPTR source)

    {
    dest->type=source->type;
    dest->code=source->code;
    dest->value=source->value;
    }
