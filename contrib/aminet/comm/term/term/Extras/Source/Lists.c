/*
**	Lists.c
**
**	Generic list management routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* ClearGenericList(struct GenericList *List):
	 *
	 *	Clear a generic list, free its contents.
	 */

VOID
ClearGenericList(struct GenericList *List,BOOL Notify)
{
	ObtainSemaphore(&List->ListSemaphore);

	if(List->Notify && Notify)
		(*List->Notify)(List,0);

	FreeList((struct List *)&List->ListHeader);

	List->ListNode = NULL;
	List->ListOffset = -1;
	List->ListCount = 0;

	if(List->Notify && Notify)
		(*List->Notify)(List,1);

	ReleaseSemaphore(&List->ListSemaphore);
}

	/* CreateGenericList():
	 *
	 *	Create a generic list.
	 */

struct GenericList *
CreateGenericList()
{
	struct GenericList *List;

	if(List = (struct GenericList *)AllocVecPooled(sizeof(struct GenericList),MEMF_ANY | MEMF_CLEAR))
	{
		NewList((struct List *)&List->ListHeader);

		InitSemaphore(&List->ListSemaphore);

		List->ListCount = 0;
		List->ListNode = NULL;
		List->ListOffset = -1;
	}

	return(List);
}

	/* FirstGenericListNode(struct GenericList *List):
	 *
	 *	Pick the first node in a generic list.
	 */

struct Node *
FirstGenericListNode(struct GenericList *List)
{
	struct Node *Node;

	SafeObtainSemaphoreShared(&List->ListSemaphore);

	if(!IsListEmpty((struct List *)List))
	{
		Node = List->ListNode = (struct Node *)List->ListHeader.mlh_Head;

		List->ListOffset = 0;
	}
	else
		Node = NULL;

	ReleaseSemaphore(&List->ListSemaphore);

	return(Node);
}

	/* LastGenericListNode(struct GenericList *List):
	 *
	 *	Pick the last node in a generic list.
	 */

struct Node *
LastGenericListNode(struct GenericList *List)
{
	struct Node *Node;

	SafeObtainSemaphoreShared(&List->ListSemaphore);

	if(!IsListEmpty((struct List *)List))
	{
		Node = List->ListNode = (struct Node *)List->ListHeader.mlh_TailPred;

		List->ListOffset = List->ListCount - 1;
	}
	else
		Node = NULL;

	ReleaseSemaphore(&List->ListSemaphore);

	return(Node);
}

	/* NextGenericListNode(struct GenericList *List):
	 *
	 *	Pick the next successive node in a generic list.
	 */

struct Node *
NextGenericListNode(struct GenericList *List)
{
	struct Node *Node;

	SafeObtainSemaphoreShared(&List->ListSemaphore);

	if(List->ListNode)
	{
		if(List->ListNode->ln_Succ->ln_Succ)
		{
			Node = List->ListNode = List->ListNode->ln_Succ;

			List->ListOffset++;
		}
		else
			Node = NULL;

		ReleaseSemaphore(&List->ListSemaphore);
	}
	else
	{
		ReleaseSemaphore(&List->ListSemaphore);

		Node = FirstGenericListNode(List);
	}

	return(Node);
}

	/* PrevGenericListNode(struct GenericList *List):
	 *
	 *	Pick the preceding node in a generic list.
	 */

struct Node *
PrevGenericListNode(struct GenericList *List)
{
	struct Node *Node;

	SafeObtainSemaphoreShared(&List->ListSemaphore);

	if(List->ListNode)
	{
		if(List->ListNode->ln_Pred->ln_Pred)
		{
			Node = List->ListNode = List->ListNode->ln_Pred;

			List->ListOffset--;
		}
		else
			Node = NULL;

		ReleaseSemaphore(&List->ListSemaphore);
	}
	else
	{
		ReleaseSemaphore(&List->ListSemaphore);

		Node = LastGenericListNode(List);
	}

	return(Node);
}

	/* DeleteGenericListNode(struct GenericList *List,struct Node *Node):
	 *
	 *	Delete a single node from a generic list.
	 */

struct Node *
DeleteGenericListNode(struct GenericList *List,struct Node *Node,BOOL Notify)
{
	struct Node *Result;

	ObtainSemaphore(&List->ListSemaphore);

	if(List->Notify && Notify)
		(*List->Notify)(List,0);

	if(!Node)
		Node = List->ListNode;

	if(Node)
	{
		if(Node == List->ListNode)
		{
			if(Node->ln_Succ->ln_Succ)
				List->ListNode = Node->ln_Succ;
			else
			{
				if(Node->ln_Pred->ln_Pred)
				{
					List->ListNode = Node->ln_Pred;
					List->ListOffset--;
				}
				else
				{
					List->ListNode = NULL;
					List->ListOffset = -1;
				}
			}

			Remove(Node);
		}
		else
		{
			Remove(Node);

			if(List->ListNode)
			{
				struct Node *LocalNode;
				LONG i;

				for(i = 0, LocalNode = (struct Node *)List->ListHeader.mlh_Head ; i < List->ListCount - 1 ; i++, LocalNode = LocalNode->ln_Succ)
				{
					if(LocalNode == List->ListNode)
					{
						List->ListOffset = i;
						break;
					}
				}
			}
		}

		FreeVecPooled(Node);

		List->ListCount--;
	}

	if(List->Notify && Notify)
		(*List->Notify)(List,1);

	Result = List->ListNode;

	ReleaseSemaphore(&List->ListSemaphore);

	return(Result);
}

	/* CreateGenericListNode(LONG Size,STRPTR Name):
	 *
	 *	Create a new generic list node.
	 */

