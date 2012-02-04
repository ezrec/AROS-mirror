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
#include "ClElement.h"
#include "ClName.h"
#include "ClRoot.h"
#include "ClDirectory.h"
#include <libclass/dos.h>
#include "../ISOStructures.h"
#include "../RRStructures.h"
#include <Generic/HookAttrT.h>
#include <Generic/Debug.h>
#include <LibC/LibC.h>
/*
 * initialize typical data
 */
ClElement::ClElement(ClRoot *pRoot, ClDirectory *pParent) :
   rr_rr(RR_RR::Use_PX | RR_RR::Use_NM),
   rr_px(0, false),
   rr_nm(""),
   rr_as(0, 0)
{
   this->pRoot    = pRoot;
   this->pParent  = pParent;

   setISOPosition(0);
   setJolietPosition(0);

   setISOSize(0);
   setJolietSize(0);

   bIsISO         = true;
   bIsJoliet      = true;
   bIsRR          = true;
   bParentIsISO   = true;
   bParentIsJoliet= true;
   bParentIsRR    = true;
   setISOLevel(ClName::ISOLevel_3);
   setISORelaxed(true);

   if (this != pRoot)
   {
      bParentIsISO   = pParent->isISOEntry();
      bParentIsJoliet= pParent->isJolietEntry();
      bParentIsRR    = pParent->isRockRidgeEntry();
      setISOLevel(pParent->getISOLevel());
      setISORelaxed(pParent->isISORelaxed());
   }

   DOS->DateStamp(&hDateStamp);

   hExtensions << &rr_rr;
   hExtensions << &rr_px;
   hExtensions << &rr_as;
   hExtensions << &rr_nm;
}

/*
 * destroy child elements
 */
ClElement::~ClElement()
{
}
   
/*
 * method for other programs - do not remove
 */
void ClElement::dispose()
{
   delete this;      // NEVER EVER CALL DELETE FROM ANOTHER TASK
}

/*
 * returns full path to file or path to dir to be scanned
 */
const String& ClElement::getPath()
{
   return sFullPath;
}

/*
 * sets full path to file or dir to be scanned
 */
void ClElement::setPath(const String& sString)
{
   sFullPath = sString;
}

/*
 * update all information to reflect current status
 */
bool ClElement::update()
{
   rr_px.SetDir(isDirectory());
   pISOName.setShortForm(isDirectory());
   arrangeISODirTable();
   return true;
}

/*
 * returns iso name
 */
const char *ClElement::getISOName() const
{
   return pISOName.getName();
}

/*
 * set to true if element is imported -- NOT IMPLEMENTED
 */
bool ClElement::isImported()
{
   return false;
}

/*
 * returns parent directory to which this item belongs
 */
ClDirectory *ClElement::getParent()
{
   return pParent;
}

/*
 * returns root directory
 */
ClRoot *ClElement::getRoot()
{
   return pRoot;
}

/*
 * set iso size (files and directories)
 */
void ClElement::setISOSize(unsigned long lSize)
{
   this->lISOSize = lSize;
}

/*
 * set iso position (files and directories)
 */
void ClElement::setISOPosition(unsigned long lPosition)
{
   this->lISOPosition = lPosition;
}

/*
 * set joliet size (files and directories)
 */
void ClElement::setJolietSize(unsigned long lSize)
{
   this->lJolietSize = lSize;
}

/*
 * set joliet position (files and directories)
 */
void ClElement::setJolietPosition(unsigned long lPosition)
{
   this->lJolietPosition = lPosition;
}

/*
 * creation date
 */
void ClElement::setDate(const struct DateStamp *pDS)
{
   this->hDateStamp = *pDS;      // copy
}

/*
 * protection flags
 */
void ClElement::setProtection(unsigned char lProtection)
{
   if (bProtection == lProtection)
      return;

   this->bProtection = lProtection;
   rr_px.SetFlags(lProtection);
   rr_as.SetFlags(lProtection);
}

/*
 * change name and update reflecting elements
 */
void ClElement::setNormalName(const char* sName)
{
   _d(Lvl_Info, "Normal name: %s", (int)sName);

   if (strcmp(pISOName.getFullName().Data(), sName) == 0)
      return;
   
   //MessageBox("Info", "%s <> %s = %ld", "Ok", ARRAY((int)pISOName->getFullName().Data(), (int)sName, pISOName->getFullName() == sName)); 
   pISOName.setFullName(sName);
   rr_nm.SetName(sName);
   
   buildISOName();
}
   
/*
 * rebuild iso name
 */
void ClElement::buildISOName()
{
   pISOName.update();
}

