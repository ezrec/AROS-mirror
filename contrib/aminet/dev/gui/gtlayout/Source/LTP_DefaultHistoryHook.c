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

#include "Assert.h"

ULONG SAVE_DS ASM
LTP_DefaultHistoryHook(REG(a0) struct Hook *Hook,REG(a2) struct Gadget *Gadget,REG(a1) STRPTR NewString)
{
	ObjectNode			*Node	= (ObjectNode *)Gadget->UserData;
	struct LayoutHandle	*Handle = Node->Special.String.LayoutHandle;
	struct MinList		*List	= (struct MinList *)Hook->h_Data;

	if(NewString)
	{
		ULONG *Data;

		if(Data = (ULONG *)LTP_Alloc(Handle,sizeof(ULONG) + sizeof(struct Node) + strlen(NewString) + 1))
		{
			struct Node *NewNode;

			*Data++ = sizeof(ULONG) + sizeof(struct Node) + strlen(NewString) + 1;

			NewNode = (struct Node *)Data;

			NewNode->ln_Name = (STRPTR)(NewNode + 1);

			strcpy(NewNode->ln_Name,NewString);

			AddTail((struct List *)List,NewNode);
		}
		else
			return(FALSE);
	}
	else
	{
		if(List->mlh_Head->mln_Succ)
		{
			ULONG *Data = (ULONG *)RemHead((struct List *)List);

			LTP_Free(Handle,&Data[-1],Data[-1]);
		}
	}

	return(TRUE);
}
