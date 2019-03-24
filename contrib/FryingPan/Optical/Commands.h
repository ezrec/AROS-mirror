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

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include "Various.h"
#ifndef __mc68000
#pragma pack(1)
#endif

#include <Generic/String.h>
#include <Generic/DynList.h>
#include <Generic/Types.h>
#include "IOptItem.h"

#define OFFSET(type, field) \
   ((size_t)(&((type*)1)->field)-1)

#define OFFSETWITH(type, field) \
   (((size_t)(&((type*)1)->field))+sizeof(type::field)-1)


using namespace GenNS;

class aByte
{
   uint8       byte;
public:
   uint8       getField(int8 off, int8 len);
   aByte      &setField(int8 off, int8 len, uint8 data);
};

class aWord
{
   uint16      word;
public:
               operator uint16();
   aWord      &operator =(uint16 data);
   uint16      getField(int8 off, int8 len);
   aWord      &setField(int8 off, int8 len, uint16 data);
};

class aLong
{
   uint32      word;
public:
               operator uint32();
   aLong      &operator =(uint32 data);
   uint32      getField(int8 off, int8 len);
   aLong      &setField(int8 off, int8 len, uint32 data);
};


struct TrackInfo
{
private:

   aWord       length;
   uint8       track_number_lsb;
   uint8       session_number_lsb;

   struct _1 : public aLong
   {
      bool     isDamaged()    { return getField(21, 1); }
      bool     isCopy()       { return getField(20, 1); }
      uint8    getTrackMode() { return getField(16, 4); }
      bool     isReserved()   { return getField(15, 1); }
      bool     isBlank()      { return getField(14, 1); }
      bool     isPacket()     { return getField(13, 1); }
      bool     isFixedPacket(){ return getField(12, 1); }
      uint8    getDataMode()  { return getField(8,  4); }
      bool     isLRAValid()   { return getField(1,  1); }
      bool     isNWAValid()   { return getField(0,  1); }
   } flags;

   aLong       track_start;
   aLong       next_writable_address;
   aLong       free_blocks;
   aLong       packet_size;
   aLong       track_size;
   aLong       last_recorded_address;

   uint8       track_number_msb;
   uint8       session_number_msb;
   aWord       reserved;
   aLong       read_compatibility_lba;

   uint8       resvd[0];

public:

   int32      Length(void)
   {
      return length+2;
   };

   int32      GetTrackNumber(void)
   {
      if (Length() >= 34)
         return (track_number_msb<<8) | track_number_lsb;
      else
         return (track_number_lsb);
   };

   int32      GetSessionNumber(void)
   {
      if (Length() >= 34)
         return (session_number_msb<<8) | session_number_lsb;
      else
         return (session_number_lsb);
   };

   uint32    GetStartAddress(void)
   {
      return track_start;
   };

   uint32    GetEndAddress(void)
   {
      return track_start + track_size - 1;
   };

   uint32    GetActualSize(void)
   {
      if (IsBlank()) 
         return 0;
      if (!flags.isLRAValid()) 
         return GetTotalSize();
      return last_recorded_address - track_start;
   }

   uint32    GetTotalSize(void)
   {
      return track_size;
   };

   uint32    GetFreeBlocks()
   {
      return free_blocks;
   }

   int32      IsRecordable(void)
   {
      return flags.isNWAValid();
   };

   int32      IsDamaged(void)
   {
      return flags.isDamaged();
   };

   int32      IsIncremental(void)
   {
      if ((flags.getTrackMode() & ~2) == 5) 
         return 1;               // track partially/entirely recorded in incremental mode
      return flags.isPacket();   // track configured to be recorded in incremental mode
   };

   int32      IsReserved(void)
   {
      return flags.isReserved();
   };

   int32      IsProtected(void)
   {
      return Transform::CtlToProtection(flags.getTrackMode());
   }

   EDataType GetTrackType(void)
   {
      if (flags.isBlank()) 
         return Data_Unknown;

      switch (flags.getTrackMode() & ~3) {
         case 0:     
            return Data_Audio;
         case 4:     
           if       (flags.getDataMode() == 1)  
              return Data_Mode1;  // yep
           else if  (flags.getDataMode() == 2)  
              return Data_Mode2;  // alright
           else if  (flags.getDataMode() == 15) 
              return Data_Mode1;  // officially: UNKNOWN
           else                       
              return Data_Unknown;    // officially: UNSUPPORTED
         case 8:                      
           return Data_Audio;
         case 12:                     
           return Data_Unknown;
      };
      return Data_Unknown;
   };

   int32      IsPreemphasized(void)
   {
      return Transform::CtlToEmphasy(flags.getTrackMode());
   };

   int32      IsBlank(void)
   {
      return flags.isBlank();
   };

   int32      GetPacketSize(void)
   {
      return packet_size ? packet_size : 16;
   };

   int32      GetSectorSize(void)
   {
      if (IsBlank()) return 2048;                              // a small fix for faranheit; will display blanks properly
      return Transform::TrackTypeToSectorSize(GetTrackType());
   };

   void     FillInDiscItem(IOptItem*);
};

struct DiscInfo 
{
private:
   aWord       length;
   struct _1 : public aByte
   {
      bool  isErasable()            { return getField(4, 1); }
      uint8 getLastSessionStatus()  { return getField(2, 2); }
      uint8 getDiscStatus()         { return getField(0, 2); }
   } status __attribute__((packed));

   uint8       first_track_number;
   uint8       num_sessions_lsb;
   uint8       first_track_in_last_session_lsb;
   uint8       last_track_in_last_session_lsb;

   struct _2 : public aByte
   {
      bool isDiscIDValid()          { return getField(7, 1); }
      bool isDiscBarCodeValid()     { return getField(6, 1); }
      bool isDiscUsageUnrestricted(){ return getField(5, 1); }
   } flags __attribute__((packed));

   uint8       disc_type;
   uint8       num_sessions_msb;
   uint8       first_Track_in_last_session_msb;
   uint8       last_track_in_last_session_msb;

   aLong       disc_id_code;
   aLong       last_session_leadin_start_time;
   aLong       last_possible_leadout_start_time;
   aLong       disc_barcode[2];

   uint8       resvd3;
   uint8       num_opc_entries;

   struct OPCEntry {
      aWord  write_speed;
      aWord  opc_values[3];
   } opc[0];

   public:
   int32   Length(void)
   {
      return length+2;
   };

   bool    IsLastSessionIncomplete()
   {
      return status.getLastSessionStatus() == 1;
   }

   bool    IsLastSessionEmpty()
   {
      return status.getLastSessionStatus() == 0;
   }

   bool    IsLastSessionClosed()
   {
      return status.getLastSessionStatus() > 1;
   }

   bool    IsDiscEmpty()
   {
      return status.getDiscStatus() == 0;
   }

   bool    IsDiscIncomplete()
   {
      return status.getDiscStatus() == 1;
   }

   bool    IsDiscClosed()
   {
      return status.getDiscStatus() > 1;
   }

   int32   IsWritable(void)
   {
      if  (last_possible_leadout_start_time == 0xffffffff) return 0;
      return 1;
   }

   int32   IsErasable(void)
   {
      return status.isErasable();
   }
      
   int32   GetNumTracks(void)
   {
      return (last_track_in_last_session_msb << 8) | (last_track_in_last_session_lsb);
   }

   unsigned long GetLeadInStart()
   {
      return Transform::MsfToLba(last_session_leadin_start_time);
   }
   
   unsigned long GetLeadInLength()
   {
      if ((last_session_leadin_start_time != 0xffffff) &&
          (last_session_leadin_start_time != 0x000000))
      {
         if (last_session_leadin_start_time >= (80<<16))  // only if larger than 80 minutes
         {
            return 450000 - Transform::MsfToLba(last_session_leadin_start_time);
         }
         return Transform::MsfToLba(1, 0, 0);
      }
      return 0;
   }
};



