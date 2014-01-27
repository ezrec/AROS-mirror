/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef ASOCKET_INTERN_H
#define ASOCKET_INTERN_H

#include <errno.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <aros/debug.h>
#include <proto/arossupport.h>
#include <libraries/asocket.h>
#include <exec/nodes.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))
#endif

#include "bsd_socket.h"

#include <net/if.h>

struct ASocket {
    struct Node as_Node;

    struct {
        ULONG domain, type, protocol;
    } as_Socket;

    struct {
        BOOL enabled;
        ULONG backlog;
    } as_Listen;

    /* type = SOCK_RAW information */
    struct {
        int index;
        TEXT name[IFNAMSIZ];
    } as_IFace;

    struct ASocket_Notify as_Notify;

    struct bsd_sock *as_bsd;
};

struct ASocketBase {
    struct Library ab_Lib;

    struct SignalSemaphore ab_Lock;
    struct List ab_SocketList;

    struct bsd *ab_bsd;

    BPTR ab_SegList;
};

#endif /* ASOCKET_INTERN_H */
