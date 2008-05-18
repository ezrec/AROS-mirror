#define DEBUG 1

#include <asm/amcc440.h>

#include <aros/debug.h>
#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/errors.h>
#include <exec/lists.h>

#include <aros/libcall.h>
#include <aros/symbolsets.h>

#include <oop/oop.h>

#include <devices/sana2.h>
#include <devices/sana2specialstats.h>

#include <utility/utility.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>

#include <hidd/pci.h>

#include <proto/oop.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include "emac.h"
#include LC_LIBDEFS_FILE

static int GM_UNIQUENAME(Init)(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[EMAC ] Init()\n"));

    if (!FindResident("pci-amcc440.hidd"))
    {
        D(bug("[EMAC ] Emac driver may run only on AMCC440 CPU's\n"));
        return FALSE;
    }
    
    if (FindTask(EMAC_TASK_NAME) != NULL)
    {
        D(bug("[EMAC ] Device already up and running.\n"));
        return FALSE;
    }

    LIBBASE->emb_Sana2Info.HardwareType = S2WireType_Ethernet;
    LIBBASE->emb_Sana2Info.MTU = ETH_MTU;
    LIBBASE->emb_Sana2Info.AddrFieldSize = 8 * ETH_ADDRESSSIZE;

    return TRUE;
}

ADD2INITLIB(GM_UNIQUENAME(Init),0)

AROS_LH1(void, beginio,
    AROS_LHA(struct IOSana2Req *, req, A1),
    LIBBASETYPEPTR, LIBBASE, 5, EMAC)
{
    AROS_LIBFUNC_INIT

    AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, abortio,
    AROS_LHA(struct IOSana2Req *, req, A1),
    LIBBASETYPEPTR, LIBBASE, 5, EMAC)
{
    AROS_LIBFUNC_INIT

    return 0;

    AROS_LIBFUNC_EXIT
}
