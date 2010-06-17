#ifndef PROTO_PALETTE_H
#define PROTO_PALETTE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/palette.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Palette_protos.h>

#if !defined(PaletteBase) && !defined(__NOLIBBASE__) && !defined(__PALETTE_NOLIBBASE__)
 #ifdef __PALETTE_STDLIBBASE__
  extern struct Library *PaletteBase;
 #else
  extern struct Library *PaletteBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(PALETTE_NOLIBDEFINES)
#   include <defines/Palette.h>
#endif

#endif /* PROTO_PALETTE_H */
