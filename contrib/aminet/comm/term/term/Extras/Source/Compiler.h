/*
**	Compiler.h
**
**	Compiler specific definitions
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _COMPILER_H
#define _COMPILER_H 1

	/* SAS/C */

#ifdef __SASC
#define SAVE_DS		__saveds
#define FAR		__far
#define STACKARGS	__stdargs
#define ASM		__asm
#define REG(x)		register __##x
#endif	/* __SASC */

	/* DICE */

#ifdef _DCC
#define NO_PRAGMAS
#define SAVE_DS
#define FAR
#define STACKARGS	__stkargs
#define ASM
#define REG(x)		register __##x
#endif	/* _DCC */

	/* GCC */

#ifdef __GNUC__
#define NO_PRAGMAS
#define USE_GLUE
#define SAVE_DS
#define FAR
#define STACKARGS
#define ASM
#define REG(x)
#endif	/* _DCC */

	/* StormC */

#ifdef __STORM__
#define NO_PRAGMAS
#define USE_GLUE
#define SAVE_DS
#define FAR
#define STACKARGS
#define ASM
#define REG(x)
#endif	/* __STORM__ */

#ifdef __AROS__
#ifdef USE_GLUE
# undef USE_GLUE
#endif
#include <aros/asmcall.h>
#endif

	/* Should we use glue code, i.e. no direct assembly interface routines? */

#ifdef USE_GLUE

#ifdef ASM
#undef ASM
#endif	/* ASM */

#ifdef REG
#undef REG
#endif	/* REG */

#define ASM	/* nothing */
#define REG(x)	/* nothing */

#endif	/* USE_GLUE */

#endif	/* _COMPILER_H */
