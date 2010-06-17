#ifndef PROTO_POPSCREEN_H
#define PROTO_POPSCREEN_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/popscreen.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Popscreen_protos.h>

#if !defined(PopscreenBase) && !defined(__NOLIBBASE__) && !defined(__POPSCREEN_NOLIBBASE__)
 #ifdef __POPSCREEN_STDLIBBASE__
  extern struct Library *PopscreenBase;
 #else
  extern struct Library *PopscreenBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(POPSCREEN_NOLIBDEFINES)
#   include <defines/Popscreen.h>
#endif

#endif /* PROTO_POPSCREEN_H */
