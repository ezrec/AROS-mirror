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
#include "Disc_CD_ROM.h"
#include "Disc_CD_R.h"
#include "Disc_CD_RW.h"
#include "Disc_DVD_ROM.h"
#include "Disc_DVD_PlusR.h"
#include "Disc_DVD_PlusRW.h"
#include "Disc_DVD_MinusR.h"
#include "Disc_DVD_MinusRW.h"
#include "Disc_DVD_RAM.h"
#include "Config.h"
#include <Generic/Thread.h>
#include "Humming.h"
#include <libclass/utility.h>
 
DriveSpool *DriveSpool::pInstance = 0;
 
Drive  *Drive::OpenDrive(Drive *& pChain, char* pDevice, int lUnit)
{
   AllClear = Clear1 && Clear2;
 
   Drive *pDrive = new Drive(pDevice, lUnit);

   pDrive->next_drive = pChain;
   pChain = pDrive;
   return pDrive;
}

uint32 Drive::SendMessage(uint32* pMsg)
{
   return pThread->DoSync(pMsg[0], &pMsg[2]);
}

Drive::~Drive()
{
   delete pThread;
   _D(Lvl_Info, "Drive done.");
   _ED();
};

Drive::Drive(char *drv, int unit)
{
   _ND("Drive");

   next_drive  = 0;
   clients     = 0;
   current_disc.Assign(0);
   bLockInterOperations = false;
   inquiry     = 0;
   
   Exec->InitSemaphore(&rwaccess); 
   numLocks = 0;

   drive_name  = drv;
   unit_number = unit;

   hHkProcMsgs.Initialize(this, &Drive::ProcMsgs);
   hHkProcHandle.Initialize(this, &Drive::HandleMessages);

   bRegistered = Clear2;

   _D(Lvl_Info, "Spawning new drive process for %s.%ld, class %08lx", (uint32)drv, unit, (int)this);
   pThread = new Thread("Drive Subprocess", hHkProcMsgs.GetHook(), 0);
   pThread->SetHandler(hHkProcHandle.GetHook());

   /*
    * the line below waits for drive to init itself, but has so many odds, it's better
    * to keep it commented out.
    * the worst of the odds is that if drive is no longer present -- the program won't start.
    */
   //pThread->DoSync(DRV_NoOP, 0);
   _D(Lvl_Info, "Process initialized.");
};

Drive *Drive::FindDrive(char *drv, int unit)
{
   if ((!strcmp(drv, drive_name)) && (unit == unit_number)) return this;
   if (next_drive) return next_drive->FindDrive(drv, unit);
   return 0;
};

Drive *Drive::CleanUpDrives()
{
   register Drive *e = 0;

   if (next_drive) next_drive = next_drive->CleanUpDrives();
   if (clients) return this;
   _D(Lvl_Info, "Disposing unused drive...");
   e = next_drive;
   delete this;
   return e;
};

DriveClient *Drive::AddClient()
{
   _D(Lvl_Info, "Creating new client...");
   clients = new DriveClient(clients, this);
   return clients;
}

void Drive::RemClient(DriveClient* el)
{
   if (clients) clients = clients->UnLink(el);
};

bool Drive::ProcInit()
{
   driveio        =  0;
   mode_io        =  0; // if anything fails
   tur            =  0; // we don't dispose illegal pointers.
   features       =  0;
   inquiry        =  0;
   pc             =  0;
   pw             =  0;
   discinfo       =  0;
   hwconfig       =  0;
   selected_read_speed  = 0xffff;
   selected_write_speed = 0xffff;
   current_disc.Assign(0);

   driveio = new DriveIO(DEBUG_ENGINE); 

   _D(Lvl_Debug, "Opening drive %s.%ld@%08lx", (uint32)drive_name.Data(), unit_number, (uint32)driveio);
   if (!driveio->OpenDev(drive_name.Data(), unit_number))
      return false;
   _D(Lvl_Info, "Drive initialization went fine. Proceeding with thread data initialization");
   drive_status   = DRT_DriveStatus_DeviceBusy;
   mode_io = new cmd_Mode(driveio);
   tur     = new cmd_TestUnitReady(driveio);
   features= new cmd_GetConfiguration(driveio);
   inquiry = new cmd_Inquiry(driveio);
   return true;
}

