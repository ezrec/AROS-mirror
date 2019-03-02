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

#include "rSplitISOData.h"
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <exec/lists.h>
#include <Generic/HookT.h>
#include <Generic/XMLDocument.h>
#include <Generic/SumMD5.h>

#define CHECK_FIRST_CNT 64

IFileReader         *rSplitISOData::openRead(const char* sFile, EDtError &rc)
{
   rSplitISOData *pSkel = 0;
   if (true == checkFile(sFile, rc))
   {
      pSkel = new rSplitISOData(sFile, rc);
   }
   return pSkel;
}
   
rSplitISOData::rSplitISOData(const char *sName, EDtError &rc)
{
   rc = DT_OK;
   sFileName = sName;
   memset(&item, 0, sizeof(item));

   sector_size    = 0;
   sector_count   = 0;
   num_parts      = 0;
   current_file   = 0;

   if (false == readDescriptor(rc))
   {
      sector_count = 0;
      sector_size  = 0;
   }
}

rSplitISOData::~rSplitISOData()
{
   for (int i=0; i<descs.Count(); i++)
      delete descs[i];
   cleanUp();
}

bool                 rSplitISOData::checkFile(const char* sFile, EDtError &rc)
{
   bool res = false;
   FileInfoBlock *b = new FileInfoBlock;

   BPTR l = DOS->Lock((char*)sFile, ACCESS_READ);

   if (l != 0)
   {
      if (DOS->Examine(l, b))
      {
         // our files should never grow any larger :P

         rc = DT_InvalidFormat;

         if (b->fib_Size < 4*1024)
         {
            XMLDocument xdoc;

            if (true == xdoc.ReadXML(sFile))
            {
               if (xdoc.GetName() == "SplitISOInfo")
               {
                  res = true;
                  rc = DT_OK;
               }
            }
         }
      }
      else
         rc = DT_UnableToOpenFile;

      DOS->UnLock(l);
   }
   else
      rc = DT_UnableToOpenFile;

   delete b;

   return res;
}

bool                 rSplitISOData::readDescriptor(EDtError &rc)
{
   XMLDocument    xdoc;
   XMLAttribute  *elem;

   xdoc.ReadXML(sFileName.Data());

   sector_size = 0;
   elem = xdoc.FindAttribute("SectorSize");
   if (elem != 0)
   {
      sector_size = elem->GetValue().ToLong();
   }

   sector_count = 0;
   elem = xdoc.FindAttribute("SectorCount");
   if (elem != 0)
   {
      sector_count = elem->GetValue().ToLong();
   }

   num_parts = 0;
   elem = xdoc.FindAttribute("NumParts");
   if (elem != 0)
   {
      num_parts = elem->GetValue().ToLong();
   }

   for (int i=0; i<num_parts; i++)
   {
      for (int j=0; j<xdoc.GetChildrenCount(); j++)
      {
         XMLElement *xel = xdoc.GetChild(j);

         elem = xel->FindAttribute("PartNumber");
         if ((0 != elem) && (elem->GetValue().ToLong() == i))
         {
            PartDescriptor *pdesc = new PartDescriptor;
            
            elem = xel->FindAttribute("MD5Sum");
            if (elem != 0)
               pdesc->md5sum = elem->GetValue();

            elem = xel->FindAttribute("PartName");
            if (elem != 0)
               pdesc->fileName = elem->GetValue();

            elem = xel->FindAttribute("PartSize");
            if (elem != 0)
               pdesc->sectors = elem->GetValue().ToLong();

            descs << pdesc;

            if (validatePart(pdesc) == 0)
            {
               rc = DT_FileMalformed;
               return false;
            }
         }
      }
   }

   rc = DT_OK;
   return true;
}
   
bool                 rSplitISOData::validatePart(PartDescriptor *pdesc)
{
   BPTR   fh;
   uint8 *mem;
   SumMD5 sum;
   uint32 sval[4];
   int    len;
   String sstr;
   bool   res = false;

   fh = DOS->Open(pdesc->fileName.Data(), MODE_OLDFILE);
   if (0 != fh)
   {
      mem = new uint8[CHECK_FIRST_CNT * sector_size];
      if (0 != mem)
      {
         len = DOS->Read(fh, mem, CHECK_FIRST_CNT * sector_size);
         if (len > 0)
         {
            sum.Initialize();
            sum.Update(mem, len);
            sum.Finalize();

            sum.GetSum(sval);
            sstr.FormatStr("%08lx%08lx%08lx%08lx", ARRAY(sval[0], sval[1], sval[2], sval[3]));

            if (sstr == pdesc->md5sum)
            {
               res = true;
            }
         }
         delete [] mem;
      }
      DOS->Close(fh);
   }

   return res;
}

const char          *rSplitISOData::static_getName()
{
   return "Split ISO Data Track";
}

bool                 rSplitISOData::static_isAudio()
{
   return false;
}

bool                 rSplitISOData::static_isData()
{
   return true;
}

bool                 rSplitISOData::static_isSession()
{
   return false;
}

bool                 rSplitISOData::isAudio()
{
   return static_isAudio();
}

bool                 rSplitISOData::isData()
{
   return static_isData();
}

const char          *rSplitISOData::getName()
{
   return static_getName();
}

const char*          rSplitISOData::getTrackName()
{
   return sFileName.Data();
}

bool                 rSplitISOData::readData(const IOptItem* item, void* pBuff, int pLen)
{
   int32 len;

   ASSERT(pBuff != 0);
   ASSERT(pLen != 0);
   if ((pBuff == 0) || (pLen == 0))
      return false;

   if (0 == current_file)
   {
      current_file = DOS->Open(descs[current_elem]->fileName.Data(), MODE_OLDFILE);
      current_size = descs[current_elem]->sectors;
   }

   len = current_size - current_pos;
   len = len < pLen ? len : pLen;

   ASSERT(len > 0);

   ASSERT(0 != current_file);
   if (current_file != 0)
   {
      DOS->Read(current_file, pBuff, len * sector_size);
   }
   else
   {
      return false;
   }
   
   pLen        -= len;
   current_pos += len;

   if (current_pos >= current_size)
   {
      if (current_file)
         DOS->Close(current_file);

      current_file = 0;
      current_pos  = 0;
      current_elem++;
   }
   
   if (pLen > 0)
      return readData(item, &((char*)pBuff)[len * sector_size], pLen);

   return true;
}

bool                 rSplitISOData::setUp()
{
   current_file   = 0;
   current_pos    = 0;
   current_size   = 0;
   current_elem   = 0;
   return true;
}

void                 rSplitISOData::cleanUp()
{
   if (current_file != 0)
   {
      DOS->Close(current_file);
   }
   current_file = 0;
}

void                 rSplitISOData::dispose()
{
   delete this;
}

uint32               rSplitISOData::getBlockCount()
{
   return sector_count;
}

uint16               rSplitISOData::getBlockSize()
{
   return sector_size;
}

bool                 rSplitISOData::fillOptItem(IOptItem *item)
{
   item->setDataType(Data_Mode1);
   item->setDataBlockCount(sector_count);
   item->setSectorSize(2048);

   return true;
}

// vim: ts=3 et
