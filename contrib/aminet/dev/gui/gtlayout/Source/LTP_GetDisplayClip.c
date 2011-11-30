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

#include <graphics/videocontrol.h>

/*****************************************************************************/

#include "Assert.h"

VOID
LTP_GetDisplayClip(struct Screen *screen,WORD *left,WORD *top,WORD *width,WORD *height)
{
	struct TagItem tags[2] = { { VTAG_VIEWPORTEXTRA_GET, (IPTR)NULL }, { TAG_DONE } };

	if(!VideoControl(screen->ViewPort.ColorMap,tags))
	{
		struct ViewPortExtra	*vpe = (struct ViewPortExtra *)tags[0].ti_Data;
		struct Rectangle		 clip;

		QueryOverscan(GetVPModeID(&screen->ViewPort),&clip,OSCAN_TEXT);

		*width	= vpe->DisplayClip.MaxX - vpe->DisplayClip.MinX + 1;
		*height = vpe->DisplayClip.MaxY - vpe->DisplayClip.MinY + 1;

		if(*width < clip.MaxX - clip.MinX + 1)
			*width = clip.MaxX - clip.MinX + 1;

		if(*height < clip.MaxY - clip.MinY + 1)
			*height = clip.MaxY - clip.MinY + 1;

		if(*width > screen->Width)
			*width = screen->Width;

		if(*height > screen->Height)
			*height = screen->Height;
	}
	else
	{
		*width	= screen->Width;
		*height = screen->Height;
	}

	*left = screen->LeftEdge;
	*top  = screen->TopEdge;

	if(*left > 0)
		*left = 0;
	else
		*left = -(*left);

	if(*top > 0)
		*top = 0;
	else
		*top = -(*top);

	if((*left > screen->Width) || (*left < 0))
		*left = 0;

	if ((*top > screen->Height) || (*top < 0))
		*top = 0;
}
