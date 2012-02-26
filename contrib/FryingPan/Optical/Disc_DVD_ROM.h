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

#ifndef __DISC_DVD_ROM_H
#define __DISC_DVD_ROM_H

#include "Disc_Generic.h"
#include <Generic/VectorT.h>

using namespace GenNS;

class Disc_DVD_ROM : public Disc
{
protected:
   cmd_ReadDVDStructure                rdvd;
   VectorT<uint32>                     dcbs;
   DVDStructure<DVD_DiscControlBlock>  cb;
public:
			                  Disc_DVD_ROM(class Drive*);
   virtual                ~Disc_DVD_ROM(void);

   virtual void            Init(void); // OUGHT TO BE INHERITED EVERYWHERE!

   virtual int             LayoutTracks(const IOptItem *)   {  return ODE_IllegalCommand; };
   virtual bool            AllowMultiSessionLayout()  {  return false;              };
   virtual bool            AllowMultiTrackLayout()    {  return false;              };
   virtual int             SessionGapSize()           {  return 0;                  };
   virtual int             TrackGapSize()             {  return 0;                  };
   virtual bool            IsCD()                     {  return false;              };
   virtual bool            IsDVD()                    {  return true;               };
   virtual bool            IsFormatted()              {  return false;              };
   virtual bool            IsWritable()               {  return false;              };
   virtual bool            IsErasable()               {  return false;              };
   virtual bool            IsFormatable()             {  return false;              };
   virtual bool            IsOverwritable()           {  return false;              };
   virtual int             EraseDisc(int)             {  return ODE_IllegalCommand; };
   virtual int             FormatDisc(int)            {  return ODE_IllegalCommand; };
   virtual int             StructureDisc()            {  return ODE_IllegalCommand; };
   virtual int             DiscType()                 {  return DRT_Profile_DVD_ROM;};
   virtual int             DiscSubType()              {  return 0;                  };
   virtual int             LayoutAdjustment()         {  return 0;                  };
   virtual int             BeginTrackWrite(const IOptItem*) {  return ODE_IllegalCommand; };
   virtual int             EndTrackWrite(const IOptItem*)   {  return ODE_IllegalCommand; };
  
   virtual int             CheckItemData(const IOptItem*);
   virtual void            FillDiscSpeed(DiscSpeed&);
};


#endif

