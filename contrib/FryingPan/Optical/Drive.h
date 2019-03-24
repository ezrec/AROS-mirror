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

#ifndef __DRIVE_H
#define __DRIVE_H

#include "Commands.h"
#include "Various.h"
#include "CfgHardware.h"
#include <Generic/RWSyncT.h>
#include <Generic/Thread.h>
#include <Generic/HookT.h>
#include <Generic/Set.h>
#include <Generic/Debug.h>

using namespace GenNS;

extern   class DriveSpool    *Spool;

namespace GenNS {
class Thread;
};


struct DriveMsg
{
   uint32    cmd;
   uint32    client;
   uint32    params[0];
};

class Drive 
{
   DEFINE_DEBUG;

   String                     drive_name;
   int                        unit_number;
   class Drive               *next_drive;
   class DriveClient         *clients;
   class DriveIO             *driveio;
   class cmd_Inquiry         *inquiry;                // these few remain *here*
   class cmd_TestUnitReady   *tur;                    //
   class cmd_Mode            *mode_io;                //
   class cmd_GetConfiguration*features;               // :)
   class cmd_ReadDiscInfo    *discinfo;               //
   Page<Page_Write>           pw;
   Page<Page_Capabilities>    pc;
   struct SignalSemaphore     rwaccess;               // for DRV_LockDrive
   
   long                       numLocks;
   Thread                    *pThread;
   unsigned long              lCurrentDiscSeqNum;
   unsigned long              drive_status;

   DRT_Mechanism              eMechanismType;
   Set                        eCapabilities;
   bool                       bRegistered;
   bool                       bLockInterOperations;   // true if we dont want additional operations inbetween our calls

   HookT<Drive, Thread*, void*>                 hHkProcMsgs;
   HookT<Drive, uint, uint*>                  hHkProcHandle;
   
   CfgHardware         *hwconfig;

   uint32                media_write_capabilities;
   uint32                media_read_capabilities;
   uint32                data_write_capabilities;
   uint32                data_read_capabilities;
   UWORD                drive_buffer_size;         // in kb

   uint32                current_drive_profile;
   uint16               selected_read_speed;
   uint16               selected_write_speed;

   RWSyncT<class Disc*> current_disc;
                        Drive(char* pDeviceName, int lUnitNum);

   bool                 ProcInit(void);
   unsigned long        ProcMsgs(Thread*, void*);
   void                 ProcExit(void);



protected:
                       ~Drive();                
public:
   static Drive        *OpenDrive(Drive*& pChain, char*, int);
   Drive               *FindDrive(char*, int);
   Drive               *CleanUpDrives(void);
   DriveClient         *AddClient(void);
   void                 RemClient(DriveClient*);
   void                 AnalyseDisc(void);
   void                 AnalyseDrive(void);

   static uint32         ScanDevice(char* sDeviceName);
   static uint32         FreeScanResults(ScanData*pData);
   
   uint32                SendMessage(uint32* pMsg);

   long                 LockDrive(long lLockType);

   DriveIO             *GetDriveIO(void)
      {  return driveio;                     };

   DbgHandler          *GetDebug(void)
      {  return DEBUG_ENGINE;                };

   uint32                CurrentStatus(void)
      {  return drive_status;                };

   uint32                MediaReadCapabilities(void)
      {  return media_read_capabilities;     };

   uint32                MediaWriteCapabilities(void)
      {  return media_write_capabilities;    };

   uint32                DataReadCapabilities(void)
      {  return data_read_capabilities;      };

   uint32                DataWriteCapabilities(void)
      {  return data_write_capabilities;     };

   uint32                DriveBufferSize(void)
      {  return drive_buffer_size;           };

   uint32                CurrentProfile(void)
      {  return (int)current_drive_profile;  };

   CfgHardware         *GetHardwareConfig()
      {  return hwconfig;                    };

   uint                             GetDriveAttrs(uint, uint);
   uint                             SetDriveAttrs(uint, uint);

   uint32                            SetDriveAttrs(TagItem*);
   uint                             HandleMessages(uint Cmd, uint *Msg);

   Page<Page_Write>                &GetWritePage();
   Page<Page_Capabilities>         &GetCapabilitiesPage();
   int                              SetPage(Page_Header*);
   int                              StartStopUnit(int);

   class cmd_GetConfiguration::Feature *GetDriveFeature(cmd_GetConfiguration::FeatureId id);
};

class DriveSense 
{
/// struct _sense
   struct _sense
   {
      struct _1 : protected aLong
      {
         bool     isValid()      { return getField(31, 1); }
         uint8    getCode()      { return getField(24, 7); }
         void     setCode(uint8 v) { setField(24, 7, v);   }
         bool     isFileMark()   { return getField(15, 1); }
         bool     isEndOfMedium(){ return getField(14, 1); }
         bool     isILI()        { return getField(13, 1); }
         uint8    getKey()       { return getField(8, 3);  }
      } primary;
      
      struct _2 : protected aLong
      {
         uint8    getLength()    { return getField(0, 8);  }
      } extended;
      
      aLong       specific;

      struct _3 : protected aWord
      {
         uint8    getASC()       { return getField(8, 8);  }
         uint8    getASCQ()      { return getField(0, 8);  }
      } secondary __attribute__((packed));

      struct _4 : protected aLong
      {
         uint8    getFRU()       { return getField(24, 8); }
         bool     isDataValid()  { return getField(23, 1); }
         uint32   getSenseData() { return getField(0, 23); }
      } sensedata __attribute__((packed));
   };
///.
   _sense   *sense;

   public:

               DriveSense(void);
              ~DriveSense(void);
   UBYTE*      SenseData(void);
   int         SenseLength(void);
   void        Prepare(void);
   int         Key(void);
   int         Code(void);
   int         ASC(void);
   int         ASCQ(void);
   uint32       SCSIError(void);
};

class DriveIO 
{
   MsgPort       *ioport;
   IOStdReq      *ioreq;
   bool           opened;
   DriveSense    *sense;
   DEFINE_DEBUG;

   public:

                  DriveIO(DbgHandler*);
                 ~DriveIO(void);
   bool           OpenDev(const char* AName, int AUnit);
   int            Exec(SCSICommand*);
   DriveSense    *Sense(void);


   int            IsOpened(void)
     {  return opened;                 };
};

class DriveClient 
{
   DEFINE_DEBUG;
   DriveClient         *next_client;
   Drive               *drive;

   public:
                       ~DriveClient(void);         // DON'T CALL IT
                        DriveClient(DriveClient*, Drive*);
   DriveClient         *UnLink(DriveClient*);
   class Drive         *GetDrive(void);
   uint32               GetDriveStatus(void);
   uint32               Send(uint32*);
   uint32               WaitForDisc(int lDuration);
   uint                 GetDriveAttrs(TagItem*);
   uint                 GetDriveAttrs(uint);
};

class DriveSpool 
{
   DEFINE_DEBUG;
   Drive               *drive_list;
   static DriveSpool   *pInstance;           // SINGLETON

                        DriveSpool(void);
                       ~DriveSpool(void);
   public:

   static DriveSpool   *GetInstance();
   static void          ExitInstance();
   
   DriveClient         *NewClient(char*, int);
   void                 DelClient(DriveClient*);
   DriveClient         *CloneClient(DriveClient*);
};

#endif