class Page_Header
{ 
private:
   aWord    length;
   uint8    ccs_code;
   uint8    reserved[4];
   uint8    scsi;
   uint8    scsidata[0];

public:

   inline int32  TotalSize(void)
   {  
      return length+2;      
   };
  
   inline void*GetPage()
   {
      return &scsidata[scsi];
   }
};

class Page_ID
{
protected:
   struct _1 : public aByte
   {
      bool  isSavable()    { return getField(7, 1); }
      bool  isModified()   { return getField(6, 1); }
      void  setModified()  { setField(6, 1, 1);     }
      void  clrModified()  { setField(6, 1, 0);     }
      uint8 getPage()      { return getField(0, 6); }
   } header __attribute__((packed));
   uint8          page_length;

public:
   uint8          PageID()
   {
      return header.getPage();
   }

   int32          PageSize()
   {
      return page_length + 2;
   }

   void           SetModified()
   {
      header.setModified();
   }

   bool           IsModified()
   {
      return header.isModified();
   }

   void           ClearModified()
   {
      header.clrModified();
   }
};

struct Page_Capabilities : public Page_ID
{
private:
   struct _1 : protected aLong
   {
      bool doesReadDVDRam()      { return getField(29, 1); }
      bool doesReadDVDMinusR()   { return getField(28, 1); }
      bool doesReadDVDRom()      { return getField(27, 1); }
      bool doesReadMethod2()     { return getField(26, 1); }
      bool doesReadCDRW()        { return getField(25, 1); }
      bool doesReadCDR()         { return getField(24, 1); }
      bool doesWriteDVDRam()     { return getField(21, 1); }
      bool doesWriteDVDMinusR()  { return getField(20, 1); }
      bool doesWriteTestMode()   { return getField(18, 1); }
      bool doesWriteCDRW()       { return getField(17, 1); }
      bool doesWriteCDR()        { return getField(16, 1); }
      bool doesBurnProof()       { return getField(15, 1); }
      bool doesMultisession()    { return getField(14, 1); }
      bool doesMode2Form2()      { return getField(13, 1); }
      bool doesMode2Form1()      { return getField(12, 1); }
      bool hasDigitalPort2()     { return getField(11, 1); }
      bool hasDigitalPort1()     { return getField(10, 1); }
      bool hasCompositeOut()     { return getField(9, 1);  }
      bool doesPlayAudio()       { return getField(8, 1);  }
      bool doesReadBarCode()     { return getField(7, 1);  }
      bool doesReadUPC()         { return getField(6, 1);  }
      bool doesReadISRC()        { return getField(5, 1);  }
      bool hasC2Pointers()       { return getField(4, 1);  }
      bool doesRWCorrectio()     { return getField(3, 1);  }
      bool doesRWData()          { return getField(2, 1);  }
      bool isAudioAccurate()     { return getField(1, 1);  }
      bool isCDDASupported()     { return getField(0, 1);  }
   } rw;

   struct _2 : protected aWord
   {
      uint8 getMechanismType()   { return getField(13, 3); }
      bool  doesEject()          { return getField(11, 1); }
      bool  hasPreventionJumper(){ return getField(10, 1); }
      bool  doesLockState()      { return getField(9, 1);  }
      bool  doesLock()           { return getField(8, 1);  }
      bool  doesRWLeadIn()       { return getField(5, 1);  }
      bool  doesChangeSide()     { return getField(4, 1);  }
      bool  doesSoftSlotSelect() { return getField(3, 1);  }
      bool  doesDiscDetection()  { return getField(2, 1);  }
      bool  doesChannelMute()    { return getField(1, 1);  } // separate channel mute
      bool  doesChannelVolume()  { return getField(0, 1);  }
   } mechanism __attribute__((packed));

   aWord          max_read_speed;
   aWord          num_volume_levels;
   aWord          buffer_size;
   aWord          current_read_speed;

   struct _3 : protected aWord
   {
      uint8 getLength()          { return getField(4, 2); }
      bool  doesSupportLSBF()    { return getField(3, 1); }
      bool  doesSupportRCK()     { return getField(2, 1); }
      bool  doesSupportBCKF()    { return getField(1, 1); }
   } data __attribute__((packed));

   aWord          max_write_speed;
   aWord          current_write_speed_old;

   aWord          copy_management_revision;

   struct _4 : protected aLong
   {
      uint8 getSelRotationCtl()  { return getField(0, 2); }
   } rotation __attribute__((packed));

   aWord          current_write_speed;
   aWord          num_speed_performance_descriptors;
   aLong          speed_performance_descriptors[];

public:

   int32            GetSpeedDescriptorsCount();
   int32            GetSpeedDescriptor(int32 i);
   int32            GetMediaReadSupport();
   int32            GetMediaWriteSupport();
   uint             GetCapabilities();
   int32            GetAudioFlags();
   int32            GetAudioVolumeLevels();
   uint16           GetCurrentWriteSpeed();
   uint16           GetCurrentReadSpeed();
   uint16           GetMaximumWriteSpeed();
   uint16           GetMaximumReadSpeed();
   uint16           GetBufferSize();
   DRT_Mechanism    GetMechanismType();
};

struct Page_Write : public Page_ID
{
private:
   struct _1 : protected aLong
   {
      bool  isBurnProof()              { return getField(30, 1);  }
      void  setBurnProof(bool f)       { setField(30, 1, f);      }
      bool  isLinkSizeValid()          { return getField(29, 1);  }
      void  setLinkSizeValid(bool f)   { setField(29, 1, f);      }
      bool  isTestMode()               { return getField(28, 1);  }
      void  setTestMode(bool f)        { setField(28, 1, f);      }
      uint8 getWriteType()             { return getField(24, 4);  }
      void  setWriteType(uint8 t)      { setField(24, 4, t);      }
      
      uint8 getMultisession()          { return getField(22, 2);  }
      void  setMultisession(uint8 t)   { setField(22, 2, t);      }
      bool  isFixedPacket()            { return getField(21, 1);  }
      void  setFixedPacket(bool f)     { setField(21, 1, f);      }
      bool  isCopy()                   { return getField(20, 1);  }
      void  setCopy(bool f)            { setField(20, 1, f);      }
      uint8 getTrackMode()             { return getField(16, 4);  }
      void  setTrackMode(uint8 t)      { setField(16, 4, t);      }

      uint8 getDataType()              { return getField(8, 4);   }
      void  setDataType(uint8 t)       { setField(8, 4, t);       }

      uint8 getLinkSize()              { return getField(0, 8);   }
      void  setLinkSize(uint8 t)       { setField(0, 8, t);       }
   } set1;

   struct _2 : protected aLong
   {
      uint8 getApplicationCode()       { return getField(16, 6);  }
      void  setApplicationCode(uint8 t){ setField(16, 6, t);      }
      uint8 getSessionFormat()         { return getField(8, 8);   }
      void  setSessionFormat(uint8 t)  { setField(8, 8, t);       }
   } set2;

   aLong          packet_size;
   aWord          audio_pause_length;
   int8           upc[16];
   int8           isrc[16];
   uint8          subheader[4];
   uint8          vendor_specific[4];

public:

   enum WriteType
   {
      WriteType_Packet,
      WriteType_TrackAtOnce,
      WriteType_SessionAtOnce,
      WriteType_Raw
   };

   enum TrackMode
   {
      TrackMode_Audio            = 0,
      TrackMode_Data_Sequential  = 4,
      TrackMode_Data_Incremental = 5
   };

