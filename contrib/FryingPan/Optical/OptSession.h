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

#ifndef _OPTSESSION_H_
#define _OPTSESSION_H_

/*
 * this is for optical's internal use *only*
 */
#include "OptItem.h"

class OptDisc;

class OptSession : public OptItem
{
public:
                           OptSession(IOptItem *parent);

   virtual EItemType       getItemType() const;
   virtual void            setItemType(EItemType lNewType);
   virtual EDataType       getDataType() const;
   virtual void            setDataType(EDataType lNewType);
   virtual uint16          getPreGapSize() const;
   virtual void            setPreGapSize(uint16 lNewSize);
   virtual uint16          getPostGapSize() const;
   virtual void            setPostGapSize(uint16 lNewSize);
   virtual int32           getStartAddress() const;
   virtual void            setStartAddress(int32 lNewAddress);
   virtual int32           getEndAddress() const;
   virtual void            setEndAddress(int32 lNewAddress);
   virtual void            setDataBlockCount(int32 lNewSize);

   virtual int32           getDataBlockCount() const;

   virtual void            setProtected(bool bProtected);
   virtual void            setPreemphasized(bool bPreemph);

   virtual uint32          getDiscID() const;
   virtual void            setDiscID(uint32 id);
   
   virtual uint16          getFlags() const;
   virtual void            setFlags(uint16 lNewFlags);

   virtual IOptItem       *addChild();
};

#endif

