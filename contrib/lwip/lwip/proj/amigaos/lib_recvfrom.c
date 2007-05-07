/*
    Copyright © 2002-2007, The AROS Development Team. 
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
#include "lwip/sockets.h"

/* #define MYDEBUG */
#include "debug.h"

/*****************************************************************************

    NAME */
#ifndef __AROS__
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
	  int rc;
	  void *ud;
	  struct Task *t;

    AROS_LIBFUNC_INIT

    t = FindTask(NULL);
    ud = t->tc_UserData;
    /* set user data (used by sys_arch functions) */
    t->tc_UserData = SOCKB(SocketBase)->data;

    rc = lwip_recvfrom(s,buf,len,flags,addr,(int*)addrlen);

    D(bug("recvfrom()=%ld\n",rc));
    /* restore old user data */
    t->tc_UserData = ud;
    return rc;

    AROS_LIBFUNC_EXIT

}
