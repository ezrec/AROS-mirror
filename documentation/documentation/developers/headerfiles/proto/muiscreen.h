#ifndef PROTO_MUISCREEN_H
#define PROTO_MUISCREEN_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muiscreen/muiscreen.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/muiscreen_protos.h>

#if !defined(MUIScreenBase) && !defined(__NOLIBBASE__) && !defined(__MUISCREEN_NOLIBBASE__)
 #ifdef __MUISCREEN_STDLIBBASE__
  extern struct Library *MUIScreenBase;
 #else
  extern struct Library *MUIScreenBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(MUISCREEN_NOLIBDEFINES)
#   include <defines/muiscreen.h>
#endif

#endif /* PROTO_MUISCREEN_H */
