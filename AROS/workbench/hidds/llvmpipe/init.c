/*
    Copyright 2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <hidd/gallium.h>
#include <proto/oop.h>
#include <proto/exec.h>

#include "llvmpipe_intern.h"

static int LLVMPipeHidd_ExpungeLib(LIBBASETYPEPTR LIBBASE)
{
    if (LIBBASE->sd.LLVMPipeCyberGfxBase)
        CloseLibrary(LIBBASE->sd.LLVMPipeCyberGfxBase);

    if (LIBBASE->sd.hiddGalliumAB)
        OOP_ReleaseAttrBase((STRPTR)IID_Hidd_Gallium);

    return TRUE;
}

static int LLVMPipeHidd_InitLib(LIBBASETYPEPTR LIBBASE)
{
    LIBBASE->sd.LLVMPipeCyberGfxBase = OpenLibrary((STRPTR)"cybergraphics.library",0);

    LIBBASE->sd.hiddGalliumAB = OOP_ObtainAttrBase((STRPTR)IID_Hidd_Gallium);

    if (LIBBASE->sd.LLVMPipeCyberGfxBase && LIBBASE->sd.hiddGalliumAB)
        return TRUE;

    return FALSE;
}

ADD2INITLIB(LLVMPipeHidd_InitLib, 0)
ADD2EXPUNGELIB(LLVMPipeHidd_ExpungeLib, 0)

ADD2LIBS((STRPTR)"gallium.hidd", 7, static struct Library *, GalliumHiddBase);