   enum DataMode
   {
      DataMode_Raw                  = 0,     // 2352
      DataMode_Raw_PQ               = 1,     // 2368
      DataMode_Raw_PW               = 2,     // 2448
      DataMode_Raw_PW_Interleaved   = 3,     // 2448
      DataMode_Mode1                = 8,     // 2048
      DataMode_Mode2                = 9,     // 2336
      DataMode_Mode2Form1           = 10,    // 2048
      DataMode_Mode2Form1B          = 11,    // 2056
      DataMode_Mode2Form2           = 12,    // 2324
      DataMode_Mode2Form2B          = 13     // 2332
   };

   enum SessionFormat
   {
      Session_Data                  = 0,     // regular
      Session_Interactive           = 16,    // cd-i
      Session_ExtendedArchitecture  = 32
   };

   void           SetWriteType(WriteType t);
   WriteType      GetWriteType(void);
   void           SetPacketSize(ULONG);
   uint32         GetPacketSize(void);
   DataMode       GetDataMode(void);
   void           SetDataMode(DataMode);
   TrackMode      GetTrackMode(void);
   void           SetTrackMode(TrackMode);
   void           SetLinkSize(uint8);
   SessionFormat  GetSessionFormat(void);
   void           SetSessionFormat(SessionFormat);
   void           SetTestMode(int32);
   int32          IsTestMode(void);
   void           SetMultisession(int32);
   int32          IsMultisession(void);
};

template <class T=class Page_ID>
class Page 
{
protected:
   Page_Header      *header;
   T                *page;
   bool              own;     // true = page is owned.

public:
   Page()
   {
      header   = 0;
      page     = 0;
      own      = true;
   }

   Page(Page_Header*p)
   {
      header   = 0;
      page     = 0;
      own      = true;
      *this = p;
   };

   Page<T> &operator =(Page_Header* p)
   {
      if (own && (header != 0))
         delete header;

      header = p;
      if (header != 0)
         page   = (T*)p->GetPage();
      else
         page   = 0;
      return *this;
   }

   ~Page()
   {
      if (own && (header != 0))
         delete header;
      header = 0;
   }

   operator Page_Header*()
   {
      return header;
   }

   T* operator ->()
   {
      return page;
   }

   operator T*()
   {
      return page;
   }

   bool IsValid()
   {
      return (header != 0) ? true : false;
   }

   void SetOwnPage(bool isown)
   {
      own = isown;
   }
};


class DVD_Structure
{
private:
   aWord       len;
   aWord       resvd;

public:
   int32         Length()
   {
      return len+2;
   }
};

template <class T=class DVD_Structure>
class DVDStructure 
{
   T*       structure;

public:
                        DVDStructure()
   {
      structure = 0;
   }      

                        DVDStructure(DVD_Structure* str)
   {
      *this = str;
   }
                 
                       ~DVDStructure()
   {
      delete [] (structure);
      structure = 0;
   }      
   
   DVDStructure<T>     &operator =(DVD_Structure *t)
   {
      if (structure)
         delete [] (structure);
      structure = (T*)t;
      return *this;
   }

   T                   *operator->()
   {
      return structure;
   }

   bool                 IsValid()
   {
      return (structure != 0) ? true : false;
   }
};

class DVD_Physical : public DVD_Structure
{
public:
   enum LayerType
   {
      Layer_Embossed = 0,
      Layer_Recordable,
      Layer_Rewritable,
      Layer_Unknown
   };

   enum DiscType
   {
      Disc_DVD_ROM = 0,
      Disc_DVD_RAM,
      Disc_DVD_MinusR,
      Disc_DVD_MinusRW,

      Disc_DVD_PlusRW=9,
      Disc_DVD_PlusR,

      Disc_DVD_PlusRW_DL=13,
      Disc_DVD_PlusR_DL
   };
private:
   struct _1 : public aLong
   {
      uint8 getBookType()        { return getField(28, 4); }
      uint8 getPartVersion()     { return getField(24, 4); }
      uint8 getDiscSize()        { return getField(20, 4); }
      uint8 getMaxRate()         { return getField(16, 4); }
      uint8 getLayerCount()      { return getField(13, 2); }
      bool  isTrackPath()        { return getField(12, 1); }
      uint8 getLayerType()       { return getField(8, 4);  }
      uint8 getLinearDensity()   { return getField(4, 4);  }
      uint8 getTrackDensity()    { return getField(0, 4);  }
   } conf;

   aLong             start_physical_sector;
   aLong             end_physical_sector;
   aLong             end_physical_sector_layer0;

   struct _2 : public aByte
   {
      bool isBurstCuttingAreaPresent() { return getField(7, 1); }
   } bca __attribute__((packed));

// medium-specific data to follow.

public:
   int32               GetDiscSize()
   {
      return conf.getDiscSize();
   }

   int32               GetNumLayers()
   {
      return conf.getLayerCount();
   }

   LayerType         GetLayerType()
   {
      return (LayerType)conf.getLayerType();
   };

   DiscType          GetDiscType()
   {
      return (DiscType)conf.getBookType();
   }

   bool              BCAPresent()
   {
      return bca.isBurstCuttingAreaPresent();
   }
};

class DVD_Copyright : public DVD_Structure
{
   enum ProtectionType
   {
      Prot_None,
      Prot_CSS,
      Prot_CPRM,

      Prot_Unknown
   };

protected:
   uint8             protection_type;
   uint8             region_management_information;      // each bit for one of eight regions
   uint16            reserved;                           // where disc can be played

public:  
   ProtectionType    GetProtectionType()
   {
      return (ProtectionType)protection_type;
   }

   uint8             RegionsAllowed()
   {
      return region_management_information;
   }
};

class DVD_DiscKey: public DVD_Structure
{
protected:
   uint8       key[2048];

public:
   uint8      *GetKey()
   {
      return (uint8*)&key;
   }
};

class DVD_BurstCutArea : public DVD_Structure
{
protected:
   uint8       bca[0];

public:
   uint8      *GetBCA()
   {
      return (uint8*)&bca;
   }
};

class DVD_Manufacturer : public DVD_Structure
{
protected:
   uint8       manufacturing_info[];

public:
   uint8      *GetManufacturingInfo()
   {
      return (uint8*)&manufacturing_info;
   }
};

class DVD_CopyrightMgmt : public DVD_Structure
{
protected:
   uint8       copyright_mgmt;
   uint8       resvd0[3];

public:
   // these fields make sense only with minus r(w) and rom media.
   bool        IsCPM()        
   {
      return (copyright_mgmt & 0x80) ? true : false;
   }

   bool        IsCPSector()
   {
      return (copyright_mgmt & 0x40) ? true : false;
   }

   bool        UseCSS()
   {
      if (IsCPSector())
      {
         return (copyright_mgmt & 7) == 0;
      }
      return false;
   }

   bool        UseCPPM()
   {
      if (IsCPSector())
      {
         return (copyright_mgmt & 7) == 1;
      }
      return false;
   }
};

class DVD_MediaIdentifier : public DVD_Structure
{
protected:
   uint8       media_id_data[0];

public:
   uint8      *GetMediaID()
   {
      return (uint8*)&media_id_data;
   };
};

class DVD_MediaKeyBlock : public DVD_Structure
{
protected:
   uint8       media_key_block[0];

public:
   uint8      *GetMediaKeyBlock()
   {
      return (uint8*)&media_key_block;
   }
};

class DVD_PreRecordedLeadIn : public DVD_Structure
{
protected:
   uint8             field_id_1;
   uint8             disc_application_code;
   
   struct _1 : public aLong
   {
      uint8  getDiscPhysicalSize()        { return getField(24, 8); }
      uint32 getLastRecordableAddress()   { return getField(0, 24); }
   } size __attribute__((packed));

