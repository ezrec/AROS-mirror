#ifndef PROTO_REQTOOLS_H
#define PROTO_REQTOOLS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/reqtools/reqtools.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/reqtools_protos.h>

#if !defined(ReqToolsBase) && !defined(__NOLIBBASE__) && !defined(__REQTOOLS_NOLIBBASE__)
 #ifdef __REQTOOLS_STDLIBBASE__
  extern struct Library *ReqToolsBase;
 #else
  extern struct ReqToolsBase *ReqToolsBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(REQTOOLS_NOLIBDEFINES)
#   include <defines/reqtools.h>
#endif

#endif /* PROTO_REQTOOLS_H */
