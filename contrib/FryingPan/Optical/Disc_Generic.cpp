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
#include "Disc_Generic.h"
#include "Drive.h"
#include "Config.h"
#include "OptTrack.h"
#include "OptSession.h"


// TODO: przerobiæ ca³y ten ba³agan.

Disc::Disc(Drive *d)
{
   lSeqSector     = 0;
   lSeqLastSector = 0;
   pSeqStructure  = 0;
   pSeqCurrentTrack  = 0;
   read_speed     = 0xffff;
   write_speed    = 0xffff;
   write_method   = DRT_WriteMethod_Default;
   refresh_period = 0;
   pReadDiscInfo  = 0;
   pDiscInfo      = 0;
   bNeedUpdate    = false;
   dio            = d->GetDriveIO();
   drive          = d;
   SET_DEBUG_ENGINE(d->GetDebug());
   bNeedUpdate    = false;
   write_method   = DRT_WriteMethod_Default;   // do not use SetWriteMethod!!!
   pRead          = new cmd_Read(dio);
   pWrite         = new cmd_Write(dio);
   
   optcontent     = new OptDisc();
};

Disc::~Disc()
{
   optcontent->dispose();

   delete pReadDiscInfo;      // disposes pDiscInfo as well!
   delete pRead;
   delete pWrite;
};

void Disc::Init()
{
   pReadDiscInfo = new cmd_ReadDiscInfo(dio);
   if (pReadDiscInfo->Go() == ODE_OK)
   {
      pDiscInfo = pReadDiscInfo->GetData();
   }
   else
   {
      pDiscInfo = 0;  
   }
   
   SetRefreshPeriod(150);
   
   bool res = false;
   res = tryReadTrackInfo();
   if (!res)
      res = tryReadTOC();
   if (!res)
      res = tryPrimitiveTOC();

   read_speed = 0;
   write_speed = 0;
   SetWriteMethod(GetWriteMethod());   // assure no change is made

   ReadSpeeds();
}

const IOptItem *Disc::GetContents()
{
   return optcontent;
}

bool Disc::tryReadTrackInfo(void)
{
   IOptItem            *osess = 0;
   IOptItem            *otrak = 0;
   int                  current_track;
   TrackInfo           *i;
   int                  current_session = -1;
   cmd_ReadTrackInfo    rti(dio);

   if (!pDiscInfo) 
      return false;
   if (!drive->MediaWriteCapabilities()) 
      return false;                          // in such case drive is unable to provide us the TrackInfos

   for (current_track = 1; current_track <= pDiscInfo->GetNumTracks(); current_track++)
   {
      rti.SelectTrack(current_track);
      rti.Go();
      if (rti.OpticalError()) break;

      i = rti.GetData();

      if (i->GetSessionNumber() != current_session)
      {
         if (osess)
            osess->printReport(drive->GetDebug());

         osess = optcontent->addChild();
         osess->setItemNumber(i->GetSessionNumber());
         osess->setComplete(true);
         current_session = i->GetSessionNumber();
      }
   
      otrak = osess->addChild();
      i->FillInDiscItem(otrak);
      
      otrak->printReport(drive->GetDebug());
   }

   /* if read track info is available, read disc info is available, too. */

   if (osess)
   {
      osess->setComplete(!pDiscInfo->IsLastSessionIncomplete());
      osess->setBlank(pDiscInfo->IsLastSessionEmpty());
   }
   if (optcontent)
   {
      optcontent->setComplete(!pDiscInfo->IsDiscIncomplete());
      optcontent->setBlank(pDiscInfo->IsDiscEmpty());
   }

   if (osess)
      osess->printReport(drive->GetDebug());
   optcontent->printReport(drive->GetDebug());
   return true;
};

bool Disc::tryReadTOC(void)
{
   int                  current_track;
   cmd_ReadTOC          rtc(dio);
   TOC_FullTOC         *toc = 0;
   int                  current_session=0;
   IOptItem            *osess = 0;
   IOptItem            *otrak = 0;

   toc = rtc.GetFullTOC();
   if (0 == toc) 
      return false;

   for (current_track=1; current_track<=toc->GetNumTracks(); current_track++) 
   {
      if (toc->FindTOCEntry(current_track)->GetTrackSession() != current_session) 
      {
         if (osess)
            osess->printReport(drive->GetDebug());
         
         current_session = toc->FindTOCEntry(current_track)->GetTrackSession();
         
         osess = optcontent->addChild();
         osess->setItemNumber(current_session);
      }

      otrak = osess->addChild();
      toc->FillInDiscItem(otrak, current_track);

      otrak->printReport(drive->GetDebug());
   }

   if (osess)
      osess->printReport(drive->GetDebug());
   optcontent->printReport(drive->GetDebug());
   return true;
};

