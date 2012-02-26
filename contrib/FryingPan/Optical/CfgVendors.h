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

#ifndef _OPTICAL_CFGVENDORS_H_
#define _OPTICAL_CFGVENDORS_H_

#include <Generic/ConfigParser.h>
#include <Generic/Vector.h>

using namespace GenNS;

class CfgVendors 
{
protected:
   struct Vendor
   {
      uint8    m;
      uint8    s;
      uint8    f;
      bool     valid;
      String   name;
   };

protected:
   ConfigParser     *config;
   Vector<Vendor*>   vendors;
   String            novendor;

public:
                     CfgVendors(ConfigParser *parent);
   virtual          ~CfgVendors();                     
   virtual String   &getVendor(uint8 m, uint8 s, uint8 f);
};

#endif

