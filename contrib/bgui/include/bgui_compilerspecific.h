#ifndef BGUI_COMPILERSPECIFIC_H
#define BGUI_COMPILERSPECIFIC_H
/*
**      $Id$
**
**      Copyright (C) 1997 Bernardo Innocenti <bernie@cosmos.it>
**      All rights reserved.
**
**      Defines wrappers for several compiler dependent constructs,
**      including function attributes and register specification for
**      function arguments. Supports SAS/C, gcc, EGCS, Storm C, VBCC,
**      Maxon C, DICE and Aztec C.
**
**      You can easily add support for other compilers as well. Please
**      return any changes you make to me, so I can add them to my
**      personal copy of this file.
**
**      Here is a short description of the macros defined below:
**
**      LIBCALL
**              Shared library entry point, with register args
**
**      HOOKCALL
**              Hook or boopsi dispatcher entry point with arguments
**              passed in registers
**
**      GLOBALCALL
**              Attribute for functions to be exported to other modules for
**              global access within the same executable file.
**              Usually defined to "extern", but can be overridden for special
**              needs, such as compiling all modules together in a single
**              object module to optimize code better.
**
**      XDEF
**              Attribute for symbols to be exported to other modules for
**              global access within the same executable file.
**              Usually defined to an empty value.
**
**      XREF
**              Attribute for symbols to be imported from other modules
**              within the same executable file.
**              Usually defined to "extern".
**
**      INLINE
**              Please put function body inline to the calling code
**
**      STDARGS
**              Function uses standard C conventions for arguments
**
**      ASMCALL
**              Function takes arguments in the specified 68K registers
**
**      REGCALL
**              Function takes arguments in registers choosen by the compiler
**
**      CONSTCALL
**              Function does not modify any global variable
**
**      FORMATCALL(archetype,string_index,first_to_check)
**              Function uses printf or scanf-like formatting
**
**      SAVEDS
**              Function needs to reload context for small data model
**
**      INTERRUPT
**              Function will be called from within an interrupt
**
**      NORETURN
**              Function does never return
**
**      ALIGNED
**              Variable must be aligned to longword boundaries
**
**      UNUSED(var)
**              Eclicitly specify a function parameter as being
**              unused to prevent a compiler warning.
**
**      CHIP
**              Variable must be stored in CHIP RAM
**
**      REG(reg,arg)
**              Put argument <arg> in 68K register <reg>
**
**      min(a,b)
**              Return the minimum between <a> and <b>
**
**      max(a,b)
**              Return the maximum between <a> and <b>
**
**      abs(a)
**              Return the absolute value of <a>
**
**      _COMPILED_WITH
**              A string containing the name of the compiler
*/

#ifdef REG
#undef REG
#endif

#ifdef ASM
#undef ASM
#endif

#ifdef SAVEDS
#undef SAVEDS
#endif

#ifdef __SASC
	/* SAS/C 6.58 or better */

	#define INLINE          static __inline
	#define STDARGS         __stdargs
	#define ASMCALL         __asm
	#define ASM             ASMCALL
	#define REGCALL         __regcall
	#define CONSTCALL       /* unsupported */
	#define FORMATCALL      /* unsupported */
	#define SAVEDS          __saveds
	#define INTERRUPT       __interrupt
	#define NORETURN        /* unsupported */
	#define ALIGNED         __aligned
	#define UNUSED(var)     var /* unsupported */
	#define CHIP            __chip
	#define REG(reg)        register __##reg
	#define _COMPILED_WITH  "SAS/C"

	#define __D0 __d0
	#define __D1 __d1
	#define __D2 __d2
	#define __D3 __d3
	#define __D4 __d4
	#define __D5 __d5
	#define __D6 __d6
	#define __D7 __d7
	#define __A0 __a0
	#define __A1 __a1
	#define __A2 __a2
	#define __A3 __a3
	#define __A4 __a4
	#define __A5 __a5
	#define __A6 __a6
	#define __A7 __a7


	/* For min(), max() and abs() */
	//#define USE_BUILTIN_MATH
	//#include <string.h>
#else
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
	/* GeekGadgets GCC 2.7.2.1 or better */

//      #define INLINE          static inline
//      #define STDARGS         __attribute__((stkparm))
//      #define ASMCALL         /* nothing */
//      #define REGCALL         /* nothing */
//      #define CONSTCALL       __attribute__((const))
//      #define FORMATCALL(a,s,f)       __attribute__((format(a,s,f)))
//      #define SAVEDS          __attribute__((saveds))
#define SAVEDS
//      #define INTERRUPT       __attribute__((interrupt))
//      #define NORETURN        __attribute__((noreturn))
//      #define ALIGNED         __attribute__((aligned(4)))
//      #define UNUSED(var)     var __attribute__((unused))
//      #define REG(reg,arg) arg __asm(#reg)

#ifdef __AROS__

	#define makearosproto

	#define REG(reg)
	#define ASM
	#define __stdargs
	#define __chip
	#define __saveds
