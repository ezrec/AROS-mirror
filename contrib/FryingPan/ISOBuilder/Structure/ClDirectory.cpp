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
#include "ClDirectory.h"
#include "ClFile.h"
#include "ClRoot.h"
#include "../ISOStructures.h"
#include <libclass/dos.h>
#include <dos/exall.h>
#include <dos/dosextens.h>
#include <Generic/VectorT.h>
#include <Generic/Debug.h>
#include <LibC/LibC.h>

void ClDirectory::initialize(const char *sName)
{
   if (strlen(sName) > 0)
   {
      setNormalName(sName);
   }
}

ClDirectory::ClDirectory(ClRoot *pRoot, ClDirectory *pParent)
   : ClElement(pRoot, pParent)
{
   initialize("New Directory");
}
 
ClDirectory::ClDirectory(ClRoot *pRoot, ClDirectory *pParent, const ExAllData *ed)
   : ClElement(pRoot, pParent)
{
   struct DateStamp hDS;
   
   hDS.ds_Days    = ed->ed_Days;
   hDS.ds_Minute  = ed->ed_Mins;
   hDS.ds_Tick    = ed->ed_Ticks;
   
   setComment((char*)ed->ed_Comment);
   setDate((struct DateStamp*)&hDS);
   initialize((char*)ed->ed_Name);
   setProtection(ed->ed_Prot);
}

ClDirectory::ClDirectory(ClRoot *pRoot, ClDirectory *pParent, const FileInfoBlock *fib) 
   : ClElement(pRoot, pParent)
{
   setComment((char*)fib->fib_Comment);
   setDate(&fib->fib_Date);
   initialize((char*)fib->fib_FileName);
   setProtection(fib->fib_Protection);
}

ClDirectory::ClDirectory(ClRoot *pRoot, ClDirectory *pParent, const String &sName)
   : ClElement(pRoot, pParent)
{
   initialize(sName.Data());  
}

ClDirectory::~ClDirectory()
{
   hChildren.ForEach(&ClDirectory::vecDeleteChild);
}
   
bool ClDirectory::vecDeleteChild(ClElement* const &e)
{
   delete e;
   return true;
}
   
int ClDirectory::vecCompareChildren(ClElement* const& a, ClElement*  const& b)
{
   if (a->isDirectory() && (!b->isDirectory()))
      return -1;
   else if (b->isDirectory() && (!a->isDirectory()))
      return 1;
   else
      return stricmp(a->getNormalName(), b->getNormalName());
}
   
int ClDirectory::vecCompareISONames(const ClName* const& a, const ClName* const& b)
{
   return a->compareBase(b);
}

void ClDirectory::sort()
{
   hChildren.Sort(&vecCompareChildren);
}

bool ClDirectory::isDirectory()
{
   return true;
}

bool ClDirectory::addChild(ClElement *aChild)
{
   if (internalAddChild(aChild))
   {
      sort();
      return true;
   }
   return false;
}

void ClDirectory::remChild(ClElement *aChild)
{
   if (aChild == 0)
      return;

   hChildren >> aChild;
   delete aChild;
}

