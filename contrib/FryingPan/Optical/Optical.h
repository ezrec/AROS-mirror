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

#ifndef _OPTICAL_DRIVE_H
#define _OPTICAL_DRIVE_H

#include <Generic/Types.h>
#ifdef __amigaos4__
#include <exec/interfaces.h>
#endif
#include <libclass/exec.h>

#undef MOS_DIRECT_OS
#include <GenericLib/Calls.h>

#ifndef __mc68000
#pragma pack (1)
#endif

enum 
{
   DRV_NoOP       =  0x000000,      // [..][       ]=[drive]
   DRV_ScanDevice,                  // [..][ScanData*]=[char*] call this to get a list of scsi devices found on a given device
   DRV_FreeScanResults,             // [..][error  ]=[ScanData* ] call this to release scan results.

   // now the dedicated commands
   DRV_NewDrive   =  0x100000,      // [..][drive  ]=[name, unit]
   DRV_CloneDrive,                  // [..][drive  ]=[drive     ]
   DRV_EndDrive,                    // [..][       ]=[drive]
   DRV_Format,                      // [FA][error  ]=[drive, type]
   DRV_Blank,                       // [FA][error  ]=[drive, type]
   DRV_ControlUnit,                 // [FA][error  ]=[drive, type]
   DRV_PrepareWrite,                // [..][error  ]=[drive, type]
   DRV_AbortOperation,              // [..][       ]=[drive], abort pending operation
   DRV_AddTrack,                    // [..][error  ]=[drive, type, size] = block offset
   DRV_CloseDisc,                   // [.A][error  ]=[drive, type, track]
   DRV_GetAttrs,                    // [..][error  ]=[drive, tagarray]
   DRV_SetAttrs,                    // [..][error  ]=[drive, tagarray]
   DRV_SCSICommand,                 // [..][error  ]=[drive, cmd, cmdlen, buff, blen, flags]
   DRV_PlayAudio,                   // [..][error  ]=[drive, track, trkoffset, endoffset]
   DRV_PlayControl,                 // [..][error  ]=[drive, playctl]
   DRV_GetAttr,                     // [..][value  ]=[drive, attr]
   DRV_SetAttr,                     // [..][error  ]=[drive, attr, value]
   DRV_RepairDisc,                  // [.A][error  ]=[drive, type], not advisable.
   DRV_LayoutTracks,                // [..][error  ]=[drive, DiscItem*]
   DRV_UploadLayout,                // [..][error  ]=[drive, DiscItem*]
   DRV_StructureDisc,               // [..][error  ]=[drive]
   DRV_ReadTrackAbsolute,           // [..][error  ]=[drive, DiscItem*, startsec, count, buffer] always from track start
   DRV_ReadTrackRelative,           // [..][error  ]=[drive, DiscItem*, startsec, count, buffer] always relative to 0
   DRV_WriteTrackAbsolute,          // [..][error  ]=[drive, DiscItem*, startsec, count, buffer] always from track start
   DRV_WriteTrackRelative,          // [..][error  ]=[drive, DiscItem*, startsec, count, buffer] always relative to 0
   DRV_WriteSequential,             // [..][error  ]=[drive, data, numsectors];
   DRV_WaitForDisc,                 // [..][error  ]=[drive, timeout [s]]
   DRV_LockDrive,                   // [..][error  ]=[drive, DRT_LockDrive_*
   DRV_ForceUpdate,                 // [..][error  ]=[drive]
};

