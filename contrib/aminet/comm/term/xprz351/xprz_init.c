/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Init of xprzmodem.library
    Lang: english
*/
#include <utility/utility.h>

#include "libdefs.h"

extern struct ExecBase * SysBase;

#define LC_NO_OPENLIB
#define LC_NO_CLOSELIB
#define LC_RESIDENTPRI	    -120

#include <libcore/libheader.c>


extern int  __UserLibInit (LC_LIBHEADERTYPEPTR *);
extern void __UserLibCleanup (LC_LIBHEADERTYPEPTR *);


ULONG SAVEDS L_InitLib (LC_LIBHEADERTYPEPTR lh)
{
	return __UserLibInit(lh);
}

void SAVEDS L_ExpungeLib (LC_LIBHEADERTYPEPTR lh)
{
	__UserLibCleanup(lh);
}
