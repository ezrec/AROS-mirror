/******************************************************************************

    MODUL
	mikro.c

    DESCRIPTION
	hier ist alles, was zu klein war fuer einen vernuenftigen Oberbegriff

    NOTES

    BUGS

    TODO

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	******** b_noll created
	01-08-93 b_noll added MICRO_Init
	26-08-93 b_noll added MICRO_Exit
	07-08-94 null updated some docs

*!***************************************************************************
*!
*!  Misc Commands (Mikro.c)
*!
******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
/* #include "COM.h" */


/**************************************
	   Interne Prototypes
**************************************/
/* control */
Prototype void	  do_quiet    (void);
Prototype void	  do_eval     (void);
Prototype void	  do_force    (void);
Prototype void	  do_abort    (void);
Prototype void	  do_unabort  (void);
Prototype void	  do_break    (void);
Prototype void	  do_continue (void);
Prototype void	  clearcont   (void);
Prototype void	  clearbreak  (void);
Prototype void	  clearbreaks (void);

/* write */
Prototype void	  do_insertwrite (void);
Prototype void	  do_printf	 (void);
Prototype void	  m_write	 (char *);

/* flags */
Prototype void	  do_viewmode	 (void);
Prototype void	  do_simpletabs  (void);
Prototype void	  do_sourcebreak (void);
Prototype void	  do_fwovr	 (void);
Prototype void	  do_longline	 (void);



/*
**  some commands to modify the command flow
**
*! >ABORT
*! >UNABORT
*!  they toggle a flag which controlles macro-executing
*!  ABORT sets the break-flag which causes e.g. immediately macro-abortion at keys
*!  or terminates line-execution
*!  UNABORT allows to reset this flag in an AREXX-script,
*!  is the last line produced an error, you may clear the error-flag
*!  if UNABORT is the first (or only) command of a line
*!
*! >BREAK
*! >CONTINUE
*!  these two commands do nearly the same like their C-language conterparts
*!  BREAK leave the current loop-construct and
*!  CONTINUE goes to the next turn of the current loop-construct
*!
**  Note that UNABORT is obsolete since Async Rexxports are introduced,
**	 as after every RexxComm got in mainloop we do a clearbreaks()
*/

void do_abort ( void )
{
    SETF_ABORTCOMMAND(Ep, 1);
} /* do_abort */

void do_unabort ( void )
{
    SETF_ABORTCOMMAND(Ep, 0);
} /* do_unabort */

int is_aborted ( void )
{
    return (GETF_ABORTCOMMAND(Ep));
} /* is_aborted */



void do_break (void)
{
    LoopBreak++;
} /* do_break */



void do_continue (void)
{
    LoopCont++;
} /* do_continue */


#if 0
void clearcont (void)  { LoopCont = 0;  } /* clearcont */
void clearbreak (void) { LoopBreak = 0; } /* clearbreak */
#endif


void clearbreaks (void)
{
    LoopCont  = 0;
    LoopBreak = 0;
    SETF_ABORTCOMMAND(Ep, 0);
} /* clearbreaks */




/*
**  some re-evaluation commands
**
*! >EVAL com
*!
*!  to split longer commandsequences to fit their length down to MAXIA
*!  so MAXIA may stay "small" and anybody can avoid "expression too complex"
*!
*! >FORCE [T|S|F|R|D] command
*!
*!  That's a command that means conjunction
*!  of EVAL, QUIET, UNFAIL and UNTITLE
*!
*!	T == unTitle
*!	S == quiet (no Screen updating)
*!	F == unFail
*!	R == no Requestors
*!	D == enable debugging mode
*!     <no flags> -> simple eval
*!
*!  for this reason we have declared UNTITLE, QUIET and UNFAIL
*!  obsolete, and have removed them;
*!  most of their functionality can be accessed via macros.
*!
*/

