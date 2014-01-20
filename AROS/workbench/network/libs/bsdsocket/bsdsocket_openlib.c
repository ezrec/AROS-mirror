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

    struct bsdsocketBase *bsdsocketBase;
    struct Library *asocketBase;
    struct MsgPort *mp;

    asocketBase = OpenLibrary("asocket.library",0);
    if (asocketBase == NULL)
        return FALSE;

    mp = CreateMsgPort();
    if (mp == NULL) {
        CloseLibrary(asocketBase);
        return NULL;
    }

    bsdsocketBase = (struct bsdsocketBase *)MakeLibrary(BSDSocket_FuncTable, NULL, NULL, sizeof(struct bsdsocketBase), NULL);
    if (!bsdsocketBase) {
        DeleteMsgPort(mp);
        CloseLibrary(asocketBase);
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

    bsdsocketBase->bsd_fds = BSD_DEFAULT_DTABLESIZE;
    bsdsocketBase->bsd_fd = AllocVec(sizeof(bsdsocketBase->bsd_fd[0]), MEMF_ANY | MEMF_CLEAR);
    if (bsdsocketBase->bsd_fd == NULL) {
        APTR addr;
        addr = (APTR)((IPTR)bsdsocketBase - bsdsocketBase->lib.lib_NegSize);
        FreeMem(addr, bsdsocketBase->lib.lib_NegSize + bsdsocketBase->lib.lib_PosSize);
        DeleteMsgPort(mp);
        CloseLibrary(asocketBase);
        return NULL;
    }

    bsdsocketBase->bsd_MsgPort = mp;

    bsdsocketBase->bsd_sigmask.intr = SIGBREAKF_CTRL_C;
    bsdsocketBase->bsd_syslog.fd = -1;
    bsdsocketBase->bsd_syslog.facility = LOG_USER;
    bsdsocketBase->bsd_syslog.mask = 0xff;
    bsdsocketBase->bsd_syslog.tag = FindTask(NULL)->tc_Node.ln_Name;

    bsdsocketBase->lib_BSDSocketBase = BSDSocketBase;
    bsdsocketBase->lib_ASocketBase = asocketBase;

    bsdsocketBase->lib.lib_OpenCnt++;
    BSDSocketBase->lib.lib_OpenCnt++;

    return &bsdsocketBase->lib;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(BPTR, BSDSocket_CloseLib,
         struct bsdsocketBase *, SocketBase, 2, BSDSocket)
{
    AROS_LIBFUNC_INIT
    struct Library *BSDSocketBase = &SocketBase->lib_BSDSocketBase->lib;

    /* Close the syslog socket, if needed */
    if (SocketBase->bsd_syslog.fd >= 0)
        CloseSocket(SocketBase->bsd_syslog.fd);

    SocketBase->lib.lib_OpenCnt--;
    BSDSocketBase->lib_OpenCnt--;

    if (SocketBase->lib.lib_OpenCnt == 0) {
        APTR addr;

        CloseLibrary(SocketBase->lib_DOSBase);
        CloseLibrary(SocketBase->lib_ASocketBase);
        DeleteMsgPort(SocketBase->bsd_MsgPort);

        if (SocketBase->bsd_timerequest)
            DeleteIORequest(&SocketBase->bsd_timerequest->tr_node);

        addr = (APTR)((IPTR)SocketBase - SocketBase->lib.lib_NegSize);
        FreeMem(addr, SocketBase->lib.lib_NegSize + SocketBase->lib.lib_PosSize);
        return BNULL;
    }

    if (BSDSocketBase->lib_OpenCnt > 0) {
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
