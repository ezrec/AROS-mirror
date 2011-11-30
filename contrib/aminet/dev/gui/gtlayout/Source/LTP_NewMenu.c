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

#include <exec/memory.h>

#include <dos/dos.h>	/* For AmigaDOS error definitions */

#include <clib/alib_protos.h>	/* For NewList */

/*****************************************************************************/

#include "Assert.h"

#ifdef DO_MENUS	/* Support code */

	/* LTP_NewMenu():
	 *
	 *	Create a new menu, based on the Screen and Font given.
	 */

RootMenu *
LTP_NewMenu(struct Screen *Screen,struct TextAttr *TextAttr,struct Image *AmigaGlyph,struct Image *CheckGlyph,LONG *ErrorPtr)
{
	LONG Error;

	if(Screen)
	{
		APTR Pool;

			// Wrap all the allocations into a pool

		if(Pool = AsmCreatePool(MEMF_ANY | MEMF_PUBLIC | MEMF_CLEAR,1024,1024,SysBase))
		{
			struct RootMenu *Root;

				// Create the root

			if(Root = (struct RootMenu *)AsmAllocPooled(Pool,sizeof(struct RootMenu),SysBase))
			{
				Root->Pool = Pool;

				if(TextAttr)
					Root->TextAttr = TextAttr;
				else
					Root->TextAttr = Screen->Font;

					// Open the menu font

				if(Root->Font = OpenFont(Root->TextAttr))
				{
						// Get the drawing information

					if(Root->DrawInfo = GetScreenDrawInfo(Screen))
					{
							// Fill in the dummy rastport

						InitRastPort(&Root->RPort);

						Root->RPort.BitMap = Screen->RastPort.BitMap;

						SetFont(&Root->RPort,Root->Font);

								// Get the text rendering pen

						if(Root->DrawInfo->dri_Version < 2)
							Root->TextPen = Root->DrawInfo->dri_Pens[DETAILPEN];
						else
							Root->TextPen = Root->DrawInfo->dri_Pens[BARDETAILPEN];

								// Something to remember

						Root->Screen = Screen;

							// Let's hope it won't grow in the future

						CopyMem(Root->TextAttr,&Root->BoldAttr,sizeof(struct TTextAttr));

							// Make it boldface

						Root->BoldAttr.tta_Style |= FSF_BOLD;

							// Initialize the lists

						NewList((struct List *)&Root->MenuList);
						NewList((struct List *)&Root->ItemList);

							// Get the glyph widths

						if(CheckGlyph)
						{
							GetAttr(IA_Width,	(Object *)CheckGlyph,&Root->CheckWidth);
							GetAttr(IA_Height,	(Object *)CheckGlyph,&Root->CheckHeight);
						}
						else
						{
								// No glyph is provided, use the default values

							if(V39)
							{
								struct Image *Glyph;

								if(Glyph = NewObject(NULL,SYSICLASS,
									SYSIA_DrawInfo, 		Root->DrawInfo,
									SYSIA_Which,			MENUCHECK,
									SYSIA_ReferenceFont,	Root->Font,
								TAG_DONE))
								{
									GetAttr(IA_Width,	(Object *)Glyph,&Root->CheckWidth);
									GetAttr(IA_Height,	(Object *)Glyph,&Root->CheckHeight);

									DisposeObject(Glyph);
								}
							}

							if(!Root->CheckWidth)
								Root->CheckWidth = 15;

							if(!Root->CheckHeight)
								Root->CheckHeight = 8;
						}

						if(AmigaGlyph)
						{
							GetAttr(IA_Width,	(Object *)AmigaGlyph,&Root->AmigaWidth);
							GetAttr(IA_Height,	(Object *)AmigaGlyph,&Root->AmigaHeight);
						}
						else
						{
								// No glyph is provided, use the default values

							if(V39)
							{
								struct Image *Glyph;

								if(Glyph = NewObject(NULL,SYSICLASS,
									SYSIA_DrawInfo, 		Root->DrawInfo,
									SYSIA_Which,			AMIGAKEY,
									SYSIA_ReferenceFont,	Root->Font,
								TAG_DONE))
								{
									GetAttr(IA_Width,	(Object *)Glyph,&Root->AmigaWidth);
									GetAttr(IA_Height,	(Object *)Glyph,&Root->AmigaHeight);

									DisposeObject(Glyph);
								}
							}

							if(!Root->AmigaWidth)
								Root->AmigaWidth = 23;

							if(!Root->AmigaHeight)
								Root->AmigaHeight = 8;
						}

							// Establish default menu item height

						Root->ItemHeight = Root->RPort.TxHeight;

						if(Root->CheckHeight > Root->ItemHeight)
							Root->ItemHeight = Root->CheckHeight;

						if(Root->AmigaHeight > Root->ItemHeight)
							Root->ItemHeight = Root->AmigaHeight;

						Root->ItemHeight += 2;

						return(Root);
					}
					else
						Error = ERROR_NO_FREE_STORE;

					CloseFont(Root->Font);
				}
				else
					Error = ERROR_OBJECT_NOT_FOUND;
			}
			else
				Error = ERROR_NO_FREE_STORE;

			AsmDeletePool(Pool,SysBase);
		}
		else
			Error = ERROR_NO_FREE_STORE;
	}
	else
		Error = ERROR_REQUIRED_ARG_MISSING;

	if(ErrorPtr)
		*ErrorPtr = Error;

	return(NULL);
}

#endif	/* DO_MENUS */
