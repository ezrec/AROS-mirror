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

VOID
LTP_ResetListViewTextAttrs(ObjectNode *group)
{
	ObjectNode *node;

	SCANGROUP(group,node)
	{
		if(node->Type == GROUP_KIND)
			LTP_ResetListViewTextAttrs(node);
		else
		{
			if(node->Type == LISTVIEW_KIND && node->Special.List.TextAttr)
				node->Special.List.TextAttr = NULL;
		}
	}
}

VOID
LTP_Rescale(LayoutHandle *handle,BOOL trimWidth,BOOL trimHeight)
{
	STATIC const struct TextAttr topazAttr =
	{
		"topaz.font",
		8,
		FS_NORMAL,
		FPF_ROMFONT | FPF_DESIGNED
	};

	if(handle->TextAttr != &topazAttr && (Stricmp(handle->TextAttr->ta_Name,"topaz.font") || handle->TextAttr->ta_YSize != 8))
	{
		struct RastPort *rp;

		LONG	oldWidth;
		LONG	oldHeight;
		LONG	count;
		LONG	i;
		LONG	total;
		LONG	width;
		UBYTE	glyph;
		LONG	oldStyle;

		rp = &handle->RPort;

		oldStyle	= rp->AlgoStyle;
		oldWidth	= handle->GlyphWidth;
		oldHeight	= handle->GlyphHeight;

		LTP_ResetListViewTextAttrs(handle->TopGroup);

		if(handle->CloseFont)
			CloseFont(rp->Font);

		/*
		 * Note from Martin:
		 * GfxBase->DefaultFont is guaranteed not to ever be removed from
		 * memory, so we use that fact...
		 *
		 * I wasn't comfortable with that assumption, so I returned to
		 * the old way of doing it in v26.17
		 */

		Forbid();

		LTP_GetDefaultFont((struct TTextAttr *)(handle->TextAttr = (APTR)&handle->CopyTextAttr));

		LTP_SetFont(handle,LTP_OpenFont(handle->TextAttr));

		handle->CloseFont = TRUE;

		Permit();

		SetSoftStyle(rp,FSF_BOLD | FSF_UNDERLINED,FSF_BOLD | FSF_UNDERLINED);

		FOREVER
		{
			count = total = 0;

			for(i = 32 ; i < 256 ; i++)
			{
				if(i == 128)
					i = 160;

				glyph = i;

				width = TextLength(rp,&glyph,1);

				if(width > 0 && width < 32768)
				{
					total += width;
					count++;
				}
			}

			if((trimWidth && (total / count >= oldWidth)) || (trimHeight && (rp->TxHeight >= oldHeight)))
			{
				if(handle->TextAttr == &topazAttr || (!Stricmp(handle->TextAttr->ta_Name,"topaz.font") && handle->TextAttr->ta_YSize == 8))
					break;
				else
				{
					if(handle->CloseFont)
						CloseFont(rp->Font);

					handle->TextAttr = (struct TextAttr *)&topazAttr;
					LTP_SetFont(handle,OpenFont(handle->TextAttr));

					handle->CloseFont = TRUE;
				}
			}
			else
			{
				handle->Rescaled = TRUE;

				LTP_ResetGroups(handle->TopGroup);

				LTP_GlyphSetup(handle,handle->TextAttr);

				break;
			}
		}

		SetSoftStyle(rp,oldStyle,FSF_BOLD | FSF_UNDERLINED);
	}
}
