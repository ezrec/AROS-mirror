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

#ifndef _GENERIC_HOOKATTRT_H
#define _GENERIC_HOOKATTRT_H

#include "Generic.h"
#include <utility/hooks.h>
#include <libclass/utility.h>
#include "Types.h"
#include <LibC/LibC.h>

/**
 * \file HookAttrT.h
 * \brief Specialized class for hook calling - frees you from casting, makes sure you give the right params.
 */

namespace GenNS
{
   /** 
    * \class HookAttrT<T1, T2>
    * \brief Specialized class for hook calling
    * \details Types \a T1 and \a T2 represent the parameter types your hook accepts.
    * An example of HookAttrT use is:
    * \code
    *    HookAttrT<Object*, iptr> myHook;
    *    ...
    *    myHook(myObject, ARRAY(OM_METHOD1, val, 0));
    * \endcode
    * To initialize the encapsulated class use either way presented below
    * \code
    * class myClass
    * {
    *    HookAttrT<void*, void*> h;
    * public:
    *    myClass(const Hook *hook) :
    *       h(hook)
    *    {
    *       // ...
    *    }
    *    // ...
    * }
    * \endcode
    *
    * \code
    * class myClass
    * {
    *    HookAttrT<void*, void*> h;
    * public:
    *    // ...
    *    void setHook(const Hook *hook)
    *    {
    *       h = hook;
    *    }
    *    // ...
    * }
    * \endcode
    *
    */
   template <typename T1, typename T2>
      class HookAttrT 
      {
         const struct Hook   *FHook;
      public:

         /**
          * \brief Default constructor. Verifies the validity of types \a T1 and \a T2, sets hook to 0.
          */
         HookAttrT()
         {
            ASSERT(sizeof(T1) == sizeof(uint));
            ASSERT(sizeof(T2) == sizeof(uint));
            FHook = 0;
         }

         /**
          * \brief Parametrized constructor. Verifies the validity of types \a T1 and \a T2, sets internal hook to the provided pointer.
          * \sa operator = (const struct Hook*)
          */
         HookAttrT(const Hook* h)
         {
            ASSERT(sizeof(T1) == sizeof(uint));
            ASSERT(sizeof(T2) == sizeof(uint));
            FHook = h;
         }

         virtual       ~HookAttrT()
         {
         }

         /**
          * \brief Sets the internal hook pointer to new value. 
          * \sa HookAttrT(const Hook*)
          */
         HookAttrT      &operator = (const struct Hook* AHook)
         {
            FHook = AHook;
            return *this;
         }

         /**
          * \brief Use this method to call hook with adequate parameters.
          * \sa operator ()(T1, T2)
          */
         IPTR           Call(T1 object, T2 message)
         {
            ASSERT(Utility != 0);
            if (FHook == 0) 
               return 0;
            return Utility->CallHookPkt(const_cast<Hook*>(FHook), (IPTR)(object), (IPTR)(message));
         }

         /**
          * \brief Verifies whether hook is initialized. 
          * \details You don't need to check this before calling the hook. The checks are done automatically.
          */
         bool           IsValid()
         {
            return (FHook != 0);
         }

         /**
          * \brief Static method to call specific hook.
          * \details Useful if you need to call your hook in only one place - saves memory, increases speed.
          * Example usage:
          * \code
          *    // ...
          *    HookAttrT<void*, void*>::Call(hook, mem1, mem2);
          *    // ...
          * \endcode
          */
         static IPTR   Call(const Hook* pHook, T1 object, T2 message)
         {
            ASSERT(Utility != 0);
            if (pHook == 0) 
               return 0;
            return Utility->CallHookPkt(const_cast<Hook*>(pHook), (IPTR)(object), (IPTR)(message));
         }

         /**
          * \brief Use this method to call hook with adequate parameters.
          * \sa Call(T1, T2)
          */
         IPTR operator () (T1 object, T2 message)
         {
            ASSERT(Utility != 0);
            if (FHook == 0) 
               return 0;
            return Utility->CallHookPkt(const_cast<Hook*>(FHook), (IPTR)(object), (IPTR)(message));
         }
      };
};
#endif //_GENERIC_HOOKATTRT_H
