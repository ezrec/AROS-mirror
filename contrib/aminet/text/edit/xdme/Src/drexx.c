/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Another AREXX interface for the XDME. Uses HRexx.c, some tools, I
	wrote for a easier AREXX-usage.

	This module implements the same commands as "rexx.c", so it works
	with a minimum of changes in the real XDME sources.

	Because I want to implement a, which is ALWAYS calleble from the
	outer world (other tasks), I was forced to made some minor changes
	in main.c (marked with "/ * HD " as comment ). Further I had to add
	an Activate Window to cmd1.c's QUIT command , because it dosen't
	work proper if it was called with a deactivated window.

	In addition I added my new commands in the commandtable in
	"command.c"

******************************************************************************/

/**************************************
		Includes
**************************************/
#include <proto/dos.h>
#include <proto/intuition.h>
#include "defs.h"
#include "hrexx.h"
#include "rexx/storage.h"
#include "rexx/rxslib.h"
#ifndef __AROS__
#include "rexx/rexxio.h"
#endif
#include "rexx/errors.h"
#define  MYDEBUG      0
#include "debug.h"


/**************************************
	    Globale Variable
**************************************/
Prototype ULONG RexxMask;
Prototype int foundcmd; /* control for implicit ARexx macro invocation	 */
Prototype int cmderr;	/* global command error flag for do_rexx()'s use */

ULONG RexxMask; 	/* ArexxPort signalmask for Wait		*/
int   foundcmd; 	/* control for implicit ARexx macro invocation	*/
int   cmderr;		/* global command error flag for do_rexx's use  */



/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/
static char rexx_result[MAXLINELEN];   /* hier wird das Rexxresult untergerbracht */


/**************************************
	   Interne Prototypes
**************************************/
__stkargs static void in_rexx (struct RexxMsg *);


/************************************************************************/
/*			   lokale  Funktionen				*/
/************************************************************************/

__stkargs static void in_rexx (struct RexxMsg *rmsg)
/*
 * this function is called with incomming Rexxmessages, while
 * processing SyncRexxCommand
 *
 * --> rmsg	pointer to the RexxMessage
 */
{
#ifndef __AROS__
    long ret;
    foundcmd = 0;

    *rexx_result = 0;

    ret = !do_command ((char *)ARG0(rmsg));

    if (rexx_result[0])
	ReplyRexxMsg (rmsg, ret, rexx_result);
    else
	ReplyRexxMsg (rmsg, ret, 0);

    *rexx_result= 0;

    do_command ("null");     /* reset foundcommand */
#else
#warning ARG0 is not defined on AROS
#endif
}


/************************************************************************/
/*			   globale Funktionen				*/
/************************************************************************/

Prototype long do_rexx (const char * port, const char *fmt,...);

long do_rexx (const char * port, const char *fmt,...)
/*
 * This command transmittes the command, printf-style, to a named
 * port.
 *
 * --> port	name of port
 * --> fmt	printf-like format string for the command
 * --> ...	the args for the command
 */
{
    va_list va;
    char macro[MAXLINELEN];

    va_start (va, fmt);
    vsprintf (macro, fmt, va);
    va_end (va);

    return (SyncRexxCommand (port, macro, rexx_result, sizeof(rexx_result), 0));
}


/*DEFHELP #cmd arexx RXRESULT any - Copy @{B}any@{UB} into RESULT in an AREXX-script. */

DEFUSERCMD("rxresult", 1, CF_VWM|CF_ICO, void, put_rexx_result, (void),)
/*
    Copy a string to rexx_result
*/
{
    strcpy (rexx_result, av[1]);
}


Prototype char * get_rexx_result (void);

char * get_rexx_result (void)
/*
    Get Contents of rexx_result
*/
{
    /* puts (rexx_result); */

    return (rexx_result);
}


Prototype void openrexx (void);

void openrexx (void)
/* initalizes the Arexx-stuff */
{
#ifndef __AROS__
   int number;

   number = 1;

   Forbid ();

   do
   {
	sprintf (RexxPortName, "XDME.%d", number ++);
   } while (FindPort (RexxPortName) && number < 1000);

   Permit ();

   RexxMask = OpenRexx (RexxPortName, "XDME", in_rexx, 0, 0);

   if (!RexxMask)
	exiterr ("Cannot open AREXX-Port");
#endif
}


Prototype void closerexx (void);

void closerexx (void)
/* shut down the Arexx-stuff */
{
   CloseRexx ();
}


/*DEFHELP #cmd arexx RX - ARexx macro, no args (@{B}RX@{UB} macname) */
/*DEFHELP #cmd arexx RX1 - ARexx macro, one arg (@{B}RX1@{UB} macname arg1) */
/*DEFHELP #cmd arexx RX2 - ARexx macro, two args (@{B}RX2@{UB} macname arg1 arg2) */

