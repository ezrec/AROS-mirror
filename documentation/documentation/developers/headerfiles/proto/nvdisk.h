#ifndef PROTO_NVDISK_H
#define PROTO_NVDISK_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/nonvolatile/nvdisk/nvdisk.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/nvdisk_protos.h>

#if !defined(nvdBase) && !defined(__NOLIBBASE__) && !defined(__NVDISK_NOLIBBASE__)
 #ifdef __NVDISK_STDLIBBASE__
  extern struct Library *nvdBase;
 #else
  extern struct Library *nvdBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(NVDISK_NOLIBDEFINES)
#   include <defines/nvdisk.h>
#endif

#endif /* PROTO_NVDISK_H */
