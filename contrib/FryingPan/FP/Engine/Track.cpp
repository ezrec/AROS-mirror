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

#include "Track.h"
#include "Globals.h"
#include <Generic/HookAttrT.h>
#include <DTLib/IRegHook.h>

Track::Track(const char* name)
{
   trackName = name;
   regHook = 0;
   fileReader = 0;
   EDtError rc = DT_OK;

   for (unsigned int i=0; i<pRWHooks->GetCount(); i++)
   {
      if (((*pRWHooks)[i]->isReader()) && (!(*pRWHooks)[i]->isSession()))
      {
         if ((*pRWHooks)[i]->checkFile(name, rc))
         {
            regHook = (*pRWHooks)[i];
            break;
         }
      }
   }
  
   ASSERT(regHook != NULL);
   if (regHook != NULL)
   {
      const char *s = 0;
      fileReader = regHook->openRead(name, rc);

      switch (rc)
      {
         case DT_OK:
            break;

         case DT_InvalidOperation:
            s = "Module was unable to perform requested operation.";
            break;

         case DT_InvalidFormat:
            s = "Invalid file format. File will not be imported.";
            break;

         case DT_OutOfMemory:
            s = "Not enough memory to open file.";
            break;

         case DT_RequiredResourceNotAvailable:
            s = "Required library (or another resource) is not available.";
            break;

         case DT_UnableToOpenFile:
            s = "The selected file could not be found.";
            break;

         case DT_FileMalformed:
            s = "File appears to be damaged.";
            break;

         case DT_WrongChannelCount:
            s = "Number of audio channels is different than two (stereo).";
            break;

         case DT_WrongFrequency:
            s = "Audio frequency is different than 44.1kHz.";
            break;

         case DT_WrongResolution:
            s = "Audio resolution is different than 16bits per sample.";
            break;

         case DT_FileFormatNotSupported:
            s = "This particular file format (i.e. compression) is not supported.";
            break;

      }

      if (rc != DT_OK)
      {
         request(regHook->getName(), "An error occured while opening the file:\n%s\nOperation will be aborted.", "Ok", ARRAY((IPTR)s));
      }
   }
}

Track::Track(const Track &src)
{
   EDtError rc = DT_OK;
   trackName = src.trackName;
   regHook = src.regHook;
   fileReader = regHook->openRead(trackName, rc);
}

Track::~Track()
{
   regHook = 0;

   if (fileReader)
      fileReader->dispose();
   fileReader = 0;
}

bool Track::isValid() const
{
   return (0 != fileReader) ? true : false;
}

const char *Track::getTrackName() const
{
   return trackName.Data();
}

const char *Track::getModuleName() const
{
   return regHook->getName();
}

bool Track::fillOptItem(IOptItem *item) const
{
   if (isValid())
      return fileReader->fillOptItem(item);
   return false;
}

IFileReader *Track::getInterface()
{
   return fileReader;
}

void Track::dispose()
{
   delete this;
}

ITrack *Track::getClone() const
{
   return new Track(*this);
}

bool Track::update()
{
   // nothing specific to be done here
   return true;
}

void Track::setUp()
{
   fileReader->setUp();
}

void Track::cleanUp()
{
   fileReader->cleanUp();
}

int Track::read(const IOptItem *optItem, void* mem, const Hook* hk) const
{
   HookAttrT<void*, uint> hook(hk);
   
   uint16 cnt = optItem->getPacketSize();
   uint32 total = optItem->getDataBlockCount();
   
   for (uint32 i=0; i<total;)
   {
      cnt = cnt < (total - i) ? cnt : (total - i);

      fileReader->readData(optItem, mem, cnt);
      if (false == hook(mem, cnt))
         break;
      i += cnt;
   }
   return 0;
}

uint16 Track::getBlockSize() const
{
   return fileReader->getBlockSize();
}

uint32 Track::getBlockCount() const
{
   return fileReader->getBlockCount();
}
