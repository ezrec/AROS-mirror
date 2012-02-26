/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _DTLIB_H_
#define _DTLIB_H_

enum EDtError
{
   DT_OK                         =  0,       // all ok
   DT_InvalidOperation,                      // asking reader to write or writer to read
   DT_InvalidFormat,                         // the requested file is not compatible with module
   DT_OutOfMemory,                           // as the name says
   DT_RequiredResourceNotAvailable,          // when for example a library is missing.
   DT_UnableToOpenFile,                      // file could not be opened for read or write
   DT_FileMalformed,                         // file is corrupted
   DT_WrongChannelCount,                     // wrong number of channels
   DT_WrongFrequency,                        // improper frequency
   DT_WrongResolution,                       // resolution not 16bit
   DT_FileFormatNotSupported,                // particular type is not supported (compression etc)
};

#endif

