/******************************************************************************

    MODUL
	math.c

    DESCRIPTION
	Simple Math commands for DME/XDME (integer only)
	Variable swapping    for DME/XDME

    NOTES
	-/-

    BUGS
	<none known>

    TODO
	erweitertes Math-interfaces (flexibler, mehr funktionen)

    EXAMPLES
	[ $a == 10 $b == 11 ]
	inc a			[-> $a == 11]
	add b ( 3 * 4 + $b )    [-> $b == 34]

    SEE ALSO
	vars.c

    INDEX

    HISTORY
	<see RCS-File>

*!***************************************************************************
*!
*!  integer MATH Functions
*!
******************************************************************************/

/*
**  (C)Copyright 1992 by Bernd Noll for null/zero-soft
**  All Rights Reserved
**
**  RCS Header: $Id$
**
**
*/

/**************************************
		Includes
**************************************/
#include "defs.h"
#ifdef PATCH_NULL
#include "COM.h"
#include "libs/AUTO.h"
#endif

/**************************************
	    Globale Exports
**************************************/
//Prototype char    MathInfix;
Prototype void	  do_unnamedmathfunc(void);
Prototype void	  do_namedmathfunc  (void);
Prototype void	  do_infixmode	    (void);
Prototype void	  do_swapV	    (void);


/**************************************
      Interne Defines & Strukturen
**************************************/
/* --- math-function-modi if math_func_helper doesn't use function-parameters */

#define MODE_ADD simplehash('a','d')  /* x = x+y (2) */
#define MODE_DEC simplehash('d','e')  /* x = x-1 (1) */
#define MODE_DIV simplehash('d','i')  /* x = x/y (2) */
#define MODE_INC simplehash('i','n')  /* x = x+1 (1) */
#define MODE_MOD simplehash('m','o')  /* x = x%y (2) */
#define MODE_MUL simplehash('m','u')  /* x = x*y (2) */
#define MODE_NEG simplehash('n','e')  /* x = -x  (1) */
#define MODE_NOT simplehash('n','o')  /* x = ~x  (1) */
#define MODE_SUB simplehash('s','u')  /* x = x-y (2) */

#ifndef MAXIA
#define MAXIA 5
#endif

#define OP_SUB 1  /* the values are used for priorities, too */
#define OP_ADD 2
#define OP_DIV 3
#define OP_MOD 4
#define OP_MUL 5

/**************************************
	   Interne Prototypes
**************************************/


/**************************************
	   Impementation
**************************************/


/*
*!
*!  math functions offer the user a simple calculator,
*!  which recognizes +,-,*,/,%,(,);
*!  2 numbers must be separated with one operator,
*!  !!each operator must be surrounded with whitespaces!!
*!  (there are no unique +/- operators allowed, if they are
*!   not part of a number, so "-1" is allowed, "(- 1)" is not!)
*!
*!  so each expression has an inequal number of subexpressions -
*!  x+1 values and x operators with x in N
*!  there should not be more than MAXIA (== 5)
*!  expressions on one level (simply use brackets)
*!  ( that is in fact no problem, as on any evaluation
*!    we try to shorten our results due to operator prorities,
*!    and as long as there are only 4 operators used, we do
*!    normally not run out of space
*!   e.g.:> set abc 0 add abc (1 * 2 + 3 * 4 / 5 - 6) title $abc
*!	=> 1	  => 1*    => 1*2   => 1*2+   [-> 2+]
*!	=> 2+3	  => 2+3*  => 2+3*4 => 2+3*4/ [-> 2+12/]
*!	=> 2+12/5 => 2+12/5- [-> 2+2-] [-> 4-] => 4-6
*!	== -2
*!
*/

static
long domathop (long a1, char op, long a2)
{
    switch (op)
    {
    case OP_ADD:
	return (a1 + a2);
    case OP_SUB:
	return (a1 - a2);
    case OP_MUL:
	return (a1 * a2);
    case OP_DIV:
	return (a1 / a2);
    case OP_MOD:
	return (a1 % a2);
    default:
	error  ("undefined mathoperator");
	return (0);
    } /* switch */
} /* domathop */


static
char getmathop (char * str)
{
    if (!str)
    {
	return(0);
    } /* if */

    switch (*str)
    {
    case '+':
	return (OP_ADD);
    case '-':
	return (OP_SUB);
    case '*':
	return (OP_MUL);
    case '/':
	return (OP_DIV);
    case '%':
	return (OP_MOD);
    default:
	error ("unknown operator\n%s", str);
	return(0);
    } /* switch */
} /* getmathop */

