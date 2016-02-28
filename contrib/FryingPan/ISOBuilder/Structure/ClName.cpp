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

#include <Generic/LibrarySpool.h>
#include "ClName.h"
#include "../Xlate.h"

ClName::ClName()
{
   bShortForm     = false;
   bChanged       = true;
   lFileVersion   = 1;
   sFileExtension = "";
   sFileName      = "";
   bRelaxed       = false;
   aLevel         = ISOLevel_1;
   sName          = "";   
}

ClName::~ClName()
{
}

void ClName::setLevel(ClName::Level aLevel)
{
   if (this->aLevel != aLevel)
   {
      this->aLevel = aLevel;
      bChanged = true;
   }
}

ClName::Level ClName::getLevel() const 
{
   return this->aLevel;
}

void ClName::setRelaxed(bool bRelaxed)
{
   if (this->bRelaxed != bRelaxed)
   {
      this->bRelaxed = bRelaxed;
      bChanged = true;
   }
}

bool ClName::isRelaxed() const
{
   return bRelaxed;
}

void ClName::setShortForm(bool bForm)
{
   if (bShortForm != bForm)
   {
      bShortForm = bForm;
      bChanged = true;
   }
}

void ClName::setFullName(String sFullName)
{
//   DPrintfA("Changing ISO Name: %s -> %s\n", (uint32)this->sFullName.Data(), (uint32)sFullName.Data());
   this->sFullName = sFullName;
   bChanged = true;
}
   
const String &ClName::getFullName() const
{
   return sFullName;
}

void ClName::rebuild()
{
   if (!bChanged)
      return;

   // 1. split to filename + extension
   int i=sFullName.Length();
   int fnlen = 0;
   int felen = 0;

//   DPrintfA("Filename Len: %ld, ", i);

   if (false == bShortForm)
   {
      while ((--i) > 0)
      {
         if (sFullName[i] == '.')
            break;
      }
   }
   
   if (i > 0)
   {
      fnlen = i;
      felen = sFullName.Length() - i - 1;
   }
   else
   {
      fnlen = sFullName.Length();
      felen = 0;
   }

   switch (aLevel)
   {
      case ISOLevel_1:
         if (fnlen > 8)
            fnlen = 8;
         if (felen > 3)
            felen = 3;
         break;
      case ISOLevel_2:
      case ISOLevel_3:
         if (fnlen > 32)
            fnlen = 32;
         if ((fnlen+felen)>35)
            felen = 35-fnlen;
         break;
   }
   
   sFileName      = sFullName.LeftString(fnlen);
   sFileExtension = sFullName.SubString(i+1, felen);
  
//   DPrintfA("Splitted %s (%ld) into %s (%ld) and %s (%ld)\n", (uint32)sFullName.Data(), sFullName.Length(), (uint32)sFileName.Data(), sFileName.Length(), (uint32)sFileExtension.Data(), sFileExtension.Length());

   if (!bRelaxed)
   {
      sFileName      = Xlate::dChars(sFileName);
      sFileExtension = Xlate::dChars(sFileExtension);
   }
   else
   {
      sFileName      = Xlate::rChars(sFileName);
      sFileExtension = Xlate::rChars(sFileExtension);
   }

//   DPrintfA("Splitted %s (%ld) into %s (%ld) and %s (%ld)\n", (uint32)sFullName.Data(), sFullName.Length(), (uint32)sFileName.Data(), sFileName.Length(), (uint32)sFileExtension.Data(), sFileExtension.Length());
   bChanged = false;

   if (bShortForm)
   {
      if (lFileVersion == 1)
         sName.FormatStr("%s", ARRAY((IPTR)sFileName.Data()));
      else
         sName.FormatStr("%s.%03ld", ARRAY((IPTR)sFileName.Data(), lFileVersion));
   }
   else
   {
      if (aLevel == ISOLevel_1)
      {
         sName.FormatStr("%s.%s;%ld", ARRAY((IPTR)sFileName.Data(), (IPTR)sFileExtension.Data(), lFileVersion));
      }
      else
      {
         if (sFileExtension.Length() > 0)
            sName = sFileName + "." + sFileExtension;
         else
            sName = sFileName;
      }
   }
}

void ClName::update()
{
   rebuild();
}

const char* ClName::getName() const
{
   const_cast<ClName*>(this)->update();
   return sName.Data();
}

ClName::operator const char* () const
{
   return getName();
}

int16 ClName::getVersion() const
{
   return lFileVersion;
}

void ClName::setVersion(int16 newv)
{
   if (lFileVersion == newv)
      return;

   lFileVersion = newv;
   bChanged = true; 
}

int ClName::compareBase(const ClName* other) const
{
   int res = sFileName.CompareIgnoreCase(other->sFileName);
   if (0 == res)
      return sFileExtension.CompareIgnoreCase(other->sFileExtension);
   return res;
}