unsigned long Drive::ProcMsgs(Thread *pThis, void*)
{
   int               period = 0;
   bool              bReported = false;

   _D(Lvl_Info, "Initializing subprocess for class %08lx...", (int)this);
   if (!ProcInit())
   {
      _D(Lvl_Error, "Drive process initialization failed.");
      drive_status = DRT_DriveStatus_NotOpened;
      _D(Lvl_Info, "Handling remaining commands...");
      pThis->HandleSignals(0xffffffff);
   }
   else
   {
      lCurrentDiscSeqNum  = 0;  
      while (!pThis->HandleSignals(period))
      {
         if (!bLockInterOperations)
         {
            tur->Go();
            switch (tur->SCSIError() & 0xffff00) 
            {

               case 0x000000:
                  {
                     _D(Lvl_Debug, "Drive ready.");
               
                     if (drive_status != DRT_DriveStatus_Ready) 
                     {
                           drive_status = DRT_DriveStatus_Ready;
                           AnalyseDrive();
                           AnalyseDisc();
                     }
                  
                     register Disc *d = current_disc.ObtainRead();
                     register bool update = false;

                     if (d)
                     {
                        period = d->GetRefreshPeriod() * 20;
                        update = d->RequiresUpdate();
                     }
                     else
                     {
                        if (!bReported)
                        {
                           request("Warning", "It appears that there are some hardware configuration problems\n"
                                                 "and your drive will not function properly. Some features such as\n"
                                                 "automation will not report proper data.\n\n"
                                                 "Please note that this program may function in an unpredicted way\n"
                                                 "or not at all. I will take no responsibility of any damage caused by\n"
                                                 "further usage of this program on your configuration.", "OK", 0);
                           bReported = true;
                        }
                        _D(Lvl_Warning, "This is weird. There seems to be no disc but the drive still reports ready... Will try the other time");
                        period = 5000;
                     }
                  
                     current_disc.Release();

                     if (update)
                     {
                        AnalyseDrive();
                        AnalyseDisc();
                     }
                  }
                  break;

               case 0x020400:
                  {
                     drive_status = DRT_DriveStatus_DeviceBusy;
                     _D(Lvl_Debug, "Long operation in progress...");
                     period = 3000;
                  }
                  break;

               case 0x023a00:
               case 0x020000: /* fallback to default here. */
                  {
                     if (drive_status != DRT_DriveStatus_NoDisc) 
                     {
                        drive_status = DRT_DriveStatus_NoDisc;
                        current_disc.Assign(0);
                        AnalyseDrive();
                        AnalyseDisc();
                     }
                     _D(Lvl_Debug, "No disc inserted.");
                     period = 3000;
                  }
                  break;

               case 0x062800:
                  {
                     _D(Lvl_Debug, "Disc has changed!");
                     period = 200;
                  }
                  break;
            }
         }
      } 
   }

   ProcExit();
   _D(Lvl_Info, "All done, thank you.");
   return 0;
};

void Drive::ProcExit()
{
   _D(Lvl_Debug, "Cleaning up drive process.");
   if (features)  delete features;
   if (mode_io)   delete mode_io;
   if (inquiry)   delete inquiry;
   if (tur)       delete tur;
   if (driveio)   delete driveio;
}

void Drive::AnalyseDisc(void)
{
   Disc *d = 0;

   d = current_disc.Assign(0);
   if (d) delete d;

   _D(Lvl_Debug, "Analysing disc...");
   
   drive_status = DRT_DriveStatus_Analysing;

   switch (current_drive_profile) 
   {
      case DRT_Profile_NoDisc:                     d = 0; break;
      case DRT_Profile_CD_ROM:                     d = new Disc_CD_ROM      (this);   break;
      case DRT_Profile_CD_R:                       d = new Disc_CD_R        (this);   break;
      case DRT_Profile_CD_RW:                      d = new Disc_CD_RW       (this);   break;
      case DRT_Profile_DVD_ROM:                    d = new Disc_DVD_ROM     (this);   break;
      case DRT_Profile_DVD_MinusR:                 d = new Disc_DVD_MinusR  (this);   break;
      case DRT_Profile_DVD_MinusRW_Sequential:     d = new Disc_DVD_MinusRW (this);   break;
      case DRT_Profile_DVD_MinusRW_Restricted:     d = new Disc_DVD_MinusRW (this);   break;
      case DRT_Profile_DVD_PlusR:                  d = new Disc_DVD_PlusR   (this);   break;
      case DRT_Profile_DVD_PlusRW:                 d = new Disc_DVD_PlusRW  (this);   break;
      case DRT_Profile_DVD_RAM:                    d = new Disc_DVD_RAM     (this);   break;
      default:                                     d = 0;
   }   
   
   // HERE THE INIT IS DONE SO WE CAN ASSIGN OUR STUFF
   if (d)
   {
      d->Init();
      drive_status = DRT_DriveStatus_Ready;
      _D(Lvl_Info, "Drive type: %ld", d->DiscType());
      d->SetReadSpeed(selected_read_speed);
      d->SetWriteSpeed(selected_write_speed);
   }
   else
   {
      _D(Lvl_Info, "No disc inserted.");
      drive_status = DRT_DriveStatus_NoDisc;
   }

   current_disc.Assign(d);
   ++lCurrentDiscSeqNum;
}

