#ifndef PROTO_INTUITION_H
#define PROTO_INTUITION_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/intuition/intuition.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/intuition_protos.h>

#if !defined(IntuitionBase) && !defined(__NOLIBBASE__) && !defined(__INTUITION_NOLIBBASE__)
 #ifdef __INTUITION_STDLIBBASE__
  extern struct Library *IntuitionBase;
 #else
  extern struct IntuitionBase *IntuitionBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(INTUITION_NOLIBDEFINES)
#   include <defines/intuition.h>
#endif

#endif /* PROTO_INTUITION_H */
