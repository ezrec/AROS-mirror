/* amiga.h
 *
 * Globular definitions that affect all of AmigaDom.
 *
 * Originally included in unzip.h, extracted for simplicity and eeze of
 * maintenance by John Bush.
 *
 * THIS FILE IS #INCLUDE'd by unzip.h
 *
 */

#ifndef __amiga_amiga_h
#define __amiga_amiga_h
#include <time.h>
#include <fcntl.h>
#include <limits.h>

#ifdef AZTEC_C                       /* Manx Aztec C, 5.0 or newer only */
#  include <clib/dos_protos.h>
#  include <pragmas/dos_lib.h>           /* do inline dos.library calls */
#  define O_BINARY 0
#  include "amiga/z-stat.h"   /* substitute for <stat.h> and <direct.h> */
#  define direct dirent

#  define DECLARE_TIMEZONE
#  define ASM_INFLATECODES
#  define ASM_CRC

/* Note that defining REENTRANT will not eliminate all global/static */
/* variables.  The functions we use from c.lib, including stdio, are */
/* not reentrant.  Neither are the stuff in amiga/stat.c or the time */
/* functions in amiga/filedate.c, because they just augment c.lib.   */
/* If you want a fully reentrant and reexecutable "pure" UnZip with  */
/* Aztec C, assemble and link in the startup module purify.a by Paul */
/* Kienitz.  REENTRANT should be used just to reduce memory waste.   */
#endif /* AZTEC_C */


#ifdef __SASC
/* includes */
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/dir.h>
#  include <dos.h>
#  include <exec/memory.h>
#  include <exec/execbase.h>
#  if (defined(_M68020) && (!defined(__USE_SYSBASE)))
                            /* on 68020 or higher processors it is faster   */
#    define __USE_SYSBASE   /* to use the pragma libcall instead of syscall */
#  endif                    /* to access functions of the exec.library      */
#  include <proto/exec.h>   /* see SAS/C manual:part 2,chapter 2,pages 6-7  */
#  include <proto/dos.h>
#  include <proto/locale.h>

#  ifdef DEBUG
#    include <sprof.h>      /* profiler header file */
#  endif

#  if ( (!defined(O_BINARY)) && defined(O_RAW))
#    define O_BINARY O_RAW
#  endif

#  if (defined(_SHORTINT) && !defined(USE_FWRITE))
#    define USE_FWRITE      /* define if write() returns 16-bit int */
#  endif

#  if (!defined(REENTRANT) && !defined(FUNZIP))
#    define REENTRANT      /* define if unzip is going to be pure */
#  endif

#  if defined(REENTRANT) && defined(DYNALLOC_CRCTAB)
#    undef DYNALLOC_CRCTAB
#  endif

#  ifdef MWDEBUG
#    include <stdio.h>      /* both stdio.h and stdlib.h must be included */
#    include <stdlib.h>     /* before memwatch.h                          */
#    include "memwatch.h"
#    undef getenv
#  endif /* MWDEBUG */

#if 0   /* disabled for now - we don't have alloc_remember() yet */
/* SAS/C built-in getenv() calls malloc to store the envvar string.       */
/* Therefore we need to remember the allocation in order to free it later */
#  include <stdlib.h>       /* getenv() prototype in stdlib.h */
#  ifdef getenv
#    undef getenv           /* remove previously preprocessor symbol */
#  endif
#  ifndef MWDEBUG
/* #    define getenv(name)    ((char *)remember_alloc((zvoid *)getenv(name))) */
#  else                     /* MWGetEnv() ripped from memlib's memwatch.h */
#    define getenv(name)    ((char *)remember_alloc((zvoid *)MWGetEnv(name, __FILE__, __LINE__)))
#  endif
#endif /* 0 */

#endif /* SASC */


#define MALLOC_WORK
#define USE_EF_UT_TIME

#define AMIGA_FILENOTELEN 80
#define DATE_FORMAT       DF_MDY
#define lenEOL            1
#define PutNativeEOL      *q++ = native(LF);
#define PIPE_ERROR        0

#ifdef GLOBAL         /* crypt.c usage conflicts with AmigaDOS headers */
#  undef GLOBAL
#endif

/* Funkshine Prough Toe Taipes */

LONG FileDate (char *, time_t[]);
void tzset(void);
int windowheight (BPTR fh);
void _abort(void);              /* ctrl-C trap */

#define SCREENLINES windowheight(Output())


/* Static variables that we have to add to struct Globals: */
#define SYSTEM_SPECIFIC_GLOBALS \
    int N_flag;\
    int filenote_slot;\
    char *(filenotes[DIR_BLKSIZ]);\
    int created_dir, renamed_fullpath, rootlen;\
    char *rootpath, *buildpath, *build_end;\
    DIR *wild_dir;\
    char *dirname, *wildname, matchname[FILNAMSIZ];\
    int dirnamelen, notfirstcall;

/* N_flag, filenotes[], and filenote_slot are for the -N option that      */
/*    restores zipfile comments as AmigaDOS filenotes.  The others        */
/*    are used by functions in amiga/amiga.c only.                        */
/* created_dir and renamed_fullpath are used by mapname() and checkdir(). */
/* rootlen, rootpath, buildpath, and build_end are used by checkdir().    */
/* wild_dir, dirname, wildname, matchname[], dirnamelen and notfirstcall  */
/*    are used by do_wild().                                              */
#endif /* __amiga_amiga_h */
