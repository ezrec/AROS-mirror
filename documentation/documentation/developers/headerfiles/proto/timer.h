#ifndef PROTO_TIMER_H
#define PROTO_TIMER_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/timer/timer.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/timer_protos.h>

#if !defined(TimerBase) && !defined(__NOLIBBASE__) && !defined(__TIMER_NOLIBBASE__)
 #ifdef __TIMER_STDLIBBASE__
  extern struct Library *TimerBase;
 #else
  extern struct Device *TimerBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(TIMER_NOLIBDEFINES)
#   include <defines/timer.h>
#endif

#endif /* PROTO_TIMER_H */
