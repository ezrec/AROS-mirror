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

#ifdef DO_MENUS	/* Support code */

	/* LTP_GetCommandWidth(RootMenu *Root,ItemNode *Item):
	 *
	 *	Calculate the width of a command sequence.
	 */

LONG
LTP_GetCommandWidth(RootMenu *Root,ItemNode *Item)
{
	LONG Width = 0;

		// Skip separator bars

	if(!(Item->Flags & ITEMF_IsBar))
	{
			// Add in the Amiga glyph

		if(Item->Item.Flags & COMMSEQ)
			Width = TextLength(&Root->RPort,&Item->Item.Command,1) + 2 + Root->AmigaWidth + 2;
		else
		{
				// Add in command or submenu indicator

			if(Item->Flags & (ITEMF_Command | ITEMF_HasSub))
				Width = IntuiTextLength(((struct IntuiText *)Item->Item.ItemFill)->NextText) + 2;
		}
	}

	return(Width);
}

#endif	/* DO_MENUS */
