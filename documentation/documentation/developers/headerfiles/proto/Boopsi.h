#ifndef PROTO_BOOPSI_H
#define PROTO_BOOPSI_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/boopsi.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Boopsi_protos.h>

#if !defined(BoopsiBase) && !defined(__NOLIBBASE__) && !defined(__BOOPSI_NOLIBBASE__)
 #ifdef __BOOPSI_STDLIBBASE__
  extern struct Library *BoopsiBase;
 #else
  extern struct Library *BoopsiBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(BOOPSI_NOLIBDEFINES)
#   include <defines/Boopsi.h>
#endif

#endif /* PROTO_BOOPSI_H */
