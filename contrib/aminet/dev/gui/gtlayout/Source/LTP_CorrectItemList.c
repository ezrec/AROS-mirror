/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#include "gtlayout_global.h"

#include "Assert.h"

#ifdef DO_MENUS	/* Support code */

	/* LTP_CorrectItemList(RootMenu *Root,ItemNode *First,ItemNode *Last):
	 *
	 *	Chop nasty menus in two if they're too tall for this screen.
	 */

BOOL
LTP_CorrectItemList(RootMenu *Root,ItemNode *First,ItemNode *Last)
{
	ItemNode	*Item;
	ULONG		 Count = 0;
	LONG		 Mask;
	BOOL		 Overshoot = FALSE;

	Mask = First->Flags & ITEMF_IsSub;

		// Count the number of items in this menu
		// and check if there is one too many in it

	for(Item = First ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
		if((Item->Flags & ITEMF_IsSub) == Mask)
		{
			Count++;

			if(Item->Top + Item->Item.Height + 2 > Root->Screen->Height)
				Overshoot = TRUE;
		}

		if(Item == Last)
			break;
	}

		// Did we scrape it?

	if(Overshoot && Count > 1)
	{
		ItemNode	*Here;
		ULONG		 i = (Count + 1) / 2;
		LONG		 Top,Left;

			// Find the median or whatever it's called in this part of the world

		for(Here = First ; i > 0 && Here->Node.mln_Succ ; Here = (ItemNode *)Here->Node.mln_Succ)
		{
			if((Here->Flags & ITEMF_IsSub) == Mask)
				i--;
		}

			// Shrink the two halves of the menu down to
			// their minimum sizes

		LTP_ShrinkMenu(Root,First,(ItemNode *)Here->Node.mln_Pred,Mask);
		LTP_ShrinkMenu(Root,Here,Last,Mask);

			// Chop off the other half and stick it
			// on at the right

		Left	= First->Item.LeftEdge + First->Item.Width + 2;
		Top		= First->Item.TopEdge;

			// Move the items over to the right

		for(;;)
		{
			if((Here->Flags & ITEMF_IsSub) == Mask)
			{
				Here->Item.LeftEdge	= Left;
				Here->Item.TopEdge	= Top;

				Top += Here->Item.Height;
			}

			if(Here == Last)
				break;
			else
				Here = (ItemNode *)Here->Node.mln_Succ;
		}

		return(TRUE);
	}
	else
		return(FALSE);
}

#endif	/* DO_MENUS */
