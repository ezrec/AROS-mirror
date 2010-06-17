#ifndef PROTO_COOLIMAGES_H
#define PROTO_COOLIMAGES_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/coolimages/coolimages.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/coolimages_protos.h>

#if !defined(CoolImagesBase) && !defined(__NOLIBBASE__) && !defined(__COOLIMAGES_NOLIBBASE__)
 #ifdef __COOLIMAGES_STDLIBBASE__
  extern struct Library *CoolImagesBase;
 #else
  extern struct Library *CoolImagesBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(COOLIMAGES_NOLIBDEFINES)
#   include <defines/coolimages.h>
#endif

#endif /* PROTO_COOLIMAGES_H */
