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
__asm int LIB_Inet_NetOf(register __d0 unsigned long in)
#else
	AROS_LH1(int, LIB_Inet_NetOf,

/*  SYNOPSIS */
  AROS_LHA(unsigned long, in, D0),

/*  LOCATION */
	struct Library *, SocketBase, 192, Socket)
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

    D(bug("Inet_NetOf()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
