#ifndef COMPILERSPECIFIC_H
#define COMPILERSPECIFIC_H
/*
**	$Id$
**
**	Copyright (C) 1997 Bernardo Innocenti <bernie@cosmos.it>
**	All rights reserved.
**
**	Defines wrappers for several compiler dependent constructs,
**	including function attributes and register specification for
**	function arguments. Supports SAS/C, gcc, egcs, Storm C, VBCC,
**	Maxon C, DICE and Aztec C.
**
**	You can easily add support for other compilers as well. Please
**	return any changes you make to me, so I can add them to my
**	personal copy of this file.
**
**	Here is a short description of the macros defined below:
**
**	LIBCALL
**		Shared library entry point, with register args
**
**	HOOKCALL
**		Hook or boopsi dispatcher entry point with arguments
**		passed in registers
**
**	GLOBALCALL
**		Attribute for functions to be exported to other modules for
**		global access within the same executable file.
**		Usually defined to "extern", but can be overridden for special
**		needs, such as compiling all modules together in a single
**		object module to optimize code better.
**
**	XDEF
**		Attribute for symbols to be exported to other modules for
**		global access within the same executable file.
**		Usually defined to an empty value.
**
**	XREF
**		Attribute for symbols to be imported from other modules
**		within the same executable file.
**		Usually defined to "extern".
**
**	INLINE
**		Please put function body inline to the calling code
**
**	STDARGS
**		Function uses standard C conventions for arguments
**
**	ASMCALL
**		Function takes arguments in the specified 68K registers
**
**	REGCALL
**		Function takes arguments in registers choosen by the compiler
**
**	CONSTCALL
**		Function does not modify any global variable
**
**	FORMATCALL(archetype,string_index,first_to_check)
**		Function uses printf or scanf-like formatting
**
**	SAVEDS
**		Function needs to reload context for small data model
**
**	INTERRUPT
**		Function will be called from within an interrupt
**
**	NORETURN
**		Function does never return
**
**	ALIGNED
**		Variable must be aligned to longword boundaries
**
**	SECTION_CHIP
**		Variable must be stored in CHIP RAM
**
**	UNUSED(var)
**		Explicitly specify a function parameter as being
**		unused to prevent a compiler warning.
**
**	REG(reg,arg)
**		Put argument <arg> in 68K register <reg>
**
**	min(a,b)
**		Return the minimum between <a> and <b>
**
**	max(a,b)
**		Return the maximum between <a> and <b>
**
**	abs(a)
**		Return the absolute value of <a>
**
**	_COMPILED_WITH
**		A string containing the name of the compiler
*/

#if defined(__GNUC__)
	/* GeekGadgets gcc 2.95.2 or better (works with 2.7.2.1 too) */

	#define INLINE		static inline
	#define STDARGS		__attribute__((stkparm))
	#define ASMCALL		/* nothing */
	#define REGCALL		/* nothing */
	#define CONSTCALL	__attribute__((const))
	#define FORMATCALL(a,s,f)	__attribute__((format(a,s,f)))
	#define SAVEDS		__attribute__((saveds))
	#define INTERRUPT	__attribute__((interrupt))
	#define NORETURN	__attribute__((noreturn))
	#define ALIGNED		__attribute__((aligned(4)))
	#define UNUSED(var)	var __attribute__((unused))
	#if defined(__mc68000__)
	# define SECTION_CHIP	__attribute__((chip))
	# define REG(reg,arg)	arg __asm(#reg)
	#else
	# define SECTION_CHIP	/* nothing */
	# define REG(reg,arg)	arg
	#endif
	#define _COMPILED_WITH	"GCC"
	#define min(a,b)	(((a)<(b))?(a):(b))
	#define max(a,b)	(((a)>(b))?(a):(b))

#elif defined(__SASC)
	/* SAS/C 6.58 or better */

	#define INLINE		static __inline
	#define STDARGS		__stdargs
	#define ASMCALL		__asm
	#define REGCALL		__regcall
	#define CONSTCALL	/* unsupported */
	#define FORMATCALL	/* unsupported */
	#define SAVEDS		__saveds
	#define INTERRUPT	__interrupt
	#define NORETURN	/* unsupported */
	#define ALIGNED		__aligned
	#define SECTION_CHIP	__chip
	#define UNUSED(var)	var /* unsupported */
	#define REG(reg,arg)	register __##reg arg
	#define _COMPILED_WITH	"SAS/C"

	/* For min(), max() and abs() */
	#define USE_BUILTIN_MATH
	#include <string.h>

#elif defined(__STORM__)
	/* StormC 2.00.23 or better */
	#define INLINE		__inline
	#define STDARGS		/* nothing */
	#define ASMCALL		/* nothing */
	#define REGCALL		register
	#define CONSTCALL	/* unsupported */
	#define FORMATCALL	/* unsupported */
	#define SAVEDS		__saveds
	#define INTERRUPT	__interrupt
	#define NORETURN	/* unsupported */
	#define ALIGNED		/* unsupported */
	#define SECTION_CHIP	__chip
	#define UNUSED(var)	var /* unsupported */
	#define REG(reg,arg)	register __##reg arg
	#define _COMPILED_WITH	"StormC"

	#define min(a,b)	(((a)<(b))?(a):(b))
	#define max(a,b)	(((a)>(b))?(a):(b))
	#define abs(a)		(((a)>0)?(a):-(a))

	#define _INLINE_INCLUDES
	#include <string.h>

