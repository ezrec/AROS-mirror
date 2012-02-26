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

#include "ClRoot.h"
#include <Generic/LibrarySpool.h>
#include <Generic/HookAttrT.h>
#include <Generic/Debug.h>

void ClRoot::setDebug(DbgHandler* n)
{
   debug = n;
}

DbgHandler *ClRoot::getDebug()
{
   return debug;
}

ClRoot::ClRoot(DbgHandler *dbg)
   : ClDirectory(this, this, "")
{
   setDebug(dbg);

   lFirstSector   = 0;
   setParentElementPathID(1);
   hRootName.setLevel(ClName::ISOLevel_3);
   hRootName.setRelaxed(true);
   hRootName.setShortForm(false);
   initialize("CD-ROM");
}

ClRoot::~ClRoot()
{
}
   
ClName::Level ClRoot::getISOLevel() const
{
   return (ClElement::getISOLevel());
}

bool ClRoot::setISOLevel(ClName::Level aLevel)
{
   return ClDirectory::setISOLevel(aLevel);
}

bool ClRoot::isISORelaxed() const
{
   return ClElement::isISORelaxed();
}

bool ClRoot::setISORelaxed(bool bRelaxed)
{
   return ClDirectory::setISORelaxed(bRelaxed);
}

unsigned long ClRoot::getFilesSizeB()
{
   return 0;
}

unsigned long ClRoot::getISOPathTableSizeB()
{
   if (!isISOEntry())
      return 0;
   if (!lISOPathTablesSize)
      return 1;
   return (lISOPathTablesSize + 2047) >> 11;
}

unsigned long ClRoot::getISOPathTableSize()
{
   if (!isISOEntry())
      return 0;
   return lISOPathTablesSize;
}

unsigned long ClRoot::getJolietPathTableSizeB()
{
   if (!isJolietEntry())
      return 0;
   if (!lJolietPathTablesSize)
      return 1;
   return (lJolietPathTablesSize + 2047) >> 11;
}

unsigned long ClRoot::getJolietPathTableSize()
{
   if (!isJolietEntry())
      return 0;
   return lJolietPathTablesSize;
}

void ClRoot::rebuild()
{
   update();
   _d(Lvl_Info, "Setting disc names to %s / %s", (int)getISOName(), (int)getJolietName());
   hPrimary.setVolumeID(hRootName.getName());
   hSupplementary.setVolumeID(getJolietName());
   ClDirectory::rebuild();
}

