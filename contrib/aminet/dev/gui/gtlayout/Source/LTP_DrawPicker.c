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
LTP_DrawPicker(struct RastPort *RPort,BOOL UpDirection,LONG Left,LONG Top,LONG Width,LONG Height)
{
	LONG i,Len,ArrowHeight,ArrowWidth,LineHeight,Start,Pos;

	LineHeight	= (Height + 15) / 16;
	ArrowHeight	= Height - (Height + 7) / 8;
	ArrowWidth	= Width;

	for(i = 0 ; i < ArrowHeight ; i++)
	{
		Len = ((ArrowWidth * (i + 1)) / ArrowHeight) & ~1;

		if(Len < ArrowWidth)
			Len++;

		Start = Left + (ArrowWidth - Len) / 2;

		if(UpDirection)
			Pos = Top + i;
		else
			Pos = Top + ArrowHeight - 1 - i;

		LTP_DrawLine(RPort,Start,Pos,Start + Len - 1,Pos);
	}

	for(i = 0 ; i < LineHeight ; i++)
		LTP_DrawLine(RPort,Left,Top + Height - 1 - i,Left + Width - 1,Top + Height - 1 - i);
}

VOID
LTP_DrawAdjustedPicker(
	struct RastPort *	rp,
	BOOL				upDirection,
	LONG				left,
	LONG				top,
	LONG				width,
	LONG				height,
	LONG				aspectX,
	LONG				aspectY)
{
	LONG Width,Height;

	Width	= 2 + (((width + 15) / 16) * aspectY) / aspectX;
	Height	= 1 + ((height + 15) / 16);

	if(Width < 4)
		Width = 4;

	LTP_DrawPicker(rp,FALSE,left + Width,top + Height,width - 2 * Width,height - 2 * Height);
}
