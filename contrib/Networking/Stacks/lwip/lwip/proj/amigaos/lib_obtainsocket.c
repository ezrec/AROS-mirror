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
__asm int LIB_ObtainSocket(register __d0 long id, register __d1 long domain, register __d2 long type, register __d3 long protocol )
#else
	AROS_LH4(int, LIB_ObtainSocket,

/*  SYNOPSIS */
	AROS_LHA(long, id, D0),
	AROS_LHA(long, domain, D1),
  AROS_LHA(long, type, D2),
  AROS_LHA(long, protocol, D3),

/*  LOCATION */
	struct Library *, SocketBase, 144, Socket)
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

    D(bug("ObtainSocket()\n"));

    return -1;

    AROS_LIBFUNC_EXIT

}
