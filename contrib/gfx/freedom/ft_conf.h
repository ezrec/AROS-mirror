/* This file is part of the FreeType project */

/* ft_conf.h for AmigaOS using ADE enviroment */

#define AROS

#ifndef AROS_MACHINE_H
#include <aros/machine.h>
#endif

/* Define to empty if the keyword does not work. */

/* #undef const */

/* Define if you have a working `mmap' system call. */

/* #undef HAVE_MMAP */

/* Define if you have the <stdlib.h> header file. */

#define HAVE_STDLIB_H

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX). */

#if AROS_BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif

/* Define if the X Window System is missing or not being used. */

#define X_DISPLAY_MISSING

#warning "fix SIZEOF_INT and SIZEOF_LONG for machines were these are not 4 Bytes"
/* The number of bytes in a int. */

#define SIZEOF_INT 4

/* The number of bytes in a long. */

#define SIZEOF_LONG 4

/* Define if you have the getpagesize function. */

// #define HAVE_GETPAGESIZE

/* Define if you have the memcpy function. */

#define HAVE_MEMCPY

/* Define if you have the memmove function. */

#define HAVE_MEMMOVE

/* Define if you have the valloc function. */

//#define HAVE_VALLOC

/* Define if you have the <fcntl.h> header file. */

//#define HAVE_FCNTL_H

/* Define if you have the <unistd.h> header file. */

//#define HAVE_UNISTD_H

/**********************************************************************/
/*                                                                    */
/*  The following configuration macros can be tweaked manually by     */
/*  a developer to turn on or off certain features or options in the  */
/*  TrueType engine. This may be useful to tune it for specific       */
/*  purposes..                                                        */
/*                                                                    */
/**********************************************************************/


/*************************************************************************/
/* Define this if the underlying operating system uses a different       */
/* character width than 8bit for file names.  You must then also supply  */
/* a typedef declaration for defining 'TT_Text'.  Default is off.        */

/* #define HAVE_TT_TEXT */


/*************************************************************************/
/* Define this if you want to generate code to support engine extensions */
/* Default is on, but if you're satisfied by the basic services provided */
/* by the engine and need no extensions, undefine this configuration     */
/* macro to save a few more bytes.                                       */

#define  TT_CONFIG_OPTION_EXTEND_ENGINE


/*************************************************************************/
/* Define this if you want to generate code to support gray-scaling,     */
/* a.k.a. font-smoothing or anti-aliasing. Default is on, but you can    */
/* disable it if you don't need it.                                      */

//#define  TT_CONFIG_OPTION_GRAY_SCALING


/*************************************************************************/
/* Define this if you want to use a big 'switch' statement within the    */
/* bytecode interpreter. Because some non-optimizing compilers are not   */
/* able to produce jump tables from such statements, undefining this     */
/* configuration macro will generate the appropriate C jump table in     */
/* ttinterp.c. If you use an optimizing compiler, you should leave it    */
/* defined for better performance and code compactness..                 */

#define  TT_CONFIG_OPTION_INTERPRETER_SWITCH


/*************************************************************************/
/* Define TT_CONFIG_THREAD_SAFE if you want to build a thread-safe       */
/* version of the library.                                               */

#define  TT_CONFIG_OPTION_THREAD_SAFE

/**********************************************************************/
/*                                                                    */
/*  The following macros are used to define the debug level, as well  */
/*  as individual tracing levels for each component. There are        */
/*  currently three modes of operation :                              */
/*                                                                    */
/*  - trace mode (define DEBUG_LEVEL_TRACE)                           */
/*                                                                    */
/*      The engine prints all error messages, as well as tracing      */
/*      ones, filtered by each component's level                      */
/*                                                                    */
/*  - debug mode (define DEBUG_LEVEL_ERROR)                           */
/*                                                                    */
/*      Disable tracing, but keeps error output and assertion         */
/*      checks.                                                       */
/*                                                                    */
/*  - release mode (don't define anything)                            */
/*                                                                    */
/*      Don't include error-checking or tracing code in the           */
/*      engine's code. Ideal for releases.                            */
/*                                                                    */
/* NOTE :                                                             */
/*                                                                    */
/*   Each component's tracing level is defined in its own source.     */
/*                                                                    */
/**********************************************************************/

/* Define if you want to use the tracing debug mode */
#undef  DEBUG_LEVEL_TRACE

/* Define if you want to use the error debug mode - ignored if */
/* DEBUG_LEVEL_TRACE is defined                                */
#undef  DEBUG_LEVEL_ERROR



/* AMIGA SPECIFIC */

//#define AMIGA_ASM_CALC

/* End of ft_conf.h */