/*
 * sets joliet name
 */
void ClElement::setJolietName(const char* sName)
{
   this->sJolietName = sName;
}

/*
 * comments
 */
void ClElement::setComment(const char* sComment)
{
   rr_as.SetElemComment(sComment);
}

/*
 * returns iso size for files and dirs
 */
unsigned long ClElement::getISOSize() const
{
   if (!isISOEntry())
      return 0;
   return lISOSize;
}

/*
 * returns mapping for iso block - files and dirs
 */
unsigned long ClElement::getISOPosition() const
{
   if (!isISOEntry())
      return 0;
   return lISOPosition;
}

/*
 * returns joliet size for files and dirs
 */
unsigned long ClElement::getJolietSize() const
{
   if (!isJolietEntry())
      return 0;
   return lJolietSize;
}

/*
 * returns mapping for joliet block - files and dirs
 */
unsigned long ClElement::getJolietPosition() const
{
   if (!isJolietEntry())
      return 0;
   return lJolietPosition;
}

/*
 * returns creation date
 */
const struct DateStamp &ClElement::getDate() const
{
   return hDateStamp;
}

/*
 * returns protection bits
 */
unsigned char ClElement::getProtection() const
{
   return bProtection;
}

/*
 * returns name used for rockridge and iso
 */
const char *ClElement::getNormalName() const
{
   return pISOName.getFullName().Data();
}

/*
 * returns name used for joliet
 */
const char *ClElement::getJolietName() const
{
   if (sJolietName.Length() == 0)
      return getNormalName();
   return sJolietName.Data();
}

/*
 * returns comment
 */
const char *ClElement::getComment() const
{
   return rr_as.GetElemComment().Data();
}

/*
 * arrange rr extensions
 */
void ClElement::arrangeISODirTable()
{
   uint tmp, cur=0;
   /*
    * remove extensions in this order
    */
   struct RRExt* exts[] =
   {
      &rr_as,
      &rr_nm,
      &rr_px
   };

   do
   {
      tmp = ISODirRecord::getLength(getISOName(), getRRExtensionsSize());
      if (tmp < 255)
         break;
      if (cur == 0)
         hExtensions << &rr_ce;
      hExtensions >> exts[cur];
      hContExtensions << exts[cur];
      ++cur;
   } while (cur < (sizeof(exts) / sizeof(exts[0])));
   
   ASSERTS(tmp < 255, "Recording disc is still not safe. Some extensions are just too long\nPlease report this incident!");
}

/*
 * returns iso dir table entry size
 */
unsigned long ClElement::getISODirTableEntrySize() const
{
   return ISODirRecord::getLength(getISOName(), getRRExtensionsSize());
}

/*
 * returns joliet dir table entry size
 */
unsigned long ClElement::getJolietDirTableEntrySize() const
{
   return ISOWDirRecord::getLength(getNormalName());
}
   
/*
 * returns extension size for rockridge elements
 */
unsigned long ClElement::getRRContinuationSize() const
{
   uint32 size = 0;

   if (!isRockRidgeEntry())
      return 0;

   for (int i=0; i<hContExtensions.Count(); i++)
   {
      size += hContExtensions[i]->Length();
   }
   return size;
}

/*
 * builds iso dir table entry
 */
unsigned long ClElement::buildISODirTableEntry(ISODirRecord* pRec)
{
   pRec->initialize();
   pRec->setName(getISOName());
   pRec->setPosition(getISOPosition());
   pRec->setSize(getISOSize());
   pRec->setFlags(isDirectory() ? 2 : 0);
   pRec->setDate(&hDateStamp);
   
   uint8   *buff = pRec->getExtension();
   int      len  = 0;

   if (bIsRR)
   {
      len = getRRExtensions(buff);
      pRec->setExtensionSize((len+1) &~ 1);
   }

   return pRec->getSize();
}

/*
 * builds joliet dir table entry
 */
unsigned long ClElement::buildJolietDirTableEntry(ISOWDirRecord* pRec)
{
   int lSize = getJolietDirTableEntrySize();
   pRec->initialize();
   pRec->setName(getJolietName());
   pRec->setPosition(getJolietPosition());
   pRec->setSize(getJolietSize());
   pRec->setFlags(isDirectory() ? 2 : 0);
   pRec->setDate(&hDateStamp);
   return lSize;
}

/*
 * builds rockridge continuation area
 */
unsigned long ClElement::buildRRContinuation(uint8 *buff)
{
   if (false == isRockRidgeEntry())
      return 0;

   int32 size = 0;
   for (int i=0; i<hContExtensions.Count(); i++)
   {
      buff  = hContExtensions[i]->PutData(buff);
      size += hContExtensions[i]->Length();
   }
   return size;
}

