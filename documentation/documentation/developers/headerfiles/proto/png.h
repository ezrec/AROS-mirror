#ifndef PROTO_PNG_H
#define PROTO_PNG_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/png/png.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/png_protos.h>

#if !defined(PNGBase) && !defined(__NOLIBBASE__) && !defined(__PNG_NOLIBBASE__)
 #ifdef __PNG_STDLIBBASE__
  extern struct Library *PNGBase;
 #else
  extern struct Library *PNGBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(PNG_NOLIBDEFINES)
#   include <defines/png.h>
#endif

#endif /* PROTO_PNG_H */
