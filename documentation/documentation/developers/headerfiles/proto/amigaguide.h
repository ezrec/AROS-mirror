#ifndef PROTO_AMIGAGUIDE_H
#define PROTO_AMIGAGUIDE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/amigaguide/amigaguide.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/amigaguide_protos.h>

#if !defined(AmigaGuideBase) && !defined(__NOLIBBASE__) && !defined(__AMIGAGUIDE_NOLIBBASE__)
 #ifdef __AMIGAGUIDE_STDLIBBASE__
  extern struct Library *AmigaGuideBase;
 #else
  extern struct Library *AmigaGuideBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(AMIGAGUIDE_NOLIBDEFINES)
#   include <defines/amigaguide.h>
#endif

#endif /* PROTO_AMIGAGUIDE_H */
