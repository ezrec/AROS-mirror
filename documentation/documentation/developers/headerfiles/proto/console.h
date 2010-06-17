#ifndef PROTO_CONSOLE_H
#define PROTO_CONSOLE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/devs/console/console.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/console_protos.h>

#if !defined(ConsoleDevice) && !defined(__NOLIBBASE__) && !defined(__CONSOLE_NOLIBBASE__)
 #ifdef __CONSOLE_STDLIBBASE__
  extern struct Library *ConsoleDevice;
 #else
  extern struct Device *ConsoleDevice;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(CONSOLE_NOLIBDEFINES)
#   include <defines/console.h>
#endif

#endif /* PROTO_CONSOLE_H */
