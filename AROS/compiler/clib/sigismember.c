/*
    Copyright � 1995-2002, The AROS Development Team. All rights reserved.
    $Id$

    Desc: ANSI C function sigismember()
    Lang: English
*/

/*****************************************************************************

    NAME */

#include <signal.h>

	int sigismember (

/*  SYNOPSIS */
	const sigset_t *set,
	int signum)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

******************************************************************************/
{
	if (NULL != set) {
		ULONG i = (signum >> 5);
		if (0 != (set->__val[i] & (signum & 0x1f)))
			return 1;
		return 0;
	}
	
	return -1;
} /* sigismember */
