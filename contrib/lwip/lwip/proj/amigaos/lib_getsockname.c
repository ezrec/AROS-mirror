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
__asm int LIB_getsockname(register __d0 long s, register __a0 struct sockaddr *name, register __a1 long *namelen)
#else
	AROS_LH3(int, LIB_getsockname,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(struct sockaddr *, name, A0),
	AROS_LHA(long *, namelen, A1),

/*  LOCATION */
	struct Library *, SocketBase, 102, Socket)
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
