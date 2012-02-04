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

#ifndef __DISC_DVD_PlusR
#define __DISC_DVD_PlusR

#include "Disc_DVD_ROM.h"

using namespace GenNS;

class Disc_DVD_PlusR :public Disc_DVD_ROM
{
public:
                           Disc_DVD_PlusR(class Drive*);
   virtual                ~Disc_DVD_PlusR(void);

   virtual void            Init(void); // OUGHT TO BE INHERITED EVERYWHERE!
   virtual int             OnChangeWriteMethod();

   virtual bool            AllowMultiSessionLayout()           {  return true;                     };
   virtual bool            AllowMultiTrackLayout()             {  return true;                     };
   virtual int             SessionGapSize()                    {  return 0;                        };
   virtual int             TrackGapSize()                      {  return 0;                        };
   virtual int             DiscType()                          {  return DRT_Profile_DVD_PlusR;    };
   virtual int             LayoutTracks(const IOptItem *pDI)   {  return Disc::LayoutTracks(pDI);  };
   virtual int             UploadLayout(const IOptItem *pDI)   {  return Disc::UploadLayout(pDI);  };
   virtual int             BeginTrackWrite(const IOptItem*pDI) {  return ODE_OK;                   };
   virtual int             EndTrackWrite(const IOptItem*)      {  return ODE_IllegalCommand;       };

   virtual int             CheckItemData(const IOptItem *pDI);

   virtual bool            IsWritable();
   virtual int             DiscSubType();   

   virtual int             CloseDisc(int lType, int lTrackNo);
};

#endif

