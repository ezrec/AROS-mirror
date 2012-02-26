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

#ifndef _OPTICAL_CFGCDINFO_
#define _OPTICAL_CFGCDINFO_

#include <Generic/ConfigParser.h>
#include <Generic/Vector.h>
#include "CfgIOMeas.h"

using namespace GenNS;

class CfgCDInfo 
{
protected:
   ConfigParser        *config;
   Vector<CfgIOMeas*>   qblankspeeds;     // quick blank
   Vector<CfgIOMeas*>   qformatspeeds;    // quick format
   Vector<CfgIOMeas*>   cblankspeeds;     // complete blank
   Vector<CfgIOMeas*>   cformatspeeds;    // complete format

public:
                        CfgCDInfo(ConfigParser *parent, const char *cls);
   virtual             ~CfgCDInfo();
   virtual CfgIOMeas   *getQBlankMeas(uint32 size, uint16 spd);
   virtual CfgIOMeas   *getQFormatMeas(uint32 size, uint16 spd);
   virtual CfgIOMeas   *getCBlankMeas(uint32 size, uint16 spd);
   virtual CfgIOMeas   *getCFormatMeas(uint32 size, uint16 spd);

   virtual void         onWrite();
};

#endif

