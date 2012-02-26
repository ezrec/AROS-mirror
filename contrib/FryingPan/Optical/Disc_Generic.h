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

#ifndef __DISC_GENERIC_H
#define __DISC_GENERIC_H

#include "Drive.h"
#include "Disc_Item.h"
#include "OptDisc.h"
#include <Generic/Vector.h>
#include <Generic/VectorT.h>
#include "IOptItem.h"

using namespace GenNS;

struct cmd_Read;
struct cmd_Write;

class Disc 
{
private:

   int64                         lSeqSector;                // sequential recording - sector number of current track
   int64                         lSeqLastSector;            // sequential recording - last sector to be written for current track
   const IOptItem               *pSeqStructure;             // disc structure
   const IOptItem               *pSeqCurrentTrack;          // current track;
   cmd_Read                     *pRead;
   cmd_Write                    *pWrite;
   
   bool                          tryReadTrackInfo(void);
   bool                          tryReadTOC(void);
   bool                          tryPrimitiveTOC(void);

   unsigned short                read_speed;
   unsigned short                write_speed;
   DiscWriteMethod               write_method;
   unsigned char                 refresh_period;
   cmd_ReadDiscInfo             *pReadDiscInfo;
   bool                          bNeedUpdate;

   VectorT<DiscSpeed>            readSpeeds;
   VectorT<DiscSpeed>            writeSpeeds;

   OptDisc                      *optcontent;

   int16                         laySessions;
   int16                         layTracks;
   int16                         layFirstTrack;
   int16                         layFirstSession;
   int64                         layCurrentBlock;


protected:
   DEFINE_DEBUG;
   DriveIO                      *dio;
   Drive                        *drive;
   DiscInfo                     *pDiscInfo;


   void                          RequestUpdate();
   void                          SetRefreshPeriod(UBYTE fiftieths);
   int                           WritePadData(const IOptItem *pDI, int32 len);
   int                           WriteDisc(int lBlock, unsigned short lLength, unsigned short lSectorSize, APTR pBuffer);
   virtual void                  ReadSpeeds();

public:

                                 Disc(Drive*);
   virtual                      ~Disc(void);


   //=======================//
   // now the methods...    //
   //=======================//

   virtual bool                  AllowMultiSessionLayout()  = 0;
   virtual bool                  AllowMultiTrackLayout()    = 0;
   virtual int                   SessionGapSize()           = 0;
   virtual int                   TrackGapSize()             = 0;
   virtual int                   LayoutAdjustment()         = 0;
   virtual bool                  IsCD()                     = 0;
   virtual bool                  IsDVD()                    = 0;
   virtual bool                  IsFormatted()              = 0;
   virtual bool                  IsWritable()               = 0;
   virtual bool                  IsErasable()               = 0;
   virtual bool                  IsFormatable()             = 0;
   virtual bool                  IsOverwritable()           = 0;
   virtual int                   EraseDisc(int)             = 0;
   virtual int                   FormatDisc(int)            = 0;
   virtual int                   StructureDisc()            = 0;
   virtual int                   DiscType()                 = 0;
   virtual int                   DiscSubType()              = 0;
   virtual int                   CheckItemData(const IOptItem*);
   virtual int                   BeginTrackWrite(const IOptItem*) = 0;     // for writing
   virtual int                   EndTrackWrite(const IOptItem*)   = 0;
   virtual void                  FillDiscSpeed(DiscSpeed&)  = 0;

   virtual const char           *DiscVendor();

   virtual int                   WriteData(const IOptItem*);

   virtual const uint8           GetRefreshPeriod(void);

   virtual void                  Init(void); // OUGHT TO BE INHERITED EVERYWHERE!

   virtual int                   RandomRead(const IOptItem*,  int32, int32, void*);
   virtual int                   RandomWrite(const IOptItem*, int32, int32, void*);
   virtual int                   SequentialWrite(APTR pMem, ULONG lBlocks);
   virtual int                   CloseDisc(int lType, int lTrackNo);

   virtual int                   LayoutTracks(const IOptItem*);
   virtual int                   UploadLayout(const IOptItem*);
   virtual const IOptItem       *GetNextWritableTrack(const IOptItem*);

   virtual int                   GetNumTracks(void);
   virtual int                   GetNumSessions(void);

   virtual const IOptItem       *GetContents();

   virtual const IOptItem       *FindSession(int lSessNum);
   virtual const IOptItem       *FindTrack(int lTrackNum);

   virtual int                   SetWriteSpeed(uint16);
   virtual int                   SetReadSpeed(uint16);
   virtual uint16                GetWriteSpeed();
   virtual uint16                GetReadSpeed();
   virtual bool                  RequiresUpdate();

   virtual const DiscSpeed      *GetReadSpeeds();
   virtual const DiscSpeed      *GetWriteSpeeds();

   virtual int                   SetWriteMethod(DiscWriteMethod);
   virtual DiscWriteMethod       GetWriteMethod();
   virtual int                   OnChangeWriteMethod();

   virtual int16                 GetOperationProgress();
   virtual uint32                GetDiscSize();

   virtual uint32                Calibrate();
   virtual uint32                WaitOpComplete();
};

#endif
