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

/* temporary */
typedef int fd_set;

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm int LIB_WaitSelect(register __d0 long nfds, register __a0 fd_set *readfds, register __a1 fd_set *writefds, register __a2 fd_set *exceptfds,
       register __a3 struct timeval *timeout, register __d1 ULONG *signals)
#else
	AROS_LH6(APTR, LIB_waitselect,

/*  SYNOPSIS */
	AROS_LHA(long, nfds, D0),
	AROS_LHA(fd_set *, readfds, A0),
  AROS_LHA(fd_set *, writefds, A1),
  AROS_LHA(fd_set *, exceptfds, A2),
  AROS_LHA(struct timeval *, timeout, A3),
  AROS_LHA(ULONG *,signals,D1)

/*  LOCATION */
	struct Library *, SocketBase, 126, Socket)
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

    D(bug("WaitSelect()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
