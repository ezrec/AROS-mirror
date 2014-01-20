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

struct ASocket {
    struct Node as_Node;

    ULONG as_Usage;    /* Usage count */

    /* We only support AF_INET here */
    struct sockaddr_in as_Address, as_Endpoint;

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

    struct List ab_InterfaceList;
    struct List ab_SocketList;
};

static inline BOOL addrcheck(struct ASocket_Address *aa)
{
    struct sockaddr_in *sin;

    if (aa->asa_Length < 8)
        return FALSE;

    sin = (struct sockaddr_in *)aa->asa_Address;
    if (sin == NULL)
        return FALSE;

    if (sin->sin_len != 8)
        return FALSE;

    if (sin->sin_family != AF_INET)
        return FALSE;

    return TRUE;
}

#endif /* ASOCKET_INTERN_H */
