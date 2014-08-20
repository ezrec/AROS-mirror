/* dlist.c - linked list routines by Harry Sintonen <sintonen@iki.fi>

   faster swapnodes() by Jamie van den Berge <entity@vapor.com>
   mergesortlist() by Fabio Alemagna <falemagn@aros.org>

   dlist.c and dlist.h are public domain as long name of all contributers
   still appear in both files.
*/

/* HISTORY

   2107A1 - cleaned up the source & the header file. placed in public
            domain. - Harry

   1012A1 - added qsortlist() routine. - Harry

   2204A3 - added faster swapnodes() by Jamie van den Berge
            <entity@vapor.com>. - Harry

   1512A3 - fixed LISTEMPTY macro. - Harry

   1703A4 - added mergesortlist by Fabio Alemagna
            <falemagn@aros.org>. - Harry


   NOTES:

   API design was very stronly influenced by AmigaOS exec list routines,
   since I am very familiar with them. No documentation is included, the
   use is too obvious (at least for me, oh well... ;-) - Harry
*/

#include <stddef.h>
#include "dlist.h"



#ifdef __cplusplus
extern "C" {
#endif


void newlist(
	struct MinList *l)
{
	l->mlh_Head	  = (struct MinNode *) &l->mlh_Tail;
	l->mlh_Tail	  = NULL;
	l->mlh_TailPred	  = (struct MinNode *) &l->mlh_Head;
}


void addhead(
	struct MinList *l,
	struct MinNode *n)
{
	n->mln_Succ		  = l->mlh_Head;
	n->mln_Pred		  = (struct MinNode *) &l->mlh_Head;
	l->mlh_Head->mln_Pred	= n;
	l->mlh_Head		  = n;
}


void addtail(
	struct MinList *l,
	struct MinNode *n)
{
	n->mln_Succ		  = (struct MinNode *) &l->mlh_Tail;
	n->mln_Pred		  = l->mlh_TailPred;
	l->mlh_TailPred->mln_Succ	= n;
	l->mlh_TailPred		  = n;
}


void insert(
	struct MinList *l,
	struct MinNode *n,
	struct MinNode *afterof)
{
	if (afterof == NULL)
	{
		addhead(l, n);
		return;
	}

	n->mln_Pred		  = afterof;
	n->mln_Succ		  = afterof->mln_Succ;
	afterof->mln_Succ	  = n;
	n->mln_Succ->mln_Pred	= n;
}


struct MinNode *remhead(
	struct MinList *l)
{
	struct MinNode *n = NULL;

	/* anything to remove?
	*/
	if (!LISTEMPTY(l))
	{
		n = l->mlh_Head;
		n->mln_Succ->mln_Pred = n->mln_Pred;
		n->mln_Pred->mln_Succ = n->mln_Succ;
	}

	return n;
}


struct MinNode *remtail(
	struct MinList *l)
{
	struct MinNode *n = NULL;

	/* anything to remove?
	*/
	if (!LISTEMPTY(l))
	{
		n = l->mlh_TailPred;
		n->mln_Succ->mln_Pred = n->mln_Pred;
		n->mln_Pred->mln_Succ = n->mln_Succ;
	}

	return n;
}


void remnode(
	struct MinNode *n)
{
	n->mln_Succ->mln_Pred = n->mln_Pred;
	n->mln_Pred->mln_Succ = n->mln_Succ;
}


static inline
struct MinNode *mslmerge(
	struct MinNode *l,
	COMPARFUNCPTR compar)
{
	struct MinNode *l1, *last_l1, *l2, *last_l2, *next_l;
	struct MinNode *first = NULL, **first_ptr, **last_ptr = &first;

	l1 = l;

	/* l1 points to the 1st sublist, l2 points to the 2nd.

	   Should there be no l2, we don't need to do anything special, as
	   l1 will already be linked with the rest of the list AND it won't 
	   obviously need to be merged with another list.
	*/
	while (l1 && (l2 = (last_l1 = l1->mln_Pred)->mln_Succ))
	{
		last_l2 = l2->mln_Pred;

		next_l  = last_l2->mln_Succ;

		/* This will make the below loop slightly faster, since there will only
		   be tests against the constant NULL.
		*/
		last_l1->mln_Succ = NULL;
		last_l2->mln_Succ = NULL;

		/* Pointer to the beginning of the merged sublist
		*/
		first_ptr = last_ptr;
		do
		{
			if ((*compar)(l1, l2) < 0)
			{
				l1->mln_Pred = (struct MinNode *)((char *)last_ptr -
				             offsetof(struct MinNode, mln_Succ));
				*last_ptr  = l1;
				l1         = l1->mln_Succ;
			}
			else
			{
				l2->mln_Pred = (struct MinNode *)((char *)last_ptr -
				             offsetof(struct MinNode, mln_Succ));
				*last_ptr  = l2;
				l2         = l2->mln_Succ;
			}

			last_ptr = &(*last_ptr)->mln_Succ;

		} while (l1 && l2);

		if (l1)
		{
			l1->mln_Pred = (struct MinNode *)((char *)last_ptr -
			             offsetof(struct MinNode, mln_Succ));

			*last_ptr            = l1;
			(*first_ptr)->mln_Pred = last_l1;
			last_ptr             = &last_l1->mln_Succ;
		}
		else if (l2)
		{
			l2->mln_Pred = (struct MinNode *)((char *)last_ptr -
			             offsetof(struct MinNode, mln_Succ));

			*last_ptr            = l2;
			(*first_ptr)->mln_Pred = last_l2;
			last_ptr             = &last_l2->mln_Succ;
		}
		else
		{
			(*first_ptr)->mln_Pred = (struct MinNode *)((char *)last_ptr -
			                       offsetof(struct MinNode, mln_Succ));
		}

		l1 = *last_ptr = next_l;
	}

	return first;
}

void mergesortlist(
	struct MinList *l,
	COMPARFUNCPTR compar)
{
	struct MinNode *head, *tail;

	struct MinNode *l1, *l2, *first, **last_ptr;

	if (!l)
		return;

	head = l->mlh_Head;
	tail = l->mlh_TailPred;

	if (tail == (struct MinNode *) l || head == tail)
		return;
	
	tail->mln_Succ = NULL;
	last_ptr = &first;

	/* The mslmerge() function requires a list of sublists, each of which
	   has to be a circular list. Since the given list doesn't have these 
	   properties, we need to divide the sorting algorithm in 2 parts:

	       1) we first go trough the list once, making every node's Pred pointer
	          point to the node itself, so that the given list of n nodes is
	          transformed in a list of n circular sublists. Here we do the merging
	          "manually", without the help of the mslmerge() function, as we have
	          to deal with just couples of nodes, thus we can do some extra
	          optimization.

	       2) We then feed the resulting list to the mslmerge() function, as many
	          times as it takes to the mslmerge() function to give back just one
	          circular list, rather than a list of circular sublists: that will be
	          our sorted list.
	*/

	/* This is the first part.
	*/
	l1 = head;
	l2 = l1->mln_Succ;
	do
	{
		/* It can happen that the 2 nodes are already in the right,
		   order and thus we only need to make a circular list out
		   of them, or their order needs to be reversed, but
		   in either case, the below line is necessary, because:

		       1) In the first case, it serves to build the 
		          circular list.

		       2) In the 2nd case, it does hald the job of
		          reversing the order of the nodes (the 
		          other half is done inside the if block).
		*/
		l1->mln_Pred = l2;

		if ((*compar)(l1, l2) >= 0)
		{
			/* l2 comes before l1, so rearrange them and
			   make a circular list out of them.
			*/
			l1->mln_Succ = l2->mln_Succ;
			l2->mln_Succ = l1;
			l2->mln_Pred = l1;

			l1 = l2;
		}
	
		*last_ptr = l1;
		last_ptr  = &l1->mln_Pred->mln_Succ;
		l1        = *last_ptr;

	} while (l1 && (l2 = l1->mln_Succ));

	/* An orphan node? Add it at the end of the list of sublists and 
	   make a circular list out of it.
	*/
	if (l1)
	{
		l1->mln_Pred = l1;
		*last_ptr  = l1;
	}

	/* And this is the 2nd part.
	*/
	while (first->mln_Pred->mln_Succ)
		first = mslmerge(first, compar);

	/* Now we fix up the list header.
	*/
	l->mlh_Head     = first;
	l->mlh_TailPred = first->mln_Pred;
	first->mln_Pred->mln_Succ = (struct MinNode *)&l->mlh_Tail;
	first->mln_Pred = (struct MinNode *)&l->mlh_Head;
}

#ifdef __cplusplus
} // extern "C"
#endif
