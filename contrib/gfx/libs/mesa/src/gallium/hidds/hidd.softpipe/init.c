/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <hidd/gallium.h>
#include <proto/oop.h>
#include <proto/exec.h>

#include "softpipe.h"

static int SoftpipeHidd_ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
    if (LIBBASE->sd.SoftpipeCyberGfxBase)
        CloseLibrary(LIBBASE->sd.SoftpipeCyberGfxBase);
        
    return TRUE;
}

static int SoftpipeHidd_InitLib(LIBBASETYPEPTR LIBBASE)
{
    LIBBASE->sd.SoftpipeCyberGfxBase = OpenLibrary((STRPTR)"cybergraphics.library",0);

    if (LIBBASE->sd.SoftpipeCyberGfxBase)
        return TRUE;

    return FALSE;
}

ADD2INITLIB(SoftpipeHidd_InitLib, 0)
ADD2EXPUNGELIB(SoftpipeHidd_ExpungeLib, 0)