void Drive::AnalyseDrive(void)
{
   inquiry->Go();
   if (inquiry->OpticalError()) 
   {
      _D(Lvl_Error, "%s: DriveIO reported an error, will not continue.", (int)__PRETTY_FUNCTION__);
      return;
   } 
   else
   {
      hwconfig = Cfg->GetHardware(inquiry->ProductID());
      selected_read_speed = hwconfig->getReadSpeed();
      selected_write_speed = hwconfig->getWriteSpeed();

      _D(Lvl_Info, "Drive information:");
      _D(Lvl_Info, "Drive type: %ld", inquiry->DriveType());
      _D(Lvl_Info, "Standard  : %ld", inquiry->SupportedStandard());
      _D(Lvl_Info, "Product   : %s",  (int)inquiry->ProductID());
      _D(Lvl_Info, "Vendor    : %s",  (int)inquiry->VendorID());
      _D(Lvl_Info, "Version   : %s",  (int)inquiry->ProductVersion());
   }

   pc = mode_io->GetPage(cmd_Mode::Id_Page_Capabilities);
   _D(Lvl_Info, "Capabilities: id: %02lx, len: %02lx, hdr: %08lx, page: %08lx", pc.IsValid() ? pc->PageID() : 0xff, pc.IsValid() ? pc->PageSize() : 0, pc.IsValid() ? (int)((Page_Header*)pc) : 0, pc.IsValid() ? (int)((Page_Capabilities*)pc) : 0);
   if (pc.IsValid())
   {
      eMechanismType  = pc->GetMechanismType();
      eCapabilities  << pc->GetCapabilities();
   }
   else
   {
      eMechanismType = DRT_Mechanism_Unknown;
      eCapabilities  = 0;
   }

   pw = mode_io->GetPage(cmd_Mode::Id_Page_Write);
   _D(Lvl_Info, "Write       : id: %02lx, len: %02lx, hdr: %08lx page: %08lx", pw.IsValid() ? pw->PageID() : 0xff, pw.IsValid() ? pw->PageSize() : 0, pw.IsValid() ? (int)((Page_Header*)pw) : 0, pw.IsValid() ? (int)((Page_Write*)pw) : 0);

   media_write_capabilities   =  0;
   media_read_capabilities    =  0;
   current_drive_profile      = DRT_Profile_NoDisc;

   if (drive_status != DRT_DriveStatus_NoDisc) // we used if (pw) && drive_status....
   {
      if (!discinfo)                               // jesli trzeba utworzyc...
         discinfo = new cmd_ReadDiscInfo(driveio);

      discinfo->Go();

      if (discinfo->OpticalError()) 
      {
         delete discinfo;
         discinfo = 0;
      }

   } 
   else 
   {
      if (discinfo) delete discinfo;
      discinfo = 0;
   }

   features->Go();
   
   if (!features->OpticalError()) 
   {
      media_write_capabilities   =  features->GetMediaWriteSupport();
      media_read_capabilities    =  features->GetMediaReadSupport();
      if (discinfo != 0)
         current_drive_profile   =  features->GetCurrentProfile();
      _D(Lvl_Info, "Drive reported profiles porperly. Will now analyse.");
      _D(Lvl_Info, "Current drive profile is %04lx", features->GetCurrentProfile());
      _D(Lvl_Info, "Drive can access media:  %08lx", features->GetMediaReadSupport());
      _D(Lvl_Info, "Drive can write media:   %08lx", features->GetMediaWriteSupport());
   }
   else 
   {
      _D(Lvl_Info, "Drive does not report any profiles. Will try the other way");
      if (pc.IsValid()) 
      {
         media_write_capabilities   =  pc->GetMediaWriteSupport();
         media_read_capabilities    =  pc->GetMediaReadSupport();
         _D(Lvl_Info, "Will try to learn from capabilities...");
         _D(Lvl_Info, "Drive can access media: %08lx", pc->GetMediaReadSupport());
         _D(Lvl_Info, "Drive can write media:  %08lx", pc->GetMediaWriteSupport());
      } 
      else 
      {
         return;
      }


      if (discinfo) 
      {
         current_drive_profile = DRT_Profile_CD_ROM;
         if (discinfo->GetData()->IsWritable()) current_drive_profile = DRT_Profile_CD_R;
         if (discinfo->GetData()->IsErasable()) current_drive_profile = DRT_Profile_CD_RW;
      }
   }   

   _D(Lvl_Info, "Current drive profile: %ld", current_drive_profile);

   // now the further part, speeds a.s.o.
   if (pc.IsValid()) 
   {
      drive_buffer_size          =  pc->GetBufferSize();
   }

   Cfg->onWrite();
}

