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

#include "FileWriter.h"
#include <libclass/dos.h>
#include <Optical/Optical.h>
#include <LibC/LibC.h>
#include <libclass/utility.h>

FileWriter::FileWriter(const char* sFName, EDtError &rc)
{
   sFileName   = sFName;
   fh          = 0;
   isLE        = true;
   sectorSize  = 2352;

   fh = DOS->Open(const_cast<char*>(getFileName()), MODE_NEWFILE);
   if (fh == 0)
      rc = DT_UnableToOpenFile;
   else
      rc = DT_OK;
}

FileWriter::~FileWriter()
{
}

BPTR                       FileWriter::getFile()
{
   return fh;
}

const char*                FileWriter::getFileName()
{
   return sFileName.Data();
}

bool                       FileWriter::writeData(void* buffer, int len)
{
   ASSERT(buffer != 0);
   ASSERT(fh != 0);

   if (fh == 0)
      return false;

   if (NULL == buffer)
      return false;

   if (0 == len)
      return false;

   long k;
   short *b = (short*)buffer;

   if (false == isLE)
   {
      k = len * sectorSize;
      while (k) 
      {
         *b = ((*b&0xff00)>>8)|((*b&0xff)<<8);
         b++;
         k-=2;
      }
   }

   k = DOS->Write(fh, buffer, len*sectorSize)/sectorSize;
   
   if (k != len)
      return false;
    
   return true;
}

void                       FileWriter::setLittleEndian(bool isLittle)
{
   isLE = isLittle;
}

bool                       FileWriter::setUp()
{
   ASSERT(fh != 0);
   if (fh == 0)
      return false;
   return true;
}

void                       FileWriter::cleanUp()
{
   if (0 != fh)
      DOS->Close(fh);
}

const char                *FileWriter::getTrackName()
{
    return getFileName();
}

void                       FileWriter::dispose()
{
    delete this;
}

uint16                     FileWriter::getBlockSize()
{
   return sectorSize;
}

uint32                     FileWriter::getBlockCount()
{
   return sectorCount;
}

void                       FileWriter::setBlockSize(unsigned short size)
{
   sectorSize = size;
}

void                       FileWriter::setBlockCount(unsigned long count)
{
   sectorCount = count;
}

void                       FileWriter::deleteFiles()
{
   if (fh != 0)
      DOS->Close(fh);
   fh = 0;

   DOS->DeleteFile((char*)getFileName());
}


// vim: ts=3 et