#endif
	#define _COMPILED_WITH  "GCC"

//      #define min(a,b)        (((a)<(b))?(a):(b))
//      #define max(a,b)        (((a)>(b))?(a):(b))
#else
#ifdef __STORM__
	/* StormC 2.00.23 or better */
	#define INLINE          __inline
	#define STDARGS         /* nothing */
	#define ASMCALL         /* nothing */
	#define REGCALL         register
	#define CONSTCALL       /* unsupported */
	#define FORMATCALL      /* unsupported */
	#define SAVEDS          __saveds
	#define INTERRUPT       __interrupt
	#define NORETURN        /* unsupported */
	#define ALIGNED         /* unsupported */
	#define UNUSED(var)     var /* unsupported */
	#define CHIP            __chip
	#define REG(reg)        register __##reg
	#define _COMPILED_WITH  "StormC"

	#define min(a,b)        (((a)<(b))?(a):(b))
	#define max(a,b)        (((a)>(b))?(a):(b))
	#define abs(a)          (((a)>0)?(a):-(a))

	#define _INLINE_INCLUDES
	#include <string.h>
#else
#ifdef __VBCC__
	/* VBCC 0.7 (m68k) or better */

	#define INLINE          static __inline
	#define STDARGS         /* unsupported */
	#define ASMCALL         /* nothing */
	#define REGCALL         /* nothing */
	#define CONSTCALL       /* unsupported */
	#define FORMATCALL      /* unsupported */
	#define SAVEDS          __saveds
	#define INTERRUPT       /* unsupported */
	#define NORETURN        /* unsupported */
	#define ALIGNED         /* unsupported */
	#define UNUSED(var)     var /* unsupported */
	#define CHIP            __chip
	#define REG(reg)        __reg(##reg)
	#define _COMPILED_WITH  "VBCC"

	#error VBCC compiler support is untested. Please check all the above definitions
#else
#ifdef __MAXON__
	/* Maxon C/C++ 3.0 */

	#define INLINE          static inline
	#define STDARGS         /* ? */
	#define ASMCALL         /* ? */
	#define REGCALL         /* ? */
	#define CONSTCALL       /* unsupported */
	#define FORMATCALL      /* unsupported */
	#define SAVEDS          /* unsupported */
	#define INTERRUPT       /* unsupported */
	#define NORETURN        /* unsupported */
	#define ALIGNED         /* unsupported */
	#define UNUSED(var)     var /* unsupported */
	#define REG(reg)        register __##reg
	#define _COMPILED_WITH  "Maxon C"

	/* For min(), max() and abs() */
	#define USE_BUILTIN_MATH
	#include <string.h>

	#error Maxon C compiler support is untested. Please check all the above definitions
#else
#ifdef _DCC
	/* DICE C 3.15 */

	#define INLINE          static __inline
	#define STDARGS         __stdargs
	#define ASMCALL         /* nothing */
	#define REGCALL         /* ? */
	#define CONSTCALL       /* unsupported */
	#define FORMATCALL      /* unsupported */
	#define SAVEDS          __geta4
	#define INTERRUPT       /* unsupported */
	#define NORETURN        /* unsupported */
	#define ALIGNED         __aligned
	#define UNUSED(var)     var /* unsupported */
	#define REG(reg)        __##reg
	#define _COMPILED_WITH  "DICE"

	#define min(a,b)        (((a)<(b))?(a):(b))
	#define max(a,b)        (((a)>(b))?(a):(b))
	#define abs(a)          (((a)>0)?(a):-(a))

	#error DICE compiler support is untested. Please check all the above definitions
#else
#ifdef AZTEC_C
	/* Aztec/Manx C */

	#define INLINE          static
	#define STDARGS         /* ? */
	#define ASMCALL         /* ? */
	#define REGCALL         /* ? */
	#define CONSTCALL       /* unsupported */
	#define FORMATCALL      /* unsupported */
	#define SAVEDS          __geta4
	#define INTERRUPT       /* unsupported */
	#define NORETURN        /* unsupported */
	#define ALIGNED         __aligned
	#define UNUSED(var)     var /* unsupported */
	#define REG(reg)    __  ##reg
	#define _COMPILED_WITH  "Manx C"

	#define min(a,b)        (((a)<(b))?(a):(b))
	#define max(a,b)        (((a)>(b))?(a):(b))
	#define abs(a)          (((a)>0)?(a):-(a))

	#error Aztec/Manx C compiler support is untested. Please check all the above definitions
#else
	#error Please add compiler specific definitions for your compiler
#endif
#endif
#endif
#endif
#endif
#endif
#endif


/* CONST_STRPTR is a new typedef provided since the V44 version of
 * <exec/types.h>. Passing "const char *" parameters will only work
 * if the OS protos are using CONST_STRPTR accordingly, otherwise you
 * will get a lot of compiler warnings for const to volatile conversions.
 *
 * Using "const" where it is appropriate helps the compiler optimizing
 * code better, so this mess is probably worth it.
 */
