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
#include "Disc_CD_R.h"
#include "Commands.h"
#include "Config.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Disc_CD_R::Disc_CD_R(Drive *d) : Disc_CD_ROM(d)
{
   bWriteSAO   = false;
   bCDXA       = false;
   bCDI        = false;
   pLastTrack  = 0;
   lSAOTrackGap= 0;
   pCueSheet   = 0;
   lCueElements= 0;
   atip        = 0;
   lAdjustment = 0;
   builder     = 0;
};

Disc_CD_R::~Disc_CD_R(void)
{
   if (atip) 
      delete (atip);
   if (builder)
      delete builder;
};

void Disc_CD_R::Init(void)
{
   Disc_CD_ROM::Init();

   if (0 != pDiscInfo)
   {
      lLeadInLength = pDiscInfo->GetLeadInLength();
      lLeadInStart  = pDiscInfo->GetLeadInStart();
      _D(Lvl_Info, "First leadin sector offset: %ld\n", pDiscInfo->GetLeadInLength());
   }
   else
   {
      lLeadInStart  = 0;      // we cannot upload
      lLeadInLength = 0;      // cd-text information
   }
   
   cmd_ReadTOC *toc = new cmd_ReadTOC(dio);
   if (atip) delete (atip);
   atip = toc->GetATIP();
   if (atip) 
   {
      TOC_ATIP *new_atip;
      new_atip = new TOC_ATIP;
      *new_atip = *atip;
      atip = new_atip;
      _D(Lvl_Info, "ATIP Leadin position: %08lx", atip->GetLeadInPos());
   }
   delete toc;
};

int Disc_CD_R::CloseDisc(int type, int lTrackNo)
{
   if (bWriteSAO)
   {
      if (type == DRT_Close_Track)
         return ODE_OK;
         
      cmd_Close   *cl = new cmd_Close(dio);
      cl->setType(cmd_Close::Close_FlushBuffers, 0);
      /*
       * do not let immediate go up here. immediate flush causes problems.
       */
      //cl->setImmediate(true);
      cl->Go();
      WaitOpComplete();
      delete cl;
      return ODE_OK;
   }
   else
   {
      cmd_Close        *cl       = new cmd_Close(dio);
      Page<Page_Write> &pWrite   = drive->GetWritePage();
   
      cl->setType(cmd_Close::Close_FlushBuffers, 0);
      /*
       * do not let immediate go up here. immediate flush causes problems.
       */
      //cl->setImmediate(true);
      cl->Go();
      WaitOpComplete();
   
      if (type == DRT_Close_Track) {
         cl->setType(cmd_Close::Close_CDR_Track, lTrackNo);
      } else if (type == DRT_Close_Session) {
         pWrite->SetMultisession(true);
         drive->SetPage(pWrite);
         cl->setType(cmd_Close::Close_CDR_LastSession, 0);
      } else if (type == DRT_Close_Finalize) {
         pWrite->SetMultisession(false);
         drive->SetPage(pWrite);
         cl->setType(cmd_Close::Close_CDR_LastSession, 0);
      }
      int error;
      /*
       * do not let immediate go up here. immediate flush causes problems.
       */
      //cl->setImmediate(true);
      error = cl->Go();
      WaitOpComplete();
      delete cl;
      
//      Init();
      RequestUpdate();
      return error;
   }   
}