enum 
{
   DRA_AddTagNotify = 0x80DA0000,   // [.S.] ULONG        // DRA_*, adds tag notification upon DRV_CheckUpdate
   DRA_RemTagNotify,                // [.S.] ULONG        // DRA_*, removes tag notification
   DRA_DeviceName,                  // [N.G] char*        // i.e. scsi.device
   DRA_UnitNumber,                  // [N.G] ULONG        // i.e. 1
   DRA_IsOpened,                    // [N.G] BOOL         // 1 if drive opened
   DRA_Attention,                   // [.S.] BOOL         // suspend all checking and wait for commands [FASTER]
   DRA_EnableDiag,                  // [NSG] BOOL         // enable drive self diag [hazardous for parallel blocking]

   
   DRA_Drive_ReadSpeeds =  0x81DA0000,    // [N.G] UWORD*       // read speeds in kBps
   DRA_Drive_WriteSpeeds,           // [N.G] UWORD*       // write speeds in kBps
   DRA_Drive_CurrentOperation,      // [N.G] ULONG        // OP_*
   DRA_Drive_OperationProgress,     // [N.G] UWORD        // progress in 0 .. 65535
   DRA_Drive_SenseData,             // [N.G] DriveSense*  // Drive sense data
   DRA_Drive_Status,                // [N.G] ULONG        // DRT_DriveStatus_*
   DRA_Drive_AbsoluteCDAddress,     // [N.G] LBA          // for CDAudio - absolute time
   DRA_Drive_RelativeCDAddress,     // [N.G] LBA          // for CDAudio - relative time
   DRA_Drive_TestMode,              // [NSG] BOOL         // writing in test mode
   DRA_Drive_Vendor,                // [N.G] char*/NULL   // drive vendor, i.e. TEAC
   DRA_Drive_Product,               // [N.G] char*/NULL   // drive product, i.e. CDW540E
   DRA_Drive_Version,               // [N.G] char*/NULL   // drive version, i.e. 1.0K
   DRA_Drive_Firmware,              // [N.G] char*/NULL   // drive firmware date
   DRA_Drive_ReadsMedia,            // [N.G] ULONG        // DRT_Media_*
   DRA_Drive_WritesMedia,           // [N.G] ULONG        // DRT_Media_*
   DRA_Drive_MechanismType,         // [N.G] ULONG        // DRT_Mechanism_*
   DRA_Drive_Capabilities,          // [N.G] ULONG        // DRT_Data_*
   DRA_Drive_BufferSize,            // [N.G] UWORD        // in kB
   DRA_Drive_BufferUsed,            // [N.G] UWORD        // amount [in kB] of buffer in use
   DRA_Drive_VolumeLevels,          // [N.G] UWORD        // volume levels
   DRA_Drive_CurrentAudioTrackIndex,// [N.G] UBYTE:UBYTE  // audio track / index
   DRA_Drive_CurrentProfile,        // [N.G] ULONG        // DRT_Profile_*
   DRA_Drive_CurrentReadSpeed,      // [NSG] UWORD        // this is what user wants
   DRA_Drive_CurrentWriteSpeed,     // [NSG] UWORD        // in kBps please!!!
   DRA_Drive_IsRegistered,          // [N.G] BOOL         // 1 if regged, 0 if not.
   DRA_Drive_WriteParams,           // [NSG] ULONG        // DRT_Write_*
   DRA_Drive_IsDiscInserted,        // [..G] BOOL
   
   DRA_Disc_NumTracks = 0x82da0000, // [N.G] ULONG        // num tracks on disc
   DRA_Disc_Contents,               // [N.G] DiscItem*    // array of items
   DRA_Disc_ID,                     // [N.G] ULONG        // disc id
   DRA_Disc_Size,                   // [N.G] ULONG        // disc size
   DRA_Disc_State,                  // [N.G] ULONG        // DRT_State_*
   DRA_Disc_LastSess_State,         // [N.G] ULONG        // DRT_State_*
   DRA_Disc_Format,                 // [N.G] ULONG        // DRT_DFormat_*
   DRA_Disc_BarCode,                // [N.G] ULONG[2]     // array ptr
   DRA_Disc_CatalogNumber,          // [N.G] ??
   DRA_Disc_RW_Format,
   DRA_Disc_IsWritable,             // [..G] BOOL
   DRA_Disc_IsErasable,             // [..G] BOOL
   DRA_Disc_IsFormatable,           // [..G] BOOL
   DRA_Disc_SubType,                // [..G] ULONG        // DRT_SubType
   DRA_Disc_WriteMethod,            // [.SG] ULONG        // DRT_WriteMethod_*
   DRA_Disc_IsOverwritable,         // [..G] BOOL
   DRA_Disc_NextWritableTrack,      // [..G] DiscItem*    // get first with NULL, ends with NULL
   DRA_Disc_CurrentDiscSeqNum,      // [..G] ULONG        // current disc sequential number. increases each update.
   DRA_Disc_Vendor                  // [..G] const char*
};

