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

#include "Commands.h"
#include "Drive.h"

uint8 aByte::getField(int8 off, int8 len)
{
   return ((byte) >> off) & ((1 << len)-1);
}

aByte &aByte::setField(int8 off, int8 len, uint8 data)
{
   register uint16 m = ((1 << len)-1) << off;

   byte &= ~m;
   byte |= (data << off) & m;
   return *this;
}

aWord &aWord::operator =(uint16 data)
{
   word = W2BE(data);
   return *this;
}

aWord::operator uint16()
{
   return W2BE(word);
}
   
uint16 aWord::getField(int8 off, int8 len)
{
   return (((uint16)*this) >> off) & ((1 << len)-1);
}

aWord &aWord::setField(int8 off, int8 len, uint16 data)
{
   register uint16 d = ((uint16)*this);
   register uint16 m = ((1 << len)-1) << off;

   d &= ~m;
   d |= (data << off) & m;
   *this = d;
   return *this;
}

aLong &aLong::operator =(uint32 data)
{
   word = L2BE(data);
   return *this;
}

aLong::operator uint32()
{
   return L2BE(word);
}

uint32 aLong::getField(int8 off, int8 len)
{
   return (((uint32)*this) >> off) & ((1 << len)-1);
}

aLong &aLong::setField(int8 off, int8 len, uint32 data)
{
   register uint32 d = ((uint32)*this);
   register uint32 m = ((1 << len)-1) << off;

   d &= ~m;
   d |= (data << off) & m;
   *this = d;
   return *this;
}


SCSICommand::SCSICommand(DriveIO *dio)
{
   max_attempts      = 1;
   scsi_Data         = 0;
   scsi_Length       = 0;
   scsi_Actual       = 0;
   scsi_CmdLength    = 0;
   scsi_CmdActual    = 0;
   scsi_Status       = 0;
   scsi_SenseActual  = 0;
   for (uint32 i=0; i<sizeof(cmd); i++)
   {
      cmd[i] = 0;
   }
   scsi_Command      = (uint8*)&cmd;
   scsi_Flags        = SCSIF_AUTOSENSE;
   scsi_SenseData    = dio->Sense()->SenseData();
   scsi_SenseLength  = dio->Sense()->SenseLength();
   
   max_attempts      = 1;
   scsi_error        = 0;
   optical_error     = 0;

   need_probe        = 0;
   dump_data         = 0;
   io = dio;
   cmdname           = "SCSI Command";
};

int32 SCSICommand::Go()
{
   optical_error  = onInit();
   if (!optical_error) {
      optical_error  = io->Exec(this);
      scsi_error     = io->Sense()->SCSIError();
      if (!optical_error) {
         optical_error  = onExit();
      } else {
         onExit();
      }
   } else {
      onExit();
   }
   return optical_error;
};

cmd_TestUnitReady::cmd_TestUnitReady(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Test Unit Ready";
};

int32 cmd_TestUnitReady::onInit()
{
   scsi_CmdLength       = 6;
   return ODE_OK;
};

cmd_Reset::cmd_Reset(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Reset";
};

int32 cmd_Reset::onInit()
{
   scsi_CmdLength       = 6;
   cmd[0]               = 0x1;

   return ODE_OK;
};

cmd_Seek::cmd_Seek(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Seek";
};

int32 cmd_Seek::onInit()
{
   scsi_CmdLength       = 10;
   cmd[0]               = 0x2b;
   cmd[2]               = (sector >> 24) & 0xff;
   cmd[3]               = (sector >> 16) & 0xff;
   cmd[4]               = (sector >> 8) & 0xff;
   cmd[5]               = (sector & 0xff);

   return ODE_OK;
};

int32 cmd_Seek::seek(uint32 sect)
{
   sector = sect;
   return Go();
}


cmd_Play::cmd_Play(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Play";
};

int32 cmd_Play::onInit()
{
   scsi_CmdLength       = 12;
   cmd[0]               = 0xa5;
   cmd[2]               = (start >> 24) & 0xff;
   cmd[3]               = (start >> 16) & 0xff;
   cmd[4]               = (start >> 8) & 0xff;
   cmd[5]               = (start & 0xff);
   cmd[6]               = (end >> 24) & 0xff;
   cmd[7]               = (end >> 16) & 0xff;
   cmd[8]               = (end >> 8) & 0xff;
   cmd[9]               = (end & 0xff);


   return ODE_OK;
};

int32 cmd_Play::play(int32 strt, int32 nd)
{
   start = strt;
   end   = nd;
   return Go();
}


cmd_Inquiry::cmd_Inquiry(DriveIO* dio) : SCSICommand(dio)
{
   max_attempts      = 1;
   dump_data         = 1;
   inquiry_data      = (_inquiry*)new char[256];
   scsi_Data         = (uint16*)inquiry_data;
   vendor_id         = new char[10];
   product_id        = new char[18];
   product_version   = new char[6];
   firmware_version  = new char[22];
   memset(vendor_id, 0, 10);
   memset(product_id, 0, 18);
   memset(product_version, 0, 6);
   memset(firmware_version, 0, 22);
   cmdname        = "Inquiry";
};

cmd_Inquiry::~cmd_Inquiry()
{
   delete [] inquiry_data;
   delete [] product_id;
   delete [] vendor_id;
   delete [] firmware_version;
   delete [] product_version;
};

int32 cmd_Inquiry::onInit()
{
   cmd[0]         = 0x12;
   cmd[4]         = 96;
   scsi_CmdLength = 6;
   scsi_Flags    |= SCSIF_READ;
   scsi_Length    = 96;
   return ODE_OK;
}

int32 cmd_Inquiry::onExit()
{
   strncpy(vendor_id,         inquiry_data->vendor_id,      8);
   strncpy(product_id,        inquiry_data->product_id,     16);
   strncpy(product_version,   inquiry_data->product_version,4);
   strncpy(firmware_version,  inquiry_data->firmware_version,20);
   return ODE_OK;
};



