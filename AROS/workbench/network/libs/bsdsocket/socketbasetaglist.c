/*
    Copyright © 2000-2010, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <proto/arossupport.h>

#include "amiga_errlists.h"

#include "bsdsocket_intern.h"

/*****************************************************************************

    NAME */

        AROS_LH1(ULONG, SocketBaseTagList,

/*  SYNOPSIS */
        AROS_LHA(struct TagItem *, tagList, A0),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 49, BSDSocket)

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

    int tindex = 0;
    struct TagItem *tag, *tip = tagList;

    while ((tag = LibNextTagItem(&tip))) {
        IPTR tid;
        ULONG tmp;
        APTR  ptmp;
        BOOL ok = FALSE;

        tid = tag->ti_Tag;
        tindex++;

        /* The TAG_USER bit is required */
        if (!(tid & TAG_USER))
            break;

        /* Check for any 'should be zero' bits set.. */
        if (tid & ~(SBTM_SETREF(SBTS_CODE)))
            break;

#define SBTC_ACCESS(val, tag) ({                    \
        ULONG flags = (tag)->ti_Tag;                \
        IPTR newval = (tag)->ti_Data;               \
        BOOL _ok = TRUE;                            \
        if (flags & SBTF_REF) {                     \
            if (newval) {                           \
                if (flags & SBTF_SET) {             \
                    val = *(typeof(val) *)newval;   \
                } else {                            \
                    *(typeof(val) *)newval = val;   \
                }                                   \
            } else {                                \
                _ok = FALSE;                        \
            }                                       \
        } else {                                    \
            if (flags & SBTF_SET) {                 \
                val = (typeof(val))newval;          \
            } else {                                \
                newval = (typeof(newval))val;       \
            }                                       \
        }                                           \
        _ok; })


        switch (SBTM_CODE(tid)) {
        case SBTC_BREAKMASK:
            ok = SBTC_ACCESS(SocketBase->bsd_sigmask.intr, tag);
            break;
        case SBTC_DTABLESIZE:
            tmp = SocketBase->bsd_fds;
            ok = SBTC_ACCESS(tmp, tag);
            if (ok && tmp != SocketBase->bsd_fds) {
                struct bsd_fd *fds;

                if ((fds = AllocVec(sizeof(*fds)*tmp, MEMF_ANY))) {
                    if (SocketBase->bsd_fds < tmp) {
                        /* Expand the array */
                        CopyMem(SocketBase->bsd_fd, fds, sizeof(*fds)*SocketBase->bsd_fds);
                        memset(&fds[SocketBase->bsd_fds], 0, sizeof(*fds)*(tmp - SocketBase->bsd_fds));
                    } else {
                        int i;
                        /* Shrink the array - do not permit if open sockets */
                        for (i = tmp; i < SocketBase->bsd_fds; i++) {
                            if (SocketBase->bsd_fd[i].asocket) {
                                ok = FALSE;
                                break;
                            }
                        }
                        if (ok)
                            CopyMem(SocketBase->bsd_fd, fds, sizeof(*fds)*tmp);
                    }
                    FreeVec(SocketBase->bsd_fd);

                    SocketBase->bsd_fd = fds;
                    SocketBase->bsd_fds = tmp;
                } else {
                    ok = FALSE;
                }
            }
            break;
        case SBTC_ERRNO:
            ok = SBTC_ACCESS(SocketBase->bsd_errno.value, tag);
            break;
        case SBTC_ERRNOBYTEPTR:
        case SBTC_ERRNOWORDPTR:
        case SBTC_ERRNOLONGPTR:
            /* According to the bsdsocket.library spec,
             * only SBTF_SET is permitted
             */
            if (tag->ti_Tag & SBTF_SET) {
                ptmp = SocketBase->bsd_errno.ptr;
                ok = SBTC_ACCESS(ptmp, tag);
                if (ok && ptmp != SocketBase->bsd_errno.ptr)
                    SetErrnoPtr(ptmp, SBTC_ERRNOBYTEPTR - 20);
            }
            break;
        case SBTC_ERRNOSTRPTR:
            /* According to the bsdsocket.library spec,
             * only SBTF_SET == 0 (Get) is permitted
             */
            if (!(tag->ti_Tag & SBTF_SET)) {
                ULONG err = tag->ti_Data;
                if (err < __sys_nerr) {
                    CONST_STRPTR errstr = __sys_errlist[err];
                    ok = SBTC_ACCESS(errstr, tag);
                }
            }
            break;
        case SBTC_FDCALLBACK:
            ok = SBTC_ACCESS(SocketBase->bsd_fdCallback, tag);
            break;
        case SBTC_HERRNO:
            ok = SBTC_ACCESS(SocketBase->bsd_h_errno, tag);
            break;
        case SBTC_HERRNOSTRPTR:
            /* According to the bsdsocket.library spec,
             * only SBTF_SET == 0 (Get) is permitted
             */
            if (!(tag->ti_Tag & SBTF_SET)) {
                ULONG err = tag->ti_Data;
                if (err < h_nerr) {
                    CONST_STRPTR errstr = h_errlist[err];
                    ok = SBTC_ACCESS(errstr, tag);
                }
            }
            break;
        case SBTC_IOERRNOSTRPTR:
            /* According to the bsdsocket.library spec,
             * only SBTF_SET == 0 (Get) is permitted
             */
            if (!(tag->ti_Tag & SBTF_SET)) {
                ULONG err = tag->ti_Data;
                if (err < io_nerr) {
                    CONST_STRPTR errstr = io_errlist[err];
                    ok = SBTC_ACCESS(errstr, tag);
                }
            }
            break;
        case SBTC_S2ERRNOSTRPTR:
            /* According to the bsdsocket.library spec,
             * only SBTF_SET == 0 (Get) is permitted
             */
            if (!(tag->ti_Tag & SBTF_SET)) {
                ULONG err = tag->ti_Data;
                if (err < sana2io_nerr) {
                    CONST_STRPTR errstr = sana2io_errlist[err];
                    ok = SBTC_ACCESS(errstr, tag);
                }
            }
            break;
        case SBTC_S2WERRNOSTRPTR:
            /* According to the bsdsocket.library spec,
             * only SBTF_SET == 0 (Get) is permitted
             */
            if (!(tag->ti_Tag & SBTF_SET)) {
                ULONG err = tag->ti_Data;
                if (err < sana2wire_nerr) {
                    CONST_STRPTR errstr = sana2wire_errlist[err];
                    ok = SBTC_ACCESS(errstr, tag);
                }
            }
            break;
        case SBTC_LOGFACILITY:
            ok = SBTC_ACCESS(SocketBase->bsd_syslog.facility, tag);
            break;
        case SBTC_LOGMASK:
            ok = SBTC_ACCESS(SocketBase->bsd_syslog.mask, tag);
            break;
        case SBTC_LOGSTAT:
            ok = SBTC_ACCESS(SocketBase->bsd_syslog.logstat, tag);
            break;
        case SBTC_LOGTAGPTR:
            ok = SBTC_ACCESS(SocketBase->bsd_syslog.tag, tag);
            /* SBTC_LOGTAGPTR = NULL is an alias for closelog() */
            if (SocketBase->bsd_syslog.tag == NULL) {
                SocketBase->bsd_syslog.tag = FindTask(NULL)->tc_Node.ln_Name;
                CloseSocket(SocketBase->bsd_syslog.fd);
            }
            break;
        case SBTC_SIGEVENTMASK:
            ok = SBTC_ACCESS(SocketBase->bsd_sigmask.event, tag);
            break;
        case SBTC_SIGIOMASK:
            ok = SBTC_ACCESS(SocketBase->bsd_sigmask.io, tag);
            break;
        case SBTC_SIGURGMASK:
            ok = SBTC_ACCESS(SocketBase->bsd_sigmask.urg, tag);
            break;
        default:
            break;
        }

        if (!ok)
            return tindex;
    }

    return 0;

    AROS_LIBFUNC_EXIT

} /* SocketBaseTagList */
