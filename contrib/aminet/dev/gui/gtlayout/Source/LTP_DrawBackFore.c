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
LTP_DrawBackFore(struct RastPort *RPort,BOOL Back,LONG Left,LONG Top,LONG Width,LONG Height)
{
	LONG i,Len,Start,ArrowWidth,ArrowHeight,Shift,x1,x2;

	ArrowWidth	= Width / 2;
	ArrowHeight	= Height;
	Left		= Left + (Width - ArrowWidth) / 2;
	Shift		= ArrowWidth / 2;

	for(i = 0 ; i < ArrowWidth ; i++)
	{
		Len = ((ArrowHeight * (i + 1)) / ArrowWidth) & ~1;

		if(Len < ArrowHeight)
			Len++;

		Start = Top + (ArrowHeight - Len) / 2;

		if(Back)
		{
			x1 = Left + i - Shift;
			x2 = Left + i + Shift;
		}
		else
		{
			x1 = Left + ArrowWidth - 1 - i - Shift;
			x2 = Left + ArrowWidth - 1 - i + Shift;
		}

		LTP_DrawLine(RPort,x1,Start,x1,Start + Len - 1);
		LTP_DrawLine(RPort,x2,Start,x2,Start + Len - 1);
	}
}

#endif	/* DO_TAPEDECK_KIND */
