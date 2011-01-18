/******************************************************************************

    MODUL
	lowlevelmacros.c

    DESCRIPTION
	die schnittstelle zu den wichtigsten Macro-funktionen,
	die von anderen Moduln verwendet werden duerfen.

    NOTES
	dies ist nur ein Dummy - Modul, das zwar alle
	Funktionen exportiert, die beschrieben wurden,
	ihre Realisierung ist jedoch stark eingeschraenkt,
	sie sind auf einfache Strings zurueckgeschraubt,
	so dass die sie verwendenden Moduln wie vorher
	angesprochen werden koennen.

    BUGS

    TODO
	bei Gelegenheit sollte dieses Modul komplett neu geschrieben werden
	und mit alias.c und main.c neu abgestimmt werden
	(so dass main.c callmacro verwendet neque vero do_command)

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	10-08-94 null added *simptr, created/updated docs
	21-09-94 b_noll added DEFCMD/DEFHELP-support

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"


/**************************************
	    Globale Variable
**************************************/
Prototype UBYTE * GetArg      (int);
Prototype UBYTE * CommandName (void);


Prototype void clear_record	(void);
Prototype void start_recording	(void);
Prototype void end_recording	(void);
Prototype void replay_record	(void);
Prototype void do_saverecord	(void);
Prototype void add_record	(char * string);
Prototype void do_simptr	(void);


/**************************************
      Interne Defines & Strukturen
**************************************/
#define simptr(x,y)    do{        \
			  Mx=x;   \
			  My=y;   \
			}while(0)
#define presimptr(px,py) do{        \
			 *(px)=Mx;  \
			 *(py)=My;  \
		      }while(0)

/**************************************
	    Interne Variable
**************************************/
/* static char tempmacro_buffer[LINE_LENGTH]; */


/**************************************
	   Interne Prototypes
**************************************/
#if 0
void simptr    (WORD  px, WORD  py);
void presimptr (WORD *px, WORD *py);
#endif
extern WORD Mx, My;
	    /***********************************************
	    ***********************************************

		Dies ist lediglich eine Uebergangsloesung,
		solange wir alias.c und main.c noch nicht
		umgeschrieben haben

	    ***********************************************
	    ***********************************************/




UBYTE * CommandName (void)
{
    return (av[0]);
} /* CommandName */

UBYTE * GetArg (int num)
{
    return(av[num]);
} /* GetArg */




/*
**  MACRO RECORDER
**	we can record one input - sequence and replay it
**	we also can save it to a file and are able to source
**	that file as a normal macro-script
**
**	there are 7 functions:
**
**  recording
**	int Start_recording (void)
**	int add_record	    (char * string)
**	int end_recording   (void)
**  10-08-94 added
**	void presimptr	    (WORD *px, WORD *py);
**
**  playing
**	int replay_record   (void)
**  10-08-94 added
**	void simptr	    (WORD px, WORD py);
**
**  saving
**	int save_record     (FILE * fo)
**	    (can be read via "source")
**
*/

static struct MinList record =	/* list to contain the macros */
{
    (struct MinNode *)&record.mlh_Tail, NULL,
    (struct MinNode *)&record.mlh_Head
};
static WORD	      record_replaying	 = 0;			  /* security to avoid endless loops */

struct LongNode {
    struct MinNode node;
    WORD	   x,
		   y;
    char	   com[1];
};

/*
** clear_record
**  !this function _MUST_ be called at Program exit!
* ! >CLEARRECORD
* !
* !  clear the previously recorded macro
* !  (this command won't display an error,
* ! if there was no macro recorded)
* !
*/
void clear_record (void)
{
    struct LongNode * ln;

    while ((ln = (struct LongNode *)RemHead ((struct List *)&record))) {
	FreeMem (ln, sizeof (struct MinNode) + strlen (ln->com));
    } /* while */
} /* clear_record */