   struct _2 : public aWord
   {
      uint8  getPartVersion()             { return getField(12, 4);  }
      uint8  getExtensionCode()           { return getField(8, 4);   }
   } part __attribute__((packed));

   uint8             field_id_2;
   uint8             opc_suggested_code;
   uint8             wavelength_code;
   uint8             write_strategy_code1[4];
   uint8             reserved1;

   uint8             field_id_3;
   uint8             manufacturer1[6];
   uint8             reserved2;

   uint8             field_id_4;
   uint8             manufacturer2[6];
   uint8             reserved3;

   uint8             field_id_5;
   uint8             write_strategy_code2[6];
   uint8             reserved4;

public:

   String            GetManufacturer()
   {
      String s;
      if (field_id_3 == 3)
         s += (char*)&manufacturer1;
      if (field_id_4 == 4)
         s += (char*)&manufacturer2;

      return s;
   }
};

class DVD_DiscControlBlock : public DVD_Structure
{
   aLong             id;
   aLong             unknown_content_actions;
   char              vendor[32];
public:
   uint32            GetID()
   {
      return id;
   }

   String            GetVendor()
   {
      char x[33];
      String s;
      strncpy(x, vendor, 32);
      x[32] = 0;
      s = (char*)&x;
      return s;
   }
};


struct TOC_Entry 
{
private:

   uint8    session;
   struct _1 : protected aByte
   {
      uint8 getAdr() { return getField(4, 4); }
      uint8 getCtl() { return getField(0, 4); }
   } adrctl __attribute__((packed));
   uint8    track;
   uint8    point;
   uint8    min;
   uint8    sec;
   uint8    frame;
   uint8    zero;
   uint8    pmin;
   uint8    psec;
   uint8    pframe;

public:

   int32      GetTrackSession(void)
      {  return session;                                                };

   int32      GetTrackNumber(void)
      {  return point;                                                  };

   int32      GetTrackPositionLBA(void)
      {  return Transform::MsfToLba(pmin, psec, pframe);                };

   EDataType GetTrackType(void)
      {  return Transform::CtlToTrackType(adrctl.getCtl());             };

   int32      IsProtected(void)
      {  return Transform::CtlToProtection(adrctl.getCtl());            };

   int32      IsPreemphasized(void)
      {  return Transform::CtlToEmphasy(adrctl.getCtl());               };

   int32      GetTrackSectorSize(void)
      {  return Transform::TrackTypeToSectorSize(GetTrackType());       };

   uint8    GetMin()
      {  return min;                                                    };

   uint8    GetSec()
      {  return sec;                                                    };
};

struct TOC_PrimitiveEntry
{
private:
   char           reserved;
   struct _1 : protected aByte
   {
      uint8 getAdr() { return getField(4, 4); }
      uint8 getCtl() { return getField(0, 4); }
   } adrctl __attribute__((packed));
   uint8          track;
   char           reserved2;
   aLong          position;   

public:
   int32      GetTrackSession(void)
      {  return 1;                                                      };

   uint8    GetTrackNumber(void)
      {  return track;                                                  };

   int32      GetTrackPositionLBA(void)
      {  return position;                                               };

   EDataType GetTrackType(void)
      {  return Transform::CtlToTrackType(adrctl.getCtl());             };

   int32      IsProtected(void)
      {  return Transform::CtlToProtection(adrctl.getCtl());            };

   int32      IsPreemphasized(void)
      {  return Transform::CtlToEmphasy(adrctl.getCtl());               };

   int32      GetTrackSectorSize(void)
      {  return Transform::TrackTypeToSectorSize(GetTrackType());       };

   uint8    GetMin()
      {  return (position >> 16) & 0xff;                                };

   uint8    GetSec()
      {  return (position >> 8) & 0xff;                                 };
};

struct TOC_FullTOC 
{
   private:

   aWord       length;
   uint8       first_track;
   uint8       last_track;
   TOC_Entry   elem[0];

   public:

   int32         GetNumTracks(void);
   int32         GetNumElements(void)
      {  return (length-2)/11;                                          };
   int32         GetDiscSize(void);
   TOC_Entry  *FindTOCEntry(int32);
   void        FillInDiscItem(IOptItem*, int32);
};

struct TOC_PrimitiveTOC 
{
   private:

   aWord                length;
   uint8                first_track;
   uint8                last_track;
   TOC_PrimitiveEntry   elem[0];

   public:

   int32                  GetNumTracks(void);
   int32                  GetNumElements(void)
      {  return (length-2)/8;                               };
   int32                  GetDiscSize(void)
      {  return FindTOCEntry(0xAA)->GetTrackPositionLBA();  };

   TOC_PrimitiveEntry  *FindTOCEntry(int32);
   void                 FillInDiscItem(IOptItem*, int32);
};

struct TOC_PMA 
{
};

struct TOC_ATIP
{
   private:

   aWord    length;
   aWord    resvd0;

   struct _1 : protected aLong
   {
      uint8 getWritingPower()       { return getField(28, 4); }
      uint8 getReferenceSpeed()     { return getField(24, 3); }
      bool  isDiscUseUnrestricted() { return getField(22, 1); }
      bool  isRewritable()          { return getField(14, 1); }
      uint8 getDiscSubType()        { return getField(11, 3); }
      bool  isA1Valid()             { return getField(10, 1); }
      bool  isA2Valid()             { return getField(9,  1); }
      bool  isA3Valid()             { return getField(8,  1); }
   } conf;

   uint8    atip_leadin_start_m;
   uint8    atip_leadin_start_s;
   uint8    atip_leadin_start_f;
   uint8    resvd4;

   uint8    atip_leadout_start_m;
   uint8    atip_leadout_start_s;
   uint8    atip_leadout_start_f;
   uint8    resvd5;

   uint8    a1[3];
   uint8    resvd6;

   uint8    a2[3];
   uint8    resvd7;

   uint8    a3[3];
   uint8    resvd8;

   uint8    s4[3];
   uint8    resvd9;

   public:

   uint32   Length(void)                  { return length+2;                                                                                 };
   uint32   GetDiscSubType(void)          { return conf.getDiscSubType();                                                                    };
   uint32   GetLeadInPos(void)            { return Transform::MsfToLba(atip_leadin_start_m, atip_leadin_start_s, atip_leadin_start_f);       };
   void           GetLeadInPos(int8& m, int8& s, int8& f)
   { 
      m = atip_leadin_start_m;
      s = atip_leadin_start_s;
      f = atip_leadin_start_f;
   };

   uint32   GetLeadOutPos(void)           { return Transform::MsfToLba(atip_leadout_start_m, atip_leadout_start_s, atip_leadout_start_f);    };
   uint8          AtipLeadInM()                 { return atip_leadin_start_m; };
   uint8          AtipLeadInS()                 { return atip_leadin_start_s; };
   uint8          AtipLeadInF()                 { return atip_leadin_start_f; };
};

struct TOC_CDTextEntry 
{
   uint8    block_id;
   uint8    id[2];
   uint8    charpos[1];
   uint8    cdtextinfo[12];
   aWord    crc;
};

struct TOC_CDText 
{
   private:
   aWord             length;
   aWord             resvd0;
   TOC_CDTextEntry   items[0];

   TList<String*>   *MergeBlocks(int32);

   public:

   int32               NumCDTextBlocks(void)
      {  return (length-2)/sizeof(TOC_CDTextEntry);         };


   TList<String*>   *GetTitles(void);
   TList<String*>   *GetPerformers(void);
   TList<String*>   *GetSongWriters(void);
   TList<String*>   *GetComposers(void);
   TList<String*>   *GetArrangers(void);
   TList<String*>   *GetMessages(void);
};