bool Disc::tryPrimitiveTOC(void)
{
   int                  current_track;
   cmd_ReadTOC          rtc(dio);
   TOC_PrimitiveTOC    *toc = 0;
   int                  current_session=0;
   IOptItem            *osess = 0;
   IOptItem            *otrak = 0;

   toc = rtc.GetTOC(false);
   if (!toc) 
      return false;

   for (current_track=1; current_track<=toc->GetNumTracks(); current_track++) 
   {
      if (toc->FindTOCEntry(current_track)->GetTrackSession() != current_session) 
      {
         if (osess)
            osess->printReport(drive->GetDebug());

         current_session = toc->FindTOCEntry(current_track)->GetTrackSession();

         osess = optcontent->addChild();
         osess->setItemNumber(current_session);
      }

      otrak = osess->addChild();
      toc->FillInDiscItem(otrak, current_track);

      otrak->printReport(drive->GetDebug());
   }

   if (osess)
      osess->printReport(drive->GetDebug());
   optcontent->printReport(drive->GetDebug());
   return true;
};


int Disc::WriteData(const IOptItem *di)
{
   return ODE_CommandError;
}

void Disc::SetRefreshPeriod(UBYTE fiftieths)
{
   if (fiftieths > 250) {
      _D(Lvl_Error, "Extreme refresh period truncated.");
      fiftieths = 250;
   }

   if (fiftieths < 10) {
      _D(Lvl_Warning, "Refresh period set too small, correcting BUT IT IS STILL SHORT ANYWAYS.");
      fiftieths = 10;
   }
   refresh_period = fiftieths;
}

const UBYTE Disc::GetRefreshPeriod(void)
{
   return refresh_period;
};

int Disc::RandomRead(const IOptItem *i, int32 first, int32 count, void* buff)
{
   if (0 == i)
      i = optcontent;

   if (first < i->getStartAddress())
      return ODE_BadBlockNumber;
   if ((first+count) > (i->getEndAddress()+1))
      return ODE_BadBlockNumber;
   if (!buff)                          
      return ODE_NoMemory;

   int err = pRead->ReadData(first, count, buff);
   return err;
}

int Disc::RandomWrite(const IOptItem *i, int32 first, int32 count, void* buff)
{
   if (0 == i)
      i = optcontent;

   if (first < i->getStartAddress())
      return ODE_BadBlockNumber;
   if ((first + count) > i->getEndAddress())
      return ODE_BadBlockNumber;
   if (!buff)                          
      return ODE_NoMemory;

   return WriteDisc(first, count, i->getSectorSize(), buff);
}

int Disc::WriteDisc(int lBlock, unsigned short lLength, unsigned short lSectorSize, APTR pBuffer)
{
   int err = 0;
   for (;;) {
      err = pWrite->WriteData(lBlock, lLength, lSectorSize, pBuffer);
      if (!err) break;
      if (err) {
         if ((pWrite->SCSIError() & 0xff0000) != 0x20000) break;
      }
      DOS->Delay(20);
   }
   return err;
}


int Disc::CloseDisc(int, int)
{
   return ODE_IllegalCommand;
};

