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

	/* AdjustMenuPosition(RootMenu *Root):
	 *
	 *	Calculate the effective positions of the menus
	 *	and submenus.
	 */

VOID
LTP_AdjustMenuPosition(RootMenu *Root)
{
	MenuNode *Menu;

	for(Menu = (MenuNode *)Root->MenuList.mlh_Head ; Menu->Node.mln_Succ ; Menu = (MenuNode *)Menu->Node.mln_Succ)
		Menu->Width = LTP_AdjustItemPosition(Menu->Menu.FirstItem,Menu->Menu.LeftEdge + 4,Root->Screen->BarHeight + 1);
}

#endif	/* DO_MENUS */
