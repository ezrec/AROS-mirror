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

#define DEBUG 1
#include <aros/debug.h>

/*****************************************************************************

    NAME */
	AROS_LH1(APTR, OpenTapInterface,

/*  SYNOPSIS */
	AROS_LHA(ULONG, addr, D0),
/*  LOCATION */
	struct Library *, TapInterfaceBase, 5, TapInterface)

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
    char buf[100];

    AROS_LIBFUNC_INIT

    D(bug("tapinterface.library: OpenTapInterace(addr=%08lx)\n",addr));

    fd = open(DEVTAP, O_RDWR);
    if (fd == -1)
    {
#ifdef linux
	D(bug("tapinterface.library: tapif_init: try running \"modprobe tun\" or rebuilding your kernel with CONFIG_TUN; cannot open "DEVTAP "\n"));
#else
	D(bug("tapinterface.library: tapif_init: cannot open "DEVTAP" n"));
#endif
	return NULL;
    }

#ifdef linux
    {
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TAP|IFF_NO_PI;
	if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0)
	{
	    D(bug("tapinterface.library:  "DEVTAP" ioctl TUNSETIFF\n"));
	    close(fd);
	    return NULL;
	}
    }
#endif /* Linux */

    snprintf(buf, sizeof(buf), "ifconfig tap0 inet %ld.%ld.%ld.%ld",
		(addr>>24)&0xff, (addr>>16)&0xff,(addr>>8)&0xff,addr&0xff);

    D(bug("tapinterface.library: %s\n",buf));
    system(buf);
    D(bug("tapinterface.library: Interface configured\n"));

    return (APTR)(fd + 1);

    AROS_LIBFUNC_EXIT

} /* OpenTapInterface */
