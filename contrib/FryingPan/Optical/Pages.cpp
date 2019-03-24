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

int32 Page_Capabilities::GetMediaReadSupport(void)
{
   register int32 reads = DRT_Media_CD_ROM;

   if (rw.doesReadDVDRam())     reads |= DRT_Media_DVD_RAM;
   if (rw.doesReadDVDMinusR())  reads |= DRT_Media_DVD_MinusR;
   if (rw.doesReadDVDRom())     reads |= DRT_Media_DVD_ROM;
   if (rw.doesReadCDRW())       reads |= DRT_Media_CD_RW;
   if (rw.doesReadCDR())        reads |= DRT_Media_CD_R;

   return reads;
}

int32 Page_Capabilities::GetMediaWriteSupport(void)
{
   register int32 writes = 0;

   if (rw.doesWriteDVDRam())     writes |= DRT_Media_DVD_RAM;
   if (rw.doesWriteDVDMinusR())  writes |= DRT_Media_DVD_MinusR;
   if (rw.doesWriteCDRW())       writes |= DRT_Media_CD_RW;
   if (rw.doesWriteCDR())        writes |= DRT_Media_CD_R;
   return writes;
};

int32 Page_Capabilities::GetAudioFlags(void)
{
   register int32 data = 0;

   if (rw.isAudioAccurate())              data |= DRT_Can_Read_CDAudioAccurate;
   if (rw.isCDDASupported())              data |= DRT_Can_Read_CDAudio;
   if (mechanism.doesRWLeadIn())          data |= DRT_Can_Read_CDText;
   if (rw.doesPlayAudio())                data |= DRT_Can_Play_Audio;
   if (mechanism.doesChannelMute())       data |= DRT_Can_Do_SeparateMute;
   if (mechanism.doesChannelVolume())     data |= DRT_Can_Do_SeparateVolume;
   if (num_volume_levels)                 data |= DRT_Can_Do_VolumeChange;

   return data;
}

int32 Page_Capabilities::GetAudioVolumeLevels(void)
{
   return num_volume_levels;
}

UWORD Page_Capabilities::GetCurrentWriteSpeed(void)
{
   if ((unsigned)PageSize() >= OFFSETWITH(Page_Capabilities, current_write_speed))
   {
      return current_write_speed;
   } else {
      return current_write_speed_old;
   }
}

UWORD Page_Capabilities::GetCurrentReadSpeed(void)
{
   return current_read_speed;
};

UWORD Page_Capabilities::GetMaximumWriteSpeed(void)
{
   return max_write_speed;
};

UWORD Page_Capabilities::GetMaximumReadSpeed(void)
{
   return max_read_speed;
};

UWORD Page_Capabilities::GetBufferSize(void)
{
   return buffer_size;
};

DRT_Mechanism Page_Capabilities::GetMechanismType()
{
   switch (mechanism.getMechanismType())
   {
      case 0:  return DRT_Mechanism_Caddy;
      case 1:  return DRT_Mechanism_Tray;
      case 2:  return DRT_Mechanism_Popup;
      case 4:  return DRT_Mechanism_Magazine; 
      case 5:  return DRT_Mechanism_Changer;
      default: return DRT_Mechanism_Unknown;
   }
}

uint Page_Capabilities::GetCapabilities()
{
   unsigned long caps = 0;
   if (rw.doesReadMethod2())              caps |= DRT_Can_Read_Mode2;
   if (rw.doesMode2Form1())               caps |= DRT_Can_Read_Mode2Form1;
   if (rw.doesMode2Form2())               caps |= DRT_Can_Read_Mode2Form2;
   if (rw.doesMultisession())             caps |= DRT_Can_Read_MultiSession;
   if (rw.isCDDASupported())              caps |= DRT_Can_Read_CDAudio;
   if (rw.isAudioAccurate())              caps |= DRT_Can_Read_CDAudioAccurate;
   if (rw.doesReadISRC())                 caps |= DRT_Can_Read_ISRC;
   if (rw.doesReadUPC())                  caps |= DRT_Can_Read_MCN;
   if (rw.doesReadBarCode())              caps |= DRT_Can_Read_BarCode;
   if (rw.doesRWData())                   caps |= DRT_Can_Read_CDText; // this actually means subchannel
   if (mechanism.doesRWLeadIn())          caps |= DRT_Can_Write_CDText;
   if (rw.doesPlayAudio())                caps |= DRT_Can_Play_Audio;
   if (mechanism.doesChannelMute())       caps |= DRT_Can_Do_SeparateMute;
   if (mechanism.doesChannelVolume())     caps |= DRT_Can_Do_SeparateVolume;
   if (num_volume_levels)                 caps |= DRT_Can_Do_VolumeChange;
   if (rw.doesWriteTestMode())            caps |= DRT_Can_Do_TestMode;
   if (rw.doesBurnProof())                caps |= DRT_Can_Do_BurnProof;

   return caps;
}

