#ifndef PROTO_BULLET_H
#define PROTO_BULLET_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/bullet/bullet.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/bullet_protos.h>

#if !defined(BulletBase) && !defined(__NOLIBBASE__) && !defined(__BULLET_NOLIBBASE__)
 #ifdef __BULLET_STDLIBBASE__
  extern struct Library *BulletBase;
 #else
  extern struct Library *BulletBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(BULLET_NOLIBDEFINES)
#   include <defines/bullet.h>
#endif

#endif /* PROTO_BULLET_H */
