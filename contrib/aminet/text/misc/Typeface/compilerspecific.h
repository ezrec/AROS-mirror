#ifndef COMPILERSPECIFIC_H
#define COMPILERSPECIFIC_H

#undef SAVEDS
#undef ASM

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif

#ifdef _AROS

/* AROS */

#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#endif
#ifndef AROS_ASMCALL_H
#   include <aros/asmcall.h>
#endif
#ifndef AROS_MACROS_H
#   include <aros/macros.h>
#endif

#define SAVEDS
#define ASM
#define TF_REGPARAM(reg,type,name) type name
#define LONG2BE(x) AROS_LONG2BE(x)
#define WORD2BE(x) AROS_WORD2BE(x)

#else

/* AmigaOS */

#define LONG2BE(x) x
#define WORD2BE(x) x

#ifdef __GNUC__

/* -> use large data */

#define SAVEDS
#define ASM
#define TF_REGPARAM(reg,type,name) register type name asm(#reg)

#else

/* DICE */

#define SAVEDS __geta4
#define ASM
#define TF_REGPARAM(reg,type,name) __ ## reg type name

#endif

#endif

#endif/ * COMPILERSPECIFIC_H */

