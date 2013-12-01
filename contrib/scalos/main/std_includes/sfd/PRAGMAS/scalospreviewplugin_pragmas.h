#ifndef PRAGMAS_SCALOSPREVIEWPLUGIN_PRAGMAS_H
#define PRAGMAS_SCALOSPREVIEWPLUGIN_PRAGMAS_H

/*
**	$Id$
**
**	Direct ROM interface (pragma) definitions.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#ifndef __CLIB_PRAGMA_LIBCALL
#define __CLIB_PRAGMA_LIBCALL
#endif /* __CLIB_PRAGMA_LIBCALL */
#else /* __MAXON__, __STORM__ or AZTEC_C */
#ifndef __CLIB_PRAGMA_AMICALL
#define __CLIB_PRAGMA_AMICALL
#endif /* __CLIB_PRAGMA_AMICALL */
#endif /* */

#if defined(__SASC) || defined(__STORM__)
#ifndef __CLIB_PRAGMA_TAGCALL
#define __CLIB_PRAGMA_TAGCALL
#endif /* __CLIB_PRAGMA_TAGCALL */
#endif /* __MAXON__, __STORM__ or AZTEC_C */

#ifndef CLIB_SCALOSPREVIEWPLUGIN_PROTOS_H
#include <clib/scalospreviewplugin_protos.h>
#endif /* CLIB_SCALOSPREVIEWPLUGIN_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosPreviewPluginBase SCAPreviewGenerate 1e BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosPreviewPluginBase SCAPreviewGenerateTags 1e BA9804
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosPreviewPluginBase, 0x1e, SCAPreviewGenerateTags(a0,a1,a2,a3))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */

#endif /* PRAGMAS_SCALOSPREVIEWPLUGIN_PRAGMAS_H */
