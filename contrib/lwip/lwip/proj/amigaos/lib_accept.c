/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_accept.c,v 1.1 2002/07/11 17:59:24 sebauer Exp $
*/

#include <exec/types.h>

#include "socket_intern.h"
#include "calling.h"

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm int LIB_accept(register __d0 long s, register __a0 struct sockaddr *addr, register __a1 long *addrlen)
#else
	AROS_LH3(int, LIB_accept,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(struct sockaddr *, addr, A0),
	AROS_LHA(long *, addrlen, A1),

/*  LOCATION */
	struct Library *, SocketBase, 48, Socket)
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

    return CallStackFunction(SOCKB(SocketBase), LIBMSG_ACCEPT, 3, s, addr, addrlen);

    AROS_LIBFUNC_EXIT

}
