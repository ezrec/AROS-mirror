#ifndef PROTO_MESA_H
#define PROTO_MESA_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mesa/src/mesa/arosmesa.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/mesa_protos.h>

#if !defined(MesaBase) && !defined(__NOLIBBASE__) && !defined(__MESA_NOLIBBASE__)
 #ifdef __MESA_STDLIBBASE__
  extern struct Library *MesaBase;
 #else
  extern struct Library *MesaBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(MESA_NOLIBDEFINES)
#   include <defines/mesa.h>
#endif

#endif /* PROTO_MESA_H */
