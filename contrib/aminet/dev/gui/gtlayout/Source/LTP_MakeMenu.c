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

	/* LTP_MakeMenu(RootMenu *Root,MenuNode *Menu,struct NewMenu *Template):
	 *
	 *	Create a single menu and fill it in.
	 */

MenuNode *
LTP_MakeMenu(RootMenu *Root,MenuNode *Menu,struct NewMenu *Template)
{
		// If none is provided, roll our own

	if(!Menu)
		Menu = (MenuNode *)AsmAllocPooled(Root->Pool,sizeof(MenuNode),SysBase);

	if(Menu)
	{
		CONST_STRPTR Label = Template->nm_Label;

			// Use the screen font for the layout

		SetFont(&Root->RPort,Root->DrawInfo->dri_Font);

			// Fill in the size

		Menu->Menu.Width	= TextLength(&Root->RPort,Label,strlen(Label)) + 8;
		Menu->Menu.Height	= Root->Screen->BarHeight;

		Menu->Menu.MenuName	= (BYTE *)Label;
		Menu->UserData		= Template->nm_UserData;

			// Return to the original font

		SetFont(&Root->RPort,Root->Font);

			// Take care of the only valid bit

		if(!(Template->nm_Flags & NM_MENUDISABLED))
			Menu->Menu.Flags = MENUENABLED;
	}

	return(Menu);
}

#endif	/* DO_MENUS */
