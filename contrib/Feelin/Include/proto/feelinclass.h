#ifndef _PROTO_FEELINCLASS_H
#define _PROTO_FEELINCLASS_H

/*
**    $VER: <proto/feelinclass.h> 8.0 (2004/12/18)
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
#include <clib/feelinclass_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct Library *
#ifdef __CONSTLIBBASEDECL__
__CONSTLIBBASEDECL__
#endif /* __CONSTLIBBASEDECL__ */
FeelinClassBase;
#endif

#ifdef __GNUC__
    #ifdef __AROS__
    
    	#ifndef NOLIBDEFINS
	#include <defines/feelinclass.h>
	#endif
	
    #elif defined(__PPC__)
    
    	#ifndef _NO_PPCINLINE
    	#include <ppcinline/feelinclass.h>
    	#endif /* !_NO_PPCINLINE */
	
    #else
    
    	#ifndef _NO_INLINE
    	#include <inline/feelinclass.h>
    	#endif /* !_NO_INLINE */
	
    #endif /* __PPC__ */

#elif defined(__VBCC__)

    #include <inline/feelinclass_protos.h>

#else

    #include <pragma/feelinclass_lib.h>

#endif /* __GNUC__ */

#endif /* _PROTO_FEELINCLASS_H */
