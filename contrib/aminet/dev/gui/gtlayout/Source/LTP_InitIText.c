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

	/* LTP_InitIText(RootMenu *Root,struct IntuiText *IText):
	 *
	 *	Initialize IntuiText data structure with defaults.
	 */

VOID
LTP_InitIText(RootMenu *Root,struct IntuiText *IText)
{
	IText->FrontPen		= Root->TextPen;
	IText->ITextFont	= Root->TextAttr;
}

#endif	/* DO_MENUS */