void do_eval (void)
{
    char* buffer = (char*)malloc(LINE_LENGTH); /* allocate a buffer */
    if (buffer)
    {
	strcpy(buffer, av[1]);                 /* copy the command into that buffer */
	do_command(buffer);                    /* execute the buffer */
	free(buffer);                          /* and then free it again */
    } else
    {
	nomemory();
	/* SETF_ABORTCOMMAND(Ep, 1); */
    } /* if */
} /* do_eval */



void do_force (void)
{
    char inter_screen  = Nsu;		/* extern Nsu controlles screen updates */
    char inter_title   = GETF_SHOWTITLE(Ep);     /* extern showtitle controlles changes in window-titling */
    char inter_request = GETF_NOREQUEST(Ep);     /* Disable flag for requestors */
    char inter_debug   = GETF_DEBUG(Ep);         /* Debugging mode */

    struct TextMarker chk = { NULL, 0, 0 };

    int failures       = 0;

    char* typer = av[1];
    char* av1 = av[1];
    char* av2 = av[2];

    av[1] = av[2];
    av[2] = NULL;

    chk.line   = Ep->line;
    chk.column = Ep->column;
    if (typer)
    {			     /* check for any set flags */
	while (*typer)
	{
	    switch ((*typer)|32)
	    {
	    case 't':
		SETF_SHOWTITLE(Ep, 0);
		break;
	    case 's':
		chk.ep = Ep;
		Nsu = 1;
		break;
	    case 'f':
		failures = 1;
		break;
	    case 'r':
		SETF_NOREQUEST(Ep, 1);
		break;
	    case 'd':
		SETF_DEBUG(Ep, 1);
		break;
	    default:
		error ("%s:\nInvalid flags '%s'", av[0], typer);
		goto df_quit;
	    } /* switch */
	    typer++;
	} /* while */
    } /* if */

    do_eval();                          /* do the evaluation */

    if (failures)
    {			  /* (...) cancel all breaks */
	extern char MacroBreak;  /* PATCH_NULL [08 Mar 1993] : added */
	if (MacroBreak != 1)     /* PATCH_NULL [08 Mar 1993] : added */
	    SETF_ABORTCOMMAND(Ep, 0);
	/* do_unbreak();  */
    } /* if */

df_quit:
    /* (if necessary) restore old values */
    if (chk.ep) {
	Nsu = inter_screen;
	if (Ep == chk.ep) {
	    if (Ep->line != chk.line) {
		text_adjust(FALSE);
	    } else if (Ep->column != chk.column) {
		text_adjust(FALSE);
/*	      } else {				*/
/*		  text_redisplay_currline();    */
	    } /* if moved */
/*	  } else {				*/
	} /* if ep */
    } /* if "s" */
    SETF_SHOWTITLE(Ep, inter_title);
    SETF_NOREQUEST(Ep, inter_request);
    SETF_DEBUG(Ep, inter_debug);

    av[2] = av2;
    av[1] = av1;
    return;
} /* do_force */


#if 0

/*
** the following commands are disabled, they
** were replaced by some macros with FORCE
*/

/*
* !  >UNFAIL com
* !
* ! That's really like EVAL, but it ignore's failures
* ! Very useful if U try to call an Command but U don't
* ! know if it is one, or if you don't want a failure to break your macro
* !
*/

void do_unfail (void)
{
    do_eval();
    SETF_ABORTCOMMAND(Ep, 0);
#if 0
    do_clearallbreaks();                /* simply cancel all breaks after evaluation */
#endif
} /* do_unfail */


/*
* !  >UNTITLE com
* !
* !  That's also like EVAL, except for the feature, that
* !  No Title is shown during the macrocall
* !
*/

void do_untitle (void)
{
    char inter = GETF_SHOWTITLE(Ep);   /* extern showtitle controlles changes in window-titling */
    SETF_SHOWTITLE(Ep, 0);            /* buffer that value and change it for evaluation, then restore it */
    do_eval();
    SETF_SHOWTITLE(Ep, inter);
} /* do_untitle */

#endif /* 0 */


/*
*!  >QUIET com
*!
*! That's like EVAL, but during execution no window
*! should be refreshed
*! as _No_Screen_Update is increased during operation
*!
*/

