#ifndef PROTO_REXXSYSLIB_H
#define PROTO_REXXSYSLIB_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/rexxsyslib/rexxsyslib.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/rexxsyslib_protos.h>

#if !defined(RexxSysBase) && !defined(__NOLIBBASE__) && !defined(__REXXSYSLIB_NOLIBBASE__)
 #ifdef __REXXSYSLIB_STDLIBBASE__
  extern struct Library *RexxSysBase;
 #else
  extern struct RxsLib *RexxSysBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(REXXSYSLIB_NOLIBDEFINES)
#   include <defines/rexxsyslib.h>
#endif

#endif /* PROTO_REXXSYSLIB_H */