/*
 * builds rockridge extensions
 */
int ClElement::getRRExtensions(uint8 *buff)
{
   if (false == isRockRidgeEntry())
      return 0;

   int32 size = 0;
   for (int i=0; i<hExtensions.Count(); i++)
   {
      buff  = hExtensions[i]->PutData(buff);
      size += hExtensions[i]->Length();
   }
   return size;
}

/*
 * map rockridge continuation area
 */
void ClElement::setRRContinuationLocation(uint32 block, uint32 offset)
{
   if (!isRockRidgeEntry())
      return;

   rr_ce.setBlock(block);
   rr_ce.setOffset(offset);
   rr_ce.setLength(getRRContinuationSize());
}

/*
 * calculates rockridge extensions size
 */
int ClElement::getRRExtensionsSize() const
{
   uint32 size = 0;

   if (!isRockRidgeEntry())
      return 0;

   for (int i=0; i<hExtensions.Count(); i++)
   {
      size += hExtensions[i]->Length();
   }
   return size;
}

/*
 * enables or disables presence of element in iso structure
 */
void ClElement::setISOEntry(bool bIsISO)
{
   this->bIsISO = bIsISO;
}

/*
 * enables or disables presence of element in joliet structure
 */
void ClElement::setJolietEntry(bool bIsJoliet)
{
   this->bIsJoliet = bIsJoliet;
}

/*
 * enables or disables presence of element in rockridge structure
 */
void ClElement::setRockRidgeEntry(bool isrr)
{
   bIsRR = isrr;
}

/*
 * parent-enables/disables presence of element in iso structure
 */
void ClElement::setParentISOEntry(bool bIsISO)
{
   bParentIsISO = bIsISO;
}

/*
 * parent-enables/disables presence of element in joliet structure
 */
void ClElement::setParentJolietEntry(bool bIsJoliet)
{
   bParentIsJoliet = bIsJoliet;
}

/*
 * parent-enables/disables presence of element in rockridge structure
 */
void ClElement::setParentRockRidgeEntry(bool isrr)
{
   bParentIsRR = isrr;
}

/*
 * true, if element will appear in rockridge structure
 */
bool ClElement::isRockRidgeEntry() const
{
   return (bIsRR && bParentIsRR);
}
   
/*
 * true, if element will appear in iso structure
 */
bool ClElement::isISOEntry() const
{
   return (bIsISO && bParentIsISO);
}

/*
 * true, if element will appear in joliet structure
 */
bool ClElement::isJolietEntry() const 
{
   return (bIsJoliet && bParentIsJoliet);
}

/*
 * pad sector generator
 */
bool ClElement::fWritePadSectors(const Hook *pHook, unsigned long lCount)
{
   char *pMem = new char[16384];
   bool lRes = 0;
     
//   DPrintfA("Sending %ld pad sectors...\n", lCount);
     
   memset(pMem, 0, 16384);

   while (lCount > 0)
   {
      int lSub = lCount; 
      if (lSub > 8)
         lSub = 8;
      lRes = HookAttrT<void*, int>::Call(pHook, pMem, (lSub << 11));
      lCount -= lSub;
      if (!lRes)
         break;
   }
   
   delete [] pMem;
   return lRes;
}

/*
 * obtains ClName as iso name structure
 */
const ClName *ClElement::getISONameStruct() const 
{
   return &pISOName;
}

/*
 * sets new level for iso file names
 * 
 * inputs:
 *    ClName::Level  - new level
 *
 * returns:
 *    bool - true, if level needed update
 */
bool ClElement::setISOLevel(ClName::Level lvl)
{
   if (pISOName.getLevel() == lvl)
      return false;

   pISOName.setLevel(lvl);
   return true;
}
  
/*
 * sets relaxed names for iso file names
 *
 * inputs:
 *    bool - true for relaxed names, false otherwise
 *
 * returns:
 *    bool - true, if flag needed update
 */
bool ClElement::setISORelaxed(bool state)
{
   if (pISOName.isRelaxed() == state)
      return false;

   pISOName.setRelaxed(state);
   return true;
}

/*
 * returns current state of relaxed flag
 */
bool ClElement::isISORelaxed() const
{
   return pISOName.isRelaxed();
}

/*
 * returns current iso level
 */
ClName::Level ClElement::getISOLevel() const 
{
   return pISOName.getLevel();
}

/*
 * getDebug() is something that will help us print out debug messages.
 */
DbgHandler *ClElement::getDebug()
{
   return getRoot()->getDebug();
}

