#ifdef __mc68000
#define CPU_TYPE "M68K"
#endif
#ifdef __PPC__
#define CPU_TYPE "PowerPC"
#endif
#ifdef __i386__
#define CPU_TYPE "x86"
#endif
#ifndef CPU_TYPE
#error Unknown CPU, please define
#endif

#ifndef STR
#define _STR(A) #A
#define STR(A) _STR(A)
#endif
#define VERSION         4
#define REVISION        0
#define DATE    "1.1.2006"
#define VERS    "bsdsocket.library 4.0"
#define VSTRING "bsdsocket.library" STR(VERSION) "." STR(REVISION) "(" DATE ")"
#define VERSTAG "\0$VER: bsdsocket.library 4.0 (" DATE ")"
#define STACK_RELEASE "AROSTCP kernel v0.17 " CPU_TYPE " (" DATE ")"

