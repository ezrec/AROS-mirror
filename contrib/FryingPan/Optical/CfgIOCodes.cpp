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

#include "CfgIOCodes.h"
   
static const char*   Cfg_IOCodes       =  "IOCodes";
static const char*   Cfg_IOCodeCode    =  "Code";
static const char*   Cfg_IOCodeQual    =  "Qualifier";
static const char*   Cfg_IOCodeMessage =  "Message";

CfgIOCodes::CfgIOCodes(ConfigParser *parent)
{
   XMLElement *xel;
   config = new ConfigParser(parent, Cfg_IOCodes);

   xel = config->getElement();

   iocodes.FreeOnDelete(true);

   for (int i=0; (xel!=0) && (i<xel->GetChildrenCount()); i++)
   {
      XMLElement *el = xel->GetChild(i);
      ASSERT (el != 0);
      if (el != 0)
      {
         String *c = el->GetAttributeValue(Cfg_IOCodeCode);
         String *q = el->GetAttributeValue(Cfg_IOCodeQual);
         String *m = el->GetAttributeValue(Cfg_IOCodeMessage);

         if ((c != 0) && (q != 0) && (m != 0))
         {
            IOCode *ioc = new IOCode;
            ioc->message   = *m;
            ioc->code      = c->ToLong();
            ioc->qual      = q->ToLong();

            iocodes << ioc;
         }
      }
   }
}

CfgIOCodes::~CfgIOCodes()
{
   delete config;
}

