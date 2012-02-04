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

#ifndef _IELEMENT_H_
#define _IELEMENT_H_


#include <dos/dos.h>
#include <dos/datetime.h>

class String;
class IRoot;
class IDirectory;
class ClName;

class IElement
{
public:                                                                                   // this sucks
   virtual const ClName            *getISONameStruct() const                     = 0;

public:
   virtual bool                     isDirectory()                                = 0;     // true if entry can contain sub elements
   virtual const String&            getPath()                                    = 0;     // string, NOT TO BE OVERWRITTEN
   virtual bool                     update()                                     = 0;     // causes element recheck and rebuild
   virtual const char              *getISOName() const                           = 0;     // obtains ISO name for element
   virtual bool                     isImported()                                 = 0;     // true if element is incorporated from another session
   
   virtual IDirectory              *getParent()                                  = 0;     // get parent directory
   virtual IRoot                   *getRoot()                                    = 0;     // get root directory

   virtual void                     setDate(const struct DateStamp *pDS)         = 0;     // i won't comment on these
   virtual void                     setProtection(unsigned char lProtection)     = 0;     // it's pointless
   virtual void                     setNormalName(const char* sName)             = 0;
   virtual void                     setJolietName(const char* sName)             = 0;
   virtual void                     setComment(const char* sComment)             = 0;
   virtual const struct DateStamp  &getDate() const                              = 0;     // item date
   virtual unsigned char            getProtection() const                        = 0;     // item protection
   virtual const char              *getNormalName() const                        = 0;     // item NORMAL (original) name (will be used to construct RockRidge and ISO)
   virtual const char              *getJolietName() const                        = 0;     // item JOLIET name
   virtual const char              *getComment() const                           = 0;     // :)

   virtual unsigned long            getISOSize() const                           = 0;
   virtual unsigned long            getJolietSize() const                        = 0;

   virtual void                     setISOEntry(bool bIsISO)                     = 0;     // set to true if entry should be added to ISO root
   virtual void                     setJolietEntry(bool bIsJoliet)               = 0;     // set to true if entry should be added to joliet root
   virtual bool                     isISOEntry() const                           = 0;     // true if entry should be present in iso root
   virtual bool                     isJolietEntry() const                        = 0;     // true if entry should be present in joliet root

};

#endif //_IELEMENT_H_
