/* vms configuration - it was easier to separate it from configur.h */
#if !defined(_VMS_H_INCLUDED)
#  define _VMS_H_INCLUDED

#  if defined(__vms)
#    include <unixlib.h>
#    define fork() vfork()      /* necessary */
#    include <processes.h>      /* should pull in vfork() */
#    if defined(__DECC)
/* #      define HAVE_ALLOCA_H */
/* #      define HAVE_MALLOC_H */
#      define HAVE_ASSERT_H
#      define HAVE_SETJMP_H
#      define HAVE_UNISTD_H
#      define HAVE_LIMITS_H
/* #      define HAVE_FULLPATH */
#      if __CRTL_VER >= 70000000
#        include <fcntl.h>      /* necessary for posix_do_command */
#        include <sys/utsname.h>
#        define HAVE_PUTENV
#        define HAVE_FTRUNCATE
#        define HAVE_RANDOM
#        define HAVE_USLEEP
#        if !defined _VMS_V6_SOURCE
#          define HAVE_GETTIMEOFDAY
#        endif
#      else
#        include "utsname.h"    /* for unxfuncs */
#      endif
#      define HAVE_FTIME
#      define HAVE_MEMCPY
#      define HAVE_MEMMOVE
#      define HAVE_STRERROR
#      define HAVE_VFPRINTF
#      define TIME_WITH_SYS_TIME
#    endif
#    ifndef _MAX_PATH
#      define _MAX_PATH PATH_MAX
#    endif
#    ifdef DYNAMIC
#      define DYNAMIC_VMS
#    endif
#  endif

#endif
