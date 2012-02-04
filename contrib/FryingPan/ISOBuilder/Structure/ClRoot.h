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

#ifndef _CLROOT_H_
#define _CLROOT_H_

#include "ClDirectory.h"
#include "ClFile.h"
#include "ClName.h"
#include "../ISOStructures.h"

class ClRoot : public ClDirectory
{
public:
   enum Level
   {
      ISOLevel_1 = 1,
      ISOLevel_2,
      ISOLevel_3,
   };

protected:
   class DbgHandler                   *debug;

public:
   void                                setDebug(DbgHandler*);
   virtual DbgHandler                 *getDebug();

protected:
   unsigned long                       lFirstSector;
 
   VectorT<ClDirectory*>               hDirVec;
   VectorT<ClFile*>                    hFileVec;
   ISOPrimaryVolumeDescriptor          hPrimary;
   ISOSupplementaryVolumeDescriptor    hSupplementary;

   ClName                              hRootName;
   
   unsigned long                       lISOPathTablesSize;
   unsigned long                       lJolietPathTablesSize;
   unsigned long                       lPadSize;
   unsigned long                       lTotalSize;

   virtual unsigned long               getFilesSizeB();           // blocks
   virtual unsigned long               getISOPathTableSizeB();    // blocks
   virtual unsigned long               getISOPathTableSize();     // bytes
   virtual unsigned long               getJolietPathTableSizeB(); // blocks
   virtual unsigned long               getJolietPathTableSize();  // bytes

   virtual void                        setISOSize(unsigned long lSize);
   virtual void                        setISOPosition(unsigned long lPosition);
   virtual void                        setJolietSize(unsigned long lSize);
   virtual void                        setJolietPosition(unsigned long lPosition);

   virtual unsigned long               getJolietPathTableEntrySize() const;
   virtual unsigned long               getISOPathTableEntrySize() const; 
   virtual unsigned long               buildISOPathTableEntry(struct ISOPathRecord *pRec, bool bIsMSB);
   virtual unsigned long               buildJolietPathTableEntry(struct ISOPathRecord *pRec, bool bIsMSB);


public:
                                       ClRoot(DbgHandler *debug);
   virtual                            ~ClRoot();
   
   virtual ClName::Level               getISOLevel() const;
   virtual bool                        setISOLevel(ClName::Level aLevel);
   virtual bool                        isISORelaxed() const;
   virtual bool                        setISORelaxed(bool bRelaxed);

   virtual bool                        generate(const Hook *data);
   virtual void                        rebuild();
   virtual bool                        updateSizes();             // just internally

   virtual const VectorT<ClDirectory*>&getDirVector() const;
   virtual const VectorT<ClFile*>     &getFileVector() const;

   //
   //
   //

   virtual const char                 *getVolumeID();
   virtual const char                 *getVolumeSetID();
   virtual const char                 *getSystemID();
   virtual const char                 *getPreparerID();
   virtual const char                 *getPublisherID();
   virtual const char                 *getApplicationID();

   virtual void                        setVolumeID(const char* id);
   virtual void                        setVolumeSetID(const char* id);
   virtual void                        setSystemID(const char* id);
   virtual void                        setPreparerID(const char* id);
   virtual void                        setPublisherID(const char* id);
   virtual void                        setApplicationID(const char* id);

   virtual void                        setNormalName(const char*);
   virtual const char                 *getNormalName() const;

   virtual uint32                      getImageSize();
};

#endif //_ISOROOT_H_
