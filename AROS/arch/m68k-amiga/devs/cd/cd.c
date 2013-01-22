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
    struct Task        *cu_Task;
    struct MsgPort     *cu_MsgPort;
};

/* We have a synchonous task for dispatching IO
 * to each cd.device unit.
 */
static VOID cdTask(IPTR base, IPTR unit)
{
    struct cdUnit *cu = (APTR)unit;
    struct IOStdReq *io;

    do {
        WaitPort(cu->cu_MsgPort);
        io = (struct IOStdReq *)GetMsg(cu->cu_MsgPort);

        if (io->io_Flags & IOF_ABORT) {
            io->io_Error = CDERR_ABORTED;
        } else if (io->io_Unit == (struct Unit *)cu &&
                   cu->cu_UnitOps->uo_DoIO != NULL) {
            io->io_Error = cu->cu_UnitOps->uo_DoIO(io, cu->cu_Private);
        } else {
            io->io_Error = CDERR_NOCMD;
        }

        ReplyMsg(&io->io_Message);

    } while (io->io_Unit != NULL);

    /* Terminate by fallthough */
}

LONG cdAddUnit(struct cdBase *cb, const struct cdUnitOps *ops, APTR priv)
{
    struct cdUnit *cu;

    cu = AllocVec(sizeof(*cu), MEMF_CLEAR | MEMF_ANY);
    if (cu) {
        cu->cu_Private = priv;
        cu->cu_UnitOps = ops;
        cu->cu_Task = NewCreateTask(TASKTAG_PC, cdTask,
                                    TASKTAG_NAME, ops->uo_Name,
                                    TASKTAG_ARG1, cb,
                                    TASKTAG_ARG2, cu,
                                    TASKTAG_TASKMSGPORT, &cu->cu_MsgPort,
                                    TAG_END);
        if (cu->cu_Task) {
            cu->cu_Unit = cb->cb_MaxUnit++;
            ADDTAIL(&cb->cb_Units, &cu->cu_Node);
            return cu->cu_Unit;
        }
        FreeVec(cu);
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
    struct IORequest io;
    struct MsgPort *mp = CreateMsgPort();

    while ((cu = (APTR)REMOVE(&cb->cb_Units)) != NULL) {
        D(bug("%s: Remove Unit %d\n", __func__, cu->cu_Unit));

        /* Shut down the unit's task */
        io.io_Device = (struct Device *)cb;
        io.io_Unit   = NULL;
        io.io_Command = CMD_INVALID;
        io.io_Message.mn_ReplyPort = mp;
        io.io_Message.mn_Length = sizeof(io);
        PutMsg(cu->cu_MsgPort, &io.io_Message);
        WaitPort(mp);
        GetMsg(mp);

        if (cu->cu_UnitOps->uo_Expunge)
            cu->cu_UnitOps->uo_Expunge(cu->cu_Private);
        FreeVec(cu);
    }

    DeleteMsgPort(mp);

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

    io->io_Flags &= ~IOF_QUICK;
    PutMsg(&cu->cu_MsgPort, &io->io_Message);

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
    Forbid();
    io->io_Flags |= IOF_ABORT;
    Permit();

    return TRUE;

    AROS_LIBFUNC_EXIT
}
