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

#ifndef _GENERIC_EXTPROPERTYT_H_
#define _GENERIC_EXTPROPERTYT_H_

#include "Generic.h"

namespace GenNS
{
   template<typename T>
      class ExtPropertyT : public Generic
   {
   public:
      class Handler 
      {
      public:
         virtual bool   onPropSet(ExtPropertyT<T>*, const T&)     = 0;
         virtual T&     onPropGet(ExtPropertyT<T>*)               = 0;
      };

      class Monitor
      {
      public:
         virtual void   onPropChange(ExtPropertyT<T>*, const T&)  = 0;
      };

   protected:
      Handler                         *handler;
      VectorT<Monitor*>                monitors;

   public:
      ExtPropertyT()
      {
         handler = 0;
      }

      virtual ~ExtPropertyT()
      {
      }

      virtual ExtPropertyT<T>&   operator = (const T& t)
      {
         ASSERT(handler != 0);
         if (handler == 0)
            return *this;

         if (handler->onPropSet(this, t))
            NotifyAll();
         return *this;
      }

      virtual                 operator T&()
      {
         ASSERT(handler != 0);
         return handler->onPropGet(this);
      }

      virtual ExtPropertyT<T>&   operator << (Monitor *mon)
      {
         monitors << mon;

         ASSERT(handler != 0);
         if (handler != 0)
            mon->onPropChange(this, handler->onPropGet(this));

         return *this;
      }

      virtual ExtPropertyT<T>&   operator >> (Monitor *mon)
      {
         monitors >> mon;
         return *this;
      }

      virtual ExtPropertyT<T>&   SetPropHandler(Handler *f)
      {
         handler = f;
         return *this;
      }

      virtual void            NotifyAll()
      {
         if (handler == 0)
            return;

         for (int i=0; i<monitors.Count(); i++)
            monitors[i]->onPropChange(this, handler->onPropGet(this));
      }
   };
};

#endif

