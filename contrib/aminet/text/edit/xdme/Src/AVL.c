#define XDME
/******************************************************************************

    MODULE
	AVL.c

    DESCRIPTION
	simple implementation for (recursive) AVL-trees
	this code can be used for an Amiga(c) shared library

    NOTES
	the nodes do need valid C-strings;
	currently, there is no possibility
	to change search/comparison and keys
	anything is done via strcmp.

	if You wanna use that code as a non-shared library
	You might have to modify some "#define"s below; also
	it might be useful to re"#define" A0,A1,D0 and PRE.
	(see below (search for LIB_TYPE))

	compile w/ #define ANY_DEBUG to get basic error-checking
		   #define AVL_DEBUG to get almost full-featured error-checking
	    in either case You must initialize an function-var called
	    extern void (*error)(char*, char*, char*);
	    which displays our errors;

    BUGS
	that implementation on AVL - as is - does not
	create any errormessages; user must make sure,
	not to mess up w/ the trees, nor using two nodes
	of the same name;

    TODO
	maybe certain Header structures,
	to support other comparison functions
	and error functions

    EXAMPLES
	for AVL-trees there are soooo many examples in other pgms

    SEE ALSO

    INDEX

    HISTORY
	05-07-93 b_noll created
	12-07-93 b_noll added ScanTree, FreeTree
	21-07-93 b_noll changed switch()'s to if()'s
	22-07-93 b_noll disabled all error-calls
	07-10-93 b_noll [re]activated LSON/RSON/NAME/BAL-Macros
	07-11-93 b_noll fixed bug in rem_rek (there were no reports)
	12-05-94 b_noll joined xdme-verion and lib-version
	04-09-94 b_noll BUGFIX in AVL_balance
	$Date$ last update

******************************************************************************/

/**************************************
	    Includes
**************************************/

#ifndef   AVL_H
#include <AVL.h>
#endif /* AVL_H */

#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef   STRINGS_H
#include <strings.h>
#endif /* STRINGS_H */

#ifndef   EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_TYPES_H */

#define MYDEBUG 0
#include "debug.h"

/**************************************
	    Global Variables
**************************************/

#ifdef XDME
#define   ERROR(x,y,z) error(x,y,z)
extern void error(const char*, ...);
#define   ANY_DEBUG 1
#else
#define   ERROR(x,y,z) if (error) (*error)(x,y,z)
void (*error)(char const *, char const *, char const *) = NULL;
#endif


/**************************************
	    Internal Defines & Structures
**************************************/


/**************************************
	    Internal Variables
**************************************/

/* none */

/**************************************
	    Internal Prototypes
**************************************/

/* static int AVL_rem_rek (struct TreeNode**, struct TreeNode*); */
/* static int AVL_add_rek (struct TreeNode**, struct TreeNode*); */
/* static int AVL_balance (struct TreeNode**); */

/**************************************
	    Macros
**************************************/

/**************************************************
**
**  !!	LIB_TYPE  !!
**
**  Change the following lines to switch
**  between a shared-library and a link-library
**  #if 1 -> SHARED
**  #if 0 ->LINK
**
**************************************************/
#if 0
/* SHARED LIB: */
#define AVL_Find     LIBAVL_Find
#define AVL_Append   LIBAVL_Append
#define AVL_Remove   LIBAVL_Remove
#define AVL_FreeTree LIBAVL_FreeTree
#define AVL_ScanTree LIBAVL_ScanTree
#define PRE __asm __saveds
#define A0(x) register __a0 x
#define A1(x) register __a1 x
#define D0(x) register __d0 x
#else
/* LINK LIB: */
#define PRE
#define A0(x) x
#define A1(x) x
#define D0(x) x
#endif
/*************************************************/

#define INC(x) ++(x)
#define DEC(x) --(x)

#define RSON(x) (x)->tn_Right
#define LSON(x) (x)->tn_Left
#define BLNC(x) (x)->tn_Balance
#define NAME(x) (x)->tn_Name

