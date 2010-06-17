#ifndef PROTO_DTPIC_H
#define PROTO_DTPIC_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/dtpic.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Dtpic_protos.h>

#if !defined(DtpicBase) && !defined(__NOLIBBASE__) && !defined(__DTPIC_NOLIBBASE__)
 #ifdef __DTPIC_STDLIBBASE__
  extern struct Library *DtpicBase;
 #else
  extern struct Library *DtpicBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(DTPIC_NOLIBDEFINES)
#   include <defines/Dtpic.h>
#endif

#endif /* PROTO_DTPIC_H */
