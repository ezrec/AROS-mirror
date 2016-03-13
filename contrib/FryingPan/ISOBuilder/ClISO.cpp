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
#include "ClISO.h"
#include "Structure/ClDirectory.h"
#include "Structure/ClRoot.h"
#include "Structure/ClFile.h"
#include "ISOStructures.h"
#include <libclass/utility.h>
#include <libclass/exec.h>
#include <Generic/Debug.h>

extern UtilityIFace *Utility;

struct JobItem
{
   enum JobID
   {
      J_End,
      J_Pad,
      J_PrimaryDescriptor,
      J_SupplementaryDescriptor,
      J_BootDescriptor,
      J_EndDescriptor,
      J_BootCatalogs,
      J_ISOPathTable,
      J_JolietPathTable,
      J_ISODirTable,
      J_JolietDirTable,
      J_Files
   };
   
   JobID             lJobID;
   unsigned short    lParam;
};

   static const struct JobItem   Instructions[] =
   {
      {  JobItem::J_Pad,                     16 },
      {  JobItem::J_PrimaryDescriptor,       0  },
      {  JobItem::J_SupplementaryDescriptor, 0  },
      {  JobItem::J_BootDescriptor,          0  },
      {  JobItem::J_EndDescriptor,           0  },
      {  JobItem::J_BootCatalogs,            0  },
      {  JobItem::J_ISOPathTable,            0  },    // Little Endian
      {  JobItem::J_ISOPathTable,            1  },    // Big Endian
      {  JobItem::J_JolietPathTable,         0  },    // Little Endian
      {  JobItem::J_JolietPathTable,         1  },    // Big Endian
      {  JobItem::J_ISODirTable,             0  },    // iso dir table
      {  JobItem::J_JolietDirTable,          0  },    // joliet dir table
      {  JobItem::J_Files,                   0  },    // iso specific + common files
      {  JobItem::J_Files,                   1  },    // joliet specific
      {  JobItem::J_Pad,                     2  },    // pad two sectors
      {  JobItem::J_End,                     0  }     // end. 
   };
   
void ClISO::setDebug(DbgHandler* d)
{
   debug = d;
}

DbgHandler *ClISO::getDebug()
{
   return debug;
}

ClISO::ClISO()
{
   debug = 0;
   _createDebug(1, "ISOBuilder");
   pRoot          = new ClRoot(getDebug());
   pCurrDir       = pRoot;
   hDataFetch.Initialize(this, &ClISO::fDataPass);
}

ClISO::~ClISO()
{
   delete pRoot;
   _destroyDebug();
}

ClRoot *ClISO::getRoot() 
{
   return pRoot;
}

ClDirectory *ClISO::getParent() 
{
   return pCurrDir->getParent();
}

ClDirectory *ClISO::getCurrDir() 
{
   return pCurrDir;
}

void ClISO::goRoot() 
{
   pCurrDir = pRoot;
}

void ClISO::goParent()
{
   pCurrDir = pCurrDir->getParent();
}

void ClISO::addDataHook(const Hook *pHook)
{ 
   hDataHooks << pHook;
}

void ClISO::remDataHook(const Hook *pHook)
{
   hDataHooks >> pHook;
}

unsigned long ClISO::fDataPass(IPTR Data, long lSize)
{
   // when returns something different than 0, we should abort at once!
   // fill up data buffer.
   
   if (Data == 0) 
   {
      // flush
      lSize = 0;
      for (int i=0; i<hDataHooks.Count(); i++)
      {
         Utility->CallHookPkt(const_cast<Hook*>(hDataHooks[i]), pMemBlk, (IPTR)lCurrentPos);
      }
      return true;
   }
   
   unsigned long lLen = lSize;
   
   if (lLen > (lMemBlkSize - lCurrentPos))      // make sure we
      lLen = lMemBlkSize - lCurrentPos;         // don't go too far
   Exec->CopyMem((void *)Data, &((char*)pMemBlk)[lCurrentPos], lLen);
   lCurrentPos += lLen;
   lSize       -= lLen;
   Data         = (IPTR)&((char*)Data)[lLen];         // move pointer

   if (lCurrentPos == lMemBlkSize)
   {     
      // pass the data block to receivers      
      IPTR lRes;
      lCurrentPos = 0;
      for (int i=0; i<hDataHooks.Count(); i++)
      {
         lRes = Utility->CallHookPkt(const_cast<Hook*>(hDataHooks[i]), pMemBlk, (IPTR)(lMemBlkSize >> 11));
         if (false == lRes)
         {
            // ask if we should abort!!!
            if (false == request("Warning", "An error occured during data flow.\nDo you want to continue?", "Yes|No", 0))
            {
               return false;
            }
         }
      }
      
      if (lSize)
      {
         return fDataPass(Data, lSize);    // pointers are already updated
      }
   }
      
   if (true == bAbortDataFlow)
      return false;
   return true;
}

   
unsigned long ClISO::validate()
{
   recalculate();
   return pRoot->getImageSize();
}

long ClISO::startDataFlow()
{
   bAbortDataFlow = false;
   
   if (false == recalculate())
      return -1;
      
   if (false == generate())
      return -1;
   return 0;
}

bool ClISO::recalculate()
{
   return pRoot->updateSizes();
}

bool ClISO::generate()
{
   bool bRes;

   lMemBlkSize = 32768;
   lCurrentPos = 0;
   pMemBlk     = (IPTR) new char[lMemBlkSize];
   if (!pMemBlk)
      return false;

   bRes = pRoot->generate(hDataFetch.GetHook());

   if (bRes)
   {
      Utility->CallHookPkt(const_cast<Hook*>(hDataFetch.GetHook()), 0, 0);
   }

   delete [] (char*)pMemBlk; 

   pMemBlk     = 0;
   lMemBlkSize = 0;
   lCurrentPos = 0;

   return bRes;
}

void ClISO::destroy()
{
   delete this;
}

ClDirectory *ClISO::makeDir()
{
   ClDirectory* d = new ClDirectory(getRoot(), getCurrDir());
   getCurrDir()->addChild(d);   
   return d;
}

void ClISO::setCurrDir(ClDirectory* dir)
{
   if (dir == 0)
   {
      pCurrDir = pRoot;
   }
   else
   {
      pCurrDir = dir;
   }
}

bool ClISO::isEmpty()
{
   if (pRoot->getChildrenCount() == 0)
      return true;
   return false;
}

void ClISO::setISOLevel(ISOLevel l)
{
   switch (l)
   {
      case ISOLevel_1:
         pRoot->setISOLevel(ClName::ISOLevel_1);
         break;

      case ISOLevel_2:
         pRoot->setISOLevel(ClName::ISOLevel_2);
         break;

      case ISOLevel_3:
         pRoot->setISOLevel(ClName::ISOLevel_3);
         break;
   }
}

IBrowser::ISOLevel ClISO::getISOLevel()
{
   switch (pRoot->getISOLevel())
   {
      case ClName::ISOLevel_1:
         return ISOLevel_1;
         break;

      case ClName::ISOLevel_2:
         return ISOLevel_2;
         break;

      case ClName::ISOLevel_3:
         return ISOLevel_3;
         break;
   }
   return ISOLevel_1;
}