#define BAL(x) BLNC(x)

#define AVL_SCAN_PRAEFIX  0
#define AVL_SCAN_INFIX	  1
#define AVL_SCAN_POSTFIX -1

/**************************************
	    Implementation
**************************************/

#ifdef	  ANY_DEBUG
const char* AVL_errdisp1 = "AVL:\n%s!\n";
const char* AVL_errdisp2 = "AVL:\n%s %s!\n";

const char* AVL_err_notree = "without a tree";
const char* AVL_err_cotree = "with a corrupt tree";
const char* AVL_err_multi  = "Can not manage multiple nodes\nof same name in one tree";

const char* AVL_err_balan  = "called Balance";
const char* AVL_err_rem    = "called Remove";
const char* AVL_err_add    = "called Append";
const char* AVL_err_find   = "called Find";
const char* AVL_err_free   = "called FreeTree";
const char* AVL_err_scan   = "called ScanTree";
#endif /* ANY_DEBUG */


/*****************************************************************************

    NAME
	AVL_balance

    PARAMETER
	struct TreeNode** t;

    RESULT
	the balance of the new root (? really)

    RETURN
	int;

    DESCRIPTION
	do a local rebalance if the current node's
	balance is worse than +/- 1

	the function is used by AVL_add_rek and
	by AVL_rem_rek on the way back from a
	addition/deletion;

    NOTES
	that rebalance affect only up to three nodes;

	the function might have been done in a more
	effective way - but I wanna be able to read it
	also in 2 weeks.

    BUGS
	none known

    EXAMPLES
	why - its hidden

    SEE ALSO
	AVL_add_rek, AVL_rem_rek

    INTERNALS
	verschachtelte case-construkte,
	um alle normalen Faelle abzudecken,
	die bei AVL_Append und AVL_Remove
	auftreten koennen;
	den Rest habe in mir mit Bildchen
	klargemacht.

	Hier mehr zu schreiben hat m.E. keinen
	Sinn, das geht besser mit Papier und Bleistift;

    HISTORY
	05-07-93    b_noll  created

******************************************************************************/
static
int AVL_balance (struct TreeNode** t)
{
    struct TreeNode* ot = *t;
    struct TreeNode* lt = LSON(ot);
    struct TreeNode* rt = RSON(ot);
    char	     b	= BAL(ot);

    if (!ot) {
	ERROR (AVL_errdisp2, AVL_err_balan, "with NULL ptr");
	return 0;
    }


/* D(bug("AVL:balancing...")); */

    switch (b) {

#ifdef AVL_DEBUG
    case -1:
    case  0:
    case +1:
	ERROR (AVL_errdisp2, AVL_err_balan, "without need");
	return (b);
#endif /* AVL_DEBUG */

    case +2:
	b = BAL(lt);
	switch (b) {
	case -1:
/* D(bug("LR")); */
	    rt = RSON(lt);
	    *t = rt;
	    LSON(ot) = RSON(rt);
	    RSON(lt) = LSON(rt);
	    LSON(rt) = lt;
	    RSON(rt) = ot;


	    b	    = BAL(rt);
	    BAL(rt) = 0;
	    BAL(lt) =  (1-b)/2; /* -:1 0:0 +:0 */
	    BAL(ot) = -(1+b)/2; /*   0   0  -1 */

	    return 0;

	case  0:
	case +1: /* LL */
/* D(bug("LL")); */
	    *t = lt;
	    LSON(ot) = RSON(lt);
	    RSON(lt) = ot;

	    BAL(ot) = -(b-1); /* 0:+1; +1:0 */
	    BAL(lt) = +(b-1); /*   -1;    0 */

	    return b-1;

#ifdef AVL_DEBUG
	default:
	    ERROR ( AVL_errdisp2, AVL_err_balan, AVL_err_cotree);
#endif /* AVL_DEBUG */
	} /* switch */

	break;

    case -2:
	b = BAL(rt);
	switch (b) {
	case +1:
/* D(bug("RL")); */
	    lt = LSON(rt);
	    *t = lt;
	    RSON(ot) = LSON(lt);
	    LSON(rt) = RSON(lt);
	    LSON(lt) = ot;
	    RSON(lt) = rt;

	    b	    = BAL(lt);
	    BAL(lt) = 0;
	    BAL(rt) = -(1+b)/2; /* -:0 0:0 +: */
	    BAL(ot) =  (1-b)/2; /*   1   0    */
/* D(bug("b== %d\n",b)); */

	    return 0;

	case  0:
	case -1: /* RR */

/* D(bug("RR")); */
	    *t = rt;
	    RSON(ot) = LSON(rt);
	    LSON(rt) = ot;

	    /* Vorzeichen ??? */
	    BAL(ot) = -(b+1); /* 0:-1; -1:0 */
	    BAL(rt) = +(b+1); /*   +1;    0 */
/* D(bug("ot%d rt%d\n", -(b+1), (b+1))); */
	    return -(b-1);

#ifdef AVL_DEBUG
	default:
	    ERROR ( AVL_errdisp2, AVL_err_balan, AVL_err_cotree);
#endif /* AVL_DEBUG */
	} /* switch */
	break;

#ifdef AVL_DEBUG
    default:
	ERROR ( AVL_errdisp2, AVL_err_balan, AVL_err_cotree);
#endif /* AVL_DEBUG */
    } /* switch */

    return 0;
} /* AVL_balance */