enum EItemType
{
   Item_Disc = 0,
   Item_Session,
   Item_Track,
   Item_Index
};

enum EDataType 
{ 
   Data_Unknown = 0,    // 0000 bytes per sector, writing not supported
   Data_Audio,          // 2352 bytes per sector
   Data_Mode1,          // 2048 bytes per sector
   Data_Mode2,          // 2336 bytes per sector
   Data_Mode2Form1,     // 2048 bytes per sector
   Data_Mode2Form2      // 2324 bytes per sector
};

struct DiscSpeed
{
   uint8    i;                      // integer,
   uint8    f;                      // fraction (eg. 1.4)
   uint16   kbps;                   // reference value to be used in Set Read/Write Speed
};

enum DIF_Disc
{
   DIF_Disc_CloseSession    = 1,     // close last session (use multisession)
   DIF_Disc_CloseDisc       = 2,     // close disc after write (no multisession)
   DIF_Disc_MasterizeCD     = 4,     // write cd in SAO / DAO mode (cd must be empty)
};

enum DIF_Session
{
   DIF_Session_CDXA         = 1,     // for cd: write cdxa form1/2
   DIF_Session_CDI          = 2,     // for cd: write interactive videocd and such
};

enum DIF_Common
{
   DIF_RelativeSize        = 4096,     // if set, size is relative to parent; 0 = beginning of parent, -1 = total parent blocks
   DIF_Common_RawData      = 8192,     // 2352 bytes/sector
   DIF_Common_SubChannel   = 16384,    // subchannel data, 96 bytes / sector extra
   DIF_Common_Type         = (DIF_Common_RawData | DIF_Common_SubChannel)
};

struct ScanData
{
   struct ScanData  *sd_Next;
   char             *sd_DeviceName;
   char             *sd_Vendor;
   unsigned char     sd_Unit;
   unsigned char     sd_Type;
};

enum ELockType
{
   DRT_LockDrive_Unlock  = 0,
   DRT_LockDrive_Read    = 1,
   DRT_LockDrive_Write   = 2,
};

// session flags only affect CDR/RW discs recorded in sequential mode
// (i.e. no mount rainier nor mount fuji packet writing).
#define DIT_Session_MultiSession    1  // multisession CD
#define DIT_Session_SAO             2  // session at once CD
#define DIT_Session_Interactive     4  // interactive CD
#define DIT_Session_XA              8  // eXtended Architecture CD


#define DRT_Async          0x80000000  // return as soon as command is acknowledged (not all commands)
#define DRT_Force          0x40000000
#define DRT_AllTypes       0x00ffffff  // all types except for common [above] - allowed bits

enum DRT_Format
{
   DRT_Format_Complete     = 0,
   DRT_Format_Fast,
};

enum DRT_Blank
{
   DRT_Blank_Complete      = 0,
   DRT_Blank_Fast,
   DRT_Blank_Session,
};

enum DRT_Control
{
   DRT_Unit_Stop           =  0,
   DRT_Unit_Start,
   DRT_Unit_Eject,
   DRT_Unit_Load,
   DRT_Unit_Idle,
   DRT_Unit_Standby,
   DRT_Unit_Sleep,
};

enum DRT_Mechanism
{
   DRT_Mechanism_Unknown   =  0,
   DRT_Mechanism_Caddy,
   DRT_Mechanism_Tray,
   DRT_Mechanism_Popup,
   DRT_Mechanism_Changer,
   DRT_Mechanism_Magazine
};

