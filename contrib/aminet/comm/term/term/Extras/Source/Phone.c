/*
**	Phone.c
**
**	Phonebook support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

PhoneEntry **
CreatePhonebookTable(PhoneEntry **OldTable,LONG OldTableSize,LONG NumEntriesWanted,LONG *NumEntriesGot)
{
	PhoneEntry **Table;

		/* Don't allocate less than one entry */

	if(NumEntriesWanted < 1)
		NumEntriesWanted = 1;

		/* Round to next multiple of eight */

	*NumEntriesGot = (NumEntriesWanted + 7) & ~7;

		/* Allocate the table */

	if(Table = (PhoneEntry **)AllocVecPooled(sizeof(PhoneEntry *) * (*NumEntriesGot),MEMF_ANY))
	{
		LONG From;

			/* If there is an old table given, copy it to the new table */

		if(OldTable)
		{
			CopyMem(OldTable,Table,sizeof(PhoneEntry *) * OldTableSize);

			From = OldTableSize;
		}
		else
			From = 0;

			/* Clear all the unused entries */

		memset(&Table[From],0,(*NumEntriesGot - From) * sizeof(PhoneEntry *));
	}

	return(Table);
}

VOID
DeletePhoneEntry(PhoneEntry *Entry)
{
	if(Entry)
	{
			/* Dispose of the dial node if any */

		if(Entry->DialNode)
			DeleteDialNode(Entry->DialNode);

			/* Dispose of the phone node if any */

		if(Entry->Node)
			FreeVecPooled(Entry->Node);

			/* Dispose of the configuration */

		if(Entry->Config)
			DeleteConfiguration(Entry->Config);

			/* Get rid of the rates and costs */

		FreeTimeDateList((struct List *)&Entry->TimeDateList);

			/* Is this entry member of a group? */

		if(Entry->GroupNode)
			FreeVecPooled(Entry->GroupNode);

		FreeVecPooled(Entry);
	}
}

PhoneEntry *
CreatePhoneEntry(PhonebookHandle *PhoneHandle)
{
	PhoneEntry *NewEntry;

		/* Allocate space for the entry and the header data */

	if(NewEntry = (PhoneEntry *)AllocVecPooled(sizeof(PhoneEntry) + sizeof(PhoneHeader),MEMF_ANY | MEMF_CLEAR))
	{
			/* Allocate space for the configuration data */

		if(NewEntry->Config = CreateConfiguration(FALSE))
		{
				/* Put in the header and assign a unique ID to the entry */

			NewEntry->Header		= (PhoneHeader *)(NewEntry + 1);
			NewEntry->Header->ID	= PhoneHandle->PhonebookID++;

				/* Provide a default name */

			strcpy(NewEntry->Header->Name,LocaleString(MSG_TERMPHONE_UNUSED_ENTRY_TXT));

				/* Initialize the creation time */

			GetSysTime(&NewEntry->Header->Creation);

				/* Initialize the entry as `unmarked'. */

			NewEntry->Count = -1;

				/* Initialize the dates and costs */

			NewList((struct List *)&NewEntry->TimeDateList);

				/* Check if the phonebook has a display list attached */

			if(PhoneHandle->PhoneList)
			{
					/* Create a display node */

				if(!(NewEntry->Node = CreatePhoneNode(NewEntry,TRUE)))
				{
					FreeVecPooled(NewEntry);
					return(NULL);
				}
			}

			return(NewEntry);
		}

		FreeVecPooled(NewEntry);
	}

	return(NULL);
}

VOID
RemovePhoneEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry)
{
	PhoneEntry **Phonebook = PhoneHandle->Phonebook;
	LONG i;

	for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
	{
		if(Phonebook[i] == Entry)
		{
			LONG j;

				/* Remove the entry from the table */

			for(j = i ; j < PhoneHandle->NumPhoneEntries ; j++)
				Phonebook[j] = Phonebook[j + 1];

				/* Clear the last entry and shrink the phonebook */

			Phonebook[PhoneHandle->NumPhoneEntries--] = NULL;

				/* Remove it from the dial list */

			RemoveDialNode(PhoneHandle,Entry->DialNode);

				/* Remove it from the group list */

			RemoveGroupEntry(Entry);

				/* Remove it from the display list */

			if(Entry->Node)
				Remove((struct Node *)Entry->Node);

			break;
		}
	}
}

