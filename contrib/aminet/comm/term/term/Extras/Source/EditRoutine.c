/*
**	EditRoutine.c
**
**	The common string gadget editing routine
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* CommonEditRoutine():
	 *
	 *	The common string gadget editing routine.
	 */

#ifndef __AROS__
ULONG SAVE_DS ASM
CommonEditRoutine(REG(a0) struct Hook *UnusedHook,REG(a2) struct SGWork *Work,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, CommonEditRoutine,
 AROS_UFHA(struct Hook *  , UnusedHook , A0),
 AROS_UFHA(struct SGWork *, Work, A2),
 AROS_UFHA(Msg            , msg, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	BOOL GotIt;

	switch(msg->MethodID)
	{
			/* Hit a hit. */

		case SGH_KEY:

				/* Right-Amiga-key was pressed, release the
				 * string gadget so user may select a menu
				 * item.
				 */

			if((Work->IEvent->ie_Qualifier & AMIGARIGHT) && (Work->EditOp == EO_INSERTCHAR || Work->EditOp == EO_REPLACECHAR))
			{
				Work->Actions	= SGA_REUSE | SGA_END;
				Work->Code		= (UWORD)-1;

				break;
			}

				/* End input? */

			if(Work->Actions & SGA_END)
			{
					/* Holding down any shift key causes the string gadget
					 * not be activated again.
					 */

				if(Work->IEvent->ie_Qualifier & SHIFT_KEY)
					Work->Code = 0xFF00 | '\n';
				else
					Work->Code = 0xFF00 | '\r';

				Work->Actions = SGA_END;

				break;
			}

				/* xOFF or xON? */

			if(Work->Code == CONTROL_('S') || Work->Code == CONTROL_('Q'))
			{
				Work->Actions	 = SGA_END;
				Work->Code		|= 0xFF00;

				break;
			}

				/* We haven't got anything useful yet. */

			GotIt = FALSE;

				/* This looks like a function key. Send the corresponding macro. */

			if(Work->IEvent->ie_Code >= F01_CODE && Work->IEvent->ie_Code <= F10_CODE)
				GotIt = TRUE;

				/* Amiga key + Del/Backspace clears the history list. */

			if((Work->IEvent->ie_Qualifier & (AMIGARIGHT | AMIGALEFT)) && (Work->IEvent->ie_Code == DEL_CODE || Work->IEvent->ie_Code == BACKSPACE_CODE))
				GotIt = TRUE;

				/* Browse through the history list? */

			if(Work->IEvent->ie_Code == CURSORUP || Work->IEvent->ie_Code == CURSORDOWN)
				GotIt = TRUE;

				/* Did we get anything sensible? */

			if(GotIt)
			{
					/* Merge code and qualifier. */

				Work->Actions	= SGA_END;
				Work->Code		= ((Work->IEvent->ie_Qualifier & (ALT_KEY|SHIFT_KEY|CONTROL_KEY)) << 8) | Work->IEvent->ie_Code;
			}

			/* Fall through to... */

		case SGH_CLICK:

			break;

		default:

			return(FALSE);
	}

	return(TRUE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
