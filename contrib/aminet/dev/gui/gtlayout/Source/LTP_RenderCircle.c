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
LTP_RenderCircle(struct RastPort *RPort,LONG Left,LONG Top,LONG Radius,LONG AspectX,LONG AspectY)
{
	LONG x = 0,y = Radius,Delta = 2 * (1 - Radius),delta,Limit = 0,Length;

	while(y >= Limit)
	{
		if(Delta < 0)
		{
			delta = 2 * Delta + 2 * y - 1;

			if(delta > 0)
			{
				x = x + 1;
				y = y - 1;
				Delta = Delta + 2 * x - 2 * y + 2;
			}
			else
			{
				x = x + 1;
				Delta = Delta + 2 * x + 1;
			}
		}
		else
		{
			if(Delta > 0)
			{
				delta = 2 * Delta - 2 * x - 1;

				if(delta > 0)
				{
					y = y - 1;
					Delta = Delta - 2 * y + 1;
				}
				else
				{
					x = x + 1;
					y = y - 1;
					Delta = Delta + 2 * x - 2 * y + 2;
				}
			}
			else
			{
				x = x + 1;
				y = y - 1;
				Delta = Delta + 2 * x - 2 * y + 2;
			}
		}

		Length = (x * AspectY) / AspectX;

		LTP_DrawLine(RPort,Left - Length,Top + y - 1,Left + Length - 1,Top + y - 1);
		LTP_DrawLine(RPort,Left - Length,Top - y,Left + Length - 1,Top - y);
	}
}

#endif	/* DO_TAPEDECK_KIND */
