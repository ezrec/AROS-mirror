/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_accept.c,v 1.2 2002/07/12 08:38:07 sebauer Exp $
*/

#include <exec/types.h>
#include <proto/exec.h>

#include "socket_intern.h"

#include "lwip/sys.h"
#include "lwip/sockets.h"

/* #define MYDEBUG */
#include "debug.h"

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
	  int rc;
	  void *ud;
	  struct Task *t;

    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    t = FindTask(NULL);
    ud = t->tc_UserData;
    /* set user data (used by sys_arch functions) */
    t->tc_UserData = SOCKB(SocketBase)->data;

    rc = lwip_accept(s,addr,addrlen);

    D(bug("accept()=%ld\n",rc));
    /* restore old user data */
    t->tc_UserData = ud;
    return rc;

    AROS_LIBFUNC_EXIT

}
