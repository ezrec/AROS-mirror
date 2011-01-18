/******************************************************************************

    MODUL
	$Id$

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/exec.h>
#include <string.h>

#define MYDEBUG 1
#include "debug.h"

/**************************************
	    Globale Variable
**************************************/
extern int foundcmd;	   /* control for implicit ARexx macro invocation   */
extern int cmderr;	   /* global command error flag for do_rexx()'s use */


/**************************************
      Interne Defines & Strukturen
**************************************/
#define CF_COK	1   /*	Can be executed while in command line mode	*/
#define CF_PAR	2   /*	ESCIMM special flag.. save rest of command line */
		    /*	so it can be executed after user entry		*/
#define CF_ICO	4   /*	OK to execute if iconified, else uniconify first*/
#define CF_VWM	8   /*	OK to execute if in viewmode, else abort	*/

#define BTOCP(val, type)    ((type)((long)val << 2))

typedef void (*FPTR)(long);

typedef struct
{
    const char * name;	/* command name 	*/
    UBYTE args; 	/* number of arguments	*/
    UBYTE flags;	/* flags		*/
    FPTR func;		/* c-function		*/
} COMM;

#define MAXIA	    5


/**************************************
	    Interne Variable
**************************************/

static CONST COMM Comm[] =
{
#undef DEFUSERCMD
#define DEFUSERCMD(str,nargs,flags,ret,func,param,ext)  { str, nargs, flags, (FPTR) func, },
#include "commands.h"
#undef DEFUSERCMD
#define DEFUSERCMD(str,nargs,flags,ret,func,param,ext)  ret func param ext
};


static CONST COMM repcmd =
{
    "repeat",        2, CF_COK|CF_ICO, (FPTR)do_repeat,
};


/**************************************
	   Interne Prototypes
**************************************/


Prototype int do_command (char * str);

