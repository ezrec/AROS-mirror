/*
**	PhoneList.c
**
**	Phonebook list support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

struct List *
CreateRegularPhoneList(PhonebookHandle *PhoneHandle)
{
	struct List *LocalList;

	if(LocalList = CreateList())
	{
		if(PhoneHandle->Phonebook && PhoneHandle->NumPhoneEntries > 0)
		{
			PhoneEntry **Phonebook;
			PhoneNode *Node;
			LONG i;

			Phonebook = PhoneHandle->Phonebook;

			for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
			{
				if(Node = CreatePhoneNode(Phonebook[i],FALSE))
				{
					LimitedStrcpy(sizeof(Node->LocalName),Node->LocalName,Phonebook[i]->Header->Name);

					AddTail(LocalList,(struct Node *)Node);
				}
				else
				{
					DeleteList(LocalList);

					return(NULL);
				}
			}
		}
	}

	return(LocalList);
}

PhoneNode *
CreatePhoneNode(PhoneEntry *Entry,BOOL InitName)
{
	if(Entry)
	{
		PhoneNode *Node;

		if(Node = (PhoneNode *)AllocVecPooled(sizeof(PhoneNode),MEMF_ANY|MEMF_CLEAR))
		{
				/* Set up the node name */

			Node->Node.ln_Name = Node->LocalName;

			if(InitName)
				LimitedSPrintf(sizeof(Node->LocalName),Node->LocalName,"      %s",Entry->Header->Name);

				/* Install the back link */

			Node->Entry = Entry;

			return(Node);
		}
	}

	return(NULL);
}

VOID
UpdatePhoneNode(PhoneNode *Node)
{
	if(Node)
	{
		PhoneEntry *Entry = Node->Entry;

		if(Entry->Count < 0)
			LimitedSPrintf(sizeof(Node->LocalName),Node->LocalName,"      %s",Entry->Header->Name);
		else
			LimitedSPrintf(sizeof(Node->LocalName),Node->LocalName,"%3ld - %s",1 + Entry->Count,Entry->Header->Name);

			/* Update the group node as well */

		if(Entry->GroupNode)
			LimitedStrcpy(sizeof(Entry->GroupNode->LocalName),Entry->GroupNode->LocalName,Node->LocalName);
	}
}

VOID
UpdatePhoneList(PhonebookHandle *PhoneHandle)
{
	if(PhoneHandle->PhoneList)
	{
		PhoneNode *Node;

		for(Node = (PhoneNode *)PhoneHandle->PhoneList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
			UpdatePhoneNode(Node);
	}
}

BOOL
InitPhoneList(PhonebookHandle *PhoneHandle)
{
	struct List *LocalList;

		/* Create the list */

	if(LocalList = CreateList())
	{
			/* Do we have a list to fill? */

		if(PhoneHandle->Phonebook && PhoneHandle->NumPhoneEntries)
		{
			PhoneEntry **Phonebook;
			PhoneNode *Node;
			LONG i;

			Phonebook = PhoneHandle->Phonebook;

				/* Create a node for every phonebook entry */

			for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
			{
				if(Node = CreatePhoneNode(Phonebook[i],FALSE))
					AddTail(LocalList,(struct Node *)Node);
				else
				{
					DeleteList(LocalList);

					return(FALSE);
				}
			}

				/* Discard the old list if any */

			DeleteList(PhoneHandle->PhoneList);

				/* Now initialize the nodes */

			for(Node = (PhoneNode *)LocalList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
			{
					/* Initialize the back link */

				Node->Entry->Node = Node;

					/* Update its name. */

				UpdatePhoneNode(Node);
			}

			PhoneHandle->PhoneList = LocalList;

			return(TRUE);
		}
		else
		{
			DeleteList(PhoneHandle->PhoneList);
			PhoneHandle->PhoneList = LocalList;

			return(TRUE);
		}
	}

	return(FALSE);
}

VOID
DeletePhoneList(PhonebookHandle *PhoneHandle)
{
	if(PhoneHandle->PhoneList)
	{
		PhoneNode *Node;

		for(Node = (PhoneNode *)PhoneHandle->PhoneList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
		{
			if(Node->Entry->Node == Node)
				Node->Entry->Node = NULL;
		}

		DeleteList(PhoneHandle->PhoneList);
		PhoneHandle->PhoneList = NULL;
	}
}

	/* SortPhoneEntries():
	 *
	 *	Sorts the current phone list array in ascending order.
	 */

VOID
SortPhoneList(PhonebookHandle *PhoneHandle,LONG How,BOOL ReverseOrder)
{
	if(PhoneHandle->NumPhoneEntries > 1 && PhoneHandle->PhoneList)
	{
		PhoneEntry **Phonebook;
		LONG i;

		qsort(PhoneHandle->Phonebook,PhoneHandle->NumPhoneEntries,sizeof(PhoneEntry *),GetSortFunc(How,ReverseOrder));

		Phonebook = PhoneHandle->Phonebook;

		NewList(PhoneHandle->PhoneList);

		for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
			AddTail(PhoneHandle->PhoneList,(struct Node *)Phonebook[i]->Node);
	}
}
