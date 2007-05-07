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


#ifndef __AROS__
#define AMITCP_FD_SETSIZE	256
#else
#define AMITCP_FD_SETSIZE	64
#endif

/* Define the fd macros used by AmiTCP. There are different to those actually used in LwIP
 * so they have to be converted */
#define AMITCP_NBBY	8		/* number of bits in a byte */
typedef long	amitcp_fd_mask;
#define AMITCP_NFDBITS	(sizeof(amitcp_fd_mask) * AMITCP_NBBY)	/* bits per mask */
#define amitcp_howmany(x, y)	(((x)+((y)-1))/(y))

typedef struct amitcp_fd_set {
	amitcp_fd_mask	amitcp_fds_bits[amitcp_howmany(AMITCP_FD_SETSIZE, AMITCP_NFDBITS)];
} amitcp_fd_set;

#define AMITCP_FD_SET(n, p)	((p)->amitcp_fds_bits[(n)/AMITCP_NFDBITS] |= (1 << ((n) % AMITCP_NFDBITS)))
#define AMITCP_FD_CLR(n, p)	((p)->amitcp_fds_bits[(n)/AMITCP_NFDBITS] &= ~(1 << ((n) % AMITCP_NFDBITS)))
#define AMITCP_FD_ISSET(n, p)	((p)->amitcp_fds_bits[(n)/AMITCP_NFDBITS] & (1 << ((n) % AMITCP_NFDBITS)))
#define AMITCP_FD_COPY(f, t)	bcopy(f, t, sizeof(*(f)))
#define AMITCP_FD_ZERO(p)	memset((void*)(p),0,sizeof(*(p)))

static void amitcp2lwip(int n, amitcp_fd_set *amitcp, fd_set *lwip)
{
    int i;

    FD_ZERO(lwip);
    if (!amitcp) return;

    for (i=0;i<n;i++)
    {
	if (AMITCP_FD_ISSET(i,amitcp))
	    FD_SET(i,lwip);
    }
}

static void lwip2amitcp(int n, fd_set *lwip, amitcp_fd_set *amitcp)
{
    int i;

    if (!amitcp) return;
    AMITCP_FD_ZERO(amitcp);

    for (i=0;i<n;i++)
    {
	if (FD_ISSET(i,lwip))
	    AMITCP_FD_SET(i,amitcp);
    }
}

/*****************************************************************************

    NAME */
#ifndef __AROS__
__asm int LIB_WaitSelect(register __d0 long nfds, register __a0 fd_set *readfds, register __a1 fd_set *writefds, register __a2 fd_set *exceptfds,
       register __a3 struct timeval *timeout, register __d1 ULONG *signals)
#else
	AROS_LH6(int, LIB_WaitSelect,

/*  SYNOPSIS */
	AROS_LHA(long, nfds, D0),
	AROS_LHA(fd_set *, readfds, A0),
        AROS_LHA(fd_set *, writefds, A1),
        AROS_LHA(fd_set *, exceptfds, A2),
        AROS_LHA(struct timeval *, timeout, A3),
        AROS_LHA(ULONG *,signals,D1),

/*  LOCATION */
	struct Library *, SocketBase, 126, Socket)
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

    amitcp_fd_set *amitcp_readfds = (amitcp_fd_set*)readfds;
    amitcp_fd_set *amitcp_writefds = (amitcp_fd_set*)writefds;
    amitcp_fd_set *amitcp_exceptfds = (amitcp_fd_set*)exceptfds;

    fd_set lwip_readfds, lwip_writefds, lwip_exceptfds;

#warning implement signals parameter

    AROS_LIBFUNC_INIT

    amitcp2lwip(nfds,amitcp_readfds,&lwip_readfds);
    amitcp2lwip(nfds,amitcp_writefds,&lwip_writefds);
    amitcp2lwip(nfds,amitcp_exceptfds,&lwip_exceptfds);

    t = FindTask(NULL);
    ud = t->tc_UserData;
    /* set user data (used by sys_arch functions) */
    t->tc_UserData = SOCKB(SocketBase)->data;

    rc = lwip_select(nfds, &lwip_readfds, &lwip_writefds, &lwip_exceptfds, timeout);

    /* restore old user data */
    t->tc_UserData = ud;

    lwip2amitcp(nfds,&lwip_readfds,amitcp_readfds);
    lwip2amitcp(nfds,&lwip_writefds,amitcp_writefds);
    lwip2amitcp(nfds,&lwip_exceptfds,amitcp_exceptfds);

    return rc;

    AROS_LIBFUNC_EXIT

}