BOOL
AddPhoneEntry(PhonebookHandle *PhoneHandle,PhoneEntry *NewEntry)
{
		/* If the phone book is filled `to the brim', expand it. */

	if(PhoneHandle->NumPhoneEntries + 1 > PhoneHandle->PhoneSize)
	{
		PhoneEntry	**LocalPhonebook;
		LONG		 LocalPhoneSize;

			/* Allocate another phone book table. */

		if(LocalPhonebook = CreatePhonebookTable(PhoneHandle->Phonebook,PhoneHandle->PhoneSize,PhoneHandle->PhoneSize + 1,&LocalPhoneSize))
		{
			FreeVecPooled(PhoneHandle->Phonebook);

			PhoneHandle->Phonebook = LocalPhonebook;
			PhoneHandle->PhoneSize = LocalPhoneSize;
		}
		else
			return(FALSE);
	}

		/* Add it to the end of the table */

	PhoneHandle->Phonebook[PhoneHandle->NumPhoneEntries++] = NewEntry;

		/* And do the same for the list if any */

	if(PhoneHandle->PhoneList && NewEntry->Node)
		AddTail(PhoneHandle->PhoneList,(struct Node *)NewEntry->Node);

	return(TRUE);
}

BOOL
MergePhonebooks(PhonebookHandle *SrcHandle,PhonebookHandle *DstHandle)
{
	PhoneEntry	**LocalPhonebook;
	LONG		 LocalPhoneSize;

		/* Create new table to hold the entries of both phonebooks */

	if(LocalPhonebook = CreatePhonebookTable(DstHandle->Phonebook,DstHandle->NumPhoneEntries,DstHandle->NumPhoneEntries + SrcHandle->NumPhoneEntries,&LocalPhoneSize))
	{
		LONG i,Count;

			/* Move the phonebook entries from the old phonebook to the other */

		for(i = 0, Count = DstHandle->NumPhoneEntries ; i < SrcHandle->NumPhoneEntries ; i++)
		{
			LocalPhonebook[Count] = SrcHandle->Phonebook[i];

			if(SrcHandle->Phonebook[i]->GroupNode)
				FreeVecPooled(SrcHandle->Phonebook[i]->GroupNode);

			SrcHandle->Phonebook[i]->GroupNode = NULL;
			SrcHandle->Phonebook[i]->ThisGroup = NULL;

			AddTail(DstHandle->PhoneList,(struct Node *)SrcHandle->Phonebook[i]->Node);

			LocalPhonebook[Count]->Header->ID = DstHandle->PhonebookID++;

			Count++;
		}

			/* Clear the rest */

		FreeList((struct List *)&SrcHandle->PhoneGroupList);
		SrcHandle->NumPhoneEntries = 0;

			/* Put the new table into the destination phonebook */

		FreeVecPooled(DstHandle->Phonebook);

		DstHandle->Phonebook		= LocalPhonebook;
		DstHandle->PhoneSize		= LocalPhoneSize;
		DstHandle->NumPhoneEntries	= Count;

		return(TRUE);
	}
	else
		return(FALSE);
}

PhonebookHandle *
CreatePhonebook(LONG NumEntries,BOOL AllocateEntries)
{
	PhonebookHandle *PhoneHandle;

		/* Create the header */

	if(PhoneHandle = (PhonebookHandle *)AllocVecPooled(sizeof(PhonebookHandle),MEMF_ANY|MEMF_CLEAR))
	{
			/* Create the table */

		if(PhoneHandle->Phonebook = CreatePhonebookTable(NULL,0,NumEntries,&PhoneHandle->PhoneSize))
		{
			PhoneEntry **Phonebook = PhoneHandle->Phonebook;
			struct timeval Now;
			LONG i;

				/* Set up the ID counter */

			GetSysTime(&Now);
			PhoneHandle->PhonebookID = Now.tv_secs;

				/* Create the phonebook entries one by one */

			if(AllocateEntries)
			{
				for(i = 0 ; i < NumEntries ; i++)
				{
					if(!(Phonebook[i] = CreatePhoneEntry(PhoneHandle)))
					{
						LONG j;

						for(j = 0 ; j < i ; j++)
							DeletePhoneEntry(Phonebook[j]);

						FreeVecPooled(Phonebook);
						FreeVecPooled(PhoneHandle);

						return(NULL);
					}
				}
			}

			PhoneHandle->NumPhoneEntries = NumEntries;

				/* Initialize the lists */

			NewList((struct List *)&PhoneHandle->PhoneGroupList);

				/* And the access semaphore */

			InitSemaphore(&PhoneHandle->ActiveSemaphore);

			return(PhoneHandle);
		}

		FreeVecPooled(PhoneHandle);
	}

	return(NULL);
}

