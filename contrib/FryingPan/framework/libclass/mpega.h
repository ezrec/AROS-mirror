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

#ifndef _MPEGAIFACE_H_ 
#define _MPEGAIFACE_H_

extern class MPEGAIFace *MPEGA;

#include <libclass/exec.h>
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS
#include <libraries/mpega.h>

   BEGINDECL(MPEGA, "mpega.library")
      FUNC2(MPEGA_STREAM*,    MPEGA_open,                            5, const char*,      fileName,      a0,   const MPEGA_CTRL*,            mpegaControl,  a1);
      PROC1(                  MPEGA_close,                           6, MPEGA_STREAM*,    mpegaStream,   a0);
      FUNC2(long,             MPEGA_decode_frame,                    7, MPEGA_STREAM*,    mpegaStream,   a0,   uint16**,                     mpegaPCM,      a1);
      FUNC2(long,             MPEGA_seek,                            8, MPEGA_STREAM*,    mpegaStream,   a0,   unsigned long,                millis,        d0);
      FUNC2(long,             MPEGA_time,                            9, MPEGA_STREAM*,    mpegaStream,   a0,   unsigned long*,               timePos,       a1);
      FUNC2(long,             MPEGA_find_sync,                      10, unsigned char*,   buffer,        a0,   unsigned long,                length,        d0);
      FUNC2(long,             MPEGA_scale,                          11, MPEGA_STREAM*,    mpegaStream,   a0,   unsigned long,                percent,       d0);


   ENDDECL


#endif /*_MPEGA_H_*/
