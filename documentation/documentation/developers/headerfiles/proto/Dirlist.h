#ifndef PROTO_DIRLIST_H
#define PROTO_DIRLIST_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/dirlist.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Dirlist_protos.h>

#if !defined(DirlistBase) && !defined(__NOLIBBASE__) && !defined(__DIRLIST_NOLIBBASE__)
 #ifdef __DIRLIST_STDLIBBASE__
  extern struct Library *DirlistBase;
 #else
  extern struct Library *DirlistBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(DIRLIST_NOLIBDEFINES)
#   include <defines/Dirlist.h>
#endif

#endif /* PROTO_DIRLIST_H */
