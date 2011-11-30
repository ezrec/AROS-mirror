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

#include <clib/keymap_protos.h>
#include <pragmas/keymap_pragmas.h>

#include <stddef.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

/****** gtlayout.library/LT_FindMenuCommand ******************************************
*
*   NAME
*	LT_FindMenuCommand -- Get the menu/submenu item associated
*	                      with a rawkey event (V11)
*
*   SYNOPSIS
*	Item = LT_FindMenuCommand(Menu,Code,Qualifier,Gadget)
*	 D0                        A0  D0       D0      A1
*
*	struct MenuItem *LT_FindMenuCommand(struct Menu *,
*
*	                     UWORD,ULONG,struct Gadget *);
*
*   FUNCTION
*	With the IntuiMessage data copied from a type IDCMP_RAWKEY message
*	tries to find the MenuItem the event referred to.
*
*   INPUTS
*	Menu - Pointer to Menu structure as returned by LT_NewMenuTagList.
*
*	Code - Value copied from IntuiMessage->Code
*
*	Qualifier - Value copied from IntuiMessage->Qualifier
*
*	Gadget - Value copied from IntuiMessage->IAddress
*
*   RESULT
*	Item - Pointer to the struct MenuItem * in question or NULL
*	    if none could be found
*
*   SEE ALSO
*	gtlayout.library/LT_NewMenuTagList
*
******************************************************************************
*
*/

struct MenuItem * LIBENT
LT_FindMenuCommand(REG(a0) struct Menu *Menu,REG(d0) UWORD MsgCode,REG(d1) UWORD MsgQualifier,REG(a1) struct Gadget *MsgGadget)
{
	if(!Menu)
		return(NULL);

		// Only take care of downstrokes

	if(!(MsgCode & IECODE_UP_PREFIX))
	{
		RootMenu			*Root = (RootMenu *)((IPTR)Menu - offsetof(RootMenu,Menu));
		ItemNode			*Item;
		struct InputEvent	 Event;
		UBYTE				 ANSIKey[10];
		ULONG				 Qualifier;

			// Fix up the MsgQualifier

		if(MsgQualifier & QUALIFIER_SHIFT)
			MsgQualifier = (MsgQualifier & ~QUALIFIER_SHIFT) | IEQUALIFIER_LSHIFT;

		if(MsgQualifier & QUALIFIER_ALT)
			MsgQualifier = (MsgQualifier & ~QUALIFIER_ALT) | IEQUALIFIER_LALT;

			// Convert the key

		ANSIKey[0] = 0;

		Event.ie_NextEvent		= NULL;
		Event.ie_Class			= IECLASS_RAWKEY;
		Event.ie_SubClass		= 0;
		Event.ie_Code			= MsgCode;
		Event.ie_Qualifier		= MsgQualifier;
		Event.ie_EventAddress	= (APTR)MsgGadget;

		if(!MapRawKey(&Event,ANSIKey,9,NULL))
			ANSIKey[0] = 0;

			// Run down the list...

		for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
		{
				// See if we can do with the char

			if(Item->Char)
			{
				if(Item->Char == ANSIKey[0])
					return(&Item->Item);
			}
			else
			{
					// So that didn't work, what about the raw data?

				Qualifier = Item->Qualifier;

				if(Qualifier & QUALIFIER_SHIFT)
					Qualifier = (Qualifier & ~QUALIFIER_SHIFT) | IEQUALIFIER_LSHIFT;

				if(Qualifier & QUALIFIER_ALT)
					Qualifier = (Qualifier & ~QUALIFIER_ALT) | IEQUALIFIER_LALT;

				if(Item->Code == MsgCode && (MsgQualifier & Qualifier) == Qualifier)
					return(&Item->Item);
			}
		}
	}

	return(NULL);
}

#endif	/* DO_MENUS */
