#ifndef PROTO_MUIMASTER_H
#define PROTO_MUIMASTER_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/muimaster.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/muimaster_protos.h>

#if !defined(MUIMasterBase) && !defined(__NOLIBBASE__) && !defined(__MUIMASTER_NOLIBBASE__)
 #ifdef __MUIMASTER_STDLIBBASE__
  extern struct Library *MUIMasterBase;
 #else
  extern struct Library *MUIMasterBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(MUIMASTER_NOLIBDEFINES)
#   include <defines/muimaster.h>
#endif

#endif /* PROTO_MUIMASTER_H */
