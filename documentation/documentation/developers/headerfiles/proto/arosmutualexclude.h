#ifndef PROTO_AROSMUTUALEXCLUDE_H
#define PROTO_AROSMUTUALEXCLUDE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/gadgets/arosmutualexclude/arosmutualexclude.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/arosmutualexclude_protos.h>

#if !defined(AROSMXBase) && !defined(__NOLIBBASE__) && !defined(__AROSMUTUALEXCLUDE_NOLIBBASE__)
 #ifdef __AROSMUTUALEXCLUDE_STDLIBBASE__
  extern struct Library *AROSMXBase;
 #else
  extern struct Library *AROSMXBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(AROSMUTUALEXCLUDE_NOLIBDEFINES)
#   include <defines/arosmutualexclude.h>
#endif

#endif /* PROTO_AROSMUTUALEXCLUDE_H */