int Disc::UploadLayout(const IOptItem *pDI)
{
   if (pDI == 0)
      return ODE_BadLayout;
   if (pDI->getItemType() != Item_Disc)               // regular hierarchy
      return ODE_BadLayout;
   if (pDI->getChildCount() != 1)                     // with just one session (we don't burn more)
      return ODE_BadLayout;
   if (pDI->getChild(0)->getChildCount() == 0)        // and at least one track
      return ODE_BadLayout;

   pSeqStructure     = pDI;
   pSeqCurrentTrack  = pDI->getChild(0)->getChild(0);

   lSeqSector        = pSeqCurrentTrack->getStartAddress();
   lSeqLastSector    = pSeqCurrentTrack->getEndAddress();

   _D(Lvl_Info, "The following structure will be recorded:");
   pDI->printReport(DEBUG_ENGINE);
   for (int s=0; s<pDI->getChildCount(); s++)
   {
      pDI->getChild(s)->printReport(DEBUG_ENGINE);
      for (int t=0; t<pDI->getChild(s)->getChildCount(); t++)
      {
         pDI->getChild(s)->getChild(t)->printReport(DEBUG_ENGINE);
      }
   }

   cmd_Calibrate cal(dio);
   return cal.Go();
};

int Disc::GetNumTracks()
{
   const IOptItem *i;

   if (optcontent->getChildCount() == 0)
      return 0;
   i = optcontent->getChild(optcontent->getChildCount() - 1);

   ASSERT(i->getChildCount() != 0);

   return i->getChild(i->getChildCount() - 1)->getItemNumber();
};

int Disc::GetNumSessions()
{
   if (optcontent->getChildCount() == 0)
      return 0;
   return (optcontent->getChild(optcontent->getChildCount() - 1)->getItemNumber());
};

int Disc::SetReadSpeed(uint16 rd)
{
   int err = 0;
   
   if (read_speed == rd) 
      return ODE_OK;

   cmd_SetSpeed            ss(dio);
   cmd_Mode                md(dio);
   Page<Page_Capabilities> cap;

   err = ss.SetSpeed(rd, write_speed);
   if (err == 0)
   {
      cap = md.GetPage(cmd_Mode::Id_Page_Capabilities);
      read_speed = cap->GetCurrentReadSpeed();
      write_speed = cap->GetCurrentWriteSpeed();
   }

   return err;
}

int Disc::SetWriteSpeed(uint16 wr)
{
   int err;
   
   if (write_speed == wr) 
      return ODE_OK;
   
   if (!(IsWritable() || IsErasable() || IsFormatable() || IsOverwritable()))
      return ODE_IllegalCommand;
   
   cmd_SetSpeed            ss(dio);
   cmd_Mode                md(dio);
   Page<Page_Capabilities> cap;

   err = ss.SetSpeed(read_speed, wr);
   if (err == 0)
   {
      cap = md.GetPage(cmd_Mode::Id_Page_Capabilities);
      read_speed = cap->GetCurrentReadSpeed();
      write_speed = cap->GetCurrentWriteSpeed();
   }

   return err;
}

uint16 Disc::GetWriteSpeed()
{
   return write_speed;
}

uint16 Disc::GetReadSpeed()
{
   return read_speed;
}

const char *Disc::DiscVendor()
{
   return Cfg->Vendors()->getVendor(0, 0, 0);
}

const IOptItem *Disc::GetNextWritableTrack(const IOptItem *di)
{
   bool fnd = false;
   _D(Lvl_Info, "Querying next writable track after %08lx", (int)di);
   
   if (0 == di)
      fnd = true;
 
   for (int i=0; i<optcontent->getChildCount(); i++)
   {
      const IOptItem *sess = optcontent->getChild(i);

      for (int j=0; j<sess->getChildCount(); j++)
      {
         const IOptItem *trak = sess->getChild(j);

         if ((!fnd) && (trak == di))
            fnd = true;
         else if ((fnd) && (trak->isBlank()))
            return trak;
         else if ((fnd) && (IsOverwritable()) && (trak->isIncremental()))
            return trak;
      }
   } 

   return 0;
}

int Disc::SetWriteMethod(DiscWriteMethod wm)
{
   _D(Lvl_Info, "Setting write method to %ld", wm);
   write_method = wm;
   return OnChangeWriteMethod();
};

DiscWriteMethod Disc::GetWriteMethod()
{
   return write_method;
};

int Disc::OnChangeWriteMethod()
{
   return ODE_IllegalCommand;
}