DEFUSERCMD("rx",  1, CF_VWM|CF_ICO, void , do_rx, (void),;)
DEFUSERCMD("rx1", 2, CF_VWM|CF_ICO, void , do_rx, (void),;)
DEFUSERCMD("rx2", 3, CF_VWM|CF_ICO, void , do_rx, (void),)
/*
 *  explicit invocation interface between do_command() and do_rexx
 *  for ARexx macros having NO arguments (i.e., for the "rx" command)
 */
{
    if (!av[0][2])
	do_rexx (0,"%s",av[1]);
    else if (!av[0][2] == '1')
	do_rexx (0,"%s %s",av[1],av[2]);
    else
	do_rexx (0,"%s %s %s",av[1],av[2],av[3]);
}


Prototype long do_rxImplied (char * cmd,char * args);

long do_rxImplied (char * cmd,char * args)
/*
 *  implicit invocation interface between do_command() and do_rexx
 *  for ARexx macros implicitly called; arbitrary number of arguments
 */
{
    return (do_rexx (0,"%s %s",cmd,args));
}


/***********************************************************************/
/***********		   New Commands 		   *************/
/***********************************************************************/

Prototype void extern_rexx_command (void);

void extern_rexx_command (void)
/*
 * executes a command, comming in from an external port
 */
{
#ifndef __AROS__
    long	     ret;
    struct RexxMsg * rmsg;

    *rexx_result = 0;

    rmsg = GetRexxMsg ();

    if (!rmsg) /* NO MESSAGE !?!?! strange .... */
	return;

    ret = !do_command ((char *)ARG0(rmsg));

    if (rexx_result[0])
	ReplyRexxMsg (rmsg, ret, rexx_result);
    else
	ReplyRexxMsg (rmsg, ret, 0);

    *rexx_result = 0;
#else
#warning ARG0 is not defined on AROS
#endif
} /* extern_rexx_command */


/*DEFHELP #cmd arexx PORT name cmd - Send @{B}cmd@{UB} to ARexx-Port @{B}name@{UB} */

DEFUSERCMD("port", 2, CF_VWM|CF_ICO, void, to_port, (void),)
/*
 * simply passes a command to another named port
 */
{
    do_rexx (av[1], "%s", av[2]);
} /* to_port */


/*DEFHELP #cmd arexx,win SELECT what - make a window the current one. */

DEFUSERCMD("select", 1, CF_VWM|CF_ICO, void, select_window, (void),)
/*
 * selects another window FIRST LAST NEXT PREVIOUS WINDOW=<NAME> SAVE LOAD
 * every command can be abreviated by its first character:
 *  "select window=cmd1.c" = "select w=cmd1.c"
 */
{
    ED	      * ed     = NULL;
    static ED * sto_ed = NULL;

    switch (av[1][0])
    {
	case ('l') :
	{
	    if (av[1][1] == 'a')
		ed = (ED *)GetTail (&DBase);
	    else
		ed = sto_ed;
	break; } /* case ('l') */

	case ('f') :
	{
	    ed = (ED *)GetHead (&DBase);
	break; } /* case ('f') */

	case ('n') :
	{
	    ed = (ED *)GetSucc (Ep);

	    if (ed == NULL && GETF_WINDOWCYCLING(Ep))
	    {  /* PATCH_NULL */
		ed = (ED *)GetHead (&DBase);    /* PATCH_NULL */
	    } /* if */				/* PATCH_NULL */
	break; } /* case ('n') */

	case ('p') :
	{
	    ed = (ED *)GetPred (Ep);

	    if (ed == NULL && GETF_WINDOWCYCLING(Ep))
	    {  /* PATCH_NULL */
		ed = (ED *)GetTail (&DBase);    /* PATCH_NULL */
	    } /* if */				/* PATCH_NULL */
	break; } /* case ('p') */

	case ('a') :
	{
	    ActivateWindow (Ep->win);

	    if (GETF_ACTIVATETOFRONT(Ep))
	    {		   /* PATCH_NULL */
		WindowToFront (Ep->win);        /* PATCH_NULL */
	    } /* if */				/* PATCH_NULL */
	break; } /* case ('a') */

	case ('s') :
	{
	    sto_ed = Ep;
	    return;
	break; } /* case ('s') */

	case ('w') :
	{
	    char * ptr;

	    ptr = av[1];

	    while (*ptr && *ptr != '=')
		ptr ++;

	    if (*ptr)
		ptr ++;
	    else
		break;

	    D(bug("looking for `%s'\n", ptr));

	    for (ed=(ED *)GetHead(&DBase); ed; ed=(ED *)GetSucc(ed))
		if (!stricmp (ed->name, ptr))   /* found it */
		    break;

	    D(bug("Found %08x\n", ed));
	break; } /* case ('w') */

    } /* switch (av[1][0]) */

    if (ed)
    {
	switch_ed (ed);

	ActivateWindow (ed->win);

	if (GETF_ACTIVATETOFRONT(Ep))
	{		   /* PATCH_NULL */
	    WindowToFront (ed->win);            /* PATCH_NULL */
	} /* if */				/* PATCH_NULL */
    } else
    {
	SETF_ABORTCOMMAND(Ep,1);
    }
} /* select_window */


/*DEFHELP #cmd arexx PROJECTINFO - Gives some information about the current project. */

