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


/*****************************************************************************/


BOOL
LTP_PrintLinePadded(LayoutHandle *Handle,LONG Left,LONG Top,LONG Space,STRPTR Line,LONG Len,LONG textPen,LONG backPen)
{
	LONG Size,Start,Count,i,Width;
	struct RastPort *RPort;

	RPort	= &Handle->RPort;
	Size	= 0;
	Start	= 0;
	Count	= 0;
	i		= 0;
	Width	= 0;

	while(i < Len && Line[i] == ' ')
	{
		Size++;
		i++;
	}

	while(i < Len)
	{
		while(i < Len && Line[i] != ' ')
		{
			Size++;
			i++;
		}

		Width += TextLength(RPort,&Line[Start],Size);

		Count++;

		while(i < Len && Line[i] == ' ')
			i++;

		Start	= i;
		Size	= 0;
	}

	if(Width)
	{
		LONG j,TextLeft;

		Space -= Width;

		j = Start = Size = 0;

		while(j < Len && Line[j] == ' ')
		{
			Size++;
			j++;
		}

		LTP_SetAPen(RPort,textPen);

		TextLeft = Left;

		Count--;

		for(i = 0 ; i <= Count ; i++)
		{
			if(i)
			{
				LONG Delta;

				if(Delta = (Space * i) / Count - (Space * (i - 1)) / Count)
				{
					LTP_SetAPen(RPort,backPen);
					LTP_FillBox(RPort,TextLeft,Top,Delta,Handle->GlyphHeight);
					LTP_SetAPen(RPort,textPen);

					TextLeft += Delta;
				}
			}

			while(j < Len && Line[j] != ' ')
			{
				Size++;
				j++;
			}

			LTP_PrintText(RPort,&Line[Start],Size,TextLeft,Top);

			TextLeft += TextLength(RPort,&Line[Start],Size);

			while(j < Len && Line[j] == ' ')
				j++;

			Start = j;
			Size = 0;
		}

		return(TRUE);
	}
	else
		return(FALSE);
}


/*****************************************************************************/


VOID
LTP_PrintLine(LayoutHandle *handle,LONG alignType,LONG left,LONG top,LONG space,STRPTR line,LONG len,LONG textPen,LONG backPen)
{
	struct RastPort *rp;

	/* New in 40.11: auto-adapt pen numbers if -1. */

	if(textPen == -1)
		textPen = handle->TextPen;

	if(backPen == -1)
		backPen = handle->BackgroundPen;

	rp = &handle->RPort;

	LTP_SetPens(rp,backPen,backPen,JAM2);

	if(alignType == ALIGNTEXT_PAD)
	{
		if(LTP_PrintLinePadded(handle,left,top,space,line,len,textPen,backPen))
			return;
	}
	else
	{
		struct TextExtent extent;
		LONG width;

		len		= TextFit(rp,line,len,&extent,NULL,1,space,32767);
		width	= extent.te_Width;

		if(len)
		{
			LONG textLeft;

			switch(alignType)
			{
				case ALIGNTEXT_LEFT:

					textLeft = left;

					if(width < space)
						LTP_FillBox(rp,left + width,top,space - width,handle->GlyphHeight);

					break;

				case ALIGNTEXT_CENTERED:

					textLeft = left + (space - width) / 2;

					if(width < space)
					{
						LONG fill;

						if((fill = (space - width) / 2) > 0)
							LTP_FillBox(rp,left,top,fill,handle->GlyphHeight);
						else
							fill = 0;

						fill += width;

						if(fill < space)
							LTP_FillBox(rp,left + fill,top,space - fill,handle->GlyphHeight);
					}

					break;

				case ALIGNTEXT_RIGHT:

					textLeft = left + space - width;

					if(width < space)
						LTP_FillBox(rp,left,top,space - width,handle->GlyphHeight);

					break;

				default:

					return;	/* Note: only in case none of the alignment types match; should never happen! */
			}

			LTP_SetAPen(rp,textPen);
			LTP_PrintText(rp,line,len,textLeft,top);

			return;
		}
	}

	LTP_FillBox(rp,left,top,space,handle->GlyphHeight);
}


/*****************************************************************************/


VOID
LTP_PrintBoxLine(LayoutHandle *handle,ObjectNode *node,LONG index)
{
	if(node->Special.Box.Lines && node->Special.Box.Lines[index])
	{
		LTP_PrintLine(handle,node->Special.Box.AlignText,node->Left + 4,node->Top + 2 + index * (handle->GlyphHeight + node->Special.Box.Spacing),node->Width - 8,node->Special.Box.Lines[index],strlen(node->Special.Box.Lines[index]),node->Special.Box.TextPen,node->Special.Box.BackPen);

		if(index < node->Lines - 1 && node->Special.Box.Spacing > 0)
		{
			struct RastPort *rp = &handle->RPort;
			LONG pen;

			if(node->Special.Box.BackPen == -1)
				pen = handle->BackgroundPen;
			else
				pen = node->Special.Box.BackPen;

			LTP_SetPens(rp,pen,pen,JAM2);
			LTP_FillBox(rp,node->Left + 4,node->Top + 2 + (index + 1) * (handle->GlyphHeight + node->Special.Box.Spacing),node->Width - 8,node->Special.Box.Spacing);
		}
	}
}
