#ifndef PROTO_LOWLEVEL_H
#define PROTO_LOWLEVEL_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/lowlevel/lowlevel.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/lowlevel_protos.h>

#if !defined(LowLevelBase) && !defined(__NOLIBBASE__) && !defined(__LOWLEVEL_NOLIBBASE__)
 #ifdef __LOWLEVEL_STDLIBBASE__
  extern struct Library *LowLevelBase;
 #else
  extern struct Library *LowLevelBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(LOWLEVEL_NOLIBDEFINES)
#   include <defines/lowlevel.h>
#endif

#endif /* PROTO_LOWLEVEL_H */