DEFUSERCMD("projectinfo", 0, CF_VWM|CF_ICO, void, project_info, (void),)
/*
 * builds the following string:
 * path <name> <left> <top> <width> <height> <iconleft> <iconright>
 * if Rexx called this string is return as resultstring
 */
{
    char path[PATHSIZE];		/* Puffer für Pfad */

    getpathto (Ep->dirlock, NULL, path);

    /* Read values from Window-Struct */
    if (GETF_ICONMODE(Ep))
    {
	Ep->config.iwinx     = Ep->win->LeftEdge;
	Ep->config.iwiny     = Ep->win->TopEdge;
    } else
    {
	Ep->config.winx      = Ep->win->LeftEdge;
	Ep->config.winy      = Ep->win->TopEdge;
	Ep->config.winwidth  = Ep->win->Width;
	Ep->config.winheight = Ep->win->Height;
    }

    sprintf (rexx_result, "\"%s\" \"%s\" %d %d %d %d %d %d %d",
			   path,    Ep->name,
					 Ep->config.winx,
					    Ep->config.winy,
					       Ep->config.winwidth,
						  Ep->config.winheight,
						     Ep->config.iwinx,
							Ep->config.iwiny,
							   Ep->line
    );
} /* project_info */


/*DEFHELP #cmd program,io PROJECTSAVE - Save all window-dimensions, filenames and position of iconified windows. */

DEFUSERCMD("projectsave", 0, CF_VWM|CF_ICO, void, save_project, (void),)
/*
 * saves all opened files in the directory of the currently active
 * editor to a file name XDMEArgs.projectfilename.
 * The lines are in the same format as returned from project_info
 */
{
    ED * ed;				/* Currenteditor */
    ED * act_ed;			/* active Editor */
    /* BPTR old_lock;			   / * Active dirlock */
    BPTR file;				/* our PRJ file */

    /* DO NOT CHANGE DIRECTORY ! Problem:
	    > XDME src/var.c
	    % projectsave
	will write "XDME_Project_File" into "src/" rather than "" ! */

    /* old_lock = CurrentDir (Ep->dirlock); / * change dir */
    act_ed   = Ep;			 /* save active Editor */

    file = Open (XDMEArgs.projectfilename, MODE_NEWFILE);

    if (file)
    {
	Write (file, "# path name x y w h ix iy line\n", 31L);

	for (ed=(ED *)GetHead(&DBase); ed; ed=(ED *)GetSucc((struct Node *)ed))
	{
	    switch_ed (ed);              /* change current ed */

	    project_info ();             /* build info text */

	    Write (file, rexx_result, strlen (rexx_result));
	    Write (file, "\n", 1L);
	}

	Close (file);
    } /* if (file) */

    switch_ed (act_ed);                  /* restore actuall editor */

    /* CurrentDir (old_lock);               / * go back to old dir */
} /* project_save */


/*DEFHELP #cmd program,io PROJECTLOAD - Recall session */

DEFUSERCMD("projectload", 0, CF_VWM|CF_ICO, int, load_project, (void),)
{
    FILE * file;
    char   path[PATHSIZE];		/* Puffer für Pfad */
    char   name[60];			/* file name */
    char * ptr;
    long   t, il, it, line;		/* window & icon & line */

    file = fopen (XDMEArgs.projectfilename, "r");  /* Open ProjectFile */

    if (file)
    {
	while (fgets (tmp_buffer, sizeof (tmp_buffer), file))
	{
	    if (*tmp_buffer == '#')
		continue;

	    ptr = skip_whitespace (tmp_buffer);

	    if (*ptr == '"')
	    {
		ptr ++;

		for (t=0; *ptr != '"'; )
		    path[t ++] = *ptr ++;

		ptr ++;
	    } else
		for (t=0; !isspace(*ptr); )
		    path[t ++] = *ptr ++;

	    path[t] = 0;
	    ptr = skip_whitespace (ptr);

	    if (*ptr == '"')
	    {
		ptr ++;

		for (t=0; *ptr != '"'; )
		    name[t ++] = *ptr ++;

		ptr ++;
	    } else
		for (t=0; !isspace(*ptr); )
		    name[t ++] = *ptr ++;

	    name[t] = 0;

	    av[1] = ptr;
	    do_openwindow ();

	    av[1] = path;
	    do_cd ();

	    av[0] = "newfile";
	    av[1] = name;
	    do_edit ();

	    sscanf (ptr, " %d  %d  %d  %d  %d   %d   %d\n",
			   &t, &t, &t, &t, &il, &it, &line);

	    if (line >= Ep->lines)
		line = Ep->lines - 1;

	    Ep->line = line;

	    text_load ();
	    text_adjust (FALSE);

	    Ep->config.iwinx = il;
	    Ep->config.iwiny = it;

	    do_iconify ();
	}

	fclose (file);

	t = TRUE;
    } else
    {
	warn ("projectload: Cannot find file `%s'", XDMEArgs.projectfilename);
	t = FALSE;
    }

    return (t);
} /* load_project */


/******************************************************************************
*****  ENDE drexx.c
******************************************************************************/
