#ifndef PROTO_POPUPMENU_H
#define PROTO_POPUPMENU_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/popupmenu/popupmenu.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/popupmenu_protos.h>

#if !defined(PopupMenuBase) && !defined(__NOLIBBASE__) && !defined(__POPUPMENU_NOLIBBASE__)
 #ifdef __POPUPMENU_STDLIBBASE__
  extern struct Library *PopupMenuBase;
 #else
  extern struct PopupMenuBase *PopupMenuBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(POPUPMENU_NOLIBDEFINES)
#   include <defines/popupmenu.h>
#endif

#endif /* PROTO_POPUPMENU_H */
