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
LTP_AddHistory(struct SGWork *Work)
{
	ObjectNode *Node;

	if(GETOBJECT(Work->Gadget,Node))
	{
		if(Node->Special.String.HistoryHook)
		{
			struct MinList *List = Node->Special.String.HistoryHook->h_Data;

			while(Node->Special.String.NumHistoryLines >= Node->Special.String.MaxHistoryLines)
			{
				CallHookPkt(Node->Special.String.HistoryHook,Work->Gadget,NULL);

				Node->Special.String.NumHistoryLines--;
			}

			Work->WorkBuffer[Work->NumChars] = 0;

			if(CallHookPkt(Node->Special.String.HistoryHook,Work->Gadget,Work->WorkBuffer))
				Node->Special.String.NumHistoryLines++;

			if(List->mlh_Head->mln_Succ)
				Node->Special.String.CurrentNode = (struct Node *)List->mlh_TailPred;
			else
				Node->Special.String.CurrentNode = NULL;
		}
	}
}
