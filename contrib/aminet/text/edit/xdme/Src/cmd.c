/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	In this file you find all routines for programming like
	IF, WHILE, REPEAT

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#define MYDEBUG     0
#include "debug.h"
#include <proto/intuition.h>

/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/
typedef enum
{
    _IT_POS,
    _IT_MODIFIED,
    _IT_ICONMODE,
    _IT_TABSTOP,
    _IT_MARGIN,
    _IT_INSERTMODE,
    _IT_IGNORECASE,
    _IT_WORDWRAP,
    _IT_WWCOL,
    _IT_WINDOW,
    _IT_ICON,
    _IT_PENS,
    _IT_BLOCK,
    _IT_AUTO,
    _IT_SAVETABS,
    _IT_ED,
    _IT_DISCARD,
} ItemType;

struct Item
{
    ItemType type;	    /* _IT_xxx */
    UWORD    size;	    /* 1-normal, 2-special, 3-malloc'd */
    ULONG    value;	    /* value */
};


/**************************************
	    Interne Variable
**************************************/
static ULONG	     MAX_STACK_SIZE = 0;
static struct Item * ItemStack	    = NULL;
static UWORD	     ItemStackDepth = 0;


/**************************************
	   Interne Prototypes
**************************************/
static ItemType identify_item (char *);


/*
   IF condition trueaction, IFELSE condition trueaction falseaction

    condition:	!condition  NOT the specified condition.
		#	    toggle number is SET
		top	    top of file (on first line)
		bot	    end of file (on last line)
		left	    start of line (leftmost column)
		right	    end of line (nothing but spaces under and to the right)
		modified    text has been modified
		insert	    currently in insert mode
		y[<=>]#     cursor is (any OR combo of <,>,=) row #  (line numbers start at 1)
		x[<=>]#     cursor is (<,>,<=,>=,<>) column #        (columns start at 1)
			    <> means 'not equal'

		cl	    char under cursor is lower case
		cu	    char under cursor is upper case
		ca	    char under cursor is alpha
		cn	    char under cursor is numeric
		cb	    char within selected block
		c[<=>]#     char under cursor is (combo of <,>,and =) #,
			    where # can be a

				ASCII-code (numeric: hex, oct, dez)
				String (enclosed in ""). The cursor must
				    be on the first letter of "string".
				Character-Group: [], i.e. c=[1-9] will be true
				    if cursor is on any number. c=[a-zA-DF-H]
				    is a valid construct !

		block	    will BLOCK fail ?
		selnext     will SELECT NEXT succeed ?
		selprev     will SELECT PREV succeed ?

 and especially for WHILE

		true	   infinite-loop (!)

*/


/*DEFHELP #cmd program IF cnd act - if (@{B}cnd@{UB}) @{B}act@{UB} */
/*DEFHELP #cmd program IFELSE cnd ifact elseact - if (@{B}cnd@{UB}) @{B}ifact@{UB} else @{B}elseact@{UB} */
/*DEFHELP #cmd program WHILE cnd act -while (@{B}cnd@{UB}) @{B}act@{UB} */

