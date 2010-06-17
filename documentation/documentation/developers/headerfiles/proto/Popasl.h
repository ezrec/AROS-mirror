#ifndef PROTO_POPASL_H
#define PROTO_POPASL_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/popasl.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Popasl_protos.h>

#if !defined(PopaslBase) && !defined(__NOLIBBASE__) && !defined(__POPASL_NOLIBBASE__)
 #ifdef __POPASL_STDLIBBASE__
  extern struct Library *PopaslBase;
 #else
  extern struct Library *PopaslBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(POPASL_NOLIBDEFINES)
#   include <defines/Popasl.h>
#endif

#endif /* PROTO_POPASL_H */
