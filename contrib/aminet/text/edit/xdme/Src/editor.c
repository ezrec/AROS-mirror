/******************************************************************************

    MODUL
	$Header$

    DESCRIPTION
	This file contains several commands that make the use of
	the ED structure more transparent. The use of the commands in
	here is to collect as many graphics operations as possible
	and execute them all at once.

    NOTES

    BUGS

    TODO

    EXAMPLES

    SEE ALSO

    INDEX

******************************************************************************/

/**************************************
		Includes
**************************************/
#include <defs.h>
#include <editor.h>


/**************************************
	    Globale Variable
**************************************/


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/


/**************************************
	   Interne Prototypes
**************************************/


/*****************************************************************************

    NAME
	SetLine

    SYNOPSIS
	Line line = SetLine (ED * ep, Line line, int mode);

    FUNCTION
	Sets the current line to <line>. 0 <= <line> < GetLines(ep). The
	function truncates <line> to a valid range and makes sure that
	the line is visible.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	26. Sep 1994	digulla created

******************************************************************************/

DEFCMD(Line, SetLine, (ED * ep, Line line, int mode))
{
    /* clip <line> to valid value */
    if (line < 0)
	line = 0;
    else if (line >= GetLines(ep))
	line = GetLines(ep)-1;

    /* is already the current line ? Yes: do nothing */
    if (line != GetLine(ep))
    {
	Line topline;

	ep->newline  = line; /* set new line */
	ep->df_line |= 1;   /* Flag: line has changed */

	/* <line> outside visible bounds ? yes: scroll */
	if (line < GetTopline(ep) || line >= GetTopline(ep)+Lines)
	{
	    topline = GetTopline (ep);

	    if (mode == ADJUST_MIN)
		topline += line - GetLine(ep);
	    else
		topline = line-Lines/2;

	    /* clip to bounds */
	    if (topline < 0)
		topline = 0;
	    else if (topline >= GetLines (ep))
		topline = GetLines(ep)-1;

	    ep->newtopline  = topline;
	    ep->df_topline |= 1;    /* Flag: topline has changed */
	}
    }

/* printf ("New Line: %d  New Topline %d\n", ep->newline, ep->newtopline); */

    return line;
} /* SetLine */


DEFCMD(Line, AddToLine, (ED * ep, int distance))
{
    return (SetLine (ep, GetLine(ep)+distance,
	    (distance == 1 || distance == -1) ? ADJUST_MIN : ADJUST_CENTER));
} /* AddToLine */


/*****************************************************************************

    NAME
	UpdateED

    SYNOPSIS
	void UpdateED (ED * ep);

    FUNCTION
	Process the flags and update the fields and the display.

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	26. Sep 1994	digulla created

******************************************************************************/

#define CURSOR_OFF if (cursor_on) { cursor_on = 0; text_cursor (0); }

DEFCMD (void, UpdateEd, (ED * ep))
{
    int cursor_on = 1;

    if (ep->df_topline)
    {
	if (GetTopline(ep) != ep->newtopline)
	{
	    CURSOR_OFF
	    ScrollAndUpdate (0, ep->newtopline-GetTopline(ep));
	}

	ep->df_topline = 0;
    }

    if (ep->df_line)
    {
	if (GetLine(ep) != ep->newline)
	{
	    CURSOR_OFF
	    text_sync ();
	    ep->line = ep->newline;
	    text_load ();
	}

	ep->df_line = 0;
    }

    if (cursor_on <= 0)
	text_cursor (1);

} /* UpdateED */


/******************************************************************************
*****  ENDE editor.c
******************************************************************************/
