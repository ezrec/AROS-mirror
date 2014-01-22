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

    struct bsd *ab_bsd;
};

#endif /* ASOCKET_INTERN_H */
