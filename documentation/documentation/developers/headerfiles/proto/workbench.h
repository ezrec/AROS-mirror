#ifndef PROTO_WORKBENCH_H
#define PROTO_WORKBENCH_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/workbench/workbench.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/workbench_protos.h>

#if !defined(WorkbenchBase) && !defined(__NOLIBBASE__) && !defined(__WORKBENCH_NOLIBBASE__)
 #ifdef __WORKBENCH_STDLIBBASE__
  extern struct Library *WorkbenchBase;
 #else
  extern struct Library *WorkbenchBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(WORKBENCH_NOLIBDEFINES)
#   include <defines/workbench.h>
#endif

#endif /* PROTO_WORKBENCH_H */
