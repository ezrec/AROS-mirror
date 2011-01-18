/******************************************************************************

    MODUL
	menucontrol.c

    DESCRIPTION
	[X]DME intuition interface to menustrips.c

    NOTES

    BUGS
	none known

    TODO
	handling for MENU_HELP

    EXAMPLES

    SEE ALSO
	mainloop

    INDEX

    HISTORY
	18 Dec 1992 b_null created
	11 Feb 1993 modifcations
	$Date$ last update

******************************************************************************/

/**************************************
		Includes
**************************************/
#include "defs.h"
#include "menubase.h"


/**************************************
	    Globale Variable
**************************************/
Prototype APTR menu_cmd (struct IntuiMessage * im);
/* Prototype void menu_cmd (struct IntuiMessage * im); */
Prototype void *active_menu;


/**************************************
      Interne Defines & Strukturen
**************************************/

typedef struct INode {
    struct MinNode node;
    XITEM	 * item;
    int 	   len;
    char	   com[0];  /* that means : the rest of that node contains a string */
} IINODE;


/**************************************
	    Interne Variable
**************************************/

void *active_menu = NULL;

/**************************************
	   Interne Prototypes
**************************************/


#ifndef NOT_DEF

/*****************************************************************************

    NAME
	menu_cmd

    PARAMETER
	struct IntuiMessage * im

    RESULT
	-/-

    RETURN
	void

    DESCRIPTION
	work on all menuselections done;
	first set them all into a new queue, that does not
	change if we modify currentmenu, then pass 'em to the
	parser (or better a duplicate of them)

    NOTES
	that method disables use of MENU_HELP
	we should be able to do almost everything to our menus, also during
	work on multiselections!

	THAT FUNCTION SHOULD BE RE-ENTRANT

    BUGS
	what, if we close our window during that operation ????

	WARN: that function was neither compiled nor tested up to now
	      (we still use the function at the end of this file)

    EXAMPLES

    SEE ALSO
	main/main

    INTERNALS
	our method is horrible - it takes lots of resources,
	but I think, this is the easiest way (else we had to lock our menustrip
	and then queue up all operations to it)

    HISTORY
	18 Dec 1992 b_null created
	09 Feb 1993 b_null multiselection added
	11 Feb 1993 b_null BUG FIX : all selections are done "buffered"

******************************************************************************/

APTR /* void */ menu_cmd (struct IntuiMessage * im)
{
    MENUSTRIP * ms = currentmenu();
    struct MinList buffs;

    NewList ((struct List *)&buffs);
    if (im != NULL && im->Class == IDCMP_MENUPICK && ms != NULL) {
	long	       code = im->Code;
	XITEM	     * item;
	IINODE	     * in;

	while ((item = (XITEM *)ItemAddress (ms->data, code))) {
	    if (item->com != NULL) {
		int len = sizeof (IINODE) + strlen (item->com) + 1;

		if ((in = AllocFunc (len, MEMF_ANY))) {
		    in->len = len;		 /* we MUST remember the size here, as do_command might destroy the string-contents */
		    strcpy (in->com, item->com);
		    in->item = item;
		    AddTail ((struct List *)&buffs, (struct Node *)in);
		    /* if (item->Flags & CHECKED)
			in->len = -in->len; */
		} /* if malloced */
	    } /* if command carrier */
	    code = item->item.NextSelect;
	} /* while pending selections */


	while ((in = (IINODE *)RemHead ((struct List *)&buffs))) {
	    active_menu = in->item;
	    do_command (in->com);
	    FreeFunc (in, /* ABS */ (in->len));
	} /* while commands left in queue */
	active_menu = NULL;

#ifdef NOT_DEF
	/* that method has one serious bug: we can not be sure to hit the right items */
	char buffer[LINE_LENGTH];
	while (item = (XITEM *)ItemAddress (ms->data, code)) {
	    if (item->com) {
		strcpy (buffer, item->com);
		do_command (buffer);
		clearbreaks ();
	    } /* if item->com */
	} /* while item */
#endif
    } /* if im */

    return (NULL);
} /* menu_cmd */

#else

/*****************************************************************************

    RESULT
	address of a command that is referred by the
	selected menu;

    RETURN
	APTR

    DESCRIPTION
	we simply check if the command was a correct
	menu-selection and return some data that was
	stored together with the Intuition-structure

    NOTES
	we don not know, the structure of that data;
	so user might have to call macroname, macrobody
	or something like that before he is getting the wanted data

    INTERNALS
	falls korrekter Menueaufruf
	    gib entsprechendes Macro zurueck
	sonst gib NULL zurueck

******************************************************************************/

APTR menu_cmd (struct IntuiMessage * im)
{
    XITEM *item;

    if (item = (XITEM *)ItemAddress (((MENUSTRIP *)currentmenu())->data, im->Code)) {
	return (item->com);
    } /* if found item */
    return (NULL);
} /* menu_cmd */

#endif

/******************************************************************************
*****  ENDE menucontrol.c
******************************************************************************/
