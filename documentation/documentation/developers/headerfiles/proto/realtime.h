#ifndef PROTO_REALTIME_H
#define PROTO_REALTIME_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/realtime/realtime.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/realtime_protos.h>

#if !defined(RealTimeBase) && !defined(__NOLIBBASE__) && !defined(__REALTIME_NOLIBBASE__)
 #ifdef __REALTIME_STDLIBBASE__
  extern struct Library *RealTimeBase;
 #else
  extern struct Library *RealTimeBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(REALTIME_NOLIBDEFINES)
#   include <defines/realtime.h>
#endif

#endif /* PROTO_REALTIME_H */
