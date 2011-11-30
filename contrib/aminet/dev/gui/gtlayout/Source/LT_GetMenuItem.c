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

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

/****** gtlayout.library/LT_GetMenuItem ******************************************
*
*   NAME
*	LT_GetMenuItem -- Get the menu/submenu item associated with an ID (V11)
*
*   SYNOPSIS
*	Item = LT_GetMenuItem(Menu,ID)
*	 D0                    A0  D0
*
*	struct MenuItem *LT_GetMenuItem(struct Menu *,ULONG);
*
*   FUNCTION
*	This routine scans through all menu items associated with the
*	menu and returns a pointer to the item associated with the
*	given ID value.
*
*   INPUTS
*	Menu - Pointer to Menu structure as returned by LT_NewMenuTagList.
*
*	ID - Unique ID of the item to find.
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

	/* LT_GetMenuItem(struct Menu *Menu,ULONG ID):
	 *
	 *	Obtain the menu item associated with the ID.
	 */

struct MenuItem * LIBENT
LT_GetMenuItem(REG(a0) struct Menu *Menu,REG(d0) ULONG ID)
{
	RootMenu *Root = (RootMenu *)((IPTR)Menu - offsetof(RootMenu,Menu));
	ItemNode *Item;

		// Run down the list...

	for(Item = (ItemNode *)Root->ItemList.mlh_Head ; Item->Node.mln_Succ ; Item = (ItemNode *)Item->Node.mln_Succ)
	{
		if(Item->ID == ID)
			return(&Item->Item);
	}

		// Nothing found

	return(NULL);
}

#endif	/* DO_MENUS */