/*****************************************************************************

    NAME
	AVL_swap_best_right

    PARAMETER
	struct TreeNode** swapme;

    RESULT
	swapping was necessary and done

    RETURN
	BOOL;

    DESCRIPTION
	swap the current node (swapme) with the
	leftmost follower/son of its right son

    NOTES
	swapping with the rightmost follower of its left son
	would have a similar effect;

	perhaps we should first check for a better balance to
	decide which one to use (we have to rename the function then)

    BUGS
	none known

    EXAMPLES
	why - its hidden

    SEE ALSO
	AVL_rem_rek

    INTERNALS
	die function ist in 2 Teile zerlegbar:
	die Suche nach dem "optimalen" Nachfolger
	danach das Vertauschen dieses Nachfolgers
	mit dem Root.

    HISTORY
	05-07-93    b_noll  created

******************************************************************************/
static
BOOL AVL_swap_best_right (struct TreeNode** swapme)
{
    struct TreeNode* root = *swapme;
    struct TreeNode* curr = root;
    struct TreeNode* prev = NULL;
    struct TreeNode* z;

/* D(bug("AVL:swappin' ...")); */

    { /* search the mostleft node of the right son */
	if ((curr = RSON(root)) == NULL) {
	    return FALSE;
	} /* if */


	while ((z = LSON(curr))) {
	    prev = curr;
	    curr = z;
	} /* while */
    }

    { /* swap our node and the leftmost right son */
	z = RSON(curr);
	if (prev) {
	    LSON(prev) = root;
	    RSON(curr) = RSON(root);
	} else {
	    RSON(curr) = root;
	} /* if */

	RSON(root) = z;

	*swapme    = curr;
	LSON(curr) = LSON(root);
	LSON(root) = NULL;
    }


    { /* swap the balances, too! */
	char bal  = BAL(root);
	BAL(root) = BAL(curr);
	BAL(curr) = bal;
    }

    return TRUE;
} /* AVL_swap_best_right */



