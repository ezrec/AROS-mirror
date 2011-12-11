/*
**	DialList.c
**
**	Dial list support routines
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
ToggleDialEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry)
{
	if(Entry->Count < 0)
		MarkDialEntry(PhoneHandle,Entry);
	else
		UnmarkDialEntry(PhoneHandle,Entry);
}

VOID
MarkDialEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry)
{
	if(Entry->Count < 0 && Entry->Header->Number[0] && Entry->Header->Name[0])
	{
			/* Another entry for the dial list */

		Entry->Count = PhoneHandle->DialMarker++;

			/* Update the name */

		UpdatePhoneNode(Entry->Node);
	}
}

VOID
UnmarkDialEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry)
{
	if(Entry && Entry->Count >= 0)
	{
		struct PhoneEntry **Phonebook = PhoneHandle->Phonebook;
		LONG Count = Entry->Count,i;

			/* Update all phonebook entries to show the order in which */
			/* entries are to go into the dial list */

		for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
		{
			if(Phonebook[i]->Count > Count)
			{
					/* Decrement the index if it's larger than the one */
					/* to be removed */

				Phonebook[i]->Count--;

					/* Update the name. */

				UpdatePhoneNode(Phonebook[i]->Node);
			}
		}

			/* Reset the index; this entry is not to go into the dial list */

		Entry->Count = -1;

			/* Update the name */

		UpdatePhoneNode(Entry->Node);

			/* One entry less to go into the dial list */

		PhoneHandle->DialMarker--;
	}
}

/****************************************************************************/

STATIC struct DialNode *
FindNumberInDialList(struct List *List,STRPTR Number)
{
	struct DialNode *Node;

	for(Node = (struct DialNode *)List->lh_Head ; Node->Node.ln_Succ ; Node = (struct DialNode *)Node->Node.ln_Succ)
	{
		if(!Stricmp(Node->Number,Number))
			return(Node);
	}

	return(NULL);
}

STATIC STRPTR
ExtractNextItem(STRPTR From,STRPTR To,LONG ToLen)
{
	if(From)
	{
		LONG i,Len,Start,NumberLen;

		Len = strlen(From);

		for(i = Start = 0 ; i < Len ; i++)
		{
			if(From[i] == '|' && From[i + 1] != '|')
			{
				NumberLen = i - Start;

				if(NumberLen > ToLen - 1)
					NumberLen = ToLen - 1;

				if(NumberLen > 0)
					CopyMem(&From[Start],To,NumberLen);

				To[NumberLen] = 0;

				Start = i + 1;

				while(Start < Len && From[Start] == '|')
					Start++;

				if(Start == Len)
					return(NULL);
				else
					return(&From[Start]);
			}
		}

		/* No trailing bar? */

		if(Start >= Len)
			To[0] = 0;
		else
		{
			NumberLen = Len - Start;

			if(NumberLen > ToLen - 1)
				NumberLen = ToLen - 1;

			if(NumberLen > 0)
				CopyMem(&From[Start],To,NumberLen);

			To[NumberLen] = 0;
		}
	}

	return(NULL);
}

STATIC STRPTR
CopyAndUpdate(STRPTR To,STRPTR From)
{
	LONG Len;

	Len = strlen(From) + 1;

	CopyMem(From,To,Len);

	return(To + Len);
}

STATIC struct DialNode *
CreateDialNode(PhoneEntry *Entry,STRPTR Number,STRPTR InitCommand,STRPTR ExitCommand,STRPTR DialPrefix,STRPTR DialSuffix)
{
	if(Number == NULL)
		return(NULL);
	else
	{
		struct DialNode *Node;
		LONG Len;

		Len = strlen(Number) + 1;

		if(InitCommand)
			Len += strlen(InitCommand) + 1;

		if(ExitCommand)
			Len += strlen(ExitCommand) + 1;

		if(DialPrefix)
			Len += strlen(DialPrefix) + 1;

		if(DialSuffix)
			Len += strlen(DialSuffix) + 1;

		if(Node = (struct DialNode *)AllocVecPooled(sizeof(struct DialNode) + Len,MEMF_ANY|MEMF_CLEAR))
		{
			STRPTR String;

			String = (STRPTR)(Node + 1);

				/* Set up the number */

			String = CopyAndUpdate(Node->Number = String,Number);

			if(InitCommand)
				String = CopyAndUpdate(Node->InitCommand = String,InitCommand);

			if(ExitCommand)
				String = CopyAndUpdate(Node->ExitCommand = String,ExitCommand);

			if(DialPrefix)
				String = CopyAndUpdate(Node->DialPrefix = String,DialPrefix);

			if(DialSuffix)
				CopyAndUpdate(Node->DialSuffix = String,DialSuffix);

				/* Install the back link */

			Node->Entry = Entry;

			if(Entry == NULL)
				Node->Node.ln_Name = Node->Number;
			else
			{
				Node->Node.ln_Name = Entry->Header->Name;

				if(!Entry->DialNode)
					Entry->DialNode = Node;
			}

			Node->FromPhonebook = FALSE;
		}

		return(Node);
	}
}

