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

struct Gadget *
LTP_CreateExtraObject(LayoutHandle *handle,ObjectNode *parentNode,struct Gadget *parentGadget,struct NewGadget *ng,LONG imageType,LONG incAmount)
{
	ObjectNode		*node;
	struct Gadget	*gadget;

	if((node = LTP_CreateObjectNode(handle,incAmount ? INCREMENTER_KIND : PICKER_KIND,parentGadget->GadgetID,NULL)))
	{
		ng->ng_LeftEdge		= ng->ng_LeftEdge + ng->ng_Width;
		ng->ng_Width		= incAmount ? (4 + handle->GlyphWidth + 4) : LTP_GetPickerSize(handle);
		ng->ng_GadgetText	= "";
		ng->ng_UserData		= node;
		ng->ng_Flags		= 0;

		if(incAmount)
			node->Special.Incrementer.Amount = incAmount;

		node->Special.Incrementer.Image = NewObject(LTP_ImageClass,NULL,
			IIA_ImageType,	imageType,
			IA_Width,		ng->ng_Width,
			IA_Height,		ng->ng_Height,
		TAG_DONE);

		if(node->Special.Incrementer.Image)
		{
			if((gadget = CreateGadgetA(GENERIC_KIND,handle->Previous,ng,NULL)))
			{
				gadget->GadgetType		|= GTYP_BOOLGADGET;
				gadget->Flags			|= GFLG_GADGIMAGE | GFLG_GADGHIMAGE;
				gadget->Activation		|= GACT_IMMEDIATE | GACT_RELVERIFY;
				gadget->GadgetRender	 = node->Special.Incrementer.Image;
				gadget->SelectRender	 = node->Special.Incrementer.Image;

				if(parentNode->Disabled)
					gadget->Flags |= GFLG_DISABLED;

				node->Host = gadget;

				node->Special.Incrementer.Parent = parentGadget;

				return(gadget);
			}
			else
				LTP_DeleteObjectNode(handle,node);
		}
	}

	return(NULL);
}
