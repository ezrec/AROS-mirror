#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(spout, unix_1.3, rohan@mizzenblog.com)
AM_INIT_AUTOMAKE(spout, unix_1.3)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
# AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

CFLAGS="$CFLAGS -std=gnu99"

# Checks for libraries.

# SDL
SDL_VERSION=1.2.6
AM_PATH_SDL($SDL_VERSION,:,AC_MSG_ERROR([*** couldn't find SDL $SDL_VERSION]))
#CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# Checks for header files.
AC_CHECK_HEADERS([stdint.h unistd.h string.h stdlib.h wordexp.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([
	Makefile
	])
AC_OUTPUT
