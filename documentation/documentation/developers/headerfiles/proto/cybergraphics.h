#ifndef PROTO_CYBERGRAPHICS_H
#define PROTO_CYBERGRAPHICS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/cgfx/cybergraphics.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/cybergraphics_protos.h>

#if !defined(CyberGfxBase) && !defined(__NOLIBBASE__) && !defined(__CYBERGRAPHICS_NOLIBBASE__)
 #ifdef __CYBERGRAPHICS_STDLIBBASE__
  extern struct Library *CyberGfxBase;
 #else
  extern struct Library *CyberGfxBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(CYBERGRAPHICS_NOLIBDEFINES)
#   include <defines/cybergraphics.h>
#endif

#endif /* PROTO_CYBERGRAPHICS_H */
