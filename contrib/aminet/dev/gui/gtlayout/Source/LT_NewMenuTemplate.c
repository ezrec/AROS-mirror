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

/****** gtlayout.library/LT_NewMenuTemplate ******************************************
*
*   NAME
*	LT_NewMenuTemplate -- Allocate and layout menu items (V11)
*
*   SYNOPSIS
*	Menu = LT_NewMenuTemplate(Screen,TextAttr,AmigaGlyph,CheckmarkGlyph,
*	 D0                         A0      A1        A2           A3
*
*	                          Error,MenuTemplate);
*	                            D0      D1
*
*	struct Menu *LT_NewMenuTemplate(struct Screen *,struct TextAttr *,
*	                                struct Image *,struct Image *,
*	                                LONG *,struct NewMenu *);
*
*   FUNCTION
*	Allocates Menus and MenuItems similar to LT_LayoutMenus().
*
*	As of v18 this routine will validate menu mutual exclusion
*	information.
*
*   INPUTS
*	Screen - Pointer to the screen the menu will appear on. This
*	    parameter is required and must not be omitted.
*
*	TextAttr - Pointer to the TextAttr that should be used to
*	    layout the menus. If this parameter is omitted,
*	    Screen->Font will be used instead.
*
*	AmigaGlyph - Pointer to the Image to use as the Amiga glyph.
*	    This parameter may be omitted.
*
*	        NOTE: Ignored by intuition.library v37 and below.
*
*	CheckmarkGlyph - Pointer to the Image to use as the checkmark
*	    glyph. This parameter may be omitted.
*
*	Error - Pointer to receive error code in case the menu
*	    creation or layout process fails. This parameter
*	    may be omitted.
*
*	MenuTemplate - Pointer to a series of NewMenu structures,
*	    just as you would pass to
*	    gtlayout.library/LT_LayoutMenuA.
*
*   RESULT
*	Menu - Pointer to Menu structure, ready to pass to
*	    SetMenuStrip(), NULL on failure.
*
*   NOTES
*	The menu created by this function cannot be used with the
*	routines LT_MenuControlTagList, LT_FindMenuCommand and
*	LT_GetMenuItem.
*
*	You may freely add, remove, spindle & mutilate the contents of the
*	menu strip created, just don't trash or disconnect the base menu
*	entry this routine creates as all menu memory tracking data is
*	connected with it.
*
*   SEE ALSO
*	gtlayout.library/LT_DisposeMenu
*	gtlayout.library/LT_LayoutMenuA
*	gtlayout.library/LT_NewMenuTagList
*	intuition.library/SetMenuStrip
*
******************************************************************************
*
*/

struct Menu * LIBENT
LT_NewMenuTemplate(REG(a0) struct Screen *screen,REG(a1) struct TextAttr *textAttr,REG(a2) struct Image *AmigaGlyph,REG(a3) struct Image *CheckGlyph,REG(d0) LONG *ErrorPtr,REG(d1) struct NewMenu *MenuTemplate)
{
	LONG Error;

	if(ErrorPtr)
		*ErrorPtr = 0;

	if(MenuTemplate)
	{
		RootMenu *Root;

		if(Root = LTP_NewMenu(screen,textAttr,AmigaGlyph,CheckGlyph,&Error))
		{
				// Create the menu

			if(LTP_CreateMenuTemplate(Root,&Error,MenuTemplate))
			{
					// Do the layout

				if(LTP_LayoutMenu(Root,2,2))
					return(&Root->Menu);
				else
					Error = ERROR_DISK_FULL;
			}

			LT_DisposeMenu(&Root->Menu);
		}
	}
	else
		Error = ERROR_REQUIRED_ARG_MISSING;

	if(ErrorPtr)
		*ErrorPtr = Error;

	return(NULL);
}

#endif	/* DO_MENUS */