STATIC BOOL
CreateDialNodes(struct List *List,PhoneEntry *Entry,STRPTR Number)
{
	if(Entry == NULL && Number == NULL)
		return(FALSE);
	else
	{
		UBYTE LocalNumberBuffer[256];
		UBYTE LocalInitBuffer[80];
		UBYTE LocalExitBuffer[80];
		UBYTE LocalPrefixBuffer[80];
		UBYTE LocalSuffixBuffer[80];
		STRPTR NumberIndex,InitIndex,ExitIndex,PrefixIndex,SuffixIndex;

		if(Number)
		{
			NumberIndex = Number;

			InitIndex	= NULL;
			ExitIndex	= NULL;
			PrefixIndex	= NULL;
			SuffixIndex	= NULL;
		}
		else
		{
			struct ModemSettings *ModemConfig;

			if(Entry->Config->ModemConfig)
				ModemConfig = Entry->Config->ModemConfig;
			else
				ModemConfig = Config->ModemConfig;

			NumberIndex = Entry->Header->Number;

			InitIndex	= ModemConfig->ModemInit;
			ExitIndex	= ModemConfig->ModemExit;
			PrefixIndex	= ModemConfig->DialPrefix;
			SuffixIndex	= ModemConfig->DialSuffix;
		}

		do
		{
			if(Entry)
			{
				if(InitIndex)
					InitIndex = ExtractNextItem(InitIndex,LocalInitBuffer,sizeof(LocalInitBuffer));

				if(ExitIndex)
					ExitIndex = ExtractNextItem(ExitIndex,LocalExitBuffer,sizeof(LocalExitBuffer));

				if(PrefixIndex)
					PrefixIndex = ExtractNextItem(PrefixIndex,LocalPrefixBuffer,sizeof(LocalPrefixBuffer));

				if(SuffixIndex)
					SuffixIndex = ExtractNextItem(SuffixIndex,LocalSuffixBuffer,sizeof(LocalSuffixBuffer));
			}

			NumberIndex = ExtractNextItem(NumberIndex,LocalNumberBuffer,sizeof(LocalNumberBuffer));

			if(LocalNumberBuffer[0])
			{
				struct DialNode *Node;

				if(Entry)
					Node = CreateDialNode(Entry,LocalNumberBuffer,LocalInitBuffer,LocalExitBuffer,LocalPrefixBuffer,LocalSuffixBuffer);
				else
					Node = CreateDialNode(Entry,LocalNumberBuffer,NULL,NULL,NULL,NULL);

				if(Node)
					AddTail(List,(struct Node *)Node);
				else
					return(FALSE);
			}
		}
		while(NumberIndex != NULL);

		return(TRUE);
	}
}

STATIC VOID
AddDialEntryList(PhonebookHandle *PhoneHandle,struct List *List)
{
	if(PhoneHandle->DialList)
	{
		struct DialNode *Node,*OtherNode;

			/* Remove the dial nodes from the list and try to add them. */

		while(Node = (struct DialNode *)RemHead(List))
		{
				/* Check if this number is already on the list. */

			if(OtherNode = FindNumberInDialList(PhoneHandle->DialList,Node->Number))
			{
					/* A number without a phonebook entry attached will always lose. */

				if(!OtherNode->Entry && Node->Entry)
				{
					RemoveDialNode(PhoneHandle,OtherNode);
					DeleteDialNode(OtherNode);
				}
				else
				{
					DeleteDialNode(Node);
					Node = NULL;
				}
			}

				/* Add the new list node. */

			if(Node != NULL)
				AddDialNode(PhoneHandle,Node);
		}
	}
}

