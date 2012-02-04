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

#ifndef __DISC_DVD_RAM_H
#define __DISC_DVD_RAM_H

#include "Disc_DVD_PlusRW.h"

using namespace GenNS;

class Disc_DVD_RAM : public Disc_DVD_PlusRW
{
public:
                           Disc_DVD_RAM(class Drive*);
   virtual                ~Disc_DVD_RAM(void);

   virtual void            Init(void); // OUGHT TO BE INHERITED EVERYWHERE!

   virtual int             DiscType(void)             {  return DRT_Profile_DVD_RAM;   };
   virtual int             DiscSubType(void)          {  return 0;                     };

   virtual int             CloseDisc(int type, int lTrackNo);
};

#endif

