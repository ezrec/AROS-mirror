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

#ifndef __DISC_CD_ROM_H
#define __DISC_CD_ROM_H

#include "Disc_Generic.h"

using namespace GenNS;

struct cmd_ReadCD;

class Disc_CD_ROM : public Disc
{
   cmd_ReadCD             *pReadCD;
   cmd_ReadSubChannel     *pReadSub;
   SUB_MCN                *mcn;
   SUB_Q                  *subq;
   uint32                  freedbid;

protected:
   int                     probeCDText();
   int                     probeDataType();
   int                     probeSubChannel();
   uint32                  probeFreeDBID();
   int                     sumDigits(int32 val);
   int                     getSubChannelInfo(int32 pos, uint8 &trk, uint8 &idx);

public:
			                  Disc_CD_ROM(class Drive*);
   virtual                ~Disc_CD_ROM();

   virtual void            Init(); // OUGHT TO BE INHERITED EVERYWHERE!

   virtual int             LayoutTracks(const IOptItem *)   {  return ODE_IllegalCommand; };
   virtual bool            AllowMultiSessionLayout()  {  return false;              };
   virtual bool            AllowMultiTrackLayout()    {  return false;              };
   virtual int             SessionGapSize()           {  return 0;                  };
   virtual int             TrackGapSize()             {  return 0;                  };
   virtual bool            IsCD()                     {  return true;               };
   virtual bool            IsDVD()                    {  return false;              };
   virtual bool            IsFormatted()              {  return false;              };
   virtual bool            IsWritable()               {  return false;              };
   virtual bool            IsErasable()               {  return false;              };
   virtual bool            IsFormatable()             {  return false;              };
   virtual bool            IsOverwritable()           {  return false;              };
   virtual int             EraseDisc(int)             {  return ODE_IllegalCommand; };
   virtual int             FormatDisc(int)            {  return ODE_IllegalCommand; };
   virtual int             StructureDisc()            {  return ODE_IllegalCommand; };
   virtual int             DiscType()                 {  return DRT_Profile_CD_ROM; };
   virtual int             DiscSubType()              {  return 0;                  };
   virtual int             LayoutAdjustment()         {  return 0;                  };
   virtual int             BeginTrackWrite(const IOptItem*) {  return ODE_IllegalCommand; };
   virtual int             EndTrackWrite(const IOptItem*);
   virtual int             CheckItemData(const IOptItem*);
  
   virtual int             RandomRead(const IOptItem *i, int32 first, int32 count, void *buff);
   virtual void            FillDiscSpeed(DiscSpeed&);
};

#endif
