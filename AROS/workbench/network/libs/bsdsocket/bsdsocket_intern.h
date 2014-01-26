/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef BSDSOCKET_INTERN_H
#define BSDSOCKET_INTERN_H

#include <aros/debug.h>

#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/uio.h>

#include <proto/bsdsocket.h>
#include <proto/asocket.h>

#include <libraries/bsdsocket.h>
#include <bsdsocket/socketbasetags.h>

#define BSD_DEFAULT_DTABLESIZE  64

struct bsd_fd {
        struct MinNode node;
        APTR   asocket;
        struct Task *sigioTask;
        struct Task *sigurgTask;
        ULONG  flags;               /* O_* flags */
        ULONG  fd_mask;             /* FD_* event mask in O_ASYNC mode */
        ULONG  id;                  /* Used by WaitSelect and
                                     * ObtainSocket/ReleaseSocket
                                     */
};

/* Global parent */
struct BSDSocketBase {
    struct Library lib;
    BPTR bs_SegList;
    struct Task *bs_NotifyTask;
    struct MsgPort *bs_MsgPort;     /* Serviced by bs_NotifyTask */
    struct List bs_FDList;
    struct SignalSemaphore bs_Lock;
};

/* Per-opener library */
struct bsdsocketBase {
    struct Library lib;
    struct MsgPort *bsd_MsgPort;
    /* File descriptors */
    struct bsd_fd *bsd_fd;
    ULONG bsd_fds;
    struct {
        ULONG intr;
        ULONG io;
        ULONG urg;
        ULONG event;
    } bsd_sigmask;
    struct {
        ULONG value;
        APTR  ptr;
        void (*update)(APTR addr, ULONG value);
    } bsd_errno;
    int bsd_h_errno;
    int (*bsd_fdCallback)(int fd, int action);

    struct {
        int fd;         /* Syslog socket */
        int facility;   /* Syslog facility */
        int mask;       /* Syslog mask */
        int logstat;    /* openlog() options */
        const char *tag;
    } bsd_syslog;

    /* Referenced libraries */
    struct BSDSocketBase *lib_BSDSocketBase;
    struct Library *lib_ASocketBase; /* Opened on library open */
    struct Library *lib_DOSBase;    /* Lazily opened! */
    struct timerequest *bsd_timerequest;
};

#define ASocketBase (SocketBase->lib_ASocketBase)
#define DOSBase (bsdsocket_openlibrary(&SocketBase->lib_DOSBase, "dos.library", 0))

#define BSD_SET_ERRNO(bsd, errval)  do { \
        if (errval != 0) { \
            struct bsdsocketBase *e_bsd = bsd; \
            e_bsd->bsd_errno.value = errval; \
            if (e_bsd->bsd_errno.update) { \
                ASSERT(e_bsd->bsd_errno.ptr != NULL); \
                e_bsd->bsd_errno.update(e_bsd->bsd_errno.ptr, e_bsd->bsd_errno.value); \
            } \
        } \
        D(bug("%s: ERRNO=%d\n", __func__, errval)); \
    } while (0)

#define BSD_GET_FD(bsd, s)    ({ \
        struct bsdsocketBase *a_bsd = bsd; \
        int a_s = s; \
        if (a_s < 0 || a_s >= a_bsd->bsd_fds || a_bsd->bsd_fd[a_s].asocket == NULL) { \
            BSD_SET_ERRNO(a_bsd, EBADF); \
            return -1; \
        } \
        &a_bsd->bsd_fd[a_s]; })

#define BSD_GET_ASOCKET(bsd, s) (BSD_GET_FD(bsd, s)->asocket)

/* Internal functions */

static inline struct Library *bsdsocket_openlibrary(struct Library **libp, CONST_STRPTR name, ULONG version)
{
    ASSERT_VALID_PTR(libp);
    if (*libp == NULL)
        *libp = OpenLibrary(name, version);
    return *libp;
}

#define BSD_FD_CALLBACK(sb, fd, action) \
    ((sb)->bsd_fdCallback ? (sb)->bsd_fdCallback(fd, action) : 0)

/* Find the next available fd, or -1 if done available */
static inline int bsdsocket_fd_avail(struct bsdsocketBase *SocketBase)
{
    int ns;

    for (ns = 0; ns <= SocketBase->bsd_fds; ns++) {
        if (SocketBase->bsd_fd[ns].asocket != NULL)
            continue;
        if (BSD_FD_CALLBACK(SocketBase, ns, FDCB_CHECK) != 0)
            continue;
        return ns;
    }

    BSD_SET_ERRNO(SocketBase, EMFILE);
    return -1;
}

static inline LONG bsdsocket_fd_init(struct bsdsocketBase *SocketBase, int s, APTR asocket, ULONG flags)
{
    LONG err;
    struct bsd_fd *fd;

    BSD_FD_CALLBACK(SocketBase, s, FDCB_ALLOC);

    fd = &SocketBase->bsd_fd[s];

    /* Disable notification, but set up the port to use.
     */
    err = ASocketSet(asocket, AS_TAG_NOTIFY_MSGPORT, SocketBase->bsd_MsgPort,
                              AS_TAG_NOTIFY_FD_MASK, 0, TAG_END);
    if (err)
        return err;

    fd->asocket = asocket;
    fd->sigioTask = fd->sigurgTask = FindTask(NULL);
    fd->flags = flags;

    return 0;
}

static inline struct timerequest *bsdsocket_timerequest(struct bsdsocketBase *SocketBase)
{
    if (SocketBase->bsd_timerequest == NULL) {
        if ((SocketBase->bsd_timerequest = CreateIORequest(SocketBase->bsd_MsgPort, sizeof(*SocketBase->bsd_timerequest)))) {
            if ((0 == OpenDevice("timer.device", UNIT_MICROHZ, &SocketBase->bsd_timerequest->tr_node, 0))) {
                return SocketBase->bsd_timerequest;
            }
            DeleteIORequest(SocketBase->bsd_timerequest);
            SocketBase->bsd_timerequest = NULL;
        }
    }

    return SocketBase->bsd_timerequest;
}

/* Wait for a socket FD_* event to be true
 */
LONG bsdsocket_wait_event(struct bsdsocketBase *SocketBase, APTR as, ULONG mask);

LONG bsdsocket_wait_msg(struct bsdsocketBase *SocketBase, struct ASocket_Msg *msg, size_t *len);

#endif /* BSDSOCKET_INTERN_H */
