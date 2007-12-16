#include <aros/symbolsets.h>

#include "base.h"
#include "class_protos.h"

struct UrltextBase *UrltextBase;


static int InitFunc(struct UrltextBase *lh)
{
    UrltextBase = lh;
    return TRUE;
}


static int OpenFunc(struct UrltextBase *lh)
{
    if (initBase(lh))
	return TRUE;
    return FALSE;
}


static void CloseFunc(struct UrltextBase *lh)
{
    freeBase(lh);
}


ADD2INITLIB(InitFunc, 0);
ADD2OPENLIB(OpenFunc, 0);
ADD2CLOSELIB(CloseFunc, 0);
