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

#include <stddef.h>
#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

/****** gtlayout.library/LT_MenuControlTagList ******************************************
*
*   NAME
*	LT_MenuControlTagList -- Manipulate menus, menu/submenu items (V11)
*
*   SYNOPSIS
*	LT_MenuControlTagList(Window,Menu,Tags)
*	                       A0   A1     A2
*
*	VOID LT_MenuControlTagList(struct Window *,struct Menu *,struct TagItem *);
*
*	VOID LT_MenuControlTags(struct Window *,struct Menu *,...);
*
*   FUNCTION
*	This routine provides a rather efficient way to set and to clear,
*	to enable and to disable a number of menus, menu/submenu items
*	all at once.
*
*	In v18 this routine was modified to disconnect a menu strip
*	from a window if it is about to change checkmark states. In
*	earlier releases or if a single menu is attached to several
*	windows it is recommended that you disconnect the menu from
*	the windows it is attached to before you call this routine.
*
*	As of v18 this routine is smart enough to handle menu
*	mutual exclusion.
*
*   INPUTS
*	Window - Pointer to Window this menu is attached to. Starting with
*	         gtlayout.library v16 this parameter may be NULL.
*
*	Menu - Pointer to Menu structure as returned by LT_NewMenuTagList.
*
*	Tags - Pointer to a list of tagitem values, as found
*	       in gtlayout.h
*
*
*	Tags:
*
*	LAMN_ID (ULONG) - Unique ID of menu/menu item/submenu item to
*	    manipulate.
*
*	LAMN_Checked (BOOL) - Set the checkmark state of the
*	    menu/submenu item.
*
*	LAMN_Disabled (BOOL) - Set the availability state of the
*	    menu/menu item/submenu item.
*
*	LAMN_FullMenuNum (UWORD) - Intuition menu number of
*		menu/submenu item to manipulate. You would pass the
*		result of the FULLMENUNUM() macro here for the
*		item in question. (V30)
*
*   RESULT
*	none
*
*   EXAMPLE
*	The following tagitem list will clear the checkmark and
*	disable the menu item associated with ID 5 and set the
*	checkmark for the item associated with ID 6:
*
*	    LAMN_ID,         5,
*	      LAMN_Checked,  FALSE,
*	      LAMN_Disabled, TRUE,
*	    LAMN_ID,         6,
*	      LAMN_Checked,  TRUE,
*	    TAG_DONE
*
*   BUGS
*	In library versions up to and including v17.2 this routine
*	is broken. It won't do any harm, it just doesn't do what you
*	want it to do.
*
*	Up to and including V42.1 the library could fail to find the
*	correct menu items corresponding to the LAMN_ID given. This
*	has been fixed in V43.1.
*
*	Up to and including V43.1 trying to change the attributes
*	of an item with LAMN_ID whose ID number could not be found
*	could lead to a crash. This has been fixed in V43.2.
*
*   SEE ALSO
*	gtlayout.library/LT_NewMenuTagList
*
******************************************************************************
*
*/

