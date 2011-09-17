/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/oop.h>

#include "pcimockhardware.h"
#include "pcimockhardware_intern.h"
#include "pcimock_intern.h"

#undef HiddPCIMockHardwareAttrBase
#define HiddPCIMockHardwareAttrBase (SD(cl)->hiddPCIMockHardwareAB)


OOP_Object * METHOD(PCIMockHardware, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    return o;
}

VOID PCIMockHardware__Root__Dispose(OOP_Class *cl, OOP_Object *o, OOP_Msg msg)
{
    struct HIDDPCIMockHardwareData * hwdata = OOP_INST_DATA(cl, o);
    ULONG i;

    for (i = 0; i < PCI_REGIONS_COUNT; i++)
        if (hwdata->Regions[i].Address != (IPTR)0) FreeVec((APTR)hwdata->Regions[i].Address);
}

VOID METHOD(PCIMockHardware, Root, Get)
{
    struct HIDDPCIMockHardwareData * hwdata = OOP_INST_DATA(cl, o);
    ULONG idx;

    if (IS_PCIMOCKHARDWARE_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
        case aoHidd_PCIMockHardware_ConfigSpaceAddr:
            *msg->storage = hwdata->Regions[PCI_CONFIG_SPACE].Address;
            return;
        }
    }

    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

