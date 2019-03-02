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

#ifndef _IFILEREADER_H_ 
#define _IFILEREADER_H_

#include <Generic/Generic.h>
#include <Generic/Types.h>

struct IOptItem;

class IFileReader 
{
public:  // pure-virtual
   virtual const char*                 getName()                        = 0;
   virtual const char*                 getTrackName()                   = 0;
   virtual bool                        readData(const IOptItem* item, void* pBuff, int pLen)  = 0;
   virtual bool                        setUp()                          = 0;
   virtual void                        cleanUp()                        = 0;
   virtual void                        dispose()                        = 0;
   virtual uint32                      getBlockCount()                  = 0;
   virtual uint16                      getBlockSize()                   = 0;
   virtual bool                        isAudio()                        = 0;
   virtual bool                        isData()                         = 0;
   virtual bool                        fillOptItem(IOptItem *item)      = 0;
};

#endif
// vim: ts=3 et