/* static */
long getmathresult (char* expr)
{
    long   values[MAXIA];
    char   funcs [MAXIA];
    char * str = expr;
    int    i;
    char * checker;
    char * aux2  = NULL;
    char * dummy = NULL;
    char   quote = 0;

    /* --------- erstes argument muss immer existieren */
    if (!expr)
    {
	error ("empty expression\n(input NIL)");
	return(0);
    } /* if */

    if (is_number(expr))
    {
	return(atoi(expr));
    } /* if */

    checker = (UBYTE *)breakout(&str, &quote, &aux2);
    if (!checker)
    {
#if 0
	if (aux2) { free (aux2); aux2 = NULL; } /* if */ /* seems senseless */
#endif
	error ("empty expression\n(breakout NIL)");
	return(0);
    } /* if */

    /* PATCH_NULL [07 Apr 1993] : BUGFIX >>> */
    /* now we are testing : 1. result word starts w/ alpha, 2. word was NOT multiword (AND is nonnumber), 3. word is NOT quoted (AND is not number)  */
    if (isalpha(checker[0]) || ((str == NULL || *str == 0) && (quote == 0))) { /* should be better : check up to the first space, if there is a non-digit */
	if (aux2)
	    free(aux2);
	error ("non-number to math-function");
	return (0);
    } /* if */
    /* PATCH_NULL [07 Apr 1993] : BUGFIX <<< */

    values[0] = getmathresult ( checker );
    if (aux2)
    {
	free (aux2);
	aux2 = NULL;
    } /* if */

    i = 0;

    while (!GETF_ABORTCOMMAND(Ep) && str && ( 1/* first checking here */ ))
    {

	/* --------- operator holen  - better put that line into the while-condition */
	checker = (UBYTE *)breakout(&str, (void*)&dummy, &aux2);
	if (!checker)
	{
#if 0
	    if (aux2) { free (aux2); aux2 = NULL; } /* if */ /* seems senseless */
#endif

	    goto ende;
	} else
	{

	    funcs[i] = getmathop(checker);
	    if (aux2)
	    {
		free (aux2);
		aux2 = NULL;
	    } /* if */

	    if (GETF_ABORTCOMMAND(Ep))
	    {
/* printf ("abort unbekannt\n"); */
		return (0);
	    } /* if (aborted) */

	    /* --------- wenn möglich, verkuerzen */
	    while (i>0 && funcs[i-1] >= funcs[i])
	    {
		values[i-1] = domathop(values[i-1], funcs[i-1], values[i]);
		funcs[i-1]  = funcs[i];
		i--;
	    } /* while */

	    i++;
	    if (i >= MAXIA)
	    {
		error ("Expression too complex:\n%s", expr);
		return (0);
	    } /* if */

	    /* --------- naechstes argument holen */
	    checker   = (UBYTE *)breakout(&str, (void*)&dummy, &aux2);

	    values[i] = getmathresult(checker);
	    if (aux2)
	    {
		free (aux2);
	    } /* if */
	    if (GETF_ABORTCOMMAND(Ep))
	    {
		return (0);
	    } /* if */
	} /* if ex operator */
    } /* while not end of expr */

ende:

    while (i > 0)
    {
	values[i-1] = domathop(values[i-1], funcs[i-1], values[i]);
	i--;
    } /* while */

/* printf("ok %ld\n", values[0]); */

    return (values[0]);
} /* getmathresult */



/*
**  math_hash()
**	Calculate the right math-function defined by a string
**
*/

static
int math_hash (char * str)
{
    switch (str[1]) { /* PATCH_NULL [04 Aug 1993] : BUGFIX */
	case '=':
	    switch (str[0])
	    {
		case '+':
		    return(MODE_ADD);
		case '-':
		    return(MODE_SUB);
		case '*':
		    return(MODE_MUL);
		case '/':
		    return(MODE_DIV);
		case '%':
		    return(MODE_MOD);
		case '~':
		case '^':
		    return(MODE_NOT);   /* nun ja minus statt NOT ... */
	    } /* switch */
	case '-':
	    return(MODE_DEC);
	case '+':
	    return(MODE_INC);
	default:
	    return(simplehash(str[0],str[1]));
    } /* if */
} /* math_hash */



/*
*!  MATH
*!
*!  math-commands, they are all implemented in do_mathfunc
*!
*!  Very simple syntax:
*!
*!  >[MATH1] NEG / INC / DEC / NOT	 <varname>
*!  >[MATH2] ADD / SUB / MUL / DIV / MOD <varname> (<value>)
*!
*!	<value>   is a valid dezimal number
*!	<varname> must be given at each function and must be the name
*!		  of an existing variable containing a valid number
*!
*!  (a simple hack to do some increment etc. without arexx)
*!
*!  (due to less feedback, all the math commands are planned
*!  to be replaced with one single command MATH name value)
*!
*/

