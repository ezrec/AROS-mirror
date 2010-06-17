#ifndef PROTO_CLOCK_H
#define PROTO_CLOCK_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/zune/clock/clock.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Clock_protos.h>

#if !defined(ClockBase) && !defined(__NOLIBBASE__) && !defined(__CLOCK_NOLIBBASE__)
 #ifdef __CLOCK_STDLIBBASE__
  extern struct Library *ClockBase;
 #else
  extern struct Library *ClockBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(CLOCK_NOLIBDEFINES)
#   include <defines/Clock.h>
#endif

#endif /* PROTO_CLOCK_H */
