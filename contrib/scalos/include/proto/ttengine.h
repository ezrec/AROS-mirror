/* Automatically generated header! Do not edit! */

#ifndef PROTO_TTENGINE_H
#define PROTO_TTENGINE_H

//#ifdef __AROS__
//#error You must include AROS headers
//#endif

#include <clib/ttengine_protos.h>

#ifdef __amigaos4__
# include <interfaces/ttengine.h>
#ifdef __USE_INLINE__
	#include <inline4/ttengine.h>
#endif
# ifndef __NOGLOBALIFACE__
   extern struct TTEngineIFace *ITTEngine;
# endif /* __NOGLOBALIFACE__*/
#else /* !__amigaos4__ */

#ifndef _NO_INLINE
# if defined(__GNUC__)
#  ifdef __AROS__
#   include <defines/ttengine.h>
#  elif defined(__PPC__)
#   include <ppcinline/ttengine.h>
#  else
#   include <inline/ttengine.h>
#  endif
# else
#  include <pragmas/ttengine_pragmas.h>
# endif
#endif /* _NO_INLINE */

# ifndef __NOLIBBASE__
   extern struct Library*
#  ifdef __CONSTLIBBASEDECL__
    __CONSTLIBBASEDECL__
#  endif /* __CONSTLIBBASEDECL__ */
   TTEngineBase;
# endif /* !__NOLIBBASE__ */
#endif /* !__amigaos4__ */

#endif /* !PROTO_TTENGINE_H */
