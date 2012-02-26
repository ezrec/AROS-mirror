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

#include "ISOTrack.h"
#include "Globals.h"
#include <DTLib/IRegHook.h>
#include <ISOBuilder/IBrowser.h>

ISOTrack::ISOTrack(IBrowser *image)
{
   iso = image;
   size = iso->validate();
}

ISOTrack::ISOTrack(const ISOTrack &src)
{
   iso = src.iso;
}

ISOTrack::~ISOTrack()
{
}

bool ISOTrack::isValid() const
{
   return (0 != iso) ? true : false;
}

const char *ISOTrack::getTrackName() const
{
   return "Generated ISO Image";
}

const char *ISOTrack::getModuleName() const 
{
   return "External ISO builder";
}

bool ISOTrack::fillOptItem(IOptItem *track) const
{
   track->setDataType(Data_Mode1);
   track->setStartAddress(0);
   track->setSectorSize(2048);
   track->setDataBlockCount(iso->validate());

   return !iso->isEmpty();
}

void ISOTrack::dispose()
{
   delete this;
}

ITrack *ISOTrack::getClone() const
{
   return new ISOTrack(*this);
}

bool ISOTrack::update()
{
   size = iso->validate();
   return !iso->isEmpty();
}

void ISOTrack::setUp()
{
   // track is already validated
}

void ISOTrack::cleanUp()
{
}

int ISOTrack::read(const IOptItem *, void* mem, const Hook *hk) const
{
   // we are free to ignore the mem supplied...
   iso->addDataHook(hk);
   iso->startDataFlow();
   iso->remDataHook(hk);
   return 0;
}

uint16 ISOTrack::getBlockSize() const
{
   return 2048;
}

uint32 ISOTrack::getBlockCount() const
{
   return size;
}


