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

#include <clib/keymap_protos.h>
#include <pragmas/keymap_pragmas.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

/****** gtlayout.library/LT_GetCode ******************************************
*
*   NAME
*	LT_GetCode -- Easy raw key event to ANSI conversion.
*
*   SYNOPSIS
*	Key = LT_GetCode(Qualifier,Class,Code,Gadget);
*	 D0                  D0     D1    D2    A0
*
*	LONG LT_GetCode(ULONG,ULONG,UWORD,struct Gadget *);
*
*   FUNCTION
*	The user interface layout engine can convert IDCMP_RAWKEY
*	events into ANSI codes. Pass in the data you copied from
*	the IntuiMessage here.
*
*   INPUTS
*	Qualifier - Copied from IntuiMessage->Qualifier
*
*	Class - Copied from IntuiMessage->Class
*
*	Code - Copied from IntuiMessage->Code
*
*	Gadget - Copied from IntuiMessage->IAddress
*
*   RESULT
*	Key - ANSI code generated from the input data
*	    or -1 if no such code was to be generated.
*
******************************************************************************
*
*/

LONG LIBENT
LT_GetCode(REG(d0) ULONG MsgQualifier,REG(d1) ULONG MsgClass,REG(d2) UWORD MsgCode,REG(a0) struct Gadget *MsgGadget)
{
	if(MsgClass == IDCMP_RAWKEY && !(MsgCode & IECODE_UP_PREFIX) && KeymapBase)
	{
		UBYTE				Buffer[10];
		struct InputEvent	Event;
		LONG				Len;

		Event.ie_NextEvent			= NULL;
		Event.ie_Code 				= MsgCode & ~IECODE_UP_PREFIX;
		Event.ie_Qualifier			= MsgQualifier;
		Event.ie_Class				= IECLASS_RAWKEY;
		Event.ie_SubClass			= 0;
		Event.ie_position.ie_addr	= (APTR)MsgGadget;

		Buffer[0] = 0;

		if((Len = MapRawKey(&Event,Buffer,9,NULL)) > 0)
		{
			if(Buffer[0] != 0x9B || Len == 1)
				return((LONG)Buffer[0]);
		}
	}

	return(-1);
}
