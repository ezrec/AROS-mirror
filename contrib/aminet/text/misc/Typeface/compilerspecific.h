#ifndef COMPILERSPECIFIC_H
#define COMPILERSPECIFIC_H

#undef SAVEDS
#undef ASM

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif

#include <SDI/SDI_compiler.h>
#include <SDI/SDI_hook.h>

#ifdef __AROS__

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

#define LONG2BE(x) AROS_LONG2BE(x)
#define WORD2BE(x) AROS_WORD2BE(x)
#define BE2WORD(x) AROS_BE2WORD(x)

#else

/* AmigaOS */

#define LONG2BE(x) x
#define WORD2BE(x) x
#define BE2WORD(x) x

#ifdef __GNUC__

/* -> use large data */

#define SAVEDS
#define ASM

#else

/* DICE */

#define SAVEDS __geta4
#define ASM

#endif

#endif

#endif /* COMPILERSPECIFIC_H */

