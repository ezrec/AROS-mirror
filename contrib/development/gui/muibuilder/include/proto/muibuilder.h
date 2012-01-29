#ifndef _PROTO_MUIBUILDER_H
#define _PROTO_MUIBUILDER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_MUIBUILDER_PROTOS_H) && !defined(__GNUC__)
#include <clib/muibuilder_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *MUIBBase;
#endif

#ifdef __GNUC__
#ifdef __AROS__
#include <defines/muibuilder.h>
#else
#include <inline/muibuilder.h>
#endif
#elif !defined(__VBCC__)
#include <pragma/muibuilder_lib.h>
#endif

#endif	/*  _PROTO_MUIBUILDER_H  */
