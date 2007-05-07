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
__asm int LIB_Inet_MakeAddr(register __d0 unsigned long net, register __d1 unsigned long host)
#else
	AROS_LH2(int, LIB_Inet_MakeAddr,

/*  SYNOPSIS */
  AROS_LHA(unsigned long, net, D0),
  AROS_LHA(unsigned long, host, D1),

/*  LOCATION */
	struct Library *, SocketBase, 198, Socket)
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

    D(bug("Inet_MakeAddr()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
