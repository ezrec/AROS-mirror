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
LTP_DrawPalette(struct LayoutHandle *Handle,struct ObjectNode *Node)
{
	struct RastPort *rp = &Handle->RPort;

	LTP_SetAPen(rp,Node->Current);
	LTP_FillBox(rp,Node->Left + 3,Node->Top + 2,Node->Special.Palette.IndicatorWidth - 6,Node->Height - 4);

	LTP_DrawBevel(Handle,Node->Left,Node->Top,Node->Special.Palette.IndicatorWidth,Node->Height);
}
