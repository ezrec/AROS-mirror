/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: debug.h,v 1.8 2005/12/12 14:39:16 itix Exp $
 *
 *****************************************************************************/

void handle_error(const char *format, ...);

#if defined (__AROS__)
  #define DEBUG 1
#endif

/* define D and bug */
#if defined(__MORPHOS__)
  #if DEBUG
    #define D(x)  x
    #define bug   kprintf
  #else
    #define D(x)
    #define bug
  #endif
#elif defined(__AMIGAOS4__)
  #define D(x)  x
  #define bug   DebugPrintF
#elif defined(__AROS__)
  #include <aros/debug.h>
#endif

/* DebOut */
#if defined(MGTK_DEBUG)
  #if defined(__MORPHOS__) || defined(__AROS__)
    #define DebOut(...)  D(bug(__VA_ARGS__))
  #else
    #define DebOut printf
  #endif
#else
  #define DebOut(...)  
#endif

/* WarnOut */
#if defined(MGTK_DEBUG)
  #if defined(__AROS__)
    #define WarnOut(...) printf("WARNING: ");printf(__VA_ARGS__);DebOut("WARNING");DebOut(__VA_ARGS__)
  #else 
    #define WarnOut(...) printf("WARNING: ");printf(__VA_ARGS__);
  #endif
#else 
  #if defined(__MORPHOS__) || defined(__AROS__)
    #if defined(__AROS__)
      #include <aros/debug.h>
    #endif
    #define WarnOut(...)  D(bug("WARNING: "));D(bug(__VA_ARGS__))
  #else
    #define WarnOut(...) printf("WARNING: ");printf(__VA_ARGS__)
  #endif
#endif

/* ErrOut */
#if defined(MGTK_DEBUG)
  //#define ErrOut(...) printf("ERROR: ");printf(__VA_ARGS__);DebOut("ERROR");DebOut(__VA_ARGS__)
  #define ErrOut(...) DebOut("ERROR: ");DebOut(__VA_ARGS__);handle_error(__VA_ARGS__)
#else 
  #define ErrOut(...) handle_error(__VA_ARGS__)
#endif

#if defined(ENABLE_RT)
  #include <aros/rt.h>
#endif