void do_quiet (void)
{
    int inter = Nsu;			/* extern Nsu controlles screen updates */
    Nsu = 1;				/* buffer that value and change it for evaluation, then restore it */
    do_eval();
    Nsu = inter;
} /* do_quiet */




/*
**  some write - commands
**
*!
*! >PRINTF format parameters
*!
*!  create a string with format and its (up to 8) parameters
*!  and write it into the current text
*!
*! >INSERT text
*! >OVERWRITE text
*!
*!  insert or overwrite text in the current text without respect
*!  to the specialflag INSERTMODE
*!
**    note that in DME it is possible to define that ^-combinations
**  can be interpreted like commands or like ctrl-keystrokes
**  when they are INSERTed (not in overwrite mode)
**  that patch is switched with the specialvar TEXTCOM
**    note that that dme-patch is permanent enabled in the xdme
**  commands PRINTF, INSERT and OVERWRITE.
**
*/

void m_write(char * string)
{
    char c;
    char * ptr	= string;
    char * pptr;

/*    char quoted = GETF_AUTOINDENT(Ep); */
/*    SETF_AUTOINDENT(Ep, 0);            */
    while ((c = *ptr)) {
	pptr = ptr;
	while ((c != 0) &&
	    (c != '\t' && c != ('t'&31)) &&
	    (c != '\n' && c != ('n'&31)) &&
	    (c != '\r' && c != ('r'&31))) {
	    ++ptr;
	    c = *(ptr);
	} /* while */
	*ptr = 0;
/* printf ("< '%s'\n", pptr); */
	text_write(pptr);
	if (c != 0) {
	    *ptr = c;
	    ++ptr;
	    switch (c) {
	    case '\t':
	    case 't'&31:
		do_tab();       /* do_command ("tab"); */
		/* !! might be better to create some spaces !! */
		break;
	    case '\n':
	    case 'n'&31:
		do_split();
		do_firstcolumn();
		do_down();      /*  do_command ("split first down"); OR do_command ("return first"); */
		break;
	    case '\r':
	    case 'r'&31:
		av[0] = "b";
		do_bs();        /* do_command ("bs"); */
		break;
	    /* default: */ /* !! oh yea - I know that there are others C-escapes, but these were thought to be important !! */
	    } /* switch */
	} /* if */
    } /* while */
/*    SETF_AUTOINDENT(Ep, quoted); */
} /* m_write */


void do_insertwrite (void)
{
    char insertbackup = GETF_INSERTMODE(Ep);

    SETF_INSERTMODE(Ep, (av[0][0] == 'i'));

    m_write(av[1]);

    SETF_INSERTMODE(Ep, insertbackup);
} /* do_insertwrite */


void do_printf (void)
{
    char* iav = av[0];
    char* arg [10];
    char* aux [10];
    char* ptr = av[2];
    char quoted;
    ULONG arc;
    char* buf;

    if (!(buf = malloc(512))) {
	nomemory();
	abort2();
    } /* if */

    for (arc = 0; arc < 10; ++arc) arg[arc] = NULL;
    arc = 0;
    while ((arg[arc] = breakout (&ptr, &quoted, &aux[arc]))) {
/* printf (">arg %ld == %s\n", arc, arg[arc]); */
	if (++arc >= 10)
	    break;
    } /* while */

    vsprintf (buf, av[1], (va_list)arg);
    m_write (buf);

    while (arc > 0) {
	--arc;
	if (aux[arc])
	    free(aux[arc]);
    } /* while */

    av[0] = iav;
    free (buf);
} /* do_printf */