int do_command (char * str)
{
    char     * arg;
    char     * aux1,
	     * aux2,
	     * repstr[MAXIA];
    char       quoted;
    WORD       repi = 0;
    long       j;
    WORD       low;
    WORD       high;
    WORD       diff;
    static int level = 0;
    COMM     * comm;
#if 0
#define DEBUG_FILE  /* Logging on ? */
#endif
    D(bug("cmd: %s\n", str));
#ifdef DEBUG_FILE
    static FILE * debug_file = NULL;

    if (!debug_file && GETF_DEBUG(Ep))
	debug_file = fopen ("t:XDME.debug", "w");
#endif

    if (!str || !*str)
	return (1);

#ifdef DEBUG_FILE
    if (debug_file && GETF_DEBUG(Ep))
	fprintf (debug_file, "%s\n", str);
#endif

if (GETF_DEBUG(Ep)) printf("dcom[%2d|%d]: %s\n", level+1, GETF_ABORTCOMMAND(Ep), str);

    if (MacroRecord != 0 && level == 0) {       /* PATCH_NULL [14 Feb 1993] : added */
	add_record (str);                       /* PATCH_NULL [14 Feb 1993] : added */
    } /* if */					/* PATCH_NULL [14 Feb 1993] : added */

    if (++level > 20)
    {
	error ("command:\nRecursion too deep");
	--level;

	foundcmd = 1;	/* to prevent us from trying an ARexx macro */

	return (0);
    }

    while ((arg = breakout (&str, &quoted, &aux1)) &&
	    !(LoopBreak || LoopCont) )                      /* PATCH_BREAK - should be put together */
    {
if (GETF_DEBUG(Ep)) printf("loop[%2d|%d] CHECK %s\n",level, GETF_ABORTCOMMAND(Ep), arg);
	if (quoted)
	{
if (GETF_DEBUG(Ep)) printf("loop[%2d|%d] write %s\n",level, GETF_ABORTCOMMAND(Ep), arg);
	    if (GETF_VIEWMODE(Ep) && !GETF_COMLINEMODE(Ep))               /* PATCH_NULL */
	    {
if (GETF_DEBUG(Ep)) printf("-- viewmode");
		goto fail;				    /* PATCH_NULL */
	    } /* if */					    /* PATCH_NULL */

	    if (GETF_ICONMODE(Ep))
		uniconify ();

	    text_write (arg);
	    goto loop;
	} /* if (quoted) */

	if (isalpha (*arg))
	{
	    low  = 0;
	    high = sizeof(Comm)/sizeof(Comm[0]) - 1;

	    do
	    {
		j = (low + high)/2;

		diff = stricmp (arg, Comm[j].name);

		if (!diff)
		    break;

		if (diff < 0)
		    high = j-1;
		else
		    low = j+1;
	    } while (low <= high);

	    if (!diff)
	    {
if (GETF_DEBUG(Ep)) printf("loop[%2d|%d] comm %s\n",level, GETF_ABORTCOMMAND(Ep), arg);

		comm = (COMM *)&Comm[j];

		foundcmd = 1;

		av[0] = (UBYTE *)comm->name;

		for (j=1; j<=comm->args; j++)
		{
		    av[j] = (UBYTE *)breakout (&str, &quoted, &aux2);

		    if (aux2)
		    {
			if (repi == MAXIA)
			{
			    free (aux2);

			    error ("command:\nToo many args for\n%s", av[0]);

if (GETF_DEBUG(Ep)) printf("-- varargs Oflow");

			    goto fail;
			} else
			{
			    repstr[repi ++] = aux2;
			}
		    }

		    if (!av[j])
		    {
			if (j < 2)
			    error ("command:\nBad argument %ld of\n%s", j, av[0]);
			else
			    error ("command:\nBad argument %ld of\n%s %s", j, av[0], av[1]);

if (GETF_DEBUG(Ep)) printf("-- less args");
			goto fail;
		    }
		} /* for all args */

		av[j] = NULL;	/* end of arglist */

process:
		if ((comm->flags & CF_COK) || !GETF_COMLINEMODE(Ep))
		{
		    if (GETF_VIEWMODE(Ep) && !GETF_COMLINEMODE(Ep) && !(comm->flags & CF_VWM))   /* PATCH_NULL */
		    {
if (GETF_DEBUG(Ep)) printf("-- viewmode");
			goto fail;				    /* PATCH_NULL */
		    } /* if */					    /* PATCH_NULL */

		    if (comm->flags & CF_PAR)
		    {
			if (Partial)
			    free (Partial);

			Partial = (char *)malloc (strlen (str) + 1);
			strcpy (Partial, str);
			str += strlen(str);     /*  skip string */
		    } /* if CF_PAR */

		    if (GETF_ICONMODE(Ep) && !(comm->flags & CF_ICO))
			uniconify ();

if (GETF_DEBUG(Ep)) printf ("comm[%2d|%d] %s/%s\n", level, GETF_ABORTCOMMAND(Ep), av[0], av[1]);

		    (*comm->func)(-1);
		} /* if CF_COK or Not Comlinemode */

		if (GETF_ABORTCOMMAND(Ep))
		{
if (GETF_DEBUG(Ep)) printf("-- aborted");
		    goto fail;
		}

		goto loop;
	    } /* if arg == command */
	} else if (isdigit(*arg))
	{
	    av[0] = "repeat";
	    av[1] = arg;
	    av[2] = (UBYTE *)breakout(&str, &quoted, &aux2);

	    if (aux2)
	    {
		repstr[repi ++] = aux2;
	    }
if (GETF_DEBUG(Ep)) printf("-- repeat");

	    if (!av[2])
	    {
		error ("command:\nBad second argument\nfor short-repeat");
		goto fail;
	    }

	    av[3] = 0;
	    comm = (COMM *)&repcmd;

	    goto process;
	} /* Repeat */

	{
	    /*
	    **	Search for a macro - we had to put that scan
	    **	before the checks for menus and keys, as
	    **	keys are only checked up to 3 chars
	    */

	    void * macro;
	    int    ret;

	    if ( (macro = (void*)getmacro(arg)) )
	    {
		int narg = nummacroargs (macro);

		/* av[0] = (CHAR *)((NODE*)macro)->ln_Name; */
if (GETF_DEBUG(Ep)) printf("loop[%2d|%d] macro %s\n",level, GETF_ABORTCOMMAND(Ep), arg);

		av[0] = NULL;

		foundcmd = 1;

		for (j = 1; j <= narg; ++j)
		{
		    av[j] = (UBYTE *)breakout (&str, &quoted, &aux2);

		    if (aux2)
		    {
			if (repi == MAXIA)
			{
			    free(aux2);
			    error ("macro:\nToo many args for\n%s", av[0]);
if (GETF_DEBUG(Ep)) printf("-- M:varargs Oflow");
			    goto fail;
			} else
			{
			    repstr[repi++] = aux2;
			}
		    }

		    if (!av[j])
		    {
			error ("macro:\nBad argument %ld of\n%s", j, av[0]);
if (GETF_DEBUG(Ep)) printf("-- M:less args");
			goto fail;
		    }
		}

		av[j] = NULL;	/* end of arglist */

if (GETF_DEBUG(Ep)) printf ("mac [%2d|%d] %s/%s\n", level, GETF_ABORTCOMMAND(Ep), arg, av[1]);
		ret = callmacro(macro);

		if (!ret)
		{
if (GETF_DEBUG(Ep)) printf("-- M:abort");
		    goto fail;
		} /* if no succ */

		goto loop;
	    } /* if */
	} /* block */

	/* Command not found, check for macro	*/
	{
	    char * cmdstr;
	    int    ret;

	    if ((cmdstr = keyspectomacro(arg)) || (cmdstr = menutomacro(arg)))
	    {
		cmdstr = strdup (cmdstr);

if (GETF_DEBUG(Ep)) printf ("loop[%2d|%d] key %s\n", level, GETF_ABORTCOMMAND(Ep), cmdstr);

		ret = do_command (cmdstr);

		free (cmdstr);

		if (ret)
		{
		    foundcmd = 1;
		    goto loop;
		}

if (GETF_DEBUG(Ep)) printf("-- K:abort");

		goto fail;
	    } /* if arg is macro or menu */
	}


/*
*!  PATCH_JUX (reine gaudi)
*!
*!    Direct assignments of existing vars
*! With that patch something is possible like:
*!    set abc 1 ... abc = 3 (instead of set abc 3)
*!
*! if arg1 is a "=" we search for a variable of the name in arg0
*! if there is such a variable, we assign its value to the value
*! in arg2  (the correct types are used)
*!
*!  But what shall we do, if there was not such a variable???
*/
	{
	    char * dummy = str;
	    int    type  = VAR_NEX;

	    while (*dummy && *dummy <33)
	    {
		dummy ++;
	    } /* while */

	    if (*dummy == '=')
	    {
		dummy ++;

		str = dummy;

		dummy = GetTypedVar (arg, &type);

		if (dummy)
		{
		    free (dummy);

		    dummy = breakout (&str, &quoted, &aux2);

		    if (!dummy)
		    {
			error ("Assignment without value");
			goto fail;
		    } /* if */

		    SetTypedVar (arg, dummy, type);

		    if (aux2)
		    {
			free (aux2);
		    } /* if */

		    goto loop;
		} else
		{ /* there is no variable of the given name  - WAS sollen wir jetzt machen ? einen Default verwenden ??? */

		} /* if */
	    } /* if */
	} /* block direct assignment */

	/* Command still not found, check for public macro  */
	/* code to be added */

	if (do_rxImplied (arg, str))
	    error ("command:\nUnknown command\n`%s'", arg);

if (GETF_DEBUG(Ep)) printf("running into fail");

fail:
if (GETF_DEBUG(Ep)) printf ("fail[%2d|%d]\n", level, GETF_ABORTCOMMAND(Ep));
	--level;

	while (--repi >= 0)
	    free (repstr[repi]);

	if (aux1) free (aux1);

	return(0);

loop:
	if (aux1) free (aux1);

	// aux1 = NULL;      /* PATCH_BREAK - is not necessary */
    } /* while (arg) */

/* ret_ok: - point is not necessary */
if (GETF_DEBUG(Ep)) printf ("ret1[%2d|%d]\n", level, GETF_ABORTCOMMAND(Ep));

    --level;

    while (--repi >= 0)
	free (repstr[repi]);

    // if (aux1) free (aux1); /* PATCH_BREAK - is not necessary */

    return(1);
} /* do_command */


