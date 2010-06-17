#ifndef PROTO_ASCII_H
#define PROTO_ASCII_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/ascii/ascii.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/ascii_protos.h>

#if !defined(AsciiBase) && !defined(__NOLIBBASE__) && !defined(__ASCII_NOLIBBASE__)
 #ifdef __ASCII_STDLIBBASE__
  extern struct Library *AsciiBase;
 #else
  extern struct Library *AsciiBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(ASCII_NOLIBDEFINES)
#   include <defines/ascii.h>
#endif

#endif /* PROTO_ASCII_H */
