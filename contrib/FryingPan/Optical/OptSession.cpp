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

#include "OptDisc.h"
#include "OptTrack.h"

OptSession::OptSession(IOptItem *parent)
   : OptItem(parent)
{
}

EItemType       OptSession::getItemType() const
{
   return Item_Session;
}

void            OptSession::setItemType(EItemType lNewType)
{
   ASSERT(false);
}

EDataType       OptSession::getDataType() const
{
   return Data_Unknown;
}

void            OptSession::setDataType(EDataType lNewType)
{
   ASSERT(false);
}

uint16          OptSession::getPreGapSize() const
{
   return 0;
}

void            OptSession::setPreGapSize(uint16 lNewSize)
{
   ASSERT(false);
}

uint16          OptSession::getPostGapSize() const
{
   return 0;
}

void            OptSession::setPostGapSize(uint16 lNewSize)
{
   ASSERT(false);
}

int32           OptSession::getStartAddress() const
{
   if (getChildCount() == 0)
      return 0;

   return getChild(0)->getStartAddress();
}

void            OptSession::setStartAddress(int32 lNewAddress)
{
   ASSERT(false);
}

int32           OptSession::getEndAddress() const
{
   if (getChildCount() == 0)
      return 0;

   return getChild(-1)->getEndAddress();
}

void            OptSession::setEndAddress(int32 lNewAddress)
{
   ASSERT(false);
}

int32           OptSession::getDataBlockCount() const
{
   int32 db=0;

   for (int i=0; i<getChildCount(); i++)
   {
      db += getChild(i)->getDataBlockCount();
   }

   return db;
}

void            OptSession::setDataBlockCount(int32 lNewSize)
{
   ASSERT(false);
}

void            OptSession::setProtected(bool bProtected)
{
   ASSERT(false);
}

void            OptSession::setPreemphasized(bool bPreemph)
{
   ASSERT(false);
}

uint32          OptSession::getDiscID() const
{
   return 0;
}

void            OptSession::setDiscID(uint32 id) 
{
   ASSERT(false);
}

uint16          OptSession::getFlags() const
{
   return OptItem::getFlags();
}

void            OptSession::setFlags(uint16 lNewFlags)
{
   OptItem::setFlags(lNewFlags);
}

IOptItem       *OptSession::addChild()
{
   OptTrack *trak = new OptTrack(this);
   return OptItem::addChild(trak);
}

