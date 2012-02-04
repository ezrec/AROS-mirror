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
#include "Disc_Item.h"


#pragma obsolete

Disc_Item::Disc_Item()
{
   title       = new String();
   artist      = new String();
   composer    = new String();
   lyrics      = new String();
   director    = new String();
   message     = new String();

   di_Title          = title->Data();
   di_Artist         = artist->Data();
   di_Composer       = composer->Data();
   di_Director       = director->Data();
   di_Lyrics         = lyrics->Data();
   di_Message        = message->Data();
   di_Next           = 0;
   di_ItemType       = Item_Track;
   di_Session        = 0;
   di_Track          = 0;
   di_Index          = 0;
   di_Flags          = 0;
   di_DataType       = Data_Unknown;
   di_Gap            = 0;
   di_Start          = 0;
   di_End            = 0;
   di_Actual         = 0;
   di_SectorSize     = 0;
   di_Prot           = 0;
   di_Preemph        = 0;
   di_Incremental    = 0;
   di_Blank          = 0;
   di_CDText         = 0;
   di_Pad            = 0;
   di_PacketLength   = 0;
   di_DiscID         = 0;
};

Disc_Item::~Disc_Item()
{
   delete title;
   delete artist;
   delete composer;
   delete lyrics;
   delete director;
   delete message;
};

DiscItem *Disc_Item::GetData(void)
{
   return this;
};
