#ifndef PROTO_HTML_H
#define PROTO_HTML_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/datatypes/html/html.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <exec/types.h>
#include <aros/system.h>

#include <clib/html_protos.h>

#if !defined(HtmlBase) && !defined(__NOLIBBASE__) && !defined(__HTML_NOLIBBASE__)
 #ifdef __HTML_STDLIBBASE__
  extern struct Library *HtmlBase;
 #else
  extern struct Library *HtmlBase;
 #endif
#endif

#if !defined(NOLIBDEFINES) && !defined(HTML_NOLIBDEFINES)
#   include <defines/html.h>
#endif

#endif /* PROTO_HTML_H */
