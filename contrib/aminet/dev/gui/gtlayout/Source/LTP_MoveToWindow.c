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
LTP_MoveToWindow(LayoutHandle *handle)
{
	if(V39)
	{
		WORD left,top,width,height;

		LTP_GetDisplayClip(handle->Screen,&left,&top,&width,&height);

		if(handle->Window->Width < width)
			left = (width - handle->Window->Width) / 2;
		else
			left = 0;

		if(handle->Window->Height < height)
			top = (height - handle->Window->Height) / 2;
		else
			top = 0;

		if((left = handle->Window->LeftEdge - left) < 0)
			left = 0;

		if((top = handle->Window->TopEdge - top) < 0)
			top = 0;

		ScreenPosition(handle->Screen,SPOS_MAKEVISIBLE,left,top,left + width - 1,top + height - 1);
	}
}
