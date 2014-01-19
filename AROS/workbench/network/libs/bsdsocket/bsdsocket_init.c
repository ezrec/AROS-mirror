#include <aros/symbolsets.h>
#include <proto/alib.h>
#include <proto/hostlib.h>

#include "bsdsocket_intern.h"

static int BSDSocket_Init(struct BSDSocketBase *BSDSocketBase)
{
    NEWLIST(&BSDSocketBase->bs_FDList);
    InitSemaphore(&BSDSocketBase->bs_Lock);

    return TRUE;
}

static int BSDSocket_Expunge(struct BSDSocketBase *BSDSocketBase)
{
    return TRUE;
}

ADD2INITLIB(BSDSocket_Init, 0);
ADD2EXPUNGELIB(BSDSocket_Expunge, 0);