/*****************************************************************************

    NAME
	AVL_rem_rek

    PARAMETER
	struct TreeNode** tree;
	struct TreeNode*  obj;

    RESULT
	The Tree's change of height

    RETURN
	int;

    DESCRIPTION
	hidden part of AVL_Remove

    NOTES
	no security checks involved;

    BUGS
	use of 2 nodes w/ the same Id will
	lead into trouble

    EXAMPLES
	why - its hidden

    SEE ALSO
	AVL_Remove

    INTERNALS
	the deletion is done recursively

	zunaechst suchen wir nach dem Knoten,
	den wir herausloesen wollen (nur ueber
	strcmp!) bringen den dann an eine
	Position, wo wir loeschen koennen
	und knipsen ihn dort heraus;
	auf dem Rueckweg wird der Baum bei Bedarf
	rebalaniert

    HISTORY
	05-07-93    b_noll  created
	21-07-93    b_noll  switch(strcmp) -> if(diff) to handle w/ SAS

******************************************************************************/
static
int AVL_rem_rek (struct TreeNode** tree, struct TreeNode* obj)
{

    struct TreeNode* n	 = *tree;
    int 	     diff;

    if (!n) {
#ifdef	  ANY_DEBUG
	ERROR ( AVL_errdisp1, "Can't remove non-added node", NULL);
#endif /* ANY_DEBUG */
	return 0;
    } /* if */

    if ((diff = strcmp (NAME(obj), NAME(n))) == 0) {
	if (n != obj) {
#ifdef	  ANY_DEBUG
	    ERROR ( AVL_errdisp1, AVL_err_multi, NULL);
#endif /* ANY_DEBUG */
	    return 0;
	} /* if */

	if (AVL_swap_best_right(tree)) { /* noch nich' am boden */
	    n = *tree;
	    goto remright;	    /* ab hier koennte es beschleunigt werden */
	} else {		    /* schon am rand - abschneiden */
	    *tree = LSON(n);        /* PATCH_NULL BUGFIX [07 Nov 1993] : Right <-> Left */
	    return 1;
	} /* if am rand */

    } else if (diff > 0) {
remright:
	if ( AVL_rem_rek(&RSON(n), obj) ) {
	    INC(BAL(n));
	    if (BAL(n) == 2) {
		AVL_balance(tree);
	    } /* if */
	    return !BAL(*tree);
	} /* if */
	return 0;

    } else {
	if ( AVL_rem_rek(&LSON(n), obj) ) {
	    DEC(BAL(n));
	    if (BAL(n) == -2) {
		AVL_balance(tree);
	    } /* if */
	    return !BAL(*tree);
	} /* if */
	return 0;

    } /* if */
} /* AVL_rem_rek */


/*****************************************************************************

    NAME
	AVL_add_rek

    PARAMETER
	struct TreeNode** tree;
	struct TreeNode*  obj;

    RESULT
	The Tree's change of height

    RETURN
	int;

    DESCRIPTION
	hidden part of AVL_Append

    NOTES
	no security checks involved;

    BUGS
	none known

    EXAMPLES
	why - its hidden

    SEE ALSO
	AVL_Append

    INTERNALS
	the addition is done recursively

	wir gehen solange nach unten, bis
	wir auf ein NIL-kind stossen und
	haengen unseren Node dorthin;
	auf dem Rueckweg wird der Baum bei Bedarf
	rebalaniert

    HISTORY
	05-07-93    b_noll  created
	21-07-93    b_noll  switch(strcmp) -> if(diff) to handle w/ SAS

******************************************************************************/
static
int AVL_add_rek (struct TreeNode** tree, struct TreeNode* obj)
{
    struct TreeNode* n	 = *tree;
    int 	     diff;

    if (!n) {
	*tree = obj;
	RSON(obj) = NULL;
	LSON(obj) = NULL;
	BAL(obj)  = 0;
	return 1;
    } /* if */

    if ((diff = strcmp (NAME(obj), NAME(n))) == 0) {
#ifdef	 ANY_DEBUG
	ERROR ( AVL_errdisp1, AVL_err_multi, NULL);
#endif /* ANY_DEBUG */
	return 0;

    } else if (diff > 0) {
	if (AVL_add_rek(&RSON(n), obj)) {
	    DEC(BAL(n));
	    if (BAL(n) == -2) {
		AVL_balance(tree);
	    } else {
		return BAL(n);
	    } /* if */
	} /* if */
	return 0;

    } else {
	if (AVL_add_rek(&LSON(n), obj)) {
	    INC(BAL(n));
	    if (BAL(n) == 2) {
		AVL_balance(tree);
	    } else {
		return BAL(n);
	    } /* if */
	} /* if */
	return 0;

    } /* if */
} /* AVL_add_rek */