int Disc::LayoutTracks(const IOptItem *dit)
{
   const IOptItem *pTrack = 0;
   int32    lRes;

   pTrack = GetNextWritableTrack(0);      // we start with *first*, not *last* writable track
   
   if (pTrack == 0) 
   {
      _D(Lvl_Info, "Unable to find writable track.");
      return ODE_NotEnoughSpace;
   }                            
   if (dit == 0)
   {
      _D(Lvl_Warning, "No disc structure passed");
      return ODE_BadLayout;
   }
   if (dit->getChildCount() != 1)
   {
      _D(Lvl_Warning, "No sessions defined in disc structure");
      return ODE_BadLayout;
   }
   if (dit->getChild(0)->getChildCount() == 0)
   {
      _D(Lvl_Warning, "No tracks defined in disc/session structure");
      return ODE_BadLayout;
   }

   _D(Lvl_Info, "Provided structure");
   dit->printReport(drive->GetDebug());
   lRes = CheckItemData(dit);
   if (lRes != ODE_OK)
      return lRes;

   /* based on the settings resulting from disc specification we calculate the following */

   layCurrentBlock= pTrack->getStartAddress() + LayoutAdjustment();
   layFirstTrack  = pTrack->getItemNumber();
   layFirstSession= pTrack->getParent()->getItemNumber();
   laySessions    = 0;
   layTracks      = 0;

   /* now we can perform layout */
   
   for (int sn=0; sn<dit->getChildCount(); sn++)
   {
      const IOptItem *sess = dit->getChild(sn);

      _D(Lvl_Info, "Provided structure");
      sess->printReport(drive->GetDebug());
      lRes = CheckItemData(sess);
      if (lRes != ODE_OK)
         return lRes;

      for (int tn=0; tn<sess->getChildCount(); tn++)
      {
         const IOptItem *trak = sess->getChild(tn);

         _D(Lvl_Info, "Provided structure");
         trak->printReport(drive->GetDebug());
         lRes = CheckItemData(trak);
         if (lRes != ODE_OK)
            return lRes;
      }
   }

   if (dit->getEndAddress() > pTrack->getEndAddress())
      return ODE_NotEnoughSpace;
   
   return ODE_OK;
}

int Disc::CheckItemData(const IOptItem* dit)
{
   if (dit->isBlank()) 
   {
      _D(Lvl_Warning, "Blank tracks not allowed");
      return ODE_BadLayout;
   }

   const_cast<IOptItem*>(dit)->setPacketSize(16);

   switch (dit->getItemType())
   {
      case Item_Disc:
         break;

      case Item_Session:
         if ((laySessions != 0) && (!AllowMultiSessionLayout()))
         {
            _D(Lvl_Warning, "Only one session allowed");
            return ODE_BadLayout;
         }
         else if (laySessions > 0)
         {
            layCurrentBlock += SessionGapSize();
         }
         const_cast<IOptItem*>(dit)->setItemNumber(layFirstSession);
         ++laySessions;
         ++layFirstSession;
         layTracks = 0;
         break;

      case Item_Track:
         if ((layTracks != 0) && (!AllowMultiTrackLayout()))
         {
            _D(Lvl_Warning, "Only one track allowed");
            return ODE_BadLayout;
         }
         else if (layTracks > 0)
         {
            layCurrentBlock += TrackGapSize();
         }
         const_cast<IOptItem*>(dit)->setItemNumber(layFirstTrack);
         const_cast<IOptItem*>(dit)->setStartAddress(layCurrentBlock);
         layCurrentBlock = dit->getEndAddress() + 1;
         ++layTracks;
         ++layFirstTrack;
         break;

      case Item_Index:
         _D(Lvl_Warning, "Indices not allowed.");
         return ODE_BadLayout;
         break;
   }

   _D(Lvl_Info, "Resulting structure:");
   dit->printReport(drive->GetDebug());

   return ODE_OK;
}

const IOptItem *Disc::FindTrack(int lTrackNum)
{
   const IOptItem *sess;
   const IOptItem *trak;

   for (int i=0; i<optcontent->getChildCount(); i++)
   {
      sess = optcontent->getChild(i);
      ASSERT(sess->getChildCount() != 0);

      if ((sess->getChild(0)->getItemNumber() <= lTrackNum) && (sess->getChild(sess->getChildCount()-1)->getItemNumber() >= lTrackNum))
      {
         trak = sess->getChild(0);
         trak = sess->getChild(lTrackNum - trak->getItemNumber());
         ASSERT(trak != 0);
         ASSERT(trak->getItemNumber() == lTrackNum);
         return trak;
      }
   }
   return 0;   
}
 
