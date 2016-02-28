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

#include "wSplitISOData.h"
#include <Generic/LibrarySpool.h>
#include <libclass/dos.h>
#include <libclass/exec.h>
#include <libclass/utility.h>
#include <exec/lists.h>
#include <Generic/HookT.h>
#include <Generic/SumMD5.h>
#include <Generic/String.h>
#include <Generic/XMLDocument.h>

#define MAX_FILE_SIZE (1024*1024*1024)
#define CHECK_FIRST_CNT 64

IFileWriter         *wSplitISOData::openWrite(const char* sFile, const IOptItem *di, EDtError &rc)
{
   wSplitISOData *pSkel = 0;
   pSkel = new wSplitISOData(sFile, di, rc);
   return pSkel;
}

wSplitISOData::wSplitISOData(const char *sName, const IOptItem *di, EDtError &rc)
{
   name_base      = sName;
   current_ext    = 0;
   current_file   = 0;
   master_file    = 0;

   if (name_base.RightString(4) == ".sii")
   {
      name_base = name_base.LeftString(name_base.Length()-4);
   }

   rc = DT_OK;
}

wSplitISOData::~wSplitISOData()
{
   cleanUp();

   for (int i=0; i<names.Count(); i++)
      delete names[i];
}

const char          *wSplitISOData::static_getName()
{
   return "Split ISO Data Track";
}

bool                 wSplitISOData::static_isAudio()
{
   return false;
}

bool                 wSplitISOData::static_isData()
{
   return true;
}

bool                 wSplitISOData::static_isSession()
{
   return false;
}

bool                 wSplitISOData::isAudio()
{
   return static_isAudio();
}

bool                 wSplitISOData::isData()
{
   return static_isData();
}

const char          *wSplitISOData::getName()
{
   return static_getName();
}

bool                 wSplitISOData::writeData(void* pBuff, int pLen)
{
   uint32 res;
   uint32 status;

   ASSERT(pBuff != 0);
   ASSERT(pLen != 0);

   if ((pBuff == 0) || (pLen == 0))
      return false;

   if (current_file == 0)
   {
      String *sn = new String();
      sn->FormatStr("%s.%03ld", ARRAY((IPTR)name_base.Data(), current_ext));
      
      names << sn;
      current_file = DOS->Lock(sn->Data(), ACCESS_READ);
      if (current_file != 0)
      {
         DOS->UnLock(current_file);
         current_file = 0;

         res = request("Warning", "Requested to write to file\n%s\nbut same entry already exists.\nDo you want to overwrite existing entry?", "Yes|No", ARRAY((IPTR)sn->Data()));
         if (res == 0)
         {
            return false;
         }
      }
      current_file = DOS->Open(sn->Data(), MODE_NEWFILE);
      if (current_file == 0)
      {
         return false;
      }
   }

   res = max_part_size - current_size;
   res = (uint32)pLen < res ? (uint32)pLen : res;

   ASSERT(current_file != 0);
   if (current_file == 0)
      return false;

   status = DOS->Write(current_file, pBuff, res * sector_size);

   if (status != res*sector_size)
      return false;
   
   current_size += res;
   num_sectors  += res;

   if (current_size >= max_part_size)
   {
      if (current_file != 0)
         DOS->Close(current_file);
      sizes << current_size;
      current_file = 0;
      current_size = 0;
      current_ext++;
   }
   pLen -= res;

   if (pLen > 0)
   {
      return writeData(&((char*)pBuff)[res * sector_size], pLen);
   }

   return true;
}

bool                 wSplitISOData::setUp()
{
   String s;
   
   s.FormatStr("%s.sii", ARRAY((int32)name_base.Data()));
   master_file = DOS->Open(s.Data(), MODE_NEWFILE);
   ASSERT(master_file != 0);
   if (master_file == 0)
      return false;

   current_file = 0;
   current_size = 0;
   current_ext  = 0;
   num_sectors  = 0;
  
   return true;
}

void                 wSplitISOData::cleanUp()
{
   XMLDocument xdoc("SplitISOInfo");

   if (current_file)
      DOS->Close(current_file);

   sizes << current_size;

   if (master_file)
   {
      uint32 sums[4];

      xdoc.AddAttribute("NumParts", names.Count());
      xdoc.AddAttribute("SectorSize", sector_size);
      xdoc.AddAttribute("SectorCount", num_sectors);

      for (int32 i=0; i<names.Count(); i++)
      {
         if (generateFileSum(names[i]->Data(), sums))
         {
            XMLElement *elem = new XMLElement("Part", 0);

            String x;
            x.FormatStr("%08lx%08lx%08lx%08lx", ARRAY(sums[0], sums[1], sums[2], sums[3]));
            elem->AddAttribute("MD5Sum", x.Data());
            elem->AddAttribute("PartNumber", i);
            elem->AddAttribute("PartSize", sizes[i]);
            elem->AddAttribute("PartName", names[i]->Data());
         
            xdoc.AddChild(elem);
         }
      }

      xdoc.WriteXML(master_file);
      DOS->Close(master_file);
   }

   current_file = 0;
   master_file  = 0;

   for (int i=0; i<names.Count(); i++)
      delete names[i];
   names.Empty();
   sizes.Empty();
}
   
void                 wSplitISOData::deleteFiles()
{
   if (current_file)
      DOS->Close(current_file);
   if (master_file)
      DOS->Close(master_file);

   current_file = 0;
   master_file  = 0;

   for (int i=0; i<names.Count(); i++)
      DOS->DeleteFile(names[i]->Data());

   String s;
   s.FormatStr("%s.sii", ARRAY((int32)name_base.Data()));
   DOS->DeleteFile(s.Data());
}

bool                 wSplitISOData::generateFileSum(const char* name, uint32 sums[4])
{
   uint8   *mem = new uint8[CHECK_FIRST_CNT * sector_size];
   SumMD5   sum;
   BPTR     file;
   int32    len;
   bool     res = false;

   ASSERT(mem != 0);
   if (mem != 0)
   {
      file = DOS->Open((char*)name, MODE_OLDFILE);
      ASSERT(file != 0);
      if (file != 0)
      {
         memset(mem, 0, CHECK_FIRST_CNT * sector_size);
         len = DOS->Read(file, mem, CHECK_FIRST_CNT * sector_size);
         if (len != 0)
         {
            sum.Initialize();
            sum.Update(mem, len);
            sum.Finalize();
            sum.GetSum(sums);
            res = true;
         }
         DOS->Close(file);
      }
      delete [] mem;
   }
   return res;
}

void                 wSplitISOData::dispose()
{
   delete this;
}

void                 wSplitISOData::setBlockCount(unsigned long)
{
}

void                 wSplitISOData::setBlockSize(unsigned short size)
{
   sector_size    = size;
   max_part_size  = MAX_FILE_SIZE / size;
}

// vim: ts=3 et
