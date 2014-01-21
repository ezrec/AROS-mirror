/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH3(void, SetSocketSignals,

/*  SYNOPSIS */
        AROS_LHA(ULONG, intrmask, D0),
        AROS_LHA(ULONG, iomask,   D1),
        AROS_LHA(ULONG, urgmask,  D2),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 22, BSDSocket)

/*  FUNCTION
      
        SetSocketSignals() tells bsdsocket.library which signal masks
        correspond to UNIX SIGINT, SIGIO and SIGURG signals to be used
        in   this implementation.  The  sigintrmask  mask is used to
        determine which  Amiga  signals interrupt  blocking  library
        calls.

        The sigiomask  is sent  when  asynchronous  notification  of
        socket   events   is  done,  the sigurgmask    is  sent when
        out-of-band   data  arrives, respectively.   The signals are
        sent only  to  the owning task  of   particular socket.  The
        socket  has got no owner   by default; the  owner  is set by
        FIOSETOWN (SIOCSPGRP) ioctl call.
        
        Note that the supplied  values write over  old ones. If this
        function is used and CTRL-C is still wanted to interrupt the
        calls (the default behaviour),  the value BREAKF_CTRL_C must
        be explicitly given.

    INPUTS

    RESULT

    NOTES
        The function SetSocketSignals() is obsoleted by the function
        SocketBaseTags().

    EXAMPLE

    BUGS

    SEE ALSO
        IoctlSocket(), recv(), send(), select(), WaitSelect()

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    SocketBaseTags(SBTM_SETVAL(SBTC_SIGIOMASK), iomask,
                   SBTM_SETVAL(SBTC_SIGURGMASK), urgmask,
                   SBTM_SETVAL(SBTC_BREAKMASK), intrmask,
                   TAG_END);

    AROS_LIBFUNC_EXIT

} /* SetSocketSignals */
