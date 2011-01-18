#ifndef BGUI_PROTO_H
/*
 * @(#) $Header$
 *
 * $VER: proto/bgui.h 41.10 (20.1.97)
 * bgui.library prototypes. For use with 32 bit integers only.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:23:27  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:37:14  mlemos
 * Bumped to revision 41.11
 *
 * Revision 41.1  2000/05/09 20:02:00  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/02/18 23:19:28  mlemos
 * Added support for GCC.
 *
 * Revision 1.1.2.1  1998/02/26 18:39:11  mlemos
 * Added missing proto/bgui.h header
 *
 *
 */
#define BGUI_PROTO_H
#ifdef _DCC
#include <pragmas/config.h>
#else
#define __SUPPORTS_PRAGMAS__ 1
#endif
#include <exec/types.h>
#include <clib/bgui_protos.h>
#ifdef __GNUC__
#include <inline/bgui.h>
#ifndef __NOLIBBASE__
extern struct Library *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif
BGUIBase;
#endif
#else
#ifdef __SUPPORTS_PRAGMAS__
extern struct Library *BGUIBase;
#include <pragmas/bgui_pragmas.h>
#endif
#endif
#endif
