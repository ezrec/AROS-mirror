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

#include "DynList.h"
#include "String.h"
#include "LibrarySpool.h"
#include <libclass/exec.h>
#include <LibC/LibC.h>

using namespace GenNS;

EList::EList(MinList* pList)
{
   bExternalList = true;
   PrvList  = pList;
   Count    = 0;
}
 
EList::EList()
{
   bExternalList           = false;
   PrvList                 = new MinList;
   PrvList->mlh_Head       = (MinNode*)&PrvList->mlh_Tail;
   PrvList->mlh_TailPred   = (MinNode*)&PrvList->mlh_Head;
   PrvList->mlh_Tail       = 0;   
   Count                   = 0;
} 

EList::~EList()
{
   if (false == bExternalList)
   {
      while (Count > 0)
      {
         Rem(0UL);
      }
      delete PrvList;
   }
}
   
void EList::Add(void* ApObject)
{
   ENode *pNode = new ENode;
   
   memset(pNode, 0, sizeof(ENode));
   pNode->ln_Data = ApObject;

   Exec->AddTail((List*)PrvList, (Node*)pNode);      
   if (false == bExternalList)
      ++Count;
} 

void EList::AddFeatured(void* ApObject, const char* ApName, signed char AlPri)
{
   ENode *pNode = new ENode;
   
   memset(pNode, 0, sizeof(ENode));
   pNode->ln_Data = ApObject;
   pNode->ln_Pri  = AlPri;
   pNode->ln_Name = const_cast<char*>(ApName);

   Exec->Enqueue((List*)PrvList, (Node*)pNode);  
   if (false == bExternalList)
      ++Count;
} 
  
void EList::InsertAt(void* ApObject, unsigned long APos)
{
   ENode *pNode = new ENode;
   pNode->ln_Data = ApObject;
   if (APos >=GetCount())
   {
      Exec->AddTail((List*)PrvList, (Node*)pNode);
   }
   else if (!APos)
   {
      Exec->AddHead((List*)PrvList, (Node*)pNode);
   }
   else
   {
      Node* pBefore = (Node*)PrvList->mlh_Head;
      while (--APos)
      {
         pBefore = pBefore->ln_Succ;
      }
      Exec->Insert((List*)PrvList, (Node*)pNode, pBefore);
   }
   if (false == bExternalList)
      ++Count;
}
  
void EList::Rem(void* ApObject)
{
   ENode *pNode = (ENode*)PrvList->mlh_Head;
   while (pNode->ln_Succ)
   {
      ENode *pNext = (ENode*)pNode->ln_Succ;
      if (pNode->ln_Data == ApObject)
      {
         Exec->Remove((Node*)pNode);
         if (false == bExternalList)
            --Count;
         delete pNode;
      }
      pNode = pNext;
   }
}
   
void EList::Rem(unsigned long AItem)
{
   if (false == bExternalList)
      if (AItem > Count) return;
      
   ENode *pNode = (ENode*)PrvList->mlh_Head;
   while (pNode->ln_Succ)
   {
      ENode *pNext = (ENode*)pNode->ln_Succ;
      if (!AItem)
      {
         Exec->Remove((Node*)pNode);
         if (false == bExternalList)
            --Count;
         delete pNode;
         return;
      }
      pNode = pNext;
      --AItem;
   }
}
   
void* EList::operator [] (unsigned long AItem)
{
   if (false == bExternalList)
      if (AItem > Count) return 0;
   ENode *pNode = (ENode*)PrvList->mlh_Head;
   while (pNode->ln_Succ)
   {
      ENode *pNext = (ENode*)pNode->ln_Succ;
      if (!AItem)
      {
         return pNode->ln_Data;
      }
      pNode = pNext;
      --AItem;
   }
   return 0;
}
   
unsigned long EList::GetCount()
{
   if (false == bExternalList)
      return Count;
   
   ENode *pNode = (ENode*)PrvList->mlh_Head;
   long  lItems = 0;
   
   while (pNode->ln_Succ)
   {
      pNode = (ENode*)pNode->ln_Succ;
      ++lItems;
   }
   return lItems;
}

void *EList::Get(unsigned long AItem)
{
   return (*this)[AItem];
}

Node* EList::GetNode(unsigned long AItem)
{
   if (false == bExternalList)
      if (AItem > Count) return 0;
   ENode *pNode = (ENode*)PrvList->mlh_Head;
   while (pNode->ln_Succ)
   {
      ENode *pNext = (ENode*)pNode->ln_Succ;
      if (!AItem)
      {
         return pNode;
      }
      pNode = pNext;
      --AItem;
   }
   return 0;
}

MinList *EList::GetList()
{
   return PrvList;
}

void EList::Update()
{
    bool bCopy = bExternalList;
    bExternalList = true;
    Count = GetCount();
    bExternalList = bCopy;
}

