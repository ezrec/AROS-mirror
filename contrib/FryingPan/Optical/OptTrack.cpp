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

#include "OptSession.h"
#include "OptTrack.h"
#include "OptIndex.h"

OptTrack::OptTrack(IOptItem* parent)
   : OptItem(parent)
{
}

EItemType       OptTrack::getItemType() const
{
   return Item_Track;
}

void            OptTrack::setItemType(EItemType lNewType)
{
   ASSERT(false);
}

uint32          OptTrack::getDiscID() const
{
   return 0;
}

void            OptTrack::setDiscID(uint32 id)
{
   ASSERT(false);
}

IOptItem       *OptTrack::addChild()
{
   IOptItem *idx = new OptIndex(this);

   return OptItem::addChild(idx);
}

