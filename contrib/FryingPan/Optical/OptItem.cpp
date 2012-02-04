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

#include "OptItem.h"

using namespace GenNS;

OptItem::OptItem(IOptItem *parent)
{
   di_parent         = parent;
   di_itemtype       = Item_Disc;
   di_number         = 0;
   di_flags          = 0;
   di_datatype       = Data_Unknown;

   di_pregap         = 0;
   di_postgap        = 0;

   di_start          = 0;
   di_datasize       = 0;
   di_actual         = 0;
   di_sectorsize     = 0;
   di_opencnt        = 0;

   di_prot           = false;
   di_preemph        = false;
   di_incremental    = false;
   di_blank          = false;
   di_cdtext         = false;
   di_packetlength   = 16;

   di_discid         = 0;

   claim();
}

OptItem::~OptItem()
{
   di_children.ForEach(&sFreeChild);
}

bool           OptItem::sFreeChild(IOptItem* const& child)
{
   child->dispose();
   return true;
}

IOptItem      *OptItem::addChild(IOptItem* child)
{
   di_children << child;
   return child;
}

void           OptItem::printReport(DbgHandler *__dbg) const
{
   if (0 == __dbg)
      return;

   _D(Lvl_Info, "Item type:     %s",  (int)(getItemType() == Item_Disc      ? "Disc" :
                                              getItemType() == Item_Session   ? "Session" :
                                              getItemType() == Item_Track     ? "Track" : "Index"));
   _D(Lvl_Info, "Item number:   %ld", getItemNumber());
   _D(Lvl_Info, "Flags:         %ld", getFlags());
   _D(Lvl_Info, "Type:          %ld", getDataType());
   _D(Lvl_Info, "Start:         %ld", getStartAddress());
   _D(Lvl_Info, "End:           %ld", getEndAddress());
//   _d(Lvl_Info, "Actual:        %ld", GetActualAddress());
   _D(Lvl_Info, "Total:         %ld", getBlockCount());
   _D(Lvl_Info, "SectorSize:    %ld", getSectorSize());
   _D(Lvl_Info, "Protected?:    %ld", isProtected());
   _D(Lvl_Info, "Preemph.?:     %ld", isPreemphasized());
   _D(Lvl_Info, "Incremental?:  %ld", isIncremental());
   _D(Lvl_Info, "Blank?:        %ld", isBlank());
   _D(Lvl_Info, "Packet size:   %ld", getPacketSize());
   _D(Lvl_Info, "-----------------------");
}

   /*
    * inherited methods go here
    */

void           OptItem::claim() const
{
   const_cast<OptItem*>(this)->di_sync.Acquire();
   ++const_cast<OptItem*>(this)->di_opencnt;
   const_cast<OptItem*>(this)->di_sync.Release();
}

void           OptItem::dispose() const
{
   const_cast<OptItem*>(this)->di_sync.Acquire();
   ASSERT(di_opencnt != 0);
   --const_cast<OptItem*>(this)->di_opencnt;
   if (di_opencnt == 0)
      delete const_cast<OptItem*>(this);
   else
      const_cast<OptItem*>(this)->di_sync.Release();
}

EItemType      OptItem::getItemType() const
{
   return di_itemtype;
}

void           OptItem::setItemType(EItemType lNewType)
{
   di_itemtype = lNewType;
}

uint16         OptItem::getItemNumber() const
{
   return di_number;
}

void           OptItem::setItemNumber(uint16 number)
{
   di_number = number;
}

EDataType      OptItem::getDataType() const
{
   return di_datatype;
}

void           OptItem::setDataType(EDataType lNewType)
{
   di_datatype = lNewType;
}

uint16         OptItem::getPreGapSize() const
{
   return di_pregap;
}

void           OptItem::setPreGapSize(uint16 lNewSize)
{
   di_pregap = lNewSize;
}

uint16         OptItem::getPostGapSize() const
{
   return di_postgap;
}

void           OptItem::setPostGapSize(uint16 lNewSize)
{
   di_postgap = lNewSize;
}

int32          OptItem::getStartAddress() const
{
   if (getFlags() & DIF_RelativeSize)
   {
      return getParent()->getDataStartAddress() + di_start;
   }
   return di_start;
}

void           OptItem::setStartAddress(int32 lNewAddress)
{
   di_start = lNewAddress;
}

int32          OptItem::getEndAddress() const
{
   return getStartAddress() + getDataBlockCount() + getPreGapSize() + getPostGapSize() - 1;
}

