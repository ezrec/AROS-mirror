#ifndef PROTO_FREETYPE2_H
#define PROTO_FREETYPE2_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/freetype/src/freetype2.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/freetype2_protos.h>

#if !defined(FreeType2Base) && !defined(__NOLIBBASE__) && !defined(__FREETYPE2_NOLIBBASE__)
 #ifdef __FREETYPE2_STDLIBBASE__
  extern struct Library *FreeType2Base;
 #else
  extern struct Library *FreeType2Base;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(FREETYPE2_NOLIBDEFINES)
#   include <defines/freetype2.h>
#endif

#endif /* PROTO_FREETYPE2_H */
