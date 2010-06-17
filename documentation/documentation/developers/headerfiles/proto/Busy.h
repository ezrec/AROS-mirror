#ifndef PROTO_BUSY_H
#define PROTO_BUSY_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/zune/busy/busy.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Busy_protos.h>

#if !defined(BusyBase) && !defined(__NOLIBBASE__) && !defined(__BUSY_NOLIBBASE__)
 #ifdef __BUSY_STDLIBBASE__
  extern struct Library *BusyBase;
 #else
  extern struct Library *BusyBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(BUSY_NOLIBDEFINES)
#   include <defines/Busy.h>
#endif

#endif /* PROTO_BUSY_H */
