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
#ifndef _GTLAYOUT_INTERNAL_H
#include "gtlayout_internal.h"
#endif

/*****************************************************************************/

#include <dos/dos.h>	/* For AmigaDOS error definitions */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

	/* LTP_FixExtraLabel(RootMenu *Root,LONG *Error):
	 *
	 *	Fix up the submenu indicators and command labels.
	 */

VOID
LTP_FixExtraLabel(RootMenu *Root,LONG *ErrorPtr)
{
	ItemNode	*Item;
	LONG		 Error = 0;

		// Now take care of items which have command sequences
		// or submenu items attached

	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; !Error && Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
			// Does this one need more label data?

		if(Item->Flags & (ITEMF_HasSub | ITEMF_Command))
		{
			struct IntuiText *IntuiText;

			DB(kprintf("  attach to |%s|\n",((struct IntuiText *)Item->Item.ItemFill)->IText));

				// Make room for the extra label data

			if(IntuiText = AsmAllocPooled(Root->Pool,sizeof(struct IntuiText),SysBase))
			{
					// Fill it in

				LTP_InitIText(Root,IntuiText);

					// Command sequence?

				if(Item->ExtraLabel)
				{
					IntuiText->IText		= Item->ExtraLabel;
					IntuiText->ITextFont	= (struct TextAttr *)&Root->BoldAttr;
				}
				else
					IntuiText->IText = (STRPTR)"»";

				IntuiText->TopEdge = (Root->ItemHeight - Root->RPort.TxHeight) / 2;;

					// Link to previous entry

				((struct IntuiText *)Item->Item.ItemFill)->NextText = IntuiText;
			}
			else
				Error = ERROR_NO_FREE_STORE;
		}
	}

	*ErrorPtr = Error;
}

#endif	/* DO_MENUS */
