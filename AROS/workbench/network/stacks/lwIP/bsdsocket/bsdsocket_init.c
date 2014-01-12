#include <aros/symbolsets.h>
#include <proto/alib.h>
#include <proto/hostlib.h>

#include "bsdsocket_intern.h"

static int BSDSocket_Init(struct BSDSocketBase *BSDSocketBase)
{
    BSDSocketBase->bsd_global = bsd_global_init();

    return (BSDSocketBase->bsd_global) ? TRUE : FALSE;
}

static int BSDSocket_Expunge(struct BSDSocketBase *BSDSocketBase)
{
    return bsd_global_expunge(BSDSocketBase->bsd_global);
}

ADD2INITLIB(BSDSocket_Init, 0);
ADD2EXPUNGELIB(BSDSocket_Expunge, 0);
