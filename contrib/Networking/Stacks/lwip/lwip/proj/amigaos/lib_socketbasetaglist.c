/*
    Copyright © 2002-2007, The AROS Development Team. 
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
#ifndef __AROS__

__asm int LIB_SocketBaseTagList(register __a0 struct TagItem *tags)
#else
	AROS_LH1(int, LIB_SocketBaseTagList,

/*  SYNOPSIS */
  AROS_LHA(struct TagItem *, tags, A0),

/*  LOCATION */
	struct Library *, SocketBase, 294, Socket)
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

    D(bug("SocketBaseTagList()\n"));

    return 1; /* which tag caused the error? actually it's always the first */

    AROS_LIBFUNC_EXIT
}
