/*
 * @(#) $Header$
 *
 * BGUI library debugging utility
 * Tap.c
 * Monitor broadcasts, and print to a dos window.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2001/06/18 18:23:44  hkiel
 * Added missing newline at end of file.
 *
 * Revision 42.0  2000/05/09 22:08:09  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:21:30  mlemos
 * Bumped to revision 41.11
 *
 * Revision 41.1  2000/05/09 19:53:40  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/09/20 17:31:19  mlemos
 * Ian sources
 *
 *
 */

#include <proto/exec.h>
#include <proto/dos.h>
 
void main(void)
{
	struct MsgPort *tap;
	BOOL go = TRUE;
	ULONG sig, tsig;
	struct Message *msg;
	
	if (tap = CreateMsgPort())
	{
		((struct Node *)tap)->ln_Name = "Tap";
		((struct Node *)tap)->ln_Pri  = 120;
		
		AddPort(tap);
		tsig = 1 << tap->mp_SigBit;
		
		while (go)
		{
			sig = Wait(SIGBREAKF_CTRL_C|tsig);
			if (sig & tsig)
			{
				while (msg = GetMsg(tap))
				{
					PutStr((UBYTE *)msg + sizeof(struct Message));
					FreeVec(msg);
				};
			}
			if (sig & SIGBREAKF_CTRL_C)
			{
				go = FALSE;
			};
		};
		PutStr("--------\nTap off.\n");
		RemPort(tap);
		DeleteMsgPort(tap);
	};
}