/*
** start_recording
*! >RECSTART
*!
*!  invoke the macro recorder;
*!  as long as the macro recorder is active, all
*!  "level-1" commands are stored.
*!  the recording process can be broken
*!  by calling "RECEND" (see there)
* !  or by pressing Ctl-C.
* !  (please note, that sometimes Ctl-C might be
* !  captured by loops or other breakable commands,
* !  before reaching the recorder)
*!
*/
/*DEFHELP #cmd recorder RECSTART - start the macro recorder */

DEFUSERCMD("recstart", 0, CF_VWM|CF_ICO|CF_COK, void, start_recording, (void),)
{
    if (!record_replaying) {
	if (MacroRecord == 0) {
	    clear_record();
	} /* if */

	MacroRecord ++;
    } else {
	error ("%s:\nYou can NOT start recording,\nwhile You are replaying a record", av[0]);
    } /* if */
} /* start_recording */

/*
** end_recording
*! >RECEND
*!
*!  Terminate the macro-recorder;
*/
/*DEFHELP #cmd recorder RECEND - terminate the macro recorder, if it is running */

DEFUSERCMD("recend", 0, CF_VWM|CF_ICO|CF_COK, void, end_recording, (void),)
{
    if (MacroRecord) {
	MacroRecord --;

	{
	    char ab = GETF_ABORTCOMMAND(Ep);
	    error ("recorder ended!");
	    SETF_ABORTCOMMAND(Ep, ab);
	}

#	ifdef NOT_DEF
	    /* strenggenommen muessten wir zuallererstden recorderend aus dem letzten string herausfiltern */
	    /* was aber, wenn der nicht als command sondern als macro kam ??? */
	    if (MacroRecord == ~0) {
		struct LongNode * ln;
		if (ln = (struct LongNode *)RemTail ((struct List *)&record)) {
		    strcpy (tmp_buffer, ln->com);

		    ...
		} /* if */
	    } /* if */
#	endif
 /* } else if (!record_replaying) { */
     /* ---- as long as we cannot delete endrecording from our last recorder */
     /*      line, we cannot display an error, if we encounter end_recording */
     /* error ("%s:\nYou can NOT end recording,\nif You are NOT recording", av[0]); */
    } /* if */
} /* end_recording */

/*
** add_record:
**  add a string to the recorded macro
**
*/
void add_record (char * string)
{
    if (!record_replaying) {
	struct LongNode * ln;

	if ((ln = AllocMem (sizeof (struct LongNode) + strlen (string), MEMF_ANY))) {
	    strcpy (ln->com, string);
	    presimptr (&ln->x, &ln->y);
	    AddTail ((struct List *)&record, (struct Node *)ln);
	} else {
	    nomemory ();     /* do NOT abort - we have to execute commands, too */
	} /* if */

	if (breakcheck()) {  /* that way we might have a sure record-terminator - the CTL-C signal */
	    end_recording ();
	} /* if */
    } /* if */
} /* add_record */

/*
*! >RECPLAY
*!
*!  Replay the previously recorded macro
*!
*/
/*DEFHELP #cmd recorder RECPLAY - replay the previously recorded macro */

DEFUSERCMD("recplay", 0, CF_VWM|CF_ICO|CF_COK, void, replay_record, (void),)
{
    if (record_replaying) {
	error ("%s:\nYou can NOT start replaying a record,\nwhile You are already replaying it", av[0]);
	return;
    } /* if already replaying */

    if (MacroRecord) {
	error ("%s:\nYou can NOT start replaying a record,\nwhile You are recording it", av[0]);
	return;
    } /* if */

    if (GetHead (&record)) {
	char * buffer;
	/* We use a run-time allocated buffer */
	if (!(buffer = AllocMem (LINE_LENGTH, MEMF_ANY))) {
	    nomemory ();     /* error() NEEDS memory for itself */
	    /* SETF_ABORTCOMMAND(Ep, 1); */
	    return;
	} else {
	    struct LongNode * ln;

	    for (ln = GetHead (&record); ln; ln = GetSucc (ln)) {
		strncpy (buffer, ln->com, LINE_LENGTH-1);

		simptr(ln->x, ln->y);
		do_command (buffer);

		/* You can NOT terminate Your record by errors */
		clearbreaks ();

		/* You can break Your record by pressing Ctrl-C */
		if (breakcheck ()) {
		    SETF_ABORTCOMMAND(Ep, 1);
		    break;
		} /* if */
	    } /* for */

	    FreeMem (buffer, LINE_LENGTH);
	} /* if alloced */
    } /* if ex macro */
} /* replay_record */

