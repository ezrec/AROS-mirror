/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_closesocket.c,v 1.1 2002/07/11 17:59:24 sebauer Exp $
*/

#include <exec/types.h>

#include "socket_intern.h"
#include "calling.h"

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm int LIB_CloseSocket(register __d0 LONG s)
#else
	AROS_LH1(int, LIB_CloseSocket,

/*  SYNOPSIS */
	AROS_LHA(LONG, s, D0),

/*  LOCATION */
	struct Library *, SocketBase, 120, Socket)
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

    return CallStackFunction(SOCKB(SocketBase), LIBMSG_CLOSESOCKET, 1, s);

    AROS_LIBFUNC_EXIT

}
