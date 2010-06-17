#ifndef PROTO_DOS_H
#define PROTO_DOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/dos/dos.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/dos_protos.h>

#if !defined(DOSBase) && !defined(__NOLIBBASE__) && !defined(__DOS_NOLIBBASE__)
 #ifdef __DOS_STDLIBBASE__
  extern struct Library *DOSBase;
 #else
  extern struct DosLibrary *DOSBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(DOS_NOLIBDEFINES)
#   include <defines/dos.h>
#endif

#endif /* PROTO_DOS_H */