#define DRT_DriveStatus_NotOpened   0
#define DRT_DriveStatus_Ready       1
#define DRT_DriveStatus_NoDisc      2
#define DRT_DriveStatus_DeviceBusy  3
#define DRT_DriveStatus_Preparing   4
#define DRT_DriveStatus_Analysing   5
#define DRT_DriveStatus_Playing     6
#define DRT_DriveStatus_Paused      7
#define DRT_DriveStatus_PlayError   8
#define DRT_DriveStatus_Syncing     9
#define DRT_DriveStatus_Calibrating 10
#define DRT_DriveStatus_Finalising  11
#define DRT_DriveStatus_Blanking    12
#define DRT_DriveStatus_Formatting  13
#define DRT_DriveStatus_Repairing   14
#define DRT_DriveStatus_Allocating  15    // allocating disc space for track
#define DRT_DriveStatus_Writing     16
#define DRT_DriveStatus_Unknown     255

enum DRT_Close
{
   DRT_Close_Track               = 0,
   DRT_Close_Session,
   DRT_Close_Finalize,
};

#define DRT_Write_MultiSession   1        // 1 = ms, 0 = single session
#define DRT_Write_Interactive    2        // 1 = CD-I, 0 = No CD-I
#define DRT_Write_CDXA           4        // 1 = CDXA, 0 = No CDXA
#define DRT_Write_TestMode       8        // 1 = test mode, 0 = no test mode


enum DiscWriteMethod
{
   DRT_WriteMethod_Default    =  0,       // a write method that matches best.
   DRT_WriteMethod_TAO,                   // CD only
   DRT_WriteMethod_SAO,                   // CD only
   DRT_WriteMethod_Packet                 // CD-MRW and DVD only
};


#define DRT_CD_R_WriteMethod_Default      DRT_WriteMethod_Default
#define DRT_CD_R_WriteMethod_TrackAtOnce  DRT_WriteMethod_Default
#define DRT_CD_R_WriteMethod_DiscAtOnce   1
#define DRT_CD_R_WriteMethod_Packet       2

#define DRT_CD_RW_WriteMethod_Default     DRT_WriteMethod_Default
#define DRT_CD_RW_WriteMethod_TrackAtOnce DRT_WriteMethod_Default
#define DRT_CD_RW_WriteMethod_DiscAtOnce  DRT_CD_R_WriteMethod_DiscAtOnce
#define DRT_CD_RW_WriteMethod_Packet      DRT_CD_R_WriteMethod_Packet         // turns disc to MRW (set it, then StructureDisc())

#define DRT_CD_MRW_WriteMethod_Default    DRT_WriteMethod_Default
#define DRT_CD_MRW_WriteMethod_Packet     DRT_WriteMethod_Default
#define DRT_CD_MRW_WriteMethod_Sequential 1                                   // turns disc back to RW (set it, then StructureDisc())

#define DRT_DVD_MR_WriteMethod_Default    DRT_WriteMethod_Default
#define DRT_DVD_MR_WriteMethod_Sequential DRT_WriteMethod_Default             // turns dvd from restricted overwrite to sequential (structuredisc())
#define DRT_DVD_MR_WriteMethod_Packet     1                                   // contrary.

#define DRT_DVD_PR_WriteMethod_Default    DRT_WriteMethod_Default
#define DRT_DVD_PR_WriteMethod_Packet     DRT_WriteMethod_Default             // only available

#define DRT_DVD_RAM_WriteMethod_Default   DRT_WriteMethod_Default
#define DRT_DVD_RAM_WriteMethod_Packet    DRT_WriteMethod_Default             // only available



#define DRT_Play_Track_Disc      0        // specify to set disc offset
#define DRT_Play_Track_Current   -1       // specify to set new track offset
#define DRT_Play_Track_Previous  -2       // play previous track
#define DRT_Play_Track_Next      -3       // play next track
#define DRT_Play_Offset_Current  -1       // continue playing from here, valid for track_current
#define DRT_Play_End_Disc        -1       // stop playing at end of disc
#define DRT_Play_End_Track       -2       // stop playing at end of track
#define DRT_Play_End_Now         0        // stop playing now