VOID
DeletePhonebook(PhonebookHandle *PhoneHandle)
{
	if(PhoneHandle)
	{
		PhoneEntry **Phonebook = PhoneHandle->Phonebook;
		LONG i;

			/* Delete the groups */

		FreePhoneGroupList((struct List *)&PhoneHandle->PhoneGroupList);

			/* Delete the single entries */

		for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
			DeletePhoneEntry(Phonebook[i]);

			/* Free the display list */

		FreeVecPooled(PhoneHandle->PhoneList);

			/* Delete the dial list */

		DeleteList(PhoneHandle->DialList);

			/* Delete the table */

		FreeVecPooled(PhoneHandle->Phonebook);

			/* And delete the header */

		FreeVecPooled(PhoneHandle);
	}
}

	/* CopyPhoneHeader():
	 *
	 *	Copy a phone header from one place to another. Only copy
	 *	the `sensible' data, not the unique entries.
	 */

VOID
CopyPhoneHeader(PhoneHeader *From,PhoneHeader *To)
{
	strcpy(To->Name,		From->Name);
	strcpy(To->Number,		From->Number);
	strcpy(To->Comment,		From->Comment);

	strcpy(To->UserName,	From->UserName);
	strcpy(To->Password,	From->Password);

	To->QuickMenu	= From->QuickMenu;
	To->NoRates		= From->NoRates;
	To->AutoDial	= From->AutoDial;
}

LONG
MovePhoneEntry(PhonebookHandle *PhoneHandle,LONG Index,LONG How)
{
	PhoneEntry **Phonebook;
	PhoneEntry *Entry;
	LONG NewPosition;

	Phonebook = PhoneHandle->Phonebook;
	Entry = Phonebook[Index];
	NewPosition = Index;

	switch(How)
	{
		case MOVE_HEAD:

			if(Index > 0)
			{
				LONG i;

				for(i = Index ; i > 0 ; i--)
					Phonebook[i] = Phonebook[i - 1];

				NewPosition = 0;
			}

			break;

		case MOVE_PRED:

			if(Index > 0)
			{
				NewPosition = Index - 1;

				Phonebook[Index] = Phonebook[NewPosition];
			}

			break;

		case MOVE_SUCC:

			if(Index < PhoneHandle->NumPhoneEntries - 1)
			{
				NewPosition = Index + 1;

				Phonebook[Index] = Phonebook[NewPosition];
			}

			break;

		case MOVE_TAIL:

			if(Index < PhoneHandle->NumPhoneEntries - 1)
			{
				LONG i;

				NewPosition = PhoneHandle->NumPhoneEntries - 1;

				for(i = Index ; i < NewPosition ; i++)
					Phonebook[i] = Phonebook[i + 1];
			}

			break;
	}

	if(NewPosition != Index)
	{
		Phonebook[NewPosition] = Entry;

		if(Phonebook[NewPosition]->Node)
			MoveNode(PhoneHandle->PhoneList,(struct Node *)Phonebook[NewPosition]->Node,How);
	}

	return(NewPosition);
}

PhoneEntry *
GetActiveEntry(PhonebookHandle *PhoneHandle)
{
	PhoneEntry *Entry;

	SafeObtainSemaphoreShared(&PhoneHandle->ActiveSemaphore);

	Entry = PhoneHandle->ActiveEntry;

	ReleaseSemaphore(&PhoneHandle->ActiveSemaphore);

	return(Entry);
}

VOID
SetActiveEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Entry)
{
	ObtainSemaphore(&PhoneHandle->ActiveSemaphore);

	PhoneHandle->ActiveEntry = Entry;

	ReleaseSemaphore(&PhoneHandle->ActiveSemaphore);
}