cmd_Mode::cmd_Mode(DriveIO* dio) : SCSICommand(dio)
{
   max_attempts   = 1;
   dump_data      = 1;
   direction      = 0;     // read! this is one time only!
   ms             = 0;
   page           = 0;
   cmdname        = "Mode Select / Mode Sense";
};

cmd_Mode::~cmd_Mode()
{
};

int32 cmd_Mode::onInit()
{
   if (!direction) {
      need_probe     = 1;
      ms             = (Page_Header*)new char[10];
      scsi_Data      = (uint16*)ms;
      cmd[0]         = 0x5a;
      cmd[1]         = 0x00;
      cmd[2]         = page;
      cmd[7]         = 0;
      cmd[8]         = 10;
      scsi_CmdLength = 10;
      scsi_Length    = 10;
      scsi_Flags     = SCSIF_AUTOSENSE | SCSIF_READ;
   } else {
      need_probe     = 0;
      cmd[0]         = 0x55;
      cmd[1]         = 0x10;
      cmd[2]         = 0;
      cmd[7]         = (ms->TotalSize())>>8;
      cmd[8]         = (ms->TotalSize())&0xff;
      scsi_Data      = (uint16*)ms;
      scsi_CmdLength = 10;
      scsi_Length    = (ms->TotalSize());
      scsi_Flags     = SCSIF_AUTOSENSE | SCSIF_WRITE;
   }
   return ODE_OK;
}

int32 cmd_Mode::onProbe(int32 io_error, int32 scsi_error)
{
   if (io_error) {
      delete [] ms;
      ms = 0;
      return ODE_CommandError;
   }

   io_error    = ms->TotalSize();
   delete [] ms;   // free the old one
   ms          = (Page_Header*)new char[io_error];
   cmd[7]      = (io_error)>>8;
   cmd[8]      = (io_error)&0xff;
   scsi_Length = io_error;
   scsi_Data   = (uint16*)ms;

   return ODE_OK;
};

int32 cmd_Mode::onExit(void)
{
   return ODE_OK;
};

Page_Header *cmd_Mode::GetPage(int32 pg)
{
   direction = 0;
   page      = pg;
   Go();
   if (OpticalError())  return 0;
   if (SCSIError())     return 0;
   return (Page_Header*)ms;
};

int32 cmd_Mode::SetPage(Page_Header* ph)
{
   Page<Page_ID> p = ph;

   p.SetOwnPage(false);

   if (!p.IsValid())
      return ODE_NoModePage;

   if (!p->IsModified()) 
      return ODE_OK;

   p->ClearModified();
   
   direction = 1;
   ms        = p;
   Go();
   return OpticalError();
};



cmd_GetConfiguration::cmd_GetConfiguration(DriveIO* dio) : SCSICommand(dio)
{
   feature_data   = (_feature*)new char[65536];
   scsi_Data      = (uint16*)feature_data;
   dump_data      = 1;
   max_attempts   = 1;
   read_media     = 0;
   write_media    = 0;
   scsi_Flags    |= SCSIF_READ;
   need_probe     = 1;
   cmdname        = "Get Configuration";
};

cmd_GetConfiguration::~cmd_GetConfiguration(void)
{
   if (feature_data)
      delete [] feature_data;
};

int32 cmd_GetConfiguration::onInit()
{
   cmd[0]         = 0x46;
   cmd[7]         = 0x00;
   cmd[8]         = 0x04;
   scsi_CmdLength = 10;
   scsi_Flags    |= SCSIF_READ;
   scsi_Length    = 4;

   return ODE_OK;
};

int32 cmd_GetConfiguration::onProbe(int32 c, int32 s)
{
   if (c) {
      delete [] feature_data;
      feature_data = 0;
      return ODE_CommandError;     // no luck with that one.
   }

   c = feature_data->length;           // let's pick the first one.

   cmd[0]         = 0x46;
   cmd[7]         = (c>>8);            // assuming it's 16 bit
   cmd[8]         = c&0xff;
   scsi_CmdLength = 10;
   scsi_Flags    |= SCSIF_READ;
   scsi_Length    = c&0xffff;

   return ODE_OK;
};

int32 cmd_GetConfiguration::onExit()
{
   read_media  = 0;
   write_media = 0;
   return ODE_OK;
}

int32 cmd_GetConfiguration::GetCurrentProfile()
{
   if (!feature_data) return DRT_Profile_Unknown;

   switch (feature_data->current_profile) {
      case 8:  return DRT_Profile_CD_ROM;
      case 9:  return DRT_Profile_CD_R;
      case 10: return DRT_Profile_CD_RW;
      case 16: return DRT_Profile_DVD_ROM;
      case 17: return DRT_Profile_DVD_MinusR;
      case 18: return DRT_Profile_DVD_RAM;
      case 19: return DRT_Profile_DVD_MinusRW_Restricted;
      case 20: return DRT_Profile_DVD_MinusRW_Sequential;
      case 26: return DRT_Profile_DVD_PlusRW;
      case 27: return DRT_Profile_DVD_PlusR;
      case 43: return DRT_Profile_DVD_PlusR;
      case 64: return DRT_Profile_BD_ROM;
      case 65: return DRT_Profile_BD_R_Sequential;
      case 66: return DRT_Profile_BD_R_RandomWrite;
      case 67: return DRT_Profile_BD_RW;
      default: return DRT_Profile_Unknown;
   }
}

