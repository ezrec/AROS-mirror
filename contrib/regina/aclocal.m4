dnl ---------------------------------------------------------------------------
dnl Check if C compiler supports -c -o file.ooo
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_CC_O],
[
AC_MSG_CHECKING(whether $CC understand -c and -o together)
set dummy $CC; ac_cc="`echo [$]2 |
changequote(, )dnl
                       sed -e 's/[^a-zA-Z0-9_]/_/g' -e 's/^[0-9]/_/'`"
changequote([, ])dnl
AC_CACHE_VAL(ac_cv_prog_cc_${ac_cc}_c_o,
[echo 'foo(){}' > conftest.c
# We do the test twice because some compilers refuse to overwrite an
# existing .o file with -o, though they will create one.
eval ac_cv_prog_cc_${ac_cc}_c_o=no
ac_try='${CC-cc} -c conftest.c -o conftest.ooo 1>&AC_FD_CC'
if AC_TRY_EVAL(ac_try) && test -f conftest.ooo && AC_TRY_EVAL(ac_try);
then
  ac_try='${CC-cc} -c conftest.c -o conftest.ooo 1>&AC_FD_CC'
  if AC_TRY_EVAL(ac_try) && test -f conftest.ooo && AC_TRY_EVAL(ac_try);
  then
    eval ac_cv_prog_cc_${ac_cc}_c_o=yes
  fi
fi
rm -f conftest*
])dnl
if eval "test \"`echo '$ac_cv_prog_cc_'${ac_cc}_c_o`\" = yes"; then
        O2SHO=""
        O2SAVE=""
        SAVE2O=""
        CC2O="-o $"'@'
        AC_MSG_RESULT(yes)
else
        O2SHO="-mv \`basename "$'@'" .sho\`.o "$'@'
        O2SAVE="-mv \`basename "$'@'" .sho\`.o \`basename "$'@'" .sho\`.o.save"
        SAVE2O="-mv \`basename "$'@'" .sho\`.o.save \`basename "$'@'" .sho\`.o"
        CC2O=""
        AC_MSG_RESULT(no)
fi
])

dnl ---------------------------------------------------------------------------
dnl Check if __sighandler_t is defined
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK__SIGHANDLER_T],
[
AC_MSG_CHECKING(whether __sighandler_t is defined)

AC_CACHE_VAL(
[mh_cv__sighandler_t],
[
AC_TRY_COMPILE(
[#include <sys/types.h>]
[#include <signal.h>],
[__sighandler_t fred],
[mh_cv__sighandler_t=yes],
[mh_cv__sighandler_t=no]
)
])dnl
AC_MSG_RESULT($mh_cv__sighandler_t)
if test "$mh_cv__sighandler_t" = yes ; then
        AC_DEFINE(HAVE__SIGHANDLER_T)
fi
])


dnl ---------------------------------------------------------------------------
dnl Work out if struct random_data is defined
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_STRUCT_RANDOM_DATA,
[AC_CACHE_CHECK([for struct random_data], mh_cv_struct_random_data,
[AC_TRY_COMPILE([#include <stdlib.h>
#include <stdio.h>],
[struct random_data s;],
mh_cv_struct_random_data=yes, mh_cv_struct_random_data=no)])
if test $mh_cv_struct_random_data = yes; then
  AC_DEFINE(HAVE_RANDOM_DATA)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if getgrgid_r returns an int and has 5 parameters
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_GETGRGID_R_INT_RETURN_5_PARMS,
[AC_CACHE_CHECK([if getgrgid_r has 5 args and returns an int], mh_cv_getgrgid_r_int5,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <grp.h>
#include <pwd.h>],
[
struct group *ptr,result;
int rc;
gid_t gid;
char buf[100];
rc = getgrgid_r(gid,&result,buf,sizeof(buf),&ptr);
],
mh_cv_getgrgid_r_int5=yes, mh_cv_getgrgid_r_int5=no)])
if test $mh_cv_getgrgid_r_int5 = yes; then
  AC_DEFINE(HAVE_GETGRGID_R_RETURNS_INT_5_PARAMS)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if getgrgid_r returns an int and has 4 parameters
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_GETGRGID_R_INT_RETURN_4_PARMS,
[AC_CACHE_CHECK([if getgrgid_r has 4 args and returns an int], mh_cv_getgrgid_r_int4,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <grp.h>
#include <pwd.h>],
[
struct group *ptr,result;
int rc;
gid_t gid;
char buf[100];
rc = getgrgid_r(gid,&result,buf,sizeof(buf));
],
mh_cv_getgrgid_r_int4=yes, mh_cv_getgrgid_r_int4=no)])
if test $mh_cv_getgrgid_r_int4 = yes; then
  AC_DEFINE(HAVE_GETGRGID_R_RETURNS_INT_4_PARAMS)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if getgrgid_r returns a struct group *
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_GETGRGID_R_STRUCT_RETURN,
[AC_CACHE_CHECK([if getgrgid_r returns a struct group *], mh_cv_getgrgid_r_struct,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <grp.h>
#include <pwd.h>],
[
struct group *ptr,result;
int rc;
gid_t gid;
char buf[100];
ptr = getgrgid_r(gid,&result,buf,sizeof(buf));
],
mh_cv_getgrgid_r_struct=yes, mh_cv_getgrgid_r_struct=no)])
if test $mh_cv_getgrgid_r_struct = yes; then
  AC_DEFINE(HAVE_GETGRGID_R_RETURNS_STRUCT)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if getpwuid_r returns an int
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_GETPWUID_R_INT_RETURN,
[AC_CACHE_CHECK([if getpwuid_r returns an int], mh_cv_getpwuid_r_int,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <grp.h>
#include <pwd.h>],
[
struct passwd *ptr,*result;
int rc;
uid_t uid;
char buf[100];
rc = getpwuid_r(uid,&result,buf,sizeof(buf),&ptr);
],
mh_cv_getpwuid_r_int=yes, mh_cv_getpwuid_r_int=no)])
if test $mh_cv_getpwuid_r_int = yes; then
  AC_DEFINE(HAVE_GETPWUID_R_RETURNS_INT)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if getpwuid_r returns a struct group *
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_GETPWUID_R_STRUCT_RETURN,
[AC_CACHE_CHECK([if getpwuid_r returns a struct passwd *], mh_cv_getpwuid_r_struct,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <grp.h>
#include <pwd.h>],
[
struct group *ptr,*result;
int rc;
uid_t uid;
char buf[100];
ptr = getpwuid_r(uid,&result,buf,sizeof(buf));
],
mh_cv_getpwuid_r_struct=yes, mh_cv_getpwuid_r_struct=no)])
if test $mh_cv_getpwuid_r_struct = yes; then
  AC_DEFINE(HAVE_GETPWUID_R_RETURNS_STRUCT)
fi
])

dnl ---------------------------------------------------------------------------
dnl Work out if POSIX Threads are supported
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_POSIX_THREADS],
[
if test "$enable_posix_threads" = yes; then
   AC_MSG_CHECKING(whether Posix Threads are supported)
   tmpLIBS=$LIBS
   save_cflags="$CFLAGS"
   pthread_libs="pthreads pthread"
   for mh_lib in $pthread_libs; do
      LIBS="$tmpLIBS -l${mh_lib}"
      AC_TRY_LINK(
      [#include <pthread.h>],
      [pthread_create(NULL,NULL,NULL,NULL)],
      [mh_has_pthreads=yes],
      [mh_has_pthreads=no]
      )
      if test "$mh_has_pthreads" = yes; then
         MH_MT_LIBS="-l${mh_lib}"
         AC_REQUIRE([AC_CANONICAL_SYSTEM])
         THREADING_COMPILE="-D_REENTRANT -DPOSIX"
         THREADING_LINK=""
         SHL_SCRIPT=""
         case "$target" in
            *solaris*)
               if test "$ac_cv_prog_CC" = "gcc"; then
                  THREADING_COMPILE="-D_REENTRANT -DPOSIX"
               else
                  THREADING_COMPILE="-mt -D_REENTRANT -DREENTRANT -D_PTHREAD_SEMANTICS"
                  THREADING_LINK="-mt"
               fi
               ;;
            *linux*)
               SHL_SCRIPT="-Wl,--version-script=${srcdir}/regina_elf_MT.def"
               ;;
         esac
         MT_FILE="mt_posix"
      else
         MH_MT_LIBS=""
         THREADING_COMPILE=""
         THREADING_LINK=""
         SHL_SCRIPT=""
         MT_FILE="mt_notmt"
         case "$target" in
            *linux*)
               SHL_SCRIPT="-Wl,--version-script=${srcdir}/regina_elf.def"
               ;;
         esac
      fi
      LIBS="$tmpLIBS"
      CFLAGS="$save_cflags"
      if test "$mh_has_pthreads" = yes; then
         break
      fi
   done
   AC_MSG_RESULT($mh_has_pthreads)
else
   MH_MT_LIBS=""
   THREADING=""
   MT_FILE="mt_notmt"
   SHL_SCRIPT=""
   case "$target" in
      *linux*)
         SHL_SCRIPT="-Wl,--version-script=${srcdir}/regina_elf.def"
         ;;
   esac
fi
AC_SUBST(THREADING_COMPILE)
AC_SUBST(THREADING_LINK)
AC_SUBST(MT_FILE)
AC_SUBST(MH_MT_LIBS)
AC_SUBST(SHL_SCRIPT)
])

dnl ---------------------------------------------------------------------------
dnl Work out how to create a dynamically loaded module
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_HOWTO_DYN_LINK],
[
mh_compile='${CC-cc} -c $DYN_COMP conftest.$ac_ext 1>&AC_FD_CC'
cat > conftest.$ac_ext <<EOF
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
int foo()
{
return(0);
}
EOF
if AC_TRY_EVAL(mh_compile) && test -s conftest.o; then
        mh_dyn_link='ld -shared -o conftest.rxlib conftest.o -lc 1>&AC_FD_CC'
#       mh_dyn_link='${CC} -Wl,-shared -o conftest.rxlib conftest.o -lc 1>&AC_FD_CC'
        if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
                LD_RXLIB_A1="ld -shared"
                LD_RXLIB_A2="ld -shared"
#               LD_RXLIB1="${CC} -Wl,-shared"
                LD_RXLIB_B1="-L. -l${LIBFILE}"
                LD_RXLIB_B2="-L. -l${LIBFILE}"
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
        else
                mh_dyn_link='ld -G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
#               mh_dyn_link='${CC} -Wl,-G -o conftest.rxlib conftest.o 1>&AC_FD_CC'
                if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.rxlib; then
                        LD_RXLIB_A1="ld -G"
                        LD_RXLIB_A2="ld -G"
#                       LD_RXLIB1="${CC} -Wl,-G"
                        LD_RXLIB_B1="-L. -l${LIBFILE}"
                        LD_RXLIB_B2="-L. -l${LIBFILE}"
                        SHLPRE="lib"
                        SHLPST=".so"
                        RXLIBLEN="6"
                else
                        LD_RXLIB_A1=""
                        LD_RXLIB_A2=""
                        LD_RXLIB_B1=""
                        LD_RXLIB_B2=""
                        SHLPRE=""
                        SHLPST=""
                        RXLIBLEN="0"
                fi
        fi
fi
rm -f conftest*
])dnl

dnl ---------------------------------------------------------------------------
dnl Work out how to create a shared library
dnl ---------------------------------------------------------------------------
AC_DEFUN(MH_HOWTO_SHARED_LIBRARY,
[
AC_MSG_CHECKING(how to create a shared library)
mh_compile='${CC-cc} -c $DYN_COMP conftest.$ac_ext 1>&AC_FD_CC'
cat > conftest.$ac_ext <<EOF
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
int foo()
{
return(0);
}
EOF
if AC_TRY_EVAL(mh_compile) && test -s conftest.o; then
        mh_dyn_link='ld -shared -o conftest.so.1.0 conftest.o -lc 1>&AC_FD_CC'
#       mh_dyn_link='${CC} -Wl,-shared -o conftest.so.1.0 conftest.o -lc 1>&AC_FD_CC'
        if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
                SHL_LD="ld -shared -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'" -lc"
#               SHL_LD="${CC} -Wl,-shared -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'" -lc"
        else
                mh_dyn_link='ld -G -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#               mh_dyn_link='${CC} -Wl,-G -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
                if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
                        SHL_LD="ld -G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#                       SHL_LD="${CC} -Wl,-G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                else
                        mh_dyn_link='ld -o conftest.so.1.0 -shared -no_archive conftest.o  -lc 1>&AC_FD_CC'
#                       mh_dyn_link='${CC} -o conftest.so.1.0 -Wl,-shared,-no_archive conftest.o  -lc 1>&AC_FD_CC'
                        if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
                                SHL_LD="ld -o ${SHLPRE}${SHLFILE}${SHLPST} -shared -no_archive "'$('SHOFILES')'" -lc"
#                               SHL_LD="${CC} -o ${SHLPRE}${LIBFILE}${SHLPST} -Wl,-shared,-no_archive "'$('SHOFILES')'" -lc"
                        else
                                mh_dyn_link='ld -b -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#                               mh_dyn_link='${CC} -Wl,-b -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
                                if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
                                        SHL_LD="ld -b -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#                                       SHL_LD="${CC} -Wl,-b -o ${SHLPRE}${SHLFILE}.${SHLPST} "'$('SHOFILES')'
                                else
                                        mh_dyn_link='ld -Bshareable -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#                                       mh_dyn_link='${CC} -Wl,-Bshareable -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
                                        if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
                                                SHL_LD="ld -Bshareable -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#                                               SHL_LD="${CC} -Wl,-Bshareable -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                                        else
                                                mh_dyn_link='ld -assert pure-text -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
#                                               mh_dyn_link='${CC} -Wl,-assert pure-text -o conftest.so.1.0 conftest.o 1>&AC_FD_CC'
                                                if AC_TRY_EVAL(mh_dyn_link) && test -s conftest.so.1.0; then
                                                        SHL_LD="ld -assert pure-text -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#                                                       SHL_LD="${CC} -Wl,-assert pure-text -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                                                else
                                                        SHL_LD=""
                                                fi
                                        fi
                                fi
                        fi
                fi
        fi
fi
if test "$SHL_LD" = ""; then
        AC_MSG_RESULT(unknown)
else
        AC_MSG_RESULT(found)
fi
rm -f conftest*
])

dnl ---------------------------------------------------------------------------
dnl Check for presence of various libraries
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_LIB],
[
MH_EXTRA_LIBS=''
for mh_lib in $1; do
        if test "$on_qnx" = yes; then
                AC_MSG_CHECKING(for library -l${mh_lib})
                if test -r /usr/lib/${mh_lib}3r.lib; then
                        AC_MSG_RESULT(found)
                        MH_EXTRA_LIBS="${MH_EXTRA_LIBS} -l${mh_lib}"
                else
                AC_MSG_RESULT(not found)
                fi
        else
                AC_CHECK_LIB($mh_lib,main,mh_lib_found=yes,mh_lib_found=no)
                if test "$mh_lib_found" = yes; then
                        MH_EXTRA_LIBS="${MH_EXTRA_LIBS} -l${mh_lib}"
                fi
        fi
done
])dnl

dnl ---------------------------------------------------------------------------
dnl Check for presence of various libraries
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_CRYPT],
[
mh_save_libs="$LIBS"

if test "$on_qnx" = no; then
        AC_CHECK_LIB(crypt,main,mh_crypt_found=yes,mh_crypt_found=no)
        if test "$mh_crypt_found" = yes; then
                LIBS="${LIBS} -lcrypt"
        fi
fi

AC_MSG_CHECKING(for crypt function)

AC_CACHE_VAL(
[mh_cv_func_crypt],
[
        AC_TRY_LINK(
[#include <unistd.h>],
[crypt("fred","aa")],
        [mh_cv_func_crypt=yes],
        [mh_cv_func_crypt=no]
        )
])dnl
AC_MSG_RESULT($mh_cv_func_crypt)
if test "$mh_cv_func_crypt" = yes ; then
        AC_DEFINE(HAVE_CRYPT)
        if test "$mh_crypt_found" = yes; then
                MH_EXTRA_LIBS="${MH_EXTRA_LIBS} -lcrypt"
        fi
fi

LIBS="$mh_save_libs"

])dnl

dnl ---------------------------------------------------------------------------
dnl Determine how to build shared libraries etc..
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_SHARED_LIBRARY],
[
dnl
dnl If compiler is gcc, then flags should be the same for all platforms
dnl (just guessing on this)
dnl
SHLPST=".so"
SHLPRE="lib"
LIBFILE="regina"
SHLFILE="regina"
OSAVE=".o.save"
OBJ="o"
EXE=""
LIBPST=".a"
LIBPRE="lib"
LIBEXE="ar"
GETOPT=""
OBJECTS='$('OFILES')'
STATIC_LDFLAGS=""
AIX_DYN="no"
BEOS_DYN="no"

AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
        *hp-hpux10*)
                SYS_DEFS="-D_HPUX_SOURCE"
                SHL="sl"
                ;;
        *hp-hpux*)
                SYS_DEFS="-D_HPUX_SOURCE"
                SHL="sl"
                ;;
        *ibm-aix4.2*)
                SYS_DEFS="-D_ALL_SOURCE -DAIX"
                DYN_COMP="-DDYNAMIC"
                SHL="a"
                ;;
        *ibm-aix*)
                SYS_DEFS="-D_ALL_SOURCE -DAIX"
                SHL="a"
                ;;
        *dec-osf*)
                if test "$ac_cv_prog_CC" = "gcc"; then
                        SYS_DEFS="-D_POSIX_SOURCE -D_XOPEN_SOURCE"
                else
                        SYS_DEFS="-D_POSIX_SOURCE -D_XOPEN_SOURCE -Olimit 800"
                fi
                ;;
        *sequent-dynix*)
                ;;
        *solaris*)
                ;;
        *esix*)
                ;;
        sparc*sunos*)
                SYS_DEFS="-DSUNOS -DSUNOS_STRTOD_BUG"
                ;;
        *linux*)
                ;;
        *nto-qnx*)
                ;;
        *qnx*)
                LIBPRE=""
                LIBPST=".lib"
                SHLPRE=""
                SHLPST=""
                SHLFILE=""
                ;;
        *cygwin)
                LIBPRE=""
                SHLPRE=""
                DYN_COMP="-DDYNAMIC"
                LIBPST=".a"
                SHLPST=".dll"
                EXE=".exe"
                ;;
esac

dnl
dnl set variables for compiling if we don't already have it
dnl
if test "$DYN_COMP" = ""; then
AC_MSG_CHECKING(compiler flags for a dynamic object)

cat > conftest.$ac_ext <<EOF
dnl [#]line __oline__ "[$]0"
[#]line __oline__ "configure"
int a=0
EOF

        DYN_COMP=""
        mh_cv_stop=no
        save_cflags="$CFLAGS"
        mh_cv_flags="-fPIC -KPIC +Z"
        for a in $mh_cv_flags; do
                CFLAGS="-c $a"

                mh_compile='${CC-cc} -c $CFLAGS conftest.$ac_ext > conftest.tmp 2>&1'
                if AC_TRY_EVAL(mh_compile); then
                        DYN_COMP=""
                else
                        slash="\\"
                        mh_dyncomp="`egrep -c $slash$a conftest.tmp`"
                        if test "$mh_dyncomp" = "0"; then
                                DYN_COMP="$a -DDYNAMIC"
                                AC_MSG_RESULT($a)
                                break
                        else
                                DYN_COMP=""
                        fi
                fi
        done
        if test "$DYN_COMP" = ""; then
                AC_MSG_RESULT(none of $mh_cv_flags supported)
        fi
        CFLAGS=$save_cflags
        rm -f conftest.*
fi

dnl
dnl set variables for linking
dnl
SHL_LD=""
TEST1EXPORTS=""
TEST2EXPORTS=""
TEST1EXP=""
TEST2EXP=""
REGINAEXP=""

# If the build OS can handle version numbers in the shared library name,
# then set SHL_BASE="${SHLPRE}${SHLFILE}${SHLPST}.\$(ABI)"
SHL_BASE="${SHLPRE}${SHLFILE}${SHLPST}"

# OTHER_INSTALLS is set to install a non-version numbered shared library
# by default; ie. no .\$(ABI) suffix. If the regina executable is not built, 
# then there is no shared library. Set OTHER_INSTALLS="installabilib" if you
# are building a version numbered shared library.
OTHER_INSTALLS="installlib"
BASE_INSTALL="installbase"
BASE_BINARY="binarybase"
USE_ABI="no"
case "$target" in
        *hp-hpux*)
                EEXTRA="-Wl,-E"
                LD_RXLIB_A1="ld -b -q -n"
                LD_RXLIB_A2="ld -b -q -n"
#               LD_RXLIB1="${CC} -Wl,-b,-q,-n"
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                DYNAMIC_LDFLAGS="-Wl,+s"
                SHLPRE="lib"
                SHLPST=".sl"
                RXLIBLEN="6"
                SHL_LD="ld -b -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
#               SHL_LD="${CC} -Wl,-b -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
#       *ibm-aix4.2*)
#               STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
#               LD_RXLIB1="ld -G"
#               LD_RXLIB2=""
#               SHLPRE="lib"
#               SHLPST=".so"
#               RXLIBLEN="6"
#               SHL_LD="ld -o ${SHLPRE}${SHLFILE}${SHLPST} -shared -no_archive "'$('SHOFILES')'" -lc"
#               ;;
        *ibm-aix3*)
                STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
                LD_RXLIB_A1="ld -bnoentry -bM:SRE -bT:512 -bH:512 -bI:regina.exp"
                LD_RXLIB_A2="ld -bnoentry -bM:SRE -bT:512 -bH:512 -bI:regina.exp"
                LD_RXLIB_B1="${SHLPRE}${SHLFILE}${SHLPST} -lc"
                LD_RXLIB_B2="${SHLPRE}${SHLFILE}${SHLPST} -lc"
                SHLPRE="lib"
                SHLPST=".a"
                RXLIBLEN="5"
                TEST1EXPORTS="-bE:test1.exp"
                TEST2EXPORTS="-bE:test2.exp"
                TEST1EXP="test1.exp"
                TEST2EXP="test2.exp"
                REGINAEXP="regina.exp"
                if test "$ac_cv_header_dlfcn_h" = "yes" -o "$HAVE_DLFCN_H" = "1"; then
                        AIX_DYN="yes"
                        DYN_COMP="-DDYNAMIC"
                        SHL_LD="ld -o ${SHLPRE}${SHLFILE}${SHLPST} -bnoentry -bE:regina.exp -bM:SRE -bT:512 -bH:512 "'$('SHOFILES')'" -lc"
                else
                        SHL_LD=" "'$('LIBEXE')'" "'$('LIBFLAGS')'" "'$('SHOFILES')'
                        DYN_COMP=""
                fi
                ;;
        *ibm-aix4*)
                STATIC_LDFLAGS="-bnso -bI:/lib/syscalls.exp"
                LD_RXLIB_A1="ld -bnoentry -bM:SRE"
                LD_RXLIB_A2="ld -bnoentry -bM:SRE"
                LD_RXLIB_B1="${SHLPRE}${SHLFILE}${SHLPST} -lc"
                LD_RXLIB_B2="${SHLPRE}${SHLFILE}${SHLPST} -lc"
                SHLPRE="lib"
                SHLPST=".a"
                RXLIBLEN="5"
                TEST1EXPORTS="-bE:test1.exp"
                TEST2EXPORTS="-bE:test2.exp"
                TEST1EXP="test1.exp"
                TEST2EXP="test2.exp"
                REGINAEXP="regina.exp"
                if test "$ac_cv_header_dlfcn_h" = "yes" -o "$HAVE_DLFCN_H" = "1"; then
                        AIX_DYN="yes"
                        DYN_COMP="-DDYNAMIC"
                        SHL_LD="ld -o ${SHLPRE}${SHLFILE}${SHLPST} -bnoentry -bE:regina.exp -bM:SRE "'$('SHOFILES')'" -lc "'$('SHLIBS')'
                else
                        SHL_LD=" "'$('LIBEXE')'" "'$('LIBFLAGS')'" "'$('SHOFILES')'
                        DYN_COMP=""
                fi
                ;;
        *dec-osf*)
                LD_RXLIB_A1="ld -shared"
                LD_RXLIB_A2="ld -shared"
                LD_RXLIB_B1="-lc -L. -l${LIBFILE}"
                LD_RXLIB_B2="-lc -L. -l${LIBFILE}"
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -o ${SHLPRE}${SHLFILE}${SHLPST} -shared -no_archive "'$('SHOFILES')'" -lc"
                ;;
        *esix*)
                LD_RXLIB_A1="ld -G"
                LD_RXLIB_A2="ld -G"
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                DYNAMIC_LDFLAGS=""
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
        *dgux*)
                LD_RXLIB_A1="ld -G"
                LD_RXLIB_A2="ld -G"
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                DYNAMIC_LDFLAGS=""
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
        *pc-sco*)
                LD_RXLIB_A1="ld -dy -G"
                LD_RXLIB_A2="ld -dy -G"
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                DYNAMIC_LDFLAGS=""
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -dy -G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
        *solaris*)
                LD_RXLIB_A1="ld -G"
                LD_RXLIB_A2="ld -G"
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                DYNAMIC_LDFLAGS=""
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -G -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
        sparc*sunos*)
                LD_RXLIB_A1="ld"
                LD_RXLIB_A2="ld"
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -assert pure-text -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
        *freebsd*)
                LD_RXLIB_A1="ld -Bdynamic -Bshareable"
                LD_RXLIB_A2="ld -Bdynamic -Bshareable"
                LD_RXLIB_B1="-lc -L. -l${SHLFILE}"
                LD_RXLIB_B2="-lc -L. -l${SHLFILE}"
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="ld -Bdynamic -Bshareable -o ${SHLPRE}${SHLFILE}${SHLPST} "'$('SHOFILES')'
                ;;
        *linux*)
                LD_RXLIB_A1="${CC} -shared"
                LD_RXLIB_A2="${CC} -shared"
                LD_RXLIB_B1="-L. -l${SHLFILE}"
                LD_RXLIB_B2="-L. -l${SHLFILE}"
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="${CC} -o ${SHLPRE}${SHLFILE}${SHLPST}.\$(ABI) -shared ${SHL_SCRIPT} -Wl,-soname=${SHLPRE}${SHLFILE}${SHLPST}.\$(ABI) \$(SHOFILES) -lc"
                SHL_BASE="${SHLPRE}${SHLFILE}${SHLPST}.\$(ABI)"
                OTHER_INSTALLS="installabilib"
                USE_ABI="yes"
                ;;
        *atheos*)
                LD_RXLIB_A1="${CC} -shared"
                LD_RXLIB_A2="${CC} -shared"
                LD_RXLIB_B1="-L. -l${LIBFILE} "'$('BOTHLIBS')'
                LD_RXLIB_B2="-L. -l${LIBFILE} "'$('BOTHLIBS')'
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="${CC} -o ${SHLPRE}${SHLFILE}${SHLPST} -shared -Wl,-soname=${SHLPRE}${SHLFILE}${SHLPST} \$(SHOFILES) "'$('BOTHLIBS')'
                SHL_BASE="${LIBPRE}${LIBFILE}.${SHL}"
                ;;
        *beos*)
                LD_RXLIB_A1="${CC} -Wl,-shared -nostart -Xlinker -soname=\$(@)"
                LD_RXLIB_A2="${CC} -Wl,-shared -nostart -Xlinker -soname=\$(@)"
                LD_RXLIB_B1="-L. -l${SHLFILE}"
                LD_RXLIB_B2="-L. -l${LIBFILE}"
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="${CC} -o ${SHLPRE}${SHLFILE}${SHLPST} -Wl,-shared -nostart -Xlinker -soname=${SHLPRE}${SHLFILE}${SHLPST} \$(SHOFILES)"
                SHL_BASE="${SHLPRE}${SHLFILE}${SHLPST}"
                BEOS_DYN="yes"
                BASE_INSTALL="beosinstall"
                BASE_BINARY="beosbinary"
                OTHER_INSTALLS=""
                ;;
        *nto-qnx*)
                LD_RXLIB_A1="${CC} -shared"
                LD_RXLIB_A2="${CC} -shared"
                LD_RXLIB_B1="-L. -l${SHLFILE}"
                LD_RXLIB_B2="-L. -l${SHLFILE}"
                SHLPRE="lib"
                SHLPST=".so"
                RXLIBLEN="6"
                SHL_LD="${CC} -o ${SHLPRE}${SHLFILE}${SHLPST} -shared -Wl,-soname=${SHLPRE}${SHLFILE}${SHLPST} \$(SHOFILES)"
                SHL_BASE="${SHLPRE}${SHLFILE}${SHLPST}"
                ;;
        *qnx*)
                LD_RXLIB_A1=""
                LD_RXLIB_A2=""
                LD_RXLIB_B1=""
                LD_RXLIB_B2=""
                SHLPRE=""
                SHLPST=".lib"
                SHL_LD=""
                EEXTRA="-mf -N0x20000 -Q"
                LIBPRE=""
                LIBPST=".lib"
                ;;
        *cygwin)
                LD_RXLIB_A1="dllwrap --target i386-cygwin32 --def test1.def --dllname test1.dll"
                LD_RXLIB_A2="dllwrap --target i386-cygwin32 --def test2.def --dllname test2.dll"
                LD_RXLIB_B1="-L. -lregina"
                LD_RXLIB_B2="-L. -lregina"
                SHLPRE=""
                SHLPST=".dll"
                SHL_LD="dllwrap --def regina.def --output-lib libregina.a --target i386-cygwin32 --dllname regina.dll -o regina.dll \$(SHOFILES) -lcrypt"
                EEXTRA=""
                LIBPRE="lib"
                LIBPST=".a"
                LIBFILE="rexx"
                SHL_BASE="${SHLPRE}${SHLFILE}${SHLPST}"
                TEST1EXPORTS="test1.def"
                TEST2EXPORTS="test2.def"
                TEST1EXP="test1.def"
                TEST2EXP="test2.def"
                REGINAEXP="regina.def"
                ;;
        *)
                MH_HOWTO_SHARED_LIBRARY()
                MH_HOWTO_DYN_LINK()
                ;;
esac

if test "$AIX_DYN" = "yes"; then
        STATICLIB=""
else
        STATICLIB="${LIBPRE}${LIBFILE}${LIBPST}"
fi

LIBFLAGS="cr ${LIBPRE}${LIBFILE}${LIBPST}"
LIBLINK="-L. -l${LIBFILE}"

#
# For HPUX, we must link the "regina" executable with -lregina
# otherwise you can't move the shared library anywhere other
# than where it was built. Fix by PJM
#
if test "$SHLPST" = ".sl"; then
        LINKSHL="$LIBLINK"
else
        LINKSHL="${SHLPRE}${SHLFILE}${SHLPST}"
fi

if test "$ac_cv_header_dl_h" = "yes" -o "$ac_cv_header_dlfcn_h" = "yes" -o "$AIX_DYN" = "yes" -o "$BEOS_DYN" = "yes"; then
        SHL_TARGETS="regina${EXE} ${SHL_BASE} ${SHLPRE}test1${SHLPST} ${SHLPRE}test2${SHLPST}"
        EXECISER_DEP="${SHL_BASE}"
        OTHER_INSTALLS="regina $OTHER_INSTALLS"
else
        SHL_TARGETS=""
        EXECISER_DEP="${STATICLIB}"
        OTHER_INSTALLS=""
fi

AC_SUBST(BASE_BINARY)
AC_SUBST(BASE_INSTALL)
AC_SUBST(BOTHLIBS)
AC_SUBST(CC2O)
AC_SUBST(CEXTRA)
AC_SUBST(DYNAMIC_LDFLAGS)
AC_SUBST(DYN_COMP)
AC_SUBST(EEXTRA)
AC_SUBST(EXE)
AC_SUBST(EXECISER_DEP)
AC_SUBST(GETOPT)
AC_SUBST(LD_RXLIB_A1)
AC_SUBST(LD_RXLIB_A2)
AC_SUBST(LD_RXLIB_B1)
AC_SUBST(LD_RXLIB_B2)
AC_SUBST(LIBEXE)
AC_SUBST(LIBFILE)
AC_SUBST(LIBFLAGS)
AC_SUBST(LIBLINK)
AC_SUBST(LIBPRE)
AC_SUBST(LIBPST)
AC_SUBST(LINKSHL)
AC_SUBST(O2SAVE)
AC_SUBST(O2SHO)
AC_SUBST(OBJ)
AC_SUBST(OBJECTS)
AC_SUBST(OSAVE)
AC_SUBST(OTHER_INSTALLS)
AC_SUBST(REGINAEXP)
AC_SUBST(RXLIBLEN)
AC_SUBST(SAVE2O)
AC_SUBST(SHLFILE)
AC_SUBST(SHLIBS)
AC_SUBST(SHLPRE)
AC_SUBST(SHLPST)
AC_SUBST(SHL_LD)
AC_SUBST(SHL_TARGETS)
AC_SUBST(STATICLIB)
AC_SUBST(STATIC_LDFLAGS)
AC_SUBST(TEST1EXP)
AC_SUBST(TEST1EXPORTS)
AC_SUBST(TEST2EXP)
AC_SUBST(TEST2EXPORTS)
AC_SUBST(USE_ABI)
])dnl
