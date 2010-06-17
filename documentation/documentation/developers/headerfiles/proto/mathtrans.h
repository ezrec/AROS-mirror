#ifndef PROTO_MATHTRANS_H
#define PROTO_MATHTRANS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mathtrans/mathtrans.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/mathtrans_protos.h>

#if !defined(MathTransBase) && !defined(__NOLIBBASE__) && !defined(__MATHTRANS_NOLIBBASE__)
 #ifdef __MATHTRANS_STDLIBBASE__
  extern struct Library *MathTransBase;
 #else
  extern struct Library *MathTransBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(MATHTRANS_NOLIBDEFINES)
#   include <defines/mathtrans.h>
#endif

#endif /* PROTO_MATHTRANS_H */
