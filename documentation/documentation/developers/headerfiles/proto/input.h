#ifndef PROTO_INPUT_H
#define PROTO_INPUT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/devs/input/input.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/input_protos.h>

#if !defined(InputBase) && !defined(__NOLIBBASE__) && !defined(__INPUT_NOLIBBASE__)
 #ifdef __INPUT_STDLIBBASE__
  extern struct Library *InputBase;
 #else
  extern struct Device *InputBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(INPUT_NOLIBDEFINES)
#   include <defines/input.h>
#endif

#endif /* PROTO_INPUT_H */
