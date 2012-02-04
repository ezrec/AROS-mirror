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

#ifndef _VECTOR_H_
#define _VECTOR_H_

//#warning about to be obsoleted
//#pragma obsolete

namespace GenNS
{
   template <class T, long minSize=16>
      class Vector 
   {
      T*          pElements;
      long        lCurrentSize;
      long        lTotalSize;
      bool        bFreeOnDelete;
   public:

      Vector()
      {
         if (minSize < 1)
            lTotalSize  = 4;
         else
            lTotalSize  = minSize;

         bFreeOnDelete  = false;
         lCurrentSize   = 0;
         pElements      = new T[lTotalSize];
      }

      virtual ~Vector()
      {
         if (bFreeOnDelete)
         {
            for (int i=0; i<lCurrentSize; i++)
            {
               delete pElements[i];
            }
         }
         delete [] pElements;
      }

      T operator [] (int lElement) const
      {
         if (lElement >= lCurrentSize)
            return 0;

         if (lElement >= 0)      
            return pElements[lElement];
         else
            return pElements[lCurrentSize + lElement];
      }

      T    Get(int lElement) const
      {
         return (*this)[lElement];
      }

      Vector<T, minSize>& operator << (T pNewElement)
      {
         if (lCurrentSize == lTotalSize)
         {
            lTotalSize   <<= 1; // twice that much
            T* pNewElem    = new T[lTotalSize];
            for (long i=0; i<lCurrentSize; i++)
               pNewElem[i] = pElements[i];
            delete [] pElements;
            pElements      = pNewElem;
         }

         pElements[lCurrentSize] = pNewElement;
         ++lCurrentSize;
         return *this;
      }

      void FreeOnDelete(bool bSet)
      {
         bFreeOnDelete = bSet;
      }

      bool FreeOnDelete() const
      {
         return bFreeOnDelete;
      }

      long Count() const
      {
         return lCurrentSize;
      }

      long Max() const
      {
         return lTotalSize;
      }

      void Empty()
      {
         if (FreeOnDelete())
         {
            for (int i=0; i<lCurrentSize; i++)
            {
               delete (*this)[i];
            }
         }
         lCurrentSize = 0;
      }

      Vector<T, minSize>& operator >> (T pOldElement)
      {
         int i;
         for (i=0; i<lCurrentSize; i++)
         {
            if (pElements[i] == pOldElement)
            {
               if (FreeOnDelete())
               {
                  delete pElements[i];
               }
               break;
            }
         }

         for (; i<(lCurrentSize-1); i++)
         {
            pElements[i] = pElements[i+1];
         }

         if (i == lCurrentSize)  // not found.
            return *this;

         --lCurrentSize;
         pElements[lCurrentSize] = 0;
         return *this;
      }

      const T* GetArray()
      {
         return pElements;
      }
   };
};

#endif //_VECTOR_H_
   // vim: ts=3 et
