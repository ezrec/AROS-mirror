#ifndef PROTO_LAYERS_H
#define PROTO_LAYERS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/hyperlayers/layers.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/layers_protos.h>

#if !defined(LayersBase) && !defined(__NOLIBBASE__) && !defined(__LAYERS_NOLIBBASE__)
 #ifdef __LAYERS_STDLIBBASE__
  extern struct Library *LayersBase;
 #else
  extern struct Library *LayersBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(LAYERS_NOLIBDEFINES)
#   include <defines/layers.h>
#endif

#endif /* PROTO_LAYERS_H */
