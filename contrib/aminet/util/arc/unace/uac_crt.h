#ifndef __uac_crt_h
#define __uac_crt_h


#include "acestruc.h"

CHAR *ace_fname(CHAR * s, thead * head, INT nopath);
INT  create_dest_file(CHAR * file, INT a);

#ifdef UNIX
  #define mkdir(_a) mkdir(_a,  S_IRWXU | S_IRWXG | S_IRWXO)
#endif


#ifndef _A_SUBDIR
  #define _A_SUBDIR 0x10        /* MS-DOS directory constant */
#endif

#ifndef S_IEXEC
  #ifdef S_IEXECUTE
    #define S_IEXEC S_IEXECUTE
  #else
    #define S_IEXEC 0
  #endif
#endif

#ifndef S_IDELETE
  #define S_IDELETE 0
#endif

#ifndef S_IRGRP 
  #define S_IRGRP 0
  #define S_IWGRP 0
#endif

#ifndef S_IROTH 
  #define S_IROTH 0
  #define S_IWOTH 0
#endif

#ifndef O_BINARY
  #define O_BINARY 0
#endif

#endif /* __uac_crt_h */

