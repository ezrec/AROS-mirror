/*
    Copyright © 1995-2012, The AROS Development Team. All rights reserved.
    $Id$
*/

/****************************************************************************************/

#define DEBUG 0
#define DOPEN(x)

#include <aros/debug.h>
#include <proto/exec.h>
#include <exec/errors.h>

#include LC_LIBDEFS_FILE

struct unitExt
{
    struct unit     base;
    LONG            ue_UnitNum;
};

/* Functions located here are extracted from common implementation so that they can be overriden in arch-specific */

extern const char GM_UNIQUENAME(LibName)[];

static int GM_UNIQUENAME(Open)(LIBBASETYPEPTR hdskBase, struct IOExtTD *iotd, IPTR unitnum, ULONG flags)
{
    struct unitExt *unit = NULL, *p = NULL;

    ObtainSemaphore(&hdskBase->sigsem);

    ForeachNode(&hdskBase->units, p)
    {
        if (p->ue_UnitNum == unitnum)
            unit = p;
    }

    if (unit)
    {
        unit->base.usecount++;
        ReleaseSemaphore(&hdskBase->sigsem);
            
        iotd->iotd_Req.io_Unit                    = (struct Unit *)unit;
        iotd->iotd_Req.io_Error                   = 0;
        iotd->iotd_Req.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

        DOPEN(bug("hostdisk: in libopen func. Yep. Unit is already open\n"));    
        return TRUE;
    }

    iotd->iotd_Req.io_Error = IOERR_OPENFAIL;

    ReleaseSemaphore(&hdskBase->sigsem);

    return FALSE;
}

ADD2OPENDEV(GM_UNIQUENAME(Open), 0)