DEFUSERCMD("if",     2, CF_VWM|CF_COK|CF_ICO, void, do_if, (void),;)
DEFUSERCMD("ifelse", 3, CF_VWM|CF_COK|CF_ICO, void, do_if, (void),;)
DEFUSERCMD("while",  2, CF_VWM|CF_COK|CF_ICO, void, do_if, (void),)
{
    BOOL    haselse = (av[0][2] == 'e'),    /* is there an ELSE ? */
	    iswhile = (av[0][0] == 'w'),    /* is this a WHILE ? */
	    istrue,			    /* result of condition */
	    notop   = 0;		    /* NOT ? */
    BOOL    c,
	    cx,
	    cc;
    UBYTE * buf1,			    /* two buffers */
	  * buf2,
	  * ptr,			    /* tmp ptr */
	  * cmpptr = NULL;
    ED	  * ep = Ep;
    int     i,
	    j, /* PATCH_NULL */
	    n, /* PATCH_NULL */
	    /* cxn, unused var */
	    cn;

    buf1 = (UBYTE *)malloc(MAXLINELEN);     /* get two buffers */
    buf2 = (UBYTE *)malloc(MAXLINELEN);

    if (buf1 == NULL || buf2 == NULL)
    {
	if (buf1) free(buf1);               /* :-( */
	if (buf2) free(buf2);

	title ("No Memory!");
	nomemory ();
	return;
    }

    /* prepare for Ctrl-C checking */

    breakreset ();

    ptr = av[1];

    /* check for NOT */

    if (*ptr == '!')
    {
	notop = 1;
	++ptr;
    }

    c	= tolower(ptr[0]);
    cn	= atoi ((char *)ptr);
    cx	= tolower(ptr[1]);

    strcpy ((char *)buf1, (char *)av[2]);

loop:
    istrue = 0;
    i	   = 0;
    j	   = 1; /* PATCH_NULL */

    switch (c)
    {
	case '$':
	case '&':
	    {
		char   ci;
		char * str;

		for (j = 1; isalnum(ptr[j]); ++j);
		ci = ptr[j];
		ptr[j] = 0;
		str = getvar(ptr+1);
		ptr[j] = ci;

		if (str)
		{
#if 0
		    if (!is_number(str))
		    {
			error ("%s:\nvariable %s does not contain an integer!\n(%s)",av[1], ptr+1, str);
			goto done;
		    } /* if */
		    i = atoi(str);
		    free(str);
#else
		    if (ci) {
			if (is_number(str)) {
			    i = atoi(str);
			} else {
			    i = 0x80000000;
			} /* if */
			strcpy (tmp_buffer, str);
			cmpptr = tmp_buffer;
			free(str);
			goto conditional;
		    } /* if */
#endif
		} else
		{
		    error ("%s:\nunknown variable %s", av[0], ptr+1);
		    goto done;
		}
		if (ci)
		    goto conditional;
		istrue = (i != 0);
		break;
	    }
	case 'x':
	    if (!i)
		i = ep->column + 1;

	case 'y':
	    if (!i)
		i = ep->line + 1;

conditional:
	    {
		/* int j, n; */
		char any = 0;

		for (; ptr[j] != '<' && ptr[j] != '>' && ptr[j] != '=' && ptr[j]; ++j); /* j=1 deleted */
		if (!ptr[j])
		{
		    error ("%s:\nBad conditional:\n`%s'", av[0], av[1]);
		    goto done;
		}

		for ( ; (ptr[j] == '<' || ptr[j] == '>' || ptr[j] == '=') && ptr[j]; ++j)
		{
		    switch(ptr[j])
		    {
			case '<':
			    any |= 1;
			    break;
			case '=':
			    any |= 2;
			    break;
			case '>':
			    any |= 4;
			    break;
		    }
		}

		if (!ptr[j] || !any || any == 7)
		{
		    error ("%s:\nBad conditional `%s':\nUnexp. EOL or wrong compare", av[0], av[1]);
		    goto done;
		}

		for ( ; isspace (ptr[j]); ++j);
		if (!ptr[j])
		{
		    error ("%s:\nBad conditional `%s':\nUnexp. EOL", av[0], av[1]);
		    goto done;
		}


		if (ptr[j] == '"')
		{
		    j ++;
		    for (n=j; ptr[j] != '"' && ptr[j]; j ++);
		    j -= n;

		    if (GETF_IGNORECASE(ep))
			n = strnicmp (cmpptr? cmpptr: (Current+ep->column), &ptr[n], j);
		    else
			n = strncmp  (cmpptr? cmpptr: (Current+ep->column), &ptr[n], j);
		} else if (ptr[j] == '[')
		{
		    if (any != 2 && any != 5)
		    {
			error ("%s:\nBad conditional `%s':\ncompare with [] must be '=' or '<>'", av[0], av[1]);
			goto done;
		    }

		    j ++;
		    cc = cmpptr? *cmpptr: Current[ep->column];

		    for ( ; ptr[j] != ']' && ptr[j]; j ++)
		    {
			if (ptr[j] == '\\') j ++;
			else if (ptr[j+1] == '-')
			{
			    if (!ptr[j+2] || ptr[j+2] == ']')
			    {
				error ("%s:\nBad conditional `%s':\nString ended too early", av[0], av[1]);
				goto done;
			    }

			    if (cc >= ptr[j] && cc <= ptr[j+2])
			    {
				cc = ptr[j];
				break;
			    }

			    j += 3;
			    continue;
			}

			if (cc == ptr[j]) break;
		    }

		    istrue = (cc==ptr[j] && any==2) || (cc!=ptr[j] && any==5);

		    break;
		} else if (isdigit(ptr[j]) || ptr[j] == '+' || ptr[j] == '-')
		{
		    n = i - atoi((char *)ptr+j);
		} else
		{
		    error ("%s:\nUnknown conditional `%s'", av[0], av[1]);
		    goto done;
		}

		istrue = (any&1 && n<0) || (any&2 && !n) || (any&4 && n>0);
	    }
	break;

	case 't':
	    if (cx == 'r')
		istrue = 1;
	    else if (isdigit (cx))
	    {
		cn = atoi (ptr+2);
		istrue = is_tflagset (cn);
	    }
	    else
		istrue = !ep->line;
	break;

	case 'b':
	    if (cx == 'l')
		istrue = block_ok ();
	    else istrue = ep->line == ep->lines-1;
	break;

	case 'l':
	    istrue = !ep->column;
	break;

	case 'r': {
	    UBYTE * ptr;
	    int len;

	    ptr = CONTENTS(Current);

	    len = Clen-1;

	    while (len >= 0 && ptr[len] == ' ')
		len --;

	    istrue = ep->column > len;
	break; }

	case 'm':
	    text_sync ();
	    istrue = GETF_MODIFIED(ep);
	break;

	case 'e':
	    istrue = GETF_COMLINEMODE(ep);
	break;

	case 'i':
	    istrue = GETF_INSERTMODE(ep);
	break;

	case 'c':
	    cc = Current[ep->column];

	    switch(cx)
	    {
		case 'b':
		    istrue = (is_inblock (ep->line, -1) & BP_BESIDE) != 0;
		break;

		case 'l':
		    istrue = islower (cc);
		break;

		case 'u':
		    istrue = isupper (cc);
		break;

		case 'a':
		    istrue = isalnum (cc);
		break;

		case 'n':
		    istrue = isdigit (cc);
		break;

		default:		/* c[<=>]#  */
		    i = Current[ep->column];
		    goto conditional;
		break;
	    }
	break;

	case 's':       /* SELNEXT/SELPREV */
	    switch (tolower (ptr[3]))
	    {
		case 'n':
		    if (GetSucc ((struct Node *)Ep))
			istrue = 1;
		break;

		case 'p':
		    if (GetPred ((struct Node *)Ep))
			istrue = 1;
		break;
	    }
	break;

	case 'g':                       /* PATCH_NULL: */
	    if (is_number(ptr+1))
	    {
		cn = atoi(ptr+1);
		c = '0';
	    } /* if */
	/* fall-through */

	default:
	    if (isdigit (c))
		istrue = is_gflagset (cn);
	    else
	    {
		error ("%s:\nBad condition\n`%s'", av[0], av[1]);
	    }

	break;
    }

    istrue = notop ? !istrue : (istrue != 0);

    if (istrue)
    {
	strcpy((char *)buf2, (char *)buf1); /* could be executed multiple
					       times */
	if (do_command((char *)buf2) == 0)
	    goto done;

	if (iswhile && !GETF_ABORTCOMMAND(Ep))
	{

	    if (breakcheck())
	    {
		SETF_ABORTCOMMAND(Ep,1);
	    } else if (LoopCont)
	    {
		LoopCont = 0;		/* PATCH_NULL [14 Feb 1993] : func <-> ass */
		goto loop;
	    } else if (LoopBreak)
	    {
		LoopBreak = 0;		/* PATCH_NULL [14 Feb 1993] : func <-> ass */
	    }
	    else
		goto loop;
	}
    } else
    {
	if (haselse)
	{	   /* only executed once */
	    strcpy((char *)buf2, (char *)av[3]);
	    do_command((char *)buf2);
	}
    }

done:
    free(buf1);
    free(buf2);
} /* do_if */


