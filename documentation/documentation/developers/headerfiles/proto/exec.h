#ifndef PROTO_EXEC_H
#define PROTO_EXEC_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/exec/exec.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/exec_protos.h>

#if !defined(SysBase) && !defined(__NOLIBBASE__) && !defined(__EXEC_NOLIBBASE__)
 #ifdef __EXEC_STDLIBBASE__
  extern struct Library *SysBase;
 #else
  extern struct ExecBase *SysBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(EXEC_NOLIBDEFINES)
#   include <defines/exec.h>
#endif

#endif /* PROTO_EXEC_H */
