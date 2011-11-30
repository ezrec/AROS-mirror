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

	/* LTP_MakeItem(RootMenu *Root,struct NewMenu *Template):
	 *
	 *	Create a single menu item and fill it in.
	 */

ItemNode *
LTP_MakeItem(RootMenu *Root,struct NewMenu *Template)
{
	ItemNode	*Item;
	LONG		 Size;

		// Which type of entry to create

	if(Template->nm_Label == NM_BARLABEL)
		Size = sizeof(ItemNode) + sizeof(struct Image);
	else
		Size = sizeof(ItemNode) + sizeof(struct IntuiText);

		// Make room for it

	if(Item = AsmAllocPooled(Root->Pool,Size,SysBase))
	{
			// Is it a separator bar?

		if(Template->nm_Label == NM_BARLABEL)
		{
			struct Image *Image = (struct Image *)(Item + 1);

				// Fill in the image data

			Image->LeftEdge		= 2;
			Image->TopEdge		= 2;
			Image->Depth		= Root->DrawInfo->dri_Depth;
			Image->PlaneOnOff	= Root->TextPen;
			Image->Height		= 2;

				// Now take care of the item itself

			Item->Item.ItemFill = Image;
			Item->Item.Width	= 2;
			Item->Item.Height	= 2 + Image->Height + 2;
			Item->Flags			= ITEMF_IsBar;
			Item->Item.Flags	= HIGHNONE;
		}
		else
		{
			struct IntuiText *IntuiText = (struct IntuiText *)(Item + 1);

				// Fill in the label

			LTP_InitIText(Root,IntuiText);

			IntuiText->LeftEdge	= 2;
			IntuiText->TopEdge	= (Root->ItemHeight - Root->RPort.TxHeight) / 2;
			IntuiText->IText	= (UBYTE *)Template->nm_Label;

				// Now take care of the item itself

			Item->Item.ItemFill	= IntuiText;
			Item->Item.Width	= 2 + TextLength(&Root->RPort,IntuiText->IText,strlen(IntuiText->IText)) + 2;
			Item->Item.Height	= Root->ItemHeight;
			Item->Item.Flags	= ITEMTEXT | HIGHCOMP;

				// Is there a command to take care of?

			if(Template->nm_CommKey)
			{
					// Special command?

				if(Template->nm_Flags & NM_COMMANDSTRING)
				{
					Item->ExtraLabel	= (UBYTE *)Template->nm_CommKey;
					Item->Flags			= ITEMF_Command;
				}
				else
				{
					DB(kprintf("commkey 0x%08lx\n",Template->nm_CommKey));

						// Just put in the usual command sequence in there

					Item->Item.Flags	|= COMMSEQ;
					Item->Item.Command	 = (BYTE)ToUpper(Template->nm_CommKey[0]);
				}
			}

				// Move up for the checkmark

			if(Template->nm_Flags & CHECKIT)
				Item->Item.Width += 2 + Root->CheckWidth;

				// Disable the item if necessary

			if(!(Template->nm_Flags & NM_ITEMDISABLED))
				Item->Item.Flags |= ITEMENABLED;

				// Fill in the rest of the flags

			Item->Item.Flags |= Template->nm_Flags & (CHECKIT | MENUTOGGLE | CHECKED);

				// Take care of the remaining data

			Item->Item.MutualExclude = Template->nm_MutualExclude;
		}

		Item->UserData = Template->nm_UserData;
	}

	return(Item);
}

#endif	/* DO_MENUS */