/*DEFHELP #cmd misc NULL - no operation */
/*DEFHELP #cmd misc NOP - no operation */
/*DEFHELP #cmd misc REM com - add commend */

DEFUSERCMD("nop",  0, CF_VWM|CF_ICO|CF_COK,void,do_null,(void),;)
DEFUSERCMD("rem",  1, CF_VWM|CF_ICO|CF_COK,void,do_null,(void),;)
DEFUSERCMD("null", 0, CF_VWM|CF_ICO|CF_COK,void,do_null,(void),)
{
} /* do_null */


/*DEFHELP #cmd program,io SOURCE file - source a script file. '#' in first column for comment */

DEFUSERCMD("source", 1, CF_VWM|CF_COK|CF_ICO, void, do_source, (long do_err),)
{
    char   buf[MAXLINELEN];
    FILE * fi;
    char * str;
    BPTR   oldlock;

DL;
    oldlock = CurrentDir(DupLock(Ep->dirlock));
DL;

    if ( (fi = fopen(av[1], "r")) )
    {
	while (fgets(buf, MAXLINELEN, fi))
	{
	    if (buf[0] == '#')
		continue;
	    for (str = buf; *str; ++str)
	    {
		if (*str == 9)
		    *str = ' ';
	    }

	    if (str > buf && str[-1] == '\n')
		str[-1] = 0;

	    do_command(buf);

	    if (GETF_SOURCEBREAKS(Ep))                                   /* PATCH_NULL */
	    {
		if (GETF_ABORTCOMMAND(Ep) || LoopBreak || LoopCont)      /* PATCH_NULL */
		{
		    break;					/* PATCH_NULL */
		} /* if */					/* PATCH_NULL */
	    } else						/* PATCH_NULL */
	    {
		SETF_ABORTCOMMAND(Ep,0);            /* PATCH_NULL */
		LoopBreak    = 0;		    /* PATCH_NULL */
		LoopCont     = 0;		    /* PATCH_NULL */
	    } /* if */						/* PATCH_NULL */
	}
	fclose(fi);
    } else
    {
	if (do_err)
	{
	    error ("source:\nFile `%s'\nnot found", av[1]);
	}
    }

    UnLock(CurrentDir(oldlock));
} /* do_source */


