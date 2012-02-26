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

#ifndef _HASHT_H_
#define _HASHT_H_

#include "Types.h"
#include "VectorT.h"
#include <LibC/LibC.h>

namespace GenNS
{
   template <typename T, typename U, long minSize=16>
      class HashT 
   {
   protected:
      class HashItem
      {
         T  key;
         U  value;
      public:
         HashItem()
         {
         }

         HashItem(T k, U v): 
            key(k), 
            value(v)
         {
         }

         operator const T&() const
         {
            return key;
         }

         const T& Key() const
         {
            return (const T&)(*this);
         }

         operator const U&() const
         {
            return value;
         }

         const U& Value() const
         {
            return (const U&)(*this);
         }

         void SetValue(U& val)
         {
            value = val;
         }

         int operator <(const HashItem& o) const
         {
            return key < o.key;
         }

         int operator == (const HashItem& o) const
         {
            return key == o.key;
         }

         HashItem& operator =(const HashItem& o)
         {
            key = o.key;
            value = o.value;
            return *this;
         }
      };

   protected:
      VectorT<HashItem, minSize> vector;
      U fallback;

   public:
      HashT(U fallbk)
      {
         fallback = fallbk;
      }

      virtual ~HashT()
      {
      }

      virtual int Count()
      {
         return vector.Count();
      }

      virtual const T& GetKey(int i)
      {
         return vector[i].Key();
      }

      virtual void Add(T key, U val)
      {
         int index = vector.IndexOf(HashItem(key, val), 0);
         if (index >= 0)
         {
            vector[index].SetValue(val);
         }
         else
         {
            vector.InsertSorted(HashItem(key, val), 0);
         }
      }

      virtual const U& GetVal(T key)
      {
         HashItem item(key, fallback);
         int index = vector.IndexOf(item, 0);
         
         if (index < 0)
            return fallback;
         return vector[index].Value();
      }
   };
};

#endif
