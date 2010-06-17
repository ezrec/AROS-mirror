#ifndef PROTO_ASL_H
#define PROTO_ASL_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/asl/asl.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/asl_protos.h>

#if !defined(AslBase) && !defined(__NOLIBBASE__) && !defined(__ASL_NOLIBBASE__)
 #ifdef __ASL_STDLIBBASE__
  extern struct Library *AslBase;
 #else
  extern struct Library *AslBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(ASL_NOLIBDEFINES)
#   include <defines/asl.h>
#endif

#endif /* PROTO_ASL_H */
