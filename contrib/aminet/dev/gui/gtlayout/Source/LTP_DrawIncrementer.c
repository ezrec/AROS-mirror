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
LTP_DrawIncrementer(struct RastPort *rp,struct DrawInfo *drawInfo,BOOL leftDirection,LONG left,LONG top,LONG width,LONG height)
{
	LONG arrowWidth,arrowHeight;
	LONG mid,a,b,inc,i;

	arrowWidth	= (width < height ? width : height) - 4;
	arrowHeight	= (drawInfo->dri_Resolution.X * arrowWidth) / drawInfo->dri_Resolution.Y;

	if(arrowHeight < 5)
		arrowHeight = 5;

	if(arrowHeight > height)
		arrowHeight = height;

	arrowHeight = ((arrowHeight - 1) & ~1) + 1;

	left	+= (width - arrowWidth) / 2;
	top		+= (height - arrowHeight) / 2;

	mid = top + arrowHeight / 2;

	if(leftDirection)
	{
		a	= left + arrowWidth - 1;
		b	= left;
		inc	= 1;
	}
	else
	{
		a	= left;
		b	= left + arrowWidth - 1;
		inc	= -1;
	}

	for(i = 0 ; i < 4 ; i++)
	{
			// Note: two calls to ensure symmetry

		LTP_DrawLine(rp,a,top,b,mid);
		LTP_DrawLine(rp,a,top + arrowHeight - 1,b,mid);

		b += inc;
	}
}
