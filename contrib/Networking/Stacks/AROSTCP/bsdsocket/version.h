#ifndef STR
#define _STR(A) #A
#define STR(A) _STR(A)
#endif
#define VERSION         3
#define REVISION        2
#define DATE    "18.1.2005"
#define VERS    "bsdsocket.library 3.2"
/*#define VSTRING "bsdsocket.library 3.2 (" DATE ")"*/
#define VSTRING "bsdsocket.library" STR(VERSION) "." STR(REVISION) "(" DATE ")"
#define VERSTAG "\0$VER: bsdsocket.library 3.2 (" DATE ")"
