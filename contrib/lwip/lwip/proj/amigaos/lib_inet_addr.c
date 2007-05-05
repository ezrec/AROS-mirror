/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

/* We redefine the timeval here, because lwip defines an own timeval struct,
   should be changed in the future */
#define timeval timeval_amiga

#include <exec/types.h>
#include <proto/exec.h>

#include "socket_intern.h"

#undef timeval

#include "lwip/sys.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "inet_aton.h"


#define MYDEBUG
#include "debug.h"

/*****************************************************************************

    NAME */
#ifndef __AROS__
__asm unsigned long LIB_inet_addr(register __a0 char *cp)
#else
	AROS_LH1(unsigned long, LIB_inet_addr,

/*  SYNOPSIS */
  AROS_LHA(char *, cp, A0),

/*  LOCATION */
	struct Library *, SocketBase, 180, Socket)
#endif

/*  FUNCTION

	Ascii internet address interpretation routine.
	The value returned is in network order.

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
    struct in_addr val;

    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    D(bug("inet_addr()\n"));

    if (inet_aton(cp, &val))
	return val.s_addr;
    return ~0; /* IADDR_NONE */

    AROS_LIBFUNC_EXIT

}