const IOptItem *Disc::FindSession(int lSessNum)
{
   const IOptItem *sess;

   sess = optcontent->getChild(lSessNum - 1);
   if (sess != 0)
   {
      ASSERT(sess->getItemNumber() == lSessNum);
   }
      
   return sess;
}

int Disc::SequentialWrite(APTR pMem, ULONG lBlocks)
{
   int32 err;
   uint32 count;
   
   if (!pSeqCurrentTrack) return ODE_IllegalCommand;
   
   if (0 != pSeqCurrentTrack->isIncremental())
   {
      if (0 != (lBlocks % pSeqCurrentTrack->getPacketSize()))
         return ODE_BadTransferSize;
   }
   
   if (lSeqSector == pSeqCurrentTrack->getStartAddress())                     // so, the new track has just begun
   {
      BeginTrackWrite(pSeqCurrentTrack);                                      // tell that we're warming up the next track
   
      if (pSeqCurrentTrack->getPreGapSize())                                  // meanwhile check if we need to generate any data
         WritePadData(pSeqCurrentTrack, pSeqCurrentTrack->getPreGapSize());   // write pre gap data
   }
  
   count = pSeqCurrentTrack->getDataEndAddress() - lSeqSector + 1;            // how many sectors can we actually handle?
    
   if (count > lBlocks) count = lBlocks;                                      // write no more than we can (track / available)
   lBlocks -= count;                                                          // reduce amount of blocks to be written...

   err = WriteDisc(lSeqSector, count, pSeqCurrentTrack->getSectorSize(), pMem);   
   if (err)                                                                   // oups...
      return err;                                                             // caught error -> return error.

   pMem = &((char*)pMem)[count * pSeqCurrentTrack->getSectorSize()];          // proceed to next memory location
   lSeqSector += count;                                                       // update counters

   if (lSeqSector == (pSeqCurrentTrack->getDataEndAddress()+1))               // last data sector has been written
   {
      WritePadData(pSeqCurrentTrack, pSeqCurrentTrack->getPostGapSize());     // write post gap data
      EndTrackWrite(pSeqCurrentTrack);                                        // perform cleanup operations
      CloseDisc(DRT_Close_Track, pSeqCurrentTrack->getItemNumber());          // close track
      
      for (int i=0; i<pSeqStructure->getChild(0)->getChildCount(); i++)       // browse a single session!
      {
         const IOptItem *trk = pSeqStructure->getChild(0)->getChild(i);       // unless someone mangled the structure 
         if (trk == pSeqCurrentTrack)                                         // everything will go well
         {                                                                    //
            pSeqCurrentTrack = 0;                                             // if we found the previously finished track
            continue;                                                         // mark that we're waiting for next one
         }                                                                    //
         else if (pSeqCurrentTrack == 0)                                      // if we found that track in previous iteration
         {                                                                    // just use the one we found in this one
            pSeqCurrentTrack = trk;                                           // and stop looping
            break;                                                            //
         }                                                                    // otherwise, the pSeqCurrentTrack = 0
      }                                                                       // and operation stops;

      if (!pSeqCurrentTrack)                                                  // did we reach the end of structure?
      {                                                                       // seems so
         if (pSeqStructure->getFlags() & DIF_Disc_CloseDisc)                  // do we want to close disc?
            CloseDisc(DRT_Close_Finalize, 0);                                 //
         else if (pSeqStructure->getFlags() & DIF_Disc_CloseSession)          // or close session?
            CloseDisc(DRT_Close_Session, 0);                                  //
         return ODE_OK;                                                       // finally, it's all done :)
      }
      else                                                                    //
      {
         lSeqSector     = pSeqCurrentTrack->getStartAddress();
         lSeqLastSector = pSeqCurrentTrack->getEndAddress();
      }
   }
   
   if (lBlocks <= 0) return ODE_OK;                                           // all blocks have been written
   return SequentialWrite(pMem, lBlocks);
}

int Disc::WritePadData(const IOptItem *pDI, int32 len)
{
   uint8* pBuf = new uint8[(pDI->getSectorSize() * pDI->getPacketSize())];
   int err = ODE_OK;
   int lLen;
   
   _D(Lvl_Debug, "Writing track pre-gap");
  
   while (len)
   {
      lLen = len < pDI->getPacketSize() ? len : pDI->getPacketSize();
      err = WriteDisc(lSeqSector, lLen, pDI->getSectorSize(), pBuf);
      if (err)
         break;
      lSeqSector += lLen;
      len -= lLen;
   }

   delete [] pBuf; 
   return err;
}

