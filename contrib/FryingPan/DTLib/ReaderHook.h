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

#ifndef _READERHOOK_H_
#define _READERHOOK_H_ 

#include <Generic/HookT.h>
#include <Generic/DynList.h>
#include "IFileReader.h"
#include "IRegHook.h"

template<class T, signed char pri>
class ReaderHook : public IRegHook
{
   const char *className;
public:
                           ReaderHook()
   {
      className = T::static_getName();
   }

   virtual                ~ReaderHook()
   {
   }

   virtual const char     *getName()
   {
      return className;
   }

   virtual bool            isReader()
   {
      return true;
   }

   virtual bool            isWriter()
   {
      return false;
   }

   virtual signed char     getPriority()
   {
      return pri;
   }
   
   virtual bool            checkFile(const char* sName, EDtError &rc)
   {
      return T::checkFile(sName, rc);
   }

   virtual IFileReader    *openRead(const char* sName, EDtError &rc)
   {
      IFileReader *fr = T::openRead(sName, rc);
      
      if ((rc != DT_OK) && (fr != 0))
      {
         fr->dispose();
         fr = 0;
      }
      return fr;
   }

   virtual IFileWriter    *openWrite(const char* sName, const IOptItem* item, EDtError &rc)
   {
      rc = DT_InvalidOperation;
      return 0;
   }

   virtual bool            isAudio()
   {
      return T::static_isAudio();
   }

   virtual bool            isData()
   {
      return T::static_isData();
   }

   virtual bool            isSession()
   {
      return T::static_isSession();
   }
};

#endif

