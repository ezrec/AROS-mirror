/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Init of xprzmodem.library
    Lang: english
*/
#include <utility/utility.h>
#include <aros/symbolsets.h>

#include LC_LIBDEFS_FILE

extern int  __UserLibInit (LIBBASETYPE *);
extern void __UserLibCleanup (LIBBASETYPE *);

AROS_SET_LIBFUNC(Init, LIBBASETYPE, lh)
{
	if (RETURN_OK == __UserLibInit(lh)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

AROS_SET_LIBFUNC(Expunge, LIBBASETYPE, lh)
{
	__UserLibCleanup(lh);
}

ADD2INITLIB(Init, 0);
ADD2EXPUNGELIB(Expunge, 0);