/*
 * repeat X command
 *
 * (if X is not a number it can be abbr. with 2 chars)
 *
 * X =	N     -number of repeats
 *	line  -current line # (lines begin at 1)
 *	lbot  -#lines to the bottom, inc. current
 *	cleft -column # (columns begin at 0)
 *		(thus is also chars to the left)
 *	cright-#chars to eol, including current char
 *	tr    -#char positions to get to next tab stop
 *	tl    -#char positions to get to next backtab stop
 */

#define SC(a,b) ((a)<<8|(b))

/*DEFHELP #cmd program REPEAT cnt comm - repeat @{B}comm@{UB} @{B}cnt@{UB} times. */

DEFUSERCMD("repeat", 2, CF_VWM|CF_COK|CF_ICO, void, do_repeat, (void),)
{
    UBYTE *ptr = av[1];
    unsigned long n;
    char *buf1, *buf2;

    if (!(buf1 = AllocMem (MAXLINELEN*2, 0)) )
    {
	SETF_ABORTCOMMAND(Ep,1);
	return;
    }

    buf2 = buf1 + MAXLINELEN;

    breakreset ();
    strcpy (buf1, av[2]);

    switch (SC(ptr[0],ptr[1]))
    {
	case SC('l','i'):
	    n = text_lineno ();
	    break;

	case SC('l','b'):
	    n = text_lines () - text_lineno () + 1;
	    break;

	case SC('c','l'):
	    n = text_colno ();
	    break;

	case SC('c','r'):
	    n = text_cols () - text_colno ();
	    break;

	case SC('t','r'):
	    n = text_tabsize () - (text_colno () % text_tabsize ());
	    break;

	case SC('t','l'):
	    n = text_colno () % text_tabsize ();

	    if (n == 0)
		n = text_tabsize ();

	    break;
	default:
	    n = atoi (av[1]);
	    break;
    }

    while (n > 0)
    {
	strcpy(buf2, buf1);

	if (do_command (buf2) == 0 || breakcheck ())
	{
	    SETF_ABORTCOMMAND(Ep,1);
	    break;
	}
	if (LoopBreak)
	{
	    LoopBreak = 0;		/* PATCH_NULL [14 Feb 1993] : func <-> ass */
	    break;
	} else if (LoopCont)
	{
	    LoopCont = 0;		/* PATCH_NULL [14 Feb 1993] : func <-> ass */
	} /* if */

	--n;
    }

    FreeMem (buf1, MAXLINELEN*2);
} /* do_repeat */


/*
 *  SCANF controlstring
 *
 *  The C scanf routine.  Only one variable, a string, is allowed in the
 *  control string.
 *
 *  Modified by A. Digulla to be more useful for a text-editor.
 *
 *   %s and %[] are still working
 *
 *   Additional :	 (w)ord    (l)ine    (n)umber  (b)lock   (c)har
 *			 (t)ext    (a)lpha   (r)emark
 *
 *   w - get word
 *   l - get line with EVERYTHING
 *   b - get all that fits into isalphanum()
 *   c - get single char
 *   t - like line, but without preceding and trailing spaces
 *   a - all that fits into isalpha()
 *   r - get c-style comment
 *
 *   if you precede a '+' to any of these, SCANF will start at the beginning
 *   to the left
 */

/*DEFHELP #cmd program SCANF ctlstr - scan the string at the current text position (C scanf) example: @{B}scanf %s@{UB} */

