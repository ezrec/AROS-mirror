#ifndef PROTO_NONVOLATILE_H
#define PROTO_NONVOLATILE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/nonvolatile/nonvolatile.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/nonvolatile_protos.h>

#if !defined(NonvolatileBase) && !defined(__NOLIBBASE__) && !defined(__NONVOLATILE_NOLIBBASE__)
 #ifdef __NONVOLATILE_STDLIBBASE__
  extern struct Library *NonvolatileBase;
 #else
  extern struct Library *NonvolatileBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(NONVOLATILE_NOLIBDEFINES)
#   include <defines/nonvolatile.h>
#endif

#endif /* PROTO_NONVOLATILE_H */
