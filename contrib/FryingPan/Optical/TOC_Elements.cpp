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

#include "Headers.h"
#include "Commands.h"

TOC_Entry *TOC_FullTOC::FindTOCEntry(int32 item)
{
   int32 i;
   int32 size;

   size = (length-2)/11;      // yup...
   for (i=0; i<size; i++) {
      if (elem[i].GetTrackNumber() == item) return &elem[i];
   }

   return 0;
};

int32 TOC_FullTOC::GetNumTracks(void)
{
   int32 i;

   for (i=1; i<100; i++) {
      if (!FindTOCEntry(i)) break;
   }
   return i-1;
};

void TOC_FullTOC::FillInDiscItem(IOptItem *di, int32 track)
{
   TOC_Entry *e, *f;

   e = FindTOCEntry(track);
   f = FindTOCEntry(track+1);
   if (!e) 
   {
      di->setDataType(Data_Unknown);
   }
   else
   {
      di->setItemNumber(e->GetTrackNumber());
      di->setDataType(e->GetTrackType());
      di->setStartAddress(e->GetTrackPositionLBA());
      di->setEndAddress(f ? f->GetTrackPositionLBA()-1 : GetDiscSize()-1);
      di->setSectorSize(e->GetTrackSectorSize());
      di->setProtected(e->IsProtected());
      di->setPreemphasized(e->IsPreemphasized());
      di->setPacketSize(16);
   }
}

void TrackInfo::FillInDiscItem(IOptItem *di)
{
   di->setItemNumber(GetTrackNumber());
   di->setDataType(GetTrackType());
   di->setStartAddress(GetStartAddress());
   di->setEndAddress(GetEndAddress());
//   di->setActualSize(GetActualSize());
   di->setSectorSize(GetSectorSize());
   di->setProtected(IsProtected());
   di->setPreemphasized(IsPreemphasized());
   di->setIncremental(IsIncremental());
   di->setBlank(IsBlank());
   di->setPacketSize(GetPacketSize());
   di->setComplete(!((IsBlank() == 0) && (IsIncremental()) && (GetFreeBlocks() != 0)));
}

void TOC_PrimitiveTOC::FillInDiscItem(IOptItem *di, int32 track)
{
   TOC_PrimitiveEntry *e, *f;

   e = FindTOCEntry(track);
   f = FindTOCEntry(track+1);
   if (!e) 
   {
      di->setDataType(Data_Unknown);

   } 
   else 
   {
      di->setItemNumber(e->GetTrackNumber());
      di->setDataType(e->GetTrackType());
      di->setStartAddress(e->GetTrackPositionLBA());
      di->setEndAddress(f ? f->GetTrackPositionLBA()-1 : GetDiscSize()-1);
      di->setSectorSize(e->GetTrackSectorSize());
      di->setProtected(e->IsProtected());
      di->setPreemphasized(e->IsPreemphasized());
      di->setPacketSize(16);
   }
}

TOC_PrimitiveEntry *TOC_PrimitiveTOC::FindTOCEntry(int32 item)
{
   int32 i;
   int32 size;

   size = GetNumElements();
   for (i=0; i<size; i++) {
      if (elem[i].GetTrackNumber() == item) return &elem[i];
   }

   return 0;
};

int32 TOC_PrimitiveTOC::GetNumTracks(void)
{
   int32 i;

   for (i=1; i<100; i++) {
      if (!FindTOCEntry(i)) break;
   }
   return i-1;
};

int32 TOC_FullTOC::GetDiscSize(void)
{  
   TOC_Entry *pTOC = FindTOCEntry(0xA2);
   if (!pTOC)
      return 0;
   return pTOC->GetTrackPositionLBA();
}



TList<String*> *TOC_CDText::MergeBlocks(int32 id)
{
   int32            i;
   int32            count;
   char          *block;
   TList<String*>*pArr = new TList<String*>;

   pArr->FreeOnDelete(true);

   for (count=0, i=0; i<NumCDTextBlocks(); i++) {
      if (items[i].block_id == id) count++;
   }

   if (!count) return pArr;
   block = new char[12*count+1]; 
   count = 0;

   for (i=0; i<NumCDTextBlocks(); i++) {
      if (items[i].block_id == id) {
         Exec->CopyMem(&items[i].cdtextinfo, &block[count*12], 12);
         count++;
      }
   }

   for (i=0; i<(count*12); i++) {
      pArr->Add(new String(&block[i]));
      while (block[i]!=0) i++;
   }

   delete [] block;
   return pArr;
}

TList<String*> *TOC_CDText::GetTitles(void)
{
   return MergeBlocks(0x80);
}

TList<String*> *TOC_CDText::GetPerformers(void)
{
   return MergeBlocks(0x81);
}

TList<String*> *TOC_CDText::GetSongWriters(void)
{
   return MergeBlocks(0x82);
}

TList<String*> *TOC_CDText::GetComposers(void)
{
   return MergeBlocks(0x83);
}

TList<String*> *TOC_CDText::GetArrangers(void)
{
   return MergeBlocks(0x84);
}

TList<String*> *TOC_CDText::GetMessages(void)
{
   return MergeBlocks(0x85);
}
