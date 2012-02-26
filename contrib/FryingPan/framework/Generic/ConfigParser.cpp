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

#include "ConfigParser.h"
using namespace GenNS;

ConfigParser::~ConfigParser()
{
}

ConfigParser::ConfigParser(ConfigParser *pParent, String sName, long lID)
{
   sElemName = sName;         // set element name
   this->pParent = pParent;   // set parent element
   pElement = 0;
   this->lID = lID;
   locateElement();
}

const char *ConfigParser::getValue(const char* sName, const char *sDefault)
{
   if (0 == pElement)
      return sDefault;
      
   XMLElement *pElem = pElement->FindChild(const_cast<char*>(sName));
   if (0 == pElem)
      return sDefault;

   return pElem->GetValue().Data();
}

long ConfigParser::getValue(const char* sName, long lDefault)
{
   if (0 == pElement)
      return lDefault;
      
   XMLElement *pElem = pElement->FindChild(const_cast<char*>(sName));
   if (0 == pElem)
      return lDefault;
      
   return pElem->GetValue().ToLong();
}

void ConfigParser::setValue(const char* sName, const char* sValue)
{
   if (0 == pElement)
      return;
            
   XMLElement *pElem = pElement->FindChild(const_cast<char*>(sName));
   if (0 == pElem)
   {
      pElem = pElement->AddChild(const_cast<char*>(sName), "");
   }

   pElem->SetValue(sValue);
}

void ConfigParser::setValue(const char* sName, long sValue)
{
   if (0 == pElement)
      return;
      
   XMLElement *pElem = pElement->FindChild(const_cast<char*>(sName));
   if (0 == pElem)
   {
      pElem = pElement->AddChild(const_cast<char*>(sName), "");
   }

   pElem->SetValue(sValue);
}

XMLElement *ConfigParser::addChild(const char* sElemName)
{
   if (0 == pElement)
      return 0;
            
   return pElement->AddChild(const_cast<char*>(sElemName), "");
}

void ConfigParser::locateElement()
{
   if (pParent != 0)
   {
      for (int i=0; i<pParent->pElement->GetChildrenCount(); i++)
      {
         XMLElement *pElem = pParent->pElement->GetChild(i);
         if (pElem->GetName().Equals(sElemName))
         {
            if (lID == -1)
            {
               pElement = pElem;
               return;
            }
            else
            {
               XMLAttribute *pAttr = pElem->FindAttribute("id");
               if (pAttr)
               {
                  if (pAttr->GetValue().ToLong() == lID)
                  {
                     pElement = pElem;
                     return;
                  }
               }
            }
         }
      }
      
      pElement = pParent->addChild(sElemName);
      if (-1 != lID)
      {
         pElement->AddAttribute("id", lID);
      }
   }
}

XMLElement *ConfigParser::getElement()
{
   return pElement;
}
