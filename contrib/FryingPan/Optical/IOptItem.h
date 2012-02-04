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

#ifndef _IOPTITEM_H_
#define _IOPTITEM_H_

#include "Optical.h"
#include <Generic/String.h>

using namespace GenNS;
namespace GenNS
{
   class DbgHandler;
};

class IOptItem
{
protected:
   virtual                ~IOptItem() {};
public:
   virtual void            claim() const                                = 0;
   virtual void            dispose() const                              = 0;

   virtual IOptItem       *addChild()                                   = 0;

   virtual EItemType       getItemType() const                          = 0;
   virtual void            setItemType(EItemType lNewType)              = 0;
   virtual uint16          getItemNumber() const                        = 0;
   virtual void            setItemNumber(uint16 number)                 = 0;
   virtual EDataType       getDataType() const                          = 0;
   virtual void            setDataType(EDataType lNewType)              = 0;
   virtual uint16          getPreGapSize() const                        = 0;
   virtual void            setPreGapSize(uint16 lNewSize)               = 0;
   virtual uint16          getPostGapSize() const                       = 0;
   virtual void            setPostGapSize(uint16 lNewSize)              = 0;
   virtual int32           getStartAddress() const                      = 0;     // start -> pregap -> data -> postgap -> end
   virtual void            setStartAddress(int32 lNewAddress)           = 0;     //
   virtual int32           getEndAddress() const                        = 0;     // end = start + pregap + data + postgap - 1
   virtual void            setEndAddress(int32 lNewAddress)             = 0;     // 
   virtual int32           getBlockCount() const                        = 0;     // block count = pregap + data + postgap
   virtual int32           getDataBlockCount() const                    = 0;     // data block count...
   virtual void            setDataBlockCount(int32 count)               = 0;     //
   virtual int32           getDataStartAddress() const                  = 0;     // start + pregap
   virtual int32           getDataEndAddress() const                    = 0;     // start + pregap + data - 1

   virtual bool            isProtected() const                          = 0;
   virtual void            setProtected(bool bProtected)                = 0;
   virtual bool            isPreemphasized() const                      = 0;
   virtual void            setPreemphasized(bool bPreemph)              = 0;
   virtual bool            isIncremental() const                        = 0;
   virtual void            setIncremental(bool bState)                  = 0;
   virtual bool            hasCDText() const                            = 0;
   virtual void            setCDText(bool bCDText)                      = 0;

   virtual const char*     getCDTTitle() const                          = 0;
   virtual void            setCDTTitle(const char* sNewTitle)           = 0;
   virtual const char*     getCDTArtist() const                         = 0;
   virtual void            setCDTArtist(const char* sNewArtist)         = 0;
   virtual const char*     getCDTMessage() const                        = 0;
   virtual void            setCDTMessage(const char* sNewMessage)       = 0;
   virtual const char*     getCDTLyrics() const                         = 0;
   virtual void            setCDTLyrics(const char* sNewLyrics)         = 0;
   virtual const char*     getCDTComposer() const                       = 0;
   virtual void            setCDTComposer(const char* sNewComposer)     = 0;
   virtual const char*     getCDTDirector() const                       = 0;
   virtual void            setCDTDirector(const char* sNewDirector)     = 0;

   virtual const IOptItem* getParent() const                            = 0;
   virtual const IOptItem* getChild(int32 num) const                    = 0;
   virtual int32           getChildCount() const                        = 0;

   virtual bool            isBlank() const                              = 0;
   virtual void            setBlank(bool blank)                         = 0;

   virtual uint16          getSectorSize() const                        = 0;
   virtual void            setSectorSize(uint16 size)                   = 0;

   virtual uint32          getDiscID() const                            = 0;
   virtual void            setDiscID(uint32 id)                         = 0;
   
   virtual uint16          getPacketSize() const                        = 0;
   virtual void            setPacketSize(uint16 NewSize)                = 0;
    
   virtual uint16          getFlags() const                             = 0;
   virtual void            setFlags(uint16 lNewFlags)                   = 0;

   virtual void            printReport(DbgHandler *dbg) const           = 0;
   virtual bool            isComplete() const                           = 0;
   virtual void            setComplete(bool)                            = 0;
};

#endif //_OPTITEM_H_
