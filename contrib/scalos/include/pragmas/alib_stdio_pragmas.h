#ifndef PRAGMAS_ALIB_STDIO_PRAGMAS_H
#define PRAGMAS_ALIB_STDIO_PRAGMAS_H

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

#ifdef __CLIB_PRAGMA_LIBCALL
#pragma libcall None fclose 1E 001
#pragma libcall None fgetc 24 001
#pragma libcall None fputc 2A 1002
#pragma libcall None fputs 30 0802
#pragma libcall None getchar 36 0
#pragma libcall None putchar 3C 001
#pragma libcall None puts 42 801
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_ALIB_STDIO_PRAGMAS_H */
