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

#include <dos/dos.h>	/* For AmigaDOS error definitions */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

	/* LTP_CreateMenuTemplate(RootMenu *Root,LONG *ErrorPtr,struct NewMenu *MenuTemplate):
	 *
	 *	Create the menu strip data.
	 */

BOOL
LTP_CreateMenuTemplate(RootMenu *Root,LONG *ErrorPtr,struct NewMenu *MenuTemplate)
{
	MenuNode	*LastMenu	= NULL;
	ItemNode	*LastItem	= NULL;
	ItemNode	*LastSub	= NULL;

	MenuNode	*Menu;
	ItemNode	*Item;
	ItemNode	*Sub;

	LONG		 Error;

	BOOL		 Done;

	Done	= FALSE;
	Error	= 0;

	do
	{
			// Check the type

		switch(MenuTemplate->nm_Type)
		{
				// Stop here?

			case NM_END:

				DB(kprintf("NM_END\n"));

					// Mark the last menu item

				if(LastSub)
					LastSub->Flags |= ITEMF_LastItem;
				else
				{
					if(LastItem)
						LastItem->Flags |= ITEMF_LastItem;
				}

				Done = TRUE;
				break;

				// Found a new menu?

			case NM_TITLE:

				DB(kprintf("NM_TITLE |%s|\n",MenuTemplate->nm_Label));

					// Stop on weird labels

				if(MenuTemplate->nm_Label == NM_BARLABEL)
					Error = ERROR_OBJECT_WRONG_TYPE;
				else
				{
						// Choose the menu to use

					if(!LastMenu)
						Menu = (MenuNode *)&Root->Node;
					else
						Menu = NULL;

					DB(kprintf("         make menu\n"));

						// Make room for the menu

					if(Menu = LTP_MakeMenu(Root,Menu,MenuTemplate))
					{
							// Link to last menu

						if(LastMenu)
							LastMenu->Menu.NextMenu = &Menu->Menu;

							// Mark the last menu item

						if(LastSub)
							LastSub->Flags |= ITEMF_LastItem;
						else
						{
							if(LastItem)
								LastItem->Flags |= ITEMF_LastItem;
						}

						LastMenu	= Menu;
						LastItem	= NULL;
						LastSub		= NULL;

							// Add the menu to the list

						AddTail((struct List *)&Root->MenuList,(struct Node *)Menu);
					}
					else
						Error = ERROR_NO_FREE_STORE;
				}

				break;

				// Found a new menu item?

			case NM_ITEM:

				DB(kprintf("NM_ITEM |%s|\n",MenuTemplate->nm_Label == NM_BARLABEL ? (STRPTR)"«Bar»" : MenuTemplate->nm_Label));

					// We need a menu to attach this to

				if(!LastMenu)
					Error = ERROR_INVALID_COMPONENT_NAME;
				else
				{
					DB(kprintf("        make item\n"));

						// Make room for the item

					if(Item = LTP_MakeItem(Root,MenuTemplate))
					{
							// Link it in

						if(LastItem)
							LastItem->Item.NextItem = &Item->Item;
						else
							LastMenu->Menu.FirstItem = &Item->Item;

						LastItem	= Item;
						LastSub		= NULL;

							// Add it to the item list

						AddTail((struct List *)&Root->ItemList,(struct Node *)Item);
					}
					else
						Error = ERROR_NO_FREE_STORE;
				}

				break;

			case NM_SUB:

				DB(kprintf("NM_SUB |%s|\n",MenuTemplate->nm_Label == NM_BARLABEL ? (STRPTR)"«Bar»" : MenuTemplate->nm_Label));

					// We need a menu item to attach this to

				if(!LastItem)
					Error = ERROR_INVALID_COMPONENT_NAME;
				else
				{
					DB(kprintf("       make sub\n"));

						// Make room for the item

					if(Sub = LTP_MakeItem(Root,MenuTemplate))
					{
							// Make sure it stays a submenu item

						Sub->Flags |= ITEMF_IsSub;

							// Link it in

						if(LastSub)
							LastSub->Item.NextItem = &Sub->Item;
						else
						{
								// We cannot attach submenu items to
								// separator bars

							if(LastItem->Flags & ITEMF_IsBar)
								Error = ERROR_INVALID_COMPONENT_NAME;
							else
							{
								DB(kprintf("-----------> |%s| has submenu items\n",((struct IntuiText *)LastItem->Item.ItemFill)->IText));

									// Link the submenu item in

								LastItem->Item.SubItem = &Sub->Item;

									// Zap the command sequence data

								LastItem->Flags			= (LastItem->Flags & ~ITEMF_Command) | ITEMF_HasSub;
								LastItem->ExtraLabel	= NULL;

								LastItem->Item.Flags	&= ~COMMSEQ;
								LastItem->Item.Command	 = 0;

									// This is the first submenu item for this menu item

								Sub->Flags |= ITEMF_FirstSub;
							}
						}

						LastSub = Sub;

							// Add it to the list

						AddTail((struct List *)&Root->ItemList,(struct Node *)Sub);
					}
					else
						Error = ERROR_NO_FREE_STORE;
				}

				// Fall through to...

				// Do nothing

			case NM_IGNORE:

				break;
		}

			// Proceed to the next entry

		MenuTemplate++;
	}
	while(!Done && !Error);

	DB(kprintf("%s error = %ld\n",__FILE__,Error));

		// Did we create anything at all?

	if(!Error)
	{
		if(!Root->MenuList.mlh_Head->mln_Succ)
			Error = ERROR_OBJECT_NOT_FOUND;
		else
			LTP_FixExtraLabel(Root,&Error);
	}

	DB(kprintf("through, error=%ld\n",Error));

	if(Error)
	{
		if(ErrorPtr)
			*ErrorPtr = Error;

		return(FALSE);
	}
	else
		return(TRUE);
}

#endif	/* DO_MENUS */
