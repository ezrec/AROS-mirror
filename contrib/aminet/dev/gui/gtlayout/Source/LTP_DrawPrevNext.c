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

#ifdef DO_TAPEDECK_KIND	/* Support code */

VOID
LTP_DrawPrevNext(struct RastPort *RPort,BOOL Prev,LONG Left,LONG Top,LONG Width,LONG Height)
{
	LONG i,Len,Start,ArrowWidth,ArrowHeight,LineWidth,Pos;

	LineWidth	= (Width + 15) / 16;
	ArrowWidth	= Width;
	ArrowHeight	= Height;
	Left		= Left - (LineWidth / 2);

	for(i = 0 ; i < ArrowWidth ; i++)
	{
		Len = ((ArrowHeight * (i + 1)) / ArrowWidth) & ~1;

		if(Len < ArrowHeight)
			Len++;

		Start = Top + (ArrowHeight - Len) / 2;

		if(Prev)
			Pos = Left + i;
		else
			Pos = Left + ArrowWidth - 1 - i;

		LTP_DrawLine(RPort,Pos,Start,Pos,Start + Len - 1);
	}

	for(i = 0 ; i < LineWidth ; i++)
	{
		if(Prev)
			Pos = Left + i;
		else
			Pos = Left + ArrowWidth - 1 - i;

		LTP_DrawLine(RPort,Pos,Top,Pos,Top + Height - 1);
	}
}

#endif	/* DO_TAPEDECK_KIND */
