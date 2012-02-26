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

#ifndef __DISC_CD_RW_H
#define __DISC_CD_RW_H

#include "Disc_CD_R.h"

using namespace GenNS;

class Disc_CD_RW : public Disc_CD_R
{
   class cmd_ReadFormatCapacities  *readfmtcaps;
   class CfgIOMeas                 *meas;

public:
                           Disc_CD_RW(class Drive*);
   virtual                ~Disc_CD_RW(void);

   virtual void            Init(); // OUGHT TO BE INHERITED EVERYWHERE!

   virtual bool            IsErasable()           {  return true;               };

   virtual bool            IsFormatable();
   virtual bool            IsOverwritable();
   virtual bool            IsFormatted();
   virtual bool            IsWritable();
   virtual int             DiscType();
   virtual int             DiscSubType();
   virtual bool            AllowMultiSessionLayout();
   virtual bool            AllowMultiTrackLayout();
   virtual int             SessionGapSize();
   virtual int             TrackGapSize();
   virtual int             BeginTrackWrite(const IOptItem *pDI);
   virtual int             EndTrackWrite(const IOptItem*pDI);

   virtual int             CloseDisc(int type, int lTrackNo);

   virtual int             CheckItemData(const IOptItem*);
   virtual int             EraseDisc(int);
   virtual int             FormatDisc(int);
   virtual int             StructureDisc();

   virtual int16           GetOperationProgress();
   virtual uint32          GetDiscSize();
   virtual bool            wantCDText() const;
};


#endif
