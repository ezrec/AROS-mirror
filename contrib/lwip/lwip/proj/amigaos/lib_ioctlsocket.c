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
__asm int LIB_IoctlSocket(register __d0 long s, register __d1 unsigned long req, register __a0 char *argp)
#else
	AROS_LH3(int, LIB_IoctlSocket,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(unsigned long, req, D1),
	AROS_LHA(char *, argp, A0),

/*  LOCATION */
	struct Library *, SocketBase, 114, Socket)
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
