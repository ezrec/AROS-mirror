#ifndef PRAGMAS_SCALOSPREFSPLUGIN_PRAGMAS_H
#define PRAGMAS_SCALOSPREFSPLUGIN_PRAGMAS_H

/*
**	$Id$
**
**	Direct ROM interface (pragma) definitions.
**
**	©1999-2002 The Scalos Team
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

#ifndef CLIB_SCALOSPREFSPLUGIN_PROTOS_H
#include <clib/scalosprefsplugin_protos.h>
#endif /* CLIB_SCALOSPREFSPLUGIN_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosPrefsPluginBase SCAGetPrefsInfo 1e 001
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_SCALOSPREFSPLUGIN_PRAGMAS_H */
