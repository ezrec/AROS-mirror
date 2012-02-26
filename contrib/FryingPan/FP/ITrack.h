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

#ifndef _ITRACK_H_
#define _ITRACK_H_

#include <Optical/IOptItem.h>
#include <DTLib/IFileReader.h>

class Hook;

class ITrack
{
protected:
   virtual                      ~ITrack()
   {
   }

public:
   virtual const char           *getTrackName() const          = 0;
   virtual const char           *getModuleName() const         = 0;
   virtual bool                  fillOptItem(IOptItem*) const  = 0;
   virtual void                  dispose()                     = 0;
   virtual ITrack               *getClone() const              = 0;
   virtual bool                  update()                      = 0;
   virtual void                  setUp()                       = 0;
   virtual void                  cleanUp()                     = 0;
   virtual int                   read(const IOptItem*, void*, const Hook*) const  = 0;
   virtual bool                  isValid() const               = 0;

   virtual uint16                getBlockSize() const          = 0;
   virtual uint32                getBlockCount() const         = 0;
};

#endif

