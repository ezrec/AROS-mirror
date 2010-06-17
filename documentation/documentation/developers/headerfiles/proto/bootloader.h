#ifndef PROTO_BOOTLOADER_H
#define PROTO_BOOTLOADER_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/bootloader/bootloader.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/bootloader_protos.h>

#if !defined(BootLoaderBase) && !defined(__NOLIBBASE__) && !defined(__BOOTLOADER_NOLIBBASE__)
 #ifdef __BOOTLOADER_STDLIBBASE__
  extern struct Library *BootLoaderBase;
 #else
  extern APTR BootLoaderBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(BOOTLOADER_NOLIBDEFINES)
#   include <defines/bootloader.h>
#endif

#endif /* PROTO_BOOTLOADER_H */