bool ClRoot::updateSizes()
{
   unsigned long     lSector  =  lFirstSector + 16;      // include pad data
   int               lCur;

   {  /* create two vectors: hDirVec contains flat dir tree, hFileVec contains all files */
      
      hDirVec.Empty();
      hFileVec.Empty();
   
      hDirVec << this;     // :) yea i finally got the idea :P
   
      for (lCur = 0; lCur < hDirVec.Count(); lCur++)
      {
         ClDirectory *pDir = hDirVec[lCur];
         ClDirectory *pElem;
         ClElement   *pEl;
     
         ASSERT(pDir != 0);
         for (int i=0; i<pDir->getChildrenCount(); i++)
         {
            pEl   = pDir->getChild(i);
        
            if (pEl->isDirectory())
            {
               pElem = static_cast<ClDirectory*>(pEl);
               pElem->setParentElementPathID(lCur+1);
               hDirVec << pElem;
            }
            else
            {
               hFileVec << static_cast<ClFile*>(pEl);
            }
         }         
      }
   }

   {  /* rebuild all directories so we have proper names */

      for (lCur = 0; lCur < hDirVec.Count(); lCur++)
      {
         _d(Lvl_Info, "--> Rebuilding directory %s", (int)hDirVec[lCur]->getNormalName());
         hDirVec[lCur]->rebuild();
      }
      
      _d(Lvl_Debug, "Rebuild complete");
   }

   {  /* as we have the names etc here, recalculate all structures */

      for (lCur = 0; lCur < hDirVec.Count(); lCur++)
      {
         _d(Lvl_Info, "--> Recalculating directory %s", (int)hDirVec[lCur]->getNormalName());
         hDirVec[lCur]->recalculate();
      }
      
      _d(Lvl_Debug, "Recalculation complete");
   }

   {  /* calculate path tables size */
      lISOPathTablesSize      = 0;
      lJolietPathTablesSize   = 0;
   
      for (lCur = 0; lCur < hDirVec.Count(); lCur++)
      {
         _d(Lvl_Info, "Calcing PT sizes for %s", (int)hDirVec[lCur]->getNormalName());
         lISOPathTablesSize      += hDirVec[lCur]->getISOPathTableEntrySize();
         lJolietPathTablesSize   += hDirVec[lCur]->getJolietPathTableEntrySize();
      }
   }
   
   /* we can start the finals now :) */

   {  /* root sectors: primary, supplementary and end descriptors */

      // if we are meant to include ISO root sector...
      if (isISOEntry())
      {
         ++lSector;
      }

      // or joliet root sector
      if (isJolietEntry())
      {
         ++lSector;
      }

      // and pad sector :)
      ++lSector;
   }

   {  /* path tables: two for iso, two for joliet */
      hPrimary.setLSBPathTablePosition(lSector);
      lSector += (lISOPathTablesSize    + 2047) >> 11;
      hPrimary.setMSBPathTablePosition(lSector);
      lSector += (lISOPathTablesSize    + 2047) >> 11;

      hSupplementary.setLSBPathTablePosition(lSector);
      lSector += (lJolietPathTablesSize + 2047) >> 11;
      hSupplementary.setMSBPathTablePosition(lSector);
      lSector += (lJolietPathTablesSize + 2047) >> 11;
   }

   {  /* relocate iso and joliet dir tables */
      for (lCur=0; lCur<hDirVec.Count(); lCur++)
      {
         hDirVec[lCur]->setISOPosition(lSector);
         lSector += hDirVec[lCur]->getISOSize() >> 11;
      }

      for (lCur=0; lCur<hDirVec.Count(); lCur++)
      {
         hDirVec[lCur]->setJolietPosition(lSector);
         lSector += hDirVec[lCur]->getJolietSize() >> 11;
      }
   }

   {  /* relocate all files */
      for (lCur=0; lCur<hFileVec.Count(); lCur++)
      {
         hFileVec[lCur]->setISOPosition(lSector);
         if (hFileVec[lCur]->getISOSize() > 0)
            lSector += (hFileVec[lCur]->getISOSize()+2047) >> 11;
         else
            lSector ++;
      }
   }

   {  /* mark image size & finalize */
      lPadSize   = lSector;
      lTotalSize = (lSector + 18) &~ 15;     // round up and add extra padding (at least 3 sectors)
      lPadSize   = lTotalSize - lPadSize;
      hPrimary.setVolumeSize(lTotalSize);
      hPrimary.setPathTableSize(lISOPathTablesSize);
      hPrimary.setDate(&getDate());

      hSupplementary.setVolumeSize(lTotalSize);
      hSupplementary.setPathTableSize(lJolietPathTablesSize);
      hSupplementary.setDate(&getDate());
   }

   _d(Lvl_Debug, "Estimated sizes:\n"
            "- ISO Path Tables Size:         %ld bytes\n"
            "- Joliet Path Tables Size:      %ld bytes\n"
            "- ISO Dir Tables Size:          %ld bytes\n"
            "- Joliet Dir Tables Size:       %ld bytes",
               lISOPathTablesSize,
               lJolietPathTablesSize,
               getISOSize(),
               getJolietSize());
   


   return true;
}
   
