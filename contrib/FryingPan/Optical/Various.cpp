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
#include "Various.h"
#include <Generic/Generic.h>




int32 Transform::MsfToLba(int m, int s, int f)
{
   if (m < 90)
      return (m * 60 + (s - 2)) * 75 + f;
   else
      return (m * 60 + s) * 75 + f - 450150;
};

int32 Transform::MsfToLba(unsigned long msf)
{
   return MsfToLba((msf>>16)&255, (msf>>8)&255, msf&255);
};

EDataType Transform::CtlToTrackType(int ctl)
{
   switch (ctl &~3) {
      case 0:  return Data_Audio;
      case 4:  return Data_Mode1;
      case 8:  return Data_Audio;
      default: return Data_Unknown;
   }
};

unsigned long Transform::TrackTypeToSectorSize(int t)
{
   switch (t) {
      case Data_Unknown:      return 0;
      case Data_Audio:        return 2352;
      case Data_Mode1:        return 2048;
      case Data_Mode2:        return 2336;
      case Data_Mode2Form1:   return 2048;
      case Data_Mode2Form2:   return 2324;
      default:                return 0;
   }
};

int Transform::CtlToProtection(int ctl)
{
   return (ctl & 2) ? 0 : 1;  // this bit defines copy permission rather than protection!!!
}

int Transform::CtlToEmphasy(int ctl)
{
   if (CtlToTrackType(ctl) != Data_Audio) return 0;
   return (ctl & 1) ? 1 : 0;
};

