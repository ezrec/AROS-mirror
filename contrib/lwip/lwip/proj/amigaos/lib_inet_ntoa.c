/*
    Copyright © 2002, The AROS Development Team. 
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
#ifndef __AROS
__asm int LIB_Inet_NtoA(register __d0 unsigned long ip)
#else
	AROS_LH1(int, LIB_Inet_NtoA,

/*  SYNOPSIS */
  AROS_LHA(unsigned long, ip, D0),

/*  LOCATION */
	struct Library *, SocketBase, 174, Socket)
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
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    D(bug("Inet_NtoA()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
