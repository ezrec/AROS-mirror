#ifndef _LLVMPIPE_INTERN_H
#define _LLVMPIPE_INTERN_H

/*
    Copyright 2013, The AROS Development Team. All rights reserved.
    $Id$
*/


#include LC_LIBDEFS_FILE

#define CLID_Hidd_Gallium_LLVMPipe  "hidd.gallium.llvmpipe"

struct HIDDGalliumLLVMPipeData
{
};

struct llvmpipestaticdata 
{
    OOP_Class       *galliumclass;
    OOP_AttrBase    hiddGalliumAB;
    struct Library  *LLVMPipeCyberGfxBase;
};

LIBBASETYPE 
{
    struct Library              LibNode;
    struct llvmpipestaticdata   sd;
};

#define METHOD(base, id, name) \
  base ## __ ## id ## __ ## name (OOP_Class *cl, OOP_Object *o, struct p ## id ## _ ## name *msg)

#define BASE(lib) ((LIBBASETYPEPTR)(lib))

#define SD(cl) (&BASE(cl->UserData)->sd)

#endif
