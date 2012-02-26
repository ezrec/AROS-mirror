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
#include "Disc_DVD_MinusRW.h"

Disc_DVD_MinusRW::Disc_DVD_MinusRW(Drive *d) : Disc_DVD_MinusR(d)
{
   meas = 0;
   readfmtcaps = 0;
};

void Disc_DVD_MinusRW::Init(void)
{
   Disc_DVD_MinusR::Init();      // MUSI BYC W TEJ KOLEJNOSCI
   readfmtcaps = new cmd_ReadFormatCapacities(dio);
   readfmtcaps->Go();
}

Disc_DVD_MinusRW::~Disc_DVD_MinusRW(void)
{
   delete readfmtcaps;
};

bool Disc_DVD_MinusRW::IsFormatted(void)
{
   if (NULL == readfmtcaps)
      return false;
   if (readfmtcaps->IsFormatted())
      return true;
   return false;
}

bool Disc_DVD_MinusRW::IsOverwritable(void)
{
   cmd_GetConfiguration::Feature *x;

   if (DiscType() != DRT_Profile_DVD_MinusRW_Restricted)
      return 0;

   x = drive->GetDriveFeature(cmd_GetConfiguration::Feature_RigidOverwrite);
   if (x)
      if (x->IsCurrent()) return 1;

   x = drive->GetDriveFeature(cmd_GetConfiguration::Feature_RestrictedOverwrite);
   if (x)
      if (x->IsCurrent()) return 1;

   return 0;
}

int Disc_DVD_MinusRW::EraseDisc(int met)
{
   cmd_Blank::BlankType type;
   int                  err;
   cmd_StartStopUnit    ssu(dio);

   ssu.setType(cmd_StartStopUnit::StartStop_Start);
   ssu.Go();


   Page<Page_Write> &pw = drive->GetWritePage();
   pw->SetWriteType(Page_Write::WriteType_SessionAtOnce);
   if (drive->SetPage(pw))
      return ODE_CommandError;

   switch (met) {
      case DRT_Blank_Complete:
         {
            type = cmd_Blank::Blank_All;
            meas = drive->GetHardwareConfig()->DVDMinusInfo()->getCBlankMeas(GetDiscSize(), GetWriteSpeed());
         }
         break;
      case DRT_Blank_Fast:
         {
            type = cmd_Blank::Blank_Minimal;
            meas = drive->GetHardwareConfig()->DVDMinusInfo()->getQBlankMeas(GetDiscSize(), GetWriteSpeed());
         }
         break;
      default:
         return ODE_IllegalParameter;
   }

   Calibrate();

   cmd_Blank  *blk = new cmd_Blank(dio);
   blk->setType(type, 0);
   blk->setImmediate(true);

   if (meas != 0)
      meas->begin();
   err = blk->Go();
   WaitOpComplete();
   if (meas != 0)
      meas->end();
   meas = 0;
//   if (err == ODE_OK) Init();
   RequestUpdate();
   delete blk;
   return err;
}

int Disc_DVD_MinusRW::FormatDisc(int met)
{
   cmd_Format  *fmt;
   int          err;
   cmd_StartStopUnit    ssu(dio);

   ssu.setType(cmd_StartStopUnit::StartStop_Start);
   ssu.Go();

   Page<Page_Write> &pw = drive->GetWritePage();
   pw->SetWriteType(Page_Write::WriteType_Packet);
   pw->SetPacketSize(16);
   if (drive->SetPage(pw))
      return ODE_CommandError;

   Calibrate();

   fmt = new cmd_Format(dio);
   fmt->setImmediate(true);

   switch (met) 
   {
      case DRT_Format_Complete:
         {
            _D(Lvl_Info, "Complete format of dvd-rw media requested");
            fmt->setType(cmd_Format::Format_FullFormat, readfmtcaps->GetMaxCapacity(), 16);
            meas = drive->GetHardwareConfig()->DVDMinusInfo()->getCFormatMeas(GetDiscSize(), GetWriteSpeed());
         }
         break;
      case DRT_Format_Fast:
         {
            _D(Lvl_Info, "Quick format of dvd-rw media requested");
            fmt->setType(cmd_Format::Format_DVDM_QuickFormat, 0, 16);
            meas = drive->GetHardwareConfig()->DVDMinusInfo()->getQFormatMeas(GetDiscSize(), GetWriteSpeed());
         }
         break;
   }
   
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

int Disc_DVD_MinusRW::StructureDisc(void)
{
   int err;
   err = EraseDisc(DRT_Blank_Fast);
   if (err) return err;
   err = FormatDisc(DRT_Format_Fast);
   if (err) return err;
   return ODE_OK;
};

int Disc_DVD_MinusRW::DiscType()
{
   if (NULL == readfmtcaps)
      return DRT_Profile_DVD_MinusRW_Sequential; // we dont know if it can be formatted
   
   if (readfmtcaps->IsFormatted()) {
      return DRT_Profile_DVD_MinusRW_Restricted;
   } else {
      return DRT_Profile_DVD_MinusRW_Sequential;
   }
}

bool Disc_DVD_MinusRW::IsWritable()
{
//   _D("Checking whether disc is formatted...");
//   if (!IsFormatted()) 
//      return false;
   _D(Lvl_Info, "Checking for writable tracks...");
   if (GetNextWritableTrack(0))
      return true;
   return false;
}

int16 Disc_DVD_MinusRW::GetOperationProgress()
{
   if (meas != 0)
   {
      return meas->getProgress();
   }
   return Disc::GetOperationProgress();
}

uint32 Disc_DVD_MinusRW::GetDiscSize()
{
   if (0 != readfmtcaps)
      return readfmtcaps->GetMaxCapacity();
   return Disc_DVD_MinusR::GetDiscSize();
}

