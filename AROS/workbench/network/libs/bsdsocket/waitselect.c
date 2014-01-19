/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <devices/timer.h>

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH6(int, WaitSelect,

/*  SYNOPSIS */
        AROS_LHA(int,              nfds,      D0),
        AROS_LHA(fd_set *,         readfds,   A0),
        AROS_LHA(fd_set *,         writefds,  A1),
        AROS_LHA(fd_set *,         exceptfds, A2),
        AROS_LHA(struct timeval *, timeout,   A3),
        AROS_LHA(ULONG *,          sigmask,   D1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 21, BSDSocket)

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

    /* Collect all the nodes we are interested in */
    struct MinList fdlist;
    struct bsd_fd *fd;
    int i;
    struct timerequest *tr;
    struct Message *msg;
    ULONG signals;
    int count = 0;
    const ULONG rd_mask = FD_READ;
    const ULONG wr_mask = FD_WRITE;
    const ULONG ex_mask = (FD_ACCEPT | FD_CONNECT | FD_OOB | FD_ERROR | FD_CLOSE);

    signals = (1 << SocketBase->bsd_MsgPort->mp_SigBit);
    if (sigmask)
        signals |= *sigmask;

    NEWLIST(&fdlist);
    for (i = 0; i < nfds; i++) {
        if ((readfds  && FD_ISSET(i, readfds))  ||
            (writefds && FD_ISSET(i, writefds)) ||
            (exceptfds && FD_ISSET(i, exceptfds))) {
            struct bsd_fd *fd = BSD_GET_FD(SocketBase, i);

            fd->id = i;
            ADDTAIL(&fdlist, fd);
        }
    }

    /* Enable notification for all the interested sockets */
    ForeachNode(&fdlist, fd) {
        APTR as = fd->asocket;
        ULONG mask = 0;

        /* We use read-modify-write to handle duplicates */
        ASocketGet(as, AS_TAG_NOTIFY_FD_MASK, &mask, TAG_END);
        if (readfds && FD_ISSET(fd->id, readfds))
            mask |= rd_mask;
        if (writefds && FD_ISSET(fd->id, writefds))
            mask |= wr_mask;
        if (exceptfds && FD_ISSET(fd->id, exceptfds))
            mask |= ex_mask;
        ASocketSet(as, AS_TAG_NOTIFY_FD_MASK, mask,
                       AS_TAG_NOTIFY_MSGPORT, SocketBase->bsd_MsgPort,
                       AS_TAG_NOTIFY_NAME, fd,
                       TAG_END);
    }

    /* Wait for any notification event, or timeout */
    if (timeout == NULL) {
        tr = NULL;
    } else {
        tr = bsdsocket_timerequest(SocketBase);
        tr->tr_time.tv_secs = timeout->tv_sec;
        tr->tr_time.tv_micro = timeout->tv_usec;
        tr->tr_node.io_Command = TR_ADDREQUEST;
        tr->tr_node.io_Error = 0;
        SendIO(&tr->tr_node);
    }

    /* Wait for an event or timeout */
    signals = Wait(signals);
    AbortIO(&tr->tr_node);
    WaitIO(&tr->tr_node);

    if (readfds)
        FD_ZERO(readfds);

    if (writefds)
        FD_ZERO(writefds);

    if (exceptfds)
        FD_ZERO(exceptfds);

    /* Disable notification for all the interested sockets */
    ForeachNode(&fdlist, fd) {
        APTR as = fd->asocket;

        ASocketSet(as, AS_TAG_NOTIFY_FD_MASK, 0,
                       AS_TAG_NOTIFY_MSGPORT, NULL,
                       AS_TAG_NOTIFY_NAME, NULL,
                       TAG_END);
    }

    count = 0;
    if (signals & (1 << SocketBase->bsd_MsgPort->mp_SigBit)) {
        while ((msg = GetMsg(SocketBase->bsd_MsgPort))) {
            struct ASocket_Notify *asn;
            struct bsd_fd *fd;
            ULONG mask;
            BOOL active = FALSE;

            ASSERT(msg->mn_Node.ln_Type == NT_AS_NOTIFY);

            asn = (struct ASocket_Notify *)msg;
            fd = (struct bsd_fd *)(((struct Node *)asn)->ln_Name);

            mask = asn->asn_Events & asn->asn_NotifyEvents;
            if (readfds && (mask & rd_mask)) {
                FD_SET(fd->id, readfds);
                active = TRUE;
            }
            if (writefds && (mask & wr_mask)) {
                FD_SET(fd->id, writefds);
                active = TRUE;
            }
            if (exceptfds && (mask & ex_mask)) {
                FD_SET(fd->id, exceptfds);
                active = TRUE;
            }
            if (active)
                count++;

            /* This is required to release the message
             * back to the ASocketBase
             */
            ReplyMsg(msg);
        }
    }

    /* Oh crap. We got an exception signal.
     * We need to reset those signals...
     */
    if (sigmask && (*sigmask & signals)) {
        Signal(FindTask(NULL), signals & *sigmask);
    }

    BSD_SET_ERRNO(SocketBase, 0);
    return count;

    AROS_LIBFUNC_EXIT

} /* WaitSelect */