int32 Page_Capabilities::GetSpeedDescriptorsCount()
{
   if ((unsigned)PageSize() >= OFFSET(Page_Capabilities, num_speed_performance_descriptors))
      return num_speed_performance_descriptors;

   return 0;
}

int32 Page_Capabilities::GetSpeedDescriptor(int32 i)
{
   if ((unsigned)PageSize() <= OFFSET(Page_Capabilities, speed_performance_descriptors[i]))
      return 0;
   if ((i >=0 ) && (i < GetSpeedDescriptorsCount()))
     return speed_performance_descriptors[i] & 0xffff;   // strip plextors cav / clv details (i guess)
   return 0; 
}


void Page_Write::SetWriteType(Page_Write::WriteType t)
{
   switch (t) {
      case WriteType_Packet:
	 SetLinkSize(7);
	 SetPacketSize(16);
	 SetTrackMode(TrackMode_Data_Incremental);
	 SetDataMode(DataMode_Mode1);
	 break;
      default:
	 SetLinkSize(0);
	 SetPacketSize(0);
	 break;
   }

   if (set1.getWriteType() != (uint32)t) {
      set1.setWriteType(t);
      SetModified();
   }
};

void Page_Write::SetPacketSize(ULONG s)
{
   if (packet_size != s) 
   {
      packet_size = s;
      SetModified();
   }
   if (s) 
   {
      if (!set1.isFixedPacket()) 
      {
	      set1.setFixedPacket(true);
	      SetModified();
      }
   } 
   else 
   {
      if (set1.isFixedPacket()) 
      {
	      set1.setFixedPacket(false);
         SetModified();
      }
   }
}

void Page_Write::SetLinkSize(UBYTE s)
{
   if (set1.getLinkSize() != s) 
   {
      set1.setLinkSize(s);
      SetModified();
   }

   if (s) 
   {
      if (!set1.isLinkSizeValid()) 
      {
         set1.setLinkSizeValid(true);
         SetModified();
      }
   } 
   else 
   {
      if (set1.isLinkSizeValid()) 
      {
	      set1.setLinkSizeValid(false);
         SetModified();
      }
   }
}

Page_Write::WriteType Page_Write::GetWriteType(void)
{
   return (Page_Write::WriteType)set1.getWriteType();
};

uint32 Page_Write::GetPacketSize(void)
{
   return packet_size;
}

Page_Write::DataMode Page_Write::GetDataMode(void)
{
   return (DataMode)set1.getDataType();
}

void Page_Write::SetDataMode(DataMode t)
{
   if (set1.getDataType() == (uint32)t) 
      return;
   set1.setDataType((uint32)t);
   SetModified();
}

Page_Write::TrackMode Page_Write::GetTrackMode(void)
{
   return (TrackMode)set1.getTrackMode();
}

void Page_Write::SetTrackMode(TrackMode t)
{
   if (set1.getTrackMode() == (uint32)t) 
      return;
   set1.setTrackMode((uint32)t);
   SetModified();
}

Page_Write::SessionFormat Page_Write::GetSessionFormat(void)
{
   return (Page_Write::SessionFormat)set2.getSessionFormat();
}

void Page_Write::SetSessionFormat(Page_Write::SessionFormat f)
{
   if (set2.getSessionFormat() == (UBYTE)f) 
      return;
   set2.setSessionFormat((UBYTE)f);
   SetModified();
}

void Page_Write::SetTestMode(int32 f)
{
   if ((set1.isTestMode()) && (f)) 
      return;
   if ((!set1.isTestMode()) && (!f)) 
      return;
   set1.setTestMode(f);
   SetModified();
}

int32 Page_Write::IsTestMode(void)
{
   return set1.isTestMode();
}

void Page_Write::SetMultisession(int32 f)
{
   if ((f != 0) && (set1.getMultisession() != 0)) 
      return;
   if ((!f) && (!set1.getMultisession())) 
      return;
   set1.setMultisession(f ? 3 : 0);
   SetModified();
}

int32 Page_Write::IsMultisession(void)
{
   return set1.getMultisession() ? 1 : 0;
}

