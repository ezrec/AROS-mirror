/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#include <aros/symbolsets.h>

#include "asocket_intern.h"

#include "bsd_socket.h"

static int ASocket_Init(struct ASocketBase *ASocketBase)
{
    ASocketBase->ab_bsd = bsd_init();

    D(bug("%s: ASocketBase->ab_bsd=%p\n", __func__, ASocketBase->ab_bsd));

    InitSemaphore(&ASocketBase->ab_Lock);
    NEWLIST(&ASocketBase->ab_SocketList);

    return (ASocketBase->ab_bsd) ? TRUE : FALSE;
}

static int ASocket_Expunge(struct ASocketBase *ASocketBase)
{
    D(bug("%s: Expunge\n"));
    return TRUE;
}

ADD2INITLIB(ASocket_Init, 0);
ADD2EXPUNGELIB(ASocket_Expunge, 0);
