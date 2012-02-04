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

#ifndef IBROWSER_H_
#define IBROWSER_H_

#include "Structure/ClRoot.h"      // does ClDirectory, ClElement
#include "Structure/ClFile.h"

class IBrowser
{
public:
   enum ISOLevel
   {
      ISOLevel_1,
      ISOLevel_2,
      ISOLevel_3
   };

public:
   virtual ClRoot               *getRoot()                  =  0;    // get root directory
   virtual ClDirectory          *getParent()                =  0;    // get parent directory
   virtual ClDirectory          *getCurrDir()               =  0;    // get current working dir
   virtual void                  setCurrDir(ClDirectory*)   =  0;
   virtual void                  goRoot()                   =  0;    // go to root directory
   virtual void                  goParent()                 =  0;    // go to parent directory
   virtual ClDirectory          *makeDir()                  =  0;    // create dir here;

   virtual void                  setISOLevel(ISOLevel)      =  0;
   virtual ISOLevel              getISOLevel()              =  0;

   virtual void                  addDataHook(const class Hook*) = 0;
   virtual void                  remDataHook(const class Hook*) = 0;
   virtual unsigned long         validate()                 = 0;
   virtual long                  startDataFlow()            = 0;

   virtual bool                  isEmpty()                  = 0;
};

#endif /*IBROWSER_H_*/
