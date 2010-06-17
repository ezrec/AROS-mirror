#ifndef PROTO_OSS_H
#define PROTO_OSS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/libs/oss/oss.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/oss_protos.h>

#if !defined(OSSBase) && !defined(__NOLIBBASE__) && !defined(__OSS_NOLIBBASE__)
 #ifdef __OSS_STDLIBBASE__
  extern struct Library *OSSBase;
 #else
  extern struct Library *OSSBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(OSS_NOLIBDEFINES)
#   include <defines/oss.h>
#endif

#endif /* PROTO_OSS_H */
