#ifndef PROTO_AROS_H
#define PROTO_AROS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/aros/aros.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/aros_protos.h>

#if !defined(ArosBase) && !defined(__NOLIBBASE__) && !defined(__AROS_NOLIBBASE__)
 #ifdef __AROS_STDLIBBASE__
  extern struct Library *ArosBase;
 #else
  extern struct Library *ArosBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(AROS_NOLIBDEFINES)
#   include <defines/aros.h>
#endif

#endif /* PROTO_AROS_H */
