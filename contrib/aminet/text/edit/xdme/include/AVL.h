
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
#ifndef   EXEC_NODES_H
#include <exec/nodes.h>
#endif

/**************************************
	    Global Variables
**************************************/

/* none */

/**************************************
	    Defines & Structures
**************************************/

struct TreeNode {
        struct Node tn_Node;
};

#define tn_Left tn_Node.ln_Succ
#define tn_Right tn_Node.ln_Pred
#define tn_Type tn_Node.ln_Type
#define tn_Balance tn_Node.ln_Pri
#define tn_Name tn_Node.ln_Name

#if 0
struct TreeNode {		      /* that struct should exactly fit into a Node */
	struct TreeNode * tn_Left;
	struct TreeNode * tn_Right;
	UBYTE		  tn_Type;
      BYTE		tn_Balance; /* !PRIVATE! for AVL - do not use it! */
	char		* tn_Name;
}; /* struct TreeNode */
#endif

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

