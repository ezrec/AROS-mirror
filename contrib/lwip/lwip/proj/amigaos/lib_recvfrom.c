/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_recvfrom.c,v 1.1 2002/07/11 17:59:24 sebauer Exp $
*/

#include <exec/types.h>

#include "socket_intern.h"
#include "calling.h"

/*****************************************************************************

    NAME */

#ifndef __AROS
__asm int LIB_recvfrom(register __d0 long s, register __a0 unsigned char *buf, register __d1 long len, register __d2 long flags, register __a1 struct sockaddr *addr, register __a2 long *addrlen)
#else
	AROS_LH6(int, LIB_recvfrom,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(unsigned char *, buf, A0),
	AROS_LHA(long, len, D1),
	AROS_LHA(long, flags, D2),
	AROS_LHA(struct sockaddr *, addr, A1),
	AROS_LHA(long *, addrlen, A2),
/*  LOCATION */
	struct Library *, SocketBase, 72, Socket)
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

    return CallStackFunction(SOCKB(SocketBase), LIBMSG_SOCKET, 6, s, buf, len, flags, addr, addrlen);

    AROS_LIBFUNC_EXIT

}