bool Disc::RequiresUpdate()
{
   return bNeedUpdate;
}

void Disc::RequestUpdate()
{
   bNeedUpdate = true;
}

int16 Disc::GetOperationProgress()
{
   return 0;
}
   
void Disc::ReadSpeeds()
{
   uint16                  rd = 0xffff;
   uint16                  wt = (IsOverwritable() || IsErasable() || IsWritable() || IsFormatable()) ? 0xffff : 0;
   uint16                  lr = rd;
   uint16                  lw = wt; // don't touch!
   DiscSpeed               spd;
   cmd_SetSpeed            ss(dio);
   cmd_Mode                md(dio);
   Page<Page_Capabilities> caps;
  

   ss.SetSpeed(0xffff, 0xffff);
   readSpeeds.Empty();
   writeSpeeds.Empty();

   {
      caps = md.GetPage(cmd_Mode::Id_Page_Capabilities);
      if (caps->GetSpeedDescriptorsCount()> 0)
      {
         spd.kbps = caps->GetCurrentReadSpeed();
         FillDiscSpeed(spd);
         readSpeeds << spd;
         lw = 0xffff;

         for (int i=0; i<caps->GetSpeedDescriptorsCount(); i++)
         {
            spd.kbps = caps->GetSpeedDescriptor(i);
            if (spd.kbps == lw)
               continue;
            lw = spd.kbps;
            FillDiscSpeed(spd);
            writeSpeeds << spd;
         }

         spd.kbps = 0;
         spd.i    = 0;
         spd.f    = 0;   

         readSpeeds << spd;
         writeSpeeds << spd;

         return;
      }
   }


   while ((lr != 0) && (lw != 0))
   {
      caps = md.GetPage(cmd_Mode::Id_Page_Capabilities);
      rd = caps->GetCurrentReadSpeed();
      wt = caps->GetCurrentWriteSpeed();

      if ((lr != 0) && (lr != rd))
      {
         lr = rd;
         spd.kbps = rd;
         FillDiscSpeed(spd);

         _D(Lvl_Info, "Read speed: %ld.%ld (%ldkbps)", spd.i, spd.f, spd.kbps);
         readSpeeds << spd;
         rd -= 176;
      }
      else
         lr = 0;

      if ((lw != 0) && (lw != wt))
      {
         lw = wt;
         spd.kbps = wt;
         FillDiscSpeed(spd);
         _D(Lvl_Info, "Write speed: %ld.%ld (%ldkbps)", spd.i, spd.f, spd.kbps);
         writeSpeeds << spd;
         wt -= 176;
      }
      else
         wt = 0;
      
      ss.SetSpeed(rd, wt);
   }

   spd.kbps = 0;
   spd.i    = 0;
   spd.f    = 0;   
 
   SetReadSpeed(0xffff);
   SetWriteSpeed(0xffff);

   readSpeeds << spd;
   writeSpeeds << spd;

}

const DiscSpeed *Disc::GetReadSpeeds()
{
   return readSpeeds.GetArray();
}

const DiscSpeed *Disc::GetWriteSpeeds()
{
   return writeSpeeds.GetArray();
}
   
uint32 Disc::GetDiscSize()
{
   return optcontent->getEndAddress() + 1;
}

uint32 Disc::Calibrate()
{
   cmd_Calibrate cal(dio);

   return cal.Go();
}

uint32 Disc::WaitOpComplete()
{
   cmd_TestUnitReady tur(dio);
   _D(Lvl_Debug, "Waiting for operation to complete...");

   do
   {
      DOS->Delay(25);
      tur.Go();
      if (tur.SCSIError() == 0x20404)
         continue;
      if (tur.SCSIError() == 0x20407)
         continue;
      if (tur.SCSIError() == 0x20408)
         continue;
      if (tur.SCSIError() == 0x20000)
         continue;
      break;
   } while (true);

   _D(Lvl_Debug, "Operation completed.");

   return tur.OpticalError();
}

