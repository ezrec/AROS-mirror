/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <hidd/gallium.h>
#include <proto/oop.h>
#include <proto/exec.h>

#include "nouveau.h"

/* These functions are implemented in drm/drm-aros/nouveau/nouveau_drv.c */
void nouveau_exit(void);
int nouveau_init(void);

static int NouveauHidd_ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
    if (LIBBASE->sd.nouveauinited)
        nouveau_exit();

    if (LIBBASE->sd.hiddGalliumBaseDriverAB)
        OOP_ReleaseAttrBase((STRPTR)IID_Hidd_GalliumBaseDriver);

    return TRUE;
}

static int NouveauHidd_InitLib(LIBBASETYPEPTR LIBBASE)
{
    LIBBASE->sd.hiddGalliumBaseDriverAB = OOP_ObtainAttrBase((STRPTR)IID_Hidd_GalliumBaseDriver);

    LIBBASE->sd.nouveauinited = (nouveau_init() == 0);

    if (LIBBASE->sd.hiddGalliumBaseDriverAB && LIBBASE->sd.nouveauinited)
        return TRUE;

    return FALSE;
}

ADD2INITLIB(NouveauHidd_InitLib, 0)
ADD2EXPUNGELIB(NouveauHidd_ExpungeLib, 0)

