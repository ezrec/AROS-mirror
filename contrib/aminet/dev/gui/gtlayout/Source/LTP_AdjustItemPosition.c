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

	/* LTP_AdjustItemPosition(struct MenuItem *Item,WORD Left,WORD Top):
	 *
	 *	Calculate the effective positions of the menu items.
	 */

LONG
LTP_AdjustItemPosition(struct MenuItem *Item,LONG Left,LONG Top)
{
	ItemNode	*Node;
	LONG		 Width = 0;

		// Hit the road, Jack...

	while(Item)
	{
			// Get back

		Node = (ItemNode *)((IPTR)Item - sizeof(struct MinNode));

			// Add up the left edge

		Node->Left	= Left	+ Node->Item.LeftEdge;
		Node->Top	= Top	+ Node->Item.TopEdge;

			// Fix up the sub menu

		if(Node->Item.SubItem)
			Node->Width = LTP_AdjustItemPosition(Node->Item.SubItem,Node->Left,Node->Top);

			// Update the local width

		if(Node->Item.LeftEdge + Node->Item.Width > Width)
			Width = Node->Item.LeftEdge + Node->Item.Width;

		Item = Item->NextItem;
	}

	return(Width);
}

#endif	/* DO_MENUS */
