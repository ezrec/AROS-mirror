#ifndef PROTO_GADTOOLS_H
#define PROTO_GADTOOLS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/gadtools/gadtools.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/gadtools_protos.h>

#if !defined(GadToolsBase) && !defined(__NOLIBBASE__) && !defined(__GADTOOLS_NOLIBBASE__)
 #ifdef __GADTOOLS_STDLIBBASE__
  extern struct Library *GadToolsBase;
 #else
  extern struct Library *GadToolsBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(GADTOOLS_NOLIBDEFINES)
#   include <defines/gadtools.h>
#endif

#endif /* PROTO_GADTOOLS_H */
