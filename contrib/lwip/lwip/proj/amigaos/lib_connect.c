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
__asm int LIB_connect(register __d0 long s, register __a0 struct sockaddr *name, register __d1 long namelen)
#else
	AROS_LH3(int, LIB_connect,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(struct sockaddr, name, A0),
	AROS_LHA(long, namelen, D1),

/*  LOCATION */
	struct Library *, SocketBase, 54, Socket)
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