int Disc_CD_R::OnChangeWriteMethod()
{
   Page<Page_Write> &pw = drive->GetWritePage();
   if (!pw.IsValid()) 
      return ODE_NoModePage;
   
   DiscWriteMethod dwm = GetWriteMethod();

   if (dwm == DRT_WriteMethod_Default)
   {
      return SetWriteMethod(DRT_WriteMethod_TAO);
   }
   else if (dwm == DRT_WriteMethod_TAO)
   {
      _D(Lvl_Debug, "Setting up CD-R/RW parameters: TAO");
      pw->SetSessionFormat(Page_Write::Session_Data);
      pw->SetPacketSize(0);
      pw->SetDataMode(Page_Write::DataMode_Mode1);

      pw->SetWriteType(Page_Write::WriteType_TrackAtOnce);
      pw->SetTrackMode(Page_Write::TrackMode_Data_Sequential);
      pw->SetMultisession(0);
      pw->SetLinkSize(0);
   }
   else if (dwm == DRT_WriteMethod_SAO)
   {
      _D(Lvl_Debug, "Setting up CD-R/RW parameters: SAO");
      pw->SetSessionFormat(Page_Write::Session_Data);
      pw->SetPacketSize(0);
      pw->SetDataMode(Page_Write::DataMode_Mode1);

      pw->SetWriteType(Page_Write::WriteType_SessionAtOnce);
      pw->SetTrackMode(Page_Write::TrackMode_Data_Sequential);
      pw->SetMultisession(0);
      pw->SetLinkSize(0);
   }
   else if (dwm == DRT_WriteMethod_Packet)
   {
      _D(Lvl_Debug, "Setting up PacketCD / CD-MRW parameters.");
      pw->SetSessionFormat(Page_Write::Session_ExtendedArchitecture);
      pw->SetPacketSize(16);
      pw->SetDataMode(Page_Write::DataMode_Mode1);

      pw->SetWriteType(Page_Write::WriteType_Packet);
      pw->SetTrackMode(Page_Write::TrackMode_Data_Incremental);
      pw->SetMultisession(1);
      pw->SetLinkSize(0);
   }
   else
   {
      return SetWriteMethod(DRT_WriteMethod_Default);
   }
   return drive->SetPage(pw);
}

int Disc_CD_R::DiscSubType(void)
{
   if (0 != atip)
      return atip->GetLeadInPos() % 10;
   return 0;
}

bool Disc_CD_R::IsWritable(void)
{
   if (GetNextWritableTrack(0)) return true;
   return false;
}

int Disc_CD_R::SessionGapSize()
{
   return 11402;
}

int Disc_CD_R::TrackGapSize()
{
   if (!bWriteSAO)
      return 152;
   return lSAOTrackGap;
}

