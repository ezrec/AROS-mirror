#ifndef PROTO_GAMEPORT_H
#define PROTO_GAMEPORT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/devs/gameport/gameport.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/gameport_protos.h>

#if !defined(GPBase) && !defined(__NOLIBBASE__) && !defined(__GAMEPORT_NOLIBBASE__)
 #ifdef __GAMEPORT_STDLIBBASE__
  extern struct Library *GPBase;
 #else
  extern struct Device *GPBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(GAMEPORT_NOLIBDEFINES)
#   include <defines/gameport.h>
#endif

#endif /* PROTO_GAMEPORT_H */
