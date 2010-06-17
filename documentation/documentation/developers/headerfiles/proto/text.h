#ifndef PROTO_TEXT_H
#define PROTO_TEXT_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/text/text.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/text_protos.h>

#if !defined(TextBase) && !defined(__NOLIBBASE__) && !defined(__TEXT_NOLIBBASE__)
 #ifdef __TEXT_STDLIBBASE__
  extern struct Library *TextBase;
 #else
  extern struct Library *TextBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(TEXT_NOLIBDEFINES)
#   include <defines/text.h>
#endif

#endif /* PROTO_TEXT_H */
