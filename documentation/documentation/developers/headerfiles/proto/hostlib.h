#ifndef PROTO_HOSTLIB_H
#define PROTO_HOSTLIB_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/hostlib/hostlib.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/hostlib_protos.h>

#if !defined(HostLibBase) && !defined(__NOLIBBASE__) && !defined(__HOSTLIB_NOLIBBASE__)
 #ifdef __HOSTLIB_STDLIBBASE__
  extern struct Library *HostLibBase;
 #else
  extern APTR HostLibBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(HOSTLIB_NOLIBDEFINES)
#   include <defines/hostlib.h>
#endif

#endif /* PROTO_HOSTLIB_H */
