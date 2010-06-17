#ifndef PROTO_CAMD_H
#define PROTO_CAMD_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/camd/camd.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/camd_protos.h>

#if !defined(CamdBase) && !defined(__NOLIBBASE__) && !defined(__CAMD_NOLIBBASE__)
 #ifdef __CAMD_STDLIBBASE__
  extern struct Library *CamdBase;
 #else
  extern struct Library *CamdBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(CAMD_NOLIBDEFINES)
#   include <defines/camd.h>
#endif

#endif /* PROTO_CAMD_H */