struct SUB_Header
{
private:
   unsigned char     pad;
   unsigned char     audiostatus;
   aWord             datalen;
public:
   int32 Length() 
   { 
      return datalen+4; 
   }
};

struct SUB_Position : public SUB_Header
{
private:
   uint8             code;
   struct _1 : protected aByte
   {
      uint8 getAdr() { return getField(4, 4); }
      uint8 getCtl() { return getField(0, 4); }
   } adrctl __attribute__((packed));
   uint8             track;
   uint8             index;
   aLong             absolute_address;
   aLong             relative_address;

public:
   int32 getTrack()
   {
      return track;
   }

   int32 getIndex()
   {
      return index;
   }
 
   int32 getAddress()
   {
      return absolute_address;
   }  
};

struct SUB_MCN : public SUB_Header
{
private:
   unsigned char     code;
   unsigned char     pad1[3];
   struct _1 : protected aByte
   {
      bool isValid() { return getField(7, 1); }
   } valid __attribute__((packed));
   unsigned char     mcn[13];
   unsigned char     zero;
   unsigned char     frame;

public:
   const unsigned char *getMCN() 
   {
      if (valid.isValid())
         return (unsigned char*)&mcn;
      return 0;
   };
};

struct SUB_ISRC : public SUB_Header
{
private:
   unsigned char     code;
   struct _1 : protected aByte
   {
      uint8 getAdr() { return getField(4, 4); }
      uint8 getCtl() { return getField(0, 4); }
   } adrctl __attribute__((packed));
   unsigned char     track;
   unsigned char     pad1;
   struct _2 : protected aByte
   {
      bool isValid() { return getField(7, 1); }
   } valid __attribute__((packed));
   unsigned char     country[2];
   unsigned char     owner[3];
   unsigned char     year[2];
   unsigned char     serial[5];
   unsigned char     zero;
   unsigned char     frame;
   unsigned char     pad3;

public:
   bool isValid()
   {
      return valid.isValid();
   }

   const unsigned char *getCountry()
   {
      if (isValid())
         return (unsigned char*)&country;
      return 0;
   }

   const unsigned char *getOwner()
   {
      if (isValid())
         return (unsigned char*)&owner;
      return 0;
   }

   const unsigned char *getYear()
   {
      if (isValid())
         return (unsigned char*)&year;
      return 0;
   }

   const unsigned char *getSerial()
   {
      if (isValid())
         return (unsigned char*)&serial;
      return 0;
   }

};

struct SUB_Q
{
protected:
   uint8       raw_block[2352];

   struct _1 : protected aByte
   {
      uint8 getAdr() { return getField(4, 4); }
      uint8 getCtl() { return getField(0, 4); }
   } adrctl __attribute__((packed));
   uint8       track;
   uint8       index;
   uint8       rel_m;
   uint8       rel_s;
   uint8       rel_f;
   uint8       zero;
   uint8       abs_m;
   uint8       abs_s;
   uint8       abs_f;
   aWord       crc;
   uint32      pad;

public:
   uint32      getRelPos()
   {
      return (rel_m << 16) | (rel_s << 8) | (rel_f);
   }

   uint32      getAbsPos()
   {
      return (abs_m << 16) | (abs_s << 8) | (abs_f);
   }

   uint8       getCtl()
   {
      return adrctl.getCtl();
   }

   uint8       getAdr()
   {
      return adrctl.getAdr();
   }

   uint8       getTrack()
   {
      return (((track & 0xf0)>>4) * 10) + (track & 0xf);
   }

   uint8       getIndex()
   {
      return (((index & 0xf0)>>4) * 10) + (index & 0xf);
   }
};



class SCSICommand : public SCSICmd
{
   protected:
   unsigned char  cmd[16];
   class DriveIO *io;
   uint32         max_attempts;
   uint32         scsi_error;
   int32          optical_error;

   unsigned       need_probe:1;
   unsigned       dump_data:1;

   const char     *cmdname;

   public:

                   SCSICommand(DriveIO*);
   virtual        ~SCSICommand(void)
      {                                               };

   virtual  int32    Go(void);

   virtual  int32    onInit(void)
      {  return ODE_OK;                               };

   virtual  int32    onExit(void)
      {  return ODE_OK;                               };

   virtual  int32    onLoop(void)
      {  DOS->Delay(20);  return ODE_OK;              };

   virtual  int32    onProbe(int32 x, int32 y)
      {  return x ? ODE_CommandError: ODE_OK;         };

   inline   uint32  MaxAttempts(void)
      {  return max_attempts;                         };

   inline   int32    Cmd(int32 x)
      {  return cmd[x];                               };

   inline   uint8 *Cmd(void)
      {  return (uint8*)&cmd;                         };

   inline   uint32  CmdLength(void)
      {  return scsi_CmdLength;                       };

   inline   const char*  CmdName()
   {
      return cmdname;
   };

   inline   int32    NeedProbe(void)
      {  return need_probe;                           };

   inline   uint8* Data(void)
      {  return (uint8*)scsi_Data;                    };

   inline   int32    DataLength(void)
      {  return scsi_Length;                          };

   inline   uint32  SCSIError(void)
      {  return scsi_error;                           };

   inline   int32    OpticalError(void)
      {  return optical_error;                        };

   inline   int32    DebugDumpData(void)
      {  return dump_data;                            };
};

class cmd_ReadFormatCapacities : public SCSICommand
{
   struct _fmt_capacity
   {
      aLong    num_blocks;
      struct _1 : protected aLong
      {
         uint8  getType1() { return getField(26, 6); }
         uint8  getType2() { return getField(24, 2); }
         uint32 getParam() { return getField(0, 24); }
      } conf;
   };

   struct _readcaps {
      uint8                                     pad0[3];
      uint8                                     length;
      cmd_ReadFormatCapacities::_fmt_capacity   capacities[0];
   };


   _readcaps  *caps;

   public:

    cmd_ReadFormatCapacities(DriveIO*);
   ~cmd_ReadFormatCapacities(void);
   int32                                    onInit(void);
   int32                                    onProbe(int32, int32);
   int32                                    onExit(void);

   int32                                    IsFormatted(void);
   int32                                    GetMaxCapacity(void);
   int32                                    GetMaxPacketLength(void);
};

class cmd_TestUnitReady : public SCSICommand
{
   public:

                  cmd_TestUnitReady(DriveIO*);
   virtual       ~cmd_TestUnitReady() { };
   int32            onInit(void);
};

class cmd_Reset: public SCSICommand
{
   public:

                  cmd_Reset(DriveIO*);
   virtual       ~cmd_Reset() { };
   int32            onInit(void);
};

class cmd_Inquiry : public SCSICommand
{

   struct _inquiry
   {
      struct _1 : public aLong
      {
         uint8 getQualifier()          { return getField(13, 3); }
         uint8 getDeviceType()         { return getField(8, 5);  }
         bool  isRemovable()           { return getField(7, 1);  }
         bool  getVersion()            { return getField(8,  8); }
         bool  isAsyncEventReporting() { return getField(7,  1); }
         bool  isNormalACA()           { return getField(5,  1); }
         bool  isHierarchical()        { return getField(4,  1); }
         uint8 getRespDataFormat()     { return getField(0,  4); }
      } flags;