/*****************************************************************************

    NAME
	AVL_Find

    PARAMETER
	A0( struct TreeNode** tree );
	A1( const char*       name );

    RESULT
	the treenode, that matches name - if exists

    RETURN
	PRE struct TreeNode*;

    DESCRIPTION
	search for name in the tree

    NOTES
	search is done with strcmp()

	** Be Aware - You must give the Tree's ADDRESS **
	(that would not have been necessary, here, but
	so I can show a little bit more consistency,
	since AVL_Append and AVL_Remove do need it)

    BUGS
	none known

    EXAMPLES

    SEE ALSO
	AVL_Append, AVL_Remove

    INTERNALS
	search is done iteratively

    HISTORY
	05-07-93    b_noll  created

******************************************************************************/

PRE struct TreeNode* AVL_Find (A0( struct TreeNode** tree ), A1( const char* name ))
{
    int 	     diff;
    struct TreeNode* n;
DL;

    if (!tree) {
#ifdef	 AVL_DEBUG
	ERROR ( AVL_errdisp2, AVL_err_find, AVL_err_notree);
#endif /* AVL_DEBUG */
	return NULL;
    } /* if */

    n = *tree;

D(bug("tree=%p n=%p\n", tree, n));
DL;
if (n)
    D(bug ("name=%s\n", NAME(n)));
    while ((n != NULL) && (diff = strcmp (name, NAME(n)))) {
DL;
	if (diff > 0) {
	    n = RSON(n);
	} else {
	    n = LSON(n);
	} /* if */
DL;
D(bug("n=%p\n", tree, n));
if (n)
    D(bug ("name=%s\n", NAME(n)));
    } /* while */

DL;
    return n;
} /* AVL_Find */



/*****************************************************************************

    NAME
	AVL_Remove

    PARAMETER
	A0( struct TreeNode** tree   );
	A1( struct TreeNode*  remove );

    RESULT
	*tree == the tree after Deleting the obsolete node

    RETURN
	PRE struct TreeNode*;

    DESCRIPTION
	Remove a node from an avl-tree; the right
	position is searched with strcmp()

    NOTES
	U first have to search for the node which
	is to be deleted e.g.:
	AVL_Rem(&tree, FindTreeNode(&tree, name ));

	** Be Aware - You must give the Tree's ADDRESS **

    BUGS
	none known

    EXAMPLES

    SEE ALSO
	AVL_Find, AVL_Append, AVL_rem_rek

    INTERNALS
	the deletion is actually done by AVL_rem_rek
	AVL_Remove is only something like a security-
	interface

    HISTORY
	05-07-93    b_noll  created

******************************************************************************/

PRE struct TreeNode* AVL_Remove (A0( struct TreeNode** tree ), A1( struct TreeNode* remove ))
{
    if (!tree) {
#ifdef	 AVL_DEBUG
	ERROR ( AVL_errdisp2, AVL_err_rem, AVL_err_notree);
#endif /* AVL_DEBUG */
	return NULL;
    } /* if */

    if (!(*tree))
	return NULL;

    if (remove)
	AVL_rem_rek (tree, remove);

    return *tree;
} /* AVL_Remove */