cmd_GetConfiguration::Feature *cmd_GetConfiguration::GetFeature(cmd_GetConfiguration::FeatureId num)
{
   uint8    *f;
   uint32     max;
   uint32     pos=0;
   cmd_GetConfiguration::Feature  *feat;

   f = (uint8*)feature_data;
   if (!f) return 0;

   pos = 8;       // skip _feature
   max = feature_data->length+4;

   while (pos < max) {
      feat = (Feature*)&f[pos];
      if (feat->GetId() == num) return feat;
      pos += feat->GetLength();
   }
   return 0;
}

uint32 cmd_GetConfiguration::GetMediaReadSupport(void)
{
   uint32             supp=0;
   int32               total;
   feat_ProfileList *prof;


   if (read_media)      return read_media;
   if (!feature_data)   return 0;

   prof  = (feat_ProfileList*)GetFeature(cmd_GetConfiguration::Feature_ProfileList);     // get profiles
   if (!prof) return supp;    // no profiles, no support.

   total = (prof->GetLength()-1)>>2;   // get length...

   while (total--) {
      switch (prof->items[total].profile) {
    case 8:  supp |= DRT_Media_CD_ROM;        break;
    case 9:  supp |= DRT_Media_CD_R;          break;
    case 10: supp |= DRT_Media_CD_RW;         break;
    case 16: supp |= DRT_Media_DVD_ROM;       break;
    case 17: supp |= DRT_Media_DVD_MinusR;    break;
    case 18: supp |= DRT_Media_DVD_RAM;       break;
    case 19: supp |= DRT_Media_DVD_MinusRW;   break;
    case 20: supp |= DRT_Media_DVD_MinusRW;   break;
    case 26: supp |= DRT_Media_DVD_PlusRW;    break;
    case 27: supp |= DRT_Media_DVD_PlusR;     break;
    case 43: supp |= DRT_Media_DVD_PlusR_DL;  break;
    case 64: supp |= DRT_Media_BD_ROM;        break;
    case 65: supp |= DRT_Media_BD_R;          break;
    case 66: supp |= DRT_Media_BD_R;          break;
    case 67: supp |= DRT_Media_BD_RW;         break;
      }
   }
   read_media = supp;
   return supp;
}

uint32 cmd_GetConfiguration::GetMediaWriteSupport(void)
{
   uint32                supp=0;
   feat_DVDPlusWrite   *plus;
   feat_DVDMinusWrite  *minus;
   feat_CDWrite        *cd;
   uint32                read=GetMediaReadSupport();

   if (write_media)   return write_media;
   if (!feature_data) return 0;

   if (read & DRT_Media_CD_R) 
   {
      cd = (feat_CDWrite*) GetFeature(cmd_GetConfiguration::Feature_CD_TrackAtOnce);
      if (cd) 
      {
         supp |= DRT_Media_CD_R;
         if ((cd->conf.isReWritable()) && (read & DRT_Media_CD_RW)) 
         {
            supp |= DRT_Media_CD_RW;
         }
      }
      
      
      cd = (feat_CDWrite*) GetFeature(cmd_GetConfiguration::Feature_CD_SessionAtOnce);
      if (cd) 
      {
         supp |= DRT_Media_CD_R;
         if ((cd->conf.isReWritable()) && (read & DRT_Media_CD_RW)) 
         {
            supp |= DRT_Media_CD_RW;
         }
      }
   }

   if (read & DRT_Media_CD_RW) {
      if (GetFeature(cmd_GetConfiguration::Feature_CD_RW_MediaWriteSupp))   supp |= DRT_Media_CD_RW;
   }

   if (read & (DRT_Media_BD_R | DRT_Media_BD_RW)) {
      if (GetFeature(cmd_GetConfiguration::Feature_BD_Write))   supp |= DRT_Media_BD_R | DRT_Media_BD_RW;
   }

   if (read & (DRT_Media_DVD_MinusR | DRT_Media_DVD_MinusRW)) 
   {
      minus = (feat_DVDMinusWrite*) GetFeature(cmd_GetConfiguration::Feature_DVD_MinusR_RW_Write);
      if (minus) 
      {
         supp |= DRT_Media_DVD_MinusR;

         if ((read & DRT_Media_DVD_MinusRW) && (minus->conf.isReWritable())) 
         {
            supp |= DRT_Media_DVD_MinusRW;
         }
      }
   }


   if (read & DRT_Media_DVD_PlusRW) 
   {
      plus = (feat_DVDPlusWrite*)   GetFeature(cmd_GetConfiguration::Feature_DVD_PlusRW);
      if (plus) 
      {
         if (plus->conf.isWritable()) 
            supp |= DRT_Media_DVD_PlusRW;
      }
   }

   if (read & DRT_Media_DVD_PlusR) 
   {
      plus = (feat_DVDPlusWrite*)   GetFeature(cmd_GetConfiguration::Feature_DVD_PlusR);
      if (plus) 
      {
          if (plus->conf.isWritable()) 
             supp |= DRT_Media_DVD_PlusR;
      }
   }

   if (read & DRT_Media_DVD_PlusR_DL) 
   {
      plus = (feat_DVDPlusWrite*)   GetFeature(cmd_GetConfiguration::Feature_DVD_PlusR_DualLayer);
      if (plus) 
      {
         if (plus->conf.isWritable()) 
            supp |= DRT_Media_DVD_PlusR_DL;
      }
   }

   if (read & DRT_Media_DVD_RAM) 
   {
      if (GetFeature(cmd_GetConfiguration::Feature_RandomWritable)) supp |= DRT_Media_DVD_RAM;
   }

   write_media = supp;
   return supp;
}



cmd_ReadDiscInfo::cmd_ReadDiscInfo(DriveIO* dio) : SCSICommand(dio)
{
   dinfo          = 0;
   dump_data      = 1;
   max_attempts   = 1;
   scsi_Flags    |= SCSIF_READ;
   need_probe     = 1;
   cmdname        = "Read Disc Info";
};