      struct _3 : public aLong
      {
         uint8 getAdditionalLength()   { return getField(24, 8); }
         bool  isStorageArray()        { return getField(23, 1); }
         bool  doesThirdPartyCommands(){ return getField(22, 1); }
         bool  doesBasicQueuing()      { return getField(15, 1); }
         bool  doesEnclosureServices() { return getField(14, 1); }
         bool  isVendorSpecific()      { return getField(13, 1); }
         bool  isMultiPort()           { return getField(12, 1); }
         bool  isMediumChanger()       { return getField(11, 1); }
         bool  doesRelativeAddressing(){ return getField(7,  1); }
         bool  isWideBus()             { return getField(5,  1); }
         bool  isSynchronous()         { return getField(4,  1); }
         bool  doesLinkedCommands()    { return getField(3,  1); }
         bool  doesCommandQueuing()    { return getField(1,  1); }
         bool  isVendorSpecific2()     { return getField(0,  1); }
      } features;

      char        vendor_id[8];
      char        product_id[16];
      char        product_version[4];
      char        firmware_version[20];
      char        reserved2[2];
      aWord       version_descriptors[8];
      aWord       reserved3[11];
      uint8       vendor_specific4[1];
   };

   _inquiry    *inquiry_data;
   char        *vendor_id;
   char        *product_id;
   char        *product_version;
   char        *firmware_version;

   public:

   enum {
      Supports_None  = 0,
      Supports_CCS   = 2,
      Supports_SPC1,
      Supports_SPC2,
      Supports_SPC3
   };

   enum {
      Type_Disk = 0,
      Type_Tape,
      Type_Printer,
      Type_Processor,
      Type_WriteOnce,
      Type_Optical,
      Type_Scanner,
      Type_OpticalMemory,
      Type_MediumChanger,
      Type_Communications,
      Type_GraphicArts,
      Type_GraphicArts2,
      Type_StorageArrat,
      Type_EnclosureServices,
      Type_SimpleDisk,
      Type_OpticalCard,
      Type_Reserved,
      Type_ObjectAccess
   };

               cmd_Inquiry(DriveIO*);
   virtual    ~cmd_Inquiry(void);

   int32         onInit(void);
   int32         onExit(void);

   inline char *ProductID(void)
      {  return product_id;                              };

   inline char *ProductVersion(void)
      {  return product_version;                         };

   inline char *VendorID(void)
      {  return vendor_id;                               };

   inline int32   DriveType(void)
      {  return inquiry_data->flags.getDeviceType();     };

   inline int32   SupportedStandard(void)
      {  return inquiry_data->flags.getVersion();         };

   inline char *FirmwareVersion(void)
      {  return firmware_version;                        };
};

class cmd_Mode : public SCSICommand
{
protected:
   Page_Header      *ms;
   int32             page;
   unsigned          direction:1;

public:
   enum 
   {
      Id_Page_Write        = 0x05,
      Id_Page_Capabilities = 0x2a
   };

public:
                        cmd_Mode(DriveIO*);
   virtual             ~cmd_Mode();
   Page_Header         *GetPage(int32);
   int32                  SetPage(Page_Header*);
   int32                  onInit(void);
   int32                  onProbe(int32, int32);
   int32                  onExit(void);
};

class cmd_GetConfiguration : public SCSICommand
{

   struct _feature
   {
      aLong    length;
      aWord    resvd;
      aWord    current_profile;
      uint8    profiles[0];
   };


   _feature   *feature_data;
   uint32       read_media;
   uint32       write_media;

   public:

   enum FeatureId {
      Feature_ProfileList           =  0,
      Feature_Core                  =  1,
      Feature_Morphing              =  2,
      Feature_RemovableMedium       =  3,
      Feature_WriteProtect          =  4,

      Feature_RandomReadable        =  0x10,
      Feature_MultiRead             =  0x1d,
      Feature_CDRead                =  0x1e,
      Feature_DVDRead               =  0x1f,

      Feature_RandomWritable        =  0x20,
      Feature_StreamWritable        =  0x21,
      Feature_SectorErasable        =  0x22,
      Feature_Formattable           =  0x23,
      Feature_HWDefectManagement    =  0x24,
      Feature_WriteOnce             =  0x25,
      Feature_RestrictedOverwrite   =  0x26,
      Feature_CD_RW_CAV             =  0x27,
      Feature_CD_MRW                =  0x28,
      Feature_DefectReporting       =  0x29,
      Feature_DVD_PlusRW            =  0x2a,
      Feature_DVD_PlusR             =  0x2b,
      Feature_RigidOverwrite        =  0x2c,
      Feature_CD_TrackAtOnce        =  0x2d,
      Feature_CD_SessionAtOnce      =  0x2e,
      Feature_DVD_MinusR_RW_Write   =  0x2f,

      Feature_DD_CD_Read            =  0x30,
      Feature_DD_CD_R_Write         =  0x31,
      Feature_DD_CD_RW_Write        =  0x32,
      Feature_CD_RW_MediaWriteSupp  =  0x37,    // supported media types
      Feature_BD_R_PseudoOverwrite  =  0x38,
      Feature_DVD_PlusRW_DualLayer  =  0x3a,
      Feature_DVD_PlusR_DualLayer   =  0x3b,
      Feature_BD_Read               =  0x40,
      Feature_BD_Write              =  0x41,

      Feature_PowerManagement       =  0x100,
      Feature_SMART                 =  0x101,
      Feature_Changer               =  0x102,
      Feature_ExternalAudioPlay     =  0x103,
      Feature_MicrocodeUpgradable   =  0x104,
      Feature_Timeout               =  0x105,
      Feature_DVD_CSS               =  0x106,
      Feature_RealTimeStreaming     =  0x107,
      Feature_DriveSerialNumber     =  0x108,
      Feature_MediaSerialNumber     =  0x109,   // SPC3 - Command AB/01 - ServiceAction!!!
      Feature_DiscControlBlocks     =  0x10a,
      Feature_DVD_CPRM              =  0x10b,
      Feature_FirmwareInformation   =  0x10c
   };

   struct Feature
   {
   private:
      aWord    id;
      struct _1 : protected aByte
      {
         uint8 getVersion()   { return getField(2, 4); }
         bool  isPersistent() { return getField(1, 1); }
         bool  isCurrent()    { return getField(0, 1); }  
      } type __attribute__((packed));
      uint8    length;

   public:
      int32       IsCurrent(void)      {  return type.isCurrent();                     };
      FeatureId   GetId()              {  return (FeatureId)(int16)id;                 };
      Feature*    Next()               {  return (Feature*)(((uint)this)+length+4);  };
      int32       GetLength()          {  return length+4;                             };
      APTR        GetBody()            {  return &((uint8*)this)[4];                   };
      int32       GetBodyLength()      {  return length;                               };
   };

   struct feat_ProfileList : public Feature
   {
      struct feat_ProfileItem
      {
         aWord    profile;
         struct _1 : protected aWord
         {
            bool isCurrent() { return getField(8, 1); }
         } conf __attribute__((packed));
      };

      cmd_GetConfiguration::feat_ProfileList::feat_ProfileItem  items[0];
   };

   struct feat_DVDPlusWrite : public Feature
   {
      struct _1 : protected aLong
      {
         bool isWritable() { return getField(24, 1); }
      } conf __attribute__((packed));
   };

   struct feat_DVDMinusWrite : public Feature
   {
      struct _1 : protected aLong
      {
         bool doesBurnProof() { return getField(30, 1); }
         bool doesTestWrite() { return getField(26, 1); }
         bool isReWritable()  { return getField(25, 1); }
      } conf __attribute__((packed));
   };

   struct feat_CDWrite : public Feature
   {
      struct _1 : protected aLong
      {
         bool doesBurnProof() { return getField(30, 1); }
         bool doesTestWrite() { return getField(26, 1); }
         bool isReWritable()  { return getField(25, 1); }
      } conf __attribute__((packed));
   };

   struct feat_ControlBlocks : public Feature
   {
      uint32         features[0];
   };

                                      cmd_GetConfiguration(DriveIO*);
   virtual                              ~cmd_GetConfiguration(void);

