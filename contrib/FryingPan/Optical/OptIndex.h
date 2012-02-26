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

#ifndef _OPTINDEX_H_
#define _OPTINDEX_H_

/*
 * optical internal use only
 */

#include "OptItem.h"

class OptIndex : public OptItem
{
public:
                           OptIndex(IOptItem* parent);
   virtual EItemType       getItemType() const;
   virtual void            setItemType(EItemType lNewType);
   virtual uint32          getDiscID() const;
   virtual void            setDiscID(uint32 id);
   virtual IOptItem       *addChild();
   virtual EDataType       getDataType() const;
   virtual void            setDataType(EDataType lNewType);
   virtual bool            isProtected() const;
   virtual void            setProtected(bool bProtected);
   virtual bool            isPreemphasized() const;
   virtual void            setPreemphasized(bool bPreemph);
   virtual bool            isIncremental() const;
   virtual void            setIncremental(bool bState);
   virtual bool            hasCDText() const;
   virtual void            setCDText(bool bCDText);
   virtual const char*     getCDTTitle() const;
   virtual void            setCDTTitle(const char* sNewTitle);
   virtual const char*     getCDTArtist() const;
   virtual void            setCDTArtist(const char* sNewArtist);
   virtual const char*     getCDTMessage() const;
   virtual void            setCDTMessage(const char* sNewMessage);
   virtual const char*     getCDTLyrics() const;
   virtual void            setCDTLyrics(const char* sNewLyrics);
   virtual const char*     getCDTComposer() const;
   virtual void            setCDTComposer(const char* sNewComposer);
   virtual const char*     getCDTDirector() const;
   virtual void            setCDTDirector(const char* sNewDirector);

   virtual bool            isBlank() const;
   virtual void            setBlank(bool blank);

   virtual uint16          getSectorSize() const;
   virtual void            setSectorSize(uint16 size);

   virtual uint16          getPacketSize() const;
   virtual void            setPacketSize(uint16 NewSize);
};

#endif