bool ClDirectory::scanDirectory(const String &sDirectory)
{
   ExAllControl  *eac;
   ExAllData     *ead;
   BPTR           lock;
   ClElement     *pElem;
   bool           bMore;

   VectorT<ClElement*> v;

   lock = DOS->Lock(sDirectory.Data(), ACCESS_READ);
   eac = (ExAllControl*)DOS->AllocDosObject(DOS_EXALLCONTROL, 0);
   ead = (ExAllData*)   new char[65536];

   if ((0 == lock)   ||
       (0 == eac)    ||
       (0 == ead))
   {
      if (0 != lock) 
         DOS->UnLock(lock);
      if (0 != eac)
         DOS->FreeDosObject(DOS_EXALLCONTROL, 0);
      if (0 != ead)
         delete [] ead;
      return false;
   }

   _d(Lvl_Info, "Scanning directory %s...", (int)sDirectory.Data());

   do
   {
      bMore = DOS->ExAll(lock, ead, 65536, ED_COMMENT, eac);
      ExAllData *ed = ead;
      
      if (0 == eac->eac_Entries)
         break;
         
      for (ed = ead; ed; ed = ed->ed_Next)
      {
         String sTemp(sDirectory);
         sTemp.AddPath((char*)ed->ed_Name);

         pElem = 0;
         
         switch (ed->ed_Type)
         {
            case ST_FILE:
               pElem = new ClFile(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            case ST_LINKDIR:
               pElem = new ClDirectory(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            case ST_LINKFILE:
               pElem = new ClFile(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            case ST_PIPEFILE:
               pElem = new ClFile(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            case ST_ROOT:
               pElem = new ClDirectory(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            case ST_SOFTLINK:
               pElem = new ClDirectory(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            case ST_USERDIR:
               pElem = new ClDirectory(getRoot(), this, ed);
               pElem->setPath(sTemp);
               break;
               
            default:
               pElem = 0;
               break;
         }
         v << pElem;
         internalAddChild(pElem);
      }      
   }
   while (bMore);
  
   /* no need to call exallend since we completed our task :) */
   /* ExAllEnd(lock, ead, 65535, ED_NAME, eac);               */

   delete [] ead;
   DOS->FreeDosObject(DOS_EXALLCONTROL, eac);
   DOS->UnLock(lock);
   ead = 0;
   eac = 0;

   sort();

   for (long i=0; i<v.Count(); i++)
   {
      v[i]->update();
   }

   return true;
}

ClElement* ClDirectory::addChild(const String &sElem)
{
   BPTR           lock;
   FileInfoBlock *fib = new FileInfoBlock;
   ClElement     *pElem = 0;

   _d(Lvl_Info, "Adding %s", (int)sElem.Data());
   lock = DOS->Lock(sElem.Data(), ACCESS_READ);
   if ((lock != 0) && (DOS->Examine(lock, fib) != 0))
   {
      switch (fib->fib_EntryType)
      {
         case ST_FILE:
            pElem = new ClFile(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         case ST_LINKDIR:
            pElem = new ClDirectory(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         case ST_LINKFILE:
            pElem = new ClFile(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         case ST_PIPEFILE:
            pElem = new ClFile(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         case ST_ROOT:
            pElem = new ClDirectory(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         case ST_SOFTLINK:
            pElem = new ClDirectory(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         case ST_USERDIR:
            pElem = new ClDirectory(getRoot(), this, fib);
            pElem->setPath(sElem);
            break;
               
         default:
            pElem = 0;
            break;
      }
         
      internalAddChild(pElem);
      pElem->update();
   }

   if (lock)
      DOS->UnLock(lock);

   delete fib;

   sort();

   return pElem;
}

bool ClDirectory::update()
{
   ClElement::update();

   if (getPath().Length())
   {
      scanDirectory(getPath());
      setPath(0);
   }

   return true;
}

int ClDirectory::getChildrenCount() const
{
   return hChildren.Count();
}

ClElement* ClDirectory::getChild(int lNum)
{
   return hChildren[lNum];
}
 
void ClDirectory::freeISOName()
{
   // don't rebuild. do nothing.
}

void ClDirectory::setParentElementPathID(unsigned long lID)
{
   lParentPathID = lID;
}

unsigned long ClDirectory::getJolietPathTableEntrySize() const
{
   if (!isJolietEntry())
      return 0;
      
   return lJolietPathTableEntrySize;
}

unsigned long ClDirectory::getISOPathTableEntrySize() const
{
   if (!isISOEntry())
      return 0;
      

   return lISOPathTableEntrySize;
}

void ClDirectory::setJolietPathTableEntrySize(unsigned long lSize)
{
   lJolietPathTableEntrySize = lSize;
}

void ClDirectory::setISOPathTableEntrySize(unsigned long lSize)
{
   lISOPathTableEntrySize = lSize;
}

void ClDirectory::rebuild()
{
}

bool ClDirectory::recalculate()
{
   unsigned long  lISO, 
                  lJoliet,
                  lSize,
                  lRR;

   // fix all names
   sort();

   {
      VectorT<const ClName*> names;
      for (int i=0; i<getChildrenCount(); i++)
      {
         names.InsertSorted(getChild(i)->getISONameStruct(), &vecCompareISONames);
         const_cast<ClName*>(getChild(i)->getISONameStruct())->setVersion(1);
      }

      for (int i=1; i<names.Count(); i++)
      {
         if (0 == names[i-1]->compareBase(names[i]))
         {
            _d(Lvl_Debug, "Found two same entries. Setting new version to %ld", names[i-1]->getVersion()+1);
            const_cast<ClName*>(names[i])->setVersion(names[i-1]->getVersion() + 1);
            const_cast<ClName*>(names[i])->update();
         }
      }

      names.Empty();
   }

   setISOPathTableEntrySize(ISOPathRecord::getISOLength(getISOName()));             // cool!!! ;)
   setJolietPathTableEntrySize(ISOPathRecord::getJolietLength(getJolietName()));    // and this, too! ;) 
   
   lISO     = 34 + rr_sp.Length() + rr_px.Length();    //sizeof(ISODirRecord);       // dot
   lISO    += 34 + getParent()->rr_px.Length();        //sizeof(ISODirRecord);       // dotdot
   lJoliet  = 34;    //sizeof(ISOWDirRecord);      // dot
   lJoliet += 34;    //sizeof(ISOWDirRecord);      // dotdot
   lRR      = 0;

   for (int i=0; i<getChildrenCount(); i++)
   {
      lSize     = getChild(i)->getISODirTableEntrySize();
      if ((2048 - (lISO & 2047)) < lSize)
      {
         lISO += 2047;
         lISO &= ~2047;
      }
      lISO     += lSize;

      lSize     = getChild(i)->getJolietDirTableEntrySize();
      if ((2048 - (lJoliet & 2047)) < lSize)
      {
         lJoliet += 2047;
         lJoliet &= ~2047;
      }
      lJoliet  += lSize;

      lSize     = getChild(i)->getRRContinuationSize();
      if ((2048 - (lRR & 2047)) < lSize)
      {
         lRR += 2047;
         lRR &= ~2047;
      }
      lRR      += lSize;
   }
   
   /*
    * mix ISO and RR together as they *go* together
    */
   lISO    += 2047;
   lISO    &= ~2047;
   lJoliet += 2047;
   lJoliet &= ~2047;
   lRR     += 2047;
   lRR     &= ~2047;
   
   setISOSize(lISO + lRR);
   setJolietSize(lJoliet);

   return true;
}

unsigned long ClDirectory::buildISOPathTableEntry(ISOPathRecord *pRec, bool bIsMSB)
{
   if (!isISOEntry())
      return 0;
      
   long lLen = getISOPathTableEntrySize();

   pRec->setExtent(getISOPosition(), bIsMSB);
   pRec->setParent(lParentPathID, bIsMSB);
   pRec->setISOName(getISOName());
   return lLen;
}

unsigned long ClDirectory::buildJolietPathTableEntry(ISOPathRecord *pRec, bool bIsMSB)
{
   if (!isJolietEntry())
      return 0;
      
   long lLen = getJolietPathTableEntrySize();

   pRec->setExtent(getJolietPosition(), bIsMSB);
   pRec->setParent(lParentPathID, bIsMSB);
   pRec->setJolietName(getJolietName());
   return lLen;
}
   
unsigned long ClDirectory::buildISODirTable(ISODirRecord *pRec)
{
   long lPos = 0;
   long lEnd = getISOSize();
   uint8 *buf;

   if (!isISOEntry())
      return 0;
      
   ISODotDirRecord     *dot    = ((ISODotDirRecord*)&((char*)pRec)[lPos]);
   dot->initialize();
   dot->setSize(getISOSize());
   dot->setDate(&getDate());
   dot->setPosition(getISOPosition());
   if (isRockRidgeEntry())
   {
      buf = dot->getExtension();
      buf = rr_sp.PutData(buf);
      buf = rr_px.PutData(buf);
      dot->setExtensionSize(rr_sp.Length() + rr_px.Length());
   }
   lPos += dot->getSize();

   ISODotDotDirRecord  *dotdot = ((ISODotDotDirRecord*)&((char*)pRec)[lPos]);
   dotdot->initialize();
   dotdot->setDate(&getParent()->getDate());
   dotdot->setSize(getParent()->getISOSize());
   dotdot->setPosition(getParent()->getISOPosition());
   if (isRockRidgeEntry())
   {
      buf = dotdot->getExtension();
      getParent()->rr_px.PutData(buf);
      dotdot->setExtensionSize(getParent()->rr_px.Length());
   }
   lPos += dotdot->getSize();
   

   for (int i=0; i<getChildrenCount(); i++)
   {
      int lSize = 0;

      _d(Lvl_Info, "Adding child %s [fill: %ld out of %ld]...", (uint32)getChild(i)->getNormalName(), lPos, getISOSize());

      /*
       * fun stuff: calculate rockridge extensions and place them.
       */
      lSize = getChild(i)->getRRContinuationSize();
      if (lSize > 0)
      {
         if ((lEnd & 2047) < lSize)
            lEnd &= ~2047;
         lEnd -= lSize;

         getChild(i)->buildRRContinuation(&((uint8*)pRec)[lEnd]);
         getChild(i)->setRRContinuationLocation(getISOPosition() + (lEnd >> 11), lEnd & 2047);
      }

      /*
       * and now the actual entry
       */
      lSize = getChild(i)->getISODirTableEntrySize();
      if ((2048-(lPos&2047)) < lSize)
      {
         lPos = (lPos + 2047) & ~2047;
      }
      getChild(i)->buildISODirTableEntry((ISODirRecord*)&((char*)pRec)[lPos]);
      lPos += lSize;

   }
   return lPos;
}

unsigned long ClDirectory::buildJolietDirTable(ISOWDirRecord *pRec)
{
   long lPos = 0;
   
   if (!isJolietEntry())
      return 0;
 
   ISODotDirRecord     *dot    = ((ISODotDirRecord*)&((char*)pRec)[lPos]);
   lPos += 34;
   ISODotDotDirRecord  *dotdot = ((ISODotDotDirRecord*)&((char*)pRec)[lPos]);
   lPos += 34;
   
   dot->initialize();
   dot->setDate(&getDate());
   dot->setSize(getJolietSize());
   dot->setPosition(getJolietPosition());
   dotdot->initialize();
   dotdot->setDate(&getParent()->getDate());
   dotdot->setSize(getParent()->getJolietSize());
   dotdot->setPosition(getParent()->getJolietPosition());

   for (int i=0; i<getChildrenCount(); i++)
   {
      int lSize = 0;
      lSize = getChild(i)->getJolietDirTableEntrySize();
      if ((2048-(lPos&2047)) < lSize)
      {
         lPos = (lPos + 2047) & ~2047;
      }
      lSize = getChild(i)->buildJolietDirTableEntry((ISOWDirRecord*)&((char*)pRec)[lPos]);
      lPos += lSize;
   }
   return lPos;
}

/*
 * internal add child: use this to add child without sorting tree.
 */
bool ClDirectory::internalAddChild(ClElement* aChild)
{
   hChildren << aChild;
   return true;
}

/*
 * set iso entry flag and propagate new state, if changed.
 */
void ClDirectory::setISOEntry(bool bIsISO)
{
   bool prev = isISOEntry();
   ClElement::setISOEntry(bIsISO);

   if (isISOEntry() != prev)
   {
      prev = isISOEntry();

      for (int32 i=0; i<hChildren.Count(); i++)
      {
         hChildren[i]->setParentISOEntry(prev);
      }
   }
}

/*
 * set joliet entry flag and propagate new state, if changed.
 */
void ClDirectory::setJolietEntry(bool bIsJoliet)
{
   bool prev = isJolietEntry();

   ClElement::setJolietEntry(bIsJoliet);

   if (isJolietEntry() != prev)
   {
      prev = isJolietEntry();

      for (int32 i=0; i<hChildren.Count(); i++)
      {
         hChildren[i]->setParentJolietEntry(prev);
      }
   }
}


/*
 * set rr entry flag and propagate new state, if changed.
 */
void ClDirectory::setRockRidgeEntry(bool bIsRR)
{
   bool prev = isRockRidgeEntry();
   
   ClElement::setRockRidgeEntry(bIsRR);
   
   if (isRockRidgeEntry() != prev)
   {
      prev = isRockRidgeEntry();

      for (int32 i=0; i<hChildren.Count(); i++)
      {
         hChildren[i]->setParentRockRidgeEntry(prev);
      }
   }
}


/*
 * set parent iso entry flag and propagate new state, if changed.
 */
void ClDirectory::setParentISOEntry(bool bIsISO)
{
   bool prev = isISOEntry();
   
   ClElement::setParentISOEntry(bIsISO);

   if (isISOEntry() != prev)
   {
      prev = isISOEntry();

      for (int32 i=0; i<hChildren.Count(); i++)
      {
         hChildren[i]->setParentISOEntry(prev);
      }
   }
}


/*
 * set parent joliet entry flag and propagate new state, if changed.
 */
void ClDirectory::setParentJolietEntry(bool bIsJoliet)
{
   bool prev = isJolietEntry();

   ClElement::setParentJolietEntry(bIsJoliet);

   if (isJolietEntry() != prev)
   {
      prev = isJolietEntry();

      for (int32 i=0; i<hChildren.Count(); i++)
      {
         hChildren[i]->setParentJolietEntry(prev);
      }
   }
}


/*
 * set parent rockridge entry flag and propagate new state, if changed.
 */
void ClDirectory::setParentRockRidgeEntry(bool bIsRR)
{
   bool prev = isRockRidgeEntry();
   
   ClElement::setParentRockRidgeEntry(bIsRR);
   
   if (isRockRidgeEntry() != prev)
   {
      prev = isRockRidgeEntry();

      for (int32 i=0; i<hChildren.Count(); i++)
      {
         hChildren[i]->setParentRockRidgeEntry(prev);
      }
   }
}

/*
 * set new iso level for file names
 *
 * inputs:
 *    ClName::Level - new level
 *
 * returns:
 *    bool - true, if level needed update
 */
bool ClDirectory::setISOLevel(ClName::Level lvl)
{
   if (ClElement::setISOLevel(lvl) == false)
      return false;

   for (int32 i=0; i<hChildren.Count(); i++)
   {
      hChildren[i]->setISOLevel(lvl);
   }
   return true;
}

/*
 * set relaxed iso flag for lowercase full ascii set
 *
 * inputs:
 *    bool - true for relaxed iso
 *
 * returns:
 *    bool - true if flag needed update
 */
bool ClDirectory::setISORelaxed(bool state)
{
   if (ClElement::setISORelaxed(state) == false)
      return false;

   for (int32 i=0; i<hChildren.Count(); i++)
   {
      hChildren[i]->setISORelaxed(state);
   }
   return true;
}

