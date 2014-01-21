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
 
        This functions allows caller to redirect error variable inside
        scope of  caller task.   Usually this is  used to make  task's
        global variable errno as error variable.

    INPUTS

         ptr     - pointer to error variable that is to be modified on
                   every error condition on this library function.
         size    - size of the error variable.

    RESULT

    NOTES

        Be sure that this new error variable exists until library base
        is finally closed or SetErrnoPtr() is called again for another
        variable.

    EXAMPLE

        #include <errno.h>
        #include <protos/bsdsocket.h>

        int main(int argc, char **argv)
        {
            ...
            SetErrnoPtr(&errno, sizeof(errno));
            ...
        }

    BUGS

    SEE ALSO

        Errno()

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
