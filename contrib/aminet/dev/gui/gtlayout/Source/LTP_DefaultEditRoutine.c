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

#ifndef __AROS__
ULONG SAVE_DS ASM
LTP_DefaultEditRoutine(REG(a0) struct Hook *Hook,REG(a2) struct SGWork *Work,REG(a1) Msg msg)
#else
AROS_UFH3(ULONG, LTP_DefaultEditRoutine,
	  AROS_UFHA(struct Hook *, Hook, A0),
	  AROS_UFHA(struct SGWork *, Work, A2),
	  AROS_UFHA(Msg, msg, A1)
)
#endif
{
	struct Hook *Validate;
	LayoutHandle *Handle;
	ObjectNode *Node;
	BOOL Reject;

	if(!GETOBJECT(Work->Gadget,Node))
		return(TRUE);

	switch(msg->MethodID)
	{
		case SGH_KEY:

			Handle = Hook->h_Data;

			if(Work->IEvent->ie_Code == 0x5F && Handle->HelpHook)
			{
				Work->Code		= 0x5F;
				Work->Actions	= (Work->Actions & ~SGA_BEEP) | SGA_END | SGA_USE;

				LTP_AddHistory(Work);
				break;
			}

			if(Work->IEvent->ie_Code == CURSORUP || Work->IEvent->ie_Code == CURSORDOWN)
				LTP_HandleHistory(Work);

			if(Work->EditOp == EO_ENTER)
			{
				BOOL Activate = TRUE;

				DB(kprintf("enter operation\n"));

				if(Node->Type == STRING_KIND)
				{
					if(Node->Special.String.LastGadget)
						Activate = FALSE;
				}
				else
				{
					#ifndef DO_HEXHOOK
					{
						if(Node->Type == INTEGER_KIND)
						{
							if(Node->Special.Integer.LastGadget)
								Activate = FALSE;
						}
					}
					#endif /* DO_HEXHOOK */
				}

				if(!(Work->IEvent->ie_Qualifier & QUALIFIER_SHIFT))
				{
					DB(kprintf("no shift\n"));

					if(Activate && Handle->AutoActivate)
						Work->Actions |= SGA_NEXTACTIVE;

					if(!(Work->Actions & SGA_NEXTACTIVE))
						Work->Code = '\r';
				}
				else
					DB(kprintf("has shift\n"));
			}

			Reject = FALSE;

			if(Node->Type == FRACTION_KIND && (Work->EditOp == EO_INSERTCHAR || Work->EditOp == EO_REPLACECHAR))
			{
				LONG DecimalPoint = LTP_DecimalPoint[0];

				if(Work->Code == DecimalPoint || Work->Code == '.' || (Work->Code >= '0' && Work->Code <= '9'))
				{
					if(Work->Code == DecimalPoint || Work->Code == '.')
					{
						LONG i;

						for(i = 0 ; i < Work->NumChars ; i++)
						{
							if(Work->PrevBuffer[i] == DecimalPoint)
							{
								Reject = TRUE;
								break;
							}
						}
					}
				}
				else
					Reject = TRUE;

				if(!Reject && Work->Code == '.' && Work->Code != DecimalPoint)
				{
					LONG i;

					for(i = 0 ; i < Work->NumChars ; i++)
					{
						if(Work->WorkBuffer[i] == '.')
						{
							Work->WorkBuffer[i] = DecimalPoint;
							break;
						}
					}
				}
			}

			if(Node->Type == INTEGER_KIND)
				Validate = Node->Special.Integer.ValidateHook;
			else
				Validate = Node->Special.String.ValidateHook;

			if(Validate)
			{
				if(!CallHookPkt(Validate,Work,msg))
					Reject = TRUE;
			}

			if(Reject)
			{
				Work->EditOp	= EO_BADFORMAT;
				Work->Actions	= SGA_BEEP;
			}

			if(Work->Actions & SGA_END)
				LTP_AddHistory(Work);

			/* Falls through to... */

		case SGH_CLICK:

			break;

		default:

			return(FALSE);
	}

	return(TRUE);
}
