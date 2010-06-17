#ifndef PROTO_DOSBOOT_H
#define PROTO_DOSBOOT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/dosboot/dosboot.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/dosboot_protos.h>

#if !defined(DOSBootBase) && !defined(__NOLIBBASE__) && !defined(__DOSBOOT_NOLIBBASE__)
 #ifdef __DOSBOOT_STDLIBBASE__
  extern struct Library *DOSBootBase;
 #else
  extern APTR DOSBootBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(DOSBOOT_NOLIBDEFINES)
#   include <defines/dosboot.h>
#endif

#endif /* PROTO_DOSBOOT_H */
