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
#include "Disc_DVD_ROM.h"

Disc_DVD_ROM::Disc_DVD_ROM(Drive *d) : 
   Disc(d),
   rdvd(dio)
{
};

void Disc_DVD_ROM::Init(void)
{
   Disc::Init();
   
   const IOptItem *disc = GetContents();

   for (int i=0; i<disc->getChildCount(); i++)
   {
      const IOptItem *sess = disc->getChild(i);
      const_cast<IOptItem*>(sess)->setSectorSize(2048);
   }

   cmd_GetConfiguration::feat_ControlBlocks *feat;
   feat = (cmd_GetConfiguration::feat_ControlBlocks*)drive->GetDriveFeature(cmd_GetConfiguration::Feature_DiscControlBlocks);
   dcbs.Empty();
   if (feat != 0)
   {
      for (int i=0; i<feat->GetBodyLength(); i+=4)
      {
         uint32 f = feat->features[i/4];
         _D(Lvl_Info, "Media offers control block: %08lx <%lc%lc%lc>", f, (f>>24)&255, (f>>16)&255, (f>>8)&255);
         dcbs << f;
      }
   }

   cb = rdvd.ReadStructure(cmd_ReadDVDStructure::DVD_DiscControlBlocks, 0xffffffff, 0);
};

Disc_DVD_ROM::~Disc_DVD_ROM(void)
{
};

int Disc_DVD_ROM::CheckItemData(const IOptItem *)
{
   _D(Lvl_Info, "Inserted disc is not writable!");
   return ODE_DiscFull;
}

void Disc_DVD_ROM::FillDiscSpeed(DiscSpeed &spd)
{
   spd.f = (((int32)10*spd.kbps)/1385)%10;
   spd.i = ((int32)spd.kbps)/1385;
}

