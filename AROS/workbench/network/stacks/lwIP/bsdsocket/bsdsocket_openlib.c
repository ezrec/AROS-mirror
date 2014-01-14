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
        struct BSDSocketBase *, BSDSocketBase, 1, BSDSocket)
{
    AROS_LIBFUNC_INIT
    struct bsd *bsd;

    struct bsdsocketBase *bsdsocketBase;

    bsd = bsd_init(BSDSocketBase->bsd_global);
    if (!bsd)
        return NULL;

    bsdsocketBase = (struct bsdsocketBase *)MakeLibrary(BSDSocket_FuncTable, NULL, NULL, sizeof(struct bsdsocketBase), NULL);
    if (!bsdsocketBase) {
        bsd_expunge(bsd);
        return NULL;
    }

    bsdsocketBase->lib.lib_Node.ln_Name = BSDSocketBase->lib.lib_Node.ln_Name;
    bsdsocketBase->lib.lib_Node.ln_Type = NT_LIBRARY;
    bsdsocketBase->lib.lib_Node.ln_Pri = BSDSocketBase->lib.lib_Node.ln_Pri;
    bsdsocketBase->lib.lib_Flags = LIBF_CHANGED;
    bsdsocketBase->lib.lib_Version = BSDSocketBase->lib.lib_Version;
    bsdsocketBase->lib.lib_Revision = BSDSocketBase->lib.lib_Revision;
    bsdsocketBase->lib.lib_IdString = BSDSocketBase->lib.lib_IdString;

    SumLibrary(&bsdsocketBase->lib);

    bsdsocketBase->sigintr = SIGBREAKF_CTRL_C;
    bsdsocketBase->bsd_syslog = -1;
    bsdsocketBase->lib_Master = BSDSocketBase;

    bsdsocketBase->lib.lib_OpenCnt++;
    BSDSocketBase->lib.lib_OpenCnt++;

    return &bsdsocketBase->lib;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, BSDSocket_CloseLib,
         struct bsdsocketBase *, bsdsocketBase, 2, BSDSocket)
{
    AROS_LIBFUNC_INIT
    struct Library *BSDSocketBase = &bsdsocketBase->lib_Master->lib;

    /* Close the syslog socket, if needed */
    if (bsdsocketBase->bsd_syslog >= 0)
        bsd_close(bsdsocketBase->bsd, bsdsocketBase->bsd_syslog);

    bsdsocketBase->lib.lib_OpenCnt--;
    BSDSocketBase->lib_OpenCnt--;

    if (!bsdsocketBase->lib.lib_OpenCnt) {
        APTR addr;

        addr = (APTR)((IPTR)bsdsocketBase - bsdsocketBase->lib.lib_NegSize);
        bsd_expunge(bsdsocketBase->bsd);
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