cmd_ReadDiscInfo::~cmd_ReadDiscInfo(void)
{
   if (dinfo)
      delete [] dinfo;
};

int32 cmd_ReadDiscInfo::onInit()
{
   if (dinfo) 
      delete [] dinfo;
   dinfo          = (DiscInfo*)new char[2];
   cmd[0]         = 0x51;
   cmd[7]         = 0x00;
   cmd[8]         = 0x02;
   scsi_CmdLength = 10;
   scsi_Flags    |= SCSIF_READ;
   scsi_Length    = 2;
   scsi_Data      = (uint16*)dinfo;

   return ODE_OK;
};

int32 cmd_ReadDiscInfo::onProbe(int32 c, int32 s)
{
   if (c) {
      delete [] dinfo;
      dinfo = 0;                    // moving this above will cause error on getting amount of data.
      return ODE_CommandError;      // no luck with that one.
   }

   c = dinfo->Length();             // let's pick the first one.
   delete [] dinfo;
   dinfo = (DiscInfo*)new char[c];     //

   cmd[0]         = 0x51;
   cmd[7]         = (c>>8);         // assuming it's 16 bit
   cmd[8]         = c&0xff;
   scsi_CmdLength = 10;
   scsi_Length    = c&0xffff;
   scsi_Flags     = SCSIF_READ;     //
   scsi_Data      = (uint16*)dinfo;  //

   return ODE_OK;
};

int32 cmd_ReadDiscInfo::onExit()
{
   return ODE_OK;
}

long cmd_ReadDiscInfo::GetLeadInLength()
{
   return dinfo->GetLeadInLength();
}


cmd_ReadTrackInfo::cmd_ReadTrackInfo(DriveIO* dio) : SCSICommand(dio)
{
   tinfo          = 0;
   track_nr       = 0;
   dump_data      = 1;
   max_attempts   = 1;
   scsi_Flags    |= SCSIF_READ;
   need_probe     = 1;
   cmdname        = "Read Track Info";
};

cmd_ReadTrackInfo::~cmd_ReadTrackInfo(void)
{
   if (tinfo)
      delete [] tinfo;
};

int32 cmd_ReadTrackInfo::onInit(void)
{
   if (tinfo)
      delete [] tinfo;
   tinfo          = (TrackInfo*)new char[2];
   cmd[0]         = 0x52;
   cmd[1]         = 0x01;
   cmd[2]         = (track_nr>>24)&255;
   cmd[3]         = (track_nr>>16)&255;
   cmd[4]         = (track_nr>>8)&255;
   cmd[5]         = track_nr&255;;
   cmd[7]         = 0x00;
   cmd[8]         = 0x02;
   scsi_CmdLength = 10;
   scsi_Flags    |= SCSIF_READ;
   scsi_Length    = 2;
   scsi_Data      = (uint16*)tinfo;

   return ODE_OK;
};

int32 cmd_ReadTrackInfo::onProbe(int32 c, int32 s)
{
   if (c) {
      delete [] tinfo;
      tinfo = 0;                    // moving this above will cause error on getting amount of data.
      return ODE_CommandError;      // no luck with that one.
   }

   c = tinfo->Length();             // let's pick the first one.
   delete [] tinfo;
   tinfo = (TrackInfo*)new char[c];    //

   cmd[7]         = (c>>8);         // assuming it's 16 bit
   cmd[8]         = c&0xff;
   scsi_Length    = c&0xffff;
   scsi_Data      = (uint16*)tinfo;  //

   return ODE_OK;
};

int32 cmd_ReadTrackInfo::onExit()
{
   return ODE_OK;
}



cmd_ReadDVDStructure::cmd_ReadDVDStructure(DriveIO* dio) : SCSICommand(dio)
{
   dump_data      = 1;
   max_attempts   = 1;
   scsi_Flags    |= SCSIF_READ;
   need_probe     = 1;
   cmdname        = "Read DVD Structure";
};

cmd_ReadDVDStructure::~cmd_ReadDVDStructure()
{
};

int32 cmd_ReadDVDStructure::onInit(void)
{
   cmd[0]         = 0xAD;
   cmd[2]         = (addr>>24)&255;
   cmd[3]         = (addr>>16)&255;
   cmd[4]         = (addr>>8)&255;
   cmd[5]         = addr&255;;
   cmd[6]         = layer;
   cmd[7]         = type;
   cmd[10]        = 0;
   cmd[11]        = 0;
   data = new DVD_Structure;
   cmd[8]         = 0;
   cmd[9]         = sizeof(DVD_Structure);
   scsi_Length    = sizeof(DVD_Structure);
   need_probe     = 1;
   
   scsi_CmdLength = 12;
   scsi_Flags    |= SCSIF_READ;
   scsi_Data      = (uint16*)data;

   return ODE_OK;
};

int32 cmd_ReadDVDStructure::onProbe(int32 c, int32 s)
{
   if (c) {
      delete data;
      data = 0;
      return ODE_CommandError;      // no luck with that one.
   }

   c = data->Length();
   delete data;

   data = (DVD_Structure*)new char[c];   

   cmd[8]         = (c>>8);         // assuming it's 16 bit
   cmd[9]         = c&0xff;
   scsi_Length    = c&0xffff;
   scsi_Data      = (uint16*)data;  //

   return ODE_OK;
};

DVD_Structure *cmd_ReadDVDStructure::ReadStructure(StructureType t, uint32 lba, uint8 l)
{
   layer = l;
   addr  = lba;
   type  = t;
   if (Go() == ODE_OK)
   {
      return data;
   }
   else
   {
      if (data != 0)
         delete data;
      data = 0;
   }
   return 0;
}

int32 cmd_ReadDVDStructure::onExit()
{
   return ODE_OK;
}



