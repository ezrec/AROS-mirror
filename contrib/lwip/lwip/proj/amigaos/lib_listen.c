/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>

#include "socket_intern.h"

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm int LIB_listen(register __d0 long s, register __d1 long backlog)
#else
	AROS_LH2(int, LIB_listen,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(long, backlog, D1),

/*  LOCATION */
	struct Library *, SocketBase, 42, Socket)
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

    return -1;

    AROS_LIBFUNC_EXIT

}
