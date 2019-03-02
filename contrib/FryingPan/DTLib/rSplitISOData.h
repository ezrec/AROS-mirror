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

#ifndef _R_SPLITISODATA_H_
#define _R_SPLITISODATA_H_

#include <Generic/LibrarySpool.h>
#include "IFileReader.h"
#include <Generic/HookT.h>
#include <Generic/DynList.h>
#include <Optical/IOptItem.h>
#include <Generic/String.h>
#include <Generic/VectorT.h>
#include "DTLib.h"

class rSplitISOData : public IFileReader
{
   struct PartDescriptor
   {
      String   fileName;
      String   md5sum;
      uint32   sectors;
   };

protected:
   String                        sFileName;
   IOptItem                     *item;
   uint64                        sector_count;
   uint16                        sector_size;
   int32                         num_parts;
   VectorT<PartDescriptor*>      descs;

   BPTR                          current_file;
   uint32                        current_pos;
   uint32                        current_size;
   uint16                        current_elem;
protected:
                                 rSplitISOData(const char* sFileName, EDtError &rc);
   virtual                      ~rSplitISOData();
   bool                          readDescriptor(EDtError &rc);
   bool                          validatePart(PartDescriptor *pdesc);
public:
   static IFileReader           *openRead(const char* sFile, EDtError &rc);
   static bool                   checkFile(const char* sFile, EDtError &rc);
   
   virtual const char           *getName();
   virtual bool                  isAudio();
   virtual bool                  isData();

   static bool                   static_isAudio();
   static bool                   static_isData();
   static bool                   static_isSession();
   static const char            *static_getName();

   virtual const char*           getTrackName();
   virtual bool                  readData(const IOptItem*, void* pBuff, int pLen);
   virtual bool                  setUp();
   virtual void                  cleanUp();
   virtual void                  dispose();
   virtual uint32                getBlockCount();
   virtual uint16                getBlockSize();
   virtual bool                  fillOptItem(IOptItem *item);
};

#endif /*RAWAUDIO_H_*/