VOID LIBENT
LT_MenuControlTagList(REG(a0) struct Window *Window,REG(a1) struct Menu *IntuitionMenu,REG(a2) struct TagItem *Tags)
{
	RootMenu *			Root = (RootMenu *)((IPTR)IntuitionMenu - offsetof(RootMenu,Menu));
	const struct TagItem *	List;
	struct TagItem *	Entry;
	ULONG				ID;
	MenuNode *			Menu = NULL;
	ItemNode *			Item = NULL;
	BOOL				GotIt;
	BOOL				Disconnected = FALSE;

	if(!IntuitionMenu)
		return;

	List = Tags;

		// Make sure that if window and menu are provided,
		// the menu is attached to the window.

	if(Window && IntuitionMenu)
	{
		if(Window->MenuStrip != IntuitionMenu)
			Window = NULL;
	}

	while(Entry = NextTagItem(&List))
	{
		switch(Entry->ti_Tag)
		{
				// We start with a menu/item/subitem ID

			case LAMN_ID:

				GotIt = FALSE;

				ID = (ULONG)Entry->ti_Data;

					// Check if it's on the menu list

				for(Menu = (MenuNode *)Root->MenuList.mlh_Head ; Menu->Node.mln_Succ ; Menu = (MenuNode *)Menu->Node.mln_Succ)
				{
					if(Menu->ID == ID)
					{
						GotIt = TRUE;

						Item = NULL;

						break;
					}
				}

					// If it isn't, check the item/subitem list

				if(!GotIt)
				{
					for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
					{
						if(Item->ID == ID)
						{
							GotIt = TRUE;

							Menu = NULL;

							break;
						}
					}
				}

					// Do nothing if none is found

				if(!GotIt)
				{
					Menu = NULL;
					Item = NULL;
				}

				break;

				// We start with a menu/item/subitem ID

			case LAMN_FullMenuNum:

				GotIt = FALSE;

				ID = (ULONG)Entry->ti_Data;

					// Check if it's on the menu list

				for(Menu = (MenuNode *)Root->MenuList.mlh_Head ; Menu->Node.mln_Succ ; Menu = (MenuNode *)Menu->Node.mln_Succ)
				{
					if(Menu->MenuCode == ID)
					{
						GotIt = TRUE;

						Item = NULL;

						break;
					}
				}

					// If it isn't, check the item/subitem list

				if(!GotIt)
				{
					for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
					{
						if(Item->MenuCode == ID)
						{
							GotIt = TRUE;

							Menu = NULL;

							break;
						}
					}
				}

					// Do nothing if none is found

				if(!GotIt)
				{
					Menu = NULL;
					Item = NULL;
				}

				break;

				// Fiddle with the checkmark

			case LAMN_Checked:

				if(Item)
				{
						// Disconnect the window menu before we
						// change the states

					if(Window && !Disconnected)
					{
						ClearMenuStrip(Window);

						Disconnected = TRUE;
					}

					if(Entry->ti_Data)
						Item->Item.Flags |=  CHECKED;
					else
						Item->Item.Flags &= ~CHECKED;

						// Now check for mutual exclusion

					if(Item->Item.MutualExclude && Entry->ti_Data)
					{
						ItemNode	*Node;
						ULONG		 Exclude;
						LONG		 Result;
						BOOL		 IsSub;

						Exclude = Item->Item.MutualExclude;

							// Is the current item a submenu item?

						if(SUBNUM(Item->MenuCode) == NOSUB)
							IsSub = FALSE;
						else
							IsSub = TRUE;

							// Walk the item list back to the first list entry
							// in this menu/submenu

						for(Node = Item ; Node->Node.mln_Pred ; Node = (ItemNode *)Node->Node.mln_Pred)
						{
								// Extract the item number

							if(IsSub)
								Result = SUBNUM(Node->MenuCode);
							else
								Result = ITEMNUM(Node->MenuCode);

								// Is this the first item?

							if(!Result)
							{
								LONG i,Mask,Value;

									// Now build a mask to extract the data
									// that should remain constant for all
									// items in this list.

								if(IsSub)
									Mask = 0x07FF;	// menu-item + menu
								else
									Mask = 0x001F;	// menu

									// This is the constant value

								Value = Item->MenuCode & Mask;

									// Now we walk down the list

								for(i = 0 ; Node->Node.mln_Succ && i < 32 ; i++)
								{
										// Will this one be affected by the
										// mutual exclusion stuff?

									if(Node != Item && (Node->Item.Flags & CHECKIT) && (Exclude & (1L << i)))
										Node->Item.Flags &= ~CHECKED;

										// Move to the next item

									Node = (ItemNode *)Node->Node.mln_Succ;

										// Is this still the same menu/submenu?

									if((Node->MenuCode & Mask) != Value)
										break;
								}

								break;
							}
						}
					}
				}

				break;

				// Turn menus, submenus and items off or on

			case LAMN_Disabled:

				if(Item != NULL || Menu != NULL)
				{
					if(Window != NULL)
					{
						LONG Code;

						if(Item != NULL)
							Code = Item->MenuCode;
						else
							Code = Menu->MenuCode;

						if(Entry->ti_Data)
							OffMenu(Window,Code);
						else
							OnMenu(Window,Code);
					}
					else
					{
						UWORD *Flags;

						if(Item)
							Flags = &Item->Item.Flags;
						else
							Flags = &Menu->Menu.Flags;

						if(Entry->ti_Data)
							*Flags &= ~ITEMENABLED;
						else
							*Flags |= ITEMENABLED;
					}
				}

				break;
		}
	}

		// Reconnect the menu strip if necessary

	if(Disconnected)
		ResetMenuStrip(Window,IntuitionMenu);
}


/*****************************************************************************/


#ifndef __AROS__
VOID
LT_MenuControlTags(struct Window *Window,struct Menu *Menu,...)
{
	va_list VarArgs;

	va_start(VarArgs,Menu);
	LT_MenuControlTagList(Window,Menu,(struct TagItem *)VarArgs);
	va_end(VarArgs);
}
#endif

#endif	/* DO_MENUS */
