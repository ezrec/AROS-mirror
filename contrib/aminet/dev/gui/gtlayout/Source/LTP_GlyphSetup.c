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

#include <clib/diskfont_protos.h>
#include <pragmas/diskfont_pragmas.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

VOID
LTP_GetDefaultFont(struct TTextAttr *TextAttr)
{
	struct RastPort LocalRastPort;

	InitRastPort(&LocalRastPort);

	memset(TextAttr,0,sizeof(*TextAttr));

	AskFont(&LocalRastPort,(struct TextAttr *)TextAttr);
}


/*****************************************************************************/


struct TextFont *
LTP_OpenFont(struct TextAttr *TextAttr)
{
	struct TextFont *Font;

		// Let's see if this one is special

	if(TextAttr == (struct TextAttr *)~0)
	{
		struct TTextAttr LocalTextAttr;

		memset(&LocalTextAttr,0,sizeof(LocalTextAttr));

		Forbid();

			// Borrow the system default font data

		LTP_GetDefaultFont(&LocalTextAttr);

			// Let's hope the font will open...

		Font = OpenFont((struct TextAttr *)&LocalTextAttr);

		Permit();
	}
	else
	{
		if(!(Font = OpenFont(TextAttr)))
		{
			if(FindTask(NULL)->tc_Node.ln_Type != NT_TASK)
			{
				struct Library *DiskfontBase;

				if(DiskfontBase = OpenLibrary("diskfont.library",0))
				{
					Font = OpenDiskFont(TextAttr);

					CloseLibrary(DiskfontBase);
				}
			}
		}
	}

	return(Font);
}


/*****************************************************************************/


BOOL
LTP_GlyphSetup(struct LayoutHandle *Handle,struct TextAttr *TextAttr)
{
	struct TextFont *Font;

	if(!TextAttr)
		TextAttr = Handle->Screen->Font;

	if(Font = LTP_OpenFont(Handle->TextAttr = TextAttr))
	{
		struct RastPort *RPort;
		LONG Count;
		LONG Total;
		ULONG OldStyle;
		LONG GlyphWidth;
		LONG i;
		UBYTE Glyph;

		RPort = &Handle->RPort;

		if(Handle->CloseFont)
			CloseFont(RPort->Font);

		Handle->CloseFont = TRUE;

		LTP_SetFont(Handle,Font);

		OldStyle = RPort->AlgoStyle;

		SetSoftStyle(RPort,FSF_BOLD | FSF_UNDERLINED,FSF_BOLD | FSF_UNDERLINED);

			// Cover all printable characters

		Count = Total = 0;

		for(i = 32 ; i < 256 ; i++)
		{
			if(i == 128)
				i = 160;

			Glyph = i;

			GlyphWidth = TextLength(RPort,&Glyph,1);

				// For really pathologic fonts...

			if(GlyphWidth >= 0 && GlyphWidth <= 32767)
			{
				Total += GlyphWidth;

				Count++;
			}
		}

		Handle->GlyphWidth = Total / Count;

			// Now for the numeric characters

		for(Total = 0, i = '0' ; i <= '9' ; i++)
		{
			Glyph = i;

			Total += TextLength(RPort,&Glyph,1);
		}

		Total /= 10;

			// Actually, the numeric character should be
			// just as wide as the space character. Let's
			// hope for the best.

		Count = TextLength(RPort," ",1);

		if(Total < Count)
			Total = Count;

		if(Total > Handle->GlyphWidth)
			Handle->GlyphWidth = Total;

		if(Handle->GlyphWidth <= 8)
			Handle->InterWidth = 4;
		else
			Handle->InterWidth = Handle->GlyphWidth / 2;

		Handle->GlyphHeight = RPort->TxHeight;

		if(Handle->GlyphHeight <= 8)
			Handle->InterHeight = 2;
		else
			Handle->InterHeight = Handle->GlyphHeight / 4;

		SetSoftStyle(RPort,OldStyle,FSF_BOLD | FSF_UNDERLINED);

		return(TRUE);
	}
	else
		return(FALSE);
}