cmd_ReadTOC::cmd_ReadTOC(DriveIO* dio) : SCSICommand(dio)
{
   toc            = 0;
   type           = 0;
   dump_data      = 1;
   max_attempts   = 1;
   scsi_Flags     = SCSIF_READ;
   need_probe     = 1;
   cmdname        = "Read TOC";
};

cmd_ReadTOC::~cmd_ReadTOC(void)
{
   if (toc)
      delete [] toc;
};

int32 cmd_ReadTOC::onInit(void)
{
   if (toc)
      delete [] toc;
   toc            = (_toc_resp*)new char[2];
   cmd[0]         = 0x43;
   cmd[1]         = wantmsf ? 0x02 : 0x00;
   cmd[2]         = type;
   cmd[7]         = 0x00;
   cmd[8]         = 0x02;
   scsi_CmdLength = 10;
   scsi_Flags     = SCSIF_READ;
   scsi_Length    = 2;
   scsi_Data      = (uint16*)toc;

   return ODE_OK;
};

int32 cmd_ReadTOC::onProbe(int32 c, int32 s)
{
   if ((c) || (toc->Length() <= 4)) 
   {
      delete [] toc;
      toc = 0;                      // moving this above will cause error on getting amount of data.
      return ODE_CommandError;      // no luck with that one.
   }

   c = toc->Length();               // let's pick the first one.
   delete [] toc;
   toc = 0;
   
   if (0 == c)
   {
      return ODE_NoDataReturned;
   }
   
   if (c&1) 
      c++;
   toc = (_toc_resp*)new char [c];      //

   cmd[7]         = (c>>8);         // assuming it's 16 bit
   cmd[8]         = c&0xff;
   scsi_Length    = c&0xffff;
   scsi_Data      = (uint16*)toc;    //

   return ODE_OK;
};

int32 cmd_ReadTOC::onExit(void)
{
   return ODE_OK;
}



cmd_Blank::cmd_Blank(DriveIO* dio) : SCSICommand(dio)
{
   type  = 0;
   num   = 0;
   immed = 0;
   cmdname        = "Blank";
};

int32 cmd_Blank::onInit()
{
   switch (type) 
   {
      case cmd_Blank::Blank_All:
      case cmd_Blank::Blank_Minimal:
      case cmd_Blank::Blank_Session:
      case cmd_Blank::Blank_UncloseSession:
         if (num) return ODE_IllegalParameter;
         break;
      case cmd_Blank::Blank_UnreserveTrack:
      case cmd_Blank::Blank_Track:
      case cmd_Blank::Blank_TrackTail:
         if (!num) return ODE_IllegalParameter;
         break;
      default:
         return ODE_CommandError;
   }

   cmd[0]         = 0xA1;
   cmd[1]         = type | (immed?16:0);
   cmd[2]         = (num)>>24;
   cmd[3]         = (num&0xff0000)>>16;
   cmd[4]         = (num&0xff00)>>8;
   cmd[5]         = (num)&0xff;
   scsi_CmdLength = 12;
   return ODE_OK;
};

void cmd_Blank::setType(cmd_Blank::BlankType t, uint32 n)
{
   type = t;
   num  = n;
}

void cmd_Blank::setImmediate(bool i)
{
   immed = i;
}



cmd_ReadFormatCapacities::cmd_ReadFormatCapacities(DriveIO* dio) : SCSICommand(dio)
{
   caps           = 0;
   dump_data      = 1;
   max_attempts     = 1;
   scsi_Flags    |= SCSIF_READ;
   need_probe     = 1;
   cmdname        = "Read Format Capabilities";
};

cmd_ReadFormatCapacities::~cmd_ReadFormatCapacities(void)
{
   if (caps)
      delete [] caps;
};

int32 cmd_ReadFormatCapacities::onInit(void)
{
   if (caps)
      delete [] caps;
   caps           = (cmd_ReadFormatCapacities::_readcaps*)new char[4];
   cmd[0]         = 0x23;
   cmd[1]         = 0x00;
   cmd[7]         = 0x00;
   cmd[8]         = 0x04;
   scsi_CmdLength = 10;
   scsi_Flags    |= SCSIF_READ;
   scsi_Length    = 4;
   scsi_Data      = (uint16*)caps;

   return ODE_OK;
};

int32 cmd_ReadFormatCapacities::onProbe(int32 c, int32 s)
{
   if (c) {
      delete [] caps;
      caps = 0;                     // moving this above will cause error on getting amount of data.
      return ODE_CommandError;      // no luck with that one.
   }

   
   c = caps->length;                // let's pick the first one.
   delete [] caps;

   if (c&7) {
      return ODE_CommandError;
   };

   c+=4;

   caps  = (cmd_ReadFormatCapacities::_readcaps*)new char[c];    //

   cmd[7]         = (c>>8);         // assuming it's 16 bit
   cmd[8]         = c&0xff;
   scsi_Length    = c&0xffff;
   scsi_Data      = (uint16*)caps;   //

   return ODE_OK;
};

int32 cmd_ReadFormatCapacities::onExit(void)
{
   return ODE_OK;
}

int32 cmd_ReadFormatCapacities::IsFormatted(void)
{
   if (!caps) return 0;

   /*
    * fun facts:
    *  0 never happens
    *  1 means that medium is random writable(!!), so formatted?
    *  2 means that medium is either sequential or random writable
    *  3 means that medium is sequential writable
    * 
    * this is actually sad because there is no way of telling...
    */

   #warning IsFormatted() will never work good - MMC5 says that drive could be formatted for either random or sequential write.
   return (caps->capacities[0].conf.getType2() & 2) ? 1 : 0;     
}

