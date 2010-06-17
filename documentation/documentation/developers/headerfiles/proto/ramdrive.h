#ifndef PROTO_RAMDRIVE_H
#define PROTO_RAMDRIVE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/devs/ramdrive.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/ramdrive_protos.h>

#if !defined(RamdriveBase) && !defined(__NOLIBBASE__) && !defined(__RAMDRIVE_NOLIBBASE__)
 #ifdef __RAMDRIVE_STDLIBBASE__
  extern struct Library *RamdriveBase;
 #else
  extern struct Device *RamdriveBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(RAMDRIVE_NOLIBDEFINES)
#   include <defines/ramdrive.h>
#endif

#endif /* PROTO_RAMDRIVE_H */
