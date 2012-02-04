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

#include <Generic/LibrarySpool.h>
#include "ClFile.h"
#include "ClName.h"
#include <libclass/dos.h>
#include <dos/exall.h>
#include <Generic/HookAttrT.h>

ClFile::ClFile(ClRoot *pRoot, ClDirectory *pParent, const ExAllData *ed)
   : ClElement(pRoot, pParent)
{
   struct DateStamp hDS;
   
   hDS.ds_Days    = ed->ed_Days;
   hDS.ds_Minute  = ed->ed_Mins;
   hDS.ds_Tick    = ed->ed_Ticks;
   
   setISOSize(ed->ed_Size);
   setJolietSize(ed->ed_Size);
   setNormalName((char*)ed->ed_Name);
   setComment((char*)ed->ed_Comment);
   setDate((struct DateStamp*)&hDS);
   setProtection(ed->ed_Prot);
}

ClFile::ClFile(ClRoot *pRoot, ClDirectory *pParent, const FileInfoBlock *fib)
   : ClElement(pRoot, pParent)
{
   setISOSize(fib->fib_Size);
   setJolietSize(fib->fib_Size);
   setNormalName((char*)fib->fib_FileName);
   setComment((char*)fib->fib_Comment);
   setDate(&fib->fib_Date);
   setProtection(fib->fib_Protection);
}

ClFile::~ClFile()
{
}

bool ClFile::isDirectory()
{
   return false;
}

void ClFile::setISOPosition(unsigned long lPos)
{
   if (isISOEntry())
   {
      ClElement::setISOPosition(lPos);
      if (isJolietEntry())
         ClElement::setJolietPosition(lPos);
   }
}

void ClFile::setISOSize(unsigned long lSize)
{
   if (isISOEntry())
   {
      ClElement::setISOSize(lSize);
      if (isJolietEntry())
         ClElement::setJolietSize(lSize);
   }
}

bool ClFile::getData(const Hook *h, void* mem, uint32 len)
{
   unsigned long  size  = 0;
   BPTR           fh    = 0;
   unsigned long  bs    = 0;
   unsigned long  res   = 0;
   bool           bRes  = true;

   if (isISOEntry())
   {
      size = getISOSize();
   }
   else
   {
      size = getJolietSize();
   }

   if (size == 0) 
      size++;
   size = (size + 2047) &~ 2047;

   bs = size < len ? size : len;

   if ((~getProtection()) & 8)
      fh = DOS->Open(getPath().Data(), MODE_OLDFILE);

   //MessageBox("Info", "Creating file %s - length %ld, fh: %lx, block size: %ld", "Ok", ARRAY((uint32)getPath().Data(), getISOSize(), (uint32)fh, bs));

   while (size > 0)
   {
      // ok so we may have an empty file here or
      // a file whose size != n*2048, whatever
      // even no file at all (open failed).

      if (0 != fh)
         res = DOS->Read(fh, mem, size < bs ? size : bs);
      else
         res = 0;

      if (!res)
         res = size < bs ? size : bs;

      res += 2047;
      res &= ~2047;

      bRes  = HookAttrT<void*, sint>::Call(h, mem, res);
      if (false == bRes)
         break;

      size -= res;
   }

   DOS->Close(fh);
   return bRes;
}


