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

	/* LTP_LayoutMenu(RootMenu *Root,WORD ExtraFront,WORD ExtraSpace):
	 *
	 *	Layout the menus, menu items and submenu items.
	 */

BOOL
LTP_LayoutMenu(RootMenu *Root,LONG ExtraFront,LONG ExtraSpace)
{
	LONG		MenuLeft,
				ItemTop = 0,
				ItemWidth = 0,
				ItemShift = 0,
				CommandWidth = 0,
				Width;

	MenuNode	*Menu;
	ItemNode	*Item,*FirstItem = NULL,*LastItem = NULL;

	BOOL		Correction;

		// That one's simple

	for(MenuLeft = ExtraFront, Menu = (MenuNode *)Root->MenuList.mlh_Head ; Menu->Node.mln_Succ ; Menu = (MenuNode *)Menu->Node.mln_Succ)
	{
		Menu->Menu.LeftEdge = MenuLeft;

		MenuLeft += Menu->Menu.Width + ExtraSpace;
	}

		// Now run down the list of items

	Menu = (MenuNode *)Root->MenuList.mlh_Head;
	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
			// Hit a submenu item?

		if(Item->Flags & ITEMF_FirstSub)
		{
			ItemNode *	FirstSub;
			ItemNode *	LastSub = NULL;
			ItemNode *	Sub;
			LONG		SubTop			= 0,
						SubWidth		= 0;
			LONG		CommandWidth	= 0;

				// This is where we started

			FirstSub = Item;

				// Now determine the widest entry and
				// line them up below one another

			for(Sub = FirstSub ; Sub->Node.mln_Succ && (Sub->Flags & ITEMF_IsSub) ; Sub = (ItemNode *)Sub->Node.mln_Succ)
			{
				LastSub = Sub;

				if(Sub->Item.Width > SubWidth)
					SubWidth = Sub->Item.Width;

				if((Width = LTP_GetCommandWidth(Root,Sub)) > CommandWidth)
					CommandWidth = Width;

				Sub->Item.TopEdge = SubTop;

				SubTop += Sub->Item.Height;
			}

			SubWidth += 4 + CommandWidth;

				// In the second pass, make all entries
				// use the same width

			for(Sub = FirstSub ; ; Sub = (ItemNode *)Sub->Node.mln_Succ)
			{
					// Indent the entries that need it

				if(Sub->Item.Flags & CHECKIT)
					((struct IntuiText *)Sub->Item.ItemFill)->LeftEdge += 2 + Root->CheckWidth;

				Sub->Item.Width = SubWidth;

					// Adapt the separator bar size

				if(Sub->Flags & ITEMF_IsBar)
					((struct Image *)Sub->Item.ItemFill)->Width = Sub->Item.Width - 4;
				else
				{
						// Take care of command keys

					if(Sub->Flags & ITEMF_Command)
					{
						struct IntuiText *Command = ((struct IntuiText *)Sub->Item.ItemFill)->NextText;

						Command->LeftEdge = Sub->Item.Width - (IntuiTextLength(Command) + 2);
					}
				}

					// In the next iteration, continue after the last
					// submenu item

				if(Sub == LastSub)
				{
					Item = Sub;

					break;
				}
			}

			DB(kprintf("last sub |%s|\n",((struct IntuiText *)Item->Item.ItemFill)->IText));
		}
		else
		{
				// Start a new menu?

			if(!FirstItem)
			{
					// This is where we started

				FirstItem = Item;

					// Reset the data

				ItemTop = ItemWidth = ItemShift = CommandWidth = 0;
			}

				// Line up the entries in a column

			Item->Item.TopEdge = ItemTop;

			ItemTop += Item->Item.Height;

				// Search for the widest entry

			if(Item->Item.Width > ItemWidth)
				ItemWidth = Item->Item.Width;

				// If the menu title is wider than the
				// items are, adapt the item width
				// accordingly.

			if(ItemWidth < Menu->Menu.Width - 8)
				ItemWidth = Menu->Menu.Width - 8;

				// Search for the widest command sequence

			if((Width = LTP_GetCommandWidth(Root,Item)) > CommandWidth)
				CommandWidth = Width;

				// This is for submenu items which will get
				// indented by this amount

			if(Item->Item.Width > ItemShift)
				ItemShift = Item->Item.Width;
		}

			// Is this the last item for this menu?

		if(Item->Flags & ITEMF_LastItem)
		{
			ItemWidth += 4 + CommandWidth;

				// Restart and layout all the items in this menu

			for(Item = FirstItem ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
			{
					// Indent submenu items

				if(Item->Flags & ITEMF_IsSub)
					Item->Item.LeftEdge += ItemShift + 6;
				else
				{
						// Indent items as necessary

					if(Item->Item.Flags & CHECKIT)
						((struct IntuiText *)Item->Item.ItemFill)->LeftEdge += 2 + Root->CheckWidth;

					Item->Item.Width = ItemWidth;

						// Adapt the separator bar size

					if(Item->Flags & ITEMF_IsBar)
						((struct Image *)Item->Item.ItemFill)->Width = Item->Item.Width - 4;
					else
					{
							// Take care of submenu item indicators and
							// command sequences

						if(Item->Flags & (ITEMF_Command | ITEMF_HasSub))
						{
							struct IntuiText *Command = ((struct IntuiText *)Item->Item.ItemFill)->NextText;

							Command->LeftEdge = Item->Item.Width - (IntuiTextLength(Command) + 2);
						}
					}
				}

					// Abort if this is the last item for this menu

				if(Item->Flags & ITEMF_LastItem)
					break;
			}

			DB(kprintf("last item |%s|\n",((struct IntuiText *)Item->Item.ItemFill)->IText));

				// The next iteration will start a new menu

			FirstItem = NULL;

			Menu = (MenuNode *)Menu->Node.mln_Succ;
		}
	}

		// Calculate the effective positions

	LTP_AdjustMenuPosition(Root);

	Correction = FALSE;

		// We start by chopping down the menus

	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
			// That's where we start

		if(!FirstItem)
			FirstItem = Item;

			// Skip submenu items

		if(!(Item->Flags & ITEMF_IsSub))
			LastItem = Item;

			// Did we reach the end of the menu?

		if(Item->Flags & ITEMF_LastItem)
		{
			DB(kprintf("chopping |%s|->|%s|\n",((struct IntuiText *)FirstItem->Item.ItemFill)->IText,((struct IntuiText *)LastItem->Item.ItemFill)->IText));

				// Chop down the menus

			Correction |= LTP_CorrectItemList(Root,FirstItem,LastItem);

			FirstItem = NULL;
		}
	}

	if(Correction)
	{
		Correction = FALSE;

			// Recalculate the positions

		LTP_AdjustMenuPosition(Root);
	}

		// Next we shift the menus around to make them fit

	for(Menu = (MenuNode *)Root->MenuList.mlh_Head ; Menu->Node.mln_Succ ; Menu = (MenuNode *)Menu->Node.mln_Succ)
	{
		MenuLeft = Menu->Menu.LeftEdge + 4 + Menu->Width + 4;

			// Does it cross the right screen border?

		if(MenuLeft > Root->Screen->Width)
		{
				// Is the menu wider than the screen?

			if(4 + Menu->Width + 4 > Root->Screen->Width)
			{
				DB(kprintf("menu too wide\n"));
				return(FALSE);
			}
			else
			{
				LONG Left = MenuLeft - Root->Screen->Width + 1;

					// Move up

				for(Item = (ItemNode *)((IPTR)Menu->Menu.FirstItem - sizeof(struct MinNode)) ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
				{
					if(!(Item->Flags & ITEMF_IsSub))
						Item->Item.LeftEdge -= Left;

					if(Item->Flags & ITEMF_LastItem)
						break;
				}

				Correction = TRUE;
			}
		}
	}

	if(Correction)
	{
		Correction = FALSE;

			// Recalculate the positions

		LTP_AdjustMenuPosition(Root);
	}

		// Now deal with the submenus

	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
			// Only submenu items, please

		if(Item->Flags & ITEMF_FirstSub)
		{
			ItemNode *Here;

				// Find the first and the last entry

			for(Here = Item ; Here->Node.mln_Succ ; Here = (ItemNode *)Here->Node.mln_Succ)
			{
				if(!(Here->Flags & ITEMF_IsSub))
					break;
				else
					LastItem = Here;
			}

			DB(kprintf("2) chopping |%s|->|%s|\n",((struct IntuiText *)Item->Item.ItemFill)->IText,((struct IntuiText *)LastItem->Item.ItemFill)->IText));

				// Chop down the submenus

			Correction |= LTP_CorrectItemList(Root,Item,LastItem);
		}
	}

	if(Correction)
	{
		Correction = FALSE;

		LTP_AdjustMenuPosition(Root);
	}

		// Almost finished, now shift the submenus around

	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
			// Did we hit a submenu?

		if(Item->Flags & ITEMF_HasSub)
		{
			ItemNode *Here = (ItemNode *)((IPTR)Item->Item.SubItem - sizeof(struct MinNode));

				// Does this one also cross the right screen border?

			if((MenuLeft = Here->Left + 4 + Here->Item.Width + 4) > Root->Screen->Width)
			{
				LONG Left = MenuLeft - Root->Screen->Width;

					// Check if the subitem would cover too much
					// of the item it is attached to

				if(Here->Left - Left < (Item->Left + 4 + Item->Width - CommandWidth))
				{
					Left = Here->Left + (4 + Here->Item.Width + 4) - Item->Left - 5;

					if(Here->Left - Left < 0)
					{
						DB(kprintf("sub too fat\n"));
						return(FALSE);
					}
				}

				DB(kprintf("3) shifting...\n"));

					// Shift the menu around

				do
				{
					if(!(Here->Flags & ITEMF_IsSub))
						break;
					else
					{
						Here->Item.LeftEdge -= Left;
						Here->Left -= Left;

						Correction = TRUE;

						if(Here->Flags & ITEMF_LastItem)
							break;

						Here = (ItemNode *)Here->Node.mln_Succ;
					}
				}
				while(Here->Node.mln_Succ);
			}
		}
	}

		// The last step; see if the alignment stuff worked

	if(Correction)
		LTP_AdjustMenuPosition(Root);

		// First check the menus

	for(Menu = (MenuNode *)Root->MenuList.mlh_Head ; Menu->Node.mln_Succ ; Menu = (MenuNode *)Menu->Node.mln_Succ)
	{
			// Does it cross the screen borders?

		if((MenuLeft = Menu->Menu.LeftEdge + 4 + Menu->Width + 4) > Root->Screen->Width || Menu->Menu.LeftEdge < 0)
		{
			DB(kprintf("menu crosses screen border\n"));
			return(FALSE);
		}
	}

		// Now check the submenus

	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
			// Did we hit a submenu?

		if(Item->Flags & ITEMF_HasSub)
		{
			ItemNode *Here = (ItemNode *)((IPTR)Item->Item.SubItem - sizeof(struct MinNode));

			if(Here->Left + Here->Width > Root->Screen->Width)
			{
				DB(kprintf("submenu crosses screen border\n"));
				return(FALSE);
			}
		}
	}

	LTP_FillMenu(&Root->Menu);

	return(TRUE);
}

#endif	/* DO_MENUS */
