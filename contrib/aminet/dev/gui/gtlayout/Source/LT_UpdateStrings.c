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

STATIC VOID
LTP_UpdateStrings(ObjectNode *Group)
{
	ObjectNode	*Node;
	ULONG		 Page;

	SCANPAGE(Group,Node,Page)
	{
		switch(Node->Type)
		{
			case FRACTION_KIND:
			case PASSWORD_KIND:
			case STRING_KIND:
			case INTEGER_KIND:

				if(Node->Host)
					LTP_PutStorage(Node);

				break;

			case GROUP_KIND:

				LTP_UpdateStrings(Node);
				break;
		}
	}
}

/****** gtlayout.library/LT_UpdateStrings ******************************************
*
*   NAME
*	LT_UpdateStrings -- Make sure all visible string buffer contents
*	                    get written into storage (v9).
*
*   SYNOPSIS
*	LT_UpdateStrings(LayoutHandle);
*	                        A0
*
*	VOID LT_UpdateStrings(struct LayoutHandle *);
*
*   FUNCTION
*	The user can terminate input into a string gadget or an object
*	derived from a string gadget by various means. They all have
*	in common that the application receives no notification that
*	the string gadget contents have changed. This is particularly
*	nasty with objects which make use of LA_STRPTR or other
*	tags. Using LT_UpdateStrings() you can force all visible string
*	gadget objects to hand over their contents to the internal
*	buffers. Do this before you eventually exit your input loop.
*
*   INPUTS
*	LayoutHandle - Pointer to LayoutHandle structure.
*
*   RESULT
*	none
*
******************************************************************************
*
*/

VOID LIBENT
LT_UpdateStrings(REG(a0) LayoutHandle *Handle)
{
	if(Handle)
		LTP_UpdateStrings(Handle->TopGroup);
}
