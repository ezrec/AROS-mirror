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

/****** gtlayout.library/LT_DisposeMenu ******************************************
*
*   NAME
*	LT_DisposeMenu -- Release storage space allocated by
*	                  LT_NewMenuTemplate or LT_NewMenuTagList (V11)
*
*   SYNOPSIS
*	LT_DisposeMenu(Menu)
*	                A0
*
*	VOID LT_DisposeMenu(struct Menu *);
*
*   FUNCTION
*	Menus and MenuItems allocated by LT_NewMenuTemplate or
*	LT_NewMenuTagList are deallocated.
*
*   INPUTS
*	Menu - Pointer to Menu structure as returned by
*	    LT_NewMenuTemplate or LT_NewMenuTagList. Passing
*	    NULL is harmless.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gtlayout.library/LT_NewMenuTagList
*	gtlayout.library/LT_NewMenuTemplate
*
******************************************************************************
*
*/

VOID LIBENT
LT_DisposeMenu(REG(a0) struct Menu *menu)
{
	if(menu)
	{
		struct RootMenu *Root = (struct RootMenu *)((IPTR)menu - offsetof(struct RootMenu,Menu));

		FreeScreenDrawInfo(Root->Screen,Root->DrawInfo);

		CloseFont(Root->Font);

		AsmDeletePool(Root->Pool,SysBase);
	}
}

#endif	/* DO_MENUS */