#define DRT_PlayCtl_Play         0        // resume playback
#define DRT_PlayCtl_Pause        1        // pause playback
#define DRT_PlayCtl_FastFwd      2        // fast forward
#define DRT_PlayCtl_FastRev      3        // fast reverse
#define DRT_PlayCtl_Stop         4        // stop playback
#define DRT_PlayCtl_TogglePause  5        // toggle pause / resume

enum DRT_Profile
{
   DRT_Profile_Unknown                  = 0,
   DRT_Profile_NoDisc,
   DRT_Profile_CD_ROM,
   DRT_Profile_CD_R,
   DRT_Profile_CD_RW,
   DRT_Profile_CD_MRW,
   DRT_Profile_DVD_ROM,
   DRT_Profile_DVD_MinusR,
   DRT_Profile_DVD_PlusR,
   DRT_Profile_DVD_MinusRW_Sequential,
   DRT_Profile_DVD_MinusRW_Restricted,
   DRT_Profile_DVD_PlusRW,
   DRT_Profile_DVD_RAM,
   DRT_Profile_DDCD_ROM,
   DRT_Profile_DDCD_R,
   DRT_Profile_DDCD_RW,
   DRT_Profile_BD_ROM,
   DRT_Profile_BD_R_Sequential,
   DRT_Profile_BD_R_RandomWrite,
   DRT_Profile_BD_RW,
};

enum DRT_SubType
{
   DRT_SubType_Unknown                 = 0,

   DRT_SubType_CD_RW_LowSpeed          = 1,
   DRT_SubType_CD_RW_HighSpeed,
   DRT_SubType_CD_RW_UltraSpeed,

   DRT_SubType_DVD_DualLayer           = 1,

   DRT_SubType_CD_R_Quality1           = 1,  // best
   DRT_SubType_CD_R_Quality2,
   DRT_SubType_CD_R_Quality3,
   DRT_SubType_CD_R_Quality4,
   DRT_SubType_CD_R_Quality5,
   DRT_SubType_CD_R_Quality6,
   DRT_SubType_CD_R_Quality7,
   DRT_SubType_CD_R_Quality8,
   DRT_SubType_CD_R_Quality9,                // worst
};

enum DRT_Media
{
   DRT_Media_CD_ROM                  = 1,      
   DRT_Media_CD_R                    = 2,
   DRT_Media_CD_RW                   = 4,
   DRT_Media_DVD_ROM                 = 8,
   DRT_Media_DVD_MinusR              = 16,
   DRT_Media_DVD_PlusR               = 32,
   DRT_Media_DVD_MinusRW             = 64,
   DRT_Media_DVD_PlusRW              = 128,
   DRT_Media_DVD_RAM                 = 256,
   DRT_Media_DVD_PlusR_DL            = 512,
   DRT_Media_BD_ROM                  = 1024,
   DRT_Media_BD_R                    = 2048,
   DRT_Media_BD_RW                   = 4096
};

enum DRT_Capabilities
{
   DRT_Can_Read_Mode2             = 0x00000001,
   DRT_Can_Read_CDAudio           = 0x00000002,
   DRT_Can_Read_CDAudioAccurate   = 0x00000004,
   DRT_Can_Read_Mode2Form1        = 0x00000008,
   DRT_Can_Read_Mode2Form2        = 0x00000010,
   DRT_Can_Read_ISRC              = 0x00000020,
   DRT_Can_Read_MCN               = 0x00000040,
   DRT_Can_Read_BarCode           = 0x00000080,
   DRT_Can_Read_CDText            = 0x00000100,
   DRT_Can_Read_RAW               = 0x00000200,    // silly
   DRT_Can_Read_MountRainier      = 0x00000400,    // not implemented!
   DRT_Can_Read_MultiSession      = 0x00000800,
   DRT_Can_Write_MountRainier     = 0x00001000,    // not implemented
   DRT_Can_Write_CDText           = 0x00002000,
   DRT_Can_Play_Audio             = 0x00004000,
   DRT_Can_Do_SeparateMute        = 0x00008000,
   DRT_Can_Do_VolumeChange        = 0x00010000,
   DRT_Can_Do_SeparateVolume      = 0x00020000,
   DRT_Can_Do_BurnProof           = 0x40000000,
   DRT_Can_Do_TestMode            = 0x80000000
};