static
void do_mathfunc (char* mmode, char* vname, char* mval)
{
    int   mathfunc;
    char  str[16];
    long  l1;
    long  l2;
    char* sstr;
    int   type;

    type = VAR_NEX;		/* initialise */
    str[15] = '\0';
    l2 = 0;
    /* l1 = 0; */

    /* --- get the right operation mode */
    mathfunc = math_hash(mmode);   /* recognize the mathfunction via mmode */

    /* --- if necessary and existing, get the 2nd argument */
    if ((mathfunc != MODE_NOT)&& (mathfunc != MODE_NEG)&&
	(mathfunc != MODE_INC)&& (mathfunc != MODE_DEC))
	{
			/* called with 2 arguments => read a number from slot 2 */
	if (mval != NULL)
	{
	    if (is_number(mval))
	    {
		l2 = atol(mval);
	    } else
	    {
/* Abortcommand = 0; */
/* error ("%s:\ninvalid 2nd argument %s", av[0], mval); */
		l2 = getmathresult (mval);
		if (GETF_ABORTCOMMAND(Ep))
		{
		    return;
		} /* if */
/* abort2(); */
	    } /* if */
	} else
	{
	    error ("Math:\nMissing 2nd Operand!");
	    abort2();
	} /* if */
    } /* if */

    /* --- try to get the type and contents of the variable represented by the 1st argument */
    if (vname != NULL) {        /* get the number in the variable whose name is in slot 1 */
	sstr = GetTypedVar(vname,&type);
	if ((type != VAR_NEX) && is_number(sstr))
	{
	    l1 = atol(sstr);
	    if (sstr)
		free(sstr);
	} else
	{
	    if (sstr)
		free(sstr);
	    error ("%s:\ninvalid 1st argument %s", av[0], vname);
	    abort2();
	} /* if */
    } else
    {
	error ("%s:\nmissing 1st argument %s", av[0], vname);
	abort2();
    } /* if */

    /* --- do the specified math operation */
    switch (mathfunc) {         /* apply the math-function to the numbers */
				/* it would be finer to use a function-variable  - next time perhaps */
	case MODE_NEG:
	    l2 = -l1;
	    break;
	case MODE_NOT:
	    l2 = ~l1;
	    break;
	case MODE_INC:
	    l2 = l1+1;
	    break;
	case MODE_DEC:
	    l2 = l1-1;
	    break;
	case MODE_ADD:
	    l2 = l1+l2;
	    break;
	case MODE_SUB:
	    l2 = l1-l2;
	    break;
	case MODE_MUL:
	    l2 = l1*l2;
	    break;
	case MODE_DIV:
	    if (l2!=0)
	    {
		l2 = l1/l2;
	    } else
	    {
		error ("Math:\nDivision by Zero!");
		abort2();
	    } /* if */
	    break;
	case MODE_MOD:
	    if (l2!=0)
	    {
		l2 = l1%l2;
	    } else
	    {
		error ("Math:\nDivision by Zero!");
		abort2();
	    } /* if */
	    break;
	default:
	    error ("Math:\nUnknown Operator!");
	    abort2();
    } /* switch */

    if ((type == VAR_TF) || (type == VAR_GF) || (type == VAR_SF) || (type == VAR_PF)) /* if dest is flag change to boolean */
	l2 = (l2 != 0);

    /* --- write back the value , if necessary */
    if ((l2 != l1)) {   /* the function has changed a value => put it back to its variable */
	sprintf(str,"%ld",l2);

	SetTypedVar(vname, str, type);
    } /* if */

} /* do_mathfunc */



/*
*!  SMATH Interface:
*!
*!  consisting of 8 short mathfuncs:
*!
*!  >INC/ DEC/ NEG/ NOT       x
*!  >ADD/ SUB/ MUL/ DIV/ MOD  x y
*!
*!  which expect 1 or 2 arguments
*!  their function is recognized with their name
*!  since they all use the same routines.
*!
*!  (in fact these functions are abbreviations of MATH?)
*!
*/

void do_namedmathfunc (void)
{
    do_mathfunc(av[0], av[1], av[2]);
} /* do_namedmathfunc */


/*
*!  LMATH Interface:
*!
*!  2 long math-functions:
*!
*! >MATH1 x y
*! >MATH2 x y z
*!
*!  which expect 2 or 3 arguments
*!
*!  If You use the Toggle INFIX
*!  the second argument defines the mathematical function
*!  else the first arg does
*!
*!
*! >INFIX bool
*!
*!  define the argument order of the MATH? commands
*!  with INFIX OFF we say
*!   % MATH? operator variable [value]
*!  with INFIX ON we use
*!   % MATH? variable operator [value]
*!
*/

// char MathInfix = 0;

void do_unnamedmathfunc (void)
{
    if (MathInfix)
	do_mathfunc(av[2], av[1], av[3]); /* infix notation */
    else
	do_mathfunc(av[1], av[2], av[3]); /* prefix notation */
} /* do_unnamedmathfunc */



