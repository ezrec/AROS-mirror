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

#include <intuition/sghooks.h>	/* For struct SGWork */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_HEXHOOK	/* Support code */

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_HexEditRoutine(REG(a0) struct Hook *hook,REG(a2) struct SGWork *sgw,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LTP_HexEditRoutine,
	  AROS_UFHA(struct Hook *, hook, A0),
	  AROS_UFHA(struct SGWork *, sgw, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
        AROS_USERFUNC_INIT
	
	LayoutHandle *handle;
	ObjectNode *node;
	BOOL activate;

	switch(msg->MethodID)
	{
		case SGH_KEY:

			handle = hook->h_Data;

			if(sgw->IEvent->ie_Code == 0x5F && handle->HelpHook)
			{
				sgw->Code		= 0x5F;
				sgw->Actions	= (sgw->Actions & ~SGA_BEEP) | SGA_END | SGA_USE;

				LTP_AddHistory(sgw);

				break;
			}

			if(sgw->IEvent->ie_Code == CURSORUP || sgw->IEvent->ie_Code == CURSORDOWN)
				LTP_HandleHistory(sgw);

			activate = TRUE;

			if(GETOBJECT(sgw->Gadget,node))
			{
				if(node->Type == INTEGER_KIND)
				{
					if(node->Special.Integer.LastGadget)
						activate = FALSE;

					if(!LTP_ConvertNum((node->Min < 0),sgw->WorkBuffer,(LONG *)&sgw->StringInfo->LongInt))
					{
						sgw->EditOp		= EO_BADFORMAT;
						sgw->Actions	= SGA_BEEP;
					}

					if(node->Special.Integer.ValidateHook)
					{
						if(!CallHookPkt(node->Special.Integer.ValidateHook,sgw,msg))
						{
							sgw->EditOp		= EO_BADFORMAT;
							sgw->Actions	= SGA_BEEP;
						}
					}
				}
			}

			if(sgw->EditOp == EO_ENTER && !(sgw->IEvent->ie_Qualifier & QUALIFIER_SHIFT))
			{
				if(activate && handle->AutoActivate)
					sgw->Actions |= SGA_NEXTACTIVE;

				if(!(sgw->Actions & SGA_NEXTACTIVE))
					sgw->Code = '\r';
			}

			if(sgw->Actions & SGA_END)
				LTP_AddHistory(sgw);

			/* Falls through to... */

		case SGH_CLICK:

			break;

		default:

			return(FALSE);
	}

	return(TRUE);
    
        AROS_USERFUNC_EXIT
}

#endif	/* DO_HEXHOOK */
