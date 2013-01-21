/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#define DEBUG 1
#include <aros/debug.h>

#include <aros/symbolsets.h>

#include "chinon.h"
#include "cd32.h"

#include "cd_intern.h"

static inline UWORD readw(ULONG addr)
{
    return *((volatile UWORD *)addr);
}

VOID CD32_BeginIO(struct IORequest *io, APTR priv)
{
    return;
}

LONG CD32_AbortIO(struct IORequest *io, APTR priv)
{
    return 0;
}

const struct cdUnitOps CD32Ops = {
    .uo_Name = "CD32 (Akiko)",
    .uo_OpenDevice = NULL,
    .uo_CloseDevice = NULL,
    .uo_BeginIO = CD32_BeginIO,
    .uo_AbortIO = CD32_AbortIO,
};

static int CD32_InitLib(LIBBASETYPE *cb)
{
    LONG unit;

    if (readw(AKIKO_ID) != AKIKO_ID_MAGIC) {
        D(bug("%s: No Akiko detected\n", __func__));
        return 1;
    }

    unit = cdAddUnit(cb, &CD32Ops, NULL);

    D(bug("%s: Akiko as CD Unit %d\n", __func__, unit));

    return (unit >= 0) ? 1 : 0;
}

static int CD32_ExpungeLib(LIBBASETYPE *cb)
{
    /* Nothing to do. */
    return 0;
}


ADD2INITLIB(CD32_InitLib, 32);
ADD2EXPUNGELIB(CD32_ExpungeLib, 32);
