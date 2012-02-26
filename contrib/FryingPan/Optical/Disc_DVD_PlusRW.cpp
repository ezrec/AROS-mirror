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

#include "Headers.h"
#include "Disc_DVD_PlusRW.h"

Disc_DVD_PlusRW::Disc_DVD_PlusRW(Drive *d) : Disc_DVD_PlusR(d)
{
   readfmtcaps = 0;
   meas        = 0;
};

Disc_DVD_PlusRW::~Disc_DVD_PlusRW(void)
{
   delete readfmtcaps;
};

void Disc_DVD_PlusRW::Init(void)
{
   Disc_DVD_PlusR::Init();

   if (readfmtcaps) delete readfmtcaps;

   readfmtcaps = new cmd_ReadFormatCapacities(dio);
   readfmtcaps->Go();

   Page<Page_Write> &pw = drive->GetWritePage();
   pw->SetWriteType(Page_Write::WriteType_Packet);
   if (drive->SetPage(pw)) 
   {
      _D(Lvl_Error, "Unable to set up packet writing for DVD+RW/-RAM media!!!");
   }

   _D(Lvl_Info, "Disc formatted? : %ld", readfmtcaps->IsFormatted());
   _D(Lvl_Info, "Max capacity    : %ld blocks", readfmtcaps->GetMaxCapacity());
}

int Disc_DVD_PlusRW::FormatDisc(int met)
{
   cmd_Format  *fmt;
   int          err;

   Page<Page_Write> &pw = drive->GetWritePage();
   pw->SetWriteType(Page_Write::WriteType_Packet);
   pw->SetPacketSize(16);
   if (drive->SetPage(pw))
      return ODE_CommandError;

   meas = drive->GetHardwareConfig()->DVDPlusInfo()->getCFormatMeas(GetDiscSize(), GetWriteSpeed());
   // all methods work same here.
   fmt = new cmd_Format(dio);
   fmt->setImmediate(true);
   fmt->setType(cmd_Format::Format_DVDP_FullFormat, readfmtcaps->GetMaxCapacity(), 0);

   Calibrate();

   if (meas != 0)
      meas->begin();
   err = fmt->Go();
   WaitOpComplete();
   if (meas != 0)
      meas->end();
   meas = 0;
//   if (err == ODE_OK) Init();
   RequestUpdate();
   delete fmt;
   return err;
}

int Disc_DVD_PlusRW::StructureDisc(void)
{
   int err;
   err = FormatDisc(DRT_Format_Fast);
   if (err) return err;
   return ODE_OK;
};

bool Disc_DVD_PlusRW::IsFormatted(void)
{
   if (NULL == readfmtcaps)
      return false;
      
   return readfmtcaps->IsFormatted();
}

int Disc_DVD_PlusRW::CloseDisc(int type, int)
{
   cmd_Close *cl = new cmd_Close(dio);

   cl->setType(cmd_Close::Close_FlushBuffers, 0);
   cl->Go();

   if (type == DRT_Close_Track) {
      cl->setType(cmd_Close::Close_DVDPlusRW_DeIcing, 0);
   } else if (type == DRT_Close_Session) {
      cl->setType(cmd_Close::Close_DVDPlusRW_FinalizeCompatible, 0);
   } else if (type == DRT_Close_Finalize) {
      cl->setType(cmd_Close::Close_DVDPlusRW_FinalizeCompatible, 0);
   }
   int error = cl->Go();
   delete cl;
   
//   Init();
   RequestUpdate();
   
   return error;
}

const IOptItem *Disc_DVD_PlusRW::GetNextWritableTrack(const IOptItem *di)
{
   _D(Lvl_Info, "DVD+RW/RAM Querying next writable track after %08lx", (int)di);
   
   const IOptItem *disc = GetContents();
   for (int i=0; i<disc->getChildCount(); i++)
   {
      const IOptItem *sess = disc->getChild(i);
      for (int j=0; j<sess->getChildCount(); j++)
      {
         const IOptItem *trak = sess->getChild(j);

         if (di == 0)
            return trak;
         if (di == trak)
            di = 0;
      }
   }

   return 0;
}

int Disc_DVD_PlusRW::DiscSubType()
{
   cmd_GetConfiguration::Feature *x;

   x = drive->GetDriveFeature(cmd_GetConfiguration::Feature_DVD_PlusRW_DualLayer);
   if (x == 0)
   {
      return DRT_SubType_Unknown;
   }

   if (x->IsCurrent()) {
      _D(Lvl_Debug, "Got DualLayer DVD+RW media! Yuppie!");
      return DRT_SubType_DVD_DualLayer;
   } else {
      _D(Lvl_Debug, "Got SingleLayer DVD+RW media..");
      return DRT_SubType_Unknown;
   }
}

int16 Disc_DVD_PlusRW::GetOperationProgress()
{
   if (meas != 0)
   {
      return meas->getProgress();
   }
   return Disc::GetOperationProgress();
}

uint32 Disc_DVD_PlusRW::GetDiscSize()
{
   if (0 != readfmtcaps)
      return readfmtcaps->GetMaxCapacity();
   return Disc_DVD_PlusR::GetDiscSize();
}

