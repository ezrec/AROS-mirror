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

#include <proto/bsdsocket.h>
#include "bsd_socket.h"

/* Global parent */
struct BSDSocketBase {
    struct Library lib;
    BPTR bs_SegList;
    struct bsd_global *bsd_global;
};

/* Per-opener library */
struct bsdsocketBase {
    struct Library lib;
    struct BSDSocketBase *lib_Master;
    struct bsd *bsd;
    ULONG sigintr;
    ULONG sigio;
    ULONG sigurg;
};

#endif /* BSDSOCKET_INTERN_H */