int Disc_CD_R::CheckItemData(const IOptItem *pDI)
{
   if ((pDI->getPreGapSize() != 0) && (!bWriteSAO))
   {
      _D(Lvl_Error, "Pad size other than 0 not allowed");
      return ODE_BadLayout;
   }
   if ((pDI->getPostGapSize() != 0) && (!bWriteSAO))
   {
      _D(Lvl_Error, "Pad size other than 0 not allowed");
      return ODE_BadLayout;
   }
   if (pDI->isIncremental())
   {
      _D(Lvl_Error, "Incremental tracks not allowed");
      return ODE_BadLayout; 
   }
   
   if (pDI->getItemType() == Item_Disc)
   {
      if (pDI->getFlags() & DIF_Disc_MasterizeCD)
      {
         const IOptItem *pItem = FindTrack(1);
         if (!pItem->isBlank())
         {
            _D(Lvl_Error, "SAO / DAO recording requires a blank disc to be inserted");
            return ODE_DiscNotEmpty;
         }

         bWriteSAO = true;
         lAdjustment = -150;
      }
      else
      {
         bWriteSAO   = false;
         lAdjustment = 0;
      }
      pLastTrack = 0;
   }
   else if (pDI->getItemType() == Item_Session)
   {
      if (pLastTrack)
      {
         _D(Lvl_Error, "Multisession SAO / DAO recording not allowed");
         return ODE_BadLayout;
      }
      if (pDI->hasCDText())
      {
         if (!bWriteSAO)
         {
            _D(Lvl_Error, "CD-Text allowed only in DAO / SAO mode");
            return ODE_BadLayout;
         }
         // other session options should follow, like interactive or cd-xa session etc
      }
      bCDXA = pDI->getFlags() & DIF_Session_CDXA ? true : false;
      bCDI  = pDI->getFlags() & DIF_Session_CDI  ? true : false;
      if (pDI->isPreemphasized())
      {
         _D(Lvl_Error, "Preemphasy not allowed for sessions");
         return ODE_BadLayout;
      }
   }
   else if (pDI->getItemType() == Item_Track)
   {
      int secsize = 2048;

      /*
       * ensure at least one index is present
       */
      if (pDI->getChildCount() == 0)
      {
         IOptItem *pIdx = const_cast<IOptItem*>(pDI)->addChild();
         pIdx->setItemNumber(1);
         pIdx->setFlags(DIF_RelativeSize);
         pIdx->setStartAddress(0);
         pIdx->setDataBlockCount(-1);
      }
   
      /* 
       * verify if track comes with cd-text
       */
      if (pDI->hasCDText())
      {
         if (!bWriteSAO)
         {
            _D(Lvl_Error, "CD-Text allowed only in DAO / SAO mode");
            return ODE_BadLayout;
         }
         if (pDI->getDataType() != Data_Audio)
         {
            _D(Lvl_Error, "CD-Text allowed only for audio tracks");
            return ODE_BadTrackMode;
         }
      }
      
      /*
       * verify that raw data comes only for sao recording
       */
      if ((pDI->getFlags() & DIF_Common_Type) != 0)
      {
         if (!bWriteSAO)
         {
            _D(Lvl_Error, "RAW tracks are only allowed in DAO recording");
            return ODE_BadLayout;
         }
      }

      /*
       * verify thar preemphasy is applied only to audio tracks
       */
      if (pDI->isPreemphasized())
      {
         if (pDI->getDataType() != Data_Audio)
         {
            _D(Lvl_Error, "Preemphasy not allowed for data tracks");
            return ODE_BadLayout;
         }
      }
     
      /*
       * position track
       */ 
      if (bWriteSAO)
      {
         int32 gap = 0;
         int32 est = 0;

         if ((pDI->getChildCount() > 0) && (pDI->getChild(0)->getItemNumber() == 0))   // index 0!
            gap = pDI->getChild(0)->getBlockCount();
         else
            gap = pDI->getPreGapSize();

         if (!pLastTrack)
         {
            est = 2*75;
         }
         else if (pDI->getDataType() != Data_Audio)
         {
            if (pLastTrack->getDataType() != pDI->getDataType())
               est = 3*75;
            else
               est = 2*75;
         }
         else if (pLastTrack->getDataType() != Data_Audio)
         {
            est = 2*75;
         }
        
         if ((gap != 0) && (est != gap))
         {
            _D(Lvl_Warning, "Desired gap length (%ld) is different than acceptable (%ld)", gap, est);
            return ODE_BadLayout;
         }
         else if (gap == 0)
         {
            const_cast<IOptItem*>(pDI)->setPreGapSize(est);
         }
         lSAOTrackGap = pDI->getPreGapSize();
      }
         
      switch (pDI->getDataType())
      {
         case Data_Audio:
            {
               secsize = 2352;
            }
            break;

         case Data_Mode1:
            {
               if (pDI->getFlags() & DIF_Common_RawData)
                  secsize = 2352;
               else
                  secsize = 2048;
            }
            break;

         case Data_Mode2:
            {
               if (pDI->getFlags() & DIF_Common_RawData)
                  secsize = 2352;
               else
                  secsize = 2336;
            }
            break;

         case Data_Mode2Form1:
            {
               if (!(bCDXA | bCDI))
               {
                  _D(Lvl_Error, "XA tracks allowed only for XA / Interactive sessions");
                  return ODE_BadLayout;
               }
               if (pDI->getFlags() & DIF_Common_RawData)
                  secsize = 2352;
               else
                  secsize = 2048;
            }
            break;

         case Data_Mode2Form2:
            {
               if (!(bCDXA | bCDI))
               {
                  _D(Lvl_Error, "XA tracks allowed only for XA / Interactive sessions");
                  return ODE_BadLayout;
               }
               if (pDI->getFlags() & DIF_Common_RawData)
                  secsize = 2352;
               else
                  secsize = 2336;
            }
            break;

         default:
            _D(Lvl_Error, "Unrecognized data type selected");
            return ODE_BadLayout;
      }

      if (pDI->getFlags() & DIF_Common_SubChannel)
         secsize += 96;

      const_cast<IOptItem*>(pDI)->setSectorSize(secsize);
      pLastTrack = pDI;
   }
   else if (pDI->getItemType() == Item_Index)
   {
      if (!bWriteSAO) 
      {
         _D(Lvl_Error, "Indices allowed only in DAO / SAO mode");
         return ODE_BadLayout;
      }
#warning INDICES NOT IMPLEMENTED
      _D(Lvl_Error, "Indices are not implemented yet");
      return ODE_BadLayout;
   }

   return Disc::CheckItemData(pDI);
} 

