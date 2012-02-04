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

#include "OptIndex.h"

OptIndex::OptIndex(IOptItem* parent)
   : OptItem(parent)
{
   setComplete(true);
}

EItemType       OptIndex::getItemType() const
{
   return Item_Index;
}

void            OptIndex::setItemType(EItemType lNewType)
{
   ASSERT(false);
}

uint32          OptIndex::getDiscID() const
{
   return 0;
}

void            OptIndex::setDiscID(uint32 id)
{
   ASSERT(false);
}

IOptItem       *OptIndex::addChild()
{
   ASSERTS(false, "Indices may not have children");
   return 0;
}

EDataType       OptIndex::getDataType() const
{
   return getParent()->getDataType();
}

void            OptIndex::setDataType(EDataType lNewType)
{
   ASSERT(false);
}

bool            OptIndex::isProtected() const
{
   return getParent()->isProtected();
}

void            OptIndex::setProtected(bool bProtected)
{
   ASSERT(false);
}

bool            OptIndex::isPreemphasized() const
{
   return getParent()->isPreemphasized();
}

void            OptIndex::setPreemphasized(bool bPreemph)
{
   ASSERT(false);
}

bool            OptIndex::isIncremental() const
{
   return getParent()->isIncremental();
}

void            OptIndex::setIncremental(bool bState)
{
   ASSERT(false);
}

bool            OptIndex::hasCDText() const
{
   return false;
}

void            OptIndex::setCDText(bool bCDText)
{
   ASSERT(false);
}

const char*     OptIndex::getCDTTitle() const
{
   return 0;
}

void            OptIndex::setCDTTitle(const char* sNewTitle)
{
   ASSERT(false);
}

const char*     OptIndex::getCDTArtist() const
{
   return 0;
}

void            OptIndex::setCDTArtist(const char* sNewArtist)
{
   ASSERT(false);
}

const char*     OptIndex::getCDTMessage() const
{
   return 0;
}

void            OptIndex::setCDTMessage(const char* sNewMessage)
{
   ASSERT(false);
}

const char*     OptIndex::getCDTLyrics() const
{
   return 0;
}

void            OptIndex::setCDTLyrics(const char* sNewLyrics)
{
   ASSERT(false);
}

const char*     OptIndex::getCDTComposer() const
{
   return 0;
}

void            OptIndex::setCDTComposer(const char* sNewComposer)
{
   ASSERT(false);
}

const char*     OptIndex::getCDTDirector() const
{
   return 0;
}

void            OptIndex::setCDTDirector(const char* sNewDirector)
{
   ASSERT(false);
}

bool            OptIndex::isBlank() const
{
   return getParent()->isBlank();
}

void            OptIndex::setBlank(bool blank)
{
   ASSERT(false);
}

uint16          OptIndex::getSectorSize() const
{
   return getParent()->getSectorSize();
}

void            OptIndex::setSectorSize(uint16 size)
{
   ASSERT(false);
}

uint16          OptIndex::getPacketSize() const
{
   return getParent()->getPacketSize();
}

void            OptIndex::setPacketSize(uint16 NewSize)
{
   ASSERT(false);
}
    
