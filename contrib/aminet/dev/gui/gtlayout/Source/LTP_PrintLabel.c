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
LTP_PrintLabel(LayoutHandle *handle,ObjectNode *node,LONG left,LONG top)
{
	struct RastPort *rp;
	STRPTR label;
	LONG underscore;
	LONG len;

	rp = &handle->RPort;

	LTP_SetPens(rp,node->HighLabel ? handle->DrawInfo->dri_Pens[HIGHLIGHTTEXTPEN] : handle->TextPen,0,JAM1);

	Move(rp,left - (node->LabelWidth + INTERWIDTH),top + rp->TxBaseline);

	label = node->Label;

	len = strlen(label);
	underscore = 0;

	while(label[underscore] != '_' && underscore < len)
		underscore++;

	if(underscore)
		Text(rp,label,underscore);

	if(underscore < len - 1)
	{
		ULONG OldStyle;

		underscore += 2;
		label += underscore;
		len -= underscore;

		OldStyle = rp->AlgoStyle;

		SetSoftStyle(rp,FSF_UNDERLINED,FSF_UNDERLINED);

		Text(rp,label - 1,1);

		SetSoftStyle(rp,OldStyle,FSF_UNDERLINED);

		if(len)
			Text(rp,label,len);
	}
}
