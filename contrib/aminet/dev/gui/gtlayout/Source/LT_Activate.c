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

/****** gtlayout.library/LT_Activate ******************************************
*
*   NAME
*	LT_Activate -- Activate a string type gadget.
*
*   SYNOPSIS
*	LT_Activate(Handle,ID);
*	              A0   D0
*
*	VOID LT_Activate(LayoutHandle *,LONG);
*
*   FUNCTION
*	The equivalent to intuition.library/ActivateGadget().
*
*   INPUTS
*	Handle - Pointer to LayoutHandle structure.
*
*	ID - ID of Gadget to activate.
*
*   RESULT
*	none
*
*   SEE ALSO
*	intuition.library/ActivateGadget
*
******************************************************************************
*
*/

VOID LIBENT
LT_Activate(REG(a0) LayoutHandle *handle,REG(d0) LONG id)
{
	struct Gadget *gadget;

	if(gadget = LTP_FindGadget(handle,id))
	{
		ObjectNode *node;

		if(GETOBJECT(gadget,node))
		{
			if(NOT node->Disabled)
			{
				register BOOL CanActivate;

				#ifdef DO_BOOPSI_KIND
				{
					CanActivate = (BOOL)(	LIKE_STRING_KIND(node) ||
											(node->Type == INTEGER_KIND) ||
											(node->Type == BOOPSI_KIND && node->Special.BOOPSI.ActivateHook)
										);
				}
				#else
				{
					CanActivate = (BOOL)(	LIKE_STRING_KIND(node) ||
											(node->Type == INTEGER_KIND)
										);
				}
				#endif /* DO_BOOPSI_KIND */

				if(CanActivate)
				{
					handle->Previous = gadget;

					#ifdef DO_BOOPSI_KIND
					{
						if(node->Type == BOOPSI_KIND)
						{
							if(node->Special.BOOPSI.ActivateHook)
								CallHookPkt(node->Special.BOOPSI.ActivateHook,(APTR)handle,node->Host);
						}
						else
						{
							ActivateGadget(handle->Previous,handle->Window,NULL);
						}
					}
					#else
					{
						ActivateGadget(handle->Previous,handle->Window,NULL);
					}
					#endif /* DO_BOOPSI_KIND */
				}
			}
		}
	}
}