#define DRT_CheckUpdate_Everything     1  // get *ALL* tags, not just those updated.

#define DRT_State_Empty                0
#define DRT_State_Appendable           1
#define DRT_State_Reserved             2
#define DRT_State_Finalized            3
#define DRT_State_RandomWritable       4
#define DRT_State_Unformatted          5

#define DRT_DFormat_Unknown            0
#define DRT_DFormat_CDROM              1
#define DRT_DFormat_CDI                2
#define DRT_DFormat_CDXA               3


enum EOpticalError 
{
   ODE_OK                  =  0,          // clear :D
   ODE_NoMemory            =  -1,         // not enough memory
   ODE_NoHandler           =  -2,         // unable to spawn handler [process]
   ODE_NoDevice            =  -3,         // there is no such device or unit
   ODE_InitError           =  -4,         // drive doesn't seem to respond to HD_SCSICMD
   ODE_BadDriveType        =  -5,         // drive is not a CD/DVD drive
   ODE_MediumError         =  -6,         // problem accessing medium
   ODE_IllegalType         =  -7,         // illegal type of action [like FORMAT] requested
   ODE_NoDevOpened         =  -8,         // no device has been opened
   ODE_CommandError        =  -9,         // device failed to execute a command
   ODE_NoFormatDescriptors =  -10,        // device does not specify any format IDs
   ODE_NoModePage          =  -11,        // no mode page is present
   ODE_DeviceBusy          =  -12,        // device is performing some operation and shouldn't be disturbed
   ODE_IllegalCommand      =  -13,        // this command shouldn't be issued here
   ODE_TooManyTracks       =  -14,        // track limit exceeded for writing
   ODE_TooManySessions     =  -15,        // session limit exceeded for writing (only one allowed)
   ODE_IllegalParameter    =  -16,        // illegal command parameter
   ODE_NoDisc              =  -17,        // no disc inside
   ODE_NotEnoughSpace      =  -18,        // not enough disc space
   ODE_DiscNotEmpty        =  -19,        // disc is not empty
   ODE_BadSessionNumber    =  -20,        // session number does not match
   ODE_BadTrackNumber      =  -21,        // track number does not match
   ODE_BadIndexNumber      =  -22,        // index number is wrong
   ODE_BadLayout           =  -23,        // when, say, we get session after tracks
   ODE_DiscFull            =  -24,        // disc is finalised, no spare space.
   ODE_DiscReserved        =  -25,        // disc is reserved.
   ODE_DiscNotFormatted    =  -26,        // disc has not been formatted yet.
   ODE_DeviceNotReady      =  -27,        // device not ready for operations.
   ODE_BadTrackMode        =  -28,        // the specified track mode is invalid
   ODE_BadBlockNumber      =  -29,        // bad block number or number beyond boundary
   ODE_Exiting             =  -30,        // when application is exiting. don't expect that too often.
   ODE_DriveLockedForWrite =  -31,
   ODE_DriveLockedForRead  =  -32,
   ODE_BadTransferSize     =  -33,        // for packet/incremental discs you have to send packets of fixed size, i.e. 16 sectors.
   ODE_NoDataReturned      =  -34,        // no data has been returned during previous transfer
};

   BEGINDECL(Optical, "Optical")
      FUNC1(IPTR,           OptDoMethodA,  5, IPTR,   parm, a0);
      FUNC0(class IOptItem*,  OptCreateDisc, 6);
   ENDDECL


#ifndef __mc68000
#pragma pack()
#endif

#endif
