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

#ifndef _ENGINE_ISOTRACK_H_
#define _ENGINE_ISOTRACK_H_

#include "../ITrack.h"
#include <Generic/String.h>
#include <DTLib/IRegHook.h>
#include <Optical/IOptItem.h>

class IBrowser;

class ISOTrack : public ITrack
{
protected:
   IBrowser                     *iso;
   uint32                        size;
protected:
   virtual                      ~ISOTrack();
public:
                                 ISOTrack(class IBrowser *iso);
                                 ISOTrack(const ISOTrack &src);
   virtual const char           *getTrackName() const;
   virtual const char           *getModuleName() const;
   virtual bool                  fillOptItem(IOptItem *item) const;
   virtual void                  dispose();
   virtual ITrack               *getClone() const;
   virtual bool                  update();
   virtual void                  setUp();
   virtual void                  cleanUp();
   virtual int                   read(const IOptItem* item, void* mem, const Hook *hk) const;
   virtual bool                  isValid() const;
   virtual uint16                getBlockSize() const;
   virtual uint32                getBlockCount() const;
};

#endif

