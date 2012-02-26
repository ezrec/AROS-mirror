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

#ifndef __DISC_DVD_PLUSRW_H
#define __DISC_DVD_PLUSRW_H

#include "Disc_DVD_PlusR.h"

using namespace GenNS;

class Disc_DVD_PlusRW : public Disc_DVD_PlusR
{
protected:
   class cmd_ReadFormatCapacities  *readfmtcaps;
   class CfgIOMeas                 *meas;

public:
                           Disc_DVD_PlusRW(class Drive*);
   virtual                ~Disc_DVD_PlusRW(void);

   virtual void            Init(void); // OUGHT TO BE INHERITED EVERYWHERE!

   virtual bool            AllowMultiSessionLayout()  {  return false;                 };
   virtual bool            AllowMultiTrackLayout()    {  return false;                 };
   virtual int             SessionGapSize()           {  return 0;                     };
   virtual int             TrackGapSize()             {  return 0;                     };
   virtual bool            IsWritable()               {  return true;                  };
   virtual bool            IsErasable()               {  return false;                 };
   virtual bool            IsFormatable()             {  return true;                  };
   virtual int             EraseDisc(int)             {  return ODE_IllegalCommand;    };
   virtual int             DiscType()                 {  return DRT_Profile_DVD_PlusRW;};
   virtual bool            IsOverwritable()           {  return true;                  };

   virtual int             FormatDisc(int);
   virtual int             StructureDisc();
   virtual bool            IsFormatted();

   virtual const IOptItem *GetNextWritableTrack(const IOptItem *di);
   virtual int             CloseDisc(int type, int lTrackNo);
   virtual int             DiscSubType();   
   virtual int16           GetOperationProgress();
   virtual uint32          GetDiscSize();
};

#endif

