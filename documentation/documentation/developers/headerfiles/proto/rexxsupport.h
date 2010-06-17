#ifndef PROTO_REXXSUPPORT_H
#define PROTO_REXXSUPPORT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/rexxsupport/rexxsupport.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/rexxsupport_protos.h>

#if !defined(RexxSupportBase) && !defined(__NOLIBBASE__) && !defined(__REXXSUPPORT_NOLIBBASE__)
 #ifdef __REXXSUPPORT_STDLIBBASE__
  extern struct Library *RexxSupportBase;
 #else
  extern struct Library *RexxSupportBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(REXXSUPPORT_NOLIBDEFINES)
#   include <defines/rexxsupport.h>
#endif

#endif /* PROTO_REXXSUPPORT_H */
