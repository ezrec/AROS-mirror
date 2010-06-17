#ifndef PROTO_THREAD_H
#define PROTO_THREAD_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/thread/thread.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/thread_protos.h>

#if !defined(ThreadBase) && !defined(__NOLIBBASE__) && !defined(__THREAD_NOLIBBASE__)
 #ifdef __THREAD_STDLIBBASE__
  extern struct Library *ThreadBase;
 #else
  extern struct Library *ThreadBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(THREAD_NOLIBDEFINES)
#   include <defines/thread.h>
#endif

#endif /* PROTO_THREAD_H */
