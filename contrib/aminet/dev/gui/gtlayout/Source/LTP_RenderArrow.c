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
LTP_RenderArrow(struct RastPort *RPort,BOOL LeftDirection,LONG Left,LONG Top,LONG Width,LONG Height)
{
	LONG i,Len,Start,Pos;

	for(i = 0 ; i < Width ; i++)
	{
		Len = ((Height * (i + 1)) / Width) & ~1;

		if(Len < Height)
			Len++;

		Start = Top + (Height - Len) / 2;

		if(LeftDirection)
			Pos = Left + i;
		else
			Pos = Left + Width - 1 - i;

		LTP_DrawLine(RPort,Pos,Start,Pos,Start + Len - 1);
	}
}

#endif	/* DO_TAPEDECK_KIND */
