#ifndef PROTO_SERSCRIPT_H
#define PROTO_SERSCRIPT_H
/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef __NOLIBBASE__       
extern struct Library *SerScriptBase;
#endif

#include <libraries/serscript.h>
#include <clib/serscript_protos.h>
#if __SASC
#include <pragmas/serscript_pragmas.h>
#elif __GNUC__
#include <inline/serscript.h>
#endif

#endif /* PROTO_SERSCRIPT_H */
