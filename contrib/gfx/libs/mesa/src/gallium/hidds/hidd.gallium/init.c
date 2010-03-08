/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <hidd/gallium.h>
#include <proto/oop.h>

#include "gallium.h"

static int GalliumHidd_ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
    if (LIBBASE->sd.hiddGalliumBaseDriverAB)
        OOP_ReleaseAttrBase((STRPTR)IID_Hidd_GalliumBaseDriver);

    return TRUE;
}

static int GalliumHidd_InitLib(LIBBASETYPEPTR LIBBASE)
{
    LIBBASE->sd.hiddGalliumBaseDriverAB = OOP_ObtainAttrBase((STRPTR)IID_Hidd_GalliumBaseDriver);

    if (LIBBASE->sd.hiddGalliumBaseDriverAB)
        return TRUE;

    return FALSE;
}

ADD2INITLIB(GalliumHidd_InitLib, 0)
ADD2EXPUNGELIB(GalliumHidd_ExpungeLib, 0)

