/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_bind.c,v 1.2 2002/07/12 08:38:07 sebauer Exp $
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
__asm int LIB_bind(register __d0 long s, register __a0 struct sockaddr *name, register __d1 long namelen)
#else
	AROS_LH3(int, LIB_bind,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(struct sockaddr, name, A0),
	AROS_LHA(long, namelen, D1),

/*  LOCATION */
	struct Library *, SocketBase, 36, Socket)
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

    rc = lwip_bind(s,name,namelen);

    D(bug("bind()=%ld\n",rc));
    /* restore old user data */
    t->tc_UserData = ud;
    return rc;

    AROS_LIBFUNC_EXIT

}
