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

#ifndef _GENERIC_PROPERTYT_H_
#define _GENERIC_PROPERTYT_H_

#include "VectorT.h"

namespace GenNS
{
   template<typename T>
      class PropertyT 
   {
   public:
      class Monitor
      {
      public:
         virtual void   onPropChange(PropertyT<T>*, const T&)  = 0;
      };

   protected:
      T                                val;
      VectorT<Monitor*>                monitors;

   public:

      PropertyT()
      {
      }

      virtual ~PropertyT()
      {
      }

      virtual PropertyT<T>&   operator = (const T& t)
      {
         if (val == t)
            return *this;

         val = t;
         NotifyAll();
         return *this;
      }

      virtual                 operator T&()
      {
         return val;
      }

      virtual PropertyT<T>&   operator << (Monitor *mon)
      {
         monitors << mon;

         mon->onPropChange(this, val);

         return *this;
      }

      virtual PropertyT<T>&   operator >> (Monitor *mon)
      {
         monitors >> mon;
         return *this;
      }

      virtual void            NotifyAll()
      {
         for (int i=0; i<monitors.Count(); i++)
            monitors[i]->onPropChange(this, val);
      }
   };
};

#endif