PhoneEntry *
LockActiveEntry(PhonebookHandle *PhoneHandle)
{
	SafeObtainSemaphoreShared(&PhoneHandle->ActiveSemaphore);

	return(PhoneHandle->ActiveEntry);
}

VOID
UnlockActiveEntry(PhonebookHandle *PhoneHandle)
{
	ReleaseSemaphore(&PhoneHandle->ActiveSemaphore);
}

PhoneEntry *
ClonePhoneEntry(PhonebookHandle *PhoneHandle,PhoneEntry *Source)
{
	PhoneEntry *Destination;

	if(Destination = CreatePhoneEntry(PhoneHandle))
	{
			/* Copy the header information */

		CopyPhoneHeader(Source->Header,Destination->Header);

			/* Delete the rates and costs of the new entry */

		FreeTimeDateList((struct List *)&Destination->TimeDateList);

			/* And replace it with a copy of the source data */

		if(CopyTimeDateList((struct List *)&Source->TimeDateList,(struct List *)&Destination->TimeDateList,FALSE))
		{
			LONG i,Len1,Len2;
			LONG Copies;
			UBYTE TempName[80];
			STRPTR BaseName;
			BOOL GotIt = TRUE;

				/* Allocate space for the config data. */

			for(i = PREF_SERIAL ; i <= PREF_FASTMACROFILENAME ; i++)
			{
				if(!CreateConfigEntry(Destination->Config,i))
				{
					GotIt = FALSE;
					break;
				}
			}

			if(GotIt)
			{
				STRPTR Trailer = NULL;

					/* Copy the data. */

				SaveConfig(Source->Config,Destination->Config);

					/* Build the "Copy <filler>of" template */

				LimitedSPrintf(sizeof(TempName),TempName,LocaleString(MSG_COPY_OF_TXT)," <filler>");

					/* Now look where the "<filler>" ended up */

				for(i = 0 ; i < strlen(TempName) ; i++)
				{
					if(!Strnicmp(&TempName[i],"<filler>",strlen("<filler>")))
					{
							/* Chop off everything after the "Copy " string */

						TempName[i] = 0;
						Trailer = &TempName[i + strlen("<filler>")];
						break;
					}
				}

					/* Does this entry have a leading "Copy " string? */

				if(!Strnicmp(TempName,Destination->Header->Name,strlen(TempName)))
				{
					BaseName = &Destination->Header->Name[strlen(TempName)];

						/* Skip the numbers */

					while(*BaseName >= '0' && *BaseName <= '9')
						BaseName++;

						/* Skip the spaces */

					while(*BaseName == ' ')
						BaseName++;

						/* If there is the trailing "of", skip it */

					if(!Strnicmp(BaseName,Trailer,strlen(Trailer)))
						BaseName += strlen(Trailer);

						/* Skip the spaces */

					while(*BaseName == ' ')
						BaseName++;
				}
				else
					BaseName = Destination->Header->Name;

				Len1 = strlen(BaseName);

				Copies = 0;

				for(i = 0 ; i < PhoneHandle->NumPhoneEntries ; i++)
				{
					if(PhoneHandle->Phonebook[i] != Destination)
					{
						Len2 = strlen(PhoneHandle->Phonebook[i]->Header->Name);

						if(Len2 >= Len1)
						{
							if(!Stricmp(&PhoneHandle->Phonebook[i]->Header->Name[Len2 - Len1],BaseName))
								Copies++;
						}
					}
				}

				if(Copies)
				{
					UBYTE Counter[10];

					if(Copies > 1)
						LimitedSPrintf(sizeof(Counter),Counter," %ld ",Copies);
					else
						strcpy(Counter," ");

					LimitedSPrintf(sizeof(TempName),TempName,LocaleString(MSG_COPY_OF_TXT),Counter);
					LimitedStrcat(sizeof(TempName),TempName," ");

					if(strlen(TempName) + strlen(BaseName) < sizeof(Destination->Header->Name))
					{
						strcat(TempName,BaseName);

						strcpy(Destination->Header->Name,TempName);
					}
				}

				if(Destination->Node)
					LimitedSPrintf(sizeof(Destination->Node->LocalName),Destination->Node->LocalName,"      %s",Destination->Header->Name);

				return(Destination);
			}
		}

		DeletePhoneEntry(Destination);
	}

	return(NULL);
}
