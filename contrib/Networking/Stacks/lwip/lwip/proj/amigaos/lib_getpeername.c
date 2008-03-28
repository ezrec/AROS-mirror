/*
    Copyright © 2002-2007, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>

#include "socket_intern.h"

/*****************************************************************************

    NAME */
#ifndef __AROS__
__asm int LIB_getpeername(register __d0 long s, register __a0 struct sockaddr *h, register __a1 long *n)
#else
	AROS_LH3(int, LIB_getpeername,

/*  SYNOPSIS */
	AROS_LHA(LONG, s, D0),
	AROS_LHA(struct sockaddr *, h, A0),
	AROS_LHA(long, n, A1),

/*  LOCATION */
	struct Library *, SocketBase, 108, Socket)
#endif

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS
	The function itself is a bug ;-) Remove it!

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return -1;

    AROS_LIBFUNC_EXIT

}
