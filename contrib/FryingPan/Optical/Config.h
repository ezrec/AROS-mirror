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

#ifndef _OPTICAL_CONFIG_H_
#define _OPTICAL_CONFIG_H_

#include <Generic/Configuration.h>
#include <Generic/Vector.h>
#include "CfgDrivers.h"
#include "CfgVendors.h"
#include "CfgIOCodes.h"
#include "CfgCDInfo.h"
#include "CfgHardware.h"
#include <Generic/Synchronizer.h>

using namespace GenNS;

class Config 
{
protected:
   Configuration          *config;
   CfgDrivers             *drivers;
   CfgVendors             *vendors;
   CfgIOCodes             *iocodes;
   int32                   hwcount;
   Vector<CfgHardware*>    hardware;
   Synchronizer            sync;

public:
                           Config();
   virtual                ~Config();
   virtual CfgDrivers     *Drivers();
   virtual CfgVendors     *Vendors();
   virtual CfgIOCodes     *IOCodes();
   virtual CfgHardware    *GetHardware(const char *id);

   virtual void            onWrite();
};

#endif

