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

    return (ASocketBase->ab_bsd) ? TRUE : FALSE;
}

static int ASocket_Expunge(struct ASocketBase *ASocketBase)
{
    return TRUE;
}

ADD2INITLIB(ASocket_Init, 0);
ADD2EXPUNGELIB(ASocket_Expunge, 0);
