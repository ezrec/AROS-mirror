#ifndef _AMIGA_MACROS_H
#define _AMIGA_MACROS_H

/*
 *  amiga_macros.h - small macros for compiler specific stuff
 *  This file is public domain.
 */

#include <exec/types.h>

/*
 * macros for function definitions and declarations
 */

#ifdef __GNUC__
#define REG(xn, parm) parm __asm(#xn)
#define REGARGS __regargs
#define STDARGS __stdargs
#define SAVEDS __saveds
#define ALIGNED __attribute__ ((aligned(4))
#define FAR
#define CHIP
#define INLINE __inline__
#else /* of __GNUC__ */

#ifdef __SASC
#define REG(xn, parm) register __ ## xn parm
#define REGARGS __asm
#define SAVEDS __saveds
#define ALIGNED __aligned
#define STDARGS __stdargs
#define FAR 
#define CHIP __chip
#define INLINE __inline
#else /* of __SASC */

#ifdef _DCC
#define REG(xn, parm) __ ## xn parm
#define REGARGS
#define SAVEDS __geta4
#define FAR 
#define CHIP __chip
#define INLINE
#endif /* _DCC */

#endif /* __SASC */

#endif /* __GNUC__ */


#endif /* _AMIGA_MACROS_H */