VOID
ReattachDialNode(PhonebookHandle *PhoneHandle,PhoneEntry *Entry)
{
	if(PhoneHandle->DialList && Entry)
	{
		struct DialNode *Node;

		for(Node = (struct DialNode *)PhoneHandle->DialList->lh_Head ; Node->Node.ln_Succ ; Node = (struct DialNode *)Node->Node.ln_Succ)
		{
			if(Node->Entry == Entry && Node->Entry->DialNode == NULL)
			{
				Node->Entry->DialNode = Node;
				break;
			}
		}
	}
}

VOID
CleanseDialList(PhonebookHandle *PhoneHandle)
{
	if(PhoneHandle->DialList)
	{
		struct DialNode *Node,*Next;

		for(Node = (struct DialNode *)PhoneHandle->DialList->lh_Head ; Next = (struct DialNode *)Node->Node.ln_Succ ; Node = Next)
		{
			if(Node->Entry && Node->FromPhonebook)
			{
				if(Node->Entry->Count < 0)
				{
					RemoveAndDeleteRelatedDialNodes(PhoneHandle,Node);

					Next = (struct DialNode *)PhoneHandle->DialList->lh_Head;
				}
			}
		}
	}
}

struct DialNode *
RemoveAndDeleteRelatedDialNodes(PhonebookHandle *PhoneHandle,struct DialNode *ThisNode)
{
	struct DialNode *NextNode;

	if(!PhoneHandle->DialList || !ThisNode)
		 NextNode = NULL;
	else
	{
		struct DialNode *Node,*Next;
		PhoneEntry *Entry;

		if(ThisNode->Node.ln_Succ->ln_Succ)
			NextNode = (struct DialNode *)ThisNode->Node.ln_Succ;
		else
			NextNode = NULL;

		Entry = ThisNode->Entry;

		if(Entry)
			Entry->DialNode = NULL;

		for(Node = (struct DialNode *)PhoneHandle->DialList->lh_Head ; Next = (struct DialNode *)Node->Node.ln_Succ ; Node = Next)
		{
			if(Node->Entry == Entry)
			{
				if(Node == NextNode)
				{
					if(NextNode->Node.ln_Succ->ln_Succ)
						NextNode = (struct DialNode *)NextNode->Node.ln_Succ;
					else
						NextNode = NULL;
				}

				RemoveDialNode(PhoneHandle,Node);
				DeleteDialNode(Node);
			}
		}
	}

	return(NextNode);
}

VOID
DeleteDialNode(struct DialNode *SomeNode)
{
	if(SomeNode)
	{
		if(SomeNode->Entry && SomeNode->Entry->DialNode == SomeNode)
			SomeNode->Entry->DialNode = NULL;

		FreeVecPooled(SomeNode);
	}
}

VOID
RemoveDialNode(PhonebookHandle *PhoneHandle,struct DialNode *SomeNode)
{
		/* Do we have a dial list */

	if(PhoneHandle->DialList && SomeNode)
	{
			/* Remove all list entries that refer to the given phonebook entry */

		if(SomeNode->Entry && SomeNode->FromPhonebook)
		{
			UnmarkDialEntry(PhoneHandle,SomeNode->Entry);

			SomeNode->Entry->DialNode = NULL;
		}

		Remove((struct Node *)SomeNode);

		if(SomeNode->FromPhonebook)
		{
			struct DialNode *Node;

			for(Node = (struct DialNode *)PhoneHandle->DialList->lh_Head ; Node->Node.ln_Succ ; Node = (struct DialNode *)Node->Node.ln_Succ)
			{
				if(Node->FromPhonebook && Node->Entry == SomeNode->Entry)
				{
					Node->Entry->DialNode = Node;
					break;
				}
			}
		}
	}
}

