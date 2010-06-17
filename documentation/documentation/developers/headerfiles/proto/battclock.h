#ifndef PROTO_BATTCLOCK_H
#define PROTO_BATTCLOCK_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/battclock/battclock.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/battclock_protos.h>

#if !defined(BattClockBase) && !defined(__NOLIBBASE__) && !defined(__BATTCLOCK_NOLIBBASE__)
 #ifdef __BATTCLOCK_STDLIBBASE__
  extern struct Library *BattClockBase;
 #else
  extern struct Library *BattClockBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(BATTCLOCK_NOLIBDEFINES)
#   include <defines/battclock.h>
#endif

#endif /* PROTO_BATTCLOCK_H */
