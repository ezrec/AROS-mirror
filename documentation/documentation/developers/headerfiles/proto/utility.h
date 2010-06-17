#ifndef PROTO_UTILITY_H
#define PROTO_UTILITY_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/utility/utility.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/utility_protos.h>

#if !defined(UtilityBase) && !defined(__NOLIBBASE__) && !defined(__UTILITY_NOLIBBASE__)
 #ifdef __UTILITY_STDLIBBASE__
  extern struct Library *UtilityBase;
 #else
  extern struct UtilityBase *UtilityBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(UTILITY_NOLIBDEFINES)
#   include <defines/utility.h>
#endif

#endif /* PROTO_UTILITY_H */
