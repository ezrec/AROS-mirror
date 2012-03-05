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
#include "Disc_DVD_PlusR.h"

Disc_DVD_PlusR::Disc_DVD_PlusR(Drive *d) : Disc_DVD_ROM(d)
{
};

void Disc_DVD_PlusR::Init(void)
{
   Disc_DVD_ROM::Init();
}

Disc_DVD_PlusR::~Disc_DVD_PlusR(void)
{
};

int Disc_DVD_PlusR::CloseDisc(int type, int lTrackNo)
{
   cmd_Close *cl = new cmd_Close(dio);

   cl->setType(cmd_Close::Close_FlushBuffers, 0);
   /*
    * do not let immediate go up here. immediate flush causes problems.
    */
   //cl->setImmediate(true);
   cl->Go();
   WaitOpComplete();

   if (type == DRT_Close_Track) {
      cl->setType(cmd_Close::Close_DVDPlusR_Track, lTrackNo);
   } else if (type == DRT_Close_Session) {
      cl->setType(cmd_Close::Close_DVDPlusR_LastSession, 0);
   } else if (type == DRT_Close_Finalize) {
      cl->setType(cmd_Close::Close_DVDPlusR_FinalizeNominal, 0);
   }
   int error = cl->Go();
   /*
    * do not let immediate go up here. immediate flush causes problems.
    */
   //cl->setImmediate(true);
   WaitOpComplete();
   delete cl;

//   Init();
   RequestUpdate();

   return error;
}

int Disc_DVD_PlusR::OnChangeWriteMethod()
{
   DiscWriteMethod dwm = GetWriteMethod();            // works exactly same as -R/RW version

   if (dwm == DRT_WriteMethod_Default)
   {
      return SetWriteMethod(DRT_WriteMethod_Packet);  // a small loopback
   }
   else if (dwm != DRT_WriteMethod_Packet)
   {
      SetWriteMethod(DRT_WriteMethod_Packet);         // reject
      return ODE_IllegalType;
   }

   Page<Page_Write> &pw = drive->GetWritePage();
   if (!pw.IsValid()) return ODE_NoModePage;
   _D(Lvl_Debug, "Setting up DVD+R/+RW/-RAM parameters: packet writing enabled");
   pw->SetSessionFormat(Page_Write::Session_Data);
   pw->SetPacketSize(16);
   pw->SetDataMode(Page_Write::DataMode_Mode1);
   pw->SetWriteType(Page_Write::WriteType_Packet);
   pw->SetTrackMode(Page_Write::TrackMode_Data_Incremental);
   pw->SetMultisession(1);
   pw->SetLinkSize(0);
   return drive->SetPage(pw);
}

int Disc_DVD_PlusR::DiscSubType()
{
   cmd_GetConfiguration::Feature *x;

   x = drive->GetDriveFeature(cmd_GetConfiguration::Feature_DVD_PlusR_DualLayer);
   if (x == 0)
   {
      return DRT_SubType_Unknown;
   }

   if (x->IsCurrent()) {
      _D(Lvl_Debug, "Got DualLayer DVD+R media! Yuppie!");
      return DRT_SubType_DVD_DualLayer;
   } else {
      _D(Lvl_Debug, "Got SingleLayer DVD+R media..");
      return DRT_SubType_Unknown;
   }
}

bool Disc_DVD_PlusR::IsWritable()
{
   if (GetNextWritableTrack(0))
      return true;
   return false;
}

int Disc_DVD_PlusR::CheckItemData(const IOptItem *pDI)
{
// FIXME: INCREMENTAL TRACKS PASSED ON

   const_cast<IOptItem*>(pDI)->setIncremental(true);

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
   if (pDI->getPreGapSize() != 0)
   {
      _D(Lvl_Error, "Pad size other than 0 not allowed");
      return ODE_BadLayout;
   }

   if (pDI->getItemType() == Item_Disc)
   {
      if (pDI->getFlags() & DIF_Disc_MasterizeCD)
      {
         _D(Lvl_Error, "DVD discs can not be masterized");
         return ODE_BadLayout;
      }
      const_cast<IOptItem*>(pDI)->setSectorSize(2048);
   }
   else if (pDI->getItemType() == Item_Session)
   {
      const_cast<IOptItem*>(pDI)->setSectorSize(2048);
   }
   else if (pDI->getItemType() == Item_Track)
   {
      if (pDI->getDataType() != Data_Mode1)
      {
         _D(Lvl_Error, "Only Data/Mode1 tracks allowed on DVD media");
         return ODE_BadTrackMode;
      }
      const_cast<IOptItem*>(pDI)->setSectorSize(2048);
      const_cast<IOptItem*>(pDI)->setIncremental(true);
   }
   else if (pDI->getItemType() == Item_Index)
   {
      _D(Lvl_Error, "Indices allowed only on CD media");
      return ODE_BadLayout;
   }

   return Disc::CheckItemData(pDI);
}
