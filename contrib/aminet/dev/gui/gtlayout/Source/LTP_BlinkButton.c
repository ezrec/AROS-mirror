/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"

BOOL
LTP_BlinkButton(LayoutHandle *handle,struct Gadget *gadget)
{
	ObjectNode *Node;
	BOOL Result;

	Result = FALSE;

	Node = (ObjectNode *)gadget->UserData;

	if(Node->Host == gadget)
	{
		switch(Node->Type)
		{
			#ifdef DO_POPUP_KIND
			{
				case POPUP_KIND:

					SetGadgetAttrs(Node->Host,handle->Window,NULL,
						PIA_Highlight,	TRUE,
					TAG_DONE);

					LTP_Delay(0,80000);

					SetGadgetAttrs(Node->Host,handle->Window,NULL,
						PIA_Highlight,	FALSE,
					TAG_DONE);

					Result = TRUE;

					break;
			}
			#endif

			#ifdef DO_TAB_KIND
			{
				case TAB_KIND:

					break;
			}
			#endif

			default:

				if((gadget->Flags & (GFLG_GADGIMAGE | GFLG_GADGHIMAGE)) == (GFLG_GADGIMAGE | GFLG_GADGHIMAGE) && gadget->GadgetRender && gadget->SelectRender)
				{
					DrawImageState(&handle->RPort,gadget->SelectRender,gadget->LeftEdge,gadget->TopEdge,IDS_SELECTED,handle->DrawInfo);

					LTP_Delay(0,80000);

					DrawImageState(&handle->RPort,gadget->GadgetRender,gadget->LeftEdge,gadget->TopEdge,IDS_NORMAL,handle->DrawInfo);
				}

				Result = TRUE;

				break;
		}
	}

	return(Result);
}
