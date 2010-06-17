#ifndef PROTO_DISKFONT_H
#define PROTO_DISKFONT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/diskfont/diskfont.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/diskfont_protos.h>

#if !defined(DiskfontBase) && !defined(__NOLIBBASE__) && !defined(__DISKFONT_NOLIBBASE__)
 #ifdef __DISKFONT_STDLIBBASE__
  extern struct Library *DiskfontBase;
 #else
  extern struct Library *DiskfontBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(DISKFONT_NOLIBDEFINES)
#   include <defines/diskfont.h>
#endif

#endif /* PROTO_DISKFONT_H */
