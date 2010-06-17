#ifndef PROTO_MATHFFP_H
#define PROTO_MATHFFP_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/mathffp/mathffp.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/mathffp_protos.h>

#if !defined(MathBase) && !defined(__NOLIBBASE__) && !defined(__MATHFFP_NOLIBBASE__)
 #ifdef __MATHFFP_STDLIBBASE__
  extern struct Library *MathBase;
 #else
  extern struct Library *MathBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(MATHFFP_NOLIBDEFINES)
#   include <defines/mathffp.h>
#endif

#endif /* PROTO_MATHFFP_H */
