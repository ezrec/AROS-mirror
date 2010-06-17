#ifndef PROTO_OOP_H
#define PROTO_OOP_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/oop/oop.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/oop_protos.h>

#if !defined(OOPBase) && !defined(__NOLIBBASE__) && !defined(__OOP_NOLIBBASE__)
 #ifdef __OOP_STDLIBBASE__
  extern struct Library *OOPBase;
 #else
  extern struct Library *OOPBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(OOP_NOLIBDEFINES)
#   include <defines/oop.h>
#endif

#endif /* PROTO_OOP_H */