DEFUSERCMD("scanf", 1, CF_VWM|CF_COK|CF_ICO, void, do_scanf, (void),)
{
    UBYTE buf[MAXLINELEN], *ptr, *bptr;
    WORD  t;
    BOOL  left;

    buf[0] = 0;
    ptr = av[1];

    if (*ptr == '%')
    {
	  ptr ++;
	  if (*ptr == 's' || *ptr == '[')
	  {
	       if (*ptr == 's')
	       {
		    ptr ++;
		    *ptr = 0;	   /* Make sure there is nothing else */
	       } else
	       {
		    ptr ++;
		    if (*ptr == '~') *ptr = '^';

		    while (*ptr != ']') ptr ++;
		    ptr ++;
		    *ptr = 0;	   /* dito */
	       } /* if (*ptr == 's') */

	       sscanf(Current+Ep->column,av[1],buf);
	  } else
	  {
	       *buf = 0;
	  } /* if (*ptr == 's' && *ptr == '[') */
    } else
    {
	  if (*ptr == '+')
	  {
	       left = TRUE;
	       ptr ++;
	  } else
	       left = FALSE;

	  Clen = strlen ((char *)Current);

	  switch (*ptr)
	  {
	       case ('w') :
	       {
		    t = Ep->column;

		    if (t > Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    ptr = Current+t;

		    if (left)
		    {
			 while (t && *ptr != ' ')
			 {
			      ptr --;
			      t --;
			 }
		    }

		    while (*ptr == ' ' && t < Clen)
		    {
			 ptr ++;
			 t ++;
		    }

		    if (t == Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    bptr = buf;

		    while (*ptr != ' ' && *ptr)
			 *bptr++ = *ptr++;

		    *bptr = 0;
	       break; } /* case ('w') */

	       case ('l') :
	       {
		    if (left)
		    {
			 strncpy ((char *)buf, (char *)Current, Clen);
			 buf[Clen] = 0;
		    } else
		    {
			 strncpy ((char *)buf, (char *)Current+Ep->column,
				 Clen-Ep->column);
			 buf[Clen - Ep->column] = 0;
		    }

	       break; } /* case ('l') */

	       case ('n') :
	       {
		    t = Ep->column;

		    if (t > Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    ptr = Current+t;

		    if (left)
		    {
			 while (t && isdigit(*ptr))
			 {
			      ptr --;
			      t --;
			 }

			 if (t)
			 {
			      ptr ++;
			      t ++;
			 }
		    }

		    while (*ptr == ' ' && t < Clen)
		    {
			 ptr ++;
			 t ++;
		    }

		    if (t == Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    bptr = buf;

		    while (isdigit(*ptr))
			 *bptr++ = *ptr++;

		    *bptr = 0;

	       break; } /* case ('n') */

	       case ('b') :
	       {
		    t = Ep->column;

		    if (t > Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    ptr = Current+t;

		    if (left)
		    {
			 while (t && isalnum(*ptr))
			 {
			      ptr --;
			      t --;
			 }

			 if (t)
			 {
			      ptr ++;
			      t ++;
			 }
		    }

		    while (*ptr == ' ' && t < Clen)
		    {
			 ptr ++;
			 t ++;
		    }

		    if (t == Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    bptr = buf;

		    while (isalnum(*ptr))
			 *bptr++ = *ptr++;

		    *bptr = 0;

	       break; } /* case ('b') */

	       case ('c') :
	       {
		    *buf = *(Current+Ep->column);
		    buf[1] = 0;

	       break; } /* case ('c') */

	       case ('t') :
	       {
		    if (left)
		    {
			 ptr = Current;
			 t = 0;
		    } else
		    {
			 t = Ep->column;
			 ptr = Current+t;
		    }

		    if (t >= Clen || Clen == 0)
		    {
			 buf[0] = 0;
			 break;
		    }

		    while (*ptr == ' ')
		    {
			 ptr ++;
			 t ++;
		    }

		    if (t >= Clen)
		    {
			 buf[0] = 0;
			 break;
		    }

		    strncpy ((char *)buf, (char *)ptr, Clen-t);

		    ptr = buf + Clen - t - 1;

		    while (*ptr == ' ')
			 ptr --;

		    ptr[1] = 0;

	       break; } /* case ('t') */

	       case ('a') :
	       {
		    t = Ep->column;
		    ptr = Current+t;

		    if (left)
		    {
			 while (isalpha(*ptr) && t)
			 {
			      ptr --;
			      t --;
			 }

			 if (t)
			 {
			      ptr ++;
			      /* t ++; */
			 }
		    }

		    bptr = buf;

		    while (isalpha(*ptr))
			 *bptr++ = *ptr ++;

		    *bptr = 0;

	       break; } /* case ('a') */

	       case ('r') :
	       {
		    t = Ep->column;
		    ptr = Current+t;

		    if (left)
		    {
			 while (t)
			 {
			      if (*ptr == '*' && *(ptr-1) == '/')
			      {
				   ptr ++;
				   break;
			      }

			      t --;
			      ptr --;
			 }

			 if (!t)
			 {
			      *buf = 0;
			      break;
			 } else
			 {
			      t ++;
			      ptr ++;
			 }
		    }

		    if (t > Clen)
		    {
			 *buf = 0;
			 break;
		    }

		    bptr = buf;

		    while (t < Clen)
		    {
			 if (*ptr == '*' && ptr[1] == '/')
			 {
			      bptr --;
			      break;
			 }

			 *bptr++ = *ptr ++;
			 t ++;
		    }

		    *bptr = 0;

	       break; } /* case ('r') */

	  } /* switch (*ptr) */
    } /* if (*ptr == '%') */

    if (String)
	free (String);

    if (String = (char *)malloc (strlen ((char *)buf)+1))
    {
	strcpy ((char *)String, (char *)buf);

	title (String);
    }
} /* do_scanf */


/*****************************************************************************

    NAME
	do_push

    PARAMETER
	item

    DESCRIPTION
	Pushes an item on the internal stack.

******************************************************************************/

/*DEFHELP #cmd stack PUSH item - Push an item on the stack. */

DEFUSERCMD("push", 1, CF_VWM|CF_COK|CF_ICO, void, do_push, (void),)
{
    if (ItemStackDepth == MAX_STACK_SIZE)
    {
	struct Item * stack;

	if (stack = AllocMem (sizeof(struct Item) * (MAX_STACK_SIZE + 32), 0))
	{
	    if (MAX_STACK_SIZE)
		movmem (ItemStack, stack, sizeof(struct Item)*MAX_STACK_SIZE);

	    MAX_STACK_SIZE += 32;
	    ItemStack = stack;
	} else
	{
	    error ("push:\nCannot push %s:\nStack full", av[1]);
	    return;
	}
    }

    switch (ItemStack[ItemStackDepth].type = identify_item (av[1]))
    {
	case _IT_POS:
	{
	    struct TextMarker * pos;

	    if (pos = malloc (sizeof(struct TextMarker)))
	    {
		pos->ep     = Ep;
		pos->line   = Ep->line;
		pos->column = Ep->column;

		ItemStack[ItemStackDepth].value = (ULONG)pos;
		ItemStack[ItemStackDepth].size	= 3;
	    } else
	    {
out_of_memory:
		title ("push: Out of memory error");
		SETF_ABORTCOMMAND(Ep,1);
		return;
	    }
	} break;

	case _IT_MODIFIED:
	    ItemStack[ItemStackDepth].value = GETF_MODIFIED(Ep);
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_ICONMODE:
	    ItemStack[ItemStackDepth].value = GETF_ICONMODE(Ep);
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_TABSTOP:
	    ItemStack[ItemStackDepth].value = Ep->config.tabstop;
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_MARGIN:
	    ItemStack[ItemStackDepth].value = Ep->config.margin;
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_INSERTMODE:
	    ItemStack[ItemStackDepth].value = GETF_INSERTMODE(Ep);
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_IGNORECASE:
	    ItemStack[ItemStackDepth].value = GETF_IGNORECASE(Ep);
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_WORDWRAP:
	    ItemStack[ItemStackDepth].value = GETF_WORDWRAP(Ep);
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_WWCOL:
	    ItemStack[ItemStackDepth].value = Ep->config.wwcol;
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_SAVETABS:
	    ItemStack[ItemStackDepth].value = GETF_SAVETABS(Ep);
	    ItemStack[ItemStackDepth].size  = 1;
	break;

	case _IT_ED:
	    ItemStack[ItemStackDepth].value = (ULONG)Ep;
	    ItemStack[ItemStackDepth].size  = 2;
	break;

	case _IT_WINDOW:
	{
	    UWORD * array;

	    if (array = malloc (4*sizeof(UWORD)))
	    {
		if (!GETF_ICONMODE(Ep))
		{
		    array[0] = Ep->config.winx;
		    array[1] = Ep->config.winy;
		    array[2] = Ep->config.winwidth;
		    array[3] = Ep->config.winheight;
		} else
		{
		    array[0] = Ep->win->LeftEdge;
		    array[1] = Ep->win->TopEdge;
		    array[2] = Ep->win->Width;
		    array[3] = Ep->win->Height;
		}

		ItemStack[ItemStackDepth].value = (ULONG)array;
		ItemStack[ItemStackDepth].size	= 3;
	    } else
	    {
		goto out_of_memory;
	    }
	} break;

	case _IT_ICON:
	{
	    UWORD * array;

	    if (array = malloc (2*sizeof(UWORD)))
	    {
		if (GETF_ICONMODE(Ep))
		{
		    array[0] = Ep->config.iwinx;
		    array[1] = Ep->config.iwiny;
		} else
		{
		    array[0] = Ep->win->LeftEdge;
		    array[1] = Ep->win->TopEdge;
		}

		ItemStack[ItemStackDepth].value = (ULONG)array;
		ItemStack[ItemStackDepth].size	= 3;
	    } else
	    {
		goto out_of_memory;
	    }
	} break;

	case _IT_PENS:
	{
	    UBYTE * array;

	    if (array = malloc (6))
	    {
		array[0] = TEXT_FPEN(Ep);
		array[1] = TEXT_BPEN(Ep);
		array[2] = BLOCK_FPEN(Ep);
		array[3] = BLOCK_BPEN(Ep);
		array[4] = TITLE_FPEN(Ep);
		array[5] = TITLE_BPEN(Ep);

		ItemStack[ItemStackDepth].value = (ULONG)array;
		ItemStack[ItemStackDepth].size	= 2;
	    } else
	    {
		goto out_of_memory;
	    }
	} break;

	case _IT_BLOCK:
	{
	    Block * block;

	    if (block = malloc (sizeof(Block)))
	    {
		/* movmem (&ActualBlock, block, sizeof (Block)); */
		get_block (block);

		ItemStack[ItemStackDepth].value = (ULONG)block;
		ItemStack[ItemStackDepth].size	= 3;

		if (!GETF_ICONMODE(Ep))
		    text_redrawblock (0);
	    } else
	    {
		goto out_of_memory;
	    }
	} break;

	default:
	    error ("push:\nUnknown item\n`%s'", av[1]);
	    return;
	break;
    }

    ItemStackDepth ++;

#if MYDEBUG
    if (GETF_DEBUG(Ep))
	D(bug("PUSH Stack %d\n", ItemStackDepth));
#endif
} /* do_push */


/*****************************************************************************

    NAME
	POP/PEEK item

    PARAMETER
	item / AUTO / DISCARD

    DESCRIPTION
	Pops an item from the internal stack or (PEEK) just get's the value
	of it without poping it from the stack.

******************************************************************************/

/*DEFHELP #cmd stack POP item - Pop something from the stack and store it in item. The special item AUTO stores the thing back where it was taken from. */
/*DEFHELP #cmd stack PICK item - like POP, but doesn't remove the topmost element from stack ! */
/*DEFHELP #cmd stack PEEK item - like POP, but doesn't remove the topmost element from stack ! */

DEFUSERCMD("pick", 1, CF_VWM|CF_COK|CF_ICO, void, do_pop, (void),;)
DEFUSERCMD("peek", 1, CF_VWM|CF_COK|CF_ICO, void, do_pop, (void),;)
DEFUSERCMD("pop",  1, CF_VWM|CF_COK|CF_ICO, void, do_pop, (void),)
{
    ItemType type;

    if (ItemStackDepth == 0)
    {
	error ("%s:\nCannot pop %s:\nStack empty", av[0], av[1]);
	return;
    }

    ItemStackDepth --;

#if MYDEBUG
    if (GETF_DEBUG(Ep))
	D(bug("%s Stack %d\n", av[0], ItemStackDepth));
#endif

    type = identify_item (av[1]);

    if (type == _IT_AUTO)
	type = ItemStack[ItemStackDepth].type;

    switch (type)
    {
	case _IT_POS: {
	    struct TextMarker * pos;

	    if (ItemStack[ItemStackDepth].type != _IT_POS)
		error ("%s:\nCannot pop position\nto something else", av[0]);

	    pos = (struct TextMarker *)ItemStack[ItemStackDepth].value;

	    text_sync ();

	    text_cursor (1);
	    switch_ed (pos->ep);
	    text_cursor (0);

	    if (IntuitionBase->ActiveWindow != Ep->win)
	    {
		WindowToFront (Ep->win);
		ActivateWindow (Ep->win);
	    }

	    if (pos->line >= Ep->lines)
		Ep->line = Ep->lines - 1;
	    else
		Ep->line = pos->line;

	    Ep->column = pos->column;

	    text_load ();
	    text_adjust (FALSE);
	break; }

	case _IT_MODIFIED:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto modified", av[0]);

	    SETF_MODIFIED(Ep, ItemStack[ItemStackDepth].value);
	break;

	case _IT_ICONMODE:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto iconmode", av[0]);

	    if (GETF_ICONMODE(Ep) != ItemStack[ItemStackDepth].value)
	    {
		if (ItemStack[ItemStackDepth].value)
		    iconify ();
		else
		    uniconify ();
	    }
	break;

	case _IT_TABSTOP:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto tabstop", av[0]);

	    Ep->config.tabstop = ItemStack[ItemStackDepth].value;
	break;

	case _IT_MARGIN:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto margin", av[0]);

	    Ep->config.margin = ItemStack[ItemStackDepth].value;
	break;

	case _IT_INSERTMODE:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto insertmode", av[0]);

	    SETF_INSERTMODE(Ep, ItemStack[ItemStackDepth].value);
	break;

	case _IT_IGNORECASE:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto ignorecase", av[0]);

	    SETF_IGNORECASE(Ep, ItemStack[ItemStackDepth].value);
	break;

	case _IT_WORDWRAP:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto wordwrap", av[0]);

	    SETF_WORDWRAP(Ep, ItemStack[ItemStackDepth].value);
	break;

	case _IT_WWCOL:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto wordwrapcolumn", av[0]);

	    Ep->config.wwcol = ItemStack[ItemStackDepth].value;
	break;

	case _IT_SAVETABS:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto savetabs", av[0]);

	    SETF_SAVETABS(Ep,ItemStack[ItemStackDepth].value);
	break;

	case _IT_ED:
	    if (ItemStack[ItemStackDepth].type != _IT_ED)
		error ("%s:\nCannot pop ed\nto something else", av[0]);

	    switch_ed ((ED *)ItemStack[ItemStackDepth].value);
	    ActivateWindow (Ep->win);
	break;

	case _IT_WINDOW: {
	    UWORD * array;

	    if (ItemStack[ItemStackDepth].type != _IT_WINDOW)
		error ("%s:\nCannot pop window\nto something else", av[0]);

	    array = (UWORD *)ItemStack[ItemStackDepth].value;

	    Ep->config.winx	 = array[0];
	    Ep->config.winy	 = array[1];
	    Ep->config.winwidth  = array[2];
	    Ep->config.winheight = array[3];

	    if (!GETF_ICONMODE(Ep))
	    {
		MoveWindow (Ep->win, Ep->config.winx - Ep->win->LeftEdge,
				     Ep->config.winy - Ep->win->TopEdge);
		SizeWindow (Ep->win, Ep->config.winwidth  - Ep->win->Width,
				     Ep->config.winheight - Ep->win->Height);
	    }
	break; }

	case _IT_ICON: {
	    UWORD * array;

	    if (ItemStack[ItemStackDepth].type != _IT_ICON)
		error ("%s:\nCannot pop icon\nto something else", av[0]);

	    array = (UWORD *)ItemStack[ItemStackDepth].value;

	    Ep->config.iwinx	  = array[0];
	    Ep->config.iwiny	  = array[1];

	    if (GETF_ICONMODE(Ep))
	    {
		MoveWindow (Ep->win, Ep->config.iwinx - Ep->win->LeftEdge,
				     Ep->config.iwiny - Ep->win->TopEdge);
	    }
	break; }

	case _IT_PENS: {
	    UBYTE * array;

	    if (ItemStack[ItemStackDepth].type != _IT_PENS)
		error ("%s:\nCannot pop pens\nto something else", av[0]);

	    array = (UBYTE *)ItemStack[ItemStackDepth].value;

	    TEXT_FPEN(Ep)  = array[0];
	    TEXT_BPEN(Ep)  = array[1];
	    BLOCK_FPEN(Ep) = array[2];
	    BLOCK_BPEN(Ep) = array[3];
	    TITLE_FPEN(Ep) = array[4];
	    TITLE_BPEN(Ep) = array[5];

	    if (!GETF_ICONMODE(Ep))
		text_redisplay ();

	    SETF_MFORCETITLE(Ep,1);
	break; }

	case _IT_BLOCK: {
	    Block * block;
	    int type = ActualBlock.type;

	    if (ItemStack[ItemStackDepth].type != _IT_BLOCK)
		error ("%s:\nCannot pop block\nto something else", av[0]);

	    block = (Block *) ItemStack[ItemStackDepth].value;

	    /* movmem (block, &ActualBlock, sizeof (Block)); */
	    set_block (block);

	    if (!GETF_ICONMODE(Ep))
		if (ActualBlock.type == type)
		    text_redrawblock (1);
		else
		    text_redisplay ();
	break; }

	case _IT_DISCARD:
	break;

	default:
	    error ("%s:\nUnknown item\n`%s'", av[0], av[1]);
	break;
    }

    /* free only, if POP. If PEEK, restore Depth */
    if (av[0][1] == 'o' && av[0][2] == 'p')
    {
	if (ItemStack[ItemStackDepth].size == 3)
	    free ((void *)ItemStack[ItemStackDepth].value);
    } else
	ItemStackDepth ++;
} /* do_pop */


/*****************************************************************************

    NAME
	SWAP item

    PARAMETER
	item / AUTO / DISCARD

    DESCRIPTION
	This exchanges the actual item with the item on stack, ie. the
	topmost item from stack is poped and the actual item is pushed.

******************************************************************************/

/*DEFHELP #cmd stack SWAP item - exchange the topmost item on stack with the actual item */

DEFUSERCMD("swap", 1, CF_VWM|CF_COK|CF_ICO, void, do_swap, (void),)
{
    ItemType type;
    ULONG    value;

    if (ItemStackDepth == 0)
    {
	error ("swap:\nCannot swap %s:\nStack empty", av[1]);
	return;
    }

    ItemStackDepth --;

    value = ItemStack[ItemStackDepth].value;

#if MYDEBUG
    if (GETF_DEBUG(Ep))
	D(bug("Stack %d\n", ItemStackDepth));
#endif

    type = identify_item (av[1]);

    if (type == _IT_AUTO)
	type = ItemStack[ItemStackDepth].type;

    switch (type)
    {
	case _IT_POS: {
	    struct TextMarker pos;

	    if (ItemStack[ItemStackDepth].type != _IT_POS)
		error ("%s:\nCannot swap position\nto something else", av[0]);

	    pos.ep     = Ep;
	    pos.line   = Ep->line;
	    pos.column = Ep->column;

	    swapmem ((void *)value, &pos, sizeof (struct TextMarker));

	    text_sync ();

	    text_cursor (1);
	    switch_ed (pos.ep);
	    text_cursor (0);

	    if (IntuitionBase->ActiveWindow != Ep->win)
	    {
		WindowToFront (Ep->win);
		ActivateWindow (Ep->win);
	    }

	    if (pos.line >= Ep->lines)
		Ep->line = Ep->lines - 1;
	    else
		Ep->line = pos.line;

	    Ep->column = pos.column;

	    text_load ();
	    text_adjust (FALSE);
	break; }

	case _IT_MODIFIED:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto modified", av[0]);

	    ItemStack[ItemStackDepth].value = GETF_MODIFIED(Ep);
	    SETF_MODIFIED(Ep, value);
	break;

	case _IT_ICONMODE:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot pop a struct\nto iconmode", av[0]);

	    if (GETF_ICONMODE(Ep) != ItemStack[ItemStackDepth].value)
	    {
		if (ItemStack[ItemStackDepth].value)
		    iconify ();
		else
		    uniconify ();
	    }
	break;

	case _IT_TABSTOP:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto tabstop", av[0]);

	    ItemStack[ItemStackDepth].value = Ep->config.tabstop;
	    Ep->config.tabstop = value;
	break;

	case _IT_MARGIN:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto margin", av[0]);

	    ItemStack[ItemStackDepth].value = Ep->config.margin;
	    Ep->config.margin = value;
	break;

	case _IT_INSERTMODE:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto insertmode", av[0]);

	    ItemStack[ItemStackDepth].value = GETF_INSERTMODE(Ep);
	    SETF_INSERTMODE(Ep, value);
	break;

	case _IT_IGNORECASE:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto ignorecase", av[0]);

	    ItemStack[ItemStackDepth].value = GETF_IGNORECASE(Ep);
	    SETF_IGNORECASE(Ep, value);
	break;

	case _IT_WORDWRAP:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto wordwrap", av[0]);

	    ItemStack[ItemStackDepth].value = GETF_WORDWRAP(Ep);
	    SETF_WORDWRAP(Ep, value);
	break;

	case _IT_WWCOL:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto wordwrapcolumn", av[0]);

	    ItemStack[ItemStackDepth].value = Ep->config.wwcol;
	    Ep->config.wwcol = value;
	break;

	case _IT_SAVETABS:
	    if (ItemStack[ItemStackDepth].size != 1)
		error ("%s:\nCannot swap a struct\nto savetabs", av[0]);

	    ItemStack[ItemStackDepth].value = GETF_SAVETABS(Ep);
	    SETF_SAVETABS(Ep,value);
	break;

	case _IT_ED:
	    if (ItemStack[ItemStackDepth].type != _IT_ED)
		error ("%s:\nCannot swap ed\nto something else", av[0]);

	    ItemStack[ItemStackDepth].value = (ULONG)Ep;
	    switch_ed ((ED *)value);
	    ActivateWindow (Ep->win);
	break;

	case _IT_WINDOW: {
	    UWORD array[4];

	    if (ItemStack[ItemStackDepth].type != _IT_WINDOW)
		error ("%s:\nCannot swap window\nto something else", av[0]);

	    if (!GETF_ICONMODE(Ep))
	    {
		array[0] = Ep->config.winx;
		array[1] = Ep->config.winy;
		array[2] = Ep->config.winwidth;
		array[3] = Ep->config.winheight;
	    } else
	    {
		array[0] = Ep->win->LeftEdge;
		array[1] = Ep->win->TopEdge;
		array[2] = Ep->win->Width;
		array[3] = Ep->win->Height;
	    }

	    swapmem ((void *)value, array, sizeof(UWORD)*4);

	    Ep->config.winx	 = array[0];
	    Ep->config.winy	 = array[1];
	    Ep->config.winwidth  = array[2];
	    Ep->config.winheight = array[3];

	    if (!GETF_ICONMODE(Ep))
	    {
		MoveWindow (Ep->win, Ep->config.winx - Ep->win->LeftEdge,
				     Ep->config.winy - Ep->win->TopEdge);
		SizeWindow (Ep->win, Ep->config.winwidth  - Ep->win->Width,
				     Ep->config.winheight - Ep->win->Height);
	    }
	break; }

	case _IT_ICON: {
	    UWORD array[2];

	    if (ItemStack[ItemStackDepth].type != _IT_ICON)
		error ("%s:\nCannot swap icon\nto something else", av[0]);

	    if (GETF_ICONMODE(Ep))
	    {
		array[0] = Ep->config.iwinx;
		array[1] = Ep->config.iwiny;
	    } else
	    {
		array[0] = Ep->win->LeftEdge;
		array[1] = Ep->win->TopEdge;
	    }

	    swapmem ((void *)value, array, sizeof(UWORD)*2);

	    Ep->config.iwinx	  = array[0];
	    Ep->config.iwiny	  = array[1];

	    if (GETF_ICONMODE(Ep))
	    {
		MoveWindow (Ep->win, Ep->config.iwinx - Ep->win->LeftEdge,
				     Ep->config.iwiny - Ep->win->TopEdge);
	    }
	break; }

	case _IT_PENS: {
	    UBYTE array[6];

	    if (ItemStack[ItemStackDepth].type != _IT_PENS)
		error ("%s:\nCannot swap pens\nto something else", av[0]);

	    array[0] = TEXT_FPEN(Ep);
	    array[1] = TEXT_BPEN(Ep);
	    array[2] = BLOCK_FPEN(Ep);
	    array[3] = BLOCK_BPEN(Ep);
	    array[4] = TITLE_FPEN(Ep);
	    array[5] = TITLE_BPEN(Ep);

	    swapmem ((void *)value, array, 6);

	    TEXT_FPEN(Ep)  = array[0];
	    TEXT_BPEN(Ep)  = array[1];
	    BLOCK_FPEN(Ep) = array[2];
	    BLOCK_BPEN(Ep) = array[3];
	    TITLE_FPEN(Ep) = array[4];
	    TITLE_BPEN(Ep) = array[5];

	    if (!GETF_ICONMODE(Ep))
		text_redisplay ();

	    SETF_MFORCETITLE(Ep,1);
	break; }

	case _IT_BLOCK: {
	    Block block;
	    int type;

	    if (ItemStack[ItemStackDepth].type != _IT_BLOCK)
		error ("%s:\nCannot swap block\nto something else", av[0]);

	    /* movmem (&ActualBlock, &block, sizeof (Block)); */
	    get_block (&block);

	    type = ActualBlock.type;

	    if (!GETF_ICONMODE(Ep))
		text_redrawblock (0);

	    swapmem ((void *)value, &block, sizeof (Block));

	    /* movmem (&block, &ActualBlock, sizeof (Block)); */
	    set_block (&block);

	    if (!GETF_ICONMODE(Ep))
		if (ActualBlock.type == type)
		    text_redrawblock (1);
		else
		    text_redisplay ();
	break; }

	default:
	    error ("swap:\nUnknown item\n`%s'", av[1]);
	break;
    }

    ItemStackDepth ++;
} /* do_swap */


static ItemType identify_item (char * itemname)
{
    static CONST struct
    {
	char   * name;
	ItemType type;
    } items[] =
    {
	"auto",         _IT_AUTO,
	"block",        _IT_BLOCK,
	"discard",      _IT_DISCARD,
	"ed",           _IT_ED,
	"icon",         _IT_ICON,
	"iconmode",     _IT_ICONMODE,
	"ignorecase",   _IT_IGNORECASE,
	"insertmode",   _IT_INSERTMODE,
	"margin",       _IT_MARGIN,
	"modified",     _IT_MODIFIED,
	"pens",         _IT_PENS,
	"pos",          _IT_POS,
	"savetabs",     _IT_SAVETABS,
	"tabstop",      _IT_TABSTOP,
	"window",       _IT_WINDOW,
	"wordwrap",     _IT_WORDWRAP,
	"wwcol",        _IT_WWCOL,
    };
    WORD  low,
	  high,
	  t,
	  diff;

    low = 0;
    high = sizeof (items) / sizeof (items[0]) -1;

    do
    {
	t = (low + high) / 2;

	if (!(diff = stricmp (itemname, items[t].name)) )
	    break;
	else if (diff < 0)
	    high = t-1;
	else
	    low = t+1;
    } while (low <= high);

    if (!diff)
	return (items[t].type);

    return (-1);
} /* identify_item */


/*****************************************************************************

    NAME
	check_stack

    PARAMETER
	ED * ep;

    DESCRIPTION
	Checks the stack for thing left from editor ep or if ep == NULL
	cleans the full stack.

******************************************************************************/

Prototype void check_stack (ED * ep);

void check_stack (ED * ep)
{
    WORD i, j;

    if (ep)
    {
	for (i=j=0; i<ItemStackDepth; i ++)
	{
	    if (!((ItemStack[i].type == _IT_ED ||
			ItemStack[i].type == _IT_POS ||
			ItemStack[i].type == _IT_BLOCK) &&
			ep == *(ED **)ItemStack[i].value) )
			{
		if (i != j)
		{
		    ItemStack[j] = ItemStack[i];
		}

		j ++;
	    }
	}

	ItemStackDepth = j;
    } else
    {
	for (i=0; i<ItemStackDepth; i ++)
	{
	    if (ItemStack[i].size == 3)
		free ((void *)ItemStack[i].value);
	}

	ItemStackDepth = 0;
    }
} /* check_stack */


/******************************************************************************
*****  ENDE cmd.c
******************************************************************************/
