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
#define REVISION        52
#define DATE    "24.03.2007"
#define VERS    "bsdsocket.library 4.52"
#define VSTRING "bsdsocket.library" STR(VERSION) "." STR(REVISION) "(" DATE ")"
#define VERSTAG "\0$VER: bsdsocket.library 4.52 (" DATE ")"

#define MIAMI_VERSION 13
#define MIAMI_REVISION 4
#define MIAMI_VSTRING  "miami.library" STR(MIAMI_VERSION) "." STR(MIAMI_REVISION) "(" DATE ")"

#define STACK_RELEASE "AROSTCP kernel v0.21 " CPU_TYPE " (" DATE ")"

