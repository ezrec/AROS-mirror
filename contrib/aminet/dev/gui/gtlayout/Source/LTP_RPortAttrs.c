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


VOID
LTP_SetPens(struct RastPort *rp,LONG apen,LONG bpen,LONG mode)
{
	if(V39)
		SetABPenDrMd(rp,apen,bpen,mode);
	else
	{
		if(apen != rp->FgPen)
			SetAPen(rp,apen);

		if(bpen != rp->BgPen)
			SetBPen(rp,bpen);

		if(mode != rp->DrawMode)
			SetDrMd(rp,mode);
	}
}


/*****************************************************************************/


VOID
LTP_SetAPen(struct RastPort *rp,LONG apen)
{
	ULONG pen;

	if(V39)
		pen = GetAPen(rp);
	else
		pen = rp->FgPen;

	if(pen != apen)
		SetAPen(rp,apen);
}


/*****************************************************************************/


VOID
LTP_SetFont(LayoutHandle *handle,struct TextFont *font)
{
	SetFont(&handle->RPort,font);

	if(handle->Window)
		SetFont(handle->Window->RPort,font);
}
