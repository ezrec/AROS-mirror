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

ULONG
LTP_Atol(STRPTR String)
{
	ULONG Value = 0;

	while(*String == ' ' || *String == '\t')
		String++;

	while(*String)
	{
		if(*String >= '0' && *String <= '9')
			Value = (Value * 10) + ((*String++) - '0');
		else
			break;
	}

	return(Value);
}
