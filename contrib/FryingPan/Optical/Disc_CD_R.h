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

#ifndef __DISC_CD_R_H
#define __DISC_CD_R_H

#include "Disc_CD_ROM.h"
#include "CDText.h"

using namespace GenNS;

class Disc_CD_R : public Disc_CD_ROM
{
   bool                    bWriteSAO;
   bool                    bCDXA;
   bool                    bCDI;
   const IOptItem         *pLastTrack;
   int                     lSAOTrackGap;
   uint8                  *pCueSheet;
   int                     lCueElements;
   unsigned long           lLeadInLength;
   unsigned long           lLeadInStart;
   int                     lAdjustment;
   CDText                 *builder;
   bool                    cdText;
   
protected:

   TOC_ATIP               *atip;

public:
                           Disc_CD_R(class Drive*);
   virtual                ~Disc_CD_R(void);

   virtual void            Init(void);          // OUGHT TO BE INHERITED EVERYWHERE!
   virtual int             CloseDisc(int type, int lTrackNo);

   virtual int             OnChangeWriteMethod();

   virtual bool            AllowMultiSessionLayout()     {  return true;                     };
   virtual bool            AllowMultiTrackLayout()       {  return true;                     };
   virtual int             SessionGapSize();
   virtual int             TrackGapSize();  
   virtual bool            IsCD(void)                    {  return true;                     };
   virtual bool            IsDVD(void)                   {  return false;                    };
   virtual bool            IsFormatted(void)             {  return false;                    };
   virtual bool            IsErasable(void)              {  return false;                    };
   virtual bool            IsFormatable(void)            {  return false;                    };
   virtual int             EraseDisc(int)                {  return ODE_IllegalCommand;       };
   virtual int             FormatDisc(int)               {  return ODE_IllegalCommand;       };
   virtual int             StructureDisc(void)           {  return ODE_IllegalCommand;       };
   virtual int             DiscType(void)                {  return DRT_Profile_CD_R;         };
   virtual int             LayoutAdjustment()            {  return lAdjustment;              };
   virtual int             LayoutTracks(const IOptItem *pDI)   {  return Disc::LayoutTracks(pDI);  };
   virtual int             BeginTrackWrite(const IOptItem*pDI);
   virtual int             EndTrackWrite(const IOptItem*pDI);
   virtual int             CheckItemData(const IOptItem*);
   virtual int             DiscSubType(void);
   virtual bool            IsWritable(void);
   virtual int             UploadLayout(const IOptItem *pDI);  
   virtual const char     *DiscVendor();
   virtual uint32          GetDiscSize();

   virtual bool            wantCDText() const;
};


#endif

