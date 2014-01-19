/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include "bsdsocket_intern.h"

/* Set errno number pointer */
static inline void errno_byte(APTR addr, ULONG errno_val)
{
    *(UBYTE *)addr = (UBYTE)errno_val;
}

static inline void errno_word(APTR addr, ULONG errno_val)
{
    *(UWORD *)addr = (UWORD)errno_val;
}

static inline void errno_long(APTR addr, ULONG errno_val)
{
    *(ULONG *)addr = (ULONG)errno_val;
}

static inline void errno_quad(APTR addr, ULONG errno_val)
{
    *(UQUAD *)addr = (UQUAD)errno_val;
}

/*****************************************************************************

    NAME */

        AROS_LH2(void, SetErrnoPtr,

/*  SYNOPSIS */
        AROS_LHA(void *, ptr,  A0),
        AROS_LHA(int,    size, D0),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 28, BSDSocket)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    void (*func)(APTR addr, ULONG errno_val);

    switch (size) {
    case 1: func = errno_byte; break;
    case 2: func = errno_word; break;
    case 4: func = errno_long; break;
    case 8: func = errno_quad; break;
    default:
        return;
    }

    SocketBase->bsd_errno.ptr = ptr;
    SocketBase->bsd_errno.update = func;

    AROS_LIBFUNC_EXIT
} /* SetErrnoPtr */
