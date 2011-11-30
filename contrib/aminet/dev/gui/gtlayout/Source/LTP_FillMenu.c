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

	/* LTP_FillSub(struct MenuItem *Item):
	 *
	 *	Fill in the submenu item IDs.
	 */

VOID
LTP_FillSub(
	ULONG				MenuID,
	ULONG				ItemID,
	struct MenuItem *	Item)
{
	ULONG				Count = 0;
	ItemNode *			Node;
	struct MenuItem *	First = Item;

	do
	{
		Node = (ItemNode *)((IPTR)Item - sizeof(struct MinNode));

		Node->MenuCode = FULLMENUNUM(MenuID,ItemID,Count);

		if(Item->MutualExclude && (Item->Flags & CHECKED))
		{
			struct MenuItem	*	This	= First;
			ULONG				Exclude	= Item->MutualExclude;
			LONG				i;

			for(i = 0 ; i < 32 && This ; i++, This = This->NextItem)
			{
				if(This != Item && (This->Flags & CHECKIT) && (Exclude & (1L << i)))
					This->Flags &= ~CHECKED;
			}
		}

		Count++;
	}
	while(Item = Item->NextItem);
}

	/* FillItem(struct MenuItem *Item):
	 *
	 *	Fill in the menu item IDs.
	 */

VOID
LTP_FillItem(ULONG MenuID,struct MenuItem *Item)
{
	ULONG			 Count = 0;
	ItemNode		*Node;
	struct MenuItem	*First = Item;

	do
	{
		Node = (ItemNode *)((IPTR)Item - sizeof(struct MinNode));

		Node->MenuCode = FULLMENUNUM(MenuID,Count,NOSUB);

		if(Item->SubItem)
			LTP_FillSub(MenuID,Count,Item->SubItem);

		if(Item->MutualExclude && (Item->Flags & CHECKED))
		{
			struct MenuItem	*This		= First;
			ULONG			 Exclude	= Item->MutualExclude;
			LONG			 i;

			for(i = 0 ; i < 32 && This ; i++, This = This->NextItem)
			{
				if(This != Item && (This->Flags & CHECKIT) && (Exclude & (1L << i)))
					This->Flags &= ~CHECKED;
			}
		}

		Count++;
	}
	while(Item = Item->NextItem);
}

	/* LTP_FillMenu(struct Menu *Menu):
	 *
	 *	Fill in the menu IDs.
	 */

VOID
LTP_FillMenu(struct Menu *Menu)
{
	ULONG		 Count = 0;
	MenuNode	*Node;

	do
	{
		Node = (MenuNode *)((IPTR)Menu - sizeof(struct MinNode));

		Node->MenuCode = FULLMENUNUM(Count,NOITEM,NOSUB);

		if(Menu->FirstItem)
			LTP_FillItem(Count,Menu->FirstItem);

		Count++;
	}
	while(Menu = Menu->NextMenu);
}

#endif	/* DO_MENUS */
