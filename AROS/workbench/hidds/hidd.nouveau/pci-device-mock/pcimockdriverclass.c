/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <proto/oop.h>

#include <aros/symbolsets.h>

#include "pcimock_intern.h"

#undef HiddPCIDriverAttrBase

#define HiddPCIDriverAttrBase   (SD(cl)->hiddPCIDriverAB)
#define HiddAttrBase            (SD(cl)->hiddAB)

OOP_Object * METHOD(PCIMock, Root, New)
{
    struct pRoot_New mymsg;

    struct TagItem mytags[] = 
    {
        { aHidd_Name, (IPTR)"PCIMock" },
        { aHidd_HardwareName, (IPTR)"Mock PCI Driver" },
        { aHidd_PCIDriver_DirectBus, FALSE },
        { TAG_DONE, 0 }
    };

    mymsg.mID = msg->mID;
    mymsg.attrList = (struct TagItem *)&mytags;

    if (msg->attrList)
    {
        mytags[3].ti_Tag = TAG_MORE;
        mytags[3].ti_Data = (IPTR)msg->attrList;
    }

    msg = &mymsg;

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);

    return o;
}

static int PCIMock_ExpungeClass(LIBBASETYPEPTR LIBBASE)
{
    D(bug("[PCIMock] deleting classes\n"));

    OOP_ReleaseAttrBase(IID_Hidd_PCIDriver);
    OOP_ReleaseAttrBase(IID_Hidd);

    return TRUE;
}
	
static int PCIMock_InitClass(LIBBASETYPEPTR LIBBASE)
{
    OOP_Object *pci = NULL;

    D(bug("[PCIMock] Driver initialization\n"));

    LIBBASE->sd.hiddPCIDriverAB = OOP_ObtainAttrBase(IID_Hidd_PCIDriver);
    LIBBASE->sd.hiddAB = OOP_ObtainAttrBase(IID_Hidd);

    if (LIBBASE->sd.hiddPCIDriverAB)
    {
        /*
        * The class may be added to the system. Add the driver
        * to PCI subsystem as well
        */
        struct pHidd_PCI_AddHardwareDriver msg;

        /*
        * PCI is suppose to destroy the class on its Dispose
        */
        msg.driverClass = LIBBASE->sd.driverClass;
        msg.mID = OOP_GetMethodID(IID_Hidd_PCI, moHidd_PCI_AddHardwareDriver);

        pci = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);
        OOP_DoMethod(pci, (OOP_Msg)&msg);
        OOP_DisposeObject(pci);
    }
    else
        return FALSE;

    D(bug("[PCIMock] Driver ClassPtr = %x\n", psd->driverClass));

    return TRUE;
}

ADD2INITLIB(PCIMock_InitClass, 0)
ADD2EXPUNGELIB(PCIMock_ExpungeClass, 0)
