/*
**	PhoneGroup.c
**
**	Phonebook group support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

VOID
DeletePhoneGroupNode(PhoneGroupNode *GroupNode)
{
	PhoneNode *Node;

		/* Delete the group links */

	while(Node = (PhoneNode *)RemHead((struct List *)&GroupNode->GroupList))
	{
		Node->Entry->ThisGroup = NULL;
		Node->Entry->GroupNode = NULL;

		FreeVecPooled(Node);
	}

		/* Delete group node */

	FreeVecPooled(GroupNode);
}

VOID
FreePhoneGroupList(struct List *List)
{
	PhoneGroupNode *Node;

		/* Delete all the group nodes */

	while(Node = (PhoneGroupNode *)RemHead(List))
		DeletePhoneGroupNode(Node);
}

	/* InsertSorted(struct List *List,struct Node *Node):
	 *
	 *	Insert a node into a linked list whilst keeping the
	 *	list sorted in alphabetic order.
	 */

VOID
InsertSorted(struct List *List,struct Node *Node)
{
	struct Node	*Other;
	STRPTR		 Name;

	Name = Node->ln_Name;

	for(Other = List->lh_Head ; Other->ln_Succ ; Other = Other->ln_Succ)
	{
		if(Stricmp(Name,Other->ln_Name) <= 0)
		{
			Insert(List,Node,Other->ln_Pred);

			return;
		}
	}

	AddTail(List,Node);
}

PhoneGroupNode *
CreatePhoneGroup(PhonebookHandle *PhoneHandle,STRPTR Name)
{
	PhoneGroupNode *Node;

		/* Check if the name already exists */

	for(Node = (PhoneGroupNode *)PhoneHandle->PhoneGroupList.mlh_Head ; Node->Node.ln_Succ ; Node = (PhoneGroupNode *)Node->Node.ln_Succ)
	{
		if(!Stricmp(Node->Node.ln_Name,Name))
			return(Node);
	}

		/* Create the new group */

	if(Node = (PhoneGroupNode *)AllocVecPooled(sizeof(PhoneGroupNode),MEMF_ANY|MEMF_CLEAR))
	{
			/* Set up the node name */

		strcpy(Node->Node.ln_Name = Node->LocalName,Name);

			/* Clear the list */

		NewList((struct List *)&Node->GroupList);

			/* Add the group to the global list */

		InsertSorted((struct List *)&PhoneHandle->PhoneGroupList,(struct Node *)Node);
	}

	return(Node);
}

VOID
RemoveGroupEntry(PhoneEntry *Entry)
{
	if(Entry->ThisGroup)
	{
		PhoneNode *Node = Entry->GroupNode;

		Remove((struct Node *)Node);

		Entry->ThisGroup = NULL;
	}
}

VOID
DeleteGroupEntry(PhoneEntry *Entry)
{
	if(Entry && Entry->GroupNode)
	{
		PhoneNode *Node = Entry->GroupNode;

		FreeVecPooled(Node);

		Entry->GroupNode = NULL;
	}
}

PhoneNode *
AddGroupEntry(PhoneGroupNode *NewGroup,PhoneEntry *Entry)
{
		/* Is this entry already in a group? */

	if(Entry->ThisGroup)
	{
			/* Is it already in the group it should be placed in? */

		if(Entry->ThisGroup == NewGroup)
			return(Entry->GroupNode);					/* Just return the group then */
		else
			Remove((struct Node *)Entry->GroupNode);	/* Remove it from the old group */
	}
	else
	{
		PhoneNode *Node;

			/* Ok, so it isn't in a group yet. We'll create a new group */
			/* node for it. */

		if(Node = CreatePhoneNode(Entry,TRUE))
		{
				/* This points to the group the entry is in. One entry can be */
				/* only in one single group */

			Entry->GroupNode = Node;
		}
		else
			return(NULL);
	}

		/* Add the entry to the group list. Group lists are by default */
		/* kept in alphabetically sorted order. */

	InsertSorted((struct List *)&NewGroup->GroupList,(struct Node *)Entry->GroupNode);

		/* Remember the group to which this entry belongs now */

	Entry->ThisGroup = NewGroup;

	return(Entry->GroupNode);
}

STATIC VOID
InsertGroupSorted(struct List *List,PhoneNode *Node,SORTFUNC SortFunc)
{
	PhoneNode	*Other;
	STRPTR		 Name;

	Name = Node->Node.ln_Name;

	for(Other = (PhoneNode *)List->lh_Head ; Other->Node.ln_Succ ; Other = (PhoneNode *)Other->Node.ln_Succ)
	{
		if((*SortFunc)(Name,Other->Node.ln_Name) <= 0)
		{
			Insert(List,(struct Node *)Node,Other->Node.ln_Pred);

			return;
		}
	}

	AddTail(List,(struct Node *)Node);
}

VOID
SortGroupList(struct List *List,LONG How,BOOL ReverseOrder)
{
	LONG Count = GetListSize(List);

	if(Count > 1)
	{
		PhoneEntry **Table;

		if(!(Table = (PhoneEntry **)AllocVecPooled(sizeof(PhoneEntry *) * Count,MEMF_ANY)))
		{
			struct List LocalList;
			PhoneNode *Node;
			SORTFUNC SortFunc;

			NewList(&LocalList);
			MoveList(List,&LocalList);
			SortFunc = GetSortFunc(How,ReverseOrder);

			while(Node = (PhoneNode *)RemHead(&LocalList))
				InsertGroupSorted(List,Node,SortFunc);
		}
		else
		{
			PhoneNode *Node;
			LONG i;

			for(i = 0, Node = (PhoneNode *)List->lh_Head ; Node->Node.ln_Succ ; i++, Node = (PhoneNode *)Node->Node.ln_Succ)
				Table[i] = Node->Entry;

			qsort(Table,Count,sizeof(PhoneEntry *),GetSortFunc(How,ReverseOrder));

			NewList(List);

			for(i = 0 ; i < Count ; i++)
				AddTail(List,(struct Node *)Table[i]->GroupNode);

			FreeVecPooled(Table);
		}
	}
}