bool ClRoot::generate(const Hook* data)
{
   ISOEndDescriptor  hDesc;
   bool              bRes;
   int               lCur;

   bRes = fWritePadSectors(data, 16);
   if (!bRes)
      return false;

   {  /* write roots */
      if (isISOEntry())
      {
         _d(Lvl_Debug, "Writing primary volume descriptor");
         bRes = HookAttrT<void*, sint>::Call(data, hPrimary.getData(), 2048);
         if (!bRes)
            return false;
      }

      if (isJolietEntry())
      {
         _d(Lvl_Debug, "Writing supplementary volume descriptor");
         bRes = HookAttrT<void*, sint>::Call(data, &hSupplementary, 2048);
         if (!bRes)
            return false;
      }

      {  /* terminator */
         _d(Lvl_Debug, "Writing end descriptor");
         bRes = HookAttrT<void*, sint>::Call(data, &hDesc, 2048);
         if (!bRes)
            return false;
      }
   }

   {  /* write path tables */
      if (isISOEntry())
      {
         unsigned char    *pPathRec;    // path table
         int               lPathRec;
         int               lPathPos;

         lPathRec = (lISOPathTablesSize + 2047) &~ 2047;
         pPathRec = new unsigned char[lPathRec];
         memset(pPathRec, 0, lPathRec);

         _d(Lvl_Debug, "Generating LE Path Tables (%ld bytes max)", lPathRec);

         {  /* Little Endian goes first */
            lPathPos = 0;
            for (lCur=0; lCur<hDirVec.Count(); lCur++)
            {
//#warning removed!
               lPathPos += hDirVec[lCur]->buildISOPathTableEntry((ISOPathRecord*)&pPathRec[lPathPos], false);
            }
            bRes = HookAttrT<void*, sint>::Call(data, pPathRec, lPathRec);
            if (bRes == false)
            {
               delete [] pPathRec;
               return false;
            }
         }

         _d(Lvl_Debug, "Generating BE Path Tables (%ld bytes max)", lPathRec);

         {  /* Big Endian goes last */
            lPathPos = 0;
            for (lCur=0; lCur<hDirVec.Count(); lCur++)
            {
//#warning removed!
               lPathPos += hDirVec[lCur]->buildISOPathTableEntry((ISOPathRecord*)&pPathRec[lPathPos], true);
            }
            bRes = HookAttrT<void*, sint>::Call(data, pPathRec, lPathRec);
            if (bRes == false)
            {
               delete [] pPathRec;
               return false;
            }
         }
      
         _d(Lvl_Debug, "Path tables generated.");
         delete [] pPathRec;
      }

      if (isJolietEntry())
      {
         unsigned char    *pPathRec;    // path table
         int               lPathRec;
         int               lPathPos;

         lPathRec = (lJolietPathTablesSize + 2047) &~ 2047;
         pPathRec = new unsigned char[lPathRec];
         memset(pPathRec, 0, lPathRec);
         
         _d(Lvl_Debug, "Path Joliet LE Path Tables (%ld bytes max)", lPathRec);

         {  /* Little Endian goes first */
            lPathPos = 0;
            for (lCur=0; lCur<hDirVec.Count(); lCur++)
            {
               lPathPos += hDirVec[lCur]->buildJolietPathTableEntry((ISOPathRecord*)&pPathRec[lPathPos], false);
            }
            bRes = HookAttrT<void*, sint>::Call(data, pPathRec, lPathRec);
            if (bRes == false)
            {
               delete [] pPathRec;
               return false;
            }
         }

         _d(Lvl_Debug, "Path Joliet BE Path Tables (%ld bytes max)", lPathRec);

         {  /* Big Endian goes last */
            lPathPos = 0;
            for (lCur=0; lCur<hDirVec.Count(); lCur++)
            {
               lPathPos += hDirVec[lCur]->buildJolietPathTableEntry((ISOPathRecord*)&pPathRec[lPathPos], true);
            }
            bRes = HookAttrT<void*, sint>::Call(data, pPathRec, lPathRec);
            if (bRes == false)
            {
               delete [] pPathRec;
               return false;
            }
         }
      
         _d(Lvl_Debug, "Path tables generated.");
         delete [] pPathRec;
      }

   }

   {  /* write dir tables */

      _d(Lvl_Debug, "Generating ISO Dir Tables");

      if (isISOEntry())
      {
         unsigned char    *pDirRec  = 0;    // path table
         int               lDirRec  = 0;
         int               lDirSize = 0;

         for (lCur=0; lCur<hDirVec.Count(); lCur++)
         {
            lDirSize = hDirVec[lCur]->getISOSize();
            if (lDirSize > lDirRec)
            {
               if (pDirRec != 0)
                  delete [] pDirRec;
               pDirRec = 0;

               pDirRec = new unsigned char[lDirSize];
               lDirRec = lDirSize;
            }
            memset(pDirRec, 0, lDirSize);

            hDirVec[lCur]->buildISODirTable((ISODirRecord*)pDirRec);
            bRes = HookAttrT<void*, sint>::Call(data, pDirRec, lDirSize);
            if (bRes == false)
               break;
         }

         if (0 != pDirRec)
            delete [] pDirRec;
         
         if (bRes == false)
         {
            return false;
         }
      }

      _d(Lvl_Debug, "Generating Joliet Dir Tables");

      if (isJolietEntry())
      {
         unsigned char    *pDirRec  = 0;    // path table
         int               lDirRec  = 0;
         int               lDirSize = 0;

         for (lCur=0; lCur<hDirVec.Count(); lCur++)
         {
            lDirSize = hDirVec[lCur]->getJolietSize();
            if (lDirSize > lDirRec)
            {
               if (pDirRec != 0)
                  delete [] pDirRec;
               pDirRec = 0;

               pDirRec = new unsigned char[lDirSize];
               lDirRec = lDirSize;
            }
            memset(pDirRec, 0, lDirSize);

            hDirVec[lCur]->buildJolietDirTable((ISOWDirRecord*)pDirRec);
            bRes = HookAttrT<void*, sint>::Call(data, pDirRec, lDirSize);
            if (bRes == false)
               break;
         }

         if (0 != pDirRec)
            delete [] pDirRec;
         
         if (bRes == false)
         {
            return false;
         }
      }

   }
 
   _d(Lvl_Debug, "Writing files");
   {  /* write files */
      char *mem = new char[32768];

      for (lCur=0; lCur<hFileVec.Count(); lCur++)
      {
         bRes = hFileVec[lCur]->getData(data, mem, 32768);
         if (false == bRes)
            return false;
      }

      delete [] mem;
   }


   _d(Lvl_Debug, "Writing Pad Sectors");
   fWritePadSectors(data, lPadSize);

   return true;
}