int Disc_CD_R::BeginTrackWrite(const IOptItem*pDI)
{
   Page<Page_Write> &pW = drive->GetWritePage();

   if (bWriteSAO == true)
   {
      if (wantCDText())
      {
         uint8*   buff;
         int32    size;

         _D(Lvl_Warning, "Writing session data");

         if (!builder->getDataBuffer(buff, size))
         {
            _D(Lvl_Warning, "unable to obtain CDText block! filling sectors with zeroes!");
            size = 96*32;
            buff = new uint8[size];
            memset(buff, 0, size);
         }

         /*
          * here we need a:
          * - atip leadin location (taken from atip msf, minus 450150 blocks),
          * - total number of blocks to write (-atip_loc-150)
          * - data to write ;]
          */

         int32    inpos = atip->GetLeadInPos();
         int32    sects = -inpos - 150;
         int32    lcnt  = 0;

         while (sects > 0)
         {
            int32 cnt = sects < 32 ? sects : 32;
            cnt = cnt < (size - lcnt) ? cnt : (size - lcnt);

            WriteDisc(inpos, cnt, 96, &buff[lcnt * 96]);

            lcnt  += cnt;
            inpos += cnt;
            sects -= cnt;

            if (lcnt == size)
               lcnt = 0;
         }

         delete [] buff;
         delete builder;
#warning are we leaking memory here if no cdtext is wanted????
         cdText = false;
         builder = 0;
      }
      return 0;
   }

   if (pDI->getItemType() == Item_Disc)
   {
      pW->SetSessionFormat(bCDXA ? Page_Write::Session_ExtendedArchitecture :
            bCDI  ? Page_Write::Session_Interactive :
            Page_Write::Session_Data);      
   }
   else if (pDI->getItemType() == Item_Track)
   {
      switch (pDI->getDataType())
      {
         case Data_Unknown:
         case Data_Audio:
            pW->SetDataMode(Page_Write::DataMode_Raw);
            pW->SetTrackMode(Page_Write::TrackMode_Audio);
            break;
         case Data_Mode1:
            pW->SetDataMode(Page_Write::DataMode_Mode1);
            pW->SetTrackMode(Page_Write::TrackMode_Data_Sequential);
            break;
         case Data_Mode2:
            pW->SetDataMode(Page_Write::DataMode_Mode2);
            pW->SetTrackMode(Page_Write::TrackMode_Data_Sequential);
            break;
         case Data_Mode2Form1:
            pW->SetDataMode(Page_Write::DataMode_Mode2Form1);
            pW->SetTrackMode(Page_Write::TrackMode_Data_Sequential);
            break;
         case Data_Mode2Form2:
            pW->SetDataMode(Page_Write::DataMode_Mode2Form2);
            pW->SetTrackMode(Page_Write::TrackMode_Data_Sequential);
            break;
      }
   }
   return drive->SetPage(pW);
}

int Disc_CD_R::EndTrackWrite(const IOptItem*pDI)
{
   return ODE_OK;
}

