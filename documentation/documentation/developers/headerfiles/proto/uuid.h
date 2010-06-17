#ifndef PROTO_UUID_H
#define PROTO_UUID_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/uuid/uuid.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/uuid_protos.h>

#if !defined(UUIDBase) && !defined(__NOLIBBASE__) && !defined(__UUID_NOLIBBASE__)
 #ifdef __UUID_STDLIBBASE__
  extern struct Library *UUIDBase;
 #else
  extern struct Library *UUIDBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(UUID_NOLIBDEFINES)
#   include <defines/uuid.h>
#endif

#endif /* PROTO_UUID_H */
