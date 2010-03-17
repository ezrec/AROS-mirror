/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <hidd/gallium.h>
#include <proto/oop.h>
#include <aros/debug.h>

#include "gallium.h"

#undef HiddGalliumBaseDriverAttrBase
#define HiddGalliumBaseDriverAttrBase   (SD(cl)->hiddGalliumBaseDriverAB)

OOP_Object *METHOD(GALLIUMBASEDRIVER, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);

    return o;
}

VOID METHOD(GALLIUMBASEDRIVER, Root, Get)
{
    ULONG idx;

    if (IS_GALLIUMBASEDRIVER_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
            case aoHidd_GalliumBaseDriver_GalliumInterfaceVersion:
                /* Always return 0 here. It is up to child class to return its
                   proper version. We can end up in situation where child classes
                   are from different build than base driver class. In such case,
                   returning here proper version would made the client use child
                   class even though it had different interface version */
                *msg->storage = 0; 
                break;
        }
    }
    else
    {
        OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
    }
}

APTR METHOD(GALLIUMBASEDRIVER, Hidd_GalliumBaseDriver, CreatePipeScreen)
{
    bug("[GalliumBaseDriver]::CreatePipeScreen - abstract called\n");
    return NULL;
}

VOID METHOD(GALLIUMBASEDRIVER, Hidd_GalliumBaseDriver, QueryDepthStencil)
{
    *msg->depthbits = 0;
    *msg->stencilbits = 0;
    bug("[GalliumBaseDriver]::QueryDepthStencil - abstract called\n");
}

VOID METHOD(GALLIUMBASEDRIVER, Hidd_GalliumBaseDriver, DisplaySurface)
{
    bug("[GalliumBaseDriver]::DisplaySurface - abstract called\n");
}

VOID METHOD(GALLIUMBASEDRIVER, Hidd_GalliumBaseDriver, DestroyPipeScreen)
{
    bug("[GalliumBaseDriver]::DestroyPipeScreen - abstract called\n");
}
