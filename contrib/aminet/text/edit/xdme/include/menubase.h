/******************************************************************************

    MODUL
	menubase.h

    DESCRIPTION
	basic definitions for our menu modules

******************************************************************************/

#ifndef MENUBASE_H
#define MENUBASE_H

/**************************************
		Includes
**************************************/
#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef   EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_NODES_H */

#ifndef   INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif /* INTUITION_INTUITION_H */


/**************************************
	    Globale Variable
**************************************/


/**************************************
	Defines und Strukturen
**************************************/

/* typedef struct Menu MENU; */

typedef struct MenuStrip {
    struct Node   node; 	    /* node for Usage in the list of menustrips */
    struct Menu * data; 	    /* ptr to Intuition-Menus	*/
    long	  offCount;	    /* stack of menuoff-calls	*/
} MENUSTRIP;

typedef struct XItem {
    struct MenuItem item;	    /* Intuition structure */
    char	  * com;	    /* command-string	   */
    char	  * help;	    /* help-string	   */
    union {			    /* item.ItemFill	   */
	struct Image	 im;
	struct IntuiText it;
    }		    fill;	    /* same size */
} XITEM;


#define   BAR	  ((void *)(~NULL))


/**************************************
	       Prototypes
**************************************/


#endif /* MENUBASE_H */

/******************************************************************************
*****  ENDE menubase.h
******************************************************************************/