/*
**  save a recorder macro as a script.
*/
void save_record (FILE * fo)
{
    if (fo) {
	struct LongNode * ln;
	for (ln = GetHead (&record); ln; ln = GetSucc (ln)) {
	    if ((ln->com[0] == '\'') && !(ln->com[2]))
		sprintf (tmp_buffer, "%s\n", ln->com);
	    else
		sprintf (tmp_buffer, "simptr %d %d\n%s\n", (int)ln->x, (int)ln->y, ln->com);
	   if (fputs (tmp_buffer, fo) < 0) {
		error ("%s:\nError while writing the\ncurrent record!", av[0]);
		break;
	    } /* if */
	} /* for */
    } /* if */
} /* save_record */

/*
*! >RECSAVE filename
*!
*!  save a previously recorded macro as a XDME
*!  scripting file. The file can be read w/
*!  SOURCE (see there)
*!
*/
/*DEFHELP #cmd recorder RECSAVE filename - save the previously recorded macro as a XDME script */

DEFUSERCMD("recsave", 1, CF_VWM|CF_ICO|CF_COK, void, do_saverecord, (void),)
{
    FILE  * fo;

    if (!GetHead (&record)) {
	error ("%s:\nCan not save a record,\n if there is NO record!", av[0]);
    } else {
	if ((fo = fopen (av[1], "w"))) {
	    save_record (fo);
	    fclose (fo);
	} else {
	    error ("%s:\n Can not open File\n'%s' for output", av[0], av[1]);
	} /* if */
    } /* if */
} /* do_saverecord */


#if 0
/*
**  calculate the line/column (of the window)
**  that is represented by Mx/My (the internal Mouse Position)
*/
void  presimptr (WORD *px, WORD *py) {
    WORD cx, cy;
    extern WORD Mx, My;

    /* ---- This is a copy of do_tomouse calculation part from here ... */
    cx = (Mx-Xbase);
    if (cx >= 0) {
	if (cx < Xpixs) cx /= Xsize;
	else		cx  = -2;
    } else		cx  = -1;

    cy = (My-Ybase);
    if (cy >= 0) {
	if (cy < Ypixs) cy /= Ysize;
	else		cy  = -2;
    } else		cy  = -1;
    /* ---- ... up to here; the only Difference is, we are using -2 instead of Columns/Lines */

    if (px)
	*px = cx;
    if (py)
	*py = cy;

} /* presimptr */

/*
**  recalculate the Mouseposition from
**  Window-line/column
*/
void simptr (WORD px, WORD py) {
    extern WORD Mx, My;

    if (py == -2/* || py > Lines*/)
	py = Lines + 1;

    if (px == -2/* || px > Columns*/)
	px = Columns;

    My = py * Ysize + Ybase + (Ysize / 4);
    Mx = px * Xsize + Xbase + (Xsize / 4);

} /* simptr */
#endif

/*
*! >SIMPTR x y
*!
*!  tell XDME the mousepointer is at another position than
*!  it really is; That command is needed for XDME to execute
*!  scripts which are generated w/ RECSAVE, since it is
*!  necessary that XDME knows where to go on a TOMOUSE;
*!
**  User should NEVER call that function, unless [s]he does
**  really know, what [s]he is doing, since SIMPTR does not
**  just set the pointer to a specified position, but might
**  expect its arguments be positive Window - line/column
**  values; (please look into the source for further information)
**
*/
/*DEFHELP #cmd macros SIMPTR x y - simulate the mouse being at windowpos x/y; that command is needed to replay saved macros, it is not helpful in any other situation */

DEFUSERCMD("simptr", 2, CF_VWM|CF_COK, void, do_simptr, (void),)
{
    simptr( atoi(av[1]), atoi(av[2]) );
} /* do_simptr */




/******************************************************************************
*****  ENDE macros.c
******************************************************************************/