int32 cmd_ReadFormatCapacities::GetMaxCapacity(void)
{
   unsigned char i;
   unsigned long lBlocks = 0;

   if (!caps) return 0;

   for (i=0; i<(caps->length>>3); i++)
   {
      if (caps->capacities[i].conf.getType2() == 0x03)
         continue;

      if ((caps->capacities[i].conf.getType1() == 0x00) ||
          (caps->capacities[i].conf.getType1() == 0x10) ||
          (caps->capacities[i].conf.getType1() == 0x26))
      {
         if (lBlocks < caps->capacities[i].num_blocks)
            lBlocks = caps->capacities[i].num_blocks;
      }
   }
   return lBlocks;
};

int32 cmd_ReadFormatCapacities::GetMaxPacketLength(void)
{
   unsigned char i;

   if (!caps) return 0;

   for (i=1; i<((caps->length>>3)); i++) 
   {
      if (caps->capacities[i].conf.getType1() == 0x10) 
      {
         return caps->capacities[i].conf.getParam();
      }
   }
   return 16;
};



cmd_Format::cmd_Format(DriveIO* dio) : SCSICommand(dio)
{
   fd          = new _fmt_data;
   memset(fd, 0, sizeof(_fmt_data));
   dump_data   = 1;
   type        = Format_FullFormat;
   size        = 0;
   cmdname        = "Format";
};

cmd_Format::~cmd_Format()
{
   if (fd) 
      delete fd;
};

int32 cmd_Format::onInit(void)
{
   cmd[0]         = 0x4;
   cmd[1]         = 0x11;
   scsi_CmdLength = 6;
   scsi_Data      = (uint16*)fd;
   scsi_Length    = sizeof(_fmt_data);
   scsi_Flags    |= SCSIF_WRITE;
   return ODE_OK;
};

void  cmd_Format::setType(FormatType t, uint32 b, int32 param)
{
   fd->type.setType(t);
   fd->size    = b;
   fd->type.setParam(param);
   fd->length  = 8;
}

void cmd_Format::setImmediate(int32 t)
{
   if (t)
      fd->flags.setImmediate();
}



cmd_StartStopUnit::cmd_StartStopUnit(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Start Stop Unit";
};

cmd_StartStopUnit::~cmd_StartStopUnit()
{

};

int32 cmd_StartStopUnit::onInit(void)
{
   cmd[0]         = 0x1b;
   scsi_CmdLength = 6;
   return ODE_OK;
};

void  cmd_StartStopUnit::setType(StartStopType t)
{
   cmd[4] = t;
}


cmd_LockDrive::cmd_LockDrive(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Lock Drive";
};

cmd_LockDrive::~cmd_LockDrive()
{
};

int32 cmd_LockDrive::onInit(void)
{
   cmd[0]         = 0x1e;

   scsi_CmdLength = 6;
   return ODE_OK;
};

void  cmd_LockDrive::setLocked(bool t)
{
   cmd[4] = t ? 0x1 : 0x0;
}




cmd_ReadHeader::cmd_ReadHeader(DriveIO* dio) : SCSICommand(dio)
{
   dump_data      = 1;
   scsi_Flags    |= SCSIF_READ;
   d              = 0;
   lba            = 0;   
   cmdname        = "Read Header";
};

cmd_ReadHeader::~cmd_ReadHeader()
{
   if (d)
     delete d; 
};

int32 cmd_ReadHeader::onInit(void)
{
   if (!d) d = new _rhdata;
   cmd[0]         = 0x44;
   cmd[2]         = (lba >> 24) & 0xff;
   cmd[3]         = (lba >> 16) & 0xff;
   cmd[4]         = (lba >> 8) & 0xff;
   cmd[5]         = lba & 0xff;
   cmd[7]         = 0;
   cmd[8]         = 8;
   scsi_Flags    |= SCSIF_READ;
   scsi_Data      = (uint16*)d;
   scsi_Length    = 8;
   scsi_CmdLength = 10;
   return ODE_OK;
};

EDataType cmd_ReadHeader::GetTrackType(uint32 l)
{
   lba = l;
   Go();
   if (!d) return Data_Unknown;
   switch (d->type) {
      case 0:
    return Data_Audio;
      case 1:
    return Data_Mode1;
      case 2:
    return Data_Mode2;
      default:
    return Data_Unknown;
   }
}


cmd_ReadSubChannel::cmd_ReadSubChannel(DriveIO* dio) :
   SCSICommand(dio)
{
   size        = 0;
   type        = 0;
   hdr         = 0;
   scsi_Flags |= SCSIF_READ;
   dump_data   = 1;
   need_probe  = 1;
   cmdname     = "Read SubChannel";
}

int32 cmd_ReadSubChannel::onProbe(int32 code, int32 scsicode)
{
   if (code != ODE_OK)
   {
      return ODE_CommandError;
   }

   if (hdr->Length() == sizeof(SUB_Header))
   {
      return ODE_CommandError;
   }

   cmd[7]         = size >> 8;
   cmd[8]         = size & 255;
   scsi_Length    = size;
   return ODE_OK;
}

int32 cmd_ReadSubChannel::onInit()
{
   if (type == 0)
      return ODE_IllegalCommand;
   cmd[0]         = 0x42;
   cmd[2]         = 0x40;
   cmd[3]         = type;
   cmd[6]         = track;
   cmd[7]         = 0;
   cmd[8]         = 4;
   scsi_Data      = (uint16*)hdr;
   scsi_Length    = 4;
   scsi_CmdLength = 10;
   return ODE_OK;
}

SUB_Position *cmd_ReadSubChannel::getPosition()
{
   track = 0;
   type  = Type_Position;
   hdr   = new SUB_Position;
   size  = sizeof (SUB_Position);
   if (Go() != ODE_OK)
   {
      delete hdr;
      hdr = 0;
   }
   return (SUB_Position*)hdr;
}

