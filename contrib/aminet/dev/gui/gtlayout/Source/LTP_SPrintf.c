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

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

VOID
SPrintf(STRPTR buffer, STRPTR formatString,...)
{
	va_list varArgs;

	va_start(varArgs,formatString);
	RawDoFmt(formatString,varArgs,(VOID (*)())"\x16\xC0\x4E\x75",buffer);
	va_end(varArgs);
}
