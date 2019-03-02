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

#include "FileReader.h"
#include <libclass/dos.h>
#include <Optical/IOptItem.h>
#include <LibC/LibC.h>
#include <libclass/utility.h>

FileReader::FileReader(const char* sFName, EDtError &rc)
{
   _ND("Reader");
   _D(Lvl_Info, "%s: Setting up defaults", (int)__PRETTY_FUNCTION__);
   sFileName = sFName;
   fh = 0;
   position    = 0;
   size        = 0;
   offset      = 0;
   isLE        = true;
   sectorSize  = 2352;
   rc          = DT_OK;

   _D(Lvl_Info, "%s: accessing file", (int)__PRETTY_FUNCTION__);
   fh = DOS->Open(const_cast<char*>(getFileName()), MODE_OLDFILE);
   ASSERT(fh != 0);
   if (0 != fh)
   {
      _D(Lvl_Debug, "File opened OK");
      FileInfoBlock *fFib = new FileInfoBlock;
      if (DOS->ExamineFH(fh, fFib))
      {
         // problems with 64 bit (the size is signed -> problems with 32bit values)
         uint32 tmp = fFib->fib_Size;
         size = (unsigned long long)tmp;
         _D(Lvl_Info, "File analysed, estimated size: %lu bytes (%08lx:%08lx)", size, (size >> 32)&0xffffffff, size & 0xffffffff);
      }
      else
      {
         size = 0;
         rc = DT_UnableToOpenFile;
      }
      delete fFib;
   }
   else
   {
      _D(Lvl_Debug, "File could not be opened");
      rc = DT_UnableToOpenFile;
   }
}

FileReader::~FileReader()
{
   _D(Lvl_Info, "%s: Killing item", (int)__PRETTY_FUNCTION__);
   _ED();
   if (0 != fh)
   {
      DOS->Close(fh);
   }
}

BPTR                       FileReader::getFile()
{
   _D(Lvl_Info, "%s: asked for a file handle (%08lx)", (int)__PRETTY_FUNCTION__, (int)fh);
   return fh;
}

const char*                FileReader::getFileName()
{
   return sFileName.Data();
}

bool                       FileReader::readData(const IOptItem*, void* buffer, int len)
{
   ASSERT(buffer != 0);
   ASSERT(fh != 0);
   
   short *b = (short*)buffer;
   uint64 k;


   if (0 >= len)
      return true;

   if (0 == buffer)
   {
      return false;
   }

   k = udiv6432(size, sectorSize) - position;      // see how much left do we have
   
   _D(Lvl_Info, "%s: current file: %lx, buffer: %lx, length: %lds, position: %lds, total: %lds", 
         (int)__PRETTY_FUNCTION__, 
         (int)fh, 
         (int)buffer, 
         len, 
         position, 
         udiv6432(size, sectorSize));
   
   if (k>=(unsigned)len)                            // if still more than requested
      k = len;                            // read only as much as they need
   else                                   // otherwise
      memset(buffer, 0, len*sectorSize);  // clear memory
                                          // and read as much as we have

   k *= sectorSize;

   if (0 != k)
   {
      if (fh)
         k = DOS->Read(fh, buffer, k);
      else
         k = 0;

      k &= ~1;
      if (k < (unsigned)(len * sectorSize))
         memset(&((char*)buffer)[k>>1], 0, (len*sectorSize) - k);
   }
   position += len;

   if (k <= 0)
      return true;

   ASSERT(b == buffer);

   if (false == isLE)
   {
      while (k > 0) 
      {
         *b = ((*b&0xff00)>>8)|((*b&0xff)<<8);
         k-=2;
         b++;
      }
   }
   return true;
}

void                       FileReader::setBlockSize(unsigned short size)
{
   ASSERT(size != 0);
   _D(Lvl_Info, "%s: New sector size: %ldb", 
         (int)__PRETTY_FUNCTION__,
         size);
   sectorSize = size;
}

void                       FileReader::setLittleEndian(bool isLittle)
{
   _D(Lvl_Info, "%s: module %s byte swappning", 
         (int)__PRETTY_FUNCTION__,
         isLittle ? (int)"does not require": (int)"requires");
   isLE = isLittle;
}

bool                       FileReader::setUp()
{
   ASSERT(fh != 0);

   DOS->Seek(fh, offset, OFFSET_BEGINNING);
   _D(Lvl_Info, "%s: set up complete. data offset: %ld bytes",
         (int)__PRETTY_FUNCTION__,
         offset);
   position = 0;
   return true;
}

void                       FileReader::cleanUp()
{
   _D(Lvl_Info, "%s: cleanup complete", (int)__PRETTY_FUNCTION__);
}

bool                       FileReader::checkFile(const char* sFileName, EDtError &rc)
{
   BPTR fh;

   rc = DT_UnableToOpenFile;
   fh = DOS->Open(const_cast<char*>(sFileName), MODE_OLDFILE);
   if (0 != fh)
   {
      DOS->Close(fh);
      rc = DT_OK;
      return true;
   }
   return false;
}

void                       FileReader::setType(EDataType aType)
{
   _D(Lvl_Info, "%s: Setting new data type to %ld", (int)__PRETTY_FUNCTION__, aType);
   dataType = aType;
}

bool                       FileReader::fillOptItem(IOptItem *item)
{
   _D(Lvl_Info, "%s: Filling up disc item", (int)__PRETTY_FUNCTION__);
   item->setDataType(dataType);
   item->setDataBlockCount(getBlockCount());

   return true;
// FIXME: malo tu wypelniamy
}

void                       FileReader::setDataOffset(uint64 lOffset)
{
   _D(Lvl_Info, "%s: new data offset: %ld",
         (int)__PRETTY_FUNCTION__,
         lOffset);
   offset = lOffset;
}

void                       FileReader::setDataSize(uint64 lSize)
{
   _D(Lvl_Info, "%s: new data size: %lu (%08lx:%08lx)",
         (int)__PRETTY_FUNCTION__,
         lSize,
         lSize >> 32,
         lSize & 0xffffffff);
   size = lSize;
}

const char                *FileReader::getTrackName()
{
   _D(Lvl_Info, "%s: Asking for track name (%s)", (int)__PRETTY_FUNCTION__, (int)getFileName());
    return getFileName();
}

void                       FileReader::dispose()
{
   _D(Lvl_Info, "%s: closing current track.", (int)__PRETTY_FUNCTION__);
   delete this;
}

uint32                     FileReader::getBlockCount()
{
   ASSERT(getBlockSize() != 0);
   _D(Lvl_Info, "%s: assuming size=%lub, sector=%ldb -> block count=%ld",
         (int)__PRETTY_FUNCTION__,
         size,
         getBlockSize(),
         udiv6432(size, getBlockSize()));
   return udiv6432(size, getBlockSize());
}

uint16                     FileReader::getBlockSize()
{
   _D(Lvl_Info, "%s: Reading block size (%ld)", (int)__PRETTY_FUNCTION__, sectorSize);
   return sectorSize;
}

bool                       FileReader::isAudio()
{
   _D(Lvl_Info, "%s: Is Audio ? %ld", (int)__PRETTY_FUNCTION__, dataType == Data_Audio);
   if (dataType == Data_Audio)
      return true;
   return false;
}

bool                       FileReader::isData()
{
   _D(Lvl_Info, "%s: Is Data ? %ld", (int)__PRETTY_FUNCTION__, dataType != Data_Audio);
   if (dataType != Data_Audio)
      return true;
   return false;
}

// vim: ts=3 et
