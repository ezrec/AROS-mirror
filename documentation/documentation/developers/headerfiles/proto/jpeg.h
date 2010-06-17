#ifndef PROTO_JPEG_H
#define PROTO_JPEG_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/jpeg/jpeg.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/jpeg_protos.h>

#if !defined(JPEGBase) && !defined(__NOLIBBASE__) && !defined(__JPEG_NOLIBBASE__)
 #ifdef __JPEG_STDLIBBASE__
  extern struct Library *JPEGBase;
 #else
  extern struct Library *JPEGBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(JPEG_NOLIBDEFINES)
#   include <defines/jpeg.h>
#endif

#endif /* PROTO_JPEG_H */