struct Node *
CreateGenericListNode(LONG Size,STRPTR Name)
{
	struct Node	*Node;
	LONG		 Head;

	if(Size < sizeof(struct Node))
		Head = Size = sizeof(struct Node);
	else
		Head = Size;

	if(Name)
		Size += strlen(Name) + 1;

	if(Node = (struct Node *)AllocVecPooled(Size,MEMF_ANY | MEMF_CLEAR))
	{
		if(Name)
		{
			Node->ln_Name = ((char *)Node) + Head;

			strcpy(Node->ln_Name,Name);
		}
		else
			Node->ln_Name = NULL;
	}

	return(Node);
}

	/* AddGenericListNode(struct GenericList *List,struct Node *Node,BYTE Mode):
	 *
	 *	Add a node to a generic list.
	 */

VOID
AddGenericListNode(struct GenericList *List,struct Node *Node,LONG Mode,BOOL Notify)
{
	LONG Position = 0;

	ObtainSemaphore(&List->ListSemaphore);

	if(List->Notify && Notify)
		(*List->Notify)(List,0);

	switch(Mode)
	{
		case ADD_GLIST_BOTTOM:

			AddTail((struct List *)&List->ListHeader,Node);
			Position = List->ListCount;
			break;

		case ADD_GLIST_TOP:

			AddHead((struct List *)&List->ListHeader,Node);
			break;

		case ADD_GLIST_BEHIND:

			if(List->ListNode)
			{
				Insert((struct List *)&List->ListHeader,Node,List->ListNode);
				Position = List->ListOffset + 1;
			}
			else
			{
				AddTail((struct List *)&List->ListHeader,Node);
				Position = List->ListCount;
			}

			break;

		case ADD_GLIST_BEFORE:

			if(List->ListNode && List->ListNode != (struct Node *)List->ListHeader.mlh_Head)
			{
				Insert((struct List *)&List->ListHeader,Node,List->ListNode->ln_Pred);
				Position = List->ListOffset;
			}
			else
				AddHead((struct List *)&List->ListHeader,Node);

			break;
	}

	List->ListNode = Node;
	List->ListOffset = Position;
	List->ListCount++;

	if(List->Notify && Notify)
		(*List->Notify)(List,1);

	ReleaseSemaphore(&List->ListSemaphore);
}

	/* GenericListCount(struct GenericList *List):
	 *
	 *	Return the number of generic list entries.
	 */

LONG
GenericListCount(struct GenericList *List)
{
	LONG Count;

	SafeObtainSemaphoreShared(&List->ListSemaphore);

	Count = List->ListCount;

	ReleaseSemaphore(&List->ListSemaphore);

	return(Count);
}

	/* RemoveFirstGenericListNode(struct GenericList *List):
	 *
	 *	Removes the first node of a generic list and
	 *	returns it. This is similar to RemHead().
	 */

struct Node *
RemoveFirstGenericListNode(struct GenericList *List)
{
	struct Node *Node;

	ObtainSemaphore(&List->ListSemaphore);

	if(List->Notify)
		(*List->Notify)(List,0);

	if(Node = RemHead((struct List *)&List->ListHeader))
	{
		List->ListCount--;

		if(Node == List->ListNode)
		{
			List->ListNode = NULL;
			List->ListOffset = -1;
		}
		else
			List->ListOffset--;
	}

	if(List->Notify)
		(*List->Notify)(List,1);

	ReleaseSemaphore(&List->ListSemaphore);

	return(Node);
}

VOID
LockGenericList(struct GenericList *List)
{
	if(List)
		ObtainSemaphore(&List->ListSemaphore);
}

VOID
SharedLockGenericList(struct GenericList *List)
{
	if(List)
		SafeObtainSemaphoreShared(&List->ListSemaphore);
}

VOID
UnlockGenericList(struct GenericList *List)
{
	if(List)
		ReleaseSemaphore(&List->ListSemaphore);
}

VOID
SelectGenericListNode(struct GenericList *List,struct Node *Node,LONG Offset)
{
	ObtainSemaphore(&List->ListSemaphore);

	if(Node)
	{
		struct Node *ListNode;
		LONG i;

		Offset = -1;

		for(i = 0, ListNode = (struct Node *)List->ListHeader.mlh_Head ; i < List->ListCount ; i++, ListNode = ListNode->ln_Succ)
		{
			if(ListNode == Node)
			{
				Offset = i;
				break;
			}
		}
	}
	else
		Node = GetListNode(Offset,(struct List *)List);

	if(Node != NULL && Offset >= 0)
	{
		List->ListNode		= Node;
		List->ListOffset	= Offset;
	}
	else
	{
		List->ListNode		= NULL;
		List->ListOffset	= -1;
	}

	ReleaseSemaphore(&List->ListSemaphore);
}
