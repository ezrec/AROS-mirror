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

/****** gtlayout.library/LT_PressButton ******************************************
*
*   NAME
*	LT_PressButton -- Highlight a button so it looks as if the user
*	                  has selected it.
*
*   SYNOPSIS
*	Success = LT_PressButton(Handle,ID);
*	   D0                       A0  D0
*
*	BOOL LT_PressButton(LayoutHandle *,LONG);
*
*	    NOTE: The return value did not exist until V39!
*
*   FUNCTION
*	You can provide visual feedback for BUTTON_KIND objects by calling
*	this routine. They will briefly appear to be selected and then
*	fall back to their original states.
*
*	As of V39 and beyond this function will indicate whether the button
*	could be highlighted or not.
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure
*
*	ID - ID of button object to highlight
*
*   RESULT
*	Success (V39) - TRUE if button could be highlighted, FALSE otherwise.
*
******************************************************************************
*
*/

BOOL LIBENT
LT_PressButton(REG(a0) LayoutHandle *handle,REG(d0) LONG id)
{
	BOOL Result;

	Result = FALSE;

	if(handle)
	{
		struct Gadget *gadget;

		if(gadget = LTP_FindGadget(handle,id))
		{
			ObjectNode *node;

			if(GETOBJECT(gadget,node))
			{
				if(!node->Disabled)
					Result = LTP_BlinkButton(handle,gadget);
			}
		}
	}

	return(Result);
}
