/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <hidd/gallium.h>
#include <proto/oop.h>
#include <proto/exec.h>
#include <aros/debug.h>

#include "gallium.h"

#undef HiddGalliumBaseDriverAttrBase
#define HiddGalliumBaseDriverAttrBase   (SD(cl)->hiddGalliumBaseDriverAB)

struct Library * HIDDSoftpipeBase = NULL;

OOP_Object * METHOD(GALLIUMDRIVERFACTORY, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);

    return o;
}

OOP_Object * METHOD(GALLIUMDRIVERFACTORY, Hidd_GalliumDriverFactory, GetDriver)
{
    /* TODO: add logic to crate specialized drivers */
    OOP_Object * driver = NULL;
    
    if (HIDDSoftpipeBase == NULL)
    {
        if (!(HIDDSoftpipeBase = OpenLibrary("softpipe.hidd", 1)))
            return NULL;
    }
    
    driver = OOP_NewObject(NULL, "hidd.gallium.softpipedriver", NULL);
    
    
    /* Check driver interface version in relation to client version */
    if (driver)
    {
        /* Check interface version */
        IPTR galliuminterfaceversion = 0;
        OOP_GetAttr(driver, aHidd_GalliumBaseDriver_GalliumInterfaceVersion, &galliuminterfaceversion);
        
        /* Only matching version is allowed */
        if (msg->galliuminterfaceversion != (ULONG)galliuminterfaceversion)
        {
            bug("[GalliumDriverFactory] Error - client requested interface version %d while driver supplies version %d\n",
                msg->galliuminterfaceversion, galliuminterfaceversion);
            OOP_DisposeObject(driver);
            driver = NULL;
        }
    }
    
    return driver;
}