/*
**  some flags
**  The following commands should be placed into prefs.c
**
*!
*!  Viewmode
*!	That flag disables modifications of the current
*!	text; if TRUE any modifiing command causes an error
*!	(modifiing commands are recognized in do_command with the
*!	flag CF_VWM being FALSE)
*!
*!  ActivateToFront
*!  Windowcycling
*!	These two flags refer to SELECT:
*!
*!	ActivateToFront invokes a "WindowToFront()" every
*!			time, windows are switched
*!	Windowcycling	enables the possibility to switch
*!			to the other end of Textlist, if
*!			one end is reacjed with SELECT next/prev
*!
*!  SourceBreaks
*!	That flag refers to SOURCE
*!	if TRUE if an Abortcommand or a Break occurs
*!	on sourcing level, the operation is aborted
*!	else the next line is executed.
*!
*!  SimpleTabs
*!	That flag is an add to SAVETABS and SAVE[old|as]
*!	if Savetabs is set, SimpleTabs decides to use
*!	TAB-Optimisation until the first non-blank only
*!	(is usable, if a program ignores leading whitespace,
*!	but does not convert tabs to spaces)
*!
* !  FWOvr (Fixed Width Overwrite)
* !	 That flag is used in connection with INSERTMODE:
* !	 if the User has selected Overwrite, and that flag is
* !	 TRUE, DEL will cause replacing the deleted char
* !	 by a space instead of shortening the line and
* !	 BS is replaced by a simple LEFT (perhaps with writing
* !	 a space)
* !
* !  TextLine/LongLine (14-06-93)
* !	 this flag is used to disable movement left of the end
* !	 of the current line; note: ending spaces are usually
* !	 ignored in that mode
* !	 (this option was done to satisfy a CED user; dunno if it helps)
* !
*/

DEFUSERCMD("viewmode", 1, CF_VWM|CF_COK|CF_ICO, void,do_viewmode, (void),)
{
    SETF_VIEWMODE(Ep, tas_flag (av[1], GETF_VIEWMODE(Ep), "Viewmode"));
} /* do_viewmode */


DEFUSERCMD("activatefront", 1, CF_VWM|CF_COK|CF_ICO, void, do_activefront, (void),)
{
    SETF_ACTIVATETOFRONT(Ep, tas_flag (av[1], GETF_ACTIVATETOFRONT(Ep), "ActivateToFront"));

} /* do_activefront */


DEFUSERCMD("windowcycling", 1, CF_VWM|CF_COK|CF_ICO, void, do_windowcyc, (void),)
{
    SETF_WINDOWCYCLING(Ep, test_arg (av[1], GETF_WINDOWCYCLING(Ep)));

    if (GETF_WINDOWCYCLING(Ep))
	title ("Windowcycling is now ON");
    else
	title ("Windowcycling is now OFF");
} /* do_windowcyc */


void do_sourcebreak (void)
{
    SETF_SOURCEBREAKS(Ep, test_arg (av[1], GETF_SOURCEBREAKS(Ep)));

    if (GETF_SOURCEBREAKS(Ep))
	title ("Source - Breaks are Enabled");
    else
	title ("Source - Breaks are Ignored");
} /* do_sourcebreak */


void do_simpletabs (void)
{
    SETF_SIMPLETABS(Ep, test_arg (av[1], GETF_SIMPLETABS(Ep)));

    if (GETF_SIMPLETABS(Ep) && GETF_SAVETABS(Ep))
	title ("Tabsaving up to forstnb");
    else if (GETF_SAVETABS(Ep))
	title ("Tabsaving full");
    else
	title ("Tabsaving off");
} /* do_simpletabs */

#if 0

void do_fwovr (void)
{
    SETF_FWOVR(Ep, test_arg (av[1], GETF_FWOVR(Ep)));

    if (GETF_FWOVR(Ep))
	title ("Overwrite Delete FixWidth is now ON");
    else
	title ("Overwrite Delete FixWidth is now OFF");
} /* do_fwovr */


void do_shortline (void)
{
    SETF_SLINE(Ep, tas_flag (av[1], GETF_SLINE(Ep), "Movement upto EoL"));
} /* do_longline */


#endif


#if 0

