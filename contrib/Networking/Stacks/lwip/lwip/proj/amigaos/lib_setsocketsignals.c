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

__asm void LIB_SetSocketSignals(register __d0 unsigned long intmask, register __d1 unsigned long iomask, register __d2 unsigned long urgentmask )
#else
	AROS_LH3(void, LIB_SetSocketSignals,

/*  SYNOPSIS */
  AROS_LHA(unsigned long, intmask, D0),
  AROS_LHA(unsigned long, iomask, D1),
  AROS_LHA(unsigned long, urgentmask, D2),

/*  LOCATION */
	struct Library *, SocketBase, 132, Socket)
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

    D(bug("SetSocketSignals()\n"));

    AROS_LIBFUNC_EXIT
}
