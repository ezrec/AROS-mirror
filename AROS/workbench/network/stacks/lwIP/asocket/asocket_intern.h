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

#include <aros/debug.h>
#include <proto/arossupport.h>
#include <libraries/asocket.h>
#include <exec/nodes.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>

#include "bsd_socket.h"

struct ASocket {
    struct Node as_Node;

    struct {
        ULONG domain, type, protocol;
    } as_Socket;

    struct {
        BOOL enabled;
        ULONG backlog;
    } as_Listen;

    struct ASocket_Notify as_Notify;
};

struct ASocketBase {
    struct Library ab_Lib;

    struct SignalSemaphore ab_Lock;
    struct List ab_SocketList;

    struct bsd *ab_bsd;
};

#endif /* ASOCKET_INTERN_H */
