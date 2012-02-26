/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include "XMLDocument.h"
#include "String.h"

namespace GenNS
{
   class ConfigParser 
   {
   protected:
      String                  sElemName;
      ConfigParser           *pParent;
      XMLElement             *pElement;
      long                    lID;

   public:

      ConfigParser(ConfigParser* pParent, String sName, long lID=-1);
      virtual                ~ConfigParser(); 
      virtual XMLElement     *addChild(const char* sName);
      virtual const char     *getValue(const char* sName, const char* sDefault);
      virtual long            getValue(const char* sName, long lDefault);
      virtual void            setValue(const char* sName, const char* sValue);
      virtual void            setValue(const char* sName, long sValue);   

      XMLElement             *getElement();

      virtual void            locateElement();
   };
};

#endif /*CONFIGPARSER_H_*/
