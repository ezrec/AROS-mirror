/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

/*****************************************************************************/

#include <intuition/sghooks.h>	/* For struct SGWork */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

VOID
LTP_HandleHistory(struct SGWork *Work)
{
	ObjectNode *Node;

	if(GETOBJECT(Work->Gadget,Node))
	{
		if(Node->Special.String.HistoryHook)
		{
			struct Node	*Choice;
			struct MinList *List;

			Choice	= NULL;
			List	= Node->Special.String.HistoryHook->h_Data;

			if(Work->IEvent->ie_Qualifier & QUALIFIER_SHIFT)
			{
				if(Work->IEvent->ie_Code == CURSORUP)
				{
					if(List->mlh_Head->mln_Succ)
						Choice = (struct Node *)List->mlh_Head;
				}
				else
				{
					if(List->mlh_Head->mln_Succ)
						Choice = (struct Node *)List->mlh_TailPred;
				}
			}
			else
			{
				struct Node *Current = Node->Special.String.CurrentNode;

				if(Work->IEvent->ie_Code == CURSORUP)
				{
					if(Current)
					{
						if(Current->ln_Pred->ln_Pred)
							Choice = Current->ln_Pred;
						else
							Choice = Current;
					}
					else
					{
						if(List->mlh_Head->mln_Succ)
							Choice = (struct Node *)List->mlh_TailPred;
					}
				}
				else
				{
					if(Current)
					{
						if(Current->ln_Succ->ln_Succ)
							Choice = Current->ln_Succ;
					}
				}
			}

			if(Choice != Node->Special.String.CurrentNode)
			{
				LONG Len;

				if(Choice)
				{
					Len = strlen(Choice->ln_Name);

					if(Len >= Work->StringInfo->MaxChars)
						Len = max(0,Work->StringInfo->MaxChars - 1);

					if(Len > 0)
						CopyMem(Choice->ln_Name,Work->WorkBuffer,Len);
				}
				else
					Len = 0;

				Work->WorkBuffer[Len]	= 0;
				Work->NumChars			= Len;
				Work->BufferPos			= Len;

				if(Node->Type == INTEGER_KIND)
				{
					#ifdef DO_HEXHOOK
					{
						LTP_ConvertNum((Node->Min < 0),Work->WorkBuffer,&Work->LongInt);
					}
					#else
					{
						Work->LongInt = LTP_Atol(Work->WorkBuffer);
					}
					#endif	/* DO_HEXHOOK */
				}

				Node->Special.String.CurrentNode = Choice;

				Work->Actions = (Work->Actions & ~SGA_BEEP) | SGA_USE | SGA_REDISPLAY;
			}
		}
	}
}