/*****************************************************************************

    NAME
	AVL_Append

    PARAMETER
	A0( struct TreeNode** tree );
	A1( struct TreeNode*  new  );

    RESULT
	*tree == the tree after Appending the new node

    RETURN
	PRE struct TreeNode*;

    DESCRIPTION
	Append a new node to an avl-tree; the right
	position is calculated with strcmp()

    NOTES
	U cannot define two nodes with the same
	name in one tree

	** Be Aware - You must give the Tree's ADDRESS **

    BUGS
	none known

    EXAMPLES

    SEE ALSO
	AVL_Remove, AVL_add_rek

    INTERNALS
	the Addition is actually done by AVL_add_rek
	AVL_Append is only something like a security-
	interface

    HISTORY
	05-07-93    b_noll  created

******************************************************************************/

PRE struct TreeNode* AVL_Append (A0( struct TreeNode** tree ), A1( struct TreeNode* new ))
{
    if (!tree) {
#ifdef	 AVL_DEBUG
	ERROR ( AVL_errdisp2, AVL_err_add, AVL_err_notree);
#endif /* AVL_DEBUG */
	return NULL;
    } /* if */

    if (new)
	AVL_add_rek (tree, new);

    return *tree;
} /* AVL_Append */



/*****************************************************************************

    NAME
	AVL_FreeTree

    PARAMETER
	A0( struct TreeNode** tree );
	A1( void (*freefunc)(A0( APTR )) );

    RESULT
	-/-

    RETURN
	PRE void

    DESCRIPTION
	free all entries of a certain tree;

    NOTES
	** Be Aware - You must give the Tree's ADDRESS **

	** Be Aware - You must not delete a partial tree,
				    but only whole trees **

    BUGS
	none known

    EXAMPLES

    SEE ALSO

    INTERNALS
	the tree is completely deleted, without respect to
	any balance; we expect to call that function only on
	root-nodes;

    HISTORY
	12-07-93    b_noll  created

******************************************************************************/

PRE void AVL_FreeTree (A0( struct TreeNode** tree ), A1( void (*freefunc)(A0( APTR )) ))
{
    if (tree) {
	if (*tree) {
	    AVL_FreeTree(&(LSON(*tree)),  freefunc);
	    AVL_FreeTree(&(RSON(*tree)), freefunc);
	    (*freefunc) (*tree);
	    *tree = NULL;
	} /* if */
#ifdef	 AVL_DEBUG
    } else {
	ERROR ( AVL_errdisp2, AVL_err_free, AVL_err_notree);
#endif /* AVL_DEBUG */
    } /* if */
} /* AVL_FreeTree */



/*****************************************************************************

    NAME
	AVL_ScanTree

    PARAMETER
	A0( struct TreeNode** tree );
	A1( void (*scanfunc)(A0( APTR )) );
	D0( char mode );

    RESULT
	-/-

    RETURN
	PRE void

    DESCRIPTION
	call a certain function on all nodes of a certain tree

    NOTES
	** Be Aware - You must give the Tree's ADDRESS **

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	12-07-93    b_noll  created

******************************************************************************/

PRE void AVL_ScanTree (A0( struct TreeNode** tree ), A1( void (*scanfunc)(A0( APTR )) ), D0( char mode ))
{
    if (tree) {
	if (*tree) {
	    if (mode == AVL_SCAN_PRAEFIX) (*scanfunc) (*tree);
	    AVL_ScanTree(&(LSON(*tree)),  scanfunc, mode);
	    if (mode == AVL_SCAN_INFIX  ) (*scanfunc) (*tree);
	    AVL_ScanTree(&(RSON(*tree)), scanfunc, mode);
	    if (mode == AVL_SCAN_POSTFIX) (*scanfunc) (*tree);
	} /* if */
#ifdef	 AVL_DEBUG
    } else {
	ERROR ( AVL_errdisp2, AVL_err_scan, AVL_err_notree);
#endif /* AVL_DEBUG */
    } /* if */
} /* AVL_ScanTree */




/******************************************************************************
*****  END AVL.c
******************************************************************************/

