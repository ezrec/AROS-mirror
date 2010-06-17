#ifndef PROTO_KNOB_H
#define PROTO_KNOB_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muimaster/classes/knob.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/Knob_protos.h>

#if !defined(KnobBase) && !defined(__NOLIBBASE__) && !defined(__KNOB_NOLIBBASE__)
 #ifdef __KNOB_STDLIBBASE__
  extern struct Library *KnobBase;
 #else
  extern struct Library *KnobBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(KNOB_NOLIBDEFINES)
#   include <defines/Knob.h>
#endif

#endif /* PROTO_KNOB_H */
