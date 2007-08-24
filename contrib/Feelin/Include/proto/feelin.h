#ifndef _PROTO_FEELIN_H
#define _PROTO_FEELIN_H

/*
**    $VER: <proto/feelin.h> 8.0 (2004/12/18)
**
**    2001-2004 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef FEELIN_TYPES_H
#include <feelin/types.h>
#endif
#ifndef CLIB_FEELIN_PROTOS_H
#include <clib/feelin_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct FeelinBase *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif /* __CONSTLIBBASEDECL__ */
FeelinBase;
#endif

#ifdef __GNUC__

    #ifdef __AROS__
    
    	#ifndef NOLIBDEFINES
	#include <defines/feelin.h>
	#endif
	
    #elif defined(__PPC__)

        #ifndef _NO_PPCINLINE
        #include <ppcinline/feelin.h>
        #endif /* !_NO_PPCINLINE */

    #else

        #ifndef _NO_INLINE
        #include <inline/feelin.h>
        #endif /* !_NO_INLINE */

    #endif /* __PPC__ */

#elif defined(__VBCC__)

    #include <inline/feelin_protos.h>

#else

    #include <pragma/feelin_lib.h>

#endif /* __GNUC__ */ 

#endif /* !_PROTO_FEELIN_H */
