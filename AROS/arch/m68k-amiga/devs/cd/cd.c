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

struct cdUnit {
    struct MinNode      cu_Node;
    LONG                cu_Unit;
    APTR                cu_Private;
    const struct cdUnitOps   *cu_UnitOps;
};

LONG cdAddUnit(struct cdBase *cb, const struct cdUnitOps *ops, APTR priv)
{
    struct cdUnit *cu;

    cu = AllocVec(sizeof(*cu), MEMF_CLEAR | MEMF_ANY);
    if (cu) {
        cu->cu_Unit = cb->cb_MaxUnit++;
        cu->cu_Private = priv;
        cu->cu_UnitOps = ops;
        ADDTAIL(&cb->cb_Units, &cu->cu_Node);
        return cu->cu_Unit;
    }

    return -1;
}

static int cd_Init(LIBBASETYPE *cb)
{
    NEWLIST(&cb->cb_Units);

    return 1;
}

static int cd_Expunge(LIBBASETYPE *cb)
{
    struct cdUnit *cu;

    while ((cu = (APTR)REMOVE(&cb->cb_Units)) != NULL) {
        D(bug("%s: Remove Unit %d\n", __func__, cu->cu_Unit));
        FreeVec(cu);
    }

    return 1;
}

ADD2INITLIB(cd_Init, 0);
ADD2EXPUNGELIB(cd_Expunge, 0);

AROS_LH1(void, BeginIO,
    AROS_LHA(struct IORequest *, io, A1),
    LIBBASETYPEPTR, LIBBASE, 5, cd)
{
    AROS_LIBFUNC_INIT

    struct IOStdReq *iostd = (struct IOStdReq *)io;
    struct cdUnit *cu = (struct cdUnit *)(io->io_Unit);

    D(bug("%s.%d: %p\n"
          "io_Command: %d\n"
          "io_Length:  %d\n"
          "io_Data:    %p\n"
          "io_Offset:  %d\n",
          __func__, io, cu->cu_Unit, iostd->io_Command,
          iostd->io_Length, iostd->io_Data, iostd->io_Offset));

    io->io_Error = CDERR_NOCMD;

    cu->cu_UnitOps->uo_BeginIO(io, cu->cu_Private);

    return;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, AbortIO,
    AROS_LHA(struct IORequest *, io, A1),
    LIBBASETYPEPTR, LIBBASE, 6, cd)
{
    AROS_LIBFUNC_INIT

    struct cdUnit *cu = (struct cdUnit *)(io->io_Unit);

    D(bug("%s.%d: %p\n", __func__, cu->cu_Unit, io));
    return cu->cu_UnitOps->uo_AbortIO(io, cu->cu_Private);

    AROS_LIBFUNC_EXIT
}
