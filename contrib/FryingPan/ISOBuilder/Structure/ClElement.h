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

#ifndef _CLELEMENT_H_
#define _CLELEMENT_H_

#include <Generic/String.h>
#include <ISOBuilder/RRStructures.h>
#include <Generic/VectorT.h>
#include <dos/dos.h>
#include <dos/exall.h>
#include <dos/datetime.h>
#include "ClName.h"
#include <Generic/Debug.h>

class ClName;
class ClRoot;
class ClDirectory;
class GenNS::DbgHandler;

class ClElement 
{
   friend class ClRoot;
   friend class ClDirectory;

public:
   virtual DbgHandler     *getDebug();

private:
   String                  sFullPath;
   ClDirectory            *pParent;          // parent element.
   ClRoot                 *pRoot;            // root element
   ClName                  pISOName;         // specifies the iso name, should be zero in most cases.
   String                  sJolietName;      // if not empty, joliet specific name
   struct DateStamp        hDateStamp;       // creation
   unsigned char           bProtection;      // protection flags
   unsigned char           bPad;             // i have no idea about that.
   unsigned long           lISOSize;         // for directories -> size of all children elements
   unsigned long           lISOPosition;     // position on disc // block #
   unsigned long           lJolietSize;      // no idea how to handle the files
   unsigned long           lJolietPosition;  // but i know how to do directories.

   bool                    bIsISO;           // true, when file includes in ISO part
   bool                    bIsJoliet;        // true, when file includes in JOLIET part
   bool                    bIsRR;            // true, when file appears in rockridge extension

   bool                    bParentIsISO;
   bool                    bParentIsJoliet;
   bool                    bParentIsRR;

   VectorT<RRExt*,8>       hExtensions;
   VectorT<RRExt*,2>       hContExtensions;

   RR_RR                   rr_rr;
   RR_PX                   rr_px;
   RR_NM                   rr_nm;
   RR_AS                   rr_as;
   RR_CE                   rr_ce;

protected:
   virtual bool               fWritePadSectors(const Hook*, unsigned long lCount);
   virtual void               buildISOName();

protected:
   // warning: mess with these *c*a*r*e*f*u*l*l*y*
   virtual unsigned long      getISOPosition() const;
   virtual void               setISOSize(unsigned long lSize);
   virtual void               setISOPosition(unsigned long lPosition);
   virtual unsigned long      getJolietPosition() const;
   virtual void               setJolietSize(unsigned long lSize);
   virtual void               setJolietPosition(unsigned long lPosition);

   virtual int                getRRExtensions(uint8* buffer);
   virtual int                getRRExtensionsSize() const;

   // and the other vital information
   virtual unsigned long      getISODirTableEntrySize() const;
   virtual unsigned long      getJolietDirTableEntrySize() const;
   virtual unsigned long      getRRContinuationSize() const;
   virtual unsigned long      buildISODirTableEntry(struct ISODirRecord* pRec);
   virtual unsigned long      buildJolietDirTableEntry(struct ISOWDirRecord* pRec);
   virtual unsigned long      buildRRContinuation(uint8 *pRec);

   virtual void               setParentISOEntry(bool bIsISO);
   virtual void               setParentJolietEntry(bool bIsJoliet);
   virtual void               setParentRockRidgeEntry(bool bIsRR);

   virtual bool               setISOLevel(ClName::Level lvl);
   virtual bool               setISORelaxed(bool state);
   virtual bool               isISORelaxed() const;
   virtual ClName::Level      getISOLevel() const;
   virtual void               arrangeISODirTable();
   virtual void               setRRContinuationLocation(uint32 block, uint32 offset);


public:
                              ClElement(ClRoot *pRoot, ClDirectory *pParent);
   virtual                   ~ClElement();

   virtual void               dispose();
  
   virtual bool               isDirectory()                                = 0;
   virtual const String&      getPath();
   virtual void               setPath(const String& sString);
   virtual bool               update();
   virtual const char        *getISOName() const;
   virtual const ClName      *getISONameStruct() const;
   virtual bool               isImported();
   
   virtual ClDirectory       *getParent();
   virtual ClRoot            *getRoot();

   // now the additional information available for both derived classes and UI (who knows..)
   virtual void               setDate(const struct DateStamp *pDS);
   virtual void               setProtection(unsigned char lProtection);
   virtual void               setNormalName(const char* sName);
   virtual void               setJolietName(const char* sName);
   virtual void               setComment(const char* sComment);

   virtual unsigned long      getJolietSize() const;
   virtual unsigned long      getISOSize() const;

   virtual void               setISOEntry(bool bIsISO);
   virtual void               setJolietEntry(bool bIsJoliet);
   virtual void               setRockRidgeEntry(bool bIsRR);

   virtual bool               isISOEntry() const;
   virtual bool               isJolietEntry() const;
   virtual bool               isRockRidgeEntry() const;

   virtual const struct DateStamp   &getDate() const;
   virtual unsigned char      getProtection() const;
   virtual const char        *getNormalName() const;
   virtual const char        *getJolietName() const;
   virtual const char        *getComment() const;

};

#endif //_ISOELEMENT_H_
