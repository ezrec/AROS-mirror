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

#ifndef _CLNAME_H_
#define _CLNAME_H_

#include <Generic/String.h>

using namespace GenNS;

class ClName 
{
public:
   enum Level
   {
      ISOLevel_1    =  1,
      ISOLevel_2    =  2,
      ISOLevel_3    =  3
   };

protected:
   
   ClName::Level           aLevel;
   bool                    bRelaxed;
   String                  sFullName;
   String                  sFileName;
   String                  sFileExtension;
   unsigned short          lFileVersion;
   String                  sName;
   bool                    bChanged;
   bool                    bShortForm;
protected:
   virtual void            rebuild();

public:
                           ClName();
   virtual                ~ClName();
   
   virtual void            update();

   virtual void            setLevel(ClName::Level aLevel);
   virtual void            setShortForm(bool bForm);
   virtual ClName::Level   getLevel() const;
   virtual void            setRelaxed(bool bRelaxed);
   virtual bool            isRelaxed() const;
   virtual void            setFullName(String sFullName);
   virtual const String   &getFullName() const;
   virtual const char     *getName() const;
   virtual int16           getVersion() const;
   virtual void            setVersion(int16);
   virtual int             compareBase(const ClName* other) const;

   virtual                 operator const char*() const;
   
};

#endif //_ISONAME_H_