const VectorT<ClDirectory*> &ClRoot::getDirVector() const
{
   return hDirVec;
}

const VectorT<ClFile*> &ClRoot::getFileVector() const
{
   return hFileVec;
}

void ClRoot::setISOSize(unsigned long lSize)
{
   ClDirectory::setISOSize(lSize);
   hPrimary.setRootSize(lSize);
}

void ClRoot::setISOPosition(unsigned long lPosition)
{
   ClDirectory::setISOPosition(lPosition);
   hPrimary.setRootPosition(lPosition);
}

void ClRoot::setJolietSize(unsigned long lSize)
{
   ClDirectory::setJolietSize(lSize);
   hSupplementary.setRootSize(lSize);
}

void ClRoot::setJolietPosition(unsigned long lPosition)
{
   ClDirectory::setJolietPosition(lPosition);
   hSupplementary.setRootPosition(lPosition);
}

unsigned long ClRoot::buildISOPathTableEntry(ISOPathRecord *pRec, bool bIsMSB)
{
   if (!isISOEntry())
      return 0;
      
   long lLen = getISOPathTableEntrySize();

   pRec->setExtent(getISOPosition(), bIsMSB);
   pRec->setParent(lParentPathID, bIsMSB);
   pRec->setISOName(0);
   return lLen;
}

unsigned long ClRoot::buildJolietPathTableEntry(ISOPathRecord *pRec, bool bIsMSB)
{
   if (!isJolietEntry())
      return 0;
      
   long lLen = getJolietPathTableEntrySize();

   pRec->setExtent(getJolietPosition(), bIsMSB);
   pRec->setParent(lParentPathID, bIsMSB);
   pRec->setJolietName(0);
   return lLen;
}
   
unsigned long ClRoot::getJolietPathTableEntrySize() const
{
   if (!isJolietEntry())
      return 0;
      
   return sizeof(ISOPathRecord);
}

unsigned long ClRoot::getISOPathTableEntrySize() const
{
   if (!isISOEntry())
      return 0;
      

   return sizeof(ISOPathRecord);
}

   /* own functions...? */

const char *ClRoot::getVolumeID()
{
   return hPrimary.getVolumeID();
}

const char *ClRoot::getVolumeSetID()
{
   return hPrimary.getVolumeSetID();
}

const char *ClRoot::getSystemID()
{
   return hPrimary.getSystemID();
}

const char *ClRoot::getPreparerID()
{
   return hPrimary.getPreparerID();
}

const char *ClRoot::getPublisherID()
{
   return hPrimary.getPublisherID();
}

const char *ClRoot::getApplicationID()
{
   return hPrimary.getApplicationID();
}

void ClRoot::setVolumeID(const char* id)
{
   setNormalName(id);
   setJolietName(id);
   hPrimary.setVolumeID(id);
   hSupplementary.setVolumeID(id);
}

void ClRoot::setVolumeSetID(const char* id)
{
   hPrimary.setVolumeSetID(id);
   hSupplementary.setVolumeSetID(id);
}

void ClRoot::setSystemID(const char* id)
{
   hPrimary.setSystemID(id);
   hSupplementary.setSystemID(id);
}

void ClRoot::setPreparerID(const char* id)
{
   hPrimary.setPreparerID(id);
   hSupplementary.setPreparerID(id);
}

void ClRoot::setPublisherID(const char* id)
{
   hPrimary.setPublisherID(id);
   hSupplementary.setPublisherID(id);
}

void ClRoot::setApplicationID(const char* id)
{
   hPrimary.setApplicationID(id);
   hSupplementary.setApplicationID(id);
}

uint32 ClRoot::getImageSize()
{
   return lTotalSize;
}

void ClRoot::setNormalName(const char *name)
{
   ClDirectory::setNormalName(name);
   hRootName.setFullName(name);
   hRootName.update();
   hPrimary.setVolumeID(hRootName.getName());
}

const char *ClRoot::getNormalName() const
{
   return hRootName.getName();
}
