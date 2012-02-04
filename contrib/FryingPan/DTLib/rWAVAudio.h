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

#ifndef _R_WAVAUDIO_H_ 
#define _R_WAVAUDIO_H_ 

#include "FileReader.h"
#include <Generic/HookT.h>
#include <Generic/DynList.h>

struct WaveHdr;

class rWAVAudio : public FileReader
{
   long                          offTotal;      // offset of FORM size
   long                          offSoundChunk; // offset of sound chunk size
protected:
                                 rWAVAudio(const char* sFileName, EDtError &rc);
   static bool                   checkHeader(WaveHdr *pHdr, EDtError &rc);
   virtual void                  readHeaders();
public:
   static IFileReader           *openRead(const char* sFile, EDtError &rc);
   
   virtual const char           *getName();
   virtual bool                  isAudio();
   virtual bool                  isData();
   static bool                   static_isSession();

   static bool                   static_isAudio();
   static bool                   static_isData();
   static const char            *static_getName();

   static bool                   checkFile(const char* sFileName, EDtError &rc);
};

#endif /*RAWAUDIO_H_*/