//#if INCLUDE_VERSION < 44
//typedef char *CONST_STRPTR;
//#endif


/* Special function attributes */

#define LIBCALL         ASMCALL SAVEDS
#define HOOKCALL        ASMCALL SAVEDS
#ifdef __cplusplus
	#define GLOBALCALL extern "C"
#else
	#define GLOBALCALL
#endif

/* special variable attributes */

#define XDEF
#define XREF extern

/*** AROS/AmigaOS compatability *********************************/

#ifdef __AROS__
#define REG_A4	4
static inline IPTR getreg(int reg) { return 0xDEADCAFE; }
static inline VOID putreg(int reg, IPTR val) { }
static inline VOID geta4(void) { }
#else
typedef IPTR ULONG
#define BNULL 0
#endif

/*** Disable these macros when not building on AROS *****************/

#ifndef __AROS__
#define AROS_LIBFUNC_INIT
#define AROS_LIBFUNC_EXIT
#endif

/*** METHOD macro ***************************************************/

#ifdef __AROS__

  #define METHOD(f,mtype,m) ASM AROS_UFH4(IPTR, f, \
			  AROS_UFHA(Class *, cl, A0), \
			  AROS_UFHA(Object *, obj, A2), \
			  AROS_UFHA(Msg, _msg, A1), \
			  AROS_UFHA(APTR, _global, A4)) \
			       {AROS_USERFUNC_INIT \
			        mtype __attribute__((unused)) m = (mtype)_msg;
  #define METHODPROTO(f,mtype,m) ASM AROS_UFP4(IPTR, f, \
                          AROS_UFPA(Class *, cl, A0), \
			  AROS_UFPA(Object *, obj, A2), \
			  AROS_UFPA(Msg, msg, A1), \
			  AROS_UFPA(APTR, _global, A4));
 
  #define METHOD_END AROS_USERFUNC_EXIT}
  #define METHOD_CALL(f,cl,obj,m,g) \
    	AROS_UFC4(IPTR, f, \
    		AROS_UFCA(Class *, cl, A0), \
    		AROS_UFCA(Object *, obj, A2), \
    		AROS_UFCA(Msg, m, A1), \
    		AROS_UFCA(APTR, g, A4))

#else
  #define METHOD(f,mtype,m) SAVEDS ASM IPTR f(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg _msg, REG(a4) APTR _global) \
	{ mtype __attribute__((unused)) m = (mtype)_msg;
  #define METHODPROTO(f,mtype,m) SAVEDS ASM IPTR f(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg m, REG(a4) APTR _global)
  #define METHOD_END }
  #define METHOD_CALL(f,cl,obj,m,g) f(cl, obj, (Msg)m, (APTR)g)
#endif

/*** REGFUNC and REGPARAM macros ************************************/

#ifdef __AROS__

  #ifndef AROS_ASMCALL_H
  #include <aros/asmcall.h>
  #endif

  #define ASM
  #define SAVEDS

  #ifndef REGPARAM
    #define REGPARAM(reg,type,name) type,name,reg

    #define REGFUNC2(r,n,a1,a2) AROS_UFH2(r,n,AROS_UFHA(a1),AROS_UFHA(a2)) {AROS_USERFUNC_INIT
    #define REGFUNC3(r,n,a1,a2,a3) AROS_UFH3(r,n,AROS_UFHA(a1),AROS_UFHA(a2),AROS_UFHA(a3)) {AROS_USERFUNC_INIT

    #define REGCALL2(r,n,a1,a2) AROS_UFC2(r,n,AROS_UFCA(a1),AROS_UFCA(a2))
    #define REGCALL3(r,n,a1,a2,a3) AROS_UFC3(r,n,AROS_UFCA(a1),AROS_UFCA(a2),AROS_UFCA(a3))

    #define REGFUNCPROTO2(r,n,a1,a2) AROS_UFP2(r,n,AROS_UFHA(a1),AROS_UFHA(a2))
    #define REGFUNCPROTO3(r,n,a1,a2,a3) AROS_UFP3(r,n,AROS_UFHA(a1),AROS_UFHA(a2),AROS_UFHA(a3))

    #define REGFUNC_END AROS_USERFUNC_EXIT}
  #endif

#else

  #ifndef REGPARAM
    #define REGPARAM(reg,type,name) REG(reg) type name
    #define REGVALUE(reg,type,name) name

    #define REGFUNC2(r,n,a1,a2) r n(a1,a2)
    #define REGFUNC3(r,n,a1,a2,a3) r n(a1,a2,a3)

    #define REGCALL2(r,n,a1,a2) \
    	r n(a1,a2)
    #define REGCALL3(r,n,a1,a2,a3) \
    	r n(a1,a2,a3)

    #define REGFUNCPROTO2(r,n,a1,a2) r n(a1,a2)
    #define REGFUNCPROTO3(r,n,a1,a2,a3) r n(a1,a2,a3)

    #define REGFUNC_END
  #endif

#endif

#endif /* !BGUI_COMPILERSPECIFIC_H */
