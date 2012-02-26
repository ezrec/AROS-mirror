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

#ifndef _IROOT_H_
#define _IROOT_H_

#include "IDirectory.h"

class IRoot : public virtual IDirectory
{
public:
   enum Level
   {
      ISOLevel_1 = 1,
      ISOLevel_2,
      ISOLevel_3,
   };

public:
   
   virtual Level                       getISOLevel() const                       = 0;
   virtual void                        setISOLevel(Level aLevel)                 = 0;
   virtual bool                        isISORelaxed() const                      = 0;
   virtual void                        setISORelaxed(bool bRelaxed)              = 0;

   virtual bool                        generate(const Hook *data)                = 0;
};

#endif //_IROOT_H_
