/******************************************************************************

    MODUL
	keyaddes.c

    DESCRIPTION
	additional keyboard support commands, which do
	not need keyhashes.c

    NOTES
	we only import keycodes.c

    BUGS

    TODO
	(better) filtering commands

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	16 Dec 1992 b_null created

******************************************************************************/

/**************************************
		Includes
**************************************/
#define  KEY_INTERNAL		    /* PATCH_NULL < 24-06-94 */
#include "defs.h"
extern BOOL a2iqual (const UBYTE *str, ULONG *piqual); /* keycodes.c */


/**************************************
	    Globale Variable
**************************************/
Prototype ULONG ext_qualifiers;
Prototype int  qualifier (char *);


/**************************************
      Interne Defines & Strukturen
**************************************/


/**************************************
	    Interne Variable
**************************************/
ULONG ext_qualifiers = 0;


/**************************************
	   Interne Prototypes
**************************************/


/*****************************************************************************

    NAME
	qualifier

    PARAMETER
	char * string

    RESULT
	success

    RETURN
	int

    DESCRIPTION
	manually set some (additional) qualifiers without
	respect to the keyboard;
	it is possible to set/add/remove/AND/XOR some quals
	due to the 1st char of string

    NOTES
	make sure unused qualifiers do not disturb keycontrol.c

    BUGS
	using ^ may cause definition of nused qualifiers,
	but that should really not cause errors

    EXAMPLES

    SEE ALSO
	keycodes.c/a2iqual

    INTERNALS

    HISTORY
	16 Dec 1992 b_null created

******************************************************************************/

int qualifier (char * string)
{
    ULONG newquals = 0;
    char mode;

    if (!string) {
	return (0);
    } /* if */

    switch (*string) {
    case '+': mode =  1; string++; break;  /* add some additional quals */
    case '-': mode =  2; string++; break;  /* remove some additional quals */
    case '*': mode =  3; string++; break;  /* AND some additional quals */
    case '/': mode =  4; string++; break;  /* XOR some additional quals */
    default:  mode =  0;	   break;  /* overwrite the old quals */
    } /* switch */

    if (!a2iqual(string, &newquals)) {
	 return (0);
    } /* if */

    switch (mode) { /* see above */
    case 0: ext_qualifiers  =  newquals; break;
    case 1: ext_qualifiers |=  newquals; break;
    case 2: ext_qualifiers &= ~newquals; break;
    case 3: ext_qualifiers &=  newquals; break;
    case 4: ext_qualifiers ^=  newquals; break; /* highly dangerous: we may define unused bits */
    } /* switch */

    return (1);
} /* qualifier */




/******************************************************************************
*****  ENDE keyaddes.c
******************************************************************************/

