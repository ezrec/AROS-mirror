#ifndef	__DLIST_H__
#define	__DLIST_H__

/* dlist.h - linked list routines by Harry Sintonen <sintonen@iki.fi>

   faster swapnodes() by Jamie van den Berge <entity@vapor.com>
   mergesortlist() by Fabio Alemagna <falemagn@aros.org>

   MinList.c and MinList.h are public domain as long name of all contributers
   still appear in both files.
*/

#include <stdio.h>
#include <exec/lists.h>

#if 0
struct dnode
{
	struct dnode *n_succ;
	struct dnode *n_pred;
};

struct dlist
{
	struct dnode *l_head;
	struct dnode *l_tail;
	struct dnode *l_tailpred;
};
#endif

/* callback function type for callfornode()
*/
typedef int (*LISTFUNCPTR) (struct MinNode *, void *);

/* return value for LISTFUNCPTR:
*/
#define LFR_CONTINUE    0   /* continue list travelsal */
#define LFR_BREAK       1   /* stop list travelsal and return current node */

/* flags for callfornode()
*/
#define	CFNF_BACKWARDS	(1 << 0)

/* callback function type for qsortlist()
*/
typedef int (*COMPARFUNCPTR) (struct MinNode*, struct MinNode *);


#ifdef USE_QSL_ALL

/* use this as qsortlist() nmemb to sort all nodes from base
*/
#define	QSL_ALL		0xffffffff

#endif


#define	LISTEMPTY(l) \
((l)->mlh_TailPred == (struct MinNode *) (l))

#define	CNEWLIST(l) \
{(struct MinNode *) &(l)->mlh_Tail, NULL, (struct MinNode *) &(l)->mlh_Head}


#ifdef __cplusplus
extern "C" {
#endif


void newlist(
	struct MinList *l);

void addhead(
	struct MinList *l,
	struct MinNode *n);

void addtail(
	struct MinList *l,
	struct MinNode *n);

void insert(
	struct MinList *l,
	struct MinNode *n,
	struct MinNode *afterof);

struct MinNode *remhead(
	struct MinList *l);

struct MinNode *remtail(
	struct MinList *l);

void remnode(
	struct MinNode *n);

struct MinNode *finMinNode(
	struct MinList *l,
	struct MinNode *n);

struct MinNode *callfornode(
	struct MinList *l,
	LISTFUNCPTR func,
	void *userdata,
	unsigned long int flags);

void qsortlist(
	struct MinList *l,
	struct MinNode *base,
	unsigned long int nmemb,
	COMPARFUNCPTR compar);

void mergesortlist(
	struct MinList *l,
	COMPARFUNCPTR compar);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* __DLIST_H__ */