BOOL
AddDialNode(PhonebookHandle *PhoneHandle,struct DialNode *Node)
{
	if(!PhoneHandle->DialList)
		PhoneHandle->DialList = CreateList();

	if(!PhoneHandle->DialList)
		return(FALSE);
	else
	{
		PhoneEntry *Entry;

			/* Does this entry have a list */

		if(Entry = Node->Entry)
		{
				/* Has a list counter been assigned? */

			if(Entry->Count >= 0)
			{
				struct DialNode *ListNode;
				LONG Count;

					/* Walk down the dial list and find the place to put */
					/* the new entry */

				for(ListNode = (struct DialNode *)PhoneHandle->DialList->lh_Head ; ListNode->Node.ln_Succ ; ListNode = (struct DialNode *)ListNode->Node.ln_Succ)
				{
						/* Insert the new entry in front of the first entry */
						/* with a larger index number or in front of an entry */
						/* that doesn't have a number assigned. */

					Count = ListNode->Entry->Count;

					if(Count < 0 || Count > Entry->Count)
					{
						Insert(PhoneHandle->DialList,(struct Node *)Node,ListNode->Node.ln_Pred);

						return(TRUE);
					}
				}
			}
		}

			/* Add it to the end of the list */

		AddTail(PhoneHandle->DialList,(struct Node *)Node);

		return(TRUE);
	}
}

VOID
DeleteDialList(PhonebookHandle *PhoneHandle)
{
	if(PhoneHandle->DialList)
	{
		struct DialNode *Node;

		for(Node = (struct DialNode *)PhoneHandle->DialList->lh_Head ; Node->Node.ln_Succ ; Node = (struct DialNode *)Node->Node.ln_Succ)
		{
			if(Node->Entry)
				Node->Entry->DialNode = NULL;
		}

		DeleteList(PhoneHandle->DialList);
		PhoneHandle->DialList = NULL;
	}
}

	/* AddDialEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry,STRPTR Number):
	 *
	 *	Add a new entry to the dial list. Expand phone numbers separated by
	 *	vertical bars ("|") into several dial nodes. Eliminate duplicate
	 *	dial list entries.
	 */

BOOL
AddDialEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry,STRPTR Number)
{
	BOOL Result;

		/* Start from scratch. */

	Result = FALSE;

		/* If no dial list had been created before, create one now. */

	if(!PhoneHandle->DialList)
		PhoneHandle->DialList = CreateList();

		/* Can we continue? */

	if(PhoneHandle->DialList && (Entry || Number))
	{
		struct List *LocalList;

			/* This is to keep the new list entries. */

		if(LocalList = CreateList())
		{
				/* Create the dial list nodes corresponding
				 * to the entry/phone number.
				 */

			if(CreateDialNodes(LocalList,Entry,Number))
			{
					/* Add the contents of the list to the dial list. */

				AddDialEntryList(PhoneHandle,LocalList);

					/* Everything went fine so far. */

				Result = TRUE;
			}

				/* Delete the temporary list. */

			DeleteList(LocalList);
		}
	}

	return(Result);
}

BOOL
AddAllDialEntries(PhonebookHandle *PhoneHandle)
{
	BOOL Result;

		/* Start from scratch. */

	Result = FALSE;

		/* If no dial list had been created before, create one now. */

	if(!PhoneHandle->DialList)
		PhoneHandle->DialList = CreateList();

		/* Can we continue? */

	if(PhoneHandle->DialList)
	{
		struct List *LocalList;

		if(LocalList = CreateList())
		{
			LONG i;

				/* Add all tagged entries to the local list */

			for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
			{
				if(PhoneHandle->Phonebook[i]->Count >= 0)
				{
					if(!CreateDialNodes(LocalList,PhoneHandle->Phonebook[i],NULL))
					{
						DeleteList(LocalList);
						LocalList = NULL;

						break;
					}
				}
			}
		}

			/* Check if we have a list to work with. */

		if(LocalList)
		{
			struct DialNode *Node;

				/* Walk down the list, marking each entry as coming from the phonebook. */

			for(Node = (struct DialNode *)LocalList->lh_Head ; Node->Node.ln_Succ ; Node = (struct DialNode *)Node->Node.ln_Succ)
				Node->FromPhonebook = TRUE;

				/* Add the contents of the list to the dial list. */

			AddDialEntryList(PhoneHandle,LocalList);

				/* And delete what's left of the temporary list. */

			DeleteList(LocalList);

				/* Everything went fine so far. */

			Result = TRUE;
		}
	}

	return(Result);
}
