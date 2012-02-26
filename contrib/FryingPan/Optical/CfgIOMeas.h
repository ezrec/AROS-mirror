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

#ifndef _OPTICAL_CFGIOMEAS_H_
#define _OPTICAL_CFGIOMEAS_H_

#include <Generic/Types.h>

class CfgIOMeas 
{
protected:
   uint32         discsize;
   uint16         iospeed;
   uint16         count;
   uint16         time;

   uint32         ctime;
   uint32         ctenth;
public:
                  CfgIOMeas(uint32 ds, uint16 io, uint16 cnt=0, uint32 time=0);
   virtual       ~CfgIOMeas();
   virtual void   setTime(uint16 time);
   virtual uint16 getTime();
   virtual uint32 getDiscSize();
   virtual uint16 getIOSpeed();
   virtual uint16 getCount();

   virtual void   begin();             // initialize var for time count
   virtual void   end();               // calculate time & store it
   virtual uint16 getProgress();       // returns value from 0 to 100
};

#endif

