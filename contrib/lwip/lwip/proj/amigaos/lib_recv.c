/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_recv.c,v 1.2 2002/07/12 08:38:07 sebauer Exp $
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
__asm int LIB_recv(register __d0 long s, register __a0 unsigned char *buf, register __d1 long len, register __d2 long flags)
#else
	AROS_LH4(int, LIB_recv,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(unsigned char *, buf, A0),
	AROS_LHA(long, len, D1),
	AROS_LHA(long, flags, D2),

/*  LOCATION */
	struct Library *, SocketBase, 78, Socket)
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

    rc = lwip_recv(s,buf,len,flags);

    D(bug("recv()=%ld\n",rc));
    /* restore old user data */
    t->tc_UserData = ud;
    return rc;

    AROS_LIBFUNC_EXIT

}
