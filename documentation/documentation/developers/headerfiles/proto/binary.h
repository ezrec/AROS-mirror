#ifndef PROTO_BINARY_H
#define PROTO_BINARY_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/binary/binary.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/binary_protos.h>

#if !defined(BinaryBase) && !defined(__NOLIBBASE__) && !defined(__BINARY_NOLIBBASE__)
 #ifdef __BINARY_STDLIBBASE__
  extern struct Library *BinaryBase;
 #else
  extern struct Library *BinaryBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(BINARY_NOLIBDEFINES)
#   include <defines/binary.h>
#endif

#endif /* PROTO_BINARY_H */
