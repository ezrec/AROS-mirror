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

#ifndef __DISC_DVD_MINUSR_H
#define __DISC_DVD_MINUSR_H

#include "Disc_DVD_ROM.h"

using namespace GenNS;

class Disc_DVD_MinusR : public Disc_DVD_ROM
{
   DVDStructure<DVD_PreRecordedLeadIn> information;
public:
                           Disc_DVD_MinusR(class Drive*);
   virtual                ~Disc_DVD_MinusR(void);

   virtual void            Init(void);             // OUGHT TO BE INHERITED EVERYWHERE!
   virtual int             OnChangeWriteMethod();  // this parses -RW as well

   virtual bool            AllowMultiSessionLayout()           {  return true;                     };
   virtual bool            AllowMultiTrackLayout()             {  return true;                     };
   virtual int             SessionGapSize()                    {  return 7696;                     };
   virtual int             TrackGapSize()                      {  return 16;                       };
   virtual int             DiscType()                          {  return DRT_Profile_DVD_MinusR;   };
   virtual int             DiscSubType()                       {  return 0;                        };
   virtual int             LayoutTracks(const IOptItem *pDI)   {  return Disc::LayoutTracks(pDI);  };
   virtual int             UploadLayout(const IOptItem *pDI)   {  return Disc::UploadLayout(pDI);  };
   virtual int             BeginTrackWrite(const IOptItem*pDI) {  return ODE_OK;                   };
   virtual int             EndTrackWrite(const IOptItem*)      {  return ODE_IllegalCommand;       };
   virtual bool            IsWritable();

   virtual int             CheckItemData(const IOptItem *pDI);

   virtual int             CloseDisc(int lType, int lTrackNo);

};

#endif