uint Drive::HandleMessages(uint ACmd, uint *msg)
{
   if (drive_status == DRT_DriveStatus_NotOpened)
      return (unsigned)ODE_NoHandler;

   register uint32 res = 0;
   register Disc *d = current_disc.ObtainRead();
      
   _D(Lvl_Info, "Received message: %ld", ACmd);
   
   switch (ACmd) 
   {
      case DRV_NoOP:
         res = ODE_OK;
         break;

      case DRV_Blank:
         {
            _D(Lvl_Debug, "Blank");
            if (d) 
               res = d->EraseDisc(msg[0]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_Format:
         {
            _D(Lvl_Debug, "Format");
            if (d) 
               res = d->FormatDisc(msg[0]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_StructureDisc:
         {
            _D(Lvl_Debug, "StructureDisc");
            if (d) 
               res = d->StructureDisc();
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_ControlUnit:
         {
            _D(Lvl_Debug, "ControlUnit");
            res = (StartStopUnit(msg[0]));
         }
         break;

      case DRV_LayoutTracks:
         {
            _D(Lvl_Debug, "LayoutTracks");
            if (d) 
               res = d->LayoutTracks(reinterpret_cast<IOptItem*>(msg[0]));
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_UploadLayout:
         {
            _D(Lvl_Debug, "UploadLayout");
            if (d) 
               res = d->UploadLayout(reinterpret_cast<IOptItem*>(msg[0]));
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_ReadTrackAbsolute:
         {
            _D(Lvl_Debug, "ReadTrackAbsolute");
            if (d) 
               res = d->RandomRead(reinterpret_cast<IOptItem*>(msg[0]),
                                                 (uint32)msg[1],
                                                 (uint32)msg[2],
                                                 (APTR)msg[3]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_ReadTrackRelative:
         {
            _D(Lvl_Debug,"ReadTrackRelative");
            if (d) 
               res = d->RandomRead(reinterpret_cast<IOptItem*>(msg[0]),
                                                 (uint32)msg[1]+(reinterpret_cast<IOptItem*>(msg[0]))->getStartAddress(),
                                                 (uint32)msg[2],
                                                 (APTR)msg[3]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_WriteTrackAbsolute:
         {
            _D(Lvl_Debug, "WriteTrackAbsolute");
            if (d) 
               res = d->RandomWrite(reinterpret_cast<IOptItem*>(msg[0]), (uint32)msg[1], (uint32)msg[2], (APTR)msg[3]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_WriteTrackRelative:
         {
            _D(Lvl_Debug, "WriteTrackRelative");
            if (d) 
               res = (d->RandomWrite(reinterpret_cast<IOptItem*>(msg[0]), (uint32)msg[1]+(reinterpret_cast<IOptItem*>(msg[0]))->getStartAddress(), (uint32)msg[2], (APTR)msg[3]));
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_CloseDisc:
         {
            _D(Lvl_Debug, "CloseMedium");
            if (d) 
               res = d->CloseDisc((int)msg[0], (int)msg[1]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_SetAttr:
         {
            _D(Lvl_Debug, "SetDriveAttr");
            res = SetDriveAttrs((uint32)msg[0], (uint32)msg[1]);
         }
         break;

      case DRV_SetAttrs:
         {
            _D(Lvl_Debug, "SetDriveAttrs");
            res = SetDriveAttrs((TagItem*)&msg[0]);
         }
         break;

      case DRV_WriteSequential:
         {
            _D(Lvl_Debug, "Write sequential");
            if (d) 
               res = d->SequentialWrite((APTR)msg[0], (uint32)msg[1]);
            else              
               res = (unsigned)(ODE_NoDisc);
         }
         break;

      case DRV_LockDrive:
         {
            res = LockDrive(msg[0]);
         }
         break;

      case DRV_ForceUpdate:
         {
            _D(Lvl_Debug, "Forcing Update");
            drive_status = DRT_DriveStatus_Unknown;
            res = 0;
         }
         break;

      default:
         ASSERTS(false, "Unsupported command");
         res = (unsigned)(ODE_IllegalCommand);
   }

   current_disc.Release();
   return res;
}

long Drive::LockDrive(long lLockType)
{
   cmd_LockDrive  *pLock = new cmd_LockDrive(driveio);
   int res = ODE_OK;
   
   if (lLockType == DRT_LockDrive_Unlock)
   {
      if (!numLocks) 
      {
         res = ODE_IllegalCommand;    // free before lock.
      }
      else
      {
         if (numLocks > 0)
            --numLocks;
         else
            ++numLocks;
            
         Exec->ReleaseSemaphore(&rwaccess);
         if (!numLocks)
         {
            pLock->setLocked(false);
            pLock->Go();
         }
         bLockInterOperations = false;
      }
   }
   else if (lLockType == DRT_LockDrive_Read)
   {
      if (Exec->AttemptSemaphoreShared(&rwaccess))
      {
         ++numLocks;
         if (numLocks == 1)
         {
            pLock->setLocked(true);
            pLock->Go();
         }
      }
      else
      {
         res = ODE_DriveLockedForWrite; 
      }
   }   
   else if (lLockType == DRT_LockDrive_Write)
   {
      if (Exec->AttemptSemaphore(&rwaccess))
      {
         numLocks = -1;
         bLockInterOperations = true;
         pLock->setLocked(true);
         pLock->Go();
      }
      else
      {
         if (numLocks == -1)
            res = ODE_DriveLockedForWrite; 
         else
            res = ODE_DriveLockedForRead;
      }
   }   
   delete pLock;
   return res;
}


uint Drive::GetDriveAttrs(uint tag, uint attr)
{
   if (drive_status == DRT_DriveStatus_NotOpened)
      return 0;
  
   register Disc* d = current_disc.ObtainRead(); 
   register uint res = 0;
      
   switch (tag)
   {
      case DRA_AddTagNotify:                 
         break;

      case DRA_RemTagNotify:
          break;

      case DRA_DeviceName:                   
          res = (uint)drive_name.Data();
          break;

      case DRA_UnitNumber:                   
          res = unit_number;
          break;

      case DRA_IsOpened:
          res = driveio->IsOpened();
          break;

      case DRA_Attention:
          res = 0;                     // OBSOLETE!!
          break;

      case DRA_EnableDiag:
          res = 1;                     // OBSOLETE!!
          break;


      case DRA_Drive_ReadSpeeds:             
          res = d ? (uint)d->GetReadSpeeds() : 0;
          break;

      case DRA_Drive_WriteSpeeds:
          res = d ? (uint)d->GetWriteSpeeds() : 0;
          break;

      case DRA_Drive_CurrentOperation:       
          res = 0;                     // OBSOLETE!?
          break;

      case DRA_Drive_OperationProgress:      
          res = d ? d->GetOperationProgress() : 0;
          break;

      case DRA_Drive_SenseData:
          res = (uint)driveio->Sense();
          break;

      case DRA_Drive_Status:
          res = CurrentStatus();
          break;

      case DRA_Drive_AbsoluteCDAddress:      
          res = 0;                     // !!
          break;

      case DRA_Drive_RelativeCDAddress:      
          res = 0;                     // !!
          break;

      case DRA_Drive_TestMode:               
          res = 0;                     // !!
          break;

      case DRA_Drive_Vendor:                 
          res = (uint)((inquiry) ? inquiry->VendorID() : "");
          break;

      case DRA_Drive_Product:                
          res = (uint)((inquiry) ? inquiry->ProductID() : "");
          break;

      case DRA_Drive_Version:                
          res = (uint)((inquiry) ? inquiry->ProductVersion() : "");
          break;

      case DRA_Drive_Firmware:               
          res = (uint)((inquiry) ? inquiry->FirmwareVersion() : "");
          break;

      case DRA_Drive_ReadsMedia:             
          res = media_read_capabilities;
          break;

      case DRA_Drive_WritesMedia:            
          res = media_write_capabilities;
          break;

      case DRA_Drive_MechanismType:          
          res = eMechanismType;
          break;

      case DRA_Drive_Capabilities:           
          res = eCapabilities;
          break;

      case DRA_Drive_BufferSize:             
          res = drive_buffer_size;
          break;

      case DRA_Drive_BufferUsed:             
          res = 0;                     // !!
          break;

      case DRA_Drive_VolumeLevels:           
          res = 0;                     // !!
          break;

      case DRA_Drive_CurrentAudioTrackIndex: 
          res = 0;                     // !!
          break;

      case DRA_Drive_CurrentProfile:         
          res = d ? d->DiscType() : DRT_Profile_Unknown;
          break;

      case DRA_Drive_CurrentReadSpeed:       
          res = d ? d->GetReadSpeed() : selected_read_speed;
          break;

      case DRA_Drive_CurrentWriteSpeed:      
          res = d ? d->GetWriteSpeed() : selected_write_speed;
          break;

      case DRA_Drive_IsRegistered:           
          res = bRegistered;
          break;

      case DRA_Drive_WriteParams:            
          res = 0;                     // OBSOLETE!!!!!!
          break;

      case DRA_Drive_IsDiscInserted:         
          res = drive_status == DRT_DriveStatus_Ready ? 1 : 0;
          break;


      case DRA_Disc_NumTracks:               
          res = d ? (uint32)d->GetNumTracks() : 0;
          break;

      case DRA_Disc_Contents:                
          res = d ? (uint)d->GetContents() : 0;
          break;

      case DRA_Disc_SubType:                 
          res = d ? d->DiscSubType() : 0;
          break;

      case DRA_Disc_ID:                      
          res = 0;                     // !!
          break;

      case DRA_Disc_Size:                    
          res = d ? d->GetDiscSize() : 0;
          break;

      case DRA_Disc_State:                   
          res = 0;                     // !!
          break;

      case DRA_Disc_LastSess_State:          
          res = 0;                     // !!
          break;

      case DRA_Disc_Format:                  
          res = 0;                     // !! -rom / -xa / -i
          break;

      case DRA_Disc_BarCode:                 
          res = 0;                     // !!
          break;

      case DRA_Disc_CatalogNumber:           
          res = 0;                     // !!
          break;

      case DRA_Disc_IsWritable:              
          res = d ? d->IsWritable() : 0;
          break;

      case DRA_Disc_IsErasable:              
          res = d ? d->IsErasable() : 0;
          break;

      case DRA_Disc_IsFormatable:            
          res = d ? d->IsFormatable() : 0;
          break;

      case DRA_Disc_IsOverwritable:          
          res = d ? d->IsOverwritable() : 0;
          break;

      case DRA_Disc_NextWritableTrack:       
         res = d ? (uint)d->GetNextWritableTrack((IOptItem*)attr) : 0;
         break;

      case DRA_Disc_WriteMethod:             
         res = d ? d->GetWriteMethod() : 0;
         break;

      case DRA_Disc_CurrentDiscSeqNum:       
         res = lCurrentDiscSeqNum;
         break;

      case DRA_Disc_Vendor:                  
         res = d ? (uint)d->DiscVendor() : 0;
         break;

      default:
         ASSERTS(false, "Unknown tag requested!");
   }
   
   current_disc.Release();

   return res;
}

uint Drive::SetDriveAttrs(uint tag, uint value)
{
   if (drive_status == DRT_DriveStatus_NotOpened)
      return (unsigned)ODE_NoHandler;
   
   register uint res = 0;
   register Disc *d = current_disc.ObtainRead();

   switch (tag)
   {
      case DRA_Drive_TestMode:
         {
            Page<Page_Write> &pw = GetWritePage();
            if (!pw.IsValid()) 
            {
               res = (uint32)ODE_IllegalCommand;
            }
            else
            {
               pw->SetTestMode(value);
               res = SetPage(pw);
            }
         }
         break;

      case DRA_Drive_CurrentReadSpeed:
         {
            selected_read_speed = value;
            hwconfig->setReadSpeed(value);
            if (d) 
               res = d->SetReadSpeed(value);
            else
               res = (uint32)ODE_NoDisc;
         }
         break;

      case DRA_Drive_CurrentWriteSpeed:
         {
            selected_write_speed = value;
            hwconfig->setWriteSpeed(value);
            if (d) 
               res = d->SetWriteSpeed(value);
            else
               res = (uint32)ODE_NoDisc;
         }
         break;

      case DRA_Disc_WriteMethod:
         {
            if (d) 
               res = d->SetWriteMethod((DiscWriteMethod)value);
            else
               res = (uint32)ODE_NoDisc;
         }
         break;
   }

   current_disc.Release();
   return res;
}

uint32 Drive::SetDriveAttrs(TagItem *ti)
{
   if (drive_status == DRT_DriveStatus_NotOpened)
      return (unsigned)ODE_NoHandler;
   
   int err = 0;
   TagItem  *t;

   while ((t = Utility->NextTagItem(&ti))!=0)
   {
      if (!err)
    err = SetDriveAttrs(t->ti_Tag, t->ti_Data);
      else
    SetDriveAttrs(t->ti_Tag, t->ti_Data);
   };
   return err;
}

int Drive::SetPage(Page_Header *p)
{
   return mode_io->SetPage(p);
};

Page<Page_Write> &Drive::GetWritePage(void)
{
   return pw;
};

Page<Page_Capabilities> &Drive::GetCapabilitiesPage(void)
{
   return pc;
};

int Drive::StartStopUnit(int type)
{
   cmd_StartStopUnit::StartStopType  t;
   int err;
   switch (type) {
      case DRT_Unit_Stop:
    t = cmd_StartStopUnit::StartStop_Stop;
    break;
      case DRT_Unit_Start:
    t = cmd_StartStopUnit::StartStop_Start;
    break;
      case DRT_Unit_Eject:
    t = cmd_StartStopUnit::StartStop_Eject;
    break;
      case DRT_Unit_Load:
    t = cmd_StartStopUnit::StartStop_Load;
    break;
      case DRT_Unit_Idle:
    t = cmd_StartStopUnit::StartStop_Idle;
    break;
      case DRT_Unit_Standby:
    t = cmd_StartStopUnit::StartStop_Standby;
    break;
      case DRT_Unit_Sleep:
    t = cmd_StartStopUnit::StartStop_Sleep;
    break;
      default:
    return ODE_IllegalParameter;
   }

   cmd_StartStopUnit *ssu = new cmd_StartStopUnit(driveio);
   ssu->setType(t);
   err = ssu->Go();
   delete ssu;
   return err;
}

cmd_GetConfiguration::Feature *Drive::GetDriveFeature(cmd_GetConfiguration::FeatureId id)
{
   if (!features) return 0;

   return features->GetFeature(id);
};

uint32 Drive::ScanDevice(char* sDeviceName)
{
   DEFINE_DEBUG;
   cmd_Inquiry*pInq = 0;
   ScanData   *pData= 0;
   ScanData   *pRes = 0;

   _NDS("Device Scan");
   DriveIO    *pIO  = new DriveIO(DEBUG_ENGINE);   // has to be here, called once debug is initialized.

   _D(Lvl_Info, "Checking if device %s is harmless...", (int)sDeviceName);
   switch (Cfg->Drivers()->isHarmful(sDeviceName))
   {
      case stYes:
         {
            _D(Lvl_Warning, "Device is harmful.");
            request("Error", "Device %s is considered harmful\nOperation aborted.", "Ok", ARRAY((uint)sDeviceName));
         }
         break;

      case stUnknown:
         {
            _D(Lvl_Warning, "Device is not recorded yet.");
            if (0 == request("Warning", "Device %s is not known and may be harmful.\nDo you want to continue?", "Yes|No", ARRAY((uint)sDeviceName)))
            {
               Cfg->Drivers()->addDevice(sDeviceName, true);
               break;
            }
         }
         // no break here.
         
      case stNo:
         {
            _D(Lvl_Info, "Scanning device %s...", (uint)sDeviceName);
            for (int i=0; i<8; i++)
            {
               _D(Lvl_Info, "Accessing device %s, unit %ld...", (uint)sDeviceName, i);
               if (pIO->OpenDev(sDeviceName, i))
               {
                  pInq = new cmd_Inquiry(pIO);
                  if (pInq->Go() == ODE_OK)
                  {
                     if (!pData)
                     {
                        pData = new ScanData;
                        pRes = pData;
                     }
                     else
                     {
                        pData->sd_Next = new ScanData;
                        pData = pData->sd_Next;
                     }           
                     pData->sd_Next       = 0;
                     pData->sd_DeviceName = new char[32];
                     pData->sd_Vendor     = new char[16];
                     pData->sd_Unit       = i;
                     pData->sd_Type       = pInq->DriveType();
                     strcpy(pData->sd_DeviceName,  pInq->ProductID());
                     strcpy(pData->sd_Vendor,      pInq->VendorID());
                  }
                  if (pInq)   
                     delete pInq;   
                  pInq = 0;
               }
   
            }    

            Cfg->Drivers()->addDevice(sDeviceName, false);
         }
         break;

   }

   delete pIO;
   _ED();
   return (unsigned long)pRes;
}

uint32 Drive::FreeScanResults(ScanData*pData)
{
   while (pData)
   {
      ScanData *pNext = pData->sd_Next;
      delete [] pData->sd_Vendor;
      delete [] pData->sd_DeviceName;
      delete [] pData;
      pData = pNext;
   }
   return 0;
}




DriveSense::DriveSense(void)
{
   sense = new _sense;
};

DriveSense::~DriveSense(void)
{
   delete sense;
};

UBYTE* DriveSense::SenseData(void)
{
   return (UBYTE*)sense;
};

int DriveSense::SenseLength(void)
{
   return sizeof(_sense);
};

void DriveSense::Prepare(void)
{
   sense->primary.setCode(0);
};

int DriveSense::Key(void)
{
   if (sense->primary.getCode())
      return sense->primary.getKey();
   else
      return 0;
};

int DriveSense::Code(void)
{
   if (sense->primary.getCode())
      return sense->primary.getCode();
   else
      return 0;
};

int DriveSense::ASC(void)
{
   if (sense->primary.getCode())
      return sense->secondary.getASC();
   else
      return 0;
};

int DriveSense::ASCQ(void)
{
   if (sense->primary.getCode())
      return sense->secondary.getASCQ();
   else
      return 0;
};

uint32 DriveSense::SCSIError(void)
{
   if (sense->primary.getCode())
      return (Key()<<16) | (ASC()<<8) | (ASCQ());
   else
      return 0;
};

 


DriveIO::DriveIO(DbgHandler *dh)
{
   SET_DEBUG_ENGINE(dh);
   opened      = false;
   ioport      = 0;
   ioreq       = 0;
   sense       = 0;

   sense = new DriveSense();
   ioport = ::Exec->CreateMsgPort();
   ioreq  = (IOStdReq*)::Exec->CreateIORequest(ioport, sizeof(*ioreq));
};

bool DriveIO::OpenDev(const char* AName, int AUnit)
{
   if (opened)
      ::Exec->CloseDevice((IORequest*)ioreq);
   opened = false;
   if (::Exec->OpenDevice((STRPTR)AName, AUnit, (IORequest*)ioreq, 0))
      return false;
   opened = true;

   {
      cmd_Reset r(this);
      r.Go();
   }

   return true;
}

DriveIO::~DriveIO(void)
{
   if (sense)        delete sense;
   if (opened)       ::Exec->CloseDevice((IORequest*)ioreq);
   if (ioreq)        ::Exec->DeleteIORequest((IORequest*)ioreq);
   if (ioport)       ::Exec->DeleteMsgPort(ioport);
};

int DriveIO::Exec(SCSICommand *scmd)
{
   int err;
   unsigned int att=0;

   if (!opened) return ODE_NoDevice;

   ioreq->io_Error      = 0;
   ioreq->io_Actual     = 0;
   ioreq->io_Offset     = 0;
   ioreq->io_Command    = HD_SCSICMD;
   ioreq->io_Data       = (SCSICmd*)scmd;
   ioreq->io_Length     = sizeof(SCSICmd);
   ioreq->io_Flags      = IOF_QUICK;

   if (0 != (scmd->DataLength() & 1))
   {
      _D(Lvl_Failure, "ERROR: ODD TRANSFER LENGTH IS NOT ALLOWED! PLEASE REPORT!");
      _DDT("COMMAND CAUSING THE FAULT:", scmd->Cmd(), scmd->CmdLength());
      _D(Lvl_Failure, "REQUEST ABORTED.");
      return 0x7f7f7f;
   }

   do {
      err = ODE_OK;
      if (scmd->NeedProbe()) 
      {
         if (DEBUG_ENGINE) {
            _DDT(scmd->CmdName(), scmd->Cmd(), scmd->CmdLength());
            att = 0;
         }
         sense->Prepare();
         err = ::Exec->DoIO((IORequest*)ioreq);
         _D(Lvl_Info, "Probe result: %08lx", sense->SCSIError());
         err = scmd->onProbe(err, sense->SCSIError());
      }
      if (!err) 
      {
         if (DEBUG_ENGINE) {
            _DDT(scmd->CmdName(), scmd->Cmd(), scmd->CmdLength());
            att = 0;
         }
         sense->Prepare();
         err = ::Exec->DoIO((IORequest*)ioreq);
      }

      if (!err) break;

      att++;
      if (att>= scmd->MaxAttempts()) break;
      _D(Lvl_Debug, "%s: %08lx", (uint)scmd->CmdName(), sense->SCSIError());
      if ((err = scmd->onLoop())!=ODE_OK) break;
   } while (true);

   if (DEBUG_ENGINE) {
      if (scmd->DebugDumpData()) {
         _DD(scmd->Data(), scmd->DataLength());
      }
   }

   if (!err) 
   {
      _D(Lvl_Info, "... OK");
      err = ODE_OK;
   } 
   else if (err>0) 
   {
      _D(Lvl_Info, "... Failed: %ld [%08lx]", err, sense->SCSIError());
      err = ODE_CommandError;
   } 
   else 
   {
      _D(Lvl_Info, "... Command finished signalling error: %ld", err);
      err = err;
      // already set up by onLoop()
   }
   return err;
}

DriveSense *DriveIO::Sense(void)
{
   return sense;
};




DriveSpool *DriveSpool::GetInstance()
{
   if (!DriveSpool::pInstance)
      DriveSpool::pInstance = new DriveSpool();
   return DriveSpool::pInstance;
}

DriveSpool::DriveSpool(void)
{
   _ND("Spool");
   _D(Lvl_Info, "Created new drive spool: %08lx", (uint32)this);
   drive_list = 0;
};

DriveSpool::~DriveSpool(void)
{
   _D(Lvl_Info, "Cleaning up! Never call me directly :>");
   _ED();
};

void DriveSpool::ExitInstance(void)
{
   if (!DriveSpool::pInstance)
      return;
   do
   {
      if (DriveSpool::pInstance->drive_list) DriveSpool::pInstance->drive_list = DriveSpool::pInstance->drive_list->CleanUpDrives();
      if (!DriveSpool::pInstance->drive_list) break;
      DOS->Delay(10);
   } while (true);
   delete DriveSpool::pInstance;
};

DriveClient *DriveSpool::NewClient(char* drv, int unit)
{
   Drive       *el=0;

   _D(Lvl_Info, "Adding a client for drive at: %s.%ld", (uint)drv, unit);

   if (drive_list) el = drive_list->FindDrive(drv, unit);
   if (!el)        el = Drive::OpenDrive(drive_list, drv, unit);
   if (el)
   {
      return el->AddClient();
   }
   return 0;
}

DriveClient *DriveSpool::CloneClient(DriveClient* client)
{
   Drive *d = client->GetDrive();
   return d->AddClient();
}

void DriveSpool::DelClient(DriveClient *dc)
{
   if (dc)
   {
      _D(Lvl_Info, "Attempting to remove client: %08lx", (uint32)dc);
      _D(Lvl_Info, "Owner: %08lx", (uint32)dc->GetDrive());
      dc->GetDrive()->RemClient(dc);
      _D(Lvl_Info, "Removed.");
   }
   if (drive_list) drive_list = drive_list->CleanUpDrives();
   _D(Lvl_Info, "Cleaned up.");
}

DriveClient::DriveClient(DriveClient *next, Drive* drv)
{
   next_client = next;
   drive       = drv;
   SET_DEBUG_ENGINE(drv->GetDebug());
   _D(Lvl_Info, "New client ready at %08lx", (uint32)this);
};

DriveClient *DriveClient::UnLink(DriveClient *el)
{
   if (el == this) 
   {
      el = next_client;
      _D(Lvl_Info, "Client done. Thank You.");
      delete this;
      return el;
   } else {
      if (next_client) next_client = next_client->UnLink(el);
      return this;
   }
}

Drive *DriveClient::GetDrive(void)
{
   return drive;
}

uint32 DriveClient::GetDriveStatus(void)
{
   return drive->CurrentStatus();
};

DriveClient::~DriveClient(void)
{
};

uint32 DriveClient::Send(uint32 *msg)
{
   return drive->SendMessage(msg);
}

uint DriveClient::GetDriveAttrs(TagItem *ti)
{
   TagItem  *t;
   
   while ((t = Utility->NextTagItem(&ti))!=0)
   {
      int* dest = (int*)((long)t->ti_Data);
      ASSERT(dest);
      ASSERT(drive);
      *dest = drive->GetDriveAttrs(t->ti_Tag, *dest);
   };
   return 0x0;
}

uint DriveClient::GetDriveAttrs(uint tag)
{
   long lRet = drive->GetDriveAttrs(tag, 0UL);
   return lRet;
}

uint32 DriveClient::WaitForDisc(int lDuration)
{
   // probe periodically every 1/2 sec
   lDuration *= 50;
   do
   {
      int lWait = lDuration > 25 ? 25 : lDuration >= 0 ? lDuration : 25;
      if (GetDriveAttrs(DRA_Drive_IsDiscInserted)) return ODE_OK;
      if (lWait) DOS->Delay(lWait);
      lDuration -= lWait;
   } while (lDuration);
   return (uint32)ODE_NoDisc;
}
