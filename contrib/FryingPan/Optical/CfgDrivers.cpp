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

#include "CfgDrivers.h"
   
static const char*   Cfg_Drivers       =  "Drivers";
static const char*   Cfg_Driver        =  "Driver";
static const char*   Cfg_DriverHarmful =  "Harmful";
static const char*   Cfg_DriverName    =  "Name";

CfgDrivers::CfgDrivers(ConfigParser *parent)
{
   XMLElement *xel;
   config = new ConfigParser(parent, Cfg_Drivers);

   xel = config->getElement();

   drivers.FreeOnDelete(true);

   for (int i=0; (xel!=0) && (i<xel->GetChildrenCount()); i++)
   {
      XMLElement *el = xel->GetChild(i);
      ASSERT (el != 0);
      if (el != 0)
      {
         String *h = el->GetAttributeValue(Cfg_DriverHarmful);
         String *n = el->GetAttributeValue(Cfg_DriverName);

         if ((h != 0) && (n != 0))
         {
            Driver *d = new Driver;
            d->driver   = *n;
            d->harmful  = (h->ToLong() != 0) ? true : false;

            drivers << d;  
         }
      }
   }
}

CfgDrivers::~CfgDrivers()
{
   delete config;
}

void CfgDrivers::onWrite()
{
   XMLElement *xel = config->getElement();

   while (xel->GetChildrenCount() > 0)
   {
      xel->RemChild(xel->GetChild(0));
   }

   for (int32 i=0; i<drivers.Count(); i++)
   {
      XMLElement *el = new XMLElement(Cfg_Driver, 0);
      el->AddAttribute(Cfg_DriverHarmful, drivers[i]->harmful ? 1 : 0);
      el->AddAttribute(Cfg_DriverName,    drivers[i]->driver);
      xel->AddChild(el);
   }
}

TriState CfgDrivers::isHarmful(const char *device)
{
   for (int32 i=0; i<drivers.Count(); i++)
   {
      if (drivers[i]->driver == device)
         return drivers[i]->harmful ? stTrue : stFalse;
   }
   return stUnknown;
}
   
void CfgDrivers::addDevice(const char *name, bool harmful)
{
   for (int32 i=0; i<drivers.Count(); i++)
   {
      if (drivers[i]->driver == name)
      {
         drivers[i]->harmful = harmful;
         return;
      }
   }

   Driver *n = new Driver;
   n->driver = name;
   n->harmful = harmful;
   drivers << n;
}

