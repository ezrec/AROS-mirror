#ifndef PROTO_KEYMAP_H
#define PROTO_KEYMAP_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/keymap/keymap.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/keymap_protos.h>

#if !defined(KeymapBase) && !defined(__NOLIBBASE__) && !defined(__KEYMAP_NOLIBBASE__)
 #ifdef __KEYMAP_STDLIBBASE__
  extern struct Library *KeymapBase;
 #else
  extern struct Library *KeymapBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(KEYMAP_NOLIBDEFINES)
#   include <defines/keymap.h>
#endif

#endif /* PROTO_KEYMAP_H */
