/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#include "include.h"

/*------------------------------------------------------------------------*/

#ifdef __GNUC__
#ifdef __MORPHOS__
const int __initlibraries = 0; /* no auto-libinit */
#endif
const int __nocommandline = 1; /* no argc, argv */
#endif

/*------------------------------------------------------------------------*/

#if !defined(__AROS__)
struct ExecBase *SysBase = NULL;
struct DosLibrary *DOSBase = NULL;
#endif

/*------------------------------------------------------------------------*/

int main(void)
{
#if defined(__AROS__)
	return main2();
#else
	int rc = RETURN_FAIL;

	SysBase = *((struct ExecBase **)4l);
	if ((DOSBase = (struct DosLibrary *)OpenLibrary(DOSNAME, MINSYS))) {
		rc = main2();
		CloseLibrary((struct Library *)DOSBase);
	}
	return rc;
#endif
}

/*------------------------------------------------------------------------*/