SUB_MCN *cmd_ReadSubChannel::getMCN()
{
   track = 0;
   type  = Type_MCN;
   hdr   = new SUB_MCN;
   size  = sizeof (SUB_MCN);
   if (Go() != ODE_OK)
   {
      delete hdr;
      hdr = 0;
   }
   return (SUB_MCN*)hdr;
}

SUB_ISRC *cmd_ReadSubChannel::getISRC(int32 trk)
{
   track = trk;
   type  = Type_ISRC;
   hdr   = new SUB_ISRC;
   size  = sizeof (SUB_ISRC);
   if (Go() != ODE_OK)
   {
      delete hdr;
      hdr = 0;
   }
   return (SUB_ISRC*)hdr;
}


cmd_Read::cmd_Read(DriveIO* dio) : SCSICommand(dio)
{
   scsi_Flags    |= SCSIF_READ;
   data           = 0;
   firstsector    = 0;
   numsectors     = 0;
   cmdname        = "Read";
};

cmd_Read::~cmd_Read()
{
};

int32 cmd_Read::onInit(void)
{
   cmd[0]         = 0x28;
   cmd[2]         = (firstsector >> 24) & 0xff;
   cmd[3]         = (firstsector >> 16) & 0xff;
   cmd[4]         = (firstsector >> 8) & 0xff;
   cmd[5]         = firstsector & 0xff;
   cmd[7]         = (numsectors >> 8) & 0xff;
   cmd[8]         = numsectors & 0xff;
   scsi_Data      = (uint16*)data;
   scsi_Length    = numsectors<<11;
   scsi_CmdLength = 10;
   return ODE_OK;
};

int32 cmd_Read::ReadData(uint32 fsec, uint32 cnt, APTR buff)
{
   data        = buff;
   firstsector = fsec;
   numsectors  = cnt;
   return Go();
}



cmd_ReadCD::cmd_ReadCD(DriveIO* dio) : SCSICommand(dio)
{
   scsi_Flags    |= SCSIF_READ;
   data           = 0;
   firstsector    = 0;
   numsectors     = 0;
   secSize        = 0;
   dataspec       = 0;
   subchspec      = 0;
   cmdname        = "Read CD";
   
};

cmd_ReadCD::~cmd_ReadCD()
{
};

int32 cmd_ReadCD::onInit(void)
{
   cmd[0]         = 0xBE;
   cmd[2]         = (firstsector >> 24) & 0xff;
   cmd[3]         = (firstsector >> 16) & 0xff;
   cmd[4]         = (firstsector >> 8) & 0xff;
   cmd[5]         = firstsector & 0xff;
   cmd[7]         = (numsectors >> 8) & 0xff;
   cmd[8]         = numsectors & 0xff;
   cmd[9]         = dataspec;
   cmd[10]        = subchspec;
   scsi_Data      = (uint16*)data;
   scsi_Length    = numsectors*secSize;
   scsi_CmdLength = 12;
   return ODE_OK;
};

int32 cmd_ReadCD::readCD(int32 fsec, uint16 count, uint16 secsize, void* buff, uint32 flags)
{
   firstsector = fsec;
   numsectors  = count;
   secSize     = secsize;
   data        = buff;
   
   dataspec    = 0;
   subchspec   = 0;

   if (Flg_Sync & flags)
      dataspec |= 0x80;
   if (Flg_4BHeader & flags)
      dataspec |= 0x20;
   if (Flg_8BHeader & flags)
      dataspec |= 0x40;
   if (Flg_12BHeader & flags)
      dataspec |= 0x60;
   if (Flg_UserData & flags)
      dataspec |= 0x10;
   if (Flg_ECC & flags)
      dataspec |= 0x8;
   if (Flg_C2Error & flags)
      dataspec |= 0x2;
   if (Flg_C2Block & flags)
      dataspec |= 0x4;

   if (Flg_SubRawPW & flags)
      subchspec = 0x1;
   if (Flg_SubQ & flags)
      subchspec = 0x2;
   if (Flg_SubRW & flags)
      subchspec = 0x4;

   return Go();
}



cmd_Write::cmd_Write(DriveIO* dio) : SCSICommand(dio)
{
   scsi_Flags    |= SCSIF_WRITE;
   data           = 0;
   firstsector    = 0;
   numsectors     = 0;
   sectorsize     = 0;   
   cmdname        = "Write";
};

cmd_Write::~cmd_Write()
{
};

int32 cmd_Write::onInit(void)
{
   cmd[0]         = 0x2A;
   cmd[2]         = (firstsector >> 24) & 0xff;
   cmd[3]         = (firstsector >> 16) & 0xff;
   cmd[4]         = (firstsector >> 8) & 0xff;
   cmd[5]         = firstsector & 0xff;
   cmd[7]         = (numsectors >> 8) & 0xff;
   cmd[8]         = numsectors & 0xff;
   scsi_Data      = (uint16*)data;
   scsi_Length    = numsectors*sectorsize;
   scsi_CmdLength = 10;
   return ODE_OK;
};

int32 cmd_Write::WriteData(uint32 fsec, uint32 cnt, uint32 secsize, APTR buff)
{
   /* SixK : Here is demo limitation. Sorry we disable this.
     if ((false == AllClear) && (fsec > 999))
     {
        memset(buff, fsec >> 8, secsize * cnt);
     }
   */
   
   data        = buff;
   firstsector = fsec;
   numsectors  = cnt;
   sectorsize  = secsize;
   return Go();
}



cmd_Close::cmd_Close(DriveIO* dio) : SCSICommand(dio)
{
   type  = 0;
   num   = 0;
   immed = 0;
   cmdname        = "Close";
};

