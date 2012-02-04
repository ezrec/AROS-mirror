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

#ifndef _DYNLIST_H_
#define _DYNLIST_H_

#ifndef __linux__
   #include <exec/lists.h>
   #include <exec/nodes.h>
#else
   struct MinList {
      struct  MinNode *mlh_Head;
      struct  MinNode *mlh_Tail;
      struct  MinNode *mlh_TailPred;
   }; 

   struct MinNode {
       struct MinNode *mln_Succ;
       struct MinNode *mln_Pred;
   };
#endif

namespace GenNS
{
   class EList
   {
   protected:

      struct ENode : public Node
      {
         void*                ln_Data;
      };

      MinList                *PrvList;
      unsigned long           Count;
      bool                    bExternalList;
   public:
      EList(MinList* pList);
      EList();
      virtual                ~EList();
      virtual void            Add(void* ApObject);
      virtual void            AddFeatured(void* ApObject, const char* ApName, signed char AlPri);
      virtual void            InsertAt(void* ApObject, unsigned long APos);
      virtual void            Rem(void* ApObject);
      virtual void            Rem(unsigned long AItem);
      virtual void           *Get(unsigned long AItem);
      virtual void*           operator [] (unsigned long AItem);
      virtual unsigned long   GetCount();
      virtual Node           *GetNode(unsigned long AItem);
      virtual MinList        *GetList();
      virtual void            Update();
   };

   template <class T>
      class TList : public EList
   {
      bool                    bFreeOnDelete;   
   public:
      TList() : EList()
      {
         bFreeOnDelete = false;
      } 

      TList(MinList *aList) : EList(aList)
      {
         bFreeOnDelete = false;
      } 

      virtual                ~TList()
      {
         if ((bFreeOnDelete) && (!bExternalList))
         {
            while (GetCount())
            {
               delete Get(0);
               Rem((unsigned long)0);
            }
         }
      }

      virtual void            Add(T ApObject)
      {
         EList::Add((void*)ApObject);
      } 

      virtual void            InsertAt(T ApObject, unsigned long APos)
      {
         EList::InsertAt((void*)ApObject, APos);
      }

      virtual void            Rem(T ApObject)
      {
         EList::Rem((void*)ApObject);
      }

      virtual void            Rem(unsigned long AItem)
      {
         EList::Rem(AItem);
      }

      virtual T               operator [] (unsigned long AItem)
      {
         return (T)EList::operator [] (AItem);
      }

      virtual T               Get(unsigned long AItem)
      {
         return (T)EList::operator [] (AItem);
      }

      virtual unsigned long   GetCount()
      {
         return EList::GetCount();
      }

      virtual void            FreeOnDelete(bool bFlag)
      {
         if (bExternalList)
            bFlag = false;
         bFreeOnDelete = bFlag;
      }

      virtual void            AddFeatured(T ApObject, const char* ApName, signed char AlPri)
      {
         EList::AddFeatured((void*)ApObject, ApName, AlPri);
      }

      virtual Node           *GetNode(unsigned long AItem)
      {
         return EList::GetNode(AItem);
      }
   };
};

#endif //_DYNLIST_H_