void do_infixmode (void)
{
    MathInfix = test_arg (av[1], MathInfix);
    if (MathInfix)
    {
	title ("Math Infixmode ON");
    } else
    {
	title ("Math Infixmode OFF");
    } /* if */
} /* do_infixmode */


/*
*! >SWAP var1 var2
*!
*!  Swap the contents of 2 variables
*!
*!  the command should recognize the type of the 2 variables and
*!  use the appropriate set-funcions
*!
*/

void do_swapV (void)
{
    char*   value[2];
    int     type[2];
    int     i;

    /* --- read values */
    for (i = 0; i < 2; i++)
    {
	if (!(value[i] = GetTypedVar(av[1+i], &type[i])))
	{
	    error ("%s:\nUnknown Variable '%s'!", av[0], av[1+i]);
	    goto abortion;
	} /* if */
    } /* for */

#ifdef N_DEF
    /* --- check compability */
    for (i = 0; i < 2; i++)
    {
	switch (type[i])
	{
	    case VAR_TF:
	    case VAR_PF:
	    case VAR_GF:
	    case VAR_SF:
		dummy = toggler(1,value[1-i]);
		break;
	    case VAR_NEX:
		error ("%s:\nUnknown Variable '%s'!", av[0], av[1+i]);
		goto abortion;
	} /* switch */
    } /* for */

    if (aborted)
    {
	/* error ("%s:\nIncompatible values for\n'%s' and '%s'!", av[0], av[1], av[2]); */
	goto abortion;
    }
#endif /*  */


    /* --- Write & free values */
    for (i = 0; i < 2; i++)
    {
	SetTypedVar(av[2-i], value[i], type[1-i]);
	free (value[i]);
    } /* for */

    return;

abortion:
    free(value[0]);
    free(value[1]);
    abort2();
} /* do_swapV */


#ifdef PATCH_NULL /* PATCH_NULL < 24-06-94 */

DEFUSERCMD("math1", 2, CF_VWM|CF_ICO|CF_COK, void, do_unnamedmathfunc, (void),)
DEFUSERCMD("math2", 3, CF_VWM|CF_ICO|CF_COK, void, do_unnamedmathfunc, (void),)
DEFUSERCMD("infixmode", 1, CF_VWM|CF_ICO|CF_COK, void, do_infixmode, (void),)
DEFUSERCMD("inc", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("sub", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("add", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("dec", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("div", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("mod", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("mul", 2, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("not", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("neg", 1, CF_VWM|CF_ICO|CF_COK, void, do_namedmathfunc, (void),)
DEFUSERCMD("swapv", 2, CF_VWM|CF_ICO|CF_COK, void, do_swapV, (void),)
/*DEFHELP #cmd math MATH1 arg1 arg2 - long version for NOT INC NEG DEC; $INFIXMODE decides if arg1 or arg2 is operator, the other arg is variablename */
/*DEFHELP #cmd math MATH2 arg1 arg2 arg3 - long version for MUL MOD DIV SUB ADD; $INFIXMODE decides if arg1 or arg2 is operator, the other arg is variablename */
/*DEFHELP #cmd vars,math INC varname - increment the named variable */
/*DEFHELP #cmd vars,math DEC varname - decrement the named variable */
/*DEFHELP #cmd vars,math NEG varname - make the named variable negative */
/*DEFHELP #cmd vars,math NOT varname - invert the named variable */
/*DEFHELP #cmd vars,math ADD varname value - increase the named variable by the given value */
/*DEFHELP #cmd vars,math SUB varname value - decrease the named variable by the given value */
/*DEFHELP #cmd vars,math MUL varname value - multiply the named variable with the given value */
/*DEFHELP #cmd vars,math MOD varname value - modulo divide the named variable by the given value */
/*DEFHELP #cmd vars,math DIV varname value - divide the named variable by the given value */
/*DEFHELP #cmd vars SWAPV varname1 varname2 - try to swap the contents of 2 variables */

#ifdef STATIC_COM /* PATCH_NULL < 24-06-94 */
    COMMAND("math1",          2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_unnamedmathfunc  )
    COMMAND("math2",          3, CF_VWM|CF_ICO|CF_COK, (FPTR)do_unnamedmathfunc  )
    COMMAND("infixmode",      1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_infixmode        )
    COMMAND("inc",            1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("sub",            2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("add",            2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("dec",            1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("div",            2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("mod",            2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("mul",            2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("not",            1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("neg",            1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_namedmathfunc    )
    COMMAND("swapv",          2, CF_VWM|CF_ICO|CF_COK, (FPTR)do_swapV,           )

    DEFFLAG( 93-04-09, MathInfix, 0 )

#endif
#endif


/******************************************************************************
*****  ENDE math.c
******************************************************************************/

