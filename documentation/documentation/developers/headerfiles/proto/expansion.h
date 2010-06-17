#ifndef PROTO_EXPANSION_H
#define PROTO_EXPANSION_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/expansion/expansion.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/expansion_protos.h>

#if !defined(ExpansionBase) && !defined(__NOLIBBASE__) && !defined(__EXPANSION_NOLIBBASE__)
 #ifdef __EXPANSION_STDLIBBASE__
  extern struct Library *ExpansionBase;
 #else
  extern struct Library *ExpansionBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(EXPANSION_NOLIBDEFINES)
#   include <defines/expansion.h>
#endif

#endif /* PROTO_EXPANSION_H */