#elif defined(__VBCC__)
	/* VBCC 0.7 (m68k) or better */

	#define INLINE		static __inline
	#define STDARGS		/* unsupported */
	#define ASMCALL		/* nothing */
	#define REGCALL		/* nothing */
	#define CONSTCALL	/* unsupported */
	#define FORMATCALL	/* unsupported */
	#define SAVEDS		__saveds
	#define INTERRUPT	/* unsupported */
	#define NORETURN	/* unsupported */
	#define ALIGNED		/* unsupported */
	#define SECTION_CHIP	__chip
	#define UNUSED(var)	var /* unsupported */
	#define REG(reg,arg)	__reg(##reg) arg
	#define _COMPILED_WITH	"VBCC"

	#error VBCC compiler support is untested. Please check all the above definitions

#elif defined(__MAXON__)
	/* Maxon C/C++ 3.0 */

	#define INLINE		static inline
	#define STDARGS		/* ? */
	#define ASMCALL		/* ? */
	#define REGCALL		/* ? */
	#define CONSTCALL	/* unsupported */
	#define FORMATCALL	/* unsupported */
	#define SAVEDS		/* unsupported */
	#define INTERRUPT	/* unsupported */
	#define NORETURN	/* unsupported */
	#define ALIGNED		/* unsupported */
	#define UNUSED(var)	var /* unsupported */
	#define REG(reg,arg)	register __##reg arg
	#define _COMPILED_WITH	"Maxon C"

	/* For min(), max() and abs() */
	#define USE_BUILTIN_MATH
	#include <string.h>

	#error Maxon C compiler support is untested. Please check all the above definitions

#elif defined(_DCC)
	/* DICE C 3.15 */

	#define INLINE		static __inline
	#define STDARGS		__stdargs
	#define ASMCALL		/* nothing */
	#define REGCALL		/* ? */
	#define CONSTCALL	/* unsupported */
	#define FORMATCALL	/* unsupported */
	#define SAVEDS		__geta4
	#define INTERRUPT	/* unsupported */
	#define NORETURN	/* unsupported */
	#define ALIGNED		__aligned
	#define UNUSED(var)	var /* unsupported */
	#define REG(reg,arg)	__##reg arg
	#define _COMPILED_WITH	"DICE"

	#define min(a,b)	(((a)<(b))?(a):(b))
	#define max(a,b)	(((a)>(b))?(a):(b))
	#define abs(a)		(((a)>0)?(a):-(a))

	#error DICE compiler support is untested. Please check all the above definitions

#elif defined(AZTEC_C)
	/* Aztec/Manx C */

	#define INLINE		static
	#define STDARGS		/* ? */
	#define ASMCALL		/* ? */
	#define REGCALL		/* ? */
	#define CONSTCALL	/* unsupported */
	#define FORMATCALL	/* unsupported */
	#define SAVEDS		__geta4
	#define INTERRUPT	/* unsupported */
	#define NORETURN	/* unsupported */
	#define ALIGNED		__aligned
	#define UNUSED(var)	var /* unsupported */
	#define REG(reg,arg)	__##reg arg
	#define _COMPILED_WITH	"Manx C"

	#define min(a,b)	(((a)<(b))?(a):(b))
	#define max(a,b)	(((a)>(b))?(a):(b))
	#define abs(a)		(((a)>0)?(a):-(a))

	#error Aztec/Manx C compiler support is untested. Please check all the above definitions
#else
	#error Please add compiler specific definitions for your compiler
#endif


/* CONST_STRPTR is a new typedef provided since the V44 version of
 * <exec/types.h>. Passing "const char *" parameters will only work
 * if the OS protos are using CONST_STRPTR accordingly, otherwise you
 * will get a lot of compiler warnings for const to volatile conversions.
 *
 * Using "const" where it is appropriate helps the compiler optimizing
 * code better, so this mess is probably worth it.
 */
#if (!defined(_AROS)) && (INCLUDE_VERSION < 44)
typedef char *CONST_STRPTR;
#endif


/* Special function attributes */

#define LIBCALL		ASMCALL SAVEDS
#define HOOKCALL	ASMCALL SAVEDS
#ifdef __cplusplus
	#define GLOBALCALL extern "C"
#else
	#define GLOBALCALL
#endif

/* special variable attributes */
#define XDEF
#define XREF extern


/* AROS Compatibility: IPTR is a type which can store a pointer
 * as well as a long integer.
 */
#if !defined(IPTR) && !defined(__typedef_IPTR)
#define IPTR unsigned long
#endif /* IPTR */


#endif /* !COMPILERSPECIFIC_H */