   int32                                    onInit(void);
   int32                                    onProbe(int32, int32);
   int32                                    onExit(void);

   int32                                    GetCurrentProfile(void);
   struct cmd_GetConfiguration::Feature  *GetFeature(FeatureId);
   uint32                                  GetMediaReadSupport(void);
   uint32                                  GetMediaWriteSupport(void);

};

class cmd_ReadDiscInfo : public SCSICommand
{
   public:


   private:

   DiscInfo            *dinfo;

   public:

                        cmd_ReadDiscInfo(DriveIO*);
   virtual             ~cmd_ReadDiscInfo(void);
   int32                  onInit(void);
   int32                  onProbe(int32, int32);
   int32                  onExit(void);
   DiscInfo            *GetData(void)
      {  return dinfo;     }
   long                 GetLeadInLength();
};

class cmd_ReadTrackInfo : public SCSICommand
{
   private:

   TrackInfo        *tinfo;
   uint32             track_nr;

   public:

   enum
   {
      Track_Invisible = 0xffffffff
   };

                     cmd_ReadTrackInfo(DriveIO*);
   virtual          ~cmd_ReadTrackInfo(void);
   int32               onInit(void);
   int32               onProbe(int32, int32);
   int32               onExit(void);

   TrackInfo        *GetData(void)
      {  return tinfo;              };
   void              SelectTrack(uint32 x)
      {  track_nr = x;              };
};

class cmd_ReadTOC : public SCSICommand
{
   private:


   struct _toc_resp {
      private:
      aWord       length;
      aWord       pad[1];

      public:
      int32 Length(void)     { return length+2; };
   };


   _toc_resp        *toc;

   uint8             type;
   bool              wantmsf;

   public:

   enum {
      Type_TOC       =  0,
      Type_FullTOC   =  2,
      Type_PMA       =  3,
      Type_ATIP      =  4,
      Type_CDText    =  5
   };

                                          cmd_ReadTOC(DriveIO*);
   virtual                               ~cmd_ReadTOC(void);
   int32                                    onInit(void);
   int32                                    onProbe(int32, int32);
   int32                                    onExit(void);

   void                                   SelectType(uint32 x)
      {  type = x;                                             };

   TOC_FullTOC                           *GetFullTOC(bool msf=true)
      {  type = Type_FullTOC; wantmsf = msf; Go(); return (TOC_FullTOC*)toc;     };

   TOC_PrimitiveTOC                      *GetTOC(bool msf=true)
      {  type = Type_TOC; wantmsf = msf; Go(); return (TOC_PrimitiveTOC*)toc;    };

   TOC_PMA                               *GetPMA()
      {  type = Type_PMA; wantmsf = false; Go(); return (TOC_PMA*)toc;           };

   TOC_ATIP                              *GetATIP(void)
      {  type = Type_ATIP; wantmsf = false; Go(); return (TOC_ATIP*)toc;         };

   TOC_CDText                            *GetCDText(void)
      {  type = Type_CDText; wantmsf = false; Go(); return (TOC_CDText*)toc;     };

};

class cmd_ReadSubChannel : public SCSICommand
{
   SUB_Header       *hdr;
   int32               size;
   int32               type; 
   int32               track;
public:

   enum {
      Type_Position  =  1,
      Type_MCN       =  2,
      Type_ISRC      =  3
   };

                     cmd_ReadSubChannel(DriveIO*);
   int32               onInit(void);
   int32               onProbe(int32, int32);
   SUB_Position     *getPosition();
   SUB_MCN          *getMCN();
   SUB_ISRC         *getISRC(int32 track);   
   
};

class cmd_Seek : public SCSICommand
{
   uint32            sector;
public:
                     cmd_Seek(DriveIO*);
   int32               onInit();
   int32               seek(uint32 sector);
};

class cmd_Play : public SCSICommand
{
   int32             start;
   int32             end;
public:
                     cmd_Play(DriveIO*);
   int32               onInit();
   int32               play(int32 start, int32 end);
};

class cmd_Blank : public SCSICommand
{
   int32      type;
   uint32   num;
   bool     immed;

   public:

   enum BlankType {
      Blank_All = 0,
      Blank_Minimal,
      Blank_Track,
      Blank_UnreserveTrack,
      Blank_TrackTail,
      Blank_UncloseSession,
      Blank_Session
   };


                  cmd_Blank(DriveIO*);
   virtual       ~cmd_Blank() { };
   int32            onInit();
   void           setType(cmd_Blank::BlankType t, uint32 num);
   void           setImmediate(bool f);
};

class cmd_Format : public SCSICommand
{
   public:

   enum FormatType {
      Format_FullFormat             = 0x00,
      Format_SpareAreaExpansion     = 0x01,
      Format_ZoneReformat           = 0x04,
      Format_ZoneFormat             = 0x05,
      Format_CD_DVD_FullFormat      = 0x10,
      Format_CD_DVD_GrowSession     = 0x11,
      Format_CD_DVD_AddSession      = 0x12,
      Format_CD_DVD_QuickGrowSession= 0x13,
      Format_DVDM_QuickAddSession   = 0x14,
      Format_DVDM_QuickFormat       = 0x15,
      Format_MRW_Quick_Obsolete     = 0x20,
      Format_MRW_FullFormat         = 0x24,
      Format_DVDP_FullFormat        = 0x26
   };

   private:
///
   struct _fmt_data
   {
      uint8    resvd0;
      struct _1 : protected aByte
      {
         void setOptionsValid()  { setField(7, 1, 1); }
         void setDisablePrimary(){ setField(6, 1, 1); }
         void setCertificates()  { setField(5, 1, 1); }
         void setStopFormat()    { setField(4, 1, 1); }
         void setInitPattern()   { setField(3, 1, 1); }
         void setTryOut()        { setField(2, 1, 1); }
         void setImmediate()     { setField(1, 1, 1); }
         void setVendorSpecific(){ setField(0, 1, 1); }
      } flags __attribute__((packed));

      aWord    length;     // 8
      aLong    size;

      struct _2 : protected aLong
      {
         void setType(uint8 t)   { setField(26, 6, t); }
         void setParam(uint32 t) { setField(0, 24, t); }
      } type __attribute__((packed));
   };

   _fmt_data  *fd;
   FormatType  type;
   uint32       size;
///.
   public:

               cmd_Format(DriveIO*);
   virtual    ~cmd_Format();
   int32         onInit(void);
   void        setType(FormatType t, uint32 b, int32 param);
   void        setImmediate(int32);
};

class cmd_StartStopUnit : public SCSICommand
{
   public:

   enum StartStopType {
      StartStop_Stop    = 0,
      StartStop_Start,
      StartStop_Eject,
      StartStop_Load,
      StartStop_Idle    = 0x20,
      StartStop_Standby = 0x30,
      StartStop_Sleep   = 0x50
   };

          cmd_StartStopUnit(DriveIO*);
   virtual    ~cmd_StartStopUnit();
   int32         onInit(void);
   void        setType(StartStopType t);
};

class cmd_LockDrive : public SCSICommand
{
   public:
               cmd_LockDrive(DriveIO*);
   virtual    ~cmd_LockDrive();
   int32         onInit(void);
   void        setLocked(bool locked);
};

class cmd_ReadHeader : public SCSICommand
{
   public:


   private:

   struct _rhdata {
      uint8    type;
      uint8    resvd[3];
      aLong    lba;
   };

   struct _rhdata *d;
   uint32           lba;

   public:

         cmd_ReadHeader(DriveIO*);
   virtual             ~cmd_ReadHeader(void);
   int32                  onInit(void);
   EDataType            GetTrackType(uint32 lba);

};

class cmd_Read : public SCSICommand
{
   public:


