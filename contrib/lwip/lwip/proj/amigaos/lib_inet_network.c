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
__asm int LIB_inet_network(register __a0 char *cp)
#else
	AROS_LH1(int, LIB_inet_network,

/*  SYNOPSIS */
  AROS_LHA(char *, cp, A0),

/*  LOCATION */
	struct Library *, SocketBase, 204, Socket)
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

    D(bug("inet_network()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
