#ifndef PROTO_GRAPHICS_H
#define PROTO_GRAPHICS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/graphics/graphics.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/graphics_protos.h>

#if !defined(GfxBase) && !defined(__NOLIBBASE__) && !defined(__GRAPHICS_NOLIBBASE__)
 #ifdef __GRAPHICS_STDLIBBASE__
  extern struct Library *GfxBase;
 #else
  extern struct GfxBase *GfxBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(GRAPHICS_NOLIBDEFINES)
#   include <defines/graphics.h>
#endif

#endif /* PROTO_GRAPHICS_H */
