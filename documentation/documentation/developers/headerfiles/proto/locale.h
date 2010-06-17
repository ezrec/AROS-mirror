#ifndef PROTO_LOCALE_H
#define PROTO_LOCALE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/locale/locale.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/locale_protos.h>

#if !defined(LocaleBase) && !defined(__NOLIBBASE__) && !defined(__LOCALE_NOLIBBASE__)
 #ifdef __LOCALE_STDLIBBASE__
  extern struct Library *LocaleBase;
 #else
  extern struct LocaleBase *LocaleBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(LOCALE_NOLIBDEFINES)
#   include <defines/locale.h>
#endif

#endif /* PROTO_LOCALE_H */
