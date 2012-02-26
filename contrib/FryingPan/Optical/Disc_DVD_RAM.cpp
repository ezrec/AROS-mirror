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
#include "Disc_DVD_RAM.h"

Disc_DVD_RAM::Disc_DVD_RAM(Drive *d) : Disc_DVD_PlusRW(d)
{
};

Disc_DVD_RAM::~Disc_DVD_RAM(void)
{
};

void Disc_DVD_RAM::Init(void)
{
   _D(Lvl_Debug, "Inserted media appears to be a DVD-RAM. Setting common parameters.");
   Disc_DVD_PlusRW::Init();      // we get format capacities here
   _D(Lvl_Info, "DVD-RAM Initialization done.");
};

int Disc_DVD_RAM::CloseDisc(int type, int)
{
   cmd_Close *cl = new cmd_Close(dio);
   cl->setType(cmd_Close::Close_FlushBuffers, 0);
   /*
    * do not let immediate go up here. immediate flush causes problems.
    */
   cl->setImmediate(true);
   int error = cl->Go();
   WaitOpComplete();
   delete cl;
   
//   Init();
   RequestUpdate();
   
   return error;
}
