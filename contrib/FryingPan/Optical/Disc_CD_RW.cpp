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
#include "Disc_CD_RW.h"
#include "Config.h"
#include "CfgCDInfo.h"


Disc_CD_RW::Disc_CD_RW(Drive *d) : Disc_CD_R(d)
{
   meas        = 0;
   readfmtcaps = 0;
   readfmtcaps = new cmd_ReadFormatCapacities(dio);
   readfmtcaps->Go();
};

void Disc_CD_RW::Init(void)
{
   Disc_CD_R::Init();
}

Disc_CD_RW::~Disc_CD_RW(void)
{
   delete readfmtcaps;
};

int Disc_CD_RW::EraseDisc(int met)
{
   cmd_Blank::BlankType type;
   int                  err;
   cmd_StartStopUnit    ssu(dio);

   ssu.setType(cmd_StartStopUnit::StartStop_Start);
   ssu.Go();

   if (NULL != atip)
   {
      switch (met)
      {
         case DRT_Blank_Complete:
            meas = drive->GetHardwareConfig()->CDInfo()->getCBlankMeas(atip->GetLeadOutPos(), GetWriteSpeed());
            break;

         case DRT_Blank_Fast:
            meas = drive->GetHardwareConfig()->CDInfo()->getQBlankMeas(atip->GetLeadOutPos(), GetWriteSpeed());
            break;

         default:
            meas = 0;
      }
   }

   err = SetWriteMethod(DRT_WriteMethod_TAO);
   if (err) return err;

   switch (met) {
      case DRT_Blank_Complete:
         type = cmd_Blank::Blank_All;
         break;
      case DRT_Blank_Fast:
         type = cmd_Blank::Blank_Minimal;
         break;
      case DRT_Blank_Session:
         type = cmd_Blank::Blank_Session;
         break;
      default:
         return ODE_IllegalParameter;
   }

   Calibrate();

   cmd_Blank  *blk = new cmd_Blank(dio);
   blk->setType(type, 0);
   blk->setImmediate(true);

   if (0 != meas)
      meas->begin();

   err = blk->Go();
   WaitOpComplete();

   if ((err == ODE_OK) && (0 != meas))
      meas->end();

   meas = 0;

//   if (err == ODE_OK) Init();
   RequestUpdate();

   delete blk;
   return err;
}

int Disc_CD_RW::FormatDisc(int met)
{
   int          err;
   cmd_StartStopUnit    ssu(dio);

   ssu.setType(cmd_StartStopUnit::StartStop_Start);
   ssu.Go();
   
   err = SetWriteMethod(DRT_WriteMethod_Packet);
   if (err) return err;

   readfmtcaps->Go();
//   if (readfmtcaps->IsFormatted()) 
//      return ODE_OK;

   cmd_Format  *fmt;

   fmt = new cmd_Format(dio);

   if (NULL != atip)
   {
      meas = drive->GetHardwareConfig()->CDInfo()->getCFormatMeas(atip->GetLeadOutPos(), GetWriteSpeed());
   }


//   fmt->setType(cmd_Format::Format_MRW_FullFormat, 0xffffffff, 0);
//   fmt->setImmediate(0);

   fmt->setType(cmd_Format::Format_CD_DVD_FullFormat, readfmtcaps->GetMaxCapacity(), 0);
   fmt->setImmediate(true);

   Calibrate();

   if (0 != meas)
      meas->begin();
   err = fmt->Go();
   WaitOpComplete();
   if ((err == ODE_OK) && (0 != meas))
      meas->end();

   if (!err)
      err = fmt->Go();

   meas = 0;
      
   RequestUpdate();
   delete fmt;
   return err;
}

int Disc_CD_RW::StructureDisc(void)
{
   int err;
   err = EraseDisc(DRT_Blank_Fast);
   if (err) return err;
   return ODE_OK;
};

int Disc_CD_RW::CloseDisc(int lType, int lTrack)
{
   if (DiscType() != DRT_Profile_CD_MRW)
      return Disc_CD_R::CloseDisc(lType, lTrack);
      
   cmd_Close *cl = new cmd_Close(dio);

   cl->setType(cmd_Close::Close_FlushBuffers, 0);
   cl->setImmediate(true);
   cl->Go();
   WaitOpComplete();

   if (lType == DRT_Close_Track) {
      cl->setType(cmd_Close::Close_FlushBuffers, 0);
   } else if (lType == DRT_Close_Session) {
      cl->setType(cmd_Close::Close_CDMRW_StopFormat, 0);
   } else if (lType == DRT_Close_Finalize) {
      cl->setType(cmd_Close::Close_CDMRW_Finalize, 0);
   }

   /*
    * do not let immediate go up here. immediate flush causes problems.
    */
   //cl->setImmediate(true);
   int error = cl->Go();
   WaitOpComplete();
   delete cl;
   return error;
}

bool Disc_CD_RW::AllowMultiSessionLayout()
{
   if (DiscType() == DRT_Profile_CD_MRW)
      return false;
   else
      return Disc_CD_R::AllowMultiSessionLayout();
}

bool Disc_CD_RW::AllowMultiTrackLayout()
{
   if (DiscType() == DRT_Profile_CD_MRW)
      return false;
   else
      return Disc_CD_R::AllowMultiTrackLayout();
}

