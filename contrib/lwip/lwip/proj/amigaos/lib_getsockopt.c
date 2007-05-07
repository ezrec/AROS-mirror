/*
    Copyright © 2002-2007, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>

#include "socket_intern.h"

/*****************************************************************************

    NAME */



#ifndef __AROS__
__asm int LIB_getsockopt(register __d0 long s, register __d1 long level, register __d2 long optname, register __a0 void *optval, register __a1 long *optlen)
#else
	AROS_LH5(int, LIB_getsockopt,

/*  SYNOPSIS */
	AROS_LHA(long, s, D0),
	AROS_LHA(long, level, D1),
	AROS_LHA(long, optname, D2),
  AROS_LHA(void *, optval, A0),
  AROS_LHA(long *, optlen, A1),

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
    AROS_LIBFUNC_INIT

    return -1;

    AROS_LIBFUNC_EXIT

}
