/*
    Copyright © 2002-2007, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>
#include <proto/exec.h>

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/*****************************************************************************

    NAME */
#ifndef __AROS__
__asm int LIB_SetErrnoPtr(void)
#else
	AROS_LH2(int, LIB_SetErrnoPtr,

/*  SYNOPSIS */
  AROS_LHA(void *, errnop, A0),
  AROS_LHA(long, size, D0),

/*  LOCATION */
	struct Library *, SocketBase, 168, Socket)
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

    D(bug("SetErrnoPtr()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