/*DEFHELP #cmd win QUIT - close current window. If text was modified, a safety check is performed */

DEFUSERCMD("quit", 0, CF_VWM|CF_ICO, void, do_quit, (void),)
{
    SETF_QUITFLAG(Ep,1);

    ActivateWindow (Ep->win);                    /* HD damit aus AREXX */
} /* do_quit */


/*DEFHELP #cmd misc QUITALL - leave XDME. If any text was modified, a safety check is performed for that text */

DEFUSERCMD("quitall", 0, CF_VWM|CF_ICO, void, do_quitall, (void),)
{
    SETF_QUITFLAG(Ep,1);
    SETF_QUITALL(Ep,1);

    ActivateWindow (Ep->win);                    /* HD damit aus AREXX */
} /* do_quitall */


/*DEFHELP #cmd misc,io,program EXECUTE comm - Execute a CLI command. */

DEFUSERCMD("execute", 1, CF_VWM|CF_ICO, void, do_execute, (void),)
{
    BPTR   oldlock = CurrentDir (Ep->dirlock);
    BPTR   NilFH;
    PROC * proc    = (PROC *)FindTask (NULL);

    NilFH = Open ("NIL:", 1006);

    if (NilFH)
    {
	void *oldConsoleTask = proc->pr_ConsoleTask;

#ifndef __AROS__
	proc->pr_ConsoleTask = (APTR)BTOCP(NilFH, struct FileHandle *)->fh_Port;
#else
#warning proc->pr_ConsoleTask = fileHandle->fh_Port
	proc->pr_ConsoleTask = NULL;
#endif

	Execute (av[1], NilFH, NilFH);

	proc->pr_ConsoleTask = oldConsoleTask;

	Close(NilFH);
    } else
    {
	error ("execute:\nNIL:-device required");
    }

    CurrentDir(oldlock);
} /* do_execute */