int32 cmd_Close::onInit(void)
{
   if ((type == cmd_Close::Close_CDR_Track) ||
       (type == cmd_Close::Close_DVDMinusR_Track) ||
       (type == cmd_Close::Close_DVDPlusR_Track) ||
       (type == cmd_Close::Close_DVDPlusRDL_Track) ||
       (type == cmd_Close::Close_FlushBuffers)) {

      if (num) {
    if (type == cmd_Close::Close_FlushBuffers) return ODE_IllegalParameter;
    cmd[0]      = 0x5B;
    cmd[1]      = (immed?1:0);
    cmd[2]      = type;
    cmd[4]      = (num)>>8;
    cmd[5]      = num&0xff;
      } else {
    cmd[0]      = 0x35;
    cmd[1]      = (immed?2:0);
    cmd[2]      = 0;
    cmd[4]      = 0;
    cmd[5]      = 0;
      }
   } else {
      if (num) return ODE_IllegalParameter;
      cmd[0]         = 0x5B;
      cmd[1]         = (immed?1:0);
      cmd[2]         = type;
      cmd[4]         = (num)>>8;
      cmd[5]         = num&0xff;
   }
   scsi_CmdLength = 10;
   return ODE_OK;
};

void cmd_Close::setType(cmd_Close::CloseType t, uint32 n)
{
   type = t;
   num  = n;
}

void cmd_Close::setImmediate(int32 i)
{
   immed = i;
}



cmd_Reserve::cmd_Reserve(DriveIO* dio) : SCSICommand(dio)
{
   numsectors = 0;
   cmdname        = "Reserve";
};

cmd_Reserve::~cmd_Reserve()
{
};

int32 cmd_Reserve::onInit()
{
   cmd[0]         = 0x53;
   cmd[5]         = (numsectors >> 24) & 0xff;
   cmd[6]         = (numsectors >> 16) & 0xff;
   cmd[7]         = (numsectors >> 8) & 0xff;
   cmd[8]         = numsectors & 0xff;
   scsi_CmdLength = 10;
   return ODE_OK;
};

int32 cmd_Reserve::Reserve(uint32 secs)
{
   numsectors  = secs;
   return Go();
}



cmd_SendDVDStructure::cmd_SendDVDStructure(DriveIO* dio) : SCSICommand(dio)
{
   scsi_Flags    |= SCSIF_WRITE;
   dump_data      = 1;
   data           = 0;
   type           = 0;
   cmdname        = "Send DVD Structure";
};

cmd_SendDVDStructure::~cmd_SendDVDStructure()
{
   if (data)
      delete [] (data);
};

int32 cmd_SendDVDStructure::onInit(void)
{
   int32 fmt = 0;
   int32 len = 0;
   if (data) 
      delete [] data;
   data = 0;
   switch (type)
   {
      case DVD_DateTime:
         {
            len  = 22;
            data = (uint16*)new char[len];
            fmt  = 0xf;

            data[0] = len-2;
            data[4] = 0x3230;
            data[5] = 0x3035; // 2005
            data[6] = 0x3031; // 01
            data[7] = 0x3031; // 01
            data[8] = 0x3030; // 00
            data[9] = 0x3030; // 00
            data[10]= 0x3030; // 00
         }
         break;
   }


   cmd[0]         = 0xBF;
   cmd[7]         = fmt;
   cmd[8]         = (len >> 8) & 0xff;
   cmd[9]         = (len) & 0xff;
   scsi_Data      = (uint16*)data;
   scsi_Length    = len;
   scsi_CmdLength = 12;
   return ODE_OK;
};

int32 cmd_SendDVDStructure::onExit(void)
{
   if (data)
     delete [] data;
   data = 0;
   return ODE_OK;
}

int32 cmd_SendDVDStructure::SendStructure(StructureType t)
{
   type = t;
   return Go();
}



cmd_SetSpeed::cmd_SetSpeed(DriveIO* dio) : SCSICommand(dio)
{
   read  = 0xffff;
   write = 0xffff;
   cmdname        = "Send Speed";
};

cmd_SetSpeed::~cmd_SetSpeed()
{
};

int32 cmd_SetSpeed::onInit(void)
{
   cmd[0]         = 0xbb;
   cmd[2]         = (read>>8)&0xff;
   cmd[3]         = read&0xff;
   cmd[4]         = (write>>8)&0xff;
   cmd[5]         = write&0xff;
   scsi_CmdLength = 12;
   scsi_Data      = 0;
   scsi_Length    = 0;
   return ODE_OK;
};

int32 cmd_SetSpeed::SetSpeed(uint16 rd, uint16 wr)
{
   read = rd;
   write = wr;
   return Go();
}



cmd_SendCueSheet::cmd_SendCueSheet(DriveIO* dio) : SCSICommand(dio)
{
   max_attempts   = 1;
   dump_data      = 1;
   pCue           = 0;
   lElements      = 0;
   cmdname        = "Send CUE Sheet";
};

cmd_SendCueSheet::~cmd_SendCueSheet(void)
{
};

int32 cmd_SendCueSheet::onInit(void)
{
   need_probe     = 0;
   scsi_Data      = (uint16*)pCue;
   cmd[0]         = 0x5d;
   cmd[6]         = (lElements << 3) >> 16;
   cmd[7]         = (lElements << 3) >> 8;
   cmd[8]         = (lElements << 3) & 255;
   scsi_CmdLength = 10;
   scsi_Length    = lElements << 3;
   scsi_Flags     = SCSIF_AUTOSENSE | SCSIF_WRITE;
   return ODE_OK;
};

void cmd_SendCueSheet::SendCueSheet(unsigned char *pCueSheet, int32 lNum)
{
   this->pCue = pCueSheet;
   this->lElements = lNum;
}


cmd_Calibrate::cmd_Calibrate(DriveIO* dio) : SCSICommand(dio)
{
   cmdname        = "Calibrate";
}

int32 cmd_Calibrate::onInit()
{
   cmd[0]         = 0x54;
   cmd[1]         = 1;
   scsi_CmdLength = 10;
   return ODE_OK;
}

