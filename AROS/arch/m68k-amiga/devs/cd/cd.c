/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#define DEBUG 1

#include <aros/debug.h>
#include <proto/exec.h>

#include <devices/cd.h>

#include "cd_intern.h"

AROS_LH1(void, BeginIO,
    AROS_LHA(struct IORequest *, io, A1),
    LIBBASETYPEPTR, LIBBASE, 5, cd)
{
    AROS_LIBFUNC_INIT

    struct IOStdReq *iostd = (struct IOStdReq *)io;

    io->io_Message.mn_Node.ln_Type = NT_MESSAGE;

    D(bug("%s:\n"
          "io_Command: %d\n"
          "io_Length:  %d\n"
          "io_Data:    %p\n"
          "io_Offset:  %d\n",
          __func__, iostd->io_Command,
          iostd->io_Length, iostd->io_Data, iostd->io_Offset));

    io->io_Error = CDERR_NOCMD;

    switch (io->io_Command) {
    case CD_CONFIG:
        break;
    case CD_INFO:
        break;
    case CD_PLAYTRACK:
        break;
    case CD_TOCMSF:
        break;
    default:
        break;
    }

    if (!(io->io_Flags & IOF_QUICK))
        ReplyMsg(&io->io_Message);

    return;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, AbortIO,
    AROS_LHA(struct IORequest *, io, A1),
    LIBBASETYPEPTR, LIBBASE, 6, cd)
{
    AROS_LIBFUNC_INIT

    /* Cannot Abort IO */
    return 0;

    AROS_LIBFUNC_EXIT
}