void           OptItem::setEndAddress(int32 lNewAddress)
{
   di_datasize = lNewAddress + 1 - getPreGapSize() - getPostGapSize() - getStartAddress();
}

int32          OptItem::getBlockCount() const
{
   return (getEndAddress() + 1) - getStartAddress();
}

int32          OptItem::getDataBlockCount() const
{
   if (getFlags() & DIF_RelativeSize)
   {
      if (di_datasize == -1)
         return getParent()->getDataBlockCount();
   }
   return di_datasize;
}

void           OptItem::setDataBlockCount(int32 size)
{
   di_datasize = size;
}

int32          OptItem::getDataStartAddress() const
{
   return getStartAddress() + getPreGapSize();
}

int32          OptItem::getDataEndAddress() const
{
   return getStartAddress() + getPreGapSize() + getDataBlockCount() - 1;
}

bool           OptItem::isProtected() const
{
   return di_prot;
}

void           OptItem::setProtected(bool bProtected)
{
   di_prot = bProtected;
}

bool           OptItem::isPreemphasized() const
{
   return di_preemph;
}

void           OptItem::setPreemphasized(bool bPreemph)
{
   di_preemph = bPreemph;
}

bool           OptItem::isIncremental() const
{
   return di_incremental;
}

void           OptItem::setIncremental(bool bState)
{
   di_incremental = bState;
}

bool           OptItem::hasCDText() const
{
   return di_cdtext;
}

void           OptItem::setCDText(bool bCDText)
{
   di_cdtext = bCDText;
}

const char*    OptItem::getCDTTitle() const
{
   return di_title.Data();
}

void           OptItem::setCDTTitle(const char* sNewTitle)
{
   di_title = sNewTitle;
   if (di_title.Length() > 0)
      setCDText(true);
}

const char*    OptItem::getCDTArtist() const
{
   return di_artist.Data();
}

void           OptItem::setCDTArtist(const char* sNewArtist)
{
   di_artist = sNewArtist;
   if (di_artist.Length() > 0)
      setCDText(true);
}

const char*    OptItem::getCDTMessage() const
{
   return di_message.Data();
}

void           OptItem::setCDTMessage(const char* sNewMessage)
{
   di_message = sNewMessage;
   if (di_message.Length() > 0)
      setCDText(true);
}

const char*    OptItem::getCDTLyrics() const
{
   return di_lyrics.Data();
}

void           OptItem::setCDTLyrics(const char* sNewLyrics)
{
   di_lyrics = sNewLyrics;
   if (di_lyrics.Length() > 0)
      setCDText(true);
}

const char*    OptItem::getCDTComposer() const
{
   return di_composer.Data();
}

void           OptItem::setCDTComposer(const char* sNewComposer)
{
   di_composer = sNewComposer;
   if (di_composer.Length() > 0)
      setCDText(true);
}

const char*    OptItem::getCDTDirector() const
{
   return di_director.Data();
}

void           OptItem::setCDTDirector(const char* sNewDirector)
{
   di_director = sNewDirector;
   if (di_director.Length() > 0)
      setCDText(true);
}

const IOptItem*OptItem::getParent() const
{
   return di_parent;
}

const IOptItem*OptItem::getChild(int32 num) const
{
   return di_children[num];
}

int32          OptItem::getChildCount() const
{
   return di_children.Count();
}

bool           OptItem::isBlank() const
{
   return di_blank;
}

void           OptItem::setBlank(bool blank)
{
   di_blank = blank;
}

uint16         OptItem::getSectorSize() const
{
   return di_sectorsize;
}

void           OptItem::setSectorSize(uint16 size)
{
   di_sectorsize = size;
}

uint32         OptItem::getDiscID() const
{
   return di_discid;
}

void           OptItem::setDiscID(uint32 id)
{
   di_discid = id;
}

uint16         OptItem::getPacketSize() const
{
   return di_packetlength;
}

void           OptItem::setPacketSize(uint16 lNewSize)
{
   di_packetlength = lNewSize;
}
    
uint16          OptItem::getFlags() const
{
   return di_flags;
}

void           OptItem::setFlags(uint16 lNewFlags)
{
   di_flags = lNewFlags;
}

void           OptItem::setComplete(bool cmpl)
{
   di_complete = cmpl;
}

bool           OptItem::isComplete() const
{
   return di_complete;
}

