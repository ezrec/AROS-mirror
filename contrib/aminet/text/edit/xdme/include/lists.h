/******************************************************************************

    MODUL
	lists.h

    DESCRIPTION
	Tools/Macros fuer Exec-Listen

******************************************************************************/

#ifndef LISTS_H
#define LISTS_H

/***************************************
	       Includes
***************************************/
#ifndef EXEC_LISTS_H
#   include <exec/lists.h>
#endif


/***************************************
     Globale bzw. externe Variable
***************************************/


/***************************************
	 Defines und Strukturen
***************************************/
/* DICE defines these (but this file hides the other definition) */
#ifndef __AROS__
#define GetHead(list)       ((APTR) \
	(((struct List *)(list))->lh_Head->ln_Succ ? \
		((struct List *)(list))->lh_Head : NULL ))
#define GetTail(list)       ((APTR) \
	(((struct List *)(list))->lh_TailPred->ln_Succ ? \
		((struct List *)(list))->lh_TailPred : NULL ))
#define GetPred(node)       ((APTR) \
	(((struct Node *)(node))->ln_Pred->ln_Pred ? \
		((struct Node *)(node))->ln_Pred : NULL ))
#define GetSucc(node)       ((APTR) \
	(((struct Node *)(node))->ln_Succ->ln_Succ ? \
		((struct Node *)(node))->ln_Succ : NULL ))
#endif

/***************************************
	       Prototypes
***************************************/


#endif /* LISTS_H */

/******************************************************************************
*****  ENDE lists.h
******************************************************************************/
