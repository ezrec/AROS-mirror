/*
    Copyright © 2002-2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>

#define DEBUG 1
#include <aros/debug.h>

/*****************************************************************************

    NAME */
	AROS_LH3(LONG, ReadTapInterface,

/*  SYNOPSIS */
	AROS_LHA(APTR, interface, A0),
	AROS_LHA(void *, buf, A1),
	AROS_LHA(LONG, length, A2),
/*  LOCATION */
	struct Library *, TapInterfaceBase, 7, TapInterface)

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
    int fd;

    AROS_LIBFUNC_INIT

    if (!interface) return -1;
    fd = ((int)interface)-1;

    D(bug("tapinterface.library: ReadTapInterface()\n"));

    return read(fd,buf,length);

    AROS_LIBFUNC_EXIT

} /* WriteTapInterface */