DEFUSERCMD("abort", 0, CF_VWM|CF_ICO|CF_COK, void,do_abort, (void),)
DEFUSERCMD("unabort", 0, CF_VWM|CF_ICO|CF_COK, void,do_unabort, (void),)
DEFUSERCMD("continue", 0, CF_VWM|CF_ICO|CF_COK, void,do_continue, (void),)
DEFUSERCMD("break", 0, CF_VWM|CF_ICO|CF_COK, void,do_break, (void),)
DEFUSERCMD("rem", 1, CF_VWM|CF_ICO|CF_COK, void,do_null, (void),)
DEFUSERCMD("eval", 1, CF_VWM|CF_COK, void,do_eval, (void),)
DEFUSERCMD("force", 2, CF_VWM|CF_COK, void,do_force, (void),)
DEFUSERCMD("quiet", 1, CF_VWM|CF_ICO|CF_COK, void,do_quiet, (void),)
DEFUSERCMD("overwrite", 1, CF_COK, void,do_insertwrite, (void),)
DEFUSERCMD("insert", 1, CF_COK, void,do_insertwrite, (void),)
DEFUSERCMD("printf", 2, CF_COK, void,do_printf, (void),)
DEFUSERCMD("sourcebreaks", 1, CF_VWM|CF_COK|CF_ICO, void,do_sourcebreak, (void),)
/*DEFHELP #cmd misc ABORT - abort the current command execution */
/*DEFHELP #cmd misc UNABORT - make a previous ABORT or error unseen; that command can only be used in ARexx scripts */
/*DEFHELP #cmd misc BREAK - break the current command loop */
/*DEFHELP #cmd misc CONTINUE - break the current turn of the current command loop */
/*DEFHELP #cmd misc EVAL command - reinvoke the command interpreter;that command can b used to prevent "too many arguments" errors, which occurs, if the user has specified too many variable arguments at the same execution level */
/*DEFHELP #cmd misc FORCE flags command - similar to EVAL, but according to flags it can ignore errorvalues (F), disable screenupdates (S) or errorrequesters (R) or title updates (T) or activate Debug Mode (D) */
/*DEFHELP #cmd misc QUIET command - short for "FORCE S command" */
/*DEFHELP OVERWRITE text - write some text in overwrite mode ignoring $INSERTMODE */
/*DEFHELP INSERT text - write some text in insert mode ignoring $INSERTMODE */
/*DEFHELP PRINTF format args - write some text w/ printf formatting */


#ifdef STATIC_COM

    COMMAND("abort",          0, CF_VWM|CF_ICO|CF_COK, (FPTR)do_abort    )
    COMMAND("unabort",        0, CF_VWM|CF_ICO|CF_COK, (FPTR)do_unabort  )
    COMMAND("continue",       0, CF_VWM|CF_ICO|CF_COK, (FPTR)do_continue )
    COMMAND("break",          0, CF_VWM|CF_ICO|CF_COK, (FPTR)do_break    )
    COMMAND("rem",            1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_null     )    /* comments in lines */
    COMMAND("eval",           1,        CF_VWM|CF_COK, (FPTR)do_eval     )
    COMMAND("force",          2,        CF_VWM|CF_COK, (FPTR)do_force    )
    COMMAND("quiet",          1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_quiet )
    COMMAND("overwrite",      1,               CF_COK, (FPTR)do_insertwrite  )
    COMMAND("insert",         1,               CF_COK, (FPTR)do_insertwrite  )
    COMMAND("printf",         2,               CF_COK, (FPTR)do_printf   )
    COMMAND("sourcebreaks",   1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_sourcebreak  )
    COMMAND("viewmode",       1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_viewmode )
 /* COMMAND("fwovr",          1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_fwovr    ) */
 /* COMMAND("textline",       1, CF_VWM|CF_ICO|CF_COK, (FPTR)do_shortline    ) */
 /* COMMAND("windowcycling",  1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_windowcyc    ) */
 /* COMMAND("activatefront",  1, CF_VWM|CF_COK|CF_ICO, (FPTR)do_activefront  ) */

#endif
#endif

/******************************************************************************
*****  ENDE mikro.c
******************************************************************************/
