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
LTP_DeleteObjectNode(LayoutHandle *handle,ObjectNode *node)
{
	struct Image **ImagePtr = NULL;

	switch(node->Type)
	{
		case PICKER_KIND:

			ImagePtr = &node->Special.Picker.Image;
			break;

		case BUTTON_KIND:

			ImagePtr = &node->Special.Button.ButtonImage;
			break;

		case INCREMENTER_KIND:

			ImagePtr = &node->Special.Incrementer.Image;
			break;

		case TAPEDECK_KIND:

			ImagePtr = &node->Special.TapeDeck.ButtonImage;
			break;
	}

	if(ImagePtr)
	{
		DisposeObject(*ImagePtr);

		*ImagePtr = NULL;
	}
}
