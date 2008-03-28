/*
    Copyright © 2002-2007, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>

#include "socket_intern.h"
#include "calling.h"

/*****************************************************************************

    NAME */
#ifndef __AROS__
__asm int LIB_setsockopt(register __d0 long s, register __d1 long level, register __d2 long optname, register __a0 void *optval, register __a1 long *optlen)
#else
	AROS_LH3(int, LIB_setsockopt,

/*  SYNOPSIS */
	AROS_LHA(LONG, domain, D0),
	AROS_LHA(LONG, type, D1),
	AROS_LHA(LONG, protocol, D2),

/*  LOCATION */
	struct Library *, SocketBase, 90, Socket)
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
