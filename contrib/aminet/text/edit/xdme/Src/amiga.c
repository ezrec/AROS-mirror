/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Amiga specific things

    HISTORY
	10. Oct 1992	ada created
	$Log$
	Revision 1.1  2001/10/06 20:11:51  digulla
	Initial revision

 * Revision 1.4  1994/12/22  09:16:33  digulla
 * Makros und DEFCMD eingeführt
 *
 * Revision 1.3  1994/09/09  12:31:30  digulla
 * added new style Prototypes, DEFCMD and DEFHELP
 *
 * Revision 1.2  1994/08/30  11:07:20  digulla
 * added some parentheses for GCC
 *
 * Revision 1.1  1994/08/14  12:27:53  digulla
 * Initial revision
 *

******************************************************************************/

/**************************************
		Includes
**************************************/
#include <defs.h>


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
	set_pubscreen

    PARAMETER
	void

    RESULT

    RETURN
	void

    DESCRIPTION
	Sets the name for the pubscreen XDME should open its windows on.

    NOTES

    BUGS
	This is only a dummy (Sorry).

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	10. Oct 1992	ada created

******************************************************************************/

/*DEFHELP #cmd prefs PUBSCREEN name - open next window on screen @{B}name@{UB}. Use an empty string to turn it off (ie. "pubscreen `'") */

DEFUSERCMD("pubscreen", 1, CF_VWM|CF_ICO,void, set_pubscreen, (void),)
{
    static char * name = NULL;

    /* If there is already a name, free it first */

    if (name)
    {
	free (name);
	name = NULL;
    }

    /* If there is a new name, get it */

    if (av[1][0])
    {
	/* Make a copy of the name */
	if ((name = strdup (av[1])))
	{
	    if (XDMEArgs.publicscreenname)
		free (XDMEArgs.publicscreenname);

	    XDMEArgs.publicscreenname = name;

	    /* give user a hint what we did */

	    if (name)
		title ("Set PubScreen to `%s'", name);
	}
	else
	    warn ("PUBSCREEN: out of memory !");
    } else
    {
	/* Disable PubScreens */

	if (XDMEArgs.publicscreenname)
	    free (XDMEArgs.publicscreenname);

	XDMEArgs.publicscreenname = NULL;

	title ("turned PubScreen OFF");
    }
} /* set_pubscreen */


/******************************************************************************
*****  ENDE amiga.c
******************************************************************************/
