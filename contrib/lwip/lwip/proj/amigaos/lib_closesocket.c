/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_closesocket.c,v 1.2 2002/07/11 22:52:13 sebauer Exp $
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
	  int rc;
	  void *ud;
	  struct Task *t;

    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    t = FindTask(NULL);
    ud = t->tc_UserData;
    /* set user data (used by sys_arch functions) */
    t->tc_UserData = SOCKB(SocketBase)->data;

    rc = lwip_close(s);

    D(bug("closesocket(%ld)=%ld\n",s,rc));
    /* restore old user data */
    t->tc_UserData = ud;
    return rc;

    AROS_LIBFUNC_EXIT

}
