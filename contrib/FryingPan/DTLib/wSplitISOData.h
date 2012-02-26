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

#ifndef _W_SPLITISODATA_H_ 
#define _W_SPLITISODATA_H_ 

#include "IFileWriter.h"
#include <Generic/HookT.h>
#include <Generic/VectorT.h>
#include <Generic/String.h>
#include "DTLib.h"

using namespace GenNS;

class wSplitISOData : public IFileWriter
{
protected:
   String                        name_base;
   uint16                        current_ext;
   BPTR                          master_file;
   BPTR                          current_file;
   uint32                        current_size;
   uint16                        sector_size;
   uint64                        num_sectors;
   VectorT<String*>              names;
   VectorT<uint32>               sizes;
   uint32                        max_part_size;

protected:
                                 wSplitISOData(const char* sFileName, const IOptItem *di, EDtError &rc);
   virtual                      ~wSplitISOData();      
   virtual bool                  generateFileSum(const char*, uint32[4]);
public:
   static IFileWriter           *openWrite(const char* sFile, const IOptItem *di, EDtError &rc);
   
   virtual const char           *getName();
   virtual bool                  isAudio();
   virtual bool                  isData();

   static bool                   static_isAudio();
   static bool                   static_isData();
   static bool                   static_isSession();
   static const char            *static_getName();

   virtual bool                  writeData(void* pBuff, int pLen);
   virtual bool                  setUp();
   virtual void                  cleanUp();
   virtual void                  dispose();
   virtual void                  deleteFiles();
   virtual void                  setBlockCount(unsigned long);
   virtual void                  setBlockSize(unsigned short);
};

#endif /*RAWAUDIO_H_*/
