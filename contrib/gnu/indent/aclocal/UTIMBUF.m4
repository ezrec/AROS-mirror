dnl
dnl Macro: indent_UTIMBUF
dnl
dnl   Check if we have `struct utimbuf'.
dnl
AC_DEFUN(indent_UTIMBUF,
[dnl Do we have a working utime.h?
AC_CACHE_CHECK([if struct utimbuf needs -posix], 
unet_cv_sys_utimbuf_needs_posix,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <utime.h>],
[struct utimbuf buf;], unet_cv_sys_utimbuf_needs_posix=no,
[OLDCFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -posix"
AC_TRY_COMPILE([#include <sys/types.h>
#include <utime.h>],
[struct utimbuf buf;], unet_cv_sys_utimbuf_needs_posix=yes,
unet_cv_sys_utimbuf_needs_posix=no)
CFLAGS="$OLDCFLAGS"
])])
if test $unet_cv_sys_utimbuf_needs_posix = yes; then
  CFLAGS="$CFLAGS -posix"
fi])
