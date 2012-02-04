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

#include "Config.h"

   static const char*   Cfg_Optical       =  "Optical";
   static const char*   Cfg_Location1     =  "ENV:FryingPan/Optical.prefs";
   static const char*   Cfg_Location2     =  "ENVARC:FryingPan/Optical.prefs";
   static const char*   Cfg_HardwareCount =  "HWCount"; 

Config::Config()
{
   config = new Configuration(Cfg_Optical);
   config->readFile(Cfg_Location1);

   vendors = new CfgVendors(config);
   drivers = new CfgDrivers(config);
   iocodes = new CfgIOCodes(config);
   hardware.FreeOnDelete(true);
   String *s = 0;
   s = config->getElement()->GetAttributeValue(Cfg_HardwareCount);
   if (s != 0)
   {
      hwcount = s->ToLong();
   }
   else
   {
      config->getElement()->AddAttribute(Cfg_HardwareCount, "");
      hwcount = 0;
   }

   for (int i=0; i<hwcount; i++)
   {
      CfgHardware *hw = new CfgHardware(config, i);
      hardware << hw;
   }
}

Config::~Config()
{
   onWrite();

   if (NULL != iocodes)
      delete iocodes;
   if (NULL != drivers)
      delete drivers;
   if (NULL != vendors)
      delete vendors;
   hardware.Empty();

   delete config;
}
   
void Config::onWrite()
{
   sync.Acquire();
   // iocodes and vendors do not use onWrite because the data is static.
   for (int i=0; i<hardware.Count(); i++)
   {
      hardware[i]->onWrite();
   }
   drivers->onWrite();

   config->getElement()->FindAttribute(Cfg_HardwareCount)->SetValue(hwcount);
   config->writeFile(Cfg_Location1);
   config->writeFile(Cfg_Location2);

   sync.Release();
}

CfgDrivers *Config::Drivers()
{
   return drivers;
}

CfgVendors *Config::Vendors()
{
   return vendors;
}

CfgIOCodes *Config::IOCodes()
{
   return iocodes;
}

CfgHardware *Config::GetHardware(const char* id)
{
   CfgHardware *h;

   for (int i=0; i<hardware.Count(); i++)
   {
      if (hardware[i]->getDriveID() == id)
         return hardware[i];
   }

   h = new CfgHardware(config, hwcount++);
   h->setDriveID(id);
   hardware << h;

   return h;
}


