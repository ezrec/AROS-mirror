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

#ifdef DO_PASSWORD_KIND	/* Support code */

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_PasswordEditRoutine(REG(a0) struct Hook *Hook,REG(a2) struct SGWork *Work,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LTP_PasswordEditRoutine,
	  AROS_UFHA(struct Hook *, Hook, A0),
	  AROS_UFHA(struct SGWork *, Work, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
        AROS_USERFUNC_INIT
	
	LayoutHandle *Handle;
	ObjectNode *Node;
	STRPTR Buffer;

	if(!GETOBJECT(Work->Gadget,Node))
		return(TRUE);

	switch(msg->MethodID)
	{
		case SGH_KEY:

			Buffer = Node->Special.String.RealString;
			Handle = Hook->h_Data;

			if(Work->IEvent->ie_Code == 0x5F && Handle->HelpHook)
			{
				Work->Code		= 0x5F;
				Work->Actions	= (Work->Actions & ~SGA_BEEP) | SGA_END | SGA_USE;

				break;
			}

			switch(Work->EditOp)
			{
				case EO_INSERTCHAR:

					Buffer[Work->BufferPos - 1]	= Work->Code;
					Buffer[Work->NumChars]		= 0;

					Work->WorkBuffer[Work->BufferPos - 1] = (UBYTE)'·';

					break;

				case EO_CLEAR:

					Work->WorkBuffer[0]	= 0;
					Work->NumChars		= 0;
					Work->BufferPos		= 0;
					Work->EditOp		= EO_BIGCHANGE;

					Buffer[0] = 0;

					break;

				case EO_DELBACKWARD:

					Buffer[Work->NumChars] = 0;
					break;

				case EO_NOOP:

					break;

				case EO_ENTER:

					if(!(Work->IEvent->ie_Qualifier & QUALIFIER_SHIFT))
					{
						if(!Node->Special.String.LastGadget && Node->Special.String.LayoutHandle->AutoActivate)
							Work->Actions |= SGA_NEXTACTIVE;

						if(!(Work->Actions & SGA_NEXTACTIVE))
							Work->Code = '\r';
					}

					break;

				case EO_MOVECURSOR:
				case EO_DELFORWARD:

					Work->Actions &= ~SGA_USE;
					break;

				default:

					Work->Actions &= ~SGA_USE;
					Work->Actions |=  SGA_BEEP;
					break;
			}

			break;

		case SGH_CLICK:

			Work->BufferPos	= Work->NumChars;
			Work->EditOp	= EO_BIGCHANGE;

			break;

		default:

			return(FALSE);
	}

	return(TRUE);
    
        AROS_USERFUNC_EXIT
}

#endif	/* DO_PASSWORD_KIND */
