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

#ifndef _GENERIC_SYNCPROPERTY_H_
#define _GENERIC_SYNCPROPERTY_H_

#include "RWSyncT.h"
#include "DistributorT.h"

namespace GenNS
{
   template <typename T>
      class SyncProperty : public RWSyncT<T>, public DistributorT<T, T>
   {
   public:
      virtual T Assign(T val)
      {
         T old;
         old = RWSyncT<T>::Assign(val);
         if (old != val)
         {
            DistributorT<T,T>::Send(old, val);
         }
         return old;
      };

      virtual T AttemptAssign(T val, bool *bRes)
      {
         T old;
         old = RWSyncT<T>::AttemptAssign(val, bRes);
         if (true == *bRes)
         {
            if (old != val)
            {
               DistributorT<T,T>::Send(old, val);
            }
         }
         return old;
      };
   };
};

#endif