   private:

   APTR        data;
   uint32       firstsector;
   uint16       numsectors;

   public:

         cmd_Read(DriveIO*);
   virtual             ~cmd_Read(void);
   int32                  onInit(void);
   int32                  ReadData(uint32, uint32, APTR);
};

class cmd_ReadCD : public SCSICommand
{
public:

   enum Flags
   {
      Flg_Sync       = 1,     // synchronization (12 bytes)
      Flg_4BHeader   = 2,     // 4byte header (4 bytes)
      Flg_8BHeader   = 4,     // 8byte header (8 bytes)
      Flg_12BHeader  = 8,     // 12byte header (12 bytes)
      Flg_UserData   = 16,    // user data (2048-2352 bytes)
      Flg_ECC        = 32,    // ECC / EDC data ,
      Flg_C2Error    = 64,    // C2 Errors (294 bytes)
      Flg_C2Block    = 128,   // C2 Errors + Block Error (296 bytes)
      Flg_SubRawPW   = 256,   // P-W RAW subchannel (96 bytes)
      Flg_SubQ       = 512,   // Formatted Q subchannel (16 bytes)
      Flg_SubRW      = 1024,  // Formatted R-W subchannel (96 bytes)

      Flg_AllSubQ    = Flg_Sync | Flg_12BHeader | Flg_UserData | Flg_ECC | Flg_SubQ,
      Flg_AllRawPW   = Flg_Sync | Flg_12BHeader | Flg_UserData | Flg_ECC | Flg_SubRawPW,
      Flg_AllRawRW   = Flg_Sync | Flg_12BHeader | Flg_UserData | Flg_ECC | Flg_SubRW,
   };

private:
   void    *data;
   uint32   firstsector;
   uint16   numsectors;
   uint16   secSize;
   uint8    dataspec;
   uint8    subchspec;   

public:

                        cmd_ReadCD(DriveIO*);
   virtual             ~cmd_ReadCD(void);
   int32                  onInit(void);
   int32                  readCD(int32 start, uint16 count, uint16 secsize, void* mem, uint32 flags);
};

class cmd_Write : public SCSICommand
{
   public:


   private:

   APTR        data;
   uint32       firstsector;
   uint16       numsectors;
   uint16       sectorsize;

   public:

         cmd_Write(DriveIO*);
   virtual             ~cmd_Write(void);
   int32                  onInit(void);
   int32                  WriteData(uint32, uint32, uint32, APTR);
};

class cmd_Close : public SCSICommand
{
   int32   type;
   uint32 num;
   int32   immed;

   public:

   enum CloseType {
      Close_CDR_Track                     = 1,
      Close_CDR_LastSession               = 2,     // nominal close

      Close_DVDMinusR_Track               = 1,
      Close_DVDMinusR_LastSession         = 2,
      Close_DVDMinusR_SpecialCase         = 3,     // for restricted overwrite disc

      Close_DVDPlusR_Track                = 1,
      Close_DVDPlusR_LastSession          = 2,
      Close_DVDPlusR_FinalizeCompatible   = 5,     // the 30mm radius on layer 0
      Close_DVDPlusR_FinalizeNominal      = 6,     // finalize in nominal way

      Close_DVDPlusRDL_Track              = 1,
      Close_DVDPlusRDL_LastSession        = 2,
      Close_DVDPlusRDL_FinalizeExtended   = 4,     // the 30mm radius on both layers
      Close_DVDPlusRDL_FinalizeCompatible = 5,     // the 30mm radius on layer 0
      Close_DVDPlusRDL_FinalizeNominal    = 6,     // finalize in nominal way

      Close_DVDPlusRW_DeIcing             = 0,
      Close_DVDPlusRW_FinalizeCompatible  = 2,
      Close_DVDPlusRW_FinalizeNominal     = 3,

      Close_CDMRW_StopFormat              = 2,
      Close_CDMRW_Finalize                = 6,

      Close_FlushBuffers                  = -1
   };


        cmd_Close(DriveIO*);
   virtual       ~cmd_Close() { };
   int32            onInit(void);
   void           setType(cmd_Close::CloseType t, uint32 num);
   void           setImmediate(int32 f);
};

class cmd_Reserve : public SCSICommand
{
   public:


   private:

   uint32       numsectors;


   public:

         cmd_Reserve(DriveIO*);
   virtual             ~cmd_Reserve(void);
   int32                  onInit(void);
   int32                  Reserve(uint32);
};

class cmd_SendDVDStructure : public SCSICommand
{
   public:


   private:

   int32      type;
   uint16   *data;


   public:

   enum StructureType
   {
      DVD_DateTime = 1,
      DVD_Calibration = DVD_DateTime,
   };


   cmd_SendDVDStructure(DriveIO*);
   virtual             ~cmd_SendDVDStructure(void);
   virtual int32          onInit(void);
   virtual int32          onExit(void);
   virtual int32          SendStructure(StructureType);
};

class cmd_ReadDVDStructure : public SCSICommand
{
public:
   enum StructureType
   {
      DVD_Physical   =  0,          // TODO: unimplemented, layer=layer#
      DVD_Copyright,                // TODO: unimplemented, layer=layer#
      DVD_DiscKey,                  // TODO: unimplemented,
      DVD_BurstCutArea,             // TODO: unimplemented,
      DVD_Manufacturer,             // layer=layer#
      DVD_CopyrightMgmt,            // TODO: unimplemented, address=lba
      DVD_MediaIdentifier,          // TODO: unimplemented,
      DVD_MediaKeyBlock,            // TODO: unimplemented, address=pack#
      DVD_DiscDefinitionStructure,  // TODO: unimplemented, [DVD-RAM]
      DVD_MediumStatus,             // TODO: unimplemented, [DVD-RAM]
      DVD_SpareAreaInformation,     // TODO: unimplemented,

      DVD_RecordMgmtDataLast = 0x0c,// TODO: unimplemented, address=start field#
      DVD_RecordMgmtData,           // TODO: unimplemented, address=start sector#
      DVD_PreRecordedLeadIn,        // TODO: unimplemented,
      DVD_UniqueDiscID,             // TODO: unimplemented,
      DVD_PhysicalFormatInfo,       // TODO: unimplemented, layer=layer#

      DVD_DiscControlBlocks = 0x30, // TODO: unimplemented, address=content descriptor

      DVD_WriteProtection = 0xC0,   // TODO: unimplemented,
   };

private:
   StructureType  type;
   DVD_Structure *data;
   uint32         addr;
   uint8          layer;

public:

                           cmd_ReadDVDStructure(DriveIO*);
   virtual                ~cmd_ReadDVDStructure();
   virtual int32             onInit();
   virtual int32             onProbe(int32 c, int32 s);
   virtual int32             onExit();
   virtual DVD_Structure  *ReadStructure(StructureType, uint32 lba, uint8 layer);
};

class cmd_SetSpeed : public SCSICommand
{
   public:

   private:

   uint16       read;
   uint16       write;

   public:
         cmd_SetSpeed(DriveIO*);
   virtual             ~cmd_SetSpeed(void);
   int32                  onInit(void);
   int32                  SetSpeed(uint16, uint16);
};

class cmd_SendCueSheet : public SCSICommand
{
   unsigned char     *pCue;
   unsigned short     lElements;
public:
                        cmd_SendCueSheet(DriveIO*);
   virtual             ~cmd_SendCueSheet(void);
   int32                  onInit(void);
   void                 SendCueSheet(unsigned char *pCue, int32 lElements);
};

class cmd_Calibrate : public SCSICommand
{
public:

                        cmd_Calibrate(DriveIO*);
   int32                  onInit();
};


#ifndef __mc68000
#pragma pack()
#endif
#endif

