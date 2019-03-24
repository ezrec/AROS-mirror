/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _LIBWRAPPER_COMMON_H_
#define _LIBWRAPPER_COMMON_H_

#define __NOLIBBASE__
#include <exec/resident.h>
#include <Generic/Types.h>

#ifdef __cplusplus
extern "C"
{
#endif

   extern struct Library*      SysBase;
   extern struct Library*      OurBase;
   
   extern uint                 LIB_FuncTable[];    // LIB_FT_Begin + LIB_FT_Function + LIB_FT_End

   extern bool                 Lib_SetUp();        // user defined setup with weak local binding
   extern void                 Lib_CleanUp();      // in these functions you should only perform your own
   extern bool                 Lib_Acquire();      // initializations. returning false means library should
   extern void                 Lib_Release();      // not allow opening.

#ifdef __cplusplus
};
#endif

#if defined(__AMIGAOS4__)
   #include "LibWrapper_os4.h"
#elif defined(__MORPHOS__)
   #include "LibWrapper_mos.h"
#elif defined(__AROS__)
   #include "LibWrapper_aros.h"
#else
   #include "LibWrapper_os3.h"
#endif

#endif //_LIBWRAPPER_COMMON_H_
