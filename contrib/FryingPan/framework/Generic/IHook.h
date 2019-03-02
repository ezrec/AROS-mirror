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

#ifndef GENERIC_IHOOK_H_
#define GENERIC_IHOOK_H_

/**
 * \file IHook.h
 * \brief This file defines a \b Hook interface - a good base for hooks of any kind.
 * \details If you want to toss call a specific function with specific static or dynamic args every time a hook fires this is something you are looking for. 
 * Simply override the uint ::Call(uint a1, uint a2) method and you're all set.
 */

#include "Generic.h"
#include <utility/hooks.h>
#include "Types.h"

#ifdef __MORPHOS__
   #include <emul/emulregs.h>
#endif

namespace GenNS
{
   /**
    * \interface IHook
    * \brief Interface to Utility \b Hooks.
    * \details This interface handles all the low-level handling of your hooks. Actually it only forwards the two parameters passed to the hook further to the overridden Call(uint, uint) function.\n
    * To use this interface, create a new class that derives from ::IHook and implement a virtual Call(uint, uint) function, e.g.:
    * \code
    * class MyHook : public IHook
    * {
    * protected:
    *    virtual uint Call(uint p1, uint p2)
    *    {
    *       return request("Info", "Parameters passed: %lx and %lx", "Okay", ARRAY(p1, p2));
    *    }
    * }
    * \endcode
    * and later in your code:
    * \code
    *    MyHook h();
    *    ...
    *    Utility->CallHookPkt(h, 100, 200);
    * \endcode
    * The above example will call a hook with parameters 100 and 200 - and will open a requester saying: "Parameters passed: 100 and 200".
    * This interface is only required if you want to declare a very specific hook implementation. You are more interested in:
    * \sa
    * - HookT - a \b specialized hook
    * - MUICustomClassT - in case of MUI Custom Classes.
    */
   class IHook 
   {
   private:
      Hook                    hHook; 
#ifdef __MORPHOS__
      unsigned long           hMOSCall[2];
#endif

   public:
      /**
       * \brief Override this function to implement your own Hook caller.
       * \details Parameters:
       * \arg a1 - Object parameter passed to CallHookPkt()
       * \arg a2 - Message parameter passed to CallHookPkt()
       */
      virtual uint Call(uint a1, uint a2) = 0;

   protected:
#if defined (__AROS__) || defined (__AMIGAOS4__)
      static uint subCaller(Hook *pHook, uint pObject, uint pMessage);
#elif defined (__MORPHOS__) || defined(__mc68000)
      static uint subCaller();
#endif

   private:
      void     Initialize();

   public:
               IHook();
      virtual ~IHook();
      /**
       * \brief Call this method to obtain hook pointer.
       * \details You should call this method whenever you need to get a \b Hook from the object pointer. Do \b not use casting.
       * Alternatively, the operator const Hook* () is used by the compiler to translate the object directly into Hook pointer when applicable 
       */
      const Hook *GetHook() const;

      /**
       * \brief This method is automatically selected when a \b Hook pointer is needed and object implementing IHook is used.
       * \details The translation is done automatically by the compiler. Remember that \b casting is bad.
       * \sa GetHook()
       */
      operator const Hook* () const; 
   };
};

#endif /*HOOKT_H_*/
