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

#ifdef linux
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#define DEVTAP "/dev/net/tun"
#else  /* linux */
#define DEVTAP "/dev/tap0"
#endif /* linux */

#include <aros/asmcall.h>

/* #define DEBUG 1 */
#include <aros/debug.h>

/*****************************************************************************

    NAME */
	AROS_LH1(BOOL, WaitTapInterface,

/*  SYNOPSIS */
	AROS_LHA(APTR, interface, A0),
/*  LOCATION */
	struct Library *, TapInterfaceBase, 8, TapInterface)

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
    int fd,ret;
    fd_set fdset;

    AROS_LIBFUNC_INIT

    D(bug("tapinterface.library: WaitTapInterface(interface=%p)\n",interface));

    if (!interface) return -1;
    fd = ((int)interface)-1;

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    /* Wait for a packet to arrive. */
    ret = select(fd + 1, &fdset, NULL, NULL, NULL);
    return ret == 1;

    AROS_LIBFUNC_EXIT

} /* WaitTapInterface */
