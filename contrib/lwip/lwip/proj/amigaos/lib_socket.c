/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_socket.c,v 1.1 2002/07/11 17:59:24 sebauer Exp $
*/

#include <exec/types.h>

#include "socket_intern.h"
#include "calling.h"

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm int LIB_socket(register __d0 long domain, register __d1 long type, register __d2 long protocol)
#else
	AROS_LH3(int, LIB_socket,

/*  SYNOPSIS */
	AROS_LHA(LONG, domain, D0),
	AROS_LHA(LONG, type, D1),
	AROS_LHA(LONG, protocol, D2),

/*  LOCATION */
	struct Library *, SocketBase, 30, Socket)
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

    return CallStackFunction(SOCKB(SocketBase), LIBMSG_SOCKET, 3, domain, type, protocol);

    AROS_LIBFUNC_EXIT

}
