/*
 * Copyright (C) 2014, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <aros/debug.h>
#include <aros/asmcall.h>

#include <proto/exec.h>

#include "bsdsocket_intern.h"

extern APTR BSDSocket_FuncTable[];

AROS_LH1(struct Library *, BSDSocket_OpenLib,
        AROS_LHA(ULONG, version, D0),
        struct Library *, BSDSocketBase, 1, BSDSocket)
{
    AROS_LIBFUNC_INIT

    struct bsdsocketBase *bsdsocketBase;

    bsdsocketBase = (struct bsdsocketBase *)MakeLibrary(BSDSocket_FuncTable, NULL, NULL, sizeof(struct bsdsocketBase), NULL);

    bsdsocketBase->lib.lib_Node.ln_Name = BSDSocketBase->lib_Node.ln_Name;
    bsdsocketBase->lib.lib_Node.ln_Type = NT_LIBRARY;
    bsdsocketBase->lib.lib_Node.ln_Pri = BSDSocketBase->lib_Node.ln_Pri;
    bsdsocketBase->lib.lib_Flags = LIBF_CHANGED;
    bsdsocketBase->lib.lib_Version = BSDSocketBase->lib_Version;
    bsdsocketBase->lib.lib_Revision = BSDSocketBase->lib_Revision;
    bsdsocketBase->lib.lib_IdString = BSDSocketBase->lib_IdString;

    SumLibrary(&bsdsocketBase->lib);

    bsdsocketBase->errnoPtr = &bsdsocketBase->errnoVal;
    bsdsocketBase->errnoSize = sizeof(bsdsocketBase->errnoVal);
    bsdsocketBase->sigintr = SIGBREAKF_CTRL_C;

    /* FIXME: Make adjustable descriptor table size */
    bsdsocketBase->lib.lib_OpenCnt++;
    bsdsocketBase->lib_Master = BSDSocketBase;
    BSDSocketBase->lib_OpenCnt++;

    return &bsdsocketBase->lib;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, BSDSocket_CloseLib,
         struct bsdsocketBase *, bsdsocketBase, 2, BSDSocket)
{
    AROS_LIBFUNC_INIT
    struct Library *BSDSocketBase = bsdsocketBase->lib_Master;

    bsdsocketBase->lib.lib_OpenCnt--;
    BSDSocketBase->lib_OpenCnt--;

    if (!bsdsocketBase->lib.lib_OpenCnt) {
        APTR addr;

        addr = (APTR)((IPTR)bsdsocketBase - bsdsocketBase->lib.lib_NegSize);
        FreeMem(addr, bsdsocketBase->lib.lib_NegSize + bsdsocketBase->lib.lib_PosSize);
    }

    if (BSDSocketBase->lib_OpenCnt) {
        return BNULL;
    }

    if (BSDSocketBase->lib_Flags & LIBF_DELEXP) {
        return AROS_LC1(BPTR, BSDSocket_ExpungeLib,
                AROS_LCA(struct Library *, BSDSocketBase, D0),
                struct BSDSocketBase *, BSDSocketBase, 3, BSDSocket);
    }

    return BNULL;

    AROS_LIBFUNC_EXIT
}
