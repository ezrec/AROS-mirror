#ifndef PROTO_DATATYPES_H
#define PROTO_DATATYPES_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/datatypes/datatypes.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/datatypes_protos.h>

#if !defined(DataTypesBase) && !defined(__NOLIBBASE__) && !defined(__DATATYPES_NOLIBBASE__)
 #ifdef __DATATYPES_STDLIBBASE__
  extern struct Library *DataTypesBase;
 #else
  extern struct Library *DataTypesBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(DATATYPES_NOLIBDEFINES)
#   include <defines/datatypes.h>
#endif

#endif /* PROTO_DATATYPES_H */