int Disc_CD_RW::SessionGapSize()
{
   if (DiscType() == DRT_Profile_CD_MRW)
      return 0;
   else
      return Disc_CD_R::SessionGapSize();
}

int Disc_CD_RW::TrackGapSize()
{
   if (DiscType() == DRT_Profile_CD_MRW)
      return false;
   else
      return Disc_CD_R::TrackGapSize();
}

int Disc_CD_RW::DiscType()
{
   if (IsFormatted()) 
      return DRT_Profile_CD_MRW;
   return DRT_Profile_CD_RW;
}

int Disc_CD_RW::DiscSubType()
{
   if (atip) 
   {
      _D(Lvl_Info, "Analysing ATIP data...");
      switch (atip->GetDiscSubType()) 
      {
         case 0:
            _D(Lvl_Debug, "Got Low-Speed rewritable disc.");
            return DRT_SubType_CD_RW_LowSpeed;
         case 1:
            _D(Lvl_Debug, "Got High-Speed rewritable disc.");
            return DRT_SubType_CD_RW_HighSpeed;
         case 2:
            _D(Lvl_Debug, "Got Ultra-Speed rewritable disc.");
            return DRT_SubType_CD_RW_UltraSpeed;
         default:
            _D(Lvl_Debug, "Unknown disc sub type %ld.", atip->GetDiscSubType());
            return DRT_SubType_Unknown;
      }
   } 
   else 
   {
      _D(Lvl_Debug, "No atip data, unknown disc sub type.");
      return DRT_SubType_Unknown;
   }
}

bool Disc_CD_RW::IsOverwritable(void)
{
   if (DiscType() == DRT_Profile_CD_MRW)
      return true;

   cmd_GetConfiguration::Feature *x;

   x = drive->GetDriveFeature(cmd_GetConfiguration::Feature_RigidOverwrite);
   if (x)
      if (x->IsCurrent()) return true;

   x = drive->GetDriveFeature(cmd_GetConfiguration::Feature_RestrictedOverwrite);
   if (x)
      if (x->IsCurrent()) return true;

   return false;

};

bool Disc_CD_RW::IsWritable(void)
{
   if (DiscType() == DRT_Profile_CD_MRW)
      return true;
   return Disc_CD_R::IsWritable();
};

bool Disc_CD_RW::IsFormatted()
{
   if (0 == readfmtcaps)
      return false;
   #warning Zwracamy false poki nie naprawimy IsFormatted()!!!
//   if (readfmtcaps->IsFormatted())
//      return true;
   return false;  
}

bool Disc_CD_RW::IsFormatable()
{
   if (drive->GetDriveFeature(cmd_GetConfiguration::Feature_CD_MRW)) 
      return true;
   return false;
}

int Disc_CD_RW::CheckItemData(const IOptItem *pDI)
{
   if (DiscType() != DRT_Profile_CD_MRW)
   {
      return Disc_CD_R::CheckItemData(pDI);
   }
   
   if (pDI->getPreGapSize() != 0)
   {
      _D(Lvl_Error, "Pad size other than 0 not allowed");
      return ODE_BadLayout;
   }
   if (pDI->isIncremental())
   {
      _D(Lvl_Error, "Incremental tracks not allowed");
      return ODE_BadLayout;
   }
   if (pDI->hasCDText())
   {
      _D(Lvl_Error, "CD-Text allowed only on CD media");
      return ODE_BadLayout;
   }
   if (pDI->isPreemphasized())
   {
      _D(Lvl_Error, "Preemphasy allowed only on CD media");
      return ODE_BadLayout;
   }
   
   if (pDI->getItemType() == Item_Disc)
   {
      if (pDI->getFlags() && DIF_Disc_MasterizeCD)
      {
         _D(Lvl_Error, "Disc can not be masterized");
         return ODE_BadLayout;
      }
   }
   else if (pDI->getItemType() == Item_Session)
   {
   }
   else if (pDI->getItemType() == Item_Track)
   {
      if (pDI->getDataType() != Data_Mode1)
      {
         _D(Lvl_Error, "Only Data/Mode1 tracks allowed on MRW media");
         return ODE_BadLayout;
      }
      if (pDI->getSectorSize() != 2048)
      {
         _D(Lvl_Error, "Invalid sector size");
         return ODE_BadLayout;
      }
   }
   else if (pDI->getItemType() == Item_Index)
   {
      _D(Lvl_Error, "Indices not allowed");
      return ODE_BadLayout;
   }

   return Disc::CheckItemData(pDI);
} 

int Disc_CD_RW::BeginTrackWrite(const IOptItem*pDI)
{
   if (DiscType() != DRT_Profile_CD_MRW)
   {
      return Disc_CD_R::BeginTrackWrite(pDI);
   }
   const_cast<IOptItem*>(pDI)->setPreGapSize(0);
   return ODE_OK;
}

int Disc_CD_RW::EndTrackWrite(const IOptItem*pDI)
{
   return ODE_OK;
}

int16 Disc_CD_RW::GetOperationProgress()
{
   if (meas != 0)
   {
      return meas->getProgress();
   }
   return Disc::GetOperationProgress();
}
   
uint32 Disc_CD_RW::GetDiscSize()
{
//   if (0 != readfmtcaps)
//      return readfmtcaps->GetMaxCapacity();
   return Disc_CD_R::GetDiscSize();
}

bool Disc_CD_RW::wantCDText() const
{
   return false;
}