int Disc_CD_R::UploadLayout(const IOptItem *pDI)
{
   int err;
   int8 m, s, f;
   uint32 lBlock;

   if (pDI->getFlags() & DIF_Disc_MasterizeCD)
   {
      _D(Lvl_Info, "Requesting SAO Writing");
      SetWriteMethod(DRT_WriteMethod_SAO);
      bWriteSAO = true;
      lAdjustment = -150;
   }
   else
   {
      _D(Lvl_Info, "Requesting TAO Writing");
      SetWriteMethod(DRT_WriteMethod_TAO);
      bWriteSAO = false;
      lAdjustment = 0;
   }

   Page<Page_Write> &pW = drive->GetWritePage();

   if (pDI->getFlags() & DIF_Disc_CloseSession)
   {
      pW->SetMultisession(true);
   }
   else if (pDI->getFlags() & DIF_Disc_CloseDisc)
   {
      pW->SetMultisession(false);
   }
   else
   {
      _D(Lvl_Error, "Please specify disc closure type");
      return ODE_BadLayout;
   }
   drive->SetPage(pW);
  
   err = Disc::UploadLayout(pDI);

   if (err != ODE_OK)
      return err;


   if (bWriteSAO) 
   {
      Page<Page_Write> &pW = drive->GetWritePage();
      pW->SetWriteType(Page_Write::WriteType_SessionAtOnce);
      drive->SetPage(pW);
      
      /*
       * cd text
       */
      if (builder)
         delete builder;
      builder = new CDText(pDI);
      builder->printReport(DEBUG_ENGINE);
      cdText = builder->hasCDText();
      if (wantCDText())
      {
         _D(Lvl_Info, "We want to record CD Text");
         if (atip == 0)
         {
            cdText = false;
            _D(Lvl_Info, "but we need valid atip data to do that.");
         }
      }

      #warning nie mam pewnosci czy CD-MRW czasem tu nie wlezie...
      // we have to create a CUE SHEET out of this data
      // since everything is almost ready - we just fill up the rest.
      lCueElements = 0;
      if (!pCueSheet)
         pCueSheet = new uint8[4096]; // we will not need more
         
      const IOptItem *pItem = pDI;
      
      if (pItem->getChildCount() > 1)
         return ODE_TooManySessions;
         
      atip->GetLeadInPos(m, s, f);
      _D(Lvl_Info, "LeadIn Info: %ld / %ld / %ld", m, s, f);

      if (wantCDText())
      {
         atip->GetLeadInPos(m, s, f);

         pCueSheet[lCueElements*8 + 0] = 1;
         pCueSheet[lCueElements*8 + 1] = 0;
         pCueSheet[lCueElements*8 + 2] = 0;
         pCueSheet[lCueElements*8 + 3] = 0x41;
         pCueSheet[lCueElements*8 + 4] = 0;
         pCueSheet[lCueElements*8 + 5] = m;
         pCueSheet[lCueElements*8 + 6] = s;
         pCueSheet[lCueElements*8 + 7] = f;
         lCueElements++;
      }
      else
      {
         pCueSheet[lCueElements*8 + 0] = 1;
         pCueSheet[lCueElements*8 + 1] = 0;
         pCueSheet[lCueElements*8 + 2] = 0;
         pCueSheet[lCueElements*8 + 3] = 1;
         pCueSheet[lCueElements*8 + 4] = 0;
         pCueSheet[lCueElements*8 + 5] = 0;
         pCueSheet[lCueElements*8 + 6] = 0;
         pCueSheet[lCueElements*8 + 7] = 0;
         lCueElements++;
      }

      for (int trkno = 0; trkno < pDI->getChild(0)->getChildCount(); trkno++)
      {
         pItem = pDI->getChild(0)->getChild(trkno);
         uint8 ttype = 0;
         uint8 dtype = 0;
               
         switch (pItem->getDataType())
         {
            case Data_Audio:
               {
                  ttype =  (pItem->isPreemphasized() ? 0x11 : 0x01);
                  dtype =  0;
               }
               break;

            case Data_Mode1:
               {
                  ttype =  0x41;
                  if (!(pItem->getFlags() & DIF_Common_RawData))
                     dtype = 0x10;
                  else
                     dtype = 0x11;
               }
               break;

            case Data_Mode2:
               {
                  ttype =  0x41;
                  if (!(pItem->getFlags() & DIF_Common_RawData))
                     dtype = 0x30;
                  else
                     dtype = 0x31;
               }
               break;

            case Data_Mode2Form1:
            case Data_Mode2Form2:
               {
                  ttype =  0x41;
                  if (!(pItem->getFlags() & DIF_Common_RawData))
                     dtype = 0x20;
                  else
                     dtype = 0x21;
               }
               break;

            default:
               return ODE_BadTrackMode;
         }
         
         if (pItem->getFlags() & DIF_Common_SubChannel)
            dtype |= 0xc0;


         if (pItem->isProtected())
            ttype |= 0x20;

         if (pItem->getPreGapSize())
         {
            uint32 lBlock = pItem->getStartAddress();
            pCueSheet[lCueElements*8 + 7] = (lBlock - LayoutAdjustment()) % 75;
            pCueSheet[lCueElements*8 + 6] = ((lBlock - LayoutAdjustment()) / 75) % 60;
            pCueSheet[lCueElements*8 + 5] = ((lBlock - LayoutAdjustment()) / 75) / 60;
            pCueSheet[lCueElements*8 + 4] = 0;
            pCueSheet[lCueElements*8 + 3] = dtype;
            pCueSheet[lCueElements*8 + 2] = 0;
            pCueSheet[lCueElements*8 + 1] = pItem->getItemNumber();
            pCueSheet[lCueElements*8 + 0] = ttype;
            lCueElements++;
         }


         for (int idxno = 0; idxno < pItem->getChildCount(); idxno++)
         {
            const IOptItem *pIndex = pItem->getChild(idxno);
            lBlock = pIndex->getStartAddress();

            pCueSheet[lCueElements*8 + 7] = (lBlock - LayoutAdjustment()) % 75;
            pCueSheet[lCueElements*8 + 6] = ((lBlock - LayoutAdjustment()) / 75) % 60;
            pCueSheet[lCueElements*8 + 5] = ((lBlock - LayoutAdjustment()) / 75) / 60;
            pCueSheet[lCueElements*8 + 4] = 0;                       // alternate copy = 0x80;
            pCueSheet[lCueElements*8 + 3] = dtype;
            pCueSheet[lCueElements*8 + 2] = pIndex->getItemNumber(); // if there is a gap, track does not start here
            pCueSheet[lCueElements*8 + 1] = pItem->getItemNumber();
            pCueSheet[lCueElements*8 + 0] = ttype;

            lCueElements++;            
         }            
      }

      _D(Lvl_Info, "Final sector: %ld, offset: %ld", pDI->getEndAddress(), LayoutAdjustment());
      lBlock = pDI->getEndAddress();

      pCueSheet[lCueElements*8 + 7] = (lBlock - LayoutAdjustment() + 1) % 75;
      pCueSheet[lCueElements*8 + 6] = ((lBlock - LayoutAdjustment() + 1)/75) % 60;
      pCueSheet[lCueElements*8 + 5] = ((lBlock - LayoutAdjustment() + 1) / 75) / 60;
      pCueSheet[lCueElements*8 + 4] = 0;                      
      pCueSheet[lCueElements*8 + 3] = 0x01;
      pCueSheet[lCueElements*8 + 2] = 0x01;
      pCueSheet[lCueElements*8 + 1] = 0xaa;
      pCueSheet[lCueElements*8 + 0] = 0x01;
      lCueElements++;
      
      cmd_SendCueSheet *pSCS = new cmd_SendCueSheet(dio);
      pSCS->SendCueSheet(pCueSheet, lCueElements);

      err = pSCS->Go();
      delete pSCS;
      delete [] pCueSheet;
      pCueSheet = 0;
      if (err) return err;
   }   

   return ODE_OK;
}

const char *Disc_CD_R::DiscVendor()
{
   if (atip != 0)
      return Cfg->Vendors()->getVendor(atip->AtipLeadInM(), atip->AtipLeadInS(), atip->AtipLeadInF());
   return Disc::DiscVendor();
}

uint32 Disc_CD_R::GetDiscSize()
{
   if (0 != atip)
      return atip->GetLeadOutPos();
   return Disc::GetDiscSize();
}

bool Disc_CD_R::wantCDText() const
{
   return cdText;
}

