
/******************************************************************************

    MODULE
	AVL.c

    DESCRIPTION
	Header file for AVL-trees

    HISTORY
	05-07-93 b_noll created
	$Date$ last update

******************************************************************************/

#ifndef AVL_H
#define AVL_H

/**************************************
	    Includes
**************************************/

/* for BYTE, UBYTE */
#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

/**************************************
	    Global Variables
**************************************/

/* none */

/**************************************
	    Defines & Structures
**************************************/

struct TreeNode {		      /* that struct should exactly fit into a Node */
	struct TreeNode * tn_Left;
	struct TreeNode * tn_Right;
	UBYTE		  tn_Type;
      BYTE		tn_Balance; /* !PRIVATE! for AVL - do not use it! */
	char		* tn_Name;
}; /* struct TreeNode */

#define AVL_SCAN_PRAEFIX  0
#define AVL_SCAN_INFIX	  1
#define AVL_SCAN_POSTFIX -1


/**************************************
	    Macros
**************************************/

/* none */

/**************************************
	    Prototypes
**************************************/

struct TreeNode* AVL_Find     (struct TreeNode** tree, const char* name);
struct TreeNode* AVL_Append   (struct TreeNode** tree, struct TreeNode* obj);
struct TreeNode* AVL_Remove   (struct TreeNode** tree, struct TreeNode* obj);
void		 AVL_FreeTree (struct TreeNode** tree, void (*freefunc)(APTR));
void		 AVL_ScanTree (struct TreeNode** tree, void (*scanfunc)(APTR), char mode);

#endif /* AVL_H */

/******************************************************************************
*****  END AVL.c
******************************************************************************/

