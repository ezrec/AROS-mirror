/*******************************************************************
 *
 *  ttdebug.h
 *
 *    Debugging and Logging component (specification)
 *
 *  Copyright 1996-1998 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 *  This component contains various macros and functions used to
 *  ease the debugging of the FreeType engine. Their main purpose
 *  is in assertion checking, tracing and error detection.
 *
 *  There are now three debugging modes :
 *
 *  - trace mode :
 *
 *       error and trace messages are sent to the log file
 *       (which can be the standard error output)
 *
 *  - error mode :
 *
 *       only error messages are generated.
 *
 *  - release mode :
 *
 *       no error message is sent nor generated. The code is
 *       free from any debugging parts.
 *
 ******************************************************************/

#ifndef TTDEBUG_H
#define TTDEBUG_H

#include "ttconfig.h"


#ifdef __cplusplus
  extern "C" {
#endif

#ifdef DEBUG_LEVEL_TRACE

  typedef enum Trace_Component_
  {
    trace_any = 0,
    trace_api,
    trace_interp,
    trace_load,
    trace_gload,
    trace_memory,
    trace_file,
    trace_mutex,
    trace_cache,
    trace_calc,
    trace_cmap,
    trace_extend,
    trace_objs,
    trace_raster,

    trace_max

  } Trace_Component;

  extern Byte  tt_trace_levels[trace_max];

  /* XXXX IMPORTANT XXXX :                                      */
  /*                                                            */
  /*  Each component must define the macro TT_COMPONENT         */
  /*  to a valid Trace_Component value before using any         */
  /*  TRACE macro.                                              */
  /*                                                            */

  #define  PTRACE( level, varformat )  \
         if ( tt_trace_levels[TT_COMPONENT] >= level ) TT_Message##varformat

#elif defined(DEBUG_LEVEL_ERROR)

#else  /* RELEASE MODE */

#define TT_Assert( condition, action )  /* nothing !! */

#define PTRACE( level, varformat )  /* nothing */
#define PERROR( varformat )  /* nothing */
#define PANIC( varformat )  /* nothing */

#endif




/************************************************************************/
/*                                                                      */
/*  Define macros and fuctions that are common the the debug and trace  */
/*  modes.                                                              */
/*                                                                      */
/*  You need vprintf to be able to compile ttdebug.c                    */
/*                                                                      */

#if defined(DEBUG_LEVEL_TRACE) || defined(DEBUG_LEVEL_ERROR)

#include <stdio.h>


#define TT_Assert( condition, action )  if ( !(condition) ) ( action )

  extern  void  TT_Message( const String*  fmt, ... );
  extern  void  TT_Panic  ( const String*  fmt, ... );
  /* print a message and exit */

  extern  const String*  Cur_U_Line( void*  exec );

  #define  PERROR( varformat )   TT_Message##varformat
  #define  PANIC( varformat )    TT_Panic##varformat

#endif


#define  PTRACE0( varformat )  PTRACE( 0, varformat )
#define  PTRACE1( varformat )  PTRACE( 1, varformat )
#define  PTRACE2( varformat )  PTRACE( 2, varformat )
#define  PTRACE3( varformat )  PTRACE( 3, varformat )
#define  PTRACE4( varformat )  PTRACE( 4, varformat )
#define  PTRACE5( varformat )  PTRACE( 5, varformat )
#define  PTRACE6( varformat )  PTRACE( 6, varformat )
#define  PTRACE7( varformat )  PTRACE( 7, varformat )


#ifdef __cplusplus
  }
#endif


#endif /* TTDEBUG_H */
