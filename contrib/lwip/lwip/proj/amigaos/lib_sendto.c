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
__asm int LIB_sendto(register __d0 long s, register __a0 unsigned char *buf, register __d1 long len, register __d2 long flags, register __a1 struct sockaddr *to, register __d3 long tolen)
#else
	AROS_LH7(int, LIB_sendto,

/*  SYNOPSIS */
	AROS_LHA(LONG, domain, D0),
	AROS_LHA(LONG, type, D1),
	AROS_LHA(LONG, protocol, D2),

/*  LOCATION */
	struct Library *, SocketBase, 60, Socket)
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
