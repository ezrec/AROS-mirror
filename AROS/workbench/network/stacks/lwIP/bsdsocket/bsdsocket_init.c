#include <aros/symbolsets.h>
#include <proto/alib.h>
#include <proto/hostlib.h>

#include "bsdsocket_intern.h"

static int bsdsocket_Init(struct bsdsocketBase *SocketBase)
{
    return TRUE;
}

static int bsdsocket_Cleanup(struct bsdsocketBase *SocketBase)
{
    return TRUE;
}

ADD2INITLIB(bsdsocket_Init, 0);
ADD2EXPUNGELIB(bsdsocket_Cleanup, 0);
