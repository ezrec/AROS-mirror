/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_send.c,v 1.1 2002/07/11 17:59:24 sebauer Exp $
*/

#include <exec/types.h>

#include "socket_intern.h"
#include "calling.h"

/*****************************************************************************

    NAME */
#ifndef __AROS


__asm int LIB_send(register __d0 long s, register __a0 unsigned char *buf, register __d1 long len, register __d2 flags)
#else
	AROS_LH4(int, LIB_send,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(unsigned char *, buf, A0),
	AROS_LHA(long, len, D1),
	AROS_LHA(long, flags, D2),

/*  LOCATION */
	struct Library *, SocketBase, 66, Socket)
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

    return CallStackFunction(SOCKB(SocketBase), LIBMSG_SEND, 4, s, buf, len, flags);

    AROS_LIBFUNC_EXIT

}