/*
 *  BREAKOUT()
 *
 *  Break out the next argument.  The argument is space delimited and
 *  might be quoted with `' or (), or single quoted as 'c or )c
 *
 *  Also:	$var	    -variable insertion
 *		^c	    -control character
 */

Prototype char * breakout (char ** ptr, char * quoted, char ** paux);

char * breakout (char ** ptr, char * quoted, char ** paux)
{
    char * str = *ptr;
    char * base;
    WORD   count;	    /* Level of brace */
    char   opc; 	    /* Open-Char */
    char   clc; 	    /* Close-Char */
    char   immode;
    char   isaux;
    char   buf[MAXLINELEN];
    WORD   di, i;

/* if (GETF_DEBUG(Ep)) printf("bout[  |%d] ? %s\n", GETF_ABORTCOMMAND(Ep), str); */

    count =
	opc   =
	clc   =
	immode=
	isaux =
	di    = 0;

    *quoted = 0;
    if (paux)
	*paux = NULL;

    while (*str == ' ')
	++str;

    if (!*str)
	return (NULL);

    *ptr = str;
    base = str;

    while (*str)
    {
	if (immode)
	{
	    if (di != sizeof(buf)-1)
		buf[di++] = *str;

	    str ++;
	    continue;
	}

	if (count == 0)
	{
	    if (*str == ' ')
		break;

	    if (*str == '\'' || *str == ')')
		clc = *str;
	    else if (*str == '`')
	    {
		opc = '`';
		clc = '\'';
	    } else if (*str == '(')
	    {
		opc = '(';
		clc = ')';
	    }
	}

	if (*str == opc)
	{
	    count ++;

	    if (str == *ptr)
	    {
		*quoted = 1;
		base = ++ str;
		continue;
	    }
	}

	if (*str == clc)
	{
	    count --;

	    if (count == 0 && *quoted)     /*  end of argument     */
		break;

	    if (str == *ptr && count < 0)
	    {
		immode = 1;
		*quoted = 1;
		base = ++str;

		continue;
	    }
	}

	/*
	 *  $varname $(varname) $`varname'.  I.E. three forms are allowed,
	 *  which allows one to insert the string almost anywhere.  The
	 *  first form names are limited to alpha-numerics, '-', and '_'.
	 */

	if (*str == '$')
	{
	    char * ptr2;
	    char * tmpptr;
	    char   c,
		   ce;
	    WORD   len;

/* if (GETF_DEBUG(Ep)) printf("bout[  |%d] $ %s\n", GETF_ABORTCOMMAND(Ep), str); */

	    ce = 0;			    /*	first form  */
	    str ++;			    /*	skip $	    */

	    if (*str == '(')                /*  second form */
	    {
		ce = ')';
		++str;
	    } else if (*str == '`')         /*  third form  */
	    {
		ce = '\'';
		++str;
	    }

	    ptr2 = str; 		    /*	start of varname    */

	    if (ce)                         /*  until end char OR   */
	    {
		while (*ptr2 && *ptr2 != ce)
		    ptr2 ++;
	    } else			    /*	smart end-varname   */
	    {
		while (isalnum(*ptr2) || *ptr2 == '-' || *ptr2 == '_' )
		{
		    ptr2 ++;
		}
	    }

	    len = ptr2 - str;		    /*	length of variable  */
	    c = *ptr2; *ptr2 = 0;	    /*	temp. terminate \0  */

/* if (GETF_DEBUG(Ep)) printf("bout[  |%d] var '%s'\n", GETF_ABORTCOMMAND(Ep), str); */

	    if (stricmp(str, "currentdir") == 0)
	    {
		*ptr2  = c;
		isaux = 1;

		if (NameFromLock (Ep->dirlock, tmp_buffer, sizeof(tmp_buffer)))
		{
		    i = strlen(tmp_buffer);

		    if (di + i < sizeof(buf)-1)
		    {
			strcpy (buf + di, tmp_buffer);
			di += i;
		    }
		}

		str += len;

		if (ce)
		    ++str;

		continue;
	    } else if (stricmp(str, "path") == 0)
	    {
		*ptr2 = c;
		isaux = 1;

		if (NameFromLock(Ep->dirlock, tmp_buffer, sizeof(tmp_buffer)))
		{
		    i = strlen (tmp_buffer);

		    if (di + i < sizeof(buf)-1)
		    {
			strcpy (buf+di, tmp_buffer);
			di += i;
		    }
		}

		str += len;
		if (ce)
		    ++str;

		continue;
	    } else if (stricmp(str, "file") == 0)
	    {
		i = strlen(Ep->name);
		*ptr2 = c;
		isaux = 1;
		if (di + i < sizeof(buf)-1)
		{
		    /*movmem(Ep->name, buf + di, i);*/
		    memmove (buf + di, Ep->name, i);
		    di += i;
		    buf[di] = 0;
		}
		str += len;
		if (ce)
		    ++str;
		continue;
	    } else if (stricmp(str, "filename") == 0)
	    {
		*ptr2 = c;
		isaux = 1;

		if (NameFromLock(Ep->dirlock, tmp_buffer, sizeof(tmp_buffer)))
		{
		    if (AddPart (tmp_buffer, Ep->name, sizeof(tmp_buffer)))
		    {
			i = strlen (tmp_buffer);

			if (di + i < sizeof(buf)-1)
			{
			    strcpy(buf + di, tmp_buffer);
			    di += i;
			}
		    }
		}

		str += len;
		if (ce)
		    ++str;
		continue;
	    } else if (!stricmp(str, "rexxport"))           /* TJM */
	    {
		*ptr2 = c;
		isaux = 1;

		if (di + strlen(RexxPortName) < sizeof(buf)-1)
		{
		    strcpy(buf + di, RexxPortName);
		    di += strlen(buf + di);
		}

		str += len;

		if (ce)
		    ++str;
		continue;
	    } else if (!stricmp(str, "rxresult"))
	    {
		*ptr2 = c;
		isaux = 1;

		if (di + strlen(get_rexx_result()) < sizeof(buf)-1)
		{
		    strcpy(buf + di, get_rexx_result());
		    di += strlen(buf + di);
		}

		str += len;

		if (ce)
		    ++str;
		continue;
	    } else if ( (tmpptr = getvar(str)) )
	    {
		ptr2 = tmpptr;
		str[len] = c;
		isaux = 1;

		if (di + strlen(ptr2) < sizeof(buf)-1)
		{
		    strcpy(buf + di, ptr2);
		    di += strlen(buf + di);
		}

		str += len;

		if (ce)
		    ++str;

		free(ptr2);
		continue;
	    }

	    *ptr2 = c;
	    str --;

	    if (ce)
		str --;
	} else if (*str == '^' && (str[1] & 0x1F))   /* CTRL-sequence */
	{
	    str ++;
	    *str &= 0x1F;
	    isaux = 1;
	} else if (*str == '\\' && str[1])
	{
	    /* reverted to old code */
#ifdef NOTDEF
	    if (!count) /* ignore \ */
	    {
		str ++;
	    } else if (*quoted && count == 1) /* remove it, but check next char */
	    {
#endif
		str ++;
#ifdef NOTDEF
		isaux = 1;
		continue;
	    } else /* don't remove it, but also ignore next char */
	    {
		buf[di ++] = *str ++;
	    }
#endif
	    isaux = 1;
	}

	buf[di ++] = *str ++;
    } /* while (*str) */

    buf[di ++] = 0;

    if (isaux)
    {
	*paux = malloc (di);
	strcpy (*paux, buf);
	base = *paux;
    }

    if (*str)               /*  space ended */
    {
	*str = '\0';
	*ptr = str + 1;     /*	next arg    */
    } else
    {
	*ptr = str;	    /*	last arg    */
    }

    return (base);
} /* breakout */


/******************************************************************************
*****  ENDE command.c
******************************************************************************/
