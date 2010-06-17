#ifndef PROTO_MISC_H
#define PROTO_MISC_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/misc/misc.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/misc_protos.h>

#if !defined(MiscBase) && !defined(__NOLIBBASE__) && !defined(__MISC_NOLIBBASE__)
 #ifdef __MISC_STDLIBBASE__
  extern struct Library *MiscBase;
 #else
  extern APTR MiscBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(MISC_NOLIBDEFINES)
#   include <defines/misc.h>
#endif

#endif /* PROTO_MISC_H */
